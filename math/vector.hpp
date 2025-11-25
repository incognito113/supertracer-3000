#pragma once

#include <iostream>

class Vector {
private:
    static constexpr double EPS = 1e-9;
    double x, y, z;

public:
    // Constructors
    Vector() : x(0), y(0), z(0) {}
    Vector(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    // Basic vector operations
    Vector add(const Vector& other) const;
    Vector subtract(const Vector& other) const;
    Vector scale(double scalar) const;

    double dot(const Vector& other) const;
    double mag() const;
    double magSq() const;
    Vector norm() const;

    // Extra features
    Vector proj(const Vector& other) const;    // projection
    Vector cross(const Vector& other) const;   // cross product

    // Operator overloads
    Vector& operator=(const Vector& other); 
    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    Vector operator-() const;
    Vector operator*(double scalar) const; 
    double operator*(const Vector& other) const;
    Vector operator/(double scalar) const;

    bool operator==(const Vector& other) const;
    bool operator!=(const Vector& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Vector& v);
};

// scalar * Vector
Vector operator*(double scalar, const Vector& vec);