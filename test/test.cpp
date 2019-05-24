#include "macros.hpp"

void test() {
    ASSERT_EQUAL(7, 7);
}

void test2() {
    ASSERT_EQUAL(7, 7);
}

void test3() {
    ASSERT_EQUAL(8, 8);
}

int main(void) {
  test();
  test2();
  return 0;
}

