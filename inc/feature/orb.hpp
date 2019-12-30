/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __ORB_HPP__
#define __ORB_HPP__

#include "types.hpp"
#include "job.hpp"

class FeatureExtractorOrb : Job {
public:
    FeatureExtractorOrb(program_args &pargs) : _pargs(pargs) {}
    ~FeatureExtractorOrb(void) {}
private:
    program_args  &_pargs;
};

#endif
