#include "tracer.hpp"

#include <vector>

#include "scene/scene.hpp"

// Trace a ray through the scene and return the resulting color
const Color Tracer::traceRay(const Scene& scene, const Ray& ray,
                             int depth) const {
  // Check for intersections with all shapes in the scene
  std::optional<HitInfo> closestHit;
  double closest_t = std::numeric_limits<double>::max();

  for (const std::unique_ptr<Shape>& shape : scene.shapes) {
    std::optional<HitInfo> hitOpt = shape->intersects(ray);
    if (hitOpt.has_value() && hitOpt->t < closest_t) {
      closest_t = hitOpt->t;
      closestHit.emplace(hitOpt.value());
    }
  }

  if (closestHit.has_value()) {
    return computeLighting(scene, closestHit.value(), depth);
  } else {
    return scene.getBackground();
  }
}

// Compute lighting for all lights at the hit point
const Color Tracer::computeLighting(const Scene& scene, const HitInfo& hitInfo,
                                    int depth) const {
  // Convenience variables
  const Vector i = hitInfo.pos;
  const Vector d = hitInfo.ray.dir;
  const Vector n = hitInfo.normal;
  const Material* mat = hitInfo.material;

  // Ambient light contribution (doesn't depend on lights)
  const double ambFactor = scene.getAmbientLight() * (1 - mat->reflectivity);
  const Color ambient = mat->color * ambFactor;

  Color finalColor = ambient;

  for (const Light& light : scene.lights) {
    bool inShadow = false;

    // Shadow check (cast shadow ray toward light)
    for (const std::unique_ptr<Shape>& shape : scene.shapes) {
      const Vector toLight = light.position - i;
      // Offset origin slightly to avoid self-intersection
      const Ray shadowRay(i, toLight);
      std::optional<HitInfo> shadowHitOpt = shape->intersects(shadowRay);

      if (shadowHitOpt.has_value()) {
        const double distToLightSq = toLight.magSq();
        const double tSq = shadowHitOpt->t * shadowHitOpt->t;
        if (tSq < distToLightSq && shadowHitOpt->t > Shape::EPS) {
          inShadow = true;
          break;
        }
      }
    }

    const Vector lt = (light.position - i).norm();

    // Diffuse light contribution
    Color diffuse;
    if (!inShadow) {
      const double diffFactor =
          (1 - ambFactor) * (1 - mat->reflectivity) * std::max(0.0, n * lt);
      diffuse = mat->color * light.color * diffFactor;
    }

    // Specular light contribution
    Color specular;
    if (!inShadow) {
      const Vector h = (lt - d.norm()).norm();
      specular = mat->specular * mat->specularFactor *
                 std::pow(std::max(0.0, n * h), mat->shininess) * light.color;
    }

    // Reflective contribution
    Color reflective;
    if (depth > 0 && mat->reflectivity > 0) {
      const Vector reflectDir = d - 2.0 * d.proj(n);
      const Ray ray(i, reflectDir);
      const Color reflectColor = traceRay(scene, ray, depth - 1);
      reflective = (1 - ambFactor) * mat->reflectivity * reflectColor;
    }

    // Sum contributions
    finalColor = finalColor + diffuse + specular + reflective;
  }

  return finalColor;
}

// Expects preallocated pixels vector
// Updates pixels in place by adding new rays
// If simple, performs uniform sampling with 1 ray per pixel
std::vector<Color> Tracer::render_pixels(const Scene& scene) {
  std::vector<Color> pixels(scene.getWidth() * scene.getHeight());

  const int w = scene.getWidth();
  const int h = scene.getHeight();
  const int refl = scene.reflections();
  const Camera camera = scene.getCamera();

  for (int j = 0; j < h; ++j) {
    for (int i = 0; i < w; ++i) {
      const Ray ray = camera.ray(i + 0.5, j + 0.5, w, h);
      const Color color = traceRay(scene, ray, refl);
      pixels[j * w + i] = color;
    }
  }

  return pixels;
}
