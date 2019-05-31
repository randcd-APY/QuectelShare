##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

#
# This is an example of a toolchain file for cross compiling for a generic
# Linux target platform. For validation and demonstration purposes, the actual
# values configured result in a cross compilation for the host platform, which
# is technically not a cross compile. The values should be changed to fit the
# desired target platform, as described in the individual comments below.
#

#
# Setting CMAKE_SYSTEM_NAME causes CMake to enter cross compilation mode
# (CMAKE_CROSSCOMPILE will be set automatically). The name "Linux" is
# recognized by CMake internally and configures CMake for targetting a generic
# Linux platform.
#
set(CMAKE_SYSTEM_NAME Linux)

#
# Specify the desired C and C++ cross compilers. The second parameter is
# the compiler identification. Refer to the CMake documentation for possible
# values (GNU is applicable when using gcc in any form).
#
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("arm-none-linux-gnueabi-gcc" GNU)
CMAKE_FORCE_CXX_COMPILER("arm-none-linux-gnueabi-g++" GNU)

#
# Set the path to search for libraries/headers, and instruct CMake to never
# search for programs and only search for libraries/headers in the given
# location. See platformlibs-LINUX.cmake which indirectly uses these paths
# to search for libraries/headers.
#
set(CMAKE_FIND_ROOT_PATH              "/usr")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#
# Optional options for enabling compiling of kernel modules. Adjust to the
# desired values for the Linux kernel of the target platform. See also the
# comments in build_system_linux_kernel_module.cmake for a more detailed
# description of these parameters.
#
set(CSR_LINUX_KERNEL_DIR "/lib/modules/${CMAKE_HOST_SYSTEM_VERSION}/build")
set(CSR_LINUX_KERNEL_ARCH "i386")
set(CSR_LINUX_KERNEL_CROSS_COMPILE "/usr/bin/")
set(CSR_LINUX_KERNEL_C_FLAGS)

#
# Reuse the LINUX platform. This causes CSR_PLATFORM to be set to "LINUX"
# and all existing code that is tagged as applicable for that platform will
# be compiled. This also causes the platform-LINUX.cmake and
# platformlibs-LINUX.cmake files to be included which contains additional
# configuration applicable to this platform.
#
set(CSR_PLATFORM_DEFAULT "LINUX")
