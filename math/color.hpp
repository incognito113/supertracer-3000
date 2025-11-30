#pragma once

#include <algorithm>
#include <iostream>

#include "vector.hpp"

class Color {
 private:
  Vector v;  // internal storage for RGB

 public:
  // Constructors
  Color() : v(0, 0, 0) {}
  Color(double r, double g, double b) : v(r, g, b) {}
  Color(int red, int green, int blue);
  Color(const Vector& vec) : v(vec) {}

  // Accessors
  double r() const { return v.x(); }
  double g() const { return v.y(); }
  double b() const { return v.z(); }

  // Clamping (returns new Color)
  Color clamp() const;
  std::array<u_char, 3> getBytes() const;

  // Arithmetic operators
  Color operator+(const Color& other) const { return Color(v + other.v); }
  Color operator-(const Color& other) const { return Color(v - other.v); }
  Color operator*(double scalar) const { return Color(v * scalar); }
  Color operator*(const Color& other) const;
  Color operator/(double scalar) const { return Color(v / scalar); }

  Color& operator+=(const Color& other);
  Color& operator-=(const Color& other);
  Color& operator*=(double scalar);
  Color& operator/=(double scalar);
  bool operator==(const Color& other) const { return v == other.v; }
  bool operator!=(const Color& other) const { return v != other.v; }

  double mag() const { return v.mag(); }

  friend std::ostream& operator<<(std::ostream& os, const Color& c);
  friend inline Color operator*(double scalar, const Color& color) {
    return color * scalar;
  }
};