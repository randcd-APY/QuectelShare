##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(CSR_ARM_COMPILER_VERSION "2.2")

if (DEFINED "ENV{RVCT22BIN}")
    set(CSR_ARM_PATH_BIN "$ENV{RVCT22BIN}")
    set(CSR_ARM_PATH_INC "$ENV{RVCT22INC}")
    set(CSR_ARM_PATH_LIB "$ENV{RVCT22LIB}")
else()
    message (FATAL_ERROR "RealView ARM Compiler v${CSR_ARM_COMPILER_VERSION} environment variables not found")
endif()

set(CMAKE_SYSTEM_NAME    Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_FIND_ROOT_PATH              ${CSR_ARM_PATH_BIN})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

find_program(CMAKE_C_COMPILER   NAMES armcc PATHS ${CSR_ARM_PATH_BIN} NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES armcc PATHS ${CSR_ARM_PATH_BIN} NO_DEFAULT_PATH)
find_program(CMAKE_ASM_COMPILER NAMES armasm PATHS ${CSR_ARM_PATH_BIN} NO_DEFAULT_PATH)
find_program(CMAKE_AR           NAMES armar PATHS ${CSR_ARM_PATH_BIN} NO_DEFAULT_PATH)

include(CMakeForceCompiler)

CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" RVCT)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" RVCT)

set(CSR_LINKING_WORKS FALSE)

set(CMAKE_C_CREATE_STATIC_LIBRARY   "<CMAKE_AR> -r <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY})
set(CMAKE_ASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> -r <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_ASM_COMPILE_OBJECT        "<CMAKE_ASM_COMPILER> <FLAGS> -o <OBJECT> <SOURCE>")

set(CSR_PLATFORM_DEFAULT "BDB3")
