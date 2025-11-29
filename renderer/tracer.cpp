#include "tracer.hpp"

#include <fstream>
#include <cmath> // i think has INFINITY macro?

#include "scene/scene.hpp"

// Trace a ray through the scene and return the resulting hit info
const Color Tracer::traceRay(const Ray& ray) const {

  // gets the object we're intersecting with first
  float closestHit = INFINITY; // does this work??? i hope so
  Vector distanceVector;
  float euclideanDistance;
  HitInfo currHitInfo;
  HitInfo bestHitInfo;
  for (const AnyShape currShape : this->scene->shapes) {
    currHitInfo = currShape.intersects(ray);
    if (currHitInfo != std::nullopt) {
      distanceVector = (this->scene->camera).subtract(currHitInfo->pos);
      euclideanDistance = distanceVector.mag();
      if (euclideanDistance < closestHit) {
        bestHitInfo = currHitInfo;
      }
    }
  }

  // if the ray didn't hit anything, set it to be the background color
  if (closestHit == INFINITY) {
    return this->scene->background;
  }

  //else, we have to calculate the lighting for the object we hit
  return this.computeLighting(bestHitInfo);
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
