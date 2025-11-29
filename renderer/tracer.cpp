#include "tracer.hpp"
#include "scene/scene.hpp"

#include <fstream>
#include <cmath>
#include <limits>

#define INFINITY std::numeric_limits<double>::infinity()

template <typename AnyShape>
// Trace a ray through the scene and return the resulting hit info
const Color Tracer::traceRay(const Ray& ray, const int depth) const {
  // gets the object we're intersecting with first
  double closestHit = INFINITY; // does this work??? i hope so
  Vector distanceVector;
  double euclideanDistance;
  HitInfo currHitInfo;
  HitInfo bestHitInfo;

  for (const AnyShape& currShape : this->scene->shapes) {
    currHitInfo = currShape.intersects(ray);
    if (currHitInfo != std::nullopt) {
      distanceVector = (this->scene->camera).subtract(currHitInfo->pos);
      euclideanDistance = distanceVector.mag();
      if (euclideanDistance < closestHit) {
        bestHitInfo = currHitInfo;
        closestHit = euclideanDistance;
      }
    }
  }

  // if the ray didn't hit anything, set it to be the background color
  // ooh i have an idea what if we make it so that we can set background images wouldn't that be cool? like set every background pixel to 0 0 0 and then go through the ppm and matte over it with an image
  // ohhhh and then we could include a couple of background images pre-loaded and let them pick one of them or put in their own image or pass it a flat color... i want to do this
  if (closestHit == INFINITY) {
    return this->scene->background;
  }

  //else, we have to calculate the lighting for the object we hit

  // get ambient light per handout math
  double ambientFactor = this->scene->ambientLight * (1 - hitInfo->material->reflectivity);
  Color ambientLight = hitInfo->material->diffuse * ambientFactor;


  // do diffuse and specular together bc they both need line of sight to the light source
  Color diffuseLight(0, 0, 0);
  Color specularLight(0, 0, 0);
  Vector honestlyIDK;

  Vector vectToLight;
  double distToLight;
  Ray lightPath
  bool blocked;
  Vector distanceVector;
  double euclideanDistance;
  HitInfo currHitInfo;
  for (const Light& currLight : this->scene->lights) {
    // get the vector between the light and the hit point
    vectToLight = (hitInfo->pos).subtract(currLight->position); // i hope this subtract is in the right direction
    distToLight = lightHitpointPath.mag();

    // make a ray between the light and hit point
    lightPath = Ray(hitInfo->pos, vectToLight);

    // crunch every object to see if it's blocking the path between the light and the hit point
    blocked = false;
    for (const AnyShape& currShape : this->scene->shapes) {
      currHitInfo = currShape.intersects(lightPath);
      if (currHitInfo != std::nullopt) {
        distanceVector = (this->scene->camera).subtract(currHitInfo->pos);
        euclideanDistance = distanceVector.mag();
        // if the object is in between the light and hit point, we know it's blocked and give up
        // if it intersects the ray but is behind the light, we don't care
        if (euclideanDistance < distToLight) {
          blocked = true;
          break;
        }
      }
    }

    if (blocked) {
      continue;
    }

    normalizedToLight = vectToLight.norm();
    originalRayVect = (this->camera->position).subtract(hitInfo->pos) // again make sure i have the subtraction the right way around

    // more fucky math copied from the handout
    diffuseLight += (1 - ambientFactor) * (1 - hitInfo->material->reflectivity) * max(0, (hitInfo->normal).norm().dot(normalizedToLight)) * hitInfo->material->diffuse;

    honestlyIDK = normalizedToLight + -(originalRayVect.norm());
    honestlyIDK = honestlyIDK.norm();
    specularLight += hitInfo->material->specular * pow(max(0, honestlyIDK.dot((hitInfo->normal).norm())), hitInfo->material->shininess) * Color(255, 255, 255)
    // ewwwww
  }

  // get the stuff for the reflected ray
  // i don't know if i understood this so maybe i did it wrong... idk
  Color reflectedLight;

  if (depth == 0) {
    reflectedLight = 0;
  }
  else {
    Vector reflectedVector = originalRayVect - (2 * originalRayVect.proj(hitInfo->normal));
    Ray reflectedRay(hitInfo->pos, reflectedVector);
    Color reflectedColor;
    reflectedColor = this->traceRay(reflectedRay, depth - 1)

    reflectedLight = (1 - ambientFactor) * hitInfo->material->reflectivity * reflectedColor;
  }

  return (ambientLight + diffuseLight + specularLight + reflectedLight);
}

const std::vector<Color> Tracer::render_pixels() const {
  std::vector<Color> pixels;
  pixels.reserve(scene.getWidth() * scene.getHeight());
  Ray currentRay;
  Color objectColor;
  for (int y = 0; y < scene.getHeight(); y++) {
    for (int x = 0; x < scene.getWidth(); x++) {
      currentRay = camera.getRay(col, row, scene.getWidth(), scene.getHeight());
      objectColor = this.traceRay(currentRay);
      pixels.push_back(objectColor);
    }
  }
  return pixels;
}
