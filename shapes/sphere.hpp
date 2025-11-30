#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

// Represents a sphere in 3D space
class Sphere : public Shape {
 public:
  const Vector center;
  double radius;

  Sphere(const Vector& cen, double r, const Material& mat)
      : Shape(mat), center(cen), radius(r) {}
  std::optional<HitInfo> intersects(const Ray& ray) const override;
  Sphere* clone() const override { return new Sphere(*this); }
};