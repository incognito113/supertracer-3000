#pragma once

#include <stddef.h>

#include "math/vector.hpp"
#include "shape.hpp"

// A finite vertical cylinder aligned with the Z-axis.

class Cylinder : public BoundedShape {
 public:
  Vector center;  // geometric center
  double radius;  // radius in the xy-plane
  double height;  // total height of the cylinder

  Cylinder(const Vector& c, double r, double h, size_t matIndex);

  std::optional<HitInfo> intersects(const Ray& ray) const override;

  int getShapeType() const override { return Shape::CYLINDER; }

  Cylinder* clone() const override { return new Cylinder(*this); }
};