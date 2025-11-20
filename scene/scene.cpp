#include "scene.hpp"

#include <fstream>

#include "light.hpp"
#include "math/color.hpp"
#include "math/vector.hpp"

// Set camera parameters
void Scene::setCamera(const Vector pos, const Vector dir,
                      const double fov_deg) {
  camera.position = pos;
  camera.direction = dir;
  camera.fov = fov_deg;
}

// Set background color
void Scene::setBackground(const int r, const int g, const int b) {
  background = Color(r, g, b);
}

// Add a light to the scene
void Scene::addLight(const Vector pos, const Color color) {
  lights.push_back(Light{pos, color});
}

// Add a shape to the scene
void Scene::addShape(std::unique_ptr<Shape> shape) {
  shapes.push_back(std::move(shape));
}
