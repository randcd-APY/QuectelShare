###############################################################################
#
# Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#                
###############################################################################

# Compiler prefix for selecting Android ARM compiler
COMPILER_PREFIX := android-arm-

# This flag is required by Android but not set by agcc script
TARGET_CFLAGS := -fno-short-enums

include $(TARGET_SCRIPT_ROOT)/target-default.mk

# Override target libraries specified in target-default.mk
TARGET_LIBS := cutils
