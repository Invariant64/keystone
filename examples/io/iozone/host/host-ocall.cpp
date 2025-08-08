//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "edge/edge_call.h"
#include "host/keystone.h"
#include <sys/time.h>   // For gettimeofday on the host
#include <string.h>     // For memcpy

using namespace Keystone;

void get_timeofday_wrapper(void* buffer) ;
#define OCALL_GET_TIMEOFDAY 1
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

  register_call(OCALL_GET_TIMEOFDAY, get_timeofday_wrapper);

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



// Edge-wrapper function for ocall_get_timeofday
void get_timeofday_wrapper(void* buffer) {
    struct edge_call* edge_call = (struct edge_call*)buffer;
    uintptr_t call_args;
    size_t arg_len;
    struct timeval tv_host;
    int ret_val;

    // Parse and validate the incoming call data (struct timeval buffer from enclave)
    if (edge_call_args_ptr(edge_call, &call_args, &arg_len) != 0 ||
        arg_len != sizeof(struct timeval)) {
        edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
        return;
    }

    // Call the host's gettimeofday function
    ret_val = gettimeofday(&tv_host, NULL);

    // Copy the result (struct timeval) back to the enclave's buffer
    memcpy((void*)call_args, &tv_host, sizeof(struct timeval));

    // Setup return data (integer return value of gettimeofday)
    uintptr_t data_section = edge_call_data_ptr();
    memcpy((void*)data_section, &ret_val, sizeof(int));
    if (edge_call_setup_ret(
            edge_call, (void*)data_section, sizeof(int))) {
        edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
    } else {
        edge_call->return_data.call_status = CALL_STATUS_OK;
    }

    // This will now eventually return control to the enclave
    return;
}