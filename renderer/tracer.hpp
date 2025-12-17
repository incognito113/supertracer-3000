#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

#include "math/color.hpp"
#include "math/vector.hpp"
#include "pool.hpp"
#include "scene/bvh.hpp"
#include "shapes/shape.hpp"

// Forward declaration
class Ray;
class Scene;

struct Pixels {
  std::vector<int> pxSamples;   // Number of samples per pixel
  std::vector<Color> pxColors;  // Accumalated color per pixel (not averaged)
  std::vector<std::atomic_bool> rowReady;  // Marks if row is ready for display

  Pixels(int w, int h) : pxSamples(w * h), pxColors(w * h), rowReady(h) {
    for (int y = 0; y < h; ++y) {
      rowReady[y].store(false, std::memory_order_release);
    }
  }
  Pixels(const Pixels& px) = default;
  Pixels& operator=(const Pixels& other) = default;

  ~Pixels() = default;
};

// Responsible for tracing rays through the scene and computing pixel colors
class Tracer {
 private:
  static constexpr int ANTI_ALIAS_GRID_SIZE = 2;
  const Color traceRay(const Scene& scene, const Ray& ray) const;
  const Color computeLighting(const Scene& scene, const HitInfo& hitInfo) const;
  const Scene& scene;
  BVH bvh;
  ThreadPool pool{std::thread::hardware_concurrency()};

 public:
  Tracer(Scene& sc) : scene(sc), bvh(sc) {
    std::function<void(const std::vector<BVHNode>&, int, int)> printNode =
        [&](const std::vector<BVHNode>& nodes, int index, int depth) {
          if (index < 0) return;
          const auto& n = nodes[index];
          for (int i = 0; i < depth; ++i) std::cout << "  ";
          std::cout << "Node " << index << ": " << "bounds=[" << n.bounds.min
                    << " - " << n.bounds.max << ", shapes=" << n.shapeCount
                    << "\n";
          printNode(nodes, n.left, depth + 1);
          printNode(nodes, n.right, depth + 1);
        };
    // Uncomment to print BVH structure
    // printNode(bvh.getNodes(), 0, 0);
  }

  void refinePixels(Pixels& pixels);
  void wait();

  ~Tracer() = default;

  friend class Renderer;
};