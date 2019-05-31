###############################################################################
# Copyright (c) 2012-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_gatt_demo_db_utils.c
    csr_bt_gatt_app_uuid.c)

set(HDRS
    csr_bt_gatt_demo_app.h
    csr_bt_gatt_demo_db_utils.h
    csr_bt_gatt_demo_defines.h)


csr_add_library(csr_bt_app_gatt
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_LE_ENABLE SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)

csr_add_subdirectory(generic_server)
csr_add_subdirectory(hogp)
csr_add_subdirectory(le_browser)
csr_add_subdirectory(proximity)
csr_add_subdirectory(rsc)
csr_add_subdirectory(thermometer)