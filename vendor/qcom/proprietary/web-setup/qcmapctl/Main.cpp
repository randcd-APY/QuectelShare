/**************************************************************
 * Copyright (C) 2017-2018 Qualcomm Technologies, Inc.
 * All rights reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/
#include <signal.h>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <qmi_cci_target_ext.h>
#include <QCMAP_Client.h>

#include "CommandParser.h"

enum CommandCode {
	CMD_ERROR = -1,
	CMD_WLAN_ENABLE,
	CMD_WLAN_DISABLE,
	CMD_WLAN_RESTART,
	CMD_WLAN_ACTIVATE,
	CMD_WLAN_CONFIG,
	CMD_WLAN_STA,
	CMD_WLAN_AP,
	CMD_WLAN_BOOT_ON,
	CMD_WLAN_BOOT_OFF,
	CMD_WLAN_RELOAD_AP,
	CMD_WLAN_RELOAD_STA,
};

// typedef to avoid having to pass the template parameters all the time
typedef CommandParser<CommandCode, CMD_ERROR> Parser;

Parser commandList = Parser::Map
	("wlan", Parser::Map // command with sub commands
		("enable", CMD_WLAN_ENABLE)
		("disable", CMD_WLAN_DISABLE)
		("activate", CMD_WLAN_ACTIVATE)
		("restart", CMD_WLAN_RESTART)
		("config", CMD_WLAN_CONFIG)
		("sta", CMD_WLAN_STA)
		("ap", CMD_WLAN_AP)
		("boot", Parser::Map
			("on", CMD_WLAN_BOOT_ON)
			("off", CMD_WLAN_BOOT_OFF)
		)
		("reload", Parser::Map
			("ap", CMD_WLAN_RELOAD_AP)
			("sta", CMD_WLAN_RELOAD_STA)
		)
	)
	;

struct PrintIp {
	uint32_t ip;
	PrintIp(uint32_t ip): ip(ip) {}
};

inline PrintIp printIp(uint32_t ip) {
	return PrintIp(ip);
}

namespace std {
	template<typename CharT, typename Traits>
	inline basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& st, PrintIp ip) {
		st << ((ip.ip >> 24) & 0xFF)
			<< '.' << ((ip.ip >> 16) & 0xFF)
			<< '.' << ((ip.ip >> 8) & 0xFF)
			<< '.' << ((ip.ip >> 0) & 0xFF);
		return st;
	}
}

void qcmapStatusCb(qmi_client_type /*user_handle*/, unsigned int /*msg_id*/, void* /*ind_buf*/, unsigned int /*ind_buf_len*/, void* /*ind_cb_data*/) {
}

boost::shared_ptr<QCMAP_Client> createQcmapClient() {
	boost::shared_ptr<QCMAP_Client> client = boost::make_shared<QCMAP_Client>(qcmapStatusCb);
	if (client->qmi_qcmap_msgr_handle == 0) {
		std::cerr << "Failed to setup client" << std::endl;
		client.reset();
	}

	return client;
}

bool enableMobileap(boost::shared_ptr<QCMAP_Client> client) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	bool result = client->EnableMobileAP(&qmiErrNum);
	if (!result) {
		std::cerr << "Failed to enable MobileAP: " << std::hex << qmiErrNum << std::dec << std::endl;
	}
	return result;
}

bool enableWlan(boost::shared_ptr<QCMAP_Client> client, bool enable) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	bool result = (enable ? client->EnableWLAN(&qmiErrNum) : client->DisableWLAN(&qmiErrNum));
	if (!result) {
		std::cerr << "Failed to enable/disable Wlan: " << std::hex << qmiErrNum << std::dec << std::endl;
	}
	return result;
}

bool activateWlan(boost::shared_ptr<QCMAP_Client> client) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	bool result = client->ActivateWLAN(&qmiErrNum);
	if (!result) {
		std::cerr << "Failed to activate WLAN: " << std::hex << qmiErrNum << std::dec << std::endl;
	}
	return result;
}

