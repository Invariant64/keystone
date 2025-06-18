1. Setup Keystone as the [documentation](https://docs.keystone-enclave.org/en/latest/Getting-Started/Running-Keystone-with-QEMU.html) describes.

2. Download Musl cross compiler at [https://musl.cc/riscv64-linux-musl-cross.tgz](https://musl.cc/riscv64-linux-musl-cross.tgz). Set the MUSL_CROSS_TOOLCHAIN_PATH variable in examples/TH/toolchain-riscv64.cmake to the path where you extracted the musl cross compiler.

3. Build TH Library:
```bash
cd examples/lenet/TH
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-riscv64.cmake
make -j
```

4. Build lapack Library (TH Library use it)
```bash
cd examples/lenet/lapack-3.12.1/
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-riscv64.cmake
make -j
```

5. Configue toolchain support OpenMP
```bash
make buildroot-configure
```
Choose：
Toolchain  --->
 [*] Enable compiler OpenMP support

6. Rebuild the whole keystone to rebuild toolchain (still don't known how to precisely rebuild toolchain)
Delete some build file(like build-generic64/buildroot.build/host) or clone new keystone
```bash
make -j
```

7. Build Keystone example:
```bash
BUILDROOT_TARGET=keystone-examples make -j
```
