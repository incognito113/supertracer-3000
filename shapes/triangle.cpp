#include "triangle.hpp"

// Construct triangle and compute normal
Triangle::Triangle(const Vector& a, const Vector& b, const Vector& c,
                   const Material& mat)
    : BoundedShape(mat, a.min(b).min(c), a.max(b).max(c)),
      v0(a),
      v1(b),
      v2(c),
      normal((b - a).cross(c - a).norm()) {}

// Calculate intersection of ray with triangle using Möller–Trumbore algorithm
// Using implementation from wikipedia
std::optional<HitInfo> Triangle::intersects(const Ray& ray) const {
  Vector edge1 = v1 - v0;
  Vector edge2 = v2 - v0;
  Vector rayCrossEdge2 = ray.dir.cross(edge2);
  double det = edge1 * rayCrossEdge2;

  if (std::abs(det) < Vector::EPS)
    return std::nullopt;  // Ray is parallel to triangle plane

  double invDet = 1.0 / det;
  Vector s = ray.orig - v0;
  double u = (s * rayCrossEdge2) * invDet;

  // Validate u parameter
  if (u < Vector::EPS || u > 1.0 + Vector::EPS) return std::nullopt;

  Vector sCrossEdge1 = s.cross(edge1);
  double v = invDet * (ray.dir * sCrossEdge1);

  // Validate v parameter
  if (v < -Vector::EPS || v > 1.0 + Vector::EPS || u + v > 1.0 + Vector::EPS)
    return std::nullopt;

  double t = invDet * (edge2 * sCrossEdge1);

  if (t < Vector::EPS) return std::nullopt;  // Intersection behind ray origin

  // Calculate intersection details
  return HitInfo{ray.at(t), normal, ray, t, &material};
}