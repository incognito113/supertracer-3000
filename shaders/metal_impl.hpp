#pragma once

#if __has_include(<Metal/Metal.hpp>)
#include <Metal/Metal.hpp>

#include "metal.hpp"

struct MetalDevice {
  MTL::Device* raw;
};
struct MetalLibrary {
  MTL::Library* raw;
};
struct MetalCommandQueue {
  MTL::CommandQueue* raw;
};
struct MetalCommandBuffer {
  MTL::CommandBuffer* raw;
};

// Run a compute kernel with given name and data buffers
// If callback is nullptr, the function runs synchronously
template <typename... Ts>
void MetalCompute::runKernel(const std::string& kernelName,
                             std::function<void()> callback,
                             std::vector<Ts>&... vectors) {
  using BufPtr =
      decltype(device->raw->newBuffer(0, MTL::ResourceStorageModeShared));

  NS::Error* error = nullptr;
  auto func = lib->raw->newFunction(
      NS::String::string(kernelName.c_str(), NS::UTF8StringEncoding));
  auto pipeline = device->raw->newComputePipelineState(func, &error);
  if (!pipeline)
    throw std::runtime_error(
        "Failed to create pipeline: " +
        std::string(error->localizedDescription()->utf8String()));

  // --- create buffers in runtime container ---
  std::vector<BufPtr> buffers = {device->raw->newBuffer(
      vectors.size() * sizeof(Ts), MTL::ResourceStorageModeShared)...};

  // Build arrays of source pointers and byte sizes
  constexpr std::size_t N = sizeof...(Ts);
  std::array<const void*, N> src_ptrs = {
      static_cast<const void*>(vectors.data())...};
  std::array<size_t, N> byte_sizes = {vectors.size() * sizeof(Ts)...};

  // Copy into GPU buffers
  for (std::size_t i = 0; i < buffers.size(); ++i) {
    memcpy(buffers[i]->contents(), src_ptrs[i], byte_sizes[i]);
  }

  // Create command buffer and encoder
  MetalCommandBuffer* cmdBufferStruct = new MetalCommandBuffer();
  cmdBufferStruct->raw = queue->raw->commandBuffer();
  auto commandBuffer = cmdBufferStruct->raw;
  auto encoder = commandBuffer->computeCommandEncoder();
  encoder->setComputePipelineState(pipeline);

  // Bind buffers to encoder
  for (std::size_t i = 0; i < buffers.size(); ++i) {
    encoder->setBuffer(buffers[i], 0, static_cast<unsigned>(i));
  }

  size_t maxSize = std::max({vectors.size()...});
  MTL::Size threadsPerGroup(256, 1, 1);
  MTL::Size numThreadgroups((maxSize + 255) / 256, 1, 1);
  encoder->dispatchThreadgroups(numThreadgroups, threadsPerGroup);
  encoder->endEncoding();

  // Prepare arrays for copying results back
  std::array<void*, N> dst_ptrs = {static_cast<void*>(vectors.data())...};

  // Define cleanup and copy lambda
  auto release_and_copy = [buffers = std::move(buffers), dst_ptrs, byte_sizes,
                           pipeline, func, cmdBufferStruct]() mutable {
    for (std::size_t i = 0; i < buffers.size(); ++i) {
      memcpy(dst_ptrs[i], buffers[i]->contents(), byte_sizes[i]);
      buffers[i]->release();
    }
    pipeline->release();
    func->release();
    delete cmdBufferStruct;
  };

  if (callback) {
    // capture release_and_copy and callback by value
    addHandler(cmdBufferStruct, [release_and_copy, callback]() mutable {
      release_and_copy();
      callback();
    });
    commandBuffer->commit();
  } else {
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    release_and_copy();
  }
}

template <typename... Ts>
void MetalCompute::runKernel(const std::string& kernelName,
                             std::vector<Ts>&... vectors) {
  runKernel(kernelName, {}, vectors...);
}
#endif