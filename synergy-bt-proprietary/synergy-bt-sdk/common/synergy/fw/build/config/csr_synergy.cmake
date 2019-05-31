##############################################################################
#
# Copyright (c) 2010-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary. 
#
##############################################################################

####### Build
csr_define_cache_parameter(CSR_BACKWARDS_COMPATIBILITY_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable backwards compatibility (allow building against older versions)")

csr_define_cache_parameter(CSR_DUMP_FILES_INTO_SINGLE_DIR
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Dump all source files/headers into single directory")

csr_define_cache_parameter(CSR_PROFILING_SUPPORT
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Setting this to enable will compiler profiling if supported by the toolchain")

csr_define_cache_parameter(CSR_ALLOW_WARNINGS
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable this option to allow compiler warnings globally")

csr_define_cache_parameter(CSR_ALLOW_NON_ANSI
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable this option to allow non ANSI/ISO compiler language features globally")

csr_define_cache_parameter(CSR_PERFORM_BUILD
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Build libraries/executable/archives")

csr_define_cache_parameter(CSR_CODESIZE_LIB_OPTIONS
                           TYPE STRING
                           DEFAULT "--symbols"
                           DESCRIPTION "Codesize options for <lib>-codesize targets")

csr_define_cache_parameter(CSR_LINUX_KERNEL_DIR
                           TYPE STRING
                           DEFAULT "/lib/modules/${CMAKE_HOST_SYSTEM_VERSION}/build"
                           DESCRIPTION "The Linux kernel Module Symvers path")
						   
csr_define_cache_parameter(CSR_TARGET_PATHS
                           TYPE STRING
                           DEFAULT "/usr/lib"
                           DESCRIPTION "used to set the CMAKE_FIND_ROOT_PATH for target libraries")

csr_define_cache_parameter(CSR_PLATFORM_ARM
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "used to set the platform to ARM.This is 'ON' automatically for both Atlas6 or Prima2 builds")

csr_define_cache_parameter(CSR_HOST_PLATFORM
                           TYPE STRING
                           DEFAULT "MDM" STRINGS "MSM" "MDM" "ATLAS7"
                           DESCRIPTION "Select host platform in MSM, MDM, ATLAS7")

csr_define_cache_parameter(CSR_BUILD_TYPE
                           TYPE STRING
                           DEFAULT debug
                           DESCRIPTION "used to set build type to debug/release/relwithdebinfo/minsizerel/custom.These options control debug symbols and optimization level of binary generated")

####### Features
csr_define_cache_parameter(CSR_LOG_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Logging support")

csr_define_cache_parameter(CSR_BT_CHIP_TYPE
                           TYPE STRING
                           DEFAULT "QCA" STRINGS "BLUECORE" "QCA"
                           DESCRIPTION "Set BT chip type: BlueCore, QCA BT")

csr_define_cache_parameter(CSR_ENABLE_QCA_CHIP_DSP
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Eanble Kalimba DSP in QCA chip, such as Napier")

csr_define_cache_parameter(CSR_BOOT_QCA_CHIP_MODE
                           TYPE STRING
                           DEFAULT "EXTERNAL" STRINGS "INTERNAL" "EXTERNAL"
                           DESCRIPTION "Boot QCA BT in Synergy framework internally or externally")

csr_define_cache_parameter(CSR_QCA_CHIP_CTL_LOG_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Enable controller log for QCA chip, such as QCA6574")

csr_define_cache_parameter(CSR_ASYNC_LOG_TRANSPORT
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Async Log Transport support")

csr_define_cache_parameter(CSR_H4_TRANSPORT
                           TYPE STRING
                           DEFAULT "H4IBS" STRINGS "H4" "H4IBS" "H4DS"
                           DESCRIPTION "Select H4 trasport in H4, H4IBS, H4DS")
						   
csr_define_cache_parameter(CSR_ENABLE_SHUTDOWN
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enable scheduler shutdown support")

csr_define_cache_parameter(CSR_EXCEPTION_HANDLER
                           TYPE BOOL
                           DEFAULT ON
                           DESCRIPTION "Enables exception handling")

csr_define_cache_parameter(CSR_EXCEPTION_PANIC
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Exception should cause a panic")

csr_define_cache_parameter(CSR_CHIP_MANAGER_ENABLE
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Chip manager support")

csr_define_cache_parameter(CSR_CHIP_MANAGER_QUERY
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Chip manager query support"
                           DEPENDS CSR_CHIP_MANAGER_ENABLE)

csr_define_cache_parameter(CSR_CHIP_MANAGER_TEST
                           TYPE BOOL
                           DEFAULT OFF
                           DESCRIPTION "Chip manager test support"
                           DEPENDS CSR_CHIP_MANAGER_ENABLE)

if (CSR_PLATFORM STREQUAL BDB3)
    csr_define_cache_parameter(CSR_INSTRUMENTED_PROFILING_SERVICE
                               TYPE BOOL
                               DEFAULT OFF
                               DESCRIPTION "Instrumented profiling support")
endif()

if (DEFINED CSR_COMPONENT_BT AND DEFINED CSR_COMPONENT_WIFI)
    csr_define_cache_parameter(CSR_BLUETOOTH_WIFI_COEX
                               TYPE BOOL
                               DEFAULT OFF
                               DESCRIPTION "Software co-existance support")
endif()

if (DEFINED CSR_COMPONENT_BT)
    csr_define_cache_parameter(CSR_AMP_ENABLE
                               TYPE BOOL
                               DEFAULT OFF
                               DESCRIPTION "AMP Support")

    csr_define_cache_parameter(CSR_BT_LE_ENABLE
                               TYPE BOOL
                               DEFAULT ON
                               DESCRIPTION "Bluetooth Low Energy support")

    csr_define_cache_parameter(CSR_APP_AMP_FAKE
                               TYPE BOOL
                               DEFAULT OFF
                               DESCRIPTION "AMP - Fake PHY support")

    if (DEFINED CSR_COMPONENT_WIFI)
        csr_define_cache_parameter(CSR_APP_AMP_WIFI
                                   TYPE BOOL
                                   DEFAULT OFF
                                   DESCRIPTION "AMP - WiFi PHY support")
    endif()

    if (DEFINED CSR_COMPONENT_UWB)
        csr_define_cache_parameter(CSR_APP_AMP_UWB
                                   TYPE BOOL
                                   DEFAULT OFF
                                   DESCRIPTION "AMP - UWB PHY support")
    endif()
endif()

####### FTS
csr_define_cache_parameter(CSR_EXTERNAL_FTS_VER
                           TYPE STRING
                           DEFAULT "9.9.19.0"
                           DESCRIPTION "FTS Version")
