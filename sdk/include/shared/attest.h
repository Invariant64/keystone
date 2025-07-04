#ifndef __KESTONE_ATTEST_H__
#define __KESTONE_ATTEST_H__

#include "../verifier/report_struct.h"

struct keystone_ioctl_attest_sm {
  // driver -> host
  struct sm_report_t report;
};

#endif  // __KESTONE_ATTEST_H__