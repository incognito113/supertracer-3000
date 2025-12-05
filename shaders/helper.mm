#include "metal.hpp"
#include <Metal/Metal.hpp>

struct MetalCommandBuffer {
  MTL::CommandBuffer *raw;
};

void MetalCompute::addHandler(MetalCommandBuffer *cb,
                              std::function<void()> fn) {
  cb->raw->addCompletedHandler(^(MTL::CommandBuffer *) {
    fn();
  });
}
