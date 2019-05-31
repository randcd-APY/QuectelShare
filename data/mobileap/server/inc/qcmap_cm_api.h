#ifndef _QCMAP_CM_API_H_
#define _QCMAP_CM_API_H_

/******************************************************************************

                           QCMAP_CM_API.H

******************************************************************************/

/******************************************************************************

  @file    qcmap_cm_api.h
  @brief   Mobile AP Connection Manager Lib API

  DESCRIPTION
  Header file for Mobile AP Connection Manager Lib.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/11/12   gk         9x25
10/26/12   cp         Added support for Dual AP and different types of NAT.
12/19/12   sb         Added support for RNDIS/ECM USB tethering
02/27/13   cp         Added support for deprecating of prefix when switching
                      betweenstation mode and WWAN mode.
04/17/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
06/12/13   sg         Added DHCP Reservation Feature
01/11/14   sr         Added support for connected devices information in SoftAP
03/27/14   cp         Added support to DUN+SoftAP.
02/05/14   pm         Added log message macros
01/05/15   rk         qtimap offtarget support.
******************************************************************************/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include <glib.h>
#include <stdbool.h>
#include "comdef.h"
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <linux/if_addr.h>
#include <netinet/ether.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netinet/icmp6.h>
#include "qcmap_firewall_util.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "qmi_client.h"
#include "dsi_netctrl.h"
#include "ds_util.h"
#include "ds_list.h"
#include "data_system_determination_v01.h"
#include "network_access_service_v01.h"

#define QCMAP_CM_QMI_TIMEOUT_VALUE     15000
#define QCMAP_CM_QMI_SCM_TIMEOUT_VALUE    90000
#define QCMAP_COEX_TIMEOUT_VALUE       1000
#define QCMAP_COEX_MAX_RETRY           5
#define MAX_COMMAND_STR_LEN 500
#define DATA_SIZE 512
#define IPV4_ADDR_LEN 4
#define IPV6_ADDR_LEN 16
#define NA_PACKET_SIZE 32
#define DSI_PROFILE_3GPP2_OFFSET (1000)
#define DSI_PROFILE_NUM_MAX      (9999)
#define DSI_IP_FAMILY_4   "IP"
#define DSI_IP_FAMILY_6   "IPV6"
#define DSI_IP_FAMILY_4_6 "IPV4V6"
#define QCMAP_DEFAULT_DSS_INIT_TIME    6
#define QCMAP_DSI_UDS_FILE "/var/run/qcmap_dsi_uds_file"
#define QCMAP_CMDQ_UDS_FILE "/var/run/qcmap_cmdq_uds_file"
#define QCMAP_NAS_UDS_FILE "/var/run/qcmap_nas_uds_file"
#define QCMAP_DSD_UDS_FILE "/var/run/qcmap_dsd_uds_file"
#define QCMAP_QMI_SERVICE_UDS_FILE "/var/run/qcmap_qmi_service_file"
#define QCMAP_TIMER_UDS_FILE "/var/run/qcmap_timer_uds_file"
#define QCMAP_STA_UDS_FILE "/var/run/qcmap_sta_uds_file"
#define QCMAP_SCM_UDS_FILE "/var/run/qcmap_scm_uds_file"
#define QCMAP_NL_UDS_FILE "/var/run/qcmap_nl_uds_file"

#define QCMAP_PMIP_FILE "/var/run/qcmap_pmip_file"
#define PMIP_QCMAP_FILE "/var/run/pmip_qcmap_file"
#define PMIP_QCMAP_IND_FILE "/var/run/pmip_qcmap_ind_file"

#define IPA_CFG_FILE_PATH "/data/misc/ipa/ipa_config.txt"
#define IPA_XML_PATH   "/etc/data/ipa/IPACM_cfg.xml"
#define AUTO_CONNECT_TIMER 1
#define QCMAP_MSGR_INTF_LEN 20
#define QCMAP_EPOCH_TIME_LEN 16

#define WLAN0 "wlan0"
#define WLAN1 "wlan1"
#define WLAN2 "wlan2"
#define WLAN3 "wlan3"

#define P2P_DEFAULT_IFACE "p2p0"

#define ETH0 "eth0"
#define BRIDGE_IFACE "bridge0"
#define ALL_BRIDGES "bridge+"

#define MAX_CMD_SIZE 100
#define GATEWAY_URL "mobileap.qualcomm.com"
#define DEFAULT_SIP_SERVER "abcd.com"

/* Default network config */
//#define AR6003 "ar6003" // refer gerrit::1586737
//#define AR6004 "ar6004" // refer gerrit::1586737
/*Default CDT Config*/
#define CDT_DUAL_WIFI 3

/*CDT config for SDX24  < refer go/cdt for values > */
#define CDT_DUAL_WIFI_MTP_CHIRON 4 /* for MTP platform */
#define CDT_DUAL_WIFI_CDP_CHIRON 5 /* for CDP platform */

/*Platform type*/
#define PLATFORM_MTP "MTP"
#define PLATFORM_CDP "Surf"
#define PLATFORM_STRING_LENGTH 5

/* WIFI iDriver/Firmware Init Delay Micro seconds */
#define WIFI_DEV_INIT_DELAY  50000
#define WIFI_DEV_INIT_DELAYS_MAX 5000000
#define DHCP_LEASE_TIME 43200 /*iLease time in seconds */
#define MIN_DHCP_LEASE 120 /*Lease time in seconds */
#define MAX_LAN_CLIENTS 40

/* For per client stats feature, IPACM expects ethernet
 * LAN category to be ODU to differentiate between RNDIS/ECM
 * and Ethernet. For info, by default ethernet mode is ODU
 * category in IPACM_cfg.xml.
 */
#define ETH_LAN_CATEGORY "ODU"
#define LAN_CATEGORY "LAN"
#define WAN_CATEGORY "WAN"

#define BRIDGE_MODE "BRIDGE"
#define ROUTER_MODE "ROUTER"

#define QCMAP_HOSTAPDCLI_MAX_RETRIES 15
#define QCMAP_HOSTAPDCLI_TIMEOUT_US 100000 /*100 milisecond*/
#define QCMAP_HOSTAPD_TIMEOUT_US 100000 /*100 milisecond*/

#define QCMAP_FIREWALL_CONFIG             "/etc/data/mobileap_firewall.xml"
#define QCMAP_DEFAULT_CONFIG              "/etc/data/mobileap_cfg.xml"
#define HOSTAPD_ENTROPY                   "/data/entropy_file"
#define HOSTAPD_ENTROPY_SSID2             "/data/entropy_file1"
#define HOSTAPD_CFG_PATH_SSID2            "/etc/misc/wifi/hostapd-wlan1.conf"
#define HOSTAPD_ENTROPY_SSID3             "/data/entropy_file2"
#define HOSTAPD_CFG_PATH_SSID3            "/etc/misc/wifi/hostapd-wlan2.conf"
#define HOSTAPD_ENTROPY_SSID4             "/data/entropy_file3"
#define HOSTAPD_CFG_PATH_SSID4            "/etc/misc/wifi/hostapd-wlan3.conf"

#define STA_MODE_HOSTAPD_CFG_PATH         "/etc/misc/wifi/sta_mode_hostapd.conf"
#define HOSTAPD_CFG_PATH                  "/etc/misc/wifi/hostapd.conf"
#define SCM_CFG_PATH                      "/etc/misc/wifi/scm.conf"
#define SCM_CTRL_PATH                     "/usr/bin/scm"
#define STA_INTERFACE_CTRL_PATH           "/usr/bin/QCMAP_StaInterface"

#define DDCLIENT_PID_FILE "/var/run/ddclient.pid"

#define P2P_CFG_PATH                      "/etc/misc/wifi/p2p_supplicant.conf"
#define P2P_PID_PATH                      "/var/run/wpa_supplicant_p2p0_pid.pid"

#define QCMAP_HOST_PATH "/data/hosts"

#define QCMAP_ROUTE_TABLE_PATH  "/etc/data/iproute2/rt_tables"
#define QCMAP_ROUTE_TABLE_TMP_PATH  "/tmp/rt_tables"

#define QCMAP_DYNDNS_SERVER_URL "members.dyndns.org"
#define QCMAP_DYNDNS_PROTOCOL "dyndns2"
#define QCMAP_DDNS_CFG_PATH "/etc/data/ddclient.conf"
#define QCMAP_DDNS_CACHE_PATH "/var/run/ddclient.cache"
#define DDNS_DEFAULT_LOGIN "user_defined"
#define DDNS_DEFAULT_PASSWORD "user_defined"
#define DDNS_DEFAULT_HOSTNAME "mobileap.dyndns.org"
#define DDNS_DEFAULT_TIMEOUT 60

#define WLAN_DEFAULT_DELAY 1

#define SUPPLICANT_CFG_PATH "/etc/misc/wifi/wpa_supplicant.conf"
#define WPA_CTRL_PATH "/var/run/wpa_supplicant"
#define QCMAP_ERI_CONFIG "/data/mobileap_eri_config.bin"

#define SOCKSV5_DEFAULT_CONF "/etc/data/qti_socksv5_conf.xml"
#define SOCKSV5_DEFAULT_AUTH "/etc/data/qti_socksv5_auth.xml"

#define RNDIS_IFACE "rndis0"
#define ECM_IFACE "ecm0"
#define QCMAP_CM_LOG(...)                         \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define QCMAP_CM_LOG_FUNC_ENTRY()  \
    QCMAP_CM_LOG                   \
    (                              \
        "Entering function %s\n",  \
        __FUNCTION__               \
    )

#define QCMAP_CM_LOG_FUNC_EXIT()   \
    QCMAP_CM_LOG                   \
    (                              \
        "Exiting function %s\n",   \
        __FUNCTION__               \
    )

#define QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(profile) \
    LOG_MSG_INFO1("Entering function %s, Profile=%d", __FUNCTION__, profile, 0)

#define Cradle_TAG                      "Cradle"
#define EnableIPV6_TAG                  "EnableIPV6"
#define EnableIPV4_TAG                  "EnableIPV4"
#define MobileAPWanCfg_TAG              "MobileAPWanCfg"
#define EriConfig_TAG                   "EriConfig"
#define FirstPreferredBackhaul_TAG      "FirstPreferredBackhaul"
#define SecondPreferredBackhaul_TAG     "SecondPreferredBackhaul"
#define ThirdPreferredBackhaul_TAG      "ThirdPreferredBackhaul"
#define FourthPreferredBackhaul_TAG     "FourthPreferredBackhaul"
#define FifthPreferredBackhaul_TAG      "FifthPreferredBackhaul"
#define EthBackhaul_TAG                 "EthBackhaul"
#define PacketStats_TAG                 "PacketStats"
#define IPAPacketStats_TAG              "LANStats"

