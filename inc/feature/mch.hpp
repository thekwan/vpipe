/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __MCH_HPP__
#define __MCH_HPP__

#include <opencv2/opencv.hpp>
//#include <opencv2/features2d.hpp>
//#include <opencv/core/types.hpp>
#include <thread>
#include <mutex>
#include "types.hpp"
#include "job.hpp"


class FeatureMatcher : public Job {
public:
    FeatureMatcher(std::string name, program_args &pargs) 
        : Job(name), _pargs(pargs) {}
    ~FeatureMatcher(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void) { return true; }
    bool bfMatching(std::shared_ptr<Image> q, std::shared_ptr<Image> t);
    static void cpu_thread_wrapper(FeatureMatcher *handle,
            std::shared_ptr<Image> q, std::shared_ptr<Image> t) {
        handle->bfMatching(q, t);
    }
private:
    bool RunNoThread(job_context &context);
    bool RunThread(job_context &context);
    
    program_args  &_pargs;

    /* matching result
     */
    std::map<
        std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>,
        std::vector<cv::DMatch>
            > _bf_match_out;

    /* Thread related variables
     */
    std::vector<std::thread>  _threadList;
    std::mutex _mutexLock;
};


#endif
