#ifndef _QCMAP_BACKHAUL_H_
#define _QCMAP_BACKHAUL_H_

/*====================================================

FILE:  QCMAP_Backhaul.h

SERVICES:
   QCMAP Connection Manager Backhaul Class

=====================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
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
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_Backhaul_Ethernet.h"
#include "QCMAP_BT_Tethering.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Firewall.h"


#define WWAN_BACKHAUL          "wwan"
#define USB_CRADLE_BACKHAUL    "usb_cradle"
#define WLAN_BACKHAUL          "wlan"
#define ETH_BACKHAUL           "eth"
#define BT_BACKHAUL            "bt-pan"
#define MULTICAST_ADDR         "FF02::1"

#define RDNSS_SERVER_TYPE 25 //Recursive DNS server Type
#define INFINITE_LIFE_TIME htonl(0xffffffff) //infinite life time
#define RADISH_INFO_SIZE (3 + INET6_ADDRSTRLEN)

#define IPV6_MIN_PREFIX_LENGTH       64
#define IPV6_MAX_PREFIX_LENGTH       128
#define PDN_TABLE_ID_START           4095 /* PDN Table Start */

/*PMIP - Constants & file paths*/
#define PMIP_MODE_ENABLED  1
#define PMIP_MODE_DISABLED 0

#define PMIP_MAG_CONF_FILE "/etc/data/pmip-mag.conf"
#define TEMP_PMIP_LOG_FILE "/tmp/pmip.log"
#define PMIP_MAG_MAX_TUNL_IF 5
#define PMIP_PATH "/usr/sbin/pmip6d"
#define PMIP_PROCESS "pmip6d"
#define PROCESS_ID_OF_CMD "pidof"
#define PROCESS_CHECK_TIME 200000
#define PMIP_RESERVED_PORT 5436
#define PMIP_INVALID_ID_STRING "-1"


/* Since Backhaul has a cyclic dependency with Firewall/NATALG define them here
   first so we don't get compilation errors. */
#ifndef QCMAP_Firewall
class QCMAP_Firewall;
#endif

#ifndef QCMAP_NATALG
class QCMAP_NATALG;
#endif

typedef enum
{
CONFIG_ENABLE_IPV4 = 1,
CONFIG_ENABLE_IPV6,
CONFIG_WEBSERVER_WWAN_ACCESS,
CONFIG_ERI_CONFIG,
CONFIG_FIRST_PREFFERED_BACKHAUL,
CONFIG_SECOND_PREFFERED_BACKHAUL,
CONFIG_THIRD_PREFFERED_BACKHAUL,
CONFIG_FOURTH_PREFFERED_BACKHAUL,
CONFIG_FIFTH_PREFFERED_BACKHAUL
}qcmap_backhaul_config_enum;

struct rdnss {
  uint8_t type;
  uint8_t length;
  uint16_t reserved;
  uint32_t lifetime;
  struct in6_addr addr;
  struct in6_addr addr1;
};

typedef enum
{
  PMIP_DISABLE_EV = 0,
  PMIP_ENABLE_EV,
  PMIP_STOP_EV
}pmip_event_type;

typedef struct
{
  bool  pmipv6_mode_enabled;
  bool  pmip_debug_mode_enabled;
  bool  pmipv6_mode_active;
  bool  pmipv4_mode_enabled;
  bool  pmipv4_mode_active;
  bool isValidLMAV6Ip;
  bool isValidLMAV4Ip;
  uint8_t LMA_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint32_t LMA_v4_addr;
  qcmap_msgr_ip_family_enum_v01 pmip_tunnel_mode;
  qcmap_msgr_ip_family_enum_v01 pmip_mode_type;
  qcmap_msgr_pmipv4_mode_enum_v01 pmipv4_work_mode;
  qcmap_pmipv4_sec_router_param_v01 pmipv4_sec_router_param;
  qcmap_msgr_pmip_mobile_node_identifier_enum_v01 pmip_mobile_node_identifier_type;
  char pmip_mn_id_string[QCMAP_MSGR_PMIP_MN_ID_STRING_LENGTH_V01 + 1];
  char pmip_service_selection_string[QCMAP_MSGR_PMIP_SERVICE_SELECTION_STRING_LENGTH_V01 + 1];
}pmip_conf_type;


typedef struct
{
  uint8_t primary_ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t secondary_ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
}public_dns;


class QCMAP_Backhaul
{
  private:
    /* Private member functions */
    void InitializeStaticParams();

  public:
    //Constructor of the class
    QCMAP_Backhaul(profile_handle_type_v01 profile_handle);
    //Destructor of the class
    ~QCMAP_Backhaul();

    /* ----------------------MobileAP Backhaul Execution--------------------*/

    //ALL VARIABLES HERE//
    /* VLAN Mapping */
    int16_t vlan_id;

    /* profile handle */
    profile_handle_type_v01   profileHandle;

    /* Class Objects */
    static QCMAP_Backhaul_WLAN         *QcMapBackhaulWLAN;
    static QCMAP_Backhaul_Cradle       *QcMapBackhaulCradle;
    QCMAP_Backhaul_WWAN                *QcMapBackhaulWWAN;
    QCMAP_Firewall                     *QcMapFirewall;
    QCMAP_NATALG                       *QcMapNatAlg;
    static profile_handle_type_v01     defaultProfileHandle;

