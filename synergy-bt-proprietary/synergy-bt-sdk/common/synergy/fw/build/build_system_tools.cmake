##############################################################################
#
# Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# Perl
find_package(Perl)

# Coverity
set (COVERITY_PATHS /usr/local/prevent/bin
                    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\ej-technologies\\install4j\\installations]/instdir2905-5584-7095-2487"
                    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\ej-technologies\\install4j\\installations]/allinstdirs2905-5584-7095-2487"
                    "C:/Program Files/Coverity/Coverity Static Analysis/bin")
find_program(COVERITY_ANALYZE_EXECUTABLE
             NAMES cov-analyze
             PATHS ${COVERITY_PATHS})
find_program(COVERITY_TRANSLATE_EXECUTABLE
             NAMES cov-translate
             PATHS ${COVERITY_PATHS})
find_program(COVERITY_COMMIT_EXECUTABLE
             NAMES cov-commit-defects
             PATHS ${COVERITY_PATHS})
find_program(COVERITY_FORMAT_EXECUTABLE
             NAMES cov-format-errors
             PATHS ${COVERITY_PATHS})
find_program(COVERITY_MAKE_LIBRARY_EXECUTABLE
             NAMES cov-make-library
             PATHS ${COVERITY_PATHS})
find_package_handle_standard_args("Coverity - Analyze" DEFAULT_MSG COVERITY_ANALYZE_EXECUTABLE)
find_package_handle_standard_args("Coverity - Translate" DEFAULT_MSG COVERITY_TRANSLATE_EXECUTABLE)
find_package_handle_standard_args("Coverity - Commit Defects" DEFAULT_MSG COVERITY_COMMIT_EXECUTABLE)
find_package_handle_standard_args("Coverity - Format Errors" DEFAULT_MSG COVERITY_FORMAT_EXECUTABLE)
find_package_handle_standard_args("Coverity - Make Library" DEFAULT_MSG COVERITY_MAKE_LIBRARY_EXECUTABLE)
mark_as_advanced(COVERITY_ANALYZE_EXECUTABLE
                 COVERITY_TRANSLATE_EXECUTABLE
                 COVERITY_COMMIT_EXECUTABLE
                 COVERITY_FORMAT_EXECUTABLE
                 COVERITY_MAKE_LIBRARY_EXECUTABLE)
set(COVERITY_SERVER "" CACHE STRING "Coverity server address")
set(COVERITY_DATA_PORT "" CACHE STRING "Coverity server data port")
set(COVERITY_STREAM_NAME "" CACHE STRING "Coverity commit stream name")
set(COVERITY_USER "" CACHE STRING "Coverity commit user")
set(COVERITY_PASSWORD "" CACHE STRING "Coverity commit password")
set(COVERITY_ANALYZE_OPTIONS "--all --checker-option STACK_USE:max_single_base_use_bytes:512 --checker-option PASS_BY_VALUE:size_threshold:512" CACHE STRING "Coverity analyze options")
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/coverity)

# Uncrustify
set(UNCRUSTIFY_SUPPORTED TRUE)
set(UNCRUSTIFY_PATHS "/usr/bin"
                     "c:/Program Files/Uncrustify"
                     "c:/Program Files (x86)/Uncrustify")
find_program (UNCRUSTIFY_EXECUTABLE
              NAMES uncrustify
              PATHS ${UNCRUSTIFY_PATHS})
if (UNCRUSTIFY_EXECUTABLE)
    execute_process(COMMAND "${UNCRUSTIFY_EXECUTABLE}" --version
                    OUTPUT_VARIABLE _csr_tools_uncrustify_out ERROR_VARIABLE _csr_tools_uncrustify_out)
    string (REGEX MATCH "([0-9\\.]+)" _match ${_csr_tools_uncrustify_out})
    if (NOT _match STREQUAL 0.56)
        message(STATUS "Uncrustify-${_match} not supported (must be 0.56) - Uncrustify support disabled")
        set(UNCRUSTIFY_SUPPORTED FALSE)
    endif()
