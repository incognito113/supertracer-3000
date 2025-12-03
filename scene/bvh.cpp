#include "bvh.hpp"

#include <array>
#include <memory>
#include <numeric>

// Clear bin data
void BVH::Bin::clear() {
  bounds = Bounds();
  count = 0;
}

// Add bounds to bin
void BVH::Bin::add(const Bounds& b) {
  if (count == 0) {
    bounds = b;
  } else {
    bounds.expand(b);
  }
  count++;
}

// Recursively build BVH and return index of this node
int BVH::buildRecursive(
    const std::vector<std::unique_ptr<BoundedShape>>& shapes, int start,
    int end) {
  // Compute bounds for this node
  const int n = end - start;
  Bounds nodeBounds = shapes[shapeIndices[start]]->bounds;
  Bounds centroidBounds = Bounds(shapes[shapeIndices[start]]->bounds.center);

  // Already handled first shape
  for (int i = start + 1; i < end; i++) {
    const Bounds& b = shapes[shapeIndices[i]]->bounds;
    nodeBounds.expand(b);
    centroidBounds.expand(b.center);
  }

  // Create node placeholder
  int nodeIndex = nodes.size();
  nodes.emplace_back();
  BVHNode& node = nodes.back();

  // If number of shapes is below threshold, make leaf node
  if (n <= LEAF_THRESHOLD) {
    node.bounds = nodeBounds;
    node.shapeIndex = start;
    node.shapeCount = n;
    return nodeIndex;
  }

  // Choose axis to split on (longest axis of centroid bounds)
  Vector extent = centroidBounds.max - centroidBounds.min;
  int axis = 0;
  if (extent.y() > extent.x()) axis = 1;
  if (extent.z() > extent[axis]) axis = 2;

  auto [splitIndex, splitPos] = getBestSAHSplit(shapes, start, end, axis);

  if (splitIndex <= start || splitIndex >= end) {
    // SAH failed to find a good split, do median split
    splitIndex = start + n / 2;
    // Sort shape indices by centroid along chosen axis
    std::nth_element(shapeIndices.begin() + start,
                     shapeIndices.begin() + splitIndex,
                     shapeIndices.begin() + end, [&](int a, int b) {
                       return shapes[a]->bounds.center[axis] <
                              shapes[b]->bounds.center[axis];
                     });
  } else {
    // Partition shapes around split position found by SAH
    auto midIter =
        std::partition(shapeIndices.begin() + start, shapeIndices.begin() + end,
                       [&](int index) {
                         return shapes[index]->bounds.center[axis] < splitPos;
                       });
    splitIndex = midIter - shapeIndices.begin();
  }

  // Recursively build child nodes
  int leftChild = buildRecursive(shapes, start, splitIndex);
  int rightChild = buildRecursive(shapes, splitIndex, end);

  // Swap children if needed to improve traversal performance (left first)
  if (nodes[leftChild].bounds.area > nodes[rightChild].bounds.area) {
    std::swap(leftChild, rightChild);
  }

  // Update current node
  node.bounds = nodeBounds;
  node.left = leftChild;
  node.right = rightChild;
  return nodeIndex;
}

// Find best split using Surface Area Heuristic (SAH)
// Returns pair of (split index, split position)
std::pair<int, double> BVH::getBestSAHSplit(
    const std::vector<std::unique_ptr<BoundedShape>>& shapes, int start,
    int end, int axis) {
  const int n = end - start;
  if (n <= 2) return std::make_pair(start, 0.0);  // No split possible

  // Compute node and centroid bounds
  Bounds parentBounds = shapes[shapeIndices[start]]->bounds;
  double centerMin = shapes[shapeIndices[start]]->bounds.center[axis];
  double centerMax = centerMin;
  for (int i = start + 1; i < end; i++) {
    const Bounds& b = shapes[shapeIndices[i]]->bounds;
    parentBounds.expand(b);
    double c = b.center[axis];
    if (c < centerMin) centerMin = c;
    if (c > centerMax) centerMax = c;
  }

  // If centroid bounds is degenerate, cannot split
  if (centerMax - centerMin < Vector::EPS) return std::make_pair(start, 0.0);

  // Initialize and fill bins
  std::vector<Bin> bins(BIN_COUNT);
  const double extentInv = 1.0 / (centerMax - centerMin);
  for (int i = start; i < end; ++i) {
    const Bounds& b = shapes[shapeIndices[i]]->bounds;
    double c = b.center[axis];
    int binIndex =
        std::min(static_cast<int>(BIN_COUNT * (c - centerMin) * extentInv),
                 BIN_COUNT - 1);
    bins[binIndex].add(b);
  }

  // Build prefix arrays for bins
  std::vector<Bounds> prefixBounds(BIN_COUNT);
  std::vector<int> prefixCounts(BIN_COUNT);
  Bounds accumBounds;
  int accumCount = 0;
  for (int i = 0; i < BIN_COUNT; ++i) {
    if (bins[i].count > 0) {
      if (accumCount == 0)
        accumBounds = bins[i].bounds;
      else
        accumBounds.expand(bins[i].bounds);
    }
    accumCount += bins[i].count;
    prefixBounds[i] = accumBounds;
    prefixCounts[i] = accumCount;
  }

  // Build suffix arrays for bins
  std::vector<Bounds> suffixBounds(BIN_COUNT);
  std::vector<int> suffixCounts(BIN_COUNT);
  accumBounds = Bounds();
  accumCount = 0;
  for (int i = BIN_COUNT - 1; i >= 0; --i) {
    if (bins[i].count > 0) {
      if (accumCount == 0)
        accumBounds = bins[i].bounds;
      else
        accumBounds.expand(bins[i].bounds);
    }
    accumCount += bins[i].count;
    suffixBounds[i] = accumBounds;
    suffixCounts[i] = accumCount;
  }

  // Evaluate split costs and find best split
  double minCost = std::numeric_limits<double>::max();
  int bestBin = -1;
  for (int i = 0; i < BIN_COUNT - 1; ++i) {
    // If either side is empty, skip
    if (prefixCounts[i] == 0 || suffixCounts[i + 1] == 0) continue;

    double leftSA = prefixBounds[i].area;
    double rightSA = suffixBounds[i + 1].area;
    double cost = TRAVERSAL_COST + INTERSECTION_COST *
                                       (leftSA * prefixCounts[i] +
                                        rightSA * suffixCounts[i + 1]) /
                                       parentBounds.area;
    if (cost < minCost) {
      minCost = cost;
      bestBin = i;
    }
  }

  if (bestBin == -1) return std::make_pair(start, 0.0);  // No valid split found

  // Convert best bin to split index
  double splitPos =
      centerMin + (bestBin + 1) * (centerMax - centerMin) / BIN_COUNT;

  // Count how many shapes go to the left of the split
  int leftCount = 0;
  for (int i = start; i < end; ++i) {
    const Bounds& b = shapes[shapeIndices[i]]->bounds;
    if (b.center[axis] < splitPos) {
      leftCount++;
    }
  }
  return std::make_pair(start + leftCount, splitPos);
}

