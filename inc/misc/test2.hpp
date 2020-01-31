/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __TEST2_HPP__
#define __TEST2_HPP__

#include "types.hpp"
#include "job.hpp"

static void display(void);

class Test2 : public Job {
public:
    Test2(std::string name, program_args &pargs);
    ~Test2(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void);
private:
    void TestRun0(job_context &context);
    void init(void);

    program_args  &_pargs;
    std::string    _iflist;
};

#endif
