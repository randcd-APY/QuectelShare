#ifndef _QCMAP_BACKHAUL_WWAN_H_
#define _QCMAP_BACKHAUL_WWAN_H_

/*====================================================

FILE:  QCMAP_Backhaul_WWAN.h

SERVICES:
   QCMAP Connection Manager Backhaul WWAN Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/04/14   ka           Created
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
#include "pugixml/pugixml.hpp"

#include "dsi_netctrl.h"
#include "ds_list.h"
#include <map>

#ifndef QCMAP_Backhaul
class QCMAP_Backhaul;
#endif

#define TINYPROXY_CONFIG_PATH   "/etc/data/tinyproxy.conf"

#define IPV6_ADDR_SIZE_IN_BYTES      16
#define DNS_METRIC_NUMBER            100
#define DNS_METRIC_FOR_DEFAULT_PDN   10
#define MAX_DEFAULT_TABLE_LEN        100

typedef enum
{
  CONFIG_AUTOCONNECT = 1,
  CONFIG_ROAMING,
  CONFIG_V4_UMTS_PROFILE_INDEX,
  CONFIG_V4_CDMA_PROFILE_INDEX,
  CONFIG_V6_UMTS_PROFILE_INDEX,
  CONFIG_V6_CDMA_PROFILE_INDEX,
  CONFIG_TECH,
  CONFIG_DEFAULT_SIPSERVER_CONFIG_TYPE,
  CONFIG_DEFAULT_SIPSERVER_CONFIG,
  CONFIG_PREFIX_DELEGATION,
}qcmap_wwan_config_enum;


typedef enum {
  QCMAP_MSGR_DHCPV6_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_MSGR_DHCPV6_MODE_UP_V01 = 0x01, /**<  dhcpv6 is in UP mode  */
  QCMAP_MSGR_DHCPV6_MODE_DOWN_V01 = 0x02, /**<  dhcpv6 is in DOWN Mode  */
  QCMAP_MSGR_DHCPV6_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_msgr_dhcpv6_mode_enum_v01;

typedef enum modify_mode
{
  ADD_V6DNS_SERVER,
  REMOVE_V6DNS_SERVER
} mode;

typedef struct
{
  qcmap_msgr_dhcpv6_mode_enum_v01  dhcpv6_xml_state;
  qcmap_msgr_dhcpv6_mode_enum_v01  dhcpv6_enable_state;
  uint8_t  pub_ip[16];           /* public ipIP address   */
  uint8_t  pri_dns_addr[16];     /* Primary IPV6 DNS IP address   */
  uint8_t  sec_dns_addr[16];     /* Secondary IPV6 DNS IP address   */

} dhcpv6dns_conf_t;

typedef struct
{
  profile_handle_type_v01          profileHandle;
  qcmap_msgr_net_policy_info_v01   wwan_policy;
  boolean                          auto_connect;
  boolean                          roaming;
} wwan_config_params;

typedef struct
{
  unsigned int    wwan_op_ind_msg_id;
  boolean         wwan_status_ind_regd;
} qmi_client_ref_type;

typedef enum
{
  QMI_CLIENT_REG_WWAN_BRING_UP_DOWN_EVENT,
  QMI_CLIENT_REG_WWAN_STATUS_IND,
}qmi_client_ref_reg_event;

class QCMAP_Backhaul_WWAN
{
  private:
    qcmap_timer_data_t              *pTimerData_v4;
    qcmap_timer_data_t              *pTimerData_v6;
    qcmap_cm_state_e                 state;
    qcmap_cm_v6_state_e              ipv6_state;
    char                             ipv6_default_table_rule_suffix[MAX_DEFAULT_TABLE_LEN];
    char                             ipv6_default_oif_rule_suffix[MAX_DEFAULT_TABLE_LEN];

