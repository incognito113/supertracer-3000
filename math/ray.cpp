#include "ray.hpp"

#include "vector.hpp"

Vector Ray::at(double t) const { return orig + dir * t; }
