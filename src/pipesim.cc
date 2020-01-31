/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <iostream>
#include <opencv2/core/utility.hpp>

#include "pipesim.hpp"
#include "orb.hpp"
#include "mch.hpp"
#include "test0.hpp"
#include "test1.hpp"
#include "test2.hpp"
#include "calib.hpp"
#include "read_input.hpp"

void PipeSimulator::CmdParser(int argc, char *argv[]) {
    const char *keys = 
        "{help h     |                       | Print help message.     }"
        "{iflist     | input_file_list.txt   | Input image file list.  }"
        "{oflist     | output_file_list.txt  | Output image file list. }"
        "{cmd        | auto_reconstruct      | Command to do.          }";


    cv::CommandLineParser parser( argc, argv, keys );
    _iflist   = parser.get<std::string>("iflist");
    _oflist   = parser.get<std::string>("oflist");
    _main_cmd = parser.get<std::string>("cmd");

#if 1
    // DEBUG: Check the program arguments
    std::cout << "iflist = " << _iflist << std::endl;
    std::cout << "oflist = " << _oflist << std::endl;
    std::cout << "cmd    = " << _main_cmd << std::endl;
#endif

    _args.argc = argc;
    _args.argv = argv;
}

void PipeSimulator::CreateJobs(void) {
    /* Push a basic job 'const input image DB'.
     */
    _jqueue.push( std::make_shared<ReadImages>("Construct input iamge DB",_args) );


    if( _main_cmd.compare( "auto_reconstruct" ) == 0 ) {
        _jqueue.push( std::make_shared<FeatureExtractorOrb>("Feature extractor ORB",_args) );
        _jqueue.push( std::make_shared<FeatureMatcher>("Feature matcher ORB",_args) );
    }
    else if( _main_cmd.compare( "feature_extract_orb" ) == 0 ) {
        _jqueue.push( std::make_shared<FeatureExtractorOrb>("Feature extractor ORB",_args) );
    }
    else if( _main_cmd.compare( "test0" ) == 0 ) {
        _jqueue.push( std::make_shared<Test0>("Aruco detector test code 0",_args) );
    }
    else if( _main_cmd.compare( "test1" ) == 0 ) {
        _jqueue.push( std::make_shared<Test1>("OpenCV/OpenGL interoperability test",_args) );
    }
    else if( _main_cmd.compare( "test2" ) == 0 ) {
        _jqueue.push( std::make_shared<Test2>("OpenGL working test",_args) );
    }
    else if( _main_cmd.compare( "calibration" ) == 0 ) {
        _jqueue.push( std::make_shared<CameraCalibration>("Camera calibration test",_args) );
    }
    else {
        // throw exception
    }
}

void PipeSimulator::Run(void) {

    /* Executes each job process
     */
    while( _jqueue.size() > 0 ) { 
        auto job = _jqueue.front();
        _jqueue.pop();

#if 1   // DEBUG
        std::cout << "Run Job: '" << job->getName() << "'" << std::endl;
#endif
        job->Run(_context);
        //job->Stop();
    }
}