/* Router/Bridge Config flags */
#define BridgeCfg_TAG                   "BridgeCfg"
#define StationModeCfg_TAG              "StationModeCfg"
#define STAModeConnType_TAG             "STAModeConnType"
#define STAModeStaticIPAddr_TAG         "StaticIPAddr"
#define STAModeStaticConfigDNSAddr_TAG  "StaticConfigDNSAddr"
#define STAModeStaticConfigGWAddr_TAG   "StaticConfigGWAddr"
#define STAModeStaticConfigNetMask_TAG  "StaticConfigNetMask"
#define Cfg_TAG                         "Config"
#define ProfileHandle_TAG               "Profile"
#define Roaming_TAG                     "Roaming"
#define TECH_TAG                        "TECH"
#define V4_UMTS_PROFILE_INDEX_TAG       "V4_UMTS_PROFILE_INDEX"
#define V4_CDMA_PROFILE_INDEX_TAG       "V4_CDMA_PROFILE_INDEX"
#define V6_UMTS_PROFILE_INDEX_TAG       "V6_UMTS_PROFILE_INDEX"
#define V6_CDMA_PROFILE_INDEX_TAG       "V6_CDMA_PROFILE_INDEX"
#define IPFamily_TAG                    "IPFamily"
#define TECH_ANY_TAG                    "ANY"
#define TECH_3GPP_TAG                   "3GPP"
#define TECH_3GPP2_TAG                  "3GPP2"
#define DefaultSIPServerConfigType_TAG  "DefaultSIPServerConfigType"
#define DefaultSIPServerConfig_TAG      "DefaultSIPServerConfig"
#define FQDN_TAG                        "FQDN"
#define IP_TAG                          "IP"
#define PrefixDelegation_TAG            "PrefixDelegation"
#define GatewayURL_TAG                  "GatewayURL"
#define IPAOffload_TAG                  "IPAOffload"

/* DDNS server */
#define DDNSCFG_TAG                      "DDNSCfg"
#define DDNS_Enable_TAG                  "EnableDDNS"
#define DDNS_Server_TAG                  "DDNSServer"
#define DDNS_Protocol_TAG                "DDNSProtocol"
#define DDNS_Login_TAG                   "DDNSLogin"
#define DDNS_Password_TAG                "DDNSPassword"
#define DDNS_Hostname_TAG                "DDNSHostname"
#define DDNS_Timeout_TAG                 "DDNSTimeout"

#define HostAPDEntropy_TAG              "HostAPDEntropy"
#define STAModeHostAPDCfg_TAG           "STAModeHostAPDCfg"
/*Tiny Proxy Entries*/
#define TINYPROXYCFG_TAG                "TinyProxyCfg"
#define TINY_PROXY_Enable_TAG           "EnableTinyProxy"
/* LAN Config Entries */
#define WlanMode_TAG                    "WlanMode"
#define WlanRestartDelay_TAG            "WlanRestartDelay"
#define GuestAPCfg_TAG                  "GuestAPCfg"
#define GuestAP2Cfg_TAG                 "GuestAP2Cfg"
#define GuestAP3Cfg_TAG                 "GuestAP3Cfg"
#define EnableBridgeMode_TAG            "MobileAPSTABridgeEnable"

/* VLAN */
#define VLAN_ID_TAG                     "VlanID"

#define MobileAPLanCfg_TAG              "MobileAPLanCfg"
#define Enable_TAG                      "Enable"
#define Module_TAG                      "Module"
#define Wlan_Gpio_TAG                   "WlanGpioNum"
#define DevMode_TAG                     "DevMode"
#define HostAPDCfg_TAG                  "HostAPDCfg"
#define APIPAddr_TAG                    "APIPAddr"
#define SubNetMask_TAG                  "SubNetMask"
#define AccessProfile_TAG               "AccessProfile"
#define EnableDHCPServer_TAG            "EnableDHCPServer"
#define DHCPCfg_TAG                     "DHCPCfg"
#define DHCPStartIP_TAG                 "StartIP"
#define DHCPEndIP_TAG                   "EndIP"
#define DHCPLeaseTime_TAG               "LeaseTime"
#define EnableSupplicant_TAG            "EnableSupplicant"
#define SupplicantCfg_TAG               "SupplicantCfg"
#define AP_TAG                          "AP"
#define STA_ONLY_TAG                    "STA"
#define AP_AP_TAG                       "AP-AP"
#define AP_AP_AP_TAG                    "AP-AP-AP"
#define AP_STA_TAG                      "AP-STA"
#define AP_AP_STA_TAG                   "AP-AP-STA"
#define AP_AP_AP_AP_TAG                 "AP-AP-AP-AP"
#define AP_P2P_TAG                      "AP-P2P"
#define STA_P2P_TAG                     "STA-P2P"
#define FULL_TAG                        "FULL"
#define INTERNET_TAG                    "INTERNETONLY"
#define DataPathOpt_Tag                 "DataPathOpt"

#define DHCPReservationRecord_TAG       "DHCPReservationRecord"
#define ClientReservedIP_TAG            "ClientReservedIP"
#define ClientName_TAG                  "ClientName"
#define ClientMACAddr_TAG               "ClientMACAddr"
#define AutoConnect_TAG                 "AutoConnect"
#define Reservation_TAG                 "Reservation"
#define IPPassthroughCfg_TAG            "IPPassthroughCfg"
#define IPPassthroughEnable_TAG         "IPPassthroughEnable"
#define IPPassthroughDeviceType_TAG     "IPPassthroughDeviceType"
#define IPPassthroughHostName_TAG       "IPPassthroughHostName"
#define IPPassthroughMacAddr_TAG        "IPPassthroughMacAddr"

/* IP Passthrough configuration needed for IPACM. */
#define IPPassthroughFlag_TAG           "IPPassthroughFlag"
#define IPPassthroughMode_TAG           "IPPassthroughMode"

/* Subnet configuration needed for IPACM. */
#define PrivateSubnetFlag_TAG           "IPACMPrivateSubnet"
#define SubnetFlag_TAG                  "Subnet"
#define SubnetAddress_TAG               "SubnetAddress"
#define SubnetMask_TAG                  "SubnetMask"

/* Debug Tags */
#define SUPPLICANT_DEBUG_TAG            "SupplicantCmdAppend"
#define HOSTAPD_DEBUG_TAG               "HostapdCmdAppend"
#define HOSTAPD_GUEST_DEBUG_TAG         "GuestHostapdCmdAppend"
#define HOSTAPD_GUEST_2_DEBUG_TAG       "Guest2HostapdCmdAppend"
#define HOSTAPD_GUEST_3_DEBUG_TAG       "Guest3HostapdCmdAppend"
#define P2P_SUPPLICANT_DEBUG_TAG        "P2PSupplicantCmdAppend"


/* QCMAP bootup flags*/
#define MobileAPBootUp_TAG             "MobileAPBootUpCfg"
#define MobileAPEnable_TAG             "MobileAPEnableAtBootup"
#define WLANEnable_TAG                 "WLANEnableAtBootup"


/* IPA ETH Tags */
#define odu_cfg_TAG                    "ODUCFG"
#define mode_TAG                       "Mode"
#define Bridge_TAG                     "bridge"
#define Router_TAG                     "router"

/* IPA Iface Tags */
#define IPACM_TAG                       "IPACM"
#define IPACMIface_TAG                  "IPACMIface"
#define Iface_TAG                       "Iface"
#define Name_TAG                        "Name"
#define WAN_TAG                         "Wan"
#define Category_TAG                    "Category"
#define Mode_TAG                        "Mode"
#define WLANMode_TAG                    "WlanMode"
#define WLAN_Full_TAG                   "full"
#define WLAN_Internet_TAG               "internet"

/*Firewall Tags*/
#define FirewallEnabled_TAG             "FirewallEnabled"
#define FirewallPktsAllowed_TAG         "FirewallPktsAllowed"
#define UPNPInboundPinhole_TAG          "UPNPInboundPinhole"

#define Firewall_TAG                    "Firewall"
#define FirewallStartPort_TAG           "FirewallStartPort"
#define FirewallEndPort_TAG             "FirewallEndPort"
#define FirewallProtocol_TAG            "FirewallProtocol"
#define FirewallHandle_TAG              "FirewallHandle"
#define FirewallDirection_TAG           "FirewallDirection"

#define MobileAPFirewallCfg_TAG         "MobileAPFirewallCfg"
#define IPFamily_TAG                    "IPFamily"
#define IPV4SourceAddress_TAG           "IPV4SourceAddress"
#define IPV4SourceIPAddress_TAG         "IPV4SourceIPAddress"
#define IPV4SourceSubnetMask_TAG        "IPV4SourceSubnetMask"
#define PinholeEntry_TAG                "PinholeEntry"


#define IPV4DestinationAddress_TAG "IPV4DestinationAddress"
#define IPV4DestinationIPAddress_TAG "IPV4DestinationIPAddress"
#define IPV4DestinationSubnetMask_TAG "IPV4DestinationSubnetMask"

#define IPV4TypeOfService_TAG "IPV4TypeOfService"
#define TOSValue_TAG "TOSValue"
#define TOSMask_TAG "TOSMask"

#define IPV4NextHeaderProtocol_TAG "IPV4NextHeaderProtocol"

#define IPV6SourceAddress_TAG "IPV6SourceAddress"
#define IPV6SourceIPAddress_TAG "IPV6SourceIPAddress"
#define IPV6SourcePrefix_TAG "IPV6SourcePrefix"

#define IPV6DestinationAddress_TAG "IPV6DestinationAddress"
#define IPV6DestinationIPAddress_TAG "IPV6DestinationIPAddress"
#define IPV6DestinationPrefix_TAG "IPV6DestinationPrefix"

#define IPV6TrafficClass_TAG "IPV6TrafficClass"
#define TrfClsValue_TAG "TrfClsValue"
#define TrfClsMask_TAG "TrfClsMask"

#define IPV6NextHeaderProtocol_TAG "IPV6NextHeaderProtocol"

#define TCPSource_TAG "TCPSource"
#define TCPSourcePort_TAG "TCPSourcePort"
#define TCPSourceRange_TAG "TCPSourceRange"

