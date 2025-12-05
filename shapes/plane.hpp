#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

// Represents an infinite plane
class Plane : public Shape {
 public:
  Vector point;   // a point on plane
  Vector normal;  // normalized normal vector

  Plane(const Vector& p, const Vector& n, const size_t matIndex);

  std::optional<HitInfo> intersects(const Ray& ray) const override;
  int getShapeType() const override { return 2; }

  Plane* clone() const override { return new Plane(*this); }
};