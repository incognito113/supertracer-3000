#ifndef INTERSECTIONS_METAL
#define INTERSECTIONS_METAL

#include <metal_stdlib>
using namespace metal;

#include "structs.metal"

constant float EPS = 0.0001;

inline bool intersectsSphere(const GPU_Ray ray, const device GPU_Sphere* sphere,
                             thread GPU_HitInfo* hitInfo) {
  float a = dot(ray.dir, ray.dir);
  float b = 2.0 * dot(ray.dir, ray.orig - sphere->center);
  float c = dot(ray.orig - sphere->center, ray.orig - sphere->center) -
            sphere->radius * sphere->radius;

  float discriminant = b * b - 4 * a * c;

  // Negative discriminant means no intersection
  if (discriminant < 0) {
    return false;
  }

  float sqrtDisc = sqrt(discriminant);
  float t1 = (-b - sqrtDisc) / (2.0 * a);
  float t2 = (-b + sqrtDisc) / (2.0 * a);

  // Find the nearest positive intersection
  // We know that t1 <= t2, so check t1 first
  float t = -1.0;
  if (t1 > EPS) {
    t = t1;
  } else if (t2 > EPS) {
    t = t2;
  }

  // Both intersections are negative, no intersection
  if (t < 0.0) {
    return false;
  }

  // Calculate intersection details
  hitInfo->t = t;
  hitInfo->position = ray.orig + t * ray.dir;
  hitInfo->normal = normalize(hitInfo->position - sphere->center);
  hitInfo->materialIndex = sphere->materialIndex;

  return true;
}

inline bool intersectsPlane(const GPU_Ray ray, const device GPU_Plane* plane,
                            thread GPU_HitInfo* hitInfo) {
  float denom = dot(plane->normal, ray.dir);

  // Ray is essentially parallel to the plane, no intersection
  if (abs(denom) < EPS) {
    return false;
  }

  float t = dot(plane->point - ray.orig, plane->normal) / denom;
  // Negative t, no intersection
  if (t < EPS) {
    return false;
  }

  hitInfo->t = t;
  hitInfo->position = ray.orig + t * ray.dir;
  hitInfo->normal = plane->normal;
  hitInfo->materialIndex = plane->materialIndex;

  return true;
}

inline bool intersectsTriangle(const GPU_Ray ray,
                               const device GPU_Triangle* triangle,
                               thread GPU_HitInfo* hitInfo) {
  float3 edge1 = triangle->v1 - triangle->v0;
  float3 edge2 = triangle->v2 - triangle->v0;
  float3 rayCrossEdge2 = cross(ray.dir, edge2);
  float det = dot(edge1, rayCrossEdge2);

  if (abs(det) < EPS) {
    return false;  // Ray is parallel to triangle
  }

  float invDet = 1.0 / det;
  float3 s = ray.orig - triangle->v0;
  float u = dot(s, rayCrossEdge2) * invDet;

  // Validate u parameter
  if (u < EPS || u > 1.0 + EPS) {
    return false;
  }

  float3 sCrossEdge1 = cross(s, edge1);
  float v = invDet * dot(ray.dir, sCrossEdge1);

  // Validate v parameter
  if (v < EPS || v > 1.0 + EPS || u + v > 1.0 + EPS) {
    return false;
  }

  float t = invDet * dot(edge2, sCrossEdge1);

  if (t < EPS) {
    return false;  // Intersection behind ray origin
  }

  // Compute intersection details
  hitInfo->t = t;
  hitInfo->position = ray.orig + t * ray.dir;

  // Calculate interpolated normal (barycentric interpolation)
  // If vertex normals are all equivalent, this is just that normal
  float3 n0 = triangle->n0;
  float3 n1 = triangle->n1;
  float3 n2 = triangle->n2;
  hitInfo->normal = normalize(n0 * (1.0 - u - v) + n1 * u + n2 * v);
  hitInfo->materialIndex = triangle->materialIndex;

  return true;
}

inline bool intersectsNode(const GPU_Ray ray, const device GPU_Node* node,
                           thread float* tmin, thread float* tmax) {
  *tmin = 0.0;
  *tmax = FLT_MAX;

#pragma unroll
  for (int i = 0; i < 3; ++i) {
    float invD = 1.0 / ray.dir[i];
    float t0 = (node->boundsMin[i] - ray.orig[i]) * invD;
    float t1 = (node->boundsMax[i] - ray.orig[i]) * invD;

    if (invD < 0.0) {
      float temp = t0;
      t0 = t1;
      t1 = temp;
    }

    *tmin = max(t0, *tmin);
    *tmax = min(t1, *tmax);

    if (*tmax <= *tmin) return false;
  }
  return true;
}

#endif