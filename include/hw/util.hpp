//
// Created by julian on 5/24/19.
//

#ifndef HWCPP_UTIL_HPP
#define HWCPP_UTIL_HPP

#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

namespace hw {
namespace util {

size_t decode(std::vector<cv::KeyPoint>::const_iterator begin,
              std::vector<cv::KeyPoint>::const_iterator end) {
    size_t val = 0;
    for (auto it = begin; it != end; it++) {
        size_t exp = std::distance(it, end) - 1;
        val += std::pow(2, exp);
    }
    return val;
}

namespace units {

class Degrees {
public:
    explicit constexpr Degrees(float amount) : amount(amount) {}

    constexpr Degrees() = delete;

    ~Degrees() = default;

    constexpr float to_rad() {
        return this->amount * M_PI / 180.0;
    }

private:
    float amount;
};

} // namespace hw::util::units

namespace color {

enum Color {
    RED,
    GREEN,
    BLUE,
    BLACK,
};

const static cv::Vec3b RGBRED = cv::Vec3b{255, 0, 0};
const static cv::Vec3b RGBGREEN = cv::Vec3b{0, 255, 0};
const static cv::Vec3b RGBBLUE = cv::Vec3b{0, 0, 255};
const static cv::Vec3b RGBBLACK = cv::Vec3b{0, 0, 0};

bool same_color(const cv::Mat &im, const cv::KeyPoint &point, Color color, float tolerance = 40.0) {
    // opencv idiots thought it was a good idea to return color in BGR
    auto true_color = im.at<cv::Vec3b>(point.pt);
    switch (color) {
        case RED:
            if (std::abs(RGBRED[0] - true_color.val[2]) < tolerance &&
                std::abs(RGBRED[1] - true_color.val[1]) < tolerance &&
                std::abs(RGBRED[2] - true_color.val[0]) < tolerance)
                return true;
            break;
        case GREEN:
            if (std::abs(RGBGREEN[1] - true_color.val[1]) < tolerance) return true;
            break;
        case BLUE:
            if (std::abs(RGBBLUE[2] - true_color.val[0]) < tolerance) return true;
            break;
        case BLACK:
            if (std::abs(RGBBLACK[0] - true_color.val[2]) < tolerance &&
                std::abs(RGBBLACK[1] - true_color.val[1]) < tolerance &&
                std::abs(RGBBLACK[2] - true_color.val[0]) < tolerance)
                return true;
            break;
        default:
            break;
    }

    return false;
}

bool is_black(const cv::Mat &im, const cv::KeyPoint &point, float tolerance = 40.0) {
    return same_color(im, point, Color::BLACK, tolerance);
}

bool is_red(const cv::Mat &im, const cv::KeyPoint &point, float tolerance = 40.0) {
    return same_color(im, point, Color::RED, tolerance);
}

} // namespace hw::util::color

namespace geo {

using UnitVector = cv::Point2f;

geo::UnitVector connecting_vector(const cv::Point2f &a, const cv::Point2f &b) {
    float x = std::abs(a.x - b.x);
    float y = std::abs(a.y - b.y);
    float length = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
    return geo::UnitVector{x / length, y / length};
}

// see https://stackoverflow.com/questions/25642532/opencv-pointx-y-represent-column-row-or-row-column
void to_cartesian(cv::Point2f &point) {
    point.y *= -1;
}

} // namespace hw::util::geo

namespace calc {

void rotate(cv::Point2f &point, units::Degrees degrees) {
    float angle = degrees.to_rad();
    auto x = point.x; // original x is saved for later calculation
    point.x = point.x * std::cos(angle) - point.y * std::sin(angle);
    point.y = x * std::sin(angle) + point.y * std::cos(angle);
}

float norm(const cv::Point2f &point) {
    return std::sqrt(std::pow(point.x, 2) + std::pow(point.y, 2));
}

} // namespace hw::util::calc
} // namespace hw::util
} // namespace hw

#endif //HWCPP_UTIL_HPP
