##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

csr_include_directories(COMPONENT CSR_COMPONENT_FRW_BSP_CORE ${CSR_FRW_ROOT}/bsp/ports/bdb3/inc PLATFORM BDB3)
csr_include_directories(COMPONENT CSR_COMPONENT_FRW_BSP_CORE ${CSR_FRW_ROOT}/bsp/ports/pcwin/inc PLATFORM WINDOWS)
csr_include_directories(COMPONENT CSR_COMPONENT_FRW_BSP_CORE ${CSR_FRW_ROOT}/bsp/ports/pclin/inc PLATFORM LINUX ANDROID)
csr_include_directories(COMPONENT CSR_COMPONENT_FRW ${CSR_FRW_ROOT}/gsp/inc)
csr_include_directories(COMPONENT CSR_COMPONENT_FRW ${CSR_FRW_ROOT}/bsp/inc)
csr_include_directories(COMPONENT CSR_COMPONENT_TOOLS_CASTE_COMMON ${CSR_FRW_ROOT}/tools/caste/inc)

