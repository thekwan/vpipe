/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include "read_input.hpp"

ReadImages::ReadImages(std::string name, program_args &pargs) : Job(name), _pargs(pargs) {
    const char *keys = 
        "{iflist     |                       | Input image file list.  }"
        "{ivideo     |                       | Input video file name.  }"
        "{save_db    | false                 | save image DB into file }"
        "{load_db    | false                 | load image DB from file }";

    cv::CommandLineParser parser( _pargs.argc, _pargs.argv, keys );

    if (parser.has("iflist")) {
        _iflist   = parser.get<std::string>("iflist");
    }
    if (parser.has("ivideo")) {
        _ivideo = parser.get<std::string>("ivideo");
    }

    _save_db = parser.get<bool>("save_db");
    _load_db = parser.get<bool>("load_db");

#if 1
    if (_iflist.size() > 0) {
        std::cout << "ReadImages._iflist = " << _iflist << std::endl;
    }
    if (_ivideo.size() > 0) {
        std::cout << "ReadImages._ivideo = " << _ivideo << std::endl;
    }
    std::cout << "ReadImages._save_db= " << _save_db << std::endl;
    std::cout << "ReadImages._load_db= " << _load_db << std::endl;
#endif
}

bool ReadImages::Run(job_context &context) {

    if (_load_db) {
        /* Load image DB from pre-built file.
         */
        context.imageDB.loadDB();
    }
    else {
        /* Read image data from file.
         */
        if (_iflist.size() > 0) {
            context.imageDB.readImages(_iflist);
        }
        if (_ivideo.size() > 0) {
            context.imageDB.readVideo(_ivideo);
        }

        /* Save image DB into file.
         */
        if (_save_db) {
            context.imageDB.saveDB();
        }
    }
    
    return true;
}

bool ReadImages::Stop(void) {
    return true;
}
