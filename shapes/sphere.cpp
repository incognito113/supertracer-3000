#include "sphere.hpp"

#include <cmath>
#include <optional>

#include "math/vector.hpp"
#include "scene/light.hpp"

// Calculate intersection of ray with sphere
std::optional<HitInfo> Sphere::intersects(const Ray& ray) const {
  double a = ray.dir * ray.dir;
  double b = 2.0 * (ray.dir * (ray.orig - center));
  double c = (ray.orig - center) * (ray.orig - center) - radius * radius;

  double discriminant = b * b - 4 * a * c;

  // Negative discriminant means no intersection
  if (discriminant < 0) {
    return std::nullopt;
  }

  double sqrtDisc = sqrt(discriminant);
  double t1 = (-b - sqrtDisc) / (2.0 * a);
  double t2 = (-b + sqrtDisc) / (2.0 * a);

  // Find the nearest positive intersection
  // We know that t1 <= t2, so check t1 first
  double t = (t1 > EPS) ? t1 : ((t2 > 1e-6) ? t2 : -1);

  // Both intersections are negative, no intersection
  if (t < 0) {
    return std::nullopt;
  }

  // Calculate intersection details
  const Vector pos = ray.at(t);
  const Vector normal = (pos - center).norm();
  const Material* mat = &material;

  const HitInfo hitInfo(pos, normal, ray, t, mat);
  return hitInfo;
}