#include "vector.hpp"

#include <cmath>

// Add two vectors
Vector Vector::add(const Vector& other) const {
  return Vector(_x + other._x, _y + other._y, _z + other._z);
}

// Subtract two vectors
Vector Vector::subtract(const Vector& other) const {
  return Vector(_x - other._x, _y - other._y, _z - other._z);
}

// Scale vector by a double
Vector Vector::scale(double scalar) const {
  return Vector(_x * scalar, _y * scalar, _z * scalar);
}

// Dot product of two vectors
double Vector::dot(const Vector& other) const {
  return _x * other._x + _y * other._y + _z * other._z;
}

// Magnitude (length) of a vector
double Vector::mag() const { return sqrt(this->dot(*this)); }

// Magnitude squared of a vector (dot product with itself)
double Vector::magSq() const { return this->dot(*this); }

// Normalize the vector (make its length 1)
Vector Vector::norm() const {
  double magnitude = this->mag();

  // Return zero vector if magnitude is zero to avoid division by zero
  if (magnitude == 0) {
    return Vector();
  }

  return this->scale(1.0 / magnitude);
}

// Project this vector onto another vector
Vector Vector::proj(const Vector& other) const {
  // Formula: (this * other / other * other) * other
  double numerator = this->dot(other);
  double denominator = other.dot(other);
  double scalar = numerator / denominator;

  // If the denominator of the scalar is zero, return zero vector
  if (denominator == 0) {
    return Vector();
  }

  return other.scale(scalar);
}

// Cross product of two vectors
Vector Vector::cross(const Vector& other) const {
  return Vector(_y * other._z - _z * other._y, _z * other._x - _x * other._z,
                _x * other._y - _y * other._x);
}

// Component-wise minimum
Vector Vector::min(const Vector& other) const {
  return Vector(std::min(_x, other._x), std::min(_y, other._y),
                std::min(_z, other._z));
}

// Component-wise maximum
Vector Vector::max(const Vector& other) const {
  return Vector(std::max(_x, other._x), std::max(_y, other._y),
                std::max(_z, other._z));
}

// ----- Overloaded Operators -----

// Assignment operator
Vector Vector::operator=(const Vector& other) {
  this->_x = other._x;
  this->_y = other._y;
  this->_z = other._z;
  return *this;
}

// Addition
Vector Vector::operator+(const Vector& other) const { return this->add(other); }

// Subtraction
Vector Vector::operator-(const Vector& other) const {
  return this->subtract(other);
}

// Negation
Vector Vector::operator-() const { return Vector(-_x, -_y, -_z); }

// Scalar multiplication
Vector Vector::operator*(double scalar) const { return this->scale(scalar); }
// Scalar multiplication from the left
Vector operator*(double scalar, const Vector& vec) { return vec.scale(scalar); }

// Scalar division
Vector Vector::operator/(double scalar) const {
  return this->scale(1.0 / scalar);
}

// Add and assign
Vector& Vector::operator+=(const Vector& other) {
  this->_x += other._x;
  this->_y += other._y;
  this->_z += other._z;
  return *this;
}

// Subtract and assign
Vector& Vector::operator-=(const Vector& other) {
  this->_x -= other._x;
  this->_y -= other._y;
  this->_z -= other._z;
  return *this;
}

// Scale and assign
Vector& Vector::operator*=(double scalar) {
  this->_x *= scalar;
  this->_y *= scalar;
  this->_z *= scalar;
  return *this;
}

// Divide and assign
Vector& Vector::operator/=(double scalar) {
  this->_x /= scalar;
  this->_y /= scalar;
  this->_z /= scalar;
  return *this;
}

// Dot product
double Vector::operator*(const Vector& other) const { return this->dot(other); }

// Check if two vectors are equal
bool Vector::operator==(const Vector& other) const {
  return (std::abs(_x - other._x) < EPS) && (std::abs(_y - other._y) < EPS) &&
         (std::abs(_z - other._z) < EPS);
}

// Check if two vectors are not equal
bool Vector::operator!=(const Vector& other) const { return !(*this == other); }

// Indexing operator
double Vector::operator[](int index) const {
  switch (index) {
    case 0:
      return _x;
    case 1:
      return _y;
    case 2:
      return _z;
  }
  return 0.0;  // Default case (should not happen)
}

// Printing: Vector(x, y, z)
std::ostream& operator<<(std::ostream& os, const Vector& vec) {
  os << "Vector(" << vec._x << ", " << vec._y << ", " << vec._z << ")";
  return os;
}