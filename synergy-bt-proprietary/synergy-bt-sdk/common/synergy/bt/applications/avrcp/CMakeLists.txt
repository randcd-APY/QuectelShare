###############################################################################
#
# Copyright (c) 2012-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_avrcp_main.c
    csr_bt_avrcp_utils.c
    csr_bt_av2_avrcp.c
    csr_bt_avrcp_down.c
    csr_bt_avrcp_app_ui_sef.c
    csr_bt_avrcp_app_util.c )

set(HDRS
	csr_bt_avrcp.h
	csr_bt_avrcp_app_ui_sef.h
	csr_bt_avrcp_app_util.h
	csr_bt_avrcp_app_ui_strings.h
	../inc/csr_bt_avrcp_app_task.h)

if(NOT EXCLUDE_CSR_BT_AVRCP_MODULE_COVER_ART)
    csr_add_subdirectory(coverart_files)
endif()

csr_add_library(csr_bt_app_avrcp
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL NOT EXCLUDE_CSR_BT_AVRCP_MODULE SOURCES ${SRCS}
                HEADERS  ${HDRS}
                ALLOW_WARNINGS)

