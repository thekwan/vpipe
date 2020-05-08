/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include "mch.hpp"

bool FeatureMatcher::Run(job_context &context) {
    /* TEST CODE: no thread version.
     */
    return RunNoThread(context);

    /* TEST CODE: tile-based thread version
     */
    //return RunTileThread( context );
}

bool FeatureMatcher::RunNoThread(job_context &context) {
    int image_num = context.imageDB.getImageNum();

    cv::namedWindow("Output", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Output", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    for(int i = 0; i < image_num-1; i++) {
        std::shared_ptr<Image> querry = context.imageDB.getImage(i  );
        std::shared_ptr<Image> train  = context.imageDB.getImage(i+1);

        //std::cout << "Matching: [" << querry->getFileName();
        //std::cout << "]  <--->  [" << train->getFileName() << std::endl;

        bfMatching( querry, train );
    }

#if 0
    
    /* Run ORB feature extractor
     */
    //int image_num = context.imageDB.getImageNum();
    cv::Ptr<cv::ORB> h_orb = cv::ORB::create( 3000 );
    for(int i = 0 ; i < image_num ; i++) {
        auto image = context.imageDB.getImage(i);
        std::vector<cv::KeyPoint> kptr;
        cv::Mat desc;
        h_orb->detectAndCompute( *(image->getDataPtr()), cv::noArray(), kptr, desc );

#if 1
        // DEBUG: display found keypointer
        cv::Mat kptr_image;
        cv::drawKeypoints( *(image->getDataPtr()), kptr, kptr_image, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT);
        cv::imshow("Window", kptr_image);
        cv::waitKey(0);
#endif
    }

#endif

	cv::destroyWindow("Output");

    return true;
}

/* Tile-level thread running function
 */
bool FeatureMatcher::RunThread(job_context &context) {
#if 0
    /* Get divided tiles for given image data
     */
    int xdiv = 2;
    int ydiv = 2;
    int level = 2;
    int max_keypoint = 5000;
    for(int i = 0; i < context.imageDB.getImageNum(); i++) {
        std::shared_ptr<Image> image = context.imageDB.getImage(i);
        divideImageIntoTiles( _tileList, image, cv::Size(image->getImageSize()), 
                cv::Point2f(0,0), level, xdiv, ydiv, max_keypoint);
    }


#if 1
    /* Creates Feature extraction threads and runs the threads.
     */
    int thread_num = 5;
    _threadList.clear();

    // Creates thread instances.
    for(int i = 0; i < thread_num; i++)
        _threadList.emplace_back( std::thread(cpu_thread_wrapper, this) );

    // Waiting to stop thread instances
    for( auto &thread: _threadList )
        thread.join();

    // de-initializes thread list
    _threadList.clear();
#else
    /* DEBUG: No thread version.
     */
    extractFeature();
#endif



    /* DEBUG: check each tile info, and display found feature drew image.
     */
#if 0
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    for( auto &tile : _tileList ) {
        // draw image and found key-points
        cv::Mat kpt_image;
        cv::Mat tile_image = cv::Mat(*(tile.image->getDataPtr()), 
                cv::Rect(tile.gPos.x, tile.gPos.y, tile.tile_size.width, tile.tile_size.height));
        cv::drawKeypoints(tile_image, tile._orb_result.keypoints, kpt_image, 
                cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT );
        cv::imshow("Window", kpt_image);
        std::cout << "Image size = " << tile.image->getDataPtr()->size();
        std::cout << "\ttile size = " << tile.tile_size;
        std::cout << "\ttile pos = " << tile.gPos;
        std::cout << "\tmax_kp = " << tile.max_keypoint << std::endl;
        cv::waitKey(0);
    }
	cv::destroyWindow("Window");
#endif


    /* Updates keypoint position with global position of each tile.
     * and copy them into ImageDB's _keypoint_desc variable.
     */
    for( auto &tile : _tileList ) {
        for( auto &kps : tile._orb_result.keypoints ) {
            kps.pt.x += tile.gPos.x;
            kps.pt.y += tile.gPos.y;
        }
        tile.image->addOrbKeyPoints( tile._orb_result.keypoints );
        tile.image->addOrbDescriptors( tile._orb_result.descriptors );
    }


    /* DEBUG: check found feature on whole image (display found features on whole image)
     */
#if 1
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    for(int i = 0; i < context.imageDB.getImageNum(); i++) {
        std::shared_ptr<Image> image = context.imageDB.getImage(i);
        // draw image and found key-points
        cv::Mat kpt_image;
        cv::drawKeypoints(*(image->getDataPtr()), image->getOrbKeyPoints(), kpt_image, 
                cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT );
        cv::imshow("Window", kpt_image);
        cv::waitKey(0);
    }
	cv::destroyWindow("Window");
#endif

#endif

    return true;
}


