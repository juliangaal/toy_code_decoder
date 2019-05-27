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
    cv::Point2i decoded_point;
    bool result;

    decoder.calculate_keypoints(Mark_Keypoints::YES);
    std::tie(orientation, result) = decoder.calculate_orientation();
    decoder.open_img("before");
    decoder.rotate_img(util::units::Degrees(orientation));
    decoder.save_img("main.jpg");
    decoder.open_img("after");
    decoder.rotate_keypoints(util::units::Degrees(orientation));
    fmt::print("Rotation: {}\n", orientation);
    std::tie(decoded_point, result) = decoder.decode();

    // Experiment, hough transform
    // Read the image as gray-scale
    cv::Mat img = imread("../pics/rect_bw_90.jpg", cv::COLOR_GRAY2BGR);
//// Convert to gray-scale
//    cv::cvtColor(img);
// Store the edges
    cv::Mat edges;
// Find the edges in the image using canny detector
    Canny(img, edges, 50, 200);
// Create a vector to store lines of the image
    std::vector<cv::Vec4i> lines;
// Apply Hough Transform
    cv::HoughLinesP(edges, lines, 1, CV_PI/180, 100, 50, 10);
// Draw lines on the image
    for (size_t i=0; i<lines.size(); i++) {
        cv::Vec4i l = lines[i];
        line(img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 3, cv::LINE_AA);
    }
// Show result image
    imshow("Result Image", img);
    cv::waitKey(0);
    return 0;
}