#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

class Triangle : public Shape {
public:
    Vector v0, v1, v2;
    Vector normal;
    
    Triangle(const Vector& a,
             const Vector& b,
             const Vector& c,
             const Material& mat)
        : Shape(mat), v0(a), v1(b), v2(c) {
        normal = ((v1 - v0).cross(v2 - v0)).norm();
    }

    std::optional<HitInfo> intersects(const Ray& ray) const override;

    Triangle* clone() const override { return new Triangle(*this); }
};