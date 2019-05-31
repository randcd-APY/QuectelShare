#ifndef _QCMAP_L2TP_H_
#define _QCMAP_L2TP_H_

/*======================================================

FILE:  QCMAP_L2TP.h

SERVICES:
   QCMAP L2TP Class

=======================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

======================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  03/15/17   jc           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "ds_util.h"
#include "ds_list.h"
#include "qcmap_cm_api.h"
#include "ds_string.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Virtual_LAN.h"

/*L2TP XML path's*/
#define TEMP_L2TP_CFG_XML           "/tmp/l2tp_cfg_temp.xml"
#define L2TP_CFG_XML                "/etc/data/l2tp_cfg.xml"

/*L2TP flags*/
#define L2TPConfig_TAG               "L2TPConfig"
#define L2TPCONFIG_ENABLE_TAG        "enable"
#define L2TPEnableMTU_TAG            "MTU_enable"
#define L2TPEnableTCPMSS_TAG         "TCP_MSS_enable"

/*L2TP xml flags*/
#define L2TPXMLCfg_TAG               "l2tpcfg"
#define L2TPTunnel_TAG               "tunnel"
#define L2TPTunnelID_TAG             "local_tunnel_id"
#define L2TPPeerTunnelID_TAG         "peer_tunnel_id"
#define L2TPTunnelIface_TAG          "local_iface"
#define L2TPTunnelIPVer_TAG          "IPVersion"
#define L2TPTunnelRemoteIP_TAG       "remote_IP"
#define L2TPTunnelEncapProto_TAG     "encap_proto"
#define L2TPTunnelUDPPort_TAG        "udp_sport"
#define L2TPTunnelPeerUDPPort_TAG    "udp_dport"

/*L2TP Session Tags*/
#define L2TPSession_Tag               "session"
#define L2TPSessionID_Tag             "session_ID"
#define L2TPSessionPeerID_Tag         "peer_session_ID"

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
  #define IPA_DEVICE_NAME            "/dev/ipa"
#endif

#define MAX_UINT32_VAL               4294967295   /*2^32 - 2*/
#define MAX_UINT16_VAL               65535        /*2^16 - 1*/
#define L2TP_MTU_SIZE                1562   /*1506 + L2TP header Size +
                                                 IPv6 header size*/
#define STD_MTU_SIZE                 1500
#define L2TP_v4_MSS_SIZE             1398   /*1460 - L2TP header Size(8 bytes)-
                                                   IPv6 header size(40 bytes) -
                                                  inner eth header (14 bytes)*/
#define L2TP_v6_MSS_SIZE             1378   /*1440 - L2TP header Size(8 bytes)-
                                                   IPv6 header size(40 bytes) -
                                                  inner eth header (14 bytes)*/
#define MAX_L2TP_COMMAND_LEN         5000
#define MAX_SESSION_INDEX            100

#define VALIDATE_PHY_TYPE(phy_type)  \
        (phy_type <= QCMAP_TETH_MIN || phy_type >= QCMAP_MAX_PHY_LAN_IFACE)

typedef struct
{
  ds_dll_el_t *SessionIDHead;
  ds_dll_el_t *SessionIDTail;
}qcmap_session_id_info_list_t;

typedef struct
{
  ds_dll_el_t *L2TP_TunnelListHead;
  ds_dll_el_t *L2TP_TunnelListTail;
}qcmap_L2TP_Tunnel_info_list_t;

typedef struct
{
  ds_dll_el_t *L2TP_SessionListHead;
  ds_dll_el_t *L2TP_SessionListTail;
}qcmap_L2TP_Session_info_list_t;


