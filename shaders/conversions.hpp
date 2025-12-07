#pragma once

#ifdef METAL

#include <simd/simd.h>

#include "math/ray.hpp"
#include "scene/bvh.hpp"
#include "scene/light.hpp"
#include "scene/material.hpp"
#include "scene/scene.hpp"
#include "shapes/plane.hpp"
#include "shapes/sphere.hpp"
#include "shapes/triangle.hpp"

class Converter {
 public:
  struct GPU_SceneData {
    uint width;
    uint height;
    uint maxReflections;
    float ambientLight;
    uint iteration;
    simd_float3 backgroundColor;
    simd_float3 position;
    simd_float3 direction;
    float fov;
    uint32_t numLights;
    uint32_t numMaterials;
    uint32_t numPlanes;
    uint32_t numSpheres;
    uint32_t numTriangles;
    uint32_t numNodes;
  };

  struct GPU_Material {
    simd_float3 color;
    simd_float3 specular;
    float specularFactor;
    float shininess;
    float reflectivity;
  };

  struct GPU_Light {
    simd_float3 position;
    simd_float3 color;
  };

  struct GPU_Ray {
    simd_float3 orig;
    simd_float3 dir;
  };

  struct GPU_HitInfo {
    simd_float3 position;
    simd_float3 normal;
    float t;
    uint32_t materialIndex;
  };

  struct GPU_Node {
    simd_float3 boundsMin;
    simd_float3 boundsMax;
    int32_t left;       // Index of left child in BVH array (-1 if leaf)
    int32_t right;      // Index of right child in BVH array (-1 if leaf)
    int32_t primIndex;  // Index into shape ref array (-1 if not leaf)
    int32_t primCount;  // Number of objects in this node (0 if not leaf)
  };

  struct GPU_ShapeRef {
    int32_t shapeIndex;  // Index into triangle/sphere array
    int32_t shapeType;
  };

  struct GPU_Triangle {
    simd_float3 v0, v1, v2;
    simd_float3 n0, n1, n2;
    uint32_t materialIndex;
  };

  struct GPU_Sphere {
    simd_float3 center;
    float radius;
    uint32_t materialIndex;
  };

  struct GPU_Plane {
    simd_float3 point;
    simd_float3 normal;
    uint32_t materialIndex;
  };

  struct GPU_Data {
    GPU_SceneData sceneData;
    std::vector<GPU_Material> materials;
    std::vector<GPU_Light> lights;
    std::vector<GPU_Node> nodes;
    std::vector<GPU_ShapeRef> shapeRefs;
    std::vector<GPU_Triangle> triangles;
    std::vector<GPU_Sphere> spheres;
    std::vector<GPU_Plane> planes;
  };

 public:
  Converter() = default;

  Vector toVector(const simd_float3& simdVec);
  Color toColor(const simd_float3& simdColor);
  simd_float3 fromVector(const Vector& vec);
  simd_float3 fromColor(const Color& color);

  GPU_Data convertAll(const Scene& scene, const BVH& bvh);
  GPU_SceneData convertSceneData(const Scene& scene, const BVH& bvh,
                                 int32_t numSpheres, int32_t numTriangles);
  GPU_Material convertMaterial(const Material& mat);
  GPU_Light convertLight(const Light& light);
  GPU_Ray convertRay(const Ray& ray);
  GPU_Node convertNode(const BVHNode& node);
  GPU_HitInfo convertHitInfo(const HitInfo& hitInfo, int materialIndex);
  GPU_Triangle convertTriangle(const Triangle& tri);
  GPU_Sphere convertSphere(const Sphere& sph);
  GPU_Plane convertPlane(const Plane& plane);

  ~Converter() = default;
};

#endif