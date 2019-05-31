##############################################################################
#
# Copyright (c) 2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    connx_hfg_ril_main.c 
    connx_hfg_ril_sef.c)

include_directories("${MDM_ROOT}/common/inc/")
include_directories("${MDM_ROOT}/mcm-api/api/")

set(HDRS
    connx_hfg_ril_main.h
    connx_hfg_ril_sef.h)

add_library(connx_hfg_ril ${SRCS} ${HDRS})
