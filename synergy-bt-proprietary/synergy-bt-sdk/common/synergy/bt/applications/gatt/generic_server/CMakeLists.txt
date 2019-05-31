###############################################################################
#
# Copyright (c) 2012-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB ${CSR_BT_ROOT}/applications/gatt)

set(SRCS
	csr_bt_gatt_demo_generic_server_app.c
	csr_bt_gatt_demo_generic_server_app_ui_sef.c
	csr_bt_gatt_demo_generic_srv_db.c
	csr_bt_gatt_demo_generic_srv_lib.c)

set(HDRS
	csr_bt_gatt_demo_generic_server_app.h
	csr_bt_gatt_demo_generic_server_app_private_prim.h
	csr_bt_gatt_demo_generic_server_app_strings.h
	csr_bt_gatt_demo_generic_server_app_ui_sef.h
	csr_bt_gatt_demo_generic_srv_db.h
	csr_bt_gatt_demo_generic_srv_lib.h
	csr_bt_gatt_demo_generic_srv_prim.h
	../csr_bt_gatt_demo_app.h
	../csr_bt_gatt_demo_defines.h
	../../inc/csr_bt_gatt_app_task.h
	)


csr_add_library(csr_bt_app_generic_server
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_LE_ENABLE SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)
