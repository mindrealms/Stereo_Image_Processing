#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <pthread.h>

#include "StereoUtils.h"
#include "PfmLoader.h"
#include "CalibParser.h"

#define WINDOW_W 800
#define WINDOW_H 700

cv::String _windowname = "Depth Map Reprojection";
cv::Mat _stereo1, _stereo2,
        _pfm1, _pfm2,
        _epi1, _epi2,
        _reproj3d;

typedef struct ButtonData {
    int state = 0;
    pthread_mutex_t mtx;
    cv::String file;
} ButtonData;


/** CHECK ALL THIS MUTEX RELATED STUFF PLEASE */

void stereoButtonCall1(int state, void *data) {

    //converting user data to the right type
    ButtonData *ptr = (ButtonData *)data;
    if (!ptr) {
        std::cout << "Stereo Button 1 user data is empty" << std::endl;
        return;
    }
    pthread_mutex_lock(&ptr->mtx);
    std::cout << ptr->state << std::endl;
    ptr->state = state;
    if (_stereo1.empty()) {
        std::cout << "Could not read stereo 1 image" << std::endl; return;
    }
    cv::imshow(_windowname, _stereo1);
    pthread_mutex_unlock(&ptr->mtx);
}

void stereoButtonCall2(int state, void *data) {

    //converting user data to the right type
    ButtonData *ptr = (ButtonData *)data;
    if (!ptr) {
        std::cout << "Stereo Button 1 user data is empty" << std::endl;
        return;
    }
    pthread_mutex_lock(&ptr->mtx);
    ptr->state = state;
    if (_stereo2.empty()) {
        std::cout << "Could not read stereo 2 image" << std::endl; return;
    }
    cv::imshow(_windowname, _stereo2);
    pthread_mutex_unlock(&ptr->mtx);
}

void epilineButtonCall1(int state, void *data) {

    ButtonData *ptr = (ButtonData *)data;
    if (!ptr) {
        std::cout << "Epiline Button 1 user data is empty" << std::endl;
        return;
    }
    pthread_mutex_lock(&ptr->mtx);
    ptr->state = state;
    if (_epi1.empty()) {
        std::cout << "Computation of epilines failed" << std::endl; return;
    }
    cv::imshow(_windowname, _epi1);
    pthread_mutex_unlock(&ptr->mtx);
}

void epilineButtonCall2(int state, void *data) {

    ButtonData *ptr = (ButtonData *)data;
    if (!ptr) {
        std::cout << "Epiline Button 2 user data is empty" << std::endl;
        return;
    }
    pthread_mutex_lock(&ptr->mtx);
    ptr->state = state;
    if (_epi2.empty()) {
        std::cout << "Computation of epilines failed" << std::endl; return;
    }
    cv::imshow(_windowname, _epi2);
    pthread_mutex_unlock(&ptr->mtx);
}

void mapButtonCall1(int state, void *data) {

    ButtonData *ptr = (ButtonData *)data;
    if (!ptr) {
        std::cout << "Disparity Map Button 1 user data is empty" << std::endl;
        return;
    }
    pthread_mutex_lock(&ptr->mtx);
    ptr->state = state;
    if (_pfm1.empty()) {
        std::cout << "Could not read pfm 1 image" << std::endl; return;
    }
    cv::imshow(_windowname, _pfm1);
    pthread_mutex_unlock(&ptr->mtx);
}

void mapButtonCall2(int state, void *data) {

    ButtonData *ptr = (ButtonData *)data;
    if (!ptr) {
        std::cout << "Disparity Map Button 2 user data is empty" << std::endl;
        return;
    }
    pthread_mutex_lock(&ptr->mtx);
    ptr->state = state;
    if (_pfm2.empty()) {
        std::cout << "Could not read pfm 2 image" << std::endl; return;
    }
    cv::imshow(_windowname, _pfm2);
    pthread_mutex_unlock(&ptr->mtx);
}

void reprojButtonCall(int state, void *data) {

    ButtonData *ptr = (ButtonData *)data;
    if (!ptr) {
        std::cout << "Reprojection Button user data is empty" << std::endl;
        return;
    }
    pthread_mutex_lock(&ptr->mtx);
    ptr->state = state;


    /** pop up an Open3D window */

    pthread_mutex_unlock(&ptr->mtx);

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("stereo1", "Input stereo image 1");
    parser.addPositionalArgument("stereo2", "Input stereo image 2");
    parser.addPositionalArgument("pfm1", "Input pfm image 1");
    parser.addPositionalArgument("pfm2", "Input pfm image 2");
    parser.addPositionalArgument("calib", "Calibration text file");
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if(args.size() != 5) {
        std::cerr << "Error: Wrong number of arguments" << std::endl;
        a.exit(1);
        return 1;
    }

    //input stereo, pfm image, and calib.txt file paths
    cv::String f1 = args[0].toUtf8().constData();
    cv::String f2 = args[1].toUtf8().constData();
    cv::String pfm1 = args[2].toUtf8().constData();
    cv::String pfm2 = args[3].toUtf8().constData();
    cv::String calib = args[4].toUtf8().constData();

    //load calib.txt data
    cv::Mat g1, g2;
    CalibParser::CalibData data;
    CalibParser::parseFile(calib, data);

    //load stereo imges
    _stereo1 = cv::imread(f1, cv::IMREAD_COLOR);
    _stereo2 = cv::imread(f2, cv::IMREAD_COLOR);

    //compute and set epiline imges
    cv::cvtColor(_stereo1, g1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(_stereo2, g2, cv::COLOR_BGR2GRAY);
    StereoUtils utils(g1, g2);
    utils.renderEpilines(data.cammat1, data.cammat2, _epi1, _epi2);

    //load pfm disparity maps
    cv::Mat temp1, temp2;
    PfmLoader loader = PfmLoader();
    loader.readFilePFM(temp1, pfm1);
    _pfm1 = temp1 / 255.0;
    loader.readFilePFM(temp2, pfm2);
    _pfm2 = temp2 / 255.0;

    //set up window and buttons
    cv::namedWindow(_windowname, cv::WINDOW_NORMAL);
    cv::resizeWindow(_windowname, WINDOW_W, WINDOW_H);

    ButtonData stereo_data1, stereo_data2, epiline_data1, epiline_data2,
               map_data1, map_data2, reproj_data;
    reproj_data.file = calib;

    cv::createButton("Stereo Img 1", stereoButtonCall1, &stereo_data1, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Stereo Img 2", stereoButtonCall2, &stereo_data2, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Draw Epilines Img 1", epilineButtonCall1, &epiline_data1, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Draw Epilines Img 2", epilineButtonCall2, &epiline_data2, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Disparity Map 1", mapButtonCall1, &map_data1, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Disparity Map 2", mapButtonCall2, &map_data2, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("3D Reprojection", reprojButtonCall, &reproj_data, cv::QT_PUSH_BUTTON, 0);

    return a.exec();
}
