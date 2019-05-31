##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

if(CSR_BACKWARDS_COMPATIBILITY_ENABLE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_BINARY_DIR}/lib)
endif()

# Prepend include dirs (backwards compatibility)
set (CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)

# Include global macros
include("${CSR_BUILD_ROOT}/build_system_macros.cmake")
include("${CSR_BUILD_ROOT}/build_system_quality.cmake")
include("${CSR_BUILD_ROOT}/build_system_linux_kernel_module.cmake")

# Create the build system debug file if required
csr_debug_create()

# Export all defined components as installable
set(CSR_PRODUCT_NAME_PREFIX "all")

# Set CSR install root
if (CMAKE_HOST_WIN32)
    set(CSR_INSTALL_ROOT "c:/csr")
elseif (CMAKE_HOST_UNIX)
    set(CSR_INSTALL_ROOT "$ENV{HOME}/csr")
else()
    message(WARNING "CSR install root unspecified for host system")
endif()

# Include technology definitions
include("${CSR_CONFIG_ROOT}/technologies.cmake")
foreach(_technology ${CSR_TECHNOLOGIES})
    if (CSR_${_technology}_BUILDSYSTEM_AVAILABLE)
        include(${CSR_${_technology}_ROOT}/ver.cmake)
    endif()
    if (CSR_${_technology}_RELEASE_TYPE_ENG)
        set(CSR_${_technology}_VERSION_FIXANDBUILD "${CSR_${_technology}_VERSION_FIXLEVEL}.${CSR_${_technology}_VERSION_BUILD}")
    else()
        set(CSR_${_technology}_VERSION_FIXANDBUILD "${CSR_${_technology}_VERSION_FIXLEVEL}")
    endif()
endforeach()

# Load the configuration file for each technology
csr_load_cache_config(${CSR_CONFIG_ROOT} CSR_COMMON_CONFIG "Common config file")
foreach (_technology ${CSR_TECHNOLOGIES})
    message(STATUS "${_technology}:")
    message(STATUS "  Path: ${CSR_${_technology}_ROOT}")
    message(STATUS "  Ver: ${CSR_${_technology}_VERSION_MAJOR}.${CSR_${_technology}_VERSION_MINOR}.${CSR_${_technology}_VERSION_FIXANDBUILD}")
    if (CSR_${_technology}_BUILDSYSTEM_AVAILABLE)
        csr_load_cache_config(${CSR_${_technology}_ROOT}/config CSR_${_technology}_CONFIG "${_technology} config file")
        message(STATUS "  Config: ${CSR_${_technology}_CONFIG}")
    endif()
endforeach ()

# Some toolchain can't link ARM ADS+v2.2
if (NOT DEFINED CSR_LINKING_WORKS)
    set (CSR_LINKING_WORKS TRUE)
endif()

# Define the global include directory
set(CSR_INC_GLOBAL ${PROJECT_BINARY_DIR}/inc)
include_directories(${CSR_INC_GLOBAL})

# Define the global dump directory
if (CSR_DUMP_FILES_INTO_SINGLE_DIR)
    set(CSR_SRC_GLOBAL ${PROJECT_BINARY_DIR}/src)
    file(REMOVE_RECURSE ${CSR_SRC_GLOBAL})
    file(MAKE_DIRECTORY ${CSR_SRC_GLOBAL})
endif()

if (${CSR_BUILD_SYSTEM_DEBUG})
    # Create install debug file
    set (CSR_BUILD_DEBUG_INSTALL_FILE "${PROJECT_BINARY_DIR}/install_rules.txt")
    csr_create_cmake_file(${CSR_BUILD_DEBUG_INSTALL_FILE})
endif()

# Include the Component description file. This will add the required entries to the csr_components.h file
csr_debug_output("Available components:\n")
csr_debug_output("----------------------------------------------------\n")

