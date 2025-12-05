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

Converter::GPU_Data Converter::convertAll(const Scene& scene, const BVH& bvh) {
  Converter::GPU_Data gpuData;

  // Convert scene data
  GPU_SceneData sceneData = convertSceneData(scene, bvh);
  gpuData.sceneData = sceneData;

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

  for (const auto& shapePtr : scene.bndedShapes) {
    const Shape* shape = shapePtr.get();

    if (shape->getShapeType() == Shape::SPHERE) {
      const Sphere* sph = static_cast<const Sphere*>(shape);
      gpuData.spheres.push_back(convertSphere(*sph));
      newIndexMapping.push_back(sphereCount++);
    } else if (shape->getShapeType() == Shape::TRIANGLE) {
      const Triangle* tri = static_cast<const Triangle*>(shape);
      gpuData.triangles.push_back(convertTriangle(*tri));
      newIndexMapping.push_back(triangleCount++);
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

  // Convert planes
  for (size_t i = 0; i < scene.planes.size(); ++i) {
    gpuData.planes.push_back(
        convertPlane(*scene.planes[i], static_cast<int>(i)));
  }

  return gpuData;
}

Converter::GPU_SceneData Converter::convertSceneData(const Scene& scene,
                                                     const BVH& bvh) {
  Converter::GPU_SceneData gpuScene;
  gpuScene.backgroundColor =
      simd_make_float3(scene.getBackground().r(), scene.getBackground().g(),
                       scene.getBackground().b());
  gpuScene.position = simd_make_float3(scene.getCamera().position.x(),
                                       scene.getCamera().position.y(),
                                       scene.getCamera().position.z());
  gpuScene.direction = simd_make_float3(scene.getCamera().direction.x(),
                                        scene.getCamera().direction.y(),
                                        scene.getCamera().direction.z());
  gpuScene.fov = static_cast<float>(scene.getCamera().fov);
  gpuScene.numLights = static_cast<uint32_t>(scene.lightCount());
  gpuScene.numMaterials = static_cast<uint32_t>(scene.shapeCount());
  gpuScene.numPlanes = static_cast<uint32_t>(scene.planeCount());
  gpuScene.numSpheres = static_cast<uint32_t>(scene.boundedShapeCount());
  gpuScene.numTriangles = static_cast<uint32_t>(scene.boundedShapeCount());
  gpuScene.numNodes = static_cast<uint32_t>(bvh.getNodeCount());
  return gpuScene;
}

Converter::GPU_Material Converter::convertMaterial(const Material& mat) {
  Converter::GPU_Material gpuMat;
  gpuMat.color = simd_make_float3(mat.color.r(), mat.color.g(), mat.color.b());
  gpuMat.specular =
      simd_make_float3(mat.specular.r(), mat.specular.g(), mat.specular.b());
  gpuMat.specularFactor = static_cast<float>(mat.specularFactor);
  gpuMat.shininess = static_cast<float>(mat.shininess);
  gpuMat.reflectivity = static_cast<float>(mat.reflectivity);
  return gpuMat;
}

Converter::GPU_Light Converter::convertLight(const Light& light) {
  Converter::GPU_Light gpuLight;
  gpuLight.position = simd_make_float3(light.position.x(), light.position.y(),
                                       light.position.z());
  gpuLight.color =
      simd_make_float3(light.color.r(), light.color.g(), light.color.b());
  return gpuLight;
}

Converter::GPU_Ray Converter::convertRay(const Ray& ray) {
  Converter::GPU_Ray gpuRay;
  gpuRay.origin = simd_make_float3(ray.orig.x(), ray.orig.y(), ray.orig.z());
  gpuRay.direction = simd_make_float3(ray.dir.x(), ray.dir.y(), ray.dir.z());
  return gpuRay;
}

Converter::GPU_Node Converter::convertNode(const BVHNode& node,
                                           int32_t shapeIndex) {
  Converter::GPU_Node gpuNode;
  gpuNode.boundsMin = simd_make_float3(node.bounds.min.x(), node.bounds.min.y(),
                                       node.bounds.min.z());
  gpuNode.boundsMax = simd_make_float3(node.bounds.max.x(), node.bounds.max.y(),
                                       node.bounds.max.z());
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
  gpuHit.position =
      simd_make_float3(hitInfo.pos.x(), hitInfo.pos.y(), hitInfo.pos.z());
  gpuHit.normal = simd_make_float3(hitInfo.normal.x(), hitInfo.normal.y(),
                                   hitInfo.normal.z());
  gpuHit.t = static_cast<float>(hitInfo.t);
  gpuHit.materialIndex = materialIndex;
  return gpuHit;
}

Converter::GPU_Triangle Converter::convertTriangle(const Triangle& tri) {
  Converter::GPU_Triangle gpuTri;
  gpuTri.v0 = simd_make_float3(tri.v0.x(), tri.v0.y(), tri.v0.z());
  gpuTri.v1 = simd_make_float3(tri.v1.x(), tri.v1.y(), tri.v1.z());
  gpuTri.v2 = simd_make_float3(tri.v2.x(), tri.v2.y(), tri.v2.z());
  gpuTri.n0 = simd_make_float3(tri.n0.x(), tri.n0.y(), tri.n0.z());
  gpuTri.n1 = simd_make_float3(tri.n1.x(), tri.n1.y(), tri.n1.z());
  gpuTri.n2 = simd_make_float3(tri.n2.x(), tri.n2.y(), tri.n2.z());
  gpuTri.materialIndex = static_cast<uint32_t>(tri.materialIndex);
  return gpuTri;
}

Converter::GPU_Sphere Converter::convertSphere(const Sphere& sph) {
  Converter::GPU_Sphere gpuSph;
  gpuSph.center =
      simd_make_float3(sph.center.x(), sph.center.y(), sph.center.z());
  gpuSph.radius = static_cast<float>(sph.radius);
  gpuSph.materialIndex = static_cast<uint32_t>(sph.materialIndex);
  return gpuSph;
}

Converter::GPU_Plane Converter::convertPlane(const Plane& plane,
                                             int materialIndex) {
  Converter::GPU_Plane gpuPlane;
  gpuPlane.point =
      simd_make_float3(plane.point.x(), plane.point.y(), plane.point.z());
  gpuPlane.normal =
      simd_make_float3(plane.normal.x(), plane.normal.y(), plane.normal.z());
  gpuPlane.materialIndex = static_cast<uint32_t>(materialIndex);
  return gpuPlane;
}
