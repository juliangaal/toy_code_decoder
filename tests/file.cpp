//
// Created by julian on 5/26/19.
//
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <notqrcode/notqrcode_decoder.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <cassert>

using namespace notqrcode;
using notqrcode::util::units::Degrees;

TEST_CASE("Integration Test") {
    // blob detection parameters SPECIFICALLY for files in ../tests/pics
    // Change thresholds
    cv::SimpleBlobDetector::Params blob_params{};
    blob_params.minThreshold = 10;
    blob_params.maxThreshold = 200;

    // Filter by Area.
    blob_params.filterByArea = false;

    // Filter by Circularity
    blob_params.filterByCircularity = false;

    // Filter by Convexity
    blob_params.filterByConvexity = true;
    blob_params.minConvexity = 0.87;

    // Filter by Inertia
    blob_params.filterByInertia = true;
    blob_params.minInertiaRatio = 0.01;

    notqrcode::ImgProcessingParams img_params{};
    img_params.gaussian_size = 5;
    img_params.threshold = 245;
    img_params.threshold_repl_value = 255;
    // these margins are pretty large! Because codes were generated by hand, the precision was pretty bad
    // so high values are required
    img_params.centroid_dist_margin = 8.f;
    img_params.orientation_pt_dist_margin = 8.f;

    {
        std::string file = "../tests/pics/rect_bw_16_neg90_xlarge.jpg";
        auto decoder = NotQRCodeDecoder::file_with_params(file, img_params, blob_params);
        decoder.calculate_keypoints(Draw::YES);
        decoder.save_img("neg_90_test.jpg");

        auto orientation = decoder.calculate_orientation(Draw::YES);
        if (orientation.error != Error::None)
            throw std::runtime_error(fmt::format("Orientation calculation Error: {}", orientation.error));

        REQUIRE(orientation.val == Approx(-90.0f).epsilon(0.01));

        decoder.rotate_keypoints(Degrees(orientation.val));
        auto decoded_point = decoder.decode();
        if (orientation.error != Error::None)
            throw std::runtime_error(fmt::format("Decoder Error: {}", orientation.error));

        REQUIRE(decoded_point.val.x == 165);
        REQUIRE(decoded_point.val.y == 165);
    }

    {
        std::string file = "../tests/pics/rect_bw_16_90_xlarge.jpg";
        auto decoder = NotQRCodeDecoder::file_with_params(file, img_params, blob_params);
        decoder.calculate_keypoints(Draw::YES);
        decoder.save_img("90_test.jpg");

        auto orientation = decoder.calculate_orientation(Draw::YES);
        if (orientation.error != Error::None)
            throw std::runtime_error(fmt::format("Orientation calculation Error: got {} with {}", orientation.val, orientation.error));

        REQUIRE(orientation.val == Approx(90.0f).epsilon(0.01));

        decoder.rotate_keypoints(Degrees(orientation.val));
        auto decoded_point = decoder.decode();
        if (orientation.error != Error::None)
            throw std::runtime_error(fmt::format("Decoder Error: {}", orientation.error));

        REQUIRE(decoded_point.val.x == 165);
        REQUIRE(decoded_point.val.y == 165);
    }
}