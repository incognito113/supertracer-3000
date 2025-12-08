#include <fstream>

#include "io/image.hpp"
#include "renderer/renderer.hpp"
#include "scene/scene.hpp"

int main() {

  // set true to demo the obj importer and mesh rendering. false demos the spheres.
  const bool objDemo = false;

  Scene scene{1400, 800, 6};

  scene.setBackground(135, 206, 235);
  scene.setCamera(Vector(0.5, -1.0, 0.5), Vector(0, 1, 0), 60.0);
  scene.setAmbientLight(0.2);
  scene.addLight(Vector(0, -0.5, 1.0), Color(255, 255, 255));


  scene.addPlane(Vector(0, 0, 0), Vector(0, 0, 1),
                 Material{
                     .color = Color(255, 255, 255),
                 .reflectivity = 0,
                 });


  if (objDemo) {
      scene.importOBJ(Vector(0.5, 1, 0),
                      "teapot.obj",
                      0.1,
                      Material{
                          .color = (Color(200, 200, 200)),
                      .reflectivity = 0,
                      });

  }

  else {
      scene.addSphere(Vector(0.25, 0.45, 0.4), 0.4,
                      Material{
                          .color = Color(255, 0, 0),
                      .reflectivity = 0.3,
                      });
      scene.addSphere(Vector(1.0, 1.0, 0.25), 0.25,
                      Material{
                          .color = Color(0, 255, 0),
                      .reflectivity = 0.0,
                      });
      scene.addSphere(Vector(0.8, 0.3, 0.15), 0.15,
                      Material{
                          .color = Color(0, 0, 255),
                      .reflectivity = 0.7,
                      });
  }



  std::cout << "Scene has " << scene.shapeCount() << " shapes." << std::endl;

  // interactive window where you can move around the camera @ 60 fps
  Renderer renderer{scene, 60};
  renderer.run();

  // static image output
  // Image image{renderer};
  // image.save("output.ppm");

  return 0;
}
