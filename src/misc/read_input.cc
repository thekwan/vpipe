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

#if 0   // DEBUG
    // display each image on window
    std::cout << "ReadImages::Run() message\n";
    cv::namedWindow("Window", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    for(int i = 0 ; i < context.images->getImageNum() ; i++) {
        auto img = context.images->getImage(i);
        std::cout << "Read a file: " << img->getFileName() << std::endl;
        cv::imshow("Window", img->getDataPtr());
        cv::waitKey(500);
    }
	cv::destroyWindow("Window");
    std::cout << std::endl;
#endif

    return true;
}

bool ReadImages::Stop(void) {
    return true;
}
