#ifndef CALIBLOADER_H
#define CALIBLOADER_H

#include <opencv.hpp>
#include <cstring>
#include <QString>
#include <fstream>

/* Parses a calib.txt file of the Middleburry stereo dataset into a CalibData struct
 *
 * Dataset:
 * https://vision.middlebury.edu//stereo/data/scenes2014/#description
 *
 */

struct CalibData {
    cv::Mat cammat1;    //camera matrices for the rectified views, in the form [f 0 cx; 0 f cy; 0 0 1]
    cv::Mat cammat2;
    float doffs;        //x-difference of principal points, doffs = cx1 - cx0
    float baseline;     //camera baseline in mm
    int width;          //image dimensions
    int height;
    int ndisp;          //a conservative bound on the number of disparity levels
    int isint;          //whether the GT disparites only have integer precision
    int vmin;           //a tight bound on minimum and maximum disparities, used for color visualization
    int vmax;
    int dyavg;          //average and maximum absolute y-disparities (for calibration error)
    int dymax;
};

class CalibLoader{

public:

    CalibLoader();

    void populateMatrix(std::stringstream &stream, cv::Mat &matrix);

    void parseFile(cv::String file, CalibData &data);

};

#endif // CALIBLOADER_H

