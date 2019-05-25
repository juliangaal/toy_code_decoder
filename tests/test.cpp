#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <hw/util.hpp>
#include <opencv2/opencv.hpp>

using namespace hw::util;

TEST_CASE ("Test rotation/rotation matrix", "[test_rotation]") {
    auto in = cv::Point2f(1, 0);
    calc::rotate(in, units::Degrees(90.0));
    REQUIRE(std::abs(in.x - 0) <git  0.00001);
    REQUIRE(std::abs(in.y - 1) < 0.00001);

    auto in2 = cv::Point2f(1, 0);
    calc::rotate(in2, units::Degrees(-90.0));
    REQUIRE(std::abs(in2.x - 0) < 0.00001);
    REQUIRE(std::abs(in2.y - (-1)) < 0.00001);
}

TEST_CASE ("Test vector between two points", "[test_connecting_vector]") {
    auto a = cv::Point2f(1, 0);
    auto b = cv::Point2f(0, 0);
    auto vec1 = geo::connecting_vector(a, b);
    REQUIRE(vec1.x == 1);
    REQUIRE(vec1.y == 0);
    REQUIRE(calc::norm(vec1) == 1.0);

    auto c = cv::Point2f(4, 6);
    auto d = cv::Point2f(0, 0);
    auto vec2 = geo::connecting_vector(c, d);
    REQUIRE(std::abs(vec2.x - 0.5547) < 0.00001);
    REQUIRE(std::abs(vec2.y - 0.83205) < 0.00001);
    REQUIRE(calc::norm(vec2) == 1.0);
}