endif()
find_package_handle_standard_args("Uncrustify" DEFAULT_MSG UNCRUSTIFY_EXECUTABLE)
mark_as_advanced(UNCRUSTIFY_EXECUTABLE)

# XML Generation tool
if (PERL_EXECUTABLE)
    if (EXISTS "${PROJECT_SOURCE_DIR}/.p4config")
        set (_csr_tools_p4config "--p4config=${PROJECT_SOURCE_DIR}/.p4config")
    endif()
    add_custom_target(autogen_frw
                      COMMAND "${PERL_EXECUTABLE}" "generate.pl" "--tech=frw" ${_csr_tools_p4config} "--outpath=${PROJECT_SOURCE_DIR}" "--include=${CSR_TOOLS_ROOT}/autogen/xml/frw/bsp"
                      WORKING_DIRECTORY "${CSR_TOOLS_ROOT}/autogen"
                      COMMENT "Generating Framework files")
    add_custom_target(autogen_wifi
                      COMMAND "${PERL_EXECUTABLE}" "generate.pl" "--tech=wifi" ${_csr_tools_p4config} "--outpath=${PROJECT_SOURCE_DIR}"
                      WORKING_DIRECTORY "${CSR_TOOLS_ROOT}/autogen"
                      COMMENT "Generating Wi-Fi files")
    add_custom_target(autogen_mercury
                      COMMAND "${PERL_EXECUTABLE}" "generate.pl" "--tech=mercury" ${_csr_tools_p4config} "--outpath=${PROJECT_SOURCE_DIR}"
                      WORKING_DIRECTORY "${CSR_TOOLS_ROOT}/autogen"
                      COMMENT "Generating Mercury files")
    add_custom_target(autogen)
    add_dependencies(autogen autogen_frw autogen_wifi autogen_mercury)
endif()

# Lint
set(LINT_SUPPORTED TRUE)
set(LINT_SYSTEM_INCLUDES "${CSR_TOOLS_ROOT}/lint")
set(LINT_CONFIG_FILES "eclipse-format.lnt")
find_program(LINT_EXECUTABLE
             NAMES lint-nt flint)
