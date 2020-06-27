#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include <opencv2/opencv.hpp>

#define LOWE_THRESHOLD 0.6f //works well for bicycle imgs

class DepthMap
{

public:

    DepthMap(cv::Mat img1, cv::Mat img2);
    void generateMap(cv::Mat &newimg1, cv::Mat &newimg2);

private:

    cv::Mat undistortImage(cv::Mat img, void *data);

    void drawEpilines(cv::Mat img, cv::Mat lines, std::vector<cv::Point2f> pts, cv::Mat &newimg);

    cv::Mat _stereo1, _stereo2;
    int _width;

};

#endif // DEPTHMAP_H
