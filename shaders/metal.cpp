#ifdef METAL

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal.hpp"

#include <Metal/Metal.hpp>

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

MTL::Buffer* uploadToPrivateBuffer(MTL::Device* device,
                                   MTL::CommandQueue* queue, const void* data,
                                   size_t byteSize) {
  // Don't create zero-size buffers: make a 1-byte buffer instead
  if (byteSize == 0)
    return device->newBuffer(1, MTL::ResourceStorageModePrivate);

  // Create private buffer on GPU
  auto privateBuf =
      device->newBuffer(byteSize, MTL::ResourceStorageModePrivate);

  // Create a temporary shared buffer and copy CPU data into it
  auto stagingBuf = device->newBuffer(byteSize, MTL::ResourceStorageModeShared);
  std::memcpy(stagingBuf->contents(), data, byteSize);

  // Issue the blit
  auto commandBuffer = queue->commandBuffer();
  auto blit = commandBuffer->blitCommandEncoder();
  blit->copyFromBuffer(stagingBuf, 0, privateBuf, 0, byteSize);
  blit->endEncoding();

  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();

  // Clean up
  stagingBuf->release();
  commandBuffer->release();

  return privateBuf;
}

// Initialize MetalCompute with scene data
// NEED TO CALL init() AFTER CONSTRUCTION
void MetalCompute::init(Converter::GPU_Data& gpuData) {
  // Load ray generation function and create pipeline
  NS::Error* error = nullptr;
  auto func = lib->newFunction(
      NS::String::string("traceMultiBounce", NS::UTF8StringEncoding));
  auto localPipeline = device->newComputePipelineState(func, &error);
  if (!localPipeline) {
    if (func) func->release();
    throw std::runtime_error("Failed to create pipeline");
  }
  pipeline = localPipeline;
  // Cleanup function object
  if (func) func->release();

  // Initialize persistent GPU buffers for scene data
  materialsBuf = uploadToPrivateBuffer(
      device, queue, gpuData.materials.data(),
      gpuData.materials.size() * sizeof(Converter::GPU_Material));
  lightsBuf = uploadToPrivateBuffer(
      device, queue, gpuData.lights.data(),
      gpuData.lights.size() * sizeof(Converter::GPU_Light));
  nodesBuf =
      uploadToPrivateBuffer(device, queue, gpuData.nodes.data(),
                            gpuData.nodes.size() * sizeof(Converter::GPU_Node));
  shapeRefBuf = uploadToPrivateBuffer(
      device, queue, gpuData.shapeRefs.data(),
      gpuData.shapeRefs.size() * sizeof(Converter::GPU_ShapeRef));
  trianglesBuf = uploadToPrivateBuffer(
      device, queue, gpuData.triangles.data(),
      gpuData.triangles.size() * sizeof(Converter::GPU_Triangle));
  spheresBuf = uploadToPrivateBuffer(
      device, queue, gpuData.spheres.data(),
      gpuData.spheres.size() * sizeof(Converter::GPU_Sphere));
  planesBuf = uploadToPrivateBuffer(
      device, queue, gpuData.planes.data(),
      gpuData.planes.size() * sizeof(Converter::GPU_Plane));

  // Scene data buffer is shared as it will be updated each frame
  sceneDataBuf = device->newBuffer(sizeof(Converter::GPU_SceneData),
                                   MTL::ResourceStorageModeShared);

  // Output colors buffer is shared so CPU can read it back
  outputColorsBuf = device->newBuffer(
      gpuData.sceneData.width * gpuData.sceneData.height * sizeof(simd_float3),
      MTL::ResourceStorageModeShared);
}

// Generate initial rays from camera using Metal and pass to callback
void MetalCompute::raytrace(
    Converter::GPU_SceneData& sceneData,
    std::function<void(simd_float3*, size_t)> callback) {
  // Update persistent sceneData buffer
  std::memcpy(sceneDataBuf->contents(), &sceneData,
              sizeof(Converter::GPU_SceneData));

  const size_t nPixels = sceneData.width * sceneData.height;

  // Create command buffer and encoder
  auto commandBuffer = queue->commandBuffer();
  auto encoder = commandBuffer->computeCommandEncoder();
  encoder->setComputePipelineState(pipeline);

  // Set buffers
  encoder->setBuffer(sceneDataBuf, 0, 0);
  encoder->setBuffer(materialsBuf, 0, 1);
  encoder->setBuffer(lightsBuf, 0, 2);
  encoder->setBuffer(nodesBuf, 0, 3);
  encoder->setBuffer(shapeRefBuf, 0, 4);
  encoder->setBuffer(trianglesBuf, 0, 5);
  encoder->setBuffer(spheresBuf, 0, 6);
  encoder->setBuffer(planesBuf, 0, 7);
  encoder->setBuffer(outputColorsBuf, 0, 8);

  // Dispatch threads
  MTL::Size threadsPerGroup(8, 8, 1);
  MTL::Size numThreadGroups(
      (sceneData.width + threadsPerGroup.width - 1) / threadsPerGroup.width,
      (sceneData.height + threadsPerGroup.height - 1) / threadsPerGroup.height,
      1);
  encoder->dispatchThreadgroups(numThreadGroups, threadsPerGroup);
  encoder->endEncoding();

  // Bind callback to command buffer completion
  addHandler(commandBuffer, [this, callback, nPixels]() {
    simd_float3* colors =
        static_cast<simd_float3*>(outputColorsBuf->contents());
    callback(colors, nPixels);
  });

  commandBuffer->commit();
}

// Clean up Metal resources
MetalCompute::~MetalCompute() {
  if (queue) queue->release();
  if (lib) lib->release();
  if (device) device->release();
  if (pipeline) pipeline->release();

  if (sceneDataBuf) sceneDataBuf->release();
  if (materialsBuf) materialsBuf->release();
  if (lightsBuf) lightsBuf->release();
  if (nodesBuf) nodesBuf->release();
  if (shapeRefBuf) shapeRefBuf->release();
  if (trianglesBuf) trianglesBuf->release();
  if (spheresBuf) spheresBuf->release();
  if (planesBuf) planesBuf->release();
  if (outputColorsBuf) outputColorsBuf->release();
}

#endif