#include "PfmLoader.h"
#include <iostream>
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
 *      PfmLoader.cpp
 *
 *  Description:
 *      Used to read/write pfm images to and from
 *      opencv Mat image objects
 *
 *      Works with PF color pfm files and Pf grayscale
 *      pfm files
 */

PfmLoader::PfmLoader() {}

void PfmLoader::skipSpace(std::fstream& fileStream) {
    // skip white space in the headers or pnm files

    char c;
    do {
        c = fileStream.get();
    } while (c == '\n' || c == ' ' || c == '\t' || c == '\r');
    fileStream.unget();
}

// check whether machine is little endian
int PfmLoader::littleEndian() {
    int intval = 1;
    uchar *uval = (uchar *)&intval;
    return uval[0] == 1;
}

void PfmLoader::swapBytes(float* fptr) { // if endianness doesn't agree, swap bytes
    uchar* ptr = (uchar *) fptr;
    uchar tmp = 0;
    tmp = ptr[0]; ptr[0] = ptr[3]; ptr[3] = tmp;
    tmp = ptr[1]; ptr[1] = ptr[2]; ptr[2] = tmp;
}

/*
 *  Reads a .pfm file image file into an
 *  opencv Mat structure with type
 *  CV_32F, handles either 1 band or 3 band
 *  images
 *
 *  Params:
 *      im:     type: Mat       description: image destination
 *      path:   type: string    description: file path to pfm file
 */
int PfmLoader::readFilePFM(cv::Mat &im, std::string path) {

    // create fstream object to read in pfm file
    // open the file in binary
    std::fstream file(path.c_str(), std::ios::in | std::ios::binary);

    // init variables
    std::string bands;           // what type is the image   "Pf" = grayscale    (1-band)
                            //                          "PF" = color        (3-band)
    int width, height;      // width and height of the image
    float scalef, fvalue;   // scale factor and temp value to hold pixel value
    cv::Vec3f vfvalue;          // temp value to hold 3-band pixel value

    // extract header information, skips whitespace
    file >> bands;
    file >> width;
    file >> height;
    file >> scalef;

    // determine endianness
    int littleEndianFile = (scalef < 0);
    int littleEndianMachine = littleEndian();
    int needSwap = (littleEndianFile != littleEndianMachine);

    /*
    std::cout << std::setfill('=') << std::setw(19) << "=" << std::endl;
    std::cout << "Reading image to pfm file: " << path << std::endl;
    std::cout << "Little Endian?: "  << ((needSwap) ? "false" : "true")   << std::endl;
    std::cout << "width: "           << width                             << std::endl;
    std::cout << "height: "          << height                            << std::endl;
    std::cout << "scale: "           << scalef                            << std::endl;
    */

    // skip SINGLE newline character after reading third arg
    char c = file.get();
    if (c == '\r')      // <cr> in some files before newline
        c = file.get();
    if (c != '\n') {
        if (c == ' ' || c == '\t' || c == '\r') {
            std::cout << "newline expected";
            return -1;
        } else {
            std::cout << "whitespace expected";
            return -1;
        }
    }

    if (bands == "Pf") {          // handle 1-band image
        /*
        std::cout << "Reading grayscale image (1-band)" << std::endl;
        std::cout << "Reading into CV_32FC1 image" << std::endl;
        */
        im = cv::Mat::zeros(height, width, CV_32FC1);
        for (int i=height-1; i >= 0; --i) {
            for(int j=0; j < width; ++j){
                file.read((char*) &fvalue, sizeof(fvalue));
                if(needSwap){
                    swapBytes(&fvalue);
                }
                im.at<float>(i,j) = (float) fvalue;
            }
        }
    } else if (bands == "PF") {    // handle 3-band image
        /*
        std::cout << "Reading color image (3-band)" << std::endl;
        std::cout << "Reading into CV_32FC3 image" << std::endl;
        */
        im = cv::Mat::zeros(height, width, CV_32FC3);
        for (int i=height-1; i >= 0; --i) {
            for (int j=0; j < width; ++j) {
                file.read((char*) &vfvalue, sizeof(vfvalue));
                if (needSwap) {
                    swapBytes(&vfvalue[0]);
                    swapBytes(&vfvalue[1]);
                    swapBytes(&vfvalue[2]);
                }
                im.at<cv::Vec3f>(i,j) = vfvalue;
            }
        }
    } else {
        std::cout << "unknown bands description";
        return -1;
    }
    /* std::cout << std::setfill('=') << std::setw(19) << "=" << std::endl << std::endl; */
    return 0;
}

