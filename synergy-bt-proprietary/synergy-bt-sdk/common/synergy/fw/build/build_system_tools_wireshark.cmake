##############################################################################
#
# Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# Java
find_package(Java COMPONENTS Development)
if (NOT JAVA_FOUND)
    message(STATUS "Java SDK not found. Java output is Disabled")
endif()

# Wireshark
if (NOT CMAKE_CROSSCOMPILING)
    string(REGEX MATCH "Win64" CSR_COMPILER_GEN_ARCH_WIN64 ${CMAKE_GENERATOR})
    string(REGEX MATCH "IA64" CSR_COMPILER_GEN_ARCH_IA64 ${CMAKE_GENERATOR})
    #If generator specified is Win64 or IA64, Wireshark dissector generation is not supported.
    if (NOT ( CSR_COMPILER_GEN_ARCH_WIN64 MATCHES Win64 OR CSR_COMPILER_GEN_ARCH_IA64 MATCHES IA64 ))
        set (WIRESHARK_PATH "$ENV{ProgramFiles}/Wireshark" "C:/Program Files/Wireshark")
        find_program(WIRESHARK_EXECUTABLE
                     NAMES wireshark
                     PATHS ${WIRESHARK_PATH})
        find_package_handle_standard_args("Wireshark" DEFAULT_MSG WIRESHARK_EXECUTABLE)
        mark_as_advanced(WIRESHARK_EXECUTABLE)
        find_program(WIRESHARK_DUMPCAP_EXECUTABLE
                     NAMES dumpcap
                     PATHS ${WIRESHARK_PATH})
        find_package_handle_standard_args("Wireshark - dumpcap" DEFAULT_MSG WIRESHARK_DUMPCAP_EXECUTABLE)
        mark_as_advanced(WIRESHARK_DUMPCAP_EXECUTABLE)

        if (WIRESHARK_DUMPCAP_EXECUTABLE)
            execute_process(COMMAND "${WIRESHARK_DUMPCAP_EXECUTABLE}" "-v" OUTPUT_VARIABLE WIRESHARK_VERSION_INFO)
            string (REGEX MATCH "^Dumpcap [0-9]+.[0-9]+.[0-9]+" WIRESHARK_VERSION ${WIRESHARK_VERSION_INFO})
            string (REGEX MATCH "[0-9]+.[0-9]+.[0-9]+$" WIRESHARK_VERSION ${WIRESHARK_VERSION})
            set (WIRESHARK_PLUGINS_PATHS "$ENV{ProgramFiles}/wireshark/plugins/${WIRESHARK_VERSION}" "C:/Program Files/Wireshark/plugins/${WIRESHARK_VERSION}")
            string (REGEX REPLACE "[.]+" "_" WIRESHARK_VERSION_TEXT ${WIRESHARK_VERSION})
        endif()
		
		message("this is for cmake CSR_TOOLS_ROOT " ${CSR_TOOLS_ROOT})
        if (CMAKE_HOST_SYSTEM MATCHES Windows)
            set (WIRESHARK_DISSECTOR_INC_PATH "${CSR_TOOLS_ROOT}/wireshark/wireshark-1.2.0")
            set (WIRESHARK_DISSECTOR_LIB_PATH "${CSR_TOOLS_ROOT}/wireshark/wireshark-1.2.0/epan")
            set (GLIB2_INCLUDE_DIRS "${CSR_TOOLS_ROOT}/wireshark/wireshark-win32-libs/glib/include/glib-2.0"
                                    "${CSR_TOOLS_ROOT}/wireshark/wireshark-win32-libs/glib/include/glib-2.0/glib"
                                    "${CSR_TOOLS_ROOT}/wireshark/wireshark-win32-libs/glib/lib/glib-2.0/include")
            set (GLIB2_LIBRARY_DIRS "${CSR_TOOLS_ROOT}/wireshark/wireshark-win32-libs/glib/lib")
            find_library (GLIB2_LIBRARIES NAMES "glib-2.0.lib" PATHS ${GLIB2_LIBRARY_DIRS})
        else()
            set (WIRESHARK_PLUGINS_PATHS "/usr/lib/wireshark/plugins" "/usr/lib/wireshark/libwireshark0/plugins" "/usr/lib64/wireshark/plugins/${WIRESHARK_VERSION}" "/usr/lib/wireshark/plugins/${WIRESHARK_VERSION}")
            set (WIRESHARK_DISSECTOR_INC_PATH "/usr/include/wireshark")
            set (WIRESHARK_DISSECTOR_LIB_PATH "/usr/lib/wireshark")

            pkg_check_modules(GLIB2 glib-2.0)
        endif()
        message("this is for cmake kfsldkfjsdlk jsdlkjfkl" ${WIRESHARK_PLUGINS_PATHS})
        find_path (WIRESHARK_PLUGINS_LOCATION NAMES "asn1.so" "asn1.dll" PATHS ${WIRESHARK_PLUGINS_PATHS})
        find_path (WIRESHARK_DISSECTOR_INCLUDES NAMES "epan/epan.h" PATHS ${WIRESHARK_DISSECTOR_INC_PATH})
        find_library (WIRESHARK_DISSECTOR_LIBRARIES NAMES "libwireshark.so" "libwireshark.lib" PATHS ${WIRESHARK_DISSECTOR_LIB_PATH})

        find_package_handle_standard_args("Wireshark - plugins" DEFAULT_MSG WIRESHARK_PLUGINS_LOCATION)
        mark_as_advanced(WIRESHARK_PLUGINS_LOCATION)
        mark_as_advanced(WIRESHARK_DISSECTOR_INCLUDES)
        mark_as_advanced(WIRESHARK_DISSECTOR_LIBRARIES)

        if (WIRESHARK_PLUGINS_LOCATION AND WIRESHARK_DISSECTOR_INCLUDES AND WIRESHARK_DISSECTOR_LIBRARIES)
            set (WIRESHARK_AVAILABLE TRUE)
        endif()
    else()
        message("-- Wireshark dissector generation is not supported for Win64 and IA64 configutions")
    endif()
endif()
