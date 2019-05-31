#ifndef _QCMAP_CONNECTION_MANAGER_H_
#define _QCMAP_CONNECTION_MANAGER_H_

/*====================================================

FILE:  QCMAP_ConnectionManager.h

SERVICES:
   QCMAP Connection Manager Class

=====================================================

  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/11/12   gk         9x25
  10/26/12   cp         Added support for Dual AP and different types of NAT.
  02/27/13   cp         Added support for deprecating of prefix when switching
                        between station mode and WWAN mode.
  04/16/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
  06/12/13   sg         Added DHCP Reservation feature
  09/17/13   at         Added support to Enable/Disable ALGs
  01/11/14   sr         Added support for connected devices in SoftAP
  02/24/14   vm         Changes to Enable/Disable Station Mode to be in
                        accordance with IoE 9x15
  03/27/14   cp         Added support to DUN+SoftAP.
  06/06/14   rk         Added support DHCP option 26 for mtu in dnsmasq/dhcp server.
  08/08/14   rk         kw fixes.
  03/28/17   spr        Added support for Multi-PDN.
=====================================================================*/

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
#include "QCMAP_Backhaul.h"
#include "QCMAP_LAN.h"
#include <map>

//===================================================================
//              Class Definitions
//===================================================================

#define IPV6_NEIGH_ADDR_TABLE                  "/tmp/ipv6neighaddr.txt"
#define DNSMASQ_LEASE_FILE                     "/var/run/dnsmasq.leases"
#define DNSMASQ_CONFIG_FILE                    "/etc/data/dnsmasq.conf"
#define DNSMASQ_RESOLV_FILE                    "/etc/resolv.conf"
#define DNSMASQ_PID_FILE                       "/var/run/dnsmasq.pid"

#define TEMP_HOST_FILE                         "/tmp/dnsmasq_host.txt"
#define TEMP_LEASE_FILE                        "/tmp/tmp.leases"
#define QMI_MAX_TIMEOUT_MS                     10000
#define USB_CLK_UPDATE_NODE                    "/sys/class/udc/msm_hsusb/device/perf_mode"
#define IPv6_CONNTRACK                         "/tmp/ipv6conntrack.txt"
#define IPV6_ADDR_OFFSET                       4

using std::map;
using std::pair;

class QCMAP_ConnectionManager
{
  private:
    static bool flag;
    static QCMAP_ConnectionManager* object;
    QCMAP_ConnectionManager();
    void init();

  public:
    bool data_path_opt_enable;
    bool DUN_SoftAP_enable_flag;
    bool dual_wifi_mode;
    bool packet_stats_enabled;
    bool sfe_loaded;
    bool dont_bringdown_backhaul;


    // Data Structure to hold AP-STA v6 bridge info for NS/NA proxy
    qcmap_ap_sta_bridge_t ap_sta_bridge;

    static std::map <profile_handle_type_v01, QCMAP_Backhaul *> *QCMAP_Backhaul_Hash;

    int       wlan_restart_delay;

    /* Use for Signal the LAN/WAN connected */
    pthread_mutex_t             cm_mutex;
    pthread_cond_t              cm_cond;
    /* STA iface device number */
    int       sta_iface_index;
    /* AP iface device numbers */
    int       ap_dev_num1;
    int       ap_dev_num2;
    int       ap_dev_num3;
    int       ap_dev_num4;
    int       unused_wlan_if;
    /*To track qcmap tear down*/
    static boolean                     qcmap_tear_down_in_progress;

    /* This variable is used to revert back to ap-sta router mode when
       a failure occurs in ap-sta bridge mode */
    boolean revert_ap_sta_router_mode;

    /*This variable is used to know whether ap-sta router mode due to fallback
      from bridge or not. This is made true when revert_ap_sta_router_mode is
      made true and false when wlan is started with bridge mode again.
      This variable is used in dual wifi to ignore SCM dynamic disable and
      enable indications when fallback is in progress*/
    boolean fallback_ap_sta_router_mode;

    /*Connected Device Info storage*/
    qcmap_cm_connected_info_t  conn_device_info;
    qcmap_addr_info_list_t addrList;
    qcmap_addr_info_list_t addrList_prev;
    /* XML file name */
    static char xml_path[QCMAP_CM_MAX_FILE_LEN];
    static char firewall_xml_path[QCMAP_CM_MAX_FILE_LEN];
    /* QCMAP CM Config */
    boolean           qcmap_enable;
    qcmap_cm_conf_t   cfg;
    /* Below variable is being used in GetWWANMTUInfo() which is invoked during constructor
           execution and this has to be referenced using dereference operator.*/
    static int    qcmap_cm_handle;

    /* QCMAP CM CB and Client Information Ref Ptr */
    unsigned int* qcmap_cb_handle;

