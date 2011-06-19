//
//  cl_helpers.h
//  opencl_test
//
//  Created by Mirko on 6/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "includes.h"

cl_context CreateContext();
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device);
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName);
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], int *a, int *b);
void Cleanup(cl_context context, cl_command_queue commandQueue, cl_program program,
             cl_kernel kernel, cl_mem memObjects[3]);