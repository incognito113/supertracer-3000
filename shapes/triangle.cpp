#include "triangle.hpp"

std::optional<HitInfo> Triangle::intersects(const Ray& ray) const {
    Vector edge1 = v1 - v0;
    Vector edge2 = v2 - v0;

    Vector h = ray.dir.cross(edge2);
    double a = edge1.dot(h);

    if (std::abs(a) < EPS) return std::nullopt;  // parallel

    double f = 1.0 / a;
    Vector s = ray.orig - v0;
    double u = f * s.dot(h);

    if (u < 0.0 || u > 1.0) return std::nullopt;

    Vector q = s.cross(edge1);
    double v = f * ray.dir.dot(q);

    if (v < 0.0 || u + v > 1.0) return std::nullopt;

    double t = f * edge2.dot(q);

    if (t < EPS) return std::nullopt;

    Vector hitPos = ray.at(t);

    return HitInfo(hitPos, normal, ray, t, &material);
}