###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

# Define the macro to enable BT snoop log
if (ENABLE_BTSNOOP_LOG)
add_definitions("-DENABLE_BTSNOOP_LOG")
endif()

if (NOT EXCLUDE_BT_TEST)
add_subdirectory(bt_test)
endif()

if (NOT EXCLUDE_BTDIAG)
add_subdirectory(btdiag)
endif()
