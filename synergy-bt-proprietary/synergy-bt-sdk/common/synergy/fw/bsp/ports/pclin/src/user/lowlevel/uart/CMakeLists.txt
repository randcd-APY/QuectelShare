##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    csr_serial_com.c)

set(HDRS)

set(PUBLIC_HDRS
    ../../../../../../inc/csr_serial_com.h
    ../../../../inc/platform/csr_serial_init.h)

csr_add_library(csr_ser_com
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS})
