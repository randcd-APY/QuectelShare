###############################################################################
#
# Copyright (c) 2012-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS_PBAPC
	app_files/csr_bt_tasks.c 
	app_files/csr_bt_obex_pb_client_demo_app.c)

set(LIBS
    csr_am
    csr_app
    csr_arg_search
    csr_bccmd
    csr_bcsp
    csr_bluecore_manager
    csr_bt
    csr_bt_lib    
    csr_bt_bootstrap
    csr_bt_app_common
    csr_bt_app_gap
    csr_bt_corestack_common
    csr_bt_corestack_hci
    csr_bt_obex_common
    csr_sbc
    csr_bt_sc_db
    csr_core_msg_converter
    csr_data_store
    csr_data_store_lib
    csr_eh
    csr_fastpipe
    csr_formatted_io
    csr_framework_ext
    csr_fs
    csr_fsal
    csr_fsal_lib
    csr_hci
    csr_hq
    csr_list
    csr_mblk
    csr_message_queue
    csr_msg_converter
    csr_panic
    csr_pmem
    csr_queue_lib
    csr_random
    csr_sched
    csr_ser_com
    csr_time
    csr_tm_bluecore
    csr_ui
    csr_unicode
    csr_util
    csr_bt_app_common_ported)

if (CSR_H4_TRANSPORT STREQUAL H4DS)
    list(APPEND LIBS
         csr_h4ds)
endif()

if (CSR_H4_TRANSPORT STREQUAL H4)
    list(APPEND LIBS
         csr_h4)
endif()

if (CSR_BT_CHIP_TYPE STREQUAL QCA)
    list(APPEND LIBS
         csr_qvsc)
endif()

if (CSR_HCI_SOCKET_TRANSPORT)
    list(APPEND LIBS
         csr_hci_socket)
endif()

if(CSR_LOG_ENABLE)
    list(APPEND LIBS
        csr_bt_msg_converter
        csr_log
        csr_log_btsnoop
        csr_log_cleartext
        csr_log_fts
        csr_log_pcap
        csr_logtrans)
endif()

csr_add_executable(hci_pbapc_app
                   COMPONENT CSR_COMPONENT_BT_APP
                   CONDITIONAL NOT EXCLUDE_CSR_BT_PAC_MODULE SOURCES ${SRCS_PBAPC}
                   LIBRARIES ${LIBS} SYSTEM
                   ALLOW_WARNINGS)

