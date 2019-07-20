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

using SkipEmptyCheck = Draw;

/**
 * Error types for NotQRCodeDecoder
 */
enum Error {
    None = 0,
    InvalidKeyPoints,
    SeparationError,
    CentroidDetectionError,
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

struct ImgProcessingParams {
    size_t gaussian_size;
    size_t threshold;
    size_t threshold_repl_value;
};

/**
 * Handles all operations necessary to decode Code (sample seen in README.md)
 */
class NotQRCodeDecoder {
public:

    /**
     *
     * @param filename
     * @param mode
     * @return
     */
    static NotQRCodeDecoder file(std::string filename);

    /**
     *
     * @param filename
     * @param mode
     * @param blob_params
     * @return
     */
    static NotQRCodeDecoder
    file_with_params(std::string filename, const ImgProcessingParams &img_proc_params,
                     const cv::SimpleBlobDetector::Params &blob_params);


    /**
     *
     * @param img
     * @return
     */
    static NotQRCodeDecoder img(cv::Mat& img);

    /**
     *
     * @param img
     * @param blob_params
     * @return
     */
    static NotQRCodeDecoder img_with_params(cv::Mat &img, const ImgProcessingParams &img_proc_params,
                                            const cv::SimpleBlobDetector::Params &blob_params);

    /**
     *
     * @return
     */
    static NotQRCodeDecoder video();

    /**
     *
     * @param blob_params
     * @return
     */
    static NotQRCodeDecoder video_with_params(const ImgProcessingParams &img_proc_params,
                                              const cv::SimpleBlobDetector::Params &blob_params);

    /*
     * Init with cv::Matrix
     */
    explicit NotQRCodeDecoder(cv::Mat &img, SkipEmptyCheck skip);

    /*
     * Init with cv::Matrix and custom Simpleblobdetector params
    */
    NotQRCodeDecoder(cv::Mat &img, const ImgProcessingParams &img_proc_params,
                     const cv::SimpleBlobDetector::Params &params, SkipEmptyCheck skip);

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

    /**
     * Overloaded operator to load image from cv::VideoCapture into cv::Mat
     * @param code
     * @param cap
     * @return
     */
    friend NotQRCodeDecoder& operator<<(NotQRCodeDecoder& code, cv::VideoCapture& cap);

    /**
     *
     * @return
     */
    inline const cv::Mat& img() const { return _img; }

    /**
     * Prepare image for detecting: Apply gaussian blur, threshold and convert to grayscale
     */
    void prep_image();

private:
    /// opencv image
    cv::Mat& _img;
    /// simple blobdetector
    cv::SimpleBlobDetector::Params _blob_params;
    /// image processing params
    ImgProcessingParams _img_proc_params;
    /// vector to save keypoints
    std::vector<cv::KeyPoint> _keypoints;
    /// cv::Keypoint to save orientation point
    cv::KeyPoint _orientation_point;
    /// avg_size used to calculate avg size of all detected rectangles
    float _avg_size;
};

}

#endif //NOT_QRCODE_DECODER_HPP
