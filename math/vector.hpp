#pragma once

#include <iostream>

// Represents a 3D vector with common vector operations
class Vector {
 private:
  // Small error tolerance for floating point comparison
  static constexpr double EPS = 1e-9;
  double x;
  double y;
  double z;

 public:
  // Default constructor creates zero vector
  Vector() : x(0), y(0), z(0) {};
  Vector(double x, double y, double z) : x(x), y(y), z(z) {};

  Vector add(const Vector& other) const;
  Vector subtract(const Vector& other) const;
  Vector scale(double scalar) const;
  double dot(const Vector& other) const;
  double mag() const;
  double magSq() const;
  Vector norm() const;
  Vector proj(const Vector& other) const;
  Vector cross(const Vector& other) const;

  // Operator overloads
  Vector operator=(const Vector& other);
  Vector operator+(const Vector& other) const;
  Vector operator-(const Vector& other) const;
  Vector operator-() const;
  Vector operator*(double scalar) const;
  double operator*(const Vector& other) const;
  Vector operator/(double scalar) const;
  bool operator==(const Vector& other) const;
  bool operator!=(const Vector& other) const;

  friend std::ostream& operator<<(std::ostream& os, const Vector& vec);

  ~Vector() = default;
};

Vector operator*(double scalar, const Vector& vec);
