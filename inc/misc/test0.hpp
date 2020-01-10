/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __TEST0_HPP__
#define __TEST0_HPP__

#include "types.hpp"
#include "job.hpp"

class Test0 : public Job {
public:
    Test0(std::string name, program_args &pargs);
    ~Test0(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void);
private:
    program_args  &_pargs;
    std::string    _iflist;
};

#endif
