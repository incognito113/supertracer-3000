#include "camera.hpp"

#include <numbers>

#include "ray.hpp"
#include "vector.hpp"

// Generate a ray from the camera through pixel (i, j)
// i, j can be fractional for anti-aliasing
Ray Camera::ray(const double i, const double j, const int width,
                const int height) const {
  const double w = static_cast<double>(width);
  const double h = static_cast<double>(height);

  // Start at top-left corner of pixel
  double u = i / w;
  double v = j / h;

  // Screen half-sizes
  double halfHeight = tan(fov * 0.5 * M_PI / 180.0);
  double halfWidth = (w / h) * halfHeight;

  // Camera basis (Y up)
  Vector right = direction.cross(Vector(0, 0, 1)).norm();
  Vector up = right.cross(direction).norm();

  // Image plane coordinates
  double x = (2.0 * u - 1.0) * halfWidth;
  double y = (1.0 - 2.0 * v) * halfHeight;

  // Place image plane at distance 1 from camera position
  Vector worldPoint = position + direction + right * x + up * y;
  Vector rayDir = (worldPoint - position).norm();

  return Ray(position, rayDir);
}

void Camera::setDir(const Vector& dir) {
  direction = dir.norm();
  // Update yaw and pitch based on new direction
  pitch = asin(direction.z());
  yaw = atan2(direction.y(), direction.x());
}

// Rotate camera direction by Euler angles (given mouse movement deltas)
void Camera::eulerRotate(int dx, int dy) {
  yaw += dx * sensitivity;
  pitch += dy * sensitivity;
  // Clamp pitch to avoid flipping
  pitch = std::clamp(pitch, -M_PI / 2.0 + 0.01, M_PI / 2.0 - 0.01);

  // Calculate forward direction vector
  Vector forward =
      Vector(cos(pitch) * cos(yaw), cos(pitch) * sin(yaw), sin(pitch));

  direction = forward.norm();
}

void Camera::movePosition(const Vector& delta) {
  Vector right = direction.cross(Vector(0, 0, 1)).norm();
  Vector up = right.cross(direction).norm();

  position += right * delta.x() + up * delta.y() + direction * delta.z();
}

void Camera::zoom(double scroll) {
  double factor = std::exp(-scroll * scrollSens);  // smooth
  fov *= factor;
  fov = std::clamp(fov, 20.0, 90.0);
}
