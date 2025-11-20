#include "io/image.hpp"
#include "renderer/tracer.hpp"

const std::string OUTPUT_FILE = "output.ppm";

int main() {
  Scene scene(512, 512, 6);
  scene.setBackground(135, 206, 235);  // Sky blue
  // Need to specify properties in the future

  Tracer tracer{scene};
  Image image{tracer};

  image.render(OUTPUT_FILE);

  return 0;
}