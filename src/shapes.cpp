//
// Created by julian on 5/25/19.
// adapted from
// https://web.archive.org/web/20150317002418/http://opencv-code.com/tutorials/detecting-simple-shapes-in-an-image/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
 * Helper function to display text in the center of a contour
 */
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.4;
    int thickness = 1;
    int baseline = 0;

    cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
    cv::Rect r = cv::boundingRect(contour);

    cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
    cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), cv::Scalar(255,255,255), cv::FILLED);
    cv::circle(im, pt, 7, cv::Scalar(0, 0, 0), -1);
    cv::putText(im, label, pt, fontface, scale*0.5, cv::Scalar(0,0,0), thickness, 8);
}

static void drawSquares(cv::Mat& image, const std::vector<std::vector<cv::Point>>& squares) {
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const cv::Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        cv::polylines(image, &p, &n, 1, true, cv::Scalar(0,255,0), 1, cv::LINE_AA);
    }

    cv::imshow("marked", image);
}

int main()
{
    //cv::Mat src = cv::imread("polygon.png");
    cv::Mat src = cv::imread("../pics/rect_grey.jpg");
    if (src.empty())
        return -1;

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    // Use Canny instead of threshold to catch squares with gradient shading
    cv::Mat bw;
    cv::Canny(gray, bw, 0, 50, 5);

    // Find contours
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(bw.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> approx;
    std::vector<std::vector<cv::Point>> squares;

    cv::Mat dst = src.clone();

    for (size_t i = 0; i < contours.size(); i++)
    {
        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

        // Skip small or non-convex objects
        if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
            continue;

        if (approx.size() == 3)
        {
            setLabel(dst, "TRI", contours[i]);    // Triangles
        }
        else if (approx.size() >= 4 && approx.size() <= 6)
        {
            // Number of vertices of polygonal curve
            int vtc = approx.size();

            // Get the cosines of all corners
            std::vector<double> cos;
            for (int j = 2; j < vtc+1; j++)
                cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

            // Sort ascending the cosine values
            std::sort(cos.begin(), cos.end());

            // Get the lowest and the highest cosine
            double mincos = cos.front();
            double maxcos = cos.back();

            // Use the degrees obtained above and the number of vertices
            // to determine the shape of the contour
            if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
                squares.push_back(approx);
                setLabel(dst, "RECT", contours[i]);
            } else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27) {
                setLabel(dst, "PENTA", contours[i]);
            } else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45) {
                setLabel(dst, "HEXA", contours[i]);
            }
        }
        else
        {
            // Detect and label circles
            double area = cv::contourArea(contours[i]);
            cv::Rect r = cv::boundingRect(contours[i]);
            int radius = r.width / 2;

            if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
                std::abs(1 - (area / (M_PI * std::pow(radius, 2)))) <= 0.2)
                setLabel(dst, "CIR", contours[i]);
        }
    }

    drawSquares(src, squares);

    cv::imshow("src", src);
    cv::imshow("dst", dst);
    cv::waitKey(0);
    return 0;
}
