//
// Created by julian on 5/25/19.
//

#include <notqrcode/notqrcode_decoder.hpp>
#include <fmt/format.h>
#include <exception>
#include <numeric>
#include <algorithm>

using namespace notqrcode;

NotQRCodeDecoder::NotQRCodeDecoder(cv::Mat &img) : _img{img}, _params{}, _keypoints{}, _orientation_point{}, _avg_size{} {
    if (img.empty())
        throw std::runtime_error("opencv image empty!");

    // set simple blob detector params
    // Change thresholds
    _params.minThreshold = 10;
    _params.maxThreshold = 200;

    // Filter by Area.
    _params.filterByArea = false;

    // Filter by Circularity
    _params.filterByCircularity = false;

    // Filter by Convexity
    _params.filterByConvexity = true;
    _params.minConvexity = 0.87;

    // Filter by Inertia
    _params.filterByInertia = true;
    _params.minInertiaRatio = 0.01;

    _keypoints.reserve(10);
}

NotQRCodeDecoder::NotQRCodeDecoder(cv::Mat &img, cv::SimpleBlobDetector::Params params) : _img{img}, _params{params}, _keypoints{},
                                                                              _orientation_point{}, _avg_size{} {
    if (img.empty())
        throw std::runtime_error("opencv image empty!");

    _keypoints.reserve(10);
}

void NotQRCodeDecoder::calculate_keypoints(Draw draw) {
    // Set up detector with params and detect
    auto detector = cv::SimpleBlobDetector::create(_params);
    detector->detect(_img, _keypoints);

    switch (draw) {
        case YES:
            for (const auto &point: _keypoints) {
                cv::circle(_img, point.pt, 2, cv::Scalar(255, 255, 255), -1);
            }
            break;
        case NO:
        default:
            break;
    }
}

std::tuple<float, bool> NotQRCodeDecoder::calculate_orientation(Draw draw) {
    using namespace notqrcode::util;

    if (_keypoints.size() != 17) {
        fmt::print("Didn't get 17 keypoints: {}\n", _keypoints.size());
        return std::make_tuple(0.0f, false);
    }

    // partition in bit points and orientation points
    std::sort(_keypoints.begin(), _keypoints.end(), [&](const auto &p1, const auto &p2) {
        return p1.size < p2.size;
    });

    // orientation point is now last in vector, will be deleted after
    _orientation_point = _keypoints.back();
    _keypoints.pop_back();

    cv::Point2f centroid;
    for (const auto &kp: _keypoints) {
        centroid.x += kp.pt.x;
        centroid.y += kp.pt.y;
    }

    centroid.x /= _keypoints.size();
    centroid.y /= _keypoints.size();

    // calculate vector that is spanned by orientation point and centroid of all other points
    geo::to_cartesian(_orientation_point.pt);
    geo::to_cartesian(centroid);
    auto vec = geo::connecting_vector(centroid, _orientation_point.pt);
    // calculate absolute orientation, * -1 because of rotation in right hand rule
    if (vec.x == 0) vec.x += 0.0000001;
    float orientation = std::atan2(vec.y, vec.x) * (180.0f / PIf) * -1;

    switch (draw) {
        case YES: {
            // transformation back into opencv coordinates, that's why they are so many negative numbers here
            // -vec.y, -orientation_point.pt.y, -centroid.y ...
            // draws line across screen
            float slope = -vec.y / vec.x;
            cv::Point2f p{};
            cv::Point2f q(_img.cols, _img.rows);
            p.y = -(_orientation_point.pt.x - p.x) * slope + -_orientation_point.pt.y;
            q.y = -(centroid.x - q.x) * slope + -centroid.y;
            cv::line(_img, p, q, cv::Scalar(128, 128, 128), 2, 8, 0);
            break;
        }
        case NO:
        default:
            break;
    }

    return std::make_tuple(orientation, true);
}

void NotQRCodeDecoder::rotate_img(notqrcode::util::units::Degrees degrees) {
    auto shape = cv::Point2f{static_cast<float>(_img.cols / 2.), static_cast<float>(_img.rows / 2.)};
    auto img_rot_matrix = cv::getRotationMatrix2D(shape, degrees.to_deg(), 1);
    cv::warpAffine(_img, _img, img_rot_matrix, cv::Size{_img.cols, _img.rows});
}

