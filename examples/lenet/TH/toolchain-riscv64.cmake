# set(CMAKE_SYSTEM_NAME Linux)
# set(CMAKE_SYSTEM_PROCESSOR riscv64)
# set(CMAKE_C_COMPILER riscv64-unknown-linux-gnu-gcc)
# set(CMAKE_CXX_COMPILER riscv64-unknown-linux-gnu-g++)
# set(CMAKE_FIND_ROOT_PATH /usr/riscv64-unknown-linux-gnu)
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
# set(CMAKE_C_COMPILER riscv64-unknown-linux-gnu-gcc)
# set(CMAKE_CXX_COMPILER riscv64-unknown-linux-gnu-g++)
# set(CMAKE_ASM_COMPILER riscv64-unknown-linux-gnu-as)
# set(CMAKE_AR riscv64-unknown-linux-gnu-ar)
# set(CMAKE_LINKER riscv64-unknown-linux-gnu-ld)
# set(CMAKE_STRIP riscv64-unknown-linux-gnu-strip)
set(MUSL_CROSS_TOOLCHAIN_PATH ../riscv64-linux-musl-cross)
set(CMAKE_C_COMPILER ${MUSL_CROSS_TOOLCHAIN_PATH}/bin/riscv64-linux-musl-gcc)
set(CMAKE_CXX_COMPILER ${MUSL_CROSS_TOOLCHAIN_PATH}/bin/riscv64-linux-musl-g++)
set(CMAKE_ASM_COMPILER ${MUSL_CROSS_TOOLCHAIN_PATH}/bin/riscv64-linux-musl-as)
set(CMAKE_AR ${MUSL_CROSS_TOOLCHAIN_PATH}/bin/riscv64-linux-musl-ar)
set(CMAKE_LINKER ${MUSL_CROSS_TOOLCHAIN_PATH}/bin/riscv64-linux-musl-ld)
set(CMAKE_STRIP ${MUSL_CROSS_TOOLCHAIN_PATH}/bin/riscv64-linux-musl-strip)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv64imafdc -mabi=lp64d -mcmodel=medany")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=rv64imafdc -mabi=lp64d -mcmodel=medany")

# 使用更保守的指令集，去掉压缩指令
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv64imafd -mabi=lp64d")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=rv64imafd -mabi=lp64d")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-tree-vectorize -fno-tree-slp-vectorize -fno-tree-loop-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-tree-vectorize -fno-tree-slp-vectorize -fno-tree-loop-vectorize")

set(CMAKE_CROSSCOMPILING ON)
set(CMAKE_CROSSCOMPILING_EMULATOR "")

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# 添加调试信息和禁用优化
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

# set(CMAKE_FIND_ROOT_PATH ${MUSL_ROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)