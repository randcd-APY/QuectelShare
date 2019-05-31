##############################################################################
#
# Copyright (c) 2011-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

csr_install_directory(COMPONENT CSR_COMPONENT_FRW_BUILD Compiler)
#csr_add_subdirectory(Compiler)
csr_add_subdirectory(config)
csr_add_subdirectory(package)

set(FILES
    build_system.cmake
    build_system_java_class_list.cmake
    build_system_linux_kernel_module.cmake
    build_system_macros.cmake
    build_system_override.cmake
    build_system_quality.cmake
    build_system_tools.cmake
    build_system_tools_wireshark.cmake
    platform-ANDROID.cmake
    platform-BDB3.cmake
    platform-LINUX.cmake
    platform-WINDOWS.cmake
    platformlibs-ANDROID.cmake
    platformlibs-LINUX.cmake
    platformlibs-WINDOWS.cmake
    toolchain-android.cmake
    toolchain-clang.cmake
    toolchain-linux-cross.cmake
    toolchain-linux-cross-prima2.cmake
    toolchain-linux-cross-atlas6.cmake
    toolchain-realview-arm-2_2.cmake
    toolchain-realview-arm-3_0.cmake
    toolchain-realview-arm-3_1.cmake
    toolchain-realview-arm-4_0.cmake
    toolchain-realview-arm-4_1.cmake)

csr_add_files(COMPONENT CSR_COMPONENT_FRW_BUILD FILES ${FILES})

