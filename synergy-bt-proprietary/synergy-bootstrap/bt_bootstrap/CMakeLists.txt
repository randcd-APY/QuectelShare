###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

# Define the macro to enable BT snoop log in bt bootstrap
if (ENABLE_BTSNOOP_LOG)
add_definitions("-DENABLE_BTSNOOP_LOG")
endif()

# Define the macro to use QTIL MDM platform
if (USE_MDM_PLATFORM)
add_definitions("-DUSE_MDM_PLATFORM")
endif()

# Define the macro to use QTIL MSM platform
if (USE_MSM_PLATFORM)
add_definitions("-DUSE_MSM_PLATFORM")
endif()

# Define the macro to use IMX platform
if (USE_IMX_PLATFORM)
add_definitions("-DUSE_IMX_PLATFORM")
endif()

include_directories(inc)

if (NOT EXCLUDE_BOOT_QCA)
add_subdirectory(qca)
endif()

if (NOT EXCLUDE_BOOT_BLUECORE)
add_subdirectory(bluecore)
endif()

add_subdirectory(util)

add_subdirectory(main)
