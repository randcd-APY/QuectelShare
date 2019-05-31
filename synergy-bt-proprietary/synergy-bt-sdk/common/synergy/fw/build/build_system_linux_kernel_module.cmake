##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

#
# This file provides a macro that makes it easy to add a Linux kernel module
# to the build system. See the inline signature of the function below. The
# supplied <name> will be the name of the target added to the build system and
# also the name of the kernel module (with a .ko extension added). In
# addition, when not cross compiling, there will be a build system target of
# the form <name>-install which will execute the modules_install target of
# kbuild using the appropriate parameters.
#
# All specified sources and header will be symlinked into the binary
# directory where the build will be performed. Therefore, if a required header
# is not specified, compilation will fail even if other headers in the same
# directory are specified. Include paths given to kbuild will be derived from
# the specified headers.
#
# The specified symver files will be passed to kbuild via the
# KBUILD_EXTRA_SYMBOLS. This is for providing the linker with symbol
# information for other modules that this module depend on.
#
# The following variables can be set from the command line, from the toolchain
# file or from the CMake file that uses the macro to affect the functionality:
#
# CSR_LINUX_KERNEL_DIR - The Linux kernel source (or headers). Defaults to
#                        /lib/modules/${CMAKE_SYSTEM_VERSION}/build when
#                        not cross compiling.
# CSR_LINUX_KERNEL_ARCH - The ARCH parameter to pass to kbuild. Only required
#                         when cross compiling.
# CSR_LINUX_KERNEL_CROSS_COMPILE - The CROSS_COMPILE parameter to pass to
#                                  kbuild. Only required when cross compiling.
# CSR_LINUX_KERNEL_C_FLAGS - Additional C flags to pass to kbuild which will
#                            be applied when compiling the specified sources.
#                            Defaults to empty.
#
# When cross compiling the default value for all of these are empty. As the
# CSR_LINUX_KERNEL_(DIR/ARCH/CROSS_COMPILE) are required, these must be
# supplied. Failure to do so will trigger a message and the kernel module
# will not be included in the build process.
#

