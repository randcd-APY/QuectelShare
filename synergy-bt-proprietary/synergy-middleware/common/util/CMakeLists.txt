##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    connx_log.c
    connx_util.c
    connx_file.c
    connx_list.c
    connx_time.c
    connx_setting.c
    connx_log_setting.c
    connx_arg_search.c)

set(HDRS)

add_library(connx_util ${SRCS} ${HDRS})