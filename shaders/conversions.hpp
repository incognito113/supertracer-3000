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
    simd_float3 origin;
    simd_float3 direction;
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
    int32_t left;        // Index of left child in BVH array (-1 if leaf)
    int32_t right;       // Index of right child in BVH array (-1 if leaf)
    int32_t shapeIndex;  // Index into shape array of this type (-1 if not leaf)
    int32_t shapeCount;  // Number of objects in this node (0 if not leaf)
    int32_t shapeType;   // 0 = triangle, 1 = sphere, 2 = plane (-1 if not leaf)
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
    std::vector<GPU_Triangle> triangles;
    std::vector<GPU_Sphere> spheres;
    std::vector<GPU_Plane> planes;
  };

 private:
  GPU_SceneData convertSceneData(const Scene& scene, const BVH& bvh,
                                 int32_t numSpheres, int32_t numTriangles);
  GPU_Material convertMaterial(const Material& mat);
  GPU_Light convertLight(const Light& light);
  GPU_Ray convertRay(const Ray& ray);
  GPU_Node convertNode(const BVHNode& node, int32_t shapeIndex);
  GPU_HitInfo convertHitInfo(const HitInfo& hitInfo, int materialIndex);
  GPU_Triangle convertTriangle(const Triangle& tri);
  GPU_Sphere convertSphere(const Sphere& sph);
  GPU_Plane convertPlane(const Plane& plane);

 public:
  Converter() = default;

  GPU_Data convertAll(const Scene& scene, const BVH& bvh);

  ~Converter() = default;
};

#endif