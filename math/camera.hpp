#pragma once

#include "ray.hpp"
#include "vector.hpp"

class Camera {
 public:
  Vector position;
  Vector direction;  // Not necessarily normalized
  double fov;        // In degrees

  Ray getRay(const int x, const int y, const int width, const int height) const;
};