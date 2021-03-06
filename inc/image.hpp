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

//typedef std::vector<cv::KeyPoint>  KeyPointList;
//typedef cv::Mat                    Descriptors;

class Image {
public:
    Image();
    Image(std::string fileName);
    Image(cv::Mat data, std::string fileName);
    ~Image();

    std::string getName(void) {
        return _fileName;
    }
    void saveImageData(std::ofstream &fs);
    cv::Size getImageSize(void);
    void copyData(cv::Mat &data);
    const cv::Mat* getDataPtr(void);
    const std::string getFileName(void);
    void setOrbKeyPoints  (std::vector<cv::KeyPoint> kpts);
    void setOrbDescriptors(cv::Mat descs);
    void addOrbKeyPoints  (std::vector<cv::KeyPoint> kpts);
    void addOrbDescriptors(cv::Mat descs);
    std::vector<cv::KeyPoint> getOrbKeyPoints(void);
    cv::Mat getOrbDescriptors(void);
private:

    void fWriteData(std::ofstream &fs, char *data, int size);
    void fWriteData(std::ofstream &fs, const std::string &data);
    void fWriteData(std::ofstream &fs, cv::Mat &data);
    void fWriteData(std::ofstream &fs, std::vector<cv::KeyPoint> &data);

    cv::Mat  _data;
    const std::string _fileName;
    /* ORB: features & descriptors
     */
    std::vector<cv::KeyPoint> _orb_kpts;
    cv::Mat   _orb_desc;
};

/* Keypoint & descriptor data structure
 */
typedef struct _KeyPointAndDesc {
    std::vector<cv::KeyPoint>  keypoints;
    cv::Mat  descriptors;
} KeyPointAndDesc;

class ImageDB {
public:
    ImageDB(void);
    ~ImageDB(void);

    /* Image DB member APIs
     */
    void loadDB(void);
    void saveDB(void);

    int getImageNum(void);
    void readImages(std::string fileNameList);
    void readVideo(std::string fileNameList);
    std::shared_ptr<Image> getImage(int index);
private:
    std::vector<std::shared_ptr<Image>>  _images;

    /* Image DB member vars
     */
    std::string    _db_file_name = "imagedb.dat";
};

#endif
