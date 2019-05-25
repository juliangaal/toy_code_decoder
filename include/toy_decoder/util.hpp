//
// Created by julian on 5/24/19.
//

#ifndef HWCPP_UTIL_HPP
#define HWCPP_UTIL_HPP

#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

namespace toy_decoder {
namespace util {

size_t decode(std::vector<cv::KeyPoint>::const_iterator begin, std::vector<cv::KeyPoint>::const_iterator end);

namespace units {

class Degrees {
public:
    inline explicit constexpr Degrees(float amount) : amount(amount) {}

    constexpr Degrees() = delete;

    ~Degrees() = default;

    inline constexpr float to_rad() {
        return this->amount * M_PI / 180.0;
    }

    inline constexpr float to_deg() {
        return this->amount;
    }

private:
    float amount;
};

} // namespace toy_decoder::util::units

namespace color {

const static cv::Vec3b RGBRED = cv::Vec3b{255, 0, 0};
const static cv::Vec3b RGBGREEN = cv::Vec3b{0, 255, 0};
const static cv::Vec3b RGBBLUE = cv::Vec3b{0, 0, 255};
const static cv::Vec3b RGBBLACK = cv::Vec3b{0, 0, 0};

enum Color {
    RED,
    GREEN,
    BLUE,
    BLACK,
};

bool same_color(const cv::Mat &im, const cv::KeyPoint &point, Color color, float tolerance = 40.0);

bool is_black(const cv::Mat &im, const cv::KeyPoint &point, float tolerance = 40.0);

bool is_red(const cv::Mat &im, const cv::KeyPoint &point, float tolerance = 40.0);

} // namespace toy_decoder::util::color

namespace geo {

using UnitVector = cv::Point2f;

geo::UnitVector connecting_vector(const cv::Point2f &a, const cv::Point2f &b);

// see https://stackoverflow.com/questions/25642532/opencv-pointx-y-represent-column-row-or-row-column
void to_cartesian(cv::Point2f &point);

} // namespace toy_decoder::util::geo

namespace calc {

void rotate(cv::Point2f &point, units::Degrees degrees);

float norm(const cv::Point2f &point);

} // namespace toy_decoder::util::calc
} // namespace toy_decoder::util
} // namespace toy_decoder

#endif //HWCPP_UTIL_HPP
