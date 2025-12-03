#pragma once

#include <random>

#include "ray.hpp"
#include "vector.hpp"

// Forward declaration
class Tracer;
class Scene;

// Represents the camera in the scene
class Camera {
 private:
  double yaw = 0.0;
  double pitch = 0.0;

 public:
  Vector position;
  Vector direction;  // Must be normalized
  double fov;        // In degrees
  static constexpr double sensitivity =
      0.0015;                                 // Mouse sensitivity for rotation
  static constexpr double scrollSens = 0.01;  // Scroll sensitivity for zoom

  Camera() : position(), direction(1, 0, 0), fov(60.0) {}
  Camera(const Camera& other)
      : yaw(other.yaw),
        pitch(other.pitch),
        position(other.position),
        direction(other.direction),
        fov(other.fov) {}

  void setDir(const Vector& dir);
  void eulerRotate(int dx, int dy);
  void movePosition(const Vector& delta);
  void zoom(double scroll);
  Ray ray(const double i, const double j, const int width,
          const int height) const;

  ~Camera() = default;

  friend class Tracer;
  friend class Scene;
};