###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_gap_app_cm.c
    csr_bt_gap_app_cm_event_handler.c
    csr_bt_gap_app_handler.c
    csr_bt_gap_app_lib.c
    csr_bt_gap_app_sc.c
    csr_bt_gap_app_sc_event_handler.c
    csr_bt_gap_app_sd.c
    csr_bt_gap_app_sd_event_handler.c
    csr_bt_gap_app_ui_sef.c
    csr_bt_gap_app_util.c)

set(HDRS
	csr_bt_gap_app_cm.h
	csr_bt_gap_app_cm_event_handler.h
	csr_bt_gap_app_handler.h
	csr_bt_gap_app_sc.h
	csr_bt_gap_app_sc_event_handler.h
	csr_bt_gap_app_sd.h
	csr_bt_gap_app_sd_event_handler.h
	csr_bt_gap_app_ui_sef.h
	csr_bt_gap_app_ui_strings.h
	csr_bt_gap_app_util.h
	../inc/csr_bt_gap_app_prim.h
	../inc/csr_bt_gap_app_lib.h
	../inc/csr_bt_gap_app_task.h)
	

csr_add_library(csr_bt_app_gap
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)
