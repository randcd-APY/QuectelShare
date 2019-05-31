/*====================================================

FILE:  QCMAP_ConnectionManager.cpp

SERVICES:
   QCMAP Connection Manager Implementation

=====================================================

  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/11/12   gk         9x25
  10/26/12   cp         Added support for Dual AP and different types of NAT.
  12/19/12   sb         Added support for RNDIS/ECM USB tethering.
  02/01/13   cp         Minor IPv6 changes.
  02/27/13   cp         Added support for deprecating of prefix when switching between
                        station mode and WWAN mode.
  03/12/13   sb         Minor hostapd fixes.
  04/09/13   cp         AR6003 changes.
  04/16/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
  06/12/13   sg         Added DHCP Reservation feature
  09/17/13   at         Added support to Enable/Disable ALGs
  01/11/14   sr         Added support for connected Devices in SoftAP
  25/02/14   pm         Changes regarding Sta Assoc
  02/24/14   vm         Changes to Enable/Disable Station Mode to be in accordance
                        with IoE 9x15
  03/27/14   cp         Added support to DUN+SoftAP.
  06/06/14   rk         Added support DHCP option 26 for mtu in dnsmasq/dhcp server..
  08/08/14   rk         kw fixes.
  08/26/14   ka         Code Restructuring Changes
  06/12/15   rk         Offtarget support.
  04/06/16   pm         Load/Unload SFE modules on Enable/Disable
  03/28/17   spr        Added support for Multi-PDN.
======================================================*/
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
#include <ifaddrs.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <ctype.h>
#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "qmi_client.h"
#include "ds_qmi_qcmap_msgr.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_L2TP.h"
#include "QCMAP_GSB.h"
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
#include "linux/rmnet_ipa_fd_ioctl.h"
#endif
#include <unistd.h>
#include "qcmap_packet_stats.h"
// Define some constants.
#define ETH_HDRLEN 14  // Ethernet header length
#define IP6_HDRLEN 40  // IPv6 header length
#define ICMP_HDRLEN 8  // ICMP header length for echo request, excludes data
#define ETH_P_IPV6 0x86DD

#define HOSTAPD_CTRL_PATH "/var/run/hostapd"
#define HOSTNAME_PATH "/tmp/hostname.txt"
#define KERNEL_VERSION_PATH "/tmp/kernel_ver.txt"

#define DECIMAL 10
#define MAX_CMD_SIZE 100
#define MAX_SCAN_SIZE 100

#define MIN_NOTIFY_INTERVAL 30
#define MAX_NOTIFY_INTERVAL 60000


#define QCMAP_MULTIPLE_SYSTEM_CALL_FILE "/tmp/qcmap_multiple_system_call.sh"

#define FACTORY_CONF "/etc/data/factory_mobileap_cfg.xml"
#define FACTORY_SOCKSV5_CONF "/etc/data/factory_qti_socksv5_conf.xml"
#define FACTORY_SOCKSV5_AUTH "/etc/data/factory_qti_socksv5_auth.xml"
#define FACTORY_L2TP_CONF "/etc/data/factory_l2tp_cfg.xml"
#define FACTORY_FIREWALL_CONF "/etc/data/factory_mobileap_firewall.xml"
#define TEMP_MOBILEAP_CFG "/var/run/mobileap_cfg.tmp"
#define TEMP_FIREWALL_CFG "/var/run/mobileap_firewall.tmp"
#define TEMP_IPACM_CFG "/var/run/IPACM_cfg.tmp"
#define TEMP_GATEWAY_FILE "/tmp/gateway.txt"
#define SOFTAP_W_DUN_PATH "/etc/data/usb/softap_w_dun"

#define IPV6_DEFAULT_PREFERED_LIFETIME 2700;
#define IPV6_DEFAULT_VALID_LIFETIME 7200;
#define IPV6_MIN_PREFIX_LENGTH 64
#define IPV6_MAX_PREFIX_LENGTH 128
#define MAX_IPV6_PREFIX 40
#define IPTYPE_IPV6_ICMP        58

/*
#define QCMAP_DEBUG 1

#ifdef QCMAP_DEBUG
#undef LOG_MSG_INFO1
#define LOG_MSG_INFO1(fmtString, x, y, z) \
{ \
  if ( x != 0 && y !=0 && z != 0) \
    printf("\nINFO1:"fmtString"\n", x, y, z); \
  else if ( x != 0 && y != 0) \
    printf("\nINFO1:"fmtString"\n", x, y); \
  else if ( x != 0) \
    printf("\nINFO1:"fmtString"\n", x); \
  else \
    printf("\nINFO1:"fmtString"\n"); \
}
#endif
*/

QCMAP_LAN_Manager* QcMapLanMgr = NULL;

extern struct sigevent sev;

bool     QCMAP_ConnectionManager::flag= false;
QCMAP_ConnectionManager* QCMAP_ConnectionManager::object=NULL;
boolean  QCMAP_ConnectionManager::qcmap_tear_down_in_progress = false;
char     QCMAP_ConnectionManager::xml_path[] = {0};
int      QCMAP_ConnectionManager::qcmap_cm_handle = 0;
char     QCMAP_ConnectionManager::firewall_xml_path[] = {0};
std::map <profile_handle_type_v01, QCMAP_Backhaul *> *QCMAP_ConnectionManager::QCMAP_Backhaul_Hash = NULL;
boolean QCMAP_ConnectionManager::is_cm_enabled = false;
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
struct wan_ioctl_query_per_client_stats  ioctl_buffer, odu_ioctl_buffer;
struct wan_ioctl_send_lan_client_msg lan_client_msg;
#endif

/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and returns instance of class QCMAP_ConnectionManager

@parameters
  obj_create - flag to check if object/instance is already exist

@return
  object -  object created

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
QCMAP_ConnectionManager* QCMAP_ConnectionManager::Get_Instance
(
 char* xml_path,
 boolean obj_create
 )
{
  qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_config;
  memset(&l2tp_config,0,sizeof(qcmap_msgr_set_unmanaged_l2tp_state_config_v01));

  if (!flag && obj_create)
  {
    if (xml_path!=NULL)
    {
      strlcpy(QCMAP_ConnectionManager::xml_path, xml_path, QCMAP_CM_MAX_FILE_LEN);
    }
    else
    {
      LOG_MSG_INFO1("Updating XML path with default values %s",
                    QCMAP_DEFAULT_CONFIG,0,0);
      strlcpy(QCMAP_ConnectionManager::xml_path, QCMAP_DEFAULT_CONFIG,
              QCMAP_CM_MAX_FILE_LEN);
    }

    //Enable default LAN object
    QcMapLanMgr = QCMAP_LAN_Manager::GetInstance();

    if(!QCMAP_Virtual_LAN::Get_Instance(true))
    {
      /* File could not be opened for writing/append*/
        LOG_MSG_ERROR("\n Cannot create Virtual LAN instance!!",0,0,0);
    }

    if (QCMAP_L2TP::GetSetL2TPEnableConfigFromXML(GET_VALUE,&l2tp_config))
    {
      if(l2tp_config.enable)
      {
        LOG_MSG_INFO1("\n L2TP config enabled. Creating L2TP instance",0,0,0);
        if(!QCMAP_L2TP::Get_Instance(true))
        {
          /* File could not be opened for writing/append*/
          LOG_MSG_ERROR("\n Cannot create L2TP instance!!",0,0,0);
        }
      }
    }

    LOG_MSG_INFO1("Creating object : QCMAP",0, 0, 0);
    object = new QCMAP_ConnectionManager();
    object->init();

    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}

/*======================================================
  FUNCTION doesFileExist
======================================================*/
/*!
  @brief
  Function checks if file is present the the specified path.

@return
  boolean

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================================================*/
boolean doesFileExist (char *filename)
{
  struct stat  statbuf;

  bzero( &statbuf, sizeof(statbuf) );
  if( (stat(filename, &statbuf) == 0) && (S_ISREG(statbuf.st_mode)))
    return true;
  else
    return false;
}

/*==========================================================
  FUNCTION    qcmap_match_mac_address
===========================================================*/
/*!
@brief
  Comparator function for matching MAC address

@parameters
  Two void pointers with the MAC addresses to be compared

@return
  0 - on a match
  1 - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*==========================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_mac_address
(
  const void *first,
  const void *second
)
{
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  uint8* msg_mac_addr = (uint8*)first;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  return(memcmp( msg_mac_addr,
                 connectedDevicesList->mac_addr,
                 QCMAP_MSGR_MAC_ADDR_LEN_V01));
}

/*==========================================================
  FUNCTION    qcmap_match_mac_addr_vlan_id_pair
===========================================================*/
/*!
@brief
  Comparator function to check MAC address and VLAN ID
  combination

@parameters
  1. First - struct pointer containing MAC address and VLAN ID
  2. Second - Connected Device List

@return
  0 - on a match
  1 - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*===========================================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_mac_addr_vlan_id_pair
(
  const void *first,
  const void *second
)
{
  long int                     ret_val = 1;
  uint8                        err_val = 0;
  qcmap_cm_client_data_info_t *connectedDevicesList = NULL;
  qcmap_mac_addr_vlan_info_t  *mac_vlan_info_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if((first == NULL) || (second == NULL))
    {
      err_val = 1;
      break;
    }

    mac_vlan_info_p      = (qcmap_mac_addr_vlan_info_t *)first;
    connectedDevicesList = (qcmap_cm_client_data_info_t *)second;

    /*--------------------------------------------------------------------
    Check for MAC address and VLAN ID match, If both the parameters match
    return 0
    ----------------------------------------------------------------------*/
    if((memcmp(mac_vlan_info_p->mac_addr,
              connectedDevicesList->mac_addr,
              QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0) &&
       (mac_vlan_info_p->vlan_id == connectedDevicesList->vlan_id))
    {
       LOG_MSG_INFO1("MAC-VLAN pair match found",0,0,0);

       ret_val = 0;
    }

  }while(0);

  return ret_val;
}
/*=====================================================
  FUNCTION    qcmap_match_v6_iid
======================================================*/
/*!
@brief
  Comparator function for matching IID

@parameters
  void *first - qcmap_cm_prefix_iid_info_t ptr
  void *second - qcmap_cm_client_data_info_t ptr to client info

@return
  QCMAP_CM_SUCCESS - on a match
  QCMAP_CM_ERROR - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*=====================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_v6_iid
(
  const void *first,
  const void *second
)
{
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  qcmap_cm_prefix_iid_info_t *prefix_iid_ptr = NULL;
  struct ps_in6_addr *client_iid_ptr = NULL;

  if( (first == NULL) || (second == NULL))
  {
    return QCMAP_CM_ERROR;
  }

  prefix_iid_ptr = (qcmap_cm_prefix_iid_info_t *)first;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  if ( prefix_iid_ptr->iid_ptr == NULL )
  {
    LOG_MSG_ERROR("IID addr ptr is NULL,cannot match "
                  "IID's", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  client_iid_ptr = (struct ps_in6_addr *)connectedDevicesList->link_local_v6_addr;

  /*Check whether IID's match*/
  if(!memcmp(&prefix_iid_ptr->iid_ptr->ps_s6_addr64[1],
             &client_iid_ptr->ps_s6_addr64[1],
             sizeof(uint64_t)))
  {
    LOG_MSG_INFO1("qcmap_match_v6_iid - IID match found\n", 0, 0, 0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    //IID's do not match
    return QCMAP_CM_ERROR;
  }
}

/*=====================================================
  FUNCTION    qcmap_match_v6_addr
======================================================*/
/*!
@brief
  Comparator function for matching v6 addresses

@parameters
  void *first - qcmap_cm_prefix_iid_info_t ptr
  void *second - qcmap_cm_client_data_info_t ptr to client info

@return
  QCMAP_CM_SUCCESS - on a match
  QCMAP_CM_ERROR - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*=====================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_v6_addr
(
  const void *first,
  const void *second
)
{
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  qcmap_cm_prefix_iid_info_t *v6_ptr = NULL;
  struct ps_in6_addr *client_iid_ptr = NULL;

  if( (first == NULL) || (second == NULL))
  {
    return QCMAP_CM_ERROR;
  }

  v6_ptr = (qcmap_cm_prefix_iid_info_t *)first;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  if ( v6_ptr->iid_ptr == NULL )
  {
    LOG_MSG_ERROR("IID addr ptr is NULL,cannot match "
                  "IID's", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  client_iid_ptr = (struct ps_in6_addr *)connectedDevicesList->ipv6[0].addr;

  /*Check whether addresses match*/
  if(!memcmp(&v6_ptr->iid_ptr->ps_s6_addr64[1],
             &client_iid_ptr->ps_s6_addr64[1],
             sizeof(uint64_t)))
  {
    LOG_MSG_INFO1("qcmap_match_v6_iid - IID match found\n", 0, 0, 0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    //Addresses do not match
    return QCMAP_CM_ERROR;
  }
}

/*=====================================================
  FUNCTION    qcmap_match_proxy_entry
======================================================*/
/*!
@brief
  Comparator function for matching proxy entry

@parameters
  void *first - ptr to head of list
  void *proxyInfo - qcmap_cm_proxy_data_info_t struct

@return
  QCMAP_CM_SUCCESS - on a match
  QCMAP_CM_ERROR - otherwise

  @note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*=====================================================*/
static long int QCMAP_ConnectionManager::qcmap_match_proxy_entry
(
  const void *first,
  const void *second
)
{
  qcmap_cm_proxy_data_info_t* proxyInfo_first = NULL;
  qcmap_cm_proxy_data_info_t* proxyInfo_second = NULL;

  if( (first == NULL) || (second == NULL))
  {
    return QCMAP_CM_ERROR;
  }

  proxyInfo_first = (qcmap_cm_proxy_data_info_t *)first;
  proxyInfo_second = (qcmap_cm_proxy_data_info_t *)second;

  /*Check whether IID's match*/
  if( !memcmp(proxyInfo_first->dst_ip_v6_addr, proxyInfo_second->dst_ip_v6_addr,
              QCMAP_MSGR_IPV6_ADDR_LEN_V01) &&
      !memcmp(proxyInfo_first->src_ip_v6_addr, proxyInfo_second->src_ip_v6_addr,
              QCMAP_MSGR_IPV6_ADDR_LEN_V01) )
  {
    LOG_MSG_INFO1("qcmap_match_proxy_entry - dst and src match", 0, 0, 0);
    return QCMAP_CM_SUCCESS;
  }
  else
  {
    //no match
    return QCMAP_CM_ERROR;
  }
}

/*==========================================================
 FUNCTION    qcmap_v6_proxy_match
===========================================================*/
/*!
@brief
  Comparator function for match ConnectedDevices device type

@parameters
  Two void pointers with the device type to be compared

@return
  0 - on a match
  1 - otherwise

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
static long int QCMAP_ConnectionManager::qcmap_v6_proxy_match
(
  const void *first,
  const void *second
)
{
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  LOG_MSG_ERROR("memcmp",0,0,0);

  return(memcmp(first, second, QCMAP_MSGR_IPV6_ADDR_LEN_V01));
}

/*=====================================================
  FUNCTION    fetchHostNameAndLeasetime
======================================================*/
/*!
@brief
  Function to fetch the Device name  and lease time from
  dnsmasq.leases file

@parameters
  fd of the file to be parsed
  Device MAC address whose hostname & lease expiry has to be found
  char pointer to store the hostname of device
  uint32 pointer to hold the expiry time (in minutes) of the DHCP lease

@return
  void

@note

 @ Dependencies
    - None

 @ Side Effects
    - None
*/
/*====================================================*/
static void
fetchHostNameAndLeasetime
(
  FILE* fd,
  uint8* mac_addr,
  char* hostName,
  uint32* leaseExpiryTime
)
{
  if ((fd == NULL) || (mac_addr == NULL) ||(hostName == NULL) ||
      (leaseExpiryTime == NULL))
  {
    LOG_MSG_ERROR("Null arguements passed.\n",0,0,0);
    return;
  }
  char *expiryTime=NULL,*charMacAddr=NULL, *filehostName=NULL,*ptr;
  char stringline[MAX_COMMAND_STR_LEN];
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/
  int bufferSize = QCMAP_EPOCH_TIME_LEN +
                   QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01 +
                   QCMAP_MSGR_DEVICE_NAME_MAX_V01;
  int expTime = 0;
  time_t expTimeFormat;
  //Reset the file ptr to start of file
  if (fseek(fd,0,SEEK_SET) !=0)
  {
    LOG_MSG_ERROR("File pointer not reset to beginning of file\n",0,0,0);
    return;
  }

  ds_mac_addr_ntop(mac_addr, mac_addr_string);
  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    expiryTime = strtok_r(stringline, " ", &ptr);
    charMacAddr = strtok_r(NULL, " ", &ptr);
    /* KW fix */
    if ( charMacAddr!=NULL && !strcmp(charMacAddr, mac_addr_string))
    {
      //If the Mac address matches, store the hostname
      //Tokenize with newline to discard the new line charcter from host name
      filehostName = strtok_r(NULL, " \n",&ptr);

      //Copy hostname to CDI list only if  CDI host name is empty

      if(filehostName && (strncmp(filehostName, "*", strlen("*"))) &&
         (!strlen(hostName)) )
        strlcpy( hostName,filehostName,QCMAP_MSGR_DEVICE_NAME_MAX_V01 );
      //Convert the expiry time from char format to interger
      expTime = ds_atoi((const)expiryTime);
      expTimeFormat = expTime;
      //Convert the time from Epoch format to readable format
      *leaseExpiryTime = (expTimeFormat - time(NULL))/60;

      if (!strncmp(hostName, "*", strlen("*")))
      {
        QCMAP_ConnectionManager::fetchHostName(hostName, mac_addr_string);
      }
      return;
    }
  }
  return;
}

/*=====================================================
  FUNCTION strtolower
======================================================*/
/*!
@brief
  Converts the string passed to lower case.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================================================*/

void  QCMAP_ConnectionManager::strtolower(uint8 *str)
{
  int counter = 0;
  char c;

  if( str!=NULL )
  {
    while ( str[counter] ) {
      c = str[counter];
      str[counter] = tolower(c);
      counter++;
    }
  }
}

/*=====================================================
  FUNCTION QCMAP_ConnectionManager_callback
======================================================*/
/*!
@brief
  Handles the mobile ap events and invokes the callback function
  for the respective indications to be sent.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*====================================================*/
