#pragma once

#include <string>
#include <vector>

#if __has_include(<Metal/Metal.hpp>)
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

#else

// Empty class definition if Metal is not available
class MetalCompute {
 public:
  MetalCompute() {
    throw std::runtime_error("Metal is not available on this system!");
  }
  void runKernel(const std::string&, std::vector<float>&) {}
  ~MetalCompute() {}
};

#endif
