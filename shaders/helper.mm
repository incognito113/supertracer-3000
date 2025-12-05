#include "metal.hpp"
#include <Metal/Metal.hpp>

void MetalCompute::addHandler(MetalCommandBuffer *cb,
                              std::function<void()> fn) {
  cb->raw->addCompletedHandler(^(MTL::CommandBuffer *) {
    fn();
  });
}
