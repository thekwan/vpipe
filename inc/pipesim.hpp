/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __PIPESIM_HPP__
#define __PIPESIM_HPP__

#include <queue>
#include <memory>

#include "types.hpp"
#include "job.hpp"
#include "orb.hpp"

class PipeSimulator {
public:
    void CmdParser(int argc, char *argv[]);
    void CreateJobs(program_args args);
    void Run(void);
private:
    std::queue<std::unique_ptr<Job>> jqueue;
    std::string iflist;
    std::string oflist;
    std::string main_cmd;
};

#endif
