/**************************************************************
 * Copyright (C) 2017-2018 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/
#include <string>
#include <vector>
#include <errno.h>
#include <iostream>
#include <sstream>

#include <net/if.h>
#include <linux/nl80211.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <netlink/netlink.h>

#include <boost/assign/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// NL80211_BSS_CAPABILITY flags. Taken from wpa_supplicant's driver.h
#define IEEE80211_CAP_IBSS 0x0002
#define IEEE80211_CAP_PRIVACY 0x0010

// IE types
static const uint8_t IE_TYPE_RSN = 48;
static const uint8_t IE_TYPE_VENDOR = 221;

// IE OUI's
static const uint8_t RSN_IE_OUI[3] = { 0x00, 0x0f, 0xac };
static const uint8_t WPA_IE_OUI[3] = { 0x00, 0x50, 0xf2 };
static const uint8_t WPA_IE_OUI_EX[4] = { 0x00, 0x50, 0xf2, 0x01 };

static const char * const NL80211_GENL_FAMILY = "nl80211";

#define SKIP_BYTESTREAM(ptr, len, numBytes) \
	ptr += (numBytes); \
	len -= (numBytes);

#define LogError(ARGS...) fprintf(stderr,ARGS); fputc('\n', stderr);

struct ScanInfo {
	ScanInfo() : rssi(0), caps(0), wpa(0) {
		memset(bssid, 0, sizeof(bssid));
	}

	std::string ssid;
	uint8_t bssid[6];
	int rssi;
	uint16_t caps;
	int wpa;
};

static int noSeqCallback(struct nl_msg *msg, void *arg) {
	return NL_OK;
}

static int ackCallback(struct nl_msg *msg, void *arg) {
	int *err = (int*)arg;
	*err = 0;
	return NL_STOP;
}

static int finishCallback(struct nl_msg *msg, void *arg) {
	int *err = (int*)arg;
	*err = 0;
	return NL_SKIP;
}

static int errorCallback(struct sockaddr_nl *nladdr, struct nlmsgerr *err, void *arg) {
	int *err_arg = (int*)arg;
	*err_arg = err->error;
	return NL_SKIP;
}

static int triggerScanCallback(struct nl_msg *msg, void *data) {
	bool *scanDone = (bool *) data;
	struct genlmsghdr *gnlh = (struct genlmsghdr *) nlmsg_data(nlmsg_hdr(msg));

	if ((gnlh->cmd == NL80211_CMD_SCAN_ABORTED) || (gnlh->cmd == NL80211_CMD_NEW_SCAN_RESULTS)) {
		*scanDone = true;
	}

	return NL_SKIP;
}

static void parseIE(const uint8_t *ieData, size_t ieLength, ScanInfo &scanInfo) {
	scanInfo.wpa = 0;
	while (ieLength >= 2) {
		uint8_t type = ieData[0];
		size_t length = (size_t) ieData[1];
		if (ieLength < length) {
			break;
		}
		SKIP_BYTESTREAM(ieData, ieLength, 2);
		if (type == 0) {
			for (size_t i = 0; i < length; i++) {
				scanInfo.ssid += (char) ieData[i];
			}
		} else if (type == IE_TYPE_RSN) {
			scanInfo.wpa |= 2;  // WPA2
		} else if ((type == IE_TYPE_VENDOR)) {
			size_t ouiLen = sizeof(WPA_IE_OUI_EX);
			if ((length >= ouiLen) && (memcmp(ieData, WPA_IE_OUI_EX, ouiLen) == 0)) {
				scanInfo.wpa |= 1; // WPA
			}
		}
		SKIP_BYTESTREAM(ieData, ieLength, length);
	}
}

static int dumpScanCallback(struct nl_msg *msg, void *data) {
	ScanInfo scanInfo;
	std::vector<ScanInfo> *scanList = (std::vector<ScanInfo> *) data;
	struct genlmsghdr *gnlh = (struct genlmsghdr *) nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
	struct nlattr *bss[NL80211_BSS_MAX + 1];
	static struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {
		{       0, 0, 0 },     // __NL80211_BSS_INVALID
		{       0, 0, 0 },     // NL80211_BSS_BSSID
		{ NLA_U32, 0, 0 },     // NL80211_BSS_FREQUENCY
		{ NLA_U64, 0, 0 },     // NL80211_BSS_TSF
		{ NLA_U16, 0, 0 },     // NL80211_BSS_BEACON_INTERVAL
		{ NLA_U16, 0, 0 },     // NL80211_BSS_CAPABILITY
		{       0, 0, 0 },     // NL80211_BSS_INFORMATION_ELEMENTS
		{ NLA_U32, 0, 0 },     // NL80211_BSS_SIGNAL_MBM
		{  NLA_U8, 0, 0 },     // NL80211_BSS_SIGNAL_UNSPEC
		{ NLA_U32, 0, 0 },     // NL80211_BSS_STATUS
		{ NLA_U32, 0, 0 },     // NL80211_BSS_SEEN_MS_AGO
		{       0, 0, 0 },     // NL80211_BSS_BEACON_IES
	};

	nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb_msg[NL80211_ATTR_BSS]) {
		LogError(("[dumpScanCallback] bss not found"));
		return NL_SKIP;
	}

	if (nla_parse_nested(bss, NL80211_BSS_MAX, tb_msg[NL80211_ATTR_BSS], bss_policy)) {
		LogError(("[dumpScanCallback] error parsing nested attributes"));
		return NL_SKIP;
	}

	uint8_t *addr = (uint8_t *) nla_data(bss[NL80211_BSS_BSSID]);
	memcpy(scanInfo.bssid, addr, sizeof(scanInfo.bssid));

	if (bss[NL80211_BSS_SIGNAL_MBM]) {
		scanInfo.rssi = (int) nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM]) / 100;
	}

	if (bss[NL80211_BSS_INFORMATION_ELEMENTS]) {
		uint8_t *ieData = (uint8_t *) nla_data(bss[NL80211_BSS_INFORMATION_ELEMENTS]);
		size_t ieLength = nla_len(bss[NL80211_BSS_INFORMATION_ELEMENTS]);

		parseIE(ieData, ieLength, scanInfo);
	} // NOTE: if there are no IE's, wpa_supplicant still lists the AP (It's either WEP or not, based on cap flag)

	if (bss[NL80211_BSS_CAPABILITY]) {
		scanInfo.caps = nla_get_u16(bss[NL80211_BSS_CAPABILITY]);
	}

	if (!scanInfo.ssid.empty()) {
		scanList->push_back(scanInfo);
	}

	return NL_SKIP;
}

static int familyCallback(struct nl_msg *msg, void *arg) {
	int *groupId = (int *)arg;
	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = (struct genlmsghdr *) nlmsg_data(nlmsg_hdr(msg));

	nla_parse(tb_msg, CTRL_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb_msg[CTRL_ATTR_MCAST_GROUPS]) {
		return NL_SKIP;
	}

	int remaining = nla_len(tb_msg[CTRL_ATTR_MCAST_GROUPS]);
	struct nlattr *nla = (struct nlattr *)nla_data(tb_msg[CTRL_ATTR_MCAST_GROUPS]);
	for (; nla_ok(nla, remaining); nla = nla_next(nla, &remaining)) {
		struct nlattr *tb_group[CTRL_ATTR_MCAST_GRP_MAX + 1];

		nla_parse(tb_group, CTRL_ATTR_MCAST_GRP_MAX, (nlattr *) nla_data(nla), nla_len(nla), NULL);

		if ((!tb_group[CTRL_ATTR_MCAST_GRP_NAME]) || (!tb_group[CTRL_ATTR_MCAST_GRP_ID])) {
			continue;
		}
		const char *groupName = (const char *) nla_data(tb_group[CTRL_ATTR_MCAST_GRP_NAME]);
		size_t groupNameLen = nla_len(tb_group[CTRL_ATTR_MCAST_GRP_NAME]);
		if (strncmp(groupName, (const char *) "scan", groupNameLen) != 0) {
			continue;
		}
		*groupId = nla_get_u32(tb_group[CTRL_ATTR_MCAST_GRP_ID]);
		break;
	}

	return NL_SKIP;
}

static int s_cbData = 0;

static bool setMulticastMembership(struct nl_sock *sock) {
	bool success = false;

	nl_msg *msg = nlmsg_alloc();
	if (!msg) {
		LogError(("[setMulticastMembership] failed to alloc message"));
		return success;
	}

	int status = 0;
	int groupId = -1;
	int nlCtrlId = genl_ctrl_resolve(sock, "nlctrl");
	if (nlCtrlId < 0) {
		LogError(("[setMulticastMembership] failed to resolve generic netlink family name"));
		goto nla_put_failure;
	}
	genlmsg_put(msg, 0, 0, nlCtrlId, 0, 0, CTRL_CMD_GETFAMILY, 0);

	NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, NL80211_GENL_FAMILY);

	status = nl_send_auto_complete(sock, msg);
	if (status < 0) {
		LogError(("[setMulticastMembership] failed to send command: %d"), status);
		goto nla_put_failure;
	}

	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, familyCallback, &groupId);

	s_cbData = 1;
	do {
		status = nl_recvmsgs_default(sock);
	} while ((s_cbData > 0) && (status == 0));

	if ((s_cbData < 0) || (status < 0)) {
		LogError(("[] failed to get family: status = %d, error = %d-%s"), status, s_cbData, nl_geterror(s_cbData));
		goto nla_put_failure;
	}

	nl_socket_add_membership(sock, groupId);

	success = true;

nla_put_failure:
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_DEFAULT, NULL, NULL);
	nlmsg_free(msg);
	return success;
}


static bool sortByRssi(const ScanInfo &scanInfo1, const ScanInfo &scanInfo2) {
	if (scanInfo1.rssi != scanInfo2.rssi) {
		return scanInfo1.rssi > scanInfo2.rssi;
	}
	if (scanInfo1.ssid != scanInfo2.ssid) {
		return scanInfo1.ssid > scanInfo2.ssid;
	}
	for (size_t i = 0; i < sizeof(scanInfo1.bssid); i++) {
		if (scanInfo1.bssid[i] != scanInfo2.bssid[i]) {
			return scanInfo1.bssid[i] > scanInfo2.bssid[i];
		}
	}
	return true;
}

static bool performWiFiScan(struct nl_sock *sock, int index, std::vector<ScanInfo> &scanList) {
	int res = genl_connect(sock);
	if (res < 0) {
		LogError(("[performWiFiScan] failed to connect: %d - %s"), res, nl_geterror(res));
		return false;
	}

	nl_socket_modify_err_cb(sock, NL_CB_CUSTOM, errorCallback, &s_cbData);
	nl_socket_modify_cb(sock, NL_CB_FINISH, NL_CB_CUSTOM, finishCallback, &s_cbData);
	nl_socket_modify_cb(sock, NL_CB_ACK, NL_CB_CUSTOM, ackCallback, &s_cbData);

	int family = genl_ctrl_resolve(sock, NL80211_GENL_FAMILY);
	if (family < 0) {
		LogError(("[performWiFiScan] family %s not found"), NL80211_GENL_FAMILY);
		return false;
	}

	struct nl_msg *ssids = nlmsg_alloc();
	if (!ssids) {
		LogError(("[performWiFiScan] failed to alloc ssids"));
		return false;
	}

	nl_msg *msg = nlmsg_alloc();
	if (!msg) {
		LogError(("[performWiFiScan] failed to alloc message"));
		nlmsg_free(ssids);
		return false;
	}

	bool scanDone = false;

	bool success = setMulticastMembership(sock);
	if (!success) {
		goto nla_put_failure;
	}
	int status;
	genlmsg_put(msg, 0, 0, family, 0, 0, NL80211_CMD_TRIGGER_SCAN, 0);
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, index);
	NLA_PUT(ssids, 1, 0, "");
	nla_put_nested(msg, NL80211_ATTR_SCAN_SSIDS, ssids);

	status = nl_send_auto_complete(sock, msg);
	if (status < 0) {
		LogError(("[performWiFiScan] failed to send command: %d"), status);
		goto nla_put_failure;
	}
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, triggerScanCallback, &scanDone);
	nl_socket_modify_cb(sock, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, noSeqCallback, NULL);
	s_cbData = 1;
	do {
		status = nl_recvmsgs_default(sock);
	} while ((s_cbData > 0) && (status == 0));
	if ((s_cbData < 0) || (status < 0)) {
		LogError(("[performWiFiScan] failed to trigger scan: status = %d, error = %d-%s"), status, s_cbData, nl_geterror(s_cbData));
		goto nla_put_failure;
	}
	do {
		status = nl_recvmsgs_default(sock);
	} while ((!scanDone) && (status == 0) && (s_cbData >= 0));
	if ((s_cbData < 0) || (status < 0)) {
		LogError(("[performWiFiScan] failed waiting for scan: status = %d, error = %d-%s"), status, s_cbData, nl_geterror(s_cbData));
		goto nla_put_failure;
	}

	genlmsg_put(msg, 0, 0, family, 0, NLM_F_DUMP, NL80211_CMD_GET_SCAN, 0);
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, index);

	status = nl_send_auto_complete(sock, msg);
	if (status < 0) {
		LogError(("[performWiFiScan] failed to send command: %d"), status);
		goto nla_put_failure;
	}
	scanList.clear();
	status = 0;
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, dumpScanCallback, &scanList);
	s_cbData = 1;
	do {
		status = nl_recvmsgs_default(sock);
	} while ((s_cbData > 0) && (status == 0));
	if ((s_cbData < 0) || (status < 0)) {
		LogError(("[performWiFiScan] failed to get scan results: status = %d, error = %d-%s"), status, s_cbData, nl_geterror(s_cbData));
		goto nla_put_failure;
	}

	// Sort scanlist
	std::sort(scanList.begin(), scanList.end(), sortByRssi);

	success = true;
nla_put_failure:
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_DEFAULT, NULL, NULL);
	nlmsg_free(msg);
	nlmsg_free(ssids);
	return success;
}

static void printJsonScanlist(const std::vector<ScanInfo> &scanList) {
	boost::property_tree::ptree array;
	std::vector<ScanInfo>::const_iterator it = scanList.begin();
	std::vector<ScanInfo>::const_iterator end = scanList.end();
	for ( ; it != end; ++it) {
		boost::property_tree::ptree item;
		item.put("ssid", it->ssid);
		item.put("mode", (it->caps & IEEE80211_CAP_IBSS) ? "Ad-Hoc": "Master");

		std::stringstream bssid;
		for (size_t i = 0; i < 6; i++) {
			if (i > 0) {
				bssid << ":";
			}
			bssid << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (unsigned int) it->bssid[i];
		}
		item.put("bssid", bssid.str());
		item.put("signal", it->rssi);

		boost::property_tree::ptree encryption;

		encryption.put("wep", ((it->wpa == 0) && (it->caps & IEEE80211_CAP_PRIVACY)) ? "true" : "false");
		encryption.put("wpa", it->wpa);
		item.add_child("encryption", encryption);

		array.push_back(std::make_pair("", item));
	}

	boost::property_tree::ptree pt;
	pt.add_child("scanlist", array);
	boost::property_tree::json_parser::write_json(std::cout, pt);
}

int main(int argc, char *argv[]) {
	const char *interfaceName = "wlan0";
	if (argc > 1) {
		interfaceName = argv[1];
	}
	unsigned int index = if_nametoindex(interfaceName);

	if (index == 0) {
		LogError(("Error getting interface index for %s: %d-%s"), interfaceName, errno, strerror(errno));
		return 1;
	}

	nl_sock *sock = nl_socket_alloc();
	if (!sock) {
		LogError(("[main] create socket failed"));
		return 1;
	}

	std::vector<ScanInfo> scanList;
	performWiFiScan(sock, index, scanList);
	printJsonScanlist(scanList);

	nl_socket_free(sock);
	return 0;
}

