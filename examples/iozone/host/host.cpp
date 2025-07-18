//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "edge/edge_call.h"
#include "host/keystone.h"

using namespace Keystone;

int
main(int argc, char** argv) {
  Enclave enclave;
  Params params;

  params.setFreeMemSize(128 * 1024 * 1024);
  params.setUntrustedSize(1024 * 1024);

  struct timespec time1, time2; // Added for init time

  clock_gettime(CLOCK_MONOTONIC, &time1); // Start init time measurement

  enclave.init(argv[1], argv[2], argv[3], params);

  clock_gettime(CLOCK_MONOTONIC, &time2); // End init time measurement

  enclave.registerOcallDispatch(incoming_call_dispatch);
  edge_call_init_internals(
      (uintptr_t)enclave.getSharedBuffer(), enclave.getSharedBufferSize());

  // Time measurement variables
  struct timespec time3, time4; // Renamed for run time clarity
  long long init_time, run_time; // Added init_time
  uintptr_t ret_val; // Variable to capture return value from enclave

  // Start run time measurement
  clock_gettime(CLOCK_MONOTONIC, &time3); // Renamed for run time clarity

  enclave.run(&ret_val); // Call with return value pointer

  // End run time measurement
  clock_gettime(CLOCK_MONOTONIC, &time4); // Renamed for run time clarity

  // Calculate elapsed time in nanoseconds
  init_time = (time2.tv_sec - time1.tv_sec) * 1e9 + // Calculate init time
               (time2.tv_nsec - time1.tv_nsec);
  run_time = (time4.tv_sec - time3.tv_sec) * 1e9 + // Calculate run time
               (time4.tv_nsec - time3.tv_nsec);

  std::cout << "Enclave init time: " << init_time << " ns" << std::endl; // Print init time
  std::cout << "Enclave run time: " << run_time << " ns" << std::endl; // Print run time
  std::cout << "Enclave returned: " << ret_val << std::endl;

  return 0;
} 