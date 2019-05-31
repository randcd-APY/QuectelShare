###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB ${CSR_BT_ROOT}/applications/gatt)

set(SRCS
	csr_bt_gatt_demo_le_browser_app.c
	csr_bt_gatt_demo_le_browser_app_menus.c
	csr_bt_gatt_demo_le_browser_app_ui_sef.c)

set(HDRS
	csr_bt_gatt_demo_le_browser_app.h
	csr_bt_gatt_demo_le_browser_app_defines.h
	csr_bt_gatt_demo_le_browser_app_menus.h
	csr_bt_gatt_demo_le_browser_app_private_prim.h
	csr_bt_gatt_demo_le_browser_app_strings.h
	csr_bt_gatt_demo_le_browser_app_ui_sef.h
	../csr_bt_gatt_demo_app.h
	../../inc/csr_bt_gatt_app_task.h
	)


csr_add_library(csr_bt_app_le_browser
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_LE_ENABLE SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)
