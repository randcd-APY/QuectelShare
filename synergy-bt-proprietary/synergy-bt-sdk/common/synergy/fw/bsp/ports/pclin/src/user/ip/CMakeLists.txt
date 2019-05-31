##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
##############################################################################

# Needed for csr_autogen.h in free function files.
csr_include_directories(COMPONENT CSR_COMPONENT_FRW_BSP_CORE ${CSR_FRW_ROOT}/gsp/src/inc)

set(SRCS
    csr_ip_handler.c
    csr_ip_socket_sef.c
    csr_ip_thread.c
    csr_ip_util.c)

set(SRCS_ETHER
    csr_ip_ether_sef.c)

set(SRCS_IFCONFIG
    csr_ip_netlink_parser.c
    csr_ip_ifconfig_sef.c)

set(SRCS_TLS
    csr_ip_tls_free_down.c
    csr_ip_tls_free_up.c
    csr_ip_tls_handler.c
    csr_ip_tls_sef.c
    csr_ip_tls_lib.c
    csr_ip_tls_util.c
    csr_ip_tls_thread.c)

set(SRCS_ETHER_LIB
    csr_ip_ether_lib.c
    csr_ip_ether_private_lib.c
    csr_ip_ether_free_down.c
    csr_ip_ether_free_up.c)

set(SRCS_IFCONFIG_LIB
    csr_ip_ifconfig_lib.c
    csr_ip_ifconfig_private_lib.c
    csr_ip_ifconfig_free_down.c
    csr_ip_ifconfig_free_up.c)

set(SRCS_SOCKET_LIB
    csr_ip_socket_free_down.c
    csr_ip_socket_free_up.c
    csr_ip_socket_lib.c
    csr_ip_socket_private_lib.c)

set(HDRS
    csr_ip_handler.h
    csr_ip_netlink_parser.h
    csr_ip_socket_sef.h
    csr_ip_thread.h
    csr_ip_util.h)

set(HDRS_ETHER
    csr_ip_ether_sef.h)

set(HDRS_IFCONFIG
    csr_ip_ifconfig_sef.h)

set(PUBLIC_HDRS
    ../../../../../inc/csr_ip_socket_lib.h
    ../../../../../inc/csr_ip_socket_prim.h
    ../../../../../inc/csr_ip_socket_task.h
    ../../../../../inc/csr_ip_task.h)

set(PUBLIC_HDRS_ETHER
    ../../../../../inc/csr_ip_ether_lib.h
    ../../../../../inc/csr_ip_ether_prim.h
    ../../../../../inc/csr_ip_ether_task.h)

set(PUBLIC_HDRS_IFCONFIG
    ../../../../../inc/csr_ip_ifconfig_lib.h
    ../../../../../inc/csr_ip_ifconfig_prim.h
    ../../../../../inc/csr_ip_ifconfig_task.h)

set(HDRS_TLS
    csr_ip_tls_handler.h
    csr_ip_tls_sef.h
    csr_ip_tls_thread.h
    csr_ip_tls_util.h)

set(PUBLIC_HDRS_TLS
    ../../../../../inc/csr_tls_lib.h
    ../../../../../inc/csr_tls_prim.h
    ../../../../../inc/csr_tls_task.h)

csr_add_library(csr_ip
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS}
                CONDITIONAL CSR_IP_SUPPORT_ETHER SOURCES ${SRCS_ETHER}
                CONDITIONAL CSR_IP_SUPPORT_ETHER HEADERS ${HDRS_ETHER}
                CONDITIONAL CSR_IP_SUPPORT_ETHER PUBLIC_HEADERS ${PUBLIC_HDRS_ETHER}
                CONDITIONAL CSR_IP_SUPPORT_IFCONFIG SOURCES ${SRCS_IFCONFIG}
                CONDITIONAL CSR_IP_SUPPORT_IFCONFIG HEADERS ${HDRS_IFCONFIG}
                CONDITIONAL CSR_IP_SUPPORT_IFCONFIG PUBLIC_HEADERS ${PUBLIC_HDRS_IFCONFIG}
                CONDITIONAL OPENSSL_FOUND AND CSR_IP_SUPPORT_TLS SOURCES ${SRCS_TLS}
                CONDITIONAL OPENSSL_FOUND AND CSR_IP_SUPPORT_TLS HEADERS ${HDRS_TLS}
                CONDITIONAL OPENSSL_FOUND AND CSR_IP_SUPPORT_TLS PUBLIC_HEADERS ${PUBLIC_HDRS_TLS}
                ALLOW_NON_ANSI)

csr_add_library(csr_ip_ether_lib
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS_ETHER_LIB})

csr_add_library(csr_ip_ifconfig_lib
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS_IFCONFIG_LIB})

csr_add_library(csr_ip_socket_lib
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                SOURCES ${SRCS_SOCKET_LIB})
