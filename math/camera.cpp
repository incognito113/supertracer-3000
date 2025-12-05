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

void Camera::updateRight() {
  // If direction is too close to WORLD_UP or -WORLD_UP, use fallback
  if (fabs(direction.dot(WORLD_UP)) > 0.99) {
    right = Vector(1, 0, 0);  // arbitrary but stable
  } else {
    right = direction.cross(WORLD_UP).norm();
  }
}

void Camera::updateUp() { up = right.cross(direction).norm(); }

void Camera::setDir(const Vector& dir) {
  if (dir.magSq() < Vector::EPS * Vector::EPS) {
    throw std::invalid_argument("Camera direction cannot be zero vector");
  }

  Vector d = dir.norm();

  // Check if vertical
  if (std::fabs(d.x()) < Vector::EPS && std::fabs(d.y()) < Vector::EPS) {
    // Looking straight up or down
    yaw = d.z() > 0.5 ? -M_PI / 2.0 : M_PI / 2.0;
    pitch = (d.z() > 0.5 ? M_PI / 2.0 : -M_PI / 2.0);
  } else {
    // Normal case
    pitch = std::asin(d.z());
    yaw = std::atan2(d.y(), d.x());
  }

  // Recompute direction from yaw + pitch
  direction = Vector(std::cos(pitch) * std::cos(yaw),
                     std::cos(pitch) * std::sin(yaw), std::sin(pitch))
                  .norm();
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
  updateRight();
  updateUp();
}

void Camera::movePosition(const Vector& delta) {
  position += right * delta.x() + up * delta.y() + direction * delta.z();
}

void Camera::zoom(double scroll) {
  double factor = std::exp(-scroll * scrollSens);  // smooth
  fov *= factor;
  fov = std::clamp(fov, 20.0, 90.0);
}