static void QCMAP_ConnectionManager_callback
(
  int               handle,               /* MobileAP Application id */
  qcmap_cm_event_e  event,                /* Type of MobileAP Event  */
  void             *wwanObj,              /* Reference to Backhaul WWAN Obj */
  void             *qcmap_msgr_cb_user_data, /* Call back user data     */
  dsi_ce_reason_t   *callend_reason,
  qcmap_msgr_wwan_info_v01 *wwan_info
)
{
  boolean cradle_sta_eth_bh_active;
  QCMAP_ConnectionManager *manager;
  qmi_qcmap_msgr_status_cb_data *cbPtr;
  qmi_qcmap_msgr_softap_handle_type *softApHandle =
                  (qmi_qcmap_msgr_softap_handle_type *) qcmap_msgr_cb_user_data;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr = GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr = GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = (QCMAP_Backhaul_WWAN *)wwanObj;
  QCMAP_LAN* lan_obj = NULL;
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaul;

  if (QcMapBackhaulWWAN == NULL) /* Default object will be used on event_enabled */
  {
    QcMapBackhaulWWAN = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
    QcMapBackhaul = GET_DEFAULT_BACKHAUL_OBJECT();
    lan_obj = GET_DEFAULT_LAN_OBJECT();
  }
  else
  {
    QcMapBackhaul = QcMapBackhaulWWAN->QcMapBackhaul;

    if((lan_obj = GET_LAN_OBJECT(QcMapBackhaul->vlan_id)) == NULL)
    {
      lan_obj = GET_DEFAULT_LAN_OBJECT();
    }
  }

  QCMAP_Firewall* QcMapFirewall = QcMapBackhaul->QcMapFirewall;
  QCMAP_NATALG* QcMapNatAlg = QcMapBackhaul->QcMapNatAlg;
  uint8 roam_status;

  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 pri_dns_addr = 0;

  qcmap_msgr_wwan_call_end_type_enum_v01 call_end_type =
                                      QCMAP_MSGR_WWAN_CALL_END_TYPE_INVALID_V01;
  int call_end_reason_code=0;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  dsi_data_bearer_tech_t bearer_tech = DSI_DATA_BEARER_TECH_UNKNOWN;
  unsigned int profile_index;
  char logmsg[MAX_COMMAND_STR_LEN];
  int qcmap_cm_error;
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;

  if (softApHandle != NULL)
  {
    cbPtr = (qmi_qcmap_msgr_status_cb_data *) softApHandle->cb_ptr;
    manager = softApHandle->Mgr;
  } else {
     return;
  }

  LOG_MSG_INFO1("QCMAP AP Handle %04X Event %04X\n", handle, event,0);

  memset ( devname, 0, sizeof(devname));
  memset ( logmsg, 0, sizeof(logmsg));

  /*If connect to backhaul failed then callend_reason would be filled
    with reason and call type, same is sent to clients in failure response*/
  if( callend_reason != NULL )
  {
    if( callend_reason->reason_type == DSI_CE_TYPE_MOBILE_IP )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_MOBILE_IP_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_INTERNAL )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_INTERNAL_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_CALL_MANAGER_DEFINED )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_CALL_MANAGER_DEFINED_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_3GPP_SPEC_DEFINED )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_3GPP_SPEC_DEFINED_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_PPP )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_PPP_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_EHRPD )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_EHRPD_V01;
    else if( callend_reason->reason_type == DSI_CE_TYPE_IPV6 )
      call_end_type = QCMAP_MSGR_WWAN_CALL_END_TYPE_IPV6_V01;
    call_end_reason_code = callend_reason->reason_code;
  }

  cradle_sta_eth_bh_active = QCMAP_Backhaul::IsNonWWANBackhaulActive();
  switch ( event )
  {
  case QCMAP_CM_EVENT_ENABLED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_ENABLED\n",0,0,0);
    if (cbPtr && cbPtr->qcmap_cb)
    {
      cbPtr->qcmap_cb(cbPtr, QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01);
    }
    /* Block WWAN access for the clients. */
    lan_obj->BlockIPv4WWANAccess();
    lan_obj->BlockIPv6WWANAccess();
    break;
  case QCMAP_CM_EVENT_WAN_CONNECTING:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_CONNECTING\n",0,0,0);
    break;
  case QCMAP_CM_EVENT_WAN_CONNECTING_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_CONNECTING_FAIL\n",0,0,0);
    if (cbPtr && cbPtr->wwan_cb && QcMapBackhaulWWAN)
    {
     cbPtr->wwan_cb(cbPtr, QcMapBackhaulWWAN->GetProfileHandle(), QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01,
                     call_end_type, call_end_reason_code, wwan_info);
    }
    break;

  case QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL\n",0,0,0);
    if (cbPtr && cbPtr->wwan_cb && QcMapBackhaulWWAN)
    {
      cbPtr->wwan_cb(cbPtr, QcMapBackhaulWWAN->GetProfileHandle(), QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01,
                     call_end_type,call_end_reason_code, wwan_info);
    }
    break;

  case QCMAP_CM_EVENT_WAN_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_CONNECTED\n",0,0,0);
    /* There is no need to modify the netmask of WWAN interface. If we modify the
     * the netmask existing routes installed by the embedded applications
     * will be deleted and their data transfer will be affected.
     */

    ds_system_call("echo QCMAP:WAN connected v4 > /dev/kmsg",\
                   strlen("echo QCMAP:WAN connected v4 > /dev/kmsg"));

    /* Now time to enable NAT, ALGs etc., on A5*/
    if ( QcMapBackhaulWWAN &&
          IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
          QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
          QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
    {
      QcMapBackhaulWWAN->AddWWANIPv4SIPServerInfo();
      if (QcMapBackhaul->ComparePriority(BACKHAUL_TYPE_WWAN,QCMAP_Backhaul::current_backhaul))
      {
        if (QCMAP_Backhaul::current_backhaul != BACKHAUL_TYPE_WWAN &&
            QCMAP_Backhaul::current_backhaul != NO_BACKHAUL)
        {
          if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV4() ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
          {
            QcMapNatAlg->DisableNATonApps();
            QcMapFirewall->CleanIPv4MangleTable();
          }
          if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV6() ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() ||
              QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6())
          {
            QcMapBackhaul->DisableIPV6Forwarding(true);
            QcMapFirewall->CleanIPv6MangleTable();
            if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
            {
              QcMapBackhaul->DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
              QcMapBackhaul->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
            }
            if (QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
            {
              if (QcMapBackhaulCradleMgr && QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
                QcMapBackhaul->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
              else
              {
                QcMapBackhaul->DeleteAllV6ExternalRoute(ECM_IFACE);
                QcMapBackhaul->ClearIfaceRelatedRulesForV6(ECM_IFACE);
              }
            }
            if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
            {
              QcMapBackhaul->DeleteAllV6ExternalRoute(ETH_IFACE);
              QcMapBackhaul->ClearIfaceRelatedRulesForV6(ETH_IFACE);
            }
          }
        }
        QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_WWAN;
        QcMapNatAlg->EnableNATonApps();
        lan_obj->EnableDNS();
      }
    }
    else
    {
      //3 For Secondary PDN's
      if (QcMapBackhaulWWAN->EnableVlanPdnRules())
      {
        LOG_MSG_INFO1("Enabled VLAN rules", 0,0,0);
      }
      else
      {
        LOG_MSG_ERROR("Unable to install rules",0,0,0);
      }
    }

    if (QcMapBackhaulWWAN)
    {
      QcMapBackhaulWWAN->AddDNSRoutesForPDN(QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01);
      if (lan_obj && !(IS_DEFAULT_PROFILE(lan_obj->GetBackhaulProfileHandle())))
      {
        //only update the resolv.conf file again if lan object is not default here.
        lan_obj->EnableDNS();
      }
    }

    if ( QcMapBackhaulWWAN && (QcMapBackhaulWWAN->GetDeviceName(manager->qcmap_cm_handle,
                              QCMAP_MSGR_IP_FAMILY_V4_V01,
                              devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS) )
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", 0,0,0);
      qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    }

    //Get IP Passthrough flag from XML
    if( QcMapBackhaulWWAN &&
          IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
          lan_obj->GetIPPassthroughConfig(&enable_state,&passthrough_config, &qmi_err_num))
    {
      if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_UP_V01)
      {
        lan_obj->RestartTetheredClient(passthrough_config.device_type);
      }
    }
    else
    {
      LOG_MSG_ERROR("Get IP Passthrough Flag failed\n",0,0,0);
    }

    if (QcMapMgr->IsDynamicClockVotingNeeded())
    {
      if (QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
          qcmap_cm_check_ltefdd_cat4_bw() &&
          !QcMapMgr->CheckUsbClockAtNom())
      {
        QcMapMgr->SetUSBClockFreq(true);
      }
    }

    if (cbPtr && cbPtr->wwan_cb && QcMapBackhaulWWAN)
    {
      cbPtr->wwan_cb(cbPtr, QcMapBackhaulWWAN->GetProfileHandle(), QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01,
                     call_end_type,call_end_reason_code, wwan_info);
    }
    break;

  case QCMAP_CM_EVENT_WAN_IPv6_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_IPv6_CONNECTED",0,0,0);

    ds_system_call("echo QCMAP:WAN connected v6 > /dev/kmsg",\
                   strlen("echo QCMAP:WAN connected v6 > /dev/kmsg"));

    if (QcMapBackhaul->ComparePriority(BACKHAUL_TYPE_WWAN,QCMAP_Backhaul::current_backhaul))
    {
      if ( QcMapBackhaulWWAN &&
            IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
            QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
            QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
      {
        if (QCMAP_Backhaul::current_backhaul != BACKHAUL_TYPE_WWAN &&
            QCMAP_Backhaul::current_backhaul != NO_BACKHAUL)
        {
          if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV4() ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
          {
            QcMapNatAlg->DisableNATonApps();
            QcMapFirewall->CleanIPv4MangleTable();
          }
          if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV6() ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() ||
              QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6())
          {
            QcMapBackhaul->DisableIPV6Forwarding(true);
            QcMapFirewall->CleanIPv6MangleTable();
            if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
            {
              QcMapBackhaul->DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
              QcMapBackhaul->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
            }
            if (QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
            {
              if (QcMapBackhaulCradleMgr && QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
                QcMapBackhaul->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
              else
              {
                QcMapBackhaul->DeleteAllV6ExternalRoute(ECM_IFACE);
                QcMapBackhaul->ClearIfaceRelatedRulesForV6(ECM_IFACE);
              }
            }
            if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
            {
              QcMapBackhaul->DeleteAllV6ExternalRoute(ETH_IFACE);
              QcMapBackhaul->ClearIfaceRelatedRulesForV6(ETH_IFACE);
            }
          }
        }
        QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_WWAN;

        /*If PMIP mode is enabled, this will be done in EnablePmipMode*/
        if(!QCMAP_Backhaul::IsPmipV6ModeEnabled())
        {
          QcMapBackhaul->EnableIPV6Forwarding();
          QcMapFirewall->EnableIPV6Firewall(); //this is NA in PMIP mode
        }

        lan_obj->AddDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv6_addr,
                                       QcMapBackhaulWWAN->sec_dns_ipv6_addr);
      }
      else
      {
        //3 Secondary PDN
        if (QcMapBackhaulWWAN && (!IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle())) )
        {
          QcMapBackhaulWWAN->AddIPv6RouteForSecondaryPDN();
          QcMapBackhaulWWAN->EnableVlanPdnRules();
        }
      }
    }
    else
    {
      LOG_MSG_INFO1("Delete route for secondary PDNs",0,0,0); //TODO will be taken care in NAT/ALG gerrit.
    }

    if (QcMapBackhaulWWAN)
    {
      QcMapBackhaulWWAN->AddDNSRoutesForPDN(QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01);
      if (lan_obj &&
               (QCMAP_MSGR_DHCPV6_MODE_UP_V01 != QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_enable_state))
      {
        lan_obj->AddDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv6_addr,
                                        QcMapBackhaulWWAN->sec_dns_ipv6_addr);
      }
    }

    if ( QcMapBackhaulWWAN && QcMapBackhaulWWAN->GetDeviceName(manager->qcmap_cm_handle,
                              QCMAP_MSGR_IP_FAMILY_V6_V01,
                              devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", 0,0,0);
      qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    }

    if (IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
        QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
    {
      /*Enable PMIP mode - add check and need of new qmi err*/
      if(QCMAP_Backhaul::IsPmipV6ModeEnabled() || QCMAP_Backhaul::IsPmipV4ModeEnabled())
      {
        if(!QcMapBackhaul->EnablePmipMode(&qmi_err_num))
        {
          LOG_MSG_ERROR("EnablePmipMode failed with error:%d",qmi_err_num,0,0);
        }
      }
    }

    if (IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
        QcMapMgr->IsDynamicClockVotingNeeded())
    {
      if (QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
          qcmap_cm_check_ltefdd_cat4_bw() &&
          !QcMapMgr->CheckUsbClockAtNom())
      {
        QcMapMgr->SetUSBClockFreq(true);
      }
    }

    if (cbPtr && cbPtr->wwan_cb && QcMapBackhaulWWAN)
    {
      cbPtr->wwan_cb(cbPtr, QcMapBackhaulWWAN->GetProfileHandle(), QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01,
                     call_end_type,call_end_reason_code, wwan_info);
    }
    break;
  case QCMAP_CM_EVENT_WAN_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_DISCONNECTED\n",0,0,0);
    QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_IPV4_WWAN_DISCONNECTED_V01,
                                  QcMapMgr->conn_device_info.numOfNodes,NULL);
    /* 9x25, Disable NAT on A5 */
     if( QcMapBackhaulWWAN &&
            IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
            QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN &&
           (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false) &&
           (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
      QcMapBackhaulWWAN->DeleteWWANIPv4SIPServerInfo();
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
      if((QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED) &&
         (QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_DISCONNECTING))
      {
        QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_WWAN,
                                                     true,true);

        if (QcMapMgr->IsDynamicClockVotingNeeded())
        {
          if (!(QcMapMgr->CheckWlanClientConnected()&&
              QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
              QcMapMgr->CheckUsbClockAtNom()))
          {
            QcMapMgr->SetUSBClockFreq(false);
          }
        }
      }
      if(lan_obj)
      {
        lan_obj->DeleteDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv4_addr,
                                          QcMapBackhaulWWAN->sec_dns_ipv4_addr);
      }
    }
    else
    {
      //3 For Secondary PDN's
      if (QcMapBackhaulWWAN)
      {
        QcMapBackhaulWWAN->DeleteWWANIPv4SIPServerInfo();
        if (!QcMapBackhaulWWAN->DisableVlanPdnRules(QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01))
        {
          LOG_MSG_ERROR("Unable to Remove rules",0,0,0);
        }

        if(lan_obj && (!IS_DEFAULT_PROFILE(lan_obj->GetBackhaulProfileHandle())))
        {
          lan_obj->DeleteDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv4_addr,
                                            QcMapBackhaulWWAN->sec_dns_ipv4_addr);
        }
      }
    }

    if (QcMapBackhaulWWAN)
    {
      QcMapBackhaulWWAN->DeleteDNSRoutesForPDN(QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01);
    }

    /*Check for dun dongle mode*/
    if (QcMapBackhaulWWAN && IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
        QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED)
    {
      if (QCMAP_Tethering::BringDownPPPD())
      {
        LOG_MSG_INFO1("Bring Down PPPD Success\n",0,0,0);
      }
    }

    if (cbPtr && cbPtr->wwan_cb && QcMapBackhaulWWAN)
    {
      cbPtr->wwan_cb(cbPtr, QcMapBackhaulWWAN->GetProfileHandle(), QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01,
                     call_end_type,call_end_reason_code, wwan_info);
    }
    break;

  case QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED\n",0,0,0);
    QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_IPV6_WWAN_DISCONNECTED_V01,
                                  QcMapMgr->conn_device_info.numOfNodes,NULL);

    if( QcMapBackhaulWWAN &&
        IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN &&
        (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false) &&
        (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
      if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
      {
        if(QCMAP_Backhaul::IsPmipV6ModeEnabled() || (QCMAP_Backhaul::IsPmipV4ModeEnabled()))
        {
          QcMapBackhaul->DisablePmipMode(&qmi_err_num, PMIP_DISABLE_EV);
        }
      }
      QcMapBackhaul->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      //MAKE CHANGES TO SWITCH TO STA OR CRADLE
      if((QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED) &&
         (QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_DISCONNECTING))
      {
        QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_WWAN,
                                                       true,true);
      }

      if (QcMapMgr->IsDynamicClockVotingNeeded())
      {
        if (!(QcMapMgr->CheckWlanClientConnected()&&
            QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
            QcMapMgr->CheckUsbClockAtNom()))
        {
          QcMapMgr->SetUSBClockFreq(false);
        }
      }

      if(lan_obj)
      {
        lan_obj->DeleteDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv6_addr,
                                          QcMapBackhaulWWAN->sec_dns_ipv6_addr);
      }
    }
    else if (QcMapBackhaulWWAN &&
            ( ! IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle())))
    {
      //3 Secondary PDN
      QcMapBackhaulWWAN->DisableVlanPdnRules(QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01);
      QcMapBackhaulWWAN->DeleteIPv6RouteForSecondaryPDN();

      if(lan_obj && ( ! IS_DEFAULT_PROFILE(lan_obj->GetBackhaulProfileHandle())))
      {
        lan_obj->DeleteDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv6_addr,
                                          QcMapBackhaulWWAN->sec_dns_ipv6_addr);
      }
    }

    if (QcMapBackhaulWWAN)
    {
      QcMapBackhaulWWAN->DeleteDNSRoutesForPDN(QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01);
    }

    /*Check for dun dongle mode*/
    if (QcMapBackhaulWWAN &&
        IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle()) &&
        QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED)
    {
      if (QCMAP_Tethering::BringDownPPPD())
      {
        LOG_MSG_INFO1("Bring Down PPPD Success\n",0,0,0);
      }
    }

    if (cbPtr && cbPtr->wwan_cb && QcMapBackhaulWWAN)
    {
      cbPtr->wwan_cb(cbPtr, QcMapBackhaulWWAN->GetProfileHandle(), QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01,
                     call_end_type,call_end_reason_code, wwan_info);
    }
  break;
  case QCMAP_CM_EVENT_WAN_ADDR_RECONF:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_ADDR_RECONF\n", 0, 0, 0);
    if( !cradle_sta_eth_bh_active)
    {
      if ( QcMapBackhaulWWAN->GetIPv4NetworkConfig(&public_ip, &pri_dns_addr,
                                                  &sec_dns_addr, &default_gw_addr, &qmi_err_num)
           != QCMAP_CM_SUCCESS )
      {
        LOG_MSG_INFO1("Error in GetIPv4NetConf() %d\n", qmi_err_num,0,0);
        return false;
      }

      if((QCMAP_Backhaul::IsPmipV4ModeEnabled() || QCMAP_Backhaul::IsPmipV6ModeEnabled()) &&
         (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01))
      {
        /*If PMIP mode is enabled, this will be done in EnablePmipMode*/
        QcMapBackhaul->DisablePmipMode(&qmi_err_num, PMIP_STOP_EV);
        QcMapBackhaul->EnablePmipMode(&qmi_err_num);
      }

      if((lan_cfg = lan_obj->GetLANConfig()) != NULL)
      {
        if(!(lan_cfg->ip_passthrough_cfg.ip_passthrough_active) ||
           ((lan_cfg->ip_passthrough_cfg.ip_passthrough_active) &&
           (public_ip != lan_cfg->ip_passthrough_cfg.rmnet_reserved_ip)))
        {
          QcMapNatAlg->DisableNATonApps();
          QcMapFirewall->CleanIPv4MangleTable();
          QcMapNatAlg->EnableNATonApps();
          lan_obj->EnableDNS();
        }
      }


    }
  break;
  case QCMAP_CM_EVENT_WAN_IPv6_ADDR_RECONF:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WAN_ADDR_RECONF\n", 0, 0, 0);
    if( !cradle_sta_eth_bh_active)
    {
      QcMapBackhaul->DisableIPV6Forwarding(true);
      if(lan_obj && QcMapBackhaulWWAN && (QcMapBackhaulWWAN->GetProfileHandle() != 0))
      {
        LOG_MSG_INFO1("Error in GetIPv4NetConf() %d\n", qmi_err_num,0,0);
        return false;
      }

      if(QcMapBackhaul && IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle) &&
        (QCMAP_Backhaul::IsPmipV6ModeEnabled() || QCMAP_Backhaul::IsPmipV4ModeEnabled()) &&
         (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01))
      {
        /*If PMIP mode is enabled, this will be done in EnablePmipMode*/
        QcMapBackhaul->DisablePmipMode(&qmi_err_num, PMIP_STOP_EV);
        QcMapBackhaul->EnablePmipMode(&qmi_err_num);
      }
      else
      {
        QcMapBackhaul->DisableIPV6Forwarding(true);
        QcMapFirewall->CleanIPv6MangleTable();
        QcMapBackhaul->EnableIPV6Forwarding();
        QcMapFirewall->EnableIPV6Firewall();
      }

      if(lan_obj && QcMapBackhaulWWAN &&
        (QCMAP_MSGR_DHCPV6_MODE_UP_V01 != QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_enable_state) &&
        (IS_DEFAULT_PROFILE(QcMapBackhaulWWAN->GetProfileHandle())) &&
        (QcMapBackhaulWWAN->GetProfileHandle() != 0))
      {
        lan_obj->AddDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv6_addr,
                                       QcMapBackhaulWWAN->sec_dns_ipv6_addr);
      }
    }
  break;
  case QCMAP_CM_EVENT_DISABLED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_DISABLED\n",0,0,0);
    if (cbPtr && cbPtr->qcmap_cb)
    {
      cbPtr->qcmap_cb(cbPtr, QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01);
    }
    pthread_mutex_lock(&manager->cm_mutex);
    QcMapMgr->qcmap_tear_down_in_progress = false;
    manager->DisableHandle();
    pthread_cond_signal(&manager->cm_cond);
    pthread_mutex_unlock(&manager->cm_mutex);
    LOG_MSG_INFO1("Teardown flag=false\n",0,0,0);
    break;
  case QCMAP_CM_EVENT_STA_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_CONNECTED\n",0,0,0);
    if (NULL != QcMapBackhaulWLANMgr)
    {
      if (QcMapBackhaulWLANMgr->sta_connected_event_received == true)
      {
        LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_CONNECTED - Duplicate Event \n",0,0,0);
        break;
      }
      if (QcMapWLANMgr->IsSSRInProgress())
      {
        LOG_MSG_INFO1("Ignoring QCMAP_CM_EVENT_STA_CONNECTED event -- since it"
                      " recieved during SSR",0,0,0);
        break;
      }
      QcMapBackhaulWLANMgr->sta_connected_event_received = true;
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        QcMapBackhaulWLANMgr->ProcessStaAssocForWLANBridge(qcmap_msgr_cb_user_data);
      }
      else
      {
        QcMapBackhaulWLANMgr->ProcessStaAssoc(qcmap_msgr_cb_user_data);
      }

      /* Send WLAN Status IND */
      if(QcMapWLANMgr && cbPtr && cbPtr->wlan_status_cb)
      {
        QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                    QCMAP_MSGR_WLAN_CONNECTING_V01;
        QcMapWLANMgr->SendWLANStatusIND();
      }

      if (QcMapMgr->IsDynamicClockVotingNeeded())
      {
        if (!(QcMapMgr->CheckUsbClockAtNom()))
        {
          QcMapMgr->SetUSBClockFreq(true);
        }
      }
    }
    break;
  case QCMAP_CM_EVENT_STA_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_DISCONNECTED\n",0,0,0);
    if(NULL != QcMapBackhaulWLANMgr)
    {
      if (QcMapBackhaulWLANMgr->sta_connected_event_received == false)
      {
        LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_DISCONNECTED - Duplicate Event \n",0,0,0);
        break;
      }
      if (QcMapWLANMgr->IsSSRInProgress())
      {
        LOG_MSG_INFO1("Ignoring QCMAP_CM_EVENT_STA_DISCONNECTED event -- since"
                      " it recieved during SSR",0,0,0);
        break;
      }
      QcMapBackhaulWLANMgr->sta_connected_event_received = false;
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        QcMapBackhaulWLANMgr->ProcessStaDisAssocForWLANBridge();
      }
      else
      {
        QcMapBackhaulWLANMgr->ProcessStaDisAssoc();
      }
      if (cbPtr && cbPtr->sta_cb)
      {
          cbPtr->sta_cb(cbPtr, QCMAP_MSGR_STATION_MODE_DISCONNECTED_V01);
      }

      /* Send WLAN Status IND */
      if(QcMapWLANMgr && cbPtr && cbPtr->wlan_status_cb)
      {
        QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                    QCMAP_MSGR_WLAN_DISCONNECTED_V01;
        QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                    QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
        QcMapWLANMgr->SendWLANStatusIND();
      }

      if (QcMapMgr->IsDynamicClockVotingNeeded())
      {
        if (QcMapMgr->CheckUsbClockAtNom())
        {
          QcMapMgr->SetUSBClockFreq(false);
        }
      }
    }
    break;
    case QCMAP_CM_EVENT_STA_ASSOCIATION_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_ASSOCIATION_FAIL\n",0,0,0);
    if(NULL != QcMapBackhaulWLANMgr)
    {
      QcMapBackhaulWLANMgr->ProcessStaAssocFail(qcmap_msgr_cb_user_data);
    }
    break;
    case QCMAP_CM_EVENT_STA_DHCP_IP_ASSIGNMENT_FAIL:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_STA_DHCP_IP_ASSIGNMENT_FAIL\n",0,0,0);
    if(NULL != QcMapBackhaulWLANMgr)
      QcMapBackhaulWLANMgr->ProcessStaDHCPIPFail(qcmap_msgr_cb_user_data);
    break;
  case QCMAP_CM_EVENT_CRADLE_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_CRADLE_CONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->cradle_cb)
    {
        cbPtr->cradle_cb(cbPtr, QCMAP_MSGR_CRADLE_CONNECTED_V01);
    }

    break;
  case QCMAP_CM_EVENT_CRADLE_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_CRADLE_DISCONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->cradle_cb)
    {
        cbPtr->cradle_cb(cbPtr, QCMAP_MSGR_CRADLE_DISCONNECTED_V01);
    }
    break;
    case QCMAP_CM_EVENT_ETH_BACKHAUL_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_ETH_BACKHAUL_CONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->ethernet_cb)
    {
        cbPtr->ethernet_cb(cbPtr, QCMAP_MSGR_ETH_BACKHAUL_CONNECTED_V01);
    }
    break;
  case QCMAP_CM_EVENT_ETH_BACKHAUL_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_ETH_BACKHAUL_DISCONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->ethernet_cb)
    {
        cbPtr->ethernet_cb(cbPtr, QCMAP_MSGR_ETH_BACKHAUL_DISCONNECTED_V01);
    }
    break;
    case QCMAP_CM_EVENT_BT_BACKHAUL_CONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_BT_BACKHAUL_CONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->bt_wan_cb)
    {
      cbPtr->bt_wan_cb(cbPtr, QCMAP_MSGR_BT_WAN_MODE_CONNECTED_V01);
    }
    break;
  case QCMAP_CM_EVENT_BT_BACKHAUL_DISCONNECTED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_BT_BACKHAUL_DISCONNECTED\n",0,0,0);
    if (cbPtr && cbPtr->bt_wan_cb)
    {
        cbPtr->bt_wan_cb(cbPtr, QCMAP_MSGR_BT_WAN_MODE_DISCONNECTED_V01);
    }
    break;
  case QCMAP_CM_EVENT_WWAN_ROAMING_STATUS_CHANGED:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_WWAN_ROAMING_STATUS_CHANGED\n",0,0,0);
    roam_status = qcmap_cm_get_wwan_roam_status();
    if (cbPtr && cbPtr->wwan_roaming_cb)
    {
        cbPtr->wwan_roaming_cb(cbPtr, (qcmap_msgr_wwan_roaming_status_enum_v01)roam_status);
    }
    break;
  default:
    LOG_MSG_INFO1("QCMAP_CM_EVENT_UNKNOWN %08X\n", event,0,0);
    break;
  }

  //pBackhaulWWANObj would not be NULL if this callback is for WWAN event
  if((NULL != QcMapMgr) && IS_SOCKSV5_ALLOWED(QcMapMgr->target) && (NULL != QcMapBackhaulWWAN) &&
     (NULL != QcMapNatAlg) && (NULL != manager))
  {
    switch(event)
    {
      //WWAN IPv4 cases
      case QCMAP_CM_EVENT_WAN_CONNECTED:
      {
        //getbearertech
        if(QcMapBackhaulWWAN->GetBearerTech(&(QcMapBackhaulWWAN->dsi_net_hndl), &bearer_tech)
                                           != QCMAP_CM_SUCCESS)
        {
          LOG_MSG_ERROR("Couldn't get bearer tech.", 0, 0, 0);
          return;
        }
        LOG_MSG_INFO1("Bearer tech: %d\n", bearer_tech, 0, 0);

        //check bounds
        if((bearer_tech <= DSI_DATA_BEARER_TECH_UNKNOWN) ||
           (bearer_tech >= DSI_DATA_BEARER_TECH_MAX))
        {
          LOG_MSG_INFO1("Couldn't set SOCKSv5 backhaul as %s, bearer tech out of range",
          devname, 0, 0);
          return;
        }

        //filloutindex
        profile_index = (bearer_tech >= DSI_DATA_BEARER_TECH_WCDMA) ?
                        QcMapBackhaulWWAN->wwan_cfg.profile_id.v4.umts_profile_index :
                        QcMapBackhaulWWAN->wwan_cfg.profile_id.v4.cdma_profile_index;
        if(!(QcMapNatAlg->SetSOCKSv5Backhaul(profile_index, IPV4,
                                             QcMapBackhaulWWAN->dsi_device_name,
                                             QcMapBackhaulWWAN->pri_dns_ipv4_addr,
                                             QcMapBackhaulWWAN->sec_dns_ipv4_addr)))
        {
          LOG_MSG_INFO1("Couldn't set SOCKSv5 backhaul as %s for index %d",
                        QcMapBackhaulWWAN->dsi_device_name, profile_index, 0);
          LOG_MSG_INFO1("Couldn't set SOCKSv5 DNS Server IP as %s : %s for index %d",
                        QcMapBackhaulWWAN->pri_dns_ipv4_addr, QcMapBackhaulWWAN->sec_dns_ipv4_addr,
                        profile_index);
        } else {
          LOG_MSG_INFO1("Set SOCKSv5 backhaul as %s for index %d",
                        QcMapBackhaulWWAN->dsi_device_name, profile_index, 0);
          LOG_MSG_INFO1("Set SOCKSv5 DNS Server IP as %s : %s for index %d",
                        QcMapBackhaulWWAN->pri_dns_ipv4_addr, QcMapBackhaulWWAN->sec_dns_ipv4_addr,
                        profile_index);
        }

        break;
      }
      //WWAN IPv6 cases
      case QCMAP_CM_EVENT_WAN_IPv6_CONNECTED:
      {
        //getbearertech
        if(QcMapBackhaulWWAN->GetBearerTech(&(QcMapBackhaulWWAN->ipv6_dsi_net_hndl), &bearer_tech)
                                           != QCMAP_CM_SUCCESS)
        {
          LOG_MSG_ERROR("Couldn't get bearer tech.", 0, 0, 0);
          return;
        }
        LOG_MSG_INFO1("Bearer tech: %d\n", bearer_tech, 0, 0);

        //check bounds
        if((bearer_tech <= DSI_DATA_BEARER_TECH_UNKNOWN) ||
           (bearer_tech >= DSI_DATA_BEARER_TECH_MAX))
        {
          LOG_MSG_INFO1("Couldn't set SOCKSv5 backhaul as %s, bearer tech out of range",
          devname, 0, 0);
          return;
        }

        //filloutindex
        profile_index = (bearer_tech >= DSI_DATA_BEARER_TECH_WCDMA) ?
                        QcMapBackhaulWWAN->wwan_cfg.profile_id.v6.umts_profile_index :
                        QcMapBackhaulWWAN->wwan_cfg.profile_id.v6.cdma_profile_index;
        if(!(QcMapNatAlg->SetSOCKSv5Backhaul(profile_index, IPV6,
                                             QcMapBackhaulWWAN->ipv6_dsi_device_name,
                                             QcMapBackhaulWWAN->pri_dns_ipv6_addr,
                                             QcMapBackhaulWWAN->sec_dns_ipv6_addr)))
        {
          LOG_MSG_INFO1("Couldn't set SOCKSv5 backhaul as %s for index %d",
                        QcMapBackhaulWWAN->ipv6_dsi_device_name, profile_index, 0);
          LOG_MSG_INFO1("Couldn't set SOCKSv5 DNS Server IP as %s : %s for index %d",
                        QcMapBackhaulWWAN->pri_dns_ipv6_addr, QcMapBackhaulWWAN->sec_dns_ipv6_addr,
                        profile_index);
        } else {
          LOG_MSG_INFO1("Set SOCKSv5 backhaul as %s for index %d",
                        QcMapBackhaulWWAN->ipv6_dsi_device_name, profile_index, 0);
          LOG_MSG_INFO1("Set SOCKSv5 DNS Server IP as %s : %s for index %d",
                        QcMapBackhaulWWAN->pri_dns_ipv6_addr, QcMapBackhaulWWAN->sec_dns_ipv6_addr,
                        profile_index);
        }
        break;
      }

      //DISCONNECTED AND FAIL Cases
      case QCMAP_CM_EVENT_WAN_CONNECTING_FAIL:
      case QCMAP_CM_EVENT_WAN_DISCONNECTED:
      {
        //only delete iface if IPv6 not connected or v4 v6 iface not the same
        if((QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED) ||
           (strcmp(QcMapBackhaulWWAN->dsi_device_name, QcMapBackhaulWWAN->ipv6_dsi_device_name)))
        {
          if(!(QcMapNatAlg->DeleteSOCKSv5Backhaul(QcMapBackhaulWWAN->dsi_device_name)))
          {
            LOG_MSG_INFO1("Couldn't delete SOCKSv5 backhaul as %s",
                          QcMapBackhaulWWAN->dsi_device_name, 0, 0);
          } else {
            LOG_MSG_INFO1("Deleted SOCKSv5 backhaul %s ", QcMapBackhaulWWAN->dsi_device_name, 0, 0);
          }
        } else if(QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)
        {
          //update ip version to be only v6
          if(!(QcMapNatAlg->SetSOCKSv5WANIPVer(QcMapBackhaulWWAN->dsi_device_name, IPV6)))
          {
            LOG_MSG_INFO1("Couldn't set SOCKSv5 backhaul %s DNS IP version as %d",
                          QcMapBackhaulWWAN->dsi_device_name, IPV6, 0);
          } else {
            LOG_MSG_INFO1("Set SOCKSv5 backhaul %s DNS IP version as %d",
                          QcMapBackhaulWWAN->dsi_device_name, IPV6, 0);
          }
        }

        break;
      }

      case QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL:
      case QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED:
      {
        //only delete iface if IPv4 not connected or v4 v6 iface not the same
        if((QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED) ||
           (strcmp(QcMapBackhaulWWAN->dsi_device_name, QcMapBackhaulWWAN->ipv6_dsi_device_name)))
        {
          if(!(QcMapNatAlg->DeleteSOCKSv5Backhaul(QcMapBackhaulWWAN->ipv6_dsi_device_name)))
          {
            LOG_MSG_INFO1("Couldn't delete SOCKSv5 backhaul as %s",
                          QcMapBackhaulWWAN->ipv6_dsi_device_name, 0, 0);
          } else {
            LOG_MSG_INFO1("Deleted SOCKSv5 backhaul %s ", QcMapBackhaulWWAN->ipv6_dsi_device_name, 0,
                          0);
          }
        } else if(QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)
        {
          //update ip version to be only v4
          if(!(QcMapNatAlg->SetSOCKSv5WANIPVer(QcMapBackhaulWWAN->ipv6_dsi_device_name, IPV4)))
          {
            LOG_MSG_INFO1("Couldn't set SOCKSv5 backhaul %s DNS IP version as %d",
                          QcMapBackhaulWWAN->ipv6_dsi_device_name, IPV4, 0);
          } else {
            LOG_MSG_INFO1("Set SOCKSv5 backhaul %s DNS IP version as %d",
                          QcMapBackhaulWWAN->ipv6_dsi_device_name, IPV4, 0);
          }
        }
        break;
      }

      default:
      {
        break;
      }
    }
  }

  return;
}

/*======================================================
                             Class Definitions
  =====================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Creates QCMAP_ConnectionManager object

@parameters
none

@return
none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_ConnectionManager::QCMAP_ConnectionManager()
{
 /*All the initialization will be taken care in the init function*/
  return;
}

/*==========================================================*/
/*!
@brief
  Initializes QCMAP_ConnectionManager variables.

@parameters
none

@return
none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
void QCMAP_ConnectionManager::init()
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  char command[MAX_COMMAND_STR_LEN];
  pugi::xml_document xml_file;
  int fd = 0;

  /*-----------------------------------------------------------------------------*/
  this->qcmap_enable = true;
  this->dual_wifi_mode = false;
  /* Populate default Config */
  memset(&this->cfg, 0, sizeof(this->cfg));
  this->qcmap_cm_handle = 0;
  this->qcmap_tear_down_in_progress = false;
  this->sta_iface_index = QCMAP_LAN_INVALID_IFACE_INDEX;
  this->unused_wlan_if = QCMAP_LAN_INVALID_IFACE_INDEX;
  this->ap_dev_num1 = QCMAP_LAN_INVALID_IFACE_INDEX;
  this->ap_dev_num2 = QCMAP_LAN_INVALID_IFACE_INDEX;
  this->qcmap_cb_handle = NULL;
  pthread_mutex_init(&this->cm_mutex, NULL);
  pthread_cond_init(&this->cm_cond, NULL);

  // Initialize mobileap,wlan bootup values
  this->cfg.bootup_config.enable_wlan_at_bootup =false;
  this->cfg.bootup_config.enable_mobileap_at_bootup =false;
  this->cfg.bootup_config.enable_gsb_at_bootup =false;
  this->target =  DS_TARGET_INVALID;
  this->target = ds_get_target();

  this->cfg.packet_stats_config = false;
  this->packet_stats_enabled = false;
  this->data_path_opt_enable = false;
  this->dont_bringdown_backhaul = false;

  if(QCMAP_ConnectionManager::CheckDUNSoftAPMode())
  {
     this->DUN_SoftAP_enable_flag = true;
  }
  else
  {
     this->DUN_SoftAP_enable_flag = false;
  }

  this->sfe_loaded = false;

  this->revert_ap_sta_router_mode = false;
  this->fallback_ap_sta_router_mode = false;
  this->usb_clk_node_present = false;

  //Initialize Connected Devices Info to NULL
  memset (&this->conn_device_info, 0, sizeof(qcmap_cm_connected_info_t));
  memset (&this->addrList, NULL, sizeof(qcmap_addr_info_list_t));
  memset (&this->addrList_prev, NULL, sizeof(qcmap_addr_info_list_t));

  LOG_MSG_INFO1("Enabling conntrack helper",0,0,0);
  ds_system_call( "echo 1 > /proc/sys/net/netfilter/nf_conntrack_helper",
                  strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_helper"));

  bzero(&command,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("Enabling SIP segmentation",0,0,0);
  ds_system_call( "echo 1 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation",
                  strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation"));

  strlcpy(firewall_xml_path, QCMAP_FIREWALL_CONFIG, QCMAP_CM_MAX_FILE_LEN);

  /* Read configuration from XML file. */
  if (ReadQCMAPConfigFromXML(this->xml_path))
  {
    LOG_MSG_INFO1("QCMAP Read XML OK",0,0,0);
  }
  else
  {
    /* Read Config from XML failed. Use defaults. */
    LOG_MSG_INFO1("QCMAP Read XML failed.",0,0,0);
    LOG_MSG_ERROR("Unable to load XML file.Creating XML with defaults",0,0,0);
    ds_system_call("echo QCMAP Read XML failed. Writing XML with defaults > /dev/kmsg",
                   strlen("echo QCMAP Read XML failed. Writing XML with defaults > /dev/kmsg"));
    QCMAP_ConnectionManager::WriteDefaultsToXML();
    ReadQCMAPConfigFromXML(this->xml_path); //Read XML file again.
  }

  if (!xml_file.load_file(firewall_xml_path))
  {
    strlcpy(firewall_xml_path, QCMAP_FIREWALL_CONFIG, QCMAP_CM_MAX_FILE_LEN);

    LOG_MSG_ERROR("Unable to load XML file.Creating XML with defaults",0,0,0);
    QCMAP_Firewall::WriteDefaultsToFirewallXML();
  }

  /* If the factory configuration file doesnot exist, copy the primary
     configuration file to factory configuration */
  if( !doesFileExist( FACTORY_CONF ))
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "cp %s %s ",this->xml_path,FACTORY_CONF);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",FACTORY_CONF);
    ds_system_call(command, strlen(command));
  }

  if(IS_SOCKSV5_ALLOWED(this->target) && !doesFileExist(FACTORY_SOCKSV5_CONF))
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "cp %s %s ", SOCKSV5_DEFAULT_CONF, FACTORY_SOCKSV5_CONF);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",FACTORY_SOCKSV5_CONF);
    ds_system_call(command, strlen(command));
  }

  if(IS_SOCKSV5_ALLOWED(this->target) && !doesFileExist(FACTORY_SOCKSV5_AUTH))
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "cp %s %s ", SOCKSV5_DEFAULT_AUTH, FACTORY_SOCKSV5_AUTH);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",FACTORY_SOCKSV5_AUTH);
    ds_system_call(command, strlen(command));
  }

  if(!doesFileExist(FACTORY_L2TP_CONF))
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "cp %s %s ", L2TP_CFG_XML, FACTORY_L2TP_CONF);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",FACTORY_L2TP_CONF);
    ds_system_call(command, strlen(command));
  }

  this->is_ssr_in_progress = false;
  memset (&this->dyn_clk_info, 0, sizeof(qcmap_cm_dynamic_clock_info_t));

  if (this->target == DS_TARGET_LYKAN || this->target == DS_TARGET_LE_MDM9x06)
  {
    fd = open(USB_CLK_UPDATE_NODE,O_WRONLY);
    if(fd < 0)
    {
      LOG_MSG_ERROR("Error in opening %s", USB_CLK_UPDATE_NODE,0,0);
      this->usb_clk_node_present = false;
      return;
    }
    else
    {
      LOG_MSG_INFO1("USB Clock file node present %s", USB_CLK_UPDATE_NODE,0,0);
      this->usb_clk_node_present = true;
      close(fd);
      /* For MDM9x06, USB vote should be NOM. */
      if (this->target == DS_TARGET_LE_MDM9x06)
        this->SetUSBClockFreq(true);
    }
  }
  return;
}

