##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(CSR_ALLOW_TODO_COMMENTS OFF
    CACHE BOOL
    "Enable this option to have the codecheck quality tool ignore TODO comments in all source code.")

set(CSR_QUALITY_TOOLS_RESTRICT ON
    CACHE BOOL
    "Enable this option to restrict the quality tools to selected groups and the actual platform.")

macro(csr_add_quality_checks prefix)
    set(_csr_macro_action)
    set(_csr_macro_uncrustify_level ${CSR_DEFAULT_UNCRUSTIFY_LEVEL})
    set(_csr_macro_codecheck_level ${CSR_DEFAULT_CODECHECK_LEVEL})
    set(_csr_macro_coverity_level ${CSR_DEFAULT_COVERITY_LEVEL})
    set(_csr_macro_lint_level ${CSR_DEFAULT_LINT_LEVEL})
    set(_csr_macro_sources)
    set(_csr_macro_sources_disabled)
    set(_csr_macro_headers)
    set(_csr_macro_headers_disabled)
    set(_csr_macro_condition)
    set(_csr_macro_condition_true)
    set(_csr_macro_technology ${CSR_CURRENT_TECHNOLOGY})
    set(_csr_macro_signature
            "csr_add_quality_checks(<prefix>"
            "                       [UNCRUSTIFY <level>]"
            "                       [CODECHECK <level>]"
            "                       [COVERITY <level>]"
            "                       [LINT <level>]"
            "                       [LINT_FP <level>]"
            "                       [TECHNOLOGY <technology>]"
            "                       [[CONDITIONAL <condition>] SOURCES <source1> <source2> ...]"
            "                       [[CONDITIONAL <condition>] HEADERS <header1> <header2> ...]")

    foreach(_currentArg ${ARGN})
        if ("_${_currentArg}" STREQUAL "_UNCRUSTIFY" OR
            "_${_currentArg}" STREQUAL "_CODECHECK" OR
            "_${_currentArg}" STREQUAL "_COVERITY" OR
            "_${_currentArg}" STREQUAL "_LINT" OR
            "_${_currentArg}" STREQUAL "_LINT_FP" OR
            "_${_currentArg}" STREQUAL "_TECHNOLOGY")
            set(_csr_macro_action "${_currentArg}")
        elseif ("_${_currentArg}" STREQUAL "_SOURCES" OR
                "_${_currentArg}" STREQUAL "_HEADERS")
            set(_csr_macro_action "${_currentArg}")
            set(_csr_macro_condition_true TRUE)
            if (_csr_macro_condition)
                set(_csr_macro_condition_true ${_csr_macro_condition})
                set(_csr_macro_condition)
            endif()
        elseif ("_${_currentArg}" STREQUAL "_CONDITIONAL")
            set(_csr_macro_action "${_currentArg}")
            set(_csr_macro_condition)
        elseif (${_csr_macro_action} STREQUAL "UNCRUSTIFY")
            set(_csr_macro_uncrustify_level "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "CODECHECK")
            set(_csr_macro_codecheck_level "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "COVERITY")
            set(_csr_macro_coverity_level "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "LINT")
            set(_csr_macro_lint_level "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "LINT_FP")
            set(_csr_macro_lint_fp_level "${_currentArg}")
            MESSAGE("False positive suppression enabled ${_csr_macro_lint_fp_level}")
        elseif (${_csr_macro_action} STREQUAL "TECHNOLOGY")
            set(_csr_macro_technology "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "SOURCES")
            get_filename_component(_csr_macro_source_abs ${_currentArg} ABSOLUTE)
            file(RELATIVE_PATH _csr_macro_source_rel ${CMAKE_CURRENT_SOURCE_DIR} ${_csr_macro_source_abs})
            if (${_csr_macro_condition_true})
                list(APPEND _csr_macro_sources "${_csr_macro_source_rel}")
            else()
                list(APPEND _csr_macro_sources_disabled "${_csr_macro_source_rel}")
            endif()
        elseif (${_csr_macro_action} STREQUAL "HEADERS")
            get_filename_component(_csr_macro_header_abs ${_currentArg} ABSOLUTE)
            file(RELATIVE_PATH _csr_macro_header_rel ${CMAKE_CURRENT_SOURCE_DIR} ${_csr_macro_header_abs})
            if (${_csr_macro_condition_true})
                list(APPEND _csr_macro_headers "${_csr_macro_header_rel}")
            else()
                list(APPEND _csr_macro_headers_disabled "${_csr_macro_header_rel}")
            endif()
        elseif (${_csr_macro_action} STREQUAL "CONDITIONAL")
            list(APPEND _csr_macro_condition "${_currentArg}")
        else()
            message(FATAL_ERROR "Parameter '${_currentArg}' incorrect\n"
                                ${_csr_macro_signature})
        endif()
    endforeach()

    # Determine if the source code belongs to the selected platform (or is generic)
    set(_csr_macro_platform_selected TRUE)
    if (CSR_INTERNAL_PLATFORM)
        set(_csr_macro_platform_selected FALSE)
        foreach (_csr_macro_platform ${CSR_INTERNAL_PLATFORM})
            if (${_csr_macro_platform} STREQUAL ${CSR_PLATFORM})
                set(_csr_macro_platform_selected TRUE)
                break()
            endif()
        endforeach()
    endif()

    # Determine if the sources can be built (group selected and actual platform)
    set(_csr_macro_should_build)
    if (_csr_macro_platform_selected AND CSR_INTERNAL_GROUP_SHOULD_BUILD)
        set(_csr_macro_should_build TRUE)
    endif()

    # Determine if the sources can/should be checked and modify the checker prefix if necessary
    set(_csr_macro_should_check)
    if (CSR_QUALITY_TOOLS_RESTRICT)
        if (_csr_macro_platform_selected AND CSR_INTERNAL_GROUP_SHOULD_BUILD)
            set(_csr_macro_should_check TRUE)
        endif()
    else()
        set(_csr_macro_should_check TRUE)
    endif()

    if (_csr_macro_should_check)

        # Uncrustify
        if (UNCRUSTIFY_EXECUTABLE AND UNCRUSTIFY_SUPPORTED AND PERL_EXECUTABLE AND _csr_macro_uncrustify_level)

            # Check
            foreach (_csr_macro_source ${_csr_macro_sources} ${_csr_macro_sources_disabled} ${_csr_macro_headers} ${_csr_macro_headers_disabled})
                get_filename_component(_csr_macro_inputfile "${CMAKE_CURRENT_SOURCE_DIR}/${_csr_macro_source}" ABSOLUTE)
                get_filename_component(_csr_macro_outputfile "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source}" ABSOLUTE)
                get_filename_component(_csr_macro_outputdirectory ${_csr_macro_outputfile} PATH)
                file(MAKE_DIRECTORY ${_csr_macro_outputdirectory})
                if (${_csr_macro_uncrustify_level} EQUAL 2)
                    if (DIFF_EXECUTABLE)
                        set(_csr_macro_compare_command "${DIFF_EXECUTABLE}" -up)
                    else()
                        set(_csr_macro_compare_command "${CMAKE_COMMAND}" -E compare_files)
                    endif()
                    add_custom_command(OUTPUT "${_csr_macro_outputfile}.uncrustify${_csr_macro_uncrustify_level}"
                                       COMMAND "${CMAKE_COMMAND}" -E remove -f
                                               "${_csr_macro_source}.uncrustify${_csr_macro_uncrustify_level}"
                                       COMMAND "${UNCRUSTIFY_EXECUTABLE}" -q -l C
                                               -c "${CSR_TOOLS_ROOT}/uncrustify/uncrustify.transform.cfg"
                                               -f ${_csr_macro_inputfile}
                                               -o "${_csr_macro_source}.uncrustify"
                                       COMMAND ${_csr_macro_compare_command}
                                               ${_csr_macro_inputfile}
                                               "${_csr_macro_outputfile}.uncrustify"
                                       COMMAND "${CMAKE_COMMAND}" -E touch
                                               "${_csr_macro_source}.uncrustify${_csr_macro_uncrustify_level}"
                                       DEPENDS ${_csr_macro_source}
                                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                                       COMMENT "Uncrustify (Q${_csr_macro_uncrustify_level}): ${_csr_macro_inputfile}"
                                       VERBATIM)
                elseif (${_csr_macro_uncrustify_level} EQUAL 1)
                    add_custom_command(OUTPUT "${_csr_macro_outputfile}.uncrustify${_csr_macro_uncrustify_level}"
                                       COMMAND "${CMAKE_COMMAND}" -E remove -f
                                               "${_csr_macro_source}.uncrustify${_csr_macro_uncrustify_level}"
                                       COMMAND "${UNCRUSTIFY_EXECUTABLE}" -q -l C
                                               -c "${CSR_TOOLS_ROOT}/uncrustify/uncrustify.check.cfg"
                                               -f ${_csr_macro_inputfile}
                                               -o "${_csr_macro_source}.uncrustify"
                                       COMMAND "${PERL_EXECUTABLE}" "${CSR_TOOLS_ROOT}/uncrustify/uncrustify-compare.pl"
                                               ${_csr_macro_inputfile}
                                               "${_csr_macro_outputfile}.uncrustify"
                                       COMMAND "${CMAKE_COMMAND}" -E touch
                                               "${_csr_macro_source}.uncrustify${_csr_macro_uncrustify_level}"
                                       DEPENDS ${_csr_macro_source}
                                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                                       COMMENT "Uncrustify (Q${_csr_macro_uncrustify_level}): ${_csr_macro_inputfile}"
                                       VERBATIM)
                else()
                    message(FATAL_ERROR "Invalid Uncrustify quality level (${_csr_macro_uncrustify_level})")
                endif()
                set_property(DIRECTORY APPEND
                             PROPERTY "${prefix}_uncrustify_DEPENDS"
                                      "${_csr_macro_outputfile}.uncrustify${_csr_macro_uncrustify_level}")
            endforeach()

            # Transform
            set(_csr_macro_sourcesheaders ${_csr_macro_sources} ${_csr_macro_sources_disabled} ${_csr_macro_headers} ${_csr_macro_headers_disabled})
            list(GET _csr_macro_sourcesheaders 0 _csr_macro_source)
            add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source}.uncrustify.symbolic"
                               COMMAND "${PERL_EXECUTABLE}" "${CSR_TOOLS_ROOT}/uncrustify/uncrustify-transform.pl"
                                       --cfg "${CSR_TOOLS_ROOT}/uncrustify/uncrustify.transform.cfg"
                                       --cmd ${UNCRUSTIFY_EXECUTABLE}
                                       --files "${_csr_macro_sourcesheaders}"
                               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                               DEPENDS ${_csr_macro_sourcesheaders}
                               COMMENT "Uncrustifying: ${CMAKE_CURRENT_SOURCE_DIR}/..."
                               VERBATIM)
            set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source}.uncrustify.symbolic"
                                        PROPERTIES SYMBOLIC TRUE)
            set_property(DIRECTORY APPEND
                         PROPERTY "${prefix}_uncrustify_transform_DEPENDS"
                                  "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source}.uncrustify.symbolic")
        endif()

        # Codecheck
        if (PERL_EXECUTABLE AND _csr_macro_codecheck_level)
            set(_csr_macro_codecheck_parameters)
            if (CSR_INTERNAL_PLATFORM)
                list(APPEND _csr_macro_codecheck_parameters "--platform")
            endif()
            if (CSR_ALLOW_TODO_COMMENTS)
                list(APPEND _csr_macro_codecheck_parameters "--allow-todo-comments")
            endif()
            set(_csr_macro_sourcesheaders ${_csr_macro_sources} ${_csr_macro_sources_disabled} ${_csr_macro_headers} ${_csr_macro_headers_disabled})
            list(GET _csr_macro_sourcesheaders 0 _csr_macro_source)
            get_filename_component(_csr_macro_outputfile "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source}" ABSOLUTE)
            get_filename_component(_csr_macro_outputdirectory ${_csr_macro_outputfile} PATH)
            file(MAKE_DIRECTORY ${_csr_macro_outputdirectory})
            add_custom_command(OUTPUT "${_csr_macro_outputfile}.codecheck${_csr_macro_codecheck_level}"
                               COMMAND "${CMAKE_COMMAND}" -E remove -f
                                       "${_csr_macro_outputfile}.codecheck${_csr_macro_codecheck_level}"
                               COMMAND "${PERL_EXECUTABLE}" "${CSR_TOOLS_ROOT}/codecheck/codecheck.pl"
                                       ${_csr_macro_codecheck_parameters}
                                       --tech "${_csr_macro_technology}"
                                       --level "${_csr_macro_codecheck_level}"
                                       --files "${_csr_macro_sourcesheaders}"
                               COMMAND "${CMAKE_COMMAND}" -E touch
                                       "${_csr_macro_outputfile}.codecheck${_csr_macro_codecheck_level}"
                               DEPENDS ${_csr_macro_sourcesheaders} "${CSR_TOOLS_ROOT}/codecheck/codecheck.pl"
                               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                               COMMENT "Codecheck (Q${_csr_macro_codecheck_level}): ${CMAKE_CURRENT_SOURCE_DIR}/..."
                               VERBATIM)
            set_property(DIRECTORY APPEND
                         PROPERTY "${prefix}_codecheck_DEPENDS"
                                  "${_csr_macro_outputfile}.codecheck${_csr_macro_codecheck_level}")
        endif()
    endif()

    if (_csr_macro_should_build)

        # Coverity
        if (COVERITY_TRANSLATE_EXECUTABLE AND _csr_macro_coverity_level)
            get_directory_property(_csr_macro_include_directories INCLUDE_DIRECTORIES)
            get_directory_property(_csr_macro_compile_definitions COMPILE_DEFINITIONS)
            set(_csr_macro_coverity_parameters)
            foreach (_csr_macro_include_directory ${_csr_macro_include_directories})
                set(_csr_macro_coverity_parameters  ${_csr_macro_coverity_parameters} "-I${_csr_macro_include_directory}")
            endforeach()
            foreach (_csr_macro_compile_definition ${_csr_macro_compile_definitions})
                list(APPEND _csr_macro_coverity_parameters "-D${_csr_macro_compile_definition}")
            endforeach()
            foreach (_csr_macro_source ${_csr_macro_sources})
                get_filename_component(_csr_macro_inputfile "${CMAKE_CURRENT_SOURCE_DIR}/${_csr_macro_source}" ABSOLUTE)
                file(RELATIVE_PATH _csr_macro_stamp ${PROJECT_SOURCE_DIR} ${_csr_macro_inputfile})
                string(REPLACE "/" "_" _csr_macro_stamp "${_csr_macro_stamp}.stamp")
                add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/coverity/${_csr_macro_stamp}"
                                   COMMAND "${COVERITY_TRANSLATE_EXECUTABLE}"
                                           --verbose 0 --force
                                           --dir coverity
                                           --redirect "stdout,coverity/${_csr_macro_stamp}"
                                           --preinclude "${PROJECT_SOURCE_DIR}/tools/coverity/csr_tools_coverity_nodefs.h"
                                           ${CMAKE_C_COMPILER}
                                           ${_csr_macro_coverity_parameters}
                                           ${_csr_macro_inputfile}
                                   DEPENDS ${_csr_macro_source} "${PROJECT_BINARY_DIR}/inc/csr_synergy.h"
                                   IMPLICIT_DEPENDS C ${_csr_macro_inputfile}
                                   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                                   COMMENT "Coverity: ${_csr_macro_inputfile}"
                                   VERBATIM)
                set_property(DIRECTORY APPEND
                             PROPERTY "${prefix}_coverity_DEPENDS"
                                      "${PROJECT_BINARY_DIR}/coverity/${_csr_macro_stamp}")
            endforeach()
        endif()

        # Lint
        if (LINT_EXECUTABLE AND LINT_SUPPORTED AND _csr_macro_lint_level)
            get_directory_property(_csr_macro_include_directories INCLUDE_DIRECTORIES)
            get_directory_property(_csr_macro_compile_definitions COMPILE_DEFINITIONS)
            set(_csr_macro_lint_parameters "-zero" "-v" "-b" "-u" "-width(0,0)")
            foreach (_csr_macro_include_directory ${_csr_macro_include_directories} ${LINT_SYSTEM_INCLUDES})
                list(APPEND _csr_macro_lint_parameters "-I${_csr_macro_include_directory}")
            endforeach()
            foreach (_csr_macro_compile_definition ${_csr_macro_compile_definitions})
                list(APPEND _csr_macro_lint_parameters "-D${_csr_macro_compile_definition}")
            endforeach()
            foreach (_csr_macro_source ${_csr_macro_sources})
                get_filename_component(_csr_macro_inputfile "${CMAKE_CURRENT_SOURCE_DIR}/${_csr_macro_source}" ABSOLUTE)
                get_filename_component(_csr_macro_outputfile "${CMAKE_CURRENT_BINARY_DIR}/${_csr_macro_source}" ABSOLUTE)
                get_filename_component(_csr_macro_outputdirectory ${_csr_macro_outputfile} PATH)
                file(MAKE_DIRECTORY ${_csr_macro_outputdirectory})
                if ("${_csr_macro_lint_fp_level}" STREQUAL "fp")
                    set(CSR_LINT_FP_FILE "ql-${_csr_macro_lint_fp_level}.lnt")	 
                endif()
                add_custom_command(OUTPUT "${_csr_macro_outputfile}.lint${_csr_macro_lint_level}"
                                   COMMAND "${LINT_EXECUTABLE}"
                                           ${_csr_macro_lint_parameters}
                                           ${CSR_LINT_FP_FILE}
                                           "ql-${_csr_macro_lint_level}.lnt"
                                           "ql-all.lnt"
                                           ${LINT_CONFIG_FILES}
                                           ${_csr_macro_inputfile}
                                   COMMAND "${CMAKE_COMMAND}" -E touch
                                           "${_csr_macro_outputfile}.lint${_csr_macro_lint_level}"
                                   DEPENDS ${_csr_macro_source} "${CSR_TOOLS_ROOT}/lint/ql-${_csr_macro_lint_level}.lnt" "${CSR_TOOLS_ROOT}/lint/ql-all.lnt" "${PROJECT_BINARY_DIR}/inc/csr_synergy.h"
                                   IMPLICIT_DEPENDS C ${_csr_macro_inputfile}
                                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                                   COMMENT "Lint (Q${_csr_macro_lint_level}): ${_csr_macro_inputfile}"
                                   VERBATIM)
                set_property(DIRECTORY APPEND
                             PROPERTY "${prefix}_lint_DEPENDS"
                                      "${_csr_macro_outputfile}.lint${_csr_macro_lint_level}")
            endforeach()
        endif()
    endif()
