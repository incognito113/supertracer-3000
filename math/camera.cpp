#include "camera.hpp"

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
