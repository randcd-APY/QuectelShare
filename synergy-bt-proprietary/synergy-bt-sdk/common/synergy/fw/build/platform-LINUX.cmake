##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

# Additional compiler options
if(CSR_PLATFORM_ARM)
set(CSR_32BIT_BUILD_FLAG "")
else()
# [QCOM] Comment to work-around the build error in MDM_LE.
# set(CSR_32BIT_BUILD_FLAG "-m32")
endif()

if( NOT DEFINED CSR_BUILD_TYPE)
    set(CSR_BUILD_TYPE "release")
endif()

if(${CSR_BUILD_TYPE} STREQUAL "debug")
   set(CMAKE_BUILD_TYPE "Debug")
elseif(${CSR_BUILD_TYPE} STREQUAL "release")
   set(CMAKE_BUILD_TYPE "Release")
elseif(${CSR_BUILD_TYPE} STREQUAL "relwithdebinfo")
   set(CMAKE_BUILD_TYPE "RelWithDebInfo")
elseif(${CSR_BUILD_TYPE} STREQUAL "minsizerel")
   set(CMAKE_BUILD_TYPE "MinSizeRel")
else()
   set(CMAKE_BUILD_TYPE "None")
endif()

if(CSR_HYDRA_SSD)
set(CMAKE_C_FLAGS_INIT   "-W -Wall -Wno-unused-parameter -Wno-unused-function -Wdeclaration-after-statement -D_REENTRANT -fno-strict-aliasing ${CSR_32BIT_BUILD_FLAG}")
set(CMAKE_CXX_FLAGS_INIT "-W -Wall -Wno-unused-parameter -Wno-unused-function -Wdeclaration-after-statement -D_REENTRANT -fno-strict-aliasing ${CSR_32BIT_BUILD_FLAG}")
else()
set(CMAKE_C_FLAGS_INIT   "-W -Wall -Wno-unused-parameter  -Wdeclaration-after-statement -D_REENTRANT -fno-strict-aliasing ${CSR_32BIT_BUILD_FLAG}")
set(CMAKE_CXX_FLAGS_INIT "-W -Wall -Wno-unused-parameter  -Wdeclaration-after-statement -D_REENTRANT -fno-strict-aliasing ${CSR_32BIT_BUILD_FLAG}")
endif()
# Optional definitions of compiler switches for profiling, warnings as error and force ANSI.
set(CSR_BUILD_PROFILING_COMPILE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
set(CSR_BUILD_PROFILING_LIBS          "gcov")
set(CSR_BUILD_NOWARNINGS              "-Werror")
set(CSR_BUILD_FORCEANSI               "-ansi -pedantic")
set(CSR_BUILD_LINK_START_GROUP        "-Wl,--start-group")
set(CSR_BUILD_LINK_END_GROUP          "-Wl,--end-group")
set(CSR_FUNCATTR_NORETURN             "x __attribute__ ((noreturn))")
#Enable exception handling. Generates extra code needed to propagate exceptions. 
#For some targets, this implies GCC generates frame unwind information for all 
#functions, which can produce significant data size overhead, although it does 
#not affect execution.
if(CSR_LOG_ENABLE)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fexceptions -funwind-tables")
endif()
