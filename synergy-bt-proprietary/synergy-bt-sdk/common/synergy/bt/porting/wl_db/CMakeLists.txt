###############################################################################
#
# Copyright (c) 2018 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
###############################################################################
set(SRCS
    csr_bt_wl_db-file.c)

csr_add_library(csr_bt_wl_db
                COMPONENT CSR_COMPONENT_BT_PORTING
                SOURCES ${SRCS})
