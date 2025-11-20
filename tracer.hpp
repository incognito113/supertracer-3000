#pragma once

#include "color.hpp"
#include "vector.hpp"

struct Light {
  Vector position;
  Color color;
};

class Ray {
 public:
  const Vector orig;
  const Vector dir;

  Ray(const Vector& origin, const Vector& direction)
      : orig(origin), dir(direction) {}

  // Returns the point along the ray at distance t from the origin
  Vector at(double t) const;
};

class Camera {
 public:
  Vector position;
  Vector direction;  // Not necessarily normalized
  double fov;        // In degrees

  Ray getRay(const int x, const int y, const int width, const int height) const;
};