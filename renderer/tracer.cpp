#include "tracer.hpp"

#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "renderer/pool.hpp"
#include "scene/scene.hpp"

// Trace a ray through the scene and return the resulting color
const Color Tracer::traceRay(const Scene& scene, const Ray& ray,
                             int depth) const {
  // Iterative implementation: follow reflection bounces using a loop
  Color finalColor{0, 0, 0};
  double throughput = 1.0;

  Ray currentRay = ray;

  for (int bounce = 0; bounce < depth; ++bounce) {
    // Check for intersections with all shapes in the scene
    std::optional<HitInfo> closestHit;
    double closestT = std::numeric_limits<double>::max();

    // Check non-bounded shapes normally
    for (const std::unique_ptr<Plane>& shape : scene.planes) {
      std::optional<HitInfo> hitOpt = shape->intersects(currentRay);
      if (hitOpt.has_value() && hitOpt->t < closestT) {
        closestT = hitOpt->t;
        closestHit.emplace(hitOpt.value());
      }
    }

    // Check bounded shapes using BVH
    bvh.traverse(scene.bndedShapes, currentRay, [&](const HitInfo& hitInfo) {
      if (hitInfo.t < closestT) {
        closestT = hitInfo.t;
        closestHit.emplace(hitInfo);
      }
    });

    if (!closestHit.has_value()) {
      // No hit: add background scaled by current throughput and finish
      finalColor += throughput * scene.getBackground();
      break;
    }

    const HitInfo hit = closestHit.value();

    // Compute local color at hit point
    const Color localColor = computeLighting(scene, hit);
    finalColor += throughput * localColor;

    // Prepare for next reflection bounce
    const Material* mat = hit.material;

    // If no more depth or no reflectivity, stop iterating
    if (mat->reflectivity <= 0) {
      break;
    }

    // Compute reflection direction and offset to avoid self intersection
    const Vector i = hit.pos + hit.normal * Vector::EPS;
    const Vector d = currentRay.dir;
    const Vector reflectDir = d - 2.0 * d.proj(hit.normal);

    // Update throughput and currentRay for next iteration
    throughput *= mat->reflectivity;
    currentRay = Ray(i, reflectDir);

    // If throughput is very small, break early
    if (throughput <= 0.001) {
      break;
    }
  }

  return finalColor;
}

// Compute lighting for all lights at the hit point
// NOTE: does NOT perform recursive reflections
// Reflection is handled iteratively inside traceRay
const Color Tracer::computeLighting(const Scene& scene,
                                    const HitInfo& hitInfo) const {
  // Convenience variables
  // Offset origin slightly to avoid self-intersection
  const Vector i = hitInfo.pos + hitInfo.normal * Vector::EPS;
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
    for (const std::unique_ptr<Plane>& shape : scene.planes) {
      const Vector toLight = light.position - i;
      const Ray shadowRay(i, toLight);
      std::optional<HitInfo> shadowHitOpt = shape->intersects(shadowRay);

      if (shadowHitOpt.has_value()) {
        const double distToLightSq = toLight.magSq();
        const double tSq = shadowHitOpt->t * shadowHitOpt->t;
        if (tSq < distToLightSq && shadowHitOpt->t > Vector::EPS) {
          inShadow = true;
          break;
        }
      }
    }
    // Shadow check for bounded shapes using BVH
    if (!inShadow) {
      const Vector toLight = light.position - i;
      const Ray shadowRay(i, toLight);
      bvh.traverseFirstHit(
          scene.bndedShapes, shadowRay, [&](const HitInfo& shadowHit) {
            const double distToLightSq = toLight.magSq();
            const double tSq = shadowHit.t * shadowHit.t;
            if (tSq < distToLightSq && shadowHit.t > Vector::EPS) {
              inShadow = true;
            }
          });
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

    // Sum contributions
    finalColor = finalColor + diffuse + specular;
  }

  return finalColor;
}

// Expects preallocated pixels vector
// Updates pixels in place by adding new rays
// If simple, performs uniform sampling with 1 ray per pixel
void Tracer::refinePixels(Pixels& pixels) {
  const int w = scene.getWidth();
  const int h = scene.getHeight();
  const int refl = scene.reflections();
  const Camera& camera = scene.getCamera();

  for (int row = 0; row < h; ++row) {
    pool.enqueue([this, &pixels, camera, row, w, h, refl]() {
      thread_local std::mt19937 rng(std::random_device{}());
      thread_local std::uniform_real_distribution<double> dist(-0.5, 0.5);

      for (int x = 0; x < w; ++x) {
        const int i = row * w + x;
        int oldSamples = pixels.pxSamples[i];

        double xQuad = 0.5, yQuad = 0.5, xOffset = 0.0, yOffset = 0.0;
        if (oldSamples > 0) {
          const int a = ANTI_ALIAS_GRID_SIZE;
          xQuad = ((oldSamples % a + 0.5) / a);
          yQuad = (((oldSamples / a) % a + 0.5) / a);
          xOffset = xQuad + dist(rng) / a;
          yOffset = yQuad + dist(rng) / a;
        }

        Ray ray = camera.ray(x + xOffset, row + yOffset, w, h);
        Color c = traceRay(scene, ray, refl);

        pixels.pxColors[i] += c;
        pixels.pxSamples[i]++;
      }
      // Mark row as ready
      pixels.rowReady[row] = true;
    });
  }
}

void Tracer::wait() { pool.wait(); }
