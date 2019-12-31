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
#include "read_input.hpp"

class PipeSimulator {
public:
    PipeSimulator() {}
    ~PipeSimulator() {}
    void CmdParser(int argc, char *argv[]);
    void CreateJobs(void);
    void Run(void);
private:
    std::queue<std::shared_ptr<Job>> _jqueue;
    std::string   _iflist;
    std::string   _oflist;
    std::string   _main_cmd;
    program_args  _args;
    job_context   _context;
};

#endif
