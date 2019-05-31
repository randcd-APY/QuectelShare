##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    connx_app_bas_transport.c
    connx_app_bas_external_lib.c
    connx_app_transport.c
    connx_peer_com_main.c
    connx_peer_com_sef.c)

set(HDRS)

#set(LIBS
#    connx_ipc)

add_library(connx_peer_com ${SRCS} ${HDRS})
#target_link_libraries(connx_peer_com ${LIBS})