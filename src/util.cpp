//
// Created by julian on 5/24/19.
//

#include <toy_decoder/util.hpp>

using namespace toy_decoder::util;

size_t decode(std::vector<cv::KeyPoint>::const_iterator begin,
              std::vector<cv::KeyPoint>::const_iterator end) {
    size_t val = 0;
    for (auto it = begin; it != end; it++) {
        size_t exp = std::distance(it, end) - 1;
        val += std::pow(2, exp);
    }
    return val;
}

bool color::same_color(const cv::Mat &im, const cv::KeyPoint &point, Color color, float tolerance) {
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

bool color::is_black(const cv::Mat &im, const cv::KeyPoint &point, float tolerance) {
    return same_color(im, point, Color::BLACK, tolerance);
}

bool color::is_red(const cv::Mat &im, const cv::KeyPoint &point, float tolerance) {
    return same_color(im, point, Color::RED, tolerance);
}

geo::UnitVector geo::connecting_vector(const cv::Point2f &a, const cv::Point2f &b) {
    float x = b.x - a.x;
    float y = b.y - a.y;
    float length = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
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
    return std::sqrt(std::pow(point.x, 2) + std::pow(point.y, 2));
}