#define TCPDestination_TAG "TCPDestination"
#define TCPDestinationPort_TAG "TCPDestinationPort"
#define TCPDestinationRange_TAG "TCPDestinationRange"

#define UDPSource_TAG "UDPSource"
#define UDPSourcePort_TAG "UDPSourcePort"
#define UDPSourceRange_TAG "UDPSourceRange"

#define UDPDestination_TAG "UDPDestination"
#define UDPDestinationPort_TAG "UDPDestinationPort"
#define UDPDestinationRange_TAG "UDPDestinationRange"

#define ICMPType_TAG "ICMPType"
#define ICMPCode_TAG "ICMPCode"

#define ESP_TAG "ESP"
#define ESPSPI_TAG "ESPSPI"

#define TCP_UDPSource_TAG "TCP_UDPSource"
#define TCP_UDPSourcePort_TAG "TCP_UDPSourcePort"
#define TCP_UDPSourceRange_TAG "TCP_UDPSourceRange"

#define TCP_UDPDestination_TAG "TCP_UDPDestination"
#define TCP_UDPDestinationPort_TAG "TCP_UDPDestinationPort"
#define TCP_UDPDestinationRange_TAG "TCP_UDPDestinationRange"

#define NetDev_TAG   "NetDev"
#define DefaultProfile_TAG "DefaultProfile"

/*PMIPv6 Tags*/
#define PMIPV6_TAG                          "PMIPv6"
#define EnablePMIPmode_TAG                  "EnablePMIPmode"
#define PMIPmodeType_TAG                    "PMIPmodeType"
#define LMAv6Address_TAG                    "LMAv6Address"
#define LMAv4Address_TAG                    "LMAv4Address"
#define PMIPV4WorkMode_TAG                  "PMIPV4WorkMode"
#define PMIPV4SecRouterDMNPPrefix_TAG       "PMIPV4SecRouterDMNPPrefix"
#define PMIPV4SecRouterdmnpPrefixLen_TAG    "PMIPV4SecRouterdmnpPrefixLen"
#define PMIPTunnelType                      "PMIPTunnelType"
#define EnablePMIPDebugmode                 "EnablePMIPDebugmode"
#define PMIPMobileNodeIdentifierType        "PMIPMobileNodeIdentifierType"
#define PMIPMobileNodeIdentifierString      "PMIPMobileNodeIdentifierString"
#define PMIPServiceSelectionString          "PMIPServiceSelectionString"


/* PPP Generic Driver Init Delay Micro seconds */
#define PPP_DEV_INIT_DELAY  50000
#define PPP_DEV_INIT_DELAYS_MAX 5000000

#define USB_GW_IP_OFFSET 1
#define USB_IP_OFFSET 2
#define DHCP_IP_OFFSET 4
#define NUM_USB_ADDR 4
#define MIN_DHCP_ADDR_RANGE 7

/* Default Timeout Values. */
#define QCMAP_NAT_ENTRY_DEFAULT_GENERIC_TIMEOUT 200
#define QCMAP_NAT_ENTRY_DEFAULT_ICMP_TIMEOUT 30
#define QCMAP_NAT_ENTRY_DEFAULT_TCP_TIMEOUT 3600
#define QCMAP_NAT_ENTRY_DEFAULT_UDP_TIMEOUT 60
#define QCMAP_NAT_ENTRY_MIN_TIMEOUT 30
#define QCMAP_MIN_INITIAL_PKT_THRESHOLD 3

#define MAC_NULL_STR "00:00:00:00:00:00" /*MAC Null String*/
#define QCMAP_PROCESS_KILL_WAIT_MS  50000 /* 50 mili seconds*/
#define QCMAP_PROCESS_KILL_RETRY  40
#define QCMAP_BRIDGE_MAX_RETRY 20
#define QCMAP_BRIDGE_MAX_TIMEOUT_MS 500 /* Micro seconds */
#define QCMAP_RESET_CONFIG_TIMEOUT 5 /* 5 seconds*/
#define QCMAP_QTI_MSG_TIMEOUT_S 2
#define DSI_INITED 1
#define QCMAP_HOSTAPD_START_POLL_MAX_COUNT 300 /*3Sec considering WLAN 11AC ACS*/
#define QCMAP_HOSTAPD_END_POLL_MAX_COUNT 30    /* 300 ms */
#define QCMAP_HOSTAPD_POLL_DELAY_MS 10000 /*10 ms or 10000 us*/
#define QCMAP_ASSOC_WLAN_TO_BRIDGE_MAX_RETRIES 200
#define QCMAP_ASSOC_WLAN_TO_BRIDGE_TIMEOUT_US 100000 /*100 milisecond*/
#define LL_SUBNET_MASK         "255.255.255.0"
#define RANDOM_MAC_KEY  3

#define STATE_FDB_TABLE "/data/statefdbtable.txt"
#define PERMANENT "permanent"
#define QCMAP_RETRY_FDB_TIMEOUT_US 100000 /*100 milisecond*/
#define QCMAP_MAX_FDB_RETRY 10

/* Timer values for WLAN-STA events */
#define QCMAP_WLAN_STA_ASSOC_TIMEOUT 60 /* Seconds */
#define QCMAP_WLAN_STA_DHCP_TIMEOUT  60 /* Seconds */
#define QCMAP_BRIDGE_PROXY_TIMEOUT     60 /* Seconds */

/*Max retry count for sta association on recieving sta disconnect in ap-sta bridge mode*/
#define QCMAP_STA_BRDIGE_MAX_RETRY   5

/*Address lengths*/
#define INET_ADDRSTRLEN        16
#define INET6_ADDRSTRLEN       46

#define QCMAP_DDNS_PROTOCOL_LEN 100
#define QCMAP_MSGR_MAX_DDNS_CONF 2
#define DDNS_ENABLED 1
#define DDNS_DISABLED 0

#define QCMAP_SSR_DSI_INIT_MAX_RETRY 3
#define QCMAP_SSR_DSI_INIT_MAX_TIMEOUT 1 /* Micro seconds */

#define QCMAP_PER_CLIENT_STATS_DISCONNECT_RETRY 60
#define QCMAP_PER_CLIENT_STATS_DISCONNECT_RETRY_TIMEOUT 1

/*Service tags */
#define System_TAG                      "system"
#define Debug_TAG                       "MobileAPDbg"
#define MobileAPCfg_TAG                 "MobileAPCfg"
#define ConcurrentMobileAPCfg_TAG       "ConcurrentMobileAPCfg"
#define MobileAPSrvcCfg_TAG             "MobileAPSrvcCfg"

/* Daemon Services Congig Entries*/
#define UPNP_TAG                        "UPnP"
#define DLNA_TAG                        "DLNA"
#define MDNS_TAG                        "MDNS"
#define DLNAWhitelistingAllow_TAG       "DLNAWhitelistingAllow"
#define DLNAWhitelistIPList_TAG         "DLNAWhitelistIPList"
#define WhitelistedIP_TAG               "WhitelistedIP"

/* NAT Config Tags */
#define MobileAPNatCfg_TAG              "MobileAPNatCfg"
#define PortFwding_TAG                  "PortFwding"
#define PortFwdingPrivateIP_TAG         "PortFwdingPrivateIP"
#define PortFwdingPrivatePort_TAG       "PortFwdingPrivatePort"
#define PortFwdingGlobalPort_TAG        "PortFwdingGlobalPort"
#define PortFwdingProtocol_TAG          "PortFwdingProtocol"
#define NatEntryGenericTimeout_TAG         "NatEntryGenericTimeout"
#define NatEntryICMPTimeout_TAG            "NatEntryICMPTimeout"
#define NatEntryTCPEstablishedTimeout_TAG  "NatEntryTCPEstablishedTimeout"
#define NatEntryUDPTimeout_TAG             "NatEntryUDPTimeout"
#define DmzIP_TAG                       "DmzIP"
#define EnableIPSECVpnPassthrough_TAG   "EnableIPSECVpnPassthrough"
#define EnablePPTPVpnPassthrough_TAG    "EnablePPTPVpnPassthrough"
#define EnableL2TPVpnPassthrough_TAG    "EnableL2TPVpnPassthrough"
#define EnableWebserverWWANAccess_TAG   "EnableWebserverWWANAccess"
#define ALGCfg_TAG                      "ALGCfg"
#define EnableRTSPAlg_TAG               "EnableRTSPAlg"
#define EnableSIPAlg_TAG                "EnableSIPAlg"
#define NATType_TAG                     "NATType"
#define SYMMETRIC_TAG                   "SYM"
#define PRC_TAG                         "PRC"
#define FULLCONE_TAG                    "FC"
#define ARC_TAG                         "ARC"
#define Initial_Pkt_Limit_TAG           "Initial_Pkt_Limit"
#define Enable_SOCKSv5_Proxy_TAG        "EnableSOCKSv5Proxy"
#define SOCKSv5_Proxy_Conf_File_TAG     "SOCKSv5ProxyConfFile"
#define SOCKSv5_Proxy_Auth_File_TAG     "SOCKSv5ProxyAuthFile"

/*BT tethering Tags*/
#define BTTetheringCfg          "BTTetheringCfg"
#define BTTethFlag              "BTTethFlag"

/*dhcpv6 Entries*/
#define DHCPV6CFG_TAG           "Dhcpv6Cfg"
#define DHCPV6_Enable_TAG       "EnableDhcpv6Dns"

/*packet stats entries*/
#define PacketStatsCfg_TAG      "PacketStatsCfg"
#define IPAPacketStatsCfg_TAG   "EnableLANStats"

/*Dun+Softap DunDongleMode entries*/
#define Dun_Dongle_Mode_Tag     "DunDongleMode"

/*Always on WLAN entries*/
#define AlwaysOnWLAN_TAG        "AlwaysOnWLAN"

//Used to define the maximum value of the variable passed in getset apis
#define MAX_STRING_LENGTH 100
#define MAX_PROFILE_HANDLE_LENGTH 15

/**
  Checks whether the prefix is link local.
 */
#define QCMAP_IN6_IS_PREFIX_LINKLOCAL(v6)                                      \
  ((ntohl(*v6) & 0xffff0000) == 0xfe800000)
#define QCMAP_IN6_IS_ZERO(v6) ((ntohl(*v6)) == 0)


#define APPS_LAN_IP_ADDR "192.168.225.1"
#define APPS_SUB_NET_MASK "255.255.255.0"

