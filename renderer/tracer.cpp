#include "tracer.hpp"

#include <fstream>

#include "scene/scene.hpp"

// Trace a ray through the scene and return the resulting color
const Color Tracer::traceRay(const Ray& ray) const {
  // Placeholder implementation: return background color
  (void)ray;
  return scene.getBackground();
}

// Compute lighting for all lights at the hit point
const Color Tracer::computeLighting(const HitInfo& hitInfo) const {
  // Placeholder
  (void)hitInfo;
  return Color(1.0, 1.0, 1.0);
}

const std::vector<Color> Tracer::render_pixels() const {
  // Placeholder
  std::vector<Color> pixels;
  pixels.reserve(scene.getWidth() * scene.getHeight());
  for (int j = 0; j < scene.getHeight(); ++j) {
    for (int i = 0; i < scene.getWidth(); ++i) {
      pixels.push_back(scene.getBackground());
    }
  }
  return pixels;
}