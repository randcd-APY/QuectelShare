###############################################################################
#
# Copyright (c) 2012-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP common)
csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP bootstrap)

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB common)
csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB bootstrap)

add_definitions("-DCSR_BT_RUN_STACK=1")
add_definitions("-DCSR_BT_RUN_APP=1")

if(CSR_PLATFORM MATCHES LINUX)
    add_definitions("-DCSR_BT_APP_USE_SERIAL_COM")
endif()

csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP inc)
csr_include_directories(COMPONENT CSR_COMPONENT_BT_APP_LIB inc)

csr_add_subdirectory(bootstrap PLATFORM LINUX)

csr_add_subdirectory(av PLATFORM LINUX)
csr_add_subdirectory(gatt PLATFORM LINUX)
csr_add_subdirectory(obex_pb/client PLATFORM LINUX)
csr_add_subdirectory(hfp PLATFORM LINUX)
if(NOT EXCLUDE_CSR_BT_BSL_MODULE)
csr_add_subdirectory(pan PLATFORM LINUX)
endif()
csr_add_subdirectory(spp PLATFORM LINUX)
csr_add_subdirectory(gap PLATFORM LINUX)
csr_add_subdirectory(common PLATFORM LINUX)
csr_add_subdirectory(common/ports)
csr_add_subdirectory(generic PLATFORM LINUX)
# csr_add_subdirectory(obex_pb PLATFORM LINUX)