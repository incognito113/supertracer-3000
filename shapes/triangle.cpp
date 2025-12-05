#include "triangle.hpp"

// Construct triangle and compute normal
Triangle::Triangle(const Vector& a, const Vector& b, const Vector& c,
                   const size_t matIndex)
    : BoundedShape(a.min(b).min(c), a.max(b).max(c), matIndex),
      v0(a),
      v1(b),
      v2(c),
      n0((b - a).cross(c - a).norm()),
      n1(n0),
      n2(n0) {}

// Construct triangle with per-vertex normals
Triangle::Triangle(const Vector& a, const Vector& b, const Vector& c,
                   const Vector& nA, const Vector& nB, const Vector& nC,
                   const size_t matIndex)
    : BoundedShape(a.min(b).min(c), a.max(b).max(c), matIndex),
      v0(a),
      v1(b),
      v2(c),
      n0(nA.norm()),
      n1(nB.norm()),
      n2(nC.norm()) {}

// Calculate intersection of ray with triangle using Möller–Trumbore
// algorithm Using implementation from wikipedia
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

  // Calculate interpolated normal (barycentric interpolation)
  // If vertex normals are all equivalent, this is just that normal
  Vector normal = (n0 * (1 - u - v) + n1 * u + n2 * v).norm();

  // Calculate intersection details
  return HitInfo{ray.at(t), normal, ray, t, materialIndex};
}