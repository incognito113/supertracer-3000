#include <cassert>
#include <cmath>
#include <iostream>

#include "color.hpp"
#include "vector.hpp"

void test_color() {
  Color c0;
  Color c1(0.5, 0.2, 0.3);
  Color c2(0.4, 0.6, 0.1);
  Color c3(255, 51, 150);

  Color c4 = c1 + c2;
  Color c5 = c1 * c2;
  Color c6 = c1 * 2.0;
  Color c7 = c3 + c3;

  assert(c0.r == 0.0 && c0.g == 0.0 && c0.b == 0.0);
  assert(c4.r == 0.9 && c4.g == 0.8 && c4.b == 0.4);
  assert(c5.r == 0.2 && c5.g == 0.12 && c5.b == 0.03);
  assert(c6.r == 1.0 && c6.g == 0.4 && c6.b == 0.6);
  assert(c7.r == 1.0 && c7.g == 0.4 && c7.b == 1.0);
  assert(c3.get255String() == "255 51 150");
}

void test_vector() {
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

  assert(v3.x == 3.0 && v3.y == 4.0 && v3.z == 1.0);
  assert(v4.x == 1.0 && v4.y == 2.0 && v4.z == 1.0);
  assert(v5.x == 4.0 && v5.y == 6.0 && v5.z == 2.0);
  assert(dot_product == 5.0);
  assert(magnitude_v1_sq == 14.0);
  assert(magnitude_v1 == std::sqrt(14.0));
  assert(v6.x == 2.5 && v6.y == 2.5 && v6.z == 0.0);
  assert(v7.x == -2.0 && v7.y == -3.0 && v7.z == -1.0);
  assert(v5 == v8 && v8 == v9);
  assert(v1 != v2);
  assert(v10.mag() == 1.0 && v1 / v1.mag() == v10);
}

int main() {
  test_color();
  test_vector();
  return 0;
}