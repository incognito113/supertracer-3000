#pragma once

#include <simd/simd.h>

struct GPU_Camera {
  simd_float3 position;
  simd_float3 direction;
  float fov;
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
  int32_t shapeIndex;  // Index into scene's shape array (-1 if not leaf)
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