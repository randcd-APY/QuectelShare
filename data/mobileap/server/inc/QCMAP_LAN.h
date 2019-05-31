#ifndef _QCMAP_LAN_H_
#define _QCMAP_LAN_H_

/*====================================================

FILE:  QCMAP_LAN.h

SERVICES:
   QCMAP LAN Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/26/14   ka           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "qcmap_cm_api.h"

#define GatewayURL_TAG                  "GatewayURL"
#define WWAN_Reserved_IP                "169.254.5.1"
#define DHCP_RESERVATION_TMP_FILE       "/tmp/tmp_reservation_file"
#define XLATIP                          "192.0.0.0"
#define XLATNETWORK                     "255.255.255.248"

//(strlen(RESOLV_FILE_PATH_PREFIX) + max of 4 VLAN ID digits + '\0' character)
#define MAX_RESOLV_FILE_PATH_LEN        32
#define RESOLV_FILE_PATH_PREFIX         "/var/run/resolv.conf.bridge"

//(strlen(LEASE_FILE_PATH_PREFIX) + max of 4 VLAN ID digits + '\0' character)
#define MAX_LEASE_FILE_PATH_LEN         35
#define LEASE_FILE_PATH_PREFIX          "/var/run/dnsmasq.leases.bridge"

//(strlen(PID_FILE_PATH_PREFIX) + max of 4 VLAN ID digits + '\0' character)
#define MAX_PID_FILE_PATH_LEN           32
#define PID_FILE_PATH_PREFIX            "/var/run/dnsmasq.pid.bridge"

//(strlen(QCMAP_HOST_NAME_PREFIX) + max of 4 VLAN ID digits + '\0' character)
#define  MAX_HOST_NAME_LEN              23
#define  QCMAP_HOST_NAME_PREFIX         "/data/hosts.bridge"


#define ETH_VLAN_ENABLED   0b00000100
#define ECM_VLAN_ENABLED   0b00000010
#define RNDIS_VLAN_ENABLED 0b00000001

#define DEFAULT_BRIDGE_ID 0 //default QCMAP_LAN object will have birdge_vlan_id 0
#define IS_DEFAULT_BRIDGE(bridge_id)  (bridge_id == DEFAULT_BRIDGE_ID)

typedef enum {
  CONFIG_START_IP = 1,
  CONFIG_END_IP,
  CONFIG_LEASE_TIME,
  CONFIG_AP_IP,
  CONFIG_SUBNET_MASK,
  CONFIG_ENABLE_DHCP
}qcmap_lan_config_enum;


class QCMAP_LAN
{
  private:
    bool bridge_inited;
    qcmap_cm_lan_conf_t lan_cfg;
    qcmap_msgr_lan_config_v01 prev_lan_config;

    /* Variable used to store the local mac address assigned to bridge on bootup */
    struct ether_addr bridge_local_mac_addr;

    /* Socket used for sending RA's to deprecate prefix. */
    int bridge_sock;

    /* String to hold the DHCP command. Useful when LAN config has changed, but LAN has not been
      reactivated */
    char dhcp_command_str[MAX_COMMAND_STR_LEN];

    bool enable_dns;

    //eth,rndis,ecm bitmask indicating virtual iface bridged to this class int16_t vlan_id
    uint8_t vlan_iface_mask;
    int16_t bridge_vlan_id;
    bool    ipa_offload_enabled;
    profile_handle_type_v01 profile_handle;
    bool auto_reboot_flag;

    char bridge_name[QCMAP_MAX_IFACE_NAME_SIZE_V01];
    char dnsmasq_resolv_file_path[MAX_RESOLV_FILE_PATH_LEN];
    char dnsmasq_lease_file_path[MAX_LEASE_FILE_PATH_LEN];
    char dnsmasq_pid_file_path[MAX_PID_FILE_PATH_LEN];
    char host_file_path[MAX_HOST_NAME_LEN];

    bool AddDHCPResRecEntryToList(qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
                                  qcmap_msgr_dhcp_reservation_v01* dhcp_record);

    bool AddDHCPV6ProxyDNSOption(char dhcp_command[]) const;

    bool UpdateIPAWithVLANIOCTL(bool isAdd);
    /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
    bool UpdateLANConfigToIPACM(void) const;
