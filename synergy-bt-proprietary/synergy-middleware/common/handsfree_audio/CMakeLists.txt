##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

# Define the macro to use bt audio service

if(USE_MDM_PLATFORM)
add_definitions("-DUSE_BT_AUDIO_SERVICE")
endif()

set(SRCS
    connx_hf_audio.c 
    connx_hfg_audio.c 
    connx_handsfree_audio_common.c)

set(HDRS
    connx_handsfree_audio_common.h)

add_library(connx_handsfree_audio ${SRCS} ${HDRS})