#define QCMAP_LAN_CONNECTED_TIMEOUT 60 // 60 seconds for LAN connected
#define QCMAP_WAN_CONNECTED_TIMEOUT 60 // 60 seconds for WAN connected
#define ETH_IFACE "eth0"
#define ETH_LINK "eth"
#define ETH_NET_CARRIER "/sys/class/net/eth0/carrier"
#define STATIC_WLAN "Static IP WLAN Client"
#define BT_IFACE "bt-pan"



/* RS/RA Constants */
#define ROUTER_MULTICAST "ff02::2"
#define FRAME_SIZE 100
#define MAC_SIZE 6
#define SENDING_PID 1000
#define MAX_HOPS 255
#define MESSAGE_CODE 0
#define SEQUENCE_NUM 0
#define ICMP_HEADER_CHKSUM 0;
#define ETH_TYPE 2
#define MAX_IPV6_PREFIX 40
#define RANDOM_MAC_KEY 3
#define MAX_EIO_RETRY 3
#define DS_SYS_CALL_SUCCESS 0
#define DS_SYS_INTERNAL_FAIL 255



#define KERNEL_VERSION_LENGTH 16

#define FLOW_TOTAL 28
#define FLOW_LABEL 20

#ifdef QTIMAPSERVER_OFFTARGET
  #define ds_log_med printf
#endif

#define MAX_DHCPCD_INFO_RETRY 3
#define DHCPCD_INFO_RETRY_DELAY 1
#define FIREWALL_MARK 0x35

/* Check whether profile is default */
#define IS_DEFAULT_PROFILE(profile) \
    ((profile) == QCMAP_Backhaul::defaultProfileHandle)

/* target check for UL firewall*/
#define IS_UL_FIREWALL_ALLOWED(target) \
    ((target == DS_TARGET_LE_STINGRAY) || (target == DS_TARGET_LE_CHIRON))

/* Multi-PDN allowed targets check */
#define IS_MULTI_PDN_ALLOWED(target) \
    ( (target == DS_TARGET_LE_ATLAS) || (target == DS_TARGET_LE_CHIRON) )

/* L2TP allowed targets check */
#define IS_L2TP_ALLOWED(target) \
    (target != DS_TARGET_LE_STINGRAY)

/* VLAN allowed targets check */
#define IS_VLAN_ALLOWED(target) \
    (target == DS_TARGET_LE_CHIRON)

/* Multi-PDN to VLAN Mapping allowed targets check */
#define IS_MULTI_PDN_VLAN_MAPPING_ALLOWED(target) \
    ( IS_VLAN_ALLOWED(target) && IS_MULTI_PDN_ALLOWED(target) )

/* SOCKSv5 allowed targets check */
#define IS_SOCKSV5_ALLOWED(target) \
    (target != DS_TARGET_LE_STINGRAY)

/* Check whether firewall for secondary PDN's */
#define IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(target) \
    ( (target == DS_TARGET_LE_CHIRON) )

/* Check whether target support's Neutrino IPA offload */
/* Hard-coded to FALSE, since no target is supported on mainline */
#define IS_NEUTRINO_IPA_SUPPORTED(target) (FALSE)

/* Check whether target support's Neutrino IPA offload */
#define IS_EMAC_IPA_SUPPORTED(target) \
    (target == DS_TARGET_LE_CHIRON)

typedef enum{
  UPDATE_MOBILEAP_XML = 0,
  UPDATE_FIREWALL_XML,
  UPDATE_IPACFG_XML
}qcmap_update_xml;

typedef enum{
  NO_BACKHAUL = 0,
  BACKHAUL_TYPE_WWAN,
  BACKHAUL_TYPE_AP_STA_BRIDGE,
  BACKHAUL_TYPE_AP_STA_ROUTER,
  BACKHAUL_TYPE_CRADLE,
  BACKHAUL_TYPE_ETHERNET,
  BACKHAUL_TYPE_BT
}qcmap_backhaul_type;


typedef enum {
CONFIG_MOBILEAP= 1,
CONFIG_WLAN,
}qcmap_bootup_config_enum;


typedef enum {
  CONFIG_WLAN_MODE = 1,
  CONFIG_GUEST_ACCESS_PROFILE = 2,
  CONFIG_GUEST_2_ACCESS_PROFILE,
  CONFIG_GUEST_3_ACCESS_PROFILE,
}qcmap_wlan_config_enum;

typedef enum
{
  QCMAP_MSGR_INTF_AP_INDEX          = 0x00,
  QCMAP_MSGR_INTF_GUEST_AP_INDEX    = 0x01,
  QCMAP_MSGR_INTF_GUEST_AP_2_INDEX  = 0x02,
  QCMAP_MSGR_INTF_GUEST_AP_3_INDEX  = 0x03,
  QCMAP_MSGR_INTF_STATION_INDEX     = 0x04
} qcmap_cm_intf_index_type;

typedef enum
{
  QCMAP_MSGR_TETH_LINK_INDEX_NONE      = -1,
  QCMAP_MSGR_TETH_LINK_INDEX1          = 0,
  QCMAP_MSGR_TETH_LINK_INDEX2          = 1
} qcmap_cm_teth_index_type;

typedef enum
{
  QCMAP_CM_PROFILE_FULL_ACCESS         = 0x01,
  QCMAP_CM_PROFILE_INTERNET_ONLY       = 0x02
} qcmap_cm_access_profile_type;

typedef enum
{
  QCMAP_CM_DEVMODE_AP                  = 0x01,
  QCMAP_CM_DEVMODE_STA                 = 0x02
} qcmap_cm_devmode_type;

typedef enum
{
  QCMAP_MSGR_INPUT_CHAIN,
  QCMAP_MSGR_OUTPUT_CHAIN,
  QCMAP_MSGR_FORWARD_CHAIN,
} qcmap_msgr_iptable_chain;

typedef enum
{
  QCMAP_CM_VLAN_ADDED_ON_PHY           = 0x01,
  QCMAP_CM_LAST_VLAN_REMOVED_ON_PHY    = 0x02,
  QCMAP_CM_VLAN_ADD_REMOVE_ON_BRIDGE0  = 0x03,
  QCMAP_CM_VLAN_ADD_WITH_PKT_STATS_ON  = 0x04
}qcmap_cm_auto_reboot_reason_code;

/*----------------------------------------------------------------------------
  RNDIS and ECM enum types
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_QTI_TETHERED_LINK_NONE                       = 0x00,
  QCMAP_QTI_TETHERED_LINK_RNDIS                      = 0x01,
  QCMAP_QTI_TETHERED_LINK_ECM                        = 0x02,
  QCMAP_QTI_TETHERED_LINK_PPP                        = 0x03,
  QCMAP_QTI_TETHERED_LINK_ETH                        = 0x04
}qcmap_qti_tethered_link_type;

typedef enum
{
  QCMAP_MSGR_UL_FIREWALL = 0x01,
  QCMAP_MSGR_DL_FIREWALL = 0x02
}qcmap_msgr_firewall_direction;

#define IPTABLE_CHAIN 8

typedef struct {
  dsi_hndl_t handle;
  const char* tech;
  const char* family;
  int profile;
} dsi_call_info_t;



/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef struct qcmap_dsi_buffer_s {
  dsi_hndl_t            dsi_nethandle;
  void                  *user_data;
  dsi_net_evt_t         evt;
  dsi_evt_payload_t     *payload_ptr;
} qcmap_dsi_buffer_t;

typedef struct qcmap_nas_buffer_s {
  qmi_client_type user_handle;                    /* QMI user handle       */
  unsigned int    msg_id;                         /* Indicator message ID  */
  void           *ind_buf;                        /* Raw indication data   */
  unsigned int    ind_buf_len;                    /* Raw data length       */
  void           *ind_cb_data;                     /* User call back handle */
} qcmap_nas_buffer_t;


typedef struct qcmap_dsd_buffer_s {
  qmi_client_type user_handle;                    /* QMI user handle       */
  unsigned int    msg_id;                         /* Indicator message ID  */
  void           *ind_buf;                        /* Raw indication data   */
  unsigned int    ind_buf_len;                    /* Raw data length       */
  void           *ind_cb_data;                     /* User call back handle */
} qcmap_dsd_buffer_t;

typedef struct qcmap_scm_buffer_s {
  qmi_client_type user_handle;                    /* QMI user handle       */
  unsigned int    msg_id;                         /* Indicator message ID  */
  void           *ind_buf;                        /* Raw indication data   */
  unsigned int    ind_buf_len;                    /* Raw data length       */
  void           *ind_cb_data;                     /* User call back handle */
} qcmap_scm_buffer_t;

/*---------------------------------------------------------------------------
  QCMAP QMI SERVICE
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_QMI_NAS_IN_SERVICE = 0,
  QCMAP_QMI_NAS_NOT_IN_SERVICE,
}qcmap_qmi_service_t;


typedef struct qcmap_qmi_service_buffer_s {
  qcmap_qmi_service_t  qmi_service_id;
} qcmap_qmi_service_buffer_t;

typedef enum
{
  AUTO_CONNECT_V4 = 0x0,
  AUTO_CONNECT_V6,
  STA_ASSOC_FAIL,
  STA_DHCP_FAIL,
  NS_NA_PROXY_CLEANUP
}qcmap_timer_enum_t;

typedef struct
{
    qcmap_timer_enum_t timer_id;
    void *pUserData;
}qcmap_timer_data_t;

typedef struct qcmap_timer_buffer_s {
qcmap_timer_enum_t msg_id;                         /* Indicator message ID  */
uint32             user_data;
} qcmap_timer_buffer_t;

typedef enum {
  STA_CONNECTED,
  STA_DISCONNECTED,
  AP_STA_CONNECTED,
  AP_STA_DISCONNECTED
}qcmap_sta_event_t;

typedef enum {
  CRADLE_CONNECTED,
  CRADLE_DISCONNECTED
}qcmap_cradle_event_t;

typedef enum {
  ETH_BACKHAUL_CONNECTED,
  ETH_BACKHAUL_DISCONNECTED
}qcmap_eth_backhaul_event_t;

typedef enum {
  BT_BACKHAUL_CONNECTED,
  BT_BACKHAUL_DISCONNECTED
}qcmap_bt_backhaul_event_t;


