//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "edge/edge_call.h"
#include "host/keystone.h"

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
  int reqmemsize = 1400 * 1024;
  int respmemsize = 0;
  int reserved_id = -1;

  enclave.request(hash, public_key, signature, reqmemsize, &respmemsize, &reserved_id);

  unsigned char message[96];
  memcpy(message, hash, 64);
  memcpy(message + 64, public_key, 32);

  int sm_valid = ed25519_verify(
      (const unsigned char*)signature, (const unsigned char*)message,
      64 + 32, dev_public_key);
  
  if (sm_valid) {
    printf("\nSM Attestation succeeded!\n");
  } else {
    printf("\nSM Attestation failed, exiting!\n");
    return -1;
  }

  printf("Reserved ID: %d, Response Memory Size: %d\n", reserved_id, respmemsize);

  if (reqmemsize != respmemsize) {
    printf("Requested memory size (%d) does not match response size (%d)\n",
           reqmemsize, respmemsize);
    return -1;
  }

  params.setFreeMemSize(respmemsize);
  params.setUntrustedSize(256 * 1024);
  params.setReservedID(reserved_id);

  if (enclave.init(argv[1], argv[2], argv[3], params) != Error::Success) {
    printf("Failed to initialize enclave!\n");
    return -1;
  } else {
    printf("Enclave initialized successfully!\n");
  }

  enclave.registerOcallDispatch(incoming_call_dispatch);
  edge_call_init_internals(
      (uintptr_t)enclave.getSharedBuffer(), enclave.getSharedBufferSize());

  enclave.run();

  return 0;
}
