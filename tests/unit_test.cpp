#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <notqrcode/util.hpp>
#include <notqrcode/notqrcode_decoder.hpp>
#include <opencv2/opencv.hpp>

using namespace notqrcode::util;

TEST_CASE("Test rotation/rotation matrix") {
    auto in = cv::Point2f(1, 0);
    calc::rotate(in, units::Degrees(90.0));
    REQUIRE(in.x == Approx(0.f).margin(0.0001));
    REQUIRE(in.y == Approx(1.f).margin(0.0001));

    auto in2 = cv::Point2f(1, 0);
    calc::rotate(in2, units::Degrees(-90.0));
    REQUIRE(in2.x == Approx(0.f).margin(0.0001));
    REQUIRE(in2.y == Approx(-1.f).margin(0.0001));
}

TEST_CASE("Test vector between two points") {
    auto a = cv::Point2f(1, 0);
    auto b = cv::Point2f(0, 0);
    auto vec1 = geo::connecting_vector(a, b);

    REQUIRE(vec1.x == Approx(-1.0f).margin(0.0001));
    REQUIRE(vec1.y == Approx(0.0f).margin(0.0001));
    REQUIRE(calc::norm(vec1) == Approx(1.0f).margin(0.0001));

    auto c = cv::Point2f(4, 6);
    auto d = cv::Point2f(0, 0);
    auto vec2 = geo::connecting_vector(c, d);
    REQUIRE(vec2.x == Approx(-0.5547).margin(0.0001));
    REQUIRE(vec2.y == Approx(-0.83205).margin(0.0001));
    REQUIRE(calc::norm(vec2) == Approx(1.0f).margin(0.0001));

    auto e = cv::Point2f(2, -2);
    auto f = cv::Point2f(2, -5);
    auto vec3 = geo::connecting_vector(e, f);

    REQUIRE(vec3.x == Approx(0.0f).margin(0.0001));
    REQUIRE(vec3.y == Approx(-1.0f).margin(0.0001));
    REQUIRE(calc::norm(vec3) == Approx(1.0f).margin(0.0001));

    auto vec4 = geo::connecting_vector(f, e);

    REQUIRE(vec4.x == Approx(0.0f).margin(0.0001));
    REQUIRE(vec4.y == Approx(1.0f).margin(0.0001));
    REQUIRE(calc::norm(vec4) == Approx(1.0f).margin(0.0001));
}

TEST_CASE("Test vector decoder") {
    // TODO fill with actual values
    std::vector<cv::KeyPoint> points(10);
    std::fill(points.begin(), points.end(), cv::KeyPoint(cv::Point2f(1, 0), 1));
    auto val = decode(points.cbegin(), points.cend(), 0.0);
    REQUIRE(val == std::pow(2, points.size())-1);

    std::fill(points.begin(), points.begin() + 0.5*points.size(), cv::KeyPoint(cv::Point2f(1, 0), 2));
    std::fill(points.begin() + 0.5*points.size(), points.end(), cv::KeyPoint(cv::Point2f(1, 0), 0));
    val = decode(points.cbegin(), points.cend(), 1.0);
    // size in vector 2222200000
    // should translate with avg 1.0 to 1111100000
    REQUIRE(val == 992);

    std::stable_partition(points.begin(), points.end(), [](const auto& p) { return p.size == 0; });
    val = decode(points.cbegin(), points.cend(), 1.0);
    // size in vector 0000022222
    // should translate with avg 1.0 to 1111100000
    REQUIRE(val == 31);
}

TEST_CASE("Various") {
    using namespace notqrcode::util::units;
    units::Degrees x = 90.0_deg;
    REQUIRE(x.to_rad() == Approx(PIf/2.0).margin(0.0001));
    REQUIRE(x.to_deg() == Approx(90.0f).margin(0.0001));
}

TEST_CASE("Constructor/Factory tests") {
    cv::Mat img = cv::imread("../tests/pics/rect_bw_16_0_xlarge.jpg");
    REQUIRE_THROWS(notqrcode::NotQRCodeDecoder::img(img));
}
