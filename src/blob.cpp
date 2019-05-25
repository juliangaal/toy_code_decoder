#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <hw/util.hpp>
#include <vector>
#include <cmath>

using namespace hw::util;
namespace fs = boost::filesystem;

int main(int argc, char **argv) {
    if (argc < 2) {
        fmt::print("No file argument!\n");
        return -1;
    }

    if (!fs::exists(argv[1])) {
        fmt::print("File not found\n");
        return -1;
    }

    auto file = argv[1];
    fmt::print("OPENCV version {}.{}\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);
    fmt::print("Got file {}\n", file);

    // Read image
    cv::Mat im = cv::imread(file, cv::IMREAD_COLOR);

    cv::SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 200;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 1000000;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.01;

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

    // Print keypoint information
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
    auto vec = geo::connecting_vector(keypoints[0].pt, keypoints[1].pt);
    float rotation = std::atan(vec.y / vec.x) * 180.0 / M_PI;
    // TODO adjust rotation in relation to north/south dot
    if (keypoints[1].pt.y < keypoints[0].pt.y) rotation *= -1;

    // apply rotation to image
    auto shape = cv::Point2f{static_cast<float>(im.cols/2.), static_cast<float>(im.rows/2.)};
    auto img_rot_matrix = cv::getRotationMatrix2D(shape, rotation, 1);
    cv::warpAffine(im, im, img_rot_matrix, cv::Size{im.cols, im.rows});

    // TODO
    // [x] apply rotating to keypoints
    // [ ] check with matplotlib
    for (auto& keypoint: keypoints) {
        geo::to_cartesian(keypoint.pt);
        fmt::print("> rotated ({0:0.1f},{1:0.1f})", keypoint.pt.x, keypoint.pt.y);
        calc::rotate(keypoint.pt, units::Degrees(rotation));
        fmt::print(" to ({0:0.1f},{1:0.1f})\n", keypoint.pt.x, keypoint.pt.y);
    }

    // save rotated image
    cv::imwrite("rotated.jpg", im);
    fmt::print("Rotated output image ({} degrees) saved to {}\n", rotation, "rotated.jpg");

    // TODO
    // [ ] separate with std::partition in bits to decode
    // [ ] decode bits
    
    // Show blobs
    // cv::imshow("keypoints", im_with_keypoints);
    // cv::waitKey(0);
}

