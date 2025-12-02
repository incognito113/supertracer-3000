#include "plane.hpp"
#include <cmath>

std::optional<HitInfo> Plane::intersects(const Ray& ray) const {
    double denom = normal.dot(ray.dir);

    if (std::abs(denom) < EPS)
        return std::nullopt;  // parallel

    double t = (point - ray.orig).dot(normal) / denom;
    if (t < EPS)
        return std::nullopt;  // behind camera

    Vector hitPos = ray.at(t);
    return HitInfo(hitPos, normal, ray, t, &material);
}