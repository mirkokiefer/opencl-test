//
//  cl_helpers.c
//  opencl_test
//
//  Created by Mirko on 6/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "cl_helpers.h"

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)


cl_context CreateContext() {
  cl_int errNum;
  cl_uint numPlatforms;
  cl_platform_id firstPlatformId;
  cl_context context = NULL;
  
  //Select platform to run on
  errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
  if(errNum != CL_SUCCESS || numPlatforms <= 0) {
    printf("Failed to find an OpenCL platform");
    return NULL;
  }
  
  //Create context on platform (try GPU if fails then CPU)
  cl_context_properties contextProperties[] = {
    CL_CONTEXT_PLATFORM,
    (cl_context_properties)firstPlatformId,
    0
  };
  context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
  if(errNum != CL_SUCCESS) {
    context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU, NULL, NULL, &errNum);
    if(errNum != CL_SUCCESS) {
      printf("Failed to create an OpenCL context");
      return NULL;
    }
  }
  return context;
}

cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device) {
  cl_int errNum;
  cl_device_id *devices;
  cl_command_queue commandQueue = NULL;
  size_t deviceBufferSize = -1;
  
  //Get size of devices buffer
  errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
  if (errNum != CL_SUCCESS) {
    printf("failed call to clGetContextInfo");
    return NULL;
  }
  
  if(deviceBufferSize <= 0) {
    printf("no devices available");
    return NULL;
  }
  
  //allocate memory for devices buffer
  //devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
  devices = (cl_device_id*)malloc(deviceBufferSize);
  errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
  if(errNum != CL_SUCCESS) {
    printf("failed to get device IDs");
    return NULL;
  }
  
  //use first available devices
  commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
  if(commandQueue == NULL) {
    printf("failed to create commandQueue for device 0");
    return NULL;
  }
  *device = devices[0];
  //delete [] devices;
  free((void*)devices);
  return commandQueue;
}

cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName) {
  cl_int errNum;
  cl_program program;
  FILE *fp;
  char *sourceStr;
  size_t source_size;
  /* Load the source code containing the kernel*/
  fp = fopen(fileName, "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.");
    exit(1);
  }
  sourceStr = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread(sourceStr, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);
  
  program = clCreateProgramWithSource(context, 1, (const char**)&sourceStr, NULL, NULL);
  if(program == NULL) {
    printf("Failed to create program from source");
    return NULL;
  }
  
  errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (errNum != CL_SUCCESS) {
    // Determine the reason for the error
    char buildLog[16384];
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
    printf("Error in kernel\n %s: ", buildLog);
    clReleaseProgram(program);
    return NULL;
  }
  
  return program;
}

bool CreateMemObjects(cl_context context, cl_mem memObjects[3], int *a, int *b) {
  memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                                 sizeof(int) * ARRAY_SIZE, a, NULL);
  memObjects[1] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(int) * ARRAY_SIZE, b, NULL);
  memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * ARRAY_SIZE, NULL, NULL);
  if (memObjects[0] == NULL || memObjects[1] == NULL || memObjects[2] == NULL) {
    printf("Error creating memory objects.");
    return false;
  }
  return true;
}

void Cleanup(cl_context context, cl_command_queue commandQueue, cl_program program,
             cl_kernel kernel, cl_mem memObjects[3]) {
  clReleaseMemObject(memObjects[0]);
  clReleaseMemObject(memObjects[1]);
  clReleaseMemObject(memObjects[2]);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(commandQueue);
  clReleaseContext(context);
}
