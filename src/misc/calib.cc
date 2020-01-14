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


    for(int i = 0; i < image_num; i++) {
        std::cout << "------- " << i << "'th image processing ------" << std::endl;
        std::shared_ptr<Image> img = context.imageDB.getImage(i);
        cv::Mat img_src( *(img->getDataPtr()) );

        //cv::resize( img_src, img_src, cv::Size(img_src.cols/2, img_src.rows/2), 0, 0, CV_INTER_CUBIC);

        // Calibration initial info settings
        std::vector<cv::Point2f> pointBuf;
        int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
        cv::Size boardSize(9,6);

        bool found = findChessboardCorners( img_src, boardSize, pointBuf, chessBoardFlags );
        std::cout << "pointBuf.size() = " << pointBuf.size() << std::endl;

        if( found ) {
            std::cout << "Found all chess board corner position." << std::endl;
            cv::Mat img_gray;
            cv::cvtColor( img_src, img_gray, cv::COLOR_BGR2GRAY );
            cv::cornerSubPix( img_gray, pointBuf, cv::Size(11,11),
                    cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::COUNT, 30, 0.1 ) );
        }

        cv::drawChessboardCorners( img_src, boardSize, cv::Mat(pointBuf), found );

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
