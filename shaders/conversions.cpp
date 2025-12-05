#include "conversions.hpp"

#include "math/camera.hpp"
#include "scene/bvh.hpp"
#include "scene/light.hpp"
#include "scene/material.hpp"
#include "shapes/plane.hpp"
#include "shapes/shape.hpp"
#include "shapes/sphere.hpp"
#include "shapes/triangle.hpp"

GPU_Camera convertCamera(const Camera& cam) {
  GPU_Camera gpuCam;
  gpuCam.position =
      simd_make_float3(cam.position.x(), cam.position.y(), cam.position.z());
  gpuCam.direction =
      simd_make_float3(cam.direction.x(), cam.direction.y(), cam.direction.z());
  gpuCam.fov = static_cast<float>(cam.fov);
  return gpuCam;
}

GPU_Material convertMaterial(const Material& mat) {
  GPU_Material gpuMat;
  gpuMat.color = simd_make_float3(mat.color.r(), mat.color.g(), mat.color.b());
  gpuMat.specular =
      simd_make_float3(mat.specular.r(), mat.specular.g(), mat.specular.b());
  gpuMat.specularFactor = static_cast<float>(mat.specularFactor);
  gpuMat.shininess = static_cast<float>(mat.shininess);
  gpuMat.reflectivity = static_cast<float>(mat.reflectivity);
  return gpuMat;
}

GPU_Light convertLight(const Light& light) {
  GPU_Light gpuLight;
  gpuLight.position = simd_make_float3(light.position.x(), light.position.y(),
                                       light.position.z());
  gpuLight.color =
      simd_make_float3(light.color.r(), light.color.g(), light.color.b());
  return gpuLight;
}

GPU_Ray convertRay(const Ray& ray) {
  GPU_Ray gpuRay;
  gpuRay.origin = simd_make_float3(ray.orig.x(), ray.orig.y(), ray.orig.z());
  gpuRay.direction = simd_make_float3(ray.dir.x(), ray.dir.y(), ray.dir.z());
  return gpuRay;
}

GPU_HitInfo convertHitInfo(const HitInfo& hitInfo, int materialIndex) {
  GPU_HitInfo gpuHit;
  gpuHit.position =
      simd_make_float3(hitInfo.pos.x(), hitInfo.pos.y(), hitInfo.pos.z());
  gpuHit.normal = simd_make_float3(hitInfo.normal.x(), hitInfo.normal.y(),
                                   hitInfo.normal.z());
  gpuHit.t = static_cast<float>(hitInfo.t);
  gpuHit.materialIndex = materialIndex;
  return gpuHit;
}

GPU_Node convertNode(const BVHNode& node) {
  GPU_Node gpuNode;
  gpuNode.boundsMin = simd_make_float3(node.bounds.min.x(), node.bounds.min.y(),
                                       node.bounds.min.z());
  gpuNode.boundsMax = simd_make_float3(node.bounds.max.x(), node.bounds.max.y(),
                                       node.bounds.max.z());
  gpuNode.left = node.left;
  gpuNode.right = node.right;
  gpuNode.shapeIndex = node.shapeIndex;
  gpuNode.shapeCount = node.shapeCount;
  gpuNode.shapeType = node.shapeType;
  return gpuNode;
}

GPU_Triangle convertTriangle(const Triangle& tri) {
  GPU_Triangle gpuTri;
  gpuTri.v0 = simd_make_float3(tri.v0.x(), tri.v0.y(), tri.v0.z());
  gpuTri.v1 = simd_make_float3(tri.v1.x(), tri.v1.y(), tri.v1.z());
  gpuTri.v2 = simd_make_float3(tri.v2.x(), tri.v2.y(), tri.v2.z());
  gpuTri.n0 = simd_make_float3(tri.n0.x(), tri.n0.y(), tri.n0.z());
  gpuTri.n1 = simd_make_float3(tri.n1.x(), tri.n1.y(), tri.n1.z());
  gpuTri.n2 = simd_make_float3(tri.n2.x(), tri.n2.y(), tri.n2.z());
  gpuTri.materialIndex = static_cast<uint32_t>(tri.materialIndex);
  return gpuTri;
}

GPU_Sphere convertSphere(const Sphere& sph) {
  GPU_Sphere gpuSph;
  gpuSph.center =
      simd_make_float3(sph.center.x(), sph.center.y(), sph.center.z());
  gpuSph.radius = static_cast<float>(sph.radius);
  gpuSph.materialIndex = static_cast<uint32_t>(sph.materialIndex);
  return gpuSph;
}

GPU_Plane convertPlane(const Plane& plane, int materialIndex) {
  GPU_Plane gpuPlane;
  gpuPlane.point =
      simd_make_float3(plane.point.x(), plane.point.y(), plane.point.z());
  gpuPlane.normal =
      simd_make_float3(plane.normal.x(), plane.normal.y(), plane.normal.z());
  gpuPlane.materialIndex = static_cast<uint32_t>(materialIndex);
  return gpuPlane;
}
