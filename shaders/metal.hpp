#pragma once

#include <functional>
#include <string>
#include <vector>

// Forward declarations for Metal types to avoid including Metal headers here
struct MetalDevice;
struct MetalLibrary;
struct MetalCommandQueue;
struct MetalCommandBuffer;

class MetalCompute {
 private:
  MetalDevice* device;
  MetalLibrary* lib;
  MetalCommandQueue* queue;

  void addHandler(MetalCommandBuffer* cb, std::function<void()> fn);

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
