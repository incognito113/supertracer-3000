#pragma once

#include "math/color.hpp"

// Stores matieral properties for shapes
struct Material {
  const Color diffuse;
  const Color specular;
  const double shininess;
  const double reflectivity;
};