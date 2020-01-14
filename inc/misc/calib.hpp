/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __CALIB_HPP__
#define __CALIB_HPP__

#include "types.hpp"
#include "job.hpp"

class CameraCalibration : public Job {
public:
    CameraCalibration(std::string name, program_args &pargs);
    ~CameraCalibration(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void);
private:
    void TestRun0(job_context &context);
    void TestRun1(job_context &context);
    program_args  &_pargs;
    std::string    _iflist;
};

#endif
