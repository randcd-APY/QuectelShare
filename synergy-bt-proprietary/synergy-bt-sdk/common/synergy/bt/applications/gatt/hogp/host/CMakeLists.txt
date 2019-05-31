###############################################################################
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB ${CSR_BT_ROOT}/applications/gatt)

set(SRCS
    csr_bt_hogh_app.c
    csr_bt_hogh_app_ui_sef.c
    csr_bt_hogh_app_sef.c)

set(HDRS
    csr_bt_hogh_app.h
    csr_bt_hogh_app_strings.h
    ${CSR_BT_ROOT}/applications/inc/csr_bt_gatt_app_task.h)


csr_add_library(csr_bt_app_hogh
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_LE_ENABLE AND 
                            CSR_BT_APP_USE_GATT_HOGH AND 
                            NOT EXCLUDE_CSR_BT_HOGH_MODULE 
                                SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)
