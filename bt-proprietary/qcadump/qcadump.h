/*
 *
 *  qcadump - Captures vendor specific events for firmware crash
 *
 *  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef __QCADUMP_H
#define __QCADUMP_H

#define MAX_PKT_LEN	1500

#ifndef AF_BLUETOOTH
#define AF_BLUETOOTH    31
#define PF_BLUETOOTH    AF_BLUETOOTH
#endif

#define BTPROTO_HCI     1
#define SOL_HCI         0

#define BT_COMMAND_PKT         0x01
#define BT_ACLDATA_PKT         0x02
#define BT_SCODATA_PKT         0x03
#define BT_EVENT_PKT           0x04
#define BT_VENDOR_PKT          0xff

#define HCI_FILTER      2

#define MAX_BUF_SIZE    204800

#define LAST_SEQUENCE_NUM       0xffff

#define VENDOR_PKT_SIZE		256
#define VENDOR_FIRST_PKT_HDR_SIZE       12
#define VENDOR_OTHER_PKT_HDR_SIZE       8

struct sockaddr_hci {
	sa_family_t     hci_family;
	unsigned short  hci_dev;
	unsigned short  hci_channel;
};

struct frame {
	void    *ptr;
	uint32_t len;
};

struct hci_filter {
	uint32_t type_mask;
	uint32_t event_mask[2];
	uint16_t opcode;
};

#endif /* __QCADUMP_H */
