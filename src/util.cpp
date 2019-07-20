//
// Created by julian on 5/24/19.
//

#include <notqrcode/util.hpp>

using namespace notqrcode::util;

bool notqrcode::util::centroids_eq_dist_to(const cv::Point2f &centroid_a, const cv::Point2f &centroid_b,
                                           const cv::Point2f &point,
                                           float margin) {
    return (std::abs(calc::euc_dist(centroid_a, point) - calc::euc_dist(centroid_b, point))) < margin;
}

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

cv::ImreadModes notqrcode::util::python::int_to_imread_mode(int mode) noexcept {
    switch (mode) {
        case -1:
            return cv::IMREAD_UNCHANGED;
        case 0:
            return cv::IMREAD_GRAYSCALE;
        case 1:
            return cv::IMREAD_COLOR;
        case 2:
            return cv::IMREAD_ANYDEPTH;
        case 4:
            return cv::IMREAD_ANYCOLOR;
        case 8:
            return cv::IMREAD_LOAD_GDAL;
        case 16:
            return cv::IMREAD_REDUCED_GRAYSCALE_2;
        case 17:
            return cv::IMREAD_REDUCED_COLOR_2;
        case 32:
            return cv::IMREAD_REDUCED_GRAYSCALE_4;
        case 33:
            return cv::IMREAD_REDUCED_COLOR_4;
        case 64:
            return cv::IMREAD_REDUCED_GRAYSCALE_8;
        case 65:
            return cv::IMREAD_REDUCED_COLOR_8;
        default:
            return cv::IMREAD_GRAYSCALE;
    }
}
