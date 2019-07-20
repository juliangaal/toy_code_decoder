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

struct VideoParams {
    size_t id;
    size_t width;
    size_t height;
    size_t frame_rate;
};

using Point2i = Point<int>;

/**
 * Handles all operations necessary to decode Code (sample seen in README.md)
 */
class NotQRCodeDecoder {
public:
    /**
     * Factory method: initialize QRCodeDecoder with video stream on
     * @param video_device_id: id of /dev/videoX
     * @param params: VideoParams video stream parameters
     * @return NotQRCodeDecoder object
     */
    static NotQRCodeDecoder video(const VideoParams &vid_params);

    /**
     * Factory method: initialize QRCodeDecoder with video stream on and custom blobdetector parameters
     * @param video_device_id: id of /dev/videoX
     * @param params: VideoParams video stream parameters
     * @param params:  custom cv::SimpleBlobdetector::Params
     * @return NotQRCodeDecoder object
     */
    static NotQRCodeDecoder
    video_parameterized(const VideoParams &vid_params, const cv::SimpleBlobDetector::Params &blob_params);

    /**
     * Initialize with open cv Image
     * @param img: cv::Mat
     * @return NotQRCodeDecoder object
     */
    static NotQRCodeDecoder cv_img(cv::Mat img);

    /**
     * Initialize with open cv Image and custom BlobDetector parameters
     * @param img: cv::Mat
     * @param params: custom cv::SimpleBlobDetector::Params
     * @return NotQRCodeDecoder object
     */
    static NotQRCodeDecoder cv_img_parameterized(cv::Mat img, const cv::SimpleBlobDetector::Params &params);

    /**
     * Initialize with open filename
     * @param filename: filename (path, relative from binary)
     * @param mode: https://docs.opencv.org/3.1.0/d4/da8/group__imgcodecs.html#ga61d9b0126a3e57d9277ac48327799c80
     * @return NotQRCodeDecoder object
    */
    static NotQRCodeDecoder file(std::string filename, cv::ImreadModes mode);

    /**
     * Initialize with open filename and custom SimpleBlobDetector parameters
     * @param filename: filename (path, relative from binary)
     * @param mode: https://docs.opencv.org/3.1.0/d4/da8/group__imgcodecs.html#ga61d9b0126a3e57d9277ac48327799c80
     * @param: custom cv::SimpleBlobDetector::Params params
     * @return NotQRCodeDecoder object
    */
    static NotQRCodeDecoder
    file_parameterized(std::string filename, cv::ImreadModes mode, const cv::SimpleBlobDetector::Params &params);

    /**
     * FOR PYTHON WRAPPER ONLY
     * Initialize with open filename
     * @param filename: filename (path, relative from binary)
     * @param mode AS SIZE_T: https://docs.opencv.org/3.1.0/d4/da8/group__imgcodecs.html#ga61d9b0126a3e57d9277ac48327799c80
     * @return NotQRCodeDecoder object
    */
    static NotQRCodeDecoder file_py(std::string filename, int mode);

    /**
     * FOR PYTHON WRAPPER ONLY
     * Initialize with open filename and custom SimpleBlobDetector parameters
     * @param filename: filename (path, relative from binary)
     * @param mode AS SIZE_T: https://docs.opencv.org/3.1.0/d4/da8/group__imgcodecs.html#ga61d9b0126a3e57d9277ac48327799c80
     * @param: custom cv::SimpleBlobDetector::Params params
     * @return NotQRCodeDecoder object
    */
    static NotQRCodeDecoder
    file_parameterized_py(std::string filename, size_t mode, const cv::SimpleBlobDetector::Params &params);

    /// default constructor deleted
    NotQRCodeDecoder() = delete;

    /// default destructor
    ~NotQRCodeDecoder() = default;

    /**
     * Calculates keypoints
     * @param mark: marks keypoint when chosen (optional)
     */
    void calculate_keypoints(Draw mark = Draw::NO);

    /**
     * Calculates orientation from keypoint
     * @return Result with orientation, error type
     */
    Result<float> calculate_orientation(Draw mark = Draw::NO);

    /**
     * Rotate keypoints by specified amout
     * @param degrees amount to rotate
     */
    void rotate_keypoints(notqrcode::util::units::Degrees degrees);

    /**
     * Get's next frame in cv::VideoCapture stream and resets all parameters used for
     * previous detection of keypoints
     */
    void next_frame();

    /**
     * cv::Mat image getter
     * @return img
     */
    const cv::Mat& img() const;

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
     * Change SimpleBlobDetector params on the go
     * @param params: custom cv::SimpleBlobDetector::Params struct
     */
     void change_blob_params(const cv::SimpleBlobDetector::Params &params);

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

    /**
     * NotQRCode decoder from cv::Mat
     * @param img: cv::Mat
     */
    explicit NotQRCodeDecoder(cv::Mat img);

    /**
     * NotQRCode decoder from cv::Mat
     * @param img: cv::Mat
     * @param params: custom cv::SimpleBlobDetector::Params params
    */
    NotQRCodeDecoder(cv::Mat img, const cv::SimpleBlobDetector::Params &params);

    /**
     * Initialize with open filename
     * @param filename: filename (path, relative from binary)
     * @param mode: https://docs.opencv.org/3.1.0/d4/da8/group__imgcodecs.html#ga61d9b0126a3e57d9277ac48327799c80
    */
    NotQRCodeDecoder(std::string filename, cv::ImreadModes mode);

    /**
     * Initialize with open filename and custom SimpleBlobDetector parameters
     * @param filename: filename (path, relative from binary)
     * @param mode: https://docs.opencv.org/3.1.0/d4/da8/group__imgcodecs.html#ga61d9b0126a3e57d9277ac48327799c80
     * @param params: custom cv::SimpleBlobDetector::Params params
    */
    NotQRCodeDecoder(std::string filename, cv::ImreadModes mode, const cv::SimpleBlobDetector::Params &params);

    /**
      * initialize QRCodeDecoder with video stream on
      * @param video_device_id: id of /dev/videoX
      * @param params: VideoParams video stream parameters
      * @return NotQRCodeDecoder object
      */
    explicit NotQRCodeDecoder(const VideoParams &vid_params);

    /**
     * initialize QRCodeDecoder with video stream on and custom blobdetector parameters
     * @param video_device_id: id of /dev/videoX
     * @param params: VideoParams video stream parameters
     * @param params:  custom cv::SimpleBlobdetector::Params
     * @return NotQRCodeDecoder object
     */
    NotQRCodeDecoder(const VideoParams &vid_params, const cv::SimpleBlobDetector::Params &blob_params);

    /// opencv image
    cv::Mat _img;
    /// opencv video stream
    cv::VideoCapture _video;
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
