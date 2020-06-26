#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>

#include "DepthMap.h"
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("stereo1", "Input stereo image 1");
    parser.addPositionalArgument("stereo2", "Input stereo image 2");
    parser.addPositionalArgument("output", "Output depth map image"); //TEMP
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if(args.size() != 3) {
        std::cerr << "Error: Wrong number of arguments" << std::endl;
        a.exit(1);
        return 1;
    }

    //input stereo files & output rendered file paths
    cv::String f1 = args[0].toUtf8().constData();
    cv::String f2 = args[1].toUtf8().constData();
    cv::String output = args[2].toUtf8().constData();

    //load images
    cv::Mat stereo1, stereo2;
    stereo1 = cv::imread(f1, cv::IMREAD_COLOR);
    if(stereo1.empty()) {
        std::cout << "Could not read the image: " << f1 << std::endl;
        return 1;
    }
    stereo2 = cv::imread(f2, cv::IMREAD_COLOR);
    if(stereo2.empty()) {
        std::cout << "Could not read the image: " << f2 << std::endl;
        return 1;
    }

    cv::Mat g1, g2, disp;

    //convert to grayscale
    cv::cvtColor(stereo1, g1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(stereo2, g2, cv::COLOR_BGR2GRAY);

    cv::MatSize mat = g1.size;
    disp.create(mat[0],mat[1],0);

    int n_disp = 192; //must be divisible by 16
    cv::Ptr<cv::StereoBM> stereo = cv::StereoBM::create(n_disp, 15);
    stereo->compute(g1, g2, disp);

    cv::String dmr = "Depth Map Reprojection";
    cv::namedWindow(dmr, cv::WINDOW_NORMAL);
    cv::resizeWindow(dmr, 600, 450);
    cv::imshow(dmr, disp);

    int k = cv::waitKey(0);
    switch(k){
    case 's': {
        cv::imwrite(output, disp);
        break;
    }
    case 'q': {
        cv::destroyWindow(dmr);
        break;
    }
    }

    a.exit();
}


//    cv::String dmr_2 = "Depth Map Reprojection 2";
//    cv::namedWindow(dmr_2, cv::WINDOW_NORMAL);
//    cv::resizeWindow(dmr_2, 600, 450);
//    cv::imshow(dmr_2, img2);
