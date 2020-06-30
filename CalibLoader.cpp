#include "CalibLoader.h"

CalibLoader::CalibLoader() {}

void CalibLoader::populateMatrix(std::stringstream &stream, cv::Mat &matrix) {

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

void CalibLoader::parseFile(cv::String file, CalibData &data) {

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
