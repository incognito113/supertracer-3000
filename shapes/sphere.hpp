#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

class Sphere : public Shape {
 public:
  Vector center;
  double radius;

  Sphere(const Vector& cen, double r, const Material& mat)
      : Shape(mat), center(cen), radius(r) {}
  virtual std::optional<HitInfo> intersects(const Ray& ray) const override;
};