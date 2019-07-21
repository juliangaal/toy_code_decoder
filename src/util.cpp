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

#include <notqrcode/util.hpp>

using namespace notqrcode::util;

geo::UnitVector geo::connecting_vector(const cv::Point2f &a, const cv::Point2f &b) {
    float x = b.x - a.x;
    float y = b.y - a.y;
    float length = std::sqrt(std::pow(x, 2.f) + std::pow(y, 2.f));
    return geo::UnitVector{x / length, y / length};
}

// see https://stackoverflow.com/questions/25642532/opencv-pointx-y-represent-column-row-or-row-column
void geo::to_cartesian(cv::Point2f &point) {
    point.y *= -1;
}

void calc::rotate(cv::Point2f &point, units::Degrees degrees) {
    float angle = degrees.to_rad();
    auto x = point.x; // original x is saved for later calculation
    point.x = point.x * std::cos(angle) - point.y * std::sin(angle);
    point.y = x * std::sin(angle) + point.y * std::cos(angle);
}

float calc::norm(const cv::Point2f &point) {
    return std::sqrt(std::pow(point.x, 2.f) + std::pow(point.y, 2.f));
}

float calc::euc_dist(const cv::Point2f a, const cv::Point2f b) {
    return std::sqrt(std::pow((b.x - a.x), 2.0f) + std::pow((b.y - a.y), 2.0f));
}

int notqrcode::util::decode(std::vector<cv::KeyPoint>::const_iterator begin,
                            std::vector<cv::KeyPoint>::const_iterator end, float avg_size) {
    int val = 0;
    int bit = 0;

    for (auto it = begin; it != end; it++) {
        int exp = std::distance(it, end) - 1u;
        bit = ((*it).size < avg_size) ? 0 : 1;
        val += bit * std::pow(2, exp);
    }

    return val;
}

std::vector<cv::KeyPoint>::iterator
notqrcode::util::partition_by_height(std::vector<cv::KeyPoint>::iterator begin, std::vector<cv::KeyPoint>::iterator end,
                                     float heigth) {
    const auto separator_it = std::stable_partition(begin, end, [&](const auto &p) {
        return p.pt.y > heigth;
    });

    return separator_it;
}
