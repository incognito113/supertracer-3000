#pragma once
#include <Metal/Metal.hpp>

class MetalCompute {
 private:
  MTL::Device* device = nullptr;
  MTL::Library* lib = nullptr;
  MTL::CommandQueue* queue = nullptr;

 public:
  MetalCompute();

  void runKernel(const std::string& kernelName, std::vector<float>& data);

  ~MetalCompute();
};
