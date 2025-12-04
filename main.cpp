#include <fstream>

#include "io/image.hpp"
#include "renderer/renderer.hpp"
#include "scene/scene.hpp"

int main() {
  Scene scene{512, 512, 6};

  scene.setBackground(135, 206, 235);
  scene.setCamera(Vector(0, 0.5, 1.5), Vector(0, 0, -1), 60.0);
  scene.setAmbientLight(0.2);
  scene.addLight(Vector(0, 0, 10.0), Color(255, 255, 255));

  scene.importOBJ(Vector(0.0, 0.0, 0.0), "bunny.obj", 5,
                  Material{
                      .color = Color(255, 0, 0),
                      .reflectivity = 0.0,
                  });

  std::cout << "Scene has " << scene.shapeCount() << " shapes." << std::endl;

  Renderer renderer{scene, 60};
  renderer.run();

  // Image image{renderer};
  // image.save("output.ppm");

  return 0;
}