foreach (_csr_build_technology ${CSR_TECHNOLOGIES})
    set (CSR_CURRENT_TECHNOLOGY "${_csr_build_technology}")
    if (CSR_${_csr_build_technology}_BUILDSYSTEM_AVAILABLE)
        file (GLOB _files "${CSR_${_csr_build_technology}_ROOT}/components*.cmake")
        foreach (_file ${_files})
            file (RELATIVE_PATH _relative_file ${PROJECT_SOURCE_DIR} ${_file})
            include (${_file})
        endforeach()
        file (GLOB _files "${CSR_${_csr_build_technology}_ROOT}/build/mapping_library_*.cmake")
        foreach (_file ${_files})
            file (RELATIVE_PATH _relative_file ${PROJECT_SOURCE_DIR} ${_file})
            include (${_file})
        endforeach()
        file (GLOB _files "${CSR_${_csr_build_technology}_ROOT}/lib/csr_exports_[_A-Za-z0-9]+.cmake$")
        foreach (_file ${_files})
            file (RELATIVE_PATH _relative_file ${PROJECT_SOURCE_DIR} ${_file})
            include (${_file})
        endforeach()
    endif()
endforeach ()

csr_debug_output("----------------------------------------------------\n\n")

# Validate dependencies between components
csr_validate_components()
if (${CSR_BUILD_SYSTEM_DEBUG})
    message (STATUS "Defined components:")
    foreach (_component ${CSR_COMPONENTS})
        message (STATUS "  ${_component}")
    endforeach()
endif()

# Include the global configuration
if (NOT EXISTS ${CSR_CONFIG_ROOT}/csr_synergy.cmake)
    message(FATAL_ERROR "Selected configuration invalid\nFile: ${CSR_CONFIG_ROOT}/csr_synergy.cmake does not exists")
endif ()
include(${CSR_CONFIG_ROOT}/csr_synergy.cmake)

if(DEFINED CSR_PRODUCT_FILE)
    # Include the product file
    set(CSR_PRODUCT_FILE "${CSR_PRODUCT_FILE}" CACHE FILEPATH "Product definition file")
    get_filename_component(_abs_file ${CSR_PRODUCT_FILE} ABSOLUTE)
    file (RELATIVE_PATH _relative_file ${PROJECT_SOURCE_DIR} ${_abs_file})
    message (STATUS "Product definition file: ${_relative_file}")
    include (${CSR_PRODUCT_FILE})
    if (CSR_PRODUCT_RELEASE_TYPE_ENG)
        set(CSR_PRODUCT_VERSION_FIXANDBUILD "${CSR_PRODUCT_VERSION_FIXLEVEL}.${CSR_PRODUCT_VERSION_BUILD}")
    else()
        set(CSR_PRODUCT_VERSION_FIXANDBUILD "${CSR_PRODUCT_VERSION_FIXLEVEL}")
    endif()
    set (CSR_PRODUCT_NAME ${CSR_PRODUCT_NAME_PREFIX})
    if (CSR_PRODUCT_NAME_SUFFIX)
        set (CSR_PRODUCT_NAME "${CSR_PRODUCT_NAME}_${CSR_PRODUCT_NAME_SUFFIX}")
    endif()

endif()

install(CODE "if (\"${CSR_PRODUCT_FILE}\" STREQUAL \"\")\n    message (FATAL_ERROR \"CSR_PRODUCT_FILE not specified\")\n  endif()")

if (${CSR_BUILD_SYSTEM_DEBUG})
    message (STATUS "Selected components:")
    foreach (_component ${CSR_COMPONENTS_SELECTED})
        message (STATUS "  ${_component}")
    endforeach()
endif()

