#pragma once

#include <iostream>

class Color {
 public:
  const double r;
  const double g;
  const double b;

  Color() : r(0), g(0), b(0) {};
  Color(double red, double green, double blue) : r(red), g(green), b(blue) {};
  Color(int red, int green, int blue);

  Color add(const Color& other) const;
  Color mult(const Color& other) const;
  Color scale(double factor) const;
  std::string get255String() const;

  Color operator+(const Color& other) const { return this->add(other); };
  Color operator*(const Color& other) const { return this->mult(other); };
  Color operator*(double factor) const { return this->scale(factor); };

  ~Color() = default;
};

std::ostream& operator<<(std::ostream& os, const Color& color);
