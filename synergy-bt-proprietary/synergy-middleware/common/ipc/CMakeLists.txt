##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    connx_ipc_common.c
    connx_ipc.c
    connx_ipc_socket.c
    connx_socket.c
    connx_socket_client.c
    connx_socket_server.c)

set(HDRS)

add_library(connx_ipc ${SRCS} ${HDRS})

