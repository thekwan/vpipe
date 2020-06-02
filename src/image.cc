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

Image::Image(cv::Mat data, std::string fileName) : _fileName(fileName) {
    data.copyTo(_data);
    if( _data.empty() ) {
        std::cerr << "[ERROR] Image::Image() can't read image data." << std::endl;
        // TODO: exception;
    }
}

Image::~Image() {
}

void Image::copyData(cv::Mat &data) {
    //data = _data;
    _data.copyTo(data);
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

void Image::fWriteData(std::ofstream &fs, char *data, int size) {
    fs.write((char*) &size, sizeof(int));  // 4-bytes header.
    fs.write((char*) data, size);  // 4-bytes header.
}

void Image::fWriteData(std::ofstream &fs, const std::string &data) {
    fWriteData(fs, (char*)data.c_str(), data.size());
}

void Image::fWriteData(std::ofstream &fs, cv::Mat &data) {
    struct _mat_header {
        int rows;
        int cols;
        int type;
    } mat_header;
    mat_header.rows = data.rows;
    mat_header.cols = data.cols;
    mat_header.type = data.type();
    fWriteData(fs, (char*) &mat_header, sizeof(mat_header));  // n-bytes data

    // cv::Mat data write
    cv::Mat tmp;
    data.copyTo(tmp);
    fWriteData(fs, (char*) tmp.data, tmp.elemSize() * tmp.total());
}

void Image::fWriteData(std::ofstream &fs, std::vector<cv::KeyPoint> &data) {
    int total_num = data.size();
    
    /* Create temporary buffer
     */
    struct _pack {
        float angle;
        int   class_id;
        int   octave;
        float point_x;
        float point_y;
        float response;
        float size;
    };
    struct _pack *buf = new struct _pack[total_num];
    

    /* Save datas in (continuous) buffer.
     */
    for (int i = 0; i < total_num; ++i) {
        buf[i].angle    = data[i].angle;
        buf[i].class_id = data[i].class_id;
        buf[i].octave   = data[i].octave;
        buf[i].point_x  = data[i].pt.x;
        buf[i].point_y  = data[i].pt.y;
        buf[i].response = data[i].response;
        buf[i].size     = data[i].size;
    }

    /* Write data into file.
     */
    fWriteData(fs, (char*) buf, sizeof(struct _pack) * total_num);

    delete [] buf;
}

void Image::saveImageData(std::ofstream &fs) {

    /* filename */
    fWriteData(fs, _fileName);

    /* cv::Mat _data */
    fWriteData(fs, _data);
    fWriteData(fs, _orb_kpts);
    fWriteData(fs, _orb_desc);
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
        std::cout << "ERROR ImageDB file open error";
        std::cout << ": '" << fileNameList << "'\n";
        // TODO: exception;
    }

    std::string imageFilePath;
    while( getline(iflist, imageFilePath) ) { 
        _images.emplace_back( std::make_shared<Image>(imageFilePath) );
    }
}

void ImageDB::readVideo(std::string fileName) {
    cv::VideoCapture cap(fileName);

    if (!cap.isOpened()) {
        std::cout << "[ERROR] ImageDB file open error";
        std::cout << ": '" << fileName << "'\n";
        // TODO: exception;
    }

    int idx = 0;    // frame index
    while (1) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            break;
        }

        std::string fname = fileName+"_"+std::to_string(idx++);
        _images.emplace_back( std::make_shared<Image>(frame, fileName) );
    }

    cap.release();

#if 0
    /* TEST: display all image DB.
     */
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    //cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    int cnt = 0;
    for (auto &a : _images) {
        std::cout << "frame #" << cnt++ << std::endl;
        cv::imshow("Window", *(a->getDataPtr()));
        cv::waitKey(0);
    }

    cv::destroyWindow("Window");
#endif
}

void ImageDB::loadDB(void) {
    std::ifstream dbfp(_db_file_name);

    if (! dbfp.is_open()) {
        std::cerr << "[ERROR] Can't open image DB file (load) '" << _db_file_name << "'\n";
        exit(1);
    }

    /* Load DB data from a file.
     */

    dbfp.close();

    return;
}

void ImageDB::saveDB(void) {
    /*
    std::ofstream dbfp(_db_file_name);

    if (! dbfp.is_open()) {
        std::cerr << "[ERROR] Can't open image DB file '" << _db_file_name << "'\n";
        exit(1);
    }
    */
    std::cout << "[INFO] SaveDB is called()\n";

    std::ofstream fs(_db_file_name);
    if (! fs.is_open()) {
        std::cerr << "[ERROR] Can't open image DB file (save) '" << _db_file_name << "'\n";
        exit(1);
    }

    /* Save DB data into a file.
     */
    for (auto &img: _images) {
        img->saveImageData(fs);
    }

    fs.close();


    return;
}
