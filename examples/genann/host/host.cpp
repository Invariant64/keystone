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

  params.setFreeMemSize(256 * 1024 * 1024);
  params.setUntrustedSize(64 * 1024 * 1024);

  struct timespec time1, time2;

  clock_gettime(CLOCK_MONOTONIC, &time1);

  enclave.init(argv[1], argv[2], argv[3], params);

  clock_gettime(CLOCK_MONOTONIC, &time2);

  enclave.registerOcallDispatch(incoming_call_dispatch);
  edge_call_init_internals(
      (uintptr_t)enclave.getSharedBuffer(), enclave.getSharedBufferSize());

  // Time measurement variables
  struct timespec time3, time4;
  long long init_time, run_time;
  uintptr_t ret_val; // Variable to capture return value from enclave

  // Start time measurement
  clock_gettime(CLOCK_MONOTONIC, &time3);

  enclave.run(&ret_val); // Call with return value pointer

  // End time measurement
  clock_gettime(CLOCK_MONOTONIC, &time4);

  // Calculate elapsed time in nanoseconds
  init_time = (time2.tv_sec - time1.tv_sec) * 1e9 +
               (time2.tv_nsec - time1.tv_nsec);
  run_time = (time4.tv_sec - time3.tv_sec) * 1e9 +
               (time4.tv_nsec - time3.tv_nsec);

  std::cout << "Enclave init time: " << init_time << " ns" << std::endl;
  std::cout << "Enclave run time: " << run_time << " ns" << std::endl;

  std::cout << "Enclave returned: " << ret_val << std::endl;

  return 0;
}
