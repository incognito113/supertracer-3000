#pragma once

#include <memory>
#include <string>
#include <vector>

#include "math/camera.hpp"
#include "math/color.hpp"
#include "math/ray.hpp"
#include "math/vector.hpp"
#include "scene/light.hpp"
#include "shapes/plane.hpp"
#include "shapes/shape.hpp"

// Forward declaration
class Tracer;
class Renderer;
class Converter;

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
  std::vector<std::unique_ptr<BoundedShape>> bndedShapes;
  std::vector<std::unique_ptr<Plane>> planes;
  std::vector<Material> materials;

  template <typename ShapeT, typename... Args>
  void addBoundedShape(const Material& m, Args&&... args) {
    materials.push_back(m);
    size_t matIndex = materials.size() - 1;
    bndedShapes.push_back(
        std::make_unique<ShapeT>(std::forward<Args>(args)..., matIndex));
  }

  void addPlaneShape(const Material& m, const Vector& point,
                     const Vector& normal) {
    materials.push_back(m);
    size_t matIndex = materials.size() - 1;
    planes.push_back(std::make_unique<Plane>(point, normal, matIndex));
  }

 public:
  Scene(const int w, const int h, const int maxRefl)
      : width(w),
        height(h),
        maxReflections(maxRefl),
        ambientLight(),
        camera(),
        background(),
        lights() {}
  Scene(const Scene& other)
      : width(other.width),
        height(other.height),
        maxReflections(other.maxReflections),
        ambientLight(other.ambientLight),
        camera(other.camera),
        background(other.background),
        lights(other.lights),
        bndedShapes(),
        planes(),
        materials(other.materials) {
    // Deep copy of bounded shapes
    for (const std::unique_ptr<BoundedShape>& bshape : other.bndedShapes) {
      bndedShapes.push_back(std::unique_ptr<BoundedShape>(
          static_cast<BoundedShape*>(bshape->clone())));
    }
    // Deep copy of planes
    for (const std::unique_ptr<Plane>& shape : other.planes) {
      planes.push_back(std::unique_ptr<Plane>(shape->clone()));
    }
  }

  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int reflections() const { return maxReflections; }
  double getAmbientLight() const { return ambientLight; }
  const Color getBackground() const { return background; }
  const Camera getCamera() const { return camera; }

  size_t lightCount() const { return lights.size(); }
  size_t planeCount() const { return planes.size(); }
  size_t boundedShapeCount() const { return bndedShapes.size(); }
  size_t shapeCount() const { return planeCount() + boundedShapeCount(); }

  void setAmbientLight(const double ambient);
  void setCamera(const Vector pos, const Vector dir, const double fovDeg);
  void setCameraPos(const Vector pos);
  void setCameraDir(const Vector dir);
  void setCameraFov(const double fov_deg);
  void eulerRotateCamera(int dx, int dy);
  void moveCameraPosition(const Vector& delta);
  void zoomCamera(double scroll);
  void setBackground(const int r, const int g, const int b);
  void addLight(const Vector pos, const Color color);

  void addPlane(const Vector& point, const Vector& normal, const Material& mat);
  void addSphere(const Vector& center, double radius, const Material& mat);
  void addTriangle(const Vector& a, const Vector& b, const Vector& c,
                   const Material& mat);
  void addTriangle(const Vector& a, const Vector& b, const Vector& c,
                   const Vector& nA, const Vector& nB, const Vector& nC,
                   const Material& mat);
  bool importOBJ(const Vector& offset, const std::string fileName,
                 const double scale, const Material& material);

  friend class Tracer;
  friend class Renderer;
  friend class Converter;

  ~Scene() = default;
};
