###############################################################################
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

if(NOT EXCLUDE_CSR_BT_HOGH_MODULE AND CSR_BT_APP_USE_GATT_HOGH)
    csr_add_subdirectory(host)
endif()

if(CSR_BT_APP_USE_GATT_HOGD)
    csr_add_subdirectory(device)
endif()