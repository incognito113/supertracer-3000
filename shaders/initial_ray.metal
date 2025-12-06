#include <metal_stdlib>
using namespace metal;

#include "structs.metal"

#define M_PI 3.14159265358979323846

// Simple per-thread PRNG using XORShift, seeded from pixel index and iteration
inline uint xorShift32(uint x) {
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return x;
}

// Returns a random float in [0, 1)
inline float random(uint gid, uint iteration) {
  uint state = gid + iteration * 73856093;
  state = xorShift32(state);
  return float(state) / 4294967296.0;
}

// Kernel to generate initial rays from the camera
inline GPU_Ray generateInitialRay(const device GPU_SceneData* sceneData,
                                  const uint gid_x, const uint gid_y) {
  const float width = sceneData->width;
  const float height = sceneData->height;
  const uint idx = gid_y * sceneData->width + gid_x;
  const uint iteration = sceneData->iteration;
  const float3 position = sceneData->position;
  const float3 direction = sceneData->direction;

  float halfHeight = tan(sceneData->fov * 0.5 * M_PI / 180.0);
  float halfWidth = (width / height) * halfHeight;

  float3 right = normalize(cross(sceneData->direction, float3(0, 0, 1)));
  float3 up = normalize(cross(right, sceneData->direction));

  float xOffset = 0.5;
  float yOffset = 0.5;
  const int a = 2;
  const float a_f = float(a);

  if (sceneData->iteration >= 1) {
    const float xQuad = ((iteration % a) + 0.5) / a_f;
    const float yQuad = ((iteration / a) % a + 0.5) / a_f;
    // Jitter within the subpixel for anti-aliasing
    xOffset = xQuad + random(idx, iteration) / a_f;
    yOffset = yQuad + random(idx, iteration + 1) / a_f;
  }

  const float u = (float(gid_x) + xOffset) / width;
  const float v = (float(gid_y) + yOffset) / height;

  const float x = (2.0 * u - 1.0) * halfWidth;
  const float y = (1.0 - 2.0 * v) * halfHeight;

  float3 worldPoint = position + direction + x * right + y * up;
  float3 rayDir = normalize(worldPoint - position);
  return GPU_Ray{position, rayDir};
}