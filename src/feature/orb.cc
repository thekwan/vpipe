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
    //return RunNoThread(context);

    /* TEST CODE: tile-based thread version
     */
    return RunTileThread( context );
}

bool FeatureExtractorOrb::RunNoThread(job_context &context) {

    //cv::namedWindow("Window", cv::WINDOW_NORMAL);
    //cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    
    /* Run ORB feature extractor
     */
    int image_num = context.imageDB.getImageNum();
    cv::Ptr<cv::ORB> h_orb = cv::ORB::create( 1000 );
    for(int i = 0 ; i < image_num ; i++) {
        auto image = context.imageDB.getImage(i);
        std::vector<cv::KeyPoint> kptr;
        cv::Mat desc;
        h_orb->detectAndCompute( *(image->getDataPtr()), cv::noArray(), kptr, desc );

#if 0
        // DEBUG: display found keypointer
        cv::Mat kptr_image;
        cv::drawKeypoints( *(image->getDataPtr()), kptr, kptr_image, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT);
        cv::imshow("Window", kptr_image);
        cv::waitKey(0);
#endif
    }

	//cv::destroyWindow("Window");

    return true;
}

/* Tile-level thread running function
 */
bool FeatureExtractorOrb::RunTileThread(job_context &context) {

    /* Get divided tiles for given image data
     */
    int xdiv = 2;
    int ydiv = 2;
    int level = 1;
    int max_keypoint = 3000;
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
        //_threadList.emplace_back( std::thread(cpu_thread_wrapper, this) );
        _threadList.push_back( std::thread(cpu_thread_wrapper, this) );

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
#if 0
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    for(int i = 0; i < context.imageDB.getImageNum(); i++) {
        std::shared_ptr<Image> image = context.imageDB.getImage(i);
        // draw image and found key-points
        cv::Mat kpt_image;
        cv::drawKeypoints(*(image->getDataPtr()), image->getOrbKeyPoints(), kpt_image, 
                cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT );
        cv::imshow("Window", kpt_image);
        cv::waitKey(1);
    }
	cv::destroyWindow("Window");
#endif


    return true;
}

/* Recursive function to divide the given image into several tiles
 */
void FeatureExtractorOrb::divideImageIntoTiles(
        std::vector<imageTile> &tiles,
        std::shared_ptr<Image> image, 
        cv::Size itSize,    // input tile size
        cv::Point2f gPos,
        int level, int xdiv, int ydiv,
        int max_keypoint )
{
    int unit_max_keypoint = (max_keypoint + (xdiv*ydiv)-1) / (xdiv*ydiv);
    //int size_w = tile_size.width;
    //int size_h = tile_size.height;
    cv::Size utSize, otSize;
    utSize.width  = (itSize.width + xdiv-1) / xdiv;
    utSize.height = (itSize.height+ ydiv-1) / ydiv;

    for(int i = 0; i < xdiv; i++) {
        int pos_x = utSize.width * i;
        otSize.width = (pos_x+utSize.width > itSize.width) ? (itSize.width - pos_x) : utSize.width;

        for(int j = 0; j < ydiv; j++) {
            int pos_y = utSize.height * j;
            otSize.height = (pos_y+utSize.height > itSize.height) ? (itSize.height - pos_y) : utSize.height;
            
            /* Get image tile data
             */
            imageTile tile = {
                .gPos = cv::Point2f(gPos.x+pos_x, gPos.y+pos_y),
                .image = image,
                .tile_size = otSize,
                .max_keypoint = unit_max_keypoint,
                .checked = false };

            //tile.gPos = cv::Point2f(gPos.x+pos_x, gPos.y+pos_y);
            //tile.tile = cv::Mat(image, cv::Rect(pos_x, pos_y, otSize.width, otSize.height));
            //tile.max_keypoint = unit_max_keypoint;

            if( level > 1 )
                divideImageIntoTiles( 
                    tiles, image, tile.tile_size, tile.gPos, level-1, xdiv, ydiv, unit_max_keypoint );
            else
                tiles.push_back( tile );
        }
    }

    return;
}

bool FeatureExtractorOrb::extractFeature(void) {
    int tile_list_num = _tileList.size();

    for(int i = 0; i < tile_list_num; i++) {
        /* Get valid(unprocessed) tile data from '_tileList'
         */
        _mutexLock.lock();
        bool checked = _tileList[i].checked;
        if( checked == false )
            _tileList[i].checked = true;
        _mutexLock.unlock();

        if( checked )
            continue;

        /* Running OBR processing
         */
        std::cout << "OrbTHread::_tileList[" << i << "] is running.\n";

#if 1
        imageTile &tinfo = _tileList[i];
        cv::Ptr<cv::ORB> orb_handle = cv::ORB::create( tinfo.max_keypoint );
        cv::Mat tile_image = cv::Mat(*(tinfo.image->getDataPtr()), cv::Rect(tinfo.gPos.x, tinfo.gPos.y, tinfo.tile_size.width, tinfo.tile_size.height));
        orb_handle->detectAndCompute( tile_image, cv::noArray(),
                tinfo._orb_result.keypoints, tinfo._orb_result.descriptors );
#else
        // TEST version: FAST detector
        //cv::FAST( _tileList[i].tile, _tileList[i]._orb_result.keypoints, 30, true);
#endif
    }

    return true;
}

