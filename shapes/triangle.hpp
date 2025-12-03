#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

// Represents a triangle in 3D space
class Triangle : public BoundedShape {
 public:
  const Vector v0;
  const Vector v1;
  const Vector v2;
  const Vector normal;

  Triangle(const Vector& a, const Vector& b, const Vector& c,
           const Material& mat);

  std::optional<HitInfo> intersects(const Ray& ray) const override;

  Triangle* clone() const override { return new Triangle(*this); }
};