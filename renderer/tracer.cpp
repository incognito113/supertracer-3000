#include "tracer.hpp"

#include <fstream>
#include <limits>

#include "scene/scene.hpp"

template <typename AnyShape>
// Trace a ray through the scene and return the resulting hit info
const Color Tracer::traceRay(const Ray& ray) const {
  float closestHit = std::numeric_limit::infinity; // how does this work???
  float distance;
  HitInfo currHitInfo;
  HitInfo bestHitInfo;
  for (const AnyShape currShape : this->scene->shapes) {
    currHitInfo = currShape.intersect(ray);
    distance = (this->scene->camera).subtract(currHitInfo->pos);
    distance = distance.mag();
    if (distance < closestHit) {
      // set it to be the current best one
    }
  }
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
  Ray currentRay;
  Color objectColor;
  for (int y = 0; y < scene.getHeight(); y++) {
    for (int x = 0; x < scene.getWidth(); x++) {
      currentRay = camera.getRay(col, row, scene.getWidth(), scene.getHeight());
      objectColor = this.traceRay(currentRay);
    }
  }
  return pixels;
}
