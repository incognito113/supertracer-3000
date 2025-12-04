#pragma once

#include <string>
#include <vector>

// Forward declarations for Metal types to avoid including Metal headers here
struct MetalDevice;
struct MetalLibrary;
struct MetalCommandQueue;

class MetalCompute {
 private:
  MetalDevice* device;
  MetalLibrary* lib;
  MetalCommandQueue* queue;

 public:
  MetalCompute();
  void runKernel(const std::string& kernelName, std::vector<float>& data);
  ~MetalCompute();
};