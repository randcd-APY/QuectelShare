##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

# Ensure newer CL compiler doesn't complain about using 'old' C standard library functionality
add_definitions("-D_CRT_SECURE_NO_WARNINGS")
add_definitions("-D_CRT_SECURE_NO_DEPRECATE")

if( NOT DEFINED CSR_BUILD_TYPE)
    set(CSR_BUILD_TYPE "debug")
endif()

if(${CSR_BUILD_TYPE} STREQUAL "debug")
   set(CMAKE_BUILD_TYPE "debug")
elseif(${CSR_BUILD_TYPE} STREQUAL "release")
   set(CMAKE_BUILD_TYPE "Release")
elseif(${CSR_BUILD_TYPE} STREQUAL "relwithdebinfo")
   set(CMAKE_BUILD_TYPE "relwithdebinfo")
elseif(${CSR_BUILD_TYPE} STREQUAL "minsizerel")
   set(CMAKE_BUILD_TYPE "minsizerel")
else()
   set(CMAKE_BUILD_TYPE "none")
endif()

# Use static libraries
foreach (_lang C CXX)
    foreach (_config DEBUG MINSIZEREL RELEASE RELWITHDEBINFO)
        if ( NOT ("${CMAKE_${_lang}_FLAGS_${_config}_INIT}" STREQUAL "") )
            string (REPLACE "/MD" "/MT" CMAKE_${_lang}_FLAGS_${_config}_INIT ${CMAKE_${_lang}_FLAGS_${_config}_INIT})
        endif()
    endforeach()
endforeach()

# Optional definitions of compiler switches for profilling, warnings as error and force ANSI.
set(CSR_BUILD_PROFILING_COMPILE_FLAGS)
set(CSR_BUILD_PROFILING_LIBS)
set(CSR_BUILD_NOWARNINGS               "-WX")
set(CSR_BUILD_FORCEANSI)
set(CSR_BUILD_LINK_START_GROUP)
set(CSR_BUILD_LINK_END_GROUP)
set(CSR_FUNCATTR_NORETURN              "_declspec(noreturn) x")