/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroying the QCMAP Object.

@parameters
  None

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

QCMAP_ConnectionManager::~QCMAP_ConnectionManager()
{
  QCMAP_MediaService* QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  LOG_MSG_INFO1("Disabling SIP segmentation",0,0,0);
  ds_system_call( "echo 0 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation",
                  strlen("echo 0 > /proc/sys/net/netfilter/nf_conntrack_enable_sip_segmentation"));

  QCMAP_LAN_Manager::DestroyInstance();

  if (QcMapMediaServiceMgr)
    delete QcMapMediaServiceMgr;
  QCMAP_CM_LOG_FUNC_ENTRY();
  pthread_mutex_destroy(&this->cm_mutex);
  pthread_cond_destroy(&this->cm_cond);

  if (this->IsDynamicClockVotingNeeded())
  {
    if (this->CheckUsbClockAtNom())
    {
      this->SetUSBClockFreq(false);
    }

    this->usb_clk_node_present = false;
  }

  /* Clean-up Backhual objects and map */
  QCMAP_Backhaul *QcMapBackhaul;
  if (QCMAP_ConnectionManager::QCMAP_Backhaul_Hash != NULL)
  {
    DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
    START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
    while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
    {
      QcMapBackhaul = GET_BACKHAUL_OBJ_AND_INC;
      delete QcMapBackhaul;
    }
    delete QCMAP_ConnectionManager::QCMAP_Backhaul_Hash;
  }
  flag=false;
  object=NULL;
}

/*======================================================
  FUNCTION CleanIPtables
======================================================*/
/*!
@brief
  Clean IPTables NAT, Filter, Mangle and Raw tables

@parameters
   void

@return
   void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================*/
void QCMAP_ConnectionManager::CleanIPtables(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_NATALG *QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT();

  LOG_MSG_INFO1("CleanIPtables()",0,0,0);

  if (QcMapNatAlg)
  {
    QcMapNatAlg->CleanIPv4NatTable();
    QcMapNatAlg->CleanIPv4FilterTableFwdChain();
    QcMapNatAlg->CleanIPv4FilterTableInChain();
  }
  if (QcMapFirewall)
    QcMapFirewall->CleanIPv4MangleTable();
}

/*=====================================================
 FUNCTION CleanEbtables
======================================================*/
/*!
@brief
  Clean EbTables NAT, Filter and broute tables

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================*/
void QCMAP_ConnectionManager::CleanEbtables(void)
{
  char command[MAX_COMMAND_STR_LEN];
  FILE *access_fd = NULL;
  LOG_MSG_INFO1("CleanEbtables()",0,0,0);

  access_fd = fopen(QCMAP_MULTIPLE_SYSTEM_CALL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
     LOG_MSG_ERROR("Cannot open %s file for writing",QCMAP_MULTIPLE_SYSTEM_CALL_FILE,
                   0,0);
     return;
  }
  fprintf( access_fd,"#!/bin/sh \n");

  fprintf( access_fd,"ebtables -t nat --flush \n");
  fprintf( access_fd,"ebtables -t filter --flush \n");
  fprintf( access_fd,"ebtables -t broute --flush \n");
  fclose(access_fd);

  /* Changing permissions */
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
}

/*=====================================================
  FUNCTION CheckDUNSoftAPMode
=====================================================*/
/*!
@brief
  Checks if we are in DUN+SoftAP Mode or not.

@parameters
  None.

@return
  true  - If we are in DUN+SoftAP mode.
  false - if we are not in DUN+SoftAP mode.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*===================================================*/
boolean QCMAP_ConnectionManager:: CheckDUNSoftAPMode(void)
{
  FILE *cmd = NULL;
  char mode = 0;
  char process[MAX_COMMAND_STR_LEN];
  snprintf(process, MAX_COMMAND_STR_LEN, "cat %s", SOFTAP_W_DUN_PATH);
  cmd = popen(process, "r");
  if(cmd)
  {
    mode = fgetc(cmd);
    pclose(cmd);
  }

  if ( mode == 'Y' )
  {
    LOG_MSG_INFO1("In DUN+SoftAP mode.", 0, 0, 0);
    return TRUE;
  }
  else
  {
    LOG_MSG_INFO1(" Not in DUN+SoftAP mode.", 0, 0, 0);
    return FALSE;
  }
}


/*=====================================================
  FUNCTION Enable
======================================================*/
/*!
@brief
  Enables MobileAP based on the configuration.
  As a part of enable mobileap, registers mobileap event callback.
  It will bring up RmNet between Q6 and A5.
  If AutoConnect is enabled then initiates connect to backhaul.
  If station mode is configured then enables it.

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
boolean QCMAP_ConnectionManager::Enable
(
  int *handle,
  void *cb_user_data,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_errno;
  char Kernel_ver[KERNEL_VERSION_LENGTH] = {0};
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_L2TP* QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(false);
  QCMAP_Virtual_LAN* QcMapVLANMgr = QCMAP_Virtual_LAN::Get_Instance(false);
  qcmap_msgr_vlan_config_v01 vlan_config_arr[QCMAP_MSGR_MAX_VLAN_ENTRIES_V01];
  uint32 len = 0;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;
  std::vector<int16_t> bridge_vlan_vec;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  bool is_ipa_offloaded = false;

  if(!QcMapMgr)
    return false;

  ds_system_call("echo QCMAP:Enable mobileap > /dev/kmsg",
          strlen("echo QCMAP:Enable mobileap > /dev/kmsg"));
  QCMAP_CM_LOG_FUNC_ENTRY();

  ((qmi_qcmap_msgr_softap_handle_type *)cb_user_data)->Mgr = QcMapMgr;

  QcMapMgr->qcmap_cm_handle = *handle =qcmap_cm_enable(&QcMapMgr->cfg,
                                                       QCMAP_ConnectionManager_callback,
                                                       cb_user_data,
                                                       &qcmap_cm_errno,
                                                       qmi_err_num);

  if (QcMapMgr->qcmap_cm_handle <= 0)
  {
    LOG_MSG_ERROR("QCMAP Connection failed \n",0,0,0);
    return false;
  }

  QCMAP_ConnectionManager::is_cm_enabled = true;
  if(QcMapMgr->cfg.packet_stats_config)
  {
    QcMapMgr->packet_stats_enabled = true;

    ds_system_call(CREATE_IPTABLES_CHAIN, strlen(CREATE_IPTABLES_CHAIN));

    ipv4_tag_pool = (uint8*)ds_malloc(sizeof(uint8) * MAX_MARK_VALUE);
    if( ipv4_tag_pool == NULL )
    {
      LOG_MSG_ERROR("Error in allocating memory for ipv4tagpool",0,0,0);
      return false;
    }
    memset(ipv4_tag_pool, 0, sizeof(uint8) * MAX_MARK_VALUE);
    ipv6_tag_pool = (uint8*)ds_malloc(sizeof(uint8) * MAX_MARK_VALUE);
    if( ipv6_tag_pool == NULL )
    {
      LOG_MSG_ERROR("Error in allocating memory for ipv4tagpool",0,0,0);
      return false;
    }
    memset(ipv6_tag_pool, 0, sizeof(boolean) * MAX_MARK_VALUE);
  }

  /* Store all the Client/CB related information */
  QcMapMgr->qcmap_cb_handle =
         (void *)((qmi_qcmap_msgr_softap_handle_type *)cb_user_data)->cb_ptr;

  LOG_MSG_INFO1("QCMAP Enabled this->qcmap_cm_handle=%x \n",
                QcMapMgr->qcmap_cm_handle,0,0);

  ds_system_call("echo QCMAP:Enable mobileap done > /dev/kmsg",
          strlen("echo QCMAP:Enable mobileap done > /dev/kmsg"));

#ifdef FEATURE_DATA_TARGET_MDM9607

  ds_system_call("echo 16 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold",
          strlen("echo 16 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold"));

#endif

  if((DS_TARGET_LYKAN == QcMapMgr->target) ||
     ((DS_TARGET_LE_ATLAS == QcMapMgr->target || DS_TARGET_LE_STINGRAY == QcMapMgr->target)
       && QcMapMgr->dual_wifi_mode))
  {

/* To make sure that the dpo binary is not called during reboot*/
    if((QcMapMgr->target == DS_TARGET_LE_ATLAS) &&
                !(QcMapMgr->data_path_opt_enable))
    {
    /* enable data path optimizer when dual wifi functionalities are enabled */
      if (QcMapMgr->SetDataPathOptManagerStatus(true, qmi_err_num))
      {
        LOG_MSG_INFO1("Set Data Path Optimizer when in dual wifi mode",0,0,0);
      }
      else
      {
        LOG_MSG_ERROR("Set Data Path optimizer failed in dual wifi mode",0,0,0);
        return false;
      }
    }

    else
    {
      /*enable bridge_netfilter module first */
      snprintf(command,MAX_COMMAND_STR_LEN,
              "insmod /usr/lib/modules/$(uname -r)/kernel/net/bridge/br_netfilter.ko");
       ds_system_call(command, strlen(command));
       snprintf(command,MAX_COMMAND_STR_LEN,
               "insmod /usr/lib/modules/$(uname -r)/extra/shortcut-fe.ko");
       ds_system_call(command, strlen(command));
       snprintf(command,MAX_COMMAND_STR_LEN,
               "insmod /usr/lib/modules/$(uname -r)/extra/shortcut-fe-ipv6.ko");
       ds_system_call(command, strlen(command));
       snprintf(command,MAX_COMMAND_STR_LEN,
               "insmod /usr/lib/modules/$(uname -r)/extra/shortcut-fe-cm.ko");
      ds_system_call(command, strlen(command));
     }

    if (QcMapMgr->packet_stats_enabled)
    {
      QcmapSFEv6NLInit();
      QcmapSFEv4NLInit();
      ds_system_call("echo 1 > /proc/sys/debug_v6/sfe_v6/packet_stats_on",
          strlen("echo 1 > /proc/sys/debug_v6/sfe_v6/packet_stats_on"));
      ds_system_call("echo 1 > /proc/sys/debug/sfe/packet_stats_on",
          strlen("echo 1 > /proc/sys/debug/sfe/packet_stats_on"));
      createSFEDumpFile();
    }
    QcMapMgr->sfe_loaded = true;
    ds_system_call("echo 0 > /proc/sys/debug/sfe/aggr_on",
          strlen("echo 0 > /proc/sys/debug/sfe/aggr_on"));

    ds_system_call("echo 0 > /proc/sys/debug_v6/sfe_v6/v6_aggr_on",
          strlen("echo 0 > /proc/sys/debug_v6/sfe_v6/v6_aggr_on"));
  }

  //Check mobileap_cfg.xml and enable SOCKSv5 if necessary
  if(IS_SOCKSV5_ALLOWED(QcMapMgr->target))
  {
    if(QCMAP_NATALG::GetSOCKSv5ProxyEnableCfg(qmi_err_num))
    {
      if(!(QCMAP_NATALG::EnableSOCKSv5Proxy(qmi_err_num)))
      {
        LOG_MSG_INFO1("Failed to enable SOCKSv5 proxy", 0, 0, 0);
      }
    }
  }

  if(IS_VLAN_ALLOWED(QcMapMgr->target))
  {
    //Add bridges
    if(lan_mgr)
    {
      //create bridges
      bridge_vlan_vec = lan_mgr->GetBridgeIDs();
      for(std::vector<int16_t>::iterator it = bridge_vlan_vec.begin(); it != bridge_vlan_vec.end();
          ++it)
      {
        LOG_MSG_INFO1("There is a VLAN %d configured", *it, 0, 0);
        if(((lan_obj = lan_mgr->GetLANBridge(*it)) != NULL) &&
           (GET_DEFAULT_LAN_OBJECT() != lan_obj))
        {
          lan_obj->InitBridge();
        }
      }
    }

    //Add VLANs, from mobileap_cfg.xml
    if(QcMapVLANMgr)
    {
      if(QcMapVLANMgr->GetVLANConfig(vlan_config_arr, &len, qmi_err_num))
      {
        for(int i = 0; i < len; i++)
        {
          if(!(QcMapVLANMgr->SetVLANConfig(vlan_config_arr[i], false, qmi_err_num, &is_ipa_offloaded)))
          {
            LOG_MSG_INFO1("Failed to set VLAN config", 0, 0, 0);
          }
          else if ((vlan_config_arr[i].ipa_offload == 1) && !(is_ipa_offloaded))
          {
            LOG_MSG_ERROR("Set VLAN config without offload as IPA offload count already reached max", 0, 0, 0);
          }
        }
      } else {
        LOG_MSG_INFO1("Failed to GetVLANConfig", 0, 0, 0);
      }
    }

    if (IS_L2TP_ALLOWED(QcMapMgr->target))
    {
      //Install all L2TP tunnels from config xml
      if(QcMapL2TPMgr)
      {
        if(!(QcMapL2TPMgr->ReadInstallL2TPConfigFromXML()))
        {
          LOG_MSG_INFO1("Failed to delete all L2TP tunnels", 0, 0, 0);
        }
      }
    }
  }

  return true;
}


/*=====================================================
  FUNCTION Disable
======================================================*/
/*!
@brief
  Disable Mobile ap will bring down the backhaul and wlan.

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

boolean QCMAP_ConnectionManager::Disable
(
  int *err_num,
  qmi_error_type_v01 *qmi_err_num
)
{
  return QCMAP_ConnectionManager::Disable_Ext(err_num, qmi_err_num, false);
}
/*=====================================================
  FUNCTION Disable_Ext
======================================================*/
/*!
@brief
  Disable Mobile ap will bring down the backhaul and wlan.

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

boolean QCMAP_ConnectionManager::Disable_Ext
(
  int *err_num,
  qmi_error_type_v01 *qmi_err_num,
  boolean sig_term_flag
)
{
  int ret = 0, qcmap_cm_errno;
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                             GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_L2TP* QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(false);
  QCMAP_Virtual_LAN* QcMapVLANMgr = QCMAP_Virtual_LAN::Get_Instance(false);
  qcmap_msgr_vlan_config_v01 vlan_config_arr[QCMAP_MSGR_MAX_VLAN_ENTRIES_V01];
  uint32 len = 0;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;
  std::vector<int16_t> bridge_vlan_vec;
  qmi_error_type_v01 qmi_err;

  LOG_MSG_INFO1("QCMAP_ConnectionManager::Disable",0,0,0);
  QCMAP_CM_LOG_FUNC_ENTRY();
  boolean retval;
  if(!QcMapMgr)
    return false;
  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }


  if(QcMapMgr->packet_stats_enabled)
  {
    QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_MOBILEAP_DISABLED_V01,
                                  QcMapMgr->conn_device_info.numOfNodes,NULL);
    QcMapMgr->packet_stats_enabled =  false;
    ds_system_call(CLEANUP_IPTABLES_CHAIN, strlen(CLEANUP_IPTABLES_CHAIN));

    ds_free(ipv4_tag_pool);
    ds_free(ipv6_tag_pool);

    if(QcMapMgr->sfe_loaded)
    {
      /*In this case sfe is going to be unloaded so
      sfe nodes will be deleted as part of cleanup process
      No need ot send netlink to delete sfe nodes.*/
      close(sfev6_nl_sock_fd);
      close(sfev4_nl_sock_fd);
      removeSFEDumpFile();
    }
  }

  QcMapMgr->qcmap_tear_down_in_progress = true;
  pthread_cond_signal(&QcMapMgr->cm_cond);

  ret = qcmap_cm_disable(QcMapMgr->qcmap_cm_handle, &qcmap_cm_errno, qmi_err_num);

  if (ret < 0)
  {
    LOG_MSG_ERROR("QCMAP Disable fails: %d\n", qcmap_cm_errno,0,0);
    retval = false;
  }
  else
  {
    *err_num = qcmap_cm_errno;
    LOG_MSG_ERROR("QCMAP Disable in progress\n",0,0,0);
    retval = true;
  }

  QCMAP_ConnectionManager::is_cm_enabled = false;

  if (sig_term_flag == true)
  {
    if ((QcMapWLANMgr) && (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01))
      QCMAP_WLAN::DisableWLAN(qmi_err_num);
  }
  else
  {
    /*Do not unload WLAN driver if Always on WLAN feature is enabled*/
    if ((QcMapWLANMgr) && (QcMapWLANMgr->always_on_wlan == false) &&
        (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01))
    {
      QCMAP_WLAN::DisableWLAN(qmi_err_num);
    }
  }

  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
      QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
    QcMapBackhaulCradleMgr->DisableCradle(qmi_err_num);

  if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() ||
      QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
    QcMapBackhaulEthMgr->DisableEthBackhaul(qmi_err_num);

  //Disable SOCKSv5, do not update mobileap_cfg.xml
  if(IS_SOCKSV5_ALLOWED(QcMapMgr->target))
  {
    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "killall -s 15 qti_socksv5; sleep 1; rmmod tcp_splice");
    ds_system_call(command, strlen(command));
  }

  if(IS_L2TP_ALLOWED(QcMapMgr->target) && IS_VLAN_ALLOWED(QcMapMgr->target))
  {
    //Delete all L2TP tunnels
    if(QcMapL2TPMgr)
    {
      if(!(QcMapL2TPMgr->DeleteAllTunnels()))
      {
        LOG_MSG_INFO1("Failed to delete all L2TP tunnels", 0, 0, 0);
      }
    }

    //Delete VLANs, do not update mobileap_cfg.xml
    if(QcMapVLANMgr)
    {
      if(QcMapVLANMgr->GetVLANConfig(vlan_config_arr, &len, qmi_err_num))
      {
        for(int i = 0; i < len; i++)
        {
          memset(command, 0, sizeof(command));
          snprintf(command, sizeof(command), "vconfig rem %s.%d", vlan_config_arr[i].local_iface,
                   vlan_config_arr[i].vlan_id);
          ds_system_call(command, strlen(command));
        }
      } else {
        LOG_MSG_INFO1("Failed to GetVLANConfig", 0, 0, 0);
      }
    }

    //Delete all extraneous bridges
    if(lan_mgr)
    {
      bridge_vlan_vec = lan_mgr->GetBridgeIDs();
      for(std::vector<int16_t>::iterator it = bridge_vlan_vec.begin(); it != bridge_vlan_vec.end();
          ++it)
      {
        LOG_MSG_INFO1("There is a VLAN %d configured", *it, 0, 0);
        if(((lan_obj = lan_mgr->GetLANBridge(*it)) != NULL) &&
           (GET_DEFAULT_LAN_OBJECT() != lan_obj))
        {
          lan_obj->DelBridge();
        }
      }
    }
  }


#ifdef FEATURE_DATA_TARGET_MDM9607

  ds_system_call("echo 1 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold",
          strlen("echo 1 > /sys/module/msm_rmnet_bam/parameters/pkt_threshold"));

#endif


  if((DS_TARGET_LYKAN == QcMapMgr->target) ||
     ((DS_TARGET_LE_ATLAS == QcMapMgr->target || DS_TARGET_LE_STINGRAY == QcMapMgr->target)
       && QcMapMgr->dual_wifi_mode))
  {
    ds_system_call("rmmod br_netfilter",
            strlen("rmmod br_netfilter"));
    ds_system_call("rmmod shortcut_fe_cm",
            strlen("rmmod shortcut-fe_cm"));

    ds_system_call("rmmod shortcut_fe_ipv6",
            strlen("rmmod shortcut_fe_ipv6"));

    ds_system_call("rmmod shortcut_fe",
            strlen("rmmod shortcut_fe"));
    QcMapMgr->sfe_loaded = false;
  }

  return retval;
}

/*=====================================================
  FUNCTION DisableHandle
======================================================*/
/*!
@brief
 Initialize mobileap handle to zero.

@return
  None.

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_ConnectionManager::DisableHandle()
{
  this->qcmap_cm_handle = 0;

  QCMAP_NATALG *QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT();

  /* Flush the forward chain. This is needed to remove rule added
  * to block WWAN traffic.
  */
  if (QcMapNatAlg)
  {
    QcMapNatAlg->CleanIPv4FilterTableFwdChain();
    QcMapNatAlg->CleanIPv6FilterTableFwdChain();
  }
}



/*=====================================================
  FUNCTION IsInterfaceEnabled
======================================================*/
/*!
@brief
 This function returns if the interface is enabled.

@parameters
  Interface whose status to be checked.

@return
None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
int QCMAP_ConnectionManager::IsInterfaceEnabled(char *interface)
{
  struct ifreq ifr;
  int sock;
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock < 0)
  {
    LOG_MSG_ERROR("%s: Unable to  socket %d", __func__, errno, 0);
    return -1;
  }
  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));
  if (ioctl(sock,SIOCGIFINDEX,&ifr) < 0)
  {
    return -1;
  }
  LOG_MSG_INFO1( "QCMAP ifr.ifr_ifindex =%d ",ifr.ifr_ifindex,0,0);
  close(sock);
  return ifr.ifr_ifindex;
}


/*======================================================
  FUNCTION WriteConfigToXML
======================================================*/
/*!
@brief
  Writes the mobileap configuration file with the latest configuration values.

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
/*====================================================*/

boolean QCMAP_ConnectionManager::WriteConfigToXML
(
  qcmap_update_xml type,
  pugi::xml_document *xml_file
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  bzero(&command,MAX_COMMAND_STR_LEN);

  if (type == UPDATE_MOBILEAP_XML)
  {
    xml_file->save_file(TEMP_MOBILEAP_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_MOBILEAP_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_MOBILEAP_CFG ,
              QcMapMgr->xml_path);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_MOBILEAP_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",QcMapMgr->xml_path);
    ds_system_call(command, strlen(command));
  }
  else if (type == UPDATE_FIREWALL_XML)
  {
    xml_file->save_file(TEMP_FIREWALL_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_FIREWALL_CFG);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_FIREWALL_CFG ,
             QcMapMgr->firewall_xml_path);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_FIREWALL_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",QcMapMgr->firewall_xml_path);
    ds_system_call(command, strlen(command));
  }
  else if (type == UPDATE_IPACFG_XML)
  {
    /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
    xml_file->save_file(TEMP_IPACM_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_IPACM_CFG);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_IPACM_CFG ,
             IPA_XML_PATH);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_IPACM_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",IPA_XML_PATH);
    ds_system_call(command, strlen(command));
#endif /* FEATURE_DATA_TARGET_MDM9607 */
  }
  else
  {
    LOG_MSG_ERROR("Incorrect xml File type",0,0,0);
    return false;
  }
  return true;
}


/*======================================================
  FUNCTION StartMcastDaemon
======================================================*/
/*!
@brief
  Starts pimd service, which provides protocol independent multicast service.

@parameters
  void

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_ConnectionManager::StartMcastDaemon(void)
{
  ds_system_call("pimd", strlen("pimd"));
}

/*======================================================
  FUNCTION StopMcastDaemon
======================================================*/
/*!
@brief
  Stops pimd service.

@parameters
  void

@return
  void
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_ConnectionManager::StopMcastDaemon(void)
{
  ds_system_call("killall -15 pimd", strlen("killall -15 pimd"));
}


/*======================================================
  FUNCTION GetGatewayIP
======================================================*/
/*!
@brief
  Gets the Gateway IP assigned to the STA interface.

@parameters
  uint32 *gwIP

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::GetGatewayIP(uint32 *gwIP)
{
  FILE *fp = NULL;
  char gatewayIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  in_addr addr;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qmi_error_type_v01 qmi_err_num;
  int ret, retries;

  if ( gwIP == NULL )
  {
     LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
     return false;
  }

  if (NULL == QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("Backhaul not available", 0, 0, 0);
    return false;
  }

  memset(gatewayIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(devname, 0, sizeof(devname));

  /*----------------------------------------------------------
    Get the default gateway info from dhcpcd-<iface>.info.
    The info file may take few seconds to populate.
    Hence add a retry mechanism
  ----------------------------------------------------------*/
  for ( retries=0; retries < MAX_DHCPCD_INFO_RETRY; retries++)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,"rm -rf %s", TEMP_GATEWAY_FILE);
    ds_system_call(command, strlen(command));

    if(QcMapBackhaulMgr->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V4_V01, &qmi_err_num) == 0)
    {
      LOG_MSG_ERROR("Couldn't get ipv4 rmnet name. error %d\n", qmi_err_num,0,0);
      return false;
    }

    snprintf(command, MAX_COMMAND_STR_LEN,
             "cat /var/run/dhcpcd-%s.info | grep GATEWAYS | tr -d \"'\" | awk '{split($0,a,/=/); print a[2]}' > %s", devname, TEMP_GATEWAY_FILE);

    ds_system_call(command, strlen(command));
    fp = fopen(TEMP_GATEWAY_FILE, "r");

    if ( fp == NULL )
    {
      LOG_MSG_ERROR("Error opening Gateway IP file err : %d. Retrying\n", errno, 0, 0);
      sleep(DHCPCD_INFO_RETRY_DELAY);
      continue;
    }

    if (fscanf(fp, "%s", gatewayIP) != 1)
    {
      LOG_MSG_ERROR("Error reading Getway IP file err: %d.Retrying\n", errno, 0, 0);
      fclose(fp);
      sleep(DHCPCD_INFO_RETRY_DELAY);
      continue;
    }
    else
    {
      fclose(fp);
      break;
    }
  }

  if (retries == MAX_DHCPCD_INFO_RETRY)
  {
    LOG_MSG_ERROR("Reached max number of retries in getting default gateway", 0, 0, 0);
    return false;
  }

  memset(&addr,0,sizeof(in_addr));
  if (inet_aton(gatewayIP, &addr))
  {
    *gwIP= addr.s_addr;
  }
  else
  {
    LOG_MSG_ERROR("Invalid Getway IP: %d.\n", errno,0,0);
    return false;
  }

  ds_log_med("STA Gateway IP: 0x%s\n", inet_ntoa(addr));

  return true;
}

/*=====================================================
  FUNCTION readable_addr
======================================================*/
/*!
@brief
  Converts the address iin intgeger to readable string.

@parameters
  int            domain
  const uint32_t *addr
  char           *str

@return
  -1 - on failure
   0 - on success
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
int QCMAP_ConnectionManager::readable_addr
(
  int domain,
  const uint32_t *addr,
  char *str
)
{
  if ( inet_ntop(domain, (void *)addr, str, INET6_ADDRSTRLEN) == NULL )
  {
    LOG_MSG_ERROR("\n Not in presentation format \n",0,0,0);
    return -1;
  }
  return 0;
}


/*======================================================
  FUNCTION ReadQCMAPConfigFromXML
======================================================*/
/*!
@brief
  Reads the QCMAP Configuration from XML

@parameters
  int            domain
  const uint32_t *addr
  char           *str

@return
  -1 - on failure
   0 - on success
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/

boolean QCMAP_ConnectionManager::ReadQCMAPConfigFromXML(char *xml_path)
{
  pugi::xml_document      xml_file;
  pugi::xml_node          root;
  pugi::xml_node          pdnNode;
  pugi::xml_node          concurrentCfgNode;
  QCMAP_Backhaul         *pBackhaul;
  profile_handle_type_v01 profile_handle;
  int value_debug = 0;

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!xml_file.load_file(xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
  if (root != NULL)
  {
    /* Read default APWANCfg */
    pdnNode = root;
    profile_handle = atol(pdnNode.child(ProfileHandle_TAG).child_value());

    /* Set Default Profile Handle */
    QCMAP_Backhaul::defaultProfileHandle = profile_handle;
    LOG_MSG_INFO1("Read default profile %d", profile_handle, 0, 0);

    /* Create Backhaul, WWAN Object.
     * This is going to be default Backhaul object, for default profile.
     */
    QCMAP_Backhaul::CreateBackhualAndRelatedObjects(profile_handle, (void *) &pdnNode);
  }

  /* Read Concurrent Config */
  concurrentCfgNode = xml_file.child(System_TAG).child(ConcurrentMobileAPCfg_TAG);
  if (concurrentCfgNode != NULL)
  {
    for (pdnNode = concurrentCfgNode.child(Cfg_TAG);
            pdnNode != NULL;
            pdnNode = pdnNode.next_sibling(Cfg_TAG))
    {
      /* If APWanCfg is not found in ConcurrentConfig then skip */
      if (pdnNode.child(MobileAPWanCfg_TAG) == NULL)
      {
        continue;
      }

      profile_handle = atol(pdnNode.child(MobileAPWanCfg_TAG).
                                    child(ProfileHandle_TAG).child_value());

      /* Create Backhaul, WWAN Object.
       * This is going to be concurrent Backhaul Objects.
       */
      QCMAP_Backhaul::CreateBackhualAndRelatedObjects(profile_handle,
                                        (void *) &(pdnNode.child(MobileAPWanCfg_TAG)));
      LOG_MSG_INFO1("Read concurrent profile %d", profile_handle, 0, 0);
    }
  }
  else
  {
    LOG_MSG_INFO1("Concurrent profile NOT found!!!", 0, 0, 0);
  }

  QCMAP_Firewall::ResetNetDevInXML();
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  strlcpy(this->firewall_xml_path,root.child(Firewall_TAG).child_value(),
          QCMAP_CM_MAX_FILE_LEN);

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  this->wlan_restart_delay = atoi(root.child(WlanRestartDelay_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPBootUp_TAG);
  this->cfg.bootup_config.enable_mobileap_at_bootup =
                             atoi(root.child(MobileAPEnable_TAG).child_value());
  this->cfg.bootup_config.enable_wlan_at_bootup =
                                atoi(root.child(WLANEnable_TAG).child_value());
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(GSB_CONFIG_TAG);
  this->cfg.bootup_config.enable_gsb_at_bootup =
                              atoi(root.child(GSB_BOOTUP_CFG_TAG).child_value());
  if(this->cfg.bootup_config.enable_gsb_at_bootup)
    ds_system_call("echo QCMAP:GSB enabled > /dev/kmsg",
                   strlen("echo QCMAP:GSB enabled > /dev/kmsg"));
  else
    ds_system_call("echo QCMAP:GSB disabled > /dev/kmsg",
        strlen("echo QCMAP:GSB disabled > /dev/kmsg"));
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PacketStats_TAG);
  this->cfg.packet_stats_config  =
                                atoi(root.child(PacketStatsCfg_TAG).child_value());
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPBootUp_TAG);
  this->data_path_opt_enable = (boolean) atoi(root.child(DataPathOpt_Tag).child_value());
  if (this->data_path_opt_enable)
  {
    value_debug = QCMAP_ConnectionManager::DataPathOptInit(this->data_path_opt_enable);
    LOG_MSG_INFO1("Enabling data path optimization ret val: %d,%d",value_debug, this->data_path_opt_enable,0);
  }

  return true;
}


