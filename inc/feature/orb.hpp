/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __ORB_HPP__
#define __ORB_HPP__

#include <thread>
#include <mutex>
#include "types.hpp"
#include "job.hpp"

typedef struct _imageTile {
    cv::Point2f  gPos;  // tile global position
    cv::Mat      tile;  // image tile data
    int          max_keypoint;
    bool         checked = false;
    KeyPointAndDesc  _orb_result;
} imageTile;


/*
class OrbThreadDesc : Thread {
public:
    OrbThreadDesc(pthread_mutex_t &lock, std::vector<imageTile> &queue)
        : Thread(lock), _queue(queue) {}
    ~OrbThreadDesc() {}
    virtual bool Run();
    virtual bool Stop();
private:
    bool mainThread(void);
    std::vector<imageTile> &_queue;
    int _queue_idx;
};
*/


class FeatureExtractorOrb : public Job {
public:
    FeatureExtractorOrb(std::string name, program_args &pargs) 
        : Job(name), _pargs(pargs) {}
    ~FeatureExtractorOrb(void) {}
    virtual bool Run(job_context &context);
    virtual bool Stop(void) { return true; }
    bool extractFeature(void);
    static void cpu_thread_wrapper(FeatureExtractorOrb *handle) {
        handle->extractFeature();
    }
private:
    bool RunNoThread(job_context &context);
    bool RunTileThread(job_context &context);
    void divideImageIntoTiles(
            std::vector<imageTile> &tiles,
            cv::Mat imageData, cv::Point2f gPos, int level, 
            int xdiv, int ydiv, int max_keypoint );
    

    program_args  &_pargs;
    /* Thread queue:
     * pending_q: Q for threads to wait therad start.
     * finish_q: Q for threads to finish its working.
     */
    //std::queue<std::shared_ptr<OrbThreadDesc>>  pending_q, finish_q;

    /* Image tile list
     */
    std::vector<imageTile> _tileList;

    /* Thread related variables
     */
    std::vector<std::thread>  _threadList;
    std::mutex _mutexLock;
};


#endif
