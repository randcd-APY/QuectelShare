##############################################################################
#
# Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_FRW_BSP_CORE inc)
csr_add_subdirectory(src/kernel)
csr_add_subdirectory(src/user)
csr_add_subdirectory(inc)
csr_add_subdirectory(scripts)

csr_configure_install(paths.mk.in CSR_COMPONENT_FRW_BSP_CORE)
