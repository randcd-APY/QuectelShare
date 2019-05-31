###############################################################################
#
# Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#                
###############################################################################

set (TARGET_SCRIPTS
    target-pclin-2.6-generic.mk
    target-pclin-2.6-x86_64.mk
    target-pclin-2.6-x86.mk
    target-pclin-android2.6-armv5.mk
    target-pclin-kernel2.6-x86.mk)

foreach (_file ${TARGET_SCRIPTS})
    csr_add_files(COMPONENT CSR_COMPONENT_FRW_BSP_CORE FILES ${_file})
endforeach()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/target-default.mk.dist")
    set(_install_src "${CMAKE_CURRENT_BINARY_DIR}/target-default.mk") 
    configure_file(target-default.mk.dist ${_install_src} COPYONLY)
    csr_install_files(COMPONENT CSR_COMPONENT_FRW_BSP_CORE TECHNOLOGY FRW ${_install_src})
else()
    csr_add_files(COMPONENT CSR_COMPONENT_FRW_BSP_CORE FILES target-default.mk)
endif()