bool FeatureMatcher::bfMatching(
        std::shared_ptr<Image> querry,
        std::shared_ptr<Image> train ) 
{
    cv::BFMatcher bfmatcher(cv::NORM_HAMMING,true);
#if 0
    std::vector<std::vector<cv::DMatch>> matchout;
    bfmatcher.knnMatch( querry->getOrbKeyPoints(), train->getOrbKeyPoints(), matchout, 2);
#else
    cv::Mat image_matches;
    std::vector<cv::DMatch> matchout;
    bfmatcher.match( querry->getOrbDescriptors(), train->getOrbDescriptors(), matchout, cv::noArray() );

    /* Find minDist, maxDist.
     */
    float minDist = 1000000.0, maxDist = 0;
    for(auto &mch : matchout) {
        if( minDist > mch.distance )
            minDist = mch.distance;
        if( maxDist < mch.distance )
            maxDist = mch.distance;
    }

    /* Remove unreliable matching from the list.
     */
#if 1
    float thrDist = minDist + (maxDist-minDist)/4;
    std::vector<cv::DMatch> matchout2;
    for(std::vector<cv::DMatch>::iterator iter = matchout.begin();
            iter != matchout.end(); iter++) {
        if( iter->distance < thrDist )
            matchout2.push_back( *iter );
        //std::cout << "matchout.dist = " << iter->distance << std::endl;
    }

    std::cout << "minDist = " << minDist;
    std::cout << "\tmaxDist = " << maxDist;
    std::cout << "\tthrDist = " << thrDist << std::endl;
    std::cout << "before remove, matchout.size() = " << matchout.size() << std::endl;
    std::cout << "after remove,  matchout.size() = " << matchout2.size() << std::endl;
#endif

#if 0   // cv::drawMatches
    cv::drawMatches( *(querry->getDataPtr()), querry->getOrbKeyPoints(),
                     *(train->getDataPtr()), train->getOrbKeyPoints(),
                     matchout2, image_matches, cv::Scalar(0,0,255), cv::Scalar(0,0,255),
                     std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
#else
    cv::Mat qGray, tGray;
    cv::cvtColor(*(querry->getDataPtr()), qGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(*(train->getDataPtr()) , tGray, cv::COLOR_BGR2GRAY);

    image_matches = (qGray + tGray)/2;
    cv::cvtColor(image_matches, image_matches, cv::COLOR_GRAY2BGR);

    std::cout << "BF matching results ------------------ \n";
    std::vector<cv::KeyPoint> qk = querry->getOrbKeyPoints();
    std::vector<cv::KeyPoint> tk = train->getOrbKeyPoints();
    for (std::vector<cv::DMatch>::iterator iter = matchout2.begin();
         iter != matchout2.end();
         iter++ ) {
        cv::KeyPoint &qkptr = qk[iter->queryIdx];
        cv::KeyPoint &tkptr = tk[iter->trainIdx];

        //std::cout << "[Q] " << qkptr.pt;
        //std::cout << "   [T] " << tkptr.pt;
        //std::cout << "   [M] " << iter->distance << std::endl;

        //cv::circle(image_matches, qkptr.pt, 2, cv::Scalar(0,0,255));
        cv::circle(image_matches, tkptr.pt, 2, cv::Scalar(0,0,255));
        cv::line(image_matches, qkptr.pt, tkptr.pt, cv::Scalar(0,0,255));
    }
#endif

    cv::imshow("Output", image_matches);
    if (cv::waitKey(33) == 'q') {
	    cv::destroyWindow("Output");
        exit(1);
    }
#endif

    return true;
}

