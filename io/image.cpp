#include "image.hpp"

#include <fstream>

#include "io/tinyfiledialogs.hpp"
#include "renderer/tracer.hpp"

Image::Image(const Scene& sc) : scene(sc), pixels(*(new std::vector<Color>())) {
  Tracer tracer(scene);
  pixels = tracer.render_pixels(scene);
}

// Save the image to a PPM file (return true on success)
// If filename is empty, prompts user for filename
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
  for (Color px : pixels) {
    output.write(reinterpret_cast<const char*>(px.getBytes().data()), 3);
  }
  output.close();
  return true;
}