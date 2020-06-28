#include "StereoUtils.h"

StereoUtils::StereoUtils(cv::Mat img1, cv::Mat img2) :
    _stereo1(img1), _stereo2(img2), _width(img1.cols) {}

StereoUtils::~StereoUtils(){
    _stereo1.release();
    _stereo2.release();
    _width = 0;
}

void StereoUtils::renderEpilines(cv::Mat data1, cv::Mat data2, cv::Mat &newimg1, cv::Mat &newimg2) {

    // ******************** START TEMP ***********************
    /*   From the paper:
     * "We tried lens distortion models of up to four parameters (radial κ1, κ2;
     * tangential p1, p2), and found that the 2-parameter model without
     * tangential distortion is sufficiently accurate for the lenses we employ."
     *
     * Do I need to undistort? And if so what are the coeffs?
     */
    _stereo1 = undistortImage(_stereo1, data1);
    _stereo2 = undistortImage(_stereo2, data2);
    // ********************* END TEMP **********************

    //find keypoints and descriptors (SIFT)
    std::vector<cv::KeyPoint> key1, key2;
    cv::Mat desc1, desc2;

    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    CV_Assert(sift != NULL);
    sift->detectAndCompute(_stereo1, cv::Mat(), key1, desc1);
    sift->detectAndCompute(_stereo2, cv::Mat(), key2, desc2);

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
    std::vector<cv::Scalar> colors(lines1.rows);

    cv::computeCorrespondEpilines(pts2, 2, f_mat, lines1);
    draw(_stereo1, lines1, pts1, newimg1, colors);

    cv::computeCorrespondEpilines(pts1, 1, f_mat, lines2);
    draw(_stereo2, lines2, pts2, newimg2, colors);
}

//draws on img1
void StereoUtils::draw(cv::Mat img, cv::Mat lines, std::vector<cv::Point2f> pts, cv::Mat &newimg,
                       std::vector<cv::Scalar> &colors) {

    //convert to vector
    std::vector<cv::Point3f> array;
    if (lines.isContinuous()) {
       array.assign((cv::Point3f*)lines.data, (cv::Point3f*)lines.data + lines.total()*lines.channels()); //1947x1
    }

    //zip
    cv::Mat temp;
    cv::cvtColor(img, temp, cv::COLOR_GRAY2BGR);
    cv::Scalar color;

    for (int i=0; i<lines.rows; i++) {
        if (static_cast<int>(colors.size()) != lines.rows) {
            color = cv::Scalar((std::rand())*255,(std::rand())*255,(std::rand())*255);
            colors.push_back(color);
        } else {
            color = colors.at(i);
        }

        cv::Point point1(0, -lines.at<cv::Point3f>(i).z/lines.at<cv::Point3f>(i).y);
        cv::Point point2(_width, -(lines.at<cv::Point3f>(i).z + lines.at<cv::Point3f>(i).x*_width)
                         / lines.at<cv::Point3f>(i).y);

        cv::line(temp, point1, point2, color, 1);
        cv::circle(temp, pts[i], 5, color, -1);
    }
    newimg = temp;
}


cv::Mat StereoUtils::undistortImage(cv::Mat img, cv::Mat data) {

    /*
     * I've stored the data.cammatX as cv::Mat's for the purposes of the reprojection
     * calculations which benefit more from this type. Apologies for the whole
     * cv::Mat --> float * here.
     */

    float flat_data[CAMMAT_SZ] = {data.at<float>(0,0), data.at<float>(0,1), data.at<float>(0,2),
                             data.at<float>(1,0), data.at<float>(1,1), data.at<float>(1,2),
                             data.at<float>(2,0), data.at<float>(2,1), data.at<float>(2,2)};

    int height = img.size[0];
    int width = img.size[1];
    int imgsz = height * width;
    cv::Size size(height, width);

    cv::Mat undist;
    undist.reserve(imgsz);
    cv::Mat cammat(3, 3, CV_32F, flat_data); //camera matrix

    //for now, all coeffs = zero
    cv::Mat coeffs = cv::Mat().zeros(1,5,0);
    cv::Mat newcammat = cv::getOptimalNewCameraMatrix(cammat, coeffs, size, imgsz);
    cv::undistort(img, undist, cammat, coeffs, newcammat);

    //also here once again, if you do end up undistorting, check for potential ownership issues
    return undist;
}