if (LINT_EXECUTABLE)
    if (CSR_PLATFORM STREQUAL WINDOWS)
        if (MSVC60)
            list(APPEND LINT_CONFIG_FILES "co-msc60.lnt")
        elseif (MSVC70)
            list(APPEND LINT_CONFIG_FILES "co-msc70.lnt")
        elseif (MSVC71)
            list(APPEND LINT_CONFIG_FILES "co-msc71.lnt")
        elseif (MSVC80)
            list(APPEND LINT_CONFIG_FILES "co-msc80.lnt")
        elseif (MSVC90)
            list(APPEND LINT_CONFIG_FILES "co-msc90.lnt")
        elseif (MSVC10)
            list(APPEND LINT_CONFIG_FILES "co-msc100.lnt")
        else()
            set(LINT_SUPPORTED FALSE)
        endif()
    elseif (CSR_PLATFORM STREQUAL LINUX)
        if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
            list(APPEND LINT_CONFIG_FILES "co-gcc.lnt")
            set(_csr_tools_lint_include_path_file "${PROJECT_BINARY_DIR}/lint/gcc-include-path.lnt")
        elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
            list(APPEND LINT_CONFIG_FILES "co-clang.lnt")
            set(_csr_tools_lint_include_path_file "${PROJECT_BINARY_DIR}/lint/clang-include-path.lnt")
        else()
            set(LINT_SUPPORTED FALSE)
        endif()

        if (LINT_SUPPORTED)
            file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/lint)
            list(APPEND LINT_SYSTEM_INCLUDES "${PROJECT_BINARY_DIR}/lint")
            file(WRITE "${PROJECT_BINARY_DIR}/lint/empty.c" "")

            # Determine system include paths
            execute_process(COMMAND ${CMAKE_C_COMPILER} -v -c "${PROJECT_BINARY_DIR}/lint/empty.c"
                            WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/lint"
                            OUTPUT_VARIABLE _csr_tools_output
                            ERROR_VARIABLE _csr_tools_error)
            string(REGEX MATCH "> search starts here:\n(.+)\nEnd of search list" match ${_csr_tools_error})
            string(REGEX MATCHALL "([^ \n]+)" match ${CMAKE_MATCH_1})
            file(WRITE ${_csr_tools_lint_include_path_file} "")
            foreach (_csr_tools_include_path ${match})
                file(APPEND ${_csr_tools_lint_include_path_file} "--i${_csr_tools_include_path}\n")
            endforeach()

            # Determine internal compiler defines
            execute_process(COMMAND ${CMAKE_C_COMPILER} -E -dM "${PROJECT_BINARY_DIR}/lint/empty.c" -o "${PROJECT_BINARY_DIR}/lint/lint_cmac.h")

            # Determine size options from predefined macros
            file(READ "${PROJECT_BINARY_DIR}/lint/lint_cmac.h" _csr_tools_output)
            string(REGEX MATCH "__SIZEOF_SHORT__ ([0-9]+)" match ${_csr_tools_output})
            file(WRITE "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-ss${CMAKE_MATCH_1}\n")
            string(REGEX MATCH "__SIZEOF_INT__ ([0-9]+)" match ${_csr_tools_output})
            file(APPEND "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-si${CMAKE_MATCH_1}\n")
            string(REGEX MATCH "__SIZEOF_LONG__ ([0-9]+)" match ${_csr_tools_output})
            file(APPEND "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-sl${CMAKE_MATCH_1}\n")
            string(REGEX MATCH "__SIZEOF_LONG_LONG__ ([0-9]+)" match ${_csr_tools_output})
            file(APPEND "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-sll${CMAKE_MATCH_1}\n")
            string(REGEX MATCH "__SIZEOF_FLOAT__ ([0-9]+)" match ${_csr_tools_output})
            file(APPEND "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-sf${CMAKE_MATCH_1}\n")
            string(REGEX MATCH "__SIZEOF_DOUBLE__ ([0-9]+)" match ${_csr_tools_output})
            file(APPEND "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-sd${CMAKE_MATCH_1}\n")
            string(REGEX MATCH "__SIZEOF_LONG_DOUBLE__ ([0-9]+)" match ${_csr_tools_output})
            file(APPEND "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-sld${CMAKE_MATCH_1}\n")
            string(REGEX MATCH "__SIZEOF_POINTER__ ([0-9]+)" match ${_csr_tools_output})
            file(APPEND "${PROJECT_BINARY_DIR}/lint/size-options.lnt" "-sp${CMAKE_MATCH_1}\n")

            file(REMOVE "${PROJECT_BINARY_DIR}/lint/empty.c")
            file(REMOVE "${PROJECT_BINARY_DIR}/lint/empty.o")
        endif()
    elseif (CSR_PLATFORM STREQUAL BDB3)
        list(APPEND LINT_SYSTEM_INCLUDES ${CSR_ARM_PATH_INC})
        list(APPEND LINT_CONFIG_FILES "co-rvct.lnt")
    else()
        set(LINT_SUPPORTED FALSE)
    endif()
endif()
find_package_handle_standard_args("Lint" DEFAULT_MSG LINT_EXECUTABLE)
mark_as_advanced(LINT_EXECUTABLE)
if (NOT LINT_SUPPORTED)
    message(STATUS "Lint not supported with the selected compiler/platform")
endif()

# P4 Command Line tool
set (P4_PATHS "/usr/bin"
              "c:/Program Files/Perforce"
              "c:/Program Files (x86)/Perforce")
find_program(P4_EXECUTABLE
             NAMES p4
             PATHS ${P4_PATHS})
find_package_handle_standard_args("P4" DEFAULT_MSG P4_EXECUTABLE)
mark_as_advanced(P4_EXECUTABLE)

# Diff
find_program(DIFF_EXECUTABLE
             NAMES diff)
find_package_handle_standard_args("Diff" DEFAULT_MSG DIFF_EXECUTABLE)
mark_as_advanced(DIFF_EXECUTABLE)
