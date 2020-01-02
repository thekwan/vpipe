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
    RunTileThread( context.images->getImage(0)->getDataPtr() );
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
bool FeatureExtractorOrb::RunTileThread(const cv::Mat &image) {
    /* Get divided tiles for given image data
     */
    std::vector<imageTile> tileList;
    divideImageIntoTiles( tileList, image, cv::Point2f(0,0), 2, 2, 2, 5000);

#if 1
    // DEBUG: check the tile info
    std::cout << "Image size = " << image.size() << std::endl;
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    for( auto tile : tileList ) {
        cv::imshow("Window", tile.tile);
        std::cout << "\ttile size = " << tile.tile.size();
        std::cout << "\ttile pos = " << tile.gPos;
        std::cout << "\tmax_kp = " << tile.max_keypoint << std::endl;
        cv::waitKey(0);
    }
	cv::destroyWindow("Window");
#endif

    /* Creates thread descriptor for each tile structure
     */

    /* Creates threads to do feature extraction
     */
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
