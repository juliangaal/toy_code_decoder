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

#ifndef NOT_QRCODE_DECODER_HPP
#define NOT_QRCODE_DECODER_HPP

#include <notqrcode/util.hpp>

namespace notqrcode {

/// Draw keypoints/orientation in image with Draw::YES
enum Draw {
    YES = 0,
    NO,
};

/// helper
using SkipEmptyCheck = Draw;

/// Error types for NotQRCodeDecoder
enum Error {
    None = 0,
    InvalidKeyPoints,
    SeparationError,
    CentroidDetectionError,
};

/// rust style Result type
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

/// helper
using Point2i = Point<int>;

/// Image Processing Params: adjusts image before decoding takes place
struct ImgProcessingParams {
    /// size of gaussian filter, e.g. 3 for 3x3 gaussian blur matrix
    int gaussian_size;
    /// threshold for pixels that will be applied to cv::threshold
    int threshold;
    /// thresholded pixel replacement values
    int threshold_repl_value;
    /**
     * error margin when calculating distance between centroids
     * if error !<= centroid_dist_margin decoding errors
     */
    float centroid_dist_margin;
    /**
     * error margin when calculating distance between centroids and orientation point
     * if error !<= origin_pt_dist_margin decoding errors
     */
    float orientation_pt_dist_margin;
    /**
     * Depending on the physical code, the centroid distance varies in comparison to
     * the distance between centroids and orientation point.
     * e.g. the ration for tests/pics/rect_bw_16_90_xlarge.jpg is 0.75
     */
    float centroid_orientation_ratio;
};

/// Class handles decoding of images
class NotQRCodeDecoder {
public:

    /**
     * image instance from file of decoder
     * @return NotQRCodeDecoder
     */
    static NotQRCodeDecoder file(std::string filename);

    /**
     * image instance from file of decoder with custom parameters
     * @param img_proc_params: ImgProcessingParams
     * @param blob_params: cv::SimpleBlobDetector::Params
     * @return NotQRCodeDecoder
     */
    static NotQRCodeDecoder
    file_with_params(std::string filename, const ImgProcessingParams &img_proc_params,
                     const cv::SimpleBlobDetector::Params &blob_params);


    /**
     * image instance from cv::Mat of decoder
     * @return NotQRCodeDecoder
     */
    static NotQRCodeDecoder img(cv::Mat& img);

    /**
     * image instance from cv::Mat of decoder with custom parameters
     * @param img_proc_params: ImgProcessingParams
     * @param blob_params: cv::SimpleBlobDetector::Params
     * @return NotQRCodeDecoder
     */
    static NotQRCodeDecoder img_with_params(cv::Mat &img, const ImgProcessingParams &img_proc_params,
                                            const cv::SimpleBlobDetector::Params &blob_params);

    /**
     * Creates video instance for decoder
     * @return NotQRCodeDecoder
     */
    static NotQRCodeDecoder video();

    /**
     * Video instance of decoder with custom parameters
     * @param img_proc_params: ImgProcessingParams
     * @param blob_params: cv::SimpleBlobDetector::Params
     * @return NotQRCodeDecoder
     */
    static NotQRCodeDecoder video_with_params(const ImgProcessingParams &img_proc_params,
                                              const cv::SimpleBlobDetector::Params &blob_params);

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
     * @param code: decoder Object
     * @param cap: opencv Video Capure object
     * @return
     */
    friend NotQRCodeDecoder& operator<<(NotQRCodeDecoder& code, cv::VideoCapture& cap);

    /**
     *
     * @return reference to current image (cv::Mat)
     */
    inline const cv::Mat& img() const { return _img; }

    /**
     * Prepare image for detecting: Apply gaussian blur, threshold and convert to grayscale
     */
    void prep_image();

    /**
     * Prepare image (from video, must be converted to Grayscale) for detecting:
     * Apply gaussian blur, threshold and convert to grayscale
    */
    void prep_image_from_video();

private:
    /**
     * Init with cv::Matrix
     * @param img: cv::Mat
     * @param skip: whether or not to skip image empty check (only YES when using video)
     */
    explicit NotQRCodeDecoder(cv::Mat &img, SkipEmptyCheck skip);

    /**
     * Init with cv::Matrix and custom Simpleblobdetector params
     * @param img: cv::Mat
     * @param img_proc_params
     * @param params
     * @param skip: whether or not to skip image empty check (only YES when using video)
     */
    NotQRCodeDecoder(cv::Mat &img, const ImgProcessingParams &img_proc_params,
                     const cv::SimpleBlobDetector::Params &params, SkipEmptyCheck skip);

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
