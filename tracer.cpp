#include "tracer.hpp"

Vector Ray::at(double t) const { return orig + dir * t; }
