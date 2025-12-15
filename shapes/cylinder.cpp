#include "cylinder.hpp"

#include <algorithm>
#include <cmath>

Cylinder::Cylinder(const Vector& c, double r, double h, size_t matIndex)
    : BoundedShape(c - Vector(r, r, h * 0.5), c + Vector(r, r, h * 0.5),
                   matIndex),
      center(c),
      radius(r),
      height(h) {}

// Ray–cylinder intersection
std::optional<HitInfo> Cylinder::intersects(const Ray& ray) const {
  const double EPS = Vector::EPS;

  Vector o = ray.orig - center;
  Vector d = ray.dir;

  const double zMin = -height * 0.5;
  const double zMax = height * 0.5;

  //
  double a = d.x() * d.x() + d.y() * d.y();
  double b = 2.0 * (o.x() * d.x() + o.y() * d.y());
  double c = o.x() * o.x() + o.y() * o.y() - radius * radius;

  double tSide = -1.0;
  double disc = b * b - 4.0 * a * c;

  if (disc >= 0.0 && std::abs(a) > EPS) {
    double s = std::sqrt(disc);
    double t1 = (-b - s) / (2.0 * a);
    double t2 = (-b + s) / (2.0 * a);

    auto checkSide = [&](double t) {
      if (t < EPS) return -1.0;
      double z = o.z() + d.z() * t;  // local z
      return (z >= zMin && z <= zMax) ? t : -1.0;
    };

    double c1 = checkSide(t1);
    double c2 = checkSide(t2);

    if (c1 > 0.0 && c2 > 0.0)
      tSide = std::min(c1, c2);
    else
      tSide = std::max(c1, c2);
  }

  // Cap disks (z = ±height/2 in local coordinates)
  auto cap = [&](double zPlaneLocal) {
    if (std::abs(d.z()) < EPS) return -1.0;

    double t = (zPlaneLocal - o.z()) / d.z();
    if (t < EPS) return -1.0;

    Vector pLocal = o + d * t;
    if (pLocal.x() * pLocal.x() + pLocal.y() * pLocal.y() <=
        radius * radius + EPS) {
      return t;
    }
    return -1.0;
  };

  double tTop = cap(zMax);
  double tBottom = cap(zMin);

  double t = 1e30;
  int type = 0;

  if (tSide > 0.0 && tSide < t) {
    t = tSide;
    type = 1;
  }
  if (tTop > 0.0 && tTop < t) {
    t = tTop;
    type = 2;
  }
  if (tBottom > 0.0 && tBottom < t) {
    t = tBottom;
    type = 3;
  }

  if (type == 0) return std::nullopt;

  Vector pos = ray.at(t);
  Vector normal;

  if (type == 1) {
    Vector pLocal = pos - center;
    normal = Vector(pLocal.x(), pLocal.y(), 0.0).norm();
  } else if (type == 2) {
    normal = Vector(0.0, 0.0, 1.0);
  } else {
    normal = Vector(0.0, 0.0, -1.0);
  }

  return HitInfo(pos, normal, ray, t, materialIndex);
}