//
// Created by julian on 5/24/19.
//

#ifndef NOTQRCODE_UTIL_HPP
#define NOTQRCODE_UTIL_HPP

#include <opencv2/opencv.hpp>
#include <vector>

namespace notqrcode {

namespace util {

template<typename T, typename S>
constexpr T to(const S &&val) {
    return static_cast<T>(val);
}

/// float version of M_PI
constexpr static float PIf = static_cast<float>(M_PI);

/*
 * decode translates vector of keypoints with differing sizes in binary notation to a number
 * objects bigger than avg_size are treated as 1s
 * objects smaller than avg_size are treated as 0s
 */
int decode(std::vector<cv::KeyPoint>::const_iterator begin,
           std::vector<cv::KeyPoint>::const_iterator end, float avg_size) noexcept ;

std::vector<cv::KeyPoint>::iterator
partition_by_height(std::vector<cv::KeyPoint>::iterator begin, std::vector<cv::KeyPoint>::iterator end, float height) noexcept ;

namespace units {

/**
 * Helper class to avoid calculation errors between degrees and radians
 * Used for more explicit function parameters, e.g. notqrcode::rotate_keypoints
 */
class Degrees {
public:
    /// constructor
    inline explicit constexpr Degrees(float amount) : amount(amount) {}

    /// default constructor deleted
    constexpr Degrees() = delete;

    /// default destructor
    ~Degrees() = default;

    /**
     * converts degrees to rad
     * @return value of amount as rad
     */
    inline constexpr float to_rad() {
        return this->amount * PIf / 180.0f;
    }

    /**
     * returns degrees
     * @return amount, unchanged
     */
    inline constexpr float to_deg() {
        return this->amount;
    }

private:
    /// saves amount of Degrees
    float amount;
};

/// User defined literal, syntactic sugar
constexpr Degrees operator "" _deg(long double deg) {
    return Degrees{static_cast<float>(deg)};
}

} // namespace notqrcode::util::units

namespace geo {

/// explicit UnitVector type for explicitness
using UnitVector = cv::Point2f;

/**
 * Calculates the connecting vector between to points in space
 * @param a Point 1
 * @param b Point 2
 * @return UnitVector of distance and orientation between a and b
 */
geo::UnitVector connecting_vector(const cv::Point2f &a, const cv::Point2f &b) noexcept;

/**
 * Converts opencv coordinates in something usable, see
 * see https://stackoverflow.com/questions/25642532/opencv-pointx-y-represent-column-row-or-row-column
 * @param point
 */
void to_cartesian(cv::Point2f &point) noexcept;

} // namespace notqrcode::util::geo

namespace calc {

/**
 * Rotate point in 2d space aka rotation matrix
 * @param point to rotate
 * @param degrees amount of degrees to rotate
 */
void rotate(cv::Point2f &point, units::Degrees degrees) noexcept;

/**
 * Calculates vector length
 * @param point to calculate length from
 * @return vector length
 */
float norm(const cv::Point2f &point) noexcept;

} // namespace notqrcode::util::calc
} // namespace notqrcode::util
} // namespace notqrcode

#endif //NOTQRCODE_UTIL_HPP