if(DEFINED CSR_PRODUCT_FILE)
    # Generate the new components file according to:
    # 1) what is available (e.g. */components.cmake)
    # 2) what is wanted (e.g. product_bar.cmake)
    foreach (_csr_build_technology ${CSR_TECHNOLOGIES})
        set (_component_file)
        foreach (_csr_config_component ${CSR_COMPONENTS_SELECTED})
            if (${${_csr_config_component}_TECHNOLOGY} STREQUAL ${_csr_build_technology})
                if (NOT _component_file)
                    string (TOLOWER "${_csr_build_technology}" _csr_build_technology_lc)
                    set (_component_file "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/components_${CSR_PRODUCT_NAME}.cmake")
                    csr_create_cmake_file(${_component_file})
                endif()
                file (APPEND ${_component_file} "csr_define_component(${_csr_config_component}")
                if (${_csr_config_component}_DEPENDS)
                    file (APPEND ${_component_file} " DEPENDS ${${_csr_config_component}_DEPENDS}")
                endif()
                if (${_csr_config_component}_PROTECTED)
                    file (APPEND ${_component_file} " PROTECTED")
                endif()
                if (${_csr_config_component}_BINARY_ONLY)
                    file (APPEND ${_component_file} " BINARY")
                endif()
                file (APPEND ${_component_file} ")\n")
            endif()
        endforeach()
        unset (_component_file)
    endforeach()
endif()

# Create the csr_components.h file with the components available for this platform
set (CSR_COMPONENT_HEADER_FILE "${CSR_INC_GLOBAL}/csr_components.h")
csr_create_file("${CSR_COMPONENT_HEADER_FILE}.tmp")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "#ifndef CSR_COMPONENTS_H__\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "#define CSR_COMPONENTS_H__\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "/*****************************************************************************\n\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "        Copyright Cambridge Silicon Radio Limited 2011-2017\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "\n\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "        Refer to LICENSE.txt included with this source for details\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "        on the license terms.\n\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "*****************************************************************************/\n\n")
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "/* Note: this is an auto-generated file. */\n\n")
foreach (_csr_config_namespaces ${CSR_COMPONENTS})
    file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "#define ${_csr_config_namespaces} (1)\n")
endforeach()
file (APPEND "${CSR_COMPONENT_HEADER_FILE}.tmp" "\n#endif\n")
configure_file ("${CSR_COMPONENT_HEADER_FILE}.tmp" ${CSR_COMPONENT_HEADER_FILE} COPYONLY)

foreach (_csr_build_technology ${CSR_TECHNOLOGIES})
    set (CSR_CURRENT_TECHNOLOGY "${_csr_build_technology}")
    string (TOLOWER ${_csr_build_technology} _csr_build_technology_lc)
    # Include the group definitions
    include (${CSR_${_csr_build_technology}_ROOT}/groups.cmake OPTIONAL)
    # Install precompiled Wireshark dissectors
    include (${CSR_${_csr_build_technology}_ROOT}/plugin/dissectors_install.cmake OPTIONAL)
    # Handle the library mapping
    set (CSR_${_csr_build_technology}_MAPPING_LIBRARY_FILE "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/mapping_library_${CSR_PRODUCT_NAME}.cmake")
    csr_create_cmake_file(${CSR_${_csr_build_technology}_MAPPING_LIBRARY_FILE})
    # Create the buildsystem install file
    set (CSR_${_csr_build_technology}_INSTALL_FILE "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/build/install_buildsystem.cmake")
    csr_create_cmake_file(${CSR_${_csr_build_technology}_INSTALL_FILE})
    # Handle artifacts mapping
    set_property(GLOBAL PROPERTY CSR_${CSR_CURRENT_TECHNOLOGY}_ARTIFACT_MAPPING "")
    # Create the temporary dissector file
    set (CSR_${_csr_build_technology}_DISSECTOR_FILE "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/build/dissectors.cmake")
    csr_create_cmake_file(${CSR_${_csr_build_technology}_DISSECTOR_FILE})
    file (APPEND ${CSR_${_csr_build_technology}_DISSECTOR_FILE} "set (CSR_${_csr_build_technology}_DISSECTOR_SHARED_LIBRARIES)\n")
    file (APPEND ${CSR_${_csr_build_technology}_DISSECTOR_FILE} "set (CSR_${_csr_build_technology}_DISSECTOR_TARGETS)\n")
    file (APPEND ${CSR_${_csr_build_technology}_DISSECTOR_FILE} "\n")
    # Create document install file
    set (CSR_${_csr_build_technology}_INSTALLER_SCRIPT_FILE "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/build/install_documents.cmake")
    csr_create_cmake_file(${CSR_${_csr_build_technology}_INSTALLER_SCRIPT_FILE})
    install (SCRIPT ${CSR_${_csr_build_technology}_INSTALLER_SCRIPT_FILE} COMPONENT BUILD)
