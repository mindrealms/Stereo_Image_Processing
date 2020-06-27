#include "DepthMap.h"
//#include "boost/range/combine.hpp"

DepthMap::DepthMap(cv::Mat img1, cv::Mat img2) {
    int size = img1.size[0] * img2.size[1];
    _stereo1.reserve(size);
    _stereo2.reserve(size);
    _stereo1 = img1;
    _stereo2 = img2;
    _width = img1.cols;
}

//ADD A DESTRUCTORRRRRR
//ADD A PARSERRRRRR

void DepthMap::generateMap(cv::Mat &newimg1, cv::Mat &newimg2) {

    //Undistort images (???)

    /*   From the paper:
     * "We tried lens distortion models of up to four parameters (radial κ1, κ2;
     * tangential p1, p2), and found that the 2-parameter model without
     * tangential distortion is sufficiently accurate for the lenses we employ." --> ???
     */

    float data1[9] = {3962.004, 0, 1146.717, 0, 3962.004, 975.476, 0, 0, 1};
    cv::Mat img1 = undistortImage(_stereo1, data1);
    float data2[9] = {3962.004, 0, 1254.628, 0, 3962.004, 975.476, 0, 0, 1};
    cv::Mat img2 = undistortImage(_stereo2, data2);

    //find keypoints and descriptors (SIFT)
    std::vector<cv::KeyPoint> key1, key2;
    cv::Mat desc1, desc2;

    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    CV_Assert(sift != NULL);
    sift->detectAndCompute(img1, cv::Mat(), key1, desc1);
    sift->detectAndCompute(img2, cv::Mat(), key2, desc2);

    /* Draw keypoints:
     *
     * cv::Mat out;
     * cv::drawKeypoints(img1, key1, out, cv::Scalar(233,133,176), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
     * cv::drawKeypoints(img2, key2, out, cv::Scalar(233,133,176), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
     */

    //matching descriptor vectors (FLANN)
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher->knnMatch(desc1, desc2, knn_matches, 2);

    //Lowe's test
    std::vector<cv::DMatch> good_matches;
    std::vector<cv::Point2f> pts1, pts2;
    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < LOWE_THRESHOLD * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
            pts1.push_back(key1[knn_matches[i][0].queryIdx].pt);
            pts2.push_back(key2[knn_matches[i][0].trainIdx].pt);
        }
    }

    /* Draw keypoint matches between images:
     *
     * cv::Mat img_matches;
     * drawMatches(img1, key1, img2, key2, good_matches, img_matches, cv::Scalar(211,9,45),
                cv::Scalar(255,141,24), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
     */

    cv::Mat mask;
    cv::Mat f_mat = cv::findFundamentalMat(pts1, pts2, cv::FM_LMEDS, 3, 0.99f, mask);

    //flatten mask
    mask = mask.reshape(1,1);

    //filter out outliers
    cv::Mat inliers1, inliers2;
    int size = mask.rows*mask.cols;
    for (int i=0; i<size; i++) {
        if (mask.at<int>(i) == true) {
            inliers1.push_back(pts1[i]);
            inliers2.push_back(pts2[i]);
        }
    }

    //find epilines corr. to points in image 2 and to be drawn on image 1
    cv::Mat3f lines1, lines2;
    cv::Mat new3, new4, new5, new6;

    cv::computeCorrespondEpilines(pts2, 2, f_mat, lines1);
    drawEpilines(img1, img2, lines1, pts1, pts2, new3, new4);

    cv::computeCorrespondEpilines(pts1, 1, f_mat, lines2);
    drawEpilines(img2, img1, lines2, pts2, pts1, new5, new6);
    newimg1 = new3;
    newimg2 = new5;

}

