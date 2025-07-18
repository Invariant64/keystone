######################
## QEMU run targets ##
######################

KEYSTONE_PORT   ?= 9821
QEMU_DBG_PORT   ?= $(shell echo $$(( $(KEYSTONE_PORT) + 1)) )
QEMU_DEBUG      := -gdb tcp::$(QEMU_DBG_PORT) -S

QEMU_MEM        ?= 4G
QEMU_SMP        ?= 1

QEMU_FLAGS := -m $(QEMU_MEM) -smp $(QEMU_SMP) -nographic \
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

# QEMU_FLAGS := -m $(QEMU_MEM) -smp $(QEMU_SMP) -nographic \
#                 -machine virt,rom=$(BUILDROOT_BUILDDIR)/images-bak/bootrom.bin,nvdimm=on \
#                 -bios $(BUILDROOT_BUILDDIR)/images-bak/fw_jump.elf \
#                 -kernel $(BUILDROOT_BUILDDIR)/images-bak/Image \
#                 -drive file=$(BUILDROOT_BUILDDIR)/images/rootfs.ext2,format=raw,id=hd0 \
#                 -device virtio-blk-device,drive=hd0 \
#                 -append "console=ttyS0 ro root=/dev/vda" \
#                 -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::$(KEYSTONE_PORT)-:22,hostfwd=tcp::1234-:1234 \
#                 -device virtio-net-device,netdev=net0 \
#                 -device virtio-rng-pci \
#                 -object memory-backend-file,id=nv_mem,share=on,mem-path=$(BUILDDIR)/nvdimm.bin,size=256M \
#                 -device nvdimm,id=nv0,memdev=nv_mem,label-size=2M

# QEMU_FLAGS := -m $(QEMU_MEM) -smp $(QEMU_SMP) -nographic \
#                 -machine virt \
#                 -bios $(BUILDROOT_BUILDDIR)/images/fw_payload.elf \
#                 -drive file=$(BUILDROOT_BUILDDIR)/images-bak/rootfs.ext2,format=raw,id=hd0 \
#                 # -device virtio-blk-device,drive=hd0 \
#                 # -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128,hostfwd=tcp::$(KEYSTONE_PORT)-:22 \
#                 # -device virtio-net-device,netdev=net0 \
#                 # -device virtio-rng-pci \

ifneq ($(KEYSTONE_DEBUG),)
        QEMU_FLAGS += $(QEMU_DEBUG)
endif

run:
	$(call log,info,Starting QEMU)
	$(BUILDROOT_BUILDDIR)/host/bin/qemu-system-riscv$(KEYSTONE_BITS) $(QEMU_FLAGS)

debug-connect:
	$(call log,info,Connecting to QEMU)
	$(BUILDROOT_BUILDDIR)/host/bin/riscv$(KEYSTONE_BITS)-buildroot-linux-gnu-gdb \
                -iex "set KEYSTONE=$(KEYSTONE)" \
                -x $(KEYSTONE)/scripts/gdb/generic.cfg
