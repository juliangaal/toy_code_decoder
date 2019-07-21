//
// Created by julian on 5/26/19.
//
#include <notqrcode/notqrcode_decoder.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>

using namespace notqrcode;
using notqrcode::util::units::Degrees;

int main() {
    cv::Mat img = cv::imread("../tests/pics/rect_bw_16_90_xlarge.jpg", cv::IMREAD_GRAYSCALE);
    // default params, for custom use NotQRCodeDecoder::img_with_params
    auto decoder = NotQRCodeDecoder::img(img);
    decoder.calculate_keypoints(Draw::YES);

    auto orientation = decoder.calculate_orientation();
    if (orientation.error != Error::None) {
        fmt::print("Error: {}", orientation.error);
        return -1;
    }

    decoder.rotate_keypoints(util::units::Degrees(orientation.val));
    auto decoded_point = decoder.decode();
    if (decoded_point.error != Error::None) {
        fmt::print("Error: {}", decoded_point.error);
        return -1;
    }

    fmt::print("Rotation: {:.3f}° - Decoded: ({},{})\n", orientation.val, decoded_point.val.x, decoded_point.val.y);
    return 0;
}