macro(csr_add_linux_kernel_module name)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(_csr_macro_action)
        set(_csr_macro_sources)
        set(_csr_macro_headers)
        set(_csr_macro_symvers)
        set(_csr_macro_condition)
        set(_csr_macro_condition_true)
        set(_csr_macro_signature
                "csr_add_linux_kernel_module(<name>\n"
                "[[CONDITIONAL <condition>] SOURCES <source1> <source2> ...]\n"
                "[[CONDITIONAL <condition>] HEADERS <header1> <header2> ...]\n"
                "[[CONDITIONAL <condition>] SYMVERS <symver1> <symver2> ...])\n")

        foreach(_currentArg ${ARGN})
            if ("_${_currentArg}" STREQUAL "_SOURCES" OR
                "_${_currentArg}" STREQUAL "_HEADERS" OR
                "_${_currentArg}" STREQUAL "_SYMVERS")
                set(_csr_macro_action "${_currentArg}")
                set(_csr_macro_condition_true TRUE)
                if (_csr_macro_condition)
                    set(_csr_macro_condition_true ${_csr_macro_condition})
                    set(_csr_macro_condition)
                endif()
            elseif ("_${_currentArg}" STREQUAL "_CONDITIONAL")
                set(_csr_macro_action "${_currentArg}")
                set(_csr_macro_condition)
            elseif (${_csr_macro_action} STREQUAL "SOURCES")
                if (${_csr_macro_condition_true})
                    list(APPEND _csr_macro_sources "${_currentArg}")
                endif()
            elseif (${_csr_macro_action} STREQUAL "HEADERS")
                if (${_csr_macro_condition_true})
                    list(APPEND _csr_macro_headers "${_currentArg}")
                endif()
            elseif (${_csr_macro_action} STREQUAL "SYMVERS")
                if (${_csr_macro_condition_true})
                    list(APPEND _csr_macro_symvers "${_currentArg}")
                endif()
            elseif (${_csr_macro_action} STREQUAL "CONDITIONAL")
                list(APPEND _csr_macro_condition "${_currentArg}")
            else()
                message(FATAL_ERROR "Parameter '${_currentArg}' incorrect\n"
                                    ${_csr_macro_signature})
            endif()
        endforeach()

        set(_csr_macro_includes ${CMAKE_BINARY_DIR}/inc)

        # Create Kbuild
        set(_csr_macro_kbuild "${CMAKE_CURRENT_BINARY_DIR}/Kbuild")
        file(WRITE ${_csr_macro_kbuild} "obj-m := ${name}.o\n")

        # Process Sources
        file(APPEND ${_csr_macro_kbuild} "${name}-y := ")
        foreach(_csr_macro_source ${_csr_macro_sources})
            get_filename_component(_csr_macro_source ${_csr_macro_source} ABSOLUTE)
            file(RELATIVE_PATH _csr_macro_source_relative ${PROJECT_SOURCE_DIR} ${_csr_macro_source})
            string(REGEX REPLACE "^[\\./]+" "" _csr_macro_source_relative ${_csr_macro_source_relative})
            get_filename_component(_csr_macro_source_relative_path "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source_relative}" PATH)
            file(MAKE_DIRECTORY ${_csr_macro_source_relative_path})
            execute_process(COMMAND ${CMAKE_COMMAND}
                            -E create_symlink
                            "${_csr_macro_source}"
                            "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source_relative}")
            string(REGEX REPLACE "c$" "o" _csr_macro_object "${_csr_macro_source_relative}")
            file(APPEND ${_csr_macro_kbuild} "\\\n    ${_csr_macro_object} ")
        endforeach()
        file(APPEND ${_csr_macro_kbuild} "\n\n")

        # Process Headers
        foreach(_csr_macro_header ${_csr_macro_headers})
            get_filename_component(_csr_macro_header ${_csr_macro_header} ABSOLUTE)
            file(RELATIVE_PATH _csr_macro_header_relative ${PROJECT_SOURCE_DIR} ${_csr_macro_header})
            string(REGEX REPLACE "^[\\./]+" "" _csr_macro_header_relative ${_csr_macro_header_relative})
            get_filename_component(_csr_macro_header_relative_path "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_header_relative}" PATH)
            file(MAKE_DIRECTORY ${_csr_macro_header_relative_path})
            execute_process(COMMAND ${CMAKE_COMMAND}
                            -E create_symlink
                            "${_csr_macro_header}"
                            "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_header_relative}")
            list(APPEND _csr_macro_includes ${_csr_macro_header_relative_path})
        endforeach()
        list(REMOVE_DUPLICATES _csr_macro_includes)
        file(APPEND ${_csr_macro_kbuild} "EXTRA_CFLAGS += ")
        foreach(_csr_macro_include ${_csr_macro_includes})
            file(APPEND ${_csr_macro_kbuild} "\\\n    -I${_csr_macro_include} ")
        endforeach()
        file(APPEND ${_csr_macro_kbuild} "\n\n")

        # Process Symvers
        file(APPEND ${_csr_macro_kbuild} "KBUILD_EXTRA_SYMBOLS := ")
        foreach(_csr_macro_symver ${_csr_macro_symvers})
            file(APPEND ${_csr_macro_kbuild} "\\\n    ${_csr_macro_symver} ")
        endforeach()
        file(APPEND ${_csr_macro_kbuild} "\n\n")

        if (NOT CSR_LINUX_KERNEL_DIR AND NOT CMAKE_CROSSCOMPILING)
            set(CSR_LINUX_KERNEL_DIR /lib/modules/${CMAKE_SYSTEM_VERSION}/build)
            file(APPEND ${_csr_macro_kbuild} "\nINSTALLED_VERSION := $(shell uname -r)\n")
            file(APPEND ${_csr_macro_kbuild} "ifneq ($(INSTALLED_VERSION),${CMAKE_SYSTEM_VERSION})\n")
            file(APPEND ${_csr_macro_kbuild} "$(error Build system generated for Linux version ${CMAKE_SYSTEM_VERSION} different to installed version $(INSTALLED_VERSION))\n")
            file(APPEND ${_csr_macro_kbuild} "endif\n")
        endif()

        if (CSR_LINUX_KERNEL_DIR AND
            (CSR_LINUX_KERNEL_ARCH OR NOT CMAKE_CROSSCOMPILING) AND
            (CSR_LINUX_KERNEL_CROSS_COMPILE OR NOT CMAKE_CROSSCOMPILING))
            if (NOT CSR_ALLOW_WARNINGS)
                set(_csr_macro_extra_drv_cflags "${CSR_BUILD_NOWARNINGS}")
            endif()
            if (CSR_LINUX_KERNEL_C_FLAGS)
                set(_csr_macro_extra_drv_cflags "${_csr_macro_extra_drv_cflags} ${CSR_LINUX_KERNEL_C_FLAGS}")
            endif()

            # Default is not to build (for instance during packaging as building everything takes time)
            set(_csr_macro_all)
            if (CSR_PERFORM_BUILD)
                set(_csr_macro_all ALL)
            endif()

            set(_csr_macro_kbuild_cmd ${CMAKE_MAKE_PROGRAM}
                                      -C ${CSR_LINUX_KERNEL_DIR}
                                      M=${CMAKE_CURRENT_BINARY_DIR}
                                      EXTRA_DRV_CFLAGS=${_csr_macro_extra_drv_cflags}
                                      V=1)

            if (CSR_LINUX_KERNEL_ARCH)
                list(APPEND _csr_macro_kbuild_cmd "ARCH=${CSR_LINUX_KERNEL_ARCH}")
            endif()

            if (CSR_LINUX_KERNEL_CROSS_COMPILE)
                list(APPEND _csr_macro_kbuild_cmd "CROSS_COMPILE=${CSR_LINUX_KERNEL_CROSS_COMPILE}")
            endif()

            add_custom_target(${name}
                              ${_csr_macro_all}
                              COMMAND ${_csr_macro_kbuild_cmd} modules
                              COMMENT "Building Linux kernel module ${name}"
                              VERBATIM
                              SOURCES ${_csr_macro_sources} ${_csr_macro_headers} ${_csr_macro_kbuild})
            if (NOT CMAKE_CROSSCOMPILING)
                add_custom_target(${name}-install
                                  COMMAND ${_csr_macro_kbuild_cmd} modules_install)
                add_dependencies(${name}-install ${name})
                add_custom_target(${name}-install_sudo
                                  COMMAND sudo ${_csr_macro_kbuild_cmd} modules_install)
                add_dependencies(${name}-install_sudo ${name})
            endif()
        else()
            message(STATUS "Skipping ${name} - make sure CSR_LINUX_KERNEL_(DIR/ARCH/CROSS_COMPILE) are set to enable building of Linux kernel modules.")
        endif()
    endif()
endmacro(csr_add_linux_kernel_module)
