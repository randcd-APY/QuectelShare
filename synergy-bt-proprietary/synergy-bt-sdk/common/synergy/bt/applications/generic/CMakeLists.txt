###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    ../common/csr_bt_init_tasks.c 
    ../common/csr_bt_file.c
    csr_bt_tasks.c
    csr_bt_declare_tasks.c)

set(HDRS
    ../inc/csr_bt_app_prim.h
    ../inc/csr_bt_spp_app_task.h
    ../inc/csr_bt_pan_app_task.h
    ../inc/csr_bt_gatt_app_task.h)

set(LIBS   
    csr_bt_bootstrap
    csr_bt_app_gap)

if(NOT EXCLUDE_CSR_BT_AV_MODULE)
    list(APPEND LIBS
        csr_bt_app_av)
    if ((CSR_PLATFORM MATCHES LINUX AND CSR_BT_APP_AV_FILTER STREQUAL "sbcaudio"))
        list(APPEND LIBS    
            asound)
    endif()
    if ((CSR_PLATFORM MATCHES LINUX AND CSR_BT_APP_AV_FILTER STREQUAL "sbc_SRCwave_SNKaudio"))
        list(APPEND LIBS    
            asound)
    endif()
endif()

if(NOT EXCLUDE_CSR_BT_HFG_MODULE)
    list(APPEND LIBS
    csr_bt_app_hfg)
endif()

if(NOT EXCLUDE_CSR_BT_HF_MODULE)
    list(APPEND LIBS
    csr_bt_app_hf)
endif()

if(NOT EXCLUDE_CSR_BT_PAC_MODULE AND NOT CSR_BT_APP_USE_PAC STREQUAL "0")
    list(APPEND LIBS
    csr_bt_app_pac)
endif()

if(NOT EXCLUDE_CSR_BT_SPP_MODULE)
    list(APPEND LIBS
    csr_bt_app_spp)
endif()

if(NOT EXCLUDE_CSR_BT_BSL_MODULE AND NOT EXCLUDE_CSR_BT_BNEP_MODULE)
    list(APPEND LIBS
    csr_bt_app_pan)
endif()

if(CSR_BT_LE_ENABLE)
    list(APPEND LIBS
         csr_bt_app_therms
         csr_bt_app_thermc
         csr_bt_app_prox_srv
         csr_bt_app_proxc
         csr_bt_app_le_browser
         csr_bt_app_generic_server
         csr_bt_app_gatt
         csr_bt_app_rscs
         csr_bt_app_rscc)
    if(NOT EXCLUDE_CSR_BT_HOGH_MODULE)
        list(APPEND LIBS csr_bt_app_hogh)
    endif()
    if(CSR_BT_APP_USE_GATT_HOGD)
        list(APPEND LIBS csr_bt_app_hogd)
    endif()
endif()

list(APPEND LIBS
    csr_app_main_bluecore
    csr_eh
    csr_framework_ext
    csr_fs
    csr_main
    csr_panic
    csr_pmem
    csr_ser_com
    csr_time
    csr_util
    csr_ip
    csr_ip_ether_lib
    csr_ip_socket_lib
    csr_ip_ifconfig_lib)
if(EXCLUDE_CSR_BT_ASM_MODULE OR (DEFINED CSR_BT_INTEROP_TEST_BUILD AND CSR_BT_INTEROP_TEST_BUILD))
    csr_add_executable(csr_bt_app_default_hci
                       COMPONENT CSR_COMPONENT_BT_APP
                       SOURCES ${SRCS}
                       HEADERS ${HDRS}
                       LIBRARIES ${LIBS} SYSTEM
                       ALLOW_NON_ANSI
                       ALLOW_WARNINGS)

    message(STATUS "CMAKE_CURRENT_LIST_FILE: ${CMAKE_CURRENT_LIST_FILE}")

    get_filename_component(_LIB_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)
    get_filename_component(_LIB_PATH "${_LIB_PATH}" PATH)
    get_filename_component(_LIB_PATH "${_LIB_PATH}" PATH)
    get_filename_component(_LIB_PATH "${_LIB_PATH}" PATH)

    set(_BT_LIB_PATH ${_LIB_PATH}/output/host/lib)
    set(_LIB_PREFIX lib)
    set(_LIB_SUFFIX a)

    if(BT_CHIP_TYPE STREQUAL "QCA")
        TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}synergy_bt_qca.${_LIB_SUFFIX})
    else()
        TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}synergy_bt_bluecore.${_LIB_SUFFIX})
    endif()

    TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}csr_bt_sc_db.${_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}csr_bt_wl_db.${_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}connx_util.${_LIB_SUFFIX})
    
    if(USE_MDM_PLATFORM)
        TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}connx_handsfree_audio.${_LIB_SUFFIX})
        TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}connx_peer_com.${_LIB_SUFFIX})
        TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}connx_ipc.${_LIB_SUFFIX}) 
        if(USE_HFG_RIL)
            TARGET_LINK_LIBRARIES(csr_bt_app_default_hci ${_BT_LIB_PATH}/${_LIB_PREFIX}connx_hfg_ril.${_LIB_SUFFIX})
            TARGET_LINK_LIBRARIES(csr_bt_app_default_hci mcm)
        endif()
    endif() 
endif()
