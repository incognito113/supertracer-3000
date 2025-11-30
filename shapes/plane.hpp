#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

// Represents an infinite plane in 3D space
class Plane : public Shape {
 public:
  const Vector point;
  const Vector normal;

  Plane(const Vector& pt, const Vector& norm, const Material& mat)
      : Shape(mat), point(pt), normal(norm.norm()) {}

  virtual std::optional<HitInfo> intersects(const Ray& ray) const override;
  Plane* clone() const override { return new Plane(*this); }
};