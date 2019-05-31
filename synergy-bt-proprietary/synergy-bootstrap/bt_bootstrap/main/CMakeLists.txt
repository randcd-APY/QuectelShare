###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

set(SRCS
    bt_bootstrap_main.c 
    ../util/bt_bootstrap.c 
    ../util/bt_bootstrap_util.c 
    ../util/bt_bootstrap_log.c 
    ../util/connx_serial_com.c
    ../util/connx_log_btsnoop.c)

set(HDRS)

set(LIBS
    connx_util)

if (NOT EXCLUDE_BOOT_QCA)
    list(APPEND LIBS
         boot_qca)
endif()

if (NOT EXCLUDE_BOOT_BLUECORE)
    list(APPEND LIBS
         boot_bluecore)
endif()

add_executable(bt_bootstrap ${SRCS} ${HDRS})
target_link_libraries(bt_bootstrap ${LIBS} ${CONNX_SYSTEM_LIBS})