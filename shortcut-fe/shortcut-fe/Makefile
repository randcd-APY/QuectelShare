#
# Copyright (c) 2013-2015 The Linux Foundation. All rights reserved.
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all copies.
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
# OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Makefile for Shortcut FE.
#
SFE_SUPPORT_IPV6 = IPV6
KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
DEBUG_CFLAGS = -g
KVER = $(KERNEL_VERSION)

ifeq ($(KVER),$(filter 4.9%,$(KVER)))
ccflags-y += -DISKERNELUPGRADED
endif

ifeq ($(KVER),$(filter 4.14%,$(KVER)))
ccflags-y += -DISKERNELUPGRADED -DISKERNEL4_14
endif
obj-m += shortcut-fe.o

ifdef SFE_SUPPORT_IPV6
obj-m += shortcut-fe-ipv6.o
endif

obj-m += shortcut-fe-cm.o

shortcut-fe-objs := \
	sfe_ipv4.o

ifdef SFE_SUPPORT_IPV6
shortcut-fe-ipv6-objs := \
	sfe_ipv6.o
endif

shortcut-fe-cm-objs := \
	sfe_cm.o

	ccflags-y += -D__CHECK_ENDIAN__
	ccflags-y += $(CFLAGS)
ifdef CFLAGS
	ccflags-y += -g
endif
CDEFINES += -D__CHECK_ENDIAN__

KBUILD_CPPFLAGS += $(CDEFINES)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules
debug:
	 $(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules EXTRA_CFLAGS=$(DEBUG_CFLAGS)
modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(shell pwd) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

