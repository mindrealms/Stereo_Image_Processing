#ifndef PFMLOADER_H
#define PFMLOADER_H

/*  Name:
 *      ImageIOpfm.h
 *
 *  Description:
 *      Used to read/write pfm images to and from
 *      opencv Mat image objects
 *
 *      Works with PF color pfm files and Pf grayscale
 *      pfm files
 */

#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <cmath>


class PfmLoader {

public:
    PfmLoader();

    void skip_space(std::fstream& fileStream);
    int littleendian();
    void swapBytes(float* fptr);
    int ReadFilePFM(cv::Mat &im, std::string path);
    int WriteFilePFM(const cv::Mat &im, std::string path, float scalef=1/255.0);

};

#endif // PFMLOADER_H
