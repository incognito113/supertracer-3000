#include "color.hpp"

#include <algorithm>
#include <iostream>

Color Color::add(const Color& other) const {
  // Bound resulting color components to the range [0, 1]
  int new_r = std::min(this->r + other.r, 1.0);
  int new_g = std::min(this->g + other.g, 1.0);
  int new_b = std::min(this->b + other.b, 1.0);
  return Color(new_r, new_g, new_b);
}

Color Color::mult(const Color& other) const {
  // Multiply color components and scale to [0, 1]
  int new_r = this->r * other.r;
  int new_g = this->g * other.g;
  int new_b = this->b * other.b;
  return Color(new_r, new_g, new_b);
}

Color Color::scale(double factor) const {
  // Scale color components and bound to [0, 1]
  int new_r = std::min(this->r * factor, 1.0);
  int new_g = std::min(this->g * factor, 1.0);
  int new_b = std::min(this->b * factor, 1.0);
  return Color(new_r, new_g, new_b);
}

std::ostream& Color::operator<<(std::ostream& os) {
  os << "Color(" << r << ", " << g << ", " << b << ")";
  return os;
}
