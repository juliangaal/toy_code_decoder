#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <hw/util.hpp>
#include <opencv2/opencv.hpp>

using namespace hw::util;

TEST_CASE ("Test rotation/rotation matrix", "[test_rotation]") {
    auto in = cv::Point2f(1, 0);
    calc::rotate(in, units::Degrees(90.0));
    REQUIRE(static_cast<int>(in.x) == 0); // weird fuck: -0.0000 if not casted to int
    REQUIRE(in.y == 1);

    auto in2 = cv::Point2f(1, 0);
    calc::rotate(in2, units::Degrees(-90.0));
    REQUIRE(static_cast<int>(in2.x) == 0); // weird fuck: -0.0000 if not casted to int
    REQUIRE(in2.y == -1);
}

TEST_CASE ("Test vector between two points", "[test_connecting_vector]") {
    auto a = cv::Point2f(1, 0);
    auto b = cv::Point2f(0, 0);
    auto vec1 = geo::connecting_vector(a, b);
    REQUIRE(vec1.x == 1);
    REQUIRE(vec1.y == 0);
    REQUIRE(calc::norm(vec1) == 1.0);

    auto c = cv::Point2f(10, 10);
    auto d = cv::Point2f(-10, -10);
    auto vec2 = geo::connecting_vector(c, d);
    REQUIRE(vec2.x == 0.70711);
    REQUIRE(vec2.y == 0.70711);
    REQUIRE(calc::norm(vec2) == 1.0);
}
