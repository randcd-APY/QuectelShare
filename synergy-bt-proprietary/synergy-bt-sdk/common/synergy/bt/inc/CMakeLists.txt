##############################################################################
#
# Copyright (c) 2011-2013 Qualcomm Technologies International, Ltd. 
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
##############################################################################

set(SRCS)

set(HDRS)

set(PUBLIC_HDRS
    bluetooth.h
    csr_bt_bluestack_types.h
    csr_bt_bluestack_config.h
    csr_bt_common.h
    csr_bt_config_global.h
    csr_bt_hand_coded.h
    csr_bt_profiles.h
    csr_bt_result.h
    csr_bt_tasks.h
    csr_bt_types.h
    csr_bt_util.h
    hci_prim.h
    csr_bt_internet_primitives.h
    csr_bt_addr.h
    csr_bt_uuids.h
    csr_bt_sdc_support.h
    csr_bt_dhcp_prim.h
    csr_bt_icmp_prim.h
    csr_bt_ip_prim.h
    csr_bt_log_version.h
    csr_bt_lsl_prim.h
    csr_bt_ipcp.h
    csr_bt_ppp_prim.h
    csr_bt_ppp_lib.h
    csr_bt_udp_prim.h)

csr_add_headers(COMPONENT CSR_COMPONENT_BT ${PUBLIC_HDRS})
