#pragma once

#include <random>

#include "ray.hpp"
#include "vector.hpp"

// Forward declaration
class Tracer;
class Scene;

// Represents the camera in the scene
class Camera {
 public:
  Vector position;
  Vector direction;  // Must be normalized
  double fov;        // In degrees

  Camera() : position(), direction(1, 0, 0), fov(60.0) {}
  Camera(const Camera& other)
      : position(other.position), direction(other.direction), fov(other.fov) {}

  Ray ray(const double i, const double j, const int width,
          const int height) const;

  ~Camera() = default;

  friend class Tracer;
  friend class Scene;
};