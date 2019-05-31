/*====================================================

FILE:  QCMAP_Backhaul_WWAN.cpp

SERVICES:
   QCMAP Connection Manager Backhaul (WWAN) Specific Implementation

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   ka         Created
  03/28/17   spr        Added support for Multi-PDN.
===========================================================================*/
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "ds_qmi_qcmap_msgr.h"
#include "wireless_data_service_v01.h"

#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_Virtual_LAN.h"

/*=============================================================*/
/* Extern definitions                                          */
/*=============================================================*/
extern unsigned int dsi_qcmap_sockfd;
extern unsigned int cmdq_qcmap_sockfd;
extern unsigned int modem_loopback_mode;
extern int get_nas_config
(
  QCMAP_Backhaul_WWAN *pBackhaulWWAN,
  int                  qcmap_cm_handle,   /* Handle for MobileAP CM */
  int                 *qcmap_cm_errno,    /* Error condition value  */
  qmi_error_type_v01  *qmi_err_num
);
extern struct sigevent sev;


/*=============================================================*/
/* Local definitions                                          */
/*=============================================================*/
qcmap_dynamic_dns_t           QCMAP_Backhaul_WWAN::ddns_conf = {0};
qcmap_ddns_server_info_t      QCMAP_Backhaul_WWAN::ddns_server_info = {0};
uint16                        QCMAP_Backhaul_WWAN::eri_roam_data_len = 0;
uint8                         QCMAP_Backhaul_WWAN::eri_roam_data[QCMAP_WAN_MAX_ERI_DATA_SIZE] = {0};

#define LOAD_XML_FILE(xml_doc, qcMapMgr) \
    if (qcMapMgr && !xml_doc.load_file(qcMapMgr->xml_path)) \
    {  \
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);  \
      return false;  \
    } \


/* Check if Profile is valid, return failure if not valid */
#define IS_PROFILE_VALID(profile_index, ret_val, qmi_err_num) \
              if (profile_index == 0) \
              {  \
                LOG_MSG_ERROR("Profile (%p), policy=%d is invalid", \
                              QcMapBackhaul->profileHandle, profile_index, 0); \
                ret_val = false; \
                qmi_err_num = QMI_ERR_INVALID_ARG_V01; \
              }  \

/* Copy wwan config parameters, profileHandle, auto-connect, roaming */
#define COPY_WWAN_CONFIG_PARAM(WWAN_OBJ, config_params) \
            config_params.profileHandle = WWAN_OBJ->QcMapBackhaul->profileHandle; \
            config_params.wwan_policy.tech_pref = WWAN_OBJ->wwan_cfg.tech; \
            config_params.wwan_policy.v4_profile_id_3gpp =  \
                        WWAN_OBJ->wwan_cfg.profile_id.v4.umts_profile_index; \
            config_params.wwan_policy.v4_profile_id_3gpp2 = \
                        WWAN_OBJ->wwan_cfg.profile_id.v4.cdma_profile_index; \
            config_params.wwan_policy.v6_profile_id_3gpp =  \
                        WWAN_OBJ->wwan_cfg.profile_id.v6.umts_profile_index; \
            config_params.wwan_policy.v6_profile_id_3gpp2 = \
                        WWAN_OBJ->wwan_cfg.profile_id.v6.cdma_profile_index; \
            config_params.auto_connect =  WWAN_OBJ->auto_connect; \
            config_params.roaming = WWAN_OBJ->wwan_cfg.roaming \


/*===================================================================
  Class Definitions
  ===================================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes Backhaul WWAN variables.

@parameters
none

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
QCMAP_Backhaul_WWAN::QCMAP_Backhaul_WWAN
(
  QCMAP_Backhaul* QcMapBackhaul
)
{
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  memset(&wwan_cfg, 0, sizeof(wwan_cfg));
  memset(&this->dhcpv6_dns_conf, 0, sizeof(this->dhcpv6_dns_conf));
  memset(pri_dns_ipv4_addr, 0, INET_ADDRSTRLEN);
  memset(sec_dns_ipv4_addr, 0, INET_ADDRSTRLEN);
  memset(pri_dns_ipv6_addr, 0, INET6_ADDRSTRLEN);
  memset(sec_dns_ipv6_addr, 0, INET6_ADDRSTRLEN);
  memset(ipv6_default_table_rule_suffix, 0, MAX_DEFAULT_TABLE_LEN);
  memset(ipv6_default_oif_rule_suffix, 0, MAX_DEFAULT_TABLE_LEN);
  memset(dsi_device_name, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2);
  memset(ipv6_dsi_device_name, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2);
  memset(&default_sip_server_info, 0, sizeof(default_sip_server_info));
  memset(&sip_server_info_list, 0, sizeof(sip_server_info_list));
  sip_server_list_count = 0;

  this->QcMapBackhaul = QcMapBackhaul;

  ipv6_prefix_based_rules_added_for_guest_ap_1 = false;
  ipv6_prefix_based_rules_added_for_guest_ap_2 = false;
  ipv6_prefix_based_rules_added_for_guest_ap_3 = false;

  prefix_delegation_activated = false;

  wwan_cfg.tech = QCMAP_WAN_TECH_ANY;
  wwan_cfg.profile_id.v4.umts_profile_index = wwan_cfg.profile_id.v6.umts_profile_index = 0;
  wwan_cfg.profile_id.v4.cdma_profile_index = wwan_cfg.profile_id.v6.cdma_profile_index = 0;

  default_sip_server_info.sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;

  strlcpy( default_sip_server_info.sip_serv_info.sip_fqdn,
           "abcd.com", ( QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01 ));
  strlcpy( QCMAP_Backhaul_WWAN::ddns_server_info.server_url,
           QCMAP_DYNDNS_SERVER_URL, QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
  strlcpy( QCMAP_Backhaul_WWAN::ddns_server_info.protocol,
           QCMAP_DYNDNS_PROTOCOL, QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

  qmi_client_ref = new std::map<uint32, qmi_client_ref_type*>;

  /* Create timer for IPv4 */
  pTimerData_v4 = (qcmap_timer_data_t *)malloc(sizeof(qcmap_timer_data_t));
  ds_assert(pTimerData_v4 != NULL); //Assert if malloc fails.
  pTimerData_v4->timer_id = AUTO_CONNECT_V4;
  pTimerData_v4->pUserData = (void *) this;
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_ptr = (void *)pTimerData_v4;
  if(timer_create(CLOCKID, &sev, &(this->timerid_v4))==-1)
    LOG_MSG_ERROR("timer_create failed for v4",0,0,0);

  auto_timer_value_v4 = AUTO_CONNECT_TIMER;
  auto_connect_timer_running_v4 = false;

  /* Create timer for IPv6 */
  pTimerData_v6 = (qcmap_timer_data_t *)malloc(sizeof(qcmap_timer_data_t));
  ds_assert(pTimerData_v6 != NULL); //Assert if malloc fails.
  pTimerData_v6->timer_id = AUTO_CONNECT_V6;
  pTimerData_v6->pUserData = (void *) this;
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_ptr = (void *)pTimerData_v6;
  if(timer_create(CLOCKID, &sev, &(this->timerid_v6))==-1)
    LOG_MSG_ERROR("timer_create failed for v6",0,0,0);

  auto_timer_value_v6 = AUTO_CONNECT_TIMER;
  auto_connect_timer_running_v6 = false;

  memset(&(dsi_net_hndl), 0x0, sizeof(dsi_net_hndl));
  memset(&(ipv6_dsi_net_hndl), 0x0, sizeof(ipv6_dsi_net_hndl));

  if (QCMAP_ConnectionManager::is_cm_enabled == true)
  {
    state = QCMAP_CM_ENABLE;
    ipv6_state = QCMAP_CM_V6_ENABLE;
  }
  else
  {
    state = QCMAP_CM_DISABLE;
    ipv6_state = QCMAP_CM_V6_DISABLE;
  }
  backhaul_service = FALSE;
  eri_roam_data_len = 0;

  v4_vlan_pdn_enabled = 0;
  v6_vlan_pdn_enabled = 0;

  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroyes the Backhaul_WWAN Object.

@parameters
none

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
QCMAP_Backhaul_WWAN::~QCMAP_Backhaul_WWAN()
{
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                                     GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                   GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=
                                     GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  std::map<uint32, qmi_client_ref_type*>::iterator clnt_it;

  LOG_MSG_INFO2("Destroying Object: WWAN",0,0,0);

  if (pTimerData_v4)
    free(pTimerData_v4);
  pTimerData_v4 = NULL;

  if (pTimerData_v6)
    free(pTimerData_v6);
  pTimerData_v6 = NULL;

  /* Delete all qmi client reference in this WWAN Object */
  if (qmi_client_ref)
  {
    clnt_it = qmi_client_ref->begin();
    while(clnt_it != qmi_client_ref->end())
    {
      if (clnt_it->second)
        free(clnt_it->second);
      clnt_it++;
    }
    delete qmi_client_ref;
    //qmi_client_ref = NULL;
  }

  //if(!QcMapBackhaulWLANMgr && !QcMapBackhaulCradleMgr && !QcMapBackhaulEthMgr)
  //  delete QcMapBackhaulMgr;
}


/*==========================================================
  FUNCTION Init
==========================================================*/
/*!
@brief
  Initialize Params for WWAN Object

@parameters
  None

@return
  profile_handle

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::Init(void *pdnCacheNode)
{
  char command[MAX_COMMAND_STR_LEN];

  ReadWWANConfigFromCache(pdnCacheNode);

  /* Remove any dangling QCMAP entries from rt_tables */
  if (QCMAP_ConnectionManager::BackhaulObjectCountFromMap() == 0)
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "sed '/pdn_table/d' %s", QCMAP_ROUTE_TABLE_PATH);
    qcmap_edit_file(command, strlen(command), QCMAP_ROUTE_TABLE_PATH, strlen(QCMAP_ROUTE_TABLE_PATH));
  }

  return;
}

/*===========================================================================
  FUNCTION StopDDNS
==========================================================================*/
/*!
@brief
  Stop dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::StopDDNS()
{
  char command[MAX_COMMAND_STR_LEN];
  FILE *fp=NULL;
  int ddns_pid=0;

  LOG_MSG_INFO1("Stop DDNS",0,0,0);
  fp = fopen(DDCLIENT_PID_FILE, "r");

  if ( fp == NULL )
  {
    LOG_MSG_ERROR("Error opening ddclient pid file: %d.\n", errno, 0, 0);
    return false;
  }
  else if (fscanf(fp, "%d", &ddns_pid) != 1)
  {
    LOG_MSG_ERROR("Error reading ddclient file: %d.\n", errno, 0, 0);
    ddns_pid = 0;
    fclose(fp);
    return false;
  }
  else
  {
    LOG_MSG_INFO1("ddclient PID = %d",ddns_pid,0,0);
    snprintf(command,
           MAX_COMMAND_STR_LEN,
           "kill -9 %d",ddns_pid);
    ds_system_call(command, strlen(command));
    fclose(fp);
  }
  return true ;
}

/*===========================================================================
FUNCTION : ReadWWANConfigFromCache
==========================================================================*/
/*!
@brief
  Reads Config from cached xml

@parameters
  -pdnNode

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::ReadWWANConfigFromCache(void *pdnCacheNode)
{
  struct in_addr addr;
  uint32_t debug_string=0;
  char str[INET6_ADDRSTRLEN];
  pugi::xml_node *pdnNode;

  pdnNode = (pugi::xml_node *)pdnCacheNode;
  if (pdnNode == NULL)
  {
    LOG_MSG_ERROR("Couldn't find PDN node for profile_index=%d", QcMapBackhaul->profileHandle, 0, 0);

    auto_connect = 0;
    wwan_cfg.roaming = 0;
    wwan_cfg.tech = QCMAP_WAN_TECH_ANY;
    wwan_cfg.profile_id.v4.umts_profile_index = wwan_cfg.profile_id.v6.umts_profile_index = 0;
    wwan_cfg.profile_id.v4.cdma_profile_index = wwan_cfg.profile_id.v6.cdma_profile_index = 0;
    return false;
  }
  else
  {
    auto_connect = wwan_cfg.autoConnect = atoi(pdnNode->child(AutoConnect_TAG).child_value());
    wwan_cfg.roaming = atoi(pdnNode->child(Roaming_TAG).child_value());
    wwan_cfg.profile_id.v4.umts_profile_index = atoi(pdnNode->child(V4_UMTS_PROFILE_INDEX_TAG).child_value());
    wwan_cfg.profile_id.v4.cdma_profile_index = atoi(pdnNode->child(V4_CDMA_PROFILE_INDEX_TAG).child_value());
    wwan_cfg.profile_id.v6.umts_profile_index = atoi(pdnNode->child(V6_UMTS_PROFILE_INDEX_TAG).child_value());
    wwan_cfg.profile_id.v6.cdma_profile_index = atoi(pdnNode->child(V6_CDMA_PROFILE_INDEX_TAG).child_value());

    if (0 == strcasecmp(pdnNode->child(TECH_TAG).child_value(), TECH_ANY_TAG))
    {
      wwan_cfg.tech = QCMAP_WAN_TECH_ANY;
    }
    if (0 == strcasecmp(pdnNode->child(TECH_TAG).child_value(), TECH_3GPP_TAG))
    {
      wwan_cfg.tech = QCMAP_WAN_TECH_3GPP;
    }
    if (0 == strcasecmp(pdnNode->child(TECH_TAG).child_value(), TECH_3GPP2_TAG))
    {
      wwan_cfg.tech = QCMAP_WAN_TECH_3GPP2;
    }

    wwan_cfg.prefix_delegation = atoi(pdnNode->child(PrefixDelegation_TAG).child_value());

    /* set vlan */
    if (pdnNode->parent().child(VLAN_ID_TAG) != NULL)
      QcMapBackhaul->vlan_id = atoi(pdnNode->parent().child(VLAN_ID_TAG).child_value());
    else
      QcMapBackhaul->vlan_id = QCMAP_MSGR_VLAN_ID_NONE_V01;
    /* Future expansion can be done here to map PDN to IP or other param instead of VLAN */
  }

  /* Read SIP Params */
  if (strncasecmp(IP_TAG, pdnNode->child(DefaultSIPServerConfigType_TAG).child_value(), strlen(IP_TAG)) == 0)
  {
    default_sip_server_info.sip_serv_type = QCMAP_CM_SIP_SERVER_ADDR;
  }
  else if (strncasecmp(FQDN_TAG, pdnNode->child(DefaultSIPServerConfig_TAG).child_value(), strlen(FQDN_TAG)) == 0)
  {
    default_sip_server_info.sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;
  }

  if (default_sip_server_info.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
  {
    if (inet_aton(pdnNode->child(DefaultSIPServerConfig_TAG).child_value(), &addr))
      default_sip_server_info.sip_serv_info.sip_serv_ip = addr.s_addr;

    debug_string = htonl(default_sip_server_info.sip_serv_info.sip_serv_ip);
    readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
    LOG_MSG_INFO1("\nSIP Server IP:'%s'",str,0,0);

  }
  else if (default_sip_server_info.sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
  {
    strlcpy(default_sip_server_info.sip_serv_info.sip_fqdn,
        pdnNode->child(DefaultSIPServerConfig_TAG).child_value(),
        sizeof(default_sip_server_info.sip_serv_info.sip_fqdn));
    LOG_MSG_INFO1("SIP FQDN: %s",default_sip_server_info.sip_serv_info.sip_fqdn,0,0)
  }

  LOG_MSG_INFO1("WWAN Tech: %d SIP Server type: %d " , this->wwan_cfg.tech,
      default_sip_server_info.sip_serv_type, 0);

  LOG_MSG_INFO1("Sip server count: %d ", sip_server_list_count, 0, 0);

  return TRUE;
}

/*===========================================================================
FUNCTION : ReadDDNSAndDHCPFromXML
==========================================================================*/
/*!
@brief
  Reads SIP, DDNS and DHCP config from xml

@parameters
  -None

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::ReadStaticParamsFromXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  LOAD_XML_FILE(xml_file, QcMapMgr);


  /* Read DDNS Params */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);
  QCMAP_Backhaul_WWAN::ddns_conf.enable = atoi(root.child(DDNS_Enable_TAG).child_value());
  LOG_MSG_INFO1("DDNS Config: %d", QCMAP_Backhaul_WWAN::ddns_conf.enable, 0, 0);

  strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.server_info.server_url,
      root.child(DDNS_Server_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

  strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.server_info.protocol,
      root.child(DDNS_Protocol_TAG).child_value(), QCMAP_DDNS_PROTOCOL_LEN);

  strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.login,
      root.child(DDNS_Login_TAG).child_value(), QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);

  strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.password,
      root.child(DDNS_Password_TAG).child_value(), QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);

  strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.hostname,
      root.child(DDNS_Hostname_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

  QCMAP_Backhaul_WWAN::ddns_conf.timeout = atoi(root.child(DDNS_Timeout_TAG).child_value());

  LOG_MSG_INFO1("Server: %s Protocol: %s Login: %s",
                  QCMAP_Backhaul_WWAN::ddns_conf.server_info.server_url,
                  QCMAP_Backhaul_WWAN::ddns_conf.server_info.protocol,
                  QCMAP_Backhaul_WWAN::ddns_conf.login);

  LOG_MSG_INFO1("Password: %s Hostname: %s Timeout: %d",
                  QCMAP_Backhaul_WWAN::ddns_conf.password,
                  QCMAP_Backhaul_WWAN::ddns_conf.hostname,
                  QCMAP_Backhaul_WWAN::ddns_conf.timeout);

  /* Read DHCP Params */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DHCPV6CFG_TAG);
  this->dhcpv6_dns_conf.dhcpv6_xml_state = (qcmap_msgr_dhcpv6_mode_enum_v01)
                                       atoi(root.child(DHCPV6_Enable_TAG).child_value());
  LOG_MSG_INFO1("Dhcpv6 xml state: %d", this->dhcpv6_dns_conf.dhcpv6_xml_state, 0, 0);

  return true;
}

/*===========================================================================
FUNCTION : GetSetWWANConfigFromXML
==========================================================================*/
/*!
@brief
  Gets/Sets WWAN specific config

@parameters
  -qcmap_wwan_config_enum conf
  -qcmap_action_type action
  -char *data
  -int data_len

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetSetWWANConfigFromXML
(
 profile_handle_type_v01  profile_handle,
 qcmap_wwan_config_enum   conf,
 qcmap_action_type        action,
 char                     *data,
 int                      data_len
 )
{
  pugi::xml_document       xml_file;
  pugi::xml_node           root;
  const char              *tag_ptr = NULL;
  pugi::xml_node           searchNode;
  const char              *profileHandle_tag = ProfileHandle_TAG;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  LOAD_XML_FILE(xml_file, QcMapMgr);

  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find root XML corresponding to PDN",0,0,0);
    return false;
  }
  searchNode = root.child(MobileAPWanCfg_TAG);

  switch(conf)
  {
    case CONFIG_AUTOCONNECT:
      tag_ptr = AutoConnect_TAG;
      break;

    case CONFIG_ROAMING:
      tag_ptr = Roaming_TAG;
      break;

    case CONFIG_TECH:
      tag_ptr = TECH_TAG;
      break;

    case CONFIG_V4_UMTS_PROFILE_INDEX:
      tag_ptr = V4_UMTS_PROFILE_INDEX_TAG;
      break;

    case CONFIG_V4_CDMA_PROFILE_INDEX:
      tag_ptr = V4_CDMA_PROFILE_INDEX_TAG;
      break;

    case CONFIG_V6_UMTS_PROFILE_INDEX:
      tag_ptr = V6_UMTS_PROFILE_INDEX_TAG;
      break;

    case CONFIG_V6_CDMA_PROFILE_INDEX:
      tag_ptr = V6_CDMA_PROFILE_INDEX_TAG;
      break;

    case CONFIG_PREFIX_DELEGATION:
      tag_ptr = PrefixDelegation_TAG;
      break;

    default:
      LOG_MSG_ERROR("Invalid TAG.", 0, 0, 0);
  }


  if (action == SET_VALUE)
  {
    searchNode.child(tag_ptr).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
    strlcpy(data, searchNode.child(tag_ptr).child_value(), data_len);

  LOG_MSG_INFO1("Config: %d Action: %d data: %d " , conf, action, atoi(data));

  return true;
}

/*===========================================================================
FUNCTION : GetSetDefaultSIPServerConfigFromXML
==========================================================================*/
/*!
@brief
 Gets/Sets Default SIP Server specific config

@parameters
- qcmap_action_type action,
  qcmap_cm_sip_server_info_t *default_sip_server_info

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetSetDefaultSIPServerConfigFromXML
(
 qcmap_action_type action,
 qcmap_cm_sip_server_info_t *default_sip_server_info
 )
{
  /*
     qcmap_cm_sip_server_info_t default_sip_server_info;
     boolean  auto_connect;
     */
  pugi::xml_document xml_file;
  pugi::xml_node root,child;
  in_addr addr;
  char str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  LOAD_XML_FILE(xml_file, QcMapMgr);
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);

  switch (action)
  {
    case GET_VALUE:
      if ( strncasecmp(IP_TAG, root.child(DefaultSIPServerConfigType_TAG).child_value(),
            strlen(IP_TAG)) == 0 )
      {
        default_sip_server_info->sip_serv_type = QCMAP_CM_SIP_SERVER_ADDR;
      }
      else if ( strncasecmp(FQDN_TAG, root.child(DefaultSIPServerConfigType_TAG).child_value(),
            strlen(FQDN_TAG)) == 0 )
      {
        default_sip_server_info->sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;
      }

      if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      {
        if (inet_aton(root.child(DefaultSIPServerConfig_TAG).child_value(), &addr))
          default_sip_server_info->sip_serv_info.sip_serv_ip = addr.s_addr;
      }
      else if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
      {
        strlcpy(default_sip_server_info->sip_serv_info.sip_fqdn,
            root.child(DefaultSIPServerConfig_TAG).child_value(),
            sizeof(default_sip_server_info->sip_serv_info.sip_fqdn));
      }
      break;

    case SET_VALUE:
      if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      {
        root.child(DefaultSIPServerConfigType_TAG).text() = IP_TAG;
        addr.s_addr = default_sip_server_info->sip_serv_info.sip_serv_ip;
        root.child(DefaultSIPServerConfig_TAG).text() = inet_ntoa(addr);
        readable_addr( AF_INET,
            (const uint32_t *)&default_sip_server_info->sip_serv_info.sip_serv_ip,
            (char *)&str );
        LOG_MSG_INFO1("\nSIP Server IP:'%s'",str,0,0);
      }
      else if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
      {
        root.child(DefaultSIPServerConfigType_TAG).text() = FQDN_TAG;
        root.child(DefaultSIPServerConfig_TAG).text() =
          default_sip_server_info->sip_serv_info.sip_fqdn;

        LOG_MSG_INFO1("SIP fqdn: '%s' " ,default_sip_server_info->sip_serv_info.sip_fqdn,
            0,0);
      }
      QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
      break;
  }

  LOG_MSG_INFO1("SIP Server type: %d Action: %d    " ,
      default_sip_server_info->sip_serv_type,action ,0);
  return true;
}


/*===========================================================================
FUNCTION : GetSetSIPServerConfigFromXML
==========================================================================*/
/*!
@brief
 Gets/Sets SIP Server specific config

@parameters
- qcmap_action_type action,
  qcmap_cm_sip_server_info_t *default_sip_server_info

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetSetSIPServerConfigFromXML
(
 qcmap_action_type action,
 qcmap_cm_sip_server_info_t *default_sip_server_info
 )
{
  /*
     qcmap_cm_sip_server_info_t default_sip_server_info;
     boolean  auto_connect;
     */
  pugi::xml_document xml_file;
  pugi::xml_node root,child;
  pugi::xml_node searchNode;
  in_addr addr;
  char str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  LOAD_XML_FILE(xml_file, QcMapMgr);
  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find root XML corresponding to PDN",0,0,0);
    return false;
  }
  searchNode = root.child(MobileAPWanCfg_TAG);

  switch (action)
  {
    case GET_VALUE:
      if ( strncasecmp(IP_TAG, searchNode.child(DefaultSIPServerConfigType_TAG).child_value(),
            strlen(IP_TAG)) == 0 )
      {
        default_sip_server_info->sip_serv_type = QCMAP_CM_SIP_SERVER_ADDR;
      }
      else if ( strncasecmp(FQDN_TAG, searchNode.child(DefaultSIPServerConfigType_TAG).child_value(),
            strlen(FQDN_TAG)) == 0 )
      {
        default_sip_server_info->sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;
      }

      if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      {
        if (inet_aton(searchNode.child(DefaultSIPServerConfig_TAG).child_value(), &addr))
          default_sip_server_info->sip_serv_info.sip_serv_ip = addr.s_addr;
      }
      else if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
      {
        strlcpy(default_sip_server_info->sip_serv_info.sip_fqdn,
            searchNode.child(DefaultSIPServerConfig_TAG).child_value(),
            sizeof(default_sip_server_info->sip_serv_info.sip_fqdn));
      }
      break;

    case SET_VALUE:
      if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      {
        searchNode.child(DefaultSIPServerConfigType_TAG).text() = IP_TAG;
        addr.s_addr = default_sip_server_info->sip_serv_info.sip_serv_ip;
        searchNode.child(DefaultSIPServerConfig_TAG).text() = inet_ntoa(addr);
        readable_addr( AF_INET,
            (const uint32_t *)&default_sip_server_info->sip_serv_info.sip_serv_ip,
            (char *)&str );
        LOG_MSG_INFO1("\nSIP Server IP:'%s'",str,0,0);
      }
      else if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
      {
        searchNode.child(DefaultSIPServerConfigType_TAG).text() = FQDN_TAG;
        searchNode.child(DefaultSIPServerConfig_TAG).text() = default_sip_server_info->sip_serv_info.sip_fqdn;

        LOG_MSG_INFO1("SIP fqdn: '%s' " ,default_sip_server_info->sip_serv_info.sip_fqdn, 0,0);
      }
      QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
      break;
  }

  LOG_MSG_INFO1("SIP Server type: %d Action: %d" , default_sip_server_info->sip_serv_type,action ,0);
  return true;
}

/*===========================================================================
 FUNCTION GetProfileHandle()
===========================================================================*/
/*
 DESCRIPTION
   Get's profile handle for this backhaul WWAN object

 DEPENDENCIES
   None.

 RETURN VALUE
   profileHandle

 SIDE EFFECTS
*/
/*=========================================================================*/
profile_handle_type_v01 QCMAP_Backhaul_WWAN::GetProfileHandle()
{
  return ((QcMapBackhaul != NULL) ? QcMapBackhaul->profileHandle : 0);
}

/*===========================================================================
  FUNCTION SetProfileHandle()
===========================================================================*/
/*
  DESCRIPTION
    Set's profile handle for this backhaul WWAN object

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
*/
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::SetProfileHandle
(
  profile_handle_type_v01 profile_handle
 )
{
  QcMapBackhaul->profileHandle = profile_handle;
}

/*===========================================================================
  FUNCTION IsAutoConnectOnForAnyPDN()
===========================================================================*/
/*
  DESCRIPTION
    Check's if auto_connect is on for any profiles(PDN's).

  DEPENDENCIES
    None.

  RETURN VALUE
    boolean

  SIDE EFFECTS
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::IsAutoConnectOnForAnyPDN()
{
  boolean ret_val = FALSE;

  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    if (pBackhaulWWAN && pBackhaulWWAN->auto_connect == TRUE)
    {
      ret_val = TRUE;
      break;
    }
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION GetState()
===========================================================================*/
/*
  DESCRIPTION
    Get's ipv4_state for this backhaul WWAN Object.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
*/
/*=========================================================================*/
qcmap_cm_state_e QCMAP_Backhaul_WWAN::GetState()
{
  return state;
}

/*===========================================================================
  FUNCTION SetState()
===========================================================================*/
/*
  DESCRIPTION
    Set's ipv4_state for this backhaul WWAN Object.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
*/
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::SetState(qcmap_cm_state_e state)
{
  this->state = state;
}

/*===========================================================================
  FUNCTION GetIPv6State()
===========================================================================*/
/*
  DESCRIPTION
    Get's IPv6_state for this backhual WWAN Object.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
*/
/*=========================================================================*/
qcmap_cm_v6_state_e QCMAP_Backhaul_WWAN::GetIPv6State()
{
  return ipv6_state;
}

/*===========================================================================
  FUNCTION SetIPv6State()
===========================================================================*/
/*
  DESCRIPTION
    Set's ipv6_state for this backhual WWAN object.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
*/
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::SetIPv6State(qcmap_cm_v6_state_e state)
{
  this->ipv6_state = state;
}

/*===========================================================================

FUNCTION EnableIPv6()

DESCRIPTION

  It will enable IPv6.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_cm_errno Values
  ----------------
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::EnableIPv6 /* static function */
(
 int                 qcmap_cm_handle,        /* Handle for MobileAP CM    */
 qmi_error_type_v01 *qmi_err_num             /* Error condition value     */
 )
{
  int ret = QCMAP_CM_SUCCESS;
  int qcmap_cm_errno;
  ds_assert(qmi_err_num != NULL);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  LOG_MSG_INFO1("EnableIPv6(): enter", 0, 0, 0);

  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    if ( pBackhaulWWAN->auto_connect &&
        pBackhaulWWAN->ipv6_state == QCMAP_CM_V6_ENABLE)
    {
      pBackhaulWWAN->ConnectBackHaul(qcmap_cm_get_handle(),
          QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01, &qcmap_cm_errno, qmi_err_num);
    }
  }

  return QCMAP_CM_SUCCESS;

}

/*===========================================================================

FUNCTION DisableIPv6()

DESCRIPTION

  It will disable IPv6.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_cm_errno Values
  ----------------
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::DisableIPv6 /* static function */
(
 int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
 int     *qcmap_cm_errno,                    /* Error condition value     */
 qmi_error_type_v01 *qmi_err_num
 )
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  int ret = QCMAP_CM_SUCCESS;

  ds_assert(qcmap_cm_errno != NULL);

  LOG_MSG_INFO1("DisableIPv6(): enter", 0, 0, 0);

  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    ret = pBackhaulWWAN->DisconnectBackHaul(qcmap_cm_get_handle(),
        QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01,
        qcmap_cm_errno, qmi_err_num);
  }
  return ret;
}

/*===========================================================================

FUNCTION EnableIPv4()

DESCRIPTION

  It will enable IPv4 backhaul functionlity.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_cm_errno Values
  ----------------
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

SIDE EFFECTS
===========================================================================*/
int QCMAP_Backhaul_WWAN::EnableIPv4 /* static function */
(
 int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
 qmi_error_type_v01 *qmi_err_num             /* Error condition value     */
 )
{
  int ret = QCMAP_CM_SUCCESS;
  int qcmap_cm_errno;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  ds_assert(qmi_err_num != NULL);

  LOG_MSG_INFO1("EnableIPv4(): enter", 0, 0, 0);

  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    if ( pBackhaulWWAN->auto_connect &&
        pBackhaulWWAN->state == QCMAP_CM_ENABLE)
    {
      pBackhaulWWAN->ConnectBackHaul(qcmap_cm_get_handle(),
          QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01, &qcmap_cm_errno, qmi_err_num);
    }
  }
  return QCMAP_CM_SUCCESS;

}

/*===========================================================================

FUNCTION DisableIPv4()

DESCRIPTION

  It will disable IPv4 backhaul functionality.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_cm_errno Values
  ----------------
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::DisableIPv4 /* static function */
(
 int      qcmap_cm_handle,                   /* Handle for MobileAP CM    */
 int     *qcmap_cm_errno,                    /* Error condition value     */
 qmi_error_type_v01 *qmi_err_num
 )
{
  int ret = QCMAP_CM_SUCCESS;

  ds_assert(qcmap_cm_errno != NULL);

  LOG_MSG_INFO1("DisableIPv4(): enter", 0, 0, 0);

  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    ret = pBackhaulWWAN->DisconnectBackHaul(qcmap_cm_get_handle(),
        QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
        qcmap_cm_errno, qmi_err_num);
  }
  return ret;
}

