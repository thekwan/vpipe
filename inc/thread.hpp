/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <thread>

enum class ThreadStatus {
    RUN,
    STOP,
};

class Thread {
public:
    Thread() {}
    ~Thread() {}
    virtual bool Run(void) = 0;
    virtual bool Stop(void) = 0;
    ThreadStatus GetStatus() {
        return _tstatus;
    }
private:
    //pthread_t _tid;
    //std::mutex  _lock;
    ThreadStatus _tstatus;
};

#endif
