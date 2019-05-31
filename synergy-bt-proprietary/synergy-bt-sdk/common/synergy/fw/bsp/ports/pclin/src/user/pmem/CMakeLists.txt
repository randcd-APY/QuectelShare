##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    csr_pmem.c)

set(HDRS)

set(PUBLIC_HDRS
    ../../../../../inc/csr_pmem.h
    ../../../../../inc/csr_pmem_hook.h
    ../../../inc/platform/csr_pmem_init.h)

csr_add_library(csr_pmem
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS})
