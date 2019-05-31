##############################################################################
#
# Copyright (c) 2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    bt_audio_service_main.c 
    bt_audio_service.c
    bt_audio_service_sef.c
    bt_audio_service_transport.c)

set(HDRS)

set(LIBS
    bt_audio_util
    bt_audio_hw
    connx_ipc
    connx_util)

add_executable(bt_audio_service ${SRCS} ${HDRS})
target_link_libraries(bt_audio_service ${LIBS} ${CONNX_SYSTEM_LIBS})
