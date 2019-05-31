###############################################################################
#
# Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#                
###############################################################################

CC := $(COMPILER_PREFIX)gcc
LD := $(COMPILER_PREFIX)ld
AR := $(COMPILER_PREFIX)ar
CXX := $(CC)

# D-13752
NO_TREAT_WARN_AS_ERROR := 1

ifneq ($(NO_TREAT_WARN_AS_ERROR),1)
    TARGET_CFLAGS += -Werror
endif

ifeq ($(FORCE_ANSI),1) # D-13752
TARGET_CFLAGS += -ansi -pedantic
endif

TARGET_CFLAGS += -W -Wall -Wno-unused-parameter -Wdeclaration-after-statement -D_REENTRANT
LDFLAGS += -Wl,-Map=$(1).map

ifeq ($(DEBUG),1)
TARGET_CFLAGS += -g
#Enable exception handling. Generates extra code needed to propagate exceptions. 
#For some targets, this implies GCC generates frame unwind information for all 
#functions, which can produce significant data size overhead, although it does 
#not affect execution.
TARGET_CFLAGS += -fexceptions -funwind-tables
else
TARGET_CFLAGS += -O2 -fno-strict-aliasing
endif

TARGET_CXXFLAGS += $(TARGET_CFLAGS)
EXTRA_CXXFLAGS += $(EXTRA_CFLAGS)
CXXFLAGS += $(CFLAGS)

MODULE_TEST_MAIN ?= $(FW_ROOT)/test/module_test/csr_module_test_main.c
MODE=user

SUB_TARGET_ARCH := Linux
SUB_TARGET_VERSION := 2.6

TARGET_LIBS += pthread rt

# $(1) = name
lib_name = lib$(1).a

# $(1) = name
bin_name = $(1)

# $(1) = name
# $(2) = OBJ
create_lib = $(AR) -r $(1) $(2)

# $(1) = name
# $(2) = OBJ
# $(3) = LDPATH
# $(4) = LIBS
create_bin = $(CC) -o $(1) $(EXTRA_CFLAGS) $(TARGET_CFLAGS) $(CFLAGS) $(EXTRA_LDFLAGS) $(TARGET_LDFLAGS) $(LDFLAGS) $(2) $(3) $(4) $(4)
create_exec = $(CC) -o $(1) $(EXTRA_CFLAGS) $(TARGET_CFLAGS) $(CFLAGS) $(EXTRA_LDFLAGS) $(TARGET_LDFLAGS) $(LDFLAGS) $(2) $(3) $(4) $(4)

# Clean up after create_exec
#
# $(1) = Output file name (with path)
#
clean_exec = $(RM) $(1)

# $(1) = list of libs path.
create_ldpath = $(addprefix -L,$(1))

# $(1) = list of libs.
use_lib = $(addprefix -l,$(1))

ifeq ($(USE_ABSOLUTE_PATH),1)
abs_path = "$(shell pwd)/$(1)"
else
abs_path = $(1)
endif

#
# Rules
#
$(OBJ_PATH)/%.o:%.c
	@$(ECHO) $<
	$(MKDIR) $(@D)
	$(CC) $(EXTRA_CFLAGS) $(TARGET_CFLAGS) $(CFLAGS) $(INC) -c -o $@ $(call abs_path,$<)

$(OBJ_PATH)/%.o:%.cpp
	@$(ECHO) $<
	$(MKDIR) $(@D)
	$(CXX) $(EXTRA_CXXFLAGS) $(TARGET_CXXFLAGS) $(CXXFLAGS) $(INC) -c -o $@ $(call abs_path,$<)
