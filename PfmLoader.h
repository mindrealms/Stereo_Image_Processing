#ifndef PFMLOADER_H
#define PFMLOADER_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <cmath>

/*
 * Source: https://github.com/dscharstein/pfmLib
 *
 *
 *
 * Name:
 *      ImageIOpfm.h
 *
 *  Description:
 *      Used to read/write pfm images to and from
 *      opencv Mat image objects
 *
 *      Works with PF color pfm files and Pf grayscale
 *      pfm files
 */

class PfmLoader {

public:
    PfmLoader();

    void skipSpace(std::fstream& fileStream);
    int littleEndian();
    void swapBytes(float* fptr);
    int readFilePFM(cv::Mat &im, std::string path);

};

#endif // PFMLOADER_H