typedef struct qcmap_sta_buffer_s {
  uint32 sta_cookie;
  unsigned int iface_num;
  char mac_addr[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_sta_event_t event;
}qcmap_sta_buffer_t;

typedef struct qcmap_dynamic_dns_server_config_s {
 char server_url[QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01];
 char protocol[QCMAP_DDNS_PROTOCOL_LEN];
}qcmap_ddns_server_info_t;

typedef struct qcmap_dynamic_dns_s {
 char login[QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01];
 char password[QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01];
 char hostname[QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01];
 uint32 timeout;
 qcmap_ddns_server_info_t server_info;
 uint8 enable;
}qcmap_dynamic_dns_t;


/*---------------------------------------------------------------------------
           Port Forwarding Entry Configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   port_fwding_private_ip;
  uint16   port_fwding_private_port;
  uint16   port_fwding_global_port;
  uint8    port_fwding_protocol;
} qcmap_cm_port_fwding_entry_conf_t;

typedef struct
{
  ds_dll_el_t *natEntryListHead;
  ds_dll_el_t *natEntryListTail;
}qcmap_nat_entry_list_t;

typedef struct
{
  ds_dll_el_t *sipServerEntryListHead;
  ds_dll_el_t *sipServerEntryListTail;
}qcmap_sip_server_info_list_t;


typedef struct{
  ds_dll_el_t *dhcpInfoEntryListHead;
  ds_dll_el_t *dhcpInfoEntryListTail;
}qcmap_dhcp_information_info_list_t;

typedef struct{
  ds_dll_el_t *firewallEntryListHead;
  ds_dll_el_t *firewallEntryListTail;
}qcmap_firewall_entry_list_t;

typedef struct{
  ds_dll_el_t *dlnawhitelistListHead;
  ds_dll_el_t *dlnawhitelistListTail;
}qcmap_dlna_whitelist_entry_list_t;

typedef struct
{
  boolean ip_passthrough_active;
  uint32  rmnet_reserved_ip;
  in_addr_t  public_ip;
  in_addr_t rmnet_gateway_ip;
  qcmap_msgr_device_type_enum_v01       device_type;
  in_addr_t  public_netmask;
  char rmnet_def_route [MAX_COMMAND_STR_LEN];
} qcmap_cm_ip_passthrough_conf_t;


/*---------------------------------------------------------------------------
           Extended FireWall Entry Configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  ip_filter_type       filter_spec;
  uint32               firewall_handle;
  /* Boolean to indicate pinhole. */
  boolean upnp_pinhole_handle;

  /* Direction of the firewall. */
  qcmap_msgr_firewall_direction firewall_direction ;

} qcmap_msgr_firewall_entry_conf_t;

/*---------------------------------------------------------------------------
           Extended FireWall handle list configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 handle_list[QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01];
  ip_version_enum_type ip_family;
  int num_of_entries;
} qcmap_msgr_get_firewall_handle_list_conf_t;

/*---------------------------------------------------------------------------
           Extended FireWall configuration.
---------------------------------------------------------------------------*/
typedef union
{
  qcmap_msgr_firewall_entry_conf_t extd_firewall_entry;
  qcmap_msgr_get_firewall_handle_list_conf_t extd_firewall_handle_list;
  ip_version_enum_type ip_family;
} qcmap_msgr_firewall_conf_t;

typedef enum
{
SET_VALUE = 0,
GET_VALUE
}qcmap_action_type;

/*---------------------------------------------------------------------------
           APSTA Configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean  sta_bridge_mode;
  boolean  ap_sta_bridge_sta_assoc_running;
  timer_t  timerid_sta_assoc;
  qcmap_msgr_sta_static_ip_config_v01 static_ip_config;
  boolean  ap_sta_bridge_sta_dhcp_running;
  timer_t  timerid_sta_dhcp;
  char     sta_interface[QCMAP_MSGR_INTF_LEN];
  qcmap_msgr_sta_connection_enum_v01 conn_type;
  timer_t   timerid_proxy_cleanup;
}qcmap_cm_apsta_conf_t;

/*---------------------------------------------------------------------------
   IPV6 Address Type.
---------------------------------------------------------------------------*/
typedef enum {
  LINK_LOCAL_IPV6_ADDR = 1,
  GLOBAL_IPV6_ADDR
} qcmap_ipv6_addr_t;

/*---------------------------------------------------------------------------
  Data Structure to store AP-STA router mode info
---------------------------------------------------------------------------*/
typedef struct
{
  char bridge_mac[ETH_ALEN];
  char ap_mac[ETH_ALEN];
  char ap_mac2[ETH_ALEN];
  char sta_mac[ETH_ALEN];
  struct in_addr bridge_ip;
  struct in6_addr bridge_ip6;
  struct in6_addr bridge_ip6_link_local;
  uint8_t prefix_len;
  struct in_addr bridge_netmask;
  struct in_addr bridge_def_gw;
}qcmap_ap_sta_bridge_t;


/*---------------------------------------------------------------------------
           NAT Configuration.
---------------------------------------------------------------------------*/

#define QCMAP_CM_MAX_FILE_LEN          120

typedef struct
{
  uint32   nat_entry_generic_timeout;
  uint32   nat_entry_icmp_timeout;
  uint32   nat_entry_tcp_established_timeout;
  uint32   nat_entry_udp_timeout;
  uint8    initial_pkt_limit;
} qcmap_cm_nat_conf_common_t;

typedef struct
{
  uint8                     enable_ipsec_vpn_pass_through;
  uint8                     enable_pptp_vpn_pass_through;
  uint8                     enable_l2tp_vpn_pass_through;
  uint8                     enable_alg_mask;
  uint32                    dmz_ip; /* 0 mean disable DMZ */
  uint8                     num_port_fwding_entries;
  qcmap_nat_entry_list_t    port_fwding_entries;
  qcmap_msgr_nat_enum_v01   nat_type;
  qcmap_msgr_nat_enum_v01   prev_nat_type;
} qcmap_cm_nat_conf_t;


/*---------------------------------------------------------------------------
           LAN Configuration.
---------------------------------------------------------------------------*/
#define QCMAP_LAN_INVALID_QCMAP_HANDLE (-1)
#define QCMAP_LAN_INVALID_IFACE_INDEX  (-1)
#define QCMAP_LAN_MAX_IPV4_ADDR_SIZE   16    /* 3 dots + 4 * 3 #s + 1 null */
/* 4 Interfaces (AP1, AP2, AP3,AP4, STA)
     Possible Modes: AP, AP+AP, AP+AP+AP, AP+AP+AP+AP, STA+AP, STA+AP+AP */
#define QCMAP_MAX_NUM_INTF             5
/* Maximum Tethered Instances Supported USB+CPE, USB+USB */
#define QCMAP_QTI_MAX_TETH_LINK        2

#define QCMAP_V4_DEFAULT_DEVICE_NAME "rmnet0"
#define QCMAP_V6_DEFAULT_DEVICE_NAME "rmnet1"
#define RMNET_INTERFACE              "rmnet_data"

/*-------------------------------------------------------------------------
  USB config structure to store IP addresses for RNDIS/ECM tethering
--------------------------------------------------------------------------*/
typedef struct
{
  boolean link_enabled;
  qcmap_qti_tethered_link_type link_type;
  uint32 gateway_addr;
  uint32 subnet_mask;
  uint32 remote_link_ip4_address;
  uint8_t remote_link_mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint32   dhcp_lease_time;
}qcmap_cm_tethered_conf_t;

typedef struct
{
  /*------------ Interface parameters common to STA,AP modes ------------- */
  /* Enable and configure main interface. */
  boolean  enable;
  /*------------ Interface parameters specific to AP Mode ---------------- */
  /* Path to WLAN AP config which contain SSID/Mode/Encryption info */
  char     path_to_hostapd_conf[QCMAP_CM_MAX_FILE_LEN];
  char     path_to_hostapd_entropy[QCMAP_CM_MAX_FILE_LEN];
  /* Path to WLAN AP config which contain SSID/Mode/Encryption info in
   * Station Mode. Applicable only to Primary AP. */
  char     path_to_sta_mode_hostapd_conf[QCMAP_CM_MAX_FILE_LEN];

  /* LL interface configuration. All Addresses are in host order */
  uint32   ll_ip_addr;
  uint32   ll_subnet_mask;

  /* Type of access main interface has to networks. */
  qcmap_msgr_access_profile_v01 access_profile;


} qcmap_cm_intf_conf_t;

typedef struct
{
  qcmap_cm_intf_conf_t interface[QCMAP_MAX_NUM_INTF];
  char  module[QCMAP_CM_MAX_FILE_LEN];
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
}qcmap_cm_wlan_conf_t;

typedef struct
{
  qcmap_msgr_wlan_iface_index_enum_v01 wlan_ap_type;
  char if_name[QCMAP_MAX_IFACE_NAME_SIZE_V01];
  qcmap_msgr_wlan_iface_active_state_enum_v01 state;
  qcmap_msgr_wlan_device_type_v01 wlan_dev_type;
}qcmap_cm_wlan_if_info_t;

typedef struct
{
  /* Interface information. */
  uint32   ppp_reserved_ip;
  uint32   apps_ip_addr;
  uint32   sub_net_mask;
   /* DHCP server config */
  boolean  enable_dhcpd;
  uint32   dhcp_start_address;
  uint32   dhcp_end_address;
  uint32     dhcp_lease_time;
  uint32 num_dhcp_reservation_records;
  qcmap_dhcp_information_info_list_t dhcp_reservation_records;
  boolean dhcp_reservations_updated;
  char gateway_url[QCMAP_MSGR_MAX_GATEWAY_URL_V01];
  char bridge_mac[ETH_ALEN];
  qcmap_cm_ip_passthrough_conf_t ip_passthrough_cfg;
  char ipv6_ula_addr[QCMAP_MAX_IPV6_ADDRSTRLEN_V01];
} qcmap_cm_lan_conf_t;



/*---------------------------------------------------------------------------
           WAN Configuration.
---------------------------------------------------------------------------*/
#define QCMAP_WAN_INVALID_QCMAP_HANDLE 0xFFFFFFFF
#define QCMAP_WAN_MAX_ERI_DATA_SIZE    256
#define QCMAP_WAN_TECH_ANY             0
#define QCMAP_WAN_TECH_3GPP            1
#define QCMAP_WAN_TECH_3GPP2           2
#define MAX_WAN_CON_TIMEOUT 120
#define CLOCKID CLOCK_REALTIME
#define SIG SIGUSR1

