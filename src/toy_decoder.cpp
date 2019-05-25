//
// Created by julian on 5/25/19.
//

#include <toy_decoder/toy_decoder.hpp>
#include <fmt/format.h>
#include <exception>

using namespace toy_decoder::util;
using namespace toy_decoder;

ToyDecoder::ToyDecoder(cv::Mat &img) : img{img}, params{}, keypoints{}, orientation_line{} {
    // set simple blob detector params
    // Change thresholds
    this->params.minThreshold = 10;
    this->params.maxThreshold = 200;

    // Filter by Area.
    this->params.filterByArea = true;
    this->params.minArea = 100;
    this->params.maxArea = 1000000;

    // Filter by Circularity
    this->params.filterByCircularity = true;
    this->params.minCircularity = 0.01;

    keypoints.reserve(10);
    orientation_line.reserve(2);
}

ToyDecoder::ToyDecoder(cv::Mat &img, cv::SimpleBlobDetector::Params params) : img{img}, params{params}, keypoints{},
                                                                              orientation_line{} {
    keypoints.reserve(10);
    orientation_line.reserve(2);
}

void ToyDecoder::calculate_keypoints(Mark_Keypoints mark) {
    // Set up detector with params and detect
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
    detector->detect(this->img, this->keypoints);

    switch (mark) {
        case YES:
            cv::drawKeypoints(this->img, keypoints, this->img, cv::Scalar(0, 0, 255),
                              cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            break;
        case NO:
        default:
            break;
    }
}

std::tuple<float, float, bool> ToyDecoder::calculate_rotation() {
    if (this->keypoints.size() != 10) {
        fmt::print("Not enough keypoint found!\n");
        return std::make_tuple(0.0, 0.0, false);
    }

    // partition in bit points and orientation points
    auto bit_end_it = std::stable_partition(keypoints.begin(), keypoints.end(), [&](const auto &p) {
        return !color::is_black(this->img, p) and !color::is_red(this->img, p);
    });

    if (std::distance(bit_end_it, keypoints.end()) != 2) {
        fmt::print("Couldn't find line. Line length {}", std::distance(bit_end_it, keypoints.end()));
        return std::make_tuple(0.0, 0.0, false);
    }

    // move orientations points to new vec and delete from old
    this->orientation_line = std::vector<cv::KeyPoint>(std::make_move_iterator(bit_end_it),
                                                       std::make_move_iterator(keypoints.end()));
    keypoints.erase(bit_end_it, keypoints.end());

    // partition again: red point is now in front: position 0
    std::stable_partition(this->orientation_line.begin(), this->orientation_line.end(),
                          [&](const auto &p) { return color::is_red(this->img, p); }); // ignore iterator
    auto &red_point = this->orientation_line[0].pt;
    auto &black_point = this->orientation_line[1].pt;

    // calculate vector that is spanned by red and black point
    geo::to_cartesian(red_point);
    geo::to_cartesian(black_point);
    auto vec = geo::connecting_vector(red_point, black_point);
    // calculate absolute orientation, * -1 because of rotation in right hand rule
    float orientation = (std::atan(vec.y / vec.x) * 180.0 / M_PI) * -1;
    float img_rotation = orientation;
    // It's not enough to just turn parallel (enough for orientation estimation),
    // but the red dot has to be to the right of the black one
    // therefore a manual flip is necessary, if following condition is met
    // saved in img_rotation
    if (red_point.x < black_point.x) {
        orientation < 0 ? img_rotation -= 180.0 : img_rotation += 180;
    }

    return std::make_tuple(orientation, img_rotation, true);
}

void ToyDecoder::rotate_img(toy_decoder::util::units::Degrees degrees) {
    auto shape = cv::Point2f{static_cast<float>(this->img.cols / 2.), static_cast<float>(this->img.rows / 2.)};
    auto img_rot_matrix = cv::getRotationMatrix2D(shape, degrees.to_deg(), 1);
    cv::warpAffine(this->img, this->img, img_rot_matrix, cv::Size{this->img.cols, this->img.rows});
}

void ToyDecoder::rotate_keypoints(toy_decoder::util::units::Degrees degrees) {
    // apply rotation to keypoints
    for (auto &keypoint: keypoints) {
        geo::to_cartesian(keypoint.pt);
        calc::rotate(keypoint.pt, degrees);
    }

    // apply rotation to this->orientation_line
    for (auto &keypoint: this->orientation_line) {
        // no need to convert to cartesian, already done in calculate_rotation
        calc::rotate(keypoint.pt, degrees);
    }
}

std::tuple<int, bool> ToyDecoder::decode() {
    if ((this->orientation_line.size() + this->keypoints.size()) != 10) {
        fmt::print("Can't decode less than 10 points");
        return std::make_tuple(0, false);
    }

    // TODO
    // [x] separate with std::partition in bits to decode
    // [ ] catch erroneuos rotations
    // [ ] decode bits: color/size?!
    const auto separator_it = std::partition(this->keypoints.begin(), this->keypoints.end(), [&](const auto &p) {
        // line is rotated => any points below/above points below to either bits
        return p.pt.y > this->orientation_line[0].pt.y;
    });

    // Sort keypoints for bit reading: first half of vector will be keypoints above
    std::sort(this->keypoints.begin(), separator_it, [](const auto &a, const auto &b) { return a.pt.x < b.pt.x; });
    std::sort(separator_it, this->keypoints.end(), [](const auto &a, const auto &b) { return a.pt.x < b.pt.x; });

    // TODO decoding

    return std::make_tuple(1, true);
}

void ToyDecoder::save_img(std::string name) {
    cv::imwrite(name, this->img);
}

void ToyDecoder::open_img() {
    cv::imshow("rotated image", this->img);
    cv::waitKey(0);
}

