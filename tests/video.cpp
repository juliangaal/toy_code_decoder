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
    size_t frame_rate = 24;
    cv::VideoCapture cap(vid_id); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        throw std::runtime_error(fmt::format("Couldn't open video at /dev/video{}", vid_id));

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    cap.set(cv::CAP_PROP_FPS, frame_rate);

    // blob detection parameters
    // Change thresholds
    cv::SimpleBlobDetector::Params params;
    params.minThreshold = 10;
    params.maxThreshold = 200;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 3.14159 * 10.0f * 10.0f; // Min 10.0f diameter
    params.maxArea = 3.14159 * 40.0f * 40.0f; // Max 30.0f diameter

    // filter by color: only black
    params.filterByColor = 0;

    // Filter by Circularity
    params.filterByCircularity = false;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;

    auto decoder = NotQRCodeDecoder::video_with_params(params);

    for(;;) {
        decoder << cap;

        decoder.calculate_keypoints(Draw::YES);
        auto orientation = decoder.calculate_orientation(Draw::YES);
        if (orientation.error != Error::None)
            continue;

        decoder.rotate_keypoints(util::units::Degrees(orientation.val));
        auto decoded_point = decoder.decode();
        if (decoded_point.error != Error::None)
            continue;

        fmt::print("Rotation: {} - Decoded: ({},{})\n", orientation.val, decoded_point.val.x, decoded_point.val.y);
    }
    return 0;
}