    /* Private member function */
    QCMAP_Backhaul_WWAN();
    void AddDNSv4RoutesForPDN();
    void AddDNSv6RoutesForPDN();
    static boolean FindNodeInXML(profile_handle_type_v01         profile_handle,
                                      pugi::xml_document        *xml_file,
                                      pugi::xml_node            *searchNode);
    static profile_handle_type_v01 FindFreeProfileHandle();
    static boolean IsDuplicateProfile(qcmap_msgr_net_policy_info_v01 wwan_policy);
    boolean UpdateProfileParam(qcmap_msgr_update_profile_enum_v01 update_req,
                                      qcmap_msgr_net_policy_info_v01     wwan_policy,
                                      qmi_error_type_v01  *qmi_err_num);
    boolean SwapConcurrentConfigToDefault(qmi_error_type_v01  *qmi_err_num);
    static void SetMobileAPConfigInXML(pugi::xml_node *wwan_config_node,
                                        wwan_config_params       wwan_config);
  public:
    QCMAP_Backhaul            *QcMapBackhaul;

    /* VLAN to PDN Route Enabled */
    boolean                   v4_vlan_pdn_enabled;
    boolean                   v6_vlan_pdn_enabled;

    /* AutoConnect */
    boolean                   auto_connect;
    boolean                   auto_connect_timer_running_v4;
    timer_t                   timerid_v4;
    int                       auto_timer_value_v4;
    boolean                   auto_connect_timer_running_v6;
    timer_t                   timerid_v6;
    int                       auto_timer_value_v6;

    /* Whether it has backhaul service  */
    boolean                   backhaul_service;

    static uint16             eri_roam_data_len;
    static uint8              eri_roam_data[QCMAP_WAN_MAX_ERI_DATA_SIZE];

    /* dsi related info */
    dsi_call_info_t           dsi_net_hndl;
    char                      dsi_device_name[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];

    /* dsi ipv6 related info */
    dsi_call_info_t           ipv6_dsi_net_hndl;
    char                      ipv6_dsi_device_name[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];

    /* IPv4 Primary and Secondary DNS */
    char                      pri_dns_ipv4_addr[INET_ADDRSTRLEN];
    char                      sec_dns_ipv4_addr[INET_ADDRSTRLEN];

    /* IPv6 Primary and Secondary DNS */
    char                      pri_dns_ipv6_addr[INET6_ADDRSTRLEN];
    char                      sec_dns_ipv6_addr[INET6_ADDRSTRLEN];

    qcmap_cm_wwan_conf_t                 wwan_cfg;
    static qcmap_dynamic_dns_t           ddns_conf;
    static qcmap_ddns_server_info_t      ddns_server_info;
    qcmap_cm_sip_server_info_t           default_sip_server_info;
    int                                  sip_server_list_count;
    qcmap_sip_server_info_list_t         sip_server_info_list;
    boolean ipv6_prefix_based_rules_added_for_guest_ap_1;
    boolean ipv6_prefix_based_rules_added_for_guest_ap_2;
    boolean ipv6_prefix_based_rules_added_for_guest_ap_3;
    /* Prefix Delegation Status */
    boolean prefix_delegation_activated;

    /*variable to hold dhcpv6 state*/
    dhcpv6dns_conf_t          dhcpv6_dns_conf;

    /* Map to store QMI client references */
    std::map<uint32, qmi_client_ref_type*> *qmi_client_ref;

    /* Member Functions */
    QCMAP_Backhaul_WWAN(QCMAP_Backhaul* QcMapBackhaul);

    ~QCMAP_Backhaul_WWAN();

    void Init(void *pdnCacheNode);

    profile_handle_type_v01 GetProfileHandle();

    void SetProfileHandle(profile_handle_type_v01 profile_handle);

    static boolean IsAutoConnectOnForAnyPDN();

    qcmap_cm_state_e GetState();
    void SetState(qcmap_cm_state_e state);

    qcmap_cm_v6_state_e GetIPv6State();
    void SetIPv6State(qcmap_cm_v6_state_e state);