endmacro(csr_add_quality_checks)

macro(csr_add_quality_targets prefix)
    set(prefix_full ${prefix})

    # Determine if the source code belongs to the selected platform
    set(_csr_macro_platform_selected TRUE)
    if (CSR_INTERNAL_PLATFORM)
        set(_csr_macro_platform_selected FALSE)
        foreach (_csr_macro_platform ${CSR_INTERNAL_PLATFORM})
            if (${_csr_macro_platform} STREQUAL ${CSR_PLATFORM})
                set(_csr_macro_platform_selected TRUE)
                break()
            endif()
        endforeach()
    endif()

    # Determine if the sources can be built (group selected and actual platform)
    set(_csr_macro_should_build)
    if (_csr_macro_platform_selected AND CSR_INTERNAL_GROUP_SHOULD_BUILD)
        set(_csr_macro_should_build TRUE)
    endif()

    # Determine if the sources can/should be checked and modify the checker prefix if necessary
    set(_csr_macro_should_check)
    if (CSR_QUALITY_TOOLS_RESTRICT)
        if (_csr_macro_platform_selected AND CSR_INTERNAL_GROUP_SHOULD_BUILD)
            set(_csr_macro_should_check TRUE)
        endif()
    else()
        set(_csr_macro_should_check TRUE)
        if (NOT _csr_macro_platform_selected)
            string(REPLACE ";" "_" prefix_full "${prefix_full}_${CSR_INTERNAL_PLATFORM}")
        endif()
        if (NOT CSR_INTERNAL_GROUP_SHOULD_BUILD)
            set(prefix_full "${prefix_full}_${CSR_INTERNAL_GROUP_NAME}")
        endif()
    endif()

    if (_csr_macro_should_check)
        # Uncrustify (module)
        get_property(_csr_macro_uncrustify_depends DIRECTORY PROPERTY "${prefix}_uncrustify_DEPENDS")
        if (_csr_macro_uncrustify_depends)
            set_property(GLOBAL APPEND PROPERTY "QUALITY_TARGETS_UNCRUSTIFY_${CSR_CURRENT_TECHNOLOGY}" "${prefix_full}-uncrustify")
            add_custom_target("${prefix_full}-uncrustify"
                              DEPENDS ${_csr_macro_uncrustify_depends})
        endif()
        get_property(_csr_macro_uncrustify_transform_depends DIRECTORY PROPERTY "${prefix}_uncrustify_transform_DEPENDS")
        if (_csr_macro_uncrustify_transform_depends)
            set_property(GLOBAL APPEND PROPERTY "QUALITY_TARGETS_UNCRUSTIFY_TRANSFORM_${CSR_CURRENT_TECHNOLOGY}" "${prefix_full}-uncrustify_transform")
            add_custom_target("${prefix_full}-uncrustify_transform"
                              DEPENDS ${_csr_macro_uncrustify_transform_depends})
        endif()

        # Codecheck (module)
        get_property(_csr_macro_codecheck_depends DIRECTORY PROPERTY "${prefix}_codecheck_DEPENDS")
        if (_csr_macro_codecheck_depends)
            set_property(GLOBAL APPEND PROPERTY "QUALITY_TARGETS_CODECHECK_${CSR_CURRENT_TECHNOLOGY}" "${prefix_full}-codecheck")
            add_custom_target("${prefix_full}-codecheck"
                              DEPENDS ${_csr_macro_codecheck_depends})
        endif()
    endif()

    if (_csr_macro_should_build)
        # Coverity (module)
        get_property(_csr_macro_coverity_depends DIRECTORY PROPERTY "${prefix}_coverity_DEPENDS")
        if (_csr_macro_coverity_depends)
            set_property(GLOBAL APPEND PROPERTY "QUALITY_TARGETS_COVERITY_${CSR_CURRENT_TECHNOLOGY}" "${prefix}-coverity")
            add_custom_target("${prefix}-coverity"
                              DEPENDS ${_csr_macro_coverity_depends})
        endif()

        # Lint (module)
        get_property(_csr_macro_lint_depends DIRECTORY PROPERTY "${prefix}_lint_DEPENDS")
        if (_csr_macro_lint_depends)
            set_property(GLOBAL APPEND PROPERTY "QUALITY_TARGETS_LINT_${CSR_CURRENT_TECHNOLOGY}" "${prefix}-lint")
            add_custom_target("${prefix}-lint"
                              DEPENDS ${_csr_macro_lint_depends})
        endif()
    endif()