typedef struct
{
  struct
  {
    int  umts_profile_index;
    int  cdma_profile_index;
  }v4;
  struct
  {
    int  umts_profile_index;
    int  cdma_profile_index;
  }v6;
}qcmap_cm_profile_index;


// Backhaul Interface
typedef enum{
  TYPE_IPV4  =1,
  TYPE_IPV6
}qcmap_backhaul_interface_type;

typedef enum
{
  QCMAP_CM_SIP_SERVER_MIN,
  QCMAP_CM_SIP_SERVER_ADDR,
  QCMAP_CM_SIP_SERVER_FQDN,
  QCMAP_CM_SIP_SERVER_MAX
} qcmap_cm_sip_server_type_e;

typedef union
{
  uint32 sip_serv_ip;
  char sip_fqdn[QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01];
} qcmap_cm_sip_server_info;

typedef struct
{
  qcmap_cm_sip_server_type_e sip_serv_type;
  qcmap_cm_sip_server_info sip_serv_info;
} qcmap_cm_sip_server_info_t ;

typedef struct{
  uint8 num_firewall_entries;
  qcmap_firewall_entry_list_t extd_firewall_entries;
  uint8 firewall_enabled;
  uint8 firewall_pkts_allowed;
  uint8 upnp_inbound_pinhole;
  uint8 enable_ul_firewall;
}qcmap_cm_firewall_conf_t;

typedef struct{
  uint8 num_whitelist_entries;
  qcmap_dlna_whitelist_entry_list_t whitelist_entries;
}qcmap_cm_dlna_whitelist_conf_t;

typedef struct
{
  char     eri_config_file[QCMAP_CM_MAX_FILE_LEN];
  int      ip_family;
  uint8    enable_webserver_wwan_access;
  char     ipv4_interface[QCMAP_MSGR_INTF_LEN];
  char     ipv6_interface[QCMAP_MSGR_INTF_LEN];
} qcmap_cm_wan_conf_t;

typedef struct
{
  boolean                       roaming;
  boolean                       autoConnect;
  qcmap_cm_profile_index        profile_id;
  int                           tech;
  int                           prefix_delegation;
}qcmap_cm_wwan_conf_t;


typedef struct
{
  boolean upnp_config;
  boolean dlna_config;
  boolean mdns_config;
  boolean dlna_whitelisting_config;
} qcmap_cm_srvc_conf_t;

typedef struct
{
  boolean enable_wlan_at_bootup;
  boolean enable_mobileap_at_bootup;
  boolean enable_gsb_at_bootup;
}qcmap_cm_bootup_conf_t;

typedef struct
{
  boolean supplicant_debug;
  char    supplicant_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
  boolean hostapd_debug;
  char    hostapd_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
  boolean hostapd_guest_debug;
  char    hostapd_guest_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
  boolean hostapd_guest_2_debug;
  char    hostapd_guest_2_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
  boolean hostapd_guest_3_debug;
  char    hostapd_guest_3_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
  boolean p2p_supplicant_debug;
  char    p2p_supplicant_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
}qcmap_cm_debug_conf_t;

typedef struct
{
  qcmap_cm_intf_conf_t  hostapd;
  qcmap_cm_intf_conf_t  hostapd_guest;
  qcmap_cm_intf_conf_t  hostapd_guest_2;
  qcmap_cm_intf_conf_t  hostapd_guest_3;
  char                  supplicant[QCMAP_CM_MAX_FILE_LEN];
  qcmap_cm_debug_conf_t debug;
} qcmap_cm_wlan_paths_conf_t;

typedef struct
{
  qcmap_msgr_cradle_mode_v01 cradle_mode;
}qcmap_cm_cradle_conf_t;

typedef struct
{
  qcmap_msgr_ethernet_mode_v01 eth_mode;
}qcmap_cm_eth_backhaul_conf_t;
/*---------------------------------------------------------------------------
           Master Mobile AP Config.
---------------------------------------------------------------------------*/
typedef struct
{

  qcmap_cm_bootup_conf_t bootup_config;
  boolean packet_stats_config;
} qcmap_cm_conf_t;


/*---------------------------------------------------------------------------
           Embedded Profile
---------------------------------------------------------------------------*/
typedef struct
{
  int      tech;
  int      umts_profile_index;
  int      cdma_profile_index;
  int      ip_family;

} qcmap_cm_embd_conf;

typedef struct qcmap_cm_nl_prefix_info_s {
  boolean prefix_info_valid;
  unsigned char prefix_len;
  unsigned int mtu;
  struct sockaddr_storage prefix_addr;
  struct ifa_cacheinfo          cache_info;
} qcmap_cm_nl_prefix_info_t;

/*------------------------------------------------------------
Store the head of the list which stores the MAC and IP address
-------------------------------------------------------------*/
typedef struct
{
  ds_dll_el_t *addrListHead;
  ds_dll_el_t *addrListTail;
}qcmap_addr_info_list_t;

typedef struct
{
  uint8 src_mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint8_t src_ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t dst_ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  time_t time;
}qcmap_cm_proxy_data_info_t;

typedef struct
{
  uint64_t tx_data;
  uint64_t rx_data;
}qcmap_cm_stats_data_info_t;

typedef enum{
  QCMAP_CLEANUP_PACKET_STATS_RULES = 0,
  QCMAP_UPDATE_WLAN_CLIENTS_DATA,
  QCMAP_RESET_IPA_CLIENT_DATA
}qcmap_update_cdi;

typedef struct
{
  uint8 mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  qcmap_msgr_device_type_enum_v01 device_type;
  char host_name[QCMAP_MSGR_DEVICE_NAME_MAX_V01];
  uint32_t lease_expiry_time;
  uint32 ip_addr;
  uint8_t link_local_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  qcmap_msgr_client_ipv6_addr_v01 ipv6[QCMAP_MSGR_MAX_IPV6_ADDR_V01];
  uint64_t bytes_rx;
  uint64_t bytes_tx;
  qcmap_cm_stats_data_info_t sfe_data;
  qcmap_cm_stats_data_info_t ipa_data;
  qcmap_cm_stats_data_info_t exception_data;
  int16_t vlan_id;
}qcmap_cm_client_data_info_t;

typedef struct
{
  uint8     mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  int16_t   vlan_id;
}qcmap_mac_addr_vlan_info_t;

typedef struct
{
  uint32_t    numOfNodes;
  bool        wlanclientpresent;
  uint8_t     numofWlanclients;
}qcmap_cm_connected_info_t;

typedef enum
{
  QCMAP_AP_STA_DISCONNECTED = 0,
  QCMAP_DELNEIGH
}qcmap_cm_cdi_del_evt_t;

typedef struct {
  boolean rat_lte_fdd;
  boolean lte_cat4_bw;
  boolean usb_clock_at_nom;
}qcmap_cm_dynamic_clock_info_t;

typedef struct
{
  struct ps_in6_addr *iid_ptr;
}qcmap_cm_prefix_iid_info_t;

/*---------------------------------------------------------------------------
                   QCMAP Backhaul status info
---------------------------------------------------------------------------*/
typedef struct
{
  int                 backhaul_v4_available;
  int                 backhaul_v6_available;
  qcmap_msgr_backhaul_type_enum_v01 backhaul_type;
} qcmap_cm_backhaul_status;

/*---------------------------------------------------------------------------
                    Return values backhaul status
---------------------------------------------------------------------------*/
#define BACKHAUL_NOT_APPLICABLE       -1
#define BACKHAUL_NOT_AVAILABLE         0
#define BACKHAUL_AVAILABLE             1

/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define QCMAP_CM_SUCCESS               0         /* Successful operation   */
#define QCMAP_CM_ERROR                -1         /* Unsuccessful operation */

/*---------------------------------------------------------------------------
           Error Condition Values
---------------------------------------------------------------------------*/
#define QCMAP_CM_ENOERROR              0        /* No error                */
#define QCMAP_CM_EWOULDBLOCK           1        /* Operation would block   */
#define QCMAP_CM_EINVAL                2        /* Invalid operation       */
#define QCMAP_CM_EOPNOTSUPP            3        /* Operation not supported */
#define QCMAP_CM_EBADAPP               4        /* Invalid application ID  */
#define QCMAP_CM_ENOWWAN               5        /* WWAN not connected      */
#define QCMAP_CM_EALDCONN              6        /* Already connected  */
#define QCMAP_CM_EALDDISCONN           7        /* Already disconnected  */
#define QCMAP_CM_DISCONNECTED          8        /* Disconnected */
#define QCMAP_MSGR_ENTRY_PRESENT       -8
#define QCMAP_MSGR_ENTRY_FULL          -9
#define QCMAP_MSGR_INVALID_PARAM       -10

#define SA_FAMILY(addr)         (addr).sa_family
#define SA_DATA(addr)           (addr).sa_data
#define SASTORAGE_FAMILY(addr)  (addr).ss_family
#define SASTORAGE_DATA(addr)    (addr).__ss_padding

#define MAX_ND_PKT_SIZE 1280