void NotQRCodeDecoder::rotate_keypoints(notqrcode::util::units::Degrees degrees) {
    using namespace notqrcode::util;
    // apply rotation to keypoints
    // use opportunity to calculate average blob size in keypoints, which are encoding bits at this stage
    for (auto &keypoint: _keypoints) {
        geo::to_cartesian(keypoint.pt);
        calc::rotate(keypoint.pt, degrees);
        _avg_size += keypoint.size;
    }

    // produces mean blob size
    _avg_size /= _keypoints.size();

    calc::rotate(_orientation_point.pt, degrees);
}

std::tuple<cv::Point2i, bool> NotQRCodeDecoder::decode() {
    if (_keypoints.size() != 16) {
        fmt::print("Invalid number of keypoints detected: {}", _keypoints.size());
        return std::make_tuple(cv::Point2i{}, false);
    }

    // line is rotated => any points below/above points below to either bits
    // we are decoding 16 bits, 8 per "points over orientation line"
    // this separator is the first right down the horizontal line
    const auto h_separator_it = util::partition_by_height(_keypoints.begin(), _keypoints.end(), _orientation_point.pt.y);
    // if separator fails for any reason, return
    if (h_separator_it == _keypoints.end()) {
        fmt::print("Can't separate keypoints\n");
        return std::make_tuple(cv::Point2i{}, false);
    }

    // centroid of each x bits and y bits, needed for next separation height
    cv::Point2f x_centroid{};
    cv::Point2f y_centroid{};

    std::for_each(_keypoints.begin(), h_separator_it, [&](const auto& p){
        x_centroid.x += p.pt.x;
        x_centroid.y += p.pt.y;
    });

    float x_bits_num = std::distance(_keypoints.begin(), h_separator_it);
    x_centroid.x /= x_bits_num;
    x_centroid.y /= x_bits_num;

    std::for_each(h_separator_it, _keypoints.end(), [&](const auto& p){
        y_centroid.x += p.pt.x;
        y_centroid.y += p.pt.y;
    });

    float y_bits_num = std::distance(_keypoints.begin(), h_separator_it);
    y_centroid.x /= y_bits_num;
    y_centroid.y /= y_bits_num;

    // this separator separates the 2 levels of bits above horizontal line, so the x bits
    const auto x_separator_it = util::partition_by_height(_keypoints.begin(), h_separator_it, x_centroid.y);
    // if separator fails for any reason, return
    if (x_separator_it == _keypoints.end()) {
        fmt::print("Can't separate keypoints x\n");
        return std::make_tuple(cv::Point2i{}, false);
    }

    // this separator separates the 2 levels of bits below horizontal line, so the y bits
    const auto y_separator_it = util::partition_by_height(h_separator_it, _keypoints.end(), y_centroid.y);
    // if separator fails for any reason, return
    if (y_separator_it == _keypoints.end()) {
        fmt::print("Can't separate keypoints y\n");
        return std::make_tuple(cv::Point2i{}, false);
    }

    // lambda helper function
    auto point_further_left = [](const auto &a, const auto &b) { return a.pt.x < b.pt.x; };

    // Sort keypoints for x bits from left to right, left being the smallest after sorting
    std::sort(_keypoints.begin(), x_separator_it, point_further_left);
    std::sort(x_separator_it, h_separator_it, point_further_left);

    // Sort keypoints for y bits from left to right, left being the smallest after sorting
    std::sort(h_separator_it, y_separator_it, point_further_left);
    std::sort(y_separator_it, _keypoints.end(), point_further_left);

    cv::Point2i p{util::decode(_keypoints.cbegin(), h_separator_it, _avg_size),
                  util::decode(h_separator_it, _keypoints.cend(), _avg_size)};

    return std::make_tuple(p, true);
}

void NotQRCodeDecoder::save_img(std::string name) {
    cv::imwrite(name, _img);
}

void NotQRCodeDecoder::open_img(std::string name) {
    cv::imshow(name, _img);
    cv::waitKey(0);
}
