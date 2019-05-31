###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB ${CSR_BT_ROOT}/porting/inc)

set(SRCS_HFG
    csr_bt_hfg_app_ui_sef.c
    csr_bt_hfg_demo_utils.c
    csr_bt_hfg_menu_utils.c
    csr_bt_hfg_app_util.c
    csr_bt_hfg_handler_hfg.c
    csr_bt_hfg_demo_app.c)

set(HDRS_HFG
    csr_bt_hfg_app_ui_sef.h
    csr_bt_hfg_app_ui_strings.h
    csr_bt_hfg_app_util.h
    csr_bt_hfg_demo_app.h
    csr_bt_hfg_menu_utils.h
    ../inc/csr_bt_hfg_app_task.h)

set(SRCS_HF
    csr_bt_hf_app_ui_sef.c
    csr_bt_hf_demo_app.c
    csr_bt_hf_app_sef.c
    csr_bt_hf_demo_app_lib.c
    csr_bt_hf_demo_handler.c)

set(HDRS_HF
    csr_bt_hf_app_ui_sef.h
    csr_bt_hf_app_ui_strings.h
    csr_bt_hf_demo_app.h
    csr_bt_hf_app_sef.h
    ../inc/csr_bt_hf_app_task.h
    ../inc/csr_bt_hf_app_lib.h
    ../inc/csr_bt_hf_app_prim.h
    ../inc/csr_bt_avrcp_app_prim.h
    ../inc/csr_bt_pac_app_files.h)

csr_add_library(csr_bt_app_hfg
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL NOT EXCLUDE_CSR_BT_HFG_MODULE SOURCES ${SRCS_HFG}
                HEADERS ${HDRS_HFG}
                ALLOW_WARNINGS)

csr_add_library(csr_bt_app_hf
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL NOT EXCLUDE_CSR_BT_HF_MODULE SOURCES ${SRCS_HF}
                HEADERS ${HDRS_HF}
                ALLOW_NON_ANSI
                ALLOW_WARNINGS)

