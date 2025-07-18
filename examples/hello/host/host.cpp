//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "edge/edge_call.h"
#include "host/keystone.h"
#include <time.h>     // For clock_gettime

#include "verifier/report.h"

using namespace Keystone;

int
main(int argc, char** argv) {
  Enclave enclave;
  Params params;

  const unsigned char dev_public_key[] = {
    0x0f, 0xaa, 0xd4, 0xff, 0x01, 0x17, 0x85, 0x83, 0xba, 0xa5, 0x88,
    0x96, 0x6f, 0x7c, 0x1f, 0xf3, 0x25, 0x64, 0xdd, 0x17, 0xd7, 0xdc,
    0x2b, 0x46, 0xcb, 0x50, 0xa8, 0x4a, 0x69, 0x27, 0x0b, 0x4c};
  
  // static struct sm_report_t report;
  
  if(enclave.initDevice() != Error::Success) {
    printf("Failed to initialize device!\n");
    return -1;
  }
  else {
    printf("Device initialized successfully!\n");
  }

  unsigned char hash[64];
  unsigned char public_key[32];
  unsigned char signature[64];
  enclave.attestSM(hash, public_key, signature);

  unsigned char message[96];
  memcpy(message, hash, 64);
  memcpy(message + 64, public_key, 32);

  int sm_valid = ed25519_verify(
      (const unsigned char*)signature, (const unsigned char*)message,
      64 + 32, dev_public_key);
  
  if (sm_valid) {
    printf("\nSM Attestation succeeded, starting EAPP!\n");
  } else {
    printf("\nSM Attestation failed, exiting!\n");
    return -1;
  }

  params.setFreeMemSize(256 * 1024);
  params.setUntrustedSize(256 * 1024);

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
