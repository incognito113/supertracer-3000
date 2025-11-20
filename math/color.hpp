#pragma once

#include <iostream>

// Represents an RGB color: components in [0, 1]
class Color {
 private:
  double r;
  double g;
  double b;
  static constexpr double EPS = 1e-9;

 public:
  Color() : r(0), g(0), b(0) {};
  Color(double red, double green, double blue) : r(red), g(green), b(blue) {};
  Color(int red, int green, int blue);
  Color(Color const& other) : r(other.r), g(other.g), b(other.b) {};

  Color add(const Color& other) const;
  Color mult(const Color& other) const;
  Color scale(double factor) const;
  std::string get255String() const;

  Color operator=(const Color& other);
  Color operator+(const Color& other) const;
  Color operator*(const Color& other) const;
  Color operator*(double factor) const;
  bool operator==(const Color& other) const;
  bool operator!=(const Color& other) const;

  friend std::ostream& operator<<(std::ostream& os, const Color& color);

  ~Color() = default;
};
