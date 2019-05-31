###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_bootstrap_generic.c)
    
set(SRCS_SB
    csr_bt_bootstrap_specific_string_based.c)

set(SRCS_FB 
    csr_bt_bootstrap_specific_file_based.c)

set(SRCS_P  
    csr_bt_bootstrap_specific_platform.c)

set(HDRS
    csr_bt_bootstrap_generic.h
    csr_bt_bootstrap_specific.h)

csr_add_library(csr_bt_bootstrap
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                SOURCES ${SRCS}
                CONDITIONAL CSR_FRW_GROUP_APP_BOOTSTRAP STREQUAL "File" SOURCES ${SRCS_FB}
                CONDITIONAL CSR_FRW_GROUP_APP_BOOTSTRAP STREQUAL "String" SOURCES ${SRCS_SB}
                CONDITIONAL CSR_FRW_GROUP_APP_BOOTSTRAP STREQUAL "Platform" SOURCES ${SRCS_P}
                HEADERS ${HDRS} 
                ALLOW_WARNINGS
                ALLOW_NON_ANSI)
                
