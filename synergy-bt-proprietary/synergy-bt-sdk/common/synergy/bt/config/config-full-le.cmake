###############################################################################
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved.
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################


include(${CSR_BT_ROOT}/config/config-full.cmake)

csr_set_cache_parameter(CSR_BT_LE_ENABLE  VALUE ON)
csr_set_cache_parameter(EXCLUDE_CSR_BT_HOGH_MODULE VALUE OFF)
csr_set_cache_parameter(EXCLUDE_CSR_BT_PROX_SRV_MODULE VALUE OFF)
csr_set_cache_parameter(EXCLUDE_CSR_BT_THERM_SRV_MODULE VALUE OFF)
csr_set_cache_parameter(EXCLUDE_CSR_BT_HOGH_MODULE VALUE OFF)

csr_set_cache_parameter(CSR_BT_APP_USE_GATT VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_GENERIC_SERVER VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_LE_BROWSER VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_THERMC VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_THERMS VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_HOGD VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_PROXC VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_PROXS VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_RSCC VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_GATT_RSCS VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_PHDC_AG VALUE 1)
csr_set_cache_parameter(CSR_BT_APP_USE_PHDC_MGR VALUE 1)
