#include "renderer.hpp"

const std::string OUTPUT_FILE = "output.ppm";

int main() {
  Scene scene(512, 512, 6);
  scene.setBackground(135, 206, 235);  // Sky blue
  // Need to specify properties in the future

  scene.render(OUTPUT_FILE);
  return 0;
}