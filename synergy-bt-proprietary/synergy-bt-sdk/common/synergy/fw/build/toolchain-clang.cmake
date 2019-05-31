##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

#
# This is a simple toolchain file that makes it easy to switch the C/C++
# compiler to Clang when compiling for the host. the clang and clang++
# binaries must be present in the system search path.
#
# Note that using this toolchain file is equivalent to setting these variables
# from the command line or manually setting CC and CXX to clang and clang++
# respectively before invoking CMake.
#

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
