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
#include <fmt/format.h>
#include <cassert>

using namespace notqrcode;
using notqrcode::util::units::Degrees;

int main(int argc, char* argv[]) {
    if (argc != 2)
	throw std::runtime_error("No file name argument");

    // set simple blob detector params
    // Change thresholds
    cv::SimpleBlobDetector::Params blob_params{};
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

    notqrcode::ImgProcessingParams img_params{};
    img_params.gaussian_size = 5;
    img_params.threshold = 245;
    img_params.threshold_repl_value = 255;
    img_params.centroid_dist_margin = 1.5f;
    img_params.orientation_pt_dist_margin = 3.5f;
    img_params.centroid_orientation_ratio = 0.74f;

    std::string filename(argv[1]);
    // default params, for custom use NotQRCodeDecoder::file_with_params
    auto decoder = NotQRCodeDecoder::file_with_params(filename, img_params, blob_params);
    decoder.calculate_keypoints(Draw::YES);
    decoder.save_img("file_test.jpg");


    auto orientation = decoder.calculate_orientation();
    if (orientation.error != Error::None) {
        fmt::print("Orientation Error: {}\n", orientation.error);
        return -1;
    }

    decoder.rotate_keypoints(util::units::Degrees(orientation.val));
    auto decoded_point = decoder.decode();
    if (decoded_point.error != Error::None) {
        fmt::print("Decoder Error: {}\n", decoded_point.error);
        return -1;
    }

    fmt::print("Rotation: {:.3f}° - Decoded: ({},{})\n", orientation.val, decoded_point.val.x, decoded_point.val.y);
    return 0;
}
