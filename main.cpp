#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <pthread.h>
#include <QStringList>
#include <QProcess>

#include "StereoUtils.h"
#include "PfmLoader.h"
#include "CalibLoader.h"
#include "Reprojection.h"

#define WINDOW_W 800
#define WINDOW_H 700

cv::String _windowname = "Depth Map Reprojection";
cv::Mat _stereo1, _stereo2,
        _pfm1, _pfm2,
        _epi1, _epi2,
        _key1, _key2,
        _matchimage,
        _reproj3d;

typedef struct ButtonData {
    int state = 0; //??????????
    cv::Mat file;
} ButtonData;


void buttonCall(int state, void *data) {
    ButtonData *d = (ButtonData *)data;
    if (!d) {
        std::cout << "Error: ButtonCall user data is empty" << std::endl;
        return;
    }
    if (d->file.empty()) {
        std::cout << "Error: Could not read image to be rendered" << std::endl; return;
    }
    cv::imshow(_windowname, d->file);
}

void reprojButtonCall(int state, void *data) {
    if (!data) {
        std::cout << "Reprojection Button user data is empty" << std::endl;
        return;
    }

    /** link python script for Open3D window */
    QString path = "/Users/marky/Desktop/BVC/dmr_project/depth_map_reproj";
    QString  command("python");
    QStringList params = QStringList() << "reprojection_script.py";

    QProcess *process = new QProcess();
    process->startDetached(command, params, path);
    process->waitForFinished();
    process->close();
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
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
    CalibData data;
    CalibLoader cloader;
    cloader.parseFile(calib, data);

    //load stereo imges
    _stereo1 = cv::imread(f1, cv::IMREAD_COLOR);
    _stereo2 = cv::imread(f2, cv::IMREAD_COLOR);

    //compute and set epiline imges
    cv::cvtColor(_stereo1, g1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(_stereo2, g2, cv::COLOR_BGR2GRAY);
    StereoUtils utils(g1, g2);
    utils.renderEpilines(data.cammat1, data.cammat2, _epi1, _epi2,
                         _key1, _key2, _matchimage);

    //load pfm disparity maps
    cv::Mat temp1, temp2;
    PfmLoader loader = PfmLoader();
    loader.readFilePFM(temp1, pfm1);
    _pfm1 = temp1 / 255.0;
    loader.readFilePFM(temp2, pfm2);
    _pfm2 = temp2 / 255.0;

    //generate depth map
//    StereoUtils::generateDepthMap(_pfm1, data);

    //set up window and buttons
    cv::namedWindow(_windowname, cv::WINDOW_NORMAL);
    cv::resizeWindow(_windowname, WINDOW_W, WINDOW_H);

    ButtonData stereo_data1, stereo_data2, epiline_data1, epiline_data2,
               key_data1, key_data2, match_data, map_data1, map_data2, reproj_data;
    stereo_data1.file = _stereo1;
    stereo_data2.file = _stereo2;
    epiline_data1.file = _epi1;
    epiline_data2.file = _epi2;
    key_data1.file = _key1;
    key_data2.file = _key2;
    match_data.file = _matchimage;
    map_data1.file = _pfm1;
    map_data2.file = _pfm2;
    //reproj_data = ...

    cv::createButton("Stereo 1", buttonCall, &stereo_data1, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Stereo 2", buttonCall, &stereo_data2, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Keypoints 1", buttonCall, &key_data1, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Keypoints 2", buttonCall, &key_data2, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Keypoint Matches", buttonCall, &match_data, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Draw Epilines 1", buttonCall, &epiline_data1, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Draw Epilines 2", buttonCall, &epiline_data2, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Disparity Map 1", buttonCall, &map_data1, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("Disparity Map 2", buttonCall, &map_data2, cv::QT_PUSH_BUTTON, 0);
    cv::createButton("3D Reprojection", reprojButtonCall, &reproj_data, cv::QT_PUSH_BUTTON, 0);

    return a.exec();
}
