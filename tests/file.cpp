//
// Created by julian on 5/26/19.
//

#include <notqrcode/notqrcode_decoder.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>

using namespace notqrcode;

int main(int argc, char **argv) {
    if (argc < 2) {
        fmt::print("NO file argument!\n");
        return -1;
    }

    auto file = argv[1];
    fmt::print("OPENCV version {}.{}\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);
    fmt::print("Got file {}\n", file);

    auto decoder = NotQRCodeDecoder::img(file, cv::IMREAD_GRAYSCALE);
    
    decoder.calculate_keypoints(Draw::YES);
    auto orientation = decoder.calculate_orientation(Draw::YES);
    if (orientation.error != Error::None)
        return 1;
    
    decoder.rotate_keypoints(util::units::Degrees(orientation.val));
    auto decoded_point = decoder.decode();
    if (decoded_point.error != Error::None)
        return 1;

    fmt::print("Rotation: {}\n", orientation.val);
    fmt::print("Decoded: ({},{})\n", decoded_point.val.x, decoded_point.val.y);

    return 0;
}