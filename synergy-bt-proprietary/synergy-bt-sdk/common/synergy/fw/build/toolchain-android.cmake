##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

if (DEFINED "ENV{ANDROID_BUILD_PATHS}" AND DEFINED "ENV{ANDROID_TOOLCHAIN}" AND DEFINED "ENV{ANDROID_BUILD_TOP}")
    set (ANDROID_BUILD_TOP "$ENV{ANDROID_BUILD_TOP}")
    set (ANDROID_TOOLCHAIN "$ENV{ANDROID_TOOLCHAIN}")
    set (ANDROID_BUILD_PATHS "$ENV{ANDROID_BUILD_PATHS}")
elseif (DEFINED ANDROID_ROOT)
    set (ANDROID_BUILD_TOP "${ANDROID_ROOT}")
    set (ANDROID_TOOLCHAIN "${ANDROID_ROOT}/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin")
    set (ANDROID_BUILD_PATHS "${ANDROID_ROOT}/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin")
else()
    message (FATAL_ERROR "Building for Android requires that CMake can locate the Android source code\n"
                         "This can be achieved by one of two methods:\n"
                         " 1: Setting up the environment thought the Android tools\n"
                         "       android:> source build/envsetup.sh\n"
                         "       android:> set_stuff_for_environment\n"
                         " 2: Call CMake with a -DANDROID_ROOT=<location>\n"
                         " Method 1 is the recommended usage since method 2 resolves the location of the tools based on fixed path and is therefore less flexable")
endif()

set(CMAKE_SYSTEM_NAME                           Linux)

include(CMakeForceCompiler)

CMAKE_FORCE_C_COMPILER("${ANDROID_TOOLCHAIN}/arm-eabi-gcc" GNU)
CMAKE_FORCE_CXX_COMPILER("${ANDROID_TOOLCHAIN}/arm-eabi-gcc" GNU)

if (EXISTS ${CMAKE_C_COMPILER})
    execute_process(COMMAND ${CMAKE_C_COMPILER} "-v" OUTPUT_VARIABLE _csr_c_compiler_version ERROR_VARIABLE _csr_c_compiler_version)
    string (REGEX MATCH "version[ ]+[0-9]+.[0-9]+.[0-9]+" _csr_c_compiler_version ${_csr_c_compiler_version})
    string (REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" ANDROID_COMPILER_VERSION ${_csr_c_compiler_version})
else()
    message (FATAL_ERROR "Unable to locate the compiler at ${CMAKE_C_COMPILER}")
endif()

set(CMAKE_FIND_ROOT_PATH                        "${ANDROID_BUILD_TOP}/out/target/product/generic/obj"
                                                "${ANDROID_BUILD_TOP}/external/openssl")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM           NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY           ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE           ONLY)

set(ANDROID_LIBRARY_PATH                       "${ANDROID_BUILD_TOP}/out/target/product/generic/obj/lib")

include_directories(                            "${ANDROID_BUILD_TOP}/bionic/libc/arch-arm/include"
                                                "${ANDROID_BUILD_TOP}/bionic/libc/include"
                                                "${ANDROID_BUILD_TOP}/bionic/libstdc++/include"
                                                "${ANDROID_BUILD_TOP}/bionic/libc/kernel/common"
                                                "${ANDROID_BUILD_TOP}/bionic/libc/kernel/arch-arm"
                                                "${ANDROID_BUILD_TOP}/bionic/libm/include"
                                                "${ANDROID_BUILD_TOP}/bionic/libm/include/arm"
                                                "${ANDROID_BUILD_TOP}/bionic/libthread_db/include"
                                                "${ANDROID_BUILD_TOP}/bionic/libm/arm"
                                                "${ANDROID_BUILD_TOP}/bionic/libm"
                                                "${ANDROID_BUILD_TOP}/external/bluetooth/bluez/lib/"
                                                "${ANDROID_BUILD_TOP}/system/core/include")

set(CSR_PLATFORM_DEFAULT "ANDROID")
