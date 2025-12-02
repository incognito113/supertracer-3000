#include <cmath>
#include "sphere.hpp"
#include "shape.hpp"

std::optional<HitInfo> Sphere::intersects(const Ray& ray) const {
    Vector s = ray.orig;
    Vector d = ray.dir;

    double a = d.dot(d);
    double b = 2.0 * d.dot(s - center);
    double c = (s - center).dot(s - center) - radius * radius;

    double discriminant = b*b - 4*a*c;
    if (discriminant < 0) return std::nullopt;

    double sqrtDisc = std::sqrt(discriminant);

    double t1 = (-b - sqrtDisc) / (2*a);
    double t2 = (-b + sqrtDisc) / (2*a);

    double t = -1;
    if (t1 > EPS) t = t1;
    else if (t2 > EPS) t = t2;
    else return std::nullopt;

    Vector hitPos = ray.at(t);
    Vector normal = (hitPos - center).norm();

    return HitInfo(hitPos, normal, ray, t, &material);
}