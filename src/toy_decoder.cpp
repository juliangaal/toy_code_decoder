//
// Created by julian on 5/25/19.
//

#include <toy_decoder/toy_decoder.hpp>
#include <fmt/format.h>
#include <exception>
#include <numeric>
#include <algorithm>

using namespace toy_decoder;

ToyDecoder::ToyDecoder(cv::Mat &img) : img{img}, params{}, keypoints{}, orientation_point{}, avg_size{} {
    // set simple blob detector params
    // Change thresholds
    this->params.minThreshold = 10;
    this->params.maxThreshold = 200;

    // Filter by Area.
    this->params.filterByArea = false;

    // Filter by Circularity
    this->params.filterByCircularity = false;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;

    keypoints.reserve(10);
}

ToyDecoder::ToyDecoder(cv::Mat &img, cv::SimpleBlobDetector::Params params) : img{img}, params{params}, keypoints{},
                                                                              orientation_point{}, avg_size{} {
    keypoints.reserve(10);
}

// based on simple blob detector
void ToyDecoder::calculate_keypoints(Mark_Keypoints mark) {
    // Set up detector with params and detect
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
    detector->detect(this->img, this->keypoints);

    switch (mark) {
        case YES:
            for (const auto& point: keypoints) {
                cv::circle(this->img, point.pt, 3, cv::Scalar(255, 255, 255), -1);
            }
            break;
        case NO:
        default:
            break;
    }
}

std::tuple<float, bool> ToyDecoder::calculate_orientation() {
    using namespace toy_decoder::util;

    // partition in bit points and orientation points
    std::sort(keypoints.begin(), keypoints.end(), [&](const auto& p1, const auto& p2) {
        return p1.size < p2.size;
    });

    // orientation point is now last in vector, will be deleted after
    this->orientation_point = keypoints.back();
    keypoints.pop_back();

    cv::Point2f centroid;
    for (const auto& kp: keypoints) {
        centroid.x += kp.pt.x;
        centroid.y += kp.pt.y;
    }

    centroid.x /= keypoints.size();
    centroid.y /= keypoints.size();

    // calculate vector that is spanned by orientation point and centroid of all other points
    geo::to_cartesian(orientation_point.pt);
    geo::to_cartesian(centroid);
    auto vec = geo::connecting_vector(centroid, orientation_point.pt);
    // calculate absolute orientation, * -1 because of rotation in right hand rule
    if (vec.x == 0) vec.x += 0.0000001;
    float orientation = std::atan2(vec.y, vec.x) * (180.0 / M_PI) * -1;

    return std::make_tuple(orientation, true);
}

void ToyDecoder::rotate_img(toy_decoder::util::units::Degrees degrees) {
    auto shape = cv::Point2f{static_cast<float>(this->img.cols / 2.), static_cast<float>(this->img.rows / 2.)};
    auto img_rot_matrix = cv::getRotationMatrix2D(shape, degrees.to_deg(), 1);
    cv::warpAffine(this->img, this->img, img_rot_matrix, cv::Size{this->img.cols, this->img.rows});
}

void ToyDecoder::rotate_keypoints(toy_decoder::util::units::Degrees degrees) {
    using namespace toy_decoder::util;
    // apply rotation to keypoints
    // use opportunity to calculate average blob size in keypoints, which are encoding bits at this stage
    for (auto &keypoint: keypoints) {
        geo::to_cartesian(keypoint.pt);
        calc::rotate(keypoint.pt, degrees);
        this->avg_size += keypoint.size;
    }

    // produces mean blob size
    this->avg_size /= keypoints.size();

    calc::rotate(orientation_point.pt, degrees);
}

std::tuple<cv::Point2i, bool> ToyDecoder::decode() {
    // TODO
    // [x] separate with std::partition in bits to decode
    // [ ] catch erroneuos rotations
    // [x] decode bits: size differences
    const auto separator_it = std::partition(this->keypoints.begin(), this->keypoints.end(), [&](const auto &p) {
        // line is rotated => any points below/above points below to either bits
        return p.pt.y > this->orientation_point.pt.y;
    });

    // Sort keypoints for bit reading: first half of vector will be keypoints above
    std::sort(this->keypoints.begin(), separator_it, [](const auto &a, const auto &b) { return a.pt.x < b.pt.x; });
    std::sort(separator_it, this->keypoints.end(), [](const auto &a, const auto &b) { return a.pt.x < b.pt.x; });

    cv::Point2i p{0, 0};
    p.x = util::decode(this->keypoints.cbegin(), separator_it, avg_size);
    p.y = util::decode(separator_it, this->keypoints.cend(), avg_size);

    return std::make_tuple(p, true);
}

void ToyDecoder::save_img(std::string name) {
    cv::imwrite(name, this->img);
}

void ToyDecoder::open_img(std::string name) {
    cv::imshow(name, this->img);
    cv::waitKey(0);
}