/*=====================================================
  FUNCTION GetMobileAPhandle
======================================================*/
/*!
@brief
  Gets the status of mobileap handle.

@parameters
  qcmap_msgr_get_firewall_entry_resp_msg_v01 *resp_msg
  uint32_t handle

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
int  QCMAP_ConnectionManager::GetMobileAPhandle(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (QcMapMgr)
    return QcMapMgr->qcmap_cm_handle;
  else
    return false;
}


/*======================================================
  FUNCTION SetQCMAPBootupConfig
======================================================*/
/*!
@brief
  Set QCMAP Bootup Configuration

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
/*=====================================================*/
boolean QCMAP_ConnectionManager::SetQCMAPBootupConfig
(
qcmap_msgr_bootup_flag_v01 mobileap_enable,
qcmap_msgr_bootup_flag_v01 wlan_enable,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!QcMapMgr)
  {
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  LOG_MSG_INFO1( "QCMAP_ConnectionManager::SetQCMAPBootupConfig()\
                 Enter  mobileap_enable =%d, wlan_enable =%d",
                 mobileap_enable, wlan_enable,0 );

  // Write to MobileAP Bootup Flag
  if (mobileap_enable != QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01)
  {
    QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup =
               (mobileap_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01)? true : false;

    QCMAP_ConnectionManager::GetSetBootupConfigFromXML
                       (CONFIG_MOBILEAP,SET_VALUE,
                        QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup);
  }

  // Write to WLAN Bootup Flag
  if (wlan_enable != QCMAP_MSGR_BOOTUP_FLAG_MIN_ENUM_VAL_V01)
  {
    QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup =
                   (wlan_enable == QCMAP_MSGR_ENABLE_ON_BOOT_V01)? true : false;

    QCMAP_ConnectionManager::GetSetBootupConfigFromXML
                            (CONFIG_WLAN,SET_VALUE,
                             QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup);
  }
  return true;
}


/*======================================================
  FUNCTION GetQCMAPBootupConfig
======================================================*/
/*!
@brief
  Get QCMAP Bootup Configuration

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
/*=====================================================*/
boolean QCMAP_ConnectionManager::GetQCMAPBootupConfig
(
  qcmap_msgr_bootup_flag_v01 *mobileap_enable,
  qcmap_msgr_bootup_flag_v01 *wlan_enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!QcMapMgr)
  {
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }
   LOG_MSG_INFO1("QCMAP_ConnectionManager::SetQCMAPBootupConfig() Enter  "
                 "CFG mobileap_enable =%d, wlan_enable =%d",
                 QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup,
                 QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup,0);

   *mobileap_enable = QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup;
   *wlan_enable= QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup;
   return true;
  }


/*=====================================================
  FUNCTION GetKernelVer
======================================================*/
/*!
@brief
  Queries the kernel version.

@parameters
  char *version

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
boolean QCMAP_ConnectionManager::GetKernelVer
(
  char *version
)
{
  FILE *fp = NULL;
  char command[MAX_COMMAND_STR_LEN];
  char kernel_ver[KERNEL_VERSION_LENGTH];

  if ( version == NULL )
  {
     LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
     return false;
  }
  memset(kernel_ver, 0, KERNEL_VERSION_LENGTH);

  snprintf( command, MAX_COMMAND_STR_LEN,
            "uname -r | awk '{print $1}' | cut -d '-' -f 1 > %s",KERNEL_VERSION_PATH);
  ds_system_call(command, strlen(command));

  fp = fopen(KERNEL_VERSION_PATH, "r");

  if ( fp == NULL )
  {
     LOG_MSG_ERROR("Error opening Kernel version file: %d.\n", errno, 0, 0);
     return false;
  }

  if (fscanf(fp, "%s", kernel_ver) != 1)
  {
    LOG_MSG_ERROR("Error reading Kernel version file: %d.\n", errno, 0, 0);
    fclose(fp);
    return false;
  }
  memcpy(version,kernel_ver,MIN(strlen(kernel_ver),KERNEL_VERSION_LENGTH - 1));
  fclose(fp);
  return true;
}

/*=====================================================
  FUNCTION CompareKernelVer
======================================================*/
/*!
@brief
  Compares the kernel version with given kernel version.

@parameters
  char     *compare_kernel_ver

@return
  true  - if actual kernel version higher than or equal to
          given kernel version
  false - if actual kernel version less than given kernel version
  -1    - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
int QCMAP_ConnectionManager::CompareKernelVer
(
  char *compare_kernel_ver
)
{
  char *ver=NULL, *input_ver=NULL,*ptr1,*ptr2;
  char buff[KERNEL_VERSION_LENGTH];
  char Kernel_ver[KERNEL_VERSION_LENGTH];

  memset(buff, 0, KERNEL_VERSION_LENGTH);
  memset(Kernel_ver, 0, KERNEL_VERSION_LENGTH);

  if (false == GetKernelVer(Kernel_ver))
  {
    LOG_MSG_ERROR("Unable to get the kernel version info", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  ver = strtok_r(Kernel_ver,".", &ptr1);
  snprintf(buff,KERNEL_VERSION_LENGTH,"%s",compare_kernel_ver);
  input_ver = strtok_r(buff,".", &ptr2);

  while ((ver != NULL) && (input_ver != NULL))
  {
    if (atoi(ver) > atoi(input_ver))
      return TRUE;
    else if (atoi(ver) < atoi(input_ver))
      return FALSE;

    ver = strtok_r(NULL,".",&ptr1);
    input_ver = strtok_r(NULL,".",&ptr2);
  }

  if (input_ver == NULL)
    return TRUE;
  else
    return FALSE;

return TRUE;
}
/*======================================================
  FUNCTION RestoreFactoryConfig
======================================================*/
/*!
@brief
  Restore to Factory Configuration

@parameters
   none

@return
   returns succesful
/*====================================================*/
boolean QCMAP_ConnectionManager::RestoreFactoryConfig
(
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  int current_packet_stat_conf = 0;
  FILE* fp = NULL;

  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  bzero(&command,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("QCMAP_ConnectionManager::RestoreFactoryConfig()",0,0,0);
  snprintf( command, MAX_COMMAND_STR_LEN,
            "cp %s %s ",FACTORY_CONF,QcMapMgr->xml_path);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",QcMapMgr->xml_path);
  ds_system_call(command, strlen(command));

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);

  } else if(QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "cp %s %s ", FACTORY_SOCKSV5_CONF,
             socksv5_file_path.conf_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",socksv5_file_path.conf_file);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN, "cp %s %s ", FACTORY_SOCKSV5_AUTH,
             socksv5_file_path.auth_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",socksv5_file_path.auth_file);
    ds_system_call(command, strlen(command));

  } else {
    LOG_MSG_INFO1("Couldn't get socksv5 config file paths from: %s", QcMapMgr->xml_path, 0, 0);
  }

  snprintf(command, MAX_COMMAND_STR_LEN, "cp %s %s ", FACTORY_L2TP_CONF,
            L2TP_CFG_XML);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",L2TP_CFG_XML);
  ds_system_call(command, strlen(command));

  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  //read packet stat configuration
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PacketStats_TAG);
  current_packet_stat_conf = atoi(root.child(PacketStatsCfg_TAG).child_value());

#ifndef FEATURE_DATA_TARGET_MDM9607
  //write to IPACM xml file the current packet stat configuration
  if(!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(IPAPacketStats_TAG);
  root.child(IPAPacketStatsCfg_TAG).text() = current_packet_stat_conf;
  QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif /* FEATURE_DATA_TARGET_MDM9607 */

  snprintf( command, MAX_COMMAND_STR_LEN,
            "cp %s %s ",FACTORY_FIREWALL_CONF,QcMapMgr->firewall_xml_path);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",QcMapMgr->firewall_xml_path);
  ds_system_call(command, strlen(command));

#ifndef FEATURE_DATA_TARGET_MDM9607
  //overwrite and create file if necessary
  if((fp = fopen(IPA_CFG_FILE_PATH, "w")) != NULL)
  {
    //write empty file.
    fprintf(fp, "%s", "");

    if(fclose(fp))
    {
      LOG_MSG_ERROR("Error closing %s for writing: %s", IPA_CFG_FILE_PATH, strerror(errno), 0);
    }
  }
#endif /* FEATURE_DATA_TARGET_MDM9607 */

  /* sleep for 500 miliseconds */
  sleep(QCMAP_RESET_CONFIG_TIMEOUT);
  snprintf( command, MAX_COMMAND_STR_LEN,
            "reboot",strlen("reboot"));
  ds_system_call(command, strlen(command));
  return true;
}
/*=====================================================
  FUNCTION MatchMacAddrInList
======================================================*/
/*!
@brief
  - Match the MAC address
  - If a match is found, store the IP address of the client in a Linked List
@parameters
  Pointer to qcmap_nl_sock_msg_t
@return
  1 if MAC address found in list
  0 if not found
  -1 other errors

@note
- Dependencies
- None
- Side Effects
- None
*/
/*=====================================================*/
int QCMAP_ConnectionManager::MatchMacAddrInList
(
  qcmap_nl_sock_msg_t* qcmap_nl_buf
)
{
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  qmi_error_type_v01 qmi_err_num;
  struct ps_in6_addr *prefix_ptr, *nl_prefix_ptr;
  uint8 origIPv6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8 ipv6PrefixLen = 0;
  int qcmap_cm_error = QCMAP_CM_SUCCESS;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qcmap_cm_nl_prefix_info_t   ipv6_del_prefix_info;
  in_addr addr;
  char ipv6addr[INET6_ADDRSTRLEN];
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  struct ps_in6_addr *global_addr_ptr = NULL, *iid_addr_ptr = NULL;
  struct in6_addr null_ipv6_address;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr = NULL;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_LAN* QCMAPLANMgr=NULL;
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
  uint32_t lease_expiry_time = 0;
  FILE *fd = NULL;
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char command[MAX_COMMAND_STR_LEN];
  qcmap_mac_addr_vlan_info_t  mac_vlan_info;
  qcmap_nl_addr_t  *nl_addr = NULL;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  std::string dns_masq_file_name(DNSMASQ_LEASE_FILE);
  std::string tmp_str(".bridge");

  if ((qcmap_nl_buf == NULL) || (lan_mgr == NULL))
  {
    LOG_MSG_ERROR("Input params is NULL \n",0,0,0);
    return 0;
  }

  nl_addr = &(qcmap_nl_buf->nl_addr);
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(origIPv6,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  memset(ipv6addr, 0, INET6_ADDRSTRLEN);
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  memset(&null_ipv6_address, 0, sizeof(struct in6_addr));

  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("\nMatchMacAddrInList - Linked list head is NULL \n",0,0,0);
    return 0;
  }

  ds_mac_addr_ntop(nl_addr->mac_addr, mac_addr_char);
  LOG_MSG_INFO1("match mac addr %s and vlan_id %d ", mac_addr_char,qcmap_nl_buf->vlan_id,0);

  mac_vlan_info.vlan_id = qcmap_nl_buf->vlan_id;
  memcpy(mac_vlan_info.mac_addr,nl_addr->mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);

  QCMAPLANMgr = lan_mgr->GetLANBridge(mac_vlan_info.vlan_id);

  if(QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("QCMAPLANMgr is NULL for vlan_id:%d \n",mac_vlan_info.vlan_id,
                  0,0);
    return 0;
  }

  node = ds_dll_search (addrList->addrListHead ,(void*)&mac_vlan_info,
                        QcMapMgr->qcmap_match_mac_addr_vlan_id_pair);

  if( node == NULL)
  {
    LOG_MSG_ERROR("\nMatchMacAddrInList - No match for MAC address "
                  "found in the list\n",0,0,0);
    return 0;
  }
  else
  {
    //When a node is found, update the IP address from the netlink event
    list_data = (qcmap_cm_client_data_info_t*)(node->data);
    if (list_data == NULL)
    {
      LOG_MSG_ERROR("MatchMacAddrInList - Error in fetching node data ",0,0,0);
      return -1;
    }
    ds_mac_addr_ntop(list_data->mac_addr, mac_addr_char);
    LOG_MSG_INFO1(" QCMAP_ConnectionManager::MatchMacAddrInList() Found node with MAC addr %s \n",
               mac_addr_char,0,0);

    if(mac_vlan_info.vlan_id != 0)
    {
      QcMapBackhaulMgr = GetBackhaulObjectFromVLANid(mac_vlan_info.vlan_id);
    }
    else
    {
      QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
    }


    if (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01 &&
        !strlen(list_data->host_name))
    {
      // Delete the hostname.txt file if present
      snprintf(command, MAX_COMMAND_STR_LEN, "rm %s",HOSTNAME_PATH);
      ds_system_call(command, strlen(command));

      if(mac_vlan_info.vlan_id != 0)
      {
        tmp_str.append(std::to_string(mac_vlan_info.vlan_id));
        dns_masq_file_name.append(tmp_str);
        LOG_MSG_INFO2("dns_file_loc:%s",dns_masq_file_name.c_str(),0,0);
      }

      //Prune the dnsmasq.leases file to have lease expiry time, MAC address and hostname
      snprintf(command, MAX_COMMAND_STR_LEN,
               "cut -f1,2,4 -d ' '  %s >    %s",
               dns_masq_file_name.c_str(),
               HOSTNAME_PATH);
      ds_system_call(command, strlen(command));

      //Open the hostname.txt file to fetch lease time and hostname
      fd = fopen(HOSTNAME_PATH,"r");
      if (fd == NULL)
      {
        LOG_MSG_ERROR("Error in opening %s", HOSTNAME_PATH, 0, 0);
        return false;
      }

      fetchHostNameAndLeasetime(fd ,list_data->mac_addr,
                                list_data->host_name,
                                &lease_expiry_time);
      fclose(fd);
    }

    if(nl_addr->isValidIPv4address == true && nl_addr->ip_addr != 0)
    {
      if (list_data->ip_addr == nl_addr->ip_addr)
      {
        addr.s_addr = nl_addr->ip_addr;
        LOG_MSG_INFO1(" Duplicate NEWNEIGH Event recieved for IP %s \n",inet_ntoa(addr),0,0);
      }
      else
      {
        addr.s_addr = nl_addr->ip_addr;
        LOG_MSG_INFO1("Adding a new entry for IP %s \n",inet_ntoa(addr),0,0);
        if (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
        {
          // Delete the hostname.txt file if present
          snprintf(command, MAX_COMMAND_STR_LEN, "rm %s",HOSTNAME_PATH);
          ds_system_call(command, strlen(command));

          if(mac_vlan_info.vlan_id != 0)
          {
            tmp_str.append(std::to_string(mac_vlan_info.vlan_id));
            dns_masq_file_name.append(tmp_str);
            ds_log_med("dns_file_loc:%s",dns_masq_file_name.c_str());
          }

          //Prune the dnsmasq.leases file to have lease expiry time, MAC address and hostname
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "cut -f1,2,4 -d ' '  %s >    %s",
                   dns_masq_file_name.c_str(),
                   HOSTNAME_PATH);
          ds_system_call(command, strlen(command));

          //Open the hostname.txt file to fetch lease time and hostname
          fd = fopen(HOSTNAME_PATH,"r");
          if (fd == NULL)
          {
            LOG_MSG_ERROR("Error in opening %s", HOSTNAME_PATH, 0, 0);
            return false;
          }

          fetchHostNameAndLeasetime(fd ,list_data->mac_addr,
                                    list_data->host_name,
                                    &lease_expiry_time);
          fclose(fd);
        }
        /* Check if IP passthrough active and update the IP only if it is public IP. */
        QCMAPLANMgr->GetIPPassthroughConfig(&enable_state,&passthrough_config,&qmi_err_num);
        LOG_MSG_INFO1("IP Passthrough hostnames IP:%s\n%s\n", list_data->host_name, passthrough_config.client_device_name, 0);

        if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
        {
          LOG_MSG_INFO1("Failed to get LAN config", 0, 0, 0);
          return false;
        }
        /* If IP passthrough is active, for USB client, only for the client with specified hostname
         * check if the IP is matching the public ip. For Ethernet client check will be based on
         * mac address. For rest of the clients, ip address will be privat ip.
         */
        if (QCMAPLANMgr && lan_cfg->ip_passthrough_cfg.ip_passthrough_active &&
            (((list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01) &&
            !strncasecmp(list_data->host_name, passthrough_config.client_device_name, strlen(list_data->host_name))) ||
            ((list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01) &&
            !memcmp(list_data->mac_addr, passthrough_config.mac_addr,
            QCMAP_MSGR_MAC_ADDR_LEN_V01))))
        {
          if (nl_addr->ip_addr != (uint32_t)lan_cfg->ip_passthrough_cfg.public_ip)
          {
            addr.s_addr = (uint32_t)lan_cfg->ip_passthrough_cfg.public_ip;
            LOG_MSG_INFO1("IP Passthrough active and IP doesn't match with public ip %s ignore\n",inet_ntoa(addr),0,0);
          }
          else
          {
            list_data->ip_addr = nl_addr->ip_addr;
            addr.s_addr = list_data->ip_addr;
            LOG_MSG_INFO1("IP Passthrough active and updated IP %s\n",inet_ntoa(addr),0,0);

#ifndef FEATURE_DATA_TARGET_MDM9607
            /* Updating the IPACM XML with the mac address for which IP passthrough is enabled. */
            if (!xml_file.load_file(IPA_XML_PATH))
            {
              LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
              return false;
            }
            root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPPassthroughFlag_TAG);
            if (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
            {
              /* mac address is not provided for USB client*/
              root.child(IPPassthroughMacAddr_TAG).text() = ether_ntoa((struct ether_addr *)list_data->mac_addr);
            }
            QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
          }
        }
        else
        {
          if (QCMAPLANMgr &&
              ((lan_cfg->apps_ip_addr & lan_cfg->sub_net_mask) !=
              (ntohl(nl_addr->ip_addr) & lan_cfg->sub_net_mask)) &&
              (!QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated()) &&
              (!QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated()))
          {
            addr.s_addr = nl_addr->ip_addr;
            LOG_MSG_INFO1("IP not in LAN subnet, ignore %s\n",inet_ntoa(addr), 0, 0);
          }
          else
          {
            list_data->ip_addr = nl_addr->ip_addr;
            if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
            {
              if (!QCMAPLANMgr->AddEbtablesRuleForBridgeMode(list_data))
                LOG_MSG_ERROR( "Unable to Add Ebtables rules for NEWNEIGH AP-STA Bridge Mode",
                           0, 0, 0 );
            }
          }
        }
      }
    }

    memcpy(origIPv6, nl_addr->ip_v6_addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    nl_prefix_ptr = (struct ps_in6_addr*)(nl_addr->ip_v6_addr);

    inet_ntop(AF_INET6,(void *)origIPv6, ipv6addr, INET6_ADDRSTRLEN);
    LOG_MSG_INFO1("MatchMacAddrInList- Received V6 Address %s, ipv6 valid? %d",ipv6addr,nl_addr->isValidIPv6address,0);

    if((nl_addr->isValidIPv6address == true) &&
         !(QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32)) && (QcMapBackhaulMgr != NULL))
    {
      if(QcMapBackhaulMgr->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num) == 0)
      {
        LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d. Ignore v6 address\n", qcmap_cm_error,0,0);
        return 1;
      }
      memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
      QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(QcMapBackhaulMgr->ipv6_prefix_info));
      prefix_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)&
                   (QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr);

      //Store the original Ipv6 address
      memcpy(origIPv6, nl_addr->ip_v6_addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      nl_prefix_ptr = (struct ps_in6_addr*)(nl_addr->ip_v6_addr);
      ipv6PrefixLen = QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;

      /* Zero out the non prefix bits. */
      if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid )
      {
        prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
        prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
        nl_prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
        nl_prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
      }

      /* Zero out the IID part */
      prefix_ptr->ps_s6_addr64[1] = 0;
      nl_prefix_ptr->ps_s6_addr64[1] = 0;

      /*Compare backhaul stored prefix with the NL even received prefix*/
      if (!memcmp(prefix_ptr->ps_s6_addr64,
                   nl_prefix_ptr->ps_s6_addr64,
                   sizeof(uint64)))
      {
        if (memcmp(&null_ipv6_address, (struct in6_addr *)list_data->link_local_v6_addr,
             sizeof(struct in6_addr) ) != 0)
        {
          /* Update only prefix part. IID part will be from LL address. */
          global_addr_ptr = (struct ps_in6_addr *)origIPv6;
          iid_addr_ptr = (struct ps_in6_addr *)list_data->link_local_v6_addr;
          global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
        }

        if (!memcmp(list_data->ipv6[0].addr,&null_ipv6_address,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
        {
          memcpy(list_data->ipv6[0].addr, origIPv6, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          inet_ntop(AF_INET6,(void *)list_data->ipv6[0].addr,ipv6addr, INET6_ADDRSTRLEN);
          LOG_MSG_INFO1("MatchMacAddrInList- New V6 Address %s",ipv6addr,0,0);
        }
      }
      else if (IS_DEFAULT_PROFILE(QcMapBackhaulMgr->profileHandle) &&
               QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated)
      {
          if (QcMapBackhaulWWANMgr->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)
          {
            if (memcmp(&null_ipv6_address, (struct in6_addr *)list_data->link_local_v6_addr,
                 sizeof(struct in6_addr) ) != 0)
            {
              /* Update only prefix part. IID part will be from LL address. */
              global_addr_ptr = (struct ps_in6_addr *)origIPv6;
              iid_addr_ptr = (struct ps_in6_addr *)list_data->link_local_v6_addr;
              global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
            }

            if (!memcmp(list_data->ipv6[0].addr,&null_ipv6_address,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
            {
              memcpy (list_data->ipv6[0].addr,origIPv6,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
              inet_ntop(AF_INET6,(void *)list_data->ipv6[0].addr,ipv6addr, INET6_ADDRSTRLEN);
              LOG_MSG_INFO1("MatchMacAddrInList- New V6 Address %s",ipv6addr,0,0);
            }

          }
          else
          {
            memset(&ipv6_del_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
            ipv6_del_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
            ipv6_del_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
            ipv6_del_prefix_info.prefix_len =  (unsigned char) IPV6_MIN_PREFIX_LENGTH;
            memcpy(((struct sockaddr_in6*)&(ipv6_del_prefix_info.prefix_addr))->sin6_addr.s6_addr,
                   list_data->ipv6[0].addr, ipv6PrefixLen);
            if( QcMapBackhaulMgr &&
              QcMapBackhaulMgr->UpdatePrefix(&ipv6_del_prefix_info,true, true,
              list_data->link_local_v6_addr) == -1)
            {
              LOG_MSG_ERROR("Error: Unable to deprecate prefix", 0, 0, 0);
              return 1;
            }
          }
      }
    }
    else if ((nl_addr->isValidIPv6address == true) &&
              QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32))
    {
    //Copy the link local Ipv6 address of the client
      if ((memcmp(&null_ipv6_address, (struct in6_addr *)list_data->link_local_v6_addr,
         sizeof(struct in6_addr)) == 0))
      {
        memcpy (list_data->link_local_v6_addr,
                origIPv6,QCMAP_MSGR_IPV6_ADDR_LEN_V01);

        //Send NS for LL addr to be REACHABLE in neighbor show
        if(!QcMapMgr->SendNSOnBridgeIface( mac_vlan_info.vlan_id,list_data->link_local_v6_addr))
          LOG_MSG_ERROR("Cannot send NS on %s ", GET_ASSOCIATED_BRIDGE_IFACE_NAME(mac_vlan_info.vlan_id), 0, 0);

        if(QcMapBackhaulMgr)
          QcMapBackhaulMgr->SendRSForClient(list_data->link_local_v6_addr);
         inet_ntop(AF_INET6,(void *)list_data->link_local_v6_addr,ipv6addr, INET6_ADDRSTRLEN);
         LOG_MSG_INFO1("MatchMacAddrInList- New LL V6 Address %s updated\n",ipv6addr,0,0);
      }

      if (QcMapBackhaulMgr && IS_DEFAULT_PROFILE(QcMapBackhaulMgr->profileHandle))
      {
        if(QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated)
        {
          LOG_MSG_ERROR("Prefix delegation Enabled.Don't update client prefixes",
                        0, 0, 0);
          return 1;
        }
      }
      if ( memcmp(&null_ipv6_address, (struct in6_addr *)list_data->ipv6[0].addr,
            sizeof(struct in6_addr) ) )
      {
        /* Update the global address with new IID and prefix. */
        if((QcMapBackhaulMgr == NULL) || (QcMapBackhaulMgr->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num) == 0))
        {
          LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d. Ignore v6 address\n", qcmap_cm_error,0,0);
          return 1;
        }
        memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
        QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
        QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;

        QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(QcMapBackhaulMgr->ipv6_prefix_info));
        prefix_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)&
                   (QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr);
        prefix_ptr->ps_s6_addr64[1] <<=
              QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;
        prefix_ptr->ps_s6_addr64[1] >>=
            QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;
        iid_addr_ptr = (struct ps_in6_addr *)list_data->link_local_v6_addr;
        prefix_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
        memcpy(list_data->ipv6[0].addr, prefix_ptr, sizeof(struct in6_addr));
        inet_ntop(AF_INET6,(void *)list_data->ipv6[0].addr,ipv6addr, INET6_ADDRSTRLEN);
        LOG_MSG_INFO1("MatchMacAddrInList- New Global V6 Address %s updated\n",ipv6addr,0,0);
      }
    }
  }
  LOG_MSG_INFO1("bye match mac addr in list",0,0,0);
  return 1;

}


