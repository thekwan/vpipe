/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#include <iostream>

#include "pipesim.hpp"

int main(int argc, char *argv[]) {
    //std::cout << "This is a comptuer vision pipeline simulator program.\n";

    PipeSimulator psim;

    psim.CmdParser(argc, argv);
    psim.CreateJobs();
    psim.Run();

    return 0;
}
