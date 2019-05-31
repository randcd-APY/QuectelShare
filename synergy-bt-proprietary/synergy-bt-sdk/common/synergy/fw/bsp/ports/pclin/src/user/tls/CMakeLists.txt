##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_FRW_TLS
                        ../../../../../../gsp/src/inc)

set(SRCS
    csr_tls_free_down.c
    csr_tls_free_up.c
    csr_tls_handler.c
    csr_tls_sef_tls.c
    csr_tls_util.c
    csr_tls_thread.c
    csr_tls_lib.c)

set(HDRS
    csr_tls_handler.h
    csr_tls_sef_tls.h
    csr_tls_thread.h
    csr_tls_util.h)

set(PUBLIC_HDRS
    ../../../../../inc/csr_tls_lib.h
    ../../../../../inc/csr_tls_prim.h
    ../../../../../inc/csr_tls_task.h)

csr_add_library(csr_tls
                COMPONENT CSR_COMPONENT_FRW_TLS
                CONDITIONAL OPENSSL_FOUND AND NOT CSR_IP_SUPPORT_TLS SOURCES ${SRCS}
                CONDITIONAL OPENSSL_FOUND AND NOT CSR_IP_SUPPORT_TLS HEADERS ${HDRS}
                CONDITIONAL OPENSSL_FOUND AND NOT CSR_IP_SUPPORT_TLS PUBLIC_HEADERS ${PUBLIC_HDRS}
                ALLOW_NON_ANSI)