    static int EnableIPv6
    (
     int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
     qmi_error_type_v01 *qmi_err_num             /* Error condition value     */
    );
    static int DisableIPv6
    (
     int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
     int     *qcmap_cm_errno,                    /* Error condition value     */
     qmi_error_type_v01 *qmi_err_num
    );
    static int EnableIPv4
    (
     int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
     qmi_error_type_v01 *qmi_err_num             /* Error condition value     */
    );
    static int DisableIPv4
    (
     int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
     int     *qcmap_cm_errno,                    /* Error condition value     */
     qmi_error_type_v01 *qmi_err_num
    );
    void StartAutoConnect(qcmap_msgr_ip_family_enum_v01 family);
    void StopAutoTimer(qcmap_msgr_ip_family_enum_v01 family);
    static void DSINetCB
    (
     dsi_hndl_t hndl,
     void * user_data,
     dsi_net_evt_t evt,
     dsi_evt_payload_t *payload_ptr
    );
    static void ProcessDSI_NetEvent
    (
     dsi_hndl_t hndl,
     void * user_data,
     dsi_net_evt_t evt,
     dsi_evt_payload_t *payload_ptr
    );
    int BringUpIpv4WWAN
    (
     int *qcmap_cm_errno,
     qmi_error_type_v01 *qmi_err_num
    );
    int BringUpIpv6WWAN
    (
     int *qcmap_cm_errno,
     qmi_error_type_v01 *qmi_err_num
    );
    int BringUpIpv4v6WWAN
    (
     int *qcmap_cm_errno,
     qmi_error_type_v01 *qmi_err_num
    );
    int BringDownIpv4WWAN
    (
     int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
     int    *qcmap_cm_errno,                      /* error condition value    */
     qmi_error_type_v01 *qmi_err_num              /* QMI Error type */
    );
    int BringDownIpv6WWAN
    (
     int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
     int    *qcmap_cm_errno,                      /* error condition value    */
     qmi_error_type_v01 *qmi_err_num
    );
    int BringDownIpv4v6WWAN
    (
     int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
     int    *qcmap_cm_errno,                      /* error condition value    */
     qmi_error_type_v01 *qmi_err_num
    );
    int ConnectBackHaul //qcmap_cm_connect_backhaul
    (
     int                               qcmap_cm_handle, /* Handle for MobileAP CM */
     qcmap_msgr_wwan_call_type_v01     call_type,       /* Call type to be brought UP. */
     int                               *qcmap_cm_errno, /* Error condition value  */
     qmi_error_type_v01                *qmi_err_num    /* QMI error type */
    );
    int DisconnectBackHaul  //qcmap_cm_disconnect_backhaul
    (
     int                               qcmap_cm_handle, /* Handle for MobileAP CM */
     qcmap_msgr_wwan_call_type_v01     call_type,       /* Call type to be brought down. */
     int                               *qcmap_cm_errno, /* Error condition value  */
     qmi_error_type_v01                *qmi_err_num    /* QMI error type */
    );
    void ConnectV4AndV6();
    void DisconnectV4AndV6();
    static void ConnectAllPDNs();
    static void  DisconnectAllPDNs();
    int SetAutoconnect
    (
     int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
     int     *qcmap_cm_errno,                    /* Error condition value     */
     boolean  auto_connect,                      /* Autoconnect Enable or Not */
     qmi_error_type_v01 *qmi_err_num
    );

    int GetBearerTech
    (
      const dsi_call_info_t  *dsi_net_hndl,     /* Handle for DSI net       */
      dsi_data_bearer_tech_t *bearer_tech       /* Bearer Type              */
    );

    boolean ReadWWANConfigFromCache(void *pdnCacheNode);
    boolean ReadStaticParamsFromXML();
    boolean GetSetWWANConfigFromXML(profile_handle_type_v01 profile_handle,
                                    qcmap_wwan_config_enum conf,
                                    qcmap_action_type action,
                                    char *data, int data_len
                                    );

    static boolean GetSetDefaultSIPServerConfigFromXML
                               ( qcmap_action_type,qcmap_cm_sip_server_info_t*);
    boolean GetSetSIPServerConfigFromXML
                               ( qcmap_action_type,qcmap_cm_sip_server_info_t*);

    boolean ConnectBackHaul( qcmap_msgr_wwan_call_type_v01 call_type,
                             int *err_num,
                             qmi_error_type_v01 *qmi_err_num);

    boolean DisconnectBackHaul( qcmap_msgr_wwan_call_type_v01 call_type,
                                int *err_num,
                                qmi_error_type_v01 *qmi_err_num);

