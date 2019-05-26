//
// Created by julian on 5/26/19.
//

#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <toy_decoder/util.hpp>
#include <algorithm>
#include <iterator>
#include <toy_decoder/toy_decoder.hpp>
#include <tuple>

using namespace toy_decoder;
namespace fs = boost::filesystem;

int main(int argc, char **argv) {
    if (argc < 2) {
        fmt::print("NO file argument!\n");
        return -1;
    }

    if (!fs::exists(argv[1])) {
        fmt::print("File not found\n");
        return -1;
    }

    auto file = argv[1];
    fmt::print("OPENCV version {}.{}\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);
    fmt::print("Got file {}\n", file);

    // this image encodes 0 (raw) degrees orientation and
    // 0101: 5
    // 1010: 10
    cv::Mat im = cv::imread(file, cv::IMREAD_GRAYSCALE);
    ToyDecoder decoder(im);

    float orientation;
    float img_rotation;
    cv::Point2i decoded_point;
    bool result;

    decoder.calculate_keypoints(Mark_Keypoints::YES);
    std::tie(orientation, img_rotation, result) = decoder.calculate_rotation();
    decoder.open_img("before");
    decoder.rotate_img(util::units::Degrees(img_rotation));
    decoder.save_img("main.jpg");
    decoder.open_img("after");
    decoder.rotate_keypoints(util::units::Degrees(img_rotation));
    fmt::print("Rotation: {}\n", orientation);
    std::tie(decoded_point, result) = decoder.decode();

    return 0;
}