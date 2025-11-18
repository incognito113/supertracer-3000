#pragma once

#include <iostream>

class Color {
 public:
  const double r;
  const double g;
  const double b;

  Color() : r(0), g(0), b(0) {};
  Color(double red, double green, double blue) : r(red), g(green), b(blue) {};
  Color(int red, int green, int blue)
      : r(static_cast<double>(red / 255.0)),
        g(static_cast<double>(green / 255.0)),
        b(static_cast<double>(blue / 255.0)) {};

  Color add(const Color& other) const;
  Color mult(const Color& other) const;
  Color scale(double factor) const;

  std::ostream& operator<<(std::ostream& os);
  Color operator+(const Color& other) const { return this->add(other); };
  Color operator*(const Color& other) const { return this->mult(other); };
  Color operator*(double factor) const { return this->scale(factor); };

  ~Color() = default;
};
