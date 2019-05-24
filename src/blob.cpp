#include <opencv2/opencv.hpp>
#include <fmt/format.h>
#include <vector>
#include <boost/filesystem.hpp>
#include <cmath>

namespace fs = boost::filesystem;

namespace geo {

struct UnitVector {
    float x;
    float y;
};

}

enum Color {
    RED,
    GREEN,
    BLUE
};

const static cv::Vec3b RGBRED   = cv::Vec3b{255, 0, 0};
const static cv::Vec3b RGBGREEN = cv::Vec3b{0, 255, 0};
const static cv::Vec3b RGBBLUE  = cv::Vec3b{0, 0, 255};

bool same_color(const cv::Mat& im, const cv::KeyPoint& point, Color color) {
    // opencv idiots thought it was a good idea to return color in BGR
    auto true_color = im.at<cv::Vec3b>(point.pt);
    switch (color) {
        case RED:
            if (std::abs(RGBRED[0] - true_color.val[2]) < 100) return true; 
        case GREEN:
            if (std::abs(RGBGREEN[1] - true_color.val[1]) < 100) return true; 
        case BLUE:
            if (std::abs(RGBBLUE[2] - true_color.val[0]) < 100) return true; 
    }

    return false;
}

geo::UnitVector connecting_vector(const cv::KeyPoint& a, const cv::KeyPoint& b) {
    float x = std::abs(a.pt.x - b.pt.x);
    float y = std::abs(a.pt.y - b.pt.y);
    float length = std::sqrt(std::pow(x, 2.0) + std::pow(y, 2.0));
    return geo::UnitVector{x/length, y/length};
}

int main(int argc, char** argv) { 
    if (argc < 2) {
        fmt::print("No file argument!\n");
        return -1;
    } 

    if (!fs::exists(argv[1])) {
        fmt::print("File not found\n");
        return -1;
    }
    
    auto file = argv[1];
    fmt::print("Got file {}\n", file);

    // Read image
    cv::Mat im = cv::imread(file, cv::IMREAD_COLOR);

	// Setup SimpleBlobDetector parameters.
    cv::SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 10;
	params.maxThreshold = 200;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 100;
    params.maxArea = 1000000;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.01;

	// Storage for blobs
    std::vector<cv::KeyPoint> keypoints;

#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

	// Set up detector with params
    cv::SimpleBlobDetector detector(params);

	// Detect blobs
	detector.detect(im, keypoints);
#else 

	// Set up detector with params
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);   

	// Detect blobs
	detector->detect(im, keypoints);
#endif 

    // Print keypoint information
    for (auto &point: keypoints) {
        auto color = im.at<cv::Vec3b>(point.pt); 
        fmt::print("> keypoint\n  - pos  : x: {} y: {}\n  - size : {}\n  - color: ({},{},{})\n",
                point.pt.y, point.pt.x, 
                point.size, 
                color.val[2], color.val[1], color.val[0]);        
        // correction, strangely size is exactly half the python version
        point.size *= 2;
    }

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
	// the size of the circle corresponds to the size of blob
    cv::Mat im_with_keypoints;
    cv::drawKeypoints(im, keypoints, im_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::imwrite("result.jpg", im_with_keypoints);
    fmt::print("Output image saved to {}\n", "result.jpg");
    
    // Rotate image
    auto vec = connecting_vector(keypoints[0], keypoints[1]); 
    float rotation = std::atan(vec.y/vec.x) * 180.0/M_PI;
    // TODO adjust rotation in relation to north/south dot
    if (keypoints[1].pt.y < keypoints[0].pt.y) rotation *= -1; 
    auto rot_m = cv::getRotationMatrix2D(cv::Point2f{im.cols/2., im.rows/2.}, rotation, 1);
    cv::warpAffine(im, im, rot_m, cv::Size{im.cols, im.rows});
    cv::imwrite("rotated.jpg", im);
    fmt::print("Rotated output image ({} degrees) saved to {}\n", rotation, "rotated.jpg");

	// Show blobs
    // cv::imshow("keypoints", im_with_keypoints);
    // cv::waitKey(0);
}

