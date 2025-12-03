#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "shapes/shape.hpp"

struct BVHNode {
  Bounds bounds;
  int left;        // Index of left child in BVH array (-1 if leaf)
  int right;       // Index of right child in BVH array (-1 if leaf)
  int shapeIndex;  // Index into scene's shape array (-1 if not leaf)
  int shapeCount;  // Number of objects in this node (0 if not leaf)

  BVHNode(const Bounds& b)
      : bounds(b), left(-1), right(-1), shapeIndex(-1), shapeCount(0) {}
  BVHNode() : bounds(), left(-1), right(-1), shapeIndex(-1), shapeCount(0) {}
};

class BVH {
 private:
  std::vector<BVHNode> nodes;
  std::vector<int> shapeIndices;
  static constexpr int LEAF_THRESHOLD = 4;
  static constexpr int BIN_COUNT = 32;
  static constexpr double TRAVERSAL_COST = 1.0;
  static constexpr double INTERSECTION_COST = 1.0;

  int buildRecursive(const std::vector<std::unique_ptr<BoundedShape>>& shapes,
                     int start, int end);
  std::pair<int, double> getBestSAHSplit(
      const std::vector<std::unique_ptr<BoundedShape>>& shapes, int start,
      int end, int axis);

  struct Bin {
    Bounds bounds;
    int count = 0;

    void clear();
    void add(const Bounds& b);

    Bin() : bounds(), count(0) {}

    ~Bin() = default;
  };

 public:
  BVH(const std::vector<std::unique_ptr<BoundedShape>>& shapes)
      : nodes(), shapeIndices() {
    build(shapes);
  }

  const std::vector<BVHNode>& getNodes() const { return nodes; }
  const std::vector<int>& getShapeIndices() const { return shapeIndices; }

  void build(const std::vector<std::unique_ptr<BoundedShape>>& shapes);
  void traverse(const std::vector<std::unique_ptr<BoundedShape>>& shapes,
                const Ray& ray,
                const std::function<void(const HitInfo&)>& callback) const;
  void traverseFirstHit(
      const std::vector<std::unique_ptr<BoundedShape>>& shapes, const Ray& ray,
      const std::function<void(const HitInfo&)>& callback) const;

  ~BVH() = default;
};