#ifdef METAL

#include <Metal/Metal.hpp>
#include "metal.hpp"

void MetalCompute::addHandler(MTL::CommandBuffer* cb, std::function<void()> fn) {
  cb->addCompletedHandler(^(MTL::CommandBuffer*) {
    fn();
  });
}

#endif