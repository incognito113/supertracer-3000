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

static double gammaCorrect(double x) {
    x = std::clamp(x, 0.0, 1.0);
    return std::pow(x, 1.0 / 2.2);
}

std::array<u_char, 3> Color::getBytes() const {
    double r = gammaCorrect(v.x());
    double g = gammaCorrect(v.y());
    double b = gammaCorrect(v.z());

    return {
        static_cast<u_char>(std::round(r * 255)),
        static_cast<u_char>(std::round(g * 255)),
        static_cast<u_char>(std::round(b * 255))
    };
}
// Print color as Color(r, g, b)
std::ostream& operator<<(std::ostream& os, const Color& color) {
  os << "Color(" << color.v.x() << ", " << color.v.y() << ", " << color.v.z()
     << ")";
  return os;
}
