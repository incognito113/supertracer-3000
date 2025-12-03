#include "scene.hpp"

#include "light.hpp"
#include "math/color.hpp"
#include "math/vector.hpp"
#include "shapes/plane.hpp"
#include "shapes/sphere.hpp"
#include "shapes/triangle.hpp"

// Set camera parameters
void Scene::setCamera(const Vector pos, const Vector dir, const double fovDeg) {
  camera.position = pos;
  camera.setDir(dir);
  camera.fov = fovDeg;
}

void Scene::setCameraPos(const Vector pos) { camera.position = pos; }

void Scene::setCameraDir(const Vector dir) { camera.setDir(dir); }

void Scene::setCameraFov(const double fovDeg) { camera.fov = fovDeg; }

void Scene::eulerRotateCamera(int dx, int dy) { camera.eulerRotate(dx, dy); }

void Scene::moveCameraPosition(const Vector& delta) {
  camera.movePosition(delta);
}

void Scene::zoomCamera(double scroll) { camera.zoom(scroll); }

void Scene::setAmbientLight(const double ambient) { ambientLight = ambient; }

// Set background color
void Scene::setBackground(const int r, const int g, const int b) {
  background = Color(r, g, b);
}

// Add a light to the scene
void Scene::addLight(const Vector pos, const Color color) {
  lights.push_back(Light{pos, color});
}

void Scene::addPlane(const Vector& point, const Vector& normal,
                     const Material& mat) {
  if (normal.magSq() < Vector::EPS * Vector::EPS) {
    throw std::invalid_argument("Plane normal cannot be zero vector");
  }
  planes.push_back(std::make_unique<Plane>(point, normal.norm(), mat));
}

void Scene::addSphere(const Vector& center, double radius,
                      const Material& mat) {
  if (radius < Vector::EPS) {
    throw std::invalid_argument("Sphere radius must be positive");
  }
  bndedShapes.push_back(std::make_unique<Sphere>(center, radius, mat));
}

void Scene::addTriangle(const Vector& a, const Vector& b, const Vector& c,
                        const Material& mat) {
  bndedShapes.push_back(std::make_unique<Triangle>(a, b, c, mat));
}
