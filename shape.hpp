#pragma once

#include <optional>

#include "color.hpp"
#include "tracer.hpp"
#include "vector.hpp"

// Stores matieral properties for shapes
struct Material {
  const Color diffuse;
  const Color specular;
  const double shininess;
  const double reflectivity;
};

// Stores information about collisions between rays and shapes
struct HitInfo {
  const Vector pos;
  const Vector normal;
  const double t;
  const Material* material;

  HitInfo(const Vector& p, const Vector& n, double tval, const Material* m)
      : pos(p), normal(n), t(tval), material(m) {};
};

class Shape {
 protected:
  static constexpr double EPS = 1e-9;

 public:
  const Material material;

  Shape(const Material& mat) : material(mat) {};

  // Returns HitInfo if intersection, std::nullopt otherwise
  virtual std::optional<HitInfo> intersects(const Ray& ray) const = 0;

  virtual ~Shape() {};
};
