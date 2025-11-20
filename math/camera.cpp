#include "camera.hpp"

#include "ray.hpp"
#include "vector.hpp"

Ray Camera::getRay(const int i, const int j, const int width,
                   const int height) const {
  // Normalize corrdinates to center of pixel
  double u = (i + 0.5) / width;
  double v = (j + 0.5) / height;

  // Compute size of "screen" based on fov
  double halfHeight = tan(fov * 0.5 * M_PI / 180.0);
  double halfWidth = (width / (double)height) * halfHeight;

  // Camera basis vectors
  Vector right = direction.cross(Vector(0, 1, 0)).norm();
  Vector up = right.cross(direction).norm();
  Vector forward = direction.norm();

  // Image plane coordinates relative to camera
  double x = (2 * u - 1) * halfWidth;
  double y = (1 - 2 * v) * halfHeight;
  double z = 1;  // distance of screen from camera

  // Map to world coordinates
  Vector worldPoint = position + x * right + y * up + z * forward;
  Vector dir = (worldPoint - position).norm();

  return Ray(position, dir);
}