#endif /* FEATURE_DATA_TARGET_MDM9607 */

    bool SetLanInXlatRangeToDefault(qcmap_msgr_lan_config_v01 *lan_config);

    bool EnableGatewayUrl(void) const;
    bool SetGatewayConfigToXML(const char*) const;

    // Utility function to check for update in lan/wlan configs
    bool IsLanCfgUpdated(void) const;

    void sync_dhcp_hosts(void);

  public:
    QCMAP_LAN(int16_t bridge_vlan_id);
    ~QCMAP_LAN(void);

    bool IsBridgeInited(void) const;

    bool SetLANConfigToXML(qcmap_msgr_lan_config_v01 *);
    bool SetLANConfigToXML(void);

    bool ReadLANConfigFromXML(void);

    /* Set LAN Mode. */
    bool SetLANConfig(qcmap_msgr_lan_config_v01 *lan_config, qmi_error_type_v01 *qmi_err_num);

    /* Get Configured LAN Mode */
    bool GetLANConfig(qcmap_msgr_lan_config_v01 *lan_config, qmi_error_type_v01 *qmi_err_num) const;
    const qcmap_cm_lan_conf_t* GetLANConfig(void) const;
    const qcmap_msgr_lan_config_v01* GetPrevLANConfig(void) const;

    /* Activate LAN Request. */
    bool ActivateLAN( qmi_error_type_v01 *qmi_err_num,
                                     bool admin = false );

    /*PMIP restart LAN*/
    bool PmipRestartLAN(qmi_error_type_v01 *qmi_err_num, pmip_event_type event);

    bool StartDHCPD(void);
    bool StopDHCPD(void);

    struct ether_addr GetBridgeMacAddr(void) const;

    int GetBridgeSockFd(void) const;

    /*DHCP Reservation*/
    /* Add a DHCP Reservation Record to the configuration and update XML file. */
    bool AddDHCPReservRecord(qcmap_msgr_dhcp_reservation_v01* dhcp_reservation_record,
                             qmi_error_type_v01 *qmi_err_num);

    bool AddDHCPResRecToXML(qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record);
    bool EditDHCPResRecToXML(uint32_t client_ip,
                            qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record);
    bool DeleteDHCPResRecFromXML(uint32_t client_ip);

    /* Edit a DHCP Reservation Record from the configuration and update XML file. */
    bool EditDHCPReservRecord(uint32_t* client_ip,
                              qcmap_msgr_dhcp_reservation_v01* dhcp_reservation_record,
                              qmi_error_type_v01* qmi_err_num);

    /* Delete a DHCP Reservation Record from the configuration and update XML file. */
    bool DeleteDHCPReservRecord(uint32_t* client_reserved_ip,
                                qmi_error_type_v01* qmi_err_num);

    /* Get DHCP Reservation Records from the XML configuration. */
    bool GetDHCPReservRecords
    (
     qcmap_msgr_dhcp_reservation_v01* dhcp_reservation_record,
     unsigned int* num_entries,
     qmi_error_type_v01* qmi_err_num
    ) const;

    /* Update DHCPD daemon with network SIP server, MTU info */
    bool UpdateDHCPDNetworkInfo();

    /* Enable DNS */
    bool EnableDNS(void);
    void AddDNSNameServers(char* pri_dns_addr, char* sec_dns_addr);
    void DeleteDNSNameServers(char* pri_dns_addr, char* sec_dns_addr);
    bool IsDNSEnabled(void) const;
    void SetDNSEnableFlag(bool flag);

    bool CheckforAddrConflict(qcmap_msgr_lan_config_v01 *lan_config,
                              qcmap_msgr_station_mode_config_v01 *station_config) const;

    bool AddEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t *data);
    bool DelEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t *data);

    bool DelEbtablesUSBRulesForBridgeMode(void);
    bool AddEbtablesUSBRulesForBridgeMode(void);

    bool GetGatewayUrl(char *url, uint32_t *url_len,
                                  qmi_error_type_v01 *qmi_err_num) const;
    bool SetGatewayUrl(const char *url, uint32_t url_len, qmi_error_type_v01* qmi_err_num);

    /* Intialize Bridge Interface. */
    void InitBridge();
    void DelBridge();
    void DeleteLANConfigFromXML(bool updateIPA);
    void  AddPhyInterfaceToDefaultBridge
    (
      qcmap_msgr_interface_type_enum_v01 iface_type
     );
    void CheckInterfacesWithVLAN();
    void AllowBackhaulAccessOnIface(ip_version_enum_type ip_vsn, char *devname);
    boolean BlockIPv4WWANAccess(bool clean_up=false);
    boolean BlockIPv6WWANAccess(bool clean_up=false);
    int CreateRawSocket(const char *devname) const;

    bool EnableIPPassthrough
    (
      char *mac,
      char *host,
      qcmap_msgr_device_type_enum_v01 dev_type
    );

    bool DisableIPPassthrough(bool default_route);
    bool SetIPPassthroughConfig
    (
      qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
      bool new_config ,
      qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
      qmi_error_type_v01 * qmi_err_num
    );

    bool GetIPPassthroughConfig
    (
      qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
      qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
      qmi_error_type_v01 *qmi_err_num
    ) const;

    bool RestartTetheredClient
    (
      qcmap_msgr_device_type_enum_v01 dev_type
    ) const;

    bool GetIPPassthroughState(qmi_error_type_v01 *qmi_err_num) const;

    const char* GetBridgeNameAsCStr(void) const;

    bool SetBackhaulProfileHandle(profile_handle_type_v01 profile_handle);
    profile_handle_type_v01 GetBackhaulProfileHandle(void) const;
    int16_t GetVLANID(void) const;
    uint8_t GetVLANIfaceMask(void) const;
    bool AssociateVLANIface(qcmap_msgr_interface_type_enum_v01 iface_type);
    bool DisassociateVLANIface(qcmap_msgr_interface_type_enum_v01 iface_type);
    void SetIPAoffload(bool ipa_offload);

    /*Toggles Autoneg parameter for ethernet iface*/
    static inline void ToggleAutoNegForEth(char* iface)
    {
      char command[MAX_COMMAND_STR_LEN];
      snprintf(command, MAX_COMMAND_STR_LEN, "ethtool -s %s autoneg off",iface);
      ds_system_call(command, strlen(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "ethtool -s %s autoneg on",iface);
      ds_system_call(command, strlen(command));
    }

    static bool check_non_empty_mac_addr(uint8 *mac, char mac_addr_string[]);

    static long int qcmap_match_device_type(const void *first, const void *second);
};
#endif
