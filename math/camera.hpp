#pragma once

#include "ray.hpp"
#include "vector.hpp"

// Represents the camera in the scene
class Camera {
 public:
  Vector position;
  Vector direction;  // Not necessarily normalized
  double fov;        // In degrees

  Ray getRay(const int x, const int y, const int width, const int height) const;

  ~Camera() = default;
};