#ifndef STEREOUTILS_H
#define STEREOUTILS_H

#include <opencv2/opencv.hpp>
//#include "CalibLoader.h"

#define LOWE_THRESHOLD 0.6f //works well for bicycle imgs

class StereoUtils {

public:

    StereoUtils(cv::Mat img1, cv::Mat img2);

    ~StereoUtils();

    void renderEpilines(cv::Mat data1, cv::Mat data2, cv::Mat &newimg1, cv::Mat &newimg2,
                        cv::Mat &keymage1, cv::Mat &keymage2, cv::Mat &matchimage);

//    void generateDepthMap(cv::Mat pfm, CalibLoader::CalibData data);

private:

    cv::Mat undistortImage(cv::Mat img, cv::Mat data);

    void draw(cv::Mat img, cv::Mat lines, std::vector<cv::Point2f> pts, cv::Mat &newimg,
              std::vector<cv::Scalar> &colors);

    cv::Mat _stereo1, _stereo2;

    int _width;

};

#endif // STEREOUTILS_H
