/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <opencv2/aruco.hpp>
#include "calib.hpp"

CameraCalibration::CameraCalibration(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
}

bool CameraCalibration::Run(job_context &context) {
    TestRun0( context );
    //TestRun1( context );

    return true;
}

bool CameraCalibration::Stop(void) {
    return true;
}

void CameraCalibration::TestRun0(job_context &context) {
    int image_num = context.imageDB.getImageNum();

    /* Open image display window.
     */
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    /* Get the image position of found calibration chessboard corners.
     */
    cv::Size boardSize(9,6);
    cv::Size imageSize;
    std::vector<std::vector<cv::Point2f>> imagePoints;

    for(int i = 0; i < image_num; i++) {
        std::cout << "------- " << i << "'th image processing ------" << std::endl;
        std::shared_ptr<Image> img = context.imageDB.getImage(i);
        cv::Mat img_src( *(img->getDataPtr()) );
        imageSize = img_src.size();

        //cv::resize( img_src, img_src, cv::Size(img_src.cols/2, img_src.rows/2), 0, 0, CV_INTER_CUBIC);

        // Calibration initial info settings
        std::vector<cv::Point2f> pointBuf;
        int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;

        bool found = findChessboardCorners( img_src, boardSize, pointBuf, chessBoardFlags );
        std::cout << "pointBuf.size() = " << pointBuf.size() << std::endl;

        if( found ) {
            std::cout << "Found all chess board corner position." << std::endl;
            cv::Mat img_gray;
            cv::cvtColor( img_src, img_gray, cv::COLOR_BGR2GRAY );
            cv::cornerSubPix( img_gray, pointBuf, cv::Size(11,11),
                    cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::COUNT, 30, 0.1 ) );

            imagePoints.push_back( pointBuf );
        }

        cv::drawChessboardCorners( img_src, boardSize, cv::Mat(pointBuf), found );

        /* Display image 
         */
        //cv::imshow("Window", img_src);
        //cv::waitKey(0);
    }

    /* Get the world coordinate position of the chessboard.
     */
    int square_size = 5000;
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<cv::Point3f> objPoints;
    for(int i = 0; i < boardSize.height; i++)
        for(int j = 0; j < boardSize.width; j++)
            objPoints.push_back( cv::Point3f(j*square_size, i*square_size, 0) );
    objectPoints.resize( imagePoints.size(), objPoints );

    /* Camera calibration
     * Get intrinsic/extrinsic matrix of camera.
     */
    cv::Mat cameraMatrix(4,4,CV_64F);
    cv::Mat distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    int calibFlags = 0;
    cv::calibrateCamera( objectPoints, imagePoints, imageSize, 
            cameraMatrix, distCoeffs, rvecs, tvecs, calibFlags);

    std::cout << "Image size     = (" << imageSize.width << " , " << imageSize.height << ")" << std::endl;
    std::cout << "Camera matrix = " << cameraMatrix << std::endl;
    std::cout << "Focal length X = " << cameraMatrix.at<double>(0,0) << std::endl;
    std::cout << "Focal length Y = " << cameraMatrix.at<double>(1,1) << std::endl;
    std::cout << "Camera pos X   = " << cameraMatrix.at<double>(0,2) << std::endl;
    std::cout << "Camera pos Y   = " << cameraMatrix.at<double>(1,2) << std::endl;
    std::cout << "Distor coeffs  = " << distCoeffs << std::endl;
    std::cout << "rvecs.size     = " << rvecs.size() << std::endl;
    std::cout << "tvecs.size     = " << tvecs.size() << std::endl;

    /* Project Axis on image.
     */
    // Set axis position on world coordinate.
    std::vector<cv::Point3f> axisPoints;
    axisPoints.emplace_back( cv::Point3f( 0, 0, 0 ) );
    axisPoints.emplace_back( cv::Point3f( -square_size, 0, 0 ) );
    axisPoints.emplace_back( cv::Point3f( 0, -square_size, 0 ) );
    axisPoints.emplace_back( cv::Point3f( 0, 0, -square_size ) );

    std::cout << "== check camera parameter for each scene ==" << std::endl;

    for(int i = 0; i < rvecs.size(); i++) {
        std::vector<cv::Point2f> projPoints;
        cv::projectPoints( axisPoints, rvecs[i], tvecs[i], cameraMatrix, distCoeffs, projPoints );

        std::cout << "tvecs = " << tvecs[i] << std::endl;

        /* Draw axis on the image.
         */
        std::shared_ptr<Image> img = context.imageDB.getImage(i);
        cv::Mat img_src( *(img->getDataPtr()) );
        cv::line( img_src, projPoints[0], projPoints[1], cv::Scalar(255, 0, 0), 4 );
        cv::line( img_src, projPoints[0], projPoints[2], cv::Scalar(0, 255, 0), 4 );
        cv::line( img_src, projPoints[0], projPoints[3], cv::Scalar(0, 0, 255), 4 );

        /* Display image 
         */
        cv::imshow("Window", img_src);
        cv::waitKey(0);
    }

    cv::destroyWindow("Window");

    return;
}

void CameraCalibration::TestRun1(job_context &context) {
    return;
}
