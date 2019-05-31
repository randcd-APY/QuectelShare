##############################################################################
#
# Copyright (c) 2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(CMAKE_SYSTEM_NAME Linux)
set(CSR_CROSS_COMPILER "arm-linux-gnueabihf-")
#set(CMAKE_C_COMPILER "${CSR_CROSS_COMPILER}gcc")
set(CMAKE_C_COMPILER ${CC})
#set(CMAKE_CXX_COMPILER "${CSR_CROSS_COMPILER}g++")
set(CMAKE_CXX_COMPILER ${CPP})
set(CSR_KERNEL_DIR "")
set(CSR_KERNEL_ARCH "arm")
set(CMAKE_FIND_ROOT_PATH "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM           NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY           ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE           ONLY)



