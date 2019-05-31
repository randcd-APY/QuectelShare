##############################################################################
#
#    Copyright (c) 2011-2013 Qualcomm Technologies International, Ltd. 
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
##############################################################################

set(SRCS
    csr_bt_app_main.c
    csr_bt_platform.c)

csr_add_library(csr_bt_app_common_ported
                COMPONENT CSR_COMPONENT_BT_APP_PORTING
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS
                ALLOW_NON_ANSI)
