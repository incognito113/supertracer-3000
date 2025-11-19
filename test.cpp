#include <cassert>
#include <iostream>

#include "color.hpp"

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

int main() {
  test_color();
  return 0;
}