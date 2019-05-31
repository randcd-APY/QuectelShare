##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    csr_panic.c)

set(HDRS)

set(PUBLIC_HDRS
    ../../../../../inc/csr_panic.h
    ../../../inc/platform/csr_panic_init.h)

csr_add_library(csr_panic
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS})
