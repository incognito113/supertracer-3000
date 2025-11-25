#pragma once

#include <vector>

#include "math/camera.hpp"
#include "math/color.hpp"
#include "math/ray.hpp"
#include "math/vector.hpp"
#include "scene/light.hpp"
#include "shapes/shape.hpp"

class Tracer;

// Represents the entire 3D scene to be rendered
class Scene {
 private:
  const int width;
  const int height;
  const int maxReflections;
  int ambientLight;
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

  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int getReflections() const { return maxReflections; }
  int getAmbientLight() const { return ambientLight; }
  const Color getBackground() const { return background; }
  const Camera getCamera() const { return camera; }
  void setAmbientLight(const int ambient) { ambientLight = ambient; }
  void setCamera(const Vector pos, const Vector dir, const double fov_deg);
  void setBackground(const int r, const int g, const int b);
  void addLight(const Vector pos, const Color color);
  void addShape(std::unique_ptr<Shape> shape);
  // remove light/shape?

  ~Scene() = default;

  friend class Tracer;
};