/*===========================================================================
  FUNCTION ConnectBackHaul
==========================================================================*/
/*!
@brief
  Tries to connect to backhaul. As a part of connect to backhaul checks for the
  network service and nas configuration.
  If the network is in roaming mode,  connects to backhaul based on the roaming flag configuration.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num
  qmi_error_type_v01            *qmi_err_num
@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::ConnectBackHaul
(
 qcmap_msgr_wwan_call_type_v01 call_type,
 int *err_num,
 qmi_error_type_v01 *qmi_err_num
 )
{
  int qcmap_cm_errno;
  int ret_val;
  int rc = 0;
  struct timespec ts;
  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 pri_dns_addr = 0;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  LOG_MSG_INFO1("In ConnectBackHaul %x Call Type:%d\n",QcMapMgr->qcmap_cm_handle, call_type, 0);

  ret_val = ConnectBackHaul(QcMapMgr->qcmap_cm_handle, call_type, &qcmap_cm_errno,
                            qmi_err_num);

  if (ret_val == QCMAP_CM_SUCCESS)
  {
    *err_num = qcmap_cm_errno;
    return true;
  }

  if (ret_val == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK)
  {
    LOG_MSG_INFO1("QCMAP WAN Connecting Inprogress \n",0,0,0);
    return true;
  }
  else
  {
    LOG_MSG_INFO1( "QCMAP WAN Connecting Fail, ret_val %d qcmap_cm_errno %d\n",
                  ret_val, qcmap_cm_errno,0 );
    return false;
  }
}

/*===========================================================================
  FUNCTION DisconnectBackHaul
==========================================================================*/
/*!
@brief
  Brings down the backhaul service.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num
  qmi_error_type_v01            *qmi_err_num

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DisconnectBackHaul
(
 qcmap_msgr_wwan_call_type_v01 call_type,
 int *err_num,
 qmi_error_type_v01 *qmi_err_num
 )
{
  int qcmap_cm_errno;
  int ret_val;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (QcMapMgr == NULL || QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr=%p, LAN Object=%p", QcMapMgr, QCMAPLANMgr, 0);
    return false;
  }

  ret_val = DisconnectBackHaul(QcMapMgr->qcmap_cm_handle, call_type, &qcmap_cm_errno, qmi_err_num);

  if (ret_val == QCMAP_CM_SUCCESS)
  {
    QCMAPLANMgr->SetDNSEnableFlag(true);
    *err_num = qcmap_cm_errno;
    return true;
  }
  if (ret_val == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK)
  {
    /* Backhaul disconnecting in progress */
    QCMAPLANMgr->SetDNSEnableFlag(true);
    return true;
  }
  else
  {
    return false;
  }
}

/*===========================================================================
  FUNCTION SetAutoconnect
==========================================================================*/
/*!
@brief
  This function will enable the autoconnect flag.
  If the mobileap and auto connect is enabled, connect to backhaul is triggered.
  In case connect to backhaul fails, then a autoconnect timer is triggered. After the expiry of
  autoconnect timer a new request to connect to backhaul is initiated.

@parameters
  boolean enable

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetAutoconnect
(
 boolean enable,
 qmi_error_type_v01 *qmi_err_num
 )
{
  int qcmap_cm_errno;
  int ret_val;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  char autoConnect[MAX_STRING_LENGTH] = {0};

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (state == QCMAP_CM_DISABLE && ipv6_state == QCMAP_CM_V6_DISABLE)
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR( "QCMAP CM disable state", 0, 0, 0);
    return false;
  }

  auto_connect = enable;

  if (auto_connect == TRUE)
  {
    /* Bring-up V4 and V6 data-call */
    ConnectV4AndV6();
  }
  else
  {
    /* Stop any running auto-connect timers. */
    if( auto_connect_timer_running_v4)
    {
      StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V4_V01);
    }
    if (auto_connect_timer_running_v6)
    {
      StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V6_V01);
    }
  }

  snprintf(autoConnect, sizeof(autoConnect), "%d", enable);
  GetSetWWANConfigFromXML(QcMapBackhaul->profileHandle, CONFIG_AUTOCONNECT, SET_VALUE, autoConnect, strlen(autoConnect));

  return true;
}
/*===========================================================================
  FUNCTION GetAutoconnect
==========================================================================*/
/*!
@brief
  Returns autoconnect flag value.

@parameters
  value

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetAutoconnect()
{
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());
  return auto_connect;
}
/*===========================================================================
  FUNCTION SetRoaming
==========================================================================*/
/*!
@brief
  This function will set the roaming flag value. Enabling the roaming flag
  will enable connecting to a Network in roaming mode.

@parameters
  boolean enable

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetRoaming(boolean enable, qmi_error_type_v01 *qmi_err_num)
{
  int ret_val;
  int qcmap_cm_errno;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  char roaming_state[MAX_STRING_LENGTH]={0};

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  wwan_cfg.roaming = enable;

  /* if it is OK, call write to XML */
  snprintf(roaming_state, sizeof(roaming_state), "%d", enable);
  GetSetWWANConfigFromXML(QcMapBackhaul->profileHandle, CONFIG_ROAMING, SET_VALUE, roaming_state, strlen(roaming_state));

  if (auto_connect)
  {
    /* Bring-up V4 and V6 data-call */
    ConnectV4AndV6();
  }

  return true;
}


/*===========================================================================
  FUNCTION GetRoaming
==========================================================================*/
/*!
@brief
  Returns roaming flag value.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetRoaming()
{
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  return wwan_cfg.roaming;
}
/*===========================================================================
  FUNCTION GetWWANStatistics
==========================================================================*/
/*!
@brief
  Used to Get the WWAN data transfer statistics.

@parameters
  qcmap_msgr_ip_family_enum_v01       ip_family
  qcmap_msgr_wwan_statistics_type_v01 *wwan_stats
  int                                 *p_error

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetWWANStatistics
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
  qcmap_msgr_wwan_statistics_type_v01 *wwan_stats,
  qmi_error_type_v01 *qmi_err_num
 )
{
  int ret_val = false;
  int rval;
  unsigned char data[DATA_SIZE];
  dsi_data_pkt_stats stats_info;
  dsi_hndl_t dsi_handle;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  /* Get WWAN Statistics */
  if (ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    dsi_handle = dsi_net_hndl.handle;
  }
  else if (ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    dsi_handle = ipv6_dsi_net_hndl.handle;
  }
  else
  {
    *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
    LOG_MSG_ERROR( "QCMAP CM Invalid family %d", ip_family, 0, 0);
    return false;
  }

  memset(&stats_info, 0, sizeof(stats_info));

  rval = dsi_get_pkt_stats(dsi_handle, &stats_info);

  if (rval != DSI_SUCCESS)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("dsi_get_pkt_stats Failed error", 0,0,0);
    return false;
  }

  wwan_stats->bytes_rx = stats_info.bytes_rx;
  wwan_stats->bytes_tx = stats_info.bytes_tx;
  wwan_stats->pkts_rx  = stats_info.pkts_rx;
  wwan_stats->pkts_tx  = stats_info.pkts_tx;
  wwan_stats->pkts_dropped_rx = stats_info.pkts_dropped_rx;
  wwan_stats->pkts_dropped_tx = stats_info.pkts_dropped_tx;

  return true;
}

/*===========================================================================
  FUNCTION ResetWWANStatistics
==========================================================================*/
/*!
@brief
  Resets WWAN data transfer statistics to zero.

@parameters
  qcmap_msgr_ip_family_enum_v01       ip_family
  int                                 *p_error

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::ResetWWANStatistics
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
  qmi_error_type_v01 *qmi_err_num
 )
{
  int ret_val = FALSE;
  int rval;
  dsi_hndl_t dsi_handle;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  /* Reset WWAN Statistics */
  if (ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    dsi_handle = dsi_net_hndl.handle;
  }
  else if (ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    dsi_handle = ipv6_dsi_net_hndl.handle;
  }
  else
  {
    *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
    LOG_MSG_ERROR( "QCMAP CM Invalid family %d", ip_family, 0, 0);
    return false;
  }

  rval = dsi_reset_pkt_stats(dsi_handle);

  if (rval != DSI_SUCCESS)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("dsi_reset_pkt_stats Failed error",0,0,0);
    return ret_val;
  }

  return TRUE;
}
/*===========================================================================
  FUNCTION GetWWANStatus
==========================================================================*/
/*!
@brief
  Gets the WWAN status.

@parameters
  uint8_t *status

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetWWANStatus
(
 qcmap_msgr_wwan_call_type_v01 call_type,
 uint8_t *status,
 qmi_error_type_v01 *qmi_err_num
 )
{
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if ( status == NULL )
  {
    LOG_MSG_ERROR("NULL args", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 )
    *status = state;
  else if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
    *status = ipv6_state;
  else
  {
    LOG_MSG_ERROR("Invalid Call Type:%d", call_type, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetWWANPolicy
==========================================================================*/
/*!
@brief
  Gets the WWAN profile configured value.

@parameters
  qcmap_msgr_net_policy_info_v01 *wwan_policy,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetWWANPolicy
(
  qcmap_msgr_get_wwan_policy_resp_msg_v01 *resp_msg,
  qmi_error_type_v01 *qmi_err_num
  )
{
  char val[MAX_STRING_LENGTH]={0};

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  /* Get WWAN config */
  resp_msg->wwan_policy.tech_pref = (qcmap_msgr_tech_pref_mask_v01) wwan_cfg.tech;
  resp_msg->wwan_policy.ip_family = QCMAP_MSGR_IP_FAMILY_V4V6_V01;
  resp_msg->wwan_policy.v4_profile_id_3gpp = wwan_cfg.profile_id.v4.umts_profile_index;
  resp_msg->wwan_policy.v4_profile_id_3gpp2 = wwan_cfg.profile_id.v4.cdma_profile_index;
  resp_msg->wwan_policy.v6_profile_id_3gpp = wwan_cfg.profile_id.v6.umts_profile_index;
  resp_msg->wwan_policy.v6_profile_id_3gpp2 = wwan_cfg.profile_id.v6.cdma_profile_index;

  return true;
}

/*===========================================================================
  FUNCTION GetWWANPolicyList
==========================================================================*/
/*!
@brief
  Gets ALL the WWAN profile configured value.

@parameters
  qcmap_msgr_wwan_policy_list_resp_msg_v01 *wwan_policy,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetWWANPolicyList
(
 qcmap_msgr_wwan_policy_list_resp_msg_v01 *resp_msg,
 qmi_error_type_v01                       *qmi_err_num
)
{
  int i = 0;
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  QCMAP_Backhaul *QcMapBackhaul;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ;
    QcMapBackhaul = GET_BACKHAUL_OBJ_AND_INC;

    resp_msg->wwan_policy_len++;
    resp_msg->wwan_policy[i].tech_pref = (qcmap_msgr_tech_pref_mask_v01)pBackhaulWWAN->wwan_cfg.tech;
    resp_msg->wwan_policy[i].ip_family = QCMAP_MSGR_IP_FAMILY_V4V6_V01;
    resp_msg->wwan_policy[i].v4_profile_id_3gpp = pBackhaulWWAN->wwan_cfg.profile_id.v4.umts_profile_index;
    resp_msg->wwan_policy[i].v4_profile_id_3gpp2 = pBackhaulWWAN->wwan_cfg.profile_id.v4.cdma_profile_index;
    resp_msg->wwan_policy[i].v6_profile_id_3gpp = pBackhaulWWAN->wwan_cfg.profile_id.v6.umts_profile_index;
    resp_msg->wwan_policy[i].v6_profile_id_3gpp2 = pBackhaulWWAN->wwan_cfg.profile_id.v6.cdma_profile_index;
    resp_msg->wwan_policy[i].profile_handle = QcMapBackhaul->profileHandle;
    LOG_MSG_INFO1("WWAN_Policy = %d,%d,%d", resp_msg->wwan_policy[i].tech_pref,
                    resp_msg->wwan_policy[i].ip_family, resp_msg->wwan_policy[i].v4_profile_id_3gpp);
    LOG_MSG_INFO1(" -Cont =%d, %d, %d",
                   resp_msg->wwan_policy[i].v4_profile_id_3gpp2, resp_msg->wwan_policy[i].v6_profile_id_3gpp,
                   resp_msg->wwan_policy[i].v6_profile_id_3gpp2);

    i++;
  }

  resp_msg->default_profile_handle_valid = TRUE;
  resp_msg->default_profile_handle = QCMAP_Backhaul::GetDefaultProfileHandle();

  return TRUE;
}

/*===========================================================================
  FUNCTION SetWWANPolicy
==========================================================================*/
/*!
@brief
  Sets the WWAN profile.

@parameters
  boolean                         isDefaultPDN
  qcmap_msgr_net_policy_info_v01 *wwan_policy,
  profile_handle_type_v01        *profile_handle,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetWWANPolicy
(
 qcmap_msgr_net_policy_info_v01  wwan_policy,
 qmi_error_type_v01             *qmi_err_num
 )
{
  QCMAP_Backhaul_WWAN *QCMAP_Backhaul_WWAN_Obj;
  boolean ret_val = false;
  char val[MAX_STRING_LENGTH] = {0};
  const char *tech_pref;

  if( wwan_policy.tech_pref != QCMAP_WAN_TECH_ANY && wwan_policy.tech_pref != QCMAP_WAN_TECH_3GPP &&
      wwan_policy.tech_pref != QCMAP_WAN_TECH_3GPP2)
  {
    *qmi_err_num = QMI_ERR_INVALID_TECH_PREF_V01;
    LOG_MSG_ERROR("Set wwan policy: invalid tech preference %d \n",
        wwan_policy.tech_pref, 0, 0);
    return false;
  }

  QCMAP_Backhaul_WWAN_Obj = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  if (QCMAP_Backhaul_WWAN_Obj)
  {
    if (QCMAP_Backhaul_WWAN_Obj->state > QCMAP_CM_ENABLE ||
         QCMAP_Backhaul_WWAN_Obj->ipv6_state > QCMAP_CM_V6_ENABLE)
    {
      LOG_MSG_ERROR("Cannot modify profile params while data-call is active.", 0,0,0);
      *qmi_err_num = QMI_ERR_DEVICE_IN_USE_V01;
      return FALSE;
    }

    /* Set WWAN config */
    switch (wwan_policy.ip_family)
    {
      case QCMAP_MSGR_IP_FAMILY_V4_V01:
        LOG_MSG_INFO1("SetWWANPolicy() v4: enter", 0, 0, 0);
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v4.umts_profile_index = wwan_policy.v4_profile_id_3gpp;
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v4.cdma_profile_index = wwan_policy.v4_profile_id_3gpp2;
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V4_UMTS_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp2);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V4_CDMA_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        ret_val = TRUE;
        break;

      case QCMAP_MSGR_IP_FAMILY_V6_V01:
        LOG_MSG_INFO1("SetWWANPolicy() v6: enter", 0, 0, 0);
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v6.umts_profile_index = wwan_policy.v6_profile_id_3gpp;
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v6.cdma_profile_index = wwan_policy.v6_profile_id_3gpp2;
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V6_UMTS_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp2);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V6_CDMA_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        ret_val = TRUE;
        break;

      case QCMAP_MSGR_IP_FAMILY_V4V6_V01:
        LOG_MSG_INFO1("SetWWANPolicy() v4v6: enter", 0, 0, 0);
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v4.umts_profile_index = wwan_policy.v4_profile_id_3gpp;
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v4.cdma_profile_index = wwan_policy.v4_profile_id_3gpp2;
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v6.umts_profile_index = wwan_policy.v6_profile_id_3gpp;
        QCMAP_Backhaul_WWAN_Obj->wwan_cfg.profile_id.v6.cdma_profile_index = wwan_policy.v6_profile_id_3gpp2;
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V4_UMTS_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp2);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V4_CDMA_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V6_UMTS_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp2);
        QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_V6_CDMA_PROFILE_INDEX, SET_VALUE, val, strlen(val));
        ret_val = TRUE;
        break;

      default:
        LOG_MSG_ERROR( "QCMAP CM Invalid family %d", wwan_policy.ip_family, 0, 0);
        *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
        return ret_val;
    }

    QCMAP_Backhaul_WWAN_Obj->wwan_cfg.tech = wwan_policy.tech_pref;
    if (wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_ANY_V01)
       snprintf(val, sizeof(val), "%s", TECH_ANY_TAG);
    else if (wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_3GPP_V01)
       snprintf(val, sizeof(val), "%s", TECH_3GPP_TAG);
    else if (wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_3GPP2_V01)
       snprintf(val, sizeof(val), "%s", TECH_3GPP2_TAG);
    QCMAP_Backhaul_WWAN_Obj->GetSetWWANConfigFromXML(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->profileHandle,
                                                     CONFIG_TECH, SET_VALUE, val, strlen(val));
  }

  return ret_val;
}

/*===========================================================================
FUNCTION CreateWWANPolicy
==========================================================================*/
/*!
@brief
  Creates WWAN policy and updates XML file.

@parameters
  qcmap_msgr_net_policy_info_v01 wwan_policy,
  profile_handle_type_v01 *profile_handle,
  qmi_error_type_v01  *qmi_err_num

@return
  true - success
  false - fail

@note
  Error Codes:
    - QMI_ERR_OP_DEVICE_UNSUPPORTED_V01 - If target is unsupported.
    - QMI_ERR_INVALID_ARG_V01 - If 3gpp v4/v6 profile_index already exists.
    - QMI_ERR_NO_FREE_PROFILE_V01 - If there are no free profiles.
                                    Max being QCMAP_MAX_NUM_BACKHAULS_V01.

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::CreateWWANPolicy
(
 qcmap_msgr_net_policy_info_v01   wwan_policy,
 profile_handle_type_v01         *profile_handle,
 qmi_error_type_v01              *qmi_err_num
 )
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  pugi::xml_document xml_file;
  pugi::xml_node     root, pdnNode, child, parent, natNode;
  char data[MAX_STRING_LENGTH] = { 0 };
  profile_handle_type_v01 free_profile;

  if ( !(IS_MULTI_PDN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN is not allowed on this target(%d)", QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return false;
  }

  LOG_MSG_INFO1("Creating policy", 0, 0, 0);
  if (IsDuplicateProfile(wwan_policy) == true)
  {
    LOG_MSG_ERROR("Profile already exists", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  LOAD_XML_FILE(xml_file, QcMapMgr);

  root = xml_file.child(System_TAG).child(ConcurrentMobileAPCfg_TAG);
  if (root == NULL)
  {
    root = xml_file.child(System_TAG).append_child(ConcurrentMobileAPCfg_TAG);
  }
  pdnNode = root.append_child(Cfg_TAG).append_child(MobileAPWanCfg_TAG);

  //Profile Tag
  free_profile = FindFreeProfileHandle();
  if (free_profile > QCMAP_MAX_NUM_BACKHAULS_V01)
  {
    LOG_MSG_ERROR("Max number of backhaul's reached", 0,0,0);
    *qmi_err_num = QMI_ERR_NO_FREE_PROFILE_V01;
    return false;
  }
  *profile_handle = free_profile;
  child = pdnNode.append_child(ProfileHandle_TAG);
  snprintf(data, sizeof(data), "%d", free_profile);
  child.append_child(pugi::node_pcdata).set_value(data);

  //Autoconnect
  child = pdnNode.append_child(AutoConnect_TAG);
  snprintf(data, sizeof(data), "%d", 0);
  child.append_child(pugi::node_pcdata).set_value(data);

  //Roaming
  child = pdnNode.append_child(Roaming_TAG);
  snprintf(data, sizeof(data), "%d", 0);
  child.append_child(pugi::node_pcdata).set_value(data);

  //Tech
  child = pdnNode.append_child(TECH_TAG);
  LOG_MSG_INFO1("Tech Pref=%d", wwan_policy.tech_pref, 0, 0);
  if (wwan_policy.tech_pref == QCMAP_WAN_TECH_3GPP2)
  {
    child.append_child(pugi::node_pcdata).set_value(TECH_3GPP2_TAG);
  }
  else if (wwan_policy.tech_pref == QCMAP_WAN_TECH_3GPP)
  {
    child.append_child(pugi::node_pcdata).set_value(TECH_3GPP_TAG);
  }
  else
  {
    child.append_child(pugi::node_pcdata).set_value(TECH_ANY_TAG);
  }

  //V4_CDMA_PROFILE
  child = pdnNode.append_child(V4_CDMA_PROFILE_INDEX_TAG);
  snprintf(data, sizeof(data), "%d", wwan_policy.v4_profile_id_3gpp2);
  child.append_child(pugi::node_pcdata).set_value(data);
  //V4_UMTS_PROFILE
  child = pdnNode.append_child(V4_UMTS_PROFILE_INDEX_TAG);
  snprintf(data, sizeof(data), "%d", wwan_policy.v4_profile_id_3gpp);
  child.append_child(pugi::node_pcdata).set_value(data);

  //V6_CDMA_PROFILE
  child = pdnNode.append_child(V6_CDMA_PROFILE_INDEX_TAG);
  snprintf(data, sizeof(data), "%d", wwan_policy.v6_profile_id_3gpp2);
  child.append_child(pugi::node_pcdata).set_value(data);
  //V6_UMTS_PROFILE
  child = pdnNode.append_child(V6_UMTS_PROFILE_INDEX_TAG);
  snprintf(data, sizeof(data), "%d", wwan_policy.v6_profile_id_3gpp);
  child.append_child(pugi::node_pcdata).set_value(data);

  //SIP Type
  child = pdnNode.append_child(DefaultSIPServerConfigType_TAG);
  snprintf(data, sizeof(data), "FQDN");
  child.append_child(pugi::node_pcdata).set_value(data);

  //SIP server Type
  child = pdnNode.append_child(DefaultSIPServerConfig_TAG);
  snprintf(data, sizeof(data), "abcd.com");
  child.append_child(pugi::node_pcdata).set_value(data);

  /********************* Default NAT Config ***********************/
  /****************************************************************/
  parent = pdnNode.parent();
  // add new NAT config here.
  natNode = parent.append_child(MobileAPNatCfg_TAG);

  //DMZ
  child = natNode.append_child(DmzIP_TAG);
  snprintf(data, sizeof(data), "0.0.0.0");
  child.append_child(pugi::node_pcdata).set_value(data);

  //IPSEC VPN
  child = natNode.append_child(EnableIPSECVpnPassthrough_TAG);
  snprintf(data, sizeof(data), "1");
  child.append_child(pugi::node_pcdata).set_value(data);

  //PPTP VPN
  child = natNode.append_child(EnablePPTPVpnPassthrough_TAG);
  snprintf(data, sizeof(data), "1");
  child.append_child(pugi::node_pcdata).set_value(data);

  //L2TP VPN
  child = natNode.append_child(EnableL2TPVpnPassthrough_TAG);
  snprintf(data, sizeof(data), "1");
  child.append_child(pugi::node_pcdata).set_value(data);

  //NAT Type
  child = natNode.append_child(NATType_TAG);
  snprintf(data, sizeof(data), "SYM");
  child.append_child(pugi::node_pcdata).set_value(data);
  /****************************************************************/

  if (!QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file))
  {
    LOG_MSG_ERROR("Save file failed", 0,0,0);
    return false;
  }

  LOG_MSG_INFO1("Creating Backhaul Object, profile=%p", *profile_handle, 0, 0);
  //Create Backhaul Object
  QCMAP_Backhaul::CreateBackhualAndRelatedObjects(*profile_handle, (void *) &pdnNode);

  LOG_MSG_INFO1("Creating policy - success", 0, 0, 0);
  return true;
}


/*===========================================================================
FUNCTION UpdateWWANPolicy
==========================================================================*/
/*!
@brief
  Updates WWAN policy and updates XML file.

@parameters
  qcmap_msgr_update_profile_enum_v01 update_req,
  qcmap_msgr_net_policy_info_v01 WWAN_policy,
  qmi_error_type_v01  *qmi_err_num

@return
  true - success
  false - fail

@note
  Error Codes:
    - QMI_ERR_OP_DEVICE_UNSUPPORTED_V01 - If target is unsupported.
    - QMI_ERR_DEVICE_IN_USE_V01 - If v4/v6 PDN is connected for current profile
    - QMI_ERR_INVALID_ARG_V01 - If 3gpp v4/v6 profile_index already exists.

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::UpdateWWANPolicy
(
  qcmap_msgr_update_profile_enum_v01 update_req,
  qcmap_msgr_net_policy_info_v01     WWAN_policy,
  qmi_error_type_v01                *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  boolean ret_val = false;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if ( !(IS_MULTI_PDN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN is not allowed on this target(%d)", QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return false;
  }

  if (state > QCMAP_CM_ENABLE || ipv6_state > QCMAP_CM_V6_ENABLE)
  {
    LOG_MSG_ERROR("PDN is active, Cannot be deleted. state=%d, ipv6_state=%d",
                    state, ipv6_state, 0);
    *qmi_err_num = QMI_ERR_DEVICE_IN_USE_V01;
    return FALSE;
  }

  LOG_MSG_INFO1("Profile (%p), updating WWAN policy, update_req=%d",
                  QcMapBackhaul->profileHandle,update_req,0);

  if ( (update_req == QCMAP_MSGR_UPDATE_V4_3GPP_PROFILE_INDEX_V01 ||
        update_req == QCMAP_MSGR_UPDATE_V6_3GPP_PROFILE_INDEX_V01 ||
        update_req == QCMAP_MSGR_UPDATE_ALL_PROFILE_INDEX_V01) &&
        IsDuplicateProfile(WWAN_policy) == true)
  {
    LOG_MSG_ERROR("Profile already exists/Invalid profile.", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  switch(update_req)
  {
    case QCMAP_MSGR_UPDATE_TECH_TYPE_V01:
    case QCMAP_MSGR_UPDATE_V4_3GPP_PROFILE_INDEX_V01:
    case QCMAP_MSGR_UPDATE_V4_3GPP2_PROFILE_INDEX_V01:
    case QCMAP_MSGR_UPDATE_V6_3GPP_PROFILE_INDEX_V01:
    case QCMAP_MSGR_UPDATE_V6_3GPP2_PROFILE_INDEX_V01:
    case QCMAP_MSGR_UPDATE_ALL_PROFILE_INDEX_V01:
      ret_val = UpdateProfileParam(update_req, WWAN_policy, qmi_err_num);
      break;

    case QCMAP_MSGR_SET_DEFAULT_PROFILE_V01:
      ret_val = SwapConcurrentConfigToDefault(qmi_err_num);
      break;

    default:
      LOG_MSG_ERROR("Invalid update req=%d", update_req, 0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      ret_val = false;
      break;
  }

  return ret_val;
}


/*===========================================================================
 FUNCTION DeleteWWANPolicy
==========================================================================*/
/*!
@brief
 Disconnects and Delete's WWAN profile.

@parameters
 profile_handle_type_v01 profile_handle,
 qmi_error_type_v01 *qmi_err_num

@return
 true  - on Success
 false - on Failure

@note
  Error Codes:
    - QMI_ERR_OP_DEVICE_UNSUPPORTED_V01 - If target is unsupported
    - QMI_ERR_DEVICE_IN_USE_V01 - If v4/v6 PDN is connected for current profile
    - QMI_ERR_INVALID_HANDLE_V01 - If deleting a default profile.
                                  User is not allowed to delete default profile.

Dependencies
 None

Side Effects
 None
*/
/*=========================================================================*/
 boolean QCMAP_Backhaul_WWAN::DeleteWWANPolicy
 (
  profile_handle_type_v01         profile_handle,
  qmi_error_type_v01             *qmi_err_num
  )
{
  pugi::xml_document       xml_file;
  pugi::xml_node           root, child;
  const char              *tag_ptr;
  pugi::xml_node           pdnNode;
  pugi::xml_node           searchNode;
  pugi::xml_node           concurrentCfgNode;
  const char              *profileHandle_tag = ProfileHandle_TAG;
  qmi_error_type_v01       qmi_error_type;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if ( !(IS_MULTI_PDN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("Multi-PDN is not allowed on this target(%)", QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return false;
  }

  QCMAP_Backhaul_WWAN *QCMAP_Backhaul_WWAN_Obj = GET_BACKHAUL_WWAN_OBJECT(profile_handle);

  if (profile_handle == QCMAP_Backhaul::GetDefaultProfileHandle())
  {
    LOG_MSG_ERROR("Default PDN cannot be deleted", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return FALSE;
  }

  if (QCMAP_Backhaul_WWAN_Obj == NULL)
  {
    LOG_MSG_ERROR("Profile doesn't exists", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return FALSE;
  }

  if (QCMAP_Backhaul_WWAN_Obj->state > QCMAP_CM_ENABLE ||
      QCMAP_Backhaul_WWAN_Obj->ipv6_state > QCMAP_CM_V6_ENABLE)
  {
    LOG_MSG_ERROR("PDN is active, Cannot be deleted. state=%d, ipv6_state=%d",
                    QCMAP_Backhaul_WWAN_Obj->state,
                    QCMAP_Backhaul_WWAN_Obj->ipv6_state, 0);
    *qmi_err_num = QMI_ERR_DEVICE_IN_USE_V01;
    return FALSE;
  }

  /* If current WWAN profile is associated to vlan, then
   *  delete vlan mapping before deleting WWAN profile.
   */
  if (QCMAP_Backhaul_WWAN_Obj && QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul &&
       QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->vlan_id != QCMAP_MSGR_VLAN_ID_NONE_V01)
  {
    LOG_MSG_INFO1("Current profile is mapped to vlan(%d), removing mapping",
                      QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->vlan_id, 0,0);
    QCMAP_Backhaul_WWAN_Obj->DeleteVlanMapping(QCMAP_Backhaul_WWAN_Obj->QcMapBackhaul->vlan_id, &qmi_error_type);
  }

  /* Remove profile from mobileap_cfg.xml */
  LOAD_XML_FILE(xml_file, QcMapMgr);
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);

  /* Only concurrent node to be deleted */
  concurrentCfgNode = xml_file.child(System_TAG).child(ConcurrentMobileAPCfg_TAG);
  for (pdnNode = concurrentCfgNode.child(Cfg_TAG);
          (pdnNode != NULL && (pdnNode.child(MobileAPWanCfg_TAG)));
          pdnNode = pdnNode.next_sibling(Cfg_TAG))
  {
    if (atol(pdnNode.child(MobileAPWanCfg_TAG).child(profileHandle_tag).child_value()) ==
            profile_handle)
    {
      searchNode = pdnNode;
      break;
    }
  }

  if (searchNode == NULL)
  {
    LOG_MSG_ERROR("Couldn't find PDN node for profile_handle=%p", profile_handle, 0, 0);
    return false;
  }

  if(!searchNode.parent().remove_child(searchNode))
  {
    LOG_MSG_ERROR("Couldn't remove child!!!", profile_handle, 0, 0);
    return false;
  }

  if (!QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file))
  {
    LOG_MSG_ERROR("Save file failed", 0,0,0);
    return false;
  }

  QCMAP_Firewall *QcMapFirewallObj = GET_FIREWALL_OBJECT(profile_handle);
  if( QcMapFirewallObj )
    QcMapFirewallObj->DeleteConfigFromFirewallXML();
  else
  {
    LOG_MSG_ERROR("QcMapFirewallObj is NULL \n", 0, 0, 0);
  }
  LOG_MSG_INFO1("Deleted profile (%p)", profile_handle, 0, 0);

  QCMAP_ConnectionManager::RemoveBackhaulObjectFromMap(profile_handle);
  return true;
}

/*===========================================================================
  FUNCTION GetDataBitrate(void)
==========================================================================*/
/*!
@brief
  Gets the current channel rate from dsi_netctrl

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetDataBitrate
(
 qcmap_msgr_data_bitrate_v01 *data_rate,
 qmi_error_type_v01 *qmi_err_num
 )
{
  boolean ret_val = false;
  dsi_data_channel_rate_t stats_info;
  dsi_hndl_t dsi_handle;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  memset(&stats_info, 0, sizeof(dsi_data_channel_rate_t));
  /* Get Data Bitrate */
  if(state == QCMAP_CM_WAN_CONNECTED)
  {
    dsi_handle = dsi_net_hndl.handle;
  }
  else
  {
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    LOG_MSG_ERROR( "QCMAP CM WAN is down", 0, 0, 0);
    return ret_val;
  }

  if (dsi_get_current_data_channel_rate(dsi_handle, &stats_info) == DSI_ERROR)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("dsi_get_current_data_channel Failed error",
        0,0,0);
    ret_val = false;
    return ret_val;
  }

  data_rate->tx_rate = stats_info.current_tx_rate;
  data_rate->rx_rate = stats_info.current_rx_rate;
  data_rate->max_tx_rate = stats_info.max_tx_rate;
  data_rate->max_rx_rate = stats_info.max_rx_rate;

  return true;
}


