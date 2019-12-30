/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __JOB_HPP__
#define __JOB_HPP__

enum class JobStatus {
    RUN,
    STOP,
};

class Job {
public:
    virtual bool Run(void);
    virtual bool Stop(void);
    JobStatus GetStatus() {
        return jstatus;
    }
private:
    JobStatus jstatus;
};

#endif
