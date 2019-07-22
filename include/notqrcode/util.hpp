// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org>

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

/**
 * decode translates vector of keypoints with differing sizes in binary notation to a numbe
 * @param begin: begin of keypoints vector
 * @param end: end of keypoints vector
 * @param size: objects bigger than size are treated as 1s, objects smaller than size are treated as 0s
 * @return
 */
int decode(std::vector<cv::KeyPoint>::const_iterator begin,
           std::vector<cv::KeyPoint>::const_iterator end, float size);

/**
 * Performs std::stable_partition by separating keypoints according to height
 * @param begin: begin of keypoints vector
 * @param end: end of keypoints vector
 * @param height: height to separate with
 * @return iterator of std::stable_partition
 */
std::vector<cv::KeyPoint>::iterator
partition_by_height(std::vector<cv::KeyPoint>::iterator begin, std::vector<cv::KeyPoint>::iterator end, float height);

namespace units {

/// Helper class to avoid calculation errors between degrees and radians
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
geo::UnitVector connecting_vector(const cv::Point2f &a, const cv::Point2f &b);

/**
 * Converts opencv coordinates in something usable, see
 * see https://stackoverflow.com/questions/25642532/opencv-pointx-y-represent-column-row-or-row-column
 * @param point
 */
void to_cartesian(cv::Point2f &point);

} // namespace notqrcode::util::geo

namespace calc {

/**
 * Rotate point in 2d space aka rotation matrix
 * @param point to rotate
 * @param degrees amount of degrees to rotate
 */
void rotate(cv::Point2f &point, units::Degrees degrees);

/**
 * Calculates vector length
 * @param point to calculate length from
 * @return vector length
 */
float norm(const cv::Point2f &point);

float euc_dist(const cv::Point2f a, const cv::Point2f b);

} // namespace notqrcode::util::calc
} // namespace notqrcode::util
} // namespace notqrcode

#endif //NOTQRCODE_UTIL_HPP
