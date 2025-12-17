#pragma once

#include <stddef.h>

#include <limits>
#include <optional>

#include "math/ray.hpp"
#include "math/vector.hpp"

// Stores information about collisions between rays and shapes
struct HitInfo {
  const Vector pos;     // Point of intersection
  const Vector normal;  // Surface normal at intersection
  const Ray ray;        // Ray that caused the hit
  const double t;
  const size_t materialIndex;

  HitInfo(const Vector& p, const Vector& n, const Ray& r, double tval,
          const size_t matIndex)
      : pos(p), normal(n), ray(r), t(tval), materialIndex(matIndex) {}
  HitInfo(const HitInfo& other)
      : pos(other.pos),
        normal(other.normal),
        ray(other.ray),
        t(other.t),
        materialIndex(other.materialIndex) {};
};

// Abstract base class for all shapes in the scene
class Shape {
 protected:
  static constexpr int TRIANGLE = 0;
  static constexpr int SPHERE = 1;
  static constexpr int PLANE = 2;
  static constexpr int CYLINDER = 3;

 public:
  const size_t materialIndex;

  Shape(const size_t matIndex) : materialIndex(matIndex) {}

  // Returns HitInfo if intersection, std::nullopt otherwise
  virtual std::optional<HitInfo> intersects(const Ray& ray) const = 0;
  virtual int getShapeType() const = 0;  // 0 = triangle, 1 = sphere, 2 = plane
  virtual Shape* clone() const = 0;

  virtual ~Shape() = default;

  friend class Converter;
};

struct Bounds {
 private:
  void compCenter();
  void compArea();

 public:
  Vector min;
  Vector max;
  Vector center;
  double area;

  Bounds()
      : min(std::numeric_limits<double>::max()),
        max(-std::numeric_limits<double>::max()),
        center(),
        area(0) {}
  Bounds(const Vector& point)
      : min(point), max(point), center(point), area(0) {}
  Bounds(const Vector& bmin, const Vector& bmax) : min(bmin), max(bmax) {
    compCenter();
    compArea();
  }

  void expand(const Bounds& other);
  void expand(const Vector& point);
  bool intersects(const Ray& ray, double& tmin, double& tmax) const;

  ~Bounds() = default;
};

class BoundedShape : public Shape {
 public:
  Bounds bounds;

  BoundedShape(const Vector& bmin, const Vector& bmax, const size_t matIndex)
      : Shape(matIndex), bounds(bmin, bmax) {}

  virtual ~BoundedShape() = default;
};