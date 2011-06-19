//
//  main.c
//  opencl_test
//
//  Created by Mirko on 6/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "includes.h"
#include "cl_helpers.h"

int main (int argc, const char * argv[])
{

  cl_context context = 0;
  cl_command_queue commandQueue = 0;
  cl_program program = 0;
  cl_device_id device = 0;
  cl_kernel kernel = 0;
  cl_mem memObjects[3] = {0, 0, 0};
  cl_int errNum;
  
  // Create an OpenCL context on first available platform
  context = CreateContext();
  if(context == NULL) {
    printf("Failed to create OpenCL context");
    return 1;
  }
  
  //Create a command queue on the first device available on the context
  commandQueue = CreateCommandQueue(context, &device);
  if(commandQueue == NULL) {
    Cleanup(context, commandQueue, program, kernel, memObjects);
    return 1;
  }
  
  //Create OpenCL programm from kernel source
  program = CreateProgram(context, device, "/Users/mirko/Desktop/Code/Projects/opencl_test/opencl_test/HelloWorld.cl");
  
  if(program == NULL) {
    Cleanup(context, commandQueue, program, kernel, memObjects);
    return 1;
  }
  
  //Create OpenCL kernel
  kernel = clCreateKernel(program, "hello_kernel", NULL);
  if(program == NULL) {
    Cleanup(context, commandQueue, program, kernel, memObjects);
    return 1;
  }
  
  //Create memory objects that will be used as arguments to kernel
  int result[ARRAY_SIZE];
  int a[ARRAY_SIZE];
  int b[ARRAY_SIZE];
  for (int i=0; i<ARRAY_SIZE; i++) {
    a[i] = i;
    b[i] = i*2;
  }
  
  if(!CreateMemObjects(context, memObjects, a, b)) {
    Cleanup(context, commandQueue, program, kernel, memObjects);
    return 1;
  }
  
  //Set the kernel arguments (result, a, b)
  errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
  errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);
  errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);
  if (errNum != CL_SUCCESS) {
    printf("Error setting kernel arguments");
    Cleanup(context, commandQueue, program, kernel, memObjects);
    return 1;
  }
  
  size_t globalWorkSize[1] = {ARRAY_SIZE};
  size_t localWorkSize[1] = {1};
  
  //Queue the kernel up for execution across the array
  errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, globalWorkSize,
                                  localWorkSize, 0, NULL, NULL);
  if (errNum != CL_SUCCESS) {
    printf("Error queing kernel for execution");
    Cleanup(context, commandQueue, program, kernel, memObjects);
    return 1;
  }
  
  // Read the output buffer back to the Host
  errNum = clEnqueueReadBuffer(commandQueue, memObjects[2], CL_TRUE, 0, 
                               ARRAY_SIZE * sizeof(int), result, 0, NULL, NULL);
  if (errNum != CL_SUCCESS) {
    printf("Error reading result buffer");
    Cleanup(context, commandQueue, program, kernel, memObjects);
    return 1;
  }
  printf("Executed successfully");
  for (int i = 0; i < ARRAY_SIZE; i++) {
    printf("%i, ", result[i]);
  }
  return 0;
}

