#include "vector.hpp"

#include <cmath>

// Add two vectors
Vector Vector::add(const Vector& other) const {
  return Vector(x + other.x, y + other.y, z + other.z);
}

// Subtract two vectors
Vector Vector::subtract(const Vector& other) const {
  return Vector(x - other.x, y - other.y, z - other.z);
}

// Scale vector by a double
Vector Vector::scale(double scalar) const {
  return Vector(x * scalar, y * scalar, z * scalar);
}

// Dot product of two vectors
double Vector::dot(const Vector& other) const {
  return x * other.x + y * other.y + z * other.z;
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

// ----- Overloaded Operators -----

// Assignment operator
Vector Vector::operator=(const Vector& other) const {
  return Vector(other.x, other.y, other.z);
}

// Addition
Vector Vector::operator+(const Vector& other) const { return this->add(other); }

// Subtraction
Vector Vector::operator-(const Vector& other) const {
  return this->subtract(other);
}

// Negation
Vector Vector::operator-() const { return Vector(-x, -y, -z); }

// Scalar multiplication
Vector Vector::operator*(double scalar) const { return this->scale(scalar); }
// Scalar multiplication from the left
Vector operator*(double scalar, const Vector& vec) { return vec.scale(scalar); }

// Scalar division
Vector Vector::operator/(double scalar) const {
  return this->scale(1.0 / scalar);
}

// Dot product
double Vector::operator*(const Vector& other) const { return this->dot(other); }

// Check if two vectors are equal
bool Vector::operator==(const Vector& other) const {
  return (std::abs(x - other.x) < EPS) && (std::abs(y - other.y) < EPS) &&
         (std::abs(z - other.z) < EPS);
}

// Check if two vectors are not equal
bool Vector::operator!=(const Vector& other) const { return !(*this == other); }

// Printing: Vector(x, y, z)
std::ostream& operator<<(std::ostream& os, const Vector& vec) {
  os << "Vector(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
  return os;
}