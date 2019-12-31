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
} imageTile;

class FeatureExtractorOrb : public Job {
public:
    FeatureExtractorOrb(std::string name, program_args &pargs) 
        : Job(name), _pargs(pargs) {}
    ~FeatureExtractorOrb(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void) { return true; }
private:
    bool RunNoThread(job_context &context);
    void divideImageIntoTiles(
            std::vector<imageTile> &tiles,
            cv::Mat imageData, int level, int max_keypoint );

    program_args  &_pargs;
};

#endif
