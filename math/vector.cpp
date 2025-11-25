#include "vector.hpp"
#include <cmath>

// Basic vector operations

// Add two vectors
Vector Vector::add(const Vector& other) const {
    return Vector(x + other.x, y + other.y, z + other.z);
}

// Subtract two vectors
Vector Vector::subtract(const Vector& other) const {
    return Vector(x - other.x, y - other.y, z - other.z);
}

// Multiply vector by a scalar
Vector Vector::scale(double scalar) const {
    return Vector(x * scalar, y * scalar, z * scalar);
}

// Dot product of two vectors
double Vector::dot(const Vector& other) const {
    return x * other.x + y * other.y + z * other.z;
}

// Magnitude (length)
double Vector::mag() const {
    return std::sqrt(this->dot(*this));
}

// Magnitude squared
double Vector::magSq() const {
    return this->dot(*this);
}

// Unit vector in same direction
Vector Vector::norm() const {
    double magnitude = this->mag();

    if (magnitude == 0) {  // avoid division by zero
        return Vector();
    }
    return this->scale(1.0 / magnitude);
}

// Projection of this vector onto another vector
Vector Vector::proj(const Vector& other) const {
    double denom = other.dot(other);
    if (denom == 0) return Vector(); // avoid divide by zero

    double scalar = this->dot(other) / denom;
    return other.scale(scalar);
}

// Cross product
Vector Vector::cross(const Vector& other) const {
    return Vector(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

// Operator Overloads

// Assignment operator
Vector& Vector::operator=(const Vector& other) {
    if (this != &other) {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }
    return *this;
}

// Addition
Vector Vector::operator+(const Vector& other) const {
    return this->add(other);
}

// Subtraction
Vector Vector::operator-(const Vector& other) const {
    return this->subtract(other);
}

// Negate vector
Vector Vector::operator-() const {
    return Vector(-x, -y, -z);
}

// Scalar multiplication from the right
Vector Vector::operator*(double scalar) const {
    return this->scale(scalar);
}

// Dot product as operator
double Vector::operator*(const Vector& other) const {
    return this->dot(other);
}

// Scalar division
Vector Vector::operator/(double scalar) const {
    return this->scale(1.0 / scalar);
}

// Equality check
bool Vector::operator==(const Vector& other) const {
    return (std::abs(x - other.x) < EPS) &&
           (std::abs(y - other.y) < EPS) &&
           (std::abs(z - other.z) < EPS);
}

// Inequality check
bool Vector::operator!=(const Vector& other) const {
    return !(*this == other);
}

// Output formatting
std::ostream& operator<<(std::ostream& os, const Vector& vec) {
    os << "Vector(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

// Scalar * Vector
Vector operator*(double scalar, const Vector& vec) {
    return vec.scale(scalar);
}