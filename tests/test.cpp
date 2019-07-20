#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <notqrcode/notqrcode_decoder.hpp>
#include <notqrcode/util.hpp>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>

using namespace notqrcode::util;

TEST_CASE("Test rotation/rotation matrix", "[test_rotation]") {
    auto in = cv::Point2f(1, 0);
    calc::rotate(in, units::Degrees(90.0));
    REQUIRE(in.x == Approx(0.f).margin(0.0001));
    REQUIRE(in.y == Approx(1.f).margin(0.0001));

    auto in2 = cv::Point2f(1, 0);
    calc::rotate(in2, units::Degrees(-90.0));
    REQUIRE(in2.x == Approx(0.f).margin(0.0001));
    REQUIRE(in2.y == Approx(-1.f).margin(0.0001));
}

TEST_CASE("Test vector between two points", "[test_connecting_vector]") {
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
    using namespace notqrcode;

    {
        std::string file = "rect_bw_16_45_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~45, in this case 43.94
        REQUIRE(orientation.val == Approx(45).epsilon(0.03));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.save_img(file + "_rotated.jpg");
    }

    {
        std::string file = "rect_bw_16_90_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~90, in this case 89.25
        REQUIRE(orientation.val == Approx(90.0f).epsilon(0.01));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.save_img(file + "_rotated.jpg");
    }

    {
        std::string file = "rect_bw_16_neg45_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~-45, in this case -44.47
        REQUIRE(orientation.val == Approx(-45).epsilon(0.015));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.save_img(file + "_rotated.jpg");
    }

    {
        std::string file = "rect_bw_16_neg135_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~-135, in this case -135.5
        REQUIRE(orientation.val == Approx(-135.f).epsilon(0.005));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.save_img(file + "_rotated.jpg");
    }
}

TEST_CASE("Test decoder values", "[test_decoder_values]") {
    using namespace notqrcode;

    {
        // this image encodes -135 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_16_neg135_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~-135, in this case -135.5
        REQUIRE(orientation.val == Approx(-135.f).epsilon(0.005));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.rotate_keypoints(units::Degrees(orientation.val));

        auto decoded_point = decoder.decode();
        REQUIRE(decoded_point.val.x == 165);
        REQUIRE(decoded_point.val.y == 165);
    }

    {
        // this image encodes -45 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_16_neg45_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~-45, in this case -44.47
        REQUIRE(orientation.val == Approx(-45.0f).epsilon(0.015));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.rotate_keypoints(units::Degrees(orientation.val));

        auto decoded_point = decoder.decode();
        REQUIRE(decoded_point.val.x == 165);
        REQUIRE(decoded_point.val.y == 165);
    }

    {
        // this image encodes 90 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_16_90_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~90, in this case 89.25
        REQUIRE(orientation.val == Approx(90.f).epsilon(0.01));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.rotate_keypoints(units::Degrees(orientation.val));

        auto decoded_point = decoder.decode();
        REQUIRE(decoded_point.val.x == 165);
        REQUIRE(decoded_point.val.y == 165);
    }

    {
        // this image encodes 45 degrees orientation and
        // 1010: 10
        // 0101: 5
        std::string file = "rect_bw_16_45_xlarge";
        auto decoder = NotQRCodeDecoder::file("../tests/pics/" + file + ".jpg");

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(YES);
        // should detect ~45, in this case 43.94
        REQUIRE(orientation.val == Approx(45.f).epsilon(0.03));

        decoder.rotate_img(units::Degrees(orientation.val));
        decoder.rotate_keypoints(units::Degrees(orientation.val));

        auto decoded_point = decoder.decode();
        REQUIRE(decoded_point.val.x == 165);
        REQUIRE(decoded_point.val.y == 165);
    }
}

TEST_CASE("Various", "[various]") {
    using namespace notqrcode::util::units;
    units::Degrees x = 90.0_deg;
    REQUIRE(x.to_rad() == Approx(PIf/2.0).margin(0.0001));
    REQUIRE(x.to_deg() == Approx(90.0f).margin(0.0001));
}
