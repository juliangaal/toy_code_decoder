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

template <typename T, typename S>
constexpr T to(const S&& val) {
    return static_cast<T>(val);
}

constexpr static float PIf = static_cast<float>(M_PI);

int decode(std::vector<cv::KeyPoint>::const_iterator begin,
           std::vector<cv::KeyPoint>::const_iterator end, float avg_size);

namespace units {

class Degrees {
public:
    inline explicit constexpr Degrees(float amount) : amount(amount) {}

    constexpr Degrees() = delete;

    ~Degrees() = default;

    inline constexpr float to_rad() {
        return this->amount * PIf / 180.0f;
    }

    inline constexpr float to_deg() {
        return this->amount;
    }

private:
    float amount;
};

constexpr Degrees operator "" _deg(long double deg)
{
    return Degrees{static_cast<float>(deg)};
}

} // namespace toy_decoder::util::units

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
