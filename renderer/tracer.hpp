#pragma once

#include "math/color.hpp"
#include "math/ray.hpp"
#include "scene/scene.hpp"

// Responsible for tracing rays through the scene and computing pixel colors
class Tracer {
 private:
  const Color traceRay(const Ray& ray) const;
  const Color computeLighting(const HitInfo& hitInfo) const;

 public:
  const Scene& scene;

  explicit Tracer(const Scene& sc) : scene(sc) {}
  Tracer(const Tracer& other) = default;

  const std::vector<Color> render_pixels() const;

  ~Tracer() = default;
};