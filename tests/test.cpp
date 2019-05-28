#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <toy_decoder/toy_decoder.hpp>
#include <toy_decoder/util.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <sstream>
#include <string>

using namespace toy_decoder::util;

TEST_CASE("Test rotation/rotation matrix", "[test_rotation]") {
    auto in = cv::Point2f(1, 0);
    calc::rotate(in, units::Degrees(90.0));
    REQUIRE(std::abs(in.x - 0) < 0.00001);
    REQUIRE(std::abs(in.y - 1) < 0.00001);

    auto in2 = cv::Point2f(1, 0);
    calc::rotate(in2, units::Degrees(-90.0));
    REQUIRE(std::abs(in2.x - 0) < 0.00001);
    REQUIRE(std::abs(in2.y - (-1)) < 0.00001);
}

TEST_CASE("Test vector between two points", "[test_connecting_vector]") {
    auto a = cv::Point2f(1, 0);
    auto b = cv::Point2f(0, 0);
    auto vec1 = geo::connecting_vector(a, b);

    REQUIRE(vec1.x == -1.0f);
    REQUIRE(vec1.y == 0.0f);
    REQUIRE(calc::norm(vec1) == 1.0f);

    auto c = cv::Point2f(4, 6);
    auto d = cv::Point2f(0, 0);
    auto vec2 = geo::connecting_vector(c, d);
    REQUIRE(std::abs(vec2.x - (-0.5547)) < 0.01f);
    REQUIRE(std::abs(vec2.y - (-0.83205)) < 0.01f);
    REQUIRE(calc::norm(vec2) == 1.0);

    auto e = cv::Point2f(2, -2);
    auto f = cv::Point2f(2, -5);
    auto vec3 = geo::connecting_vector(e, f);

    REQUIRE(vec3.x == 0.0f);
    REQUIRE(vec3.y == -1.0f);
    REQUIRE(calc::norm(vec3) == 1.0f);

    auto vec4 = geo::connecting_vector(f, e);

    REQUIRE(vec4.x == 0.0f);
    REQUIRE(vec4.y == 1.0f);
    REQUIRE(calc::norm(vec4) == 1.0f);
}

TEST_CASE("Test vector decoder", "[test_vector_decoder]") {
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

TEST_CASE("Test rotation detection", "[test_rotation_detection]") {
    using namespace toy_decoder;
    float orientation;
    bool result;

    {
        std::string file = "rect_bw_45";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~45, in this case 43
        REQUIRE(std::abs(orientation - 43.94f) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.save_img(file + "_rotated.jpg");
    }

    {
        std::string file = "rect_bw_90";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~90, in this case 89.25
        REQUIRE(std::abs(orientation - 89.25f) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.save_img(file + "_rotated.jpg");
    }

    {
        std::string file = "rect_bw_neg45";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~-45, in this case -44.47
        REQUIRE(std::abs(orientation - (-44.47f)) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.save_img(file + "_rotated.jpg");
    }

    {
        std::string file = "rect_bw_neg135";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~-135, in this case -135.5
        REQUIRE(std::abs(orientation - (-135.5f)) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.save_img(file + "_rotated.jpg");
    }
}

TEST_CASE("Test decoder vallues", "[test_decoder_values]") {
    using namespace toy_decoder;
    float orientation;
    cv::Point2i decoded_point;
    bool result;

    {
        // this image encodes -135 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_neg135";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~-135, in this case -135.5
        REQUIRE(std::abs(orientation - (-135.5f)) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.rotate_keypoints(units::Degrees(orientation));

        std::tie(decoded_point, result) = decoder.decode();
        REQUIRE(decoded_point.x == 10);
        REQUIRE(decoded_point.y == 5);
    }

    {
        // this image encodes -45 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_neg45";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~-45, in this case -44.47
        REQUIRE(std::abs(orientation - (-44.47f)) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.rotate_keypoints(units::Degrees(orientation));

        std::tie(decoded_point, result) = decoder.decode();
        REQUIRE(decoded_point.x == 10);
        REQUIRE(decoded_point.y == 5);
    }

    {
        // this image encodes 90 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_90";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~90, in this case 89.25
        REQUIRE(std::abs(orientation - 89.25f) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.rotate_keypoints(units::Degrees(orientation));

        std::tie(decoded_point, result) = decoder.decode();
        REQUIRE(decoded_point.x == 10);
        REQUIRE(decoded_point.y == 5);
    }

    {
        // this image encodes 45 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_45";
        cv::Mat im = cv::imread(("../pics/" + file + ".jpg"), cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Draw::YES);
        std::tie(orientation, result) = decoder.calculate_orientation(YES);
        // should detect ~45, in this case 43.94
        REQUIRE(std::abs(orientation - 43.94f) < 0.01f);

        decoder.rotate_img(units::Degrees(orientation));
        decoder.rotate_keypoints(units::Degrees(orientation));

        std::tie(decoded_point, result) = decoder.decode();
        REQUIRE(decoded_point.x == 10);
        REQUIRE(decoded_point.y == 5);
    }
}

TEST_CASE("Various", "[various]") {
    using namespace toy_decoder::util::units;
    units::Degrees x = 90.0_deg;
    REQUIRE(x.to_rad() == PIf/2.0);
    REQUIRE(x.to_deg() == 90.0);
}
