###############################################################################
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB ${CSR_BT_ROOT}/applications/gatt
                                                           ${CSR_BT_ROOT}/profile_managers/inc)

set(SRCS
    csr_bt_hogd_app.c
    csr_bt_hogd_app_gatt_sef.c
    csr_bt_hogd_app_db.c
    csr_bt_hogd_app_ui_sef.c)

set(HDRS
    csr_bt_hogd_app.h
    csr_bt_hogd_app_db.h
    csr_bt_hogd_app_strings.h
    ${CSR_BT_ROOT}/applications/inc/csr_bt_gatt_app_task.h)


csr_add_library(csr_bt_app_hogd
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_LE_ENABLE SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)
