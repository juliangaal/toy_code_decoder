//
// Created by julian on 5/25/19.
//

#ifndef NOT_QRCODE_DECODER_HPP
#define NOT_QRCODE_DECODER_HPP

#include <notqrcode/util.hpp>
#include <vector>

namespace notqrcode {

/**
 * Draw keypoints/orientation in image with Draw::YES
 */
enum Draw {
    YES = 0,
    NO,
};

/**
 * Error types for NotQRCodeDecoder
 */
enum Error {
    None = 0,
    InvalidKeyPoints,
    SeparationError,
};

/**
 * Result type
 * @tparam T type of result value
 */
template <typename T>
struct Result {
    // Value of calculation
    T val;

    // Error of calculation
    Error error;
};

/// Point typee used for pybind convenience
template <typename T>
struct Point {
    T x;
    T y;
};

using Point2i = Point<int>;

/**
 * Handles all operations necessary to decode Code (sample seen in README.md)
 */
class NotQRCodeDecoder {
public:
    /*
     * Init with cv::Matrix
     */
    explicit NotQRCodeDecoder(cv::Mat& img);

    /*
     * Init with cv::Matrix and custom Simpleblobdetector params
    */
    NotQRCodeDecoder(cv::Mat& img, cv::SimpleBlobDetector::Params params);

    /// default constructor deleted
    NotQRCodeDecoder() = delete;

    /// default destructor
    ~NotQRCodeDecoder() = default;

    /**
     * Calculates keypoints
     * @param mark: marks keypoint when chosen (optional)
     */
    void calculate_keypoints(Draw mark = NO);

    /**
     * Calculates orientation from keypoint
     * @return Result with orientation, error type
     */
    Result<float> calculate_orientation(Draw mark = NO);

    /**
     * Rotate keypoints by specified amout
     * @param degrees amount to rotate
     */
    void rotate_keypoints(notqrcode::util::units::Degrees degrees);

    /**
     * Rotate opencv image
     * @param degrees amount to rotate
     */
    void rotate_img(notqrcode::util::units::Degrees degrees);

    /**
     * Decodes code
     * @return Result with cv::Point2i with coordinates and error type
     */
    Result<Point2i> decode();

    /**
     * Saves image
     * @param name to save with (optional)
     */
    void save_img(std::string name = "output.jpg");

    /**
     * Opens opencv window with image in current state
     * @param name of the window (optional)
     */
    void open_img(std::string name = "img");
private:
    /// opencv image
    cv::Mat& _img;
    /// simple blobdetector
    cv::SimpleBlobDetector::Params _params;
    /// vector to save keypoints
    std::vector<cv::KeyPoint> _keypoints;
    /// cv::Keypoint to save orientation point
    cv::KeyPoint _orientation_point;
    /// avg_size used to calculate avg size of all detected rectangles
    float _avg_size;
};

}

#endif //NOT_QRCODE_DECODER_HPP
