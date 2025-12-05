#ifdef METAL

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal.hpp"

#include <Metal/Metal.hpp>

#include "metal_impl.hpp"

// Initialize Metal device, library, and command queue
MetalCompute::MetalCompute() {
  device = MTL::CreateSystemDefaultDevice();
  if (!device) throw std::runtime_error("No Metal device found!");

  NS::Error* error = nullptr;
  lib = device->newLibrary(NS::String::string("build/shaders/Shader.metallib",
                                              NS::UTF8StringEncoding),
                           &error);
  if (!lib)
    throw std::runtime_error(
        "Failed to load Metal library: " +
        std::string(error->localizedDescription()->utf8String()));

  queue = device->newCommandQueue();
}

// Clean up Metal resources
MetalCompute::~MetalCompute() {
  queue->release();
  lib->release();
  device->release();
}

#endif