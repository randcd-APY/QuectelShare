/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WLAN_MSG_H_
#define __WLAN_MSG_H_

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_arr) (sizeof(_arr) / sizeof((_arr)[0]))
#endif

#define WLAN_MSG_WLAN_TP_IND        0x109
#define WLAN_MSG_WLAN_TP_TX_IND     0x10B

struct wlan_hdr {
	unsigned short type;
	unsigned short length;
};

#endif /* __WLAN_MSG_H_ */
