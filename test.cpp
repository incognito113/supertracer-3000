#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <optional>
#include <thread>

#include "math/camera.hpp"
#include "math/color.hpp"
#include "math/ray.hpp"
#include "math/vector.hpp"

#ifdef METAL
#include "shaders/conversions.hpp"
#include "shaders/metal.hpp"
#endif

#include "shapes/plane.hpp"
#include "shapes/sphere.hpp"
#include "shapes/triangle.hpp"

void test_color() {
  std::cout << "Testing Color class..." << std::endl;

  Color c0;
  Color c1(0.5, 0.2, 0.3);
  Color c2(0.4, 0.6, 0.1);
  Color c3(255, 51, 150);

  Color c4 = c1 + c2;
  Color c5 = c1 * c2;
  Color c6 = c1 * 2.0;
  Color c7 = c3 + c3;

  assert(c0 == Color(0.0, 0.0, 0.0));
  assert(c4 == Color(0.9, 0.8, 0.4));
  assert(c5 == Color(0.2, 0.12, 0.03));
  assert(c6 == Color(1.0, 0.4, 0.6));
  assert(c7.clamp() == Color(1.0, 0.4, 1.0));

  const std::array<u_char, 3> bytes = c3.getBytes();
  assert(bytes[0] == static_cast<u_char>(255));
  assert(bytes[1] == static_cast<u_char>(51));
  assert(bytes[2] == static_cast<u_char>(150));
}

void test_vector() {
  std::cout << "Testing Vector class..." << std::endl;

  Vector v1(2.0, 3.0, 1.0);
  Vector v2(1.0, 1.0, 0.0);

  Vector v3 = v1 + v2;
  Vector v4 = v1 - v2;
  Vector v5 = v1 * 2.0;
  double dot_product = v1 * v2;
  double magnitude_v1 = v1.mag();
  double magnitude_v1_sq = v1.magSq();
  Vector v6 = v1.proj(v2);
  Vector v7 = -v1;
  Vector v8 = v1 / 0.5;
  Vector v9 = 2.0 * v1;
  Vector v10 = v1.norm();
  Vector v11 = v1.cross(v2);

  assert(v3 == Vector(3.0, 4.0, 1.0));
  assert(v4 == Vector(1.0, 2.0, 1.0));
  assert(v5 == Vector(4.0, 6.0, 2.0));
  assert(dot_product == 5.0);
  assert(magnitude_v1_sq == 14.0);
  assert(magnitude_v1 == std::sqrt(14.0));
  assert(v6 == Vector(2.5, 2.5, 0.0));
  assert(v7 == Vector(-2.0, -3.0, -1.0));
  assert(v5 == v8 && v8 == v9);
  assert(v1 != v2);
  assert(v10.mag() == 1.0 && v1 / v1.mag() == v10);
  assert(v11 == Vector(-1.0, 1.0, -1.0));
}

void test_sphere_intersect() {
  std::cout << "Testing Sphere intersection..." << std::endl;

  Sphere sphere1(Vector(0.0, 0.0, 0.0), 1.0, 0);
  Sphere sphere2(Vector(2.0, 2.0, 2.0), 0.5, 0);
  Ray ray(Vector(0.0, 0.0, -5.0), Vector(0.0, 0.0, 1.0));

  std::optional<HitInfo> hitInfoOpt1 = sphere1.intersects(ray);
  assert(hitInfoOpt1.has_value());
  HitInfo hitInfo = hitInfoOpt1.value();
  assert(std::abs(hitInfo.t - 4.0) < 1e-6);
  assert(hitInfo.pos == Vector(0.0, 0.0, -1.0));
  assert(hitInfo.normal == Vector(0.0, 0.0, -1.0));

  std::optional<HitInfo> hitInfoOpt2 = sphere2.intersects(ray);
  assert(!hitInfoOpt2.has_value());
}

void test_plane_intersect() {
  std::cout << "Testing Plane intersection..." << std::endl;

  Plane plane1(Vector(0.0, 5.0, 0.0), Vector(0.0, 1.0, 0.0), 0);
  Plane plane2(Vector(0.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0), 0);
  Ray ray(Vector(0.0, -1.0, 0.0), Vector(0.0, 1.0, 0.0));

  std::optional<HitInfo> hitInfoOpt1 = plane1.intersects(ray);
  assert(hitInfoOpt1.has_value());
  HitInfo hitInfo = hitInfoOpt1.value();
  assert(std::abs(hitInfo.t - 6.0) < 1e-6);
  assert(hitInfo.pos == Vector(0.0, 5.0, 0.0));
  assert(hitInfo.normal == Vector(0.0, 1.0, 0.0));

  std::optional<HitInfo> hitInfoOpt2 = plane2.intersects(ray);
  assert(!hitInfoOpt2.has_value());
}

void test_triangle_intersect() {
  std::cout << "Testing Triangle intersection..." << std::endl;

  Vector v1(0.0, 0.0, 0.0);
  Vector v2(1.0, 0.0, 0.0);
  Vector v3(0.0, 1.0, 0.0);

  Triangle tri(v1, v2, v3, 0);

  Ray ray1(Vector(0.25, 0.25, 1.0), Vector(0.0, 0.0, -1.0));
  auto hit1 = tri.intersects(ray1);

  assert(hit1.has_value());
  assert(std::abs(hit1->t - 1.0) < 1e-6);
  assert(hit1->pos == Vector(0.25, 0.25, 0.0));
  assert(hit1->normal == Vector(0.0, 0.0, 1.0) ||
         hit1->normal == Vector(0.0, 0.0, -1.0));

  Ray ray2(Vector(2.0, 2.0, 1.0), Vector(0.0, 0.0, -1.0));
  auto hit2 = tri.intersects(ray2);
  assert(!hit2.has_value());

  Ray ray3(Vector(0.2, 0.2, 1.0), Vector(1.0, 0.0, 0.0));
  auto hit3 = tri.intersects(ray3);
  assert(!hit3.has_value());
}

void test_metal() {
#ifdef METAL
  std::cout << "Testing Metal integration..." << std::endl;

  Scene scene(1400, 800, 5);
  scene.setBackground(25, 25, 112);

  BVH bvh(scene);

  Converter converter;
  auto result = converter.convertAll(scene, bvh);
  bool callbackCalled = false;

  MetalCompute metalCompute;
  metalCompute.init(result);

  auto time_start = std::chrono::high_resolution_clock::now();

  metalCompute.raytrace(result.sceneData,
                        [&](simd_float3*, size_t) { callbackCalled = true; });

  while (!callbackCalled) {
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
  auto time_end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                     time_end - time_start)
                     .count();
  std::cout << "Metal raytrace completed in " << elapsed << " microseconds."
            << std::endl;

#else
  std::cout << "Metal not available, skipping test." << std::endl;
#endif
}

int main() {
  test_color();
  test_vector();
  test_sphere_intersect();
  test_plane_intersect();
  test_triangle_intersect();
  test_metal();

  std::cout << "All tests passed!" << std::endl;

  return 0;
}