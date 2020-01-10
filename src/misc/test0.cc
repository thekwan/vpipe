/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include "test0.hpp"

Test0::Test0(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
}

bool Test0::Run(job_context &context) {
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

    return true;
}

bool Test0::Stop(void) {
    return true;
}

