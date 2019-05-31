##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    csr_log2file.c
    csr_log2file_async.c
    csr_log2stdout.c
    csr_logtransport.c)

set(HDRS)

set(PUBLIC_HDRS
    ../../../../../inc/csr_logtransport.h
    ../../../inc/platform/csr_logtransport_init.h)

csr_add_library(csr_logtrans
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS}
                ALLOW_NON_ANSI)
