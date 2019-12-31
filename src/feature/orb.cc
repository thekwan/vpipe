/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <opencv2/opencv.hpp>
#include "orb.hpp"

bool FeatureExtractorOrb::Run(job_context &context) {
    /* TEST CODE: no thread version.
     */
    return RunNoThread(context);
}

bool FeatureExtractorOrb::RunNoThread(job_context &context) {

    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    
    /* Run ORB feature extractor
     */
    auto img_db= context.images;
    int image_num = img_db->getImageNum();
    cv::Ptr<cv::ORB> h_orb = cv::ORB::create( 100000 );
    for(int i = 0 ; i < image_num ; i++) {
        auto image = img_db->getImage(i);
        std::vector<cv::KeyPoint> kptr;
        cv::Mat desc;
        h_orb->detectAndCompute( image->getDataPtr(), cv::noArray(), kptr, desc );

#if 1
        //std::cout << "keypoint # is " << kptr.size() << "\t";
        //std::cout << "desc size is " << desc.size() << "\n";
        // DEBUG: display found keypointer
        cv::Mat kptr_image;
        cv::drawKeypoints( image->getDataPtr(), kptr, kptr_image, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT);
        cv::imshow("Window", kptr_image);
        cv::waitKey(0);
#endif
    }

	cv::destroyWindow("Window");

    return true;
}
