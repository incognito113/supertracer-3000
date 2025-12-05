#pragma once

#ifdef METAL

#include <Metal/Metal.hpp>

#include "metal.hpp"

// Run a compute kernel with given name and data buffers
// If callback is nullptr, the function runs synchronously
template <typename... Ts>
void MetalCompute::runKernel(const std::string& kernelName,
                             std::function<void()> callback,
                             std::vector<Ts>&... vectors) {
  using BufPtr = decltype(device->newBuffer(0, MTL::ResourceStorageModeShared));

  NS::Error* error = nullptr;
  auto func = lib->newFunction(
      NS::String::string(kernelName.c_str(), NS::UTF8StringEncoding));
  auto pipeline = device->newComputePipelineState(func, &error);
  if (!pipeline)
    throw std::runtime_error(
        "Failed to create pipeline: " +
        std::string(error->localizedDescription()->utf8String()));

  // --- create buffers in runtime container ---
  std::vector<BufPtr> buffers = {device->newBuffer(
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
  auto commandBuffer = queue->commandBuffer();
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
                           pipeline, func, commandBuffer]() mutable {
    for (std::size_t i = 0; i < buffers.size(); ++i) {
      memcpy(dst_ptrs[i], buffers[i]->contents(), byte_sizes[i]);
      buffers[i]->release();
    }
    pipeline->release();
    func->release();
    commandBuffer->release();
  };

  if (callback) {
    // capture release_and_copy and callback by value
    addHandler(commandBuffer, [release_and_copy, callback]() mutable {
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