/*---------------------------------------------------------------------------
           Mobile AP Events
---------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_CM_EVENT_ENABLED = 0,
  QCMAP_CM_EVENT_STA_CONNECTED,
  QCMAP_CM_EVENT_STA_DISCONNECTED,
  QCMAP_CM_EVENT_STA_ASSOCIATION_FAIL,
  QCMAP_CM_EVENT_STA_DHCP_IP_ASSIGNMENT_FAIL,
  QCMAP_CM_EVENT_WAN_CONNECTING,
  QCMAP_CM_EVENT_WAN_CONNECTING_FAIL,
  QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL,
  QCMAP_CM_EVENT_WAN_CONNECTED,
  QCMAP_CM_EVENT_WAN_IPv6_CONNECTED,
  QCMAP_CM_EVENT_WAN_DISCONNECTED,
  QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED,
  QCMAP_CM_EVENT_WAN_ADDR_RECONF,
  QCMAP_CM_EVENT_WAN_IPv6_ADDR_RECONF,
  QCMAP_CM_EVENT_CRADLE_CONNECTED,
  QCMAP_CM_EVENT_CRADLE_DISCONNECTED,
  QCMAP_CM_EVENT_ETH_BACKHAUL_CONNECTED,
  QCMAP_CM_EVENT_ETH_BACKHAUL_DISCONNECTED,
  QCMAP_CM_EVENT_BT_BACKHAUL_CONNECTED,
  QCMAP_CM_EVENT_BT_BACKHAUL_DISCONNECTED,
  QCMAP_CM_EVENT_DISABLED,
  QCMAP_CM_EVENT_WWAN_ROAMING_STATUS_CHANGED
} qcmap_cm_event_e;

/*---------------------------------------------------------------------------
   Type representing enumeration of QCMAP CM states
---------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_CM_DISABLE = 0,
  QCMAP_CM_ENABLE,
  QCMAP_CM_WAN_CONNECTING,
  QCMAP_CM_WAN_DISCONNECTING,
  QCMAP_CM_WAN_CONNECTED
} qcmap_cm_state_e;

typedef enum
{
  QCMAP_CM_V6_DISABLE = 0,
  QCMAP_CM_V6_ENABLE,
  QCMAP_CM_V6_WAN_CONNECTING,
  QCMAP_CM_V6_WAN_DISCONNECTING,
  QCMAP_CM_V6_WAN_CONNECTED
} qcmap_cm_v6_state_e;

#define QCMAP_CM_IND_V4_WAN_CONNECTED            1002
#define QCMAP_CM_IND_V4_WAN_DISCONNECTED         1003
#define QCMAP_CM_IND_V6_WAN_CONNECTED            1004
#define QCMAP_CM_IND_V6_WAN_DISCONNECTED         1005

/*---------------------------------------------------------------------------
           Mobile AP CM call back declarations.
---------------------------------------------------------------------------*/
typedef void (*qcmap_cm_cb_fcn)
(
  int                 handle,                  /* Mobile AP Application id */
  qcmap_cm_event_e    event,                   /* Type of Mobile AP Event  */
  void               *wwanObj,                 /* Context of WWAN Object   */
  void               *qcmap_cm_cb_user_data,   /* Call back User data     */
  dsi_ce_reason_t    *callend_reason,
  qcmap_msgr_wwan_info_v01 *wwan_info
);

/*---------------------------------------------------------------------------
  Mobile AP Statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint64     bytes_rx;
  uint64     bytes_tx;
  uint32     pkts_rx;
  uint32     pkts_tx;
  uint32     pkts_dropped_rx;
  uint32     pkts_dropped_tx;
}qcmap_cm_statistics_t;

/*---------------------------------------------------------------------------
  QCMAP Netlink Events
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_NL_NEWNEIGH = 0,
  QCMAP_NL_DELNEIGH,
  QCMAP_NL_NEWADDR,
  QCMAP_NL_PPP_IPV6_ROUTE,
  QCMAP_NL_RS,
  QCMAP_NL_RA,
  QCMAP_NL_NS,
  QCMAP_NL_NA
}qcmap_nl_event_t;

/*---------------------------------------------------------------------------
  QCMAP Netlink Interface
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_NL_USB = 0,
  QCMAP_NL_WLAN,
  QCMAP_NL_PPP,
  QCMAP_NL_BRIDGE,
  QCMAP_NL_ETH,
  QCMAP_NL_BT,
  QCMAP_NL_ANY
}qcmap_nl_iface_t;

/*---------------------------------------------------------------------------
  QCMAP Netlink Addresses
----------------------------------------------------------------------------*/
typedef struct
{
  bool isValidIPv4address;
  bool isValidIPv6address;
  char                  iface_name[QCMAP_MSGR_INTF_LEN];
  uint8                 ip_vsn;
  uint32_t              ip_addr;
  uint8_t               ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t               mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint8                 ipv6_dst_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8                 ipv6_src_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8                 target_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t               prefix_len;
  char                  nd_packet_buf[MAX_ND_PKT_SIZE];
}qcmap_nl_addr_t;

/*------------------------------------------------------------
  QCMAP Netlink Socket Payload
-------------------------------------------------------------*/
typedef struct
{
  int16_t               vlan_id;
  qcmap_nl_event_t      nl_event;
  qcmap_nl_iface_t      nl_iface;
  qcmap_nl_addr_t       nl_addr;
} qcmap_nl_sock_msg_t;

/*---------------------------------------------------------------------------
  QCMAP V6 MCAST SOCK IFACE
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_V6_SOCK_BRIDGE = 0,
  QCMAP_V6_SOCK_WLAN,
  QCMAP_V6_SOCK_ETH,
  QCMAP_V6_SOCK_ECM,
  QCMAP_V6_SOCK_BT
}qcmap_v6_sock_iface_t;

/*---------------------------------------------------------------------------
  QCMAP PMIP SOCK TYPES
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_PMIP_DEL_DEVICE = 1,
  QCMAP_PMIP_QUERY_DEVICE_REQ,
  QCMAP_PMIP_QUERY_DEVICE_RESP
}qcmap_pmip_event_t;

typedef struct
{
  uint8_t     mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint8_t     if_id;
  qcmap_msgr_ip_family_enum_v01       pmip_mode;
  bool isValidMac;
}qcmap_pmip_info_t;

typedef struct
{
  qcmap_pmip_event_t    pmip_event;
  qcmap_pmip_info_t     dev_info;
} qcmap_pmip_sock_msg_t;

typedef struct
{
  boolean                 p2p_status;
  uint8_t                 p2p_role_valid;
  qcmap_p2p_role_type_v01 p2p_role;
} qcmap_p2p_config;

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION qcmap_cm_get_handle
===========================================================================*/
/*!
@brief
  Get qcmap_cm handle

@return
  int

@note

- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/
int qcmap_cm_get_handle();


/*===========================================================================
  FUNCTION qcmap_cm_is_disable_in_process
===========================================================================*/
/*!
@brief
  Get disable_in_process value

@return
  boolean

@note

- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/
boolean qcmap_cm_is_disable_in_process();

/*===========================================================================
  FUNCTION qcmap_cm_send_event
===========================================================================*/
/*!
@brief
  Get qcmap_cm handle

@return
  boolean

@note

- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/

void qcmap_cm_send_event
(
  qcmap_cm_event_e event,
  void *wwanObj,
  dsi_ce_reason_t *dsi_reason,
  qcmap_msgr_wwan_info_v01 *wwan_info
);

/*===========================================================================
  FUNCTION qcmap_cm_register_nas_sys_info
===========================================================================*/
/*!
@brief
  Register for NAS sys_info indication.

@return
  void

@note

- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/
void qcmap_cm_register_nas_sys_info(boolean enable);

/*===========================================================================
  FUNCTION start_wlan_sta_timer
===========================================================================*/
/*!
@brief
  Funtion to WLAN STA timer

@return
  QCMAP_CM_SUCCESS
  QCMAP_CM_ERROR

@note
 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
int start_wlan_sta_timer(qcmap_timer_enum_t sta_event);

int readable_addr(int domain, const uint32_t *addr, char *str);



/*===========================================================================
  FUNCTION stop_wlan_sta_timer
===========================================================================*/
/*!
@brief
  Funtion to stop WLAN STA timer

@return
  QCMAP_CM_SUCESS
  QCMAP_CM_ERROR

@note

  - Dependencies
  - None

  - Side Effects
  - None
*/
/*=========================================================================*/
int stop_wlan_sta_timer(qcmap_timer_enum_t sta_event);

/*===========================================================================
  FUNCTION qcmap_timer_handler
===========================================================================*/
/*!
@brief
   Function to handle various QCMAP timers
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
extern void qcmap_timer_handler(int sig, siginfo_t *si, void *uc );

/*===========================================================================
  FUNCTION qcmap_cm_qmi_nas_ind
===========================================================================*/
/*!
@brief
   Function to handle QMI NAS indications
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

extern int
qcmap_cm_qmi_nas_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
);

/*===========================================================================
  FUNCTION qcmap_cm_qmi_dsd_ind
===========================================================================*/
/*!
@brief
   Function to handle QMI DSD indications
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

extern int
qcmap_cm_qmi_dsd_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
);

extern void
qcmap_cm_process_qmi_nas_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
);

extern void
qcmap_cm_process_qmi_dsd_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
);

extern void
qcmap_cm_process_qmi_service_availabilty
(
  qcmap_qmi_service_t  qmi_service_id
);

extern void qcmap_cm_process_qmi_timer_ind ( unsigned int msg_id, uint32 user_data );

extern void
qcmap_cm_process_sta_ind
(
  uint32 sta_cookie,
  qcmap_sta_event_t event
);

extern void
qcmap_cm_process_cradle_ind
(
  qcmap_cradle_event_t event
);

extern void
qcmap_cm_process_eth_backhaul_ind
(
  qcmap_eth_backhaul_event_t event
);

extern void
qcmap_cm_process_bt_backhaul_ind
(
  qcmap_bt_backhaul_event_t event
);

/*===========================================================================
  FUNCTION qcmap_edit_file
===========================================================================*/
/*!
@brief
 This function executes the command and copies console
 content to temporary file. Copies temporary file into
 the file provided.
 This function is kind of replacement for sed -i by
 doing cat and then cp.

@parameters
  command to be executed
  command length
  file path where the content has to be copied
  file path length

@return
  int - numeric value 0 on success, -1 otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int qcmap_edit_file(
  const char*            command_to_execute,
  unsigned int           cmdlen,
  const char*            file_path,
  unsigned int           pathlen
);

/*===========================================================================

FUNCTION QCMAP_CM_ENABLE()

DESCRIPTION

  Enable Mobile AP CM based on the config.
  It will register Mobile AP event callback.
  It will configure Modem in Mobile AP Mode and bring up RmNet between Q6 and A5.
  It will also bring up LAN if it is config.

DEPENDENCIES
  None.

RETURN VALUE
  Returns Mobile AP CM application ID on success.

  On error, return 0 and place the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_enable
(
  qcmap_cm_conf_t   *qcmap_cm_cfg,              /* Config for Mobile AP CM */
  qcmap_cm_cb_fcn    qcmap_cm_cb,               /* Callback function       */
  void              *qcmap_cm_cb_user_data,     /* Callback user data      */
  int               *qcmap_cm_errno,            /* Error condition value   */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_DISABLE()

DESCRIPTION

  Disable Mobile AP CM.
  It will teardown LAN.
  It will configure Modem in non-Mobile AP mode.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_disable
(
  int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
  int   *qcmap_cm_errno,                       /* Error condition value    */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
  FUNCTION:  qcmap_cm_write_xml

  DESCRIPTION

  This function write QCMAP CM XML based on QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_write_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);

