//
// Created by julian on 5/26/19.
//

#include <notqrcode/notqrcode_decoder.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <iterator>
#include <tuple>

using namespace notqrcode;

int main(int argc, char **argv) {
    if (argc < 2) {
        fmt::print("NO file argument!\n");
        return -1;
    }

    auto file = argv[1];
    fmt::print("OPENCV version {}.{}\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);
    fmt::print("Got file {}\n", file);

    // this image encodes 0 (raw) degrees orientation and
    // 0101: 5
    // 1010: 10
    cv::Mat im = cv::imread(file, cv::IMREAD_GRAYSCALE);
    NotQRCodeDecoder decoder(im);

    float orientation;
    cv::Point2i decoded_point;
    bool worked;

    decoder.calculate_keypoints(Draw::YES);
    std::tie(orientation, worked) = decoder.calculate_orientation(Draw::YES);
    if (!worked)
        return 1;
    decoder.open_img("before");
    decoder.rotate_img(util::units::Degrees(orientation));
    decoder.save_img("main.jpg");
    decoder.open_img("after");
    decoder.rotate_keypoints(util::units::Degrees(orientation));
    fmt::print("Rotation: {}\n", orientation);
    std::tie(decoded_point, worked) = decoder.decode();
    if (!worked)
        return 1;
    fmt::print("Decoded: ({},{}), result: {}", decoded_point.x, decoded_point.y, worked);
    return 0;
}