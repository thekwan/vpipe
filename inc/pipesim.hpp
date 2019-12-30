/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __PIPESIM_HPP__
#define __PIPESIM_HPP__

#include <queue>

#include "job.hpp"

class PipeSimulator {
public:
private:
    std::queue<Job> jqueue;
};

#endif
