#include <metal_stdlib>
using namespace metal;

#include "intersections.metal"
#include "structs.metal"

inline bool BVHintersects(const GPU_Ray ray, const device GPU_Node* nodes,
                          const device GPU_ShapeRef* shapeRefs,
                          const device GPU_Triangle* triangles,
                          const device GPU_Sphere* spheres,
                          thread GPU_HitInfo* hitInfo) {
  bool hitSomething = false;
  GPU_HitInfo bestHit;
  bestHit.t = FLT_MAX;

  struct StackItem {
    int nodeIndex;
    float tMin;
  };

  const int stackSize = 64;
  thread StackItem stack[stackSize];
  int stackPtr = 0;

  stack[stackPtr++] = {0, 0.0};  // Start with root node

  while (stackPtr > 0) {
    StackItem stackItem = stack[--stackPtr];
    int nodeIndex = stackItem.nodeIndex;
    float stackTMin = stackItem.tMin;
    const device GPU_Node* node = &nodes[nodeIndex];

    // Check if ray intersects the node's bounding box
    float tmin = 0;
    float tmax = FLT_MAX;
    // if (!intersectsNode(ray, node, &tmin, &tmax)) continue;
    if (tmax < stackTMin) continue;

    if (node->primCount > 0) {
      // Leaf node: test all shapes in this node
      for (int i = 0; i < node->primCount; ++i) {
        GPU_HitInfo candidateHit;
        candidateHit.t = FLT_MAX;
        bool hit = false;

        const device GPU_ShapeRef* shapeRef = &shapeRefs[node->primIndex + i];

        switch (shapeRef->shapeType) {
          case 0:  // Triangle
            hit = intersectsTriangle(ray, &triangles[shapeRef->shapeIndex],
                                     &candidateHit);
            break;

          case 1:  // Sphere
            hit = intersectsSphere(ray, &spheres[shapeRef->shapeIndex],
                                   &candidateHit);
            break;
        }

        if (hit && candidateHit.t < bestHit.t) {
          bestHit = candidateHit;
          hitSomething = true;
        }
      }
    } else {
      // Internal node: push child nodes onto stack
      if (node->left >= 0 && stackPtr < stackSize) {
        stack[stackPtr++] = {node->left, tmin};
      }
      if (node->right >= 0 && stackPtr < stackSize) {
        stack[stackPtr++] = {node->right, tmin};
      }
    }
  }

  if (hitSomething) {
    hitInfo->t = bestHit.t;
    hitInfo->position = bestHit.position;
    hitInfo->normal = bestHit.normal;
    hitInfo->materialIndex = bestHit.materialIndex;
  }
  return hitSomething;
}
