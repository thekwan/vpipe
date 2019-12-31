/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __JOB_HPP__
#define __JOB_HPP__

#include <string>
#include "types.hpp"

enum class JobStatus {
    RUN,
    STOP,
};

class Job {
public:
    Job(std::string name) : _name(name) {}
    ~Job() {}
    virtual bool Run(job_context &context) = 0;
    virtual bool Stop(void) = 0;
    JobStatus GetStatus() {
        return _jstatus;
    }
    std::string getName() {
        return _name;
    }
private:
    std::string _name;
    JobStatus _jstatus;
};

#endif
