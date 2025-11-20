#include "image.hpp"

#include <fstream>

#include "renderer/tracer.hpp"

// Render the scene to a ppm file
void Image::render(const std::string& filename) const {
  std::ofstream output(filename);
  if (!output.is_open()) {
    throw std::runtime_error("Failed to open output file: " + filename);
  }

  // PPM format:
  // P3
  // <width> <height>
  // 255
  // r g b r g b r g b ...
  output << "P3" << std::endl;  // ASCII format
  output << scene.getWidth() << " " << scene.getHeight() << std::endl;
  output << "255" << std::endl;
  for (const Color& color : pixels) {
    output << color.get255String() << " ";
  }
  output << std::endl;
  output.close();
}