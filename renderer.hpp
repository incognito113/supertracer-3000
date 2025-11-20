#pragma once
#include <vector>

#include "shape.hpp"
#include "tracer.hpp"

class Scene {
 private:
  const int width;
  const int height;
  const int maxReflections;
  Camera camera;
  Color background;
  std::vector<Light> lights;
  std::vector<std::unique_ptr<Shape>> shapes;

  Color traceRay(const Ray& ray) const;
  Color computeLighting(const HitInfo& hitInfo) const;

 public:
  Scene(const int w, const int h, const int maxRefl)
      : width(w),
        height(h),
        maxReflections(maxRefl),
        camera(),
        background(),
        lights(),
        shapes() {}

  void setCamera(const Vector pos, const Vector dir, const double fov_deg);
  void setBackground(const int r, const int g, const int b);
  void addLight(const Vector pos, const Color color);
  void addShape(std::unique_ptr<Shape> shape);

  std::vector<Color> render_pixels() const;

  void render(const std::string& filename) const;

  ~Scene() = default;
};