/*===========================================================================
  FUNCTION AddIPv4SIPServerInfoToList(void)
==========================================================================*/
/*!
@brief
  Adds SIP server information to the list.

@parameters
   sip_entry - sip server info, whose entry has to be added in to the link list

@return
  TRUE   - success
  false  - fail
@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::AddIPv4SIPServerInfoToList
(
 qcmap_cm_sip_server_info_t sip_entry
 )
{
  qcmap_sip_server_info_list_t *sip_server_list = &(this->sip_server_info_list);
  qcmap_cm_sip_server_info_t *new_sip_entry = NULL;
  ds_dll_el_t * node = NULL;
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (sip_server_list->sipServerEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
      information. This is done to make use of the doubly linked list framework which
      is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddIPv4SIPServerInfoToList - Error in allocating memory for node",
          0,0,0);
      return false;
    }
    sip_server_list->sipServerEntryListHead = node;
  }

  new_sip_entry = (qcmap_cm_sip_server_info_t*)ds_malloc(sizeof(qcmap_cm_sip_server_info_t));

  if( new_sip_entry == NULL )
  {
    LOG_MSG_ERROR("AddIPv4SIPServerInfoToList - Error in allocating memory for"
        "new NAT entry",0,0,0);
    return false;
  }

  memset(new_sip_entry, 0, sizeof(qcmap_cm_sip_server_info_t));

  /* save into the config */
  memcpy(new_sip_entry, &sip_entry, sizeof(qcmap_cm_sip_server_info_t));

  //Enque the node
  if ((node = ds_dll_enq(sip_server_list->sipServerEntryListHead,
          NULL, (void*)new_sip_entry )) == NULL)
  {
    LOG_MSG_ERROR("AddIPv4SIPServerInfoToList - Error in adding a node",0,0,0);
    ds_free(new_sip_entry);
    new_sip_entry = NULL;
    return false;
  }

  sip_server_list_count++;
  LOG_MSG_INFO1("sip_server_list_count = %d", sip_server_list_count,0,0);
  return TRUE;
}


/*===========================================================================
  FUNCTION  AddWWANIPv4SIPServerInfo
==========================================================================*/
/*!
@brief
- retrieves the network assigned PCSCF address list and PCSCF FQDN list
  from modem
- restarts dnsmasq with the network assigned PCSCF address and PCSCF
  FQDN list

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::AddWWANIPv4SIPServerInfo()
{
  dsi_pcscf_addr_info_t pcscf_addr_list;
  dsi_pcscf_fqdn_list_t pcscf_fqdn_list;
  qmi_error_type_v01    qmi_err_num;
  int                   ret_val;
  int                   addr_count;
  int                   fqdn_count;
  int                   dsi_err;
  int                   ret_val_fqdn;
  int                   ret_val_ip;
  struct sockaddr_in    *sin   = NULL;
  qcmap_cm_sip_server_info_t sip_entry;

  /*-------------------------------------------------------------------------*/
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLAN = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();

  memset(&pcscf_addr_list, 0, sizeof(dsi_pcscf_addr_info_t));
  memset(&pcscf_fqdn_list, 0, sizeof(dsi_pcscf_fqdn_list_t));

  if (QcMapBackhaul)
  {
    QCMAPLAN = GET_LAN_OBJECT(QcMapBackhaul->vlan_id);
  }

  if (QCMAPLAN == NULL)
  {
    LOG_MSG_ERROR("LAN Object is NULL!!!", 0,0,0);
    return false;
  }

  if (state != QCMAP_CM_WAN_CONNECTED)
  {
    LOG_MSG_ERROR( "QCMAP CM not in wan connected state", 0, 0, 0);
    return false;
  }

  ret_val_ip = dsi_iface_ioctl(dsi_net_hndl.handle,
                                DSI_IFACE_IOCTL_GET_PCSCF_SERV_ADDRESS,
                                &pcscf_addr_list,
                                &dsi_err);

  if(ret_val_ip == DSI_SUCCESS)
  {
    LOG_MSG_INFO1("Successfully got PCSCF server address", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Failure in getting PCSCF server address: %d", dsi_err, 0, 0);
  }

  ret_val_fqdn = dsi_iface_ioctl(dsi_net_hndl.handle,
                                  DSI_IFACE_IOCTL_GET_PCSCF_DOMAIN_NAME_LIST,
                                  &pcscf_fqdn_list,
                                  &dsi_err);

  if(ret_val_fqdn == DSI_SUCCESS)
  {
    LOG_MSG_INFO1("Successfully got PCSCF domain name list", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Failure in getting PCSCF domain name list: %d", dsi_err, 0, 0);
  }

  if(ret_val_ip != DSI_SUCCESS && ret_val_fqdn != DSI_SUCCESS)
  {
    qmi_err_num = QMI_ERR_INTERNAL_V01;
    ret_val = QCMAP_CM_ERROR;
  }
  else
  {
    qmi_err_num = QMI_ERR_NONE_V01;
    ret_val = QCMAP_CM_SUCCESS;
  }
  if (ret_val == QCMAP_CM_SUCCESS)
  {
    LOG_MSG_INFO1("Successfully obtained PCSCF info", 0, 0, 0);

    LOG_MSG_INFO1("Got %d PCSCF addresses", pcscf_addr_list.addr_count, 0, 0);

    for (addr_count = 0; addr_count < pcscf_addr_list.addr_count; addr_count ++)
    {
      if (pcscf_addr_list.pcscf_address[addr_count].valid_addr)
      {
        memset(&sip_entry, 0, sizeof(qcmap_cm_sip_server_info));
        sin = (struct sockaddr_in *)&pcscf_addr_list.pcscf_address[addr_count].addr;
        LOG_MSG_INFO1("pcscf_ip:0x%x", sin->sin_addr.s_addr, 0, 0);

        sip_entry.sip_serv_type = QCMAP_CM_SIP_SERVER_ADDR;
        sip_entry.sip_serv_info.sip_serv_ip = (uint32)sin->sin_addr.s_addr;
        AddIPv4SIPServerInfoToList(sip_entry);
      }
    }

    LOG_MSG_INFO1("Successfully got %d PCSCF FQDN", pcscf_fqdn_list.fqdn_count, 0, 0);

    for (fqdn_count = 0; fqdn_count < pcscf_fqdn_list.fqdn_count; fqdn_count ++)
    {
      if (pcscf_fqdn_list.pcscf_domain_list[fqdn_count].fqdn_length > 0)
      {
        memset(&sip_entry, 0, sizeof(qcmap_cm_sip_server_info));
        sip_entry.sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;
        memcpy
          (sip_entry.sip_serv_info.sip_fqdn,
           pcscf_fqdn_list.pcscf_domain_list[fqdn_count].fqdn_string,
           pcscf_fqdn_list.pcscf_domain_list[fqdn_count].fqdn_length);
        AddIPv4SIPServerInfoToList(sip_entry);
      }
    }
    QCMAPLAN->UpdateDHCPDNetworkInfo();
  }
  else
  {
    LOG_MSG_ERROR("Failure in getting PCSCF info %d", qmi_err_num, 0, 0);
    return false;
  }

  return true;
}
/*===========================================================================
  FUNCTION  DeleteWWANIPv4SIPServerInfo
==========================================================================*/
/*!
@brief
- resets the WWAN provided IPv4 SIP server information

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DeleteWWANIPv4SIPServerInfo()
{
  qcmap_sip_server_info_list_t *sip_server_list = NULL;
  QCMAP_LAN* QCMAPLAN = NULL;
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (QcMapBackhaul)
  {
    QCMAPLAN = GET_LAN_OBJECT(QcMapBackhaul->vlan_id);
  }

  if (QCMAPLAN == NULL)
  {
    LOG_MSG_ERROR("LAN Object is NULL!!!", 0,0,0);
    return false;
  }

  sip_server_list_count = 0;
  sip_server_list = &(this->sip_server_info_list);
  ds_dll_delete_all(sip_server_list->sipServerEntryListHead);

  /*making head pointer NULL*/
  sip_server_list->sipServerEntryListHead = NULL;
  sip_server_list->sipServerEntryListTail = NULL;
  QCMAPLAN->UpdateDHCPDNetworkInfo();

  return true;
}
/*===========================================================================
  FUNCTION SetSIPServerInfo
==========================================================================*/
/*!
@brief
  - Sets the default user configured SIP Server Information onto qcmap config.
  - Restarts DHCP server with the user provided SIP server information.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetSIPServerInfo
(
 qcmap_msgr_sip_server_info_v01 default_sip_server_info,
 qmi_error_type_v01 *qmi_err_num
 )
{
  qcmap_cm_sip_server_info_t default_sip_entry;
  QCMAP_LAN* QCMAPLAN = NULL;

  if (QcMapBackhaul)
  {
    QCMAPLAN = GET_LAN_OBJECT(QcMapBackhaul->vlan_id);
  }

  if (QCMAPLAN == NULL)
  {
    LOG_MSG_ERROR("LAN Object is NULL!!!", 0,0,0);
    return false;
  }

  memset(this->default_sip_server_info.sip_serv_info.sip_fqdn, 0, QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
  this->default_sip_server_info.sip_serv_type =
            (qcmap_cm_sip_server_type_e)default_sip_server_info.pcscf_info_type;

  if (this->default_sip_server_info.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
  {
    this->default_sip_server_info.sip_serv_info.sip_serv_ip =
              default_sip_server_info.pcscf_ip_addr;
  }
  else if (this->default_sip_server_info.sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
  {
    strlcpy(this->default_sip_server_info.sip_serv_info.sip_fqdn,
        default_sip_server_info.pcscf_fqdn,
        strlen(default_sip_server_info.pcscf_fqdn)+1);
  }

  LOG_MSG_INFO1("Default SIP server info type %d set", this->default_sip_server_info.sip_serv_type, 0, 0);

  default_sip_entry.sip_serv_type = (qcmap_cm_sip_server_type_e)default_sip_server_info.pcscf_info_type;
  if (default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
  {
    default_sip_entry.sip_serv_info.sip_serv_ip = default_sip_server_info.pcscf_ip_addr;
  }
  else
  {
    memset(default_sip_entry.sip_serv_info.sip_fqdn, 0, QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
    strlcpy(default_sip_entry.sip_serv_info.sip_fqdn,
        default_sip_server_info.pcscf_fqdn,
        QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
  }

  GetSetSIPServerConfigFromXML(SET_VALUE, &default_sip_entry);

  QCMAPLAN->UpdateDHCPDNetworkInfo();

  return true;
}

/*===========================================================================
  FUNCTION GetSIPServerInfo
==========================================================================*/
/*!
@brief
  - Retrives default user configured SIP server Information
  - Also retrieves network assigned SIP server information list.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetSIPServerInfo
(
 qcmap_msgr_sip_server_info_v01 *default_sip_info,
 qcmap_msgr_sip_server_info_v01 *network_sip_info,
 int *count_network_sip_info,
 qmi_error_type_v01 *qmi_err_num
 )
{
  int cnt;
  qcmap_sip_server_info_list_t *sip_server_list = NULL;
  qcmap_cm_sip_server_info_t *sip_entry = NULL;
  qcmap_cm_sip_server_info_t default_sip_entry;
  ds_dll_el_t * node = NULL;

  /*-------------------------------------------------------------------------*/
  if (state == QCMAP_CM_WAN_CONNECTED)
  {
    if (default_sip_server_info.sip_serv_type != QCMAP_CM_SIP_SERVER_MIN)
    {
      default_sip_info->pcscf_info_type =
        (qcmap_msgr_pcscf_info_type_v01)default_sip_server_info.sip_serv_type;

      if (default_sip_server_info.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      {
        default_sip_info->pcscf_ip_addr =
          default_sip_server_info.sip_serv_info.sip_serv_ip;
      }
      else if (default_sip_server_info.sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
      {
        strlcpy(default_sip_info->pcscf_fqdn,
            default_sip_server_info.sip_serv_info.sip_fqdn,
            strlen(default_sip_server_info.sip_serv_info.sip_fqdn)+1);
      }

      LOG_MSG_INFO1("Default SIP server info type %d retrieved",
          default_sip_info->pcscf_info_type, 0, 0);
    }
    else
    {
      LOG_MSG_INFO1("Default SIP server info not configured", 0, 0, 0);
    }
    sip_server_list = &(this->sip_server_info_list);

    if (sip_server_list_count > 0)
    {
      LOG_MSG_INFO1(" Sip server count: %d ", sip_server_list_count, 0, 0);
      LOG_MSG_INFO1("%d network assigned SIP server info present", sip_server_list_count, 0, 0);
      *count_network_sip_info = sip_server_list_count;

      node = sip_server_list->sipServerEntryListHead;
      node = ds_dll_next (node, (const void**)(&sip_entry));

      for (cnt=0; cnt < sip_server_list_count && sip_entry; cnt++)
      {
        network_sip_info[cnt].pcscf_info_type =
          (qcmap_msgr_pcscf_info_type_v01)sip_entry->sip_serv_type;
        if (network_sip_info[cnt].pcscf_info_type == QCMAP_CM_SIP_SERVER_ADDR)
        {
          network_sip_info[cnt].pcscf_ip_addr =
            sip_entry->sip_serv_info.sip_serv_ip;
        }
        else if (network_sip_info[cnt].pcscf_info_type == QCMAP_CM_SIP_SERVER_FQDN)
        {
          strlcpy(network_sip_info[cnt].pcscf_fqdn,
              sip_entry->sip_serv_info.sip_fqdn,
              strlen(sip_entry->sip_serv_info.sip_fqdn)+1);
        }
        node = ds_dll_next (node, (const void**)(&sip_entry));
      }
    }
    else
    {
      LOG_MSG_INFO1("Network SIP server info not available", 0, 0, 0);
    }
  }
  else
  {
    GetSetSIPServerConfigFromXML(GET_VALUE, &default_sip_entry);
    if (default_sip_entry.sip_serv_type != QCMAP_CM_SIP_SERVER_MIN)
    {
      default_sip_info->pcscf_info_type = (qcmap_msgr_pcscf_info_type_v01)default_sip_entry.sip_serv_type;
    }
    if(default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      default_sip_info->pcscf_ip_addr = default_sip_entry.sip_serv_info.sip_serv_ip;
    else
    {
      strlcpy(default_sip_info->pcscf_fqdn,
          default_sip_entry.sip_serv_info.sip_fqdn,
          QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
    }
  }
  return true;
}
/*===========================================================================
  FUNCTION GetV6SIPServerInfo
==========================================================================*/
/*!
@brief
  -retrieves network assigned SIP server information list.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetV6SIPServerInfo
(
 qcmap_msgr_ipv6_sip_server_info_v01 *network_v6_sip_info,
 int *count_network_v6_sip_info,
 qmi_error_type_v01 *qmi_err_num
 )
{
  int                   dsi_err;
  int                   ret_val_fqdn;
  int                   ret_val_ip;
  dsi_pcscf_addr_info_t pcscf_addr_list;
  dsi_pcscf_fqdn_list_t pcscf_fqdn_list;
  int                   addr_count, count = 0;
  int                   ret_val = QCMAP_CM_SUCCESS;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.\n",0,0,0);
    return false;
  }
  if ( network_v6_sip_info == NULL || count_network_v6_sip_info == NULL )
  {
    LOG_MSG_ERROR("NULL params.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }
  if (!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr pointer is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }
  if (QcMapBackhaulWWANMgr->ipv6_state != QCMAP_CM_V6_WAN_CONNECTED)
  {
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    LOG_MSG_ERROR( "QCMAP CM not in v6 wan connected state", 0, 0, 0);
    return false;
  }

  memset(&pcscf_addr_list, 0, sizeof(dsi_pcscf_addr_info_t));
  memset(&pcscf_fqdn_list, 0, sizeof(dsi_pcscf_fqdn_list_t));
  *count_network_v6_sip_info = 0;

  /*-------------------------------------------------------------------------*/

  QCMAP_CM_LOG_FUNC_ENTRY();

  ret_val_ip = dsi_iface_ioctl(QcMapBackhaulWWANMgr->ipv6_dsi_net_hndl.handle,
                                 DSI_IFACE_IOCTL_GET_PCSCF_SERV_ADDRESS,
                                 &pcscf_addr_list,
                                 &dsi_err);

  if(ret_val_ip == DSI_SUCCESS)
  {
    LOG_MSG_INFO1("Successfully got IPV6 PCSCF server address", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Failure in getting IPV6 PCSCF server address: %d", dsi_err, 0, 0);
  }

  ret_val_fqdn = dsi_iface_ioctl(QcMapBackhaulWWANMgr->ipv6_dsi_net_hndl.handle,
                                  DSI_IFACE_IOCTL_GET_PCSCF_DOMAIN_NAME_LIST,
                                  &pcscf_fqdn_list,
                                  &dsi_err);

  if(ret_val_fqdn == DSI_SUCCESS)
  {
    LOG_MSG_INFO1("Successfully got IPV6 PCSCF domain name list", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Failure in getting IPV6 PCSCF domain name list: %d", dsi_err, 0, 0);
  }

  if(ret_val_ip != DSI_SUCCESS && ret_val_fqdn != DSI_SUCCESS)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }
  else
  {
    *qmi_err_num = QMI_ERR_NONE_V01;
    ret_val = QCMAP_CM_SUCCESS;
  }

  LOG_MSG_INFO1("GetV6SIPServerInfo(): pcscf_addr, rval:%d", ret_val, 0, 0);
  if(ret_val != QCMAP_CM_SUCCESS)
  {
    LOG_MSG_ERROR("Couldn't get pcscf ipv6  address. rval %d", ret_val, 0, 0);
    return false;
  }
  else
  {
    LOG_MSG_INFO1("Got %dpcscf %dFQDN IPv6 Address, rval:%d", pcscf_addr_list.addr_count,
                   pcscf_fqdn_list.fqdn_count, ret_val);
  }

  // store valid IPV6 pcscf address recieved
  for (addr_count = 0; addr_count < pcscf_addr_list.addr_count; addr_count ++)
  {
    if (pcscf_addr_list.pcscf_address[addr_count].valid_addr)
    {
      if (SASTORAGE_FAMILY(pcscf_addr_list.pcscf_address[addr_count].addr) == AF_INET6)
      {
        memcpy(network_v6_sip_info[count].pcscf_ipv6_addr ,
               SASTORAGE_DATA(pcscf_addr_list.pcscf_address[addr_count].addr),
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
        network_v6_sip_info[count].pcscf_info_type =
                   (qcmap_msgr_pcscf_info_type_v01)QCMAP_CM_SIP_SERVER_ADDR;
        count++;
      }
      else
      {
        LOG_MSG_INFO1("Network IPV6 SIP server info not available", 0, 0, 0);
      }
    }
    else
    {
      LOG_MSG_INFO1("Network IPV6 SIP server %d info not valid", addr_count, 0, 0);
    }
  }

  // store valid pcscf fqdn list rcieved
  for (addr_count = 0; addr_count < pcscf_fqdn_list.fqdn_count; addr_count ++)
  {
    if (pcscf_fqdn_list.pcscf_domain_list[addr_count].fqdn_length > 0)
    {
      memcpy(network_v6_sip_info[count].pcscf_fqdn,
             pcscf_fqdn_list.pcscf_domain_list[addr_count].fqdn_string,
             pcscf_fqdn_list.pcscf_domain_list[addr_count].fqdn_length);
      network_v6_sip_info[count].pcscf_info_type =
               (qcmap_msgr_pcscf_info_type_v01)QCMAP_CM_SIP_SERVER_FQDN;
      count++;
    }
    else
    {
      LOG_MSG_INFO1("Network %d IPV6 SIP server FQDN not available", addr_count, 0, 0);
    }
  }
  *count_network_v6_sip_info = count;
  LOG_MSG_INFO1("Network %d IPV6 SIP server  available", count, 0, 0);
  return true;
}

/*===========================================================================

FUNCTION SendDeleteDelegatedPrefix()

DESCRIPTION
 - Removes a single prefix if prefix_valid is set, otherwise removes all
 delegated prefix's

@parameters
  prefix_valid - Boolean to flush single or all
  ipv6_addr    - prefix to delete
  qmi_err_num  - error no.

@return
  0  - Success
  -1 - Failure

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::SendDeleteDelegatedPrefix
(
  boolean             prefix_valid,/*Boolean to flush single or all*/
  uint8_t             *ipv6_addr,  /*Prefix to delete*/
  qmi_error_type_v01  *qmi_err_num /*QMI error number*/
)
{
  dsi_delegated_ipv6_prefix_type ipv6_prefix;
  int dsi_err,ret_val;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  *qmi_err_num = QMI_ERR_NONE_V01;

  LOG_MSG_INFO1("Entered SendDeleteDelegatedPrefix", 0, 0, 0);


  if (QcMapBackhaulWWANMgr == NULL)
  {
    LOG_MSG_ERROR("No Backhaul object", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return  QCMAP_CM_ERROR;
  }

  memset(&ipv6_prefix, 0, sizeof(ipv6_prefix));

  if(prefix_valid)
  {
    memcpy(ipv6_prefix.ipv6_addr, ipv6_addr, QMI_WDS_IPV6_ADDR_LEN_V01);
    ipv6_prefix.prefix_len = IPV6_MIN_PREFIX_LENGTH;
  }

  ret_val = dsi_iface_ioctl(QcMapBackhaulWWANMgr->ipv6_dsi_net_hndl.handle,
                                 DSI_IFACE_IOCTL_REMOVE_DELEGATED_IPV6_PREFIX,
                                 &ipv6_prefix,
                                 &dsi_err);
  if(ret_val == DSI_SUCCESS)
  {
    LOG_MSG_INFO1("Successfully deleted delegated ipv6 prefix", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("Failure in deleting delegated ipv6 prefix: %d", dsi_err, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return  QCMAP_CM_ERROR;
  }
  return QCMAP_CM_SUCCESS;
}

/*===========================================================================
  FUNCTION DeleteDelegatedPrefix
==========================================================================*/
/*!
@brief
  flushes client prefixes

@parameters
  mac_addr    - mac address
  qmi_err_num - error no.

@return
  true  - Prefix Delegation config set
  false - Prefix Delegation config not set

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DeleteDelegatedPrefix(void *mac_addr, qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  uint8 mac_addr_hex[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  unsigned int num_entries, i;
  qcmap_msgr_connected_device_info_v01 connected_devices[QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01];

  if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated)
  {
    //Convert the MAC from char to hex
    if(!ds_mac_addr_pton((char*)mac_addr, mac_addr_hex))
    {
      LOG_MSG_ERROR("DeleteClientAddrInfo - Error in MAC address conversion", 0,0,0);
      return false;
    }
    mac_addr = (void*)mac_addr_hex;

    if(QcMapMgr->GetConnectedDevicesInfo(connected_devices,
                                          &num_entries, qmi_err_num))
    {
      for (i = 0; i < num_entries; i++)
      {
        if (memcmp(connected_devices[i].client_mac_addr, mac_addr, sizeof(mac_addr)) == 0)
          break;
      }
      if(i == num_entries)
      {
        LOG_MSG_ERROR("DeleteClientAddrInfo - Couldn't find device with mac addr", 0,0,0);
        return false;
      }
      /*deleting only delegated prefix here.This can be done on first ipv6 address.
        This is how we did before merging packet stats feature*/
      if (QCMAP_Backhaul_WWAN::SendDeleteDelegatedPrefix(true,
                                                 connected_devices[i].ipv6[0].addr,
                                                 qmi_err_num) == QCMAP_CM_ERROR)
      {
        LOG_MSG_ERROR("Error: Unable flush prefix's %d", *qmi_err_num, 0, 0);
      }
    }
    else{
      LOG_MSG_ERROR("Error: Unable get connectedDevices %d", *qmi_err_num, 0, 0);

      return false;
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION GetPrefixDelegationConfig
==========================================================================*/
/*!
@brief
  Retreives the Prefix Delegation config.

@parameters
  char *process

@return
  true  - Prefix Delegation config set
  flase - Prefix Delegation config not set

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetPrefixDelegationConfig
(
 boolean *pd_mode,
 qmi_error_type_v01 *qmi_err_num
 )
{
  char enable[MAX_STRING_LENGTH] = {0};

  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_Backhaul *QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.\n",0,0,0);
    return false;
  }

  if (pd_mode == NULL)
  {
    LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr pointer is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }
  if(QcMapBackhaulWWANMgr)
    *pd_mode = QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation;
  else
  {
    QcMapBackhaulWWANMgr->GetSetWWANConfigFromXML(QcMapBackhaul->profileHandle,
                                                  CONFIG_PREFIX_DELEGATION, GET_VALUE,
                                                  enable, MAX_STRING_LENGTH);
    *pd_mode=(boolean)(atoi(enable));
  }
  return true;

}
/*===========================================================================
  FUNCTION SetPrefixDelegationConfig
==========================================================================*/
/*!
@brief
  Enable/disable the Prefix Delegation config.

@parameters
  char *process

@return
  true  - Prefix Delegation config set
  flase - Prefix Delegation config not set

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetPrefixDelegationConfig
(
 boolean pd_mode,
 qmi_error_type_v01 *qmi_err_num
 )
{
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_Backhaul *QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
  char val[MAX_STRING_LENGTH] = {0};

  snprintf(val, sizeof(val), "%d", pd_mode);
  if (qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("NULL Arg.\n", 0,0,0);
    return false;
  }

  if(pd_mode == TRUE && QCMAP_Backhaul::IsPmipV6ModeEnabled())
  {
    LOG_MSG_ERROR("PMIP mode is already set. This is NOT supported\n", 0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapBackhaulWWANMgr || !QcMapBackhaul)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr value: %p, QcMapBackhaul value: %p", QcMapBackhaulWWANMgr, QcMapBackhaul, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if (pd_mode == QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation)
  {
    LOG_MSG_ERROR("Prefix delegation config requested is already set.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation = pd_mode;
  QcMapBackhaulWWANMgr->GetSetWWANConfigFromXML(QcMapBackhaul->profileHandle, CONFIG_PREFIX_DELEGATION,
                                                SET_VALUE, val, MAX_STRING_LENGTH);

  if ( QcMapBackhaulWWANMgr->ipv6_state == QCMAP_CM_V6_WAN_CONNECTED )
  {
    LOG_MSG_ERROR("Prefix delegation config set, but v6 call is active. Will "
        "take effect on next Call\n",0,0,0);
    *qmi_err_num = QMI_ERR_DEVICE_IN_USE_V01;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION GetPrefixDelegationStatus
  ==========================================================================*/
/*!
  @brief
   Retreives the current Prefix Delegation mode.

  @parameters
  char *process

  @return
  true  - Prefix Delegation mode set
  flase - Prefix Delegation mode not set

  @note
  - Dependencies
  - None

  - Side Effects
  - None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetPrefixDelegationStatus
(
 boolean *pd_mode,
 qmi_error_type_v01 *qmi_err_num
 )
{
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.\n",0,0,0);
    return false;
  }

  if (pd_mode == NULL)
  {
    LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr pointer is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  *pd_mode = false;

  /* If WAN v6 call is disconnected or STA/Cradle is active, we return true,
     but pd_mode is always false */
  if ( QcMapBackhaulWWANMgr->ipv6_state != QCMAP_CM_V6_WAN_CONNECTED )
  {
    LOG_MSG_ERROR("Prefix delegation can only be enabled when a v6 call is active\n", 0,0,0);
    return true;
  }

  /* We only have control for Prefix Delegation over WAN */
  if (QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    LOG_MSG_ERROR("STA/Cradle connected, Prefix Delegation query invalid\n", 0,0,0);
    return true;
  }

  *pd_mode = QcMapBackhaulWWANMgr->prefix_delegation_activated;
  return true;
}


/*===========================================================================
  FUNCTION GetSetDDNSConfigFromXML
  ==========================================================================*/
/*!
  @brief
    gets/sets dynamic dns config from xml

  @parameters
    action      - action get/set
    ddns_config - dynamic dns config struct

  @return
  true  - success
  flase - fail

  @note
  - Dependencies
  - None

  - Side Effects
  - None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML
(
 qcmap_action_type action,
 qcmap_dynamic_dns_t *ddns_config
 )
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  LOAD_XML_FILE(xml_file, QcMapMgr);
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);

  if (action == SET_VALUE)
  {
    snprintf(data, sizeof(data), "%d", ddns_config->enable);
    root.child(DDNS_Enable_TAG).text() = data;

    root.child(DDNS_Server_TAG).text() = ddns_config->server_info.server_url;
    root.child(DDNS_Protocol_TAG).text() = ddns_config->server_info.protocol;
    root.child(DDNS_Login_TAG).text() = ddns_config->login;
    root.child(DDNS_Password_TAG).text() = ddns_config->password;
    root.child(DDNS_Hostname_TAG).text() = ddns_config->hostname;

    snprintf(data, sizeof(data), "%d", ddns_config->timeout);
    root.child(DDNS_Timeout_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    ddns_config->enable = atoi(root.child(DDNS_Enable_TAG).child_value());

    strlcpy(ddns_config->server_info.server_url,
            root.child(DDNS_Server_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

    strlcpy(ddns_config->server_info.protocol,
            root.child(DDNS_Protocol_TAG).child_value(), QCMAP_DDNS_PROTOCOL_LEN);

    strlcpy(ddns_config->login,
            root.child(DDNS_Login_TAG).child_value(), QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);

    strlcpy(ddns_config->password,
            root.child(DDNS_Password_TAG).child_value(), QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);

    strlcpy(ddns_config->hostname,
          root.child(DDNS_Hostname_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

    ddns_config->timeout = atoi(root.child(DDNS_Timeout_TAG).child_value());
  }
  LOG_MSG_INFO1("DDNS Config: Enable: %d ", ddns_config->enable, 0, 0);
  LOG_MSG_INFO1("Server: %s Protocol: %s Login: %s",
                ddns_config->server_info.server_url,
                ddns_config->server_info.protocol, ddns_config->login);

  LOG_MSG_INFO1("Password: %s Hostname: %s Timeout: %d",
                ddns_config->password,
                ddns_config->hostname, ddns_config->timeout);
  return true;
}


/*===========================================================================
  FUNCTION DeleteDDNS
==========================================================================*/
/*!
@brief
  The function will delete the dyndns configuration values

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DeleteDDNS()
{
  char command[MAX_COMMAND_STR_LEN];

  LOG_MSG_INFO1("Deleting old configuration", 0, 0, 0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/server=.*/server=/g %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/protocol=.*/protocol=/g %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  snprintf(command,
      MAX_COMMAND_STR_LEN,
      "sed s/daemon=.*/daemon=/g %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  snprintf(command,
      MAX_COMMAND_STR_LEN,
      "sed s/login=.*/login=/g %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/password=.*/password=/g %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/use=if, if=.*/use=if, if=/g %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/usev6=if, if=.*/usev6=if, if=/g %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));

  if(strlen(QCMAP_Backhaul_WWAN::ddns_conf.hostname)!=0)
  {
    snprintf(command,
        MAX_COMMAND_STR_LEN,
        "sed '/%s/d' %s", QCMAP_Backhaul_WWAN::ddns_conf.hostname,QCMAP_DDNS_CFG_PATH);
    qcmap_edit_file(command, strlen(command) ,QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  }

  return true ;
}


/*===========================================================================
  FUNCTION EnableDDNS
==========================================================================*/
/*!
@brief
  Enable Dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::StartDDNS(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();

  LOG_MSG_INFO1("Enable DDNS ",0,0,0);

  if((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE) ||
      (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER))
  {
    LOG_MSG_ERROR("STA or Cradle backhaul is connected \n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  if (!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr pointer is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  /* supported of IPV4 only */
  if( QcMapBackhaulWWANMgr->GetState() != QCMAP_CM_WAN_CONNECTED &&
      QcMapBackhaulWWANMgr->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED)//hence assuming wwan object to be available beyond this point and using directly
  {
    LOG_MSG_ERROR("QCMAP backhaul not connected \n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  /*only stop DDNS*/
  QcMapBackhaulWWANMgr->StopDDNS();

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed 's/use=if, if=.*/use=if, if=/g' %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed 's/usev6=if, if=.*/usev6=if, if=/g' %s",QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "rm %s",QCMAP_DDNS_CACHE_PATH);
  ds_system_call(command, strlen(command));

  if ( QcMapBackhaulWWANMgr->GetDeviceName(QcMapMgr->qcmap_cm_handle,
                                             QCMAP_MSGR_IP_FAMILY_V4_V01,
                                             devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("Couldn't get ipv4 rmnet name. error %d\n", qcmap_cm_error,0,0);
  }
  else
  {
    snprintf(command,
        MAX_COMMAND_STR_LEN,
        "sed \"s/use=if, if=.*/use=if, if=%s/g\" %s",devname,QCMAP_DDNS_CFG_PATH);
    qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
    LOG_MSG_INFO1(" Interface used for ipv4 ddclient: %s ",devname,0,0);
  }

  if ( QcMapBackhaulWWANMgr->GetDeviceName(QcMapMgr->qcmap_cm_handle,
                                             QCMAP_MSGR_IP_FAMILY_V6_V01,
                                             devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d\n", qcmap_cm_error,0,0);
  }
  else
  {
    snprintf(command,
        MAX_COMMAND_STR_LEN,
        "sed \"s/usev6=if, if=.*/usev6=if, if=%s/g\" %s",devname,QCMAP_DDNS_CFG_PATH);
    qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
    LOG_MSG_INFO1(" Interface used for ipv6 ddclient: %s ",devname,0,0);
  }

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "ddclient -file %s &",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION SetDDNS
==========================================================================*/
/*!
@brief
  Set dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetDDNS
(
 qcmap_msgr_set_dynamic_dns_config_req_msg_v01 setddns,
 qmi_error_type_v01 *qmi_err_num
 )
{
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  char command[MAX_COMMAND_STR_LEN];
  qcmap_dynamic_dns_t ddns_conf, *ddns_ptr = NULL;

  if(QCMAP_Backhaul::IsPmipV6ModeEnabled() || QCMAP_Backhaul::IsPmipV4ModeEnabled())
  {
    LOG_MSG_ERROR("PMIP mode is already set. This is NOT supported\n", 0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if(QcMapBackhaulWWANMgr)
  {
    QcMapBackhaulWWANMgr->DeleteDDNS();
    if( setddns.timeout_valid)
      QCMAP_Backhaul_WWAN::ddns_conf.timeout = setddns.timeout;
    bzero(&(QCMAP_Backhaul_WWAN::ddns_conf.hostname),QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.hostname,setddns.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    if(!strncmp(QCMAP_Backhaul_WWAN::ddns_server_info.server_url,setddns.ddns_server,
                strlen(QCMAP_Backhaul_WWAN::ddns_server_info.server_url)))
    {
      strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.server_info.server_url,
              setddns.ddns_server, QCMAP_MSGR_DDNS_URL_LENGTH_V01);

      strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.server_info.protocol,
              QCMAP_Backhaul_WWAN::ddns_server_info.protocol,
              QCMAP_DDNS_PROTOCOL_LEN);
    }
    else
    {
      LOG_MSG_ERROR("Server URL not supported : %s\n", setddns.ddns_server, 0, 0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.login,setddns.login,QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);
    strlcpy(QCMAP_Backhaul_WWAN::ddns_conf.password,setddns.password,QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);
    ddns_ptr = &(QCMAP_Backhaul_WWAN::ddns_conf);
  }
  else
  {
    memset(&ddns_conf, 0, sizeof(ddns_conf));
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(GET_VALUE, &ddns_conf);
    if( setddns.timeout_valid)
      ddns_conf.timeout = setddns.timeout;

    bzero(&(ddns_conf.hostname),QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    strlcpy(ddns_conf.hostname,setddns.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

    if(!strncmp(QCMAP_DYNDNS_SERVER_URL, setddns.ddns_server,
                QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01))
    {
      bzero(&(ddns_conf.server_info),sizeof(qcmap_ddns_server_info_t));
      strlcpy(ddns_conf.server_info.server_url,setddns.ddns_server,QCMAP_MSGR_DDNS_URL_LENGTH_V01);
      strlcpy(ddns_conf.server_info.protocol, QCMAP_DYNDNS_PROTOCOL,QCMAP_DDNS_PROTOCOL_LEN);
    }
    else
    {
      LOG_MSG_ERROR("Server URL not supported : %s\n", setddns.ddns_server, 0, 0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }

    bzero(&(ddns_conf.login),QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);
    strlcpy(ddns_conf.login,setddns.login,QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);
    bzero(&(ddns_conf.password),QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);
    strlcpy(ddns_conf.password,setddns.password,QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);
    ddns_ptr = &ddns_conf;
  }

  LOG_MSG_INFO1("A server URL =%s ",ddns_ptr->server_info.server_url,0,0);
  LOG_MSG_INFO1("A login      =%s ",ddns_ptr->login,0,0);
  LOG_MSG_INFO1("A password   =%s ",ddns_ptr->password,0,0);
  LOG_MSG_INFO1("A hostname   =%s ",ddns_ptr->hostname,0,0);
  LOG_MSG_INFO1("A timeout    =%d ",ddns_ptr->timeout,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/login=.*/login=%s/g %s",ddns_ptr->login,QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  LOG_MSG_INFO1(" LOGIN : %s ",ddns_ptr->login,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed \"s/password=.*/password=%s/g\" %s",ddns_ptr->password,QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  LOG_MSG_INFO1(" PASSWORD : %s ",ddns_ptr->password,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/server=.*/server=%s/g %s",ddns_ptr->server_info.server_url,QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  LOG_MSG_INFO1(" SERVER URL : %s ",ddns_ptr->server_info.server_url,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/protocol=.*/protocol=%s/g %s",ddns_ptr->server_info.protocol,QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  LOG_MSG_INFO1(" PROTOCOL : %s ",ddns_ptr->server_info.protocol,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/\"# your-dynamic-host.dyndns.org.*\"/\"# your-dynamic-host.dyndns.org \\n %s\"/g %s",ddns_ptr->hostname,QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  LOG_MSG_INFO1(" Enable HOSTNAME : %s ",ddns_ptr->hostname,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed s/daemon=.*/daemon=%d/g %s",ddns_ptr->timeout,QCMAP_DDNS_CFG_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_DDNS_CFG_PATH, strlen(QCMAP_DDNS_CFG_PATH));
  LOG_MSG_INFO1(" timeout : %d ",ddns_ptr->timeout,0,0);

  QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(SET_VALUE, ddns_ptr);
  /*restart ddns*/
  if( QcMapBackhaulWWANMgr && !QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if ( QcMapBackhaulWWANMgr->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED
         && QcMapBackhaulWWANMgr->GetState() != QCMAP_CM_WAN_CONNECTED )
    {
      LOG_MSG_INFO1("none of backhaul is enable", 0, 0, 0);
    }
    else if(QCMAP_Backhaul_WWAN::ddns_conf.enable == DDNS_ENABLED)
    {
      QcMapBackhaulWWANMgr->StartDDNS(qmi_err_num);
    }
  }

  return true ;
}


/*===========================================================================
  FUNCTION GetDDNS
==========================================================================*/
/*!
@brief
  Get dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetDDNS(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 *getddns,qmi_error_type_v01 *qmi_err_num)
{
  int len=0;
  qcmap_dynamic_dns_t ddns_conf;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  if(QcMapBackhaulWWANMgr)
  {
    len = strlen(QCMAP_Backhaul_WWAN::ddns_server_info.server_url);
    if(len == 0)
    {
      LOG_MSG_ERROR("invalid argument\n", 0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    LOG_MSG_INFO1(" GetDDNS  ",0,0,0);
    LOG_MSG_INFO1(" Hostname =%s ", QCMAP_Backhaul_WWAN::ddns_conf.hostname,0,0);
    LOG_MSG_INFO1(" server URL =%s ",QCMAP_Backhaul_WWAN::ddns_server_info.server_url,0,0);
    LOG_MSG_INFO1(" timeout =%d ", QCMAP_Backhaul_WWAN::ddns_conf.timeout,0,0);
    getddns->timeout = QCMAP_Backhaul_WWAN::ddns_conf.timeout;
    getddns->timeout_valid = true;
    strlcpy(getddns->hostname,QCMAP_Backhaul_WWAN::ddns_conf.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    getddns->hostname_valid = true;
    strlcpy( getddns->ddns_config[0].server_url,
             QCMAP_Backhaul_WWAN::ddns_server_info.server_url,
             QCMAP_MSGR_DDNS_URL_LENGTH_V01);
    /* Currently we are supportying only one Server */
    getddns->ddns_config_len = 1;
    if( QCMAP_Backhaul_WWAN::ddns_conf.enable == DDNS_ENABLED)
      getddns->enable = TRUE;
    else
      getddns->enable = false;
  }
  else
  {
    memset(&ddns_conf, 0, sizeof(ddns_conf));
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(GET_VALUE, &ddns_conf);
    len = strlen(ddns_conf.server_info.server_url);
    if(len == 0)
    {
      LOG_MSG_ERROR("invalid argument\n", 0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    getddns->timeout = ddns_conf.timeout;
    getddns->timeout_valid = true;
    strlcpy(getddns->hostname, ddns_conf.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    getddns->hostname_valid = true;
    strlcpy( getddns->ddns_config[0].server_url,
             ddns_conf.server_info.server_url,
             QCMAP_MSGR_DDNS_URL_LENGTH_V01);
    /* Currently we are supportying only one Server */
    getddns->ddns_config_len = 1;
    if(ddns_conf.enable == DDNS_ENABLED)
      getddns->enable = TRUE;
    else
      getddns->enable = false;
  }
  return true ;
}

/*===========================================================================
  FUNCTION EnableDDNS
==========================================================================*/
/*!
@brief
  Enable dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::EnableDDNS(qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(QCMAP_Backhaul::IsPmipV6ModeEnabled() || QCMAP_Backhaul::IsPmipV4ModeEnabled())
  {
    LOG_MSG_ERROR("PMIP mode is already set. This is NOT supported\n", 0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if(QcMapBackhaulWWANMgr)
  {
    QCMAP_Backhaul_WWAN::ddns_conf.enable = DDNS_ENABLED;
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(SET_VALUE, &QCMAP_Backhaul_WWAN::ddns_conf);
    QcMapBackhaulWWANMgr->StartDDNS(qmi_err_num);
  }
  else
  {
    /* Update XML file directly. */
    LOAD_XML_FILE(xml_file, QcMapMgr);
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);

    snprintf(data, sizeof(data), "%d", DDNS_ENABLED);
    root.child(DDNS_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  return true ;
}

/*===========================================================================
  FUNCTION DisableDDNS
==========================================================================*/
/*!
@brief
  Disable dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DisableDDNS(qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if(QcMapBackhaulWWANMgr)
  {
    QcMapBackhaulWWANMgr->StopDDNS();
    QCMAP_Backhaul_WWAN::ddns_conf.enable = DDNS_DISABLED;
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(SET_VALUE, &QCMAP_Backhaul_WWAN::ddns_conf);
  }
  else
  {
    /* Update XML file directly. */
    LOAD_XML_FILE(xml_file, QcMapMgr);
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);

    snprintf(data, sizeof(data), "%d", DDNS_DISABLED);
    root.child(DDNS_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  return true ;
}



/*===========================================================================
  FUNCTION GetTinyProxyStatus
==========================================================================*/
/*!
@brief
  get QcMapBackhaulMgr tiny proxy state (up/down) from xml

@parameters
  status       - tiny proxy status to get
  qmi_err_num  - error number

@return
  qmi_err_num -

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetTinyProxyStatus
(
 qcmap_msgr_tiny_proxy_mode_enum_v01 *status,
 qmi_error_type_v01 *qmi_err_num
 )
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  pugi::xml_document xml_file;
  pugi::xml_node root;

  if (QcMapBackhaulMgr)
  {
    *status = QcMapBackhaulMgr->tiny_proxy_enable_state;
  }
  else
  {
    LOG_MSG_ERROR("Backhaul is not enable.",0,0,0);
    /* read XML file directly. */
    LOAD_XML_FILE(xml_file, QcMapMgr);
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
    atoi(root.child(TINY_PROXY_Enable_TAG).child_value())? (*status =
          QCMAP_MSGR_TINY_PROXY_MODE_UP_V01) :
          (*status = QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01);
  }

  return true;
}


/*===========================================================================
  FUNCTION EnableTinyProxy
==========================================================================*/
/*!
@brief
  Enable tiny proxy

@parameters
  qmi_err_num  - error no.

@return
  true  - enabled
  false - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::EnableTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(QcMapBackhaulMgr)
  {
    /* Save the configuration of TinyProxy even when backhaul is disconnected */
    QcMapBackhaulMgr->tiny_proxy_enable_state= QCMAP_MSGR_TINY_PROXY_MODE_UP_V01;
    QCMAP_Backhaul_WWAN::GetSetTinyProxyFromXML(SET_VALUE,
                                                &QcMapBackhaulMgr->tiny_proxy_enable_state);
    if((QcMapBackhaulWWANMgr && (QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED)) || (QCMAP_Backhaul::IsNonWWANBackhaulActive()))
    {
      /*stoping tinyproxy*/
      QCMAP_Backhaul_WWAN::StopTinyProxy(qmi_err_num);
      /*end of stoping tinyproxy*/

      QCMAP_Backhaul_WWAN::SetupTinyProxy(qmi_err_num);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "tinyproxy -c %s", TINYPROXY_CONFIG_PATH);
      ds_system_call(command, strlen(command));

    }
  }
  else
  {
    LOG_MSG_ERROR("Backhaul or cradle or station mode required.",0,0,0);
    /* Update XML file directly. */
    LOAD_XML_FILE(xml_file, QcMapMgr);
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
    snprintf(data, MAX_STRING_LENGTH, "%d", true);
    root.child(TINY_PROXY_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  }
  return true;
}

/*===========================================================================
  FUNCTION DisableTinyProxy
==========================================================================*/
/*!
@brief
  Disable tiny proxy

@parameters
  qmi_err_num  - error no.

@return
  false - enabled
  true  - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DisableTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();

  if(QcMapBackhaulMgr)
  {
    /* Save the configuration of TinyProxy even when backhaul is disconnected */
    QcMapBackhaulMgr->tiny_proxy_enable_state= QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01;
    QCMAP_Backhaul_WWAN::GetSetTinyProxyFromXML(SET_VALUE,
        &QcMapBackhaulMgr->tiny_proxy_enable_state);
    if((QcMapBackhaulWWANMgr && (QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED)) || (QCMAP_Backhaul::IsNonWWANBackhaulActive()))
    {
      /*stoping tinyproxy*/
      QCMAP_Backhaul_WWAN::StopTinyProxy(qmi_err_num);
      /*end of stoping tinyproxy*/
    }
  }
  else
  {
    LOG_MSG_ERROR("Backhaul or cradle or station mode required.",0,0,0);

    /*Update XML file directly.*/
    LOAD_XML_FILE(xml_file, QcMapMgr);
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
    snprintf(data, MAX_STRING_LENGTH, "%d", false);
    root.child(TINY_PROXY_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  }
  return true;
}


/*===========================================================================
  FUNCTION StopTinyProxy
==========================================================================*/
/*!
@brief
  stops tiny proxy

@parameters
  qmi_err_num  - error no.

@return
  false - fail
  true  - succeed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::StopTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  struct ifreq        req_if_info;
  int val;
  struct sockaddr     *saddr;
  int   sockfd;

  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, BRIDGE_IFACE,sizeof(BRIDGE_IFACE));
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  val = ioctl(sockfd, SIOCGIFADDR, &req_if_info);
  if(val < 0)
  {
    LOG_MSG_ERROR("Get bridge iface address failed",0,0,0);
    return false;
  }
  saddr = (struct sockaddr *)&(req_if_info.ifr_addr);

  /*removing  user nobody from  inet group & removing inet group*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "deluser nobody inet");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "groupdel inet");
  ds_system_call(command, strlen(command));
  /*end of removing  user nobody from  inet group & removing inet group*/

  snprintf(command, MAX_COMMAND_STR_LEN,
            "rm -rf /var/log/tinyproxy");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
             "rm -rf /var/run/tinyproxy");
  ds_system_call(command, strlen(command));

  /*clearing NAT rule*/
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -t nat -D PREROUTING -i bridge0 ! -d %s"
            " -p tcp --dport 80 -j REDIRECT --to-ports 8888",
            inet_ntoa(((struct sockaddr_in *)saddr)->sin_addr));
  ds_system_call(command, strlen(command));


  snprintf(command,MAX_COMMAND_STR_LEN, " killall -9 tinyproxy");
  ds_system_call(command, strlen(command));
  return true;
}

/*===========================================================================
  FUNCTION GetSetTinyProxyFromXML
==========================================================================*/
/*!
@brief
  get/set tiny proxy from xml

@parameters
  action - action get/set
  proxy_config - proxy config

@return
  false - fail
  true  - succeed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetSetTinyProxyFromXML
(
 qcmap_action_type action,
 qcmap_msgr_tiny_proxy_mode_enum_v01 *proxy_config
 )
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  boolean status;

  LOAD_XML_FILE(xml_file, QcMapMgr);
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);

  if (action == SET_VALUE)
  {
    if (*proxy_config == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01)
      status = true;
    else
      status = false;
    snprintf(data, MAX_STRING_LENGTH, "%d", status);
    root.child(TINY_PROXY_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    atoi(root.child(TINY_PROXY_Enable_TAG).child_value())? (*proxy_config =
         QCMAP_MSGR_TINY_PROXY_MODE_UP_V01) :
         (*proxy_config = QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01);
  }
  LOG_MSG_INFO1("Tiny Proxy: Enable: %d ", proxy_config, 0, 0);
  return true;
}


/*===========================================================================
  FUNCTION SetupTinyProxy
==========================================================================*/
/*!
@brief
  set up tiny proxy

@parameters
  qmi_err_num - error no.

@return
  false - fail
  true  - succeed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetupTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  struct ifreq        req_if_info;
  int val;
  struct sockaddr     *saddr;
  int   sockfd;

  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, BRIDGE_IFACE,sizeof(BRIDGE_IFACE));
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  val = ioctl(sockfd, SIOCGIFADDR, &req_if_info);
  if(val < 0)
  {
    LOG_MSG_ERROR("Get bridge iface address failed",0,0,0);
    return false;
  }
  saddr = (struct sockaddr *)&(req_if_info.ifr_addr);

  /*adding user nobody to inet group for socket  permission*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "groupadd -g 3003 inet");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "usermod -aG inet nobody");
  ds_system_call(command, strlen(command));
  /*end of adding user to inet group for socket  permission*/

  /*creating necessry files & setting owenership*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "mkdir /var/log/tinyproxy");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "mkdir /var/run/tinyproxy");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "chown -R nobody:inet /var/log/tinyproxy");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "chown -R nobody:inet /var/run/tinyproxy");
  ds_system_call(command, strlen(command));
  /*end of creating necessry files & setting owenership*/

  /*setting NAT rule*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t nat -A PREROUTING -i bridge0 ! -d %s"
           " -p tcp --dport 80 -j REDIRECT --to-ports 8888",
           inet_ntoa(((struct sockaddr_in *)saddr)->sin_addr));
  ds_system_call(command, strlen(command));

  /*end setting NAT rule*/
  return true;
}

/*===========================================================================
  FUNCTION StartDhcpv6Dns
  ==========================================================================*/
/*!
  @brief
   start dhcpv6 proxy dns support.

  @parameters
  qmi_error_type_v01 *qmi_err_num

  @return
  true  - success
  flase - fail

  @note
  - Dependencies
  - None

  - Side Effects
  - None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::StartDhcpv6Dns(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  char data[MAX_COMMAND_STR_LEN];
  uint32_t gateway_url_len;
  char gateway_url[QCMAP_MSGR_MAX_GATEWAY_URL_V01];
  struct sockaddr_in6 pub_ip6;
  char addr_pub_str[INET6_ADDRSTRLEN] = {0};
  QCMAP_LAN* QCMAPLANMgr = NULL;
  uint8_t  public_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};       /* IPv6 addr assigned to WWAN */
  uint8_t  pri_dns_addr[IPV6_ADDR_SIZE_IN_BYTES] = {0};    /* Primary IPV6 DNS IP address   */
  uint8_t  sec_dns_addr[IPV6_ADDR_SIZE_IN_BYTES] = {0};    /* Secondary IPV6 DNS IP address   */

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (QcMapBackhaul)
  {
    QCMAPLANMgr = GET_LAN_OBJECT(QcMapBackhaul->vlan_id);
  }

  if(QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR( "LAN object is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if ( GetIPv6NetworkConfig(public_ip, pri_dns_addr, sec_dns_addr, qmi_err_num) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("Error in GetIPv6NetworkConfig %d", qmi_err_num,0,0);
    return false;
  }

  //Setup gateway URL mapping with ipv6 address
  if(QCMAPLANMgr->GetGatewayUrl(gateway_url, &gateway_url_len, qmi_err_num))
  {
    LOG_MSG_INFO1("Get gateway url success",0,0,0);

    //remove old ipv6 host entry for gateway URL
    memcpy(pub_ip6.sin6_addr.s6_addr, dhcpv6_dns_conf.pub_ip, QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
    inet_ntop(AF_INET6, &pub_ip6.sin6_addr, addr_pub_str, sizeof addr_pub_str);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "sed -i '/%s %s/d' %s", addr_pub_str , gateway_url, QCMAP_HOST_PATH);
    ds_system_call(command, strlen(command));

    //Add ipv6 host entry for gateway URL
    memcpy(pub_ip6.sin6_addr.s6_addr, public_ip, QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

    inet_ntop(AF_INET6, &pub_ip6.sin6_addr, addr_pub_str, sizeof addr_pub_str);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "echo '%s %s' >> %s", addr_pub_str , gateway_url, QCMAP_HOST_PATH);
    ds_system_call(command, strlen(command));

    //save public IP
    memcpy(dhcpv6_dns_conf.pub_ip, public_ip, QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  }
  else
  {
    LOG_MSG_ERROR("Get gateway url failure!!",0,0,0);
  }

  dhcpv6_dns_conf.dhcpv6_enable_state = QCMAP_MSGR_DHCPV6_MODE_UP_V01;

  /*restart radish to not provide ipv6 dns server*/
  QcMapBackhaul->RestartRadish();

  //update resolve.conf with public ipv6 dns server
  this->Update_resolv_file(ADD_V6DNS_SERVER, pri_dns_addr, sec_dns_addr);

  /* starting dnsmasq to run as dhcpv6 */
  QCMAPLANMgr->StartDHCPD();

  LOG_MSG_INFO1("Successfully started dhcpv6dns\n", 0,0,0);
  ds_system_call("echo QCMAP:DHCPV6 DNS server started > /dev/kmsg",
                 strlen("echo QCMAP:DHCPV6 DNS server started > /dev/kmsg"));
  return true;
}

/*===========================================================================
  FUNCTION StopDhcpv6Dns
  ==========================================================================*/
/*!
  @brief
   stop dhcpv6 proxy dns support.

  @parameters
  qmi_error_type_v01 *qmi_err_num

  @return
  true  - success
  flase - fail

  @note
  - Dependencies
  - None

  - Side Effects
  - None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::StopDhcpv6Dns(qmi_error_type_v01 *qmi_err_num)
{
  uint32_t gateway_url_len;
  char command[MAX_COMMAND_STR_LEN];
  char gateway_url[QCMAP_MSGR_MAX_GATEWAY_URL_V01];
  struct sockaddr_in6 pub_ip6;
  char addr_pub_str[INET6_ADDRSTRLEN] = {0};
  uint8_t  empty_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};       /* empty IPV6 IP*/
  QCMAP_LAN* QCMAPLANMgr = NULL;

  if (QcMapBackhaul)
  {
    QCMAPLANMgr = GET_LAN_OBJECT(QcMapBackhaul->vlan_id);
  }

  if (QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("LAN Object is NULL!!!", 0,0,0);
    return false;
  }

  this->dhcpv6_dns_conf.dhcpv6_enable_state = QCMAP_MSGR_DHCPV6_MODE_DOWN_V01;

  //Remove gateway URL mapping with ipv6 address
  if(QCMAPLANMgr && QCMAPLANMgr->GetGatewayUrl(gateway_url, &gateway_url_len, qmi_err_num))
  {
    LOG_MSG_INFO1("Get gateway url success \n",0,0,0);
    memcpy(pub_ip6.sin6_addr.s6_addr, this->dhcpv6_dns_conf.pub_ip,
           QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

    //remove ipv6 host entry for gateway URL
    inet_ntop(AF_INET6, &pub_ip6.sin6_addr, addr_pub_str, sizeof addr_pub_str);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "sed -i '/%s %s/d' %s", addr_pub_str , gateway_url, QCMAP_HOST_PATH);
    ds_system_call(command, strlen(command));

    //clear public IP
    memcpy(this->dhcpv6_dns_conf.pub_ip, empty_ip,
           QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  }
  else
  {
    LOG_MSG_ERROR("Get gateway url failure!!",0,0,0);
  }

  //update resolve.conf to remove ipv6 dns server
  this->Update_resolv_file(REMOVE_V6DNS_SERVER ,
                           this->dhcpv6_dns_conf.pri_dns_addr ,
                           this->dhcpv6_dns_conf.sec_dns_addr);

  // run dnsmasq in normal mode
  QCMAPLANMgr->StopDHCPD();
  QCMAPLANMgr->StartDHCPD();

  LOG_MSG_INFO1("Successfully stopped dhcpv6dns\n", 0,0,0);
  ds_system_call("echo QCMAP:DHCPV6 DNS server stopped > /dev/kmsg",
                 strlen("echo QCMAP:DHCPV6 DNS server stopped > /dev/kmsg"));
  return true;
}

/*===========================================================================
  FUNCTION GetIfaceLLAddrStr
  ==========================================================================*/
/*!
  @brief
   get the interface LL ipv6 address.

  @parameters
  char *lladdr
  char *iface

  @return
  true  - success
  flase - fail

  @note
  - Dependencies
  - None

  - Side Effects
  - None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetIfaceLLAddrStr(char *lladdr, char *iface)
{
  struct ifaddrs *ifaddr, *ifa;
  struct sockaddr_in6 *current_addr;

  /* Get Link Local Addr */
  if (getifaddrs(&ifaddr) == -1)
  {
    LOG_MSG_INFO1("getifaddrs failed", 0,0,0);
    return false;
  }

  /* Walk through linked list, maintaining head pointer so we
     can free list later */
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == NULL)
      continue;

    if (ifa->ifa_addr->sa_family != AF_INET6 ||
        strncmp(ifa->ifa_name, iface, strlen(iface)) != 0)
      continue;

    /* For an AF_INET* interface address, display the address */
    current_addr = (struct sockaddr_in6 *) ifa->ifa_addr;
    if (!IN6_IS_ADDR_LINKLOCAL(&(current_addr->sin6_addr)))
      continue;

    inet_ntop(AF_INET6, &current_addr->sin6_addr, lladdr, INET6_ADDRSTRLEN);
    LOG_MSG_INFO1("---link local addr %s----", lladdr,0,0);

    freeifaddrs(ifaddr);
    return true;
  }
  freeifaddrs(ifaddr);

  return false;

}

