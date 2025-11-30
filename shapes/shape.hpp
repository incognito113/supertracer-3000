#pragma once

#include <optional>

#include "math/color.hpp"
#include "math/ray.hpp"
#include "math/vector.hpp"
#include "scene/material.hpp"

// Stores information about collisions between rays and shapes
struct HitInfo {
  const Vector pos;     // Point of intersection
  const Vector normal;  // Surface normal at intersection
  const Ray ray;        // Ray that caused the hit
  const double t;
  const Material* material;

  HitInfo(const Vector& p, const Vector& n, const Ray& r, double tval,
          const Material* m)
      : pos(p), normal(n), ray(r), t(tval), material(m) {};
  HitInfo(const HitInfo& other)
      : pos(other.pos),
        normal(other.normal),
        ray(other.ray),
        t(other.t),
        material(other.material) {};
};

// Forward declaration
class Tracer;

// Abstract base class for all shapes in the scene
class Shape {
 protected:
  static constexpr double EPS = 1e-5;

 public:
  const Material material;

  Shape(const Material& mat) : material(mat) {};

  // Returns HitInfo if intersection, std::nullopt otherwise
  virtual std::optional<HitInfo> intersects(const Ray& ray) const = 0;
  virtual Shape* clone() const = 0;

  virtual ~Shape() {};

  friend class Tracer;
};