    boolean SetAutoconnect( boolean enable, qmi_error_type_v01 *qmi_err_num);

    boolean GetAutoconnect();
    boolean SetRoaming( boolean enable,qmi_error_type_v01 *qmi_err_num);
    boolean GetRoaming();

    /* Get WWAN statistics */
    boolean GetWWANStatistics( qcmap_msgr_ip_family_enum_v01 ip_family,
                               qcmap_msgr_wwan_statistics_type_v01 *wwan_stats,
                               qmi_error_type_v01 *qmi_err_num);
    /* Reset WWAN statistics */
    boolean ResetWWANStatistics( qcmap_msgr_ip_family_enum_v01 ip_family,
                                 qmi_error_type_v01 *qmi_err_num);

    boolean GetWWANStatus( qcmap_msgr_wwan_call_type_v01 call_type,
                           uint8_t *status,
                           qmi_error_type_v01 *qmi_err_num);

    boolean GetWWANPolicy( qcmap_msgr_get_wwan_policy_resp_msg_v01 *wwan_policy,
                           qmi_error_type_v01 *qmi_err_num );

    static boolean GetWWANPolicyList( qcmap_msgr_wwan_policy_list_resp_msg_v01 *resp_msg,
                                     qmi_error_type_v01 *qmi_err_num );

    static boolean SetWWANPolicy( qcmap_msgr_net_policy_info_v01 wwan_policy,
                           qmi_error_type_v01  *qmi_err_num );
    static boolean CreateWWANPolicy(qcmap_msgr_net_policy_info_v01 wwan_policy,
                           profile_handle_type_v01 *profile_handle,
                           qmi_error_type_v01  *qmi_err_num );
    boolean UpdateWWANPolicy(qcmap_msgr_update_profile_enum_v01 update_req,
                           qcmap_msgr_net_policy_info_v01 WWAN_policy,
                           qmi_error_type_v01  *qmi_err_num );
    static boolean DeleteWWANPolicy(profile_handle_type_v01 profile,
                            qmi_error_type_v01 *qmi_err_num);

    /* Get the data bitrates */
    boolean GetDataBitrate( qcmap_msgr_data_bitrate_v01 *data_rate,
        qmi_error_type_v01 *qmi_err_num);

    /*Add WWAN IPv4 SIP server information*/
    boolean AddWWANIPv4SIPServerInfo();

    /*Delete WWAN IPv4 SIP server information*/
    boolean DeleteWWANIPv4SIPServerInfo();

    /*Set SIP Server information*/
    boolean SetSIPServerInfo
      (
       qcmap_msgr_sip_server_info_v01 default_sip_server_info,
       qmi_error_type_v01 *qmi_err_num
      );

    /*Get SIP Server information*/
    boolean GetSIPServerInfo (
                         qcmap_msgr_sip_server_info_v01 *default_sip_info,
                         qcmap_msgr_sip_server_info_v01 *network_sip_info,
                         int *count_network_sip_info,
                         qmi_error_type_v01 *qmi_err_num);

    /*Get IPV6 SIP Server information */
    boolean GetV6SIPServerInfo (
                         qcmap_msgr_ipv6_sip_server_info_v01 *network_v6_sip_info,
                         int *count_network_v6_sip_info,qmi_error_type_v01 *qmi_err_num);

    static int SendDeleteDelegatedPrefix (
                         boolean prefix_valid,/*Boolean to flush single or all*/
                         uint8_t *ipv6_addr,/*Prefix to delete*/
                         qmi_error_type_v01 *qmi_err_num /*QMI error number*/);

    static boolean DeleteDelegatedPrefix(void*, qmi_error_type_v01*);

    /* Prefix Delegation Config */
    static boolean GetPrefixDelegationConfig( boolean *pd_mode,
                                              qmi_error_type_v01 *qmi_err_num);

    static boolean SetPrefixDelegationConfig( boolean pd_mode,
                                              qmi_error_type_v01 *qmi_err_num);

    static boolean GetPrefixDelegationStatus( boolean *pd_mode,
                                              qmi_error_type_v01 *qmi_err_num);

