#pragma once

#include <optional>

#include "math/color.hpp"
#include "math/ray.hpp"
#include "math/vector.hpp"
#include "scene/material.hpp"

struct HitInfo {
    Vector pos;
    Vector normal;
    Ray ray;
    double t;
    const Material* material;

    HitInfo(const Vector& p,
            const Vector& n,
            const Ray& r,
            double tval,
            const Material* m)
        : pos(p),
          normal(n.norm()),   // ensure normalized
          ray(r),
          t(tval),
          material(m) {}

    HitInfo(const HitInfo& other)
        : pos(other.pos),
          normal(other.normal),
          ray(other.ray),
          t(other.t),
          material(other.material) {}
};

class Shape {
public:
    static constexpr double EPS = 1e-6;

public:
    Material material;

    Shape(const Material& mat) : material(mat) {}

    virtual std::optional<HitInfo> intersects(const Ray& ray) const = 0;

    virtual Shape* clone() const = 0;

    virtual ~Shape() {}
};