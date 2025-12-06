#ifndef STRUCTS_METAL
#define STRUCTS_METAL

#include <metal_stdlib>
using namespace metal;

struct GPU_SceneData {
  uint width;
  uint height;
  uint maxReflections;
  uint iteration;
  float3 backgroundColor;
  float3 position;
  float3 direction;
  float fov;
  uint numLights;
  uint numMaterials;
  uint numPlanes;
  uint numSpheres;
  uint numTriangles;
  uint numNodes;
};

struct GPU_Material {
  float3 color;
  float3 specular;
  float specularFactor;
  float shininess;
  float reflectivity;
};

struct GPU_Light {
  float3 position;
  float3 color;
};

struct GPU_Ray {
  float3 orig;
  float3 dir;
};

struct GPU_HitInfo {
  float3 position;
  float3 normal;
  float t;
  int materialIndex;
};

struct GPU_Node {
  float3 boundsMin;
  float3 boundsMax;
  int left;
  int right;
  int shapeIndex;
  int shapeCount;
  int shapeType;
};

struct GPU_Triangle {
  float3 v0, v1, v2;
  float3 n0, n1, n2;
  int materialIndex;
};

struct GPU_Sphere {
  float3 center;
  float radius;
  int materialIndex;
};

struct GPU_Plane {
  float3 point;
  float3 normal;
  int materialIndex;
};

#endif