    boolean AddIPv4SIPServerInfoToList(qcmap_cm_sip_server_info_t);

    boolean SendRSForClient(uint8_t *src_v6_addr);
    boolean SendRS(char *devname, uint8_t *src_v6_addr);
    boolean SendRSForClients(void);
    static boolean EnableDDNS( qmi_error_type_v01 *qmi_err_num);
    static boolean DisableDDNS( qmi_error_type_v01 *qmi_err_num);

    static boolean SetDDNS (qcmap_msgr_set_dynamic_dns_config_req_msg_v01 setddns,
                            qmi_error_type_v01 *qmi_err_num);

    static boolean GetDDNS (qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 *getddns,
                            qmi_error_type_v01 *qmi_err_num);

    boolean DeleteDDNS();
    boolean StopDDNS();
    static boolean StartDDNS(qmi_error_type_v01 *qmi_err_num);
    static boolean GetSetDDNSConfigFromXML( qcmap_action_type,
                                            qcmap_dynamic_dns_t *);
    static boolean GetTinyProxyStatus(qcmap_msgr_tiny_proxy_mode_enum_v01 *status,
                                      qmi_error_type_v01 *qmi_err_num);
    static boolean EnableTinyProxy(qmi_error_type_v01 *qmi_err_num);

    static boolean DisableTinyProxy(qmi_error_type_v01 *qmi_err_num);
    static boolean StopTinyProxy(qmi_error_type_v01 *qmi_err_num);
    static boolean GetSetTinyProxyFromXML(qcmap_action_type action,
                                          qcmap_msgr_tiny_proxy_mode_enum_v01 *proxy_config);

    static boolean SetupTinyProxy(qmi_error_type_v01 *qmi_err_num);
    boolean StopDhcpv6Dns(qmi_error_type_v01 *qmi_err_num);
    boolean StartDhcpv6Dns(qmi_error_type_v01 *qmi_err_num);
    boolean Update_resolv_file(mode mod, uint8_t pri_dns_addr[], uint8_t sec_dns_addr[]);
    boolean GetIfaceLLAddrStr(char *lladdr,char *iface);
    int GetDeviceName
    (
     int     qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
     qcmap_msgr_ip_family_enum_v01 qcmap_dev_type, /* Dev type                 */
     char   *qcmap_dev_name,                       /* Device name              */
     int    *qcmap_cm_errno                        /* Error condition value    */
    );

    int GetIPv4NetworkConfig
    (
      in_addr_t          *public_ip,
      uint32             *pri_dns_addr,
      in_addr_t          *sec_dns_addr,
      in_addr_t          *default_gw_addr,
      qmi_error_type_v01 *qmi_err_num
    );

    int GetIPv6NetworkConfig
    (
      uint8_t             public_ip[],
      uint8_t             pri_dns_addr[],
      uint8_t             sec_dns_addr[],
      qmi_error_type_v01 *qmi_err_num
    );

    bool AddVlanMapping(int16_t vlan_id, qmi_error_type_v01 *qmi_err_num);
    bool DeleteVlanMapping(int16_t vlan_id, qmi_error_type_v01 *qmi_err_num);
    boolean EnableVlanPdnRules();
    boolean DisableVlanPdnRules(qcmap_msgr_wwan_call_type_v01 call_type);
    void AddDNSRoutesForPDN(qcmap_msgr_wwan_call_type_v01 call_type);
    void DeleteDNSRoutesForPDN(qcmap_msgr_wwan_call_type_v01 call_type);
    void InsertQMIClient(uint32 qmi_client, qmi_client_ref_reg_event reg_event, int reg_event_value);
    static void RemoveQMIClient(uint32 qmi_client);
    void AddIPv6RouteForSecondaryPDN();
    void DeleteIPv6RouteForSecondaryPDN();
    static boolean IsAnyPDNConnected(qcmap_msgr_ip_family_enum_v01 v4orv6);
    /*Get MTU size of wwan ipv4 interface */
    boolean GetWWANMTUInfo(int* mtu_wwan_ipv4);
};
#endif