/*======================================================
  FUNCTION DeleteDeviceEntryInfo
=======================================================
@brief
  Remove the client entry (node) from the List

@parameters
  - MAC address of client
  - Device type to indicate device is USB or WLAN device
@return
  none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
bool QCMAP_ConnectionManager::DeleteDeviceEntryInfo
(
  void* mac_addr,
  qcmap_nl_addr_t *nl_addr,
  qcmap_msgr_device_type_enum_v01 device_type,
  qcmap_cm_cdi_del_evt_t          del_evt,
  int16_t                         vlan_id
)
{
  ds_dll_el_t * node;
  uint8 mac_addr_hex[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=NULL;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qmi_error_type_v01 qmi_err_num;
  char command[MAX_COMMAND_STR_LEN];
  char ipv4addr[INET_ADDRSTRLEN];
  memset(ipv4addr,0,INET_ADDRSTRLEN);
  char ipv6addr[INET6_ADDRSTRLEN];
  memset(ipv6addr,0,INET6_ADDRSTRLEN);
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  memset(zero_buff,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  qcmap_msgr_lan_config_v01 lan_config;
  uint32 nullIP = 0;
  char temp_mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_mac_addr_vlan_info_t mac_addr_vlan_info;
  qcmap_msgr_wlan_device_type_v01 wlan_dev_type;
  int i = 0;

  mac_addr_vlan_info.vlan_id = vlan_id;
  memset(&lan_config, 0, sizeof(qcmap_msgr_lan_config_v01));
  LOG_MSG_INFO1("\n In DeleteDeviceEntryInfo \n",0,0,0);

  if (mac_addr == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - mac address pointer have NULL value \n",0,0,0);
    return false;
  }

  QCMAPLANMgr = GET_DEFAULT_LAN_OBJECT();
  if(QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("QCMAPLANMgr is NULL for vlan_id:%d\n",vlan_id,0,0);
    return false;
  }

  /*Removing entry for dnsmasq script cointaining this mac address*/
  snprintf(command, sizeof(command), "sed -i \"/%s/d\" %s",mac_addr,TEMP_HOST_FILE);
  ds_system_call(command, strlen(command));

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - Linked list head is NULL \n",0,0,0);
    return false;
  }

  if ((device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01)||
      (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01) ||
      (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01) ||
      (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01))
  {
    ds_log_med("DeleteDeviceEntryInfo- Delete client mac %s, Device_type:%d \n",
               mac_addr, device_type);
    //Convert the MAC from char to hex
    if(!ds_mac_addr_pton((char*)mac_addr, mac_addr_hex))
    {
      LOG_MSG_ERROR("DeleteDeviceEntryInfo - Error in MAC address conversion",
                    0,0,0);
      return false;
    }
    mac_addr = (void*)mac_addr_hex;
  }
  else
  {
    memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
    ds_mac_addr_ntop(mac_addr, mac_addr_char);
    ds_log_med("DeleteDeviceEntryInfo- Delete client mac %s, Device_type:%d \n",
               mac_addr_char, device_type);
  }

  memcpy(mac_addr_vlan_info.mac_addr,mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);

  if ((node = ds_dll_search (addrList->addrListHead ,(void*)&mac_addr_vlan_info,
                             QcMapMgr->qcmap_match_mac_addr_vlan_id_pair)) == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - Error in finding the node",0,0,0);
    return false;
  }
  else
  {
    connectedDevicesList = (qcmap_cm_client_data_info_t*)ds_dll_data(node);
    if (connectedDevicesList == NULL)
    {
      LOG_MSG_ERROR("DeleteDeviceEntryInfo - The device information"
                     "node is NULL\n",0,0,0);
      return false;
    }

    if(QcMapBackhaulMgr && QcMapBackhaulMgr->IsPmipV6ModeEnabled() || QcMapBackhaulMgr->IsPmipV4ModeEnabled())
    {
      QcMapBackhaulMgr->PmipDeleteDevice(connectedDevicesList);
    }

    if (del_evt == QCMAP_DELNEIGH)
    {
      if((connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01) ||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01) ||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01))
      {
        if(nl_addr != NULL)
        {
          if((nl_addr->isValidIPv4address == true) && (lan_config.enable_dhcp == false))
          {
            connectedDevicesList->ip_addr = nullIP;
          }
        }
        LOG_MSG_ERROR("DeleteDeviceEntryInfo - WLAN information will not be deleted"
                      "based on DELNEIGH",0,0,0);
        return false;
      }
    }
    else if (del_evt == QCMAP_AP_STA_DISCONNECTED)
    {
      if (connectedDevicesList->device_type != device_type)
      {
        LOG_MSG_ERROR("Current device type %d not same as present in the list %d",
                      device_type, connectedDevicesList->device_type, 0);

        return false;
      }
    }
  }


  //Delete the node from the list
  if ((node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),
       (void*)&mac_addr_vlan_info, QcMapMgr->qcmap_match_mac_addr_vlan_id_pair))== NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - Error in finding the node",0,0,0);
    return false;
  }

  //Free the memory for the device details structure
  connectedDevicesList = (qcmap_cm_client_data_info_t*)ds_dll_data(node);

  if (connectedDevicesList == NULL)
  {
    LOG_MSG_ERROR("DeleteDeviceEntryInfo - The device information"
                  "node is NULL\n",0,0,0);
    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;
    return false;
  }

  //Delete the conntrack entry for the device
  QCMAP_Tethering::DelConntrack( connectedDevicesList->ip_addr );

  QcMapMgr->DelV6Conntrack(connectedDevicesList->ipv6[0].addr,
                           connectedDevicesList->mac_addr);

  if((del_evt != QCMAP_AP_STA_DISCONNECTED) && QcMapBackhaulWWANMgr)
  {
    memset(temp_mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
    ds_mac_addr_ntop(connectedDevicesList->mac_addr, temp_mac_addr_char);
    QcMapBackhaulWWANMgr->DeleteDelegatedPrefix(temp_mac_addr_char,
                                                &qmi_err_num);
  }
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if (QCMAPLANMgr->DelEbtablesRuleForBridgeMode(connectedDevicesList) == false)
    {
      ds_log_med("Could not Delete Ebtables rules for device %s\n",mac_addr);
    }
  }

  if (QcMapMgr->packet_stats_enabled)
  {
    //Get IPA update and update statistics for the client getting disconnected
    connectedDevicesList->ipa_data.rx_data = 0;
    connectedDevicesList->ipa_data.tx_data = 0;
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
    memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

    if (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_USB;
    else if (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;
    else if (QCMAP_WLAN::GetWLANDeviceTypeFromAP(connectedDevicesList->device_type, &wlan_dev_type ,&qmi_err_num))
    {
      if (wlan_dev_type == QCMAP_MSGR_WLAN_DEV_ROME_V01)
       ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_WLAN;
      else if (wlan_dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01)
       ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;
    }

    ioctl_buffer.num_clients = 1;

    memcpy(ioctl_buffer.client_info[0].mac,connectedDevicesList->mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);

    ioctl_buffer.disconnect_clnt = 1;
    ioctl_buffer.reset_stats = 1;

    UpdateIPAStats(&ioctl_buffer);

    /* Update the stats. */
    connectedDevicesList->ipa_data.rx_data = ioctl_buffer.client_info[0].ipv4_rx_bytes + ioctl_buffer.client_info[0].ipv6_rx_bytes;
    connectedDevicesList->ipa_data.tx_data = ioctl_buffer.client_info[0].ipv4_tx_bytes + ioctl_buffer.client_info[0].ipv6_tx_bytes;
    LOG_MSG_INFO1("IPA Stats: RX: %llu TX: %llu", connectedDevicesList->ipa_data.rx_data, connectedDevicesList->ipa_data.tx_data, 0);
    memset(&lan_client_msg, 0, sizeof(lan_client_msg));
    switch (device_type)
    {
       case QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01:
         snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num1);
         break;
       case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01:
         snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num2);
         break;
       case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01:
         snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num3);
         break;
       case QCMAP_MSGR_DEVICE_TYPE_USB_V01:
         if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS)
           snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "rndis0");
         else if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM)
           snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "ecm0");
         else
           goto skip;
           break;
        case QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01:
           snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "eth0");
           break;
       }
       memcpy(lan_client_msg.lan_client.mac, connectedDevicesList->mac_addr, IPA_MAC_ADDR_SIZE);
       lan_client_msg.client_event = IPA_PER_CLIENT_STATS_DISCONNECT_EVENT;
       NotifyIPACM(&lan_client_msg);
#endif
  }
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
skip:
#endif
  // before freeing the device info send message to client of this
  if( QcMapBackhaulWWANMgr && QcMapMgr->packet_stats_enabled &&
    ((QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED)||
    (QcMapBackhaulWWANMgr->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED))&&
    (QcMapBackhaulMgr != NULL && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN) &&
    !(QcMapWLANMgr != NULL &&
    ((connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
    (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)||
    (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01))
    && QcMapWLANMgr->disable_wlan_in_progress))
  {
     /* this is the place where we need to send msg to SFE/IPA & fetch data from exception path*/
    if((connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01)||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)||
         (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01))
    {
      //reset as we are about  to grab latest data.
      connectedDevicesList->exception_data.rx_data = 0;
      connectedDevicesList->exception_data.tx_data = 0;
      connectedDevicesList->sfe_data.rx_data = 0;
      connectedDevicesList->sfe_data.tx_data = 0;

      //get SFE stats - this api  will do it for both ipv6 and ipv4
      if (QcMapMgr->sfe_loaded)
      {
        UpdateSFEStats(connectedDevicesList);
      }

      if (0 != connectedDevicesList->ip_addr)
      {
        inet_ntop(AF_INET,
                 (void *)&connectedDevicesList->ip_addr,ipv4addr,
                  INET_ADDRSTRLEN);

        //obtain ipv4 exception path data
        UpdateIPV4ExceptionStats(connectedDevicesList, ipv4addr);
        if (QcMapMgr->sfe_loaded)
        {
          SendNLMsgToSFE(sfev4_nl_sock_fd, SFE_IPV4_DELETE_PACKET_STATS_NODE, ipv4addr);
        }
        //delete exception path rule for  ipv4 connections
        DeleteIPv4IptableRule(ipv4addr);
      }

      /*At this point we need ot send command to SFE and exception path v4 and v6
      remove all connections related to client*/
      for (int i = 0 ; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
      {
        // if not more ipv6 then break
        if (!memcmp(connectedDevicesList->ipv6[i].addr, zero_buff, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
        {
          break;
        }
        // we need to test this properly. Multiple NL sent quickly
        inet_ntop(AF_INET6, (void *)connectedDevicesList->ipv6[i].addr,
          ipv6addr, INET6_ADDRSTRLEN);
        //obtain ipv4 exception path data
        UpdateIPV6ExceptionStats(connectedDevicesList, ipv6addr);
        if (QcMapMgr->sfe_loaded)
        {
          SendNLMsgToSFE(sfev6_nl_sock_fd, SFE_IPV6_DELETE_PACKET_STATS_NODE, ipv6addr);
        }
        //Delete ipv6 connetions from exception path and update statistics.
        DeleteIPv6IptableRule(ipv6addr);
      }

      QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_CLIENT_DISCONNECTED_V01,
                                   1, connectedDevicesList);
    }

  }

  //Free the device information structure
  ds_free (connectedDevicesList);
  connectedDevicesList = NULL;

  //Free the memory of the linked list node
  ds_dll_free(node);
  node = NULL;

  // Add logic to check if there are no more nodes in the list and free the head
  if ( addrList->addrListHead->next == NULL)
  {
    LOG_MSG_INFO1("\n No device info exist\n",0,0,0);
    ds_dll_free(addrList->addrListHead);
    addrList->addrListHead = NULL;
  }
  QcMapMgr->conn_device_info.numOfNodes--;
  LOG_MSG_INFO1("\n Number of Connected Devices: %d\n",
                QcMapMgr->conn_device_info.numOfNodes, 0, 0);

  if (device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
      device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01 ||
      device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01 ||
      device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01)
    {
      QcMapMgr->conn_device_info.numofWlanclients--;
    }

  if (QcMapMgr->IsDynamicClockVotingNeeded())
  {
    if ( (QcMapMgr->conn_device_info.numofWlanclients == 0))
    {
      QcMapMgr->conn_device_info.wlanclientpresent = false;
      if (QcMapBackhaulWWANMgr && !(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)&&
            (QcMapMgr->IsRatLteFdd()) && (QcMapMgr->isBWLteCAT4()) &&
            ((QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED)||
             (QcMapBackhaulWWANMgr->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED))) &&
             (QcMapMgr->CheckUsbClockAtNom()))
      {
        QcMapMgr->SetUSBClockFreq(false);
      }
    }
  }
  return true;
}

/*======================================================
  FUNCTION AddNewDeviceEntry
======================================================*/
/*!
@brief
  Stores the MAC address and interface name of client in a list.

@parameters
  - MAC address of client
  - Device type
  - Ipv4 address
  - IPv6 address
  - vlan_id
  - bool to check valid ipv4 address
  - bool to check valid ipv6 address
@return
  none

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
bool QCMAP_ConnectionManager::AddNewDeviceEntry
(
  void* mac_addr,
  qcmap_msgr_device_type_enum_v01 device_type,
  uint32* ip_addr,
  uint8* ipv6_addr,
  int16_t vlan_id,
  bool isIpv4valid,
  bool isIpv6valid
)
{
  ds_dll_el_t * node = NULL;
  qmi_error_type_v01 qmi_err_num;
  struct ps_in6_addr *prefix_ptr, *nl_prefix_ptr;
  uint8 ipv6PrefixLen = 0;
  uint8 origIPv6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char ipv6addr[INET6_ADDRSTRLEN];
  struct ps_in6_addr *global_addr_ptr = NULL, *iid_addr_ptr = NULL;
  struct in6_addr null_ipv6_address;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
  in_addr addr;
  uint32_t lease_expiry_time = 0;
  FILE *fd = NULL;
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char command[MAX_COMMAND_STR_LEN];
  std::string dnsmasq_file_name(DNSMASQ_LEASE_FILE);
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();

  if((!QcMapMgr) || (lan_mgr == NULL))
    return false;

  if (QcMapMgr->conn_device_info.numOfNodes >=
                            QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01)
  {
    LOG_MSG_INFO1("Cannot add device. Max devices reached.",0,0,0);
    return false;
  }

  QCMAPLANMgr = lan_mgr->GetLANBridge(vlan_id);

  if(QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("QCMAPLANMgr NULL for vlan_idf:%d.",vlan_id,0,0);
    return false;
  }

  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  if(!QcMapWLANMgr)
  {
    LOG_MSG_INFO1("WLAN object absent",0,0,0);
  }
  if ( ((device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
       (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)||
       (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01)||
       (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01))&& (!QcMapWLANMgr))
  {
    LOG_MSG_ERROR("WLAN is not enabled. Do not add this entry",0 ,0, 0);
    return false;
  }

  connectedDevicesList =
  (qcmap_cm_client_data_info_t*)ds_malloc(sizeof(qcmap_cm_client_data_info_t));

  if( connectedDevicesList == NULL )
  {
    LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for"
                  "device information structure",0,0,0);
    return false;
  }

  memset (connectedDevicesList,0,sizeof(qcmap_cm_client_data_info_t));
  memset (origIPv6, 0 ,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  memset(ipv6addr, 0, INET6_ADDRSTRLEN);
  memset(&null_ipv6_address, 0, sizeof(struct in6_addr));

  /* NOTE - With AP-AP-STA mode, we need to set the device type accordingly*/
  ds_mac_addr_ntop((char*)mac_addr, mac_addr_char);
  ds_log_med("AddNewDeviceEntry- Add new address for mac %s \n",mac_addr_char);


  // Store Client MAC Address in QCMAP context
  memcpy( connectedDevicesList->mac_addr, (uint8*)mac_addr,
          QCMAP_MSGR_MAC_ADDR_LEN_V01);

  //Store the device type to the QCMAP context
  connectedDevicesList->device_type = device_type;
  connectedDevicesList->vlan_id = vlan_id;
  ds_log_med("AddNewDeviceEntry- Add new address for vlan_id:%d \n",
             connectedDevicesList->vlan_id);

  if(vlan_id != 0)
  {
    std::string tmp_str(".bridge");
    tmp_str.append(std::to_string(vlan_id));
    dnsmasq_file_name.append(tmp_str);
    ds_log_med("dns_file_loc:%s",dnsmasq_file_name.c_str());
    QcMapBackhaulMgr = GetBackhaulObjectFromVLANid(vlan_id);
  }
  else
  {
    QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  }

  /* For USB Clients AddNewDeviceEntry() is called from NEWNEIGH events.
   * Hence update IP address too.*/
  if(device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    if (isIpv4valid == true)
    {
      /* Check if IP passthrough active and update the IP only if it is public IP. */
      //Delete the hostname.txt file if present
      snprintf(command, MAX_COMMAND_STR_LEN, "rm %s",HOSTNAME_PATH);
      ds_system_call(command, strlen(command));
      //Prune the dnsmasq.leases file to have lease expiry time, MAC address and hostname
      snprintf(command, MAX_COMMAND_STR_LEN,
               "cut -f1,2,4 -d ' '  %s >  %s",
                 dnsmasq_file_name.c_str(),
               HOSTNAME_PATH);
      ds_system_call(command, strlen(command));

      //Open the hostname.txt file to fetch lease time and hostname
      fd = fopen(HOSTNAME_PATH,"r");
      if (fd == NULL)
      {
        LOG_MSG_ERROR("Error in opening %s", HOSTNAME_PATH, 0, 0);
        return false;
      }

      fetchHostNameAndLeasetime(fd ,connectedDevicesList->mac_addr,
                                connectedDevicesList->host_name,
                                &lease_expiry_time);
      fclose(fd);
      QCMAPLANMgr->GetIPPassthroughConfig(&enable_state,&passthrough_config,&qmi_err_num);
      LOG_MSG_INFO1("IP Passthrough hostnames IP %s\n %s\n", connectedDevicesList->host_name, passthrough_config.client_device_name,0);

      if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
      {
        LOG_MSG_INFO1("Failed to get LAN config", 0, 0, 0);
        return false;
      }
      /* If IP passthrough is active, for USB client, only for the client with specified hostname
       * check if the IP is matching the public ip. For Ethernet client check will be based on
       * mac address. For rest of the clients, ip address will be privat ip.
       */
      if (QCMAPLANMgr && lan_cfg->ip_passthrough_cfg.ip_passthrough_active &&
          !strncasecmp(connectedDevicesList->host_name, passthrough_config.client_device_name,
          strlen(connectedDevicesList->host_name)))
      {
        if (*ip_addr != (uint32_t)lan_cfg->ip_passthrough_cfg.public_ip)
        {
          addr.s_addr = (uint32_t)lan_cfg->ip_passthrough_cfg.public_ip;
          LOG_MSG_INFO1("IP Passthrough active and IP doesn't match with public ip %s ignore\n",inet_ntoa(addr),0,0);
        }
        else
        {
          connectedDevicesList->ip_addr = *ip_addr;
          addr.s_addr = connectedDevicesList->ip_addr;
          LOG_MSG_INFO1("IP Passthrough active and IP updated %s\n",inet_ntoa(addr),0,0);

#ifndef FEATURE_DATA_TARGET_MDM9607
          /* Updating the IPACM XML with the mac address of the client for which passthrough is enabled. */
          if (!xml_file.load_file(IPA_XML_PATH))
          {
            LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
            return false;
          }
          root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPPassthroughFlag_TAG);
          root.child(IPPassthroughMacAddr_TAG).text() = ether_ntoa((struct ether_addr *)connectedDevicesList->mac_addr);
          QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
        }
      }
      else
      {
        if (QCMAPLANMgr && ((lan_cfg->apps_ip_addr & lan_cfg->sub_net_mask) !=
            (ntohl(*ip_addr) & lan_cfg->sub_net_mask)) && 
            (!QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated()) &&
            (!QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated()))
        {
          addr.s_addr = *ip_addr;
          LOG_MSG_INFO1("IP not in LAN subnet, ignore %s\n",inet_ntoa(addr), 0, 0);
        }
        else
        {
          connectedDevicesList->ip_addr = *ip_addr;
          if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
          {
            if (!QCMAPLANMgr->AddEbtablesRuleForBridgeMode(connectedDevicesList))
              LOG_MSG_ERROR("Unable to Add Ebtables rules for NEWNEIGH AP-STA Bridge Mode",
                            0,0,0);
          }
        }
      }
    }

    //Store the original Ipv6 address
    if (isIpv6valid == true)
    {
      memcpy(origIPv6, ipv6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      nl_prefix_ptr = (struct ps_in6_addr*)(ipv6_addr);
     if(!(QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32)) &&
         (QCMAP_Backhaul_WLAN::IsSTAAvailableV6() ||
          QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6() ||
          QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
          QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() ||
         (QcMapBackhaulMgr && (QcMapBackhaulMgr->QcMapBackhaulWWAN) && QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)))
     {
        if(QcMapBackhaulMgr && !(QcMapBackhaulMgr->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num)));
          return 0;

        if(QcMapBackhaulMgr)
        {
          memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
          QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(QcMapBackhaulMgr->ipv6_prefix_info));
          prefix_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)&
                  (QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr);

          ipv6PrefixLen = QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;

          /* Zero out the non prefix bits. */
          if ( QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid )
          {
            prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
            prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
            nl_prefix_ptr->ps_s6_addr64[0] >>= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
            nl_prefix_ptr->ps_s6_addr64[0] <<= (IPV6_MIN_PREFIX_LENGTH - ipv6PrefixLen);
          }
        }
        /* Zero out the IID part */
        prefix_ptr->ps_s6_addr64[1] = 0;
        nl_prefix_ptr->ps_s6_addr64[1] = 0;

        if ((!memcmp(prefix_ptr->ps_s6_addr64,
                     nl_prefix_ptr->ps_s6_addr64,
                     sizeof(uint64))))
        {
          if (memcmp(&null_ipv6_address,
              (struct in6_addr *)connectedDevicesList->link_local_v6_addr,
               sizeof(struct in6_addr)) != 0)
          {
            /* Update only prefix part. IID part will be from LL address. */
            global_addr_ptr = (struct ps_in6_addr *)origIPv6;
            iid_addr_ptr = (struct ps_in6_addr *)connectedDevicesList->link_local_v6_addr;
            global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
          }
          if (!memcmp(connectedDevicesList->ipv6[0].addr,&null_ipv6_address,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
          {
            memcpy(connectedDevicesList->ipv6[0].addr,origIPv6,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
            inet_ntop(AF_INET6,(void *)connectedDevicesList->ipv6[0].addr,
                      ipv6addr, INET6_ADDRSTRLEN);
            LOG_MSG_INFO1("AddNewDeviceEntry- New V6 Address %s",ipv6addr,0,0);
          }

        }
      }
      else if (QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32))
      {
        //Copy the link local Ipv6 address of the client
        if (memcmp(&null_ipv6_address,
            (struct in6_addr *)connectedDevicesList->link_local_v6_addr,
            sizeof(struct in6_addr)) == 0)
        {
          memcpy (connectedDevicesList->link_local_v6_addr,origIPv6,
                  QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          if (QcMapBackhaulMgr)
            QcMapBackhaulMgr->SendRSForClient(connectedDevicesList->link_local_v6_addr);
        }
        memcpy(connectedDevicesList->link_local_v6_addr,origIPv6,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        if (memcmp(&null_ipv6_address, (struct in6_addr *)connectedDevicesList->ipv6[0].addr,
            sizeof(struct in6_addr)) != 0)
        {
          /* Update the global address with this IID. */
          global_addr_ptr = (struct ps_in6_addr *)connectedDevicesList->ipv6[0].addr;
          iid_addr_ptr = (struct ps_in6_addr *)connectedDevicesList->link_local_v6_addr;
          global_addr_ptr->ps_s6_addr64[1] = iid_addr_ptr->ps_s6_addr64[1];
        }
        /* Update the global address with this IID. */
        inet_ntop(AF_INET6,(void *)connectedDevicesList->link_local_v6_addr,
                  ipv6addr, INET6_ADDRSTRLEN);
        ds_log_med("AddNewDeviceEntry- New LL V6 Address %s \n",ipv6addr);
      }
    }
  }
  if (addrList->addrListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
           information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for node",
                    0,0,0);
      ds_free(connectedDevicesList);
      connectedDevicesList = NULL;
      return false;
    }
    addrList->addrListHead = node;
  }

  //Store the mac address, interface name in the linked list
  if ((node = ds_dll_enq(addrList->addrListHead,
                         NULL, (void*)connectedDevicesList )) == NULL)
  {
    LOG_MSG_ERROR("AddNewDeviceEntry - Error in adding a node",0,0,0);
    ds_free(connectedDevicesList);
    connectedDevicesList = NULL;
    return false;
  }
  QcMapMgr->conn_device_info.numOfNodes++;
  LOG_MSG_INFO1("Number of Connected Devices: %d",
                QcMapMgr->conn_device_info.numOfNodes, 0, 0);
  //Update the tail pointer when a new node is added everytime
  addrList->addrListTail = node;

  //if packet stats enabled then send ind to client
  if(QcMapMgr->packet_stats_enabled)
  {
    if((device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
         (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)||
         (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01)||
         (device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)||
         (device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01))
    {
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
      memset(&lan_client_msg, 0, sizeof(lan_client_msg));
      switch (device_type)
      {
         case QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01:
           snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num1);
           break;
         case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01:
           snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num2);
           break;
         case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01:
           snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num3);
           break;
         case QCMAP_MSGR_DEVICE_TYPE_USB_V01:
           if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS)
             snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "rndis0");
           else if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM)
             snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "ecm0");
           else
             goto skip;
           break;
         case QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01:
           snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "eth0");
           break;
       }
       memcpy(lan_client_msg.lan_client.mac, connectedDevicesList->mac_addr, IPA_MAC_ADDR_SIZE);
       lan_client_msg.client_event = IPA_PER_CLIENT_STATS_CONNECT_EVENT;
       NotifyIPACM(&lan_client_msg);
skip:
#endif
      QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_CLIENT_CONNECTED_V01,
                                   1, connectedDevicesList);
    }
  }

  if (device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
      device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01 ||
      device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01 ||
      device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01)
  {
    QcMapMgr->conn_device_info.numofWlanclients++;
    if (!QcMapMgr->CheckWlanClientConnected())
    {
      QcMapMgr->conn_device_info.wlanclientpresent = true;
    }
  }

  if (QcMapMgr->IsDynamicClockVotingNeeded())
  {
    if (QcMapMgr->CheckWlanClientConnected())
    {
      if((QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)) &&
         !(QcMapMgr->CheckUsbClockAtNom()))
      {
        QcMapMgr->SetUSBClockFreq(true);
      }
    }
  }
  return true;
}


