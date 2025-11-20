#pragma once

#include "math/color.hpp"
#include "math/vector.hpp"

// Represents a point light source in the scene
struct Light {
  const Vector position;
  const Color color;
};