################################################################################
#
# Keystone test
#
################################################################################

KEYSTONE_TEST = $(KEYSTONE)/test

ifeq ($(KEYSTONE_TEST),)
$(error KEYSTONE_TEST directory not defined)
else
include $(KEYSTONE)/mkutils/pkg-keystone.mk
endif

KEYSTONE_DRIVER_DEPENDENCIES += host-keystone-sdk

keystone-test-dirclean: keystone-examples-dirclean

ifeq ($(BR2_PACKAGE_KEYSTONE_TEST_STRESS),y)
define KEYSTONE_TEST_BUILD_CMDS
    cd $(@D)/stress; \
    if [ -f autogen.sh ]; then $(TARGET_MAKE_ENV) ./autogen.sh; fi; \
    $(TARGET_MAKE_ENV) ./configure CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" AR="$(TARGET_AR)" LD="$(TARGET_LD)" --host=$(TARGET_CROSS_ALIAS) --build=$(HOST_CROSS_ALIAS) $(TARGET_CONFIGURE_ARGS); \
    $(TARGET_MAKE_ENV) $(MAKE)
endef

define KEYSTONE_TEST_INSTALL_TARGET_CMDS
    $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/stress install DESTDIR=$(TARGET_DIR)
endef
endif # BR2_PACKAGE_KEYSTONE_TEST_STRESS

$(eval $(keystone-package))
$(eval $(generic-package))