/*=====================================================
  FUNCTION GetConnectedDevicesInfo
======================================================*/
/*!
@brief
 Fetches the list of devices connected to the Access point

@parameters
  - Pointer to qcmap_msgr_connected_device_info_v01
  - Number of device entries
  - Pointer to qmi_error_type_v01

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/


boolean QCMAP_ConnectionManager::GetConnectedDevicesInfo
(
 qcmap_msgr_connected_device_info_v01 *conn_devices,
 unsigned int* num_entries,
 qmi_error_type_v01 *qmi_err_num
)
{
  int i = 0, k = 0;
  FILE *fd = NULL;
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  char command[MAX_COMMAND_STR_LEN];
  char ipv4addr[INET_ADDRSTRLEN];
  memset(ipv4addr,0,INET_ADDRSTRLEN);
  char ipv6addr[INET6_ADDRSTRLEN];
  memset(ipv6addr,0,INET6_ADDRSTRLEN);
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  memset(zero_buff,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
  struct wan_ioctl_query_per_client_stats  wlan_stats,usb_stats, ethernet_stats, odu_stats;
#endif
  std::string dnsmasq_file_name;
  std::string tmp_str;
  qcmap_msgr_wlan_device_type_v01 wlan_dev_type;

  LOG_MSG_INFO1("QCMAP_ConnectionManager:GetConnectedDevicesInfo",0,0,0);

  if (conn_devices == NULL)
  {
    LOG_MSG_ERROR("Null arguements passed.\n",0,0,0);
    return false;
  }

  //Fetch the number of nodes in linked list and store in num_entries
  *num_entries = QcMapMgr->conn_device_info.numOfNodes;
  if ( *num_entries == 0 )
  {
    LOG_MSG_INFO1("GetConnectedDevicesInfo - No devices present",0,0,0);
    return true;
  }

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("GetConnectedDevicesInfo - Linked list head is NULL"
                  "Cant fetch connected devices info",0,0,0);
    return false;
  }

  node = addrList->addrListHead;
  node = ds_dll_next (node, (const void**)(&connectedDevicesList));
  if ( node == NULL)
  {
    LOG_MSG_ERROR("GetConnectedDevicesInfo - Device info is NULL"
                  "Cant fetch connected devices info",0,0,0);
    return false;
  }

  //Get IPA update and update statistics for the client
  if (QcMapMgr->packet_stats_enabled)
  {
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
    //Get IPA update and update statistics for the client getting disconnected
    memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

    ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_USB;

    ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

    UpdateIPAStats(&ioctl_buffer);

    memcpy(&usb_stats, &ioctl_buffer, sizeof(struct wan_ioctl_query_per_client_stats));

    //Get IPA update and update statistics for the client getting disconnected
    memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

    ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_WLAN;

    ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

    UpdateIPAStats(&ioctl_buffer);

    memcpy(&wlan_stats, &ioctl_buffer, sizeof(struct wan_ioctl_query_per_client_stats));

    //Get IPA update and update statistics for the client getting disconnected
    memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

    ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;

    ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

    UpdateIPAStats(&ioctl_buffer);

    memcpy(&ethernet_stats, &ioctl_buffer, sizeof(struct wan_ioctl_query_per_client_stats));

    //Get IPA update and update statistics for the client getting disconnected

     memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

     ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;

     ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

     UpdateIPAStats(&ioctl_buffer);

     memcpy(&odu_stats, &ioctl_buffer, sizeof(struct wan_ioctl_query_per_client_stats));

#endif
  }
  for ( i=0; i < *num_entries && connectedDevicesList; i++ )
  {
    tmp_str = ".bridge";
    dnsmasq_file_name = DNSMASQ_LEASE_FILE;
    if (QcMapMgr->packet_stats_enabled)
    {
      //reset as we are about  to grab latest data.
      connectedDevicesList->exception_data.rx_data = 0;
      connectedDevicesList->exception_data.tx_data = 0;
      connectedDevicesList->sfe_data.rx_data = 0;
      connectedDevicesList->sfe_data.tx_data = 0;
      connectedDevicesList->ipa_data.rx_data = 0;
      connectedDevicesList->ipa_data.tx_data = 0;

      //get SFE stats - this api  will do it for both ipv6 and ipv4
      if (QcMapMgr->sfe_loaded)
      {
        UpdateSFEStats(connectedDevicesList);
      }

      if (0!=connectedDevicesList->ip_addr)
      {
        inet_ntop(AF_INET,
                 (void *)&connectedDevicesList->ip_addr,ipv4addr,
                  INET_ADDRSTRLEN);
        //obtain ipv4 exception path data
        UpdateIPV4ExceptionStats(connectedDevicesList, ipv4addr);
      }
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
      if (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
      {
        for (k = 0; k < IPA_MAX_NUM_HW_PATH_CLIENTS; k++)
        {
           if (memcmp(connectedDevicesList->mac_addr, usb_stats.client_info[k].mac, QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0)
           {
             connectedDevicesList->ipa_data.rx_data = usb_stats.client_info[k].ipv4_rx_bytes + usb_stats.client_info[k].ipv6_rx_bytes;
             connectedDevicesList->ipa_data.tx_data = usb_stats.client_info[k].ipv4_tx_bytes + usb_stats.client_info[k].ipv6_tx_bytes;
             LOG_MSG_INFO1("USB Client, IPA Stats, RX: %llu, TX: %llu\n", connectedDevicesList->ipa_data.rx_data, connectedDevicesList->ipa_data.tx_data, 0);
             break;
           }
        }
      }
      else if (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
      {
        for (k = 0; k < IPA_MAX_NUM_HW_PATH_CLIENTS; k++)
        {
           if (memcmp(connectedDevicesList->mac_addr, ethernet_stats.client_info[k].mac, QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0)
           {
             connectedDevicesList->ipa_data.rx_data = ethernet_stats.client_info[k].ipv4_rx_bytes + ethernet_stats.client_info[k].ipv6_rx_bytes;
             connectedDevicesList->ipa_data.tx_data = ethernet_stats.client_info[k].ipv4_tx_bytes + ethernet_stats.client_info[k].ipv6_tx_bytes;
             LOG_MSG_INFO1("Ethernet Client, IPA Stats, RX: %llu, TX: %llu\n", connectedDevicesList->ipa_data.rx_data, connectedDevicesList->ipa_data.tx_data, 0);
             break;
           }

           /*If GSB is enabled for eth, then we need the stats from odu device type*/
           if (memcmp(connectedDevicesList->mac_addr, odu_stats.client_info[k].mac, QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0)
           {
             connectedDevicesList->ipa_data.rx_data = odu_stats.client_info[k].ipv4_rx_bytes + odu_stats.client_info[k].ipv6_rx_bytes;
             connectedDevicesList->ipa_data.tx_data = odu_stats.client_info[k].ipv4_tx_bytes + odu_stats.client_info[k].ipv6_tx_bytes;
             LOG_MSG_INFO1("Ethernet Client, IPA Stats, RX: %llu, TX: %llu\n", connectedDevicesList->ipa_data.rx_data, connectedDevicesList->ipa_data.tx_data, 0);
             break;
           }
        }
      }
      else if (QCMAP_WLAN::GetWLANDeviceTypeFromAP(connectedDevicesList->device_type, &wlan_dev_type , qmi_err_num))
      {
        if (wlan_dev_type == QCMAP_MSGR_WLAN_DEV_ROME_V01)
        {
          LOG_MSG_INFO1("WLAN Client on Rome\n", 0, 0, 0);
          for (k = 0; k < IPA_MAX_NUM_HW_PATH_CLIENTS; k++)
          {
            if (memcmp(connectedDevicesList->mac_addr, wlan_stats.client_info[k].mac, QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0)
            {
              connectedDevicesList->ipa_data.rx_data = wlan_stats.client_info[k].ipv4_rx_bytes + wlan_stats.client_info[k].ipv6_rx_bytes;
              connectedDevicesList->ipa_data.tx_data = wlan_stats.client_info[k].ipv4_tx_bytes + wlan_stats.client_info[k].ipv6_tx_bytes;
              LOG_MSG_INFO1("WLAN Client, IPA Stats, RX: %llu, TX: %llu\n", connectedDevicesList->ipa_data.rx_data, connectedDevicesList->ipa_data.tx_data, 0);
              break;
            }
          }
        }
        else if (wlan_dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01)
        {
          LOG_MSG_INFO1("WLAN Client on Tufello\n", 0, 0, 0);
          for (k = 0; k < IPA_MAX_NUM_HW_PATH_CLIENTS; k++)
          {
            if (memcmp(connectedDevicesList->mac_addr, odu_stats.client_info[k].mac, QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0)
            {
              connectedDevicesList->ipa_data.rx_data = odu_stats.client_info[k].ipv4_rx_bytes + odu_stats.client_info[k].ipv6_rx_bytes;
              connectedDevicesList->ipa_data.tx_data = odu_stats.client_info[k].ipv4_tx_bytes + odu_stats.client_info[k].ipv6_tx_bytes;
              LOG_MSG_INFO1("WLAN Client, IPA Stats, RX: %lu, TX: %lu\n", connectedDevicesList->ipa_data.rx_data, connectedDevicesList->ipa_data.tx_data, 0);
              break;
            }
          }
        }
      }
#endif

      for (int j = 0 ; j < QCMAP_MSGR_MAX_IPV6_ADDR_V01; j++)
      {
        //if no more ipv6 then break
        if (!memcmp(connectedDevicesList->ipv6[j].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
          break;
        inet_ntop(AF_INET6, (void *)connectedDevicesList->ipv6[j].addr,
          ipv6addr, INET6_ADDRSTRLEN);
        //obtain ipv6 exception path data
        UpdateIPV6ExceptionStats(connectedDevicesList, ipv6addr);
      }
    //update tx_bytes & rx_bytes
    connectedDevicesList->bytes_rx = connectedDevicesList->exception_data.rx_data+
                                     connectedDevicesList->ipa_data.rx_data+
                                     connectedDevicesList->sfe_data.rx_data;
    connectedDevicesList->bytes_tx = connectedDevicesList->exception_data.tx_data+
                                     connectedDevicesList->ipa_data.tx_data+
                                     connectedDevicesList->sfe_data.tx_data;
    LOG_MSG_INFO1("Total Stats,RX: %llu, TX: %llu\n",
           connectedDevicesList->bytes_rx, connectedDevicesList->bytes_tx, 0);
    }


    //Copy the MAC, device type, IPv4 and IPv6 address
    memcpy (conn_devices[i].client_mac_addr, connectedDevicesList->mac_addr,
            QCMAP_MSGR_MAC_ADDR_LEN_V01);

    conn_devices[i].device_type = connectedDevicesList->device_type;
    conn_devices[i].ipv4_addr = connectedDevicesList->ip_addr;
    memcpy (conn_devices[i].ll_ipv6_addr,
            connectedDevicesList->link_local_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    for (int j = 0; j < QCMAP_MSGR_MAX_IPV6_ADDR_V01; j++)
    {
      if (!memcmp(connectedDevicesList->ipv6[j].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
        break;
      memcpy(conn_devices[i].ipv6[j].addr,
             connectedDevicesList->ipv6[j].addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    }

    conn_devices[i].bytes_rx = connectedDevicesList->bytes_rx;
    conn_devices[i].bytes_tx = connectedDevicesList->bytes_tx;
    conn_devices[i].vlan_id = connectedDevicesList->vlan_id;

    if(conn_devices[i].vlan_id != 0)
    {
      tmp_str.append(std::to_string(conn_devices[i].vlan_id));
      dnsmasq_file_name.append(tmp_str);
      tmp_str.clear();
    }

    ds_log_med("dns_file_loc:%s",dnsmasq_file_name.c_str());
    //Delete the hostname.txt file if present
    snprintf(command, MAX_COMMAND_STR_LEN, "rm %s",HOSTNAME_PATH);
    ds_system_call(command, strlen(command));

    //Prune the dnsmasq.leases file to have lease expiry time, MAC address and hostname
    snprintf(command, MAX_COMMAND_STR_LEN,
             "cut -f1,2,4 -d ' '  %s >  %s",
             dnsmasq_file_name.c_str(),
             HOSTNAME_PATH);

    ds_system_call(command, strlen(command));

    //Open the hostname.txt file to fetch lease time and hostname
    fd = fopen(HOSTNAME_PATH,"r");

    if(fd != NULL)
    {
      fetchHostNameAndLeasetime(fd ,connectedDevicesList->mac_addr,
                                connectedDevicesList->host_name,
                                &conn_devices[i].lease_expiry_time);
      //Close the hostname.txt file
      fclose(fd);
    }

    //Copy CDI hostname to client responce buffer

    memcpy (conn_devices[i].host_name,connectedDevicesList->host_name,
            QCMAP_MSGR_DEVICE_NAME_MAX_V01);


    //Point to the next node
    node = ds_dll_next (node, (const void**)(&connectedDevicesList));
    dnsmasq_file_name.clear();
    tmp_str.clear();
  }

  return true;
}


/*=====================================================
  FUNCTION GetSetBootupConfigFromXML
======================================================*/
/*!
@brief
 Gets bootup config from XML

@parameters
  - enum variable of qcmap_bootup_config_enum
  - enum variable of qcmap_action_type
  - boolean to specify mode

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean QCMAP_ConnectionManager::GetSetBootupConfigFromXML
(
  qcmap_bootup_config_enum conf,
  qcmap_action_type action,
  boolean mode
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  const char *tag_ptr;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
     LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
     return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPBootUp_TAG);

  switch (conf)
  {
    case CONFIG_MOBILEAP:
      tag_ptr = MobileAPEnable_TAG;
    break;

    case CONFIG_WLAN:
      tag_ptr = WLANEnable_TAG;
    break;

    default:
      LOG_MSG_ERROR("Invalid TAG.", 0, 0, 0);
      return false;
    break;
  }

  if (action == SET_VALUE)
  {
    snprintf(data, sizeof(data), "%d", mode);
    root.child(tag_ptr).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    atoi(root.child(tag_ptr).child_value())? mode = true: mode =false;
  }

  LOG_MSG_INFO1("Config: %d Action: %d value: %d" , conf, action, mode);
  return true;
}

/*===========================================================================
 * FUNCTION GetHWAddr
==========================================================================*/
/*!
@brief
  Get Mac address for an interface

@parameters
   Char * to store retrieved MAC Address
   devname name of the iface

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
int QCMAP_ConnectionManager::GetHWAddr(char *mac,char* devname)
{
  int s, ret;
  struct ifreq buffer;
  struct sockaddr *sa;

  LOG_MSG_INFO1("GetHWAddr",0,0,0);

  if ( mac == NULL || devname == NULL )
  {
    LOG_MSG_ERROR("Null arguements passed.\n",0,0,0);
    return QCMAP_CM_ERROR;
  }

  /* Open a socket */
  s = socket(PF_INET, SOCK_DGRAM, 0);
  if ( s < 0 )
  {
    LOG_MSG_ERROR("Unable to open socket to get IP address.\n",0,0,0);
    return QCMAP_CM_ERROR;
  }
  /* Set up the interface request buffer for wlan0. */
  memset(&buffer, 0x00, sizeof(buffer));
  strlcpy(buffer.ifr_name, devname, IFNAMSIZ);

  /* Call the ioctl to get the address. */
  ret = ioctl(s, SIOCGIFHWADDR, &buffer);

  if ( ret < 0 )
  {
    LOG_MSG_ERROR("Unable to call ioctl to get MAC address.\n",0,0,0);
    /* Close the socket handle. */
    return QCMAP_CM_ERROR;
  }

  /* Copy out the ip address for the interface. */
  sa = (struct sockaddr *)&(buffer.ifr_addr);
  memcpy(mac, buffer.ifr_hwaddr.sa_data, ETH_ALEN);

  ds_log_med("HW Address for dev %s is %02x:%02x:%02x:%02x:%02x:%02x \n",
                  devname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  close(s);
  return QCMAP_CM_SUCCESS;
}

/*======================================================
  FUNCTION:  WriteDefaultsToXML
  =====================================================*/
  /*!
      @brief
      Writes default values to XML in case if XML is not present

      @params

      @return
      QCMAP_CM_ERROR
      QCMAP_CM_SUCCESS
  */
/*====================================================*/
void QCMAP_ConnectionManager::WriteDefaultsToXML()
{
  FILE *fp = NULL;
  struct in_addr addr;
  char *xml_file=QCMAP_ConnectionManager::xml_path;
  uint32 dhcp_start_address;
  uint32 dhcp_end_address;
  uint32_t static_ip_addr = 0;
  uint32_t static_dns_addr = 0;
  uint32_t static_net_mask = 0;
  uint32_t static_gw_ip = 0;
  uint32_t dmz_ip = 0;
  profile_handle_type_v01 profile_handle = rand();

  dhcp_start_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20;
  dhcp_end_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20 + MAX_LAN_CLIENTS;

  LOG_MSG_INFO1("Creating XML file using default values",0,0,0);
  fp = fopen(xml_file, "wb");

  /* File not found at the given path */
  if (NULL == fp)
  {
    return QCMAP_CM_ERROR;
  }

  fprintf(fp, "<?xml version=\"1.0\"?>\n");
  fprintf( fp, "<system xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
               " xsi:noNamespaceSchemaLocation=\"mobileap_cfg.xsd\">\n");
  fprintf(fp, "<MobileAPCfg>\n");
  // NAT Config
  fprintf(fp, "\t<MobileAPNatCfg>\n");
  fprintf(fp, "\t<Firewall>%s</Firewall>\n", QCMAP_FIREWALL_CONFIG);
  fprintf( fp, "\t<NatEntryGenericTimeout>%d</NatEntryGenericTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_GENERIC_TIMEOUT );
  fprintf( fp, "\t<NatEntryICMPTimeout>%d</NatEntryICMPTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_ICMP_TIMEOUT );
  fprintf( fp, "\t<NatEntryTCPEstablishedTimeout>%d</NatEntryTCPEstablishedTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_TCP_TIMEOUT );
  fprintf( fp, "\t<NatEntryUDPTimeout>%d</NatEntryUDPTimeout>\n",
           QCMAP_NAT_ENTRY_DEFAULT_UDP_TIMEOUT );

  addr.s_addr = htonl(dmz_ip);
  fprintf(fp, "\t<DmzIP>%s</DmzIP>\n",inet_ntoa(addr));
  fprintf(fp, "\t<EnableIPSECVpnPassthrough>%d</EnableIPSECVpnPassthrough>\n", true);
  fprintf(fp, "\t<EnablePPTPVpnPassthrough>%d</EnablePPTPVpnPassthrough>\n", true);
  fprintf(fp, "\t<EnableL2TPVpnPassthrough>%d</EnableL2TPVpnPassthrough>\n", true);
  fprintf(fp, "\t<EnableWebserverWWANAccess>%d</EnableWebserverWWANAccess>\n", false );
  fprintf(fp, "\t<NATType>SYM</NATType>\n");
  fprintf(fp, "\t<ALGCfg>\n");
  fprintf(fp, "\t\t<EnableRTSPAlg>%d</EnableRTSPAlg>\n", false );
  fprintf(fp, "\t\t<EnableSIPAlg>%d</EnableSIPAlg>\n", false );
  fprintf(fp, "\t</ALGCfg>\n");
  fprintf(fp, "\t<Initial_Pkt_Limit>%d</Initial_Pkt_Limit>\n",0);
  fprintf(fp, "</MobileAPNatCfg>\n");

  fprintf(fp, "<MobileAPLanCfg>\n");
  //Backhaul Config
  fprintf(fp, "\t<EnableIPV4>%d</EnableIPV4>\n",true);
  fprintf(fp, "\t<EnableIPV6>%d</EnableIPV6>\n",true);
  fprintf(fp,"\t<GatewayURL>%s</GatewayURL>\n",GATEWAY_URL);
  fprintf(fp,"\t<WlanMode>%s</WlanMode>\n",AP_TAG);
  fprintf(fp,"\t<WlanRestartDelay>%d</WlanRestartDelay>\n",WLAN_DEFAULT_DELAY);
  fprintf(fp, "\t<MobileAPSTABridgeEnable>%d</MobileAPSTABridgeEnable>\n", false);
  fprintf(fp, "\t<HostAPDCfg>%s</HostAPDCfg>\n", HOSTAPD_CFG_PATH);
  fprintf(fp, "\t<HostAPDEntropy>%s</HostAPDEntropy>\n", HOSTAPD_ENTROPY);
  fprintf(fp, "\t<STAModeHostAPDCfg>%s</STAModeHostAPDCfg>\n", STA_MODE_HOSTAPD_CFG_PATH);
  fprintf(fp, "\t<APIPAddr>%s</APIPAddr>\n", APPS_LAN_IP_ADDR);
  fprintf(fp, "\t<SubNetMask>%s</SubNetMask>\n", APPS_SUB_NET_MASK);

  //LAN Config
  fprintf(fp, "\t<EnableDHCPServer>%d</EnableDHCPServer>\n", true);
  fprintf(fp, "\t<DHCPCfg>\n");
  addr.s_addr = htonl(dhcp_start_address);
  fprintf(fp, "\t\t<StartIP>%s</StartIP>\n", inet_ntoa(addr));
  addr.s_addr = htonl(dhcp_end_address);
  fprintf(fp, "\t\t<EndIP>%s</EndIP>\n", inet_ntoa(addr));
  fprintf(fp, "\t\t<LeaseTime>%d</LeaseTime>\n", DHCP_LEASE_TIME);
  fprintf(fp, "\t</DHCPCfg>\n");
  fprintf(fp, "\t<IPPassthroughCfg>\n");
  fprintf(fp, "\t<IPPassthroughEnable>%d</IPPassthroughEnable>\n", false);
  fprintf(fp, "\t<IPPassthroughDevice>%d</IPPassthroughDevice>\n", -1);

  fprintf(fp, "\t<IPPassthroughUsbHost>NULL</IPPassthroughUsbHost>\n");
  fprintf(fp, "\t<IPPassthroughEthMac>NULL</IPPassthroughEthMac>\n");
  fprintf(fp, "\t</IPPassthroughCfg>\n");

  /* <GuestAPCfg> */
  fprintf(fp, "\t<GuestAPCfg>\n");
  fprintf(fp, "\t\t<HostAPDCfg>%s</HostAPDCfg>\n", HOSTAPD_CFG_PATH_SSID2);
  fprintf(fp, "\t\t<HostAPDEntropy>%s</HostAPDEntropy>\n",HOSTAPD_ENTROPY_SSID2);
  fprintf(fp, "\t\t<AccessProfile>%s</AccessProfile>\n",INTERNET_TAG);
  fprintf(fp, "\t</GuestAPCfg>\n");

  /* <GuestAP2Cfg> */
  fprintf(fp, "\t<GuestAP2Cfg>\n");
  fprintf(fp, "\t\t<HostAPDCfg>%s</HostAPDCfg>\n", HOSTAPD_CFG_PATH_SSID3);
  fprintf(fp, "\t\t<HostAPDEntropy>%s</HostAPDEntropy>\n",HOSTAPD_ENTROPY_SSID3);
  fprintf(fp, "\t\t<AccessProfile>%s</AccessProfile>\n",INTERNET_TAG);
  fprintf(fp, "\t</GuestAP2Cfg>\n");

  /* <GuestAP3Cfg> */
  fprintf(fp, "\t<GuestAPCfg>\n");
  fprintf(fp, "\t\t<HostAPDCfg>%s</HostAPDCfg>\n", HOSTAPD_CFG_PATH_SSID4);
  fprintf(fp, "\t\t<HostAPDEntropy>%s</HostAPDEntropy>\n",HOSTAPD_ENTROPY_SSID4);
  fprintf(fp, "\t\t<AccessProfile>%s</AccessProfile>\n",INTERNET_TAG);
  fprintf(fp, "\t</GuestAPCfg>\n");

  /* <StationModeCfg> */
  fprintf(fp, "\t<StationModeCfg>\n");
  fprintf(fp, "\t\t<SupplicantCfg>%s</SupplicantCfg>\n", SUPPLICANT_CFG_PATH);
  fprintf(fp, "\t\t<STAModeConnType>%d</STAModeConnType>\n",
          QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01);
  addr.s_addr = htonl(static_ip_addr);
  fprintf(fp, "\t\t<StaticIPAddr>%s</StaticIPAddr>\n", inet_ntoa(addr));
  addr.s_addr = htonl(static_dns_addr);
  fprintf(fp, "\t\t<StaticConfigDNSAddr>%s</StaticConfigDNSAddr>\n", inet_ntoa(addr));
  addr.s_addr = htonl(static_gw_ip);
  fprintf(fp, "\t\t<StaticConfigGWAddr>%s</StaticConfigGWAddr>\n", inet_ntoa(addr));
  addr.s_addr = htonl(static_net_mask);
  fprintf(fp, "\t\t<StaticConfigNetMask>%s</StaticConfigNetMask>\n", inet_ntoa(addr));
  fprintf(fp, "\t</StationModeCfg>\n");

  /* <AlwaysOnWLANCfg> */
  fprintf(fp, "\t<AlwaysOnWLAN>%d</AlwaysOnWLAN>\n", QCMAP_DEFAULT_ALWAYS_ON_WLAN);

  /* End MobileAPLanCfg tag */
  fprintf(fp, "</MobileAPLanCfg>\n");

  // WAN Config
  fprintf(fp, "<MobileAPWanCfg>\n");
  fprintf(fp, "\t<EriConfig>%s</EriConfig>\n", QCMAP_ERI_CONFIG);
  fprintf(fp, "\t<FirstPreferredBackhaul>bt-pan</FirstPreferredBackhaul>\n");
  fprintf(fp, "\t<SecondPreferredBackhaul>eth</SecondPreferredBackhaul>\n");
  fprintf(fp, "\t<ThirdPreferredBackhaul>usb_cradle</ThirdPreferredBackhaul>\n");
  fprintf(fp, "\t<FourthPreferredBackhaul>wlan</FourthPreferredBackhaul>\n");
  fprintf(fp, "\t<FifthPreferredBackhaul>wwan</FifthPreferredBackhaul>\n");
  fprintf(fp, "\t<Profile>%d</Profile>\n", 1);
  fprintf(fp, "\t<AutoConnect>%d</AutoConnect>\n", false);
  fprintf(fp, "\t<Roaming>%d</Roaming>\n", false);
  fprintf(fp, "\t<TECH>%s</TECH>\n", TECH_ANY_TAG);
  fprintf(fp, "\t<V4_UMTS_PROFILE_INDEX>%d</V4_UMTS_PROFILE_INDEX>\n", 1);
  fprintf(fp, "\t<V4_CDMA_PROFILE_INDEX>%d</V4_CDMA_PROFILE_INDEX>\n", 0);
  fprintf(fp, "\t<V6_UMTS_PROFILE_INDEX>%d</V6_UMTS_PROFILE_INDEX>\n", 1);
  fprintf(fp, "\t<V6_CDMA_PROFILE_INDEX>%d</V6_CDMA_PROFILE_INDEX>\n", 0);
  fprintf(fp, "\t<DefaultSIPServerConfigType>%s</DefaultSIPServerConfigType>\n",
            FQDN_TAG );
   fprintf(fp, "\t<DefaultSIPServerConfig>%s</DefaultSIPServerConfig>\n",
            DEFAULT_SIP_SERVER);
  fprintf(fp, "\t<PrefixDelegation>%d</PrefixDelegation>\n", false);
  fprintf(fp, "</MobileAPWanCfg>\n");

  //Daemon Services Config
  fprintf(fp, "<MobileAPSrvcCfg>\n");
  fprintf(fp, "\t<UPnP>%d</UPnP>\n", false);
  fprintf(fp, "\t<DLNA>%d</DLNA>\n", false);
  fprintf(fp, "\t<MDNS>%d</MDNS>\n", false);
  fprintf(fp, "</MobileAPSrvcCfg>\n");

  //DDNS Config
  fprintf(fp, "<DDNSCfg>\n");
  fprintf(fp, "\t<EnableDDNS>%d</EnableDDNS>\n",0);
  fprintf(fp, "\t<DDNSServer>%s</DDNSServer>\n",QCMAP_DYNDNS_SERVER_URL);
  fprintf(fp, "\t<DDNSProtocol>%s</DDNSProtocol>\n",QCMAP_DYNDNS_PROTOCOL);
  fprintf(fp, "\t<DDNSLogin>%s</DDNSLogin>\n",DDNS_DEFAULT_LOGIN);
  fprintf(fp, "\t<DDNSPassword>%s</DDNSPassword>\n",DDNS_DEFAULT_PASSWORD);
  fprintf(fp, "\t<DDNSHostname>%s</DDNSHostname>\n",DDNS_DEFAULT_HOSTNAME);
  fprintf(fp, "\t<DDNSTimeout>%d</DDNSTimeout>\n",DDNS_DEFAULT_TIMEOUT);
  fprintf(fp, "</DDNSCfg>\n");

  //TinyProxy
  fprintf(fp, "<TinyProxyCfg>\n");
  fprintf(fp, "\t<EnableTinyProxy>%d</EnableTinyProxy>\n",0);
  fprintf(fp, "</TinyProxyCfg>\n");

  /* Bootup Config Flags*/
  fprintf(fp, "<MobileAPBootUpCfg>\n");
  fprintf(fp, "\t<MobileAPEnableAtBootup>%d</MobileAPEnableAtBootup>\n", false );
  fprintf(fp, "\t<WLANEnableAtBootup>%d</WLANEnableAtBootup>\n", false );
  fprintf(fp, "</MobileAPBootUpCfg>\n");

  /* Cradle Flags */
  fprintf(fp, "<Cradle>\n");
  fprintf(fp, "\t<CradleMode>%d</CradleMode>\n",false);
  fprintf(fp, "</Cradle>\n");

  /* ETH Backhaul Flags */
  fprintf(fp, "<EthBackhaul>\n");
  fprintf(fp, "\t<EthBackhaulMode>%d</EthBackhaulMode>\n",
                        QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01);
  fprintf(fp, "</EthBackhaul>\n");

  /* Packet Stats Flags */
  fprintf(fp, "<PacketStats>\n");
  fprintf(fp, "\t<PacketStatsCfg>%d</PacketStatsCfg>\n",
          false);
  fprintf(fp, "</PacketStats>\n");

    /* GSB Flags */
  fprintf(fp, "<GSBConfig>\n");
  fprintf(fp, "\t<GSBBootUpcfg>%d</GSBBootUpcfg>\n",
          QCMAP_DEFAULT_GSB_VAL);
  fprintf(fp, "\t<num_of_entries>%d</num_of_entries>\n",
          QCMAP_DEFAULT_GSB_VAL);
  fprintf(fp, "</GSBConfig>\n");

  /*Dun Dongle Mode*/
  fprintf(fp ,"\t<DunDongleMode>%d</DunDongleMode>\n",
          QCMAP_DEFAULT_DUN_DONGLE_MODE);

  /* PMIPv6 Config Flags */
  fprintf(fp, "<PMIPv6>\n");
  fprintf(fp, "\t<EnablePMIPmode>%d</EnablePMIPmode>\n", false );
  fprintf (fp, "\t<EnablePMIPDebugmode>%d</EnablePMIPDebugmode>\n",false);
  fprintf(fp, "\t<PMIPmodeType>%d</PMIPmodeType>\n", 0 );
  fprintf(fp, "\t<LMAv6Address>%d</LMAv6Address>\n", false );
  fprintf(fp, "\t<LMAv4Address>%d</LMAv4Address>\n", false );
  fprintf(fp, "\t<PMIPV4TunnelType>%d</PMIPV4TunnelType>\n", 6 );
  fprintf(fp, "\t<PMIPV4WorkMode>%d</PMIPV4WorkMode>\n", false );
  fprintf(fp, "\t<PMIPV4SecRouterDMNPPrefix>%d</PMIPV4SecRouterDMNPPrefix>\n", false );
  fprintf(fp, "\t<PMIPV4SecRouterdmnpPrefixLen>%d</PMIPV4SecRouterdmnpPrefixLen>\n", false );
  fprintf(fp, "\t<PMIPMobileNodeIdentifierType>%d</PMIPMobileNodeIdentifierType>\n", 0 );
  fprintf(fp, "\t<PMIPMobileNodeIdentifierString>%d</PMIPMobileNodeIdentifierString>\n", -1 );
  fprintf(fp, "\t<PMIPServiceSelectionString>%d</PMIPServiceSelectionString>\n", -1 );


  fprintf(fp, "</PMIPv6>\n");

  fprintf(fp, "</MobileAPCfg>\n");
  fprintf(fp, "</system>\n");

  fclose(fp);
}

/*=====================================================================
  FUNCTION DeleteConnectedDevicesClients
======================================================================*/
/*!
@brief
  Delete all entries in Connected Devices module based on the device type passed

@parameters:
  device_type - device type

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================================*/
void QCMAP_ConnectionManager::DeleteConnectedDevicesClients
(
qcmap_msgr_device_type_enum_v01 device_type
)
{
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  boolean wlan_client = false;
  qmi_error_type_v01 qmi_err_num;
  char ip6_addr_buf[INET6_ADDRSTRLEN];
  in6_addr tmpipv6;
  char command[MAX_COMMAND_STR_LEN];
  char ipv4addr[INET_ADDRSTRLEN];
  memset(ipv4addr,0,INET_ADDRSTRLEN);
  char ipv6addr[INET6_ADDRSTRLEN];
  memset(ipv6addr,0,INET6_ADDRSTRLEN);
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  memset(zero_buff,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  char temp_mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  int i=0;
  qcmap_msgr_wlan_device_type_v01 wlan_dev_type;

#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
  struct wan_ioctl_query_per_client_stats  lan_stats;
#endif
  LOG_MSG_INFO1("DeleteConnectedDevicesClients() with device_type %d",\
                device_type,0,0);

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_INFO1("DeleteConnectedDevicesClients - Head Null -> No entries"\
                  " to delete \n",0,0,0);
    return;
  }

  memset(ip6_addr_buf, 0, INET6_ADDRSTRLEN);
  memset(command, 0, MAX_COMMAND_STR_LEN);
  memset(&tmpipv6, 0, sizeof(tmpipv6));

  wlan_client = ((device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
      (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)||
      (device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01)) ? true : false;

  if (QcMapMgr->packet_stats_enabled && !wlan_client)
  {

#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
        //Get IPA update and update statistics for the client getting disconnected
        memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

        if (device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
          ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_USB;
        else
          ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;

        ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

        ioctl_buffer.disconnect_clnt = 1;
        ioctl_buffer.reset_stats = 1;

        UpdateIPAStats(&ioctl_buffer);

        memcpy(&lan_stats, &ioctl_buffer, sizeof(struct wan_ioctl_query_per_client_stats));

#endif
   }

  // Delete devices from the connected device list
  node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),\
                     (void*)&device_type, QCMAPLANMgr->qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)ds_dll_data(node);
    // This case should not exist since we are getting the node based on node->data match in ds_dll_delete
    if (list_data == NULL)
    {
      LOG_MSG_ERROR("DeleteConnectedDevicesClients - The device information node is NULL\n",0,0,0);
    }
    else
    {
      /* Delete connection track entries for wlan client */
      if (list_data->ip_addr)
      {
        QCMAP_Tethering::DelConntrack(list_data->ip_addr);
      }

      for (i = 0 ; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
      {
        if (!memcmp(list_data->ipv6[i].addr, zero_buff, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
        {
          break;
        }
        QcMapMgr->DelV6Conntrack(list_data->ipv6[i].addr,
                               list_data->mac_addr);
      }

      memset(temp_mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
      ds_mac_addr_ntop(list_data->mac_addr, temp_mac_addr_char);
      if(QcMapBackhaulWWANMgr)
      {
        QcMapBackhaulWWANMgr->DeleteDelegatedPrefix(temp_mac_addr_char,
                                                    &qmi_err_num);
      }
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        if (QCMAPLANMgr->DelEbtablesRuleForBridgeMode(list_data) == false)
        {
          LOG_MSG_ERROR("Could not Delete Ebtables rules for device",0,0,0);
        }
      }

#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
      // before freeing the device info send message to client
      if (QcMapMgr->packet_stats_enabled && (!wlan_client || (wlan_client && QcMapWLANMgr && !QcMapWLANMgr->disable_wlan_in_progress)))
      {
        /* For WLAN client, get the stats per client if WLAN disable is not in progress. */
        if (wlan_client)
        {
          memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));
          if (QCMAP_WLAN::GetWLANDeviceTypeFromAP(device_type, &wlan_dev_type ,&qmi_err_num))
          {
            if (wlan_dev_type == QCMAP_MSGR_WLAN_DEV_ROME_V01)
              ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_WLAN;
            else if (wlan_dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01)
              ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;
          }

          ioctl_buffer.num_clients = 1;

          memcpy(ioctl_buffer.client_info[0].mac,list_data->mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);

          ioctl_buffer.disconnect_clnt = 1;
          ioctl_buffer.reset_stats = 1;

          UpdateIPAStats(&ioctl_buffer);
        }

        memset(&lan_client_msg, 0, sizeof(lan_client_msg));
        switch (device_type)
        {
            case QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01:
              snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num1);
              break;
            case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01:
              snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num2);
              break;
            case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01:
              snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num3);
              break;
            case QCMAP_MSGR_DEVICE_TYPE_USB_V01:
              if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS)
                snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "rndis0");
              else if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM)
                snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "ecm0");
              else
                goto skip;
              break;
            case QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01:
              snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "eth0");
              break;
          }
          memcpy(lan_client_msg.lan_client.mac, list_data->mac_addr, IPA_MAC_ADDR_SIZE);
          lan_client_msg.client_event = IPA_PER_CLIENT_STATS_DISCONNECT_EVENT;
          LOG_MSG_INFO1("Send IPA_PER_CLIENT_STATS_DISCONNECT_EVENT to IPACM",0,0,0);
          NotifyIPACM(&lan_client_msg);
      }
skip:
#endif
      if( QcMapBackhaulMgr && QcMapMgr->packet_stats_enabled &&
      ((QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)||
      (QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED))&&
      (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN) &&
      !(QcMapWLANMgr != NULL &&
      ((list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
      (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)||
      (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01))
      && QcMapWLANMgr->disable_wlan_in_progress))
      {
         /* this is the place where we need to send msg to SFE/IPA & fetch data from exception path*/
        if((list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
             (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01)||
             (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01)||
             (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)||
             (list_data->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01))
        {
          //reset as we are about  to grab latest data.
          list_data->exception_data.rx_data = 0;
          list_data->exception_data.tx_data = 0;
          list_data->sfe_data.rx_data = 0;
          list_data->sfe_data.tx_data = 0;

          //get SFE stats - this api  will do it for both ipv6 and ipv4
          if (QcMapMgr->sfe_loaded)
          {
            UpdateSFEStats(list_data);
          }

          list_data->ipa_data.rx_data = 0;
          list_data->ipa_data.tx_data = 0;

#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
          if (!wlan_client)
          {
            for (int i = 0; i < IPA_MAX_NUM_HW_PATH_CLIENTS; i++)
            {
              if (memcmp(list_data->mac_addr, lan_stats.client_info[i].mac, QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0)
              {
                 list_data->ipa_data.rx_data = lan_stats.client_info[i].ipv4_rx_bytes + lan_stats.client_info[i].ipv6_rx_bytes;
                 list_data->ipa_data.tx_data = lan_stats.client_info[i].ipv4_tx_bytes + lan_stats.client_info[i].ipv6_tx_bytes;
                 break;
               }
            }
          }
          else
          {
            list_data->ipa_data.rx_data = ioctl_buffer.client_info[0].ipv4_rx_bytes + ioctl_buffer.client_info[0].ipv6_rx_bytes;
            list_data->ipa_data.tx_data = ioctl_buffer.client_info[0].ipv4_tx_bytes + ioctl_buffer.client_info[0].ipv6_tx_bytes;
          }
#endif
          if (0 != list_data->ip_addr)
          {
            inet_ntop(AF_INET,
                     (void *)&list_data->ip_addr,ipv4addr,
                      INET_ADDRSTRLEN);

            //obtain ipv4 exception path data
            UpdateIPV4ExceptionStats(list_data, ipv4addr);
            if (QcMapMgr->sfe_loaded)
            {
              SendNLMsgToSFE(sfev4_nl_sock_fd, SFE_IPV4_DELETE_PACKET_STATS_NODE, ipv4addr);
            }
            //delete exception path rule for  ipv4 connections
            DeleteIPv4IptableRule(ipv4addr);
          }

          /*At this point we need ot send command to SFE and exception path v4 and v6
          remove all connections related to client*/
          for (i = 0 ; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
          {
            // if not more ipv6 then break
            if (!memcmp(list_data->ipv6[i].addr, zero_buff, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
            {
              break;
            }
            // we need to test this properly. Multiple NL sent quickly
            inet_ntop(AF_INET6, (void *)list_data->ipv6[i].addr,
              ipv6addr, INET6_ADDRSTRLEN);
            //obtain ipv4 exception path data
            UpdateIPV6ExceptionStats(list_data, ipv6addr);
            if (QcMapMgr->sfe_loaded)
            {
              SendNLMsgToSFE(sfev6_nl_sock_fd, SFE_IPV6_DELETE_PACKET_STATS_NODE, ipv6addr);
            }
            //Delete ipv6 connetions from exception path and update statistics.
            DeleteIPv6IptableRule(ipv6addr);
          }
            QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_CLIENT_DISCONNECTED_V01,
                             1, list_data);
        }
      }

      /*Delete the LAN interface devices at PMIP deamon also*/
      if(QcMapBackhaulMgr && QcMapBackhaulMgr->IsPmipV6ModeEnabled() || QcMapBackhaulMgr->IsPmipV4ModeEnabled())
      {
        QcMapBackhaulMgr->PmipDeleteDevice(list_data);
      }

      // Free the device information structure
      ds_free (list_data);
      list_data = NULL;
    }
    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;
    QcMapMgr->conn_device_info.numOfNodes--;

    if ((device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
         device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01 ||
         device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01 ||
         device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01 ))
    {
      QcMapMgr->conn_device_info.numofWlanclients--;
    }

    if (QcMapMgr->IsDynamicClockVotingNeeded())
    {
      if ( (QcMapMgr->conn_device_info.numofWlanclients == 0))
      {
        QcMapMgr->conn_device_info.wlanclientpresent = false;
        if (QcMapBackhaulMgr && !(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)&&
            (QcMapMgr->IsRatLteFdd()) && (QcMapMgr->isBWLteCAT4()) &&
            ((QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)||
             (QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED))) &&
             (QcMapMgr->CheckUsbClockAtNom()))
        {
          QcMapMgr->SetUSBClockFreq(false);
        }
      }
    }

    node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),\
                               (void*)&device_type,\
                               QCMAPLANMgr->qcmap_match_device_type);
  }
  if( addrList->addrListHead->next == NULL )
  {
    LOG_MSG_INFO1("\n No device info exist- Delete the head node\n",0,0,0);
    ds_dll_free(addrList->addrListHead);
    addrList->addrListHead = NULL;
  }
}

/*=====================================================
  FUNCTION UpdateDeviceEntryInfo
======================================================*/
/*!
@brief
  - Update the Connected Device Entry info as needed with:
    a)device type

@parameters
  Pointer to qcmap_nl_buf
  Device type

@return
  bool
/*=====================================================*/
bool QCMAP_ConnectionManager::UpdateDeviceEntryInfo
(
  qcmap_nl_sock_msg_t*             qcmap_nl_buf,
  qcmap_msgr_device_type_enum_v01  device_entry_type
)
{
  QCMAP_ConnectionManager*      QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  ds_dll_el_t *                 node = NULL;
  qcmap_addr_info_list_t*       addrList = &(QcMapMgr->addrList);
  uint8                         mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_cm_client_data_info_t*  list_data = NULL;
  qcmap_mac_addr_vlan_info_t    mac_addr_vlan_info;
  qcmap_nl_addr_t*              nl_addr = NULL;

  if (qcmap_nl_buf == NULL)
  {
    LOG_MSG_ERROR("Input nl_buf is NULL \n",0,0,0);
    return false;
  }

  nl_addr = &(qcmap_nl_buf->nl_addr);
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);

  ds_mac_addr_ntop(nl_addr->mac_addr, mac_addr_char);
  ds_log_med("Update device entry info for mac addr %s", mac_addr_char);

  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Update device entry info:Linked list head is NULL",0,0,0);
    return false;
  }

  memcpy(mac_addr_vlan_info.mac_addr,nl_addr->mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);
  mac_addr_vlan_info.vlan_id = qcmap_nl_buf->vlan_id;

  node = ds_dll_search (addrList->addrListHead , (void*)&mac_addr_vlan_info,
                        QcMapMgr->qcmap_match_mac_addr_vlan_id_pair);
  if( node == NULL)
  {
    LOG_MSG_ERROR("Update device entry info:MAC address not found in list",0,0,0);
    return false;
  }
  else
  {
    list_data = (qcmap_cm_client_data_info_t*)(node->data);
    if (list_data == NULL)
    {
      LOG_MSG_ERROR("Update device entry info: Error in fetching node data ",0,0,0);
      return false;
    }

    ds_mac_addr_ntop(list_data->mac_addr, mac_addr_char);
    ds_log_med("Update device entry info: Found node with MAC addr %s \n",
               mac_addr_char);

    /*Update device type if needed*/
    if (list_data->device_type != device_entry_type )
    {
      LOG_MSG_INFO1("Previous device type %d, current device type %d",
                    list_data->device_type,
                    device_entry_type, 0);
      list_data->device_type = device_entry_type;
      LOG_MSG_INFO1("Reset IP address since client connected over new interface",
                    0, 0, 0);
      memset(&(list_data->ip_addr), 0, sizeof(list_data->ip_addr));
      memset(&(list_data->link_local_v6_addr), 0, sizeof(list_data->link_local_v6_addr));
      for (int i = 0; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
      {
        memset(list_data->ipv6[i].addr, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      }


      /* Update device entry info with IP address as well */
      Getclientaddr(nl_addr, mac_addr_vlan_info.vlan_id);
      LOG_MSG_INFO1("UpdateDeviceEntryInfo:Updating IP address for the client",0, 0, 0);
      if((QCMAP_ConnectionManager::MatchMacAddrInList(qcmap_nl_buf))== 0)
      {
        LOG_MSG_ERROR("UpdateDeviceEntryInfo: No match found for the new client MAC",0, 0, 0);
      }
    }
  }

  return true;
}


/*===========================================================================
  FUNCTION DelV6Conntrack
==========================================================================*/
/*!
@brief
  Deleted all the conntrack entries for the Ipv6 address

@parameters
  - Ipv6 address pointer

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::DelV6Conntrack
(
  uint8* ipv6addr,
  uint8* macaddr
)
{
  in6_addr tmpipv6, nullipv6;
  char command[MAX_COMMAND_STR_LEN];
  char mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  FILE *fp = NULL;
  char stringline[MAX_COMMAND_STR_LEN];
  char *v6charAddr=NULL, *ptr;
  uint8 v6Addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01] ;
  struct ps_in6_addr *prefix_ptr;

  memset(command, 0, MAX_COMMAND_STR_LEN);
  memset(&tmpipv6, 0, sizeof(tmpipv6));
  memset(&nullipv6, 0, sizeof(nullipv6));
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  memset(v6Addr, 0, sizeof(v6Addr));

  if ( NULL == ipv6addr )
  {
    LOG_MSG_ERROR("DelV6Conntrack(), client Ipv6 addr"
                  " not present", 0, 0, 0);
    return;
  }
  else
  {
    memcpy ( &tmpipv6.s6_addr, ipv6addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01 );
    if (memcmp( &tmpipv6 , &nullipv6, sizeof(tmpipv6)) == 0 )
    {
      LOG_MSG_INFO1("DelV6Conntrack(), client Ipv6 addr not present",
                    0, 0, 0);
      return;
    }
    else
    {
      ds_mac_addr_ntop( macaddr, mac_addr_char);
      snprintf(command, MAX_COMMAND_STR_LEN,"ip -6 neighbour show |grep %s > %s",
         mac_addr_char,IPV6_NEIGH_ADDR_TABLE);
      ds_system_call(command, strlen(command));

       /* open the file */
      fp = fopen(IPV6_NEIGH_ADDR_TABLE, "r");
      if (fp == NULL)
      {
        LOG_MSG_ERROR("Couldn't open IPV6_NEIGH_ADDR_TABLE file for reading %d", errno, 0, 0);
        return;
      }

      while(fgets( stringline, MAX_COMMAND_STR_LEN, fp) != NULL)
      {
        v6charAddr = strtok_r( stringline, " ", &ptr);

        if ( NULL != v6charAddr )
        {
          if(!inet_pton(AF_INET6, v6charAddr,&v6Addr))
          {
            LOG_MSG_INFO1("Not a valid IPV6 addr ",0,0,0);
          }
          else
          {
            prefix_ptr = (struct ps_in6_addr*)(v6Addr);
            if (!QCMAP_IN6_IS_PREFIX_LINKLOCAL(prefix_ptr->ps_s6_addr32))
            {
              snprintf(command,MAX_COMMAND_STR_LEN,"echo Deleting Client Conntrack IPv6 :%s > /dev/kmsg",v6charAddr);
              ds_system_call(command,strlen(command));

              snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --orig-src %s",v6charAddr);
              ds_system_call(command, strlen(command));

              snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --reply-src %s",v6charAddr);
              ds_system_call(command, strlen(command));
            }
          }
        }
      }

      fclose(fp);
      snprintf(command, MAX_COMMAND_STR_LEN,"rm %s",IPV6_NEIGH_ADDR_TABLE);
      ds_system_call(command, strlen(command));
    }
  }

  return;
}