endforeach ()

# If the toolchain does not provide a noreturn-macro
if (NOT CSR_FUNCATTR_NORETURN)
    set (CSR_FUNCATTR_NORETURN "x")
endif()

configure_file("${CSR_CONFIG_ROOT}/csr_synergy.h.in" "${CSR_INC_GLOBAL}/csr_synergy.h")

# Configure the default values of the groups
get_property(_csr_config_groups GLOBAL PROPERTY CSR_GROUPS)
foreach (_csr_config_group ${_csr_config_groups})
    get_property(_csr_config_default GLOBAL PROPERTY ${_csr_config_group})
    get_property(_csr_config_default_platform GLOBAL PROPERTY ${_csr_config_group}_${CSR_PLATFORM})
    get_property(_csr_config_description GLOBAL PROPERTY ${_csr_config_group}_DESCRIPTION)
    if ((_csr_config_default OR _csr_config_default_platform) AND _csr_config_description)
        if (_csr_config_default_platform)
            set (_csr_config_default ${_csr_config_default_platform})
        endif()
        set (${_csr_config_group} ${_csr_config_default} CACHE STRING ${_csr_config_description})
    else()
        message (FATAL_ERROR "Group ${_csr_config_group} has no default value for this platform")
    endif()
endforeach()

if (CSR_PLATFORM_${CSR_PLATFORM}_ASM_FLAGS)
    message(STATUS "Platform specific assembler flags: ${CSR_PLATFORM_${CSR_PLATFORM}_ASM_FLAGS}")
    set (CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${CSR_PLATFORM_${CSR_PLATFORM}_ASM_FLAGS}")
endif()

# Default output locations
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bin/java)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

include(${CSR_BUILD_ROOT}/build_system_tools.cmake)
include(${CSR_BUILD_ROOT}/build_system_tools_wireshark.cmake)

# Add global module test targets
if (${CSR_BUILD_INTERNAL})
    foreach(_technology ${CSR_TECHNOLOGIES})
        string(TOLOWER ${_technology} _technology_lc)
        add_custom_target("${_technology_lc}-module-test"
                      COMMAND ctest -R "caste_csr_${_technology_lc}")
        list(APPEND _module_test_targets "${_technology_lc}-module-test")
    endforeach()

    if (_module_test_targets)
        add_custom_target(module-test COMMAND ctest)
        add_dependencies(module-test ${_module_test_targets})
    endif()

    # Enable testing for current directory and below
    enable_testing()
endif()

# Collect all public include paths
foreach (_csr_build_technology ${CSR_TECHNOLOGIES})
    set (CSR_CURRENT_TECHNOLOGY ${_csr_build_technology})
    if (CSR_${_csr_build_technology}_ROOT)
        include (${CSR_${_csr_build_technology}_ROOT}/includes.cmake OPTIONAL)
    endif()
    unset (CSR_CURRENT_TECHNOLOGY)
endforeach()

