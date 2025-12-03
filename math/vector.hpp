#pragma once

#include <iostream>

// Forward declaration
class Camera;

// Represents a 3D vector with common vector operations
class Vector {
 protected:
  double _x;
  double _y;
  double _z;

 public:
  // Small error tolerance for doubleing point comparison
  static constexpr double EPS = 1e-12;

  // Default constructor creates zero vector
  Vector() : _x(0), _y(0), _z(0) {};
  Vector(double val) : _x(val), _y(val), _z(val) {};
  Vector(double x, double y, double z) : _x(x), _y(y), _z(z) {};

  // Accessors
  double x() const { return _x; }
  double y() const { return _y; }
  double z() const { return _z; }

  Vector add(const Vector& other) const;
  Vector subtract(const Vector& other) const;
  Vector scale(double scalar) const;
  double dot(const Vector& other) const;
  double mag() const;
  double magSq() const;
  Vector norm() const;
  Vector proj(const Vector& other) const;
  Vector cross(const Vector& other) const;

  Vector min(const Vector& other) const;
  Vector max(const Vector& other) const;

  // Operator overloads
  Vector operator=(const Vector& other);
  Vector operator+(const Vector& other) const;
  Vector operator-(const Vector& other) const;
  Vector operator-() const;
  Vector operator*(double scalar) const;
  double operator*(const Vector& other) const;
  Vector operator/(double scalar) const;
  Vector& operator+=(const Vector& other);
  Vector& operator-=(const Vector& other);
  Vector& operator*=(double scalar);
  Vector& operator/=(double scalar);
  bool operator==(const Vector& other) const;
  bool operator!=(const Vector& other) const;
  double operator[](int index) const;

  friend std::ostream& operator<<(std::ostream& os, const Vector& vec);

  ~Vector() = default;
};

Vector operator*(double scalar, const Vector& vec);
