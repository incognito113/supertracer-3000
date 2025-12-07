#include <metal_stdlib>
using namespace metal;

#include "bvh_traversal.metal"
#include "initial_ray.metal"
#include "intersections.metal"
#include "structs.metal"

inline float3 computeLighting(const GPU_HitInfo hitInfo, const GPU_Ray ray,
                              const device GPU_Material* materials,
                              const device GPU_Light* lights,
                              const device GPU_SceneData* sceneData) {
  float3 d = normalize(ray.dir);

  float3 n = hitInfo.normal;
  const GPU_Material material = materials[hitInfo.materialIndex];

  // Ambient light contribution (doesn't depend on lights)
  float ambFactor = sceneData->ambientLight * (1.0 - material.reflectivity);
  float3 finalColor = material.color * ambFactor;

  for (uint i = 0; i < sceneData->numLights; ++i) {
    float3 lightColor = lights[i].color;
    float3 lightPos = lights[i].position;

    bool inShadow = false;
    // Implement shadow ray logic here if desired

    float3 lt = normalize(lightPos - hitInfo.position);

    // Diffuse light contribution
    if (!inShadow) {
      float diffFactor =
          (1 - ambFactor) * (1 - material.reflectivity) * max(0.0, dot(n, lt));
      finalColor += material.color * lightColor * diffFactor;
    }

    // Specular light contribution
    if (!inShadow) {
      float3 h = normalize(lt - d);
      finalColor += material.specular * material.specularFactor *
                    pow(max(0.0, dot(n, h)), material.shininess) * lightColor;
    }
  }
  return clamp(finalColor, 0.0, 1.0);
}

kernel void traceMultiBounce(const device GPU_SceneData* sceneData
                             [[buffer(0)]],
                             const device GPU_Material* materials [[buffer(1)]],
                             const device GPU_Light* lights [[buffer(2)]],
                             const device GPU_Node* nodes [[buffer(3)]],
                             const device GPU_ShapeRef* shapeRefs [[buffer(4)]],
                             const device GPU_Triangle* triangles [[buffer(5)]],
                             const device GPU_Sphere* spheres [[buffer(6)]],
                             const device GPU_Plane* planes [[buffer(7)]],
                             device float3* outputColors [[buffer(8)]],
                             uint2 gid [[thread_position_in_grid]]) {
  const uint width = sceneData->width;
  const uint height = sceneData->height;
  const uint idx = gid.y * width + gid.x;

  if (gid.x >= width || gid.y >= height) {
    return;
  }

  GPU_Ray ray = generateInitialRay(sceneData, gid.x, gid.y);

  float throughput = 1.0;
  float3 finalColor = float3(0.0, 0.0, 0.0);

  for (uint bounce = 0; bounce < sceneData->maxReflections; ++bounce) {
    // Keep a candidate for closest hit
    GPU_HitInfo closestHit;
    bool hitSomething = false;
    float closestT = FLT_MAX;

    // Check plane intersections normally
    for (uint i = 0; i < sceneData->numPlanes; ++i) {
      GPU_HitInfo localHit;
      localHit.t = FLT_MAX;
      if (intersectsPlane(ray, &planes[i], &localHit) &&
          localHit.t < closestT) {
        closestT = localHit.t;
        closestHit = localHit;
        hitSomething = true;
      }
    }

    // Check spheres and triangles using BVH traversal
    GPU_HitInfo bvhHit;
    bvhHit.t = FLT_MAX;
    if (BVHintersects(ray, nodes, shapeRefs, triangles, spheres, &bvhHit) &&
        bvhHit.t < closestT) {
      closestT = bvhHit.t;
      closestHit = bvhHit;
      hitSomething = true;
    }

    if (!hitSomething) {
      // No intersection, set color to background scaled by current
      // and finish
      finalColor += sceneData->backgroundColor * throughput;
      break;
    }

    // Accumulate color contribution at this bounce
    finalColor += throughput * computeLighting(closestHit, ray, materials,
                                               lights, sceneData);

    // Prepare for next reflection bounce
    const GPU_Material material = materials[closestHit.materialIndex];

    // If the material is not reflective, terminate the bounce loop
    if (material.reflectivity <= 0.0) {
      break;
    }

    // Compute reflection direction and offset to avoid self-intersection
    float3 origin = closestHit.position + closestHit.normal * EPS;
    float3 proj = dot(ray.dir, closestHit.normal) * closestHit.normal;
    float3 reflectDir = normalize(ray.dir - 2.0 * proj);

    // Update throughput and current ray for next bounce
    throughput *= material.reflectivity;
    ray.orig = origin;
    ray.dir = reflectDir;

    // If thoughput is very small, break early
    if (throughput < 0.01) break;
  }

  outputColors[idx] = clamp(finalColor, 0.0, 1.0);
}