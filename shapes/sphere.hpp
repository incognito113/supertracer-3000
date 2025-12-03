#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

// Represents a sphere in 3D space
class Sphere : public BoundedShape {
 public:
  const Vector center;
  const double radius;

  Sphere(const Vector& cen, double r, const Material& mat);

  std::optional<HitInfo> intersects(const Ray& ray) const override;

  Sphere* clone() const override { return new Sphere(*this); }
};