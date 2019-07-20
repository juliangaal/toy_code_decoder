#include <notqrcode/notqrcode_decoder.hpp>
#include <opencv2/opencv.hpp>
#include <fmt/format.h>

using namespace notqrcode;
using notqrcode::util::units::Degrees;

int main() {
    fmt::print("OPENCV version {}.{}\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);

    // blob detection parameters
    // Change thresholds
    cv::SimpleBlobDetector::Params blob_params{};
    blob_params.minThreshold = 10;
    blob_params.maxThreshold = 200;

    // Filter by Area.
    blob_params.filterByArea = true;
    blob_params.minArea = 3.14159 * 10.0f * 10.0f; // Min 10.0f diameter
    blob_params.maxArea = 3.14159 * 40.0f * 40.0f; // Max 30.0f diameter

    // filter by color: only black
    blob_params.filterByColor = 0;

    // Filter by Circularity
    blob_params.filterByCircularity = false;

    // Filter by Convexity
    blob_params.filterByConvexity = true;
    blob_params.minConvexity = 0.87;

    // Filter by Inertia
    blob_params.filterByInertia = true;
    blob_params.minInertiaRatio = 0.01;

    VideoParams vid_params{};
    vid_params.id = 0;
    vid_params.frame_rate = 10;
    vid_params.width = 320;
    vid_params.height = 240;

    fmt::print("Attempting with /dev/video{}\n", vid_params.id);

    auto decoder = NotQRCodeDecoder::video_parameterized(vid_params, blob_params);
    
    for(;;) {
        decoder.next_frame();

        decoder.calculate_keypoints(notqrcode::Draw::YES);
	    auto orientation = decoder.calculate_orientation();
        if (orientation.error != notqrcode::Error::None) {
            continue;
        }


        decoder.rotate_keypoints(Degrees(orientation.val));
	    auto decoded_point = decoder.decode();
        if (decoded_point.error != notqrcode::Error::None) {
            continue;
        }

        fmt::print("Rotation: {} - Decoded {}:{}\n", orientation.val, decoded_point.val.x, decoded_point.val.y);
    }

    return 0;
}
