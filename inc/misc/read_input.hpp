/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __READIMAGES_HPP__
#define __READIMAGES_HPP__

#include "types.hpp"
#include "job.hpp"

class ReadImages : public Job {
public:
    ReadImages(std::string name, program_args &pargs);
    ~ReadImages(void) {}
    virtual bool Run(job_context context);
    virtual bool Stop(void);
private:
    program_args  &_pargs;
    std::string    _iflist;
};

#endif
