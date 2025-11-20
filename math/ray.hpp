#pragma once

#include "math/vector.hpp"

// Represents a ray in 3D space with an origin and direction
class Ray {
 public:
  const Vector orig;
  const Vector dir;

  Ray(const Vector& origin, const Vector& direction)
      : orig(origin), dir(direction) {}

  // Returns the point along the ray at distance t from the origin
  Vector at(double t) const;

  ~Ray() = default;
};