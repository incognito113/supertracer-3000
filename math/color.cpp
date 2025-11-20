#include "color.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

// Convert rgb integers to doubles in [0, 1]
Color::Color(int red, int green, int blue)
    : r(std::min(std::max((double)red / 255.0, 0.0), 1.0)),
      g(std::min(std::max((double)green / 255.0, 0.0), 1.0)),
      b(std::min(std::max((double)blue / 255.0, 0.0), 1.0)) {};

// Bound resulting color components to the range [0, 1]
Color Color::add(const Color& other) const {
  double new_r = std::min(this->r + other.r, 1.0);
  double new_g = std::min(this->g + other.g, 1.0);
  double new_b = std::min(this->b + other.b, 1.0);
  return Color(new_r, new_g, new_b);
}

// Multiply color components (always in [0, 1])
Color Color::mult(const Color& other) const {
  double new_r = this->r * other.r;
  double new_g = this->g * other.g;
  double new_b = this->b * other.b;
  return Color(new_r, new_g, new_b);
}

// Scale color components and bound to [0, 1]
Color Color::scale(double factor) const {
  double new_r = std::max(std::min(this->r * factor, 1.0), 0.0);
  double new_g = std::max(std::min(this->g * factor, 1.0), 0.0);
  double new_b = std::max(std::min(this->b * factor, 1.0), 0.0);
  return Color(new_r, new_g, new_b);
}

std::string Color::get255String() const {
  int red_255 = static_cast<int>(std::round(this->r * 255));
  int green_255 = static_cast<int>(std::round(this->g * 255));
  int blue_255 = static_cast<int>(std::round(this->b * 255));
  return std::to_string(red_255) + " " + std::to_string(green_255) + " " +
         std::to_string(blue_255);
}

// ----- Overloaded Operators -----

// Assignment operator
Color Color::operator=(const Color& other) {
  this->r = other.r;
  this->g = other.g;
  this->b = other.b;
  return *this;
}

// Addition
Color Color::operator+(const Color& other) const { return this->add(other); }

// Multiplication
Color Color::operator*(const Color& other) const { return this->mult(other); }

// Scalar multiplication
Color Color::operator*(double factor) const { return this->scale(factor); }

// Check if two colors are equal
bool Color::operator==(const Color& other) const {
  return (std::abs(r - other.r) < EPS) && (std::abs(g - other.g) < EPS) &&
         (std::abs(b - other.b) < EPS);
}

// Check if two colors are not equal
bool Color::operator!=(const Color& other) const { return !(*this == other); }

// Print color as Color(r, g, b)
std::ostream& operator<<(std::ostream& os, const Color& color) {
  os << "Color(" << color.r << ", " << color.g << ", " << color.b << ")";
  return os;
}
