#ifdef METAL

#include "metal.hpp"
#include <Metal/Metal.hpp>

void MetalCompute::addHandler(MTL::CommandBuffer *cb,
                              std::function<void()> fn) {
  cb->addCompletedHandler(^(MTL::CommandBuffer *) {
    fn();
  });
}

#endif