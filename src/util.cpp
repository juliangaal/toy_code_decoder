//
// Created by julian on 5/24/19.
//

#include <toy_decoder/util.hpp>
#include <iostream>

using namespace toy_decoder::util;

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

int toy_decoder::util::decode(std::vector<cv::KeyPoint>::const_iterator begin,
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
