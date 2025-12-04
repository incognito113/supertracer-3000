// Flag definitions to fix Metal build issues
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal.hpp"

#include <Metal/Metal.hpp>
#include <iostream>
#include <vector>

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

// Run a compute kernel with given name on the data buffer
void MetalCompute::runKernel(const std::string& kernelName,
                             std::vector<float>& data) {
  NS::Error* error = nullptr;
  auto func = lib->newFunction(
      NS::String::string(kernelName.c_str(), NS::UTF8StringEncoding));
  auto pipeline = device->newComputePipelineState(func, &error);
  if (!pipeline)
    throw std::runtime_error(
        "Failed to create pipeline: " +
        std::string(error->localizedDescription()->utf8String()));

  auto buffer = device->newBuffer(data.size() * sizeof(float),
                                  MTL::ResourceStorageModeShared);
  memcpy(buffer->contents(), data.data(), data.size() * sizeof(float));

  auto commandBuffer = queue->commandBuffer();
  auto encoder = commandBuffer->computeCommandEncoder();
  encoder->setComputePipelineState(pipeline);
  encoder->setBuffer(buffer, 0, 0);

  // 256 is a common threadgroup size
  MTL::Size threadsPerGroup(256, 1, 1);
  MTL::Size numThreadgroups((data.size() + 255) / 256, 1, 1);
  encoder->dispatchThreadgroups(numThreadgroups, threadsPerGroup);

  encoder->endEncoding();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();

  memcpy(data.data(), buffer->contents(), data.size() * sizeof(float));

  buffer->release();
  pipeline->release();
  func->release();
}

// Clean up Metal resources
MetalCompute::~MetalCompute() {
  queue->release();
  lib->release();
  device->release();
}
