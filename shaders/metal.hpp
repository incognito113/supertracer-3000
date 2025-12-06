#pragma once

#ifdef METAL

#include <Metal/Metal.hpp>
#include <functional>
#include <string>
#include <vector>

#include "shaders/conversions.hpp"

class MetalCompute {
 private:
  MTL::Device* device;
  MTL::Library* lib;
  MTL::CommandQueue* queue;
  MTL::ComputePipelineState* pipeline;

  MTL::Buffer* sceneDataBuf = nullptr;
  MTL::Buffer* materialsBuf = nullptr;
  MTL::Buffer* lightsBuf = nullptr;
  MTL::Buffer* nodesBuf = nullptr;
  MTL::Buffer* trianglesBuf = nullptr;
  MTL::Buffer* spheresBuf = nullptr;
  MTL::Buffer* planesBuf = nullptr;
  MTL::Buffer* outputColorsBuf = nullptr;

  void addHandler(MTL::CommandBuffer* cb, std::function<void()> fn);

 public:
  MetalCompute();

  void init(Converter::GPU_Data& gpuData);
  void raytrace(Converter::GPU_SceneData& sceneData,
                std::function<void(simd_float3*, size_t)> callback);

  ~MetalCompute();
};

#endif
