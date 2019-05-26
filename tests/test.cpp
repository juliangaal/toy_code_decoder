#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <toy_decoder/toy_decoder.hpp>
#include <opencv2/opencv.hpp>
#include <toy_decoder/util.hpp>
#include <fmt/format.h>
#include <toy_decoder/util.hpp>

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

    REQUIRE(vec1.x == -1.f);
    REQUIRE(vec1.y == 0.f);
    REQUIRE(calc::norm(vec1) == 1.0);

    auto c = cv::Point2f(4, 6);
    auto d = cv::Point2f(0, 0);
    auto vec2 = geo::connecting_vector(c, d);
    REQUIRE(std::abs(vec2.x - (-0.5547)) < 0.01f);
    REQUIRE(std::abs(vec2.y - (-0.83205)) < 0.01f);
    REQUIRE(calc::norm(vec2) == 1.0);

    auto e = cv::Point2f(2, -2);
    auto f = cv::Point2f(2, -5);
    auto vec3 = geo::connecting_vector(e, f);

    REQUIRE(vec3.x == 0.f);
    REQUIRE(vec3.y == -1.f);
    REQUIRE(calc::norm(vec3) == 1.0);

    auto vec4 = geo::connecting_vector(f, e);

    REQUIRE(vec4.x == 0.f);
    REQUIRE(vec4.y == 1.f);
    REQUIRE(calc::norm(vec4) == 1.0);
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
    float img_rotation;
    bool result;

    {
        cv::Mat im = cv::imread("../pics/code180.jpg", cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Mark_Keypoints::YES);
        std::tie(orientation, img_rotation, result) = decoder.calculate_rotation();
        // should detect 0 (raw) -180 (img rot) degrees
        REQUIRE(std::abs(img_rotation - (-180.0f)) < 0.01f);
        REQUIRE(std::abs(orientation - 0.0f) < 0.01f);

        decoder.rotate_img(units::Degrees(img_rotation));
        decoder.save_img("code180_rotated.jpg");
    }

    {
        cv::Mat im = cv::imread("../pics/code90.jpg", cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Mark_Keypoints::YES);
        std::tie(orientation, img_rotation, result) = decoder.calculate_rotation();
        // should detect 90 (raw), 90 (img rot) degrees
        REQUIRE(std::abs(orientation - 90.0f) < 0.01f);
        REQUIRE(std::abs(img_rotation - 270.0f) < 0.01f);

        decoder.rotate_img(units::Degrees(img_rotation));
        decoder.save_img("code90_rotated.jpg");
    }

    {
        cv::Mat im = cv::imread("../pics/codeneg90.jpg", cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Mark_Keypoints::YES);
        std::tie(orientation, img_rotation, result) = decoder.calculate_rotation();
        // should detect 90 (raw), 90 (img rot) degrees
        REQUIRE(std::abs(orientation - 90.0f) < 0.01f);
        REQUIRE(orientation == img_rotation);

        decoder.rotate_img(units::Degrees(img_rotation));
        decoder.save_img("codeneg90_rotated.jpg");
    }

    {
        cv::Mat im = cv::imread("../pics/codeidk.jpg", cv::IMREAD_COLOR);
        ToyDecoder decoder(im);

        decoder.calculate_keypoints(Mark_Keypoints::YES);
        std::tie(orientation, img_rotation, result) = decoder.calculate_rotation();
        // should detect -43 (raw), -223.62 (img rot) degrees
        REQUIRE(std::abs(orientation - (-43.62f)) < 0.01f);
        REQUIRE(std::abs(img_rotation - (-223.62f)) < 0.01f);

        decoder.rotate_img(units::Degrees(img_rotation));
        decoder.save_img("codeidk_rotated.jpg");
    }
}

TEST_CASE("Test decoder vallues", "[test_decoder_values]") {
    using namespace toy_decoder;
    float orientation;
    float img_rotation;
    cv::Point2i decoded_point;
    bool result;

    // this image encodes 0 (raw) degrees orientation and
    // 0101: 5
    // 1010: 10
    cv::Mat im = cv::imread("../pics/code_diff_sizes.jpg", cv::IMREAD_COLOR);
    ToyDecoder decoder(im);

    decoder.calculate_keypoints(Mark_Keypoints::YES);
    std::tie(orientation, img_rotation, result) = decoder.calculate_rotation();
    // should detect 0 (raw) -180 (img rot) degrees
    REQUIRE(std::abs(img_rotation - 180.64f) < 0.01f); // 0.64 imprecision strangely
    REQUIRE(std::abs(orientation - 0.64f) < 0.01f);

    decoder.rotate_img(units::Degrees(img_rotation));
    decoder.save_img("code_diff_sizes_rotated.jpg");

    decoder.rotate_keypoints(units::Degrees(img_rotation));

    std::tie(decoded_point, result) = decoder.decode();
    REQUIRE(decoded_point.x == 5);
    REQUIRE(decoded_point.y == 10);
}
