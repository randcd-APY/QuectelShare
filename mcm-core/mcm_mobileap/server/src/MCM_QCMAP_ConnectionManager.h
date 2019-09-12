#ifndef _MCM_QCMAP_CONNECTION_MANAGER_H_
#define _MCM_QCMAP_CONNECTION_MANAGER_H_

/*====================================================

FILE:  MCM_QCMAP_ConnectionManager.h

SERVICES:
   QCMAP Connection Manager Class

=====================================================

Copyright (c) 2013 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  03/10/14   pm         Fix for IPv6 address accumulation on rmnet interface.
  08/20/13   cp         IOE Initial Module.

===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "amssassert.h"
#include "ps_iface_defs.h"
#include "ds_Utils_DebugMsg.h"
#include "qcmap_cm_api.h"
#include "mcm_common_v01.h"

#define DNSMASQ_RESOLV_FILE                    "/etc/resolv.conf"

//===================================================================
//              Class Definitions
//===================================================================
class MCM_QCMAP_ConnectionManager
{
public:

   MCM_QCMAP_ConnectionManager(char *xml_path = NULL);
   virtual ~MCM_QCMAP_ConnectionManager(void);
   /* ---------------------------MobileAP Execution---------------------------*/
   void MCM_MOBILEAP_handle_callback_events(int handle,qcmap_cm_event_e event,void * qcmap_cm_cb_user_data);

   /* Enable MobileAP */
   boolean Enable(int *handle, void *cb_user_data, mcm_error_t_v01 *qmi_err_num);

   /* Disable MobileAP */
   boolean Disable(mcm_error_t_v01 *qmi_err_num);

  /* Disable MobileAP handle. */
  void DisableHandle(void);

  /*To check whether Nat type is under change*/
  boolean GetNatTypeChangeStatus();

   /* Enable STA mode (Concurrent AP+STA) */
   boolean EnableStaMode(qcmap_sta_connection_config *cfg, mcm_error_t_v01 *qmi_err_num);

   /* Disable STA mode */
   boolean DisableStaMode(mcm_error_t_v01 *qmi_err_num);

   /* ConnectBackHaul */
   boolean ConnectBackHaul(mcm_error_t_v01 *qmi_err_num);

  /* Enable DNS */
   boolean EnableDNS(void);

   /* DisconnectBackHual */
   boolean DisconnectBackHaul(mcm_error_t_v01 *qmi_err_num);

   /* IPv4v6 family Only */
   boolean IPv4v6Only(void);

   /*IoEMode*/
   boolean IoEMode(void);

   /* TetheringOnly */
   boolean TetheringOnly(void);

   /* Send MobileAP WWAN config to QTI */
   boolean SendWanConfig(void);

   /* -----------------------------Modem Config-------------------------------*/

   /* Add a Static Nat Entry to the configuration and update XML file. */
   boolean AddStaticNatEntry(qcmap_cm_port_fwding_entry_conf_t* nat_entry, mcm_error_t_v01 *qmi_err_num);

   /* Delete a Static Nat Entry from the configuration and update XML file. */
   boolean DeleteStaticNatEntry(qcmap_cm_port_fwding_entry_conf_t* nat_entry, mcm_error_t_v01 *qmi_err_num);

   /* Get a Static Nat Entry from the configuration. */
   int GetStaticNatEntry(qcmap_cm_port_fwding_entry_conf_t* nat_entry,
                         int max_entries, mcm_error_t_v01 *qmi_err_num);

   /* Add a DMZ IP address to the configuration and update XML file. */
   boolean AddDMZ(uint32_t dmz_ip, mcm_error_t_v01 *qmi_err_num);

   /* Get the DMZ IP address from the configuration . */
   boolean GetDMZ(uint32_t* dmz_ip, mcm_error_t_v01 *qmi_err_num);

   /* Delete a DMZ IP address from the configuration and update XML file. */
   boolean DeleteDMZ(uint32_t dmz_ip, mcm_error_t_v01 *qmi_err_num);

   /* Set NAT Timeout in the configuration and update XML file. */
   boolean SetNATEntryTimeout(uint16_t timeout, mcm_error_t_v01 *qmi_err_num);

   /* Get NAT Timeout from the configuration and update XML file. */
   boolean GetNATEntryTimeout(uint16_t *timeout, mcm_error_t_v01 *qmi_err_num);

   /* Set/Get VPN Pass Through modes in the configuration and update XML file. */
   boolean SetIPSECVpnPassThrough(boolean enable, mcm_error_t_v01 *qmi_err_num);
   boolean GetIPSECVpnPassThrough(uint8_t *vpn_value, mcm_error_t_v01 *qmi_err_num);

   boolean SetL2TPVpnPassThrough(boolean enable, mcm_error_t_v01 *qmi_err_num);
   boolean GetL2TPVpnPassThrough(uint8_t *vpn_value, mcm_error_t_v01 *qmi_err_num);

   boolean SetPPTPVpnPassThrough(boolean enable, mcm_error_t_v01 *qmi_err_num);
   boolean GetPPTPVpnPassThrough(uint8_t*vpn_value, mcm_error_t_v01 *qmi_err_num);

   /* Set firewall state in the configuration and update XML file. */
   boolean SetFirewall(boolean enable, boolean pkts_allowed, mcm_error_t_v01 *qmi_err_num);
   int SetFirewallOnA5(boolean enable, boolean pkts_allowed, mcm_error_t_v01 *qmi_err_num);
   boolean ModifyExtdFirewallEntryOnA5(qcmap_cm_extd_firewall_entry_conf_t
                                                           *firewall_entry,
                                       boolean enable,
                                       boolean pkts_allowed,
                                       mcm_error_t_v01 *qmi_err_num);

   boolean SetAutoconnect(boolean enable, mcm_error_t_v01 *qmi_err_num);
   boolean GetAutoconnect(mcm_error_t_v01 *qmi_err_num);
   void    SetQtiState(boolean enable);
   boolean GetQtiState(void);
   boolean SetRoaming(boolean enable, mcm_error_t_v01 *qmi_err_num);
   boolean GetRoaming(mcm_error_t_v01 *qmi_err_num);


   /* Get WWAN statistics */
   boolean GetWWANStatistics(ip_version_enum_type ip_family,
                             qcmap_cm_statistics_t *wwan_stats, mcm_error_t_v01 *qmi_err_num);

   /* Reset WWAN statistics */
   boolean ResetWWANStatistics(ip_version_enum_type ip_family, mcm_error_t_v01 *qmi_err_num);

   /*Add an Extended Firewall rule to the configuration*/
   boolean AddExtdFireWallEntry(qcmap_cm_extd_firewall_conf_t* extd_firewall_conf, mcm_error_t_v01 *qmi_err_num);

   /*Get an Extended Firewall rule from the configuration*/
   boolean GetExtdFireWallEntry(qcmap_cm_extd_firewall_conf_t* extd_firewall_conf, mcm_error_t_v01 *qmi_err_num);

   /*Delete extended firewall rule from the configuration*/
   boolean DeleteExtdFireWallEntry(qcmap_cm_extd_firewall_conf_t* extd_firewall_conf, mcm_error_t_v01 *qmi_err_num);

   /*Get Firewall rule handles from the configuration*/
   boolean GetFireWallHandleList(qcmap_cm_extd_firewall_conf_t* extd_firewall_conf, mcm_error_t_v01 *qmi_err_num);

   boolean GetIPv4WWANNetworkConfiguration(uint32_t *public_ip, uint32_t *primary_dns, uint32_t *secondary_dns, mcm_error_t_v01 *qmi_err_num);

   /*Change NAT Type based on the option given */
   boolean ChangeNatType(qcmap_cm_nat_type nat_type, mcm_error_t_v01 *qmi_err_num);
   boolean GetNatType(qcmap_cm_nat_type * cur_nat_type, mcm_error_t_v01 *qmi_err_num);
   boolean SetNatType(void);

   /* -----------------------------Linux Config-------------------------------*/

   /* Bring up Linux Tethering DNS Routing. */
   boolean EnableTethering(void);

   /* Bring up Linux LAN. */
   boolean EnableWLAN(mcm_error_t_v01 *qmi_err_num);

   /* Enable IPV6 Forwarding */
   boolean EnableIPV6Forwarding(void);

   /* Bring down Linux LAN. */
   boolean DisableWLAN(mcm_error_t_v01 *qmi_err_num);

   /* Bring up/down and configure DualAP mode. */
   boolean SetDualAPConfig(boolean enable, uint32_t a5_ip_addr, uint32_t sub_net_mask, mcm_error_t_v01 *qmi_err_num);

   /* start and stop the Linux HostAPD server. */
   boolean StopHostAPD(int intf);
   boolean StartHostAPD(int intf);

   /* Configure, start and stop the Linux DHCPD server. */
   boolean SetDHCPDConfig(int intf, uint32_t start, uint32_t end, char * leasetime, mcm_error_t_v01 *qmi_err_num);
   boolean StartDHCPD(void);
   boolean StopDHCPD(void);

   /* Start, stop and restart the linux MCAST routing daemon. */
   void StartMcastDaemon(void);
   void StopMcastDaemon(void);
   void ReStartMcastDaemon(void);

   /* Used in case of Concurrent STA+AP mode */
   boolean AddSNATEntryOnA5(qcmap_cm_port_fwding_entry_conf_t* nat_entry, mcm_error_t_v01 *qmi_err_num);
   boolean DeleteSNATEntryOnA5(qcmap_cm_port_fwding_entry_conf_t* nat_entry, mcm_error_t_v01 *qmi_err_num);

   boolean AddDMZOnA5(uint32_t dmz_ip, mcm_error_t_v01 *qmi_err_num);
   boolean DeleteDMZOnA5(uint32_t dmz_ip, mcm_error_t_v01 *qmi_err_num);

   boolean SetIPSECVpnPassThroughOnA5(boolean enable, mcm_error_t_v01 *qmi_err_num);
   boolean SetL2TPVpnPassThroughOnA5(boolean enable, mcm_error_t_v01 *qmi_err_num);
   boolean SetPPTPVpnPassThroughOnA5(boolean enable, mcm_error_t_v01 *qmi_err_num);

   /* Use for Signal the LAN/WAN connected */
   pthread_mutex_t             cm_mutex;
   pthread_cond_t              cm_cond;
   boolean                     lan_connecting_in_process;
   boolean                     wan_connecting_in_process;
   boolean                     enable_dns;
   /* Which mode is wifi brought up in */
   qcmap_cm_wifi_mode_type     wifi_mode;
   /* Is STA connected */
   boolean                     sta_connected;
   /* Autoconnect state prior to STA mode.*/
   boolean                     auto_connect;
   /* Index of STA iface in interfaces array in lan_config */
   int                         sta_iface_index;
   /*STA IP address*/
   char                        staIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
   /* NAT Type change in progress */
   boolean                     qcmap_nat_type_change_in_progress;
   /* eth device number for first AP iface */
   int                         ap_dev_num;
   /*To track qcmap tear down*/
   boolean                     qcmap_tear_down_in_progress;

   /* use for storing IPV6 prefix info */
   qcmap_cm_nl_prefix_info_t  ipv6_prefix_info;

   /* Get IPV6 prefix information from wwan */
   boolean GetIPV6PrefixInfo(void );

   /* remove Ipv6 prefix info */
   void RemoveIPV6Address(void);

