#pragma once

#include "math/color.hpp"
#include "math/ray.hpp"
#include "scene/scene.hpp"

// Forward declaration
class Renderer;

// Responsible for tracing rays through the scene and computing pixel colors
class Tracer {
 private:
  const Scene& scene;

  const Color traceRay(const Scene& scene, const Ray& ray, int depth) const;
  const Color computeLighting(const Scene& scene, const HitInfo& hitInfo,
                              int depth) const;

 public:
  Tracer(const Scene& sc) : scene(sc) {}

  std::vector<Color> render_pixels(const Scene& scene);

  ~Tracer() = default;

  friend class Renderer;
};