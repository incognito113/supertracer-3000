#pragma once

#include <optional>

#include "math/vector.hpp"
#include "shape.hpp"

class Sphere : public Shape {
public:
    Vector center;
    double radius;

    Sphere(const Vector& c, double r, const Material& mat)
        : Shape(mat), center(c), radius(r) {}

    std::optional<HitInfo> intersects(const Ray& ray) const override;

    Sphere* clone() const override { return new Sphere(*this); }
};