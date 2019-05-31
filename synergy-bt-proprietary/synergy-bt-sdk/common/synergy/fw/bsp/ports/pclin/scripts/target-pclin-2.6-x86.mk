###############################################################################
#
# Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#                
###############################################################################

# Select 32bit compilation
TARGET_CFLAGS += -m32

# Detect support for OpenSSL
ifeq ($(shell pkg-config --exists openssl && echo yes),yes)
BSP_SUPPORT_OPENSSL := 1
TARGET_CFLAGS  += $(shell pkg-config --cflags openssl)
TARGET_LDFLAGS += $(shell pkg-config --libs openssl)
endif

# Detect support for SDL
ifeq ($(shell pkg-config --exists sdl && echo yes),yes)
BSP_SUPPORT_SDL := 1
ifneq ($(filter $(shell uname -pm),x86_64 ia64 amd64),)
TARGET_LIBS += SDL-1.2
else
TARGET_LIBS += SDL
endif
endif

ifeq ($(shell pkg-config --exists bluez && echo yes),yes)
BSP_SUPPORT_HCI_SOCKET := 1
endif

include $(TARGET_SCRIPT_ROOT)/target-default.mk

