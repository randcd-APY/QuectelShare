###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_cmdline.c
    csr_bt_input_menu.c
    csr_bt_transport.c
    csr_bt_di_lib.c
    csr_bt_cod_decode.c
    csr_bt_converter.c
    csr_bt_sc_demo_app.c
    csr_bt_sc_demo_app_dopairing.c
    csr_bt_sc_demo_app_reject.c
    csr_bt_file.c
    csr_bt_declare_tasks.c
    csr_bt_init_tasks.c)
    
if(CSR_LOG_ENABLE)
    list(APPEND SRCS csr_bt_log.c)
endif()

set(HDRS
    csr_bt_app.h
    csr_bt_demoapp.h
    ../inc/csr_bt_platform.h
    ../inc/csr_bt_declare_tasks.h
    ../inc/csr_bt_ui_strings.h
    ../inc/csr_bt_use_tasks.h
    ../inc/csr_bt_file.h
    csr_bt_sc_demo_app.h
    csr_bt_input_menu.h
    ../../inc/csr_bt_log_tech_info_register.h)

set(SRCS_BDB
        ports/bdb/csr_bt_app_main.c 
        ports/bdb/csr_bt_platform.c)

set(SRCS_WIN
        ports/pcwin/csr_bt_app_main.c 
        ports/pcwin/csr_bt_platform.c)

set(SRCS_LIN
        ports/pclin/csr_bt_app_main.c
        ports/pclin/csr_bt_platform.c)   

csr_add_library(csr_bt_app_common
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                SOURCES ${SRCS}
                CONDITIONAL CSR_PLATFORM MATCHES WINDOWS SOURCES ${SRCS_WIN}
                CONDITIONAL CSR_PLATFORM MATCHES BDB2 OR CSR_PLATFORM MATCHES BDB3 SOURCES ${SRCS_BDB}
                CONDITIONAL CSR_PLATFORM MATCHES LINUX SOURCES ${SRCS_LIN}
                HEADERS ${HDRS}
                ALLOW_WARNINGS
                ALLOW_NON_ANSI)