typedef struct
{
  uint32                           local_tunnel_id;
  uint32                           peer_tunnel_id;
  qcmap_msgr_ip_family_enum_v01    ip_family;
  struct in6_addr                  peer_ipv6_addr;
  uint32                           peer_ipv4_addr;
  qcmap_msgr_l2tp_encap_protocol_enum_v01 proto;
  uint16                           local_udp_port;
  uint16                           peer_udp_port;
  qcmap_L2TP_Session_info_list_t   session_list;
  int16_t                          vlan_id;
  QCMAP_Virtual_LAN_phy_iface phy_type;
  uint32                           num_conf_sessions;
  boolean                          is_installed;
} qcmap_L2TP_Tunnel_list_item_t;

typedef struct
{
  qcmap_msgr_l2tp_session_config_v01 sessionConfig;
  uint16                             SessionIndex;
} qcmap_L2TP_Session_list_item_t;

typedef struct
{
  char if_name[QCMAP_MAX_IFACE_NAME_SIZE_V01];
}phy_if_name;

class QCMAP_L2TP
{
private:

  static bool flag;
  static bool l2tpEnableFlag;
  static QCMAP_L2TP *object;
  QCMAP_L2TP();
  static qcmap_session_id_info_list_t sessionIDList;
  static qcmap_L2TP_Tunnel_info_list_t tunnel_list;
  static boolean is_iface_up[QCMAP_MAX_PHY_LAN_IFACE];
  static phy_if_name physical_iface_name[QCMAP_MAX_PHY_LAN_IFACE];
  static boolean MTU_config_enabled;
  static boolean TCP_MSS_config_enabled;
  static boolean TCP_MSS_Kernel_Configs_Enabled;
  static boolean eth_link_mtu_set_toggle;
  static int num_c_tunnel;
public:

  static uint8 sessionIndex[MAX_SESSION_INDEX];
  /* L2TP config XML file name */
  static char L2TP_xml_path[QCMAP_MSGR_MAX_L2TP_FILE_NAME_LENGTH_V01];
  ~QCMAP_L2TP();
  static QCMAP_L2TP *Get_Instance(boolean obj_create=false);

  /* ----------------------L2TP class Execution---------------------------*/

  boolean ReadInstallL2TPConfigFromXML();

  boolean ReadL2TPTunnelConf(pugi::xml_node *child ,
                             qcmap_L2TP_Tunnel_list_item_t *tunnel_conf);

  boolean AddL2TPTunnelEntryToList(
                              qcmap_L2TP_Tunnel_list_item_t tunnel_config_info,
                                qcmap_L2TP_Tunnel_list_item_t *tunnel_config);

  boolean InstallL2TP(qcmap_L2TP_Tunnel_list_item_t l2tp_config_node);

  boolean AddL2TPSessionEntryToList(
                            qcmap_L2TP_Session_list_item_t session_config_info,
                            qcmap_L2TP_Session_list_item_t *session_config,
                            qcmap_L2TP_Session_info_list_t *session_list);

  boolean RemoveL2TP(qcmap_L2TP_Tunnel_list_item_t l2tp_config_node);

  boolean FindLeastAvailSessionIndex(uint8 *sessionIndex);

  boolean ConstructTunnelNode(qcmap_msgr_l2tp_config_v01 tunnel_config,
                            qcmap_L2TP_Tunnel_list_item_t *tunnel_node);

  boolean MatchTunnelConfig(qcmap_L2TP_Tunnel_list_item_t *tunnel_node1,
                            qcmap_L2TP_Tunnel_list_item_t *tunnel_node2);

  void FreeTunnelList(qcmap_L2TP_Tunnel_info_list_t *tunnel_list);

  boolean CopyTunnelConfig(qcmap_msgr_l2tp_config_v01 *dest_l2tp,
                            qcmap_L2TP_Tunnel_list_item_t *src_l2tp);

  void DelL2TPNodeFromXML(qcmap_L2TP_Tunnel_list_item_t *tunnel_node);

  void AddL2TPNodeToXML(qcmap_L2TP_Tunnel_list_item_t *tunnel_node);

