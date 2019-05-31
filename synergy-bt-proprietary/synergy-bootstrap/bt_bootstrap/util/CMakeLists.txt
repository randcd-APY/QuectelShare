###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

set(SRCS
    bt_bootstrap.c 
    bt_bootstrap_util.c 
    bt_bootstrap_log.c 
    connx_serial_com.c
    connx_log_btsnoop.c)

set(HDRS)

add_library(bt_bootstrap_util ${SRCS} ${HDRS})