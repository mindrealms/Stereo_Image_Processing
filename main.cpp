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

    DepthMap dm_util(stereo1, stereo2);
    cv::Mat depth_map = dm_util.generateMap();

    cv::String windowName = "Depth Map Reprojection";
    cv::namedWindow(windowName);

    cv::imshow(windowName, depth_map);
    int k = cv::waitKey(0);
    if(k == 's') {
        cv::imwrite(output, depth_map);
    }

    cv::destroyWindow(windowName);

//    MainWindow w;
//    w.show();

    a.exit();
}
