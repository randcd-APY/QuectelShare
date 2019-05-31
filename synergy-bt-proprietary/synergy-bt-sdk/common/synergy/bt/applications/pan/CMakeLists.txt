###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP ${CSR_BT_ROOT}/applications/pan/generic_app_files)
csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB ${CSR_BT_ROOT}/applications/pan/generic_app_files)

set(GENERIC_SRCS
    generic_app_files/csr_bt_pan_app_ui_sef.c
    generic_app_files/csr_bt_pan_app.c
    generic_app_files/csr_bt_generic_pan_app_ether_sef.c)

set(GENERIC_HDRS
    generic_app_files/csr_bt_pan_app.h
    generic_app_files/csr_bt_pan_strings.h
    generic_app_files/csr_bt_pan_app_ui_sef.h
    generic_app_files/csr_bt_pan_app_private_prim.h
    generic_app_files/csr_bt_generic_ethernet_prim.h
    ../inc/csr_bt_pan_app_task.h)

csr_add_library(csr_bt_app_pan
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                SOURCES ${GENERIC_SRCS}
                HEADERS ${GENERIC_HDRS}
                ALLOW_WARNINGS)