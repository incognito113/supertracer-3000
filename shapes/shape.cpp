#include "shape.hpp"

#include "math/vector.hpp"

// Expand bounds to include another bounds
void Bounds::expand(const Bounds& other) {
  min = min.min(other.min);
  max = max.max(other.max);
  compCenter();
  compArea();
}

// Expand bounds to include a point
void Bounds::expand(const Vector& point) {
  min = min.min(point);
  max = max.max(point);
  compCenter();
  compArea();
}

// Calculate surface area of the bounds
void Bounds::compArea() {
  Vector diff = max - min;
  area =
      2.0 * (diff.x() * diff.y() + diff.y() * diff.z() + diff.z() * diff.x());
}

// Calculate center point of the bounds
void Bounds::compCenter() { center = (min + max) * 0.5; }

// Ray-box intersection test (updates tmin and tmax)
bool Bounds::intersects(const Ray& ray, double& tmin, double& tmax) const {
  tmin = 0.0;
  tmax = std::numeric_limits<double>::max();

  for (int i = 0; i < 3; ++i) {
    double invD = 1.0 / ray.dir[i];
    double t0 = (min[i] - ray.orig[i]) * invD;
    double t1 = (max[i] - ray.orig[i]) * invD;

    if (invD < 0.0) std::swap(t0, t1);

    tmin = std::max(t0, tmin);
    tmax = std::min(t1, tmax);

    if (tmax < tmin) return false;
  }
  return true;
}
