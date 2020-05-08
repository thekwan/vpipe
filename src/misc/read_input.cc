/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include "read_input.hpp"

ReadImages::ReadImages(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
    const char *keys = 
        "{iflist     |                       | Input image file list.  }"
        "{ivideo     |                       | Input video file name.  }";

    cv::CommandLineParser parser( _pargs.argc, _pargs.argv, keys );

    if (parser.has("iflist")) {
        _iflist   = parser.get<std::string>("iflist");
    }
    if (parser.has("ivideo")) {
        _ivideo = parser.get<std::string>("ivideo");
    }

#if 1
    if (_iflist.size() > 0) {
        std::cout << "ReadImages._iflist = " << _iflist << std::endl;
    }
    if (_ivideo.size() > 0) {
        std::cout << "ReadImages._ivideo = " << _ivideo << std::endl;
    }
#endif
}

bool ReadImages::Run(job_context &context) {
    if (_iflist.size() > 0) {
        context.imageDB.readImages(_iflist);
    }
    if (_ivideo.size() > 0) {
        context.imageDB.readVideo(_ivideo);
    }
    return true;
}

bool ReadImages::Stop(void) {
    return true;
}
