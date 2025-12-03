#include "image.hpp"

#include <fstream>

#include "io/tinyfiledialogs.hpp"
#include "renderer/tracer.hpp"

Image::Image(Scene& sc, int quality)
    : scene(sc),
      pixels(std::vector<uint8_t>(sc.getWidth() * sc.getHeight() * 3, 0)) {
  const Pixels tempPixels(sc.getWidth(), sc.getHeight());
  Tracer tracer{sc};

  for (int q = 0; q < quality; ++q) {
    tracer.refinePixels(const_cast<Pixels&>(tempPixels));
  }
  tracer.wait();

  // Convert to 8-bit per channel
  for (int y = 0; y < sc.getHeight(); ++y) {
    for (int x = 0; x < sc.getWidth(); ++x) {
      const int i = y * sc.getWidth() + x;
      const Color col =
          tempPixels.pxColors[i] / static_cast<double>(tempPixels.pxSamples[i]);
      const int rIndex = i * 3;
      const auto bytes = col.clamp().getBytes();
      const_cast<uint8_t&>(pixels[rIndex]) = bytes[0];
      const_cast<uint8_t&>(pixels[rIndex + 1]) = bytes[1];
      const_cast<uint8_t&>(pixels[rIndex + 2]) = bytes[2];
    }
  }
}

// Save the image to a PPM file (return true on success)
bool Image::save(std::string filename) const {
  char* saveFile;

  if (filename.empty()) {
    // Get filename from user from tinyfiledialogs
    saveFile = tinyfd_saveFileDialog("Save file",     // title
                                     "./output.ppm",  // default name
                                     0, NULL,         // no filters
                                     "Text Files"     // description
    );
    if (!saveFile) return false;
  } else {
    saveFile = const_cast<char*>(filename.c_str());
  }

  std::ofstream output(saveFile, std::ios::binary);
  if (!output.is_open()) {
    return false;
  }

  // PPM format:
  // P6
  // <width> <height>
  // 255
  // r g b r g b r g b ...
  output << "P6" << std::endl;  // Binary PPM
  output << scene.getWidth() << " " << scene.getHeight() << std::endl;
  output << "255" << std::endl;
  for (const uint8_t byte : pixels) {
    output << byte;
  }
  output.close();
  return true;
}