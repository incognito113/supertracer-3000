#pragma once

#include <string>
#include <vector>

#include "math/color.hpp"
#include "renderer/renderer.hpp"

// Handles image creation and saving to files
// Quality parameter controls number of samples per pixel
class Image {
 private:
  const Scene& scene;
  const std::vector<uint8_t> pixels;

 public:
  Image(const Renderer& renderer)
      : scene(renderer.scene), pixels(renderer.image8) {}
  Image(Scene& sc, int quality = 17);
  bool save(std::string filename = "") const;

  ~Image() = default;
};