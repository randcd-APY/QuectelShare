###############################################################################
#
# Copyright (c) 2011-2013 Qualcomm Technologies International, Ltd. 
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################
set(SRCS
    csr_bt_sc_db-file.c)

csr_add_library(csr_bt_sc_db
                COMPONENT CSR_COMPONENT_BT_PORTING
                SOURCES ${SRCS})
