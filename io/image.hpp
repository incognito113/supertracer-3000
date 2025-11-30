#pragma once

#include <string>
#include <vector>

#include "math/color.hpp"
#include "scene/scene.hpp"

// Handles image creation and saving to files
// Quality parameter controls number of samples per pixel
class Image {
 private:
  const Scene& scene;
  std::vector<Color>& pixels;

 public:
  Image(const Scene& sc);
  bool save(std::string filename = "") const;

  ~Image() = default;
};