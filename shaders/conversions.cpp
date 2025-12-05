#ifdef METAL

#include "conversions.hpp"

#include "math/camera.hpp"
#include "scene/bvh.hpp"
#include "scene/light.hpp"
#include "scene/material.hpp"
#include "scene/scene.hpp"
#include "shapes/plane.hpp"
#include "shapes/shape.hpp"
#include "shapes/sphere.hpp"
#include "shapes/triangle.hpp"

simd_float3 fromVector(const Vector& vec) {
  return simd_make_float3(static_cast<float>(vec.x()),
                          static_cast<float>(vec.y()),
                          static_cast<float>(vec.z()));
}

simd_float3 fromColor(const Color& color) {
  return simd_make_float3(static_cast<float>(color.r()),
                          static_cast<float>(color.g()),
                          static_cast<float>(color.b()));
}

Converter::GPU_Data Converter::convertAll(const Scene& scene, const BVH& bvh) {
  Converter::GPU_Data gpuData;

  gpuData.materials.reserve(scene.materials.size());
  gpuData.lights.reserve(scene.lights.size());
  gpuData.nodes.reserve(bvh.getNodeCount());
  gpuData.spheres.reserve(scene.boundedShapeCount());    // Safe overestimate
  gpuData.triangles.reserve(scene.boundedShapeCount());  // Safe overestimate
  gpuData.planes.reserve(scene.planeCount());

  // Convert materials
  for (const Material& mat : scene.materials) {
    gpuData.materials.push_back(convertMaterial(mat));
  }
  // Convert lights
  for (const Light& light : scene.lights) {
    gpuData.lights.push_back(convertLight(light));
  }

  // Old indices store spheres and triangles together; new indices separate them
  // Convert spheres and triangles, keeping track of new indices
  std::vector<int32_t> newIndexMapping(scene.shapeCount(), -1);
  int32_t sphereCount = 0;
  int32_t triangleCount = 0;
  int32_t mapIndex = 0;

  for (const auto& shapePtr : scene.bndedShapes) {
    const Shape* shape = shapePtr.get();

    if (shape->getShapeType() == Shape::SPHERE) {
      const Sphere* sph = static_cast<const Sphere*>(shape);
      gpuData.spheres.push_back(convertSphere(*sph));
      newIndexMapping[mapIndex++] = sphereCount++;
    } else if (shape->getShapeType() == Shape::TRIANGLE) {
      const Triangle* tri = static_cast<const Triangle*>(shape);
      gpuData.triangles.push_back(convertTriangle(*tri));
      newIndexMapping[mapIndex++] = triangleCount++;
    }
  }

  // Convert BVH nodes, updating shape indices to new indices
  for (const BVHNode& node : bvh.getNodes()) {
    int32_t newShapeIndex = -1;
    if (node.shapeIndex != -1) {
      newShapeIndex = newIndexMapping[node.shapeIndex];
    }
    gpuData.nodes.push_back(convertNode(node, newShapeIndex));
  }

  // Convert scene data
  GPU_SceneData sceneData =
      convertSceneData(scene, bvh, sphereCount, triangleCount);
  gpuData.sceneData = sceneData;

  // Convert planes
  for (size_t i = 0; i < scene.planes.size(); ++i) {
    gpuData.planes.push_back(convertPlane(*scene.planes[i]));
  }

  return gpuData;
}

Converter::GPU_SceneData Converter::convertSceneData(const Scene& scene,
                                                     const BVH& bvh,
                                                     int32_t numSpheres,
                                                     int32_t numTriangles) {
  Converter::GPU_SceneData gpuScene;
  gpuScene.backgroundColor = fromColor(scene.getBackground());
  gpuScene.position = fromVector(scene.getCamera().position);
  gpuScene.direction = fromVector(scene.getCamera().direction);
  gpuScene.fov = static_cast<float>(scene.getCamera().fov);
  gpuScene.numLights = static_cast<uint32_t>(scene.lightCount());
  gpuScene.numMaterials = static_cast<uint32_t>(scene.shapeCount());
  gpuScene.numPlanes = static_cast<uint32_t>(scene.planeCount());
  gpuScene.numSpheres = numSpheres;
  gpuScene.numTriangles = numTriangles;
  gpuScene.numNodes = static_cast<uint32_t>(bvh.getNodeCount());
  return gpuScene;
}