private:

   /* Read QCMAP config from XML file */
   boolean ReadConfigFromXML(void);

   /* Write QCMAP config to XML file */
   boolean WriteConfigToXML(void);

   /*Read QCMAP extended firewall config from XML */
   boolean ReadConfigFromFirewallXML(void);

   /*Write QCMAP extended firewall config to XML */
   boolean WriteConfigToFirewallXML(void);

   /* Connect/Disconnect the Station interface to external hotspot */
   boolean ConnectSTA(qcmap_sta_connection_e conn_type,
                      qcmap_sta_static_ip_config * static_ip_config,
                      mcm_error_t_v01 *qmi_err_num);
   boolean DisconnectSTA();

   /* Find IP address assigned to the STA interface and its netmask */
   boolean GetStaIP(char *staIP, int staIPBufLen, char *netMask, int netMaskBufLen);

   /* Enable/Disable NAT on A5 (In Station mode) */
   boolean EnableNATonA5(void);
   boolean DisableNATonA5(void);

   /* XML file name */
   char                        xml_path[QCMAP_CM_MAX_FILE_LEN];

   /*Firewall XML file name*/
   char                        firewall_xml_path[QCMAP_CM_MAX_FILE_LEN];

   /* QCMAP CM Config */
   boolean                     qcmap_enable;
   qcmap_cm_conf_t             cfg;

   int                         qcmap_cm_handle;

   boolean                  mobileap_ioe_mode;
   int                         mobileap_errno;
};

#endif

