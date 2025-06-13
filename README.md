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

4. Build Keystone example:
```bash
BUILDROOT_TARGET=keystone-examples make -j
```
