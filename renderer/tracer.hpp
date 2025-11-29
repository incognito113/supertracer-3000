#pragma once

#include "math/color.hpp"
#include "math/ray.hpp"
#include "scene/scene.hpp"

// Responsible for tracing rays through the scene and computing pixel colors
class Tracer {
 public:
  const Scene& scene;

 private:
  const Color traceRay(const Ray& ray, int depth = 6) const;
  // i know this is bad to have the 6 here too but honestly... it was giving me so much headaches trying to get it from scene that idk. if you can fix that, please do.

 public:
  explicit Tracer(const Scene& sc) : scene(sc) {}
  Tracer(const Tracer& other) = default;

  const std::vector<Color> render_pixels() const;

  ~Tracer() = default;
};
