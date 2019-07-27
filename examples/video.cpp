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


    // parameters are optional, tune for your widht, height etc...
    auto decoder = NotQRCodeDecoder::video();

    for(;;) {
        decoder << cap;

        decoder.calculate_keypoints(Draw::YES);
        cv::imshow("Video", decoder.img());
        cv::waitKey(20);

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