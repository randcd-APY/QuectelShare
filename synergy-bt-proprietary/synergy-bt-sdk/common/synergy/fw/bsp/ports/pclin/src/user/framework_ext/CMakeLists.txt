##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    csr_framework_ext.c)

set(HDRS)

set(PUBLIC_HDRS
    ../../../../../inc/csr_framework_ext.h
    ../../../../../inc/csr_mem_hook.h
    ../../../inc/csr_framework_ext_types.h)

csr_add_library(csr_framework_ext
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS})
