###############################################################################
#
# Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#                
###############################################################################

# D-13752
NO_TREAT_WARN_AS_ERROR := 1

CC := $(COMPILER_PREFIX)gcc
LD := $(COMPILER_PREFIX)ld
AR := $(COMPILER_PREFIX)ar
CXX := $(CC)

KDIR := /lib/modules/$(shell uname -r)/build

KERNEL_VERSION := $(shell uname -r | cut -d'.' -f1-3 | cut -d'-' -f1)

MODPOST_VER_CHECK=$(shell cd $(TARGET_SCRIPT_ROOT);sh ./linux_ver_check.sh 2.6.9 $(KERNEL_VERSION))
KBUILD_VER_CHECK=$(shell cd $(TARGET_SCRIPT_ROOT);sh ./linux_ver_check.sh 2.6.11 $(KERNEL_VERSION))
KBUILD_VER_CHECK2=$(shell cd $(TARGET_SCRIPT_ROOT);sh ./linux_ver_check.sh 2.6.15 $(KERNEL_VERSION))
KBUILD_VER_CHECK3=$(shell cd $(TARGET_SCRIPT_ROOT);sh ./linux_ver_check.sh 2.6.23 $(KERNEL_VERSION))

ifeq ($(MODPOST_VER_CHECK),0)
MOD_POST := $(KDIR)/scripts/mod/modpost
else
MOD_POST := $(KDIR)/scripts/modpost
endif

correct_path_names=$(subst -,_,$1)

TARGET_CFLAGS += -nostdinc -isystem $(shell $(CC) -print-file-name=include) \
	-Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing \
	-fno-common -pipe -fno-builtin -fno-builtin-sprintf \
	-fno-builtin-log2 -fno-builtin-puts -mpreferred-stack-boundary=2  \
	-march=i686 -mregparm=3 \
	-Os -fomit-frame-pointer

ifneq ($(NO_TREAT_WARN_AS_ERROR),1)
    TARGET_CFLAGS += -Werror
endif

TARGET_LIBS :=

CFLAGS += -D__KERNEL__ -DMODULE

ifeq (0,1)
CFLAGS += -I $(KDIR)/include/linux -I $(KDIR)/include
ifeq ($(KBUILD_VER_CHECK3),0)
CFLAGS += -imacros linux/autoconf.h
CFLAGS += -iwithprefix include -Iinclude -I$(KDIR)/include/asm-x86/mach-default
else
CFLAGS += -iwithprefix include -Iinclude -I$(KDIR)/include/asm-i386/mach-default
endif
endif

CFLAGS += -imacros linux/autoconf.h
CFLAGS += -I$(KDIR)/arch/x86/include
CFLAGS += -Iinclude
CFLAGS += -Iubuntu/include
CFLAGS += -I$(KDIR)/include

INC+= -I $(KDIR)/include

ifeq ($(DEBUG),1)
CFLAGS += -g
else
CFLAGS += -s
endif

TARGET_CXXFLAGS += $(TARGET_CFLAGS)
EXTRA_CXXFLAGS += $(EXTRA_CFLAGS)
CXXFLAGS += $(CFLAGS)

LD_MODULE_FLAGS += -m elf_i386

MODE=kernel

SUB_TARGET_ARCH := LinuxKernel

#
# Macro definitions
#
lib_name = lib$(1).a
bin_name = $(1).ko
create_lib = $(LD) $(LD_MODULE_FLAGS) -r -o $(1) $(2)

# $(1) = name
# $(2) = OBJ
# $(3) = LDPATH
# $(4) = LIBS
ifeq ($(KBUILD_VER_CHECK2),1)
create_bin = $(LD) $(LD_MODULE_FLAGS) -r -o m_$(1).o $(2) $(3) $(4);\
	$(MOD_POST) -i $(KDIR)/Module.symvers m_$(1).o;\
	$(CC) $(CFLAGS) $(TARGET_CFLAGS) $(EXTRA_CFLAGS) \
	-DKBUILD_BASENAME=\"$(MODULE_NAME)\" \
	-DKBUILD_MODNAME=$(MODULE_NAME) -c -o m_$(1).mod.o \
	m_$(1).mod.c $(INC);\
	$(LD) $(LD_MODULE_FLAGS) -r -o $(1)  m_$(1).o m_$(1).mod.o 
else
create_bin = $(LD) $(LD_MODULE_FLAGS) -r -o m_$(1).o $(2) $(3) $(4);\
	$(MOD_POST) -i $(KDIR)/Module.symvers m_$(1).o;\
	$(CC) $(CFLAGS) $(TARGET_CFLAGS) $(EXTRA_CFLAGS) \
	-DKBUILD_BASENAME=\"$(MODULE_NAME)\" \
	-DKBUILD_MODNAME=\"$(MODULE_NAME)\" -c -o m_$(1).mod.o \
	m_$(1).mod.c $(INC);\
	$(LD) $(LD_MODULE_FLAGS) -r -o $(1)  m_$(1).o m_$(1).mod.o 
endif


# $(1) = list of libs path.
create_ldpath = $(addprefix -L,$(1))

use_lib = $(addprefix -l,$(1))


#
# Rules
# Different versions of rwsem.h require different levels of quotation.
#
ifeq ($(KBUILD_VER_CHECK),0)
$(OBJ_PATH)/%.o:%.c
	$(MKDIR) $(@D)
	$(CC) $(EXTRA_CFLAGS) $(TARGET_CFLAGS) \
	-DKBUILD_BASENAME=\"$(call correct_path_names,$(*F))\" \
	-DKBUILD_MODNAME=\"$(MODULE_NAME)\" \
	$(CFLAGS) $(INC) -c -o $@ $<

$(OBJ_PATH)/%.o:%.cpp
	$(MKDIR) $(@D)
	$(CXX) $(EXTRA_CXXFLAGS) $(TARGET_CXXFLAGS) \
	-DKBUILD_BASENAME=\"$(call correct_path_names,$(*F))\" \
	-DKBUILD_MODNAME=\"$(MODULE_NAME)\" \
	$(CXXFLAGS) $(INC) -c -o $@ $<
else
$(OBJ_PATH)/%.o:%.c
	$(MKDIR) $(@D)
	$(CC) $(EXTRA_CFLAGS) $(TARGET_CFLAGS) \
	-D"KBUILD_BASENAME=$(call correct_path_names,$(*F))" \
	$(CFLAGS) $(INC) -c -o $@ $<

$(OBJ_PATH)/%.o:%.cpp
	$(MKDIR) $(@D)
	$(CXX) $(EXTRA_CXXFLAGS) $(TARGET_CXXFLAGS) \
	-D"KBUILD_BASENAME=$(call correct_path_names,$(*F))" \
	$(CXXFLAGS) $(INC) -c -o $@ $<
endif

