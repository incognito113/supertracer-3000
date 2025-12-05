#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal.hpp"

#include "metal_impl.hpp"

// Initialize Metal device, library, and command queue
MetalCompute::MetalCompute() {
  device = new MetalDevice();
  lib = new MetalLibrary();
  queue = new MetalCommandQueue();

  device->raw = MTL::CreateSystemDefaultDevice();
  if (!device->raw) throw std::runtime_error("No Metal device found!");

  NS::Error* error = nullptr;
  lib->raw = device->raw->newLibrary(
      NS::String::string("build/shaders/Shader.metallib",
                         NS::UTF8StringEncoding),
      &error);
  if (!lib->raw)
    throw std::runtime_error(
        "Failed to load Metal library: " +
        std::string(error->localizedDescription()->utf8String()));

  queue->raw = device->raw->newCommandQueue();
}

// Clean up Metal resources
MetalCompute::~MetalCompute() {
  queue->raw->release();
  lib->raw->release();
  device->raw->release();
  delete queue;
  delete lib;
  delete device;
}
