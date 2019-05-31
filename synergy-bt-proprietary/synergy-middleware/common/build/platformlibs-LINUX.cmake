##############################################################################
#
# Copyright (c) 2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################


find_package(Threads)
if (CMAKE_USE_PTHREADS_INIT)
    list(APPEND CONNX_SYSTEM_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif()

#find_package(ALSA)
#if (ALSA_FOUND)
#    list(APPEND CONNX_SYSTEM_LIBS ${ALSA_LIBRARY})
#endif(ALSA_FOUND)

list(APPEND CONNX_SYSTEM_LIBS "-lasound")