  boolean AddSessionIDEntryToList(uint32 session_id,
                                uint32 *session_id_node);

  boolean DeleteAllTunnels();

  boolean ConfigDelL2TPTCPMSS(boolean config, char *iface_name);

  boolean ConfigDelMTUSizeOnPhyVLANiface(
                                     boolean config,
                                     QCMAP_Virtual_LAN_phy_iface phy_type,
                                     int16_t                          vlan_id);

  int NumInstalledTunnelsWithPhyType(QCMAP_Virtual_LAN_phy_iface phy_type);

  int NumInstalledTunnelsWithVLANID(int16_t vlan_id);

  boolean InstallDelTunnelsOnPhyIface(char *iface_name, boolean install);

  boolean InstallDelTunnelsOnVLANIface(char *iface_name, boolean install);

  qmi_error_type_v01 GetTunnelConfigFromList(
                                qcmap_L2TP_Tunnel_info_list_t *tunnel_list,
                                qcmap_msgr_l2tp_config_v01 *l2tp_config,
                                uint32                     *num_tunnels);

  boolean CleanupL2TPTunnelConfig(
                                qcmap_L2TP_Tunnel_list_item_t *tunnel_config);

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
  boolean UpdateIPAWithL2TPIOCTL( char *iface_name,char *phy_iface,
                               qcmap_msgr_ip_family_enum_v01 ip_family,
                              uint32 session_id,boolean is_up);
#endif

  static boolean ValidateTunnelConfig(
                                 qcmap_L2TP_Tunnel_list_item_t *tunnel_conf,
                                 boolean match_session_id);

  static long int qcmap_match_tunnel_id_in_list( const void *first,
                                            const void *second );

  static long int qcmap_match_session_id_from_list( const void *first,
                                            const void *second );

  static long int qcmap_match_session_id(const void *first,
                                       const void *second);

  static boolean GetSetL2TPEnableConfigFromXML(
            qcmap_action_type action,
            qcmap_msgr_set_unmanaged_l2tp_state_config_v01 *l2tp_config_info);

  static boolean GetSetMTUEnableConfigFromXML(
            qcmap_action_type action,
            qcmap_msgr_l2tp_mtu_config_v01 *mtu_config);

  static boolean GetSetTCPMSSEnableConfigFromXML(
            qcmap_action_type action,
            qcmap_msgr_l2tp_TCP_MSS_config_v01 *mss_config);

  static boolean SetL2TPState(
                     qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_config,
                     void *softApHandle,
                     qmi_error_type_v01 *qmi_err_num );

  static boolean SetL2TPConfig( qcmap_msgr_l2tp_config_v01 l2tp_config,
                                      void *softApHandle,
                                      qmi_error_type_v01 *qmi_err_num );

  static boolean GetL2TPConfig(qcmap_msgr_l2tp_config_v01 *l2tp_config,
                            uint32                       *length,
                            qcmap_msgr_l2tp_mode_enum_v01 *mode,
                            qcmap_msgr_l2tp_mtu_config_v01 *l2tp_mtu_config,
                            qcmap_msgr_l2tp_TCP_MSS_config_v01 *l2tp_mss_config,
                            qmi_error_type_v01 *qmi_err_num );

  static boolean DeleteL2TPTunnelConfig(
                         qcmap_msgr_delete_l2tp_config_v01 l2tp_config,
                         void *softApHandle,
                         qmi_error_type_v01 *qmi_err_num );

  static boolean SetMTUConfigForL2TP(
                              qcmap_msgr_l2tp_mtu_config_v01 l2tp_mtu_config,
                              void *softApHandle,
                              qmi_error_type_v01 *qmi_err_num );

  static boolean SetTCPMSSConfigForL2TP(
                             qcmap_msgr_l2tp_TCP_MSS_config_v01 l2tp_mss_config,
                             void *softApHandle,
                             qmi_error_type_v01 *qmi_err_num );

};
#endif