Converter::GPU_Material Converter::convertMaterial(const Material& mat) {
  Converter::GPU_Material gpuMat;
  gpuMat.color = fromColor(mat.color);
  gpuMat.specular = fromColor(mat.specular);
  gpuMat.specularFactor = static_cast<float>(mat.specularFactor);
  gpuMat.shininess = static_cast<float>(mat.shininess);
  gpuMat.reflectivity = static_cast<float>(mat.reflectivity);
  return gpuMat;
}

Converter::GPU_Light Converter::convertLight(const Light& light) {
  Converter::GPU_Light gpuLight;
  gpuLight.position = fromVector(light.position);
  gpuLight.color = fromColor(light.color);
  return gpuLight;
}

Converter::GPU_Ray Converter::convertRay(const Ray& ray) {
  Converter::GPU_Ray gpuRay;
  gpuRay.origin = fromVector(ray.orig);
  gpuRay.direction = fromVector(ray.dir);
  return gpuRay;
}

Converter::GPU_Node Converter::convertNode(const BVHNode& node,
                                           int32_t shapeIndex) {
  Converter::GPU_Node gpuNode;
  gpuNode.boundsMin = fromVector(node.bounds.min);
  gpuNode.boundsMax = fromVector(node.bounds.max);
  gpuNode.left = static_cast<int32_t>(node.left);
  gpuNode.right = static_cast<int32_t>(node.right);
  gpuNode.shapeIndex = shapeIndex;
  gpuNode.shapeCount = static_cast<int32_t>(node.shapeCount);
  gpuNode.shapeType = static_cast<int32_t>(node.shapeType);
  return gpuNode;
}

Converter::GPU_HitInfo Converter::convertHitInfo(const HitInfo& hitInfo,
                                                 int materialIndex) {
  Converter::GPU_HitInfo gpuHit;
  gpuHit.position = fromVector(hitInfo.pos);
  gpuHit.normal = fromVector(hitInfo.normal);
  gpuHit.t = static_cast<float>(hitInfo.t);
  gpuHit.materialIndex = materialIndex;
  return gpuHit;
}

Converter::GPU_Triangle Converter::convertTriangle(const Triangle& tri) {
  Converter::GPU_Triangle gpuTri;
  gpuTri.v0 = fromVector(tri.v0);
  gpuTri.v1 = fromVector(tri.v1);
  gpuTri.v2 = fromVector(tri.v2);
  gpuTri.n0 = fromVector(tri.n0);
  gpuTri.n1 = fromVector(tri.n1);
  gpuTri.n2 = fromVector(tri.n2);
  gpuTri.materialIndex = static_cast<uint32_t>(tri.materialIndex);
  return gpuTri;
}

Converter::GPU_Sphere Converter::convertSphere(const Sphere& sph) {
  Converter::GPU_Sphere gpuSph;
  gpuSph.center = fromVector(sph.center);
  gpuSph.radius = static_cast<float>(sph.radius);
  gpuSph.materialIndex = static_cast<uint32_t>(sph.materialIndex);
  return gpuSph;
}

Converter::GPU_Plane Converter::convertPlane(const Plane& plane) {
  Converter::GPU_Plane gpuPlane;
  gpuPlane.point = fromVector(plane.point);
  gpuPlane.normal = fromVector(plane.normal);
  gpuPlane.materialIndex = static_cast<uint32_t>(plane.materialIndex);
  return gpuPlane;
}

#endif