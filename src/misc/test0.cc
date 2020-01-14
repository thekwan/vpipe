/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <opencv2/aruco.hpp>
#include "test0.hpp"

Test0::Test0(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
}

bool Test0::Run(job_context &context) {
    //TestRun0( context );
    TestRun1( context );

    return true;
}

bool Test0::Stop(void) {
    return true;
}

void Test0::TestRun0(job_context &context) {
    int image_num = context.imageDB.getImageNum();

    /* Open image display window.
     */
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);


    for(int i = 0; i < image_num; i++) {
        std::shared_ptr<Image> img = context.imageDB.getImage(i);
        cv::Mat img_src, img_dst;

        /* Convert image format (from colored to gray-scale)
         */
        cv::cvtColor(*(img->getDataPtr()), img_src, CV_BGR2GRAY);

        /* Adaptive threshold
         */
        cv::adaptiveThreshold( img_src, img_dst, 
                255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 91, 10);


        /* Find a contour
         */
        cv::Mat img_contour = cv::Mat::zeros(img_dst.size(), CV_8U);
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours( img_dst, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
        cv::drawContours( img_contour, contours, -1, cv::Scalar(255), 2, 8);

        std::cout << "Contour list: " << std::endl;
        for(auto &a : contours) {
            std::cout << "[";
            for( auto &b : a) {
                std::cout << b << "\t";
            }
            std::cout << "]\n";
        }
        std::cout << "\n";

        /* Display image 
         */
        //cv::imshow("Window", img_dst);
        cv::imshow("Window", img_contour);
        cv::waitKey(0);
    }

    cv::destroyWindow("Window");

    return;
}

void Test0::TestRun1(job_context &context) {
    int image_num = context.imageDB.getImageNum();

    /* Open image display window.
     */
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    /* Marker corder 3D points to solve pose estimation problem.
     */
    std::vector<cv::Point3f> marker_corner_3d;
    marker_corner_3d.push_back( cv::Point3f(-0.5f,  0.5f, 0) );
    marker_corner_3d.push_back( cv::Point3f( 0.5f,  0.5f, 0) );
    marker_corner_3d.push_back( cv::Point3f( 0.5f, -0.5f, 0) );
    marker_corner_3d.push_back( cv::Point3f(-0.5f, -0.5f, 0) );


    for(int i = 0; i < image_num; i++) {
        std::shared_ptr<Image> img = context.imageDB.getImage(i);
        cv::Mat img_src(*(img->getDataPtr())), img_resized;

        /* Convert image format (from colored to gray-scale)
         */
        //cv::cvtColor(*(img->getDataPtr()), img_src, CV_BGR2GRAY);
        cv::resize( img_src, img_resized, cv::Size(img_src.cols/4, img_src.rows/4), 0, 0, CV_INTER_NN);

        /* Aruco marker detector official function
         */
        int dictionaryId = 10;
        cv::Ptr<cv::aruco::Dictionary> dictionary =
            cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
        std::vector<std::vector<cv::Point2f>> marker_corners, reject_corners;
        std::vector<int> marker_ids;
        cv::Ptr<cv::aruco::DetectorParameters> arucoDetectorParams = 
            cv::aruco::DetectorParameters::create();
        cv::aruco::detectMarkers( img_resized, dictionary,
                marker_corners, marker_ids, arucoDetectorParams, reject_corners );

        /* Draw detected markers on the image.
         */
        if( marker_ids.size() > 0 )
            cv::aruco::drawDetectedMarkers( img_resized, marker_corners, marker_ids );
            //cv::aruco::drawDetectedMarkers( img_resized, marker_corners );

#if 0   // not implemented yet.
        for( auto &m : marker_corners ) {
        cv::Mat rot_vec, trn_vec;
        cv::solvePnP( marker_corner_3d, m, camMatrix, distCoeffs, rot_vec, trn_vec );
#endif


        /* Display image 
         */
        cv::imshow("Window", img_resized);
        //cv::imshow("Window", img_contour);
        cv::waitKey(0);
    }

    cv::destroyWindow("Window");

    return;
}
