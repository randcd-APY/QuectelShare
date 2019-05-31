###############################################################################
#
# Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_spp_app_ui_sef.c
    csr_bt_spp_app.c)

set(HDRS
	csr_bt_spp_app.h
	csr_bt_spp_strings.h
	csr_bt_spp_app_ui_sef.h
	csr_bt_spp_app_private_prim.h
	../inc/csr_bt_spp_app_task.h)

csr_add_library(csr_bt_app_spp
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                CONDITIONAL NOT EXCLUDE_CSR_BT_SPP_MODULE SOURCES ${SRCS}
                HEADERS ${HDRS}
                ALLOW_WARNINGS)
