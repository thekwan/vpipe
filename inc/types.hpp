/*
 * Computer Vision test pipeline simulator
 * Author: Deokhwan, Kim
 * Copyright (C), Deokhwan Kim, all rights reserved.
 */

#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <memory>
#include "image.hpp"

typedef struct _program_arguments {
    int argc;
    char **argv;
} program_args;

typedef struct _job_context {
    ImageDB   imageDB;
} job_context;

#endif
