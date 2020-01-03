/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <fstream>
#include <iostream>

#include "image.hpp"

/* CLASS: Image
 */
Image::Image() {
}

Image::Image(std::string fileName) : _fileName(fileName) {
    _data = cv::imread(fileName, cv::IMREAD_COLOR);
    if( _data.empty() ) {
        std::cerr << "[ERROR] Image::Image() can't read image data." << std::endl;
        // TODO: exception;
    }
}

Image::~Image() {
}

void Image::copyData(cv::Mat &data) {
    data = _data;
}

const cv::Mat* Image::getDataPtr(void) {
    return &_data;
}

const std::string Image::getFileName(void) {
    return _fileName;
}

cv::Size Image::getImageSize(void) {
    return _data.size();
}

void Image::setOrbKeyPoints  (std::vector<cv::KeyPoint> kpts) {
    _orb_kpts = kpts;
}
void Image::setOrbDescriptors(cv::Mat descs) {
    _orb_desc = descs;
}
void Image::addOrbKeyPoints  (std::vector<cv::KeyPoint> kpts) {
    _orb_kpts.insert( _orb_kpts.end(), kpts.begin(), kpts.end() );
}
void Image::addOrbDescriptors(cv::Mat descs) {
    _orb_desc.push_back( descs );
}
std::vector<cv::KeyPoint> Image::getOrbKeyPoints(void) {
    return _orb_kpts;
}
cv::Mat Image::getOrbDescriptors(void) {
    return _orb_desc;
}


/* CLASS: ImageDB
 */
ImageDB::ImageDB() {
}

ImageDB::~ImageDB() {
}

int ImageDB::getImageNum(void) {
    return _images.size();
}

std::shared_ptr<Image> ImageDB::getImage(int index) {
    if( index >= _images.size() ) {
        std::cerr << "ERROR ImageDB out of index" << std::endl;
        // TODO: exception;
    }

    return _images[index];
}

void ImageDB::readImages(std::string fileNameList) {
    std::ifstream iflist(fileNameList);
    if( ! iflist.is_open() ) {
        std::cout << "ERROR ImageDB file open error" << std::endl;
        // TODO: exception;
    }

    std::string imageFilePath;
    while( getline(iflist, imageFilePath) ) { 
        _images.emplace_back( std::make_shared<Image>(imageFilePath) );
    }
}