bool getWlanConfig(boost::shared_ptr<QCMAP_Client> client) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	qcmap_msgr_wlan_mode_enum_v01 wlanMode;
	qcmap_msgr_access_profile_v01 guestApAccessProfile;
	qcmap_msgr_station_mode_config_v01 stationConfig;

	bool result = client->GetWLANConfig(&wlanMode, &guestApAccessProfile, &stationConfig, &qmiErrNum);
	if (!result) {
		std::cerr << "Failed to get WLAN mode: " << std::hex << qmiErrNum << std::dec << std::endl;
		return false;
	}

	std::cout << "mode: ";
	switch (wlanMode) {
		case QCMAP_MSGR_WLAN_MODE_AP_V01: std::cout << "ap"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_AP_V01: std::cout << "ap-ap"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_STA_V01: std::cout << "ap-sta"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01: std::cout << "ap-ap-ap"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01: std::cout << "ap-ap-sta"; break;
		case QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01: std::cout << "sta"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01: std::cout << "ap-ap-ap-ap"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_STA_BRIDGE_V01: std::cout << "ap-sta bridge"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_BRIDGE_V01: std::cout << "ap-ap-sta bridge"; break;
		case QCMAP_MSGR_WLAN_MODE_STA_ONLY_BRIDGE_V01: std::cout << "sta bridge"; break;
		case QCMAP_MSGR_WLAN_MODE_AP_P2P_V01: std::cout << "ap-p2p"; break;
		case QCMAP_MSGR_WLAN_MODE_STA_P2P_V01: std::cout << "sta-p2p"; break;

		case QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01:
		case QCMAP_MSGR_WLAN_MODE_ENUM_MAX_ENUM_VAL_V01:
		default:
			std::cout << "unknown"; break;
	}
	std::cout << std::endl;

	switch (stationConfig.conn_type) {
		case QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01:
			std::cout << "ip type: dynamic" << std::endl;
			break;
		case QCMAP_MSGR_STA_CONNECTION_STATIC_V01:
			std::cout << "ip type: static" << std::endl;
			std::cout << "ip: " << printIp(stationConfig.static_ip_config.ip_addr) << std::endl;
			std::cout << "netmask: " << printIp(stationConfig.static_ip_config.netmask) << std::endl;
			std::cout << "gateway: " << printIp(stationConfig.static_ip_config.gw_ip) << std::endl;
			std::cout << "dns: " << printIp(stationConfig.static_ip_config.dns_addr) << std::endl;
			break;
		case QCMAP_MSGR_STA_CONNECTION_ENUM_MIN_ENUM_VAL_V01:
		case QCMAP_MSGR_STA_CONNECTION_ENUM_MAX_ENUM_VAL_V01:
		default:
			std::cout << "ip: unknown" << std::endl;
			break;

	}

	return true;
}

bool setWlanModeSta(boost::shared_ptr<QCMAP_Client> client, bool sta) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	qcmap_msgr_wlan_mode_enum_v01 wlanMode = (sta ? QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 : QCMAP_MSGR_WLAN_MODE_AP_V01);
	qcmap_msgr_access_profile_v01 guestApAccessProfile = QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
	qcmap_msgr_station_mode_config_v01 stationConfig
		= { QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01, {0, 0, 0, 0}, 0 };

	bool result = client->SetWLANConfig(wlanMode, guestApAccessProfile, stationConfig, &qmiErrNum);
	if (!result) {
		std::cerr << "Failed to set WLAN mode: " << std::hex << qmiErrNum << std::dec << std::endl;
	}
	return result;
}

bool enableWlanBoot(boost::shared_ptr<QCMAP_Client> client, bool enable) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	qcmap_msgr_bootup_flag_v01 mobileapEnable = QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01;
	qcmap_msgr_bootup_flag_v01 wlanEnable = (enable ? QCMAP_MSGR_ENABLE_ON_BOOT_V01 : QCMAP_MSGR_DISABLE_ON_BOOT_V01);

	bool result = client->SetQCMAPBootupCfg(mobileapEnable, wlanEnable, &qmiErrNum);
	if (!result) {
		std::cerr << "Failed to enable/disable WLAN boot: " << std::hex << qmiErrNum << std::dec << std::endl;
	}
	return result;
}

bool reloadApConfig(boost::shared_ptr<QCMAP_Client> client) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	qcmap_msgr_activate_hostapd_ap_enum_v01 apType = QCMAP_MSGR_PRIMARY_AP_V01;
	qcmap_msgr_activate_hostapd_action_enum_v01 actionType = QCMAP_MSGR_HOSTAPD_RESTART_V01;

	bool result = client->ActivateHostapdConfig(apType, actionType, &qmiErrNum);
	if (!result) {
		std::cerr << "Failed to reload AP config: " << std::hex << qmiErrNum << std::dec << std::endl;
	}
	return result;
}

bool reloadStaConfig(boost::shared_ptr<QCMAP_Client> client) {
	qmi_error_type_v01 qmiErrNum = QMI_ERR_NONE_V01;

	bool result = client->ActivateSupplicantConfig(&qmiErrNum);
	if (!result) {
		std::cerr << "Failed to reload STA config: " << std::hex << qmiErrNum << std::dec << std::endl;
	}
	return result;
}

