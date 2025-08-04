cmake_minimum_required(VERSION 3.10)
project(keystone_examples C CXX ASM)

# check if SDK has been built and KEYSTONE_SDK_DIR configuration variable exists
if (NOT DEFINED KEYSTONE_SDK_DIR)
  message(FATAL_ERROR "Please set KEYSTONE_SDK_DIR configuration variable to an installed SDK path")
endif()

set(ENV{KEYSTONE_SDK_DIR} ${KEYSTONE_SDK_DIR})

if (DEFINED PATH)
  set(ENV{PATH} ${PATH})
endif()

include(${KEYSTONE_SDK_DIR}/cmake/macros.cmake)
include(ExternalProject)
find_package(Git REQUIRED)

use_riscv_toolchain(${KEYSTONE_BITS})

# export include directory
include_directories(AFTER ${KEYSTONE_SDK_DIR}/include)

# set paths to the libraries
set(KEYSTONE_LIB_HOST ${KEYSTONE_SDK_DIR}/lib/libkeystone-host.a)
set(KEYSTONE_LIB_EDGE ${KEYSTONE_SDK_DIR}/lib/libkeystone-edge.a)
set(KEYSTONE_LIB_VERIFIER ${KEYSTONE_SDK_DIR}/lib/libkeystone-verifier.a)
set(KEYSTONE_LIB_EAPP ${KEYSTONE_SDK_DIR}/lib/libkeystone-eapp.a)

# create a phony target "examples"
add_custom_target("examples")

# add all examples below
add_subdirectory(hello)
add_subdirectory(hello1)
add_subdirectory(hello-native)
add_subdirectory(attestation)
add_subdirectory(tests)
# add_subdirectory(lenet)
add_subdirectory(onnx)

# just for test
add_subdirectory(fileio)
add_subdirectory(hello-cpp)

# iozone
add_subdirectory(iozone)

#rv8-bench
add_subdirectory(dhrystone)
add_subdirectory(primes)
add_subdirectory(aes)
add_subdirectory(bigint)
add_subdirectory(miniz)
add_subdirectory(norx)
add_subdirectory(qsort)
add_subdirectory(sha512)

#coremark
add_subdirectory(coremark)

#mem
add_subdirectory(stream)
add_subdirectory(membench)
#cc gapbs
add_subdirectory(cc)