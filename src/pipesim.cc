/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <iostream>
#include <opencv2/core/utility.hpp>

#include "pipesim.hpp"

void PipeSimulator::CmdParser(int argc, char *argv[]) {
    const char *keys = 
        "{help h     |                       | Print help message.     }"
        "{iflist     | input_file_list.txt   | Input image file list.  }"
        "{oflist     | output_file_list.txt  | Output image file list. }"
        "{cmd        | auto_reconstruct      | Command to do.          }";


    cv::CommandLineParser parser( argc, argv, keys );
    iflist   = parser.get<std::string>("iflist");
    oflist   = parser.get<std::string>("oflist");
    main_cmd = parser.get<std::string>("cmd");

#if 1
    // DEBUG: Check the program arguments
    std::cout << "iflist = " << iflist << std::endl;
    std::cout << "oflist = " << oflist << std::endl;
    std::cout << "cmd    = " << main_cmd << std::endl;
#endif

}

void PipeSimulator::CreateJobs(program_args args) {
    if( main_cmd.compare( "auto_reconstruct" ) == 0 ) {
        //jqueue.push( std::unique_ptr<FeatureExtractorOrb>(new FeatureExtractorOrb(std::forward(args))) );
        jqueue.push( std::make_unique<FeatureExtractorOrb>(args) );
    }
    else if( main_cmd.compare( "feature_extract_orb" ) == 0 ) {
        jqueue.push( std::make_unique<FeatureExtractorOrb>(args) );
    }
    else {
        // throw exception
    }
}

void PipeSimulator::Run(void) {

}
