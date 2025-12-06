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
  float t_pos1 = (t1 > EPS) ? t1 : -1.0f;
  float t_pos2 = (t2 > EPS) ? t2 : -1.0f;
  float t = max(t_pos1, t_pos2);

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
