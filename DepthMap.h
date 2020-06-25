#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include <opencv2/opencv.hpp>

#define LOWE_THRESHOLD 0.6f //works well for bicycle imgs

class DepthMap
{

public:

    DepthMap(cv::Mat img1, cv::Mat img2);
    cv::Mat generateMap();

private:

    cv::Mat undistortImage(cv::Mat img, void *data);
    cv::Mat _stereo1, _stereo2;

};

#endif // DEPTHMAP_H