/*===========================================================================
FUNCTION:  qcmap_cm_write_firewall_xml

DESCRIPTION

  This function write QCMAP CM Firewall XML based on QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_write_firewall_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);

/*===========================================================================
FUNCTION:  qcmap_cm_read_xml

DESCRIPTION

  This function read QCMAP CM XML and populate the QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_read_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);


/*===========================================================================
FUNCTION:  qcmap_cm_read_odu_xml

DESCRIPTION

  This function read QCMAP CM XML and populate the QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_read_odu_xml
(
  char *xml_file,                                 /* Filename and path     */
  int  *mode                                      /* ETH Mode */
);


/*===========================================================================
FUNCTION:  qcmap_cm_get_ecm_cat_ipa_xml

DESCRIPTION

  This function reads IPA XML and populate the category

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_get_ecm_cat_ipa_xml
(
  char *xml_file,                                 /* Filename and path     */
  int  *category                                      /* ETH Mode */
);

/*===========================================================================

FUNCTION:  qcmap_cm_read_firewall_xml

DESCRIPTION

  This function reads QCMAP CM Firewall XML and populate the QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_read_firewall_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);

/*===========================================================================

FUNCTION QCMAP_CM_ENABLE_STA_MODE()

DESCRIPTION
  Enable QCMobileAP CM based on the config.
  It will register QCMAP event callback.
  It will configure Modem in QCMAP Mode and bring up RmNet between Q6 and A5.
  It will also bring up LAN if it is config.

DEPENDENCIES
  None.

RETURN VALUE
  Returns QCMobileAP CM application ID on success.
  On error, return 0 and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_enable_sta_mode
(
  int    qcmap_cm_handle,                         /* Handler for QCMAP CM  */
  int   *qcmap_cm_errno                           /* error condition value */
);


/*===========================================================================

FUNCTION QCMAP_CM_DISABLE_STA_MODE()

DESCRIPTION
  Disable QCMAP CM.
  Send disable_sta_mode msg to modem.

DEPENDENCIES
  None.

RETURN VALUE
  Returns QCMAP CM application ID on success.
  On error, return 0 and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_disable_sta_mode
(
  int    qcmap_cm_handle,                           /* Handle for QCMAP CM  */
  int   *qcmap_cm_errno                             /* error condition value */
);

/*===========================================================================


FUNCTION QCMAP_CM_ERI_READ_CONFIG()

DESCRIPTION
  This function reads the passed file name to store the ERI config for
  processing.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

=============================================================================*/
int
qcmap_cm_eri_read_config
(
  int    qcmap_cm_handle,                           /* Mobile AP CM Handle   */
  char  *file_name,                                 /* ERI config file       */
  int   *qcmap_cm_errno                             /* error condition value */
);

/*===========================================================================

FUNCTION COEX_GET_WWAN_STATUS()

DESCRIPTION
  This Function retrieves the LTE frequency used by WiFi-LTE coex feature.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns TRUE.
  On error, return FALSE.

SIDE EFFECTS

===========================================================================*/
boolean coex_get_wwan_status
(
int                              *lte_frequency
);

/*===========================================================================

FUNCTION qcmap_cm_process_dsi_init_ind()

DESCRIPTION
 - Once the DSI is initialized, It sends an initication to QCMAP, to handle
   backhaul conection request.

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/
void qcmap_cm_process_dsi_init_ind(void);

/*===========================================================================

FUNCTION qcmap_cm_dsi_net_init_cb()

DESCRIPTION
 - Its a call back function passed to dsi_init_ex. After dsi is initialized,
   this call back function will be called.

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/
void qcmap_cm_dsi_net_init_cb(void);

/*============================================================
  FUNCTION qcmap_cm_nas_init
==============================================================
@brief
  API to register QMI NAS Service available call back

@return
  QCMAP_CM_ERROR - Could not register QMI NAS service callback
  QCMAP_CM_SUCCESS - Successfully registered QMI NAS service call back
@note

  - Dependencies
    - None

  - Side Effects
    - None
============================================================*/
int qcmap_cm_nas_init();


/*============================================================
  FUNCTION qmi_nas_service_available_cb
==============================================================
@brief
 QMI NAS Service Available callback handler

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
============================================================*/

void qmi_nas_service_available_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
);

/*============================================================
  FUNCTION qmi_nas_client_init
=============================================================

@brief
 QMI NAS Client Init

@return
  QCMAP_CM_ERROR in case of error
  QCMAP_CM_SUCCESS for sucess

@note

  - Dependencies
    - None

  - Side Effects
    - None
============================================================*/
int qmi_nas_client_init();

/*============================================================
  FUNCTION qmi_nas_error_cb
=============================================================

@brief
 QMI NAS Error callback handler

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
============================================================*/
void qmi_nas_error_cb();

/*============================================================
  FUNCTION qmi_nas_not_in_service
=============================================================

@brief
   Function invoked during SSR functionality, when modem is not in service.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
============================================================*/
int qmi_nas_not_in_service();

/*============================================================
  FUNCTION qmi_wds_client_init
=============================================================

@brief
 QMI WDS Client Init

@return
  QCMAP_CM_ERROR in case of error
  QCMAP_CM_SUCCESS for sucess

@note

  - Dependencies
    - None

  - Side Effects
    - None
============================================================*/
int qmi_wds_client_init();

/*===========================================================================

FUNCTION Getclientaddr

DESCRIPTION

  This function
  - Uses mac address  and vlan_id to find out the public ip address of the
    connect device.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None
==========================================================================*/
void Getclientaddr(qcmap_nl_addr_t* nl_addr, int16_t vlan_id);

/*=====================================================
  FUNCTION qcmap_cm_check_ltefdd_cat4_bw
======================================================*/
/*!
@brief
  Check whether the current RAT is LTE in FDD mode and CAT4 BW (20Mhz)

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean qcmap_cm_check_ltefdd_cat4_bw();

/*=====================================================
  FUNCTION qcmap_cm_get_wwan_roam_status
======================================================*/
/*!
@brief
  Gets the current value of Roaming status at QCMAP

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
uint8 qcmap_cm_get_wwan_roam_status();


/*===========================================================================
LOG Msg Macros
=============================================================================*/
#ifdef FEATURE_QTIMAP_OFFTARGET
#undef LOG_MSG_INFO1
#define LOG_MSG_INFO1(fmtString, x, y, z) \
  { \
    if ( x != 0 && y !=0 && z != 0) \
    fprintf(stderr, "\nINFO1:" fmtString"\n", x, y, z); \
    else if ( x != 0 && y != 0) \
    fprintf(stderr, "\nINFO1:" fmtString"\n", x, y); \
    else if ( x != 0) \
    fprintf(stderr, "\nINFO1:" fmtString"\n", x); \
    else \
    fprintf(stderr, "\nINFO1:" fmtString"\n"); \
  }
#undef LOG_MSG_INFO2
#define LOG_MSG_INFO2(fmtString, x, y, z) \
  { \
    if ( x != 0 && y !=0 && z != 0) \
    fprintf(stderr, "\nINFO1:" fmtString"\n", x, y, z); \
    else if ( x != 0 && y != 0) \
    fprintf(stderr, "\nINFO1:" fmtString"\n", x, y); \
    else if ( x != 0) \
    fprintf(stderr, "\nINFO1:" fmtString"\n", x); \
    else \
    fprintf(stderr, "\nINFO1:" fmtString"\n"); \
  }

#undef LOG_MSG_INFO3
#define LOG_MSG_INFO3(fmtString, x, y, z) \
  { \
    if ( x != 0 && y !=0 && z != 0) \
    fprintf(stderr, "\nINFO3:" fmtString"\n", x, y, z); \
    else if ( x != 0 && y != 0) \
    fprintf(stderr, "\nINFO3:" fmtString"\n", x, y); \
    else if ( x != 0) \
    fprintf(stderr, "\nINFO3:" fmtString"\n", x); \
    else \
    fprintf(stderr, "\nINFO3:" fmtString"\n"); \
  }

#undef LOG_MSG_ERROR
#define LOG_MSG_ERROR(fmtString, x, y, z) \
  { \
    if ( x != 0 && y !=0 && z != 0) \
    fprintf(stderr, "\nError:" fmtString"\n", x, y, z); \
    else if ( x != 0 && y != 0) \
    fprintf(stderr, "\nError:" fmtString"\n", x, y); \
    else if ( x != 0) \
    fprintf(stderr, "\nError:" fmtString"\n", x); \
    else \
    fprintf(stderr, "\nError:" fmtString"\n"); \
  }
#undef IPV4_ADDR_MSG
#define IPV4_ADDR_MSG(ip_addr) LOG_MSG_INFO1("IPV4 Address is %d.%d.%d.", \
              (unsigned char)(ip_addr), \
              (unsigned char)(ip_addr >> 8), \
              (unsigned char)(ip_addr >> 16))
#undef IPV6_ADDR_MSG
#define IPV6_ADDR_MSG(ip_addr) LOG_MSG_INFO1("IPV6 Address is %x.%x.%x.", \
              (uint16)(ps_ntohs(ip_addr[0])), \
              (uint16)(ps_ntohs(ip_addr[0] >> 16)), \
              (uint16)(ps_ntohs(ip_addr[0] >> 32)))
#else /* FEATURE_QTIMAP_OFFTARGET */
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);
#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}


#define IPV6_ADDR_MSG(ip_addr) MSG_8(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV6 Address %x:%x:%x:%x:%x:%x:%x:%x", \
                        (uint16)(ps_ntohs(ip_addr[0])), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[0] >> 48)), \
                        (uint16)(ps_ntohs(ip_addr[1])), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[1] >> 48)))


#define IPV4_ADDR_MSG(ip_addr) MSG_4(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV4 Address is %d.%d.%d.%d", \
                        (unsigned char)(ip_addr), \
                        (unsigned char)(ip_addr >> 8), \
                        (unsigned char)(ip_addr >> 16) , \
                        (unsigned char)(ip_addr >> 24))
#endif

/*Other required utilities*/

#define PS_IN6_IS_ADDR_UNSPECIFIED(v6)                                      \
  (((const unsigned int *) (v6))[0] == 0 &&                                 \
   ((const unsigned int *) (v6))[1] == 0 &&                                 \
   ((const unsigned int *) (v6))[2] == 0 &&                                 \
   ((const unsigned int *) (v6))[3] == 0)

inline const char* WLAN_IFACE_STRING(int num){
  switch ( num )
  {
  case 0:
    return "wlan0";
  case 1:
    return "wlan1";
  case 2:
    return "wlan2";
  case 3:
    return "wlan3";
  default:
    return NULL;
  }
}


#ifdef __cplusplus
}
#endif
#endif
