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
    if(args.size() != 4) {
        std::cerr << "Error: Wrong number of arguments" << std::endl;
        a.exit(1);
        return 1;
    }

    //input stereo files & output rendered file paths
    cv::String f1 = args[0].toUtf8().constData();
    cv::String f2 = args[1].toUtf8().constData();
    cv::String output1 = args[2].toUtf8().constData();
    cv::String output2 = args[3].toUtf8().constData();

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
    cv::Mat img1, img2;
    dm_util.generateMap(img1, img2);

    cv::String dmr_1 = "Depth Map Reprojection 1";
    cv::namedWindow(dmr_1);

    cv::String dmr_2 = "Depth Map Reprojection 2";
    cv::namedWindow(dmr_2);


    cv::imshow(dmr_1, img1);
    cv::imshow(dmr_2, img2);
    int k = cv::waitKey(0);
    if(k == 's') {
        cv::imwrite(output1, img1);
        cv::imwrite(output2, img2);
    }

    cv::destroyWindow(dmr_1);
    cv::destroyWindow(dmr_2);

//    MainWindow w;
//    w.show();

    a.exit();
}