endmacro(csr_add_quality_targets)

macro(csr_add_global_quality_targets)
    set(_uncrustify_targets)
    set(_uncrustify_transform_targets)
    set(_codecheck_targets)
    set(_coverity_targets)
    set(_lint_targets)

    # Technology targets
    foreach(_technology ${CSR_TECHNOLOGIES})
        string(TOLOWER ${_technology} _technology_lc)

        # Uncrustify
        get_property(_uncrustify_targets_tech GLOBAL PROPERTY "QUALITY_TARGETS_UNCRUSTIFY_${_technology}")
        if (_uncrustify_targets_tech)
            add_custom_target("${_technology_lc}-uncrustify")
            add_dependencies("${_technology_lc}-uncrustify" ${_uncrustify_targets_tech})
            list(APPEND _uncrustify_targets "${_technology_lc}-uncrustify")
        endif()
        get_property(_uncrustify_transform_targets_tech GLOBAL PROPERTY "QUALITY_TARGETS_UNCRUSTIFY_TRANSFORM_${_technology}")
        if (_uncrustify_transform_targets_tech)
            add_custom_target("${_technology_lc}-uncrustify_transform")
            add_dependencies("${_technology_lc}-uncrustify_transform" ${_uncrustify_transform_targets_tech})
            list(APPEND _uncrustify_transform_targets "${_technology_lc}-uncrustify_transform")
        endif()

        # Codecheck
        get_property(_codecheck_targets_tech GLOBAL PROPERTY "QUALITY_TARGETS_CODECHECK_${_technology}")
        if (_codecheck_targets_tech)
            add_custom_target("${_technology_lc}-codecheck")
            add_dependencies("${_technology_lc}-codecheck" ${_codecheck_targets_tech})
            list(APPEND _codecheck_targets "${_technology_lc}-codecheck")
        endif()

        # Coverity
        get_property(_coverity_targets_tech GLOBAL PROPERTY "QUALITY_TARGETS_COVERITY_${_technology}")
        if (_coverity_targets_tech)
            add_custom_target("${_technology_lc}-coverity")
            add_dependencies("${_technology_lc}-coverity" ${_coverity_targets_tech})
            list(APPEND _coverity_targets "${_technology_lc}-coverity")
        endif()

        # Lint
        get_property(_lint_targets_tech GLOBAL PROPERTY "QUALITY_TARGETS_LINT_${_technology}")
        if (_lint_targets_tech)
            add_custom_target("${_technology_lc}-lint")
            add_dependencies("${_technology_lc}-lint" ${_lint_targets_tech})
            list(APPEND _lint_targets "${_technology_lc}-lint")
        endif()
    endforeach()

    # Uncrustify (all)
    if (_uncrustify_targets)
        add_custom_target(uncrustify)
        add_dependencies(uncrustify ${_uncrustify_targets})
    endif()
    if (_uncrustify_transform_targets)
        add_custom_target(uncrustify_transform)
        add_dependencies(uncrustify_transform ${_uncrustify_transform_targets})
    endif()
    if (UNCRUSTIFY_EXECUTABLE AND UNCRUSTIFY_SUPPORTED AND PERL_EXECUTABLE AND P4_EXECUTABLE)
        add_custom_target(uncrustify_transform_opened
                          COMMAND "${PERL_EXECUTABLE}" "${CSR_TOOLS_ROOT}/uncrustify/uncrustify-transform.pl"
                                  --cmd ${UNCRUSTIFY_EXECUTABLE}
                                  --p4cmd ${P4_EXECUTABLE}
                                  --cfg "${CSR_TOOLS_ROOT}/uncrustify/uncrustify.transform.cfg"
                          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                          COMMENT "Uncrustifying: ${PROJECT_SOURCE_DIR}/..."
                          VERBATIM)
    endif()

    # Codecheck (all)
    if (_codecheck_targets)
        add_custom_target(codecheck)
        add_dependencies(codecheck ${_codecheck_targets})
    endif()

    # Coverity (all)
    if (_coverity_targets)
        add_custom_target(coverity)
        add_dependencies(coverity ${_coverity_targets})
        add_custom_target(coverity_clean
                          COMMAND "${CMAKE_COMMAND}" -E remove_directory
                                  "${PROJECT_BINARY_DIR}/coverity"
                          COMMAND "${CMAKE_COMMAND}" -E make_directory
                                  "${PROJECT_BINARY_DIR}/coverity"
                          VERBATIM)
        if (COVERITY_ANALYZE_EXECUTABLE AND
            COVERITY_FORMAT_EXECUTABLE AND
            COVERITY_MAKE_LIBRARY_EXECUTABLE)
            set(_coverity_analyze_options)
            if (COVERITY_ANALYZE_OPTIONS)
                string(REPLACE " " ";" _coverity_analyze_options ${COVERITY_ANALYZE_OPTIONS})
            endif()
            add_custom_target(coverity_analyze
                              COMMAND "${CMAKE_COMMAND}" -E remove -f
                                      "coverity/csr_tools_coverity_models.xmldb"
                              COMMAND "${COVERITY_MAKE_LIBRARY_EXECUTABLE}"
                                      --output-file "coverity/csr_tools_coverity_models.xmldb"
                                      "${PROJECT_SOURCE_DIR}/tools/coverity/csr_tools_coverity_models.c"
                              COMMAND "${COVERITY_ANALYZE_EXECUTABLE}"
                                      --dir coverity
                                      --user-model-file "coverity/csr_tools_coverity_models.xmldb"
                                      --force
                                      --wait-for-license
                                      ${_coverity_analyze_options}
                              COMMAND "${COVERITY_FORMAT_EXECUTABLE}"
                                      --dir coverity
                              COMMAND "${CMAKE_COMMAND}" -E "echo"
                                      "Results: ${PROJECT_BINARY_DIR}/coverity/c/output/errors/index.html"
                              WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                              VERBATIM)
        endif()
        if (COVERITY_COMMIT_EXECUTABLE AND
            COVERITY_SERVER AND
            COVERITY_DATA_PORT AND
            COVERITY_STREAM_NAME AND
            COVERITY_USER AND
            COVERITY_PASSWORD)
            add_custom_target(coverity_commit
                              COMMAND "${COVERITY_COMMIT_EXECUTABLE}"
                                       --dir coverity
                                       --host ${COVERITY_SERVER}
                                       --dataport ${COVERITY_DATA_PORT}
                                       --user ${COVERITY_USER}
                                       --password ${COVERITY_PASSWORD}
                                       --stream ${COVERITY_STREAM_NAME}
                                       --target ${CSR_PLATFORM}
                              WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                              VERBATIM)
        endif()
    endif()

    # Lint (all)
    if (_lint_targets)
        add_custom_target(lint)
        add_dependencies(lint ${_lint_targets})
    endif()
