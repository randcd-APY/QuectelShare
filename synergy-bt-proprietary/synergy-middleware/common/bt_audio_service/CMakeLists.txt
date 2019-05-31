##############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
# REVISION:      $Revision: #1 $
###############################################################################

# Define the macro to use software cvc
#add_definitions("-DUSE_SOFTWARE_CVC")

include_directories(inc)

add_subdirectory(main)

add_subdirectory(bt_audio_hw)

add_subdirectory(util)

