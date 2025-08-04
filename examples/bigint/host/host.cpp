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

  params.setFreeMemSize(64 * 1024 * 1024);
  params.setUntrustedSize(1024 * 1024);

  enclave.init(argv[1], argv[2], argv[3], params);

  enclave.registerOcallDispatch(incoming_call_dispatch);
  edge_call_init_internals(
      (uintptr_t)enclave.getSharedBuffer(), enclave.getSharedBufferSize());

  // Time measurement variables
  struct timespec start_time, end_time;
  long long elapsed_ns;
  uintptr_t ret_val; // Variable to capture return value from enclave

  // Start time measurement
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  enclave.run(&ret_val); // Call with return value pointer

  // End time measurement
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  // Calculate elapsed time in nanoseconds
  elapsed_ns = (end_time.tv_sec - start_time.tv_sec) * 1e9 +
               (end_time.tv_nsec - start_time.tv_nsec);

  std::cout << "Enclave execution time: " << elapsed_ns << " ns" << std::endl;
  std::cout << "Enclave returned: " << ret_val << std::endl;

  return 0;
}