# Process technologies
foreach (_csr_build_technology ${CSR_TECHNOLOGIES})
    if (CSR_${_csr_build_technology}_ROOT AND CSR_${_csr_build_technology}_BUILDSYSTEM_AVAILABLE)
        set (CSR_CURRENT_TECHNOLOGY ${_csr_build_technology})
        string (TOLOWER "${_csr_build_technology}" _csr_build_technology_lc)
        set (CSR_CURRENT_TECHNOLOGY_LC ${_csr_build_technology_lc})
        get_filename_component(_abs_dir ${CSR_${CSR_CURRENT_TECHNOLOGY}_ROOT} ABSOLUTE)
        file (RELATIVE_PATH _relative_file ${PROJECT_SOURCE_DIR} ${_abs_dir})
        message(STATUS "Traversing technology ${CSR_CURRENT_TECHNOLOGY}")
        csr_add_subdirectory(${CSR_${CSR_CURRENT_TECHNOLOGY}_ROOT})

        if(DEFINED CSR_PRODUCT_FILE)
            # Install buildsystem for technology
            set (_component_file "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/components_${CSR_PRODUCT_NAME}.cmake")
            if (EXISTS ${_component_file})
            csr_install_files(COMPONENT CSR_COMPONENT_${_csr_build_technology}_BUILD TECHNOLOGY ${_csr_build_technology} ${_component_file})
            endif()
            get_property(_mapping GLOBAL PROPERTY CSR_${CSR_CURRENT_TECHNOLOGY}_ARTIFACT_MAPPING)
            list (LENGTH _mapping _count)
            if (_count)
                if (EXISTS "${CSR_${CSR_CURRENT_TECHNOLOGY}_ROOT}/CMakeLists.txt.dist")
                    set (_install_src "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/CMakeLists.txt")
                    configure_file(${CSR_${CSR_CURRENT_TECHNOLOGY}_ROOT}/CMakeLists.txt.dist ${_install_src} COPYONLY)
                else()
                    set(_install_src "${CSR_${CSR_CURRENT_TECHNOLOGY}_ROOT}/CMakeLists.txt")
                endif()
            csr_install_files (COMPONENT CSR_COMPONENT_${_csr_build_technology}_BUILD TECHNOLOGY ${_csr_build_technology} ${_install_src})
            endif()
            unset (CSR_CURRENT_TECHNOLOGY)
        endif()
    endif()
endforeach()

if (CSR_BACKWARDS_COMPATIBILITY_ENABLE)
    set (_libs)
    foreach (_component ${CSR_COMPONENTS})
        get_property(_component_libs GLOBAL PROPERTY ${_component}_LIBRARIES)
        list (APPEND _libs ${_component_libs})
    endforeach()
    add_custom_target(lib DEPENDS ${_libs})
endif()

# Add the global quality targets
if (${CSR_BUILD_INTERNAL})
    csr_add_global_quality_targets()
endif()

# Verify the groups and set the parameters in the cache
get_property(_csr_config_groups GLOBAL PROPERTY CSR_GROUPS)
foreach (_csr_config_group ${_csr_config_groups})
    get_property(_csr_config_default GLOBAL PROPERTY ${_csr_config_group})
    get_property(_csr_config_selections GLOBAL PROPERTY ${_csr_config_group}_SELECTIONS)
    get_property(_csr_config_selections_available GLOBAL PROPERTY ${_csr_config_group}_SELECTIONS_AVAILABLE)
    if (_csr_config_selections_available)
        list(REMOVE_DUPLICATES _csr_config_selections_available)
        list(LENGTH _csr_config_selections_available _count)
        string (TOUPPER ${${_csr_config_group}} _csr_config_group_upper)
        list(GET _csr_config_selections_available 0 _csr_config_group_default)
        list(FIND _csr_config_selections_available ${_csr_config_group_upper} _csr_config_default_found)
        if (_csr_config_default_found EQUAL -1)
            get_property(_csr_config_description GLOBAL PROPERTY ${_csr_config_group}_DESCRIPTION)
            message (WARNING "The selected value of the group ${_csr_config_group} is not available. Changed to default value (${_csr_config_default})")
            set (${_csr_config_group} ${_csr_config_default} CACHE STRING ${_csr_config_description} FORCE)
        endif()
        if (${_count} GREATER 0)
            set_property(CACHE ${_csr_config_group} PROPERTY STRINGS ${_csr_config_selections})
        endif()
    endif()
endforeach()

