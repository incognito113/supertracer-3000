#include <stdexcept>
#include <string>
#include <vector>

#include "metal.hpp"

MetalCompute::MetalCompute() {
  throw std::runtime_error("Metal is not available on this system!");
}

void MetalCompute::runKernel(const std::string&, std::vector<float>&) {}

MetalCompute::~MetalCompute() {}