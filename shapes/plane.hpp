#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

class Plane : public Shape {
 public:
  Vector point;
  Vector normal;

  Plane(const Vector& pt, const Vector& norm, const Material& mat)
      : Shape(mat), point(pt), normal(norm.norm()) {}

  virtual std::optional<HitInfo> intersects(const Ray& ray) const override;
};