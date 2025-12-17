#pragma once

#include <stddef.h>

#include "math/vector.hpp"
#include "shape.hpp"

// Represents a sphere in 3D space
class Sphere : public BoundedShape {
 public:
  const Vector center;
  const double radius;

  Sphere(const Vector& cen, double r, const size_t matIndex);

  std::optional<HitInfo> intersects(const Ray& ray) const override;
  int getShapeType() const override { return Shape::SPHERE; }

  Sphere* clone() const override { return new Sphere(*this); }
};