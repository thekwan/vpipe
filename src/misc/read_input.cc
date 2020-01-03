/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include "read_input.hpp"

ReadImages::ReadImages(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
    const char *keys = 
        "{iflist     | input_file_list.txt   | Input image file list.  }";

    cv::CommandLineParser parser( _pargs.argc, _pargs.argv, keys );
    _iflist   = parser.get<std::string>("iflist");

}

bool ReadImages::Run(job_context &context) {
    context.imageDB.readImages(_iflist);
    return true;
}

bool ReadImages::Stop(void) {
    return true;
}
