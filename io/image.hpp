#pragma once

#include <string>
#include <vector>

#include "math/color.hpp"
#include "renderer/tracer.hpp"

class Image {
 public:
  const Scene& scene;
  std::vector<Color> pixels;

  Image(const Scene& sc, const std::vector<Color>& px)
      : scene(sc), pixels(px) {}
  Image(const Tracer& tracer) : scene(tracer.scene) {
    pixels = tracer.render_pixels();
  }
  void render(const std::string& filename) const;
};