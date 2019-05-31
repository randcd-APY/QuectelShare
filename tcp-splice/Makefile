KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

obj-m += tcp_splice.o

tcp_splice-objs := tcp-splice-interface.o tcp-splice-util.o tcp-splice.o

CDEFINES += -D__CHECK_ENDIAN__
KBUILD_CPPFLAGS += $(CDEFINES)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules

modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(shell pwd) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
