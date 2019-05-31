###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB ${CSR_BT_ROOT}/applications/gatt)

set(SRCS_SRV
	csr_bt_gatt_demo_prox_server_app.c
	csr_bt_gatt_demo_prox_server_app_ui_sef.c)

set(HDRS_SRV
	csr_bt_gatt_demo_prox_server_app.h
	csr_bt_gatt_demo_prox_server_app_private_prim.h
	csr_bt_gatt_demo_prox_server_app_strings.h
	csr_bt_gatt_demo_prox_server_app_ui_sef.h
	)

set(SRCS_CLI
	csr_bt_gatt_demo_prox_client_app.c
	csr_bt_gatt_demo_prox_client_app_ui_sef.c)

set(HDRS_CLI
	csr_bt_gatt_demo_prox_client_app.h
	csr_bt_gatt_demo_prox_client_app_private_prim.h
	csr_bt_gatt_demo_prox_client_app_strings.h
	csr_bt_gatt_demo_prox_client_app_ui_sef.h
	)

set(HDRS_COMMON
	../../inc/csr_bt_gatt_app_task.h)
	
csr_add_library(csr_bt_app_prox_srv
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_LE_ENABLE SOURCES ${SRCS_SRV}
                HEADERS ${HDRS_SRV} ${HDRS_COMMON}
                ALLOW_WARNINGS)
	
csr_add_library(csr_bt_app_proxc
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_LE_ENABLE SOURCES ${SRCS_CLI}
                HEADERS ${HDRS_CLI} ${HDRS_COMMON}
                ALLOW_WARNINGS)