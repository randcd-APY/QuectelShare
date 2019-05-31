
KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

ifeq ($(or $(CONFIG_ARCH_MDM9640), $(CONFIG_ARCH_MDM9650), $(CONFIG_ARCH_SDX20)), y)
obj-m += alx.o
CDEFINES += -DMDM_PLATFORM
else

ifeq ($(CONFIG_ARCH_MSM8998), y)
obj-m += alx.o
CDEFINES += -DAPQ_PLATFORM
else
obj-$(CONFIG_ALX) += alx.o
endif

endif
alx-objs := alx_main.o alx_ethtool.o alc_cb.o alc_hw.o alf_cb.o alf_hw.o
ccflags-y += -D__CHECK_ENDIAN__

CDEFINES += -D__CHECK_ENDIAN__

KBUILD_CPPFLAGS += $(CDEFINES)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules
modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(shell pwd) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
