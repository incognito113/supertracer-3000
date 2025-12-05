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
  void runKernel(const std::string& kernelName, std::vector<float>& data,
                 std::function<void(std::vector<float>&)> callback = nullptr);
  ~MetalCompute();
};