/*===========================================================================
  FUNCTION fetchHostName
==========================================================================*/
/*!
@brief
  fetches the host name from data file for given mac string mac_str.

@parameters
  host    - output host name
  mac_str - corrosponding mac address

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::fetchHostName
(
  char *host,
  char *mac_str
)
{
  char data[MAX_COMMAND_STR_LEN];
  char command[MAX_COMMAND_STR_LEN];
  FILE *in=NULL;

  snprintf(data, MAX_COMMAND_STR_LEN,
           "cat %s | grep %s  | awk {'print $3'}", TEMP_HOST_FILE, mac_str);

  if(!(in = popen(data, "r")))
  {
    return;
  }

  //reading buffer return by popen
  if(fgets(data, sizeof(data), in)!=NULL && (data[0] != '\n') && (data[0] != '*'))
  {
    strlcpy( host,data,strlen(data)+1 );
  }
   pclose(in);
  snprintf(command,MAX_COMMAND_STR_LEN, "rm -f %s", TEMP_HOST_FILE);
  ds_system_call(command, strlen(command));

}

/*============================================================
  FUNCTION FetchIpv6Conntrack
=============================================================*/
/*!
@brief
  Fetch Ipv6 UDP and TCP conntrack entries for processing.
  Compare the src and dst Ipv6 addresses in conntrack entries
  and flush conntrack if the src and dst Ipv6 prefix doesnt
  match the prefix of the backhaul interface Ipv6 prefix

@parameters
  - Ipv6 address pointer

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*===========================================================*/
void QCMAP_ConnectionManager::FetchIpv6Conntrack
(
  char* ipv6addr
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (ipv6addr == NULL)
  {
    LOG_MSG_ERROR("IPv6 address pointer is NULL", 0, 0, 0);
    return false;
  }
  //Fetch the Ipv6 conntrack for UDP

  snprintf(command,MAX_COMMAND_STR_LEN,
           "conntrack -L -f ipv6| grep udp | cut -f9,10,11 -d ' ' > %s",IPv6_CONNTRACK);
  ds_system_call(command, strlen(command));

  QcMapMgr->CompareIPv6PrefixAndFlushConntrack(ipv6addr);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPv6_CONNTRACK);
  ds_system_call(command, strlen(command));

  //2. Fetch the Ipv6 conntrack for TCP
  snprintf(command,MAX_COMMAND_STR_LEN,
           "conntrack -L -f ipv6| grep tcp | cut -f10,11,12 -d ' ' > %s",IPv6_CONNTRACK);
  ds_system_call(command, strlen(command));

  QcMapMgr->CompareIPv6PrefixAndFlushConntrack(ipv6addr);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPv6_CONNTRACK);
  ds_system_call(command, strlen(command));
}

/*============================================================
  FUNCTION CompareIPv6PrefixAndFlushConntrack
=============================================================*/
/*!
@brief
  Compare the src and dst Ipv6 addresses in conntrack entries
  and flush conntrack if the src and dst Ipv6 prefix doesnt
  match the prefix of the backhaul interface Ipv6 prefix

@parameters
  - Ipv6 address pointer

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*===========================================================*/
void QCMAP_ConnectionManager::CompareIPv6PrefixAndFlushConntrack
(
  char* ipv6addr
)
{
  FILE *fd = NULL;
  struct ps_in6_addr *prefix_addr_ptr = NULL;
  struct ps_in6_addr *src_addr_ptr = NULL;
  struct ps_in6_addr *dst_addr_ptr = NULL;
  char *src_addr = NULL, *dst_addr = NULL, *ptr;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN];
  char command[MAX_COMMAND_STR_LEN]={0};
  in6_addr src_ipv6_addr, dst_ipv6_addr;


  prefix_addr_ptr = (struct ps_in6_addr *)ipv6addr;


  fd = fopen(IPv6_CONNTRACK,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("CompareIPv6PrefixAndFlushConntrack - Error in opening %s",
                  IPv6_CONNTRACK,0,0);
    return;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    src_addr = strtok_r(stringline, " ", &ptr);
    dst_addr = strtok_r(NULL, " ", &ptr);

    inet_pton(AF_INET6, src_addr + IPV6_ADDR_OFFSET, (void *)&src_ipv6_addr);
    inet_pton(AF_INET6, dst_addr + IPV6_ADDR_OFFSET, (void *)&dst_ipv6_addr);

    src_addr_ptr = (struct ps_in6_addr *)src_ipv6_addr.s6_addr;
    dst_addr_ptr = (struct ps_in6_addr *)dst_ipv6_addr.s6_addr;

    if (!(QCMAP_IN6_IS_PREFIX_LINKLOCAL(src_addr_ptr->ps_s6_addr32) &&
         QCMAP_IN6_IS_PREFIX_LINKLOCAL(dst_addr_ptr->ps_s6_addr32)))
    {
      if (memcmp (prefix_addr_ptr->ps_s6_addr64,src_addr_ptr->ps_s6_addr64,
           sizeof(uint64)) == 0)
      {
        if (memcmp (prefix_addr_ptr->ps_s6_addr64,dst_addr_ptr->ps_s6_addr64,
             sizeof(uint64)) == 0)
        {
          //If both Prefix match it can be LAN-LAN, so dont delete conntrack
          continue;
        }

        //Use dst addr for deletion
        snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --orig-dst %s",
                 dst_addr+4);
        ds_system_call(command, strlen(command));
        continue;
      }

      //Use the src addr for deletion
      snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --orig-src %s",
               src_addr+4);
      ds_system_call(command, strlen(command));
    }
  }
  fclose(fd);
}


/*===========================================================================
  FUNCTION IsDynamicClockVotingNeeded
==========================================================================*/
/*!
@brief
   Is dynamic clock voting needed for the target

@parameters
   none

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_ConnectionManager::IsDynamicClockVotingNeeded()
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if ( QcMapMgr == NULL )
  {
    LOG_MSG_ERROR("ConnectionMgr Object is NULL\n", 0, 0, 0);
    return false;
  }

  if ( QcMapMgr->target == DS_TARGET_LYKAN && QcMapMgr->usb_clk_node_present)
  {
    return true;
  }
  else
  {
    return false;
  }
}


/*===========================================================================
  FUNCTION SetUSBClockFreq
==========================================================================*/
/*!
@brief
   Set the New Clock frequency

@parameters
  - Bool to indicate to set to NOM freq

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::SetUSBClockFreq( boolean setNOM )
{

  char command[MAX_COMMAND_STR_LEN];

  /* For MDM9x06, USB vote should be NOM. */
  if (setNOM || (this->target == DS_TARGET_LE_MDM9x06))
  {
    LOG_MSG_INFO1("Setting Clock Speed to NOMINAL\n", 0, 0, 0);

    snprintf(command, MAX_COMMAND_STR_LEN,
           "echo enable > /sys/class/udc/msm_hsusb/device/perf_mode");
    ds_system_call(command, strlen(command));
    this->dyn_clk_info.usb_clock_at_nom = true;
  }
  else
  {
    LOG_MSG_INFO1("Setting Clock Speed to SVS\n", 0, 0, 0);

    snprintf(command, MAX_COMMAND_STR_LEN,
           "echo disable > /sys/class/udc/msm_hsusb/device/perf_mode");
    ds_system_call(command, strlen(command));
    this->dyn_clk_info.usb_clock_at_nom = false;
  }
  return;
}

/*===========================================================================
  FUNCTION QcmapSFEv6NLInit
==========================================================================*/
/*!
@brief
  Initialize the NL socket for QCMAP to communicate with SFE IPV6 Module

@parameters
None

@return

@note
- Dependencies
- packet stats need to be enabled
 - SFE need to be loaded

- Side Effects
- None
*/
/*=========================================================================*/


int QcmapSFEv6NLInit(void)
{
  struct sockaddr_nl qcmap_addr;

  sfev6_nl_sock_fd = socket(PF_NETLINK, SOCK_RAW, NL_IPV6_PROTO_ID);
  if (sfev6_nl_sock_fd < 0)
  {
    LOG_MSG_ERROR("packet stats IPv6 NL Socket issue",0,0,0);
    return FAILURE;
  }

  memset(&qcmap_addr, 0, sizeof(qcmap_addr));

  qcmap_addr.nl_family = AF_NETLINK;
  qcmap_addr.nl_pid = (getpid() & NL_PID_MASK);

  if(bind(sfev6_nl_sock_fd, (struct sockaddr *)&qcmap_addr, sizeof(qcmap_addr))<0)
  {
    LOG_MSG_ERROR("packet stats IPv6 NL Failed to bind",0,0,0);
    close(sfev6_nl_sock_fd);
    return FAILURE;
  }

  return 0;
}

/*===========================================================================
  FUNCTION QcmapSFEv4NLInit
==========================================================================*/
/*!
@brief
  Initialize the NL socket for QCMAP to communicate with SFE IPV4 Module

@parameters
None

@return

@note
- Dependencies
- packet stats need to be enabled
- SFE need to be loaded

- Side Effects
- None
*/
/*=========================================================================*/
int QcmapSFEv4NLInit(void)
{
  struct sockaddr_nl qcmap_addr;

  sfev4_nl_sock_fd = socket(PF_NETLINK, SOCK_RAW, NL_IPV4_PROTO_ID);
  if (sfev4_nl_sock_fd < 0)
  {
    LOG_MSG_ERROR("packet stats IPv4 NL Socket issue",0,0,0);
    return FAILURE;
  }

  memset(&qcmap_addr, 0, sizeof(qcmap_addr));

  qcmap_addr.nl_family = AF_NETLINK;
  qcmap_addr.nl_pid = (getpid() & NL_PID_MASK);

  if(bind(sfev4_nl_sock_fd, (struct sockaddr *)&qcmap_addr, sizeof(qcmap_addr))<0)
  {
    LOG_MSG_ERROR("packet stats IPv4 NL Failed to bind",0,0,0);
    close(sfev4_nl_sock_fd);
    return FAILURE;
  }

  return 0;
}


/*=====================================================
  FUNCTION EnablePacketStats
======================================================*/
/*!
@brief
  - Enables Per client packet stats feature

@parameters
  Pointer to qmi_err_num

@return
  boolean

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::EnablePacketStats
(
  qmi_error_type_v01 *qmi_err_num
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if(!xml_file.load_file(this->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  /*If VLAN is configured, enabling packet stats is not allowed*/
  if (QcMapMgr && (IS_VLAN_ALLOWED(QcMapMgr->target)))
  {
    if ((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG).child(VCONFIG_TAG)) != NULL)
    {
      LOG_MSG_ERROR("Packet Stats are not allowed, since VLAN(s) is/are configured", 0, 0, 0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
    }
  }

  root =
    xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PacketStats_TAG);
    root.child(PacketStatsCfg_TAG).text() = 1;
  this->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
#ifndef FEATURE_DATA_TARGET_MDM9607
  //write to IPACM xml file to enable packet stats for IPA
  if(!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(IPAPacketStats_TAG);
  root.child(IPAPacketStatsCfg_TAG).text() = 1;
  QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif /* FEATURE_DATA_TARGET_MDM9607 */

  return true;
}

/*=====================================================
  FUNCTION DisablePacketStats
======================================================*/
/*!
@brief
  - Disables Per client packet stats feature

@parameters
  Pointer to qmi_err_num

@return
  boolean

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::DisablePacketStats
(
  qmi_error_type_v01 *qmi_err_num
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if(!xml_file.load_file(this->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root =
    xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PacketStats_TAG);
    root.child(PacketStatsCfg_TAG).text() = 0;
  this->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);

#ifndef FEATURE_DATA_TARGET_MDM9607
  //write to IPACM xml file to disable packet stats for IPA
  if(!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(IPAPacketStats_TAG);
  root.child(IPAPacketStatsCfg_TAG).text() = 0;
  QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
  return true;
}

/*=====================================================
  FUNCTION ResetPacketStats
======================================================*/
/*!
@brief
  - Resets Per client packet stats feature

@parameters
  Pointer to qmi_err_num

@return
  boolean

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::ResetPacketStats
(
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  unsigned int dummy = 0;

  if (!QcMapMgr->packet_stats_enabled)
  {
    LOG_MSG_INFO1("packet stat not enabled",0,0,0);
    return false;
  }

  ResetSfeDataStats();
  ResetExceptionDataStats();
  if (QcMapMgr->TraverseCDIAndUpdate(QCMAP_RESET_IPA_CLIENT_DATA,
                                              NULL,
                                              &dummy))
  {
    LOG_MSG_INFO1("RESET IPA succeeded, entries: %d\n",
                  dummy,0,0);
  }
  else
  {
    LOG_MSG_ERROR("No Connected Devices Information available currently",
                  0,0,0);
  }

  return true;

}

/*=====================================================
  FUNCTION GetPacketStatsStatus
======================================================*/
/*!
@brief
  - Obtain state of packet stats

@parameters
  Pointer to qmi_err_num

@return
  boolean

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_ConnectionManager::GetPacketStatsStatus
(
  qmi_error_type_v01 *qmi_err_num
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;

  if(!xml_file.load_file(this->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root =
    xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PacketStats_TAG);
  return (atoi(root.child(PacketStatsCfg_TAG).child_value()));
}


/*===========================================================================
  FUNCTION UpdateCDIWithNeighInfo
==========================================================================*/
/*!
@brief
  Updates CDI structure with any change in neigh info

@parameters
  - nl packet buffer
  - type of interface on which neigh is added

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_ConnectionManager::UpdateCDIWithNeighInfo
(
  qcmap_nl_sock_msg_t* pktbuf
)
{
  switch (pktbuf->nl_event)
  {
    case QCMAP_NL_NEWNEIGH:
    {
      switch (pktbuf->nl_iface)
      {
        case QCMAP_NL_WLAN:
          LookUpCDIAndUpdate(pktbuf);
          break;
        case QCMAP_NL_USB:
          LookUpCDIAndUpdate(pktbuf);
          break;
        case QCMAP_NL_ETH:
          LookUpCDIAndUpdate(pktbuf);
          break;
        case QCMAP_NL_BRIDGE:
          LookUpCDIAndUpdate(pktbuf);
          break;
      }
    }
      break;

    case QCMAP_NL_DELNEIGH:
        break;
  }

  return;
}
/*===========================================================================
  FUNCTION SendPacketStatsInd
==========================================================================*/
/*!
@brief
  Sends the indication to client registered to receive packet stat status

@parameters
  - packet stats status enum
   - number of devices
  - device list pointer


@return
  boolean

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_ConnectionManager::SendPacketStatsInd
(
  qcmap_msgr_packet_stats_state_enum_v01 status,
  int number_of_entries,
  qcmap_cm_client_data_info_t* dev_info
)
{
  char command[MAX_COMMAND_STR_LEN];
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;
  FILE *fd = NULL;

  cbPtr = (qmi_qcmap_msgr_status_cb_data *)this->GetQCMAPClientCBDatata();
  //send indication
  if (cbPtr && cbPtr->packet_stats_cb)
  {
    //Delete the hostname.txt file if present
    snprintf(command, MAX_COMMAND_STR_LEN, "rm %s",HOSTNAME_PATH);
    ds_system_call(command, strlen(command));

    //Prune the dnsmasq.leases file to have lease expiry time, MAC address and hostname
    snprintf(command, MAX_COMMAND_STR_LEN,
             "cut -f1,2,4 -d ' ' /var/lib/misc/dnsmasq.leases > %s",
              HOSTNAME_PATH);
    ds_system_call(command, strlen(command));

    //Open the hostname.txt file to fetch lease time and hostname
    fd = fopen(HOSTNAME_PATH,"r");
    if(fd == NULL)
    {
      LOG_MSG_ERROR("Error in opening %s",
                    HOSTNAME_PATH,0,0);
      return false;
    }
    if (dev_info != NULL)
    {
      fetchHostNameAndLeasetime(fd, dev_info->mac_addr,
                                dev_info->host_name,
                                &(dev_info->lease_expiry_time));
    }
    cbPtr->packet_stats_cb(cbPtr, status, number_of_entries ,dev_info);
  }
  else
  {
    LOG_MSG_INFO1("Callback ptr is NULL",0,0,0);
    return false;
  }

  fclose(fd);
  return true;
}

/*===========================================================================
  FUNCTION TraverseCDIAndUpdate
==========================================================================*/
/*!
@brief
traverses the CDI LL and perform the action provided

@parameters
- Action to be performed on list


@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_ConnectionManager::TraverseCDIAndUpdate(qcmap_update_cdi action,
                         qcmap_msgr_connected_device_info_v01 *conn_devices,
                         unsigned int* num_entries)
{
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  unsigned int wlan_entries = 0;
  char ipv4addr[INET_ADDRSTRLEN];
  memset(ipv4addr,0,INET_ADDRSTRLEN);
  char ipv6addr[INET6_ADDRSTRLEN];
  memset(ipv6addr,0,INET6_ADDRSTRLEN);
  char command[MAX_COMMAND_STR_LEN];
  FILE *fd = NULL;
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01] = {0};
  int k = 0;
  qcmap_msgr_wlan_device_type_v01 wlan_dev_type;
  qmi_error_type_v01 qmi_err_num;


  //Fetch the number of nodes in linked list and store in num_entries
  *num_entries = QcMapMgr->conn_device_info.numOfNodes;
  if ( *num_entries == 0 )
  {
    LOG_MSG_ERROR("No devices present",0,0,0);
    return false;
  }

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Linked list head is NULL"
                  "Cant fetch connected devices info",0,0,0);
    return false;
  }


  node = addrList->addrListHead;

  node = ds_dll_next (node, (const void**)(&connectedDevicesList));

  if ( node == NULL)
  {
    LOG_MSG_ERROR("Device info is NULL"
                  "Cant fetch connected devices info",0,0,0);
    return false;
  }

  //Delete the hostname.txt file if present
  snprintf(command, MAX_COMMAND_STR_LEN, "rm %s",HOSTNAME_PATH);
  ds_system_call(command, strlen(command));

  //Prune the dnsmasq.leases file to have lease expiry time, MAC address and hostname
  snprintf(command, MAX_COMMAND_STR_LEN,
           "cut -f1,2,4 -d ' ' /var/lib/misc/dnsmasq.leases > %s",
           HOSTNAME_PATH);
  ds_system_call(command, strlen(command));

  //Open the hostname.txt file to fetch lease time and hostname
  fd = fopen(HOSTNAME_PATH,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("Error in opening %s",
                  HOSTNAME_PATH,0,0);
    return false;
  }

  switch (action)
  {
    case QCMAP_CLEANUP_PACKET_STATS_RULES:
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
      /* Reset the IPA Stats first for all the device types. */
      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_USB;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);

      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_WLAN;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);

      //Get IPA update and update statistics for the client getting disconnected
      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);

      //Get IPA update and update statistics for the client getting disconnected

      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);


