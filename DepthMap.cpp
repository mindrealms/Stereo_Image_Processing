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
    drawEpilines(img1, lines1, pts1, newimg1);

    cv::computeCorrespondEpilines(pts1, 1, f_mat, lines2);
    drawEpilines(img2, lines2, pts2, newimg2);
}

//draws on img1
void DepthMap::drawEpilines(cv::Mat img, cv::Mat lines, std::vector<cv::Point2f> pts, cv::Mat &newimg) {

    //convert to vector
    std::vector<cv::Point3f> array;
    if (lines.isContinuous()) {
       array.assign((cv::Point3f*)lines.data, (cv::Point3f*)lines.data + lines.total()*lines.channels()); //1947x1
    }

    //zip
    cv::Mat temp;
    cv::cvtColor(img, temp, cv::COLOR_GRAY2BGR);

    for (int i=0; i<lines.rows; i++) {
        cv::Scalar color((std::rand())*255,(std::rand())*255,(std::rand())*255);
        cv::Point point1(0, -lines.at<cv::Point3f>(i).z/lines.at<cv::Point3f>(i).y);
        cv::Point point2(_width, -(lines.at<cv::Point3f>(i).z + lines.at<cv::Point3f>(i).x*_width)
                         / lines.at<cv::Point3f>(i).y);

        cv::line(temp, point1, point2, color, 1);
        cv::circle(temp, pts[i], 5, color, -1);
    }
    newimg = temp;
}


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
