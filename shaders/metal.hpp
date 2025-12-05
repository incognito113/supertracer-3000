#ifdef METAL

#pragma once

#include <Metal/Metal.hpp>
#include <functional>
#include <string>
#include <vector>

class MetalCompute {
 private:
  MTL::Device* device;
  MTL::Library* lib;
  MTL::CommandQueue* queue;

  void addHandler(MTL::CommandBuffer* cb, std::function<void()> fn);

 public:
  MetalCompute();

  template <typename... Ts>
  void runKernel(const std::string& kernelName, std::function<void()> callback,
                 std::vector<Ts>&... vectors);
  template <typename... Ts>
  void runKernel(const std::string& kernelName, std::vector<Ts>&... vectors);

  ~MetalCompute();
};

#include "metal_impl.hpp"
#endif