/*===========================================================================
  FUNCTION Update_resolv_file
  ==========================================================================*/
/*!
  @brief
   Update the resolv.conf file.

  @parameters
  mode mod
  uint8_t pri_dns_addr[]
  uint8_t  sec_dns_addr[]

  @return
  true  - success
  flase - fail

  @note
  - Dependencies
  - None

  - Side Effects
  - None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::Update_resolv_file
(
  mode mod, uint8_t
  pri_dns_addr[] ,uint8_t  sec_dns_addr[]
 )
{
  struct sockaddr_in6 pd6,sd6;
  char addrp[INET6_ADDRSTRLEN] = {0};
  char addrs[INET6_ADDRSTRLEN] = {0};
  char command[MAX_COMMAND_STR_LEN] ;
  uint8_t  empty_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};       /* empty IPV6 IP*/
  QCMAP_LAN* lan_obj = NULL;

  if(NULL == QcMapBackhaul)
  {
    LOG_MSG_INFO1("QcMapBackhaul is NULL", 0, 0, 0);
    return false;
  }

  if((lan_obj = GET_LAN_OBJECT(QcMapBackhaul->vlan_id)) == NULL)
  {
    LOG_MSG_INFO1("LAN object is NULL", 0, 0, 0);
    return false;
  }

  memcpy(pd6.sin6_addr.s6_addr, pri_dns_addr,
      QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  memcpy(sd6.sin6_addr.s6_addr, sec_dns_addr,
      QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

  inet_ntop(AF_INET6, &pd6.sin6_addr, addrp, sizeof addrp);
  inet_ntop(AF_INET6, &sd6.sin6_addr, addrs, sizeof addrs);
  LOG_MSG_INFO1(" dns addrs===%s,%s mode=%d=====",addrp,addrs,mod);

  switch(mod)
  {
    case(ADD_V6DNS_SERVER):
    {
      if (strncmp (addrp,"::",sizeof("::")))
      {
        memcpy(this->dhcpv6_dns_conf.pri_dns_addr, pri_dns_addr,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      }

      if (strncmp (addrs,"::",sizeof("::")))
      {
        memcpy(this->dhcpv6_dns_conf.sec_dns_addr, sec_dns_addr,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      }

      lan_obj->AddDNSNameServers(addrp, addrs);

      break;
    }

    case(REMOVE_V6DNS_SERVER):
    {
      if (strncmp (addrp,"::",sizeof("::")))
      {
        //clear DNS
        memcpy(this->dhcpv6_dns_conf.pri_dns_addr, empty_ip,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      }

      if (strncmp (addrs,"::",sizeof("::")))
      {
        //clear DNS
        memcpy(this->dhcpv6_dns_conf.sec_dns_addr, empty_ip,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      }

      lan_obj->DeleteDNSNameServers(addrp, addrs);

      break;
    }

    default:
    {
      LOG_MSG_INFO1("invalid mod: %d", mod, 0, 0);
      break;
    }
  }

  return true;
}

/*===========================================================================

FUNCTION GetDeviceName()

DESCRIPTION
  This Function get RMNET device name.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::GetDeviceName
(
 int     qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
 qcmap_msgr_ip_family_enum_v01 qcmap_dev_type, /* Dev type                 */
 char   *qcmap_dev_name,                       /* Device name              */
 int    *qcmap_cm_errno                        /* Error condition value    */
 )
{
  char device[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];
  int rval;

  ds_assert(qcmap_dev_name != NULL);
  ds_assert(qcmap_cm_errno != NULL);

  LOG_MSG_INFO1("GetDeviceName(): enter",0,0,0);

  memset(device,0,sizeof(device));
  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    LOG_MSG_ERROR("Wrong QCMAP CM Handle, qcmap_cm_handle:%d, qcmap_cm_cb.handle:%d",
                  qcmap_cm_handle, qcmap_cm_get_handle(),0);
    return QCMAP_CM_ERROR;
  }
  if (qcmap_dev_type == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    if (state <= QCMAP_CM_ENABLE)
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;

      LOG_MSG_ERROR("PDN(%p) V4 WAN not connected. %d", QcMapBackhaul->profileHandle, state,0);
      return QCMAP_CM_ERROR;
    }
    else if (dsi_net_hndl.handle != NULL)
    {
      rval = dsi_get_device_name(dsi_net_hndl.handle, device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
      if(rval != DSI_SUCCESS)
      {
        LOG_MSG_ERROR ("PDN(%p) Couldn't get ipv4 rmnet name. rval %d . Using saved device",
                       QcMapBackhaul->profileHandle, rval, 0);
        memcpy(device, dsi_device_name, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
        *qcmap_cm_errno = rval;
      }

      LOG_MSG_INFO1("PDN(%p) device_name:%s",  QcMapBackhaul->profileHandle, device,0);
      memcpy(qcmap_dev_name, device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
    }
    else
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      LOG_MSG_ERROR("PDN(%p) Error during get ipv4 dev_name state =%d, handle=%d ",
                    QcMapBackhaul->profileHandle, state, dsi_net_hndl.handle);
      return QCMAP_CM_ERROR;
    }
  }
  else if (qcmap_dev_type == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    if (ipv6_state <= QCMAP_CM_V6_ENABLE)
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      LOG_MSG_ERROR( "PDN(%p) V6 WAN not connected. %d", QcMapBackhaul->profileHandle, ipv6_state,0);
      return QCMAP_CM_ERROR;
    }
    else if (ipv6_dsi_net_hndl.handle != NULL)
    {
      rval = dsi_get_device_name(ipv6_dsi_net_hndl.handle, device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
      if(rval != DSI_SUCCESS)
      {
        LOG_MSG_ERROR("PDN(%p), Couldn't get ipv6 rmnet name. rval %d . Using saved device",
                      QcMapBackhaul->profileHandle, rval, 0);
        memcpy(device, ipv6_dsi_device_name, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
        *qcmap_cm_errno = rval;
      }

      LOG_MSG_INFO1("PDN(%p), device_name:%s", QcMapBackhaul->profileHandle, device, 0);
      memcpy(qcmap_dev_name, device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
    }
    else
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      LOG_MSG_ERROR("PDN(%p) Error during get ipv6 dev_name state =%d, handle=%d ",
                    QcMapBackhaul->profileHandle, ipv6_state, ipv6_dsi_net_hndl.handle);
      return QCMAP_CM_ERROR;
    }
  }

  LOG_MSG_INFO1( "PDN(%p) GetDeviceName: %s", QcMapBackhaul->profileHandle, qcmap_dev_name,0);
  return QCMAP_CM_SUCCESS;
}


/*===========================================================================

FUNCTION GetIPv4NetworkConfig()

DESCRIPTION
  This Function get the ipv4 WWAN network configuration.

DEPENDENCIES
  None.

PARAMETERS
  [out] public_ip       - Public IP for WWAN
  [out] pri_dns_addr    - Primary DNS IP address
  [out] sec_dns_addr    - Seconday DNS IP address
  [out] default_gw_addr - Default GW address.
  [out] qmi_err_num     - QMI error

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::GetIPv4NetworkConfig
(
  in_addr_t          *public_ip,
  uint32             *pri_dns_addr,
  in_addr_t          *sec_dns_addr,
  in_addr_t          *default_gw_addr,
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret = QCMAP_CM_SUCCESS;
  dsi_addr_info_t addr_info;
  int rval, num_entries;
  char ip_str[20];

  ds_assert(public_ip != NULL);
  ds_assert(pri_dns_addr != NULL);
  ds_assert(sec_dns_addr != NULL);
  ds_assert(qmi_err_num != NULL);

  memset(ip_str, 0, 20);

  if (state != QCMAP_CM_WAN_CONNECTED)
  {
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "QCMAP CM not in wan connected state", 0, 0, 0);
    return ret;
  }

  num_entries=1;
  memset(&addr_info, 0, sizeof(dsi_addr_info_t));
  rval = dsi_get_ip_addr(dsi_net_hndl.handle, &addr_info, num_entries);

  LOG_MSG_INFO1("dsi_get_ip_addr(), rval:%d", rval, 0, 0);

  if(rval != DSI_SUCCESS)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("Couldn't get ipv4 ip address. rval %d",rval,0,0);
    ret = QCMAP_CM_ERROR;
  }
  else
    LOG_MSG_INFO1("Got IP Address, rval:%d", rval, 0, 0);

  if (addr_info.iface_addr_s.valid_addr)
  {
    if (SASTORAGE_FAMILY(addr_info.iface_addr_s.addr) == AF_INET)
    {
      memset(ip_str, 0, 20);
      snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
                SASTORAGE_DATA(addr_info.iface_addr_s.addr)[0],
                SASTORAGE_DATA(addr_info.iface_addr_s.addr)[1],
                SASTORAGE_DATA(addr_info.iface_addr_s.addr)[2],
                SASTORAGE_DATA(addr_info.iface_addr_s.addr)[3]);
      *public_ip = inet_addr(ip_str);
      LOG_MSG_INFO1("public_ip:0x%x", *public_ip, 0, 0);
    }
  }
  if (addr_info.dnsp_addr_s.valid_addr)
  {
    memset(ip_str, 0, 20);
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
              SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[0],
              SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[1],
              SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[2],
              SASTORAGE_DATA(addr_info.dnsp_addr_s.addr)[3]);
    *pri_dns_addr = inet_addr(ip_str);
    LOG_MSG_INFO1("pri_dns_addr: 0x%x", *pri_dns_addr, 0, 0);
  }
  if (addr_info.dnss_addr_s.valid_addr)
  {
    memset(ip_str, 0, 20);
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
              SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[0],
              SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[1],
              SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[2],
              SASTORAGE_DATA(addr_info.dnss_addr_s.addr)[3]);
    *sec_dns_addr = inet_addr(ip_str);
    LOG_MSG_INFO1("sec_dns_addr: 0x%x", *sec_dns_addr, 0, 0);
  }
  if (addr_info.gtwy_addr_s.valid_addr)
  {
    memset(ip_str, 0, 20);
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
              SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[0],
              SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[1],
              SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[2],
              SASTORAGE_DATA(addr_info.gtwy_addr_s.addr)[3]);
   *default_gw_addr = inet_addr(ip_str);
    LOG_MSG_INFO1("default_gw_addr: 0x%x", *default_gw_addr, 0, 0);
  }

  return ret;
}

/*===========================================================================

FUNCTION GetIPv6NetworkConfig()

DESCRIPTION
  This function gets the IPv6 WWAN network configuration.

DEPENDENCIES
  None.

PARAMETERS
  [out] public_ip       - Public IP for WWAN
  [out] pri_dns_addr    - Primary DNS IP address
  [out] sec_dns_addr    - Seconday DNS IP address
  [out] qmi_err_num     - QMI error

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::GetIPv6NetworkConfig
(
  uint8_t             public_ip[],
  uint8_t             pri_dns_addr[],
  uint8_t             sec_dns_addr[],
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret = QCMAP_CM_SUCCESS;
  dsi_addr_info_t addr_info;
  int rval, num_entries;
  char ip_str[INET6_ADDRSTRLEN];

  ds_assert(public_ip != NULL);
  ds_assert(pri_dns_addr != NULL);
  ds_assert(sec_dns_addr != NULL);
  ds_assert(qmi_err_num != NULL);

  memset(ip_str, 0, INET6_ADDRSTRLEN);

  if (ipv6_state != QCMAP_CM_V6_WAN_CONNECTED)
  {
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "QCMAP CM not in IPv6 WAN connected state", 0, 0, 0);
    return ret;
  }

  num_entries=1;
  memset(&addr_info, 0, sizeof(dsi_addr_info_t));
  rval = dsi_get_ip_addr(ipv6_dsi_net_hndl.handle, &addr_info, num_entries);

  LOG_MSG_INFO1("dsi_get_ip_addr(), rval:%d",rval, 0, 0);

  if(rval != DSI_SUCCESS)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    QCMAP_CM_LOG("Couldn't get ipv6 ip address. rval %d", rval);
    ret = QCMAP_CM_ERROR;
  }
  else
    LOG_MSG_INFO1("Got IPv6 Address, rval:%d", rval, 0, 0);

  if (addr_info.iface_addr_s.valid_addr)
  {
    if (SASTORAGE_FAMILY(addr_info.iface_addr_s.addr) == AF_INET6)
    {
      memcpy(public_ip, SASTORAGE_DATA(addr_info.iface_addr_s.addr),
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
    }
  }
  if (addr_info.dnsp_addr_s.valid_addr)
  {
    if (SASTORAGE_FAMILY(addr_info.dnsp_addr_s.addr) == AF_INET6)
    {
      memcpy(pri_dns_addr, SASTORAGE_DATA(addr_info.dnsp_addr_s.addr),
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
    }
  }
  if (addr_info.dnss_addr_s.valid_addr)
  {
    if (SASTORAGE_FAMILY(addr_info.dnss_addr_s.addr) == AF_INET6)
    {
      memcpy(sec_dns_addr, SASTORAGE_DATA(addr_info.dnss_addr_s.addr),
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
    }
  }

  return ret;
}


/*===========================================================================
  FUNCTION StartAutoConnect
===========================================================================*/
/*!
@brief
  This Function is used to send connect to backhaul request incase the
  previous connect to backhal request failed.

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
  */
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::StartAutoConnect(qcmap_msgr_ip_family_enum_v01 family)
{
  struct itimerspec its;
  int ret_val;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec =0;

  //2 TODO: Duplicate logic
  if( family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    LOG_MSG_INFO1("state =%d ", state,0,0);
    auto_connect_timer_running_v4 = true;
    LOG_MSG_INFO1("V4 New Connect to backhaul request with start after %d sec ", auto_timer_value_v4, 0, 0);
    its.it_value.tv_sec = auto_timer_value_v4;
    ret_val = timer_settime(timerid_v4, 0, &its, NULL);
    if(ret_val != 0)
    {
      LOG_MSG_INFO1("Set autoconnect timer failed %d for IPv4", errno, 0, 0);
    }
    if(auto_timer_value_v4 < MAX_WAN_CON_TIMEOUT)
    {
      auto_timer_value_v4 = auto_timer_value_v4 * 2;
    }
  }
  else
  {
    LOG_MSG_INFO1("ipv6_state =%d ", ipv6_state, 0, 0);
    auto_connect_timer_running_v6 = true;
    its.it_value.tv_sec = auto_timer_value_v6;
    LOG_MSG_INFO1("V6 New Connect to backhaul request with start after %d sec ",auto_timer_value_v6,0,0);
    ret_val = timer_settime(timerid_v6, 0, &its, NULL);
    if(ret_val != 0)
    {
      LOG_MSG_INFO1("Set autoconnect timer failed %d for IPv6", errno, 0, 0);
    }
    if(auto_timer_value_v6 < MAX_WAN_CON_TIMEOUT)
    {
      auto_timer_value_v6 = auto_timer_value_v6 * 2;
    }
  }
}

/*===========================================================================
  FUNCTION stop_auto_timer
===========================================================================*/
/*!
@brief
  This Fuction will stop the running timer and reset the timer vaules.

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::StopAutoTimer(qcmap_msgr_ip_family_enum_v01 family) //stop_auto_timer
{
  struct itimerspec its;
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;

  //2 TODO Duplicate logic
  LOG_MSG_INFO1("stopping auto timer \n",0,0,0);
  if(family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    LOG_MSG_INFO1("stopping auto timer for v4 time 0x%lx \n", timerid_v4,0,0);
    if(auto_connect_timer_running_v4 == true)
    {
      auto_connect_timer_running_v4 = false;
      timer_settime(timerid_v4, 0, &its, NULL);
      auto_timer_value_v4 = AUTO_CONNECT_TIMER;
    }
    else
    {
      LOG_MSG_INFO1("auto timer not running v4 time 0x%lx \n", timerid_v4,0,0);
    }
  }
  else
  {
    LOG_MSG_INFO1("stopping auto timer for v6 time 0x%lx \n", timerid_v6,0,0);
    if(auto_connect_timer_running_v6 == true)
    {
      auto_connect_timer_running_v6 = false;
      timer_settime(timerid_v6, 0, &its, NULL);
      auto_timer_value_v6 =  AUTO_CONNECT_TIMER;
    }
    else
    {
      LOG_MSG_INFO1("auto timer not running v6 time 0x%lx \n", timerid_v6, 0,0);
    }
  }
}

/*===========================================================================
  FUNCTION DSINetCB
===========================================================================*/
/*!
@brief
  This function is a call back function to dsi_net

@input
  hndl          - dsi handle
  user_data     -
  evt           - dsi_net_event typedef
  payload_ptr   -

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::DSINetCB
(
 dsi_hndl_t hndl,
 void * user_data,
 dsi_net_evt_t evt,
 dsi_evt_payload_t *payload_ptr
 )
{
  qcmap_dsi_buffer_t qcmap_dsi_buffer;
  //  unsigned int dsi_qcmap_sockfd;
  int numBytes=0, len;
  struct sockaddr_un dsi_qcmap;

  dsi_qcmap.sun_family = AF_UNIX;
  strlcpy(dsi_qcmap.sun_path, QCMAP_DSI_UDS_FILE, sizeof(QCMAP_DSI_UDS_FILE));
  len = strlen(dsi_qcmap.sun_path) + sizeof(dsi_qcmap.sun_family);

  qcmap_dsi_buffer.dsi_nethandle = hndl;
  /* Not used currently, but if used, make sure you allocate buffer and not use the buffer provided by dsi */
  qcmap_dsi_buffer.user_data = user_data;
  qcmap_dsi_buffer.evt = evt;
  /* Not used currently, but if used, make sure you allocate buffer and not use the buffer provided by dsi */
  qcmap_dsi_buffer.payload_ptr = payload_ptr;

  if ((numBytes = sendto(dsi_qcmap_sockfd, (void *)&qcmap_dsi_buffer, sizeof(qcmap_dsi_buffer_t), 0,
          (struct sockaddr *)&dsi_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from dsi callback context", 0, 0, 0);
    return;
  }
  LOG_MSG_INFO1("Send succeeded in dsi callback context", 0, 0, 0);
  return;
}

/*===========================================================================
  FUNCTION  ProcessDSI_NetEvent
===========================================================================*/
/*!
@brief
  QCMAP CM DSS net callback

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WWAN::ProcessDSI_NetEvent
(
 dsi_hndl_t hndl,
 void * user_data,
 dsi_net_evt_t evt,
 dsi_evt_payload_t *payload_ptr
 )
{
  qcmap_cm_event_e  event;
  boolean           callback = false;
  int               i;
  int               err;
  int              *qcmap_cm_errno = &err;
  qmi_client_error_type                qmi_error;
  nas_indication_register_req_msg_v01  qcmap_nas_indication_register_req_msg_v01;
  nas_indication_register_resp_msg_v01 qcmap_nas_indication_register_resp_msg_v01;
  dsi_ce_reason_t   dsicallend;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_WWAN *pMe;
  qcmap_msgr_wwan_info_v01 wwan_info;

  if (user_data == NULL)
  {
    LOG_MSG_ERROR("ProcessDSI_NetEvent user_data=NULL", 0,0,0);
    return;
  }

  pMe = (QCMAP_Backhaul_WWAN *) user_data;

  memset((void *)&dsicallend,0, sizeof(dsicallend));
  memset((void *)&wwan_info,0, sizeof(wwan_info));

  LOG_MSG_INFO1("ProcessDSI_NetEvent: pMe=%p, Profile=%d", pMe, pMe->QcMapBackhaul->profileHandle,0);
  LOG_MSG_INFO1("ProcessDSI_NetEvent: dsi_nethandle %p evt %d state (V4) %d ",\
      hndl, evt, pMe->state);

  if(hndl == pMe->ipv6_dsi_net_hndl.handle)
  {
    if (pMe->ipv6_state == QCMAP_CM_V6_ENABLE)
    {
      if (evt == DSI_EVT_NET_IS_CONN)
      {
        LOG_MSG_INFO1("DSI_EVT_NET_IS_CONN is not supported in state V6_ENABLE",0, 0, 0);
      }
      else if(evt == DSI_EVT_NET_NO_NET)
      {
        LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in state V6_ENABLE",0,0,0);
        /* 9x25 changes */
        pMe->ipv6_state = QCMAP_CM_V6_ENABLE;
        event = QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL;
        callback = TRUE;
      }
    }
    else if(pMe->ipv6_state == QCMAP_CM_V6_WAN_CONNECTING)
    {
      if(evt == DSI_EVT_NET_IS_CONN)
      {
        /* 9x25 changes */
        char device[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];
        int rval;
        int8_t  public_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};     /* IPv6 addr assigned to WWAN */
        uint8_t  pri_dns_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};    /* Primary IPV6 DNS IP address   */
        uint8_t  sec_dns_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};    /* Secondary IPV6 DNS IP address   */
        char buffer[INET6_ADDRSTRLEN] = {0};

        memset(device, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
        pMe->ipv6_state = QCMAP_CM_V6_WAN_CONNECTED;
        event = QCMAP_CM_EVENT_WAN_IPv6_CONNECTED;
        callback = TRUE;
        LOG_MSG_INFO1("Received DS_ENETISCONN in state V6_WAN_CONNECTING",0,0,0);
        LOG_MSG_INFO1("Transition from state V6_WAN_CONNECTING to V6_WAN_CONNECTED",0,0,0);
        if(QcMapBackhaulMgr && pMe->auto_connect)
          pMe->StopAutoTimer( QCMAP_MSGR_IP_FAMILY_V6_V01 );
        rval = dsi_get_device_name(pMe->ipv6_dsi_net_hndl.handle, device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);

        if(rval != DSI_SUCCESS)
        {
          LOG_MSG_ERROR ("Couldn't get ipv6 rmnet name. rval %d",
              rval, 0, 0);
          strlcpy((char *)device, QCMAP_V6_DEFAULT_DEVICE_NAME, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
        }

        LOG_MSG_INFO1("device_name:%s",device, 0, 0);
        memcpy(&(pMe->ipv6_dsi_device_name), device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

        memset(wwan_info.iface_name, '\0', sizeof(wwan_info.iface_name));
        strlcpy(wwan_info.iface_name, device, sizeof(wwan_info.iface_name));

        if (pMe->GetIPv6NetworkConfig(public_ip, pri_dns_ip,
                                       sec_dns_ip, &qmi_err_num) == QCMAP_CM_SUCCESS)
        {
          memset(buffer, 0, INET6_ADDRSTRLEN);
          memcpy(wwan_info.v6_addr, public_ip, sizeof(wwan_info.v6_addr));
          LOG_MSG_INFO1(" IPv6 Public ip: %s", inet_ntop(AF_INET6, &public_ip, buffer,
                          sizeof (buffer)), 0,0);
          memcpy(wwan_info.v6_prim_dns_addr, pri_dns_ip, sizeof(wwan_info.v6_prim_dns_addr));
          memcpy(wwan_info.v6_sec_dns_addr, sec_dns_ip, sizeof(wwan_info.v6_sec_dns_addr));
        }


        LOG_MSG_INFO1("QCMAP AP Handle 0x%04x IPv6 WAN Connected", qcmap_cm_get_handle(), 0, 0);
        /*deregister from serving system NAS indication to save power*/
        qcmap_cm_register_nas_sys_info(FALSE);
      }
      else if(evt == DSI_EVT_NET_NO_NET)
      {
        /* 9x25 changes */
        pMe->ipv6_state = QCMAP_CM_V6_ENABLE;
        event = QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL;
        callback = TRUE;
        LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in V6_WAN_CONNECTING",0,0,0);
        LOG_MSG_INFO1("Transition from V6_WAN_CONNECTING to V6_ENABLE",0,0,0);
        if(dsi_get_call_end_reason(pMe->ipv6_dsi_net_hndl.handle,&dsicallend,DSI_IP_FAMILY_V6) == DSI_SUCCESS)
        {
          LOG_MSG_ERROR("Call Disconnected reason type=%d reason code =%d ",dsicallend.reason_type,dsicallend.reason_code,0);
        }
        if(qcmap_cm_is_disable_in_process())
        {
          qcmap_cm_disable(qcmap_cm_get_handle(), qcmap_cm_errno, &qmi_err_num);
        }
        else if (pMe->auto_connect)
        {
          /*register for serving system NAS indication*/
          qcmap_cm_register_nas_sys_info(TRUE);

          LOG_MSG_INFO1("Start autoconnect",0,0,0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          pMe->backhaul_service = false;
          pMe->StartAutoConnect( QCMAP_MSGR_IP_FAMILY_V6_V01 );
        }
      }

      if (callback)
        qcmap_cm_send_event(event, (void *)pMe, &dsicallend, &wwan_info);

      return;
    }
    else if(pMe->ipv6_state == QCMAP_CM_V6_WAN_CONNECTED)
    {
      if(evt == DSI_EVT_NET_IS_CONN)
      {
        LOG_MSG_INFO1("Received event DSI_EVT_NET_IS_CONN in V6_WAN_CONNECTED",0,0,0);
        LOG_MSG_INFO1("Ignore event DSI_EVT_NET_IS_CONN in state V6_WAN_CONNECTED",0,0,0);
      }
      else if(evt == DSI_EVT_NET_NO_NET)
      {
        /* 9x25 changes */
        pMe->ipv6_state = QCMAP_CM_V6_ENABLE;
        event = QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED;
        callback = TRUE;
        LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in V6_WAN_CONNECTED",0,0,0);
        LOG_MSG_INFO1("Transition from V6_WAN_CONNECTED to V6_ENABLE",0,0,0);
        if(dsi_get_call_end_reason(pMe->ipv6_dsi_net_hndl.handle,&dsicallend,DSI_IP_FAMILY_V6) == DSI_SUCCESS)
        {
          LOG_MSG_ERROR("Call Disconnected reason type=%d reason code =%d ",
                          dsicallend.reason_type,dsicallend.reason_code,0);
        }

        qcmap_cm_send_event(event, (void *)pMe, &dsicallend, NULL);

        if(qcmap_cm_is_disable_in_process())
        {
          qcmap_cm_disable(qcmap_cm_get_handle(), qcmap_cm_errno, &qmi_err_num);
        }
        else if (pMe->auto_connect)
        {
          /*register for serving system NAS indication*/
          qcmap_cm_register_nas_sys_info(TRUE);

          LOG_MSG_INFO1("Start autoconnect",0,0,0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          pMe->backhaul_service = false;
          pMe->StartAutoConnect( QCMAP_MSGR_IP_FAMILY_V6_V01 );
        }
        return;
      }
      else if ( evt == DSI_EVT_NET_RECONFIGURED)
      {
        event = QCMAP_CM_EVENT_WAN_IPv6_ADDR_RECONF;
        callback = TRUE;
        LOG_MSG_INFO1("Received event DSI_EVT_NET_RECONFIGURED in V6_WAN_CONNECTED", 0, 0, 0);
      }
    }
    else if(pMe->ipv6_state == QCMAP_CM_V6_WAN_DISCONNECTING)
    {
      if(evt == DSI_EVT_NET_IS_CONN)
      {
        LOG_MSG_INFO1("Received event DSI_EVT_NET_IS_CONN in V6_WAN_DISCONNECTING",0,0,0);
        LOG_MSG_INFO1("Ignore event DSI_EVT_NET_IS_CONN in V6_WAN_DISCONNECTING",0,0,0);
      }
      else if(evt == DSI_EVT_NET_NO_NET)
      {
        /* 9x25 changes */
        pMe->ipv6_state = QCMAP_CM_V6_ENABLE;
        event = QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED;

        /* Resetting timer value after disconnect*/
        if(QcMapBackhaulMgr && pMe->auto_connect)
          pMe->StopAutoTimer( QCMAP_MSGR_IP_FAMILY_V6_V01 );

        LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in V6_WAN_DISCONNECTING",0,0,0);
        LOG_MSG_INFO1("Transition from V6_WAN_DISCONNECTING to V6_ENABLE",0,0,0);
        callback = TRUE;
        if(dsi_get_call_end_reason(pMe->ipv6_dsi_net_hndl.handle,&dsicallend,DSI_IP_FAMILY_V6) == DSI_SUCCESS)
        {
          LOG_MSG_INFO1("Call Disconnected reason type=%d reason code =%d ",
                         dsicallend.reason_type,dsicallend.reason_code,0);
        }
        qcmap_cm_send_event(event, (void *)pMe, &dsicallend, NULL);
        if(qcmap_cm_is_disable_in_process())
        {
          qcmap_cm_disable(qcmap_cm_get_handle(), qcmap_cm_errno, &qmi_err_num);
        }
        return;
      }
    }
  }

  if (hndl == pMe->dsi_net_hndl.handle)
  {
    if (pMe->state == QCMAP_CM_ENABLE)
    {
      if (evt == DSI_EVT_NET_IS_CONN)
      {
        LOG_MSG_INFO1("DSI_EVT_NET_IS_CONN is not supported in state QCMAP_CM_ENABLE",
            0, 0, 0);
      }
      else if (evt == DSI_EVT_NET_NO_NET)
      {
        LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in state ENABLE",0,0,0);
        /* 9x25 */
        pMe->state = QCMAP_CM_ENABLE;
        event = QCMAP_CM_EVENT_WAN_CONNECTING_FAIL;
        LOG_MSG_INFO1("QCMAP AP Handle 0x%04x IPv4 Backhaul Connecting Fail", qcmap_cm_get_handle(), 0, 0);
        callback = TRUE;
        LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in WAN_CONNECTING",0,0,0);
      }
    }
    else if (pMe->state == QCMAP_CM_WAN_CONNECTING)
    {
      if (evt == DSI_EVT_NET_IS_CONN)
      {
        char device[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
        int rval;
        in_addr_t public_ip = 0, sec_dns_ip = 0;
        in_addr_t default_gw_addr = 0;
        uint32 pri_dns_ip = 0;
        in_addr public_ip_addr, pri_dns_addr, sec_dns_addr;

        pMe->state = QCMAP_CM_WAN_CONNECTED;
        event = QCMAP_CM_EVENT_WAN_CONNECTED;
        callback = TRUE;

        LOG_MSG_INFO1("Received DS_ENETISCONN in state WAN_CONNECTING",0,0,0);
        LOG_MSG_INFO1("Transition from state WAN_CONNECTING to WAN_CONNECTED",0,0,0);
        if(QcMapBackhaulMgr && pMe->auto_connect)
          pMe->StopAutoTimer( QCMAP_MSGR_IP_FAMILY_V4_V01 );
        memset(device, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

        rval = dsi_get_device_name(pMe->dsi_net_hndl.handle, device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);

        if(rval != DSI_SUCCESS)
        {
          LOG_MSG_ERROR ("Couldn't get ipv4 rmnet name. rval %d",
              rval, 0, 0);
          strlcpy((char *)device, QCMAP_V4_DEFAULT_DEVICE_NAME, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
        }

        LOG_MSG_INFO1("device_name:%s", device,0,0);
        memcpy(&(pMe->dsi_device_name), device, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

        memset(wwan_info.iface_name, '\0', sizeof(wwan_info.iface_name));
        strlcpy(wwan_info.iface_name, device, sizeof(wwan_info.iface_name));


        if (pMe->GetIPv4NetworkConfig(&public_ip, &pri_dns_ip,
                                       &sec_dns_ip, &default_gw_addr,
                                       &qmi_err_num) == QCMAP_CM_SUCCESS)
        {
          if (public_ip != 0)
          {
            wwan_info.v4_addr = public_ip;
            public_ip_addr.s_addr = public_ip;
            LOG_MSG_INFO1("public_ip:%s", inet_ntoa(public_ip_addr),0,0);
          }

          if (pri_dns_ip != 0)
          {
            wwan_info.v4_prim_dns_addr = pri_dns_ip;
            pri_dns_addr.s_addr = pri_dns_ip;
            LOG_MSG_INFO1("primary_dns:%s", inet_ntoa(pri_dns_addr),0,0);
          }

          if (sec_dns_ip != 0)
          {
            wwan_info.v4_sec_dns_addr = sec_dns_ip;
            sec_dns_addr.s_addr = sec_dns_ip;
            LOG_MSG_INFO1("secondary_dns:%s", inet_ntoa(sec_dns_addr),0,0);
          }
        }
        else
        {
          LOG_MSG_INFO1(" Unable to GetIPv4NetworkConfig " , 0,0,0);
        }

        LOG_MSG_INFO1("QCMAP AP Handle 0x%04x IPv4 WAN Connected", qcmap_cm_get_handle(), 0, 0);
        /*deregister from serving system NAS indication to save power*/
        qcmap_cm_register_nas_sys_info(FALSE);
      }
      else if (evt == DSI_EVT_NET_NO_NET)
      {
        /* 9x25 */
        pMe->state = QCMAP_CM_ENABLE;
        event = QCMAP_CM_EVENT_WAN_CONNECTING_FAIL;
        LOG_MSG_INFO1("QCMAP AP Handle 0x%04x IPv4 Backhaul Connecting Fail", qcmap_cm_get_handle(), 0, 0);
        callback = TRUE;

        if(dsi_get_call_end_reason(pMe->dsi_net_hndl.handle,&dsicallend,DSI_IP_FAMILY_V4) == DSI_SUCCESS)
        {
          LOG_MSG_INFO1("Call Disconnected reason type=%d reason code =%d ",dsicallend.reason_type,dsicallend.reason_code,0);
        }
        if (callback)
          qcmap_cm_send_event(event, (void *)pMe, &dsicallend, NULL);

        if(qcmap_cm_is_disable_in_process())
        {
          qcmap_cm_disable(qcmap_cm_get_handle(), qcmap_cm_errno, &qmi_err_num);
        }
        else if (pMe->auto_connect)
        {
          /*register for serving system NAS indication*/
          qcmap_cm_register_nas_sys_info(TRUE);

          LOG_MSG_INFO1("Start autoconnect",0,0,0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          pMe->backhaul_service = false;
          pMe->StartAutoConnect( QCMAP_MSGR_IP_FAMILY_V4_V01 );
          LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in WAN_CONNECTING",0,0,0);
        }
        return;
      }
    }
    else if (pMe->state == QCMAP_CM_WAN_CONNECTED)
    {
      if(evt == DSI_EVT_NET_IS_CONN)
      {
        LOG_MSG_INFO1("Received event DSI_EVT_NET_IS_CONN in WAN_CONNECTED",0,0,0);
        LOG_MSG_INFO1("Ignore event DSI_EVT_NET_IS_CONN in state WAN_CONNECTED",0,0,0);
      }
      else if(evt == DSI_EVT_NET_NO_NET)
      {
        /* 9x25 */
        pMe->state = QCMAP_CM_ENABLE;
        event = QCMAP_CM_EVENT_WAN_DISCONNECTED;
        LOG_MSG_INFO1("QCMAP AP Handle 0x%04x IPv4 Backhaul Disconnected",qcmap_cm_get_handle(),0,0);
        callback = TRUE;

        if(dsi_get_call_end_reason(pMe->dsi_net_hndl.handle,&dsicallend,DSI_IP_FAMILY_V4) == DSI_SUCCESS)
        {
          LOG_MSG_INFO1("Call Disconnected reason type=%d reason code =%d ",dsicallend.reason_type,dsicallend.reason_code,0);
        }
        LOG_MSG_INFO1("Received event DSI_EVT_NET_NO_NET in WAN_CONNECTED",0,0,0);
        LOG_MSG_INFO1("Transition from WAN_CONNECTED to ENABLE",0,0,0);
        if (callback)
        {
          qcmap_cm_send_event(event, (void *)pMe, &dsicallend, NULL);
        }
        if(qcmap_cm_is_disable_in_process())
        {
          qcmap_cm_disable(qcmap_cm_get_handle(), qcmap_cm_errno, &qmi_err_num);
        }
        else if (pMe->auto_connect)
        {
          /*register for serving system NAS indication*/
          qcmap_cm_register_nas_sys_info(TRUE);

          LOG_MSG_INFO1("Start autoconnect",0,0,0);
          /* Set backhual service status to false. We don't know the reason
             for the disconnect. Try to reconnect the backhaul. */
          pMe->backhaul_service = false;
          pMe->StartAutoConnect( QCMAP_MSGR_IP_FAMILY_V4_V01 );
        }
        return;
      }
      else if ( evt == DSI_EVT_NET_RECONFIGURED )
      {
        event = QCMAP_CM_EVENT_WAN_ADDR_RECONF;
        callback = true;
        LOG_MSG_INFO1("Received event DSI_EVT_NET_RECONFIGURED "
            "in WAN_CONNECTED", 0, 0, 0);
      }
    }
    else if (pMe->state == QCMAP_CM_WAN_DISCONNECTING)
    {
      if(evt == DSI_EVT_NET_IS_CONN)
      {
        LOG_MSG_INFO1("Received event DSI_EVT_NET_IS_CONN in WAN_DISCONNECTING",0,0,0);
        LOG_MSG_INFO1("Ignore event DSI_EVT_NET_IS_CONN in WAN_DISCONNECTING",0,0,0);
      }
      else if(evt == DSI_EVT_NET_NO_NET)
      {
        pMe->state = QCMAP_CM_ENABLE;
        event = QCMAP_CM_EVENT_WAN_DISCONNECTED;
        /* Resetting timer value after disconnect*/
        if(QcMapBackhaulMgr && pMe->auto_connect)
          pMe->StopAutoTimer( QCMAP_MSGR_IP_FAMILY_V4_V01 );

        LOG_MSG_INFO1("QCMAP AP Handle 0x%04x IPv4 Backhaul Disconnected",qcmap_cm_get_handle(), 0, 0);
        callback = TRUE;
        if(dsi_get_call_end_reason(pMe->dsi_net_hndl.handle,&dsicallend,DSI_IP_FAMILY_V4) == DSI_SUCCESS)
        {
          LOG_MSG_INFO1("Call Disconnected reason type=%d reason code =%d ",
                          dsicallend.reason_type,dsicallend.reason_code,0);
        }
        LOG_MSG_INFO1("Received event DS_ENETNONET in WAN_DISCONNECTING",0,0,0);
        if (callback)
        {
          qcmap_cm_send_event(event, (void *)pMe, &dsicallend, NULL);
        }
        if(qcmap_cm_is_disable_in_process())
        {
          qcmap_cm_disable(qcmap_cm_get_handle(), qcmap_cm_errno, &qmi_err_num);
        }
        return;
      }
    }
  }
  if (callback)
    qcmap_cm_send_event(event, (void *)pMe, NULL, &wwan_info);
  return;
}

/*===========================================================================

FUNCTION BRING_UP_IPV4_WWAN()

DESCRIPTION

  It will brings up IPV4 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is connected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::BringUpIpv4WWAN
(
 int *qcmap_cm_errno,
 qmi_error_type_v01 *qmi_err_num
 ) //bring_up_ipv4_wwan
{
  int ret = QCMAP_CM_SUCCESS, rval;
  qcmap_dsi_buffer_t qcmap_dsi_buffer;
  int numBytes=0, len;
  struct sockaddr_un cmdq_qcmap;
  dsi_call_param_value_t param_info, param_info_profile;
  char val[MAX_STRING_LENGTH]={0};
  boolean enable_v4=0;

  memset(&qcmap_dsi_buffer, 0x0, sizeof(qcmap_dsi_buffer_t));
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4, GET_VALUE, val, MAX_STRING_LENGTH);
  enable_v4=(boolean)atoi(val);

  LOG_MSG_INFO1("Enter Bring up IPv4 WWAN enable_v4=%d, handle=%x ",enable_v4,dsi_net_hndl.handle,0);

  if (enable_v4)
  {
    if (state == QCMAP_CM_WAN_CONNECTED)
    {
      ret = QCMAP_CM_SUCCESS;
      *qcmap_cm_errno = QCMAP_CM_EALDCONN;
      LOG_MSG_INFO1("QCMAP CM Backhaul already connected",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return ret;
    }
    else if ((state == QCMAP_CM_WAN_CONNECTING) ||
        (state == QCMAP_CM_WAN_DISCONNECTING))
    {
      /* We have some outstanding WAN request */
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      ret = QCMAP_CM_ERROR;
      LOG_MSG_INFO1("QCMAP CM has outstanding backhaul request",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return ret;
    }

    /* If we already have an handle release it. */
    if ( dsi_net_hndl.handle != NULL )
    {
      dsi_rel_data_srvc_hndl(dsi_net_hndl.handle);
      dsi_net_hndl.handle = NULL;
      memset(&(dsi_net_hndl), 0x0, sizeof(dsi_net_hndl));
    }

    /* Acquire the handle. */
    dsi_net_hndl.handle = dsi_get_data_srvc_hndl(QCMAP_Backhaul_WWAN::DSINetCB, (void*) this);

    if (dsi_net_hndl.handle == NULL)
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      ret = QCMAP_CM_ERROR;
      LOG_MSG_INFO1("BringUpIpv4WWAN(): Can not get dsi net handle",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
      return ret;
    }

    param_info_profile.buf_val = NULL;
    switch (wwan_cfg.tech)
    {
      case QCMAP_WAN_TECH_3GPP:
        dsi_net_hndl.tech = "UMTS";
        dsi_net_hndl.profile = wwan_cfg.profile_id.v4.umts_profile_index;
        param_info.num_val = DSI_RADIO_TECH_UMTS;
        param_info_profile.num_val = wwan_cfg.profile_id.v4.umts_profile_index;
        dsi_set_data_call_param(dsi_net_hndl.handle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &param_info_profile);
        break;
      case QCMAP_WAN_TECH_3GPP2:
        dsi_net_hndl.tech = "CDMA";
        dsi_net_hndl.profile =  wwan_cfg.profile_id.v4.cdma_profile_index;
        param_info.num_val = DSI_RADIO_TECH_CDMA;
        param_info_profile.num_val = wwan_cfg.profile_id.v4.cdma_profile_index;
        dsi_set_data_call_param(dsi_net_hndl.handle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &param_info_profile);
        break;
      case QCMAP_WAN_TECH_ANY:
      default:
        dsi_net_hndl.tech = "AUTOMATIC";
        param_info.num_val = DSI_RADIO_TECH_UNKNOWN;
        param_info_profile.num_val = wwan_cfg.profile_id.v4.umts_profile_index;
        dsi_set_data_call_param(dsi_net_hndl.handle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &param_info_profile);
        param_info_profile.num_val = wwan_cfg.profile_id.v4.cdma_profile_index;
        dsi_set_data_call_param(dsi_net_hndl.handle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &param_info_profile);
        break;
    }

    LOG_MSG_INFO1("BringUpIpv4WWAN(): umts_profile_index: %d, cdma_profile_index: %d",
                                      (int)wwan_cfg.profile_id.v4.umts_profile_index,
                                      (int)wwan_cfg.profile_id.v4.cdma_profile_index,0);
    /* set data call param */
    param_info.buf_val = NULL;

    dsi_set_data_call_param(dsi_net_hndl.handle, DSI_CALL_INFO_TECH_PREF, &param_info);

    /* For DSI_NETCTRL, see if we need to set APN in case of UMTS/LTE and Auth profile in case of CDMA */

    dsi_net_hndl.family = DSI_IP_FAMILY_4;
    param_info.buf_val = NULL;
    param_info.num_val = DSI_IP_VERSION_4;
    dsi_set_data_call_param(dsi_net_hndl.handle, DSI_CALL_INFO_IP_VERSION, &param_info);

    LOG_MSG_INFO1("BringUpIpv4WWAN(): dsi_net_handle %X", dsi_net_hndl.handle,0,0);

    /* Bring up the data call. */

    /* KPI log message */
    ds_system_call("echo QCMAP:bringup v4  > /dev/kmsg", \
                    strlen("echo QCMAP:bringup v4  > /dev/kmsg"));

    rval = dsi_start_data_call(dsi_net_hndl.handle);

    if (rval != DSI_SUCCESS)
    {
      *qcmap_cm_errno = QCMAP_CM_ENOWWAN;
      ret = QCMAP_CM_ERROR;
      LOG_MSG_ERROR("BringUpIpv4WWAN(): dsi_start_data_call rval %d", rval,0,0);

      *qmi_err_num = QMI_ERR_CALL_FAILED_V01;
      // Fake a NONET
      cmdq_qcmap.sun_family = AF_UNIX;
      strlcpy(cmdq_qcmap.sun_path, QCMAP_CMDQ_UDS_FILE, sizeof(QCMAP_CMDQ_UDS_FILE));
      len = strlen(cmdq_qcmap.sun_path) + sizeof(cmdq_qcmap.sun_family);

      qcmap_dsi_buffer.dsi_nethandle = dsi_net_hndl.handle;
      qcmap_dsi_buffer.evt = DSI_EVT_NET_NO_NET;
      qcmap_dsi_buffer.user_data =  (void*) this;
      if ((numBytes = sendto(cmdq_qcmap_sockfd, (void *)&qcmap_dsi_buffer, sizeof(qcmap_dsi_buffer_t), 0,
                               (struct sockaddr *)&cmdq_qcmap, len)) == -1)
      {
        LOG_MSG_ERROR("Send Failed from bring_up_ipv4_wwan context", 0, 0, 0);
        return QCMAP_CM_ERROR;
      }
      LOG_MSG_INFO1("Send succeeded in bring_up_ipv4_wwan context", 0, 0, 0);
    }
    else
    {
      *qcmap_cm_errno = QCMAP_CM_EWOULDBLOCK;
      state = QCMAP_CM_WAN_CONNECTING;
    }
  }
  else
  {
    ret = QCMAP_CM_ERROR;
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR("Cannot bring up IPV4 wwan:-  dsi handle 0x%x, IPv4 enabled:- %d",
                   dsi_net_hndl.handle,enable_v4,0);
  }
  return ret;
}

/*===========================================================================

FUNCTION BRING_UP_IPV6_WWAN()

DESCRIPTION

  It will brings up IPV6 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is connected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::BringUpIpv6WWAN
(
 int                *qcmap_cm_errno,
 qmi_error_type_v01 *qmi_err_num
 )
{

  int ret = QCMAP_CM_SUCCESS, rval;
  qcmap_dsi_buffer_t qcmap_dsi_buffer;
  int numBytes=0, len;
  struct sockaddr_un cmdq_qcmap;
  dsi_call_param_value_t param_info, param_info_profile;
  char val[MAX_STRING_LENGTH];
  boolean enable_v6=0;

  memset(&qcmap_dsi_buffer, 0x0, sizeof(qcmap_dsi_buffer_t));
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,GET_VALUE,val,MAX_STRING_LENGTH);
  enable_v6=(boolean)atoi(val);

  LOG_MSG_INFO1("Enter Bring up IPv4v6 WWAN enable_v6=%d ",enable_v6,0,0);
  //boolean enable_v6=read from xml QCMAP_Backhaul::enable_ipv6 ;
  if (enable_v6)
  {
    if(ipv6_state == QCMAP_CM_V6_WAN_CONNECTED)
    {
      ret = QCMAP_CM_SUCCESS;
      *qcmap_cm_errno = QCMAP_CM_EALDCONN;
      LOG_MSG_INFO1("QCMAP CM Backhaul already connected",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return ret;
    }
    else if (ipv6_state == QCMAP_CM_V6_WAN_CONNECTING)
    {
      /* We have some outstanding WAN request */
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      ret = QCMAP_CM_ERROR;
      LOG_MSG_INFO1("QCMAP CM has outstanding backhaul request",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return ret;
    }

    /* Release the previously acquired handle. */
    if (ipv6_dsi_net_hndl.handle != NULL)
    {
      dsi_rel_data_srvc_hndl(ipv6_dsi_net_hndl.handle);
      ipv6_dsi_net_hndl.handle = NULL;
      memset(&(ipv6_dsi_net_hndl), 0x0, sizeof(ipv6_dsi_net_hndl));
    }

    /* Acquire the handle. */
    ipv6_dsi_net_hndl.handle = dsi_get_data_srvc_hndl(QCMAP_Backhaul_WWAN::DSINetCB, (void*) this);
    if (ipv6_dsi_net_hndl.handle == NULL)
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      ret = QCMAP_CM_ERROR;
      LOG_MSG_INFO1("BringUpIpv6WWAN(): Can not get ipv6 dsi net handle",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
      return ret;
    }

    switch (wwan_cfg.tech)
    {
      case QCMAP_WAN_TECH_3GPP:
        ipv6_dsi_net_hndl.tech = "UMTS";
        ipv6_dsi_net_hndl.profile = wwan_cfg.profile_id.v6.umts_profile_index;
        param_info.num_val = DSI_RADIO_TECH_UMTS;
        param_info_profile.num_val = wwan_cfg.profile_id.v6.umts_profile_index;
        dsi_set_data_call_param(ipv6_dsi_net_hndl.handle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &param_info_profile);
        break;

      case QCMAP_WAN_TECH_3GPP2:
        ipv6_dsi_net_hndl.tech = "CDMA";
        ipv6_dsi_net_hndl.profile =  wwan_cfg.profile_id.v6.cdma_profile_index;
        param_info.num_val = DSI_RADIO_TECH_CDMA;
        param_info_profile.num_val = wwan_cfg.profile_id.v6.cdma_profile_index;
        dsi_set_data_call_param(ipv6_dsi_net_hndl.handle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &param_info_profile);
        break;

      case QCMAP_WAN_TECH_ANY:
      default:
        ipv6_dsi_net_hndl.tech = "AUTOMATIC";
        param_info.num_val = DSI_RADIO_TECH_UNKNOWN;
        param_info_profile.num_val = wwan_cfg.profile_id.v6.umts_profile_index;
        dsi_set_data_call_param(ipv6_dsi_net_hndl.handle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &param_info_profile);
        param_info_profile.num_val = wwan_cfg.profile_id.v6.cdma_profile_index;
        dsi_set_data_call_param(ipv6_dsi_net_hndl.handle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &param_info_profile);
        break;
    }

    LOG_MSG_INFO1("BringUpIpv6WWAN(): umts_profile_index: %d, cdma_profile_index: %d",
                                      (int)wwan_cfg.profile_id.v6.umts_profile_index,
                                      (int)wwan_cfg.profile_id.v6.cdma_profile_index,0);

    /* set data call param */
    param_info.buf_val = NULL;

    dsi_set_data_call_param(ipv6_dsi_net_hndl.handle, DSI_CALL_INFO_TECH_PREF, &param_info);

    /* For DSI_NETCTRL, see if we need to set APN in case of UMTS/LTE and Auth profile in case of CDMA */

    ipv6_dsi_net_hndl.family = DSI_IP_FAMILY_6;
    param_info.buf_val = NULL;
    param_info.num_val = DSI_IP_VERSION_6;
    dsi_set_data_call_param(ipv6_dsi_net_hndl.handle, DSI_CALL_INFO_IP_VERSION, &param_info);

    LOG_MSG_INFO1("BringUpIpv6WWAN(): ipv6_dsi_net_handle %X", ipv6_dsi_net_hndl.handle,0,0);

    /* KPI log message */
    ds_system_call("echo QCMAP:bringup v6  > /dev/kmsg", \
        strlen("echo QCMAP:bringup v6  > /dev/kmsg"));

    /* Connecting IPv6 WAN */
    rval = dsi_start_data_call(ipv6_dsi_net_hndl.handle);

    if (rval != DSI_SUCCESS )
    {
      *qcmap_cm_errno = QCMAP_CM_ENOWWAN;
      ret = QCMAP_CM_ERROR;
      LOG_MSG_ERROR("BringUpIpv6WWAN(): dsi_start_data_call ipv6 rval %d",
          rval,0,0);
      *qmi_err_num = QMI_ERR_CALL_FAILED_V01;
      // Fake a NONET to send client ind
      cmdq_qcmap.sun_family = AF_UNIX;
      strlcpy(cmdq_qcmap.sun_path, QCMAP_CMDQ_UDS_FILE, sizeof(QCMAP_CMDQ_UDS_FILE));
      len = strlen(cmdq_qcmap.sun_path) + sizeof(cmdq_qcmap.sun_family);

      qcmap_dsi_buffer.dsi_nethandle = ipv6_dsi_net_hndl.handle;
      qcmap_dsi_buffer.evt = DSI_EVT_NET_NO_NET;
      qcmap_dsi_buffer.user_data =  (void*) this;
      if ((numBytes = sendto(cmdq_qcmap_sockfd, (void *)&qcmap_dsi_buffer, sizeof(qcmap_dsi_buffer_t), 0,
              (struct sockaddr *)&cmdq_qcmap, len)) == -1)
      {
        LOG_MSG_ERROR("Send Failed from BringUpIpv6WWAN() context", 0, 0, 0);
        return QCMAP_CM_ERROR;
      }
      LOG_MSG_INFO1("Send succeeded in BringUpIpv6WWAN() context", 0, 0, 0);
    }
    else
    {
      *qcmap_cm_errno = QCMAP_CM_EWOULDBLOCK;
      ipv6_state = QCMAP_CM_V6_WAN_CONNECTING;
    }
  }
  else
  {
    ret = QCMAP_CM_ERROR;
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
  }
  return ret;
}

/*===========================================================================

FUNCTION BRING_UP_IPV4V6_WWAN()

DESCRIPTION

  It will brings up IPV4V6 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is connected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::BringUpIpv4v6WWAN
(
 int                *qcmap_cm_errno,
 qmi_error_type_v01 *qmi_err_num
 )
{
  int v4_status = QCMAP_CM_SUCCESS;
  int v6_status = QCMAP_CM_SUCCESS;

  LOG_MSG_INFO1("inside BringUpIpv4v6WWAN()",0,0,0);

  if (BringUpIpv4WWAN(qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS) {
    v4_status = QCMAP_CM_ERROR;
    LOG_MSG_ERROR("BringUpIpv4WWAN() failed with error!!",0,0,0);
  }
  if (BringUpIpv6WWAN(qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS) {
    v6_status = QCMAP_CM_ERROR;
    LOG_MSG_ERROR("BringUpIpv6WWAN() failed with error!!",0,0,0);
  }

  if(v4_status == QCMAP_CM_SUCCESS && v6_status == QCMAP_CM_SUCCESS)
  {
    LOG_MSG_INFO1("BringUpIpv4v6WWAN() Successful",0,0,0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    LOG_MSG_INFO1("BringUpIpv4v6WWAN() failed",0,0,0);
    return QCMAP_CM_ERROR;
  }

}

/*===========================================================================

FUNCTION BRING_DOWN_IPV4_WWAN()

DESCRIPTION

  It will teardown IPV4 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::BringDownIpv4WWAN
(
 int                 qcmap_cm_handle,     /* Handle for Mobile AP CM  */
 int                *qcmap_cm_errno,     /* error condition value    */
 qmi_error_type_v01 *qmi_err_num
 )
{
  int qmi_err;
  int ret = QCMAP_CM_SUCCESS;
  qmi_client_error_type qmi_error;

  ds_assert(qcmap_cm_errno != NULL);

  LOG_MSG_INFO1("BringDownIpv4WWAN(): enter",0,0,0);

  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR("Wrong QCMAP CM Handle",0,0,0);
    return ret;
  }

  if (state <= QCMAP_CM_ENABLE)
  {
    /* WAN is not connected */
    *qcmap_cm_errno = QCMAP_CM_EALDDISCONN;
    ret = QCMAP_CM_SUCCESS;
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("QCMAP CM IPV4 WAN is not connected",0,0,0);
    return ret;
  }
  /* Handle IPV4 disconnect backhaul request gracefully when
     mobile ap disable is processing */
  /* These changes are made to support consecutive usb plug unplug */

  if ((state == QCMAP_CM_WAN_CONNECTING ||
        state == QCMAP_CM_WAN_DISCONNECTING) &&
      (qcmap_cm_is_disable_in_process() == false))
  {
    /* We can some outstanding WAN request */
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("QCMAP CM has outstanding backhaul request",0,0,0);
    return ret;
  }
  /* Checks are added to make sure wan disconnected when mobile ap
     disable is triggered */
  if(state == QCMAP_CM_WAN_CONNECTED ||
      ((state == QCMAP_CM_WAN_CONNECTING)&&
       (qcmap_cm_is_disable_in_process() == TRUE)))
  {
    if (dsi_net_hndl.handle != NULL)
    {
      LOG_MSG_INFO1("Bring down IPv4 WAN",0,0,0);
      state = QCMAP_CM_WAN_DISCONNECTING;
      dsi_stop_data_call(dsi_net_hndl.handle);
      LOG_MSG_INFO1("After dsi_stop_data_call",0,0,0);
      *qcmap_cm_errno = QCMAP_CM_EWOULDBLOCK;
      ret = QCMAP_CM_ERROR;
    }
    else
    {
      LOG_MSG_ERROR("Invalid Handle: Cannot bring down IPv4",0,0,0);
      *qcmap_cm_errno = QCMAP_CM_EINVAL;
      ret = QCMAP_CM_ERROR;
      *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    }
  }
  /* return QCMAP_CM_EWOULDBLOCK when disable in process to ensure
     mobile ap disable return success  */
  else if((state == QCMAP_CM_WAN_DISCONNECTING) &&
           (qcmap_cm_is_disable_in_process() == TRUE))
  {
    LOG_MSG_INFO1("IPV4 call in disconnecting state: Disable process",0,0,0);
    *qcmap_cm_errno = QCMAP_CM_EWOULDBLOCK;
    ret = QCMAP_CM_ERROR;
  }
  else
  {
    *qcmap_cm_errno = QCMAP_CM_EINVAL;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("Cannot bring down IPV4 wwan because of invalid state %d",state,0,0);
  }

  LOG_MSG_INFO1("QCMAP AP Handle %04x tear down backhaul.",qcmap_cm_handle,0,0);

  return ret;
}

/*===========================================================================

FUNCTION bring_down_ipv6_wwan()

DESCRIPTION

  It will teardown IPV6 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::BringDownIpv6WWAN
(
 int                  qcmap_cm_handle,    /* Handle for Mobile AP CM  */
 int                 *qcmap_cm_errno,     /* error condition value    */
 qmi_error_type_v01  *qmi_err_num
 )
{
  int qmi_err;
  int ret = QCMAP_CM_SUCCESS;
  qmi_client_error_type qmi_error;

  ds_assert(qcmap_cm_errno != NULL);

  LOG_MSG_INFO1("BringDownIpv6WWAN(): enter",0,0,0);

  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR("Wrong QCMAP CM Handle",0,0,0);
    return ret;
  }

  if (ipv6_state <= QCMAP_CM_V6_ENABLE)
  {
    /* WAN is not connected */
    *qcmap_cm_errno = QCMAP_CM_EALDDISCONN;
    ret = QCMAP_CM_SUCCESS;
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("QCMAP CM IPV6 WAN is not connected",0,0,0);
    return ret;
  }
  /* Handle IPV6 disconnect backhaul request gracefully when
     mobile ap disable is processing */
  /* These changes are made to support consecutive usb plug unplug */

  if ((ipv6_state == QCMAP_CM_V6_WAN_CONNECTING ||
        ipv6_state == QCMAP_CM_V6_WAN_DISCONNECTING) &&
      (qcmap_cm_is_disable_in_process() == false))
  {
    /* We can some outstanding WAN request */
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("QCMAP CM has outstanding backhaul request",0,0,0);
    return ret;
  }
  /* Checks are added to make sure wan disconnected when mobile ap
     disable is triggered */
  if((ipv6_state == QCMAP_CM_V6_WAN_CONNECTED) ||
      ((ipv6_state == QCMAP_CM_V6_WAN_CONNECTING) &&
       (qcmap_cm_is_disable_in_process() == TRUE)))
  {
    if (ipv6_dsi_net_hndl.handle != NULL)
    {
      LOG_MSG_INFO1("Bring down IPv6 WAN",0,0,0);
      ipv6_state = QCMAP_CM_V6_WAN_DISCONNECTING;
      dsi_stop_data_call(ipv6_dsi_net_hndl.handle);
      LOG_MSG_INFO1("After dsi_stop_data_call",0,0,0);
      *qcmap_cm_errno = QCMAP_CM_EWOULDBLOCK;
      ret = QCMAP_CM_ERROR;
    }
    else
    {
      LOG_MSG_INFO1("Invalid Handle: Cannot bring down IPv6",0,0,0);
      *qcmap_cm_errno = QCMAP_CM_EINVAL;
      ret = QCMAP_CM_ERROR;
      *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    }
  }
  /* return QCMAP_CM_EWOULDBLOCK when disable in process to ensure
     mobile ap disable return success  */
  else if((ipv6_state == QCMAP_CM_V6_WAN_DISCONNECTING) &&
      (qcmap_cm_is_disable_in_process() == TRUE))
  {
    LOG_MSG_INFO1("IPV6 call in disconnecting state: Disable process",0,0,0);
    *qcmap_cm_errno = QCMAP_CM_EWOULDBLOCK;
    ret = QCMAP_CM_ERROR;
  }
  else
  {
    *qcmap_cm_errno = QCMAP_CM_EINVAL;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("Cannot bring down IPV6 wwan because of invalid state %d",ipv6_state,0,0);
  }

  LOG_MSG_INFO1("QCMAP AP Handle %04x tear down backhaul.",qcmap_cm_handle,0,0);

  return ret;
}

/*===========================================================================

FUNCTION bring_down_ipv4v6_wwan()

DESCRIPTION

  It will teardown IPV4V6 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::BringDownIpv4v6WWAN
(
 int                  qcmap_cm_handle,    /* Handle for Mobile AP CM  */
 int                 *qcmap_cm_errno,     /* error condition value    */
 qmi_error_type_v01  *qmi_err_num
)
{
  int v4_status = QCMAP_CM_SUCCESS;
  int v6_status = QCMAP_CM_SUCCESS;

  LOG_MSG_INFO1("inside BringDownIpv4v6WWAN()",0,0,0);
  if (BringDownIpv4WWAN(qcmap_cm_handle, qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS) {
    v4_status = QCMAP_CM_ERROR;
    LOG_MSG_ERROR("BringDownIpv4WWAN() failed with error!!",0,0,0);
  }
  if (BringDownIpv6WWAN(qcmap_cm_handle, qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS) {
    v6_status = QCMAP_CM_ERROR;
    LOG_MSG_ERROR("BringDownIpv6WWAN() failed with error!!",0,0,0);
  }

  if(v4_status == QCMAP_CM_SUCCESS && v6_status == QCMAP_CM_SUCCESS)
  {
    LOG_MSG_INFO1("BringDownIpv4v6WWAN() Successful",0,0,0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    LOG_MSG_INFO1("BringDownIpv4v6WWAN() failed",0,0,0);
    return QCMAP_CM_ERROR;
  }
}
/*===========================================================================

FUNCTION ConnectBackHaul()

DESCRIPTION

  It will bringup WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already connected, returns QCMAP_CM_SUCCESS.
  Otherwise, return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress or LAN is not connected yet

SIDE EFFECTS
===========================================================================*/
int QCMAP_Backhaul_WWAN::ConnectBackHaul
(
 int                               qcmap_cm_handle, /* Handle for MobileAP CM */
 qcmap_msgr_wwan_call_type_v01     call_type,       /* Call type to be brought UP. */
 int                               *qcmap_cm_errno, /* Error condition value  */
 qmi_error_type_v01                *qmi_err_num     /* QMI error type */
 )
{
  int ret = QCMAP_CM_SUCCESS, rval;
  qmi_client_error_type qmi_error;
  ds_assert(qcmap_cm_errno != NULL);
  ds_assert(qmi_err_num != NULL);
  LOG_MSG_INFO1("ConnectBackHaul(): enter ",0,0,0);


  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR("Wrong QCMAP CM Handle",0,0,0);
    return ret;
  }

#ifndef FEATURE_QTIMAP_OFFTARGET
  /* Skip NAS check for modem loopback call. */
  if ( !modem_loopback_mode )
  {
    if (get_nas_config(this, qcmap_cm_handle, qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS)
    {
      ret = QCMAP_CM_ERROR;
      LOG_MSG_ERROR("get_nas_config() returned error!!",0,0,0);
      return ret;
    }
  }
#endif

  *qcmap_cm_errno = QCMAP_CM_EWOULDBLOCK;

  switch ( call_type )
  {
    case QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01:
      {
        if (BringUpIpv4WWAN(qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS)
        {
          ret = QCMAP_CM_ERROR;
          LOG_MSG_ERROR("BringUpIpv4WWAN() returned error!!",0,0,0);
        }
        break;
      }
    case QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01:
      {
        if (BringUpIpv6WWAN(qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS)
        {
          ret = QCMAP_CM_ERROR;
          LOG_MSG_ERROR("BringUpIpv6WWAN() returned error!!",0,0,0);
        }
        break;
      }
    case QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01:
      {
        if (BringUpIpv4v6WWAN(qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS)
        {
          ret = QCMAP_CM_ERROR;
          LOG_MSG_ERROR("BringUpIpv4v6WWAN() returned error!!",0,0,0);
        }
        break;
      }
    default:
      *qcmap_cm_errno = QCMAP_CM_EINVAL;
      ret = QCMAP_CM_ERROR;
      *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
      LOG_MSG_ERROR("ConnectBackhaul() Invalid Call Type!!",0,0,0);
  }

  LOG_MSG_INFO1("QCMAP AP Handle 0x%04x Connecting Backhaul.",qcmap_cm_handle,0,0);

  return ret;
}

/*===========================================================================

FUNCTION DisconnectBackHaul()

DESCRIPTION

  It will teardown WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
 *qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_CM_EBADAPP           invalid application ID specified
 QCMAP_CM_EWOULDBLOCK       the operation would block
 QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::DisconnectBackHaul
(
 int                               qcmap_cm_handle, /* Handle for MobileAP CM */
 qcmap_msgr_wwan_call_type_v01     call_type,       /* Call type to be brought down. */
 int                               *qcmap_cm_errno, /* Error condition value  */
 qmi_error_type_v01                *qmi_err_num     /* QMI error type */
 )
{
  int qmi_err;
  int ret = QCMAP_CM_SUCCESS;
  qmi_client_error_type qmi_error;

  ds_assert(qcmap_cm_errno != NULL);
  ds_assert(qmi_err_num != NULL);

  LOG_MSG_INFO1("DisconnectBackHaul(): enter",0,0,0);

  if (qcmap_cm_handle != qcmap_cm_get_handle())
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_ERROR("Wrong QCMAP CM Handle",0,0,0);
    return ret;
  }

  if (state <= QCMAP_CM_ENABLE &&
      ipv6_state <= QCMAP_CM_V6_ENABLE)
  {
    /* WAN is not connected */
    *qcmap_cm_errno = QCMAP_CM_EALDDISCONN;
    ret = QCMAP_CM_SUCCESS;
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("QCMAP CM WAN is not connected",0,0,0);
    return ret;
  }

  switch ( call_type )
  {
    case QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01:
      {
        if (BringDownIpv4WWAN(qcmap_cm_handle, qcmap_cm_errno, qmi_err_num)
            != QCMAP_CM_SUCCESS)
        {
          ret = QCMAP_CM_ERROR;
          LOG_MSG_ERROR("BringDownIpv4WWAN() returned error!!",0,0,0);
        }
        break;
      }
    case QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01:
      {
        if (BringDownIpv6WWAN(qcmap_cm_handle, qcmap_cm_errno, qmi_err_num)
            != QCMAP_CM_SUCCESS)
        {
          ret = QCMAP_CM_ERROR;
          LOG_MSG_ERROR("BringDownIpv6WWAN() returned error!!",0,0,0);
        }
        break;
      }
    case QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01:
      {
        if (BringDownIpv4v6WWAN(qcmap_cm_handle, qcmap_cm_errno, qmi_err_num) != QCMAP_CM_SUCCESS)
        {
          ret = QCMAP_CM_ERROR;
          LOG_MSG_ERROR("BringDownIpv4v6WWAN() returned error!!",0,0,0);
        }
        break;
      }
    default:
      *qcmap_cm_errno = QCMAP_CM_EINVAL;
      ret = QCMAP_CM_ERROR;
      *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
      LOG_MSG_ERROR("DisconnectBackhaul() Invalid Call Type!!",0,0,0);
  }

  LOG_MSG_INFO1("QCMAP AP Handle %04x tear down backhaul.",qcmap_cm_handle,0,0);

  return ret;
}

/*===========================================================================

FUNCTION ConnectV4AndV6()

DESCRIPTION
   Connects v4 and v6 data call for a profile.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
void QCMAP_Backhaul_WWAN::ConnectV4AndV6()
{
  int                 qcmap_cm_errno;
  qmi_error_type_v01  qmi_err_num;

  /* Register for NAS serving system indication */
  qcmap_cm_register_nas_sys_info(TRUE);

  backhaul_service = false;

  if (state == QCMAP_CM_ENABLE)
  {
    if( auto_connect_timer_running_v4)
    {
      StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V4_V01);
    }
    LOG_MSG_INFO1("Connecting V4 call for Profile(%p)", QcMapBackhaul->profileHandle, 0, 0);
    ConnectBackHaul(qcmap_cm_get_handle(), QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
                      &qcmap_cm_errno, &qmi_err_num);
  }
  if (ipv6_state == QCMAP_CM_V6_ENABLE)
  {
    if( auto_connect_timer_running_v6)
    {
      StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V6_V01);
    }
    LOG_MSG_INFO1("Connecting V6 call for Profile(%p)", QcMapBackhaul->profileHandle, 0, 0);
    ConnectBackHaul(qcmap_cm_get_handle(), QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01,
                      &qcmap_cm_errno, &qmi_err_num);
  }
}

/*===========================================================================

FUNCTION DisconnectV4AndV6()

DESCRIPTION
   Disconnets v4 and v6 data call for a profile.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
void QCMAP_Backhaul_WWAN::DisconnectV4AndV6()
{
  int                 qcmap_cm_errno;
  qmi_error_type_v01  qmi_err_num;

  /* DeRegister for NAS serving system indication */
  //qcmap_cm_register_nas_sys_info(FALSE);

  backhaul_service = false;

  if (state == QCMAP_CM_WAN_CONNECTING || state == QCMAP_CM_WAN_CONNECTED)
  {
    if( auto_connect_timer_running_v4)
    {
      StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V4_V01);
    }
    LOG_MSG_INFO1("Disconnecting V4 call for Profile(%p)", QcMapBackhaul->profileHandle, 0, 0);
    DisconnectBackHaul(qcmap_cm_get_handle(), QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
                        &qcmap_cm_errno, &qmi_err_num);
  }
  if (ipv6_state == QCMAP_CM_V6_WAN_CONNECTING ||
        ipv6_state == QCMAP_CM_V6_WAN_CONNECTED)
  {
    if( auto_connect_timer_running_v6)
    {
      StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V6_V01);
    }
    LOG_MSG_INFO1("Disconnecting V6 call for Profile(%p)", QcMapBackhaul->profileHandle, 0, 0);
    DisconnectBackHaul(qcmap_cm_get_handle(), QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01,
                      &qcmap_cm_errno, &qmi_err_num);
  }
}

