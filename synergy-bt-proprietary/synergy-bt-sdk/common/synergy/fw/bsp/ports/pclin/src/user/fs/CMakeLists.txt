##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    csr_posix_filesystem.c)

set(HDRS)

set(PUBLIC_HDRS
    ../../../../../inc/csr_file.h
    ../../../../../inc/csr_dir.h)

csr_add_library(csr_fs
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS}
                ALLOW_NON_ANSI)
