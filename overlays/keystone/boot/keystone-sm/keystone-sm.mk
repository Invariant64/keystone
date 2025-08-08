################################################################################
#
# SM
#
################################################################################

ifeq ($(KEYSTONE_SM),)
$(error KEYSTONE_SM directory not defined)
else
include $(KEYSTONE)/mkutils/pkg-keystone.mk
endif

# Make the SM depend on the SDK since it contains the shared headers which
# specifiy the communication protocol between the host <> kernel <> sm <>
# runtime <> eapp. Consumer firmwares then depend on this target, and thus
# inherit these shared headers as well
KEYSTONE_SM_DEPENDENCIES += host-keystone-sdk
$(KEYSTONE_SM_CONFIGURE): host-keystone-sdk-install

ifeq ($(KEYSTONE_PLATFORM),mpfs)
HSS_DEPENDENCIES += keystone-sm
$(HSS_TARGET_CONFIGURE): keystone-sm-install

# Point HSS at the SM
HSS_MAKE_OPTS += KEYSTONE_SM=$(KEYSTONE_SM_BUILDDIR)

# Make keystone-sm dircleans also trigger hss-dircleans
keystone-sm-dirclean: hss-dirclean
else
OPENSBI_DEPENDENCIES += keystone-sm
$(OPENSBI_TARGET_CONFIGURE): keystone-sm-install

# Point OpenSBI at the correct location of the SM sources
OPENSBI_MAKE_ENV += PLATFORM_DIR=$(KEYSTONE_SM_BUILDDIR)/plat/

# For abi issue
OPENSBI_MAKE_ENV += PLATFORM_RISCV_TOOLCHAIN_DEFAULT=1
OPENSBI_MAKE_ENV += PLATFORM_RISCV_ABI=lp64d
OPENSBI_MAKE_ENV += FW_PAYLOAD_PATH=/home/luluji/keystone-omp/build-generic64/buildroot.build/images/Image
# OPENSBI_MAKE_ENV += FW_PAYLOAD=y
# OPENSBI_MAKE_ENV += FW_OPTIONS="console=ttyS0 ro root=/dev/vda"

# Make keystone-sm dircleans also trigger opensbi-dirclean
keystone-sm-dirclean: opensbi-dirclean
endif

$(eval $(keystone-package))
$(eval $(generic-package))