/*===========================================================================
FUNCTION ConnectAllPDNs()

DESCRIPTION
  Connects all PDN's, in each backhaul WWAN Object.

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void  QCMAP_Backhaul_WWAN::ConnectAllPDNs()
{
  qmi_error_type_v01 qmi_err_num;
  int qcmap_cm_errno;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;

    if (pBackhaulWWAN->auto_connect && pBackhaulWWAN->state == QCMAP_CM_ENABLE &&
        !qcmap_cm_is_disable_in_process())
    {
      LOG_MSG_INFO1("Auto connect start(v4)", 0, 0, 0);
      pBackhaulWWAN->backhaul_service = TRUE;

      /* Always reset timer value: This takes care of corner case where modem is OoS
         then in service, so the timer resets instead of continuing from prev value */

      pBackhaulWWAN->auto_timer_value_v4 = AUTO_CONNECT_TIMER;
      if(pBackhaulWWAN->auto_connect_timer_running_v4 )
      {
        pBackhaulWWAN->StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V4_V01);
      }
      pBackhaulWWAN->ConnectBackHaul(qcmap_cm_get_handle(),
                                      QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
                                      &qcmap_cm_errno,
                                      &qmi_err_num);
    }
    if(pBackhaulWWAN->auto_connect && pBackhaulWWAN->ipv6_state == QCMAP_CM_V6_ENABLE &&
        !qcmap_cm_is_disable_in_process())
    {
      LOG_MSG_INFO1("Auto connect start(v6)", 0, 0, 0);
      pBackhaulWWAN->backhaul_service = TRUE;

      /* Always reset timer value: This takes care of corner case where modem is OoS
         then in service, so the timer resets instead of continuing from prev value */
      pBackhaulWWAN->auto_timer_value_v6 = AUTO_CONNECT_TIMER;
      if(pBackhaulWWAN->auto_connect_timer_running_v6 )
      {
        pBackhaulWWAN->StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V6_V01);
      }
      pBackhaulWWAN->ConnectBackHaul(qcmap_cm_get_handle(),
                                      QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01,
                                      &qcmap_cm_errno,
                                      &qmi_err_num);
    }
  }
}

