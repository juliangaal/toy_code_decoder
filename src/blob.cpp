#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <fmt/color.h>
#include <toy_decoder/util.hpp>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <cmath>

using namespace toy_decoder::util;
using namespace fmt::literals;
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

    fmt::print("Attempting to find blobs\n");

    cv::Mat im = cv::imread(file, cv::IMREAD_GRAYSCALE);
    cv::SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 200;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 1000000;

    // Filter by Circularity
    params.filterByCircularity = false;

    // Storage for blobs
    std::vector<cv::KeyPoint> keypoints;

#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2
    // Set up detector with params and detect blobs
    cv::SimpleBlobDetector detector(params);
    detector.detect(im, keypoints);
#else
    // Set up detector with params and detect
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
    detector->detect(im, keypoints);
#endif

    if (keypoints.size() != 9) {
        fmt::print("Not enough keypoint found, exiting.\n");
        return -1;
    }

    // Print keypoint information and CONVERT TO CARTESIAN
    for (auto &point: keypoints) {
        auto color = im.at<cv::Vec3b>(point.pt);
        fmt::print("> keypoint\n  - pos  : x: {0:0.1f} y: {1:0.1f}\n", point.pt.x, point.pt.y);
        fmt::print("  - size : {}\n  - color: ({},{},{})\n", point.size, color.val[2], color.val[1], color.val[0]);
    }

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
    // the size of the circle corresponds to the size of blob
    cv::Mat im_with_keypoints;
    cv::drawKeypoints(im, keypoints, im_with_keypoints, cv::Scalar(0, 0, 255),
                      cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::imwrite("result.jpg", im_with_keypoints);
    fmt::print("Output image saved to {}\n\n", "result.jpg");

    // Rotate image
    // calculate rotation
//    fmt::print("Attempting to rotate iage and points\n");
//    auto bit_end_it = std::stable_partition(keypoints.begin(), keypoints.end(), [&](const auto &p) {
//        return !color::is_black(im, p) and !color::is_red(im, p);
//    });
//
//
//    if (std::distance(bit_end_it, keypoints.end()) != 2) {
//        fmt::print("Couldn't find line. Line length {}", std::distance(bit_end_it, keypoints.end()));
//        return -1;
//    }
//
//    std::vector<cv::KeyPoint> line(std::make_move_iterator(bit_end_it), std::make_move_iterator(keypoints.end()));
//    keypoints.erase(bit_end_it, keypoints.end());
//
//    // red point is now in front: position 0
//    std::stable_partition(line.begin(), line.end(), [&](const auto &p) { return color::is_red(im, p); }); // ignore iterator
//    geo::to_cartesian(line[0].pt);
//    geo::to_cartesian(line[1].pt);
//    const auto &red_point = line[0].pt;
//    fmt::print("Found red point ({0:0.1f},{1:0.1f})\n", red_point.x, red_point.y);
//    const auto &black_point = line[1].pt;
//    fmt::print("Found black point ({0:0.1f},{1:0.1f})\n", black_point.x, black_point.y);
//
//    auto vec = geo::connecting_vector(red_point, black_point);
//    float rotation = std::atan(vec.y / vec.x) * 180.0 / M_PI;
//    rotation *= -1; // right hand rule: turn to left is negative, turn to right is positive
//    // It's not enough to just turn prallel, the red dot has to be to the right of the black one
//    // therefore a manual flip is necessary, if following condition is met
//    fmt::print("raw rotation {}\n", rotation);
//    if (red_point.x < black_point.x) {
//        rotation < 0 ? rotation -= 180.0 : rotation += 180;
//    }
//
//    // TODO adjust rotation in relation to north/south dot
//    // TODO edge case parallel
//
//    // apply rotation to image
//    auto shape = cv::Point2f{static_cast<float>(im.cols / 2.), static_cast<float>(im.rows / 2.)};
//    auto img_rot_matrix = cv::getRotationMatrix2D(shape, rotation, 1);
//    cv::warpAffine(im, im, img_rot_matrix, cv::Size{im.cols, im.rows});
//
//    // apply rotation to keypoints
//    for (auto &keypoint: keypoints) {
//        geo::to_cartesian(keypoint.pt);
//        fmt::print("> rotated ({0:0.1f},{1:0.1f})", keypoint.pt.x, keypoint.pt.y);
//        calc::rotate(keypoint.pt, units::Degrees(rotation));
//        fmt::print(" to ({0:0.1f},{1:0.1f})\n", keypoint.pt.x, keypoint.pt.y);
//    }
//
//    // apply rotation to line
//    for (auto &keypoint: line) {
//        fmt::print("> rotated ({0:0.1f},{1:0.1f})", keypoint.pt.x, keypoint.pt.y);
//        calc::rotate(keypoint.pt, units::Degrees(rotation));
//        fmt::print(" to ({0:0.1f},{1:0.1f})\n", keypoint.pt.x, keypoint.pt.y);
//    }
//
//    // TODO
//    // [x] separate with std::partition in bits to decode
//    // [ ] catch erroneuos rotations
//    // [ ] decode bits: color/size?!
//    const auto separator_it = std::partition(keypoints.begin(), keypoints.end(), [&](const auto &p) {
//        // line is rotated => any points below/above points below to either bits
//        return p.pt.y > line[0].pt.y;
//    });
//
//    // Sort keypoints for bit reading: the higher x, the more significant the bit is
//    std::sort(keypoints.begin(), separator_it, [](const auto &a, const auto &b) { return a.pt.x < b.pt.x; });
//    std::sort(separator_it, keypoints.end(), [](const auto &a, const auto &b) { return a.pt.x < b.pt.x; });
//    fmt::print("Sorted points according to rotation and location:\nUpper points:\n");
//    for (auto it = keypoints.begin(); it != separator_it; it++) {
//        fmt::print("  - ({0:0.1f},{1:0.1f})\n", (*it).pt.x, (*it).pt.y);
//    }
//
//    fmt::print("Lower points:\n");
//    for (auto it = separator_it; it != keypoints.end(); it++) {
//        fmt::print("  - ({0:0.1f},{1:0.1f})\n", (*it).pt.x, (*it).pt.y);
//    }
//
//    // save rotated image
//    cv::imwrite("rotated.jpg", im);
//    fmt::print("Rotated output image ({} degrees) saved to {}\n", rotation, "rotated.jpg");

    // Show blobs
    cv::imshow("keypoints", im_with_keypoints);
    // Show rotated image
    cv::imshow("rotated image", im);
    cv::waitKey(0);

    return 0;
}

