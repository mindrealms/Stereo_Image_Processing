#ifndef CALIBPARSER_H
#define CALIBPARSER_H

#include <opencv.hpp>
#include <cstring>
#include <QString>

/* Parses a calib.txt file of the Middleburry stereo dataset into a CalibData struct
 *
 * Dataset:
 * https://vision.middlebury.edu//stereo/data/scenes2014/#description
 *
 */

namespace CalibParser {

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

void populateMatrix(std::stringstream &stream, cv::Mat &matrix) {

    std::string temp;
    char c; //discard '[' (is there a neater way to do this?)
    float a1, a2, a3, b1, b2, b3, c1, c2, c3;
    stream >> c >> a1 >> a2;
    std::getline(stream, temp, ';');
    a3 = std::stof(temp);
    stream >> b1 >> b2;
    std::getline(stream, temp, ';');
    b3 = std::stof(temp);
    stream >> c1 >> c2;
    std::getline(stream, temp, ']');
    c3 = std::stof(temp);

    float mat_data[9] = {a1, a2, a3, b1, b2, b3, c1, c2, c3};

    //I'm so sorry this is SO ugly but RIP C++ and ownership.....  TT.TT
    matrix = cv::Mat(3, 3, CV_32F, mat_data).clone();
}

void parseFile(cv::String file, CalibData &data) {

    cv::String line;
    std::ifstream inputFileStream;
    inputFileStream.open(file);

    if (inputFileStream.is_open()) {
        std::string temp;
        while (std::getline(inputFileStream,line, '=')) {
            std::getline(inputFileStream, temp, '\n');
            std::stringstream stream(temp);
            if (line == "cam0") {
                populateMatrix(stream, data.cammat1);
            } else if (line == "cam1") {
                populateMatrix(stream, data.cammat2);
            } else if (line == "doffs") {
                stream >> data.doffs;
            } else if (line == "baseline") {
                stream >> data.baseline;
            } else if (line == "width") {
                stream >> data.width;
            } else if (line == "height") {
                stream >> data.height;
            } else if (line == "ndisp") {
                stream >> data.ndisp;
            } else if (line == "isint") {
                stream >> data.isint;
            } else if (line == "vmin") {
                stream >> data.vmin;
            } else if (line == "vmax") {
                stream >> data.vmax;
            } else if (line == "dyavg") {
                stream >> data.dyavg;
            } else if (line == "dymax") {
                stream >> data.dymax;
            }
        }
        inputFileStream.close();
    }
}

}

#endif // CALIBPARSER_H
