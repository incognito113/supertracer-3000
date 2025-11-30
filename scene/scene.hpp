#pragma once

#include <vector>

#include "math/camera.hpp"
#include "math/color.hpp"
#include "math/ray.hpp"
#include "math/vector.hpp"
#include "scene/light.hpp"
#include "shapes/shape.hpp"

// Forward declaration
class Tracer;
class Renderer;

// Represents the entire 3D scene to be rendered
class Scene {
 private:
  const int width;
  const int height;
  const int maxReflections;
  double ambientLight;
  Camera camera;
  Color background;
  std::vector<Light> lights;
  std::vector<std::unique_ptr<Shape>> shapes;

 public:
  Scene(const int w, const int h, const int maxRefl)
      : width(w),
        height(h),
        maxReflections(maxRefl),
        ambientLight(),
        camera(),
        background(),
        lights(),
        shapes() {}
  Scene(const Scene& other)
      : width(other.width),
        height(other.height),
        maxReflections(other.maxReflections),
        ambientLight(other.ambientLight),
        camera(other.camera),
        background(other.background),
        lights(other.lights),
        shapes() {
    // Deep copy of shapes
    for (const std::unique_ptr<Shape>& shape : other.shapes) {
      shapes.push_back(std::unique_ptr<Shape>(shape->clone()));
    }
  }

  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int reflections() const { return maxReflections; }
  double getAmbientLight() const { return ambientLight; }
  const Color getBackground() const { return background; }
  const Camera getCamera() const { return camera; }
  void setAmbientLight(const double ambient);
  void setCamera(const Vector pos, const Vector dir, const double fov_deg);
  void setCameraPos(const Vector pos);
  void setCameraDir(const Vector dir);
  void setBackground(const int r, const int g, const int b);
  void addLight(const Vector pos, const Color color);
  void addShape(std::unique_ptr<Shape> shape);

  friend class Tracer;
  friend class Renderer;

  ~Scene() = default;
};