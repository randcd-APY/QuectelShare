/*
 * Copyright (c) 2014-2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WLAN_MSG_H_
#define __WLAN_MSG_H_

#include <linux/netlink.h>
#ifdef ANDROID
#include <linux/if.h>
#else
#include <net/if.h>
#endif


#define WLAN_MSG_MAX_PAYLOAD   256
#define WLAN_MSG_FAMILY  NETLINK_USERSOCK
#define WLAN_MSG_MCAST_GRP_ID  0x01

#define WLAN_MSG_WLAN_STATUS_IND    0x106
#define WLAN_MSG_WLAN_VERSION_IND   0x107
#define WLAN_MSG_WLAN_TP_IND        0x109
#define WLAN_MSG_RPS_ENABLE_IND     0x10A
#define WLAN_MSG_WLAN_TP_TX_IND     0x10B


#define WLAN_MSG_MAX_SSID_LEN    32
#define WLAN_MSG_MAX_BSSID_LEN   6
#define WLAN_MSG_MAX_STR_LEN     16
#define WLAN_MSG_MAX_NUM_CHAN    128
#define WLAN_MSG_COUNTRY_CODE_LEN 3
#define WLAN_SVC_IFACE_NUM_QUEUES 6


#define WLAN_MSG_BASE     0x10

#define WLAN_MSG_SVC      (WLAN_MSG_BASE + 0x0A)

enum wlan_tp_level {
        WLAN_SVC_TP_NONE,
        WLAN_SVC_TP_LOW,
        WLAN_SVC_TP_MEDIUM,
        WLAN_SVC_TP_HIGH,
};

struct wlan_hdr {
	unsigned short type;
	unsigned short length;
};

struct wlan_status_data {
	uint8_t lpss_support;
	uint8_t is_on;
	uint8_t vdev_id;
	uint8_t is_connected;
	int8_t rssi;
	uint8_t ssid_len;
	uint8_t country_code[WLAN_MSG_COUNTRY_CODE_LEN];
	uint32_t vdev_mode;
	uint32_t freq;
	uint32_t num_channels;
	uint8_t channel_list[WLAN_MSG_MAX_NUM_CHAN];
	uint8_t ssid[WLAN_MSG_MAX_SSID_LEN];
	uint8_t bssid[WLAN_MSG_MAX_BSSID_LEN];
};

struct wlan_version_data {
	uint32_t chip_id;
	char chip_name[WLAN_MSG_MAX_STR_LEN];
	char chip_from[WLAN_MSG_MAX_STR_LEN];
	char host_version[WLAN_MSG_MAX_STR_LEN];
	char fw_version[WLAN_MSG_MAX_STR_LEN];
};

/**
 * struct wlan_rps_data - structure to send RPS info to cnss-daemon
 * @ifname:         interface name for which the RPS data belongs to
 * @num_queues:     number of rx queues for which RPS data is being sent
 * @cpu_map_list:   array of cpu maps for different rx queues supported by
                    the wlan driver
 *
 * The structure specifies the format of data exchanged between wlan
 * driver and cnss-daemon. On receipt of the data, cnss-daemon is expected
 * to apply the 'cpu_map' for each rx queue belonging to the interface 'ifname'
 */
struct wlan_rps_data {
	char ifname[IFNAMSIZ];
	uint16_t num_queues;
	uint16_t cpu_map_list[WLAN_SVC_IFACE_NUM_QUEUES];
};


#endif /* __WLAN_MSG_H_ */
