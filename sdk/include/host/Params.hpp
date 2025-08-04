//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#pragma once

#include <cstdio>

#if __riscv_xlen == 64
#define DEFAULT_FREEMEM_SIZE 1024 * 1024  // 1 MB
#define DEFAULT_UNTRUSTED_PTR 0xffffffff80000000
#define DEFAULT_STACK_SIZE 1024 * 16  // 16k
#define DEFAULT_STACK_START 0x0000000040000000
#elif __riscv_xlen == 32
#define DEFAULT_FREEMEM_SIZE 1024 * 512  // 512 KiB
#define DEFAULT_UNTRUSTED_PTR 0x80000000
#define DEFAULT_STACK_SIZE 1024 * 8  // 3 KiB
#define DEFAULT_STACK_START 0x40000000
#else                                     // for x86 tests
#define DEFAULT_FREEMEM_SIZE 1024 * 1024  // 1 MB
#define DEFAULT_UNTRUSTED_PTR 0xffffffff80000000
#define DEFAULT_STACK_SIZE 1024 * 16  // 16k
#define DEFAULT_STACK_START 0x0000000040000000
#endif

#define DEFAULT_UNTRUSTED_SIZE 8192  // 8 KB

/* parameters for enclave creation */
namespace Keystone {

class Params {
 public:
  Params() {
    untrusted_size = DEFAULT_UNTRUSTED_SIZE;
    freemem_size   = DEFAULT_FREEMEM_SIZE;
    reserved_id    = -1;
  }

  void setUntrustedSize(uint64_t size) { untrusted_size = size; }
  void setFreeMemSize(uint64_t size) { freemem_size = size; }
  void setReservedID(int id) { reserved_id = id; }
  void setBudgetCycles(unsigned long cycles) { budget_cycles = cycles; }
  void setPeriodTicks(unsigned long ticks) { period_ticks = ticks; }
  void setTimeDebtThreshold(unsigned long threshold) { time_debt_threshold = threshold; }

  uint64_t getUntrustedSize() { return untrusted_size; }
  uint64_t getFreeMemSize() { return freemem_size; }
  int getReservedID() { return reserved_id; }
  unsigned long getBudgetCycles() { return budget_cycles; }
  unsigned long getPeriodTicks() { return period_ticks; }
  unsigned long getTimeDebtThreshold() { return time_debt_threshold; }

 private:
  int reserved_id;
  uint64_t untrusted_size;
  uint64_t freemem_size;
  unsigned long budget_cycles;
  unsigned long period_ticks;
  unsigned long time_debt_threshold;
};

struct RequestParams {
  unsigned char hash[64];
  unsigned char publicKey[32];
  unsigned char signature[64];

  int reqmemsize;
  int respmemsize;
  int reservedID;

  unsigned long budget_cycles;
  unsigned long period_ticks;
  unsigned long time_debt_threshold;
};

}  // namespace Keystone
