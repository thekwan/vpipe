/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __ORB_HPP__
#define __ORB_HPP__

#include "types.hpp"
#include "job.hpp"

typedef struct _imageTile {
    cv::Point2f  gPos;  // tile global position
    cv::Mat      tile;  // image tile data
    int          max_keypoint;
} imageTile;

typedef struct _threadDesc {
    imageTile   data;
} threadDesc;

class FeatureExtractorOrb : public Job {
public:
    FeatureExtractorOrb(std::string name, program_args &pargs) 
        : Job(name), _pargs(pargs) {}
    ~FeatureExtractorOrb(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void) { return true; }
private:
    bool RunNoThread(job_context &context);
    bool RunTileThread(const cv::Mat &image);
    void divideImageIntoTiles(
            std::vector<imageTile> &tiles,
            cv::Mat imageData, cv::Point2f gPos, int level, 
            int xdiv, int ydiv, int max_keypoint );

    program_args  &_pargs;
    /* Thread queue:
     * pending_q: Q for threads to wait therad start.
     * finish_q: Q for threads to finish its working.
     */
    std::queue<threadDesc>  pending_q, finish_q;
};

#endif