if (WIRESHARK_AVAILABLE)
    set (_dissector_targets)
    add_custom_command(OUTPUT dissector_info.txt
                       COMMAND ${CMAKE_COMMAND}
                       ARGS -E echo "Installing dissectors to ${WIRESHARK_PLUGINS_LOCATION}")
endif()

# Create a export file if needed
foreach (_csr_build_technology ${CSR_TECHNOLOGIES})
    get_property(_csr_config_export_file GLOBAL PROPERTY CSR_${_csr_build_technology}_EXPORT_LIB)
    string (TOLOWER "${_csr_build_technology}" _csr_build_technology_lc)
    if (_csr_config_export_file)
        message (STATUS "Generating ${_csr_config_export_file} for binary installation")
        install (EXPORT ${_csr_config_export_file} FILE csr_exports_${CSR_PRODUCT_NAME}.cmake DESTINATION ${_csr_build_technology_lc}/${CSR_${_csr_build_technology}_VERSION_MAJOR}.${CSR_${_csr_build_technology}_VERSION_MINOR}.${CSR_${_csr_build_technology}_VERSION_FIXANDBUILD}/lib)
        csr_install_files(COMPONENT CSR_COMPONENT_${_csr_build_technology}_BUILD ${CSR_${_csr_build_technology}_MAPPING_LIBRARY_FILE})
    endif()

    # Define the wireshark dissector copy target
    if (WIRESHARK_AVAILABLE AND CSR_${_csr_build_technology}_BUILDSYSTEM_AVAILABLE)
        include (${CSR_${_csr_build_technology}_DISSECTOR_FILE})
        foreach (_file ${CSR_${_csr_build_technology}_DISSECTOR_SHARED_LIBRARIES})
            get_filename_component(_filename ${_file} NAME)
            add_custom_command(OUTPUT dissector_info.txt
                               COMMAND ${CMAKE_COMMAND}
                               ARGS -E copy ${_file} ${WIRESHARK_PLUGINS_LOCATION}
                               COMMENT "Installing ${_filename}"
                               APPEND)
        endforeach()
        foreach (_target ${CSR_${_csr_build_technology}_DISSECTOR_TARGETS})
            list (APPEND _dissector_targets ${_target})
        endforeach()

        # Create the temporary dissector file
        set (CSR_${_csr_build_technology}_DISSECTOR_INSTALL_FILE "${PROJECT_BINARY_DIR}/${_csr_build_technology_lc}/plugin/dissectors_install.cmake")
        csr_create_cmake_file(${CSR_${_csr_build_technology}_DISSECTOR_INSTALL_FILE})
        file (APPEND ${CSR_${_csr_build_technology}_DISSECTOR_INSTALL_FILE} "set (CSR_${_csr_build_technology}_DISSECTOR_FILES)\n")

        foreach (_file ${CSR_${_csr_build_technology}_DISSECTOR_SHARED_LIBRARIES})
            get_filename_component(_dissector ${_file} NAME)
            file (APPEND ${CSR_${_csr_build_technology}_DISSECTOR_INSTALL_FILE} "list (APPEND CSR_${_csr_build_technology}_DISSECTOR_FILES \"${_dissector}\")\n")
        endforeach()
    endif()
endforeach ()

if (WIRESHARK_AVAILABLE)
    add_custom_target(logtool-all     DEPENDS ${_dissector_targets})
    add_custom_target(logtool-install DEPENDS ${_dissector_targets} dissector_info.txt)
    unset(_dissector_targets)
endif()

