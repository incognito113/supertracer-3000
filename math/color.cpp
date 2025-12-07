#include "color.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

// Unclamped: convert rgb integers to doubles
Color::Color(int red, int green, int blue)
    : v((double)red / 255.0, (double)green / 255.0, (double)blue / 255.0) {}

// Component-wise multiplication
Color Color::operator*(const Color& other) const {
  return Color(v.x() * other.v.x(), v.y() * other.v.y(), v.z() * other.v.z());
}

// Add and assign
Color& Color::operator+=(const Color& other) {
  v += other.v;
  return *this;
}

// Sub and assign
Color& Color::operator-=(const Color& other) {
  v -= other.v;
  return *this;
}

// Scale and assign
Color& Color::operator*=(double scalar) {
  v *= scalar;
  return *this;
}

// Divide and assign
Color& Color::operator/=(double scalar) {
  v /= scalar;
  return *this;
}

// Clamp color components to [0, 1]
Color Color::clamp() const {
  return Color(std::clamp(v.x(), 0.0, 1.0), std::clamp(v.y(), 0.0, 1.0),
               std::clamp(v.z(), 0.0, 1.0));
}

// Get color components as bytes in [0, 255]
std::array<u_char, 3> Color::getBytes() const {
  u_char red255 =
      static_cast<u_char>(std::round(std::clamp(v.x(), 0.0, 1.0) * 255));
  u_char green255 =
      static_cast<u_char>(std::round(std::clamp(v.y(), 0.0, 1.0) * 255));
  u_char blue255 =
      static_cast<u_char>(std::round(std::clamp(v.z(), 0.0, 1.0) * 255));
  return {red255, green255, blue255};
}

// Print color as Color(r, g, b)
std::ostream& operator<<(std::ostream& os, const Color& color) {
  os << "Color(" << color.v.x() << ", " << color.v.y() << ", " << color.v.z()
     << ")";
  return os;
}
