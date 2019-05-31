###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################
cmake_minimum_required(VERSION 2.8.10)

set(CONNX_BUILD_ROOT "${CMAKE_HOME_DIRECTORY}/build")

# Load the build system
include("${CONNX_BUILD_ROOT}/build_system.cmake")

# Load the system libs
include("${CONNX_BUILD_ROOT}/platformlibs-LINUX.cmake")

include_directories(inc)

add_subdirectory(util)

add_subdirectory(bt_bootstrap)

if (NOT EXCLUDE_BT_TOOL)
include_directories(bt_bootstrap/inc)

add_subdirectory(tool)
endif()
