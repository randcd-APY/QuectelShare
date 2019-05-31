##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set (_csr_classes_list)
if (CSR_JAVA_OUTPUT_FILE AND CSR_JAVA_ARCHIVE_LOCATIONS)
    file (WRITE ${CSR_JAVA_OUTPUT_FILE} "")
    string(REPLACE ":" ";" _match ${CSR_JAVA_ARCHIVE_LOCATIONS})
    foreach (_csr_java_archive_location ${_match})
        file (GLOB_RECURSE _csr_java_classes "${_csr_java_archive_location}/*.class")
        foreach(_csr_java_class ${_csr_java_classes})
            file(RELATIVE_PATH _csr_java_class ${_csr_java_archive_location} ${_csr_java_class})
            list (FIND _csr_classes_list ${_csr_java_class} _index)
            if (_index EQUAL -1)
                list (APPEND _csr_classes_list ${_csr_java_class})
                file(APPEND ${CSR_JAVA_OUTPUT_FILE} "-C ${_csr_java_archive_location} ${_csr_java_class}\n")
            endif()
        endforeach()
    endforeach()
endif()