    /*Priority list for Backhaul Switching*/
    static qcmap_backhaul_type         preffered_backhaul_first;
    static qcmap_backhaul_type         preffered_backhaul_second;
    static qcmap_backhaul_type         preffered_backhaul_third;
    static qcmap_backhaul_type         preffered_backhaul_fourth;
    static qcmap_backhaul_type         preffered_backhaul_fifth;
    /*Current Backhaul*/
    static qcmap_backhaul_type         current_backhaul;

    qcmap_addr_info_list_t             external_client_addr_list;

    static bool                        enable_ipv6;
    static bool                        enable_ipv4;
    qcmap_cm_nl_prefix_info_t          ipv6_prefix_info;
    public_dns                         ipv6_public_dns;
    qcmap_cm_wan_conf_t                wan_cfg;

    /* Holds IPv4 public address of a connected interface*/
    in_addr_t                          ipv4_public_ip;
    /* Holds IPv4 public subnet mask*/
    in_addr_t                          ipv4_public_subnet_mask;
    /* Holds IPv4 gateway address of a connected interface*/
    in_addr_t                          ipv4_default_gw_addr;

    /*variable to hold tinyproxy state*/
    qcmap_msgr_tiny_proxy_mode_enum_v01  tiny_proxy_enable_state;

    static pmip_conf_type  pmip_conf;

    void Init();
    static void CreateBackhualAndRelatedObjects(profile_handle_type_v01 profile_handle,
                                                           void *pdnNode);

    static profile_handle_type_v01 GetDefaultProfileHandle();

    static boolean GetSetBackhaulConfigFromXML (
                                   qcmap_backhaul_config_enum conf,
                                   qcmap_action_type action,
                                   char *data,int datalen );

    static boolean GetSetPmipConfigFromXML
    (
      qcmap_action_type action,
      qcmap_msgr_get_pmip_mode_resp_msg_v01 *resp_ptr
    );

    static boolean SetBackhaulPrioConfigToXML(
                            qcmap_msgr_backhaul_type_enum_v01 backhaul_type,
                            qcmap_backhaul_config_enum backhaul_config);


    //Read Backhaul Config from XML
    boolean ReadBackhaulConfigFromXML ();
    boolean GetNetworkConfig(in_addr_t *public_ip,
                                uint32 *primary_dns,
                                in_addr_t *secondary_dns,
                                qmi_error_type_v01 *qmi_err_num);

    boolean GetIPv6NetworkConfig(uint8_t public_ip[],
                                    uint8_t primary_dns[],
                                    uint8_t secondary_dns[],
                                    qmi_error_type_v01 *qmi_err_num);

    /* Enable IPV4 */
    boolean EnableIPV4 (qmi_error_type_v01 *qmi_err_num);

    /* Disable IPV4 */
    boolean DisableIPV4(qmi_error_type_v01 *qmi_err_num);

    /* Enable IPV6 */
    boolean EnableIPV6(qmi_error_type_v01 *qmi_err_num);
    /* Disable IPV6 */
    boolean DisableIPV6(qmi_error_type_v01 *qmi_err_num);
    /* Validates the prefix present in RA packet. */
    boolean ValidateGlobalV6addr(struct ps_in6_addr * , int16_t *vlan_id);
    /* Enable IPV6 Forwarding */
    boolean EnableIPV6Forwarding(void);
    /* Disable IPV6 Forwarding */
    boolean DisableIPV6Forwarding(boolean send_ra);
    /* Get Backhaul IPV6 Prefix Info. */
    void GetIPV6PrefixInfo(char *devname,
                          qcmap_cm_nl_prefix_info_t   *ipv6_prefix_info);

   /* Update Global Ipv6 from Received RA message */
   void UpdateGlobalV6addr(qcmap_nl_addr_t* nl_addr, boolean validate_addr);

    /* Assign Global Ipv6 address to all client */
    bool AssignGlobalV6AddrAllClients( struct ps_in6_addr *prefix_addr_ptr,
                                       int16_t vlan_id);
    int UpdatePrefix( qcmap_cm_nl_prefix_info_t *prefix_info,
                          boolean deprecate,boolean send_ra,
                          uint8_t *dest_v6_ip );
    static boolean SendRS(char *devname, uint8_t *src_v6_addr);
    boolean SendRSForClient(uint8_t*);
    boolean SendRSForClients();
    static void SendRSwithUnspecifiedSrc(qcmap_nl_sock_msg_t* qcmap_nl_buf);
    static boolean IsNonWWANBackhaulActive();
    void StartRadish(void);
    void StopRadish(void);
    void RestartRadish(void);
    boolean IsRadishRunning(void);
    static boolean GetIPv4State( uint8_t *status,
                                     qmi_error_type_v01 *qmi_err_num );
    static boolean GetIPv6State( uint8_t *status,
                                     qmi_error_type_v01 *qmi_err_num );

