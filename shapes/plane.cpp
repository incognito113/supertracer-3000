#include "plane.hpp"

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

// Constr plane (no bounding box needed for infinite shape)
Plane::Plane(const Vector& pt, const Vector& norm, const Material& mat)
    : Shape(mat), point(pt), normal(norm.norm()) {}

// Calculate intersection of ray with plane
std::optional<HitInfo> Plane::intersects(const Ray& ray) const {
  double denom = normal.dot(ray.dir);

  // Ray is essentially parallel to the plane, no intersection
  if (std::abs(denom) < Vector::EPS) {
    return std::nullopt;
  }

  double t = (point - ray.orig).dot(normal) / denom;
  // Negative t, no intersection
  if (t < Vector::EPS) {
    return std::nullopt;
  }

  Vector hitPoint = ray.at(t);
  return HitInfo(hitPoint, normal, ray, t, &material);
}