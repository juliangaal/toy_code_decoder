#include <hw/macros.hpp>
#include <hw/util.hpp>
#include <opencv2/opencv.hpp>

using namespace hw::util;

void test_rotation() {
    auto in = cv::Point2f(1, 0);
    calc::rotate(in, units::Degrees(90.0));
    ASSERT_EQUAL(static_cast<int>(in.x), 0); // weird fuck: -0.0000 if not casted to int
    ASSERT_EQUAL(in.y, 1);

    auto in2 = cv::Point2f(1, 0);
    calc::rotate(in2, units::Degrees(-90.0));
    ASSERT_EQUAL(static_cast<int>(in2.x), 0); // weird fuck: -0.0000 if not casted to int
    ASSERT_EQUAL(in2.y, -1);
}

void test_connecting_vector() {
    auto a = cv::Point2f(1, 0);
    auto b = cv::Point2f(0, 0);
    auto vec1 = geo::connecting_vector(a, b);
    ASSERT_EQUAL(vec1.x, 1);
    ASSERT_EQUAL(vec1.y, 0);
    ASSERT_EQUAL(calc::norm(vec1), 1.0);

    auto c = cv::Point2f(10, 10);
    auto d = cv::Point2f(-10, -10);
    auto vec2 = geo::connecting_vector(c, d);
    ASSERT_EQUAL(vec2.x, 0.707107);
    ASSERT_EQUAL(vec2.y, 0.707107);
    ASSERT_EQUAL(calc::norm(vec2), 1.0);
}

int main(void) {
  test_rotation();
  test_connecting_vector();
  return 0;
}

