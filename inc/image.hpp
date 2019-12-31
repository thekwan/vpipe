/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>

class Image {
public:
    Image();
    Image(std::string fileName);
    ~Image();

    void copyData(cv::Mat &data);
    const cv::Mat getDataPtr(void);
    const std::string getFileName(void);
private:
    cv::Mat  _data;
    const std::string _fileName;
};

/* Keypoint & descriptor data structure
 */
typedef struct _KeyPointAndDesc {
    std::vector<cv::KeyPoint>  keypoints;
    cv::Mat  descriptors;
} KeyPointAndDesc;

class ImageDB {
public:
    ImageDB(std::string fileNameList);
    ~ImageDB();

    int getImageNum(void);
    std::shared_ptr<Image> getImage(int index);
private:
    std::vector<std::shared_ptr<Image>>  _images;
    std::vector<KeyPointAndDesc> _keypoint_desc;
};

#endif
