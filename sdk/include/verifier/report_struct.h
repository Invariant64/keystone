#ifndef __REPORT_STRUCT_H__
#define __REPORT_STRUCT_H__

#include "keys_const.h"

struct enclave_report_t {
  byte hash[MDSIZE];
  uint64_t data_len;
  byte data[ATTEST_DATA_MAXLEN];
  byte signature[SIGNATURE_SIZE];
};

struct sm_report_t {
  byte hash[MDSIZE];
  byte public_key[PUBLIC_KEY_SIZE];
  byte signature[SIGNATURE_SIZE];
};

struct report_t {
  struct enclave_report_t enclave;
  struct sm_report_t sm;
  byte dev_public_key[PUBLIC_KEY_SIZE];
};

#endif  // __REPORT_STRUCT_H__
