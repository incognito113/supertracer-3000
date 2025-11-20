#pragma once

#include "math/color.hpp"
#include "math/ray.hpp"
#include "scene/scene.hpp"

class Tracer {
 public:
  const Scene& scene;

  explicit Tracer(const Scene& sc) : scene(sc) {}
  Tracer(const Tracer& other) = default;

  const Color traceRay(const Ray& ray) const;
  const Color computeLighting(const HitInfo& hitInfo) const;
  const std::vector<Color> render_pixels() const;
};