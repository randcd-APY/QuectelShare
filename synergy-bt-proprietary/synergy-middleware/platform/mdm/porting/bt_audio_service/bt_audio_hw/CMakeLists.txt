##############################################################################
#
# Copyright (c) 2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS
    bt_audio_hw.c
    hf_audio_transfer.c 
    hfg_audio_transfer.c)

#if (USE_BT_AUDIO_ALSA)
    list(APPEND SRCS
         pcm_alsa.c
	 amix_alsa.c)
#endif()

add_library(bt_audio_hw ${SRCS} ${HDRS})