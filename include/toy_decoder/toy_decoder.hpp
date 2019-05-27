//
// Created by julian on 5/25/19.
//

#ifndef HWCPP_TOY_DECODER_HPP
#define HWCPP_TOY_DECODER_HPP

#include <opencv2/opencv.hpp>
#include <toy_decoder/util.hpp>
#include <vector>
#include <tuple>

namespace toy_decoder {

enum Mark_Keypoints {
    YES,
    NO,
};

class ToyDecoder {
public:
    ToyDecoder(cv::Mat &img);

    ToyDecoder(cv::Mat &img, cv::SimpleBlobDetector::Params params);

    ToyDecoder() = delete;

    ~ToyDecoder() = default;

    void calculate_keypoints(Mark_Keypoints mark);

    std::tuple<float, bool> calculate_orientation();

    void rotate_keypoints(toy_decoder::util::units::Degrees degrees);

    void rotate_img(toy_decoder::util::units::Degrees degrees);

    std::tuple<cv::Point2i, bool> decode();

    void save_img(std::string name = "output.jpg");

    void open_img(std::string name = "img");
private:
    cv::Mat &_img;
    cv::SimpleBlobDetector::Params _params;
    std::vector<cv::KeyPoint> _keypoints;
    cv::KeyPoint _orientation_point;
    float _avg_size;
};

}

#endif //HWCPP_TOY_DECODER_HPP
