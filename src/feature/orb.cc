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
    RunTileThread( context );
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

/* Tile-level thread running function
 */
bool FeatureExtractorOrb::RunTileThread(job_context &context) {
    /* Get divided tiles for given image data
     */
    int xdiv = 4;
    int ydiv = 4;
    int level = 2;
    int max_keypoint = 10000;
    for(int i = 0; i < context.images->getImageNum(); i++) {
        divideImageIntoTiles( _tileList, context.images->getImage(i)->getDataPtr(),
                cv::Point2f(0,0), level, xdiv, ydiv, max_keypoint);
    }


    /* Creates thread descriptor for each tile structure
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


#if 1
    // DEBUG: check the tile info
    std::cout << "Image size = " << context.images->getImage(0)->getDataPtr().size() << std::endl;
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    for( auto tile : _tileList ) {
        // draw image and found key-points
        cv::Mat kpt_image;
        cv::drawKeypoints(tile.tile, tile._orb_result.keypoints, kpt_image, 
                cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT );
        cv::imshow("Window", kpt_image);
        //cv::imshow("Window", tile.tile);
        std::cout << "\ttile size = " << tile.tile.size();
        std::cout << "\ttile pos = " << tile.gPos;
        std::cout << "\tmax_kp = " << tile.max_keypoint << std::endl;
        cv::waitKey(0);
    }
	cv::destroyWindow("Window");
#endif


    return true;
}

/* Recursive function to divide the given image into several tiles
 */
void FeatureExtractorOrb::divideImageIntoTiles(
        std::vector<imageTile> &tiles,
        cv::Mat image, 
        cv::Point2f gPos,
        int level, int xdiv, int ydiv,
        int max_keypoint )
{
    int unit_max_keypoint = (max_keypoint + (xdiv*ydiv)-1) / (xdiv*ydiv);
    int size_w = image.size().width;
    int size_h = image.size().height;
    int unit_h = (size_h + ydiv-1) / ydiv;
    int unit_w = (size_w + xdiv-1) / xdiv;

    for(int i = 0; i < xdiv; i++) {
        int pos_x = unit_w * i;
        int win_w = (pos_x+unit_w > size_w) ? (size_w-pos_x) : unit_w;

        for(int j = 0; j < ydiv; j++) {
            int pos_y = unit_h * j;
            int win_h = (pos_y+unit_h > size_h) ? (size_h-pos_y) : unit_h;
            
            /* Get image tile data
             */
            imageTile tile;
            tile.gPos = cv::Point2f(gPos.x+pos_x, gPos.y+pos_y);
            tile.tile = cv::Mat(image, cv::Rect(pos_x, pos_y, win_w, win_h));
            tile.max_keypoint = unit_max_keypoint;

            if( level > 1 )
                divideImageIntoTiles( 
                    tiles, tile.tile, tile.gPos, level-1, xdiv, ydiv, unit_max_keypoint );
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
        cv::Ptr<cv::ORB> orb_handle = cv::ORB::create( _tileList[i].max_keypoint );
        orb_handle->detectAndCompute( _tileList[i].tile, cv::noArray(),
                _tileList[i]._orb_result.keypoints,
                _tileList[i]._orb_result.descriptors );
#else
        cv::FAST( _tileList[i].tile, _tileList[i]._orb_result.keypoints, 30, true);
        //cv::drawKeypoints( _tileList[i].tile, 
#endif
    }

    return true;
}

/*
bool OrbThreadDesc::Run(void) {
    return true;
}
bool OrbThreadDesc::Stop(void) {
    return true;
}
*/