void BVH::build(const std::vector<std::unique_ptr<BoundedShape>>& shapes) {
  // Initialize shape indices
  shapeIndices.resize(shapes.size());
  std::iota(shapeIndices.begin(), shapeIndices.end(), 0);

  // Clear and reserve nodes
  nodes.clear();
  nodes.reserve(shapes.size() * 2);

  if (shapes.empty()) return;

  // Build BVH recursively
  buildRecursive(shapes, 0, shapes.size());
}

// Traverse BVH with ray and invoke callback on hits
void BVH::traverse(const std::vector<std::unique_ptr<BoundedShape>>& shapes,
                   const Ray& ray,
                   const std::function<void(const HitInfo&)>& callback) const {
  if (nodes.empty()) return;

  struct StackItem {
    int nodeIndex;
    double tmin;
  };

  // Construct stack for traversal
  // Use a dynamic stack to avoid fixed-size overflow for deep BVHs
  std::vector<StackItem> stack;
  stack.emplace_back(StackItem{0, 0.0f});

  while (!stack.empty()) {
    StackItem item = stack.back();
    stack.pop_back();

    const BVHNode& node = nodes[item.nodeIndex];

    // Check if ray intersects node bounds
    double tmin, tmax;
    if (!node.bounds.intersects(ray, tmin, tmax)) continue;
    if (tmax < item.tmin) continue;

    if (node.shapeCount > 0) {
      // Leaf node: test all shapes in this node
      for (int i = 0; i < node.shapeCount; ++i) {
        std::optional<HitInfo> hitOpt =
            shapes[shapeIndices[node.shapeIndex + i]]->intersects(ray);
        if (hitOpt.has_value()) {
          callback(hitOpt.value());
        }
      }
    } else {
      // Internal node: push children onto stack (left then right)
      if (node.right >= 0) stack.emplace_back(StackItem{node.right, tmin});
      if (node.left >= 0) stack.emplace_back(StackItem{node.left, tmin});
    }
  }
}

void BVH::traverseFirstHit(
    const std::vector<std::unique_ptr<BoundedShape>>& shapes, const Ray& ray,
    const std::function<void(const HitInfo&)>& callback) const {
  if (nodes.empty()) return;

  struct StackItem {
    int nodeIndex;
    double tmin;
  };

  // Construct stack for traversal
  // Use a dynamic stack to avoid fixed-size overflow for deep BVHs
  std::vector<StackItem> stack;
  stack.emplace_back(StackItem{0, 0.0f});

  while (!stack.empty()) {
    StackItem item = stack.back();
    stack.pop_back();

    const BVHNode& node = nodes[item.nodeIndex];

    // Check if ray intersects node bounds
    double tmin, tmax;
    if (!node.bounds.intersects(ray, tmin, tmax)) continue;
    if (tmax < item.tmin) continue;

    if (node.shapeCount > 0) {
      // Leaf node: test all shapes in this node
      for (int i = 0; i < node.shapeCount; ++i) {
        std::optional<HitInfo> hitOpt =
            shapes[shapeIndices[node.shapeIndex + i]]->intersects(ray);
        if (hitOpt.has_value()) {
          callback(hitOpt.value());
          return;  // Stop after first hit
        }
      }
    } else {
      // Internal node: push children onto stack (left then right)
      if (node.right >= 0) stack.emplace_back(StackItem{node.right, tmin});
      if (node.left >= 0) stack.emplace_back(StackItem{node.left, tmin});
    }
  }
}