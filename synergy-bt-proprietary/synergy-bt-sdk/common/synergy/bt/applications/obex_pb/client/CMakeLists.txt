###############################################################################
#
# Copyright (c) 2014-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_pac_app_ui_sef.c
    csr_bt_pac_app.c)

set(HDRS
	csr_bt_pac_app.h
	csr_bt_pac_app_strings.h
	csr_bt_pac_app_ui_sef.h
	../../inc/csr_bt_pac_app_files.h
	../../inc/csr_bt_pac_app_task.h)

csr_add_library(csr_bt_app_pac
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL CSR_BT_APP_USE_PAC AND NOT EXCLUDE_CSR_BT_PAC_MODULE SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)
