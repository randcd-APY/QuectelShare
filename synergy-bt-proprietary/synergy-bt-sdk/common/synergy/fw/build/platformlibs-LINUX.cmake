##############################################################################
#
# Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

find_package(Threads)
if (CMAKE_USE_PTHREADS_INIT)
    list(APPEND CSR_SYSTEM_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif()

include(CheckLibraryExists)
check_library_exists("rt" "clock_gettime" "" RT_HAS_CLOCK_GETTIME)
#if(RT_HAS_CLOCK_GETTIME)
    list(APPEND CSR_SYSTEM_LIBS "-lrt")
#endif(RT_HAS_CLOCK_GETTIME)

#Cmake is not able to find if clock_gettime exists inside cross-compiled librt.
if (${CSR_PLATFORM_ARM})
    list(APPEND CSR_SYSTEM_LIBS "-lrt")
endif()

#find_package(SDL)
#if (SDL_FOUND)
#    list(APPEND CSR_SYSTEM_LIBS ${SDL_LIBRARY})
#    include_directories(${SDL_INCLUDE_DIR})
#endif()

#find_package(OpenSSL)
#if (OPENSSL_FOUND)
#    list(APPEND CSR_SYSTEM_LIBS ${OPENSSL_LIBRARIES})
#    include_directories(${OPENSSL_INCLUDE_DIR})
#endif()
