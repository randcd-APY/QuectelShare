###############################################################################
#
# Copyright (c) 2011-2014 Qualcomm Technologies International, Ltd. 
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
#   REVISION:      $Revision: #1 $
###############################################################################

###### Include Bt Version Numbers
include(${CSR_BT_ROOT}/ver.cmake)

###### Include the local configuration
set(CSR_BT_CONFIG_ROOT ${CSR_BT_ROOT}/config )

######## Copy to config header
configure_file(${CSR_BT_CONFIG_ROOT}/csr_bt_usr_config.h ${CSR_INC_GLOBAL}/csr_bt_usr_config.h COPYONLY)
configure_file(${CSR_BT_ROOT}/inc/csr_bt_config_global.h ${CSR_INC_GLOBAL}/csr_bt_config_global.h COPYONLY)

csr_add_subdirectory(config)
csr_add_subdirectory(inc)
csr_add_subdirectory(porting)
csr_add_subdirectory(profile_managers)
csr_add_subdirectory(applications)
csr_add_subdirectory(sdk)

# Build system install rules
csr_install_buildsystem()