//draws on img1
void DepthMap::drawEpilines(cv::Mat img1, cv::Mat img2, cv::Mat lines,
                            std::vector<cv::Point2f> pts1, std::vector<cv::Point2f> pts2,
                            cv::Mat &newimg1, cv::Mat &newimg2) {

    //convert to vector
    std::vector<cv::Point3f> array;
    if (lines.isContinuous()) {
       array.assign((cv::Point3f*)lines.data, (cv::Point3f*)lines.data + lines.total()*lines.channels()); //1947x1
    }

    //zip
    cv::Mat g1, g2;
    cv::cvtColor(img1, g1, cv::COLOR_GRAY2BGR);
    cv::cvtColor(img2, g2, cv::COLOR_GRAY2BGR);

    for (int i=0; i<lines.rows; i++) {
        cv::Scalar color((std::rand())*255,(std::rand())*255,(std::rand())*255);
        cv::Point point1(0, -lines.at<cv::Point3f>(i).z/lines.at<cv::Point3f>(i).y);
        cv::Point point2(_width, -(lines.at<cv::Point3f>(i).z + lines.at<cv::Point3f>(i).x*_width)
                         / lines.at<cv::Point3f>(i).y);

        cv::line(g1, point1, point2, color, 1);
        cv::circle(g1, pts1[i], 5, color, -1);
        cv::circle(g2, pts2[i], 5, color, -1);
    }
    newimg1 = g1;
    newimg2 = g2;
}



//cv::Mat DepthMap::renderDepthMap(cv::Mat, ) {

//}

cv::Mat DepthMap::undistortImage(cv::Mat img, void *data) {

    int height = img.size[0];
    int width = img.size[1];
    int imgsz = height * width;
    cv::Size size(height, width);

    cv::Mat undist;
    undist.reserve(imgsz);
    cv::Mat cammat(3, 3, CV_32F, data); //camera matrix


    // ******************** TEMP ***********************

    // I'm not sure where to find distortion coefficients, if there are any
    // (I think these stereo images are already calibrated/undistorted?
    // So for now coeffs = all zero)

    cv::Mat coeffs = cv::Mat().zeros(1,5,0);
    cv::Mat newcammat = cv::getOptimalNewCameraMatrix(cammat, coeffs, size, imgsz);
    cv::undistort(img, undist, cammat, coeffs, newcammat);

    // ********************* TEMP **********************

    return undist;
}





//    cv::Mat g1, g2, disp;

//    //convert to grayscale
//    cv::cvtColor(_stereo1, g1, cv::COLOR_BGR2GRAY);
//    cv::cvtColor(_stereo2, g2, cv::COLOR_BGR2GRAY);

//    cv::MatSize asd = g1.size;

//    disp.create(asd[0],asd[1],0);

//    int n_disp = 288;

//    cv::Ptr<cv::StereoBM> stereo = cv::StereoBM::create(n_disp, 15);
//    //    disparity = stereo.compute(imgL,imgR)
//    //    stereo->setMinDisparity(33);

//    stereo->compute(g1, g2, disp);





    //    Mat im_left=stereo1;
    //    Mat im_right=stereo2;
    //    cv::Size imagesize = im_left.size();
    //    cv::Mat disparity_left=cv::Mat(imagesize.height,imagesize.width,CV_16S);
    //    cv::Mat disparity_right=cv::Mat(imagesize.height,imagesize.width,CV_16S);
    //    cv::Mat g1,g2,disp,disp8;
    //    cv::cvtColor(im_left,g1,cv::COLOR_BGR2GRAY);
    //    cv::cvtColor(im_right,g2,cv::COLOR_BGR2GRAY);
    //    cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create(0,21);
    //    sbm->setDisp12MaxDiff(1);
    //    sbm->setSpeckleRange(8);
    //    sbm->setSpeckleWindowSize(9);
    //    sbm->setUniquenessRatio(0);
    //    sbm->setTextureThreshold(507);
    //    sbm->setMinDisparity(-39);
    //    sbm->setPreFilterCap(61);
    //    sbm->setPreFilterSize(5);
    //    sbm->compute(g1,g2,disparity_left);
    //    normalize(disparity_left, disp8, 0, 255, CV_MINMAX, CV_8U);
    //    cv::namedWindow("Left",CV_WINDOW_FREERATIO);
    //    cv::imshow("Left", im_left);
    //    cv::namedWindow("Right",CV_WINDOW_FREERATIO);
    //    cv::imshow("Right", im_right);
    //    cv::namedWindow("Depth map",CV_WINDOW_FREERATIO);
    //    cv::imshow("Depth map", disp8);
    //    cv::waitKey(0);

