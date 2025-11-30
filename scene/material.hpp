#pragma once

#include "math/color.hpp"

// Stores matieral properties for shapes
struct Material {
  const Color color;
  const Color specular = Color(255, 255, 255);
  const double specularFactor = 0.5;
  const double shininess = 8.0;
  const double reflectivity;
};