    ds_target_t target;

    boolean usb_clk_node_present;

    qcmap_cm_dynamic_clock_info_t dyn_clk_info;
    boolean is_ssr_in_progress;
    static boolean is_cm_enabled;

    inline unsigned int* GetQCMAPClientCBDatata(void)
           { return this->qcmap_cb_handle; }

    static QCMAP_ConnectionManager *Get_Instance\
                                  (char *xml_path = NULL,boolean obj_create =0);
    ~QCMAP_ConnectionManager();

    /* ---------------------------MobileAP Execution---------------------------*/

    /* Enable MobileAP */
    static boolean Enable(int *handle, void *cb_user_data, \
                                               qmi_error_type_v01 *qmi_err_num);

    /* Disable MobileAP */
    static boolean Disable(int *err_num, qmi_error_type_v01 *qmi_err_num);
    static boolean Disable_Ext   (int *err_num, qmi_error_type_v01 *qmi_err_num, \
                                       boolean sig_term_flag);

    static boolean CheckDUNSoftAPMode(void);

    /* Disable MobileAP handle. */
    void DisableHandle(void);

    /* ----------------------------Modem Config-------------------------*/

    /* Display the current modem configuration. */
    boolean DisplayModemConfig(void);

    /* -----------------------------Linux Config--------------------------*/

    static void CleanIPtables(void);
    static void CleanEbtables(void);


    /*Handle Connected Device events*/
    static bool AddNewDeviceEntry
    (
      void* mac_addr,
      qcmap_msgr_device_type_enum_v01 device_type,
      uint32* ip_addr,
      uint8* ipv6_addr,
      int16_t vlan_id = 0,
      bool isIpv4Valid = false,
      bool isIPv6Valid = false
    );
    /* Start, stop and restart the linux MCAST routing daemon. */
    void StartMcastDaemon(void);
    void StopMcastDaemon(void);
    void  strtolower(uint8 *str);

    int readable_addr(int domain,const uint32_t *addr, char *str);

    static int GetMobileAPhandle(qmi_error_type_v01 *qmi_err_num);

    /* Set/get QCMAP bootup fucntions*/
    static boolean SetQCMAPBootupConfig
    (
      qcmap_msgr_bootup_flag_v01 mobileap_enable,
      qcmap_msgr_bootup_flag_v01 wlan_enable,
      qmi_error_type_v01 *qmi_err_num
    );

    static boolean GetQCMAPBootupConfig
    (
      qcmap_msgr_bootup_flag_v01 *mobileap_enable,
      qcmap_msgr_bootup_flag_v01 *wlan_enable,
      qmi_error_type_v01 *qmi_err_num
    );

    // Get MobileAPEnable on bootup Flag
    static inline boolean get_mobileap_enable_cfg_flag(void)
    {
      QCMAP_ConnectionManager* QcMapMgr =
                             QCMAP_ConnectionManager::Get_Instance(NULL, false);
      return QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup;
    }

    // Get WLAN Enable on bootup flag
    static inline boolean get_wlan_enable_cfg_flag(void)
    {
      QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
      return QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup;
    }

    // Get GSB Enable on bootup flag
    static inline boolean get_gsb_enable_cfg_flag(void)
    {
      QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
      return QcMapMgr->cfg.bootup_config.enable_gsb_at_bootup;
    }

    /* get kernel version */
    boolean GetKernelVer(char *version);
    /* compare kernel version */
    int CompareKernelVer(char *compare_kernel_ver);

    //Get number of connected devices
    inline int32 GetNumofConnectedDevices (void)
    {
      return conn_device_info.numOfNodes ;
    }

    /* Check if a WLAN client is connected */
    inline bool CheckWlanClientConnected (void)
    {
      return conn_device_info.wlanclientpresent;
    }

    /* Find match a match for MAC address in the linked list*/
    static int MatchMacAddrInList(qcmap_nl_sock_msg_t* qcmap_nl_buf);

    /*Deleting information for a device from list*/
    static bool DeleteDeviceEntryInfo
                                 ( void* mac_addr,
                                   qcmap_nl_addr_t *nl_addr,
                                   qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_USB_V01,
                                   qcmap_cm_cdi_del_evt_t del_evt = QCMAP_DELNEIGH,
                                   int16_t vlan_id = 0);


    static boolean RestoreFactoryConfig(qmi_error_type_v01 *qmi_err_num);

    /*Get Connected Devices Information*/
    static boolean GetConnectedDevicesInfo
                            ( qcmap_msgr_connected_device_info_v01 *conn_devices,
                              unsigned int* num_entries,
                              qmi_error_type_v01 *qmi_err_num);

    /* Write QCMAP config to XML file */
    static boolean WriteConfigToXML(qcmap_update_xml, pugi::xml_document*);

    boolean ReadQCMAPConfigFromXML(char *);
    static int IsInterfaceEnabled(char *interface);

    /* Find Gateway IP address assigned to the STA interface */
    boolean GetGatewayIP(uint32 *gwIP);

    static long int qcmap_match_v6_iid(const void *first,
                                               const void *second);
    static long int qcmap_match_v6_addr(const void *first,
                                               const void *second);
    static long int qcmap_match_mac_address( const void *first,
                                                          const void *second);
    static long int qcmap_match_mac_addr_vlan_id_pair(const void *first,
                                                      const void *second);
    static long int qcmap_v6_proxy_match(const void *first, const void *second);
    static long int qcmap_match_proxy_entry(const void *first, const void *second);

    static boolean GetSetBootupConfigFromXML
                                        ( qcmap_bootup_config_enum conf,
                                          qcmap_action_type action,boolean mode);
    static void WriteDefaultsToXML();

    /* Utility function */
    static int GetHWAddr(char *mac,char* devname);
    void DeleteConnectedDevicesClients
                                  ( qcmap_msgr_device_type_enum_v01 device_type );

    /* Update device entry info in the linked list as needed*/
    static bool UpdateDeviceEntryInfo(qcmap_nl_sock_msg_t* qcmap_nl_buf,
                                      qcmap_msgr_device_type_enum_v01  device_entry_type);

    void DelV6Conntrack( uint8* ipv6addr, uint8* macaddr);

    static void fetchHostName(char *host, char *mac_str);

    void FetchIpv6Conntrack(char* ipv6addr);

    void CompareIPv6PrefixAndFlushConntrack(char* ipv6addr);


    boolean IsDynamicClockVotingNeeded();

    void UpdateCDIWithNeighInfo(qcmap_nl_sock_msg_t* pktbuf);
    boolean TraverseCDIAndUpdate(qcmap_update_cdi action,
                                qcmap_msgr_connected_device_info_v01 *conn_devices,
                                unsigned int* num_entries);

    /* Vote for new USB Clock frequency */
    void SetUSBClockFreq ( boolean setNOM );

    inline bool CheckUsbClockAtNom(void)
    {
      return dyn_clk_info.usb_clock_at_nom;
    }


    inline bool IsRatLteFdd(void)
    {
      return dyn_clk_info.rat_lte_fdd;
    }

    inline bool isBWLteCAT4(void)
    {
      return dyn_clk_info.lte_cat4_bw;
    }

    boolean EnablePacketStats(qmi_error_type_v01 *qmi_err_num);
    boolean DisablePacketStats(qmi_error_type_v01 *qmi_err_num);
    boolean ResetPacketStats(qmi_error_type_v01 *qmi_err_num);
    boolean GetPacketStatsStatus(qmi_error_type_v01 *qmi_err_num);
    boolean SendPacketStatsInd(qcmap_msgr_packet_stats_state_enum_v01 status,
                            int number_of_entries,
                            qcmap_cm_client_data_info_t* dev_info);
    QCMAP_Backhaul* GetBackhaulObjectFromMap(profile_handle_type_v01 profile_handle);
    QCMAP_Backhaul* GetBackhaulObjectFromVLAN(int16_t vlan_id);
    void InsertBackhaulObjectToMap(profile_handle_type_v01 profile_handle, QCMAP_Backhaul *obj);
    boolean DataPathOptInit(boolean flag);
    boolean SetDataPathOptManagerStatus(boolean data_path_opt_status, qmi_error_type_v01 *qmi_err_num);
    boolean GetDataPathOptManagerStatus( boolean *data_path_opt_status, qmi_error_type_v01 *qmi_err_num);
    static void RemoveBackhaulObjectFromMap(profile_handle_type_v01 profile_handle);
    static uint8 BackhaulObjectCountFromMap();
    static QCMAP_Backhaul_WWAN *GetBackhaulWWANObject(profile_handle_type_v01 profile_handle);
    static QCMAP_NATALG *GetNatAlgObject(profile_handle_type_v01 profile_handle);
    static QCMAP_LAN *GetLANObject(int16_t vlan_id);
    static QCMAP_Firewall *GetFirewallObject(profile_handle_type_v01 profile_handle);
    static QCMAP_Backhaul *QCMAP_ConnectionManager::GetBackhaulObject(profile_handle_type_v01 profile_handle);
    static QCMAP_Backhaul *QCMAP_ConnectionManager::GetBackhaulObjectFromVLANid(int16_t vlan_id);
    static const char *GetAssociatedBridgeIface(int16_t vlan_id);
    bool TriggerAutoReboot(bool auto_reboot_flag,
                                qcmap_cm_auto_reboot_reason_code reason_code) const;
    boolean SendNSOnBridgeIface(int16_t vlan_id, uint8_t *dst_v6_addr);
};

/* Declare iterator for map */
#define DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL    map<profile_handle_type_v01, QCMAP_Backhaul*>::iterator it

/* Assign iterator to start of map */
#define START_OF_HASH_MAP_FOR_BACKHAUL(map)              \
                                    if(map != NULL)      \
                                      it = map->begin(); \

/* Has iterator reached end of map */
#define END_OF_HASH_MAP_FOR_BACKHAUL(map)         (map != NULL && it != map->end())

/* Get Profile Handle, in hashmap Key->ProfileHandle, Value->Backhaul_Object  */
#define GET_CURRENT_PROFILE_HANDLE                it->first
/* Gets Backhaul object reference */
#define GET_CURRENT_BACKHAUL_OBJECT               it->second

/* Gets Backhual instance and increment iterator */
#define GET_BACKHAUL_OBJ_AND_INC                  (it++)->second

/* Gets Backhaul WLAN instance. */
#define GET_BACKHAUL_WLAN_OBJ                     it->second->QcMapBackhaulWLAN

/* Gets Backhaul Cradle instance. */
#define GET_BACKHAUL_CRADLE_OBJ                   it->second->QcMapBackhaulCradle

/* Gets Backhual WWAN instance. */
#define GET_BACKHAUL_WWAN_OBJ                     it->second->QcMapBackhaulWWAN

/* Gets Backhaul WWAN instance and increments iterator. */
#define GET_BACKHAUL_WWAN_OBJ_AND_INC             (it++)->second->QcMapBackhaulWWAN;

/* Gets Firewall instance and increments iterator. */
#define GET_FIREWALL_OBJ_AND_INC                  (it++)->second->QcMapFirewall;

/* Gets NAT/ALG instance and increments iterator. */
#define GET_NAT_ALG_OBJ_AND_INC                   (it++)->second->QcMapNatAlg;

/* Increment iterator */
#define INCREMENT_ITERATOR                        it++
#define NEXT_ELEMENT_IN_HASH_MAP                  ++it

/* Get Backhaul object for a profile */
#define GET_BACKHAUL_OBJECT(profile)              \
            QCMAP_ConnectionManager::GetBackhaulObject(profile)

/* Get Default Backhaul Object */
#define GET_DEFAULT_BACKHAUL_OBJECT()             \
            QCMAP_ConnectionManager::GetBackhaulObject(QCMAP_Backhaul::defaultProfileHandle)

/* Get Backhaul object for a profile */
#define GET_BACKHAUL_OBJECT_FOR_VLAN(vlan_id)              \
            QCMAP_ConnectionManager::GetBackhaulObjectFromVLANid(vlan_id)

/* Get Default Backhaul WWAN Object */
#define GET_DEFAULT_BACKHAUL_WWAN_OBJECT()        \
            QCMAP_ConnectionManager::GetBackhaulWWANObject(QCMAP_Backhaul::defaultProfileHandle)


/* Get Backhaul WWAN Object for a profile */
#define GET_BACKHAUL_WWAN_OBJECT(profile)         \
            QCMAP_ConnectionManager::GetBackhaulWWANObject(profile)


/* Get default Firewall Object */
#define GET_DEFAULT_FIREWALL_OBJECT()    \
            QCMAP_ConnectionManager::GetFirewallObject(QCMAP_Backhaul::defaultProfileHandle)

/* Get Firewall Object for a profile */
#define GET_FIREWALL_OBJECT(profile)    \
            QCMAP_ConnectionManager::GetFirewallObject(profile)

/* Get default AlgNat Object */
#define GET_DEFAULT_NATALG_OBJECT()    \
            QCMAP_ConnectionManager::GetNatAlgObject(QCMAP_Backhaul::defaultProfileHandle)

/* Get AlgNat Object for a profile */
#define GET_NATALG_OBJECT(profile)    \
            QCMAP_ConnectionManager::GetNatAlgObject(profile)

/* Get LAN Object for a VLAN ID */
#define GET_LAN_OBJECT(vlan_id)    \
            QCMAP_ConnectionManager::GetLANObject(vlan_id)

#define GET_DEFAULT_BACKHAUL_CRADLE_OBJECT()        QCMAP_Backhaul_Cradle::Get_Instance(false)
#define GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT()      QCMAP_Backhaul_Ethernet::Get_Instance(false)
#define GET_DEFAULT_BACKHAUL_WLAN_OBJECT()          QCMAP_Backhaul_WLAN::Get_Instance(false)
#define GET_DEFAULT_LAN_OBJECT()                    QCMAP_ConnectionManager::GetLANObject(0)
#define GET_ASSOCIATED_BRIDGE_IFACE_NAME(vlan_id)   QCMAP_ConnectionManager::GetAssociatedBridgeIface(vlan_id)
#endif