endmacro()

set(CSR_DEFAULT_UNCRUSTIFY_LEVEL 2)
set(CSR_DEFAULT_CODECHECK_LEVEL 4)
set(CSR_DEFAULT_COVERITY_LEVEL 1)
set(CSR_DEFAULT_LINT_LEVEL 6)

macro(csr_set_default_quality_levels)
    set(_csr_macro_action)
    set(_csr_macro_signature
            "csr_set_default_quality_levels([UNCRUSTIFY <level>]"
            "                               [CODECHECK <level>]"
            "                               [COVERITY <level>]"
            "                               [LINT <level>]")

    foreach(_currentArg ${ARGN})
        if ("${_currentArg}" STREQUAL "UNCRUSTIFY" OR
            "${_currentArg}" STREQUAL "CODECHECK" OR
            "${_currentArg}" STREQUAL "COVERITY" OR
            "${_currentArg}" STREQUAL "LINT")
            set(_csr_macro_action "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "UNCRUSTIFY")
            set(CSR_DEFAULT_UNCRUSTIFY_LEVEL "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "CODECHECK")
            set(CSR_DEFAULT_CODECHECK_LEVEL "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "COVERITY")
            set(CSR_DEFAULT_COVERITY_LEVEL "${_currentArg}")
        elseif (${_csr_macro_action} STREQUAL "LINT")
            set(CSR_DEFAULT_LINT_LEVEL "${_currentArg}")
        else()
            message(FATAL_ERROR "Parameter '${_currentArg}' incorrect\n"
                                ${_csr_macro_signature})
        endif()
    endforeach()
endmacro(csr_set_default_quality_levels)
