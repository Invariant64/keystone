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

  system("cat /proc/self/maps");


  char hash[64];
  char public_key[32];
  char signature[64];
  enclave.attestSM(hash, public_key, signature);

  for (int i = 0; i < 64; i++) {
    printf("%02x ", (unsigned char)hash[i]);
  }
  printf("\n");
  for (int i = 0; i < 32; i++) {
    printf("%02x ", (unsigned char)public_key[i]);
  }
  printf("\n");
  for (int i = 0; i < 64; i++) {
    printf("%02x ", (unsigned char)signature[i]);
  }
  printf("\n");

  // struct sm_report_t* report = (struct sm_report_t*)malloc(sizeof(struct sm_report_t));
  // printf("report addr: %p\n", report);
  // memset(report, 0, sizeof(struct sm_report_t));
  // if (report == nullptr) {
  //   printf("Failed to allocate memory for SM report!\n");
  //   return -1;
  // }
  
  // memset(buffer, 0, sizeof(buffer));
  // for (int i = 0; i < 16; i++) {
  //   printf("%02x ", buffer[i]);
  // }

  // struct sm_report_t report;
  // for (int i = 0; i < 16; i++) {
  //   printf("%02x ", report->public_key[i]);
  // }
  // printf("report addr: %p\n", report);
  
  // memcpy(&report, buffer, sizeof(struct keystone_ioctl_attest_sm));
  // print_hex(&report, sizeof(struct sm_report_t));
  // printf("Report data:\n");
  // for (int i = 0; i < MDSIZE; i++) {
  //     printf("%02x ", report->hash[i]);
  //     // if ((i + 1) % 16 == 0)
  //     //     printf("\n");
  // }
  // int sm_valid = ed25519_verify(
  //     report->signature, (const unsigned char*)report,
  //     MDSIZE + PUBLIC_KEY_SIZE, dev_public_key);
  // printf("SM Attestation report:\n");
  // // fflush(stdout);
  // if (sm_valid != 0) {
  //   // printf("SM Attestation failed!");
  //   return -1;
  // } else {
  //   // printf("SM Attestation succeeded!");
  // }

  // params.setFreeMemSize(256 * 1024);
  // params.setUntrustedSize(256 * 1024);

  // enclave.init(argv[1], argv[2], argv[3], params);

  // enclave.registerOcallDispatch(incoming_call_dispatch);
  // edge_call_init_internals(
  //     (uintptr_t)enclave.getSharedBuffer(), enclave.getSharedBufferSize());

  // enclave.run();

  return 0;
}
