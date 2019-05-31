###############################################################################
#
# Copyright (c) 2014-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_dsp_patching.c)

if (CSR_BT_APP_DSP_PATCHING STREQUAL "File")
    list(APPEND SRCS
         csr_bt_dsp_patching_file.c)
endif()

if (CSR_BT_APP_DSP_PATCHING STREQUAL "Memory")
    list(APPEND SRCS
         csr_bt_dsp_patching_memory.c
         patch_files/PB-109_GeminiA08_V8.c
         patch_files/PB-132_B-128945_DaleAutoA07_V4.c)
endif()

set(HDRS
    csr_bt_dsp_patching.h
    csr_bt_dsp_patching_generic.h)

csr_add_library(csr_bt_dsp_patching
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_DSPM_ENABLE AND NOT CSR_DSPM_SUPPORT_ACCMD SOURCES ${SRCS}
                HEADERS ${HDRS} 
                ALLOW_WARNINGS
                ALLOW_NON_ANSI)
