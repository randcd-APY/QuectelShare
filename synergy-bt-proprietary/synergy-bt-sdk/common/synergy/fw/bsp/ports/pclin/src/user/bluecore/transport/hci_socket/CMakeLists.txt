##############################################################################
#
# opyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    csr_hci_socket_transport.c)

include_directories("${MDM_ROOT}/external/bluetooth/bluez/lib/")

set(HDRS)


set(PUBLIC_HDRS
    ../../../../../inc/platform/csr_hci_socket_init.h)

include_directories(../../../../../../../../gsp/src/bluecore/inc/tm)

csr_add_library(csr_hci_socket
                COMPONENT CSR_COMPONENT_FRW_BSP_CORE
                CONDITIONAL CSR_HCI_SOCKET_TRANSPORT SOURCES ${SRCS}
                CONDITIONAL CSR_HCI_SOCKET_TRANSPORT HEADERS ${HDRS}
                CONDITIONAL CSR_HCI_SOCKET_TRANSPORT PUBLIC_HEADERS ${PUBLIC_HDRS}
                ALLOW_NON_ANSI)
