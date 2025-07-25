######################
## QEMU run targets ##
######################

KEYSTONE_PORT   ?= 9821
QEMU_DBG_PORT   ?= $(shell echo $$(( $(KEYSTONE_PORT) + 1)) )
QEMU_DEBUG      := -gdb tcp::$(QEMU_DBG_PORT) -S

QEMU_MEM        ?= 4G
QEMU_SMP        ?= 1

QEMU_TEST_FLAGS := -m $(QEMU_MEM) -smp $(QEMU_SMP) -nographic \
                -machine virt,rom=$(BUILDROOT_BUILDDIR)/images-bak/bootrom.bin \
                -bios $(BUILDROOT_BUILDDIR)/images-bak/fw_jump.elf \
                -kernel $(BUILDROOT_BUILDDIR)/images-bak/Image \
                -append "console=ttyS0 ro root=/dev/vda cma=1024M" \
                -object throttle-group,id=diskthrottle,x-bps-read=8388608,x-bps-write=4194304,x-iops-read=40,x-iops-write=20 \
                -blockdev driver=file,filename=/home/luluji/keystone-omp/build-generic64/buildroot.build/images/rootfs.ext2,node-name=hd0_file,cache.direct=true,cache.no-flush=true \
                -blockdev driver=raw,file=hd0_file,node-name=hd0_raw \
                -blockdev driver=throttle,file=hd0_raw,node-name=hd0_throttled,throttle-group=diskthrottle \
                -device virtio-blk-device,drive=hd0_throttled \
                -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::9821-:22,hostfwd=tcp::1234-:1234 \
                -device virtio-net-device,netdev=net0 \
                -device virtio-rng-pci

# QEMU_ORIGIN_FLAGS := -m $(QEMU_MEM) -smp $(QEMU_SMP) -nographic \
#                 -machine virt,rom=$(BUILDROOT_BUILDDIR)/images/bootrom.bin \
#                 -bios $(BUILDROOT_BUILDDIR)/images/fw_jump.elf \
#                 -kernel $(BUILDROOT_BUILDDIR)/images/Image \
#                 -drive file=$(BUILDROOT_BUILDDIR)/images/rootfs.ext2,format=raw,id=hd0 \
#                 -device virtio-blk-device,drive=hd0 \
#                 -append "console=ttyS0 ro root=/dev/vda" \
#                 -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::$(KEYSTONE_PORT)-:22 \
#                 -device virtio-net-device,netdev=net0 \
#                 -device virtio-rng-pci \

FW_PAYLOAD := $(abspath $(BUILDROOT_BUILDDIR)/images/fw_payload.bin)

QEMU_FLAGS := -nographic \
  -machine virt \
  -m 2G \
  -kernel $(FW_PAYLOAD)

ifneq ($(KEYSTONE_DEBUG),)
        QEMU_FLAGS += $(QEMU_DEBUG)
endif

run:
	$(call log,info,Starting QEMU)
	$(BUILDROOT_BUILDDIR)/host/bin/qemu-system-riscv$(KEYSTONE_BITS) $(QEMU_FLAGS)

FPGA_PATH := $(abspath $(KEYSTONE)/../bit-file)
BIT_PATH := $(FPGA_PATH)/nanhuv2-4-25
DATA_FILE := fw_payload_auto_data.txt
DATA_PATH := $(FPGA_PATH)/$(DATA_FILE)
BIN2FPGA := $(FPGA_PATH)/bin2fpgadata

fpga:
	$(call log,info,Starting with FPGA)
	$(BIN2FPGA) -i $(FW_PAYLOAD) -o $(DATA_PATH)
	source /tools/Xilinx/Vivado/2020.2/settings64.sh && \
	vivado -mode batch -source $(FPGA_PATH)/reworkload-119.tcl -tclargs $(BIT_PATH) $(DATA_PATH)

fpga-reload:
	$(call log,info,Reloading FPGA)
	$(BIN2FPGA) -i $(FW_PAYLOAD) -o $(DATA_PATH)
	source /tools/Xilinx/Vivado/2020.2/settings64.sh && \
	vivado -mode batch -source $(FPGA_PATH)/reworkload-119.tcl -tclargs $(BIT_PATH) $(DATA_PATH)

kill:
	$(call log,info,Killing QEMU)
	@pkill -f qemu-system-riscv$(KEYSTONE_BITS) || true

debug-connect:
	$(call log,info,Connecting to QEMU)
	$(BUILDROOT_BUILDDIR)/host/bin/riscv$(KEYSTONE_BITS)-buildroot-linux-gnu-gdb \
                -iex "set KEYSTONE=$(KEYSTONE)" \
                -x $(KEYSTONE)/scripts/gdb/generic.cfg