    void DeletePPPIPv6Route();
    static boolean IsPPPkilled();
    void AddPPPIPv6Route();
    /*Deleting the Global Ipv6 address when the Ipv6 call is brought down*/
    boolean DeleteGlobalipv6AddressInfo ();
    boolean DeprecateClientRA(void);
    boolean GetIP(uint32 *staIP, uint32 *netMask, char *iface);

    /* Configure Webserver WWAN Access. */
    static boolean SetWebserverWWANAccess (
                                     boolean enable,
                                     qmi_error_type_v01 *qmi_err_num );

    boolean SetWebserverWWANAccessOnApps (
                                     boolean enable,
                                     qmi_error_type_v01 *qmi_err_num );

    static boolean GetWebserverWWANAccessFlag (
                                      uint8 *flag,
                                      qmi_error_type_v01 *qmi_err_num );
    boolean ComparePriority(qcmap_backhaul_type intf1,qcmap_backhaul_type intf2);
    void BlockWWANAccessOnIface(ip_version_enum_type ip_vsn, char *devname);
    void ClearIfaceRelatedRulesForV6(char *devname);
    void ClearIfaceRelatedRulesForV4(char *devname);
    boolean RemoveIPV6Address(char *devname);

    void StopDHCPCD(char* iface);

    /* Send RS on Bridge Iface*/
    void SendRSOnBridgeIface(void);

    /* IPv6 Proxy functions */
    static void ProcessNS(qcmap_nl_sock_msg_t* qcmap_nl_buf);
    static void ProcessNA(qcmap_nl_addr_t* nl_addr);
    void SendProxyNS(char *nd_packet_buf);
    void SendProxyNA(struct ps_in6_addr *dst_v6_addr, char *nd_packet_buf, uint8 *src_mac_addr);

    /* Router mode External Routing Functions */
    static void AddV6ExternalRoute(qcmap_nl_addr_t* nl_addr, char* iface);
    static void DeleteV6ExternalRoute(uint8_t* v6_addr, char* iface);
    static void DeleteAllV6ExternalRoute(char* iface);

    /*Join Multicast group*/
    static void JoinMulticastGroup (qcmap_v6_sock_iface_t iface_type, char* iface);

    /* Add default gateway information from RA*/
    void AddIPv6DefaultRouteInfo(qcmap_nl_addr_t *nladdr);
    void SwitchToWWANBackhaul();
    static void SwitchToOtherBackhaul(
                                   qcmap_backhaul_type backhaul_type,
                                   boolean allow_ipv4,
                                   boolean allow_ipv6);
    static boolean SwitchToBackhaul(qcmap_backhaul_type backhaul_type);

    static inline void AddRdnssInfo(struct rdnss *rdns_info)
    {
      rdns_info->type = RDNSS_SERVER_TYPE;
      rdns_info->reserved = 0;
      rdns_info->lifetime= INFINITE_LIFE_TIME;
    }
    void AddIPv6PrefixBasedRules(int ap_dev_num);
    void RemoveIPv6PrefixBasedRules(int ap_dev_num);

    int GetBackhaulStatus(
                         qcmap_cm_backhaul_status *backhaul_status,
                         qmi_error_type_v01 *qmi_err_num);
    int GetDeviceName( char*, qcmap_msgr_ip_family_enum_v01, qmi_error_type_v01*);

    boolean IsBackhaulRouterModeEnabled();

    pugi::xml_node GetPdnRootXML(pugi::xml_document *xml_file);

    void SwapNatAlgObjectWithDefault();
    void SwapFirewallObjectWithDefault();
    void SwapVlanIdWithDefault();
    void SendBackhaulStatusInd(
                              int backhaul_v4_available,
                              int backhaul_v6_available,
                              qcmap_backhaul_type backhaul_type);
    static void SendPacketStatIndOnBackhaulSwitch();

    /*PMIPv6 related functionality*/
    static boolean IsPmipV6ModeEnabled();
    static boolean IsPmipV4ModeEnabled();
    static boolean GetPmipConfig(qcmap_msgr_get_pmip_mode_resp_msg_v01  *resp_ptr,
                                      qmi_error_type_v01 *qmi_err_num);
    static boolean SetPmipMode(qcmap_msgr_set_pmip_mode_req_msg_v01  *req_ptr,
                                    qmi_error_type_v01 *qmi_err_num);

     boolean EnablePmipMode(qmi_error_type_v01 *qmi_err_num);
     boolean DisablePmipMode(qmi_error_type_v01 *qmi_err_num, pmip_event_type action);
     boolean PmipQueryDevice(qcmap_pmip_info_t* conn_dev);
     boolean PmipDeleteDevice(qcmap_cm_client_data_info_t* conn_dev);
     static boolean WritePMIPConfToXML();

     static boolean ReadPMIPConfFromXML();
     boolean updatePMIPConfigFile();
     boolean startPMIPprocess();
     boolean stopPMIPprocess();
     static boolean checkProcessExist(char * proc);
     static QCMAP_Backhaul* GetBackhaulObjectForWWANInterface(qcmap_msgr_ip_family_enum_v01 ip_family, const char *rmnet_iface);

};

#endif