/*===========================================================================
FUNCTION DisconnectAllPDNs()

DESCRIPTION
   Disconnects all PDN's, in each backhaul WWAN Object.

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void  QCMAP_Backhaul_WWAN::DisconnectAllPDNs()
{
  uint8 prev_auto_state;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;

    /* Store the auto_connect state before SSR.*/
    prev_auto_state = pBackhaulWWAN->auto_connect;
    /* We intentionally disable auto-connect to not trigger the call during SSR. */
    pBackhaulWWAN->auto_connect= false;

    /* If we already have an v4 handle release it. */
    if ( pBackhaulWWAN->dsi_net_hndl.handle != NULL )
    {
      /* If the state is not enable, disconnect V4 call first before releasing
       * the handle. */
      if (pBackhaulWWAN->state != QCMAP_CM_ENABLE)
      {
        LOG_MSG_INFO1("Disconnect V4 call before releasing the handle.", 0, 0, 0);
        QCMAP_Backhaul_WWAN::ProcessDSI_NetEvent(pBackhaulWWAN->dsi_net_hndl.handle, (void *)pBackhaulWWAN,
                                                  DSI_EVT_NET_NO_NET, NULL);
      }
      dsi_rel_data_srvc_hndl(pBackhaulWWAN->dsi_net_hndl.handle);
      pBackhaulWWAN->dsi_net_hndl.handle = NULL;
      memset(&(pBackhaulWWAN->dsi_net_hndl), 0x0, sizeof(pBackhaulWWAN->dsi_net_hndl));
    }

    /* If we already have an v6 handle release it. */
    if (pBackhaulWWAN->ipv6_dsi_net_hndl.handle != NULL)
    {
      /* If the state is not enable, disconnect V6 call first before releasing
       * the handle. */
      if (pBackhaulWWAN->ipv6_state != QCMAP_CM_V6_ENABLE)
      {
        LOG_MSG_INFO1("Disconnect V6 call before releasing the handle",
            0, 0, 0);
        QCMAP_Backhaul_WWAN::ProcessDSI_NetEvent(pBackhaulWWAN->ipv6_dsi_net_hndl.handle, (void *)pBackhaulWWAN,
                                                  DSI_EVT_NET_NO_NET, NULL);
      }
      dsi_rel_data_srvc_hndl(pBackhaulWWAN->ipv6_dsi_net_hndl.handle);
      pBackhaulWWAN->ipv6_dsi_net_hndl.handle = NULL;
      memset(&(pBackhaulWWAN->ipv6_dsi_net_hndl), 0x0, sizeof(pBackhaulWWAN->ipv6_dsi_net_hndl));
    }

    /* Restore the auto-connect state. */
    pBackhaulWWAN->auto_connect = prev_auto_state;
  }

  /*----------------------------------------------------------------------------
    Invoke dsi_release
  ---------------------------------------------------------------------------- */
  if (dsi_release(DSI_MODE_GENERAL) != DSI_SUCCESS)
  {
    LOG_MSG_ERROR("Error in dsi_release", 0, 0, 0);
    return;
  }
  LOG_MSG_INFO1("DSI Release Success during SSR", 0, 0, 0);
}


