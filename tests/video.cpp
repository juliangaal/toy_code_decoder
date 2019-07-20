//
// Created by julian on 5/26/19.
//

#include <notqrcode/notqrcode_decoder.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>

using namespace notqrcode;

int main(void) {
    fmt::print("OPENCV version {}.{}\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);
    fmt::print("Attempting with video device /dev/video{}\n", 0);

    // opencv setup
    size_t vid_id = 0;
    size_t frame_rate = 10;
    cv::VideoCapture cap(vid_id); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        throw std::runtime_error(fmt::format("Couldn't open video at /dev/video{}", vid_id));

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    cap.set(cv::CAP_PROP_FPS, frame_rate);

    // blob detection parameters
    // Change thresholds
    cv::SimpleBlobDetector::Params blob_params;
    blob_params.minThreshold = 30;
    blob_params.maxThreshold = 200;

    // Filter by Area.
    blob_params.filterByArea = true;
    blob_params.minArea = 3.14159 * 10.0f * 10.0f; // Min 10.0f diameter
    blob_params.maxArea = 3.14159 * 50.0f * 50.0f; // Max 30.0f diameter

    // filter by color: only black
    blob_params.filterByColor = true;
    blob_params.blobColor = 0;

    // Filter by Circularity
    blob_params.filterByCircularity = false;

    // Filter by Convexity
    blob_params.filterByConvexity = true;
    blob_params.minConvexity = 0.87;

    // Filter by Inertia
    blob_params.filterByInertia = true;
    blob_params.minInertiaRatio = 0.01;

    notqrcode::ImgProcessingParams img_params;
    img_params.gaussian_size = 5;
    img_params.threshold = 245;
    img_params.threshold_repl_value = 255;

    auto decoder = NotQRCodeDecoder::video_with_params(img_params, blob_params);

    for(;;) {
        decoder << cap;

        decoder.calculate_keypoints(Draw::YES);
        cv::imshow("Video", decoder.img());
        if(cv::waitKey(30) >= 0) break;

        auto orientation = decoder.calculate_orientation();
        if (orientation.error != Error::None)
            continue;

        decoder.rotate_keypoints(util::units::Degrees(orientation.val));
        auto decoded_point = decoder.decode();
        if (decoded_point.error != Error::None)
            continue;

        fmt::print("Rotation: {:.3f}° - Decoded: ({},{})\n", orientation.val, decoded_point.val.x, decoded_point.val.y);
    }
    return 0;
}