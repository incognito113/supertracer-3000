#include <metal_stdlib>
using namespace metal;

kernel void speedTest(device float* data [[buffer(0)]],
                         uint id [[thread_position_in_grid]]) {
  float x = data[id];

  #pragma unroll
  for (uint i = 0; i < 65536; ++i) {
    x += 1;
  }

  data[id] = x;
}