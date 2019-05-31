##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

# Lock selected platform
if (NOT CSR_PLATFORM)
    if (NOT CSR_PLATFORM_DEFAULT)
        string(TOUPPER ${CMAKE_SYSTEM_NAME} CSR_PLATFORM_DEFAULT)
    endif()
    set(CSR_PLATFORM ${CSR_PLATFORM_DEFAULT} CACHE STRING "Platform - cannot be changed." FORCE)
endif()
if (CSR_PLATFORM_FIXED AND NOT CSR_PLATFORM_FIXED STREQUAL CSR_PLATFORM)
    message(FATAL_ERROR "CSR_PLATFORM=${CSR_PLATFORM_FIXED} cannot be changed once set")
endif()
set(CSR_PLATFORM_FIXED ${CSR_PLATFORM} CACHE UNINITIALIZED "" FORCE)

# Include platform definition file (if exists)
include("${CSR_BUILD_ROOT}/platform-${CSR_PLATFORM}.cmake" OPTIONAL)
