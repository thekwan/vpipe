/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __ORB_HPP__
#define __ORB_HPP__

#include "types.hpp"
#include "job.hpp"

class FeatureExtractorOrb : public Job {
public:
    FeatureExtractorOrb(std::string name, program_args &pargs) 
        : Job(name), _pargs(pargs) {}
    ~FeatureExtractorOrb(void) {}
    virtual bool Run(job_context context) { return true; }
    virtual bool Stop(void) { return true; }
private:
    program_args  &_pargs;
};

#endif
