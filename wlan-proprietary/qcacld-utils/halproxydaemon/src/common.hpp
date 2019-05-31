/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*/

#define MAC_ADDR_LEN 6
#define MAC_OUI_LEN 3
#define ARGV_IFACE_INDEX 2
#define ARGV_CMDID_INDEX 3
#define MAX_WLAN_MAC_PKT_LENGTH 2304
#define MAX_SLEEP_DURATION 1200

int mac_addr_aton(u8 *mac_addr, const char *txt, size_t length);
int mac_addr_aton(u8 *mac_addr, const char *txt);