bool processLine(boost::shared_ptr<QCMAP_Client> client, std::vector<std::string> args) {
	CommandCode code = commandList.match(args);
	if (code == CMD_ERROR) {
		if (commandList.index == 0) {
			if (commandList.index == args.size()) {
				std::cerr << "Missing command" << std::endl;
			}
			else {
				std::cerr << "Unknown command " << args[0] << std::endl;
			}
		}
		else {
			if (commandList.index == args.size()) {
				std::cerr << "Missing argument";
			}
			else {
				std::cerr << "Unknown argument '" << args[commandList.index] << "'";
			}
			std::cerr << " for command '" << args[0];
			for (size_t i = 1; i < commandList.index; i++) {
				std::cerr << ' ' << args[i];
			}
			std::cerr << "'" << std::endl;
		}
		return false;
	}
	switch (code) {
	case CMD_ERROR:
		// already handled, here just to satisfy the compiler
		return 0;
	case CMD_WLAN_ENABLE:
		return enableWlan(client, true);
	case CMD_WLAN_DISABLE:
		return enableWlan(client, false);
	case CMD_WLAN_RESTART:
		return enableWlan(client, false) && enableWlan(client, true);
	case CMD_WLAN_ACTIVATE:
		return activateWlan(client);
	case CMD_WLAN_CONFIG:
		return getWlanConfig(client);
	case CMD_WLAN_STA:
		return setWlanModeSta(client, true);
	case CMD_WLAN_AP:
		return setWlanModeSta(client, false);
	case CMD_WLAN_BOOT_ON:
		return enableWlanBoot(client, true);
	case CMD_WLAN_BOOT_OFF:
		return enableWlanBoot(client, false);
	case CMD_WLAN_RELOAD_AP:
		return reloadApConfig(client);
	case CMD_WLAN_RELOAD_STA:
		return reloadStaConfig(client);
	}

	// not reachable
	return false;
}

std::vector<std::string> parseArguments(std::string str) {
	// space and tabs are separator, \ is for escaping
	boost::escaped_list_separator<std::string::value_type> sep("\\", " \t", "");
	boost::tokenizer<boost::escaped_list_separator<std::string::value_type> > tokens(str, sep);
	return std::vector<std::string>(tokens.begin(), tokens.end());
}

int client(int argc, char *argv[]) {
	boost::shared_ptr<QCMAP_Client> client = createQcmapClient();
	if (!client) { return 1; }
	if (!enableMobileap(client)) { return false; }

	if (argc > 1) {
		std::vector<std::string> args;
		args.reserve(argc);
		for (int i = 1; i < argc; i++) {
			args.push_back(std::string(argv[i]));
		}

		return processLine(client, args);
	}
	else {
		while (!std::cin.fail()) {
			std::string str;
			std::cerr << "> ";
			std::getline(std::cin, str);
			if (str.empty()) {
				continue;
			}
			else if ((str == "quit") || (str == "exit")) {
				break;
			}

			if (processLine(client, parseArguments(str))) {
				std::cerr << "OK" << std::endl;
			}
		}
	}

	return 0;
}

int daemon() {
	sigset_t sigSet;
	sigemptyset(&sigSet);
	sigaddset(&sigSet, SIGINT);
	sigaddset(&sigSet, SIGTERM);
	sigaddset(&sigSet, SIGHUP);
	sigaddset(&sigSet, SIGUSR1);
	sigaddset(&sigSet, SIGPIPE);
	sigprocmask(SIG_BLOCK, &sigSet, NULL);

	boost::shared_ptr<QCMAP_Client> client = createQcmapClient();
	if (!client) { return 1; }
	if (!enableMobileap(client)) { return 1; }

	siginfo_t sigInfo;
	int sig;
	do {
		sig = sigwaitinfo(&sigSet, &sigInfo);
		if (sig < 0) {
			if (errno == EINTR) {
				continue;
			}

			int saveErrno = errno;
			std::cerr << "Failed in sigwaitinfo: " << saveErrno << " - " << strerror(saveErrno) << std::endl;
			break;
		}
	} while ((sig != SIGTERM) && (sig != SIGINT));

	return 0;
}

int main(int argc, char *argv[]) {
	if ((argc == 2) && (strcmp(argv[1], "-d") == 0)) {
		return daemon();
	}
	else {
		return client(argc, argv);
	}
}