# Create a file with the current values of the CSR_* variables in the cache
get_directory_property(_csr_cache_vars CACHE_VARIABLES)
set (_csr_prepopulate_file "${PROJECT_BINARY_DIR}/pre-populate-cache.cmake")
csr_create_cmake_file("${_csr_prepopulate_file}")
foreach (_csr_cache_var ${_csr_cache_vars})
    string(REGEX MATCH "CSR_(.*)" _string ${_csr_cache_var})
    if (_string)
        get_property(_csr_cache_strings CACHE ${_csr_cache_var} PROPERTY STRINGS)
        get_property(_csr_cache_value CACHE ${_csr_cache_var} PROPERTY VALUE)
        get_property(_csr_cache_type CACHE ${_csr_cache_var} PROPERTY TYPE)
        get_property(_csr_cache_helpstring CACHE ${_csr_cache_var} PROPERTY HELPSTRING)
        if (NOT ${_csr_cache_type} STREQUAL "INTERNAL")
            file (APPEND ${_csr_prepopulate_file} "set (${_csr_cache_var} \"${_csr_cache_value}\" CACHE ${_csr_cache_type} \"${_csr_cache_helpstring}\")\n")
        endif()
    endif()
endforeach()

if(DEFINED CSR_PRODUCT_FILE)
    # CMake package module
    set (CPACK_GENERATOR                        "ZIP")
    set (CPACK_SOURCE_GENERATOR                 "NONE")
    set (CPACK_PACKAGE_VENDOR                   "CSR plc")
    set (CPACK_PACKAGE_DESCRIPTION_SUMMARY      "CSR Synergy software")
    set (CPACK_PACKAGE_VERSION_MAJOR            "${CSR_PRODUCT_VERSION_MAJOR}")
    set (CPACK_PACKAGE_VERSION_MINOR            "${CSR_PRODUCT_VERSION_MINOR}")
    set (CPACK_PACKAGE_VERSION_PATCH            "${CSR_PRODUCT_VERSION_FIXANDBUILD}")

    set (_csr_release_package_name              "synergy")
    if (CSR_PRODUCT_NAME_PREFIX)
        set (_csr_release_package_name          "${_csr_release_package_name}_${CSR_PRODUCT_NAME_PREFIX}")
    endif()
    if (CSR_PRODUCT_RELEASE_TYPE_ENG)
        set (_csr_release_package_name              "${_csr_release_package_name}_${CSR_PRODUCT_VERSION_MAJOR}_${CSR_PRODUCT_VERSION_MINOR}_${CSR_PRODUCT_VERSION_FIXLEVEL}_${CSR_PRODUCT_VERSION_BUILD}")
    else()
        set (_csr_release_package_name              "${_csr_release_package_name}_${CSR_PRODUCT_VERSION_MAJOR}_${CSR_PRODUCT_VERSION_MINOR}_${CSR_PRODUCT_VERSION_FIXLEVEL}")
    endif()
    if (CSR_PRODUCT_NAME_SUFFIX)
        set (_csr_release_package_name          "${_csr_release_package_name}_${CSR_PRODUCT_NAME_SUFFIX}")
    endif()

    set (CPACK_PACKAGE_FILE_NAME                "synergy")
    set (CPACK_PACKAGE_NAME                     "${CPACK_PACKAGE_FILE_NAME}")

    set (CPACK_RESOURCE_FILE_LICENSE            "${CSR_BUILD_ROOT}/package/copyright.txt")

    string(COMPARE EQUAL ${CMAKE_BUILD_TYPE} none CSR_BUILD_TYPE_SET)
    if( (NOT (CSR_BUILD_TYPE_SET)) AND (CMAKE_HOST_WIN32))
        set (_csr_build_target          --config ${CMAKE_BUILD_TYPE})
    else()
        set (_csr_build_target          "")
    endif()

    # Add custom release target
    add_custom_target(release
                      COMMAND ${CMAKE_COMMAND} --build . ${_csr_build_target} --target package
                      COMMAND ${CMAKE_COMMAND} -E rename ${CPACK_PACKAGE_FILE_NAME}.zip ${_csr_release_package_name}.zip
                      VERBATIM)
    include(CPack)

    # Define component that can be installed
    foreach (_csr_build_technology ${CSR_TECHNOLOGIES})
        cpack_add_component(${_csr_build_technology}
                            DISPLAY_NAME ${CSR_${_csr_build_technology}_DISPLAY_NAME}
                            DESCRIPTION ${CSR_${_csr_build_technology}_DESCRIPTION})
    endforeach()
endif()