/*===========================================================================
FUNCTION GetBearerTech()

DESCRIPTION
  This Function gets Bearer Tech Type.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR.

SIDE EFFECTS

===========================================================================*/
int QCMAP_Backhaul_WWAN::GetBearerTech
(
  const dsi_call_info_t  *dsi_net_hndl,     /* Handle for DSI net       */
  dsi_data_bearer_tech_t *bearer_tech       /* Bearer Type              */
)
{

  if((dsi_net_hndl == NULL) || (dsi_net_hndl->handle == NULL) || (bearer_tech == NULL))
  {
    LOG_MSG_ERROR("dsiNetHandle not initialized or bearer_tech is NULL", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  memset(bearer_tech, 0, sizeof(dsi_data_bearer_tech_t));
  *bearer_tech = dsi_get_current_data_bearer_tech(dsi_net_hndl->handle);

  return QCMAP_CM_SUCCESS;
}


/*===========================================================================
  FUNCTION AddVLANMapping
==========================================================================*/
/*!
@brief
  - Sets the VLAN ID which this PDN is mapped to

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_Backhaul_WWAN::AddVlanMapping
(
 int16_t vlan_id,
 qmi_error_type_v01 *qmi_err_num
)
{
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char data[MAX_STRING_LENGTH] = {0};

  if(NULL == qmi_err_num)
  {
    LOG_MSG_INFO1("qmi_err_num is NULL", 0, 0, 0);
    return false;
  } else if(NULL == QcMapBackhaul) {
    LOG_MSG_INFO1("QcMapBackhaul is NULL", 0, 0, 0);
    return false;
  }

  if((QcMapBackhaul->vlan_id == vlan_id) || (QCMAP_MSGR_VLAN_ID_NONE_V01 != QcMapBackhaul->vlan_id))
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("profile handle %d has VLAN ID already set to %d", this->GetProfileHandle(),
                   QcMapBackhaul->vlan_id, 0);
    return false;
  }

  //check out of bounds error
  if( ! IS_VLAN_ID_VALID(vlan_id) )
  {
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    LOG_MSG_ERROR("VLAN ID(%d) is out of bounds", vlan_id, 0, 0);
    return false;
  }

  //get lan object and associate this profile handle
  if(!lan_mgr)
  {
    LOG_MSG_INFO1("LAN Mgr is NULL", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    return false;
  }
  if(!lan_mgr->IsVLANToPDNMappingAllowed())
  {
    LOG_MSG_INFO1("VLAN to PDN mapping not allowed", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    return false;
  }
  if((lan_obj = lan_mgr->GetLANBridge(vlan_id)) == NULL)
  {
    LOG_MSG_INFO1("LAN object %d is NULL", vlan_id, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    return false;
  }
  if(!lan_obj->SetBackhaulProfileHandle(this->GetProfileHandle()))
  {
    LOG_MSG_INFO1("Failed to set LAN object %d to have profile handle %d", vlan_id,
                  this->GetProfileHandle(), 0);
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    return false;
  }

  LOG_MSG_INFO1("%s", lan_obj->GetBridgeNameAsCStr(), 0, 0);

  LOAD_XML_FILE(xml_file, QcMapMgr);

  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find root XML corresponding to PDN",0,0,0);
    return false;
  }

  //Disable the old rules if the vlan ID is valid
  if (QcMapBackhaul->vlan_id != QCMAP_MSGR_VLAN_ID_NONE_V01)
  {
    if (!DisableVlanPdnRules(QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01))
    {
      LOG_MSG_INFO1("VLAN to PDN rules are not installed", vlan_id, 0, 0);
    }

    if (vlan_id == QCMAP_MSGR_VLAN_ID_NONE_V01)
    {
      /* Vlan ID is removed, remove from XML */
      child = root.child(VLAN_ID_TAG);
      root.remove_child(child);
    }
    else //Update VLAN ID in XML
    {
      snprintf(data, sizeof(data), "%d", vlan_id);
      root.child(VLAN_ID_TAG).text() = data;
    }
  }
  else
  {
    //Add VLAN ID in XML
    child = root.append_child(VLAN_ID_TAG);
    snprintf(data, sizeof(data), "%d", vlan_id);
    child.append_child(pugi::node_pcdata).text() = data;
  }

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  //update the vlan ID associated with the PDN for this backhaul object
  QcMapBackhaul->vlan_id = vlan_id;

  /* Install rules for new vlan. Will only install if VLAN is up and PDN is
     in connected state */
  if (QcMapBackhaul->vlan_id != QCMAP_MSGR_VLAN_ID_NONE_V01 && !EnableVlanPdnRules())
  {
    LOG_MSG_INFO1("Unable to install rules", vlan_id, 0, 0);
  }

  LOG_MSG_INFO1("Added VLAN %d to profile handle %d", vlan_id, this->GetProfileHandle(), 0);
  return true;
}

/*===========================================================================
  FUNCTION DeleteVLANMapping
==========================================================================*/
/*!
@brief
  - Deletes the VLAN ID which this PDN is mapped to

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_Backhaul_WWAN::DeleteVlanMapping
(
 int16_t vlan_id,
 qmi_error_type_v01 *qmi_err_num
)
{
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char data[MAX_STRING_LENGTH] = {0};

  //return out of bounds error if 0 or >4094
  if (! IS_VLAN_ID_VALID(vlan_id))
  {
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    LOG_MSG_ERROR("VLAN ID (%d) is out of bounds", vlan_id, 0, 0);
    return false;
  }

  //get lan object and associate this profile handle
  if(!lan_mgr)
  {
    LOG_MSG_INFO1("LAN Mgr is NULL", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    return false;
  }
  if((lan_obj = lan_mgr->GetLANBridge(vlan_id)) == NULL)
  {
    LOG_MSG_INFO1("LAN object %d is NULL", vlan_id, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    return false;
  }
  if(lan_obj->GetBackhaulProfileHandle() != this->GetProfileHandle())
  {
    LOG_MSG_INFO1("LAN object %d is already not associated with this profile handle: %d", vlan_id,
                  this->GetProfileHandle(), 0);
    return true;
  }

  LOG_MSG_INFO1("%s", lan_obj->GetBridgeNameAsCStr(), 0, 0);

  //Disable the old rules
  if(!DisableVlanPdnRules(QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01))
  {
    LOG_MSG_INFO1("VLAN to PDN rules are not installed", vlan_id, 0, 0);
  }

  if(!lan_obj->SetBackhaulProfileHandle(0))
  {
    LOG_MSG_INFO1("Failed to set LAN object %d to delete its profile handle %d", vlan_id, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ID_V01;
    return false;
  }

  //update the vlan ID associated with the PDN for this backhaul object
  QcMapBackhaul->vlan_id = QCMAP_MSGR_VLAN_ID_NONE_V01;

  LOAD_XML_FILE(xml_file, QcMapMgr);
  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find root XML corresponding to PDN",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  /* Vlan ID is removed, remove from XML */
  child = root.child(VLAN_ID_TAG);
  root.remove_child(child);

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  LOG_MSG_INFO1("Deleted VLAN %d from profile handle %d", vlan_id, this->GetProfileHandle(), 0);
  return true;
}


/*===========================================================================
  FUNCTION EnableVlanPdnRules
==========================================================================*/
/*!
@brief
  - Enables the rules to map VLAN to PDN if the VLAN iface and PDN are both up

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::EnableVlanPdnRules()
{
  char command[MAX_COMMAND_STR_LEN];
  char devname_v4[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char devname_v6[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  struct in_addr subnet_ip;
  qcmap_ip4_addr_subnet_mask_v01 ipv4_addr;
  qcmap_ip6_addr_prefix_len_v01  ipv6_addr;
  struct in6_addr ipv6_prefix;
  char ipv4_addr_str[INET_ADDRSTRLEN] = {0};
  char ip6_addr_str[MAX_IPV6_PREFIX+1] = {0};
  boolean added_rules = false;
  int ret_v4 = QCMAP_CM_ERROR, ret_v6 = QCMAP_CM_ERROR;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;
  boolean isBridgeUp = FALSE;

  if (QcMapBackhaul == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaul is NULL", 0,0,0);
    return FALSE;
  }

  ret_v4 = GetDeviceName(QcMapMgr->qcmap_cm_handle, QCMAP_MSGR_IP_FAMILY_V4_V01, devname_v4, &qcmap_cm_error);
  ret_v6 = GetDeviceName(QcMapMgr->qcmap_cm_handle, QCMAP_MSGR_IP_FAMILY_V6_V01, devname_v6, &qcmap_cm_error);

  //get lan object and associate this profile handle only if it is a valid vlan.
  if (QcMapBackhaul->vlan_id != QCMAP_MSGR_VLAN_ID_NONE_V01 &&
      lan_mgr && (lan_obj = lan_mgr->GetLANBridge(QcMapBackhaul->vlan_id)))
  {
    isBridgeUp = lan_obj->IsBridgeInited();
  }

  LOG_MSG_INFO1("v4_vlan_pdn_enabled=%d, deviceName=%d, isBridgeUp=%d", v4_vlan_pdn_enabled,
                    ret_v4, isBridgeUp);

  LOG_MSG_INFO1("v6_vlan_pdn_enabled=%d, deviceName=%d, isBridgeUp=%d", v6_vlan_pdn_enabled,
                    ret_v6, isBridgeUp);

  /* Check backhaul state first, before VLAN, this is because during boot backhaul object is NULL */
  if ( ((!v4_vlan_pdn_enabled &&  ret_v4  != QCMAP_CM_ERROR) ||
        (!v6_vlan_pdn_enabled && ret_v6 != QCMAP_CM_ERROR)) &&
        isBridgeUp)
  {
    //create new tables/rules, only if v4 or v6 should be added and only if not already added
    //rule will already be added if v4 or v6 is already enabled
    if (!v4_vlan_pdn_enabled && !v6_vlan_pdn_enabled)
    {
      /* Clear the table if it already exists. Possible after reboot */
      snprintf(command, MAX_COMMAND_STR_LEN, "sed '/%d pdn_table_%d/d' %s",
                QcMapBackhaul->vlan_id, QcMapBackhaul->profileHandle, QCMAP_ROUTE_TABLE_PATH);
      qcmap_edit_file(command, strlen(command), QCMAP_ROUTE_TABLE_PATH, strlen(QCMAP_ROUTE_TABLE_PATH));

      snprintf(command, MAX_COMMAND_STR_LEN, "echo %d pdn_table_%d >> %s; sync",
                QcMapBackhaul->vlan_id, QcMapBackhaul->profileHandle, QCMAP_ROUTE_TABLE_PATH);
      ds_system_call(command, strlen(command));
    }

    if (!v4_vlan_pdn_enabled && ret_v4 == QCMAP_CM_SUCCESS)
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "ip rule add fwmark %d table pdn_table_%d",
              QcMapBackhaul->vlan_id, QcMapBackhaul->profileHandle);
      ds_system_call(command, strlen(command));

      QCMAP_Virtual_LAN::GetIPAddrForVLAN(QcMapBackhaul->vlan_id, &ipv4_addr, &ipv6_addr);
      if(!inet_ntop(AF_INET,(void *) &ipv4_addr.addr, ipv4_addr_str, INET_ADDRSTRLEN))
      {
        LOG_MSG_ERROR("Error converting IPv4 address",0,0,0);
        return false;
      }
      subnet_ip.s_addr = ipv4_addr.subnet_mask;

      snprintf(command, MAX_COMMAND_STR_LEN, "iptables -t mangle -A PREROUTING -s %s/%s -j MARK -i %s --set-mark %d",
               ipv4_addr_str, inet_ntoa(subnet_ip), GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id), QcMapBackhaul->vlan_id);
      ds_system_call(command, strlen(command));

      QcMapBackhaul->QcMapNatAlg->EnableNATonApps();
      AddWWANIPv4SIPServerInfo();
      v4_vlan_pdn_enabled = true;
      added_rules = true;
    }

    if (!v6_vlan_pdn_enabled && ret_v6 == QCMAP_CM_SUCCESS)
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 rule add fwmark %d table pdn_table_%d",
              QcMapBackhaul->vlan_id, QcMapBackhaul->profileHandle);
      ds_system_call(command, strlen(command));

      /* Get the global ipv6 address. */
      memset(&(QcMapBackhaul->ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));
      QcMapBackhaul->GetIPV6PrefixInfo(devname_v6, &QcMapBackhaul->ipv6_prefix_info);

      if (QcMapBackhaul->ipv6_prefix_info.prefix_info_valid)
      {
        memcpy(&ipv6_prefix,
               &((struct sockaddr_in6 *)&(QcMapBackhaul->ipv6_prefix_info.prefix_addr))->sin6_addr,
               sizeof(struct in6_addr));
        for (int i = 4; i < 8; i++)
        {
          ipv6_prefix.s6_addr16[i] = 0;
        }

        memset(ip6_addr_str, 0, MAX_IPV6_PREFIX + 1);
        inet_ntop(AF_INET6, &ipv6_prefix.s6_addr, ip6_addr_str, INET6_ADDRSTRLEN);

        snprintf(command, MAX_COMMAND_STR_LEN, "ip6tables -t mangle -A PREROUTING -s %s/%d -j MARK -i %s --set-mark %d",
               ip6_addr_str, QcMapBackhaul->ipv6_prefix_info.prefix_len,
               GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id), QcMapBackhaul->vlan_id);
        ds_system_call(command, strlen(command));

        QcMapBackhaul->EnableIPV6Forwarding();
        QcMapBackhaul->QcMapFirewall->EnableIPV6Firewall();
        v6_vlan_pdn_enabled = true;
        added_rules = true;
      }
    }
  }

  if (!added_rules)
  {
    LOG_MSG_ERROR( "Unable to enable VLAN-PDN rules", 0,0,0);
    return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION DisableVlanPdnRules
==========================================================================*/
/*!
@brief
  - Disables the rules between VLAN and PDN

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DisableVlanPdnRules(qcmap_msgr_wwan_call_type_v01 call_type)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  boolean removed_rules = false;
  struct in_addr subnet_ip;
  struct in6_addr ipv6_prefix;
  qcmap_ip4_addr_subnet_mask_v01 ipv4_addr;
  qcmap_ip6_addr_prefix_len_v01  ipv6_addr;
  char ipv4_addr_str[INET_ADDRSTRLEN] = {0};
  char ip6_addr_str[MAX_IPV6_PREFIX+1] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;
  boolean isBridgeUp = FALSE;

  //get lan object and associate this profile handle
  if(lan_mgr && (lan_obj = lan_mgr->GetLANBridge(QcMapBackhaul->vlan_id)))
  {
    isBridgeUp = lan_obj->IsBridgeInited();
  }

  LOG_MSG_INFO1("v4_vlan_pdn_enabled=%d, state_v4=%d, isBridgeUp=%d", v4_vlan_pdn_enabled, state, isBridgeUp);
  LOG_MSG_INFO1("v6_vlan_pdn_enabled=%d, state_v6=%d, isBridgeUp=%d", v6_vlan_pdn_enabled, ipv6_state, isBridgeUp);
  LOG_MSG_INFO1("call_type=%d", call_type, 0, 0);

  if (v4_vlan_pdn_enabled && isBridgeUp &&
      (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 ||
       call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01))
  {
    QCMAP_Virtual_LAN::GetIPAddrForVLAN(QcMapBackhaul->vlan_id, &ipv4_addr, &ipv6_addr);
    if(!inet_ntop(AF_INET,(void *) &ipv4_addr, ipv4_addr_str, INET_ADDRSTRLEN))
    {
      LOG_MSG_ERROR("Error converting IPv4 address",0,0,0);
      return false;
    }
    subnet_ip.s_addr = ipv4_addr.subnet_mask;

    snprintf(command, MAX_COMMAND_STR_LEN, "iptables -t mangle -D PREROUTING -s %s/%s -j MARK -i %s --set-mark %d",
             ipv4_addr_str, inet_ntoa(subnet_ip), GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id), QcMapBackhaul->vlan_id);
    ds_system_call(command, strlen(command));

    QcMapBackhaul->QcMapNatAlg->DisableNATonApps();
    QcMapBackhaul->QcMapFirewall->CleanIPv4MangleTable();

    snprintf(command, MAX_COMMAND_STR_LEN, "ip rule del fwmark %d table pdn_table_%d",
              QcMapBackhaul->vlan_id, QcMapBackhaul->profileHandle);
    ds_system_call(command, strlen(command));

    v4_vlan_pdn_enabled = false;
    removed_rules = true;
  }

  if (v6_vlan_pdn_enabled && QcMapBackhaul->ipv6_prefix_info.prefix_info_valid && isBridgeUp &&
      (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 ||
       call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01))
  {
    memcpy(&ipv6_prefix, &((struct sockaddr_in6 *)&(QcMapBackhaul->ipv6_prefix_info.prefix_addr))->sin6_addr, sizeof(struct in6_addr));
    for (int i = 4; i < 8; i++)
    {
      ipv6_prefix.s6_addr16[i] = 0;
    }

    memset(ip6_addr_str, 0, MAX_IPV6_PREFIX + 1);
    inet_ntop(AF_INET6, &ipv6_prefix.s6_addr, ip6_addr_str, INET6_ADDRSTRLEN);

    v6_vlan_pdn_enabled = false;
    removed_rules = true;

    if (strlen(ipv6_default_table_rule_suffix))
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del %s", ipv6_default_table_rule_suffix);
      ds_system_call(command, strlen(command));
    }

    if (strlen(ipv6_default_oif_rule_suffix))
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 rule del %s", ipv6_default_oif_rule_suffix);
      ds_system_call(command, strlen(command));
    }

    memset(ipv6_default_table_rule_suffix, 0, MAX_DEFAULT_TABLE_LEN);
    memset(ipv6_default_oif_rule_suffix, 0, MAX_DEFAULT_TABLE_LEN);

    snprintf(command, MAX_COMMAND_STR_LEN, "ip6tables -t mangle -D PREROUTING -s %s/%d -j MARK -i %s --set-mark %d",
               ip6_addr_str, QcMapBackhaul->ipv6_prefix_info.prefix_len,
               GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id), QcMapBackhaul->vlan_id);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 rule del fwmark %d table pdn_table_%d",
              QcMapBackhaul->vlan_id, QcMapBackhaul->profileHandle);
    ds_system_call(command, strlen(command));

    QcMapBackhaul->DisableIPV6Forwarding(true);
    QcMapBackhaul->QcMapFirewall->CleanIPv6MangleTable();
  }

  if (!v4_vlan_pdn_enabled && !v6_vlan_pdn_enabled)
  {
    //remove previous rules and tables
    snprintf(command, MAX_COMMAND_STR_LEN, "sed '/%d pdn_table_%d/d' %s",
              QcMapBackhaul->vlan_id, QcMapBackhaul->profileHandle, QCMAP_ROUTE_TABLE_PATH);
    qcmap_edit_file(command, strlen(command), QCMAP_ROUTE_TABLE_PATH, strlen(QCMAP_ROUTE_TABLE_PATH));
  }

  return removed_rules;
}


/*===========================================================================
FUNCTION AddDNSRoutesForPDN()

DESCRIPTION
  This Function adds dns IPv4 and IPv6 routes for PDN's.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::AddDNSRoutesForPDN(qcmap_msgr_wwan_call_type_v01 call_type)
{
  LOG_MSG_INFO1("call_type=%d, state=%d, ipv6_state=%d", call_type, state, ipv6_state);
  if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 && state == QCMAP_CM_WAN_CONNECTED)
  {
    AddDNSv4RoutesForPDN();
  }
  if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 && ipv6_state == QCMAP_CM_V6_WAN_CONNECTED)
  {
    AddDNSv6RoutesForPDN();
  }
}


/*===========================================================================
FUNCTION AddDNSv4RoutesForPDN()

DESCRIPTION
  This Function adds dns IPv4 routes for PDN's.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::AddDNSv4RoutesForPDN()
{
  int qcmap_cm_error;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2] = {0};
  in_addr_t public_ip = 0, sec_dns_ip = 0;
  in_addr_t default_gw_addr = 0, net_mask = 0, public_ip_wwan = 0;
  uint32 pri_dns_ip = 0;
  in_addr public_ip_addr, pri_dns_addr, sec_dns_addr;
  int metric;
  char  public_ipv4_addr_str[INET_ADDRSTRLEN] = {0};
  char command[MAX_COMMAND_STR_LEN] ={0} ;

  if (state == QCMAP_CM_WAN_CONNECTED)
  {
    if (GetDeviceName(QcMapMgr->qcmap_cm_handle,
                      QCMAP_MSGR_IP_FAMILY_V4_V01,
                      devname, &qcmap_cm_error) == QCMAP_CM_SUCCESS )
    {
      strlcpy(dsi_device_name, devname, sizeof(devname));
      if (GetIPv4NetworkConfig(&public_ip, &pri_dns_ip,
                               &sec_dns_ip, &default_gw_addr,
                               &qmi_err_num) == QCMAP_CM_SUCCESS)
      {
        memset(pri_dns_ipv4_addr, 0, INET_ADDRSTRLEN);
        memset(sec_dns_ipv4_addr, 0, INET_ADDRSTRLEN);

        if (public_ip != 0)
        {
          public_ip_addr.s_addr = public_ip;
          strlcpy(public_ipv4_addr_str, inet_ntoa(public_ip_addr), INET_ADDRSTRLEN);
        }

        if (pri_dns_ip != 0)
        {
          pri_dns_addr.s_addr = pri_dns_ip;
          strlcpy(pri_dns_ipv4_addr, inet_ntoa(pri_dns_addr), INET_ADDRSTRLEN);
        }

        if (sec_dns_ip != 0)
        {
          sec_dns_addr.s_addr = sec_dns_ip;
          strlcpy(sec_dns_ipv4_addr, inet_ntoa(sec_dns_addr), INET_ADDRSTRLEN);
        }
      }

      if (QcMapBackhaul->profileHandle == QCMAP_Backhaul::GetDefaultProfileHandle())
      {
        metric = DNS_METRIC_FOR_DEFAULT_PDN;
      }
      else
      {
        metric = QcMapBackhaul->profileHandle * DNS_METRIC_NUMBER;
      }

      /* 1. Always del ip route rule, this'll make sure there is always one
       *     ip route entry for dns ip. If multiple client try to connect, then
       *     this will make sure we'll have only ip route entry.
       * 2. Since DNS IP could be same across PDN's, we'll use metric.
       *       metric = 10 will be used for default PDN.
       *       metric = 3gpp_profile_index * 100 will be used for secondary PDN.
       *     Eg: if profile_index = 3 and if it's default pdn, then metric used will be 10
       *         if profile_index = 3 and if it's secondary pdn, then metric used will be 300.
       */

      /* Primary DNS */
      if (pri_dns_ip != 0)
      {
        /* Always delete ip route rule, before adding. */
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip route del %s dev %s ", pri_dns_ipv4_addr, dsi_device_name);
        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));

        /* Add Pri DNS ip route rule*/
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip route add %s dev %s metric %d src %s",
                 pri_dns_ipv4_addr, dsi_device_name,
                 metric, public_ipv4_addr_str);

        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));
      }
      else
      {
        LOG_MSG_ERROR("route rule: pri_dns_ip is NULL, nothing to add.", 0,0,0);
      }

      /* Secondary DNS */
      if (sec_dns_ip != 0)
      {
        /* Always delete ip route rule, before adding. */
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip route del %s dev %s", sec_dns_ipv4_addr, dsi_device_name);
        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));

        /* Add Sec DNS ip route rule*/
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip route add %s dev %s metric %d src %s ",
                 sec_dns_ipv4_addr, dsi_device_name,
                 metric, public_ipv4_addr_str);

        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));
      }
      else
      {
        LOG_MSG_ERROR("route rule: sec_dns_ip is NULL, nothing to add.", 0,0,0);
      }
    }
  }
}


