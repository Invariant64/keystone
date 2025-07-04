//******************************************************************************
// Copyright (c) 2018, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#pragma once

#include <iostream>
#include <string>
#include "Keys.hpp"
#include "common/sha3.h"
#include "ed25519/ed25519.h"
#include "verifier/json11.h"

#include "report_struct.h"

class Report {
 private:
  struct report_t report;

 public:
  std::string BytesToHex(byte* bytes, size_t len);
  void HexToBytes(byte* bytes, size_t len, std::string hexstr);
  void fromJson(std::string json);
  void fromBytes(byte* bin);
  std::string stringfy();
  void printJson();
  void printPretty();
  int verify(
      const byte* expected_enclave_hash, const byte* expected_sm_hash,
      const byte* dev_public_key);
  int checkSignaturesOnly(const byte* dev_public_key);
  void* getDataSection();
  size_t getDataSize();
  byte* getEnclaveHash();
  byte* getSmHash();
};
