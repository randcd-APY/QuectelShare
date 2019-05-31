##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

cmake_minimum_required(VERSION 2.8.4)
if (CMAKE_MAJOR_VERSION EQUAL 3)
    cmake_policy(SET CMP0026 OLD)
    cmake_policy(SET CMP0045 OLD)
	if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
        set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
    endif()
endif()

if (NOT DEFINED CSR_BUILD_ROOT)
    message(STATUS "CMake Version: ${CMAKE_VERSION}")

    set(_csr_build_root_located)
    if (NOT EXISTS "${CMAKE_HOME_DIRECTORY}/tools/codecheck")
      set(CSR_BUILD_INTERNAL FALSE)
    else()
      set(CSR_BUILD_INTERNAL TRUE)
    endif()

    # Define roots
    set(CSR_BUILD_ROOT "${CMAKE_HOME_DIRECTORY}/build")
    set(CSR_CONFIG_ROOT "${CMAKE_HOME_DIRECTORY}/build/config")
    set(CSR_TOOLS_ROOT "${CMAKE_HOME_DIRECTORY}/tools")
    set(CSR_FRW_ROOT "${CMAKE_HOME_DIRECTORY}")

    # Add build root as a module location (contains assembler setup)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CSR_BUILD_ROOT}")

    # Set user make rules override (loaded after toolchain before setting cache variables)
    set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CSR_BUILD_ROOT}/build_system_override.cmake")

    # Set default build type
    if(CSR_LOG_ENABLE)
        set(CMAKE_BUILD_TYPE_INIT "Debug")
    else()
        set(CMAKE_BUILD_TYPE_INIT "Release")
    endif()
    
    # Print some information about the selected toolchain file if any
    if (CMAKE_TOOLCHAIN_FILE)
        message(STATUS "Toolchain File: ${CMAKE_TOOLCHAIN_FILE}")
    endif()

    # Set a default csr_executable_poststep in case the toolchain/platform file doesn't
    macro(csr_executable_poststep name dest)
    endmacro()

    # Synergy Project
    project(Synergy)

    # Print some information about the selected platform
    if (CMAKE_CROSSCOMPILING)
        message(STATUS "Platform selected: ${CSR_PLATFORM} (cross compiling)")
    else()
        message(STATUS "Platform selected: ${CSR_PLATFORM} (host compiling)")
    endif()

    # Enable assembler if the toolchain/platform file has set CMAKE_ASM_COMPILER_ID
    if (CMAKE_ASM_COMPILER_ID)
        enable_language(ASM)
    endif()

    # Include platform specific library loading file
    include("${CSR_BUILD_ROOT}/platformlibs-${CSR_PLATFORM}.cmake" OPTIONAL)

    # Load the build system
    include("${CSR_BUILD_ROOT}/build_system.cmake")
else()
    # Include the local configuration
    set(CSR_FRW_CONFIG_ROOT ${CSR_FRW_ROOT}/config)
    include(${CSR_FRW_CONFIG_ROOT}/csr_frw_config.cmake)

    include(ver.cmake)

    # Backwards compatibility
    set (BUILD_SCRIPTS
      global_rules.mk.dist.in
      paths.mk.in
      m_targets.mk)

    foreach (_file ${BUILD_SCRIPTS})
      csr_configure_install(${_file} CSR_COMPONENT_FRW)
    endforeach()

    csr_install_files(COMPONENT CSR_COMPONENT_FRW document_index.html)
    csr_install_files(COMPONENT CSR_COMPONENT_FRW ${CMAKE_BINARY_DIR}/frw/Third_Party_Notice_File.txt)
    
    # Private include paths
    set(CSR_FRW_PRIVATE_PRIM_INC
      ${CSR_FRW_ROOT}/gsp/src/inc
      ${CSR_FRW_ROOT}/gsp/src/log/inc
      ${CSR_FRW_ROOT}/gsp/src/bluecore/inc/bccmd
      ${CSR_FRW_ROOT}/gsp/src/bluecore/inc/hci
      ${CSR_FRW_ROOT}/gsp/src/bluecore/inc/tm
      ${CSR_FRW_ROOT}/gsp/src/application/ui)

    csr_add_subdirectory(build)
    csr_add_subdirectory(tools)
    csr_add_subdirectory(config)
    csr_add_subdirectory(devel)
    csr_add_subdirectory(bsp)
    csr_add_subdirectory(doc)
    csr_add_subdirectory(test/module_test)
    csr_add_subdirectory(gsp)
#   csr_add_subdirectory(example_apps)

    csr_add_subdirectory(tools/codesize)
    csr_add_subdirectory(tools/binsize)
    csr_add_subdirectory(tools/depmap)
    csr_add_subdirectory(tools/psr2c)
    csr_add_subdirectory(scripts)
    csr_add_subdirectory(html)

    # Build system install rules
    csr_install_buildsystem()
endif()
