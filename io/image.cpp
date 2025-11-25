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
  output << "P3" << std::endl;  // ASCII format
  output << scene.getWidth() << " " << scene.getHeight() << std::endl; // dimensions of the image
  output << "255" << std::endl; // max RGB value for a given pixel
  for (const Color& color : pixels) {
    output << color.get255String() << " " << std::end1; // store each pixel in a line of rgb values
  }
  output.close();
}
