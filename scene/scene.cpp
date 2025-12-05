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

// Add plane (point, normal) to scene
void Scene::addPlane(const Vector& point, const Vector& normal,
                     const Material& mat) {
  if (normal.magSq() < Vector::EPS * Vector::EPS) {
    throw std::invalid_argument("Plane normal cannot be zero vector");
  }
  addPlaneShape(mat, point, normal.norm());
}

// Add sphere (center, radius) to scene
void Scene::addSphere(const Vector& center, double radius,
                      const Material& mat) {
  if (radius < Vector::EPS) {
    throw std::invalid_argument("Sphere radius must be positive");
  }
  addBoundedShape<Sphere>(mat, center, radius);
}

// Add triangle (v0, v1, v2) to scene
void Scene::addTriangle(const Vector& a, const Vector& b, const Vector& c,
                        const Material& mat) {
  addBoundedShape<Triangle>(mat, a, b, c);
}

// Add triangle with vertices and normals to scene
void Scene::addTriangle(const Vector& a, const Vector& b, const Vector& c,
                        const Vector& nA, const Vector& nB, const Vector& nC,
                        const Material& mat) {
  if (nA.magSq() < Vector::EPS * Vector::EPS ||
      nB.magSq() < Vector::EPS * Vector::EPS ||
      nC.magSq() < Vector::EPS * Vector::EPS) {
    throw std::invalid_argument(
        "Triangle vertex normals cannot be zero vectors");
  }
  addBoundedShape<Triangle>(mat, a, b, c, nA.norm(), nB.norm(), nC.norm());
}

int Scene::shapeCount() const {
  return static_cast<int>(bndedShapes.size() + planes.size());
}
