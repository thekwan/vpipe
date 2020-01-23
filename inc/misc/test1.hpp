/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __TEST1_HPP__
#define __TEST1_HPP__

#include "types.hpp"
#include "job.hpp"

static void opengl_draw_callback(void *param);

class Test1 : public Job {
public:
    Test1(std::string name, program_args &pargs);
    ~Test1(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void);
private:
    void TestRun0(job_context &context);
    //void TestRun1(job_context &context);
    program_args  &_pargs;
    std::string    _iflist;
};

#endif