#endif
      for ( int i=0; i < *num_entries && connectedDevicesList; i++ )
      {
         if (0 != connectedDevicesList->ip_addr)
         {
           inet_ntop(AF_INET,
                    (void *)&connectedDevicesList->ip_addr,ipv4addr,
                     INET_ADDRSTRLEN);
           DeleteIPv4IptableRule(ipv4addr);
           if (QcMapMgr->sfe_loaded)
           {
             SendNLMsgToSFE(sfev4_nl_sock_fd, SFE_IPV4_DELETE_PACKET_STATS_NODE, ipv4addr);
           }
         }
         for (int j = 0 ; j < QCMAP_MSGR_MAX_IPV6_ADDR_V01; j++)
         {
           //if no more ipv6 then break
           if (!memcmp(connectedDevicesList->ipv6[j].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
             break;
           inet_ntop(AF_INET6, (void *)connectedDevicesList->ipv6[j].addr,
             ipv6addr, INET6_ADDRSTRLEN);
           DeleteIPv6IptableRule(ipv6addr);
           if (QcMapMgr->sfe_loaded)
           {
             SendNLMsgToSFE(sfev6_nl_sock_fd, SFE_IPV6_DELETE_PACKET_STATS_NODE, ipv6addr);
           }
           // We need to zero out the CDI IPV6 too. We can get NL later once mobileap comes backup or BH comes back up.
           memset(connectedDevicesList->ipv6[j].addr, 0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
         }
         node = ds_dll_next (node, (const void**)(&connectedDevicesList));
      }
      break;
    case QCMAP_RESET_IPA_CLIENT_DATA:
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
      /* Reset the IPA Stats first for all the device types. */
      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_USB;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);

      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_WLAN;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);

      //Get IPA update and update statistics for the client getting disconnected
      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);

      //Get IPA update and update statistics for the client getting disconnected

       memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

       ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;

       ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

       ioctl_buffer.reset_stats = 1;

       UpdateIPAStats(&ioctl_buffer);

#endif
      break;


    case QCMAP_UPDATE_WLAN_CLIENTS_DATA:
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
      //Get IPA update and update statistics for the client getting disconnected
      memset(&ioctl_buffer, 0, sizeof(ioctl_buffer));

      ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_WLAN;

      ioctl_buffer.num_clients = IPA_MAX_NUM_HW_PATH_CLIENTS;

      ioctl_buffer.disconnect_clnt = 1;
      ioctl_buffer.reset_stats = 1;

      UpdateIPAStats(&ioctl_buffer);

#endif
      for ( int i=0; i < *num_entries && connectedDevicesList; i++ )
      {
        if (connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01 ||
            connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01 ||
            connectedDevicesList->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01)
        {
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
        if(QCMAP_WLAN::GetWLANDeviceTypeFromAP(connectedDevicesList->device_type, &wlan_dev_type ,&qmi_err_num))
         {
           if (wlan_dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01)
           {
             //Get IPA update and update statistics for the client getting disconnected
             memset(&odu_ioctl_buffer, 0, sizeof(odu_ioctl_buffer));

             odu_ioctl_buffer.device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;

             odu_ioctl_buffer.num_clients = 1;
             memcpy(odu_ioctl_buffer.client_info[0].mac,connectedDevicesList->mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);

             odu_ioctl_buffer.disconnect_clnt = 1;
             odu_ioctl_buffer.reset_stats = 1;

             UpdateIPAStats(&odu_ioctl_buffer);
           }
         }
#endif
          //reset as we are about  to grab latest data.
          connectedDevicesList->exception_data.rx_data = 0;
          connectedDevicesList->exception_data.tx_data = 0;
          connectedDevicesList->sfe_data.rx_data = 0;
          connectedDevicesList->sfe_data.tx_data = 0;
          connectedDevicesList->ipa_data.rx_data = 0;
          connectedDevicesList->ipa_data.tx_data = 0;
          //get SFE stats - this api  will do it for both ipv6 and ipv4
          if (QcMapMgr->sfe_loaded)
          {
            UpdateSFEStats(connectedDevicesList);
          }
          if (0 != connectedDevicesList->ip_addr)
          {

            inet_ntop(AF_INET,
                     (void *)&connectedDevicesList->ip_addr,ipv4addr,
                      INET_ADDRSTRLEN);
            //obtain ipv4 exception path data
            UpdateIPV4ExceptionStats(connectedDevicesList, ipv4addr);
            if (QcMapMgr->sfe_loaded)
            {
              SendNLMsgToSFE(sfev4_nl_sock_fd, SFE_IPV4_DELETE_PACKET_STATS_NODE, ipv4addr);
            }
            //delete exception path rule for  ipv4 connections
            DeleteIPv4IptableRule(ipv4addr);
          }
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
          /* Update the stats. */
          if (wlan_dev_type != QCMAP_MSGR_WLAN_DEV_TUF_V01)
          {
            for (k = 0; k < IPA_MAX_NUM_HW_PATH_CLIENTS; k++)
            {
              if (memcmp(connectedDevicesList->mac_addr, ioctl_buffer.client_info[k].mac, QCMAP_MSGR_MAC_ADDR_LEN_V01) == 0)
              {
                connectedDevicesList->ipa_data.rx_data = ioctl_buffer.client_info[k].ipv4_rx_bytes + ioctl_buffer.client_info[k].ipv6_rx_bytes;
                connectedDevicesList->ipa_data.tx_data = ioctl_buffer.client_info[k].ipv4_tx_bytes + ioctl_buffer.client_info[k].ipv6_tx_bytes;
                LOG_MSG_INFO1("IPA Stats, RX: %llu, TX: %llu\n", connectedDevicesList->ipa_data.rx_data,
                   connectedDevicesList->ipa_data.tx_data, 0);
                break;
              }
            }
          }
          else
          {
			connectedDevicesList->ipa_data.rx_data = odu_ioctl_buffer.client_info[0].ipv4_rx_bytes + odu_ioctl_buffer.client_info[0].ipv6_rx_bytes;
			connectedDevicesList->ipa_data.tx_data = odu_ioctl_buffer.client_info[0].ipv4_tx_bytes + odu_ioctl_buffer.client_info[0].ipv6_tx_bytes;
			LOG_MSG_INFO1("IPA Stats, RX: %llu, TX: %llu\n", connectedDevicesList->ipa_data.rx_data,
				 connectedDevicesList->ipa_data.tx_data, 0);
          }

          memset(&lan_client_msg, 0, sizeof(lan_client_msg));
          switch (connectedDevicesList->device_type)
          {
            case QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01:
              snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num1);
              break;
            case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01:
              snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num2);
              break;
            case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01:
              snprintf(lan_client_msg.lan_client.lanIface, IFNAMSIZ, "wlan%d",QcMapMgr->ap_dev_num3);
              break;
          }
          memcpy(lan_client_msg.lan_client.mac, connectedDevicesList->mac_addr, IPA_MAC_ADDR_SIZE);
          lan_client_msg.client_event = IPA_PER_CLIENT_STATS_DISCONNECT_EVENT;
          NotifyIPACM(&lan_client_msg);
#endif
          for (int j = 0 ; j < QCMAP_MSGR_MAX_IPV6_ADDR_V01; j++)
          {
            //if no ipv6 left then break loop
            if (!memcmp(connectedDevicesList->ipv6[j].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
              break;
            inet_ntop(AF_INET6, (void *)connectedDevicesList->ipv6[j].addr,
              ipv6addr, INET6_ADDRSTRLEN);
            //obtain ipv6 exception path data
            UpdateIPV6ExceptionStats(connectedDevicesList, ipv6addr);
            if (QcMapMgr->sfe_loaded)
            {
              SendNLMsgToSFE(sfev6_nl_sock_fd, SFE_IPV6_DELETE_PACKET_STATS_NODE, ipv6addr);
            }
            //Delete ipv6 connetions from exception path and update statistics.
            DeleteIPv6IptableRule(ipv6addr);
          }
          //update tx_bytes & rx_bytes
          connectedDevicesList->bytes_rx = connectedDevicesList->exception_data.rx_data+
                                           connectedDevicesList->ipa_data.rx_data+
                                           connectedDevicesList->sfe_data.rx_data;
          connectedDevicesList->bytes_tx = connectedDevicesList->exception_data.tx_data+
                                           connectedDevicesList->ipa_data.tx_data+
                                           connectedDevicesList->sfe_data.tx_data;
          LOG_MSG_INFO1("Total Stats, RX: %llu, TX: %llu\n", connectedDevicesList->bytes_rx,
             connectedDevicesList->bytes_tx, 0);
          fetchHostNameAndLeasetime(fd ,connectedDevicesList->mac_addr,
                                        connectedDevicesList->host_name,
                                        &conn_devices[wlan_entries].lease_expiry_time);

          //Copy the MAC, device type, IPv4 and IPv6 address
          memcpy (conn_devices[wlan_entries].client_mac_addr, connectedDevicesList->mac_addr,
                  QCMAP_MSGR_MAC_ADDR_LEN_V01);

          //Copy CDI hostname to client responce buffer
          memcpy (conn_devices[wlan_entries].host_name,connectedDevicesList->host_name,
                  QCMAP_MSGR_DEVICE_NAME_MAX_V01);
          conn_devices[wlan_entries].device_type = connectedDevicesList->device_type;
          conn_devices[wlan_entries].ipv4_addr = connectedDevicesList->ip_addr;
          memcpy (conn_devices[wlan_entries].ll_ipv6_addr,
                  connectedDevicesList->link_local_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

          for (int j = 0; j < QCMAP_MSGR_MAX_IPV6_ADDR_V01; j++)
          {
            if (!memcmp(connectedDevicesList->ipv6[j].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
              break;
            memcpy(conn_devices[wlan_entries].ipv6[j].addr,
                   connectedDevicesList->ipv6[j].addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
          }

          conn_devices[wlan_entries].bytes_rx = connectedDevicesList->bytes_rx;
          conn_devices[wlan_entries].bytes_tx = connectedDevicesList->bytes_tx;
          wlan_entries++;

        }
        node = ds_dll_next (node, (const void**)(&connectedDevicesList));
      }
      break;
  }

  if (action == QCMAP_UPDATE_WLAN_CLIENTS_DATA)
  {
    *num_entries = wlan_entries;
  }
  fclose(fd);
  return true;
}


/*=====================================================
  FUNCTION GetBackhaulObjectFromMap
======================================================*/
/*!
@brief
  Gets QCMAP_Backhaul obj for a given profile_handle

@return
  QCMAP_Backhaul object pointer
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

QCMAP_Backhaul* QCMAP_ConnectionManager::GetBackhaulObjectFromMap
(
  profile_handle_type_v01 profile_handle
)
{
  std::map<profile_handle_type_v01, QCMAP_Backhaul*>::iterator it;
  if (QCMAP_ConnectionManager::QCMAP_Backhaul_Hash == NULL)
  {
    LOG_MSG_ERROR("hash_map not created yet!", 0,0,0);
    return NULL;
  }

  it = QCMAP_ConnectionManager::QCMAP_Backhaul_Hash->find(profile_handle);
  if (it == QCMAP_ConnectionManager::QCMAP_Backhaul_Hash->end())
  {
    LOG_MSG_ERROR("Unable to find QCMAP_Backhaul Object for profile_handle = %d",
                   profile_handle, 0, 0);
    return NULL;
  }
  return it->second;
}


/*=====================================================
  FUNCTION GetBackhaulObjectFromVLAN
======================================================*/
/*!
@brief
  Gets QCMAP_Backhaul obj for a given VLAN ID

@return
  QCMAP_Backhaul object pointer
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

QCMAP_Backhaul* QCMAP_ConnectionManager::GetBackhaulObjectFromVLAN
(
  int16_t vlan_id
)
{
  QCMAP_Backhaul *QcMapBackhaul;

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    QcMapBackhaul = GET_BACKHAUL_OBJ_AND_INC;
    if (vlan_id ==  QcMapBackhaul->vlan_id)
    {
      return QcMapBackhaul;
    }
  }
  return NULL;
}


/*=====================================================
  FUNCTION InsertBackhaulObjectToMap
======================================================*/
/*!
@brief
  Inserts QCMAP_Backhaul obj into map

@return
  None
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_ConnectionManager::InsertBackhaulObjectToMap
(
    profile_handle_type_v01  profile_handle,
    QCMAP_Backhaul          *object
 )
{
  ds_assert(object != NULL); //Assert if object is NULL.
  std::map<profile_handle_type_v01, QCMAP_Backhaul*>::iterator it;
  if (QCMAP_ConnectionManager::QCMAP_Backhaul_Hash == NULL)
  {
    QCMAP_ConnectionManager::QCMAP_Backhaul_Hash = new map<profile_handle_type_v01, QCMAP_Backhaul *>();
  }

  it = QCMAP_ConnectionManager::QCMAP_Backhaul_Hash->find(profile_handle);
  if (it != QCMAP_ConnectionManager::QCMAP_Backhaul_Hash->end())
  {
    LOG_MSG_ERROR("QCMAP_Backhaul Object is already present for profile_handle = %p",
                  profile_handle, 0, 0);
    //return it->second;
    return;
  }

  QCMAP_Backhaul_Hash->insert(pair<profile_handle_type_v01, QCMAP_Backhaul*> (profile_handle, object) );
}

/*=====================================================
 FUNCTION RemoveBackhaulObjectFromMap
======================================================*/
/*!
@brief
 Removes QCMAP_Backhaul obj into map

@return
 None
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
 void QCMAP_ConnectionManager::RemoveBackhaulObjectFromMap
(
  profile_handle_type_v01  profile_handle
)
{
  QCMAP_Backhaul *QcMapBackhaul;
  std::map<profile_handle_type_v01, QCMAP_Backhaul*>::iterator it;

  if (QCMAP_ConnectionManager::QCMAP_Backhaul_Hash == NULL)
  {
    return;
  }

  it = QCMAP_ConnectionManager::QCMAP_Backhaul_Hash->find(profile_handle);
  if (it == QCMAP_ConnectionManager::QCMAP_Backhaul_Hash->end())
  {
    LOG_MSG_ERROR("QCMAP_Backhaul Object is not present for profile_handle = %p",
               profile_handle, 0, 0);
    //return it->second;
    return;
  }

  QcMapBackhaul = it->second;
  if (QcMapBackhaul)
    delete QcMapBackhaul;

  //Remove Backhaul object from map
  QCMAP_Backhaul_Hash->erase(it);
}

/*=====================================================
  FUNCTION BackhaulObjectCountFromMap
======================================================*/
/*!
@brief
  Number of QCMAP_Backhaul obj in map

@return
  number of QCMAP_Backhaul obj in map
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
uint8 QCMAP_ConnectionManager::BackhaulObjectCountFromMap()
{
  if (QCMAP_ConnectionManager::QCMAP_Backhaul_Hash == NULL)
  {
    LOG_MSG_ERROR("hash_map not created yet!", 0,0,0);
    return 0;
  }
  return QCMAP_ConnectionManager::QCMAP_Backhaul_Hash->size();
}

/*=====================================================
  FUNCTION GetBackhaulWWANObject
======================================================*/
/*!
@brief
  Gets QCMAP_Backhaul_WWAN obj for a given profile_handle

@return
  QCMAP_Backhaul_WWAN object pointer
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
QCMAP_Backhaul_WWAN *QCMAP_ConnectionManager::GetBackhaulWWANObject
(
  profile_handle_type_v01 profile_handle
)
{
  QCMAP_Backhaul *backhaulObj;
  if ( QCMAP_ConnectionManager::Get_Instance(NULL,false) != NULL)
  {
    backhaulObj = QCMAP_ConnectionManager::Get_Instance(NULL,false)-> \
             GetBackhaulObjectFromMap(profile_handle);
    if (backhaulObj != NULL)
      return backhaulObj->QcMapBackhaulWWAN;
  }
  return NULL;
}

/*=====================================================
  FUNCTION GetNatAlgObject
======================================================*/
/*!
@brief
  Gets QCMAP_NATALG obj for a given profile_handle

@return
  QCMAP_NATALG object pointer
@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
QCMAP_NATALG *QCMAP_ConnectionManager::GetNatAlgObject
(
  profile_handle_type_v01 profile_handle
)
{
  QCMAP_Backhaul *backhaulObj;
  if ( QCMAP_ConnectionManager::Get_Instance(NULL,false) != NULL)
  {
    backhaulObj = QCMAP_ConnectionManager::Get_Instance(NULL,false)-> \
             GetBackhaulObjectFromMap(profile_handle);
    if (backhaulObj != NULL)
      return backhaulObj->QcMapNatAlg;
  }
  return NULL;
}


/*=====================================================
  FUNCTION GetLANObject
======================================================*/
/*!
@brief
  Gets QCMAP_LAN obj for a given vlan_id

@return
  QCMAP_LAN object pointer
@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
QCMAP_LAN* QCMAP_ConnectionManager::GetLANObject(int16_t vlan_id)
{
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;

  LOG_MSG_INFO1("Attempting to retrieve LAN object %d", vlan_id, 0, 0);

  if(lan_mgr)
  {
    lan_obj = lan_mgr->GetLANBridge(vlan_id);
  }

  return lan_obj;
}

/*=====================================================
  FUNCTION GetFirewallObject
======================================================*/
/*!
@brief
  Gets QCMAP_Firewall obj for a given profile_handle

@return
  QCMAP_Firewall object pointer
@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
QCMAP_Firewall *QCMAP_ConnectionManager::GetFirewallObject
(
  profile_handle_type_v01 profile_handle
)
{
  QCMAP_Backhaul *backhaulObj;
  if ( QCMAP_ConnectionManager::Get_Instance(NULL,false) != NULL)
  {
    backhaulObj = QCMAP_ConnectionManager::Get_Instance(NULL,false)-> \
             GetBackhaulObjectFromMap(profile_handle);
    if (backhaulObj != NULL)
      return backhaulObj->QcMapFirewall;
  }
  return NULL;
}

/*=====================================================
  FUNCTION GetBackhaulObject
======================================================*/
/*!
@brief
  Get Backhaul object for a given profile

@return
  QCMAP_Backhaul object pointer
@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
QCMAP_Backhaul *QCMAP_ConnectionManager::GetBackhaulObject
(
  profile_handle_type_v01 profile_handle
)
{
  QCMAP_Backhaul *backhaulObj;
  if ( QCMAP_ConnectionManager::Get_Instance(NULL,false) != NULL)
  {
    backhaulObj = QCMAP_ConnectionManager::Get_Instance(NULL,false)-> \
             GetBackhaulObjectFromMap(profile_handle);
    if (backhaulObj != NULL)
      return backhaulObj;
  }
  return NULL;
}

/*=====================================================
  FUNCTION GetBackhaulObjectFromVLANid
======================================================*/
/*!
@brief
  Get Backhaul object for a given vlan id

@return
  QCMAP_Backhaul object pointer
@note
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
QCMAP_Backhaul *QCMAP_ConnectionManager::GetBackhaulObjectFromVLANid
(
  int16_t vlan_id
)
{
  QCMAP_Backhaul *backhaulObj;
  if ( QCMAP_ConnectionManager::Get_Instance(NULL,false) != NULL)
  {
    backhaulObj = QCMAP_ConnectionManager::Get_Instance(NULL,false)-> \
             GetBackhaulObjectFromVLAN(vlan_id);
    if (backhaulObj != NULL)
      return backhaulObj;
  }
  return NULL;
}

/*===========================================================================
  FUNCTION DataPathOptInit
==========================================================================*/
/*!
@brief
  To install/ remove the data optimization module
@parameters
  flag

@return
  true  -on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean
QCMAP_ConnectionManager::DataPathOptInit
(
  boolean flag
)
{
    boolean ret = 0;
    if(flag == 1)
    {
      ret = ds_system_call("data_path_opt -lSFE",strlen("data_path_opt -lSFE"));
      LOG_MSG_INFO1("Enabling data path opt ret value = %d",ret,0,0);
    }
    else if(flag == 0)
    {
      ret = ds_system_call("data_path_opt -uSFE",strlen("data_path_opt -uSFE"));
      LOG_MSG_INFO1("disabling data path opt ret value = %d",ret,0,0);
    }

    return ret;
}

/*===========================================================================
FUNCTION SetDataPathOptManagerStatus
===========================================================================*/
/**
  Use to enable/disable data path optimization.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
/*=========================================================================*/

boolean QCMAP_ConnectionManager::SetDataPathOptManagerStatus
(
  boolean data_path_opt_status,
  qmi_error_type_v01 *qmi_err_num
)
{
    int ret = 0;
    pugi::xml_document xml_file;
    pugi::xml_node root, child ;
    char data[MAX_STRING_LENGTH] = {0};
/*-----------------------------------------------------------------------------*/

    if (!qmi_err_num)
    {
      LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
      return false;
    }

    LOG_MSG_INFO1("set action, module_enable = %d\n",data_path_opt_status,0,0);
    ret = this->DataPathOptInit(data_path_opt_status);
    if (ret != DS_SYS_CALL_SUCCESS)
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    this->data_path_opt_enable = data_path_opt_status;

    if (!xml_file.load_file(this->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPBootUp_TAG);
    snprintf(data,sizeof(data),"%d",this->data_path_opt_enable);
    root.child(DataPathOpt_Tag).text() = data;
    this->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    return true;
}

/*===========================================================================
FUNCTION GetDataPathOptManagerStatus()
===========================================================================*/
/**
  Use to know the status of data path optimization whether enabled/disabled.
  @datatypes
  qmi_error_type_v01

  @param [in] data_path_opt_status      Status of Data Path Optimizer
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
/*=========================================================================*/

boolean QCMAP_ConnectionManager::GetDataPathOptManagerStatus
(
  boolean *data_path_opt_status,
  qmi_error_type_v01 *qmi_err_num
)
{

    if (!qmi_err_num)
    {
      LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
      return false;
    }

    *qmi_err_num = QMI_ERR_NONE;
    if (data_path_opt_status == NULL)
    {
      LOG_MSG_ERROR("GetDataPathOptManagerStatus() - Invalid parameter received",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    *data_path_opt_status = this->data_path_opt_enable;
    LOG_MSG_INFO1("get action, module status = %d", *data_path_opt_status, 0, 0);
    return true;
}

/*===========================================================================
FUNCTION GetAssociatedBridgeIface()
===========================================================================*/
/**
  Get's bridge iface name for a particular vlan_id.

  @param [in] int16             vlan_id

  @return
  const char*       Bridge Iface name

  @dependencies
  QCMobileAP must be enabled.
*/
/*=========================================================================*/

const char *QCMAP_ConnectionManager::GetAssociatedBridgeIface(int16_t vlan_id)
{
  QCMAP_LAN *lan_obj;

  lan_obj = GET_LAN_OBJECT(vlan_id);
  if (lan_obj == NULL)
    return BRIDGE_IFACE; //Return default in case of failure???

  return lan_obj->GetBridgeNameAsCStr();
}

/*===========================================================================
FUNCTION TriggerAutoReboot()
===========================================================================*/
/**
  Auto reboot is triggered based on the reboot flag.

  @param [in] bool                              auto_reboot_flag
  @param [in] qcmap_cm_auto_reboot_reason_code  reason_code

  @return
  true  - reboot triggered
  false - reboot not triggered

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

bool QCMAP_ConnectionManager::TriggerAutoReboot
(
  bool auto_reboot_flag,
  qcmap_cm_auto_reboot_reason_code reason_code
) const
{
  pid_t pid;
  char command[MAX_COMMAND_STR_LEN] = {0};

  LOG_MSG_INFO1("Auto reboot flag: %d", auto_reboot_flag, 0, 0);

  //Fork a reboot process
  if(auto_reboot_flag)
  {
    switch(reason_code)
    {
      case QCMAP_CM_VLAN_ADDED_ON_PHY:
        ds_system_call("echo VLAN added on PHY, requires reboot > /dev/kmsg",
                       strlen("echo VLAN added on PHY, requires reboot > /dev/kmsg"));
        break;
      case QCMAP_CM_LAST_VLAN_REMOVED_ON_PHY:
        ds_system_call("echo Last VLAN removed on PHY, requires reboot > /dev/kmsg",
                       strlen("echo Last VLAN removed on PHY, requires reboot > /dev/kmsg"));
        break;
      case QCMAP_CM_VLAN_ADD_REMOVE_ON_BRIDGE0:
        ds_system_call("echo VLAN added or removed from bridge0, requires reboot > /dev/kmsg",
                       strlen("echo vlan added or removed from bridge0, requires reboot > /dev/kmsg"));
        break;
      case QCMAP_CM_VLAN_ADD_WITH_PKT_STATS_ON:
        ds_system_call("echo VLAN added when pkt stats is enabled, requires reboot > /dev/kmsg",
                       strlen("echo VLAN added when pkt stats is enabled, requires reboot > /dev/kmsg"));
        break;
      default:
        ds_system_call("echo unknown module triggered reboot > /dev/kmsg",
                       strlen("echo unknown module triggered reboot > /dev/kmsg"));
        break;
    }

    if((pid = fork()) < 0)
    {
      LOG_MSG_ERROR("failure to fork: %s", strerror(errno), 0, 0);
      return auto_reboot_flag;
    }

    if(0 == pid)
    { //child

      //sleep 5 seconds first
      LOG_MSG_INFO1("Sleeping 5 seconds before reboot...", 0, 0, 0);
      memset(command, 0, sizeof(command));
      snprintf(command, sizeof(command), "sleep 5");
      ds_system_call(command, strlen(command));

      //sync and write to flash
      memset(command, 0, sizeof(command));
      snprintf(command, sizeof(command), "sync");
      ds_system_call(command, strlen(command));

      //reboot
      LOG_MSG_INFO1("Going for reboot...", 0, 0, 0);
      memset(command, 0, sizeof(command));
      snprintf(command, sizeof(command), "sys_reboot");
      ds_system_call(command, strlen(command));

      return auto_reboot_flag;
    }
    else
    { //parent
      return auto_reboot_flag;
    }
  }

  return auto_reboot_flag;
}

/*===========================================================================
  FUNCTION SendNSOnBridgeIface
==========================================================================*/
/*!
@brief
  Generate a NS Message on bridge iface.

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
boolean QCMAP_ConnectionManager::SendNSOnBridgeIface(int16_t vlan_id, uint8_t *dst_v6_addr)
{
  int ret;
  int loop = 0;
  struct nd_neighbor_solicit ns;
  struct sockaddr_in6 dst_addr;
  QCMAP_LAN* QCMAPLANMgr=GET_LAN_OBJECT(vlan_id);

  LOG_MSG_INFO1("SendNSOnBridgeIface",0,0,0);

  if (QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! LAN Object is NULL", 0, 0, 0);
    return false;
  }

  memset(&dst_addr, 0, sizeof(dst_addr));
  memset(&ns, 0, sizeof(nd_neighbor_solicit));

  // Populate icmp6_hdr portion of neighbor solicit struct.
  ns.nd_ns_hdr.icmp6_type = ND_NEIGHBOR_SOLICIT;  // 135 (RFC 4861)
  ns.nd_ns_hdr.icmp6_code = MESSAGE_CODE;   // zero for neighbor solicitation (RFC 4861)
  ns.nd_ns_hdr.icmp6_cksum = htons(0);      // zero when calculating checksum

  dst_addr.sin6_family = AF_INET6;

  // Target IPv6 address and destination address
  if (dst_v6_addr == NULL)
  {
    inet_pton(AF_INET6, "::", ns.nd_ns_target.s6_addr);
    inet_pton(AF_INET6, "ff02::1", &dst_addr.sin6_addr);
  }
  else
  {
    memcpy(ns.nd_ns_target.s6_addr, dst_v6_addr, sizeof(uint8_t)*QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    memcpy(dst_addr.sin6_addr.s6_addr, dst_v6_addr, sizeof(dst_addr.sin6_addr.s6_addr));
  }

  do{
    if ((ret = sendto(QCMAPLANMgr->GetBridgeSockFd(), &ns, sizeof(nd_neighbor_solicit), 0,
               (struct sockaddr *)&dst_addr,sizeof(dst_addr))) < 0)
    {
      LOG_MSG_ERROR("Error: Cannot send NS on Bridge IFace error: %d.", errno, 0, 0);
      //sleep for .2 seconds
      usleep(200000);
      loop++;
    }
  }while(ret < 0 && loop < QCMAP_BRIDGE_MAX_RETRY);

  if (loop >= QCMAP_BRIDGE_MAX_RETRY)
    return false;
  else
    return true;
}