/*===========================================================================
FUNCTION AddDNSv6RoutesForPDN()

DESCRIPTION
  This Function adds dns IPv6 routes for PDN's.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::AddDNSv6RoutesForPDN()
{
  int qcmap_cm_error;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  uint8_t  public_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};     /* IPv6 addr assigned to WWAN */
  uint8_t  pri_dns_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};    /* Primary IPV6 DNS IP address   */
  uint8_t  sec_dns_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};    /* Secondary IPV6 DNS IP address   */
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2] = {0};
  struct sockaddr_in6 public_ip_addr, pdns6, sdns6;
  int metric;
  char public_ipv6_addr_str[INET6_ADDRSTRLEN] = {0};
  char buffer[INET6_ADDRSTRLEN] = {0};
  char command[MAX_COMMAND_STR_LEN] ={0} ;

  if (ipv6_state == QCMAP_CM_V6_WAN_CONNECTED)
  {
    if (GetDeviceName(QcMapMgr->qcmap_cm_handle,
                      QCMAP_MSGR_IP_FAMILY_V6_V01,
                      devname, &qcmap_cm_error) == QCMAP_CM_SUCCESS )
    {
      strlcpy(ipv6_dsi_device_name, devname, sizeof(devname));
      if (GetIPv6NetworkConfig(public_ip, pri_dns_ip,
                               sec_dns_ip, &qmi_err_num) == QCMAP_CM_SUCCESS)
      {
        memset(buffer, 0, INET6_ADDRSTRLEN);
        if (sizeof(public_ip_addr.sin6_addr.s6_addr) == IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8) &&
            sizeof(public_ip) == IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8))
        {
          memcpy(public_ip_addr.sin6_addr.s6_addr, public_ip,
                  IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8));
          inet_ntop(AF_INET6, &public_ip_addr.sin6_addr, buffer, sizeof (buffer));
        }
        if (strlen(buffer) && strncmp (buffer, "::", sizeof("::")))
        {
          strlcpy(public_ipv6_addr_str, buffer, INET6_ADDRSTRLEN);
        }

        memset(buffer, 0, INET6_ADDRSTRLEN);
        if (sizeof(pdns6.sin6_addr.s6_addr) == IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8) &&
            sizeof(pri_dns_ip) == IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8))
        {
          memcpy(pdns6.sin6_addr.s6_addr, pri_dns_ip,
                  IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8));
          inet_ntop(AF_INET6, &pdns6.sin6_addr, buffer, sizeof (buffer));
        }
        if (strlen(buffer) && strncmp (buffer, "::", sizeof("::")))
        {
          strlcpy(pri_dns_ipv6_addr, buffer, INET6_ADDRSTRLEN);
        }
        else
        {
          memset(pri_dns_ipv6_addr, 0, INET6_ADDRSTRLEN);
          LOG_MSG_ERROR("route rule: pri_dns_ip is NULL, nothing to add.", 0,0,0);
        }

        memset(buffer, 0, INET6_ADDRSTRLEN);
        if (sizeof(sdns6.sin6_addr.s6_addr) == IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8) &&
            sizeof(sec_dns_ip) == IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8))
        {
          memcpy(sdns6.sin6_addr.s6_addr, sec_dns_ip,
                  IPV6_ADDR_SIZE_IN_BYTES * sizeof(uint8));
          inet_ntop(AF_INET6, &sdns6.sin6_addr, buffer, sizeof(buffer));
        }
        if (strlen(buffer) && strncmp (buffer, "::", sizeof("::")))
        {
          strlcpy(sec_dns_ipv6_addr, buffer, INET6_ADDRSTRLEN);
        }
        else
        {
          memset(sec_dns_ipv6_addr, 0, INET6_ADDRSTRLEN);
          LOG_MSG_ERROR("route rule: sec_dns_ip is NULL, nothing to add.", 0,0,0);
        }
      }

      if (this->QcMapBackhaul->profileHandle == QCMAP_Backhaul::GetDefaultProfileHandle())
      {
        metric = DNS_METRIC_FOR_DEFAULT_PDN;
      }
      else
      {
        metric = QcMapBackhaul->profileHandle * DNS_METRIC_NUMBER;
      }

      /* 1. Always del ip route rule, this'll make sure there is always one
       *     ip route entry for dns ip. If multiple client try to connect, then
       *     this will make sure we'll have only ip route entry.
       * 2. Since DNS IP could be same across PDN's, we'll use metric.
       *       metric = 10 will be used for default PDN.
       *       metric = 3gpp_profile_index * 100 will be used for secondary PDN.
       *     Eg: if profile_index = 3 and if it's default pdn, then metric used will be 10
       *         if profile_index = 3 and if it's secondary pdn, then metric used will be 300.
       */

      /* Primary DNS */
      if (strlen(pri_dns_ipv6_addr))
      {
        /* Always delete ip route rule, before adding. */
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del %s dev %s", pri_dns_ipv6_addr, ipv6_dsi_device_name);
        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));

        /* Add Pri DNS ipv6 route rule*/
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route add %s dev %s metric %d src %s",
                 pri_dns_ipv6_addr, ipv6_dsi_device_name,
                 metric, public_ipv6_addr_str);
        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));
      }

      /* Secondary DNS */
      if (strlen(sec_dns_ipv6_addr))
      {
        /* Always delete ip route rule, before adding. */
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del %s dev %s", sec_dns_ipv6_addr, ipv6_dsi_device_name);
        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));

        /* Add Sec DNS ipv6 route rule*/
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route add %s dev %s metric %d src %s",
                 sec_dns_ipv6_addr, ipv6_dsi_device_name,
                 metric, public_ipv6_addr_str);
        LOG_MSG_INFO1("%s", command, 0,0);
        ds_system_call(command, strlen(command));
      }
    }
  }
}



/*===========================================================================
FUNCTION DeleteDNSRoutesForPDN()

DESCRIPTION
  This Function deletes DNS routes for PDN

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::DeleteDNSRoutesForPDN(qcmap_msgr_wwan_call_type_v01 call_type)
{
  char command[MAX_COMMAND_STR_LEN] ={0} ;

  /* IPv4 */
  if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 && state != QCMAP_CM_WAN_CONNECTED)
  {
    if (strlen(pri_dns_ipv4_addr))
    {
      /* Del Pri DNS ip route rule */
      snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip route del %s dev %s", pri_dns_ipv4_addr, dsi_device_name);
      LOG_MSG_INFO1("%s", command, 0,0);
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR("route rule: ip is NULL, nothing to delete.", 0,0,0);
    }
    if (strlen(sec_dns_ipv4_addr))
    {
      /* Del Sec DNS ip route rule */
      snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip route del %s dev %s", sec_dns_ipv4_addr, dsi_device_name);
      LOG_MSG_INFO1("%s", command, 0,0);
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR("route rule: ip is NULL, nothing to delete.", 0,0,0);
    }
  }

  /* IPv6 */
  if (call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 && ipv6_state != QCMAP_CM_V6_WAN_CONNECTED)
  {
    if (strlen(pri_dns_ipv6_addr))
    {
      /* Del Pri DNS ip route rule */
      snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del %s dev %s", pri_dns_ipv6_addr, ipv6_dsi_device_name);
      LOG_MSG_INFO1("%s", command, 0,0);
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR("route rule: ip is NULL, nothing to delete.", 0,0,0);
    }
    if (strlen(sec_dns_ipv6_addr))
    {
      /* Del Sec DNS ip route rule */
      snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del %s dev %s", sec_dns_ipv6_addr, ipv6_dsi_device_name);
      LOG_MSG_INFO1("%s", command, 0,0);
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR("route rule: ip is NULL, nothing to delete.", 0,0,0);
    }
  }
}


/*===========================================================================
FUNCTION AddIPv6RouteForSecondaryPDN()

DESCRIPTION
  This function add IPv6 default routes for secondary PDN.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::AddIPv6RouteForSecondaryPDN()
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2] = {0};
  int qcmap_cm_error;
  char command[MAX_COMMAND_STR_LEN] ={0} ;
  uint32_t pdn_table_id = PDN_TABLE_ID_START;

  /*
   * echo 8 pdn_table_N >> /data/iproute2/rt_tables, where N is profile_handle"
   * ip -6 route add table 8 default dev rmnet_data1
   * ip -6 rule add oif rmnet_data1 table 8
   */

  LOG_MSG_INFO1("Enter",0,0,0);

  if (QcMapBackhaul == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaul is NULL", 0,0,0);
    return;
  }

  if (GetDeviceName(QcMapMgr->qcmap_cm_handle,
                      QCMAP_MSGR_IP_FAMILY_V6_V01,
                      devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("GetDeviceName() failed", 0,0,0);
    return;
  }

  pdn_table_id += QcMapBackhaul->profileHandle;
  /* Clear the table if it already exists. Possible after reboot */
  snprintf(command, MAX_COMMAND_STR_LEN, "sed '/%d pdn_table_%d/d' %s",
            pdn_table_id, pdn_table_id, QCMAP_ROUTE_TABLE_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_ROUTE_TABLE_PATH, strlen(QCMAP_ROUTE_TABLE_PATH));

  snprintf(command, MAX_COMMAND_STR_LEN, "echo %d pdn_table_%d >> %s; sync",
            pdn_table_id, pdn_table_id, QCMAP_ROUTE_TABLE_PATH);
  ds_system_call(command, strlen(command));

  snprintf(ipv6_default_table_rule_suffix, MAX_DEFAULT_TABLE_LEN, " table pdn_table_%d default dev %s",
          pdn_table_id, devname);
  snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route add %s", ipv6_default_table_rule_suffix);
  ds_system_call(command, strlen(command));

  snprintf(ipv6_default_oif_rule_suffix, MAX_DEFAULT_TABLE_LEN, " oif %s table pdn_table_%d",
          devname, pdn_table_id);
  snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 rule add %s", ipv6_default_oif_rule_suffix);
  ds_system_call(command, strlen(command));
}


/*===========================================================================
FUNCTION DeleteIPv6RouteForSecondaryPDN()

DESCRIPTION
  This function deletes IPv6 default routes for secondary PDN.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::DeleteIPv6RouteForSecondaryPDN()
{
  char command[MAX_COMMAND_STR_LEN] ={0} ;
  uint32_t pdn_table_id = PDN_TABLE_ID_START;

  LOG_MSG_INFO1("Enter",0,0,0);

  if (strlen(ipv6_default_table_rule_suffix) == 0 ||
      strlen(ipv6_default_oif_rule_suffix) == 0)
  {
    LOG_MSG_ERROR("default rule is NULL", 0,0,0);
    return;
  }

  /*
   * ip -6 route del table 8 default dev rmnet_data1
   * ip -6 rule del oif rmnet_data1 table 8
   * sed -i '/8 pdn_table_N/d'  /data/iproute2/rt_tables, where N is profile_handle
   */
  snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del %s", ipv6_default_table_rule_suffix);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 rule del %s", ipv6_default_oif_rule_suffix);
  ds_system_call(command, strlen(command));

  pdn_table_id += QcMapBackhaul->profileHandle;
  snprintf(command, MAX_COMMAND_STR_LEN, "sed '/%d pdn_table_%d/d' %s",
              pdn_table_id, pdn_table_id, QCMAP_ROUTE_TABLE_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_ROUTE_TABLE_PATH, strlen(QCMAP_ROUTE_TABLE_PATH));

  memset(ipv6_default_table_rule_suffix, 0, MAX_DEFAULT_TABLE_LEN);
  memset(ipv6_default_oif_rule_suffix, 0, MAX_DEFAULT_TABLE_LEN);
}


/*===========================================================================
FUNCTION IsAnyPDNConnected()

DESCRIPTION
  This function returns true if any of PDN (v4-only, v6-only or v4v6) is connected.

DEPENDENCIES
  None.

RETURN VALUE
  True/False

SIDE EFFECTS

===========================================================================*/
boolean QCMAP_Backhaul_WWAN::IsAnyPDNConnected(qcmap_msgr_ip_family_enum_v01 v4orv6)
{
  boolean ret_val = FALSE;
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    if (pBackhaulWWAN == NULL)
    {
      LOG_MSG_ERROR("OOPS! WWAN Object is NULL", 0, 0, 0);
      continue;
    }

    /* If WWAN Object is in connecting, connected, disconnecting state then return true */
    /* Query for V4 Only */
    if (v4orv6 == QCMAP_MSGR_IP_FAMILY_V4_V01 && pBackhaulWWAN->GetState() <= QCMAP_CM_ENABLE)
    {
      continue;
    }
    /* Query for V6 Only */
    else if (v4orv6 == QCMAP_MSGR_IP_FAMILY_V6_V01 && pBackhaulWWAN->GetIPv6State() <= QCMAP_CM_V6_ENABLE)
    {
      continue;
    }
    /* Query for either V4 or V6 */
    if ( (v4orv6 == QCMAP_MSGR_IP_FAMILY_V4V6_V01) &&
          (pBackhaulWWAN->GetState() <= QCMAP_CM_ENABLE ||
          pBackhaulWWAN->GetIPv6State() <= QCMAP_CM_V6_ENABLE) )
    {
      continue;
    }

    ret_val = TRUE;
    break;
  }

  return ret_val;
}


/*===========================================================================
FUNCTION UpdateProfileParam()

DESCRIPTION
  This Function updates profile params.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns true.
  On error, return false
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
boolean QCMAP_Backhaul_WWAN::UpdateProfileParam
(
  qcmap_msgr_update_profile_enum_v01 update_req,
  qcmap_msgr_net_policy_info_v01     wwan_policy,
  qmi_error_type_v01                *qmi_err_num
)
{
  pugi::xml_document       xml_file;
  const char              *tag_ptr;
  pugi::xml_node           root, searchNode;
  boolean                  ret_val = true;
  char                     val[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  LOAD_XML_FILE(xml_file, QcMapMgr);

  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find root XML corresponding to PDN",0,0,0);
    return false;
  }

  searchNode = root.child(MobileAPWanCfg_TAG);
  switch(update_req)
  {
    case QCMAP_MSGR_UPDATE_TECH_TYPE_V01:
      tag_ptr = TECH_TAG;
      if (wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_ANY_V01)
         searchNode.child(tag_ptr).text() = TECH_ANY_TAG;
      else if (wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_3GPP_V01)
         searchNode.child(tag_ptr).text() = TECH_3GPP_TAG;
      else if (wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_3GPP2_V01)
         searchNode.child(tag_ptr).text() = TECH_3GPP2_TAG;
      else
      {
        LOG_MSG_ERROR("Invalid tech pref, arg=%d", wwan_policy.tech_pref,0,0);
        ret_val = false;
        *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      }

      /* Update config in WWAN Object */
      if (ret_val == true)
      {
        wwan_cfg.tech = wwan_policy.tech_pref;
      }
      break;

    case QCMAP_MSGR_UPDATE_V4_3GPP_PROFILE_INDEX_V01:
      tag_ptr = V4_UMTS_PROFILE_INDEX_TAG;
      IS_PROFILE_VALID(wwan_policy.v4_profile_id_3gpp, ret_val, *qmi_err_num);
      if (ret_val == true)
      {
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp);
        searchNode.child(tag_ptr).text() = val;

        /* Update config in WWAN Object */
        wwan_cfg.profile_id.v4.umts_profile_index = wwan_policy.v4_profile_id_3gpp;
      }
      break;

    case QCMAP_MSGR_UPDATE_V4_3GPP2_PROFILE_INDEX_V01:
      tag_ptr = V4_CDMA_PROFILE_INDEX_TAG;
      //IS_PROFILE_VALID(wwan_policy.v4_profile_id_3gpp2, ret_val, *qmi_err_num); //Check not required for 3gpp2
      if (ret_val == true)
      {
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp2);
        searchNode.child(tag_ptr).text() = val;

        /* Update config in WWAN Object */
        wwan_cfg.profile_id.v4.cdma_profile_index = wwan_policy.v4_profile_id_3gpp2;
      }
      break;

    case QCMAP_MSGR_UPDATE_V6_3GPP_PROFILE_INDEX_V01:
      tag_ptr = V6_UMTS_PROFILE_INDEX_TAG;
      IS_PROFILE_VALID(wwan_policy.v6_profile_id_3gpp, ret_val, *qmi_err_num);
      if (ret_val == true)
      {
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp);
        searchNode.child(tag_ptr).text() = val;

        /* Update config in WWAN Object */
        wwan_cfg.profile_id.v6.umts_profile_index = wwan_policy.v6_profile_id_3gpp;
      }
      break;

    case QCMAP_MSGR_UPDATE_V6_3GPP2_PROFILE_INDEX_V01:
      tag_ptr = V6_CDMA_PROFILE_INDEX_TAG;
      //IS_PROFILE_VALID(wwan_policy.v6_profile_id_3gpp2, ret_val, *qmi_err_num); //Check not required for 3gpp2
      if (ret_val == true)
      {
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp2);
        searchNode.child(tag_ptr).text() = val;

        /* Update config in WWAN Object */
        wwan_cfg.profile_id.v6.cdma_profile_index = wwan_policy.v6_profile_id_3gpp2;
      }
      break;

    case QCMAP_MSGR_UPDATE_ALL_PROFILE_INDEX_V01:
      /* Check if both 3gpp v4/v6 profile-id are valid
       *  Bitwise-AND both profiles, so that one check is enough.
       */
      IS_PROFILE_VALID( (wwan_policy.v4_profile_id_3gpp & wwan_policy.v6_profile_id_3gpp),
                        ret_val, *qmi_err_num);

      // If both 3gpp v4/v6 profile-id are valid then go ahead and modify profile params.
      if (ret_val == true)
      {
        //3gpp - V4
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp);
        searchNode.child(V4_UMTS_PROFILE_INDEX_TAG).text() = val;

        //3gpp2 - V4
        snprintf(val, sizeof(val), "%d", wwan_policy.v4_profile_id_3gpp2);
        searchNode.child(V4_CDMA_PROFILE_INDEX_TAG).text() = val;

        //3gpp - V6
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp);
        searchNode.child(V6_UMTS_PROFILE_INDEX_TAG).text() = val;

        //3gpp2 - V6
        snprintf(val, sizeof(val), "%d", wwan_policy.v6_profile_id_3gpp2);
        searchNode.child(V6_CDMA_PROFILE_INDEX_TAG).text() = val;

        /* Update config in WWAN Object */
        wwan_cfg.profile_id.v4.umts_profile_index = wwan_policy.v4_profile_id_3gpp;
        wwan_cfg.profile_id.v4.cdma_profile_index = wwan_policy.v4_profile_id_3gpp2;
        wwan_cfg.profile_id.v6.umts_profile_index = wwan_policy.v6_profile_id_3gpp;
        wwan_cfg.profile_id.v6.cdma_profile_index = wwan_policy.v6_profile_id_3gpp2;
      }
      break;

    default:
      LOG_MSG_ERROR("Invalid Update req=%d", update_req, 0, 0);
      ret_val = false;
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      break;
  }

  if (ret_val == true)
  {
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  return ret_val;
}

/*===========================================================================
FUNCTION SetMobileAPConfigInXML()

DESCRIPTION
  This Function updates MobileAP parameters in XML file.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns true.
  On error, return false
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::SetMobileAPConfigInXML
(
  pugi::xml_node          *wwan_config_node,
  wwan_config_params       wwan_config
)
{
  char          val[MAX_STRING_LENGTH] = {0};
  boolean       ret_val = false;

  if (wwan_config_node == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! wwan_config_node is NULL!", 0,0,0);
    return;
  }

  //Profile
  snprintf(val, sizeof(val), "%d", wwan_config.profileHandle);
  wwan_config_node->child(ProfileHandle_TAG).text() = val;

  //AutoConnect
  snprintf(val, sizeof(val), "%d", wwan_config.auto_connect);
  wwan_config_node->child(AutoConnect_TAG).text() = val;

  //Roaming
  snprintf(val, sizeof(val), "%d", wwan_config.roaming);
  wwan_config_node->child(Roaming_TAG).text() = val;

  //Tech
  if (wwan_config.wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_ANY_V01)
    wwan_config_node->child(TECH_TAG).text() = TECH_ANY_TAG;
  else if (wwan_config.wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_3GPP_V01)
    wwan_config_node->child((TECH_TAG)).text() = TECH_3GPP_TAG;
  else if (wwan_config.wwan_policy.tech_pref == QCMAP_MSGR_MASK_TECH_PREF_3GPP2_V01)
    wwan_config_node->child((TECH_TAG)).text() = TECH_3GPP2_TAG;

  //3gpp - V4
  snprintf(val, sizeof(val), "%d", wwan_config.wwan_policy.v4_profile_id_3gpp);
  wwan_config_node->child(V4_UMTS_PROFILE_INDEX_TAG).text() = val;

  //3gpp2 - V4
  snprintf(val, sizeof(val), "%d", wwan_config.wwan_policy.v4_profile_id_3gpp2);
  wwan_config_node->child(V4_CDMA_PROFILE_INDEX_TAG).text() = val;

  //3gpp - V6
  snprintf(val, sizeof(val), "%d", wwan_config.wwan_policy.v6_profile_id_3gpp);
  wwan_config_node->child(V6_UMTS_PROFILE_INDEX_TAG).text() = val;

  //3gpp2 - V6
  snprintf(val, sizeof(val), "%d", wwan_config.wwan_policy.v6_profile_id_3gpp2);
  wwan_config_node->child(V6_CDMA_PROFILE_INDEX_TAG).text() = val;
}


/*===========================================================================
FUNCTION SwapDefaultProfile()

DESCRIPTION
  This Function swap's concurrent WWAN to default WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns true.
  On error, return false
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
boolean QCMAP_Backhaul_WWAN::SwapConcurrentConfigToDefault
(
  qmi_error_type_v01 *qmi_err_num
)
{
  pugi::xml_document       xml_file;
  pugi::xml_node           root, defaultConfigNode, concurrentConfigNode;
  pugi::xml_node           defaultNatCfgNode, concurrentNatCfgNode;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN     *QcMapBackhaul_Default_Obj = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  wwan_config_params       default_wwan_config_params, concurrent_wwan_config_params;

  if (state > QCMAP_CM_ENABLE || ipv6_state > QCMAP_CM_V6_ENABLE)
  {
    LOG_MSG_ERROR("Current profile (%p) is in use, state=%d, ipv6_state=%d",
                    QcMapBackhaul->profileHandle, state, ipv6_state);
    *qmi_err_num = QMI_ERR_DEVICE_IN_USE_V01;
    return false;
  }
  if(QcMapBackhaul_Default_Obj == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaul_Default_Obj is NULL ", 0, 0, 0);
    *qmi_err_num =  QMI_ERR_INTERNAL_V01;
    return false;
  }

  if (QcMapBackhaul_Default_Obj->GetState() > QCMAP_CM_ENABLE ||
        QcMapBackhaul_Default_Obj->GetIPv6State() > QCMAP_CM_V6_ENABLE)
  {
    LOG_MSG_ERROR("Default profile (%p) is in use, state=%d, ipv6_state=%d",
                  QCMAP_Backhaul::GetDefaultProfileHandle(),
                  QcMapBackhaul_Default_Obj->GetState(),
                  QcMapBackhaul_Default_Obj->GetIPv6State());
    *qmi_err_num = QMI_ERR_DEVICE_IN_USE_V01;
    return false;
  }

  /* Check if profile is already default */
  if (QcMapBackhaul_Default_Obj == this)
  {
    LOG_MSG_ERROR("Current profile(%p) is already default", QcMapBackhaul->profileHandle, 0,0);
    *qmi_err_num = QMI_ERR_INVALID_PROFILE_V01;
    return false;
  }

  /* Copy WWAN params like profile handle, policy info, auto-connect, roaming
     from WWAN Object.
     Do this for Default-Backhaul Object and Concurrent Object.
   */
  COPY_WWAN_CONFIG_PARAM(QcMapBackhaul_Default_Obj, default_wwan_config_params);
  COPY_WWAN_CONFIG_PARAM(this, concurrent_wwan_config_params);

  LOAD_XML_FILE(xml_file, QcMapMgr);

  /* Find default config section */
  defaultConfigNode = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
  defaultNatCfgNode = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  /* Find concurrent config node */
  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Couldn't find PDN node for profile_handle=%p", QcMapBackhaul->profileHandle, 0, 0);
    return false;
  }
  concurrentConfigNode = root.child(MobileAPWanCfg_TAG);
  concurrentNatCfgNode = root.child(MobileAPNatCfg_TAG);

  /* Copy default params to concurrent config */
  QCMAP_Backhaul_WWAN::SetMobileAPConfigInXML(&concurrentConfigNode, default_wwan_config_params);

  /* Copy concurrent config params to default config */
  QCMAP_Backhaul_WWAN::SetMobileAPConfigInXML(&defaultConfigNode, concurrent_wwan_config_params);

  /* Swap default NAT/Firewall config with concurrent config.*/
  QcMapBackhaul->SwapNatAlgObjectWithDefault();
  QcMapBackhaul->QcMapFirewall->SwapFirewallWithDefault();
  QcMapBackhaul->SwapFirewallObjectWithDefault();

  /* Flush changes to XML file */
  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  /* Swap Vlan Params with concurrent config */
  QcMapBackhaul->SwapVlanIdWithDefault();

  /* Update Default profile handle */
  QCMAP_Backhaul::defaultProfileHandle = QcMapBackhaul->profileHandle;

  return true;
}

/*===========================================================================
FUNCTION FindFreeProfileHandle()

DESCRIPTION
  This Function finds a free Profile Handle, if any.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns free profile handle that can be used.
  On error, returns 0.

SIDE EFFECTS

===========================================================================*/
profile_handle_type_v01 QCMAP_Backhaul_WWAN::FindFreeProfileHandle()
{
  profile_handle_type_v01 profile = 1;

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    if (profile != GET_CURRENT_PROFILE_HANDLE)
      break;

    NEXT_ELEMENT_IN_HASH_MAP;
    profile++;
  }

  return profile;
}

#define COMPARE_WWAN_POLICY_INDEX(policy_index1, policy_index2) \
            (policy_index1 != 0 && policy_index1 == policy_index2)

/*===========================================================================
FUNCTION IsDuplicateProfile()

DESCRIPTION
  This Function finds if a profile already exists with any of same policy.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns true - Policy already exist.
  return false - if policy doesn't exists.

SIDE EFFECTS

===========================================================================*/
boolean QCMAP_Backhaul_WWAN::IsDuplicateProfile
(
  qcmap_msgr_net_policy_info_v01 wwan_policy
)
{
  QCMAP_Backhaul_WWAN *QcMapBackhaulWWAN;
  boolean ret_val = false;

  /* 1. Check if 3GPP profile_index are 0,
         if yes, then return failure.
   */
  if (wwan_policy.v4_profile_id_3gpp == 0 ||
      wwan_policy.v6_profile_id_3gpp == 0)
  {
    LOG_MSG_ERROR("Invalid profile, policy index are 0", 0,0,0);
    ret_val = true;
    return ret_val;
  }

  /* 2. Compare input wwan policy index against already existing profiles
     2a) Ignore if input wwan policy is zero
     2b) Compare a non-zero input wwan policy against existing profiles.
         If it matches then return failure.
   */
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    QcMapBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    if ( COMPARE_WWAN_POLICY_INDEX (wwan_policy.v4_profile_id_3gpp,
            QcMapBackhaulWWAN->wwan_cfg.profile_id.v4.umts_profile_index) )
    {
      ret_val = true;
      break;
    }
    if ( COMPARE_WWAN_POLICY_INDEX(wwan_policy.v4_profile_id_3gpp2,
            QcMapBackhaulWWAN->wwan_cfg.profile_id.v4.cdma_profile_index) )
    {
      ret_val = true;
      break;
    }
    if ( COMPARE_WWAN_POLICY_INDEX(wwan_policy.v6_profile_id_3gpp,
            QcMapBackhaulWWAN->wwan_cfg.profile_id.v6.umts_profile_index) )
    {
      ret_val = true;
      break;
    }
    if ( COMPARE_WWAN_POLICY_INDEX(wwan_policy.v6_profile_id_3gpp2,
            QcMapBackhaulWWAN->wwan_cfg.profile_id.v6.cdma_profile_index) )
    {
      ret_val = true;
      break;
    }
  }

  if (ret_val == true)
  {
    LOG_MSG_ERROR("Duplicate profile (%p) found",
                   QcMapBackhaulWWAN->QcMapBackhaul->profileHandle,0,0);
  }
  LOG_MSG_INFO1("Is Profile found=%d", ret_val, 0,0);
  return ret_val;
}


/*===========================================================================
FUNCTION InsertQMIClient()

DESCRIPTION
  Add QMI Client reference to qmi_client_ref map for a given WWAN Object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::InsertQMIClient
(
  uint32                   qmi_client,
  qmi_client_ref_reg_event reg_event,
  int                      reg_event_value
)
{
  qmi_client_ref_type *reg_client;
  std::map<uint32, qmi_client_ref_type*>::iterator it;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());
  if (qmi_client_ref == NULL)
  {
    LOG_MSG_ERROR("OOPS! qmi_client_ref is NULL", 0,0,0);
    return;
  }

  it = qmi_client_ref->find(qmi_client);
  if (it != qmi_client_ref->end())
  {
    LOG_MSG_INFO1("QMI_Client found = %p", qmi_client, 0, 0);
    reg_client = it->second;
  }
  else
  {
    reg_client = (qmi_client_ref_type *)calloc(1, sizeof(qmi_client_ref_type));
  }

  if (reg_client == NULL)
  {
    LOG_MSG_ERROR("No memory left!", 0, 0, 0);
    return;
  }

  switch (reg_event)
  {
    case QMI_CLIENT_REG_WWAN_BRING_UP_DOWN_EVENT:
      reg_client->wwan_op_ind_msg_id = reg_event_value;
      break;

    case QMI_CLIENT_REG_WWAN_STATUS_IND:
      reg_client->wwan_status_ind_regd = (boolean)reg_event_value;
      break;

    default:
      LOG_MSG_ERROR("Invalid reg_event (%d)", reg_event, 0, 0);
      break;
  }

  LOG_MSG_INFO1("Add/Update: QMI_Client params, client=%p, event=%d, value=%d",
                    qmi_client, reg_event, reg_event_value);
  if (it != qmi_client_ref->end())
  {
    it->second = reg_client;
  }
  else
  {
    qmi_client_ref->insert(std::pair<uint32, qmi_client_ref_type*> (qmi_client, reg_client) );
  }
  LOG_MSG_INFO1("Result: client=%p, op_ind=%d, reg_ind=%d",
                    qmi_client, reg_client->wwan_op_ind_msg_id,
                    reg_client->wwan_status_ind_regd)
}


/*===========================================================================
FUNCTION RemoveQMIClient()

DESCRIPTION
  This Function delete's QMI Client reference from all WWAN Objects.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void QCMAP_Backhaul_WWAN::RemoveQMIClient(uint32 qmi_client)
{
  std::map<uint32, qmi_client_ref_type*>::iterator clnt_it;
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;

  QCMAP_CM_LOG_FUNC_ENTRY();
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;
    if (pBackhaulWWAN && pBackhaulWWAN->qmi_client_ref)
    {
      clnt_it = pBackhaulWWAN->qmi_client_ref->find(qmi_client);
      if (clnt_it != pBackhaulWWAN->qmi_client_ref->end())
      {
        LOG_MSG_INFO1("Removing client %p for profile(%d)", qmi_client, pBackhaulWWAN->GetProfileHandle(), 0);
        free(clnt_it->second);
        pBackhaulWWAN->qmi_client_ref->erase(clnt_it);
      }
    }
  }
}

/*======================================================
  FUNCTION GetWWANMTUInfo
======================================================*/
/*!
@brief
  This function will get mtu size from wwan ipv4 interface.

@parameters
  int* mtu_wwan_ipv4

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_Backhaul_WWAN::GetWWANMTUInfo(int* mtu_wwan_ipv4)
{
  int socket_mtu, af = AF_INET;
  struct ifreq ifr;
  qmi_error_type_v01 qcmap_cm_error = QMI_ERR_NONE_V01;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];

  /* get mtu value for rmnet of ipv4 wwan backhaul starts */
  /* Get Device Name */
  /* Send the RS packet to the rmnet iface */
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  if(!QcMapBackhaul || QcMapBackhaul->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V4_V01,
                                                    &qcmap_cm_error) == NO_BACKHAUL )
  {
    LOG_MSG_ERROR("Couldn't get ipv4 rmnet name. error %d\n", qcmap_cm_error,0,0);
    return false;
  }

  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_name, devname, sizeof(ifr.ifr_name));
  /* get mtu from found device for ipv4 starts */
  if((socket_mtu = socket(af, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Couldn't create socket to get mtu of ipv4 wwan err %d", errno,0,0);
    return false;
  }
  ifr.ifr_addr.sa_family = AF_INET;
  if (ioctl(socket_mtu, SIOCGIFMTU, &ifr) < 0)
  {
    LOG_MSG_ERROR("Couldn't get mtu from ipv4 wwan iface. err %d", errno,0,0);
    close(socket_mtu);
    return false;
  }
  LOG_MSG_INFO1("Successfully obtained MTU of wwan IPV4 iface  %d",  ifr.ifr_mtu, 0, 0);
  close(socket_mtu);

  /* Get mtu from found device for ipv4 wwan iface ends */
  if(ifr.ifr_mtu > 0 && mtu_wwan_ipv4 != NULL)
  {
    *mtu_wwan_ipv4=(int) ifr.ifr_mtu;
    return true;
  }
  else
  {
    LOG_MSG_ERROR("Couldn't get .mtu i.e not got it from wwan  %d", ifr.ifr_mtu,0,0);
    return false;
  }
}

