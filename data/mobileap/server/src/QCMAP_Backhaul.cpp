/*====================================================

FILE:  QCMAP_Backhaul.cpp

SERVICES:
   QCMAP Connection Manager Backhaul Specific Implementation

=====================================================

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   ka         Created
  12/06/15   rk         Offtarget support.
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
#include <linux/sockios.h>
#include <linux/types.h>
#include <linux/ipv6_route.h>
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include "qcmap_cm_api.h"
#include "ds_string.h"
#include "ds_util.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "ds_qmi_qcmap_msgr.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_Virtual_LAN.h"

// Define some constants.
#define ETH_HDRLEN 14  // Ethernet header length
#define IP6_HDRLEN 40  // IPv6 header length
#define ICMP_HDRLEN 8  // ICMP header length for echo request, excludes data
#define ETH_P_IPV6 0x86DD
#define MAX_BACKAUL_COUNT 5

extern unsigned int pmip_qcmap_sockfd;
extern unsigned int pmip_qcmap_ind_sockfd;

pmip_conf_type QCMAP_Backhaul::pmip_conf;

extern struct sigevent sev;
qcmap_backhaul_type         QCMAP_Backhaul::current_backhaul = NO_BACKHAUL;
qcmap_backhaul_type         QCMAP_Backhaul::preffered_backhaul_first;
qcmap_backhaul_type         QCMAP_Backhaul::preffered_backhaul_second;
qcmap_backhaul_type         QCMAP_Backhaul::preffered_backhaul_third;
qcmap_backhaul_type         QCMAP_Backhaul::preffered_backhaul_fourth;
qcmap_backhaul_type         QCMAP_Backhaul::preffered_backhaul_fifth;
bool                        QCMAP_Backhaul::enable_ipv6 = false;
bool                        QCMAP_Backhaul::enable_ipv4 = false;
profile_handle_type_v01     QCMAP_Backhaul::defaultProfileHandle    = 0;
QCMAP_Backhaul_WLAN        *QCMAP_Backhaul::QcMapBackhaulWLAN       = NULL;
QCMAP_Backhaul_Cradle      *QCMAP_Backhaul::QcMapBackhaulCradle     = NULL;


struct icmp6_ra_pkt {
  struct nd_router_advert router_adv;
  struct nd_opt_prefix_info prefix_info;
  struct nd_opt_mtu mtu_info;
  struct rdnss rdns_info;
}__attribute__((__packed__));

/* Request message to send to the kernel to retrieve prefix info. */
typedef struct qcmap_nl_getaddr_req_s
{
  struct nlmsghdr   nlh;
  struct ifaddrmsg  ifa;
} qcmap_nl_getaddr_req_t;

typedef struct qcmap_backhaul_list_s
{
  qcmap_backhaul_type backhaul_name;
  boolean             backhaul_availale;
} qcmap_backhaul_list_type;

#define PPP_CLIENT_MAC_ADDR "ff:ff:ff:ff:ff:ff"

/* Set Default Valid and Preferred Life Time Values. */
#define IPV6_DEFAULT_PREFERED_LIFETIME 2700;
#define IPV6_DEFAULT_VALID_LIFETIME 7200;
#define IPTYPE_IPV6_ICMP        58

#define DEFAULT_VALID_LIFETIME 2
#define DEFAULT_PREFERRED_LIFETIME 1
#define CARRY16(x) (((x) & 0x10000) ? 1 : 0)
/* Use this only when the containing type can't overflow due to the carry-overs */
#define CARRYS16(x) (((x) & (~0xffff)) >> 16)
#define IPTYPE_IPV6_ICMP        58
/* Max size of the netlink response message */
#define QCMAP_NL_MAX_MSG_SIZE  (1024 * 4)


/*===========================================================================
  FUNCTION ones_comp_sum
  ===========================================================================
  @brief
    Converts array of host address from network byte order to host byte order

  @input
    ptr  - pointer to array of host addresses
    size - size of array

  @return
    ret - resultant host address in host byte order

  @dependencies

  @sideefects
    None
  =========================================================================*/
static uint16_t ones_comp_sum(uint16_t *ptr, size_t size)
{
  uint32_t ret = 0;
  size_t i;
  uint16_t *cur;
  for (cur = ptr, i = 0; i < size; i++) {
    ret += ntohs(cur[i]);
    ret = (ret + CARRY16(ret)) & 0xffff;
  }
  return ret;
}


/*===========================================================================
  FUNCTION icmp6_checksum
  ===========================================================================
  @brief
    validates icmp6_checksum value

  @input
    ip6h   -  ipv6 header
    icmp6h -  an Internet Control Message Protocol v6 Header
    icmplen - ICMP header length

  @return
    ret - host byte order of host address whose icmp6_checksum validated

  @dependencies

  @sideefects
    None
  =========================================================================*/

uint16_t icmp6_checksum(struct ip6_hdr *ip6h, struct icmp6_header *icmp6h, uint16_t icmplen)
{
  uint32_t ret = 0;
  struct ip6_hdr tmph;

  tmph = *ip6h;
  tmph.ip6_nxt = IPTYPE_IPV6_ICMP;
  tmph.ip6_plen = icmplen;

  /* Checksum computation must contain a pseudo-header, comprised of:
   * - src and dst ip addresses
   * - payload length (32 bit value)
   * - next header
   */
  ret = ones_comp_sum((uint16_t *)&ip6h->ip6_src, sizeof(struct in6_addr)/sizeof(uint16_t));
  ret += ones_comp_sum((uint16_t *)&ip6h->ip6_dst, sizeof(struct in6_addr)/sizeof(uint16_t));
  ret += ntohs(ip6h->ip6_plen);

  if (icmplen != ntohs(ip6h->ip6_plen))
    LOG_MSG_INFO1("payload length %d does not match with icmplen %d", ip6h->ip6_plen, icmplen, 0);

  ret += (uint16_t) IPTYPE_IPV6_ICMP;
  ret += ones_comp_sum((uint16_t *)icmp6h, icmplen / sizeof(uint16_t));
  ret += CARRYS16(ret);

  return htons(~ret & 0xffff);
}


/*===========================================================================
  FUNCTION DetermineBackhaulType
  ===========================================================================
  @brief
    Determines bachual type connected

  @input
    token - input parameter to check for backhual type

  @return
    return_backhaul - integer backhual type value

  @dependencies

  @sideefects
    None
  =========================================================================*/
qcmap_backhaul_type DetermineBackhaulType(char* token)
{
  qcmap_backhaul_type return_backhaul = NO_BACKHAUL;
  /*------------------------------------------------------------------------*/
  if (strncmp(token,WWAN_BACKHAUL,strlen(WWAN_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_WWAN;
  else if (strncmp(token,USB_CRADLE_BACKHAUL,strlen(USB_CRADLE_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_CRADLE;
  else if (strncmp(token,WLAN_BACKHAUL,strlen(WLAN_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_AP_STA_ROUTER;
  else if (strncmp(token,ETH_BACKHAUL,strlen(ETH_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_ETHERNET;
  else if (strncmp(token,BT_BACKHAUL,strlen(BT_BACKHAUL)) == 0)
    return_backhaul = BACKHAUL_TYPE_BT;
  else
    LOG_MSG_ERROR("Unknown preffered_backhaul_first read from xml!!",0,0,0);

  return return_backhaul;
}


/*===========================================================================
  FUNCTION InitializeStaticParams
  ===========================================================================
  @brief
    Initialize's static variable's of this class

  @input

  @return
    None

  @dependencies

  @sideefects
    None
  =========================================================================*/
void QCMAP_Backhaul::InitializeStaticParams()
{
  char data[MAX_STRING_LENGTH] = {0};
  char *token=NULL;
  char *ptr;

  memset (&(ipv6_public_dns), 0,sizeof (public_dns));
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FIRST_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    printf(" CONFIG_FIRST_PREFFERED_BACKHAUL : t%st\n",token);
    QCMAP_Backhaul::preffered_backhaul_first = DetermineBackhaulType(token);
  }
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_SECOND_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    printf(" CONFIG_SECOND_PREFFERED_BACKHAUL : t%st\n",token);
    QCMAP_Backhaul::preffered_backhaul_second = DetermineBackhaulType(token);
  }
  if (QCMAP_Backhaul::preffered_backhaul_second == QCMAP_Backhaul::preffered_backhaul_first)
    QCMAP_Backhaul::preffered_backhaul_second = NO_BACKHAUL;

  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_THIRD_PREFFERED_BACKHAUL,GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    printf(" CONFIG_THIRD_PREFFERED_BACKHAUL : t%st\n",token);
    QCMAP_Backhaul::preffered_backhaul_third = DetermineBackhaulType(token);
  }
  if (QCMAP_Backhaul::preffered_backhaul_third == QCMAP_Backhaul::preffered_backhaul_first ||
       QCMAP_Backhaul::preffered_backhaul_third == QCMAP_Backhaul::preffered_backhaul_second)
  {
    QCMAP_Backhaul::preffered_backhaul_third = NO_BACKHAUL;
  }

  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FOURTH_PREFFERED_BACKHAUL,
                                              GET_VALUE, data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    printf(" CONFIG_FOURTH_PREFFERED_BACKHAUL : t%st\n",token);

    QCMAP_Backhaul::preffered_backhaul_fourth = DetermineBackhaulType(token);
  }
  if (QCMAP_Backhaul::preffered_backhaul_fourth == QCMAP_Backhaul::preffered_backhaul_first ||
        QCMAP_Backhaul::preffered_backhaul_fourth == QCMAP_Backhaul::preffered_backhaul_second ||
        QCMAP_Backhaul::preffered_backhaul_fourth == QCMAP_Backhaul::preffered_backhaul_third
     )
  {
    QCMAP_Backhaul::preffered_backhaul_fourth = NO_BACKHAUL;
  }

  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_FIFTH_PREFFERED_BACKHAUL,
                                                GET_VALUE,
                                                data, MAX_STRING_LENGTH);
  token = strtok_r(data, "\n",&ptr);
  if(token)
  {
    printf(" CONFIG_FIFTH_PREFFERED_BACKHAUL : t%st\n",token);

    QCMAP_Backhaul::preffered_backhaul_fifth = DetermineBackhaulType(token);
  }

  if (QCMAP_Backhaul::preffered_backhaul_fifth == QCMAP_Backhaul::preffered_backhaul_first ||
        QCMAP_Backhaul::preffered_backhaul_fifth == QCMAP_Backhaul::preffered_backhaul_second ||
        QCMAP_Backhaul::preffered_backhaul_fifth == QCMAP_Backhaul::preffered_backhaul_third ||
        QCMAP_Backhaul::preffered_backhaul_fifth == QCMAP_Backhaul::preffered_backhaul_fourth
     )
  {
    QCMAP_Backhaul::preffered_backhaul_fifth = NO_BACKHAUL;
  }

  LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_first %d:",QCMAP_Backhaul::preffered_backhaul_first,0,0);
  LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_second %d:",QCMAP_Backhaul::preffered_backhaul_second,0,0);
  LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_third %d:",QCMAP_Backhaul::preffered_backhaul_third,0,0);
  LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_fourth %d:",QCMAP_Backhaul::preffered_backhaul_fourth,0,0);
  LOG_MSG_INFO1(" QCMAP_Backhaul::preffered_backhaul_fifth %d:",QCMAP_Backhaul::preffered_backhaul_fifth,0,0);

  if(QCMAP_Backhaul::ReadPMIPConfFromXML())
  {
    LOG_MSG_INFO1("QCMAP Pmip Read XML Success.",0,0,0);
  }
  else
  {
      /* Read Config from XML failed. Use defaults. */
     LOG_MSG_ERROR("QCMAP pmip Read XML failed.",0,0,0);
  }
}
/*===================================================================
                             Class Definitions
  ===================================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes Backhaul variables.

@parameters
profile_handle

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
QCMAP_Backhaul::QCMAP_Backhaul(profile_handle_type_v01 profile_handle)
{
  QcMapBackhaulWLAN = NULL;
  QcMapBackhaulCradle = NULL;
  QcMapBackhaulWWAN = NULL;
  QcMapFirewall = NULL;
  QcMapNatAlg = NULL;
  // Intialize IPv6 Prefix to Null
  memset(&this->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
  /* Intialize ipv4 address info. */
  this->ipv4_public_ip = 0;
  this->ipv4_default_gw_addr = 0;
  this->ipv4_public_subnet_mask = 0;

  memset(&this->external_client_addr_list, NULL, sizeof(qcmap_addr_info_list_t));
  memset(&this->wan_cfg, 0, sizeof(qcmap_cm_wan_conf_t));
  memset(&this->tiny_proxy_enable_state, 0, sizeof(qcmap_msgr_tiny_proxy_mode_enum_v01));

  this->profileHandle = profile_handle;

  memset(&QCMAP_Backhaul::pmip_conf, 0, sizeof(QCMAP_Backhaul::pmip_conf));

  vlan_id = QCMAP_MSGR_VLAN_ID_NONE_V01;
}


/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Cleans up Backhaul object.

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

QCMAP_Backhaul::~QCMAP_Backhaul()
{
  /* Delete WWAN Object */
  delete QcMapBackhaulWWAN;

  /* Delete NAT & Firewall Objects. */
  //TODO: delete only for first backhaul if checked in
  delete QcMapFirewall;
  delete QcMapNatAlg;

  QcMapFirewall = NULL;
  QcMapNatAlg = NULL;

  LOG_MSG_INFO2("Destroying Object: BACKHAUL",0,0,0);
}


/*==========================================================
  FUNCTION Init
==========================================================*/
/*!
@brief
  Initialize Params for Backhaul Object

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
void QCMAP_Backhaul::Init()
{
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  //Initialize Static Params
  if (QCMAP_ConnectionManager::BackhaulObjectCountFromMap() == 0)
  {
    if (QcMapMgr->xml_path)
    {
      if (this->ReadBackhaulConfigFromXML())
      {
        strlcpy(this->wan_cfg.eri_config_file, QCMAP_ERI_CONFIG, (QCMAP_CM_MAX_FILE_LEN));
      }
      else
      {
        /* Read Config from XML failed. Use defaults. */
        LOG_MSG_ERROR("QCMAP Read XML failed.",0,0,0);
      }
    }

    /*initialize current backhaul*/
    QCMAP_Backhaul::current_backhaul = NO_BACKHAUL;
    InitializeStaticParams();
  }

  return;
}
/*===========================================================================
  FUNCTION CreateBackhualAndRelatedObjects
===========================================================================*/
/*!
@brief
  Create Backhaul and store its reference

@input
  None

@return
  None

@dependencies

@sideefects
  None
*/
/*=========================================================================*/
void QCMAP_Backhaul::CreateBackhualAndRelatedObjects
(
  profile_handle_type_v01 profile_handle,
  void                   *pdnNode
)
{
  QCMAP_Backhaul          *pBackhaul;
  QCMAP_ConnectionManager *QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  //Create Backhaul Object
  pBackhaul = new QCMAP_Backhaul(profile_handle);
  ds_assert(pBackhaul != NULL);  //Assert if memory allocation fails!
  pBackhaul->Init();

  pBackhaul->QcMapBackhaulWWAN = new QCMAP_Backhaul_WWAN(pBackhaul);
  ds_assert(pBackhaul->QcMapBackhaulWWAN != NULL)  //Assert if memory allocation fails!
  pBackhaul->QcMapBackhaulWWAN->Init(pdnNode);

  /* Initialize BackhaulWWAN static params only once on boot-up */
  if (QCMAP_ConnectionManager::BackhaulObjectCountFromMap() == 0)
  {
    pBackhaul->QcMapBackhaulWWAN->ReadStaticParamsFromXML();
  }
  QcMapMgr->InsertBackhaulObjectToMap(profile_handle, pBackhaul);

  //Create object for Firewall and NATALG here
  //TODO: init only for first backhaul if checked in
  pBackhaul->QcMapFirewall = new QCMAP_Firewall(pBackhaul);
  ds_assert(pBackhaul->QcMapFirewall != NULL)  //Assert if memory allocation fails!
  pBackhaul->QcMapFirewall->Init();

  pBackhaul->QcMapNatAlg = new QCMAP_NATALG(pBackhaul);
  ds_assert(pBackhaul->QcMapNatAlg != NULL)  //Assert if memory allocation fails!
  pBackhaul->QcMapNatAlg->Init();
}


/*===========================================================================
  FUNCTION GetDefaultProfileHandle()
===========================================================================*/
/*
  DESCRIPTION
    Get's default profile handle or internet PDN.

  DEPENDENCIES
    None.

  RETURN VALUE
    profileHandle

  SIDE EFFECTS
*/
/*=========================================================================*/
profile_handle_type_v01 QCMAP_Backhaul::GetDefaultProfileHandle()
{
  return QCMAP_Backhaul::defaultProfileHandle;
}


/*===========================================================================
  FUNCTION IsCradleOrStationOrEthActive
==========================================================================*/
/*!
@brief
  Checks whether Cradle or WLAN Backhaul is active, if not returns true

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
boolean QCMAP_Backhaul::IsNonWWANBackhaulActive()
{
  boolean cradle_wlan_eth_status = false;
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  if(QcMapBackhaulMgr)
  {
    cradle_wlan_eth_status = (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE ||
                          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER ||
                          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE ||
                          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET ||
                          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT);
  }
  return cradle_wlan_eth_status;
}


/*===========================================================================
  FUNCTION RemoveIPV6Address
==========================================================================*/
/*!
@brief
  Removes IPV6 address from the interface provided

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
boolean QCMAP_Backhaul::RemoveIPV6Address(char *devname)
{
  char command[MAX_COMMAND_STR_LEN];
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();

  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
    return false;
  }

  if (QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid)
  {
    LOG_MSG_ERROR("invalid Prefix Passed", 0, 0, 0);
    return false;
  }

  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);
  inet_ntop(AF_INET6,
            ((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr,
            ip6_addr, MAX_IPV6_PREFIX );
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip -6 addr del %s/%d dev %s",
           ip6_addr,QcMapBackhaulMgr->ipv6_prefix_info.prefix_len,
           devname);
  ds_system_call( command, strlen(command));

  memset(&(QcMapBackhaulMgr->ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));

  return true;
}


/*===========================================================================
  FUNCTION EnableIPV4
==========================================================================*/
/*!
@brief
  Enables IPV4 backhaul Functionality. If autoconnect is enabled,
  triggers the backhaul to get the IPv4 address.

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
boolean QCMAP_Backhaul::EnableIPV4(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0;
  boolean retval = true;
  char command[MAX_COMMAND_STR_LEN];
  char enable[MAX_STRING_LENGTH]="1";//To update xml in case the backhaul object is not enabled
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();
  /* Cant continue if MobileAP is not enabled! */

  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (enable_ipv4)
  {
    /* QCMAP IPV4 is already enabled */
    LOG_MSG_ERROR("QCMAP IPV4 already enabled",0,0,0);
    return true;
  }

  /* Enable IPV4 so that backhaul will succeed. */
  enable_ipv4 = true;

  /* Save the Configuration. */
  GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,SET_VALUE,enable,MAX_STRING_LENGTH);
  /* Check for auto-connect and bring-up backhaul. */
  ret = QcMapBackhaulWWAN->EnableIPv4(QcMapMgr->qcmap_cm_handle, qmi_err_num);
  if ( ret < 0 )
  {
    LOG_MSG_ERROR("QCMAP IPV4 enable failed: %d", qmi_err_num, 0, 0);
    /* Restore the state. */
    enable_ipv4 = false;
    retval = false;
  }

  if ( retval == true)
  {
    if (IS_DEFAULT_PROFILE(profileHandle))
    {
      if ((NULL != QcMapBackhaulCradleMgr) &&
          (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
           QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
      {
        if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
        {
          /* Kill any stale dhcpcd intance */
          LOG_MSG_INFO1("Killing previous dhcpcd process for cradle iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          /* Obtain and assign IP address via dhcpcd daemon */
          LOG_MSG_INFO1("Running DHCP client for cradle iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Kill any stale dhcpcd intance */
          LOG_MSG_INFO1("Killing previous dhcpcd process for cradle iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ECM_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ECM_IFACE);
          ds_system_call(command, strlen(command));

          /* Obtain and assign IP address via dhcpcd daemon */
          LOG_MSG_INFO1("Running DHCP client for cradle iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",ECM_IFACE);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulCradleMgr->cradle_v4_available = false;
      }
      if (NULL != QcMapBackhaulEthMgr &&
          QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                     QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
      {
        /* Kill any stale dhcpcd intance */
        LOG_MSG_INFO1("Killing previous dhcpcd process for ETH iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ETH_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ETH_IFACE);
        ds_system_call(command, strlen(command));

        /* Obtain and assign IP address via dhcpcd daemon */
        LOG_MSG_INFO1("Running DHCP client for ETH iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",ETH_IFACE);
        ds_system_call(command, strlen(command));
        QcMapBackhaulEthMgr->eth_v4_available = false;
      }
      if (NULL != QcMapBTTethMgr &&
          QcMapBTTethMgr->bt_tethering_mode == QCMAP_MSGR_BT_MODE_WAN_V01)
      {
        /* Kill any stale dhcpcd intance */
        LOG_MSG_INFO1("Killing previous dhcpcd process for BT iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BT_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BT_IFACE);
        ds_system_call(command, strlen(command));

        /* Obtain and assign IP address via dhcpcd daemon */
        LOG_MSG_INFO1("Running DHCP client for BT iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers"
                    " --noipv4ll -b -G",BT_IFACE);
        ds_system_call(command, strlen(command));
        QcMapBTTethMgr->bt_v4_available = false;
      }
      if (NULL != QcMapBackhaulWLANMgr)
      {
        if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
        {
          /* Kill any stale dhcpcd intance */
          LOG_MSG_INFO1("Killing previous dhcpcd process for STA Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
                    BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",
                    BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          /* Obtain and assign IP address via dhcpcd daemon */
          LOG_MSG_INFO1("Running DHCP client on STA Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",
                    BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Kill any stale dhcpcd intance */
          LOG_MSG_INFO1("Killing previous dhcpcd process for STA Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
                    QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",
                    QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          /* Obtain and assign IP address via dhcpcd daemon */
          LOG_MSG_INFO1("Running DHCP client on STA Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",
                    QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulWLANMgr->sta_v4_available = false;
      }
    }

    if(!QcMapNatAlg)
      LOG_MSG_ERROR("NAT object not present",0,0,0);

    LOG_MSG_INFO1("QCMAP IPV4 Enabled",0,0,0);
  }
  return retval;
}
/*===========================================================================
  FUNCTION DisableIPV4
==========================================================================*/
/*!
@brief
  Disables IPv4 Functionality.

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

boolean QCMAP_Backhaul::DisableIPV4(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0, qcmap_cm_errno = QCMAP_CM_ENOERROR;
  char command[MAX_COMMAND_STR_LEN];
  char enable[MAX_STRING_LENGTH]="0";//To update xml in case the backhaul object is not enabled
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (!enable_ipv4)
  {
    /* QCMAP IPV4 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV4 not enabled",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return true;
  }

  /* The logic here is to disconnect the existing V4 backhaul and set the
   * enable_ipv4 flag to false. This is required even when we are in STA mode.
   * Reason being if we don't bring down the call now and when we switch from
   * WLAN backhaul goes down we will still have IPV4 WWAN backhaul which should
   * not be the case.
   */

  /* Disconnect Backhaul. */
  ret = QcMapBackhaulWWAN->DisableIPv4(QcMapMgr->qcmap_cm_handle, &qcmap_cm_errno, qmi_err_num);

  if (ret == QCMAP_CM_SUCCESS ||
      (ret == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK))
  {
    /* we just disable the IPV4 forwarding. So that none of the clients
     * will be able to access IPV4 backhaul. This is needed for both
     * station mode and regular mode as when WWAN backhaul cb is
     * hit the enable_ipv4 will not be set and so below functionality
     * will not be executed.
     */
    if(QcMapNatAlg)
      QcMapNatAlg->DisableNATonApps();
    else
      LOG_MSG_ERROR("NAT object not present",0,0,0);

    if(QcMapFirewall)
      QcMapFirewall->CleanIPv4MangleTable();
    else
      LOG_MSG_ERROR("FIREWALL object not present",0,0,0);

    if (IS_DEFAULT_PROFILE(profileHandle))
    {
      if ((NULL != QcMapBackhaulCradleMgr) &&
          (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
           QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
      {
        if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
        {
          /* Kill dhcpcd intance for Cradle Iface*/
          LOG_MSG_INFO1("Killing DHCP Client for Cradle Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Kill dhcpcd intance for Cradle Iface*/
          LOG_MSG_INFO1("Killing DHCP Client for Cradle Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ECM_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ECM_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", ECM_IFACE);
          ds_system_call(command, strlen(command));
        }
        ClearIfaceRelatedRulesForV4(ECM_IFACE);
        ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
        QcMapBackhaulCradleMgr->cradle_v4_available = false;
      }
      if (NULL != QcMapBackhaulEthMgr &&
          QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                   QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
      {
        LOG_MSG_INFO1("Killing DHCP Client for Ethernet Iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", ETH_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", ETH_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", ETH_IFACE);
        ds_system_call(command, strlen(command));

        ClearIfaceRelatedRulesForV4(ETH_IFACE);
        ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
        QcMapBackhaulEthMgr->eth_v4_available = false;
      }
      if (NULL != QcMapBTTethMgr &&
          QcMapBTTethMgr->bt_tethering_mode == QCMAP_MSGR_BT_MODE_WAN_V01)
      {
        LOG_MSG_INFO1("Killing DHCP Client for BT Iface",0,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BT_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BT_IFACE);
        ds_system_call(command, strlen(command));

        snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", BT_IFACE);
        ds_system_call(command, strlen(command));

        ClearIfaceRelatedRulesForV4(BT_IFACE);
        ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
        QcMapBTTethMgr->bt_v4_available = false;
      }
      if (NULL != QcMapBackhaulWLANMgr)
      {
        if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
        {
          LOG_MSG_INFO1("Killing DHCP Client for Bridge Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          /* Delete the default route*/
          snprintf(command, MAX_COMMAND_STR_LEN, "route del default gw %s dev %s",
                   inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw),
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Kill any stale dhcpcd intance */
          LOG_MSG_INFO1("Killing DHCP Client for STA Iface",0,0,0);
          snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s 0 up",
                    QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
        }
        ClearIfaceRelatedRulesForV4(BRIDGE_IFACE);
        ClearIfaceRelatedRulesForV4(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
        QcMapBackhaulWLANMgr->sta_v4_available = false;
      }
    }
    enable_ipv4 = false;
  }
  else
  {
    LOG_MSG_ERROR( "QCMAP IPV4 Disable Fail %d: %d",
                  qcmap_cm_errno, qmi_err_num, 0);
    return false;
  }

  if ((IS_DEFAULT_PROFILE(profileHandle)) &&
      QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false &&
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
  {
    switch (QCMAP_Backhaul::current_backhaul)
    {
      case BACKHAUL_TYPE_AP_STA_ROUTER:
        if (!QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QCMAP_Backhaul::current_backhaul, false, true);
        }
        break;

      case BACKHAUL_TYPE_CRADLE:
        if (!QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QCMAP_Backhaul::current_backhaul, false, true);
        }
        break;
      case BACKHAUL_TYPE_WWAN:
        if (QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED)
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QCMAP_Backhaul::current_backhaul, false, true);
        }
        break;
      case BACKHAUL_TYPE_ETHERNET:
        if (!QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QCMAP_Backhaul::current_backhaul, false, true);
        }
        break;

      case BACKHAUL_TYPE_BT:
        if (!QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6())
        {
          QCMAP_Backhaul::SwitchToOtherBackhaul(
                       QCMAP_Backhaul::current_backhaul, false, true);
        }
        break;

      default:
        LOG_MSG_ERROR("Invalid Backhaul", 0, 0, 0);
        break;
    }
  }
  LOG_MSG_INFO1("QCMAP IPV4 Disabled",0,0,0);
  /* Save the Configuration. */
  GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,SET_VALUE,enable,strlen(enable));
  return true;
}

/*===========================================================================
  FUNCTION GetIPV6PrefixInfo
==========================================================================*/
/*!
@brief
  Gets IP address and netmask assigned to the STA interface.

@parameters
  char *devname
  qcmap_cm_nl_prefix_info_t   *ipv6_prefix_info


@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul:: GetIPV6PrefixInfo(char *devname, qcmap_cm_nl_prefix_info_t   *ipv6_prefix_info)
{
  struct sockaddr_nl  src_addr;
  struct sockaddr_nl dest_addr;
  struct iovec iov;
  struct sockaddr_nl sa;
  struct msghdr msg = { (void *)&sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
  struct nlmsghdr *nlh = NULL;
  struct sockaddr_in6 sin6;
  void *buf = NULL;
  qcmap_nl_getaddr_req_t req;
  int sock_fd;
  int iface_idx;
  unsigned int buflen = 0;
  ssize_t recvsize;
  struct ifreq ifr;
  int socket_mtu = 0;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  memset (&sin6, 0, sizeof (sin6));
  sin6.sin6_family = AF_INET6;

  if ((sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: socket() failed: %d", errno, 0, 0);
    return -1;
  }

  /* Initialize the source address */
  memset(&src_addr, 0, sizeof(src_addr));

  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid = 0;
  src_addr.nl_groups = 0; /* Interested in unicast messages */

  /* Bind the socket to our source address */
  if (bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: bind() failed: %d", errno, 0, 0);
    goto bail;
  }

  /* Initialize destination address structure */
  memset(&dest_addr, 0, sizeof(dest_addr));

  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid    = 0;  /* Addressed to kernel */
  dest_addr.nl_groups = 0;  /* This is a unicast message */

  /* Initialize the request message */
  memset(&req, 0, sizeof(req));

  /* Fill the netlink request message */
  req.nlh.nlmsg_len   = sizeof(req);
  req.nlh.nlmsg_pid   = 0;
  req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
  req.nlh.nlmsg_type  = RTM_GETADDR;

  /* Set the ip family and interface index for which the addresses are requested */
  req.ifa.ifa_family = AF_INET6;
  iface_idx = QcMapMgr->IsInterfaceEnabled(devname);
  if (iface_idx <= 0)
  {
    LOG_MSG_ERROR("Couldn't find interface %s Error:%d", devname, errno, 0);
    goto bail;
  }

  LOG_MSG_INFO1("Get Prefix Info for interface %s with ID: %d", devname, iface_idx, 0);

  if ( sendto(sock_fd, (void*) &req, sizeof(req), 0,
               (struct sockaddr*) &dest_addr, sizeof(dest_addr)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: bind() failed: %d", errno, 0, 0);
    goto bail;
  }

  /* Allocate and initialize buffer to read message */
  buf = calloc(1, NLMSG_SPACE(QCMAP_NL_MAX_MSG_SIZE));
  if (NULL == buf)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: memory alloc failure: %d", errno, 0, 0);
    goto bail;
  }

  iov.iov_base = buf;
  iov.iov_len  = NLMSG_SPACE(QCMAP_NL_MAX_MSG_SIZE);

  /* Read message from kernel */
  if ((recvsize = recvmsg(sock_fd, &msg, 0)) < 0)
  {
    LOG_MSG_ERROR("GetIPV6PrefixInfo: memory alloc failure: %d", errno, 0, 0);
    goto bail;
  }
  else
  {
    buflen = recvsize;
    LOG_MSG_INFO3("received response from kernel size=%d", buflen, 0, 0);
  }

  nlh = (struct nlmsghdr *)buf;

  /* Parse the message one header at a time */
  while (NLMSG_OK(nlh, buflen))
  {
    struct ifaddrmsg *ifa;
    struct rtattr *rta;
    int rtattrlen;

    ifa = (struct ifaddrmsg *)NLMSG_DATA(nlh);
    rta = (struct rtattr *)IFA_RTA(ifa);

    /* Make sure that the requested and received address family is the same */
    if ( AF_INET6 != ifa->ifa_family || RT_SCOPE_UNIVERSE != ifa->ifa_scope ||
         ifa->ifa_index != iface_idx)
    {
      LOG_MSG_ERROR("GetIPV6PrefixInfo: ip family %d, Scope %d or Index %d don't match",
                    ifa->ifa_family, ifa->ifa_scope, ifa->ifa_index);
      /* Advance to next header */
      nlh = NLMSG_NEXT(nlh, buflen);
      continue;
    }

    rtattrlen = IFA_PAYLOAD(nlh);

    /* Parse the RTM_GETADDR attributes */
    while (RTA_OK(rta, rtattrlen))
    {
      switch (rta->rta_type)
      {
        case IFA_ADDRESS:
           ipv6_prefix_info->prefix_len = ifa->ifa_prefixlen;
           memcpy(&sin6.sin6_addr,
                  RTA_DATA(rta),
                  sizeof(sin6.sin6_addr));
           memcpy(&ipv6_prefix_info->prefix_addr,&sin6,sizeof(sin6));

           ipv6_prefix_info->prefix_info_valid = true;
           break;
        case IFA_CACHEINFO:
          memcpy( &ipv6_prefix_info->cache_info,
                   RTA_DATA(rta),
                   sizeof(ipv6_prefix_info->cache_info) );
          LOG_MSG_INFO2( "GetIPV6PrefixInfo: Address Cache Info - prefered=%d valid=%d",
                           ipv6_prefix_info->cache_info.ifa_prefered,
                           ipv6_prefix_info->cache_info.ifa_valid, 0);
          break;
        default:
          LOG_MSG_INFO3("GetIPV6PrefixInfo: rta_type=%x", rta->rta_type, 0, 0);
          break;
      }
      rta = RTA_NEXT(rta, rtattrlen);
    }

    /* Break out from here as by this point we would have found the address. */
    break;
  }

  memset(&ifr, 0, sizeof(ifr));
  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", devname);

  /* get mtu from device for ipv6 */
  if((socket_mtu = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Couldn't create socket to get mtu of ipv6 wwan err %d", errno,0,0);
    goto bail;
  }

  ifr.ifr_addr.sa_family = AF_INET6;

  if (ioctl(socket_mtu, SIOCGIFMTU, &ifr) < 0)
  {
    LOG_MSG_ERROR("Couldn't get mtu from ipv6 wwan iface. err %d", errno,0,0);
    goto bail;
  }

  ipv6_prefix_info->mtu = ifr.ifr_mtu;
  LOG_MSG_INFO1( "GetIPV6PrefixInfo: MTU = %d", ipv6_prefix_info->mtu,0,0);

bail:
  close(sock_fd);
  if (socket_mtu)
    close(socket_mtu);

  if ( buf != NULL )
    free(buf);

  return;
}


/*===========================================================================
  FUNCTION UpdateGlobalV6addr
==========================================================================*/
/*!
@brief
  Handles the RA packet

@parameters
  uint8_t ipv6_addr[]

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::UpdateGlobalV6addr
(
  qcmap_nl_addr_t* nl_addr,
  boolean   validate_addr
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  boolean prefix_delegation = false;

  struct ps_in6_addr *dst_addr_ptr = NULL, *iid_ptr = NULL;
  struct ps_in6_addr *global_addr_ptr = NULL, *prefix_addr_ptr = NULL;
  qmi_error_type_v01 qmi_err_num;
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t *clientInfo = NULL;
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error = QCMAP_CM_SUCCESS;
  char ipv6addr[INET6_ADDRSTRLEN];
  int ret;
  struct ps_in6_addr *prefix_ptr;
  int16_t vlan_id = 0;


  LOG_MSG_INFO1("Entering UpdateGlobalV6addr", 0, 0, 0);

  if(QCMAP_Backhaul::enable_ipv6)
  {
    ret = GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);
    LOG_MSG_INFO1("GetDeviceName returned %d", ret, 0, 0);
    if((ret == NO_BACKHAUL &&
        !QCMAP_Backhaul_WLAN::IsSTAAvailableV6() &&
        !QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6() &&
        !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() &&
        !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6()) ||
        (QcMapBackhaulWWAN && ret == BACKHAUL_TYPE_WWAN &&
        QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED))
    {
      /* QCMAP IPV6 backhaul not up */
      LOG_MSG_ERROR("QCMAP IPv6 default backhaul not UP", 0, 0, 0);
      return;
    }

    if (ret == BACKHAUL_TYPE_WWAN && !(this->vlan_id) &&
        !(IS_DEFAULT_PROFILE(this->profileHandle)))
    {
      /* Secondary PDN is up with no VLAN mapping. So don't update*/
      LOG_MSG_ERROR("Secondary PDN is UP, don't update global V6 address", 0, 0, 0);
      return;
    }
    vlan_id = this->vlan_id;

    dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ipv6_dst_addr;
    prefix_addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;

    if (QCMAP_IN6_IS_PREFIX_LINKLOCAL(dst_addr_ptr->ps_s6_addr32))
    {
      LOG_MSG_INFO1("Destination based RA address received", 0, 0, 0);
      memset(ipv6addr, 0, INET6_ADDRSTRLEN);
      inet_ntop(AF_INET6,
                  (void *)dst_addr_ptr->ps_s6_addr,
                  ipv6addr, INET6_ADDRSTRLEN);
      LOG_MSG_INFO1("UpdateGlobalV6addr- Destination V6 Address %s",ipv6addr,0,0);
      if ( addrList->addrListHead == NULL)
      {
        LOG_MSG_ERROR("UpdateGlobalV6addr() - Linked list head is NULL",0, 0, 0);
        return ;
      }

      tempPrefixIidptr.iid_ptr = dst_addr_ptr;
      node = ds_dll_search (addrList->addrListHead , (void*)&tempPrefixIidptr,
                            QcMapMgr->qcmap_match_v6_iid);

      if ( node == NULL)
      {
        LOG_MSG_ERROR("UpdateGlobalV6addr() - No IID Match found. Check why??",0,0,0);
        return;
      }
      else
      {
        clientInfo = ( qcmap_cm_client_data_info_t* )ds_dll_data(node);
        if (clientInfo == NULL)
        {
          LOG_MSG_ERROR("UpdateGlobalV6addr - Error in fetching node data ",0, 0, 0);
          return;
        }
        else
        {
          /* IID match is found Update Address the global v6 address*/
          iid_ptr = (struct ps_in6_addr *)clientInfo->link_local_v6_addr;
          global_addr_ptr = (struct ps_in6_addr *)clientInfo->ipv6[0].addr;

          /* Update only if the prefix changes. */
          if ( !memcmp(&global_addr_ptr->ps_s6_addr64[0],
                &prefix_addr_ptr->ps_s6_addr64[0],sizeof(uint64)))
          {
            LOG_MSG_INFO1("UpdateGlobalV6addr - Prefix unchanged no need to update",0, 0, 0);
          }
          else
          {
            global_addr_ptr->ps_s6_addr64[0] = prefix_addr_ptr->ps_s6_addr64[0];
            global_addr_ptr->ps_s6_addr64[1] = iid_ptr->ps_s6_addr64[1];
            memset(ipv6addr, 0, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6,(void *)clientInfo->ipv6[0].addr,ipv6addr, INET6_ADDRSTRLEN);
            LOG_MSG_INFO1("Global V6 Address %s updated\n",ipv6addr,0,0);

            if (QcMapTetheringMgr && ((clientInfo->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01) &&
                  (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
                   QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                   QCMAP_QTI_TETHERED_LINK_PPP)))
            {
              memcpy( QcMapTetheringMgr->ppp_ipv6_addr, clientInfo->ipv6[0].addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
              AddPPPIPv6Route();
            }
          }
        }
      }
    }
    else
    {
      if(!ipv6_prefix_info.prefix_info_valid)
      {
        if((NULL != QcMapBackhaulWLANMgr) ||
            (NULL != QcMapBackhaulCradleMgr) ||
            (NULL != QcMapBackhaulWWAN))
        {
          if (ret==BACKHAUL_TYPE_AP_STA_BRIDGE )
          {
            strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
          }
          else
          {
            if(!ret)
            {
              LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", qcmap_cm_error,0,0);
              return;
            }
          }
        }

        /* we only want to update the global prefix info if we are not in bridge mode,
           otherwise it will be updated in addrAssignment. Default rules will not be
           installed unless the prefix is updated in new address assignment */
        if (!QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() &&
            !QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
        {
          memset(&ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
          ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
          ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
          GetIPV6PrefixInfo(devname,&ipv6_prefix_info);
        }
      }

      QCMAP_Backhaul_WWAN::GetPrefixDelegationStatus(&prefix_delegation, &qmi_err_num);
      if(prefix_delegation)
      {
        LOG_MSG_ERROR("Prefix delegation Enabled.Don't update client prefixes",
                      0, 0, 0);
        return;
      }

      /* Check if the RA received on current backhaul interface. If not ignore the RA. */
      if ( validate_addr &&
           !ValidateGlobalV6addr(prefix_addr_ptr, &vlan_id))
      {
        LOG_MSG_ERROR("Prefix not on the current backhaul. Ignore RA.\n",
                          0, 0, 0);
        return;
      }

      LOG_MSG_INFO1(" Multicast RA received / Prefix update on EnableIpv6 forwarding",0, 0, 0);
      if (!AssignGlobalV6AddrAllClients(prefix_addr_ptr, vlan_id))
      {
        LOG_MSG_ERROR("Could not update Global Ipv6 address for clients with vlan_id : %d",vlan_id, 0, 0);
        return;
      }
    }
  }
  else
  {
    LOG_MSG_ERROR("UpdateGlobalV6addr - Ipv6 is not enabled", 0, 0, 0);
  }
}
/*===========================================================================
  FUNCTION GetNetworkConfig
==========================================================================*/
/*!
@brief
  Gets the network configured value from WWAN or external hotspot.

@parameters
  in_addr_t  *public_ip,
  uint32     *primary_dns,
  in_addr_t  *secondary_dns,
  int        *err_num

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetNetworkConfig
(
  in_addr_t *public_ip,
  uint32 *primary_dns,
  in_addr_t *secondary_dns,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qmi_error;
  uint32 default_gw = 0;
  in_addr_t netmask = 0;
  FILE *file_ptr=NULL;
  char temp_str[INET6_ADDRSTRLEN]="";
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char char_match[] = "nameserver "; //search for this pattern in file
  boolean pri_dns_found = false; // seperates out primary DNS and secondary DNS
  int ret, qcmap_cm_error;
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN *QcMapLANMgr = GET_DEFAULT_LAN_OBJECT();
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;

  struct sockaddr_in6 sa6;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(profileHandle);

  if (!QCMAP_Backhaul::enable_ipv4)
  {
    /* QCMAP IPV4 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV4 not enabled",0,0,0);
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    return false;
  }

  ret = GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num);
  if(ret != BACKHAUL_TYPE_WWAN)
  {
    if ( !GetIP(public_ip, &netmask, devname))
    {
      LOG_MSG_ERROR("Unable to get the IP address",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    file_ptr = fopen(DNSMASQ_RESOLV_FILE,"r");
    if(file_ptr == NULL)
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      LOG_MSG_ERROR("Error in getting IPv4 net config %d", *qmi_err_num,0,0);
      return false;
    }
    while((fgets(temp_str,strlen(char_match)+1,file_ptr)!=NULL))
    {
      if(strncmp(temp_str,char_match,strlen(char_match))==0)
      {
        fscanf(file_ptr, "%s", temp_str);
        if (inet_pton(AF_INET6, temp_str, &(sa6.sin6_addr)) == 1)
        {
          LOG_MSG_INFO1("Got ipv6 address %s ignore it",temp_str,0,0);
          continue;
        }
        if(!pri_dns_found)
        {
          inet_aton(temp_str, (in_addr *)primary_dns);
          pri_dns_found = TRUE;
        }
        else
        {
          inet_aton(temp_str, (in_addr *)secondary_dns);
          break; // found sec dns also - no more looping
        }
      }
    }
    fclose(file_ptr);
  }
  else
  {
    if ( QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED )
    {
      /* The WWAN is not connected, so it doesn't have an IP or DNS serers*/
      *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
      return false;
    }

    qmi_error = QcMapBackhaulWWAN->GetIPv4NetworkConfig(public_ip, primary_dns,
                                                        secondary_dns, &default_gw,
                                                        qmi_err_num);

    /* If IP Passthrough is active, update the retIP with the saved public IP*/
    if(IS_DEFAULT_PROFILE(profileHandle) && QcMapLANMgr)
    {
      if((lan_cfg = QcMapLANMgr->GetLANConfig()) == NULL)
      {
        LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }

      if(lan_cfg->ip_passthrough_cfg.ip_passthrough_active)
      {
        *public_ip = lan_cfg->ip_passthrough_cfg.public_ip;
      }
    }


    if ( qmi_error == QCMAP_CM_SUCCESS )
    {
      return true;
    }
  }

  if( (*public_ip == 0) && (*primary_dns == 0) && (*secondary_dns == 0))
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("Error in getting IPv4 net config %d", *qmi_err_num,0,0);
    return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION GetIPv6NetworkConfig
==========================================================================*/
/*!
@brief
  Gets the IPv6 network configured value.

@parameters
  uint8_t public_ip[]
  uint8_t primary_dns[]
  uint8_t secondary_dns[]
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetIPv6NetworkConfig
(
  uint8_t             public_ip[],
  uint8_t             primary_dns[],
  uint8_t             secondary_dns[],
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret;
  int qmi_error, qcmap_cm_error;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if (!QCMAP_Backhaul::enable_ipv6)
  {
    /* QCMAP IPV6 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV6 not enabled",0,0,0);
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    return false;
  }

  ret = GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01, qmi_err_num);
  if( ret != BACKHAUL_TYPE_WWAN)
  {
    memset(&(ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));
    GetIPV6PrefixInfo(devname, &ipv6_prefix_info);

    if (ipv6_prefix_info.prefix_info_valid == true)
    {
      memcpy((in6_addr *)public_ip,
          ((struct sockaddr_in6 *)&(ipv6_prefix_info.prefix_addr))\
          ->sin6_addr.s6_addr,sizeof(in6_addr));
    }
    else
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      LOG_MSG_ERROR("Error in getting v6 station "
                     "mode config %d", *qmi_err_num,0,0);
      return false;
    }
  }
  else
  {
    if ( QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED )
    {
      /* The WWAN is not connected, so no IPv6 prefix or DNS servers */
      *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
      return false;
    }

    qmi_error = QcMapBackhaulWWAN->GetIPv6NetworkConfig(public_ip,
                                                        primary_dns,
                                                        secondary_dns,
                                                        qmi_err_num);

    if ( qmi_error == QCMAP_CM_SUCCESS )
    {
      return true;
    }
  }

  if(PS_IN6_IS_ADDR_UNSPECIFIED(public_ip) &&
     PS_IN6_IS_ADDR_UNSPECIFIED(primary_dns) &&
     PS_IN6_IS_ADDR_UNSPECIFIED(secondary_dns))
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("Error in getting IPv6 WWAN config %d", *qmi_err_num,0,0);
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION SetWebserverWWANAccess
==========================================================================*/
/*!
@brief
  Configures Webserver WWAN Access flag and updates in the mobileap
  configuration file.

@parameters
  boolean enable

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SetWebserverWWANAccess
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  char data[MAX_STRING_LENGTH] = {0};
  boolean ret = true;

  snprintf(data, sizeof(data), "%d", enable);
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_WEBSERVER_WWAN_ACCESS,SET_VALUE,
                                              data,strlen(data));
  if(QcMapBackhaulMgr)
  {
    if ( !QcMapBackhaulMgr->SetWebserverWWANAccessOnApps(enable, qmi_err_num) )
    {
      ret = false;
    }
    QcMapBackhaulMgr->wan_cfg.enable_webserver_wwan_access = enable;
  }
  return ret;
}

/*===========================================================================
  FUNCTION SetWebserverWWANAccessOnApps
==========================================================================*/
/*!
@brief
  Configures the Webserver WWAN Access.

@parameters
  boolean enable

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SetWebserverWWANAccessOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  if (!GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }
#ifndef FEATURE_QTIMAP_OFFTARGET
  /* Delete the existing rule, only if it was added. */
  if( enable == TRUE && !this->wan_cfg.enable_webserver_wwan_access)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
                "iptables -t filter -D INPUT -i %s -p tcp --dport 80 -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,
                "iptables -t filter -D INPUT -i %s -p tcp --dport 443 -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
  }

  /* Add entry only in case of disable rule, by default it's accept*/
  if( !enable )
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t filter -I INPUT -i %s -p tcp --dport 80 -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,
               "iptables -t filter -I INPUT -i %s -p tcp --dport 443 -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
  }
#endif
  return true;
}

/*===========================================================================
  FUNCTION GetWebserverWWANAccessFlag
==========================================================================*/
/*!
@brief
  Gets the status of whether Webserver can be accessed from WWAN.

@parameters
  uint8 *flag

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetWebserverWWANAccessFlag
(
  uint8 *flag,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  char enable[MAX_STRING_LENGTH] = {0};
  if ( flag == NULL )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  if(QcMapBackhaulMgr)
    *flag = QcMapBackhaulMgr->wan_cfg.enable_webserver_wwan_access;
  else
  {
    QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_WEBSERVER_WWAN_ACCESS,GET_VALUE,enable,MAX_STRING_LENGTH);
    *flag=atoi(enable);
  }
  return true;
}

/*===========================================================================
  FUNCTION EnableIPV6
==========================================================================*/
/*!
@brief
  Enables IPV6 Functionality. If autoconnect is enabled,
  triggers the backhaul to get the IPv6 address.

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
boolean QCMAP_Backhaul::EnableIPV6(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0;
  boolean retval = true;
  char command[MAX_COMMAND_STR_LEN];
  char enable[MAX_STRING_LENGTH]="1";//To update xml in case the backhaul object is not enabled
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                   GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }


  if (enable_ipv6)
  {
    /* QCMAP IPV6 is already enabled */
    LOG_MSG_ERROR("QCMAP IPV6 already enabled",0,0,0);
    return true;
  }

  /* Enable IPV6 so that backhaul will succeed. */
  enable_ipv6 = true;

  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);

  /* Save the Configuration. */
  GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,SET_VALUE,enable,MAX_STRING_LENGTH);
  /* Check for auto-connect and bring-up backhaul. */
  ret = QcMapBackhaulWWAN->EnableIPv6(QcMapMgr->qcmap_cm_handle, qmi_err_num);
  if ( ret < 0 )
  {
    LOG_MSG_ERROR("QCMAP IPV6 Enable Failed: %d", qmi_err_num, 0, 0);
    /* Restore the state. */
    enable_ipv6 = false;
    retval = false;
  }
  if(retval == true && IS_DEFAULT_PROFILE(profileHandle))
  {
    /*Enable IPV6 for bridge iface everytime since iface is not
      brought down completely during tranisitions and hence old config
      might remain effective*/
    LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for Bridge Iface",0,0,0);
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
        BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    if((NULL != QcMapBackhaulWLANMgr) ||
        (NULL != QcMapBackhaulCradleMgr) ||
        (NULL != QcMapBackhaulEthMgr))
    {
      if (NULL != QcMapBackhaulWLANMgr)
      {
        if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
        {
          /* Enable ipv6 for bridge0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for Bridge Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          /*Set accept_ra flag to 2*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          snprintf(command, MAX_COMMAND_STR_LEN,
                   "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra_prefix_route", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          JoinMulticastGroup(QCMAP_V6_SOCK_BRIDGE, BRIDGE_IFACE);

          ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding",
                         strlen("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding"));

          SendRSOnBridgeIface();
        }
        else
        {
          /* Enable ipv6 for STA Iface */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for STA Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          JoinMulticastGroup(QCMAP_V6_SOCK_WLAN, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);

          /*Set accept_ra flag to 2*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
          /* Dont set the dft route when information is recevied in an RA*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulWLANMgr->sta_v6_available = false;
      }

      if ((NULL != QcMapBackhaulCradleMgr) &&
          (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
           QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
      {
        if (QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
        {
          /* Enable ipv6 for bridge0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for ECM/Cradle Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          JoinMulticastGroup(QCMAP_V6_SOCK_BRIDGE, BRIDGE_IFACE);

          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          /* Enable ipv6 for ecm0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for ECM/Cradle Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 ECM_IFACE);
          ds_system_call(command, strlen(command));

          JoinMulticastGroup(QCMAP_V6_SOCK_ECM, ECM_IFACE);

          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                  ECM_IFACE);
          ds_system_call(command, strlen(command));
          /* Dont set the dft route when information is recevied in an RA*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                  ECM_IFACE);
          ds_system_call(command, strlen(command));
        }
        QcMapBackhaulCradleMgr->cradle_v6_available = false;
      }
      if ((NULL != QcMapBackhaulEthMgr) &&
          (QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01))
      {
        LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for ETH Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 ETH_IFACE);
        ds_system_call(command, strlen(command));

        JoinMulticastGroup(QCMAP_V6_SOCK_ETH, ETH_IFACE);

        snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 ETH_IFACE);
        ds_system_call(command, strlen(command));
        /* Dont set the dft route when information is recevied in an RA*/
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                 ETH_IFACE);
        ds_system_call(command, strlen(command));
        QcMapBackhaulEthMgr->eth_v6_available = false;
      }

      if ((NULL != QcMapBTTethMgr) &&
          (QcMapBTTethMgr->bt_tethering_mode == QCMAP_MSGR_BT_MODE_WAN_V01))
      {
        LOG_MSG_INFO1("Setting disable_ipv6 flag to FALSE for BT Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 BT_IFACE);
        ds_system_call(command, strlen(command));

        JoinMulticastGroup(QCMAP_V6_SOCK_ETH, BT_IFACE);

        snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 BT_IFACE);
        ds_system_call(command, strlen(command));
        /* Dont set the dft route when information is recevied in an RA*/
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                  BT_IFACE);
        ds_system_call(command, strlen(command));
        QcMapBTTethMgr->bt_v6_available = false;
      }

    }
    LOG_MSG_INFO1("QCMAP IPV6 Enabled", 0, 0, 0);
  }

  return retval;
}

/*===========================================================================
  FUNCTION DisableIPV6
==========================================================================*/
/*!
@brief
  Disables IPv6 Functionality.

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
boolean QCMAP_Backhaul::DisableIPV6(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0, qcmap_cm_errno = QCMAP_CM_ENOERROR;
  char command[MAX_COMMAND_STR_LEN];
  char enable[MAX_STRING_LENGTH]="0";//To update xml in case the backhaul object is not enabled
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (!enable_ipv6)
  {
    /* QCMAP IPV6 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV6 not enabled",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return true;
  }

  /* The logic here is to disconnect the existing V6 backhaul and set the
   * enable_ipv6 flag to false. This is required even when we are in STA mode.
   * Reason being if we don't bring down the call now and when we switch from
   * WLAN backhaul goes down we will still have IPV6 WWAN backhaul which should
   * not be the case.
   */
  /* Disconnect Backhaul. */
  ret = QCMAP_Backhaul_WWAN::DisableIPv6(QcMapMgr->qcmap_cm_handle, &qcmap_cm_errno, qmi_err_num);

  if (ret == QCMAP_CM_SUCCESS ||
      (ret == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK))
  {
    if (IS_DEFAULT_PROFILE(profileHandle))
    {
      if (NULL != QcMapBackhaulWLANMgr)
      {
        if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
        {
          /* Disable ipv6 for bridge0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for Bridge Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          /*Set accept_ra flag to 1*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          if (RemoveIPV6Address(BRIDGE_IFACE))
            LOG_MSG_ERROR("Error Removing IPV6 address from BRIDGE Iface",0,0,0);

          ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
        }
        else
        {
          /* Disable ipv6 for STA Iface */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for STA Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          /*Set accept_ra flag to 1*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));

          if (RemoveIPV6Address(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface)) //<<< this isn't needed as the v6 address is wiped during disable_ipv6 by kernel
            LOG_MSG_ERROR("Error Removing IPV6 address from STA Iface",0,0,0);

          if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
          {
            DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
          }

          LOG_MSG_INFO1("Installing iptable rule to block IPv6 traffic on STA Iface",0,0,0);
          ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
        }
        QcMapBackhaulWLANMgr->sta_v6_available = false;
      }

      if ((NULL != QcMapBackhaulCradleMgr) &&
          (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
           QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01))
      {
        if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
        {
          /* Disable ipv6 for bridge0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for Bridge Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          /*Set accept_ra flag to 1*/
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));

          if (RemoveIPV6Address(BRIDGE_IFACE))
            LOG_MSG_ERROR("Error Removing IPV6 address from BRIDGE Iface",0,0,0);
        }
        else
        {
          /* Disable ipv6 for ecm0 */
          LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for ECM/Cradle Iface",0,0,0);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   ECM_IFACE);
          ds_system_call(command, strlen(command));

          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   ECM_IFACE);
          ds_system_call(command, strlen(command));

          if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE)
            DeleteAllV6ExternalRoute(ECM_IFACE);

          if (RemoveIPV6Address(ECM_IFACE))
            LOG_MSG_ERROR("Error Removing IPV6 address from Cradle Iface",0,0,0);
        }
        ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
        ClearIfaceRelatedRulesForV6(ECM_IFACE);
        QcMapBackhaulCradleMgr->cradle_v6_available = false;
      }
      if ((NULL != QcMapBackhaulEthMgr) &&
          (QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01))
      {
        /* Disable ipv6 for ecm0 */
        LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for ETH Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 ETH_IFACE);
        ds_system_call(command, strlen(command));

        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 ETH_IFACE);
        ds_system_call(command, strlen(command));

        if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
          DeleteAllV6ExternalRoute(ETH_IFACE);

        if (RemoveIPV6Address(ETH_IFACE))
          LOG_MSG_ERROR("Error Removing IPV6 address from ETH Iface",0,0,0);

        ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
        ClearIfaceRelatedRulesForV6(ETH_IFACE);
        QcMapBackhaulEthMgr->eth_v6_available = false;
      }

      if ((NULL != QcMapBTTethMgr) &&
          (QcMapBTTethMgr->bt_tethering_mode == QCMAP_MSGR_BT_MODE_WAN_V01))
      {
        /* Disable ipv6 for ecm0 */
        LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for BT Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 BT_IFACE);
        ds_system_call(command, strlen(command));

        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 BT_IFACE);
        ds_system_call(command, strlen(command));

        if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
          DeleteAllV6ExternalRoute(BT_IFACE);

        if (RemoveIPV6Address(BT_IFACE))
          LOG_MSG_ERROR("Error Removing IPV6 address from BT Iface",0,0,0);

        ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
        ClearIfaceRelatedRulesForV6(BT_IFACE);
        QcMapBTTethMgr->bt_v6_available = false;
      }
    }

    DisableIPV6Forwarding(true);
    QcMapFirewall->CleanIPv6MangleTable();
    enable_ipv6 = false;
  }
  else
  {
    LOG_MSG_ERROR( "QCMAP IPV6 Disable Fail %d: %d", qcmap_cm_errno, qmi_err_num, 0);
    return false;
  }

  if (IS_DEFAULT_PROFILE(profileHandle))
  {
    if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false &&
        QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
    {
      switch (QCMAP_Backhaul::current_backhaul)
      {
        case BACKHAUL_TYPE_AP_STA_ROUTER:
          if (!QCMAP_Backhaul_WLAN::IsSTAAvailableV4())
          {
            QCMAP_Backhaul::SwitchToOtherBackhaul(
                QCMAP_Backhaul::current_backhaul, true,
                false);
          }
          break;

        case BACKHAUL_TYPE_CRADLE:
          if (!QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4())
          {
            QCMAP_Backhaul::SwitchToOtherBackhaul(
                QCMAP_Backhaul::current_backhaul, true,
                false);
          }
          break;
        case BACKHAUL_TYPE_WWAN:
          if (QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED)
          {
            QCMAP_Backhaul::SwitchToOtherBackhaul(
                QCMAP_Backhaul::current_backhaul, true,
                false);
          }
          break;
        case BACKHAUL_TYPE_ETHERNET:
          if (!QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
          {
            QCMAP_Backhaul::SwitchToOtherBackhaul(
                QCMAP_Backhaul::current_backhaul, true,
                false);
          }
          break;

        case BACKHAUL_TYPE_BT:
          if (!QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4())
          {
            QCMAP_Backhaul::SwitchToOtherBackhaul(
                QCMAP_Backhaul::current_backhaul, true,
                false);
          }
          break;
        default:
          LOG_MSG_ERROR("Invalid Backhaul", 0, 0, 0);
          break;
      }
    }
  }

  LOG_MSG_INFO1("QCMAP IPV6 Disabled", 0, 0, 0);
  /* Save the Configuration. */
  GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,SET_VALUE,enable,strlen(enable));
  return true;
}
/*===========================================================================
  FUNCTION EnableIPV6Forwarding
==========================================================================*/
/*!
@brief
  Enables Ipv6 forwarding and starts radish.

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::EnableIPV6Forwarding()
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  qmi_error_type_v01 qmi_err_num;
  qcmap_nl_addr_t nl_addr;
  int ret;
  struct in6_addr all_nodes_addr = { 0xff,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1 };
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  char ipv6_addr[INET6_ADDRSTRLEN];

  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);

  LOG_MSG_INFO1("Entering IPV6 Forwarding",0,0,0);
  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    return false;
  }

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  ret = GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);
  bzero(wan_cfg.ipv6_interface, QCMAP_MSGR_INTF_LEN);
  strlcpy(wan_cfg.ipv6_interface, devname, QCMAP_MSGR_INTF_LEN);

  if (QCMAP_Backhaul::enable_ipv6)
  {
    if( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      if (this->QcMapNatAlg)
        this->QcMapNatAlg->CleanIPv6FilterTableFwdChain();
    }

    /* Remove the prefix based v6 rule since we might get a new prefix */
    if (IS_DEFAULT_PROFILE(profileHandle))
    {
      //Guest AP 1
      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01  ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ) &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        RemoveIPv6PrefixBasedRules(QcMapMgr->ap_dev_num2);
        if (QcMapBackhaulWWAN && (QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_1))
        {
          QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_1 = false;
        }
      }

      //Guest AP 2
      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        RemoveIPv6PrefixBasedRules(QcMapMgr->ap_dev_num3);
        if (QcMapBackhaulWWAN && (QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_2))
        {
          QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_2 = false;
        }
      }

      //Guest AP 3
      if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        RemoveIPv6PrefixBasedRules(QcMapMgr->ap_dev_num4);
        if (QcMapBackhaulWWAN && (QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_3))
        {
          QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_3 = false;
        }
      }
    }

    /* 1. If default profile, check if none of non-WWAN backhaul's is active
     * 2. If non-default profile, then above check is not required.
     */
    /* Update NetDev in Firewall before default route is added. */
    if ( ((IS_DEFAULT_PROFILE(profileHandle)) &&
         !QCMAP_Backhaul::IsNonWWANBackhaulActive() && this->QcMapFirewall) ||
         (!(IS_DEFAULT_PROFILE(profileHandle)) && this->QcMapFirewall) )
    {
      this->QcMapFirewall->UpdateNetDevInXML((const char*) devname);
    }

    if(QcMapBackhaulWWAN)
    {
      LOG_MSG_INFO1("Setting prefix delegation mode %d",\
                    QcMapBackhaulWWAN->wwan_cfg.prefix_delegation,0,0);
      QcMapBackhaulWWAN->prefix_delegation_activated =\
                               QcMapBackhaulWWAN->wwan_cfg.prefix_delegation;
    }

    LOG_MSG_INFO1("Setting forwarding for ipv6",0,0,0);
    if ( (QcMapBackhaulWWAN && QcMapBackhaulWWAN->prefix_delegation_activated) ||
           QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra", devname);
      ds_system_call(command, strlen(command));
      ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/proxy_ndp",
                      strlen("echo 2 > /proc/sys/net/ipv6/conf/all/proxy_ndp"));
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_prefix_route", devname);
      ds_system_call(command, strlen(command));
    }

    ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding",
                     strlen("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding"));
#ifndef FEATURE_QTIMAP_OFFTARGET
    /* Deleting the default route. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del default dev %s metric 256", devname);
    ds_system_call(command, strlen(command));

    /* Add the default route with lower metric so that the packets
     *  will follow this route.
     */
    LOG_MSG_INFO1("Adding route for inet6 ::/0",0,0,0);
    /* If backhaul was STA or Cradle, delete the default route with gateway*/
    if ((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER) &&
        QcMapBackhaulWLANMgr)
    {
      inet_ntop(AF_INET6, &QcMapBackhaulWLANMgr->ipv6_gateway_addr, ipv6_addr,
                               INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route add default via %s dev wlan%d",ipv6_addr, QcMapMgr->sta_iface_index);
      ds_system_call(command, strlen(command));
    }
    else if((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE) &&
              QcMapBackhaulCradleMgr)
    {
      QcMapBackhaulCradleMgr->GetCradleMode(&cradle_mode,&qmi_err_num);
      if (cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
      {
        inet_ntop(AF_INET6, &QcMapBackhaulCradleMgr->ipv6_gateway_addr, ipv6_addr,
                               INET6_ADDRSTRLEN);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route add default via %s dev %s",ipv6_addr, ECM_IFACE);
        ds_system_call(command, strlen(command));
      }
    }
    else if ((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
        && QcMapBackhaulEthMgr)
    {
      /* Need to add support for WAN bridge mode*/
      inet_ntop(AF_INET6, &QcMapBackhaulEthMgr->ipv6_gateway_addr, ipv6_addr,
                              INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip -6 route add default via %s dev %s",ipv6_addr, ETH_IFACE);
      ds_system_call(command, strlen(command));
    }
    else if ((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
             && QcMapBTTethMgr)
    {
      inet_ntop(AF_INET6, &QcMapBTTethMgr->ipv6_gateway_addr, ipv6_addr,
                              INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip -6 route add default via %s dev %s",ipv6_addr, BT_IFACE);
      ds_system_call(command, strlen(command));
    }
    else
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip -6 route add default dev %s metric 256", devname);
      if (vlan_id == QCMAP_MSGR_VLAN_ID_NONE_V01 && (!IS_DEFAULT_PROFILE(profileHandle)))
      {
        char scratch_buf[MAX_COMMAND_STR_LEN];
        snprintf(scratch_buf, MAX_COMMAND_STR_LEN, " table pdn_table_%d", profileHandle);
        strlcat(command, scratch_buf, MAX_COMMAND_STR_LEN);
      }
      ds_system_call(command, strlen(command));
    }
#endif
    /* Get the global ipv6 address. */
    memset(&(ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));
    ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
    ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
    GetIPV6PrefixInfo(devname, &ipv6_prefix_info);

    if ( ipv6_prefix_info.prefix_info_valid == true &&
        ((!QcMapBackhaulWWAN || !QcMapBackhaulWWAN->prefix_delegation_activated)\
         || IsNonWWANBackhaulActive()))
    {
      UpdatePrefix(&ipv6_prefix_info, false, true, NULL);

      memset(&nl_addr, 0 ,sizeof(nl_addr));
      memcpy(nl_addr.ipv6_dst_addr, all_nodes_addr.s6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      memcpy(nl_addr.ip_v6_addr,
             ((struct sockaddr_in6 *)&(ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      nl_addr.isValidIPv6address = true;
      UpdateGlobalV6addr(&nl_addr, false);
    }

    if(!QCMAP_Backhaul::IsPmipV6ModeEnabled())
    {
      /* Restart Radish. */
      RestartRadish();
    }

    if (IS_DEFAULT_PROFILE(profileHandle))
    {
      if (QcMapBackhaulWWAN && QcMapBackhaulWWAN->prefix_delegation_activated &&
          !QCMAP_Backhaul::IsNonWWANBackhaulActive())
      {
        SendRSForClients();
      }

      /* Add the prefix based v6 rule since we might get a new prefix */
      //Guest AP 1
      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        if (QcMapBackhaulWWAN && ipv6_prefix_info.prefix_info_valid )
        {
          AddIPv6PrefixBasedRules(QcMapMgr->ap_dev_num2);
          if(QcMapBackhaulWWAN &&
              (!QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_1) )
          {
            QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_1 = true;
          }
        }
        /* This was add to block data reaching A5, during embedded call scenario*/
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num2);
        ds_system_call( command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num2);
        ds_system_call( command, strlen(command));
      }

      //GuestAP2
      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) )
      {
        if (QcMapBackhaulWWAN && ipv6_prefix_info.prefix_info_valid )
        {
          AddIPv6PrefixBasedRules(QcMapMgr->ap_dev_num3);
          if(QcMapBackhaulWWAN &&
              (!QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_2) )
          {
            QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_2 = true;
          }
        }
        /* This was add to block data reaching A5, during embedded call scenario*/
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num3);
        ds_system_call( command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num3);
        ds_system_call( command, strlen(command));
      }

      //GuestAP3
      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) )
      {
        if (QcMapBackhaulWWAN && ipv6_prefix_info.prefix_info_valid )
        {
          AddIPv6PrefixBasedRules(QcMapMgr->ap_dev_num4);
          if(QcMapBackhaulWWAN &&
              (!QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_3) )
          {
            QcMapBackhaulWWAN->ipv6_prefix_based_rules_added_for_guest_ap_3 = true;
          }

        }
        /* This was add to block data reaching A5, during embedded call scenario*/
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num4);
        ds_system_call( command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num4);
        ds_system_call( command, strlen(command));
      }
    }
  }

  if(!QCMAP_Backhaul::IsNonWWANBackhaulActive() && IS_DEFAULT_PROFILE(profileHandle))
  {
    if (QCMAP_Backhaul::IsPmipV6ModeEnabled() || QCMAP_Backhaul::IsPmipV4ModeEnabled())
    {
      LOG_MSG_INFO1("DDNS & DHCPv6 is not supported when pmip is active", 0, 0, 0);
      return true;
    }
  }

  /* DDNS is supported only for default PDN */
  if( IS_DEFAULT_PROFILE(profileHandle) &&
      QcMapBackhaulWWAN && QCMAP_Backhaul_WWAN::ddns_conf.enable == DDNS_ENABLED &&
      QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)
  {
    QcMapBackhaulWWAN->StartDDNS(&qmi_err_num);
  }

  if( QcMapBackhaulWWAN &&
      QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_xml_state == QCMAP_MSGR_DHCPV6_MODE_UP_V01 &&
      QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)
  {
    QcMapBackhaulWWAN->StartDhcpv6Dns(&qmi_err_num);
  }

  /*Send Backhaul Status Indication for only default WWAN backhaul object*/
  LOG_MSG_INFO1("Send IPV6 backhaul connect indication", 0, 0, 0);
  QCMAP_Backhaul::SendBackhaulStatusInd(BACKHAUL_NOT_APPLICABLE,BACKHAUL_AVAILABLE,QCMAP_Backhaul::current_backhaul);

  return true;
}

/*===========================================================================
  FUNCTION DisableIPV6Forwarding
==========================================================================*/
/*!
@brief
  This function will disable Ipv6 forwarding and stops radish service.

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::DisableIPV6Forwarding(boolean send_ra)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  QCMAP_LAN *lan_obj = NULL;
  int ret;
  int qcmap_cm_errno;
  char ip6_addr[INET6_ADDRSTRLEN];
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  qcmap_msgr_ethernet_mode_v01 eth_mode= QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01;
  LOG_MSG_INFO1("Disable IPV6 Forwarding",0,0,0);

  if (IS_DEFAULT_PROFILE(profileHandle))
  {
    ret = GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);
    lan_obj = GET_DEFAULT_LAN_OBJECT();
  }
  else
  {
    if (IS_VLAN_ID_VALID(vlan_id))
      lan_obj = GET_LAN_OBJECT(vlan_id);

    ret = BACKHAUL_TYPE_WWAN; //set ret for non-default profile
  }

  /* Cant continue if MobileAP is not enabled! */
  if ( !QcMapMgr->qcmap_enable )
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    return false;
  }

  if (QCMAP_Backhaul::enable_ipv6)
  {
    /* Stop Radish. */
    StopRadish();

    /* Install the rule to block WWAN access. */
    if (lan_obj)
    {
      lan_obj->BlockIPv6WWANAccess();
    }

    /* Deprecate old prefix. */
    if ( ipv6_prefix_info.prefix_info_valid == true )
    {
      if (QcMapBackhaulWWAN && QcMapBackhaulWWAN->prefix_delegation_activated)
        DeprecateClientRA();
      else
        UpdatePrefix(&ipv6_prefix_info, true, send_ra, NULL);
    }
    else
    {
      /* In case of STA Mode, try to get the address again and deprecate the prefix. */
      if ( ret!=BACKHAUL_TYPE_WWAN)
      {
        memset(&(ipv6_prefix_info), 0, sizeof(qcmap_cm_nl_prefix_info_t));
        ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
        ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
        GetIPV6PrefixInfo(devname, &ipv6_prefix_info);
        if ( ipv6_prefix_info.prefix_info_valid == true )
        {
          UpdatePrefix(&ipv6_prefix_info, true, send_ra, NULL);
        }
      }
    }

    /* Delete PPP IPV6 route if present. */
    DeletePPPIPv6Route();

    LOG_MSG_INFO1("Disabling forwarding for ipv6",0,0,0);

    if(QCMAP_Backhaul::IsPmipV6ModeEnabled() &&
      (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01))
    {
      LOG_MSG_INFO1("Forwading will not be disabled since PMIP is active for "
                    " ipv6 & tunnel mode is v4\n",0,0,0);
    }
    else
    {
      if (QCMAP_Backhaul_WWAN::IsAnyPDNConnected(QCMAP_MSGR_IP_FAMILY_V6_V01) == FALSE)
      {
        ds_system_call("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding",
                        strlen("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding"));
      }
    }

    if (QCMAP_Backhaul_WWAN::IsAnyPDNConnected(QCMAP_MSGR_IP_FAMILY_V6_V01) == FALSE)
    {
      ds_system_call("echo 0 > /proc/sys/net/ipv6/conf/all/proxy_ndp",
                       strlen("echo 0 > /proc/sys/net/ipv6/conf/all/proxy_ndp"));
    }
#ifndef FEATURE_QTIMAP_OFFTARGET
    LOG_MSG_INFO1("Delete default route for inet6 ::/0",0,0,0);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del default metric 256");
    if (vlan_id == QCMAP_MSGR_VLAN_ID_NONE_V01)
    {
      char scratch_buf[MAX_COMMAND_STR_LEN];
      snprintf(scratch_buf, MAX_COMMAND_STR_LEN, " table pdn_table_%d", profileHandle);
      strlcat(command, scratch_buf, MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    /* If backhaul was STA or Cradle, delete the default route with gateway*/
    if ((ret == BACKHAUL_TYPE_AP_STA_ROUTER) && QcMapBackhaulWLANMgr)
    {
      inet_ntop(AF_INET6, &QcMapBackhaulWLANMgr->ipv6_gateway_addr, ip6_addr,
                               INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip -6 route del default via %s dev wlan%d",ip6_addr, QcMapMgr->sta_iface_index);
      ds_system_call(command, strlen(command));
    }
    else if((ret == BACKHAUL_TYPE_CRADLE) && QcMapBackhaulCradleMgr)
    {
      QcMapBackhaulCradleMgr->GetCradleMode(&cradle_mode,&qmi_err_num);
      if (cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
      {
        inet_ntop(AF_INET6, &QcMapBackhaulCradleMgr->ipv6_gateway_addr, ip6_addr,
                               INET6_ADDRSTRLEN);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del default via %s dev %s",ip6_addr, ECM_IFACE);
        ds_system_call(command, strlen(command));
      }
    }
    else if((ret == BACKHAUL_TYPE_ETHERNET) && QcMapBackhaulEthMgr)
    {
      QcMapBackhaulEthMgr->GetEthBackhaulMode(&eth_mode,&qmi_err_num);
      if (eth_mode == QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
      {
        inet_ntop(AF_INET6, &QcMapBackhaulEthMgr->ipv6_gateway_addr, ip6_addr,
                               INET6_ADDRSTRLEN);
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip -6 route del default via %s dev %s",ip6_addr, ETH_IFACE);
        ds_system_call(command, strlen(command));
      }
    }
    else if((ret == BACKHAUL_TYPE_BT) && QcMapBTTethMgr)
    {
      inet_ntop(AF_INET6, &QcMapBTTethMgr->ipv6_gateway_addr, ip6_addr,
                INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip -6 route del default via %s dev %s",ip6_addr, BT_IFACE);
      ds_system_call(command, strlen(command));
    }
#endif
    if (ret!=BACKHAUL_TYPE_WWAN)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra", devname);
      ds_system_call(command, strlen(command));
    }
    if(lan_obj)
    {
      //Enable the bridge forwarding for Ipv6
      snprintf(command, MAX_COMMAND_STR_LEN,
             "echo 1 > /proc/sys/net/ipv6/conf/%s/forwarding", lan_obj->GetBridgeNameAsCStr());
      ds_system_call(command, strlen(command));
    }

    /*Reset the Global Ipv6 address from the connected devices information*/
    if (QcMapMgr->GetNumofConnectedDevices() > 0)
    {
      if (!(DeleteGlobalipv6AddressInfo()))
      {
        LOG_MSG_ERROR("Could not delete Global IPv6 address from Connected"
                      "devices information", 0, 0, 0);
      }
    }
  }

  if (IS_DEFAULT_PROFILE(profileHandle))
  {
    /* Add the prefix based v6 rule since we might get a new prefix */
    //Guest AP 1
    if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01
          && ret!=BACKHAUL_TYPE_WWAN))
    {
      if (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile\
          == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv6 -j DROP \n",
                 QcMapMgr->ap_dev_num2);
        ds_system_call( command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv6 -j DROP \n",
                 QcMapMgr->ap_dev_num2);
        ds_system_call( command, strlen(command));
      }

      //GuestAP2
      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ) &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num3);
        ds_system_call( command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num3);
        ds_system_call( command, strlen(command));
      }

      //GuestAP3
      if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile\
           == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num4);
        ds_system_call( command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv6 -j DROP \n",
                  QcMapMgr->ap_dev_num4);
        ds_system_call( command, strlen(command));
      }
    }
    if(ret==BACKHAUL_TYPE_WWAN)
    {
      if(QCMAP_Backhaul_WWAN::ddns_conf.enable  == DDNS_ENABLED &&
         QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED &&
         QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED )
      {
        QcMapBackhaulWWAN->StopDDNS();
      }

    }
  }

  if ((IS_DEFAULT_PROFILE(profileHandle) && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN) )
  {
    if(QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_enable_state  ==
         QCMAP_MSGR_DHCPV6_MODE_UP_V01 )
    {
      QcMapBackhaulWWAN->StopDhcpv6Dns(&qmi_err_num);
    }
  }

  /*Send Backhaul Status Indication for only default WWAN backhaul object*/
  LOG_MSG_INFO1("Send IPV6 backhaul disconnect indication", 0, 0, 0);
  QCMAP_Backhaul::SendBackhaulStatusInd(BACKHAUL_NOT_APPLICABLE,BACKHAUL_NOT_AVAILABLE,QCMAP_Backhaul::current_backhaul);

  return true;
}

/*===========================================================================
  FUNCTION UpdatePrefix
==========================================================================*/
/*!
@brief
  Updates the prefix.

@param
  qcmap_cm_nl_prefix_info_t *ipv6_prefix_info,
  boolean deprecate, boolean send_ra,
  uint8_t *dest_v6_ip

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

int QCMAP_Backhaul::UpdatePrefix
(
  qcmap_cm_nl_prefix_info_t *ipv6_prefix_info,
  boolean deprecate, boolean send_ra,
  uint8_t *dest_v6_ip
)
{
  struct icmp6_ra_pkt ra_pkt;
  struct nd_router_advert *router_adv = NULL;
  struct nd_opt_prefix_info *prefix_info = NULL;
  struct nd_opt_mtu *mtu_info = NULL;
  struct rdnss *rdns_info = NULL;
  struct sockaddr_in6 dst_addr;
  struct ps_in6_addr *prefix_ptr, *addr_ptr, *addr_ptr1;
  int ret = -1;
  char ipaddr[MAX_IPV6_PREFIX + 1];
  char command[MAX_COMMAND_STR_LEN];
  int ppp_sockfd = 0;
  boolean is_multicast = false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* lan_obj = GET_LAN_OBJECT(vlan_id);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  char v6add_str[INET6_ADDRSTRLEN] = {0};
  uint8_t  public_ip[IPV6_ADDR_SIZE_IN_BYTES] = {0};       /* IPv6 addr assigned to WWAN */
  qmi_error_type_v01 qmi_err_num;
  size_t ra_length;
  boolean is_primary_public_rdnss_info_added = false;
  boolean is_secondary_public_rdnss_info_added = false;
  const char *bridgeIface;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(profileHandle);

  if ( ipv6_prefix_info == NULL )
  {
    LOG_MSG_ERROR("NULL Prefix Passed", 0, 0, 0);
    return ret;
  }

  if (lan_obj == NULL)
  {
    bridgeIface = BRIDGE_IFACE;
    LOG_MSG_ERROR("OOPS!!! LAN Object is NULL",0,0,0);
    return ret;
  }
  else
  {
    bridgeIface = GET_ASSOCIATED_BRIDGE_IFACE_NAME(vlan_id);
  }

  LOG_MSG_INFO1("deprecate=%d, send_ra=%d, dest_v6_ip=%p", deprecate, send_ra, dest_v6_ip);
  if ( lan_obj->GetBridgeSockFd() < 0 )
  {
    LOG_MSG_ERROR("Error: Bridge Sock is not created", 0, 0, 0);
    return ret;
  }

  memset(&dst_addr, 0, sizeof(dst_addr));
  memset(&ra_pkt,0,sizeof(ra_pkt));

  router_adv = &(ra_pkt.router_adv);

  /* Initialize RA Packet. */
  router_adv->nd_ra_type = ND_ROUTER_ADVERT;
  router_adv->nd_ra_code = 0;
  router_adv->nd_ra_cksum = 0;
  router_adv->nd_ra_curhoplimit = 64;
  router_adv->nd_ra_flags_reserved = deprecate ? 0 : ND_RA_FLAG_OTHER;
  router_adv->nd_ra_router_lifetime = deprecate ? 0 : htons(0xffff);
  router_adv->nd_ra_reachable = 0;
  router_adv->nd_ra_retransmit = 0;

  prefix_info = &(ra_pkt.prefix_info);

  prefix_info->nd_opt_pi_type = ND_OPT_PREFIX_INFORMATION;
  prefix_info->nd_opt_pi_len = sizeof(struct nd_opt_prefix_info)/sizeof(uint64);
  prefix_info->nd_opt_pi_prefix_len = ipv6_prefix_info->prefix_len;
  prefix_info->nd_opt_pi_flags_reserved = ND_OPT_PI_FLAG_ONLINK | ND_OPT_PI_FLAG_AUTO;
  prefix_info->nd_opt_pi_valid_time = deprecate ? htonl(DEFAULT_VALID_LIFETIME) :
                                      htonl(ipv6_prefix_info->cache_info.ifa_valid);

  prefix_info->nd_opt_pi_preferred_time = deprecate ? htonl(DEFAULT_PREFERRED_LIFETIME):
                                          htonl(ipv6_prefix_info->cache_info.ifa_prefered);

  prefix_info->nd_opt_pi_reserved2 = 0;

  mtu_info = &(ra_pkt.mtu_info);

  mtu_info->nd_opt_mtu_type = ND_OPT_MTU;
  mtu_info->nd_opt_mtu_len = sizeof(struct nd_opt_mtu)/sizeof(uint64);
  mtu_info->nd_opt_mtu_reserved = 0;
  mtu_info->nd_opt_mtu_mtu = htonl(ipv6_prefix_info->mtu);

  rdns_info = &(ra_pkt.rdns_info);
  if (!QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if(this->QcMapBackhaulWWAN &&
        this->QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_xml_state == QCMAP_MSGR_DHCPV6_MODE_UP_V01 )
    {
      if (this->QcMapBackhaulWWAN->GetIfaceLLAddrStr(v6add_str, bridgeIface))
      {
        QCMAP_Backhaul::AddRdnssInfo(rdns_info);
        //bridge LL v6 ip as PROXY RDNSS
        inet_pton(AF_INET6, v6add_str, &rdns_info->addr);
        is_primary_public_rdnss_info_added = true;
        LOG_MSG_INFO1("Added PROXY RDNSS in RA", 0, 0, 0);
      }
      else
      {
        LOG_MSG_ERROR("Error:Unable to add PROXY RDNSS in RA", 0, 0, 0);
        is_primary_public_rdnss_info_added = false;
      }
    }
    else
    {
      LOG_MSG_ERROR("IPv6 DNS proxy not enabled. Adding public RDNSS in RA", 0, 0, 0);
      memset (&(this->ipv6_public_dns), 0,sizeof (public_dns));
      if (this->QcMapBackhaulWWAN &&
          this->QcMapBackhaulWWAN->GetIPv6NetworkConfig(public_ip, this->ipv6_public_dns.primary_ip_v6_addr,
                                                     this->ipv6_public_dns.secondary_ip_v6_addr, &qmi_err_num)
          != QCMAP_CM_SUCCESS )
      {
        LOG_MSG_ERROR("Error in GetIPv6NetworkConfig %d", qmi_err_num,0,0);
        if (deprecate)
        {
          inet_ntop(AF_INET6, this->ipv6_public_dns.primary_ip_v6_addr,
                    v6add_str, sizeof v6add_str);
          if (strncmp (v6add_str, "::", sizeof("::")))
          {
            QCMAP_Backhaul::AddRdnssInfo(rdns_info);
            memcpy(rdns_info->addr.s6_addr, this->ipv6_public_dns.primary_ip_v6_addr,
                   QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
            LOG_MSG_INFO1("Adding primary public RDNSS for depricate RA success rdnss ip %s",
                          v6add_str, 0, 0);
            is_primary_public_rdnss_info_added = true;
          }
          else
          {
            LOG_MSG_ERROR("Adding public primary RDNSS in RA Failed zero v6 dns", 0, 0, 0);
            is_primary_public_rdnss_info_added = false;
          }
          inet_ntop(AF_INET6, this->ipv6_public_dns.secondary_ip_v6_addr,
                    v6add_str, sizeof v6add_str);

          if (strncmp (v6add_str, "::", sizeof("::")))
          {
            QCMAP_Backhaul::AddRdnssInfo(rdns_info);
            memcpy(rdns_info->addr1.s6_addr, this->ipv6_public_dns.secondary_ip_v6_addr,
                   QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
            LOG_MSG_INFO1("Adding secondary public RDNSS for depricate RA success rdnss ip %s",
                          v6add_str, 0, 0);
            is_secondary_public_rdnss_info_added = true;
          }
          else
          {
            LOG_MSG_ERROR("Adding public secondary RDNSS in RA Failed zero v6 dns", 0, 0, 0);
            is_secondary_public_rdnss_info_added = false;
          }
        }
        else
        {
          LOG_MSG_ERROR("Adding Any public RDNSS in RA Failed", 0, 0, 0);
          is_primary_public_rdnss_info_added = false;
        }
      }
      //successfully got ipv6 network info
      else
      {
        inet_ntop(AF_INET6, this->ipv6_public_dns.primary_ip_v6_addr,
                  v6add_str, sizeof v6add_str);
        if (strncmp (v6add_str, "::", sizeof("::")))
        {
          QCMAP_Backhaul::AddRdnssInfo(rdns_info);

          //public primary dns server v6 ip as RDNSS
          memcpy(rdns_info->addr.s6_addr,this->ipv6_public_dns.primary_ip_v6_addr,
                 QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
          LOG_MSG_INFO1("Adding primary public RDNSS in RA success rdnss ip %s",
                        v6add_str, 0, 0);
          is_primary_public_rdnss_info_added = true;
        }
        else
        {
          LOG_MSG_ERROR("Adding public primary RDNSS in RA Failed zero v6 dns", 0, 0, 0);
          is_primary_public_rdnss_info_added = false;
        }
        inet_ntop(AF_INET6, this->ipv6_public_dns.secondary_ip_v6_addr,
                  v6add_str, sizeof v6add_str);
        if (strncmp (v6add_str, "::", sizeof("::")))
        {
          QCMAP_Backhaul::AddRdnssInfo(rdns_info);

          //public secondary dns server v6 ip as RDNSS
          memcpy(rdns_info->addr1.s6_addr, this->ipv6_public_dns.secondary_ip_v6_addr,
                 QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
          LOG_MSG_INFO1("Adding secondary public RDNSS in RA success rdnss ip %s",
                 v6add_str, 0, 0);
          is_secondary_public_rdnss_info_added = true;
        }
        else
        {
          LOG_MSG_ERROR("Adding public secondary RDNSS in RA Failed zero v6 dns", 0, 0, 0);
          is_secondary_public_rdnss_info_added = false;
        }
      }
    }
  }
  else
  {
    LOG_MSG_ERROR("Not adding rdnss in RA cradle/station is active", 0, 0, 0);
  }

  memcpy(prefix_info->nd_opt_pi_prefix.s6_addr,
         ((struct sockaddr_in6 *)&(ipv6_prefix_info->prefix_addr))->sin6_addr.s6_addr,
         sizeof(prefix_info->nd_opt_pi_prefix.s6_addr));

  /* Zero the bits which are greater than Prefix length. */
  prefix_ptr = (struct ps_in6_addr *)&prefix_info->nd_opt_pi_prefix;
  if ( prefix_info->nd_opt_pi_prefix_len <= IPV6_MIN_PREFIX_LENGTH )
  {
    /* Zero out the non prefix bits. */
    prefix_ptr->ps_s6_addr64[0] >>=
                  (IPV6_MIN_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
    prefix_ptr->ps_s6_addr64[0] <<=
                   (IPV6_MIN_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
    /* Zero out the IID part */
    prefix_ptr->ps_s6_addr64[1] = 0;
  }
  else
  {
    /* No need to touch the first 64 bits. Just Modify the  */
    /* Zero out the non prefix bits. */
    prefix_ptr->ps_s6_addr64[1] >>=
                   (IPV6_MAX_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
    prefix_ptr->ps_s6_addr64[1] <<=
                   (IPV6_MAX_PREFIX_LENGTH - prefix_info->nd_opt_pi_prefix_len);
  }

  if (dest_v6_ip != NULL)
    memcpy(dst_addr.sin6_addr.s6_addr, dest_v6_ip,
           sizeof(dst_addr.sin6_addr.s6_addr));
  else
  {
    inet_pton(AF_INET6, "ff02::1", &dst_addr.sin6_addr);
    is_multicast = true;
  }

  dst_addr.sin6_family = AF_INET6;
  if (is_primary_public_rdnss_info_added)
  {
    if (is_secondary_public_rdnss_info_added)
    {
      rdns_info->length = sizeof(struct rdnss)/sizeof(uint64);
      ra_length = sizeof(ra_pkt); //RDNSS RA have both v6 dns server
    }
    else
    {
      rdns_info->length = (sizeof(struct rdnss) - sizeof(struct in6_addr))/sizeof(uint64);
      ra_length = sizeof(ra_pkt) - sizeof(struct in6_addr); //RDNSS RA have primary v6 dns server only
    }
  }
  else if (is_secondary_public_rdnss_info_added)
  {
    rdns_info->length = (sizeof(struct rdnss) - sizeof(struct in6_addr))/sizeof(uint64);
    ra_length = sizeof(ra_pkt) - sizeof(struct in6_addr); //RDNSS RA have secondary v6 dns server only
  }
  else
  {
    ra_length = sizeof(ra_pkt) - sizeof(struct rdnss); //RA have no RNDSS information
  }

  if (send_ra)
  {
    LOG_MSG_ERROR("Forwarding RA", 0, 0, 0);
    LOG_MSG_ERROR("Sending RDNSS info status primary RDNSS=%d seconadry RDNSS=%d",
                  is_primary_public_rdnss_info_added, is_secondary_public_rdnss_info_added, 0);
    if ((ret = sendto(lan_obj->GetBridgeSockFd(),&ra_pkt, ra_length, 0,
         (struct sockaddr *)&dst_addr,sizeof(dst_addr))) < 0)
    {
      LOG_MSG_ERROR("Error: Cannot send RA: %d.", errno, 0, 0);
    }

    if (QcMapTetheringMgr)
    {
      /* Check if PPP enabled and send the prefix. */
      addr_ptr = (struct ps_in6_addr *)dest_v6_ip;
      addr_ptr1 = (struct ps_in6_addr *)QcMapTetheringMgr->ppp_ipv6_iid;
      if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
          ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
            QCMAP_QTI_TETHERED_LINK_PPP) &&
          ((is_multicast == true) ||
           !memcmp(&addr_ptr->ps_s6_addr64[1],
                   &addr_ptr1->ps_s6_addr64[1],
                   sizeof(uint64))))
      {
        LOG_MSG_ERROR("Forwarding RA to PPP interface", 0, 0, 0);
        /* Send the prefix over the PPP interface. */
        ppp_sockfd = lan_obj->CreateRawSocket(PPP_IFACE);

        if ( ppp_sockfd < 0 )
        {
          LOG_MSG_ERROR("Error: PPP Sock is not created", 0, 0, 0);
          return ret;
        }

        if ((ret = sendto(ppp_sockfd,&ra_pkt, ra_length,0,
                (struct sockaddr *)&dst_addr,sizeof(dst_addr))) < 0)
        {
          LOG_MSG_ERROR("Error: Cannot send RA to PPP interface: %d.", errno, 0, 0);
        }
        close(ppp_sockfd);
      }
    }
  }
  /* Based on the deprecate flag decide whether to add/delete the prefix based route. */
  memset(ipaddr, 0, MAX_IPV6_PREFIX + 1);
  inet_ntop(AF_INET6, (struct in6_addr *)prefix_ptr, ipaddr, MAX_IPV6_PREFIX);
  if ( deprecate )
  {
    LOG_MSG_INFO1("Delete the prefix based route for inet6", 0, 0, 0);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del %s/%d", ipaddr, prefix_info->nd_opt_pi_prefix_len);
    ds_system_call(command, strlen(command));
  }
  else
  {
    LOG_MSG_INFO1( "First Delete the prefix based route for WWAN Interface",0, 0, 0 );
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip -6 route del %s/%d", ipaddr, prefix_info->nd_opt_pi_prefix_len);
    ds_system_call(command, strlen(command));
    LOG_MSG_INFO1("Add the prefix based route for Bridge Interface", 0, 0, 0);

    snprintf(command, MAX_COMMAND_STR_LEN,"ip -6 route add %s/%d dev %s",
             ipaddr, prefix_info->nd_opt_pi_prefix_len, bridgeIface);
    ds_system_call(command, strlen(command));
  }
  return ret;
}

/*===========================================================================
  FUNCTION StartRadish
==========================================================================*/
/*!
@brief
  Starts Radish with appropriate intrerface, this will ebanle Ipv6 multicast
  forwarding.

@param
  None

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::StartRadish(void)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char radish_iface[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char v6add_str[INET6_ADDRSTRLEN] = {0};
  char enable_dhcp_f_mode[RADISH_INFO_SIZE]={0};
  qmi_error_type_v01 qmi_err_num;

  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  LOG_MSG_INFO1("Starting radish",0,0,0);

  if (!GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V6_V01,&qmi_err_num))
  {
    LOG_MSG_ERROR("Get device name failed", 0, 0, 0);
    return;
  }

  if (IS_DEFAULT_PROFILE(profileHandle))
  {
    strlcpy(radish_iface, BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
  }
  else
  {
    memset(radish_iface, 0, sizeof(radish_iface));
    strlcpy(radish_iface, GET_ASSOCIATED_BRIDGE_IFACE_NAME(vlan_id), sizeof(radish_iface));
  }

  if (QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_enable_state == QCMAP_MSGR_DHCPV6_MODE_UP_V01)
  {
    //run radish with -d option to not provide DHCP info
    if (QcMapBackhaulWWAN->GetIfaceLLAddrStr(v6add_str, radish_iface))
    {
      LOG_MSG_ERROR("Adding RDNSS info for radish", 0, 0, 0);
      snprintf(enable_dhcp_f_mode, RADISH_INFO_SIZE, "-d %s", v6add_str);
    }
    else
    {
      LOG_MSG_ERROR("Error:Unable to add PROXY RDNSS in RA. not adding RDNSS info .Radish will run in normal mode", 0, 0, 0);
    }
  }

  if ( QcMapTetheringMgr &&
      QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
      (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
       QCMAP_QTI_TETHERED_LINK_PPP))
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "radish -k -b %s -i %s -x -i %s -x -i %s -x %s > /dev/null 2>&1 &",
             devname, devname, radish_iface, PPP_IFACE, enable_dhcp_f_mode);
  }
  else
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "radish -k -b %s -i %s -x -i %s -x %s > /dev/null 2>&1 &",
             devname, devname, radish_iface, enable_dhcp_f_mode);
  }
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION StopRadish
==========================================================================*/
/*!
@brief
  This function will stop Radish.

@param
  None

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::StopRadish(void)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  char radish_iface[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];

  LOG_MSG_INFO1("Stopping radish on bridge%d",vlan_id, 0, 0);

  if (IS_DEFAULT_PROFILE(profileHandle))
  {
    strlcpy(radish_iface, BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
  }
  else
  {
    memset(radish_iface, 0, sizeof(radish_iface));
    strlcpy(radish_iface, GET_ASSOCIATED_BRIDGE_IFACE_NAME(vlan_id), sizeof(radish_iface));
  }

  snprintf(command, MAX_COMMAND_STR_LEN, "kill $(ps | grep radish | grep %s | awk '{print $1}')", radish_iface);
  ds_system_call(command, strlen(command));
}


/*===========================================================================
  FUNCTION RestartRadish
==========================================================================*/
/*!
@brief
  This function will restart Radish.

@param
  None

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::RestartRadish(void)
{
  LOG_MSG_INFO1("Restarting radish",0,0,0);

  StopRadish();
  StartRadish();
}


/*===========================================================================
  FUNCTION IsRadishRunning
==========================================================================*/
/*!
@brief
  Checks to see if radish is running, waits up to 1 second

@parameters
  None

@return
  true  - process is running
  flase - process is not running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::IsRadishRunning()
{
  int i = 0;
  int exit_status;
  char is_radish_running[MAX_COMMAND_STR_LEN];

  snprintf (is_radish_running,MAX_COMMAND_STR_LEN,
            "ps | grep radish | grep %s", GET_ASSOCIATED_BRIDGE_IFACE_NAME(vlan_id));
  while (i++ < 5)
  {
    usleep(200000);
    exit_status = ds_system_call(is_radish_running, strlen(is_radish_running));
    if(exit_status == 0)
    {
      return true;
    }
  }

  LOG_MSG_ERROR("Radish is not running after waiting 1 second", 0, 0, 0);
  return false;
}

/*===========================================================================
  FUNCTION GetIPv4State
==========================================================================*/
/*!
@brief
  Gets the IPv4 state.

@param


@parameters
  uint8_t *status,
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
boolean QCMAP_Backhaul::GetIPv4State
(
  uint8_t *status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  char enable[MAX_STRING_LENGTH]= {0};
  if(QcMapBackhaulMgr)
    *status = QCMAP_Backhaul::enable_ipv4;
  else
  {
    GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,GET_VALUE,enable, MAX_STRING_LENGTH);
    *status=(uint8_t)atoi(enable);
  }
  return true;

}

/*===========================================================================
  FUNCTION GetIPv6State
==========================================================================*/
/*!
@brief
  Gets the IPv6 state.

@parameters
  uint8_t *status,
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
boolean QCMAP_Backhaul::GetIPv6State(uint8_t *status, qmi_error_type_v01 *qmi_err_num)
{
  char enable[MAX_STRING_LENGTH] = {0};
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  if(QcMapBackhaulMgr)
    *status = QCMAP_Backhaul::enable_ipv6;
  else
  {
    GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,GET_VALUE,enable,MAX_STRING_LENGTH);
    *status=(uint8_t)atoi(enable);
  }
  return true;

}

/*===========================================================================
  FUNCTION DeletePPPIPv6Route
==========================================================================*/
/*!
@brief
  Deletes the destination based route to ppp interface.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::DeletePPPIPv6Route()
{
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  memset(ip6_addr, 0, MAX_IPV6_PREFIX+1);
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (!(QcMapTetheringMgr  &&
        (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
         QCMAP_QTI_TETHERED_LINK_PPP)))
  {
    /* QCMAP PPP not enabled */
    LOG_MSG_ERROR("QCMAP PPP not enabled",0,0,0);
    return;
  }

  if (!QCMAP_Backhaul::enable_ipv6)
  {
    /* QCMAP IPV6 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV6 not enabled",0,0,0);
    return;
  }

  /* Delete the destination based route. */
  inet_ntop(AF_INET6,(struct in6_addr *)QcMapTetheringMgr->ppp_ipv6_addr, ip6_addr, MAX_IPV6_PREFIX);
  snprintf( command, MAX_COMMAND_STR_LEN,
            "ip -6 route del %s dev %s", ip6_addr, PPP_IFACE);
  ds_system_call( command, strlen(command));

  /* Reset the address. */
  memset(QcMapTetheringMgr->ppp_ipv6_addr, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

  return;
}

/*===========================================================================
  FUNCTION IsPPPdkilled
==========================================================================*/
/*!
@brief
  Waits 1 second for the process to die.

@parameters
  None

@return
  true  - process killed
  flase - process running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::IsPPPkilled()
{
  char process[MAX_COMMAND_STR_LEN];
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_COMMAND_STR_LEN];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  snprintf(process, MAX_COMMAND_STR_LEN, "pidof pppd");

  while (i++ < QCMAP_PROCESS_KILL_RETRY)
  {
    usleep(QCMAP_PROCESS_KILL_WAIT_MS);
    cmd = popen(process, "r");
    pid = 0;
    if(cmd)
    {
      fgets(pid_s, MAX_COMMAND_STR_LEN, cmd);
      pid = atoi(pid_s);
      LOG_MSG_INFO1("pid =%d", pid, 0, 0);
      pclose(cmd);
      /* If pid is zero we break from while*/
      if(pid == 0)
      {
        return true;
      }
    }
  }

  LOG_MSG_ERROR("PID still running after waiting 2 second", 0, 0, 0);
  return false;
}
/*===========================================================================
  FUNCTION AddPPPIPv6Route
==========================================================================*/
/*!
@brief
  Adds the route for PPP interface.

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
void QCMAP_Backhaul::AddPPPIPv6Route()
{
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (!QcMapTetheringMgr)
  {
    LOG_MSG_ERROR("Tethering is not enabled.", 0, 0, 0);
    return;
  }
  /* Flush the existing routes. */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "ip -6 route flush dev %s proto boot", PPP_IFACE);
  ds_system_call( command, strlen(command));

  inet_ntop( AF_INET6,(struct in6_addr *)QcMapTetheringMgr->ppp_ipv6_addr, ip6_addr,
             MAX_IPV6_PREFIX);

  snprintf( command, MAX_COMMAND_STR_LEN,
            "ip -6 route add %s dev %s", ip6_addr, PPP_IFACE);
  ds_system_call( command, strlen(command));

  return;
}

/*=====================================================
  FUNCTION DeleteGlobalipv6AddressInfo
======================================================*/
/*!
@brief
 Deletes the Global Ipv6 address from the connected
 devices linked list

@parameters
  - void

@return
  bool
  -true on successful deletion
  -false on failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_Backhaul:: DeleteGlobalipv6AddressInfo ()
{
  int i = 0;
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  memset(zero_buff,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("DeleteGlobalipv6AddressInfo - Linked list head is NULL ", 0, 0, 0);
    return false;
  }

  node = addrList->addrListHead->next;
  if ( node == NULL)
  {
    LOG_MSG_ERROR("DeleteGlobalipv6AddressInfo - Device info is NULL"
                  "Cant fetch linked list node", 0, 0, 0);
    return false;
  }

  for (i = 0; i < QcMapMgr->GetNumofConnectedDevices(); i++)
  {
    connectedDevicesList = ( qcmap_cm_client_data_info_t* )node->data;
    if (connectedDevicesList == NULL)
    {
      LOG_MSG_ERROR("DeleteGlobalipv6AddressInfo - Connected Device context for"
                    " node %d+1 is NULL", i, 0, 0);
      return false;
    }

    for (int i = 0 ; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
    {
      if (!memcmp(connectedDevicesList->ipv6[i].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
           break;

      memset(connectedDevicesList->ipv6[i].addr, 0,
              QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    }
    node = node->next;
  }

  LOG_MSG_INFO1("DeleteGlobalipv6AddressInfo:Global Ipv6 address of clients "
                "successfully deleted", 0, 0, 0);
  return true;
}

/*=====================================================
  FUNCTION DeprecateClientRA
======================================================*/
/*!
@brief
  Deprecate Client RA

@parameters
  - void

@return
  -true on successful deletion
  -false on failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean QCMAP_Backhaul::DeprecateClientRA(void)
{
  ds_dll_el_t                 *node, *node2;
  qcmap_cm_nl_prefix_info_t   ipv6_prefix_info;
  qmi_error_type_v01          qmi_err_num;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_addr_info_list_t* addrList_prev = &(QcMapMgr->addrList_prev);
  struct ps_in6_addr null_ipv6_address;
  qcmap_mac_addr_vlan_info_t mac_addr_vlan_info;

  if(addrList->addrListHead == NULL && addrList_prev->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Error: Lists are empty", 0, 0, 0);
    return false;
  }

  if(addrList->addrListHead == NULL)
  {
    LOG_MSG_INFO1("Current device list is empty, use previous", 0, 0, 0);
    node = ds_dll_next(addrList_prev->addrListHead, (const void**)(&connectedDevicesList));
  }
  else if(addrList_prev->addrListHead == NULL){
    LOG_MSG_INFO1("Use Current device list", 0, 0, 0);
    node = ds_dll_next(addrList->addrListHead, (const void**)(&connectedDevicesList));
  }
  else{
    LOG_MSG_INFO1("Merging lists", 0, 0, 0);

    node = addrList->addrListHead;
    node = ds_dll_next (node, (const void**)(&connectedDevicesList));

    while (node != NULL)
    {
      memcpy(mac_addr_vlan_info.mac_addr,connectedDevicesList->mac_addr,
             QCMAP_MSGR_MAC_ADDR_LEN_V01);
      mac_addr_vlan_info.vlan_id = connectedDevicesList->vlan_id;

      if ((node2 = ds_dll_search(addrList_prev->addrListHead,
                                 (void*)&mac_addr_vlan_info,
                                 QcMapMgr->qcmap_match_mac_addr_vlan_id_pair)) == NULL)
      {
        /* mac_addr not found, so add to list */
        LOG_MSG_INFO1("Adding element to list", 0, 0, 0);
        if ((node2 = ds_dll_enq(addrList_prev->addrListHead,
                             NULL, (void*)connectedDevicesList )) == NULL)
        {
          LOG_MSG_ERROR("Error in adding a node",0,0,0);
          return false;
        }
        addrList_prev->addrListTail = node2;
      }
      node = ds_dll_next (node, (const void**)(&connectedDevicesList));
    }

    node = ds_dll_next(addrList_prev->addrListHead, (const void**)(&connectedDevicesList));
  }

  if (node == NULL)
  {
    LOG_MSG_ERROR("Error: first element in list is empty", 0, 0, 0);
    return false;
  }

  memset(&null_ipv6_address, 0, sizeof(struct ps_in6_addr));

  while (node != NULL)
  {
    if (connectedDevicesList == NULL)
    {
      LOG_MSG_ERROR("connectedDevicesList is NULL", 0, 0, 0);
      node = ds_dll_next (node, (const void**)(&connectedDevicesList));
      continue;
    }

    if (memcmp(connectedDevicesList->ipv6[0].addr, &null_ipv6_address, sizeof(struct in6_addr)))
    {

      memset(&ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
      ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      ipv6_prefix_info.prefix_len =  (unsigned char) IPV6_MIN_PREFIX_LENGTH;
      memcpy(((struct sockaddr_in6 *)&(ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr,
             connectedDevicesList->ipv6[0].addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

      if(this->UpdatePrefix(&this->ipv6_prefix_info, true, true, connectedDevicesList->link_local_v6_addr) == -1)
      {
        LOG_MSG_ERROR("Error: Unable to deprecate prefix", 0, 0, 0);
        return false;
      }
    }
    else
      LOG_MSG_ERROR("Node does not have global v6 addr, skipping", 0, 0, 0);

    node = ds_dll_next (node, (const void**)(&connectedDevicesList));
  }

  //Remove Delegated Prefix's
  if (QCMAP_Backhaul_WWAN::SendDeleteDelegatedPrefix(false, NULL, &qmi_err_num) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Error: Unable flush prefix's %d", qmi_err_num, 0, 0);
  }

  /* Cleanup addrList_prev */
  if (addrList_prev->addrListHead != NULL)
  {
    while ((node = ds_dll_deq( addrList_prev->addrListHead, &(addrList_prev->addrListTail),
            (const void**) &connectedDevicesList)))
    {
      //Free the device information structure
      ds_free (connectedDevicesList);
      connectedDevicesList = NULL;
      ds_dll_free(node);
    }
    ds_dll_destroy(addrList_prev->addrListHead);
  }

  return true;
}

/*===========================================================================
  FUNCTION AssignGlobalV6AddrAllClients
==========================================================================*/
/*!
@brief
  Prepend the Ipv6 prefix to the Link local address for all clients with
  vlan_id passed

@parameters
  struct ps_in6_addr *prefix_addr_ptr - To get the Ipv6 prefix
  int16_t             vlan_id         - To get the clients with vlan_id

@return
  bool

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_Backhaul::AssignGlobalV6AddrAllClients
(
  struct ps_in6_addr *prefix_addr_ptr, int16_t vlan_id
 )
{
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* nodeInfo = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  struct ps_in6_addr *iid_ptr = NULL;
  struct ps_in6_addr *global_addr_ptr = NULL;
  const void   *dummy = NULL;
  char ipv6addr[INET6_ADDRSTRLEN];


  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("AssignGlobalV6AddrAllClients - Linked list head is NULL ", 0, 0, 0);
    return false;
  }

  node = addrList->addrListHead->next;
  if ( node == NULL)
  {
    LOG_MSG_ERROR("AssignGlobalV6AddrAllClients - Linked list node is NULL", 0, 0, 0);
    return false;
  }

  /*  Update Global v6 address for all the client with the prefix*/
  while(node != NULL)
  {
    nodeInfo = ( qcmap_cm_client_data_info_t* )ds_dll_data(node);

    if (nodeInfo->vlan_id == vlan_id)
    {
      iid_ptr = (struct ps_in6_addr *)nodeInfo->link_local_v6_addr;
      global_addr_ptr = (struct ps_in6_addr *)nodeInfo->ipv6[0].addr;

      global_addr_ptr->ps_s6_addr64[0] = prefix_addr_ptr->ps_s6_addr64[0];
      global_addr_ptr->ps_s6_addr64[1] = iid_ptr->ps_s6_addr64[1];

      memset(ipv6addr, 0, INET6_ADDRSTRLEN);
      inet_ntop(AF_INET6,(void *)nodeInfo->ipv6[0].addr,ipv6addr, INET6_ADDRSTRLEN);
      LOG_MSG_INFO1("Global V6 Address %s updated\n",ipv6addr,0,0);

      if (QcMapTetheringMgr && ((nodeInfo->device_type ==
              QCMAP_MSGR_DEVICE_TYPE_USB_V01) &&
            (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
             QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
             QCMAP_QTI_TETHERED_LINK_PPP)))
      {
        memcpy( QcMapTetheringMgr->ppp_ipv6_addr,
               nodeInfo->ipv6[0].addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        this->AddPPPIPv6Route();
      }
    }
    node = ds_dll_next(node, &dummy);
  }
  return true;
}
/*===========================================================================
  FUNCTION GetIP
==========================================================================*/
/*!
@brief
  Gets IP address and netmask assigned to the STA interface.

@parameters
  uint32 *staIP
  uint32 *netMask
  char* devname

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetIP(uint32 *retIP, uint32 *netMask, char *iface)
{
  int s, ret;
  struct ifreq buffer;
  struct sockaddr *sa;

  if ( retIP == NULL || netMask == NULL )
  {
    LOG_MSG_ERROR("Null arguements passed.",0,0,0);
    return false;
  }

  /* Open a socket */
  s = socket(PF_INET, SOCK_DGRAM, 0);
  if ( s < 0 )
  {
    LOG_MSG_ERROR("Unable to open socket to get IP address.",0,0,0);
    return false;
  }
  /* Set up the interface request buffer for wlan0. */
  memset(&buffer, 0x00, sizeof(buffer));
  strlcpy(buffer.ifr_name, iface, IFNAMSIZ);

  /* Call the ioctl to get the address. */
  ret = ioctl(s, SIOCGIFADDR, &buffer);

  if ( ret < 0 )
  {
    LOG_MSG_ERROR("Unable to call ioctl to get IP address.",0,0,0);
    /* Close the socket handle. */
    close(s);
    return false;
  }

  /* Copy out the ip address for the interface. */
  sa = (struct sockaddr *)&(buffer.ifr_addr);
  *retIP = ((struct sockaddr_in *)sa)->sin_addr.s_addr;

  /* Set up the interface request buffer for wlan0. */
  memset(&buffer, 0x00, sizeof(buffer));
  strlcpy(buffer.ifr_name, iface, IFNAMSIZ);

  /* Call the ioctl to get the address. */
  ret = ioctl(s, SIOCGIFNETMASK, &buffer);

  if ( ret < 0 )
  {
    LOG_MSG_ERROR("Unable to call ioctl to get netmask.",0,0,0);
    /* Close the socket handle. */
    close(s);
    return false;
  }

  /* Copy out the netmask for the interface. */
  sa = (struct sockaddr *)&(buffer.ifr_netmask);
  *netMask = ((struct sockaddr_in *)sa)->sin_addr.s_addr;

  /* Close the socket handle. */
  close(s);

  return true;
}


/*===========================================================================
  FUNCTION ReadBackhaulConfigFromXML
==========================================================================*/
/*!
@brief
  Reads the Backhaul Config from XML.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::ReadBackhaulConfigFromXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root,child;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
  strlcpy(this->wan_cfg.eri_config_file, root.child(EriConfig_TAG).child_value(),
          QCMAP_CM_MAX_FILE_LEN);
  profileHandle = atoi(root.child(ProfileHandle_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
  QCMAP_Backhaul::enable_ipv4 = atoi(root.child(EnableIPV4_TAG).child_value());
  QCMAP_Backhaul::enable_ipv6 = atoi(root.child(EnableIPV6_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);

  this->wan_cfg.enable_webserver_wwan_access =
                  atoi(root.child(EnableWebserverWWANAccess_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
  this->tiny_proxy_enable_state= atoi(root.child(TINY_PROXY_Enable_TAG).child_value());
  LOG_MSG_INFO1("Tinyproxy state: %d", this->tiny_proxy_enable_state, 0, 0);

  return true;
}


/*===========================================================================
  FUNCTION SetBackhaulPrioConfigToXML()

  DESCRIPTION
    Helper function that writes Backhaul pref to XML

  PARAMETERS
    qcmap_msgr_backhaul_type_enum_v01 backhaul_type
    cmap_backhaul_config_enum backhaul_config

  DEPENDENCIES
    qmi_qcmap_msgr_init() must have been called

  SIDE EFFECTS
    None
  ===========================================================================*/
boolean QCMAP_Backhaul::SetBackhaulPrioConfigToXML
(
  qcmap_msgr_backhaul_type_enum_v01 backhaul_type,
  qcmap_backhaul_config_enum backhaul_config
 )
{
  boolean ret_value = true;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_ERROR("Set backhaul %d and config %d",backhaul_type,backhaul_config,0);
  switch (backhaul_type)
  {
    case QCMAP_MSGR_WWAN_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,WWAN_BACKHAUL,sizeof(WWAN_BACKHAUL));
      break;
    case QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,USB_CRADLE_BACKHAUL,sizeof(USB_CRADLE_BACKHAUL));
      break;
    case QCMAP_MSGR_WLAN_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,WLAN_BACKHAUL,sizeof(WLAN_BACKHAUL));
      break;
    case QCMAP_MSGR_ETHERNET_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,ETH_BACKHAUL,sizeof(ETH_BACKHAUL));
      break;
    case QCMAP_MSGR_BT_BACKHAUL_V01:
      QCMAP_Backhaul::GetSetBackhaulConfigFromXML(backhaul_config,
                  SET_VALUE,BT_BACKHAUL,sizeof(BT_BACKHAUL));
      break;
    default:
      ret_value = false;
      break;
  }
  return ret_value;
}
/*===========================================================================
  FUNCTION GetSetBackhaulConfigFromXML
==========================================================================*/
/*!
@brief
  Get or set the Backhaul Config from XML.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::GetSetBackhaulConfigFromXML
(
 qcmap_backhaul_config_enum conf,
 qcmap_action_type action,
 char *data,
 int datalen
 )
{
  pugi::xml_document xml_file;
  pugi::xml_node root,child;
  const char *tag_ptr;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);


  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  switch(conf)
  {
    case CONFIG_ENABLE_IPV4:
      tag_ptr = EnableIPV4_TAG;
      break;

    case CONFIG_ENABLE_IPV6:
      tag_ptr = EnableIPV6_TAG;
      break;

    case CONFIG_WEBSERVER_WWAN_ACCESS:
      tag_ptr = EnableWebserverWWANAccess_TAG;
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPNatCfg_TAG);
      break;

    case CONFIG_ERI_CONFIG:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = EriConfig_TAG;
      break;

    case CONFIG_FIRST_PREFFERED_BACKHAUL:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = FirstPreferredBackhaul_TAG;
      break;

    case CONFIG_SECOND_PREFFERED_BACKHAUL:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = SecondPreferredBackhaul_TAG;
      break;

    case CONFIG_THIRD_PREFFERED_BACKHAUL:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = ThirdPreferredBackhaul_TAG;
      break;

    case CONFIG_FOURTH_PREFFERED_BACKHAUL:
      LOG_MSG_ERROR("Fourth backhaul %d, action %d.", CONFIG_FOURTH_PREFFERED_BACKHAUL, action, 0);
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = FourthPreferredBackhaul_TAG;
      break;

    case CONFIG_FIFTH_PREFFERED_BACKHAUL:
      LOG_MSG_ERROR("Fifth backhaul %d, action %d.", CONFIG_FIFTH_PREFFERED_BACKHAUL, action, 0);
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
      tag_ptr = FifthPreferredBackhaul_TAG;
      break;

    default:
      LOG_MSG_ERROR("Invalid config type.", 0, 0, 0);
      return false;
      break;
  }

  if (action == SET_VALUE)
  {
    root.child(tag_ptr).text() = data;
    LOG_MSG_ERROR("Fourth backhaul %d, action %d data %s.", CONFIG_FOURTH_PREFFERED_BACKHAUL, action, data);
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
    strlcpy(data, root.child(tag_ptr).child_value(), datalen);
  ds_log_med("Set/Get Backhaul config succesful %s %s",tag_ptr,root.child(tag_ptr).child_value());
  return true;
}

/*===========================================================================
  FUNCTION SendRSForClients
==========================================================================*/
/*!
@brief
  Send RS to modem for ipv6 link local address, so that modem replies with
  RA, which will be used to parse and store Global Ipv6 Address.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SendRSForClients(void)
{
  ds_dll_el_t         *node;
  int                 qcmap_cm_error;
  int                 i, num_entries;
  char                devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  char                ipv6addr[INET6_ADDRSTRLEN];

  if(!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR("QcMapBackhaulWWANMgr* is Invalid",0,0,0);
    return false;
  }

  memset(ipv6addr, 0, INET6_ADDRSTRLEN);

  if(addrList->addrListHead != NULL && addrList->addrListHead->next != NULL)
  {
    //Get number of connected clients
    num_entries = QcMapMgr->conn_device_info.numOfNodes;
    node = ds_dll_next(addrList->addrListHead, (void **) &connectedDevicesList);

    //Send the RS packet to the rmnet iface
    memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
    if (QcMapBackhaulWWANMgr->GetDeviceName(QcMapMgr->qcmap_cm_handle,
                                            QCMAP_MSGR_IP_FAMILY_V6_V01,
                                            devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
    {
      LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d", qcmap_cm_error,0,0);
      return false;
    }

    //Main loop, send RS for each connected client
    for (i = 0; i < num_entries; i++)
    {
      if(!QCMAP_Backhaul::SendRS(devname, connectedDevicesList->link_local_v6_addr))
      {
        inet_ntop(AF_INET6,
                  (void *)connectedDevicesList->link_local_v6_addr,
                  ipv6addr, INET6_ADDRSTRLEN);
        ds_log_med("Unable to send RS from src %s", ipv6addr);
        return false;
      }
      node = ds_dll_next(node, (void **) &connectedDevicesList);
    }
  }
  else
  {
    LOG_MSG_ERROR("No connected devices", 0,0,0);
  }

  return true;
}


/*===========================================================================
  FUNCTION SendRSForClient
==========================================================================*/
/*!
@brief
  Send RS to backhaul interface for ipv6 link local address, so that
  backhaul interface replies with RA, which will be used to parse and
  store Global Ipv6 Address.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
  */
/*=========================================================================*/
boolean QCMAP_Backhaul::SendRSForClient(uint8_t *src_v6_addr)
{
  char                devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char                ipv6addr[INET6_ADDRSTRLEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();

  memset(ipv6addr, 0, INET6_ADDRSTRLEN);

  if (!GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num))
  {
    return;
  }

  if(!QCMAP_Backhaul::SendRS(devname, src_v6_addr))
  {
    inet_ntop(AF_INET6,
              (void *)src_v6_addr,
              ipv6addr, INET6_ADDRSTRLEN);
    ds_log_med("Unable to send RS from src %s", ipv6addr);
    return false;
  }

  return true;
}


/*===========================================================================
  FUNCTION SendRSwithUnspecifiedSrc
==========================================================================*/
/*!
@brief
  Checks source against CDI then sends an unspecified RS out on wlan0.
  This forces external AP to send out a multicast RA.

@parameters
- Devname
- src_v6_addr

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::SendRSwithUnspecifiedSrc(qcmap_nl_sock_msg_t* qcmap_nl_buf)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  struct nd_opt_hdr *opt_hdr;
  struct ether_addr *source_mac_addr;
  int src_mac_set = 0;
  int packet_size;
  int payload_len;
  qcmap_mac_addr_vlan_info_t mac_addr_vlan_info;
  qcmap_nl_addr_t  *nl_addr = NULL;

  if (qcmap_nl_buf == NULL)
  {
    LOG_MSG_ERROR("Input nl_buf is NULL \n",0,0,0);
    return;
  }

  nl_addr = &(qcmap_nl_buf->nl_addr);

  LOG_MSG_INFO1("SendUnspecifiedRS",0,0,0);

  //look through packet for mac address
  packet_size = sizeof(struct nd_router_solicit);
  opt_hdr = (struct nd_opt_hdr *) (nl_addr->nd_packet_buf +
                                   sizeof(struct ip6_hdr) +
                                   sizeof(struct nd_router_solicit));

  struct ip6_hdr *hdr;
  hdr = (struct ip6_hdr*) nl_addr->nd_packet_buf;
  payload_len = ntohs(hdr->ip6_plen);

  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0 && payload_len > 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_SOURCE_LINKADDR)
    {
      source_mac_addr = (struct ether_addr*) (nl_addr->nd_packet_buf +
                                              sizeof(struct ip6_hdr) +
                                              (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));
      ds_log_med("src MAC %s", ether_ntoa(source_mac_addr));
      src_mac_set = 1;
      memcpy(mac_addr_vlan_info.mac_addr,source_mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);
      mac_addr_vlan_info.vlan_id = qcmap_nl_buf->vlan_id;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
    payload_len -= opt_hdr->nd_opt_len;
  }

  if (src_mac_set && ds_dll_search(addrList->addrListHead,
                                   (void*)&mac_addr_vlan_info,
                                   QcMapMgr->qcmap_match_mac_addr_vlan_id_pair) != NULL)
  {
    QCMAP_Backhaul::SendRS(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index), NULL);
  }
  else
  {
    LOG_MSG_ERROR("Source link address not in RS",0,0,0);
  }
}


/*===========================================================================
  FUNCTION SendRS
==========================================================================*/
/*!
@brief
  Send RS packet out on raw socket. Option to add source address, if not present
  then send out with unspecified address (::).

@parameters
- Devname
- src_v6_addr

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::SendRS(char *devname, uint8_t *src_v6_addr)
{
  int                 frame_length, sd, bytes;
  struct ip6_hdr      iphdr;
  struct icmp6_hdr    icmphdr;
  struct sockaddr_in6 dst_addr;
  struct ifreq        ifr;
  uint8               *frame = NULL;
  int status;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();

  if (QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! Backhaul Object is NULL", 0, 0, 0);
    return false;
  }

  if (src_v6_addr != NULL && !QcMapBackhaulMgr->IsRadishRunning())
  {
    /*Radish takes care of forwarding the RA, if it isn't running then
      sending a RS will do nothing*/
    LOG_MSG_ERROR("Radish is not running, not sending RS",0,0,0);
    return false;
  }

  //set mutlicast router destination
  memset(&dst_addr, 0, sizeof(dst_addr));
  inet_pton(AF_INET6, ROUTER_MULTICAST, &dst_addr.sin6_addr);
  dst_addr.sin6_family = AF_INET6;

  //open RAW socket for sending RS packet
  if ((sd = socket (AF_INET6, SOCK_RAW, IPPROTO_RAW)) < 0) {
    LOG_MSG_ERROR ("socket() failed to get socket descriptor for using ioctl() %d ", perror, 0, 0);
    return false;
  }

  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_name, devname, sizeof(ifr.ifr_name));
  setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));

  frame = (uint8 *) malloc(FRAME_SIZE);
  if (!frame)
  {
    LOG_MSG_ERROR("No memory %d ", errno, 0, 0);
    close(sd);
    return;
  }

  // IPv6 header
  memset(&iphdr, 0, sizeof(iphdr));

  // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
  iphdr.ip6_flow = htonl ((MAC_SIZE << FLOW_TOTAL) | (0 << FLOW_LABEL) | 0);

  // Payload length (16 bits): ICMP header + ICMP data
  iphdr.ip6_plen = htons (ICMP_HDRLEN);
  iphdr.ip6_nxt = IPPROTO_ICMPV6; //next header
  iphdr.ip6_hops = MAX_HOPS;

  // Source IPv6 address
  if (src_v6_addr == NULL)
  {
    inet_pton(AF_INET6, "::", iphdr.ip6_src.s6_addr);
  }
  else
  {
    memcpy(iphdr.ip6_src.s6_addr, src_v6_addr, sizeof(uint8_t)*QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  }

  // Destination IPv6 address
  if ((status = inet_pton (AF_INET6, ROUTER_MULTICAST, &(iphdr.ip6_dst))) != 1) {
    LOG_MSG_ERROR ("inet_pton failed", 0, 0, 0);
    free(frame);
    close(sd);
    return false;
  }

  // ICMP header
  memset(&icmphdr, 0, sizeof(icmphdr));

  icmphdr.icmp6_type = ND_ROUTER_SOLICIT;
  icmphdr.icmp6_code = MESSAGE_CODE;
  icmphdr.icmp6_cksum = ICMP_HEADER_CHKSUM;
  icmphdr.icmp6_cksum = icmp6_checksum(&iphdr, (struct icmp6_header*)&icmphdr, ICMP_HDRLEN);

  frame_length = IP6_HDRLEN + ICMP_HDRLEN;

  LOG_MSG_INFO1("Entered SendRS on %s iface", devname, 0, 0);

  // IPv6 header
  memcpy (frame, &iphdr, IP6_HDRLEN * sizeof (uint8));

  // ICMP header
  memcpy (frame + IP6_HDRLEN, &icmphdr, ICMP_HDRLEN * sizeof (uint8));

  // Send ethernet frame to socket.
  if ((bytes = sendto (sd, frame, frame_length * sizeof(uint8), 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr))) <= 0)
  {
    LOG_MSG_ERROR("Send to failed: %d ", errno, 0, 0);
    free(frame);
    close(sd);
    return false;
  }
  LOG_MSG_INFO1("Sent num_bytes: %d frame length: %d ", bytes, frame_length, 0);

  // Free allocated memory.
  free(frame);
  close(sd);

  return true;
}

/*===========================================================================
  FUNCTION ValidateGlobalV6addr
==========================================================================*/
/*!
@brief
  Validates the prefix present in RA packet
  If the backhaul if WWAN, gets the vlan_id mapped to WWAN profile.

@parameters
  struct ps_in6_addr *prefix_addr_ptr[]
  - Prefix to be validated.
  int16_t *vlan_id - Vlan id mapped to WWAN profile, if backhaul is WWAN

@return
  TRUE - If it is a valid prefix.
  false - If it not.

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::ValidateGlobalV6addr
(
 struct ps_in6_addr *prefix_addr_ptr,
 int16_t *vlan_id
 )
{
  struct ps_in6_addr *prefix_info_ptr;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  int ret;
  qmi_error_type_v01 qmi_err_num;
  qcmap_cm_nl_prefix_info_t  wan_ipv6_prefix_info;
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  LOG_MSG_INFO1("ValidateGlobalV6addr",0,0,0);

  ret = GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num);
  LOG_MSG_INFO1("Dev type = %d",ret,0,0);

  memset(&wan_ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
  /*Initialize the VLAN ID to zero, VLAN ID is valid only for WWAN backhaul*/
  *vlan_id = 0;

  if (ret != NO_BACKHAUL)
  {
    ds_log_med("devname %d", devname);
  }

  if (ret==BACKHAUL_TYPE_CRADLE &&
      QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
  {
    LOG_MSG_INFO1("Get v6 prefix from cradle",0,0,0);
    /* Get the STA/WWAN prefixes if available. */
    if ( QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))
    {
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      }
      else
      {
        strlcpy(devname,QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, QCMAP_MSGR_INTF_LEN);
      }
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with STA prefix when in cradle mode. Ignore RA.", 0, 0, 0);
        return false;
      }
    }
    if ( QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) &&
        QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
        QCMAP_QTI_TETHERED_LINK_ETH &&
        QcMapBackhaulEthMgr &&
        QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
    {
      strlcpy(devname,ETH_IFACE, QCMAP_MSGR_INTF_LEN); //Need to add support when bridge mode is activated
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                         &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
            &prefix_addr_ptr->ps_s6_addr64[0],
            sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with ETH Backhaul"
                      " prefix when in Cradle mode. Ignore RA.",
                      0, 0, 0);
        return false;
      }
    }
    if ((QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle,
                                             QCMAP_MSGR_IP_FAMILY_V6_V01,
                                             devname, &qcmap_cm_error)) ==
                                             QCMAP_CM_SUCCESS )
    {
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with WWAN Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with WWAN prefix when in cradle mode. Ignore RA.", 0, 0, 0);
        return false;
      }
    }
  }
  if (ret==BACKHAUL_TYPE_ETHERNET &&
      QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
  {
    if ( QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
        QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].
                                   link_type == QCMAP_QTI_TETHERED_LINK_ECM &&
        QcMapBackhaulCradleMgr &&
        (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode ==
                                             QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 ||
         QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode ==
                                     QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01) )
    {
      if ( QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() )
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      else
        strlcpy(devname,ECM_IFACE, QCMAP_MSGR_INTF_LEN);

      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
            &prefix_addr_ptr->ps_s6_addr64[0],
            sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with Cradle prefix"
                      " when in ETH Backhaul mode. Ignore RA.",
                      0, 0, 0);
        return false;
      }
    }
    if ( QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))
    {
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      }
      else
      {
        strlcpy(devname,QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, QCMAP_MSGR_INTF_LEN);
      }
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR(" Prefix matches with STA prefix"
                      " when in ETH Backhaul mode. Ignore RA.\n",
                      0, 0, 0);
        return false;
      }
    }
    if ((QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle,
                                             QCMAP_MSGR_IP_FAMILY_V6_V01,
                                             devname, &qcmap_cm_error)) ==
                                             QCMAP_CM_SUCCESS )
    {
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with WWAN Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with WWAN prefix"
                      " when in ETH Backhaul mode. Ignore RA.",
                      0, 0, 0);
        return false;
      }
    }

  }
  else if ((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER ||
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE) &&
      QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
  {
    LOG_MSG_INFO1("Get v6 prefix from external AP",0,0,0);
    /* Get the Cradle/WWAN prefixes if available. */
    if ( QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
        QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
        QCMAP_QTI_TETHERED_LINK_ECM &&
        QcMapBackhaulCradleMgr &&
        QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 )
    {
      if ( QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() )
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      else
        strlcpy(devname,ECM_IFACE, QCMAP_MSGR_INTF_LEN);

      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
            &prefix_addr_ptr->ps_s6_addr64[0],
            sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with Cradle prefix when in STA mode. Ignore RA.",
            0, 0, 0);
        return false;
      }
    }
    if ( QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) &&
        QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
        QCMAP_QTI_TETHERED_LINK_ETH &&
        QcMapBackhaulEthMgr &&
        QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
    {
      strlcpy(devname,ETH_IFACE, QCMAP_MSGR_INTF_LEN); //Need to add support when bridge mode is activated
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
            &prefix_addr_ptr->ps_s6_addr64[0],
            sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with ETH Backhaul"
                      " prefix when in STA mode. Ignore RA.",
                      0, 0, 0);
        return false;
      }
    }
    if ((QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle,
            QCMAP_MSGR_IP_FAMILY_V6_V01, devname, &qcmap_cm_error)) ==
        QCMAP_CM_SUCCESS )
    {
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with WWAN Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with WWAN prefix when in station mode. Ignore RA.",
                      0, 0, 0);
        return false;
      }
    }
  }
  else if ((QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle,
          QCMAP_MSGR_IP_FAMILY_V6_V01, devname, &qcmap_cm_error)) ==
      QCMAP_CM_SUCCESS )
  {
    LOG_MSG_INFO1("Get v6 prefix from WWAN",0,0,0);
    /*Get the VLAN ID mapped to WWAN profile */
    *vlan_id = this->vlan_id;
    /* Get the STA/Cradle prefixes if available. */
    if (QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
        QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
        QCMAP_QTI_TETHERED_LINK_ECM &&
        QcMapBackhaulCradleMgr &&
        QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01 )
    {
      if ( QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() )
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      else
        strlcpy(devname,ECM_IFACE, QCMAP_MSGR_INTF_LEN);

      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)\
                       &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with Cradle prefix when in WWAN mode. Ignore RA.",
                       0, 0, 0);
        return false;
      }
    }
    if ( QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) &&
        QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
        QCMAP_QTI_TETHERED_LINK_ETH &&
        QcMapBackhaulEthMgr &&
        QcMapBackhaulEthMgr->eth_cfg.eth_mode == QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
    {
      strlcpy(devname,ETH_IFACE, QCMAP_MSGR_INTF_LEN); //Need to add support when bridge mode is activated
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *)&(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with Cradle Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
          &prefix_addr_ptr->ps_s6_addr64[0],
          sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with ETH Backhaul"
                      " prefix when in WWAN mode. Ignore RA.",
                      0, 0, 0);
        return false;
      }
    }
    if ( QcMapWLANMgr &&
        (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))
    {
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
      {
        strlcpy(devname,BRIDGE_IFACE, QCMAP_MSGR_INTF_LEN);
      }
      else
      {
        strlcpy(devname,QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, QCMAP_MSGR_INTF_LEN);
      }
      wan_ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
      wan_ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
      GetIPV6PrefixInfo(devname,&(wan_ipv6_prefix_info));
      prefix_info_ptr = (struct ps_in6_addr *) &(((struct sockaddr_in6 *)\
                        &(wan_ipv6_prefix_info.prefix_addr))->sin6_addr);
      /* If the prefix matches with STA Prefix. Ignore RA. */
      if (!memcmp(&prefix_info_ptr->ps_s6_addr64[0],
                  &prefix_addr_ptr->ps_s6_addr64[0],
                  sizeof(uint64)))
      {
        LOG_MSG_ERROR("Prefix matches with STA prefix when in WWAN mode. Ignore RA.",
                      0, 0, 0);
        return false;
      }
    }
  }
  return true;
}

/*===========================================================================
 FUNCTION ComparePriority
==========================================================================*/
/*!
@brief
  Compares priority of the two given interfaces.

@parameters
  intf1 : First interface
  intf2 : Second Interface

@return
  true : If priority of intf1 is higher than that of intf2
  false : If priority of intf1 is lower than that of intf2
@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul::ComparePriority(qcmap_backhaul_type intf1, qcmap_backhaul_type intf2)
{
  /*Here, we compare the priority based on preffered backhual list.
    In the preffered backhaul list, we have only WWAN, AP-STA or Cradle.
    Hence, both AP-STA Router mode and Bridge mode have been covered
    in a single ENUM (BACKHAUL_TYPE_AP_STA_ROUTER) since at a time
    only one would be active. Therefore, we need to use the same ENUM
    for bridge mode also , for the sake of getting the correct priority*/
  bool ret = false;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (intf1 == BACKHAUL_TYPE_AP_STA_BRIDGE)
    intf1 = BACKHAUL_TYPE_AP_STA_ROUTER;

  if (intf2 == BACKHAUL_TYPE_AP_STA_BRIDGE)
    intf2 = BACKHAUL_TYPE_AP_STA_ROUTER;

  LOG_MSG_INFO1("ComparePriority : Comparing priority for %d and %d",intf1,intf2,0);
  if (intf1 == intf2)
  {
    LOG_MSG_INFO1("Both interfaces are same. itf1 = %d, intf2 = %d",intf1,intf2,0);
    ret = true;
  }
  if (intf2 == NO_BACKHAUL)
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    ret = true;
  }

  else if (QCMAP_Backhaul::preffered_backhaul_first == intf1)
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    ret =  true;
  }
  else if ((QCMAP_Backhaul::preffered_backhaul_second == intf1) &&
      ((QCMAP_Backhaul::preffered_backhaul_third == intf2) ||
       (QCMAP_Backhaul::preffered_backhaul_fourth == intf2)||
       (QCMAP_Backhaul::preffered_backhaul_fifth == intf2)))
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    ret = true;
  }
  else if (QCMAP_Backhaul::preffered_backhaul_third == intf1 &&
      ((QCMAP_Backhaul::preffered_backhaul_fourth == intf2)||
       (QCMAP_Backhaul::preffered_backhaul_fifth == intf2)))
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    ret = true;
  }

  else if (QCMAP_Backhaul::preffered_backhaul_fourth == intf1 &&
      QCMAP_Backhaul::preffered_backhaul_fifth == intf2)
  {
    LOG_MSG_INFO1("Priority of Intf1 is higher than Intf2",0,0,0);
    ret = true;
  }
  if (QcMapMgr && (ret == true) && QcMapMgr->packet_stats_enabled)
  {
    //Send Indication to client when switching from WWAN backahul
    if (QcMapBackhaulMgr != NULL && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN)
    {
      //Since For case when we switching from WWAN to AP-STA ComparePriority can be called
      // two times & current_backhaul variable may still hold backahul as WWAN . Made this changes
      // to send Indication only one time for AP-STA backahul switch
      if (QcMapBackhaulWLANMgr &&
          (QcMapBackhaulWLANMgr->sta_packet_stat_bh_switch_ind_send == true))
      {
        LOG_MSG_INFO1("packet stat backhaul switch indication already"
                      " send for switching from WAN to AP-STA",0,0,0);
      }
      else
      {
        //Need to set this variable As in case of AP-STA send indication can be called twice
        if (QcMapBackhaulWLANMgr && (intf1 == BACKHAUL_TYPE_AP_STA_ROUTER))
        {
          QcMapBackhaulWLANMgr->sta_packet_stat_bh_switch_ind_send = true;
        }
        QCMAP_Backhaul::SendPacketStatIndOnBackhaulSwitch();
        LOG_MSG_INFO1("Packet stat backahul switch indication send for backahul %d",intf1,0,0);
      }
    }
  }

  if (ret == false)
  {
    LOG_MSG_INFO1("Priority of Intf1 is lower than Intf2",0,0,0);
  }
  return ret;
}

/*===========================================================================
  FUNCTION BlockWWANAccessOnIface
==========================================================================*/
/*!
@brief
  This function blocks IPv4/IPv6 Access on provided Iface for MobileAP clients.

@parameters
  ip_vsn : IP type for which Access has to be blocked
  devname : device (interface) for which access has to be blocked

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
void QCMAP_Backhaul::BlockWWANAccessOnIface(ip_version_enum_type ip_vsn, char *devname)
{
  char command[MAX_COMMAND_STR_LEN];

  if (ip_vsn == IP_V4)
  {
    /*===========================================================================
      Block IPv4 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking Bridge IPv4 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*===========================================================================
      Block IPv4 Access between PPP and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking PPP IPv4 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }
  else if (ip_vsn == IP_V6)
  {
    /*===========================================================================
      Block IPv6 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking Bridge IPv6 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*===========================================================================
      Block IPv6 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Blocking PPP IPv6 Access for STA/Cradle Iface",0,0,0);

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }
}


/*===========================================================================
  FUNCTION ClearIfaceRelatedRulesforV4
==========================================================================*/
/*!
@brief
  This function blocks IPv4 Access on provided Iface for MobileAP clients.

@parameters
  devname : device (interface) for which access has to be allowed

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
void QCMAP_Backhaul::ClearIfaceRelatedRulesForV4(char *devname)
{
  char         command[MAX_COMMAND_STR_LEN];

  /*Delete iface related ip4tables rules*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s ! -o %s -j DROP",
           BRIDGE_IFACE,
           devname);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s ! -o %s -j DROP",
           PPP_IFACE,
           devname);
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION ClearIfaceRelatedRulesforV6
==========================================================================*/
/*!
@brief
  This function blocks IPv6 Access on provided Iface for MobileAP clients.

@parameters
  devname : device (interface) for which access has to be allowed

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
void QCMAP_Backhaul::ClearIfaceRelatedRulesForV6(char *devname)
{
  char         command[MAX_COMMAND_STR_LEN];

  if (devname == NULL)
  {
    LOG_MSG_ERROR("Null devname pointer",0,0,0);
    return;
  }

  /*Delete iface related ip6tables rules*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s ! -o %s -j DROP",
           BRIDGE_IFACE,
           devname);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s ! -o %s -j DROP",
           PPP_IFACE,
           devname);
  ds_system_call(command, strlen(command));
}
/*===========================================================================
  FUNCTION StopDHCPCD
==========================================================================*/
/*!
@brief
  Kills DHCPCD on given Iface

@parameters
  char* iface

@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul::StopDHCPCD
(
 char* iface
 )
{
  char command[MAX_COMMAND_STR_LEN];

  LOG_MSG_INFO1("Killing dhcpcd process.",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
            iface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", iface);
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION SendRSOnBridgeIface
==========================================================================*/
/*!
@brief
  Generate a RS Message on bridge iface.

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
void QCMAP_Backhaul::SendRSOnBridgeIface(void)
{
  int ret;
  int loop = 0;
  struct nd_router_solicit rs;
  struct sockaddr_in6 dst_addr;
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT(); /* Will be always on default bridge */

  LOG_MSG_INFO1("SendRSOnBridgeIface",0,0,0);

  if (QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! LAN Object is NULL", 0, 0, 0);
    return;
  }

  memset(&dst_addr, 0, sizeof(dst_addr));
  memset(&rs, 0, sizeof(nd_router_solicit));

  // Populate icmp6_hdr portion of router solicit struct.
  rs.nd_rs_hdr.icmp6_type = ND_ROUTER_SOLICIT;  // 133 (RFC 4861)
  rs.nd_rs_hdr.icmp6_code = MESSAGE_CODE;   // zero for router solicitation (RFC 4861)
  rs.nd_rs_hdr.icmp6_cksum = htons(0);      // zero when calculating checksum

  inet_pton(AF_INET6, "ff02::1", &dst_addr.sin6_addr);
  dst_addr.sin6_family = AF_INET6;

  do{
    if ((ret = sendto(QCMAPLANMgr->GetBridgeSockFd(), &rs, sizeof(nd_router_solicit), 0,
               (struct sockaddr *)&dst_addr,sizeof(dst_addr))) < 0)
    {
      LOG_MSG_ERROR("Error: Cannot send RS on Bridge IFace error: %d.", errno, 0, 0);
      //sleep for .2 seconds
      usleep(200000);
      loop++;
    }
  }while(ret < 0 && loop < QCMAP_BRIDGE_MAX_RETRY);
}


/*===========================================================================
  FUNCTION ProcessNS
==========================================================================*/
/*!
@brief
  Process the incoming NS packet.

@parameters
  nl_addr

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::ProcessNS(qcmap_nl_sock_msg_t* qcmap_nl_buf)
{
  struct ps_in6_addr *dst_addr_ptr = NULL, *src_addr_ptr = NULL;
  struct ps_in6_addr null_ipv6_address;
  ds_dll_el_t *dst_node = NULL, *src_node = NULL, *node = NULL;
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  struct nd_opt_hdr *opt_hdr;
  struct ether_addr *source_mac_addr;
  int src_mac_set = 0;
  int packet_size;
  struct timeval sys_time;
  char dst_str[INET6_ADDRSTRLEN], src_str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  qcmap_addr_info_list_t* cdi_addrList = &(QcMapMgr->addrList);
  qcmap_cm_proxy_data_info_t* proxyInfo = NULL;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  qcmap_mac_addr_vlan_info_t mac_addr_vlan_info;
  qcmap_nl_addr_t  *nl_addr = NULL;

  if (QcMapBackhaulWLANMgr == NULL)
  {
    LOG_MSG_ERROR("WLAN Backhaul is not up", 0, 0, 0);
    return;
  }

  if (qcmap_nl_buf == NULL)
  {
    LOG_MSG_ERROR("Input nl_buf is NULL \n",0,0,0);
    return;
  }

  nl_addr = &(qcmap_nl_buf->nl_addr);

  qcmap_addr_info_list_t* proxy_addrList = &(QcMapBackhaulWLANMgr->proxy_addr_list);

  if(cdi_addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Error: CDI list is empty", 0, 0, 0);
    return;
  }

  LOG_MSG_INFO1("ProcessNS",0,0,0);

  src_addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;
  memset(&null_ipv6_address, 0, sizeof(struct ps_in6_addr));
  if (memcmp (src_addr_ptr, &null_ipv6_address, sizeof(struct in6_addr)) == 0)
  {
    LOG_MSG_ERROR("SRC IP is emtpy, do nothing with NS",0,0,0);
    return;
  }

  packet_size = sizeof(struct nd_neighbor_solicit);
  opt_hdr = (struct nd_opt_hdr *) (nl_addr->nd_packet_buf + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_solicit));
  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_SOURCE_LINKADDR)
    {
      source_mac_addr = (struct ether_addr*) (nl_addr->nd_packet_buf + sizeof(struct ip6_hdr) + (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));
      ds_log_med("src MAC %s", ether_ntoa(source_mac_addr));
      src_mac_set = 1;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
  }

  if (!src_mac_set)
  {
    LOG_MSG_ERROR("SRC MAC is emtpy, do nothing with NS",0,0,0);
    return;
  }

  memcpy(mac_addr_vlan_info.mac_addr,source_mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);
  mac_addr_vlan_info.vlan_id = qcmap_nl_buf->vlan_id;

  src_node = ds_dll_search (cdi_addrList->addrListHead , (void*)&mac_addr_vlan_info,
                              QcMapMgr->qcmap_match_mac_addr_vlan_id_pair);

  dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ipv6_dst_addr;
  tempPrefixIidptr.iid_ptr = dst_addr_ptr;
  dst_node = ds_dll_search (cdi_addrList->addrListHead , (void*)&tempPrefixIidptr,
                              QcMapMgr->qcmap_match_v6_iid);

  if ( dst_node == NULL || src_node == NULL )
  {
    if (memcmp(src_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
        memcmp(src_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
        memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
        memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0)
    {
      LOG_MSG_ERROR("Source or destination of bridge, do nothing",0,0,0);
      return;
    }

    /* From WLAN1 client to WLAN0 client */
    LOG_MSG_INFO2("Send NS to WLAN0 with bridge MAC",0,0,0);

    //add to table
    if (proxy_addrList->addrListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
        information. This is done to make use of the doubly linked list framework which
        is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for node",0,0,0);
        return;
      }
      proxy_addrList->addrListHead = node;
    }

    if (gettimeofday(&sys_time, 0) != 0)
    {
      LOG_MSG_ERROR("Gettimeofday() failed", 0, 0, 0);
      return;
    }

    proxyInfo = (qcmap_cm_proxy_data_info_t*)ds_malloc(sizeof(qcmap_cm_proxy_data_info_t));
    if( proxyInfo == NULL )
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for"
                    "device information structure",0,0,0);
      return;
    }

    memset(proxyInfo,0,sizeof(qcmap_cm_proxy_data_info_t));
    memcpy((struct ps_in6_addr*) proxyInfo->dst_ip_v6_addr, dst_addr_ptr, sizeof(ps_in6_addr));
    memcpy((struct ps_in6_addr*) proxyInfo->src_ip_v6_addr, src_addr_ptr, sizeof(ps_in6_addr));
    memcpy(proxyInfo->src_mac_addr, source_mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
    proxyInfo->time = sys_time.tv_sec;

    //check if entry already exists
    if((node = ds_dll_search (proxy_addrList->addrListHead , (void*) proxyInfo,
                                QcMapMgr->qcmap_match_proxy_entry)) != NULL)
    {
      LOG_MSG_ERROR("Entry already exists, do not add",0,0,0);
      ds_free(proxyInfo);
      proxyInfo = NULL;

      /* update the time */
      proxyInfo = ( qcmap_cm_proxy_data_info_t* )ds_dll_data(node);
      proxyInfo->time = sys_time.tv_sec;
    }
    //Store the mac address, interface name in the linked list
    else if ((node = ds_dll_enq(proxy_addrList->addrListHead,
                           NULL, (void*)proxyInfo )) == NULL)
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in adding a node",0,0,0);
      ds_free(proxyInfo);
      proxyInfo = NULL;
    }
    else
    {
      /* Success New node added! */
      inet_ntop(AF_INET6, proxyInfo->dst_ip_v6_addr, dst_str, INET6_ADDRSTRLEN);
      inet_ntop(AF_INET6, proxyInfo->src_ip_v6_addr, src_str, INET6_ADDRSTRLEN);
      ds_log_med("New proxy entry with dst addr %s, src addr %s", dst_str, src_str);

      //Update the tail pointer when a new node is added everytime
      proxy_addrList->addrListTail = node;
    }
    /* Even if the entry already exists or we can't add node to LL we still need
       to send the NS*/
    QcMapBackhaulMgr->SendProxyNS(nl_addr->nd_packet_buf);
  }
  else
  {
    LOG_MSG_ERROR("CDI client to CDI client, do nothing",0,0,0);
  }
}


/*===========================================================================
  FUNCTION ProcessNA
==========================================================================*/
/*!
@brief
  Process the incoming NA packet.

@parameters
  nl_addr

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::ProcessNA(qcmap_nl_addr_t* nl_addr)
{
  ds_dll_el_t *node = NULL, *temp_node = NULL;
  struct ps_in6_addr *target_addr_ptr = NULL;
  struct ps_in6_addr *dst_addr_ptr = NULL;
  char dst_str[INET6_ADDRSTRLEN], src_str[INET6_ADDRSTRLEN], tar_str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  qcmap_addr_info_list_t* cdi_addrList = &(QcMapMgr->addrList);
  qcmap_cm_proxy_data_info_t *proxyInfo = NULL, *temp_proxyInfo = NULL;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();

  if (QcMapBackhaulWLANMgr == NULL)
  {
    LOG_MSG_ERROR("WLAN Backhaul is not up", 0, 0, 0);
    return false;
  }

  qcmap_addr_info_list_t* proxy_addrList = &(QcMapBackhaulWLANMgr->proxy_addr_list);

  if(cdi_addrList->addrListHead == NULL || proxy_addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Error: Lists are empty", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("ProcessNA",0,0,0);

  /* Get dst address */
  dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ipv6_dst_addr;
  target_addr_ptr = (struct ps_in6_addr *)nl_addr->target_addr;

  node = proxy_addrList->addrListHead;
  node = ds_dll_next (node, (const void**)(&proxyInfo));

  inet_ntop(AF_INET6, dst_addr_ptr, src_str, INET6_ADDRSTRLEN);
  inet_ntop(AF_INET6, target_addr_ptr, tar_str, INET6_ADDRSTRLEN);
  ds_log_med("Searching for proxy entry with dst addr %s, src addr %s, target %s", dst_str, src_str, tar_str);

  while (node != NULL)
  {
    inet_ntop(AF_INET6, proxyInfo->dst_ip_v6_addr, src_str, INET6_ADDRSTRLEN);
    ds_log_med("Proxy entry in list with src addr %s", src_str);

    /* We should only proxy NA's which are destined for bridge */
    if ((memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0 ||
          memcmp(dst_addr_ptr, &QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0) &&
        memcmp(target_addr_ptr, proxyInfo->dst_ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0)
    {
      QcMapBackhaulMgr->SendProxyNA( (struct ps_in6_addr*) proxyInfo->src_ip_v6_addr, nl_addr->nd_packet_buf, proxyInfo->src_mac_addr);

      /* save the next node before deleting current node if we are not at the tail */
      temp_node = ds_dll_next (node, (const void**)(&temp_proxyInfo));

      /* Delete node from list */
      ds_dll_delete_node(proxy_addrList->addrListHead, &(proxy_addrList->addrListTail), node);

      //Free the proxyInfo entry structure
      if (proxyInfo != NULL)
      {
        ds_free (proxyInfo);
        proxyInfo = NULL;
      }

      //Free the memory of the linked list node
      ds_dll_free(node);

      if (proxy_addrList->addrListHead->next == NULL)
      {
        LOG_MSG_INFO1("No more entries",0,0,0);
        ds_dll_free(proxy_addrList->addrListHead);
        proxy_addrList->addrListHead = NULL;
        break;
      }
      node = temp_node;
      proxyInfo = temp_proxyInfo;
    }
    else
    {
      node = ds_dll_next (node, (const void**)(&proxyInfo));
    }
  }
}


/*===========================================================================
  FUNCTION SendProxyNS
==========================================================================*/
/*!
@brief
  Generate a NS Message with bridge's MAC and send to dst.

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
void QCMAP_Backhaul::SendProxyNS(char *nd_packet_buf)
{
  int ret, source_mac_changed = 0;
  int packet_size = 0;
  struct ip6_hdr *ipv6_packet;
  struct nd_opt_hdr *opt_hdr;
  struct icmp6_hdr *icmp6;
  struct sockaddr_in6 dst_addr;
  struct ether_addr bridge_mac_addr;
  struct ether_addr *target_mac_addr;
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();

  LOG_MSG_INFO1("SendProxyNS",0,0,0);

  if (QcMapMgr == NULL || QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL or LAN Object is NULL", 0, 0, 0);
    return;
  }

  icmp6 = (struct icmp6_hdr *) (nd_packet_buf + sizeof(struct ip6_hdr));

  memset(&dst_addr, 0, sizeof(dst_addr));
  ipv6_packet = (struct ip6_hdr *)nd_packet_buf;

  memcpy(&dst_addr.sin6_addr.s6_addr, (in6_addr *) ipv6_packet->ip6_dst.s6_addr, sizeof(in6_addr));
  dst_addr.sin6_family = AF_INET6;

  if( QCMAP_CM_ERROR == QcMapMgr->GetHWAddr((char*) &bridge_mac_addr.ether_addr_octet,
                        BRIDGE_IFACE))
  {
    LOG_MSG_ERROR("Error in fetching Bridge MAC",0,0,0);
    return;
  }

  packet_size = sizeof(struct nd_neighbor_solicit);
  opt_hdr = (struct nd_opt_hdr *) (nd_packet_buf + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_solicit));

  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_SOURCE_LINKADDR)
    {
      target_mac_addr = (struct ether_addr*) (nd_packet_buf + sizeof(struct ip6_hdr) + (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));
      ds_log_med("change %s to bridge MAC %s", ether_ntoa(target_mac_addr), ether_ntoa(&bridge_mac_addr));
      memcpy(target_mac_addr, &bridge_mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
      source_mac_changed = 1;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
  }

  if (!source_mac_changed)
  {
    LOG_MSG_ERROR("source mac not present in NS packet",0,0,0);
    return;
  }

  /* explicitly set packet size from ip packet */
  packet_size = ntohs(ipv6_packet->ip6_plen);

  if ((ret = sendto(QCMAPLANMgr->GetBridgeSockFd(), icmp6, packet_size, 0,
                           (struct sockaddr *)&dst_addr, sizeof(dst_addr))) < 0)
  {
    LOG_MSG_ERROR("Error: Cannot send NS on bridge Iface error: %d.", errno, 0, 0);
  }
}

/*===========================================================================
  FUNCTION SendProxyNA
==========================================================================*/
/*!
@brief
  Generate a NA Message with bridge's MAC and send to dst.

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
void QCMAP_Backhaul::SendProxyNA(struct ps_in6_addr *dst_v6_addr, char *nd_packet_buf, uint8 *src_mac_addr)
{
  int ret;
  int target_mac_changed = 0;
  int packet_size = 0;
  struct nd_opt_hdr *opt_hdr;
  struct icmp6_hdr *icmp6;
  struct ip6_hdr *ipv6_packet;
  struct ether_addr bridge_mac_addr;
  struct sockaddr_in6 dst_addr;
  char str[INET6_ADDRSTRLEN];
  char *packet_buff_cpy;
  struct ether_addr *target_mac_addr;
  ds_dll_el_t *dst_node = NULL, *src_node = NULL;
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* cdi_addrList = &(QcMapMgr->addrList);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  qcmap_mac_addr_vlan_info_t mac_addr_vlan_info;

  LOG_MSG_INFO1("SendProxyNA",0,0,0);

  if (QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! LAN Object is NULL", 0, 0, 0);
    return;
  }

  packet_buff_cpy = (char*) ds_malloc(MAX_ND_PKT_SIZE);
  if(packet_buff_cpy == NULL)
  {
    LOG_MSG_ERROR("Error in allocating packet buffer", 0, 0, 0);
    return;
  }
  memcpy(packet_buff_cpy, nd_packet_buf, MAX_ND_PKT_SIZE);

  icmp6 = (struct icmp6_hdr *) (packet_buff_cpy + sizeof(struct ip6_hdr));
  ipv6_packet = (struct ip6_hdr *)nd_packet_buf;

  memset(&dst_addr, 0, sizeof(struct sockaddr_in6));
  memcpy(&dst_addr.sin6_addr, dst_v6_addr, sizeof(struct ps_in6_addr));
  dst_addr.sin6_family = AF_INET6;

  opt_hdr = (struct nd_opt_hdr *) (packet_buff_cpy + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_advert));

  inet_ntop(AF_INET6, dst_v6_addr, str, INET6_ADDRSTRLEN);
  ds_log_med("Sending Proxy NA to %s", str);

  packet_size = sizeof(struct nd_neighbor_advert);
  opt_hdr = (struct nd_opt_hdr *) (packet_buff_cpy + sizeof(struct ip6_hdr) + sizeof(struct nd_neighbor_advert));

  if( QCMAP_CM_ERROR == QcMapMgr->GetHWAddr((char*) &bridge_mac_addr.ether_addr_octet,
                        BRIDGE_IFACE))
  {
    LOG_MSG_ERROR("Error in fetching Bridge MAC",0,0,0);
    ds_free(packet_buff_cpy);
    return;
  }

  while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0)
  {
    packet_size += (opt_hdr->nd_opt_len * 8);

    if (opt_hdr->nd_opt_type == ND_OPT_TARGET_LINKADDR)
    {
      target_mac_addr = (struct ether_addr*) (packet_buff_cpy + sizeof(struct ip6_hdr) + (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));

      /* Check if both MAC address are in CDI, if so do not send Proxy NA
         This will hapen when tmp global addresses are used as they are not in CDI*/

      memcpy(mac_addr_vlan_info.mac_addr,target_mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);
      mac_addr_vlan_info.vlan_id = 0;

      src_node = ds_dll_search (cdi_addrList->addrListHead,(void*)&mac_addr_vlan_info,
                                QcMapMgr->qcmap_match_mac_addr_vlan_id_pair);

      memcpy(mac_addr_vlan_info.mac_addr,src_mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);
      mac_addr_vlan_info.vlan_id = 0;

      dst_node = ds_dll_search (cdi_addrList->addrListHead , (void*)&mac_addr_vlan_info,
                                QcMapMgr->qcmap_match_mac_addr_vlan_id_pair);

      if (src_node != NULL && dst_node != NULL)
      {
        LOG_MSG_ERROR("ND between local clients",0,0,0);
        break;
      }

      ds_log_med("change bridge mac %s to target MAC %s", ether_ntoa(target_mac_addr), ether_ntoa((struct ether_addr *) &bridge_mac_addr));
      memcpy(target_mac_addr, &bridge_mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
      target_mac_changed = 1;
      break;
    }

    opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
  }

  if (!target_mac_changed)
  {
    LOG_MSG_ERROR("source mac not changed",0,0,0);
    ds_free(packet_buff_cpy);
    return;
  }

  packet_size = ntohs(ipv6_packet->ip6_plen);

  if ((ret = sendto(QCMAPLANMgr->GetBridgeSockFd(), icmp6, packet_size, 0,
                           (struct sockaddr *)&dst_addr, sizeof(dst_addr))) < 0)
  {
    LOG_MSG_ERROR("Error: Cannot send NA on bridge Iface error: %d.", errno, 0, 0);
  }

  ds_free(packet_buff_cpy);
  ds_log_med("Proxy NA sent");
}


/*===========================================================================
  FUNCTION AddV6ExternalRoute
==========================================================================*/
/*!
@brief
  Add destination based route.

@parameters
  nl_addr
  iface

@return
  void

@note

 - Dependencies: AP-STA/Cradle v6 router mode
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::AddV6ExternalRoute(qcmap_nl_addr_t* nl_addr, char* iface)
{
  char external_ip[INET6_ADDRSTRLEN];
  char prefix_str[INET6_ADDRSTRLEN];
  char command[MAX_COMMAND_STR_LEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t *node_data = NULL;
  struct ps_in6_addr *dst_addr_ptr = NULL;
  struct sockaddr_in6 multi_dst;
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList;

  if (QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("Backhaul is not enabled",0,0,0);
    return;
  }

  inet_pton(AF_INET6, "ff02::1", &multi_dst.sin6_addr);
  if (memcmp(nl_addr->ipv6_dst_addr, &multi_dst.sin6_addr, sizeof(multi_dst.sin6_addr)) == 0)
  {
    LOG_MSG_ERROR("NA is multicast, do nothing", 0,0,0);
    return;
  }

  if (strncmp(iface, nl_addr->iface_name, strlen(iface)) != 0)
  {
    ds_log_med("AddV6ExternalRoute: Error NA is not on correct iface %s %s", iface, nl_addr->iface_name);
    return;
  }

  qcmap_addr_info_list_t* cdi_addrList = &(QcMapMgr->addrList);
  if(cdi_addrList->addrListHead != NULL)
  {
    qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
    struct ps_in6_addr *addr_ptr = NULL;
    ds_dll_el_t *dst_node = NULL;
    addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;
    tempPrefixIidptr.iid_ptr = addr_ptr;
    dst_node = ds_dll_search (cdi_addrList->addrListHead , (void*)&tempPrefixIidptr, QcMapMgr->qcmap_match_v6_iid);
    if (dst_node != NULL)
    {
      LOG_MSG_ERROR("NA is from internal client, do not add route", 0,0,0);
      return;
    }
  }

  /* In Dual Wifi WLAN0 is not always the STA iface */
  snprintf(sta_iface, QCMAP_MSGR_INTF_LEN, "wlan%d", QcMapMgr->sta_iface_index);

  if ((strncmp(iface, sta_iface, strlen(sta_iface)) == 0 && (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)) ||
      (strncmp(iface, ECM_IFACE, strlen(ECM_IFACE)) == 0 && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE)||
      (strncmp(iface, ETH_IFACE, strlen(ETH_IFACE)) == 0 && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)||
      (strncmp(iface, BT_IFACE, strlen(BT_IFACE)) == 0 && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT))
  {
    if (memcmp(&(((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr),
               nl_addr->ip_v6_addr, sizeof(nl_addr->ip_v6_addr)/2) != 0)
    {
      inet_ntop(AF_INET6, &(nl_addr->ip_v6_addr), external_ip, INET6_ADDRSTRLEN);
      inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr,
                prefix_str, INET6_ADDRSTRLEN);
      ds_log_med("AddV6ExternalRoute: error invalid prefix %s, %s",external_ip,prefix_str);
      return;
    }

    addrList = &(QcMapBackhaulMgr->external_client_addr_list);

    /* Add IP to linked list for cleanup */
    if (addrList->addrListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
        information. This is done to make use of the doubly linked list framework which
        is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for node",0,0,0);
        return;
      }
      addrList->addrListHead = node;
    }

    node_data = (qcmap_cm_client_data_info_t*)ds_malloc(sizeof(qcmap_cm_client_data_info_t));
    if( node_data == NULL )
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in allocating memory for"
                    "device information structure",0,0,0);
      return;
    }
    memset(node_data,0,sizeof(qcmap_cm_client_data_info_t));
    memcpy(node_data->ipv6[0].addr, nl_addr->ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    dst_addr_ptr = (struct ps_in6_addr *)nl_addr->ip_v6_addr;
    tempPrefixIidptr.iid_ptr = dst_addr_ptr;

    //check if entry already exists
    if((node = ds_dll_search (addrList->addrListHead , (void*) &tempPrefixIidptr,
                              QcMapMgr->qcmap_match_v6_iid)) != NULL)
    {
      LOG_MSG_ERROR("Entry already exists, do not add",0,0,0);
      ds_free(node_data);
      node_data = NULL;
      return;
    }
    //Store the src address
    else if ((node = ds_dll_enq(addrList->addrListHead,
                           NULL, (void*)node_data )) == NULL)
    {
      LOG_MSG_ERROR("AddNewDeviceEntry - Error in adding a node",0,0,0);
      ds_free(node_data);
      node_data = NULL;
      return;
    }
    //Update the tail pointer when a new node is added everytime
    addrList->addrListTail = node;

    inet_ntop(AF_INET6, &(nl_addr->ip_v6_addr), external_ip, INET6_ADDRSTRLEN);
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route add %s dev %s",external_ip, iface);
    ds_system_call(command, strlen(command));

    ds_log_med("New destination route for %s", external_ip);
  }
  else
  {
    ds_log_med("Incorrect iface %s for backhaul",iface);
  }
}


/*===========================================================================
  FUNCTION DeleteV6ExternalRoute
==========================================================================*/
/*!
@brief
  Delete destination based route.

@parameters
  nl_addr
  iface

@return
  void

@note

 - Dependencies: AP-STA/Cradle v6 router mode
*/
/*=========================================================================*/
void QCMAP_Backhaul::DeleteV6ExternalRoute(uint8_t* v6_addr, char* iface)
{
  char command[MAX_COMMAND_STR_LEN];
  char v6_ip_addr[INET6_ADDRSTRLEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t *node_data = NULL;
  struct ps_in6_addr *dst_addr_ptr = NULL;
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList;

  if (QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("Backhaul is not enabled",0,0,0);
    return;
  }


  snprintf(sta_iface, QCMAP_MSGR_INTF_LEN, "wlan%d", QcMapMgr->sta_iface_index);
  if ((strncmp(iface, sta_iface, strlen(sta_iface)) == 0 &&
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER) ||
      (strncmp(iface, ECM_IFACE, strlen(ECM_IFACE)) == 0 && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE) ||
      (strncmp(iface, ECM_IFACE, strlen(ETH_IFACE)) == 0 && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)||
      (strncmp(iface, BT_IFACE, strlen(BT_IFACE)) == 0 && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT))
  {
    addrList = &(QcMapBackhaulMgr->external_client_addr_list);

    dst_addr_ptr = (struct ps_in6_addr *)v6_addr;
    tempPrefixIidptr.iid_ptr = dst_addr_ptr;

    /* Delete the route */
    inet_ntop(AF_INET6, v6_addr, v6_ip_addr, INET6_ADDRSTRLEN);

    //Delete the node from the list
    if ((node = ds_dll_delete( addrList->addrListHead, &(addrList->addrListTail),
            (void*)&tempPrefixIidptr, QcMapMgr->qcmap_match_v6_addr))== NULL)
    {
      ds_log_med("DeleteDeviceEntryInfo - Error in finding the node %s on %s",v6_ip_addr,iface);
      return false;
    }

    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del %s dev %s",v6_ip_addr, iface);
    ds_system_call(command, strlen(command));

    node_data = (qcmap_cm_client_data_info_t*)ds_dll_data(node);

    if (node_data == NULL)
    {
      LOG_MSG_ERROR("node is NULL",0,0,0);
      //Free the memory of the linked list node
      ds_dll_free(node);
      node = NULL;
      return false;
    }

    //Free the device information structure
    ds_free (node_data);
    node_data = NULL;

    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;

    // Add logic to check if there are no more nodes in the list and free the head
    if ( addrList->addrListHead->next == NULL)
    {
      LOG_MSG_INFO1("No device info exist",0,0,0);
      ds_dll_free(addrList->addrListHead);
      addrList->addrListHead = NULL;
    }
  }
  else
  {
    ds_log_med("Incorrect iface %s for backhaul",iface);
  }
}

/*===========================================================================
  FUNCTION DeleteAllV6ExternalRoute
==========================================================================*/
/*!
@brief
  Remove all destination based routes.

@parameters
  iface

@return
  void

@note

 - Dependencies: AP-STA/Cradle v6 router mode
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul::DeleteAllV6ExternalRoute(char* iface)
{
  char command[MAX_COMMAND_STR_LEN];
  char v6_ip_addr[INET6_ADDRSTRLEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  ds_dll_el_t *node, *temp_node;
  qcmap_cm_client_data_info_t *node_data = NULL, *temp_data;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList;

  if (QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("Backhaul is not enabled",0,0,0);
    return;
  }

  if (iface == NULL)
  {
    LOG_MSG_ERROR("NULL interface pointer",0,0,0);
    return;
  }

  LOG_MSG_INFO1("DeleteAllV6ExternalRoute: delete all routes",0,0,0);
  snprintf(sta_iface, QCMAP_MSGR_INTF_LEN, "wlan%d", QcMapMgr->sta_iface_index);

  if ((strncmp(iface, sta_iface, strlen(sta_iface)) == 0 &&
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER) ||
      (strncmp(iface, ECM_IFACE, strlen(ECM_IFACE)) == 0 &&
       QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE))
  {

    addrList = &(QcMapBackhaulMgr->external_client_addr_list);

    if (addrList->addrListHead == NULL)
    {
      LOG_MSG_ERROR("Error: List is empty", 0, 0, 0);
      return false;
    }

    node = addrList->addrListHead;
    node = ds_dll_next (node, (const void**)(&node_data));

    while (node != NULL)
    {
      /* Delete the route */
      inet_ntop(AF_INET6, &(node_data->ipv6[0].addr), v6_ip_addr, INET6_ADDRSTRLEN);
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del %s dev %s",v6_ip_addr, iface);
      ds_system_call(command, strlen(command));

      temp_node = ds_dll_next (node, (const void**)(&temp_data));

      /* Delete node from list */
      ds_dll_delete_node(addrList->addrListHead, &(addrList->addrListTail), node);

      //Free the proxyInfo entry structure
      if (node_data != NULL)
      {
        ds_free (node_data);
        node_data = NULL;
      }

      //Free the memory of the linked list node
      ds_dll_free(node);

      if (addrList->addrListHead->next == NULL)
      {
        LOG_MSG_INFO1("No more entries",0,0,0);
        ds_dll_free(addrList->addrListHead);
        addrList->addrListHead = NULL;
        break;
      }
      node = temp_node;
      node_data = temp_data;
    }
  }
  else
  {
    ds_log_med("Incorrect iface %s for backhaul",iface);
  }
}

/*==========================================================================
  FUNCTION JoinMulticastGroup
==========================================================================*/
/*!
@brief
 Add iface to the Multicast group

@parameters
  iface
/*=========================================================================*/
void QCMAP_Backhaul::JoinMulticastGroup
(
 qcmap_v6_sock_iface_t iface_type,
 char* iface
 )
{
  static int v6sock_bridge = -1, v6sock_ecm = -1, v6sock_wlan = -1;
  static int v6sock_eth = -1, iface_name_sockfd;
  struct ifreq req_if_info;
  struct ipv6_mreq mreq;
  static struct in6_addr all_routers_in6_addr;
  int *curr_v6_sockfd = NULL;

  if (iface_type < QCMAP_V6_SOCK_BRIDGE || iface_type > QCMAP_V6_SOCK_ECM)
  {
    LOG_MSG_ERROR( "Unsupported iface_type = %d", iface_type, 0, 0);
    return;
  }

  iface_name_sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
  if (iface_name_sockfd < 0)
  {
    LOG_MSG_ERROR("Open iface_name_sockfd data sock error = %d",
                  iface_name_sockfd, 0, 0);
    return;
  }

  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, iface, sizeof(req_if_info.ifr_name));

  if (ioctl(iface_name_sockfd, SIOCGIFINDEX, &req_if_info) < 0 )
  {
    LOG_MSG_ERROR("Couldn't get index. err %d", errno, 0, 0);
    close( iface_name_sockfd );
    return;
  }

  close( iface_name_sockfd );

  if (iface_type == QCMAP_V6_SOCK_BRIDGE)
  {
    curr_v6_sockfd = &v6sock_bridge;
  }
  else if (iface_type == QCMAP_V6_SOCK_WLAN)
  {
    curr_v6_sockfd = &v6sock_wlan;
  }
  else if (iface_type == QCMAP_V6_SOCK_ETH)
  {
    curr_v6_sockfd = &v6sock_eth;
  }
  else
  {
    curr_v6_sockfd = &v6sock_ecm;
  }

  if (*curr_v6_sockfd >=0)
  {
    close(*curr_v6_sockfd);
    *curr_v6_sockfd = -1;
  }

  *curr_v6_sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_RAW);
  if (*curr_v6_sockfd < 0)
  {
    LOG_MSG_ERROR("Error creating raw socket %d",*curr_v6_sockfd, 0, 0);
    return;
  }

  inet_pton(AF_INET6, MULTICAST_ADDR, &all_routers_in6_addr);

  memset(&mreq, 0, sizeof(mreq));
  mreq.ipv6mr_multiaddr = all_routers_in6_addr;
  mreq.ipv6mr_interface = req_if_info.ifr_ifindex;

  if (setsockopt(*curr_v6_sockfd,
                 IPPROTO_IPV6,
                 IPV6_ADD_MEMBERSHIP,
                 &mreq,
                 sizeof(mreq)) < 0)
  {
    LOG_MSG_ERROR("Error joining MC group",0,0,0);
  }
}

/*===========================================================================
  FUNCTION SwitchToWWANBackhaul
==========================================================================*/
/*!
@brief
  Switches current backhaul to WWAN

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
  */
/*=========================================================================*/
void QCMAP_Backhaul::SwitchToWWANBackhaul()
{
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
  unsigned int num_entries = 0;

  if(NULL == QcMapBackhaulWWANMgr || QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("BackhaulWWAN is NULL or LAN Object is NULL", 0, 0, 0);
    return;
  }

  if (QcMapMgr && QcMapMgr->packet_stats_enabled)
  {
    //Reset IPA stats when switching backhaul to WWAN
    if (QcMapMgr->TraverseCDIAndUpdate(QCMAP_RESET_IPA_CLIENT_DATA,
                                              NULL,
                                              &num_entries))
    {
      LOG_MSG_INFO1("RESET IPA succeeded, entries: %d\n",
                    num_entries,0,0);
    }
    else
    {
      LOG_MSG_ERROR("No Connected Devices Information available currently",
                    0,0,0);
    }
  }

  if (NULL != QcMapBackhaulMgr)
    QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_WWAN;

    QcMapBackhaulWWANMgr->AddWWANIPv4SIPServerInfo();

  if ( QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED )
  {
    if(QcMapNatAlg)
      QcMapNatAlg->EnableNATonApps();

    if (QCMAPLANMgr)
    {
      QCMAPLANMgr->EnableDNS();

      //Get IP Passthrough Configuration from XML
      if(QCMAPLANMgr->GetIPPassthroughConfig
          (&enable_state,&passthrough_config,&qmi_err_num))
      {
        if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_UP_V01)
        {
          QCMAPLANMgr->RestartTetheredClient(passthrough_config.device_type);
        }
      }
      else
      {
        LOG_MSG_ERROR("Get IP Passthrough Flag failed",0,0,0);
      }
    }
  }
  if ( QcMapBackhaulWWANMgr->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED )
  {
    if(QcMapBackhaulMgr)
    {
      QcMapBackhaulMgr->EnableIPV6Forwarding();
    }

    if(QcMapFirewall)
    {
      QcMapFirewall->EnableIPV6Firewall();
    }

    if(QCMAPLANMgr &&
      (QCMAP_MSGR_DHCPV6_MODE_UP_V01 != QcMapBackhaulWWANMgr->dhcpv6_dns_conf.dhcpv6_enable_state)&&
      (QcMapBackhaulWWANMgr->GetProfileHandle() == QCMAP_Backhaul::GetDefaultProfileHandle()) &&
      (QcMapBackhaulWWANMgr->GetProfileHandle() != 0))
    {
      QCMAPLANMgr->AddDNSNameServers(QcMapBackhaulWWANMgr->pri_dns_ipv6_addr,
                                     QcMapBackhaulWWANMgr->sec_dns_ipv6_addr);
    }
  }
  /* Disable IPv4 MCAST */
  LOG_MSG_INFO1(" Stop PIMD to disable MCAST forwarding on STA interface.",0,0,0);
  if (QcMapMgr)
    QcMapMgr->StopMcastDaemon();

  if (QcMapMgr && QcMapMgr->IsDynamicClockVotingNeeded())
  {
    if(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
    {
      if((QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED ||
          QcMapBackhaulWWANMgr->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED) &&
          QcMapMgr->IsRatLteFdd() && QcMapMgr->isBWLteCAT4() &&
          !(QcMapMgr->CheckUsbClockAtNom()))
      {
        QcMapMgr->SetUSBClockFreq(true);
      }
    }
  }
}

/*===========================================================================
  FUNCTION AddIPv6DefaultRouteInfo
==========================================================================*/
/*!
@brief
  Cache default gateway information from RA received.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
/*=========================================================================*/
void QCMAP_Backhaul::AddIPv6DefaultRouteInfo(qcmap_nl_addr_t *nladdr)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                  GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  char ip6_addr[INET6_ADDRSTRLEN];
  char sta_iface[QCMAP_MSGR_INTF_LEN];
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  qcmap_msgr_ethernet_mode_v01 eth_mode=QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01;
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  qmi_error_type_v01 qmi_err_num;
  qcmap_msgr_bt_tethering_status_enum_v01 bt_teth_status;
  qcmap_bt_tethering_mode_enum_v01 bt_teth_mode;

  inet_ntop(AF_INET6, &nladdr->ipv6_src_addr, ip6_addr, INET6_ADDRSTRLEN);
  LOG_MSG_INFO1("QCMAP_Backhaul::AddIPv6DefaultRouteInfo iface %s gateway %s",
               nladdr->iface_name, ip6_addr,0);

  snprintf(sta_iface, QCMAP_MSGR_INTF_LEN, "wlan%d", QcMapMgr->sta_iface_index);

  if ((strncmp(nladdr->iface_name, sta_iface, strlen(sta_iface)) == 0) &&
      QcMapBackhaulWLANMgr &&
      !QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if (!memcmp(&QcMapBackhaulWLANMgr->ipv6_gateway_addr.s6_addr,
          &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr)))
    {
      ds_log_med("AddIPv6DefaultRouteInfo: Duplicate RA receieved");
      return;
    }

    memcpy(&QcMapBackhaulWLANMgr->ipv6_gateway_addr.s6_addr,
             &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr));
  }
  else if ((strncmp(nladdr->iface_name, ECM_IFACE, strlen(ECM_IFACE)) == 0) &&
            QcMapBackhaulCradleMgr &&
            !QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
  {
    QcMapBackhaulCradleMgr->GetCradleMode(&cradle_mode,&qmi_err_num);
    if (cradle_mode != QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
    {
      LOG_MSG_INFO1("Ignoring RA - we're not in Cradle WAN Router mode current mode %d",
                     cradle_mode,0,0);
      return;
    }

    if (!memcmp(&QcMapBackhaulCradleMgr->ipv6_gateway_addr.s6_addr,
                  &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr)))
    {
      ds_log_med("AddIPv6DefaultRouteInfo: Duplicate RA receieved");
      return;
    }

    memcpy(&QcMapBackhaulCradleMgr->ipv6_gateway_addr.s6_addr,
             &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr));
  }
  else if ((strncmp(nladdr->iface_name, ETH_IFACE, strlen(ETH_IFACE)) == 0) &&
            QcMapBackhaulEthMgr)
  {
    QcMapBackhaulEthMgr->GetEthBackhaulMode(&eth_mode,&qmi_err_num);
    if (eth_mode != QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
    {
      LOG_MSG_INFO1("Ignoring RA - we're not in ETH Backhaul"
                    " WAN Router mode current mode %d",
                     eth_mode,0,0);
      return;
    }

    if (!memcmp(&QcMapBackhaulEthMgr->ipv6_gateway_addr.s6_addr,
                  &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr)))
    {
      ds_log_med("AddIPv6DefaultRouteInfo: Duplicate RA receieved");
      return;
    }
    LOG_MSG_INFO1("Store %d for eth backhaul GW addr",
                     &nladdr->ipv6_src_addr,0,0);
    memcpy(&QcMapBackhaulEthMgr->ipv6_gateway_addr.s6_addr,
             &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr));
  }
  else if ((strncmp(nladdr->iface_name, BT_IFACE, strlen(BT_IFACE)) == 0) &&
            QcMapBTTethMgr)
  {
    QcMapBTTethMgr->GetBTTetheringStatus(&bt_teth_status,&bt_teth_mode,&qmi_err_num);
    if (bt_teth_mode != QCMAP_MSGR_BT_MODE_WAN_V01)
    {
      LOG_MSG_INFO1("Ignoring RA - we're not in BT Backhaul"
                    " Mode current mode %d",
                     bt_teth_mode,0,0);
      return;
    }

    if (!memcmp(&QcMapBTTethMgr->ipv6_gateway_addr.s6_addr,
                  &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr)))
    {
      ds_log_med("AddIPv6DefaultRouteInfo: Duplicate RA receieved");
      return;
    }
    LOG_MSG_INFO1("Store %d for BT backhaul GW addr",
                     &nladdr->ipv6_src_addr,0,0);
    memcpy(&QcMapBTTethMgr->ipv6_gateway_addr.s6_addr,
             &nladdr->ipv6_src_addr, sizeof(nladdr->ipv6_src_addr));
  }
  else
  {
    ds_log_med("QCMAP_Backhaul::AddIPv6DefaultRouteInfo -Rcvd RA in incorrect mode %d",
                 QCMAP_Backhaul::current_backhaul);
  }
}

/*=====================================================
  FUNCTION SwitchToOtherBackhaul
======================================================*/
/*!
@brief
 Switches to other backhaul from the specified backhaul.

@parameters
  qcmap_backhaul_type backhaul_type
  boolean allow_ipv4
  boolean allow_ipv6

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Backhaul::SwitchToOtherBackhaul
(
  qcmap_backhaul_type backhaul_type,
  boolean allow_ipv4,
  boolean allow_ipv6
 )
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  qcmap_backhaul_list_type backhaul_prio_list[MAX_BACKAUL_COUNT];
  qmi_error_type_v01 qmi_err_num;
  int index = 0;
  /*------------------------------------------------------------------------*/
  /* This needs to be optimized by storing the prio list in array in
     backhaul class*/
  backhaul_prio_list[0].backhaul_name = QCMAP_Backhaul::preffered_backhaul_first;
  backhaul_prio_list[1].backhaul_name = QCMAP_Backhaul::preffered_backhaul_second;
  backhaul_prio_list[2].backhaul_name = QCMAP_Backhaul::preffered_backhaul_third;
  backhaul_prio_list[3].backhaul_name = QCMAP_Backhaul::preffered_backhaul_fourth;
  backhaul_prio_list[4].backhaul_name = QCMAP_Backhaul::preffered_backhaul_fifth;


  for (int i =0; i < MAX_BACKAUL_COUNT; i++)
  {
    switch(backhaul_prio_list[i].backhaul_name)
    {
      case BACKHAUL_TYPE_WWAN:
      if(QcMapBackhaulMgr && QcMapBackhaulMgr->QcMapBackhaulWWAN)
      {
        backhaul_prio_list[i].backhaul_availale =
          (allow_ipv4 &&
               QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED) ||
          (allow_ipv6 &&
               QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED);
      }
      else
      {
        LOG_MSG_ERROR("QcMapBackhaulMgr is NULL \n", 0, 0, 0);
      }
        break;
      case BACKHAUL_TYPE_AP_STA_ROUTER:
        backhaul_prio_list[i].backhaul_availale =
          (allow_ipv4 && QCMAP_Backhaul_WLAN::IsSTAAvailableV4()) ||
          (allow_ipv6 && QCMAP_Backhaul_WLAN::IsSTAAvailableV6());
        break;

      case BACKHAUL_TYPE_CRADLE:
        backhaul_prio_list[i].backhaul_availale =
          (allow_ipv4 && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4()) ||
          (allow_ipv6 && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6());
        break;
      case BACKHAUL_TYPE_ETHERNET:
        backhaul_prio_list[i].backhaul_availale =
          (allow_ipv4 && QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4()) ||
          (allow_ipv6 && QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6());
        break;

      case BACKHAUL_TYPE_BT:
        backhaul_prio_list[i].backhaul_availale =
          (allow_ipv4 && QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4()) ||
          (allow_ipv6 && QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6());
        break;
      default:
        LOG_MSG_ERROR("SwitchToOtherBackhaul:"
            "Invalid backhaul name.",0,0,0);
        return;
        break;
    }
  }

  while (index != MAX_BACKAUL_COUNT)
  {
    if (backhaul_type != backhaul_prio_list[index].backhaul_name)
    {
      if (backhaul_prio_list[index].backhaul_availale)
      {
        LOG_MSG_ERROR("SwitchToOtherBackhaul:Switch to %d backhaul",
                      backhaul_prio_list[index].backhaul_name,0,0);

        QCMAP_Backhaul::SwitchToBackhaul(
                            backhaul_prio_list[index].backhaul_name);
        return;
      }
    }
    index++;
  }
  LOG_MSG_ERROR("SwitchToOtherBackhaul:No Available Backhaul",0,0,0);
  QCMAP_Backhaul::current_backhaul = NO_BACKHAUL;
  return;
}
/*=====================================================
  FUNCTION SwitchToBackhaul
======================================================*/
/*!
@brief
 Switches to the specified backhaul.

@parameters
  qcmap_backhaul_type backhaul_type

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
boolean QCMAP_Backhaul::SwitchToBackhaul(qcmap_backhaul_type backhaul_type)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  boolean return_value = false;
  /*------------------------------------------------------------------------*/
  LOG_MSG_ERROR("SwitchToBackhaul:"
                  "Switch to %d backhaul_type.",backhaul_type,0,0);
  switch(backhaul_type)
  {
    case BACKHAUL_TYPE_WWAN:
      if (QcMapBackhaulMgr)
      {
        QcMapBackhaulMgr->SwitchToWWANBackhaul();
        return_value = true;
      }
      break;
    case BACKHAUL_TYPE_AP_STA_BRIDGE:
    case BACKHAUL_TYPE_AP_STA_ROUTER:
      if (QcMapBackhaulWLANMgr) {
        QcMapBackhaulWLANMgr->SwitchToAPSTABackhaul();
        return_value = true;
      }
      break;
    case BACKHAUL_TYPE_CRADLE:
      if(QcMapBackhaulCradleMgr)
      {
        QcMapBackhaulCradleMgr->SwitchToCradleBackhaul();
        return_value = true;
      }
      break;
    case BACKHAUL_TYPE_ETHERNET:
      if (QcMapBackhaulEthMgr)
      {
        QcMapBackhaulEthMgr->SwitchToEthBackhaul();
        return_value = true;
      }
      break;
    case BACKHAUL_TYPE_BT:
      if (QcMapBTTethMgr)
      {
        QcMapBTTethMgr->SwitchToBTBackhaul();
        return_value = true;
      }
      break;
    default:
      LOG_MSG_ERROR("SwitchToOtherBackhaul:"
          "Invalid backhaul name.",0,0,0);
      break;
  }
  return return_value;
}

/*===========================================================================
  FUNCTION AddIPv6PrefixBasedRules
==========================================================================*/
/*!
@brief
  Add IPv6 prefix based rules

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
/*=========================================================================*/
void QCMAP_Backhaul::AddIPv6PrefixBasedRules(int ap_dev_num)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];

  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr* is Invalid",0,0,0);
    return;
  }
  /* Get new IPv6 Address  */
  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);
  inet_ntop(AF_INET6,((struct sockaddr_in6 *)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr, ip6_addr, MAX_IPV6_PREFIX );
  snprintf( command, MAX_COMMAND_STR_LEN,
           "ebtables -A INPUT -i wlan%d -p IPv6 --ip6-destination %s/%d -j DROP",
           ap_dev_num,ip6_addr,QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,
           "ebtables -A OUTPUT -o wlan%d -p IPv6 --ip6-source %s/%d -j DROP",
           ap_dev_num,ip6_addr,QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
  ds_system_call( command, strlen(command));
}

/*===========================================================================
  FUNCTION RemoveIPv6PrefixBasedRules
==========================================================================*/
/*!
@brief
  Remove IPv6 prefix based rules

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
/*=========================================================================*/
void QCMAP_Backhaul::RemoveIPv6PrefixBasedRules(int ap_dev_num)
{
  char command[MAX_COMMAND_STR_LEN];
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];
  /* Get new IPv6 Address  */
  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);
  inet_ntop(AF_INET6,((struct sockaddr_in6 *)&(ipv6_prefix_info.prefix_addr))\
               ->sin6_addr.s6_addr, ip6_addr, MAX_IPV6_PREFIX);
  snprintf( command, MAX_COMMAND_STR_LEN,
            "ebtables -D INPUT -i wlan%d -p IPv6 --ip6-destination %s/%d -j DROP",
             ap_dev_num,ip6_addr,ipv6_prefix_info.prefix_len);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,
            "ebtables -D OUTPUT -o wlan%d -p IPv6 --ip6-source %s/%d -j DROP",
               ap_dev_num,ip6_addr,ipv6_prefix_info.prefix_len);
  ds_system_call( command, strlen(command));
}


/*=====================================================
  FUNCTION GetBackhaulStatus
======================================================*/
/*!
@brief
 Gets current active backhaul status, type and IP Version if connected.

@parameters
  - Pointer to qcmap_msgr_backhaul_type_enum_v01
  - boolean - backhaul IPV4 availablity
  - boolean - backhaul IPV6 availablity
  - Pointer to qmi_error_type_v01

@return
  int
  0  - On Success
  -1 - On Failure
@notes
- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
int QCMAP_Backhaul::GetBackhaulStatus
(
  qcmap_cm_backhaul_status *backhaul_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_CM_LOG_FUNC_ENTRY();

  if ((backhaul_status == NULL))
  {
    LOG_MSG_ERROR("GetBackhaulStatus Failed: Invalid Args", 0,0,0);
    return QCMAP_CM_ERROR;
  }
/*Intialize backhaul available params to false*/
  backhaul_status->backhaul_v4_available = BACKHAUL_NOT_AVAILABLE;
  backhaul_status->backhaul_v6_available = BACKHAUL_NOT_AVAILABLE;

  if (!QCMAP_Backhaul::current_backhaul)
    return QCMAP_CM_SUCCESS;
  else
  {
    switch (QCMAP_Backhaul::current_backhaul)
    {
      case BACKHAUL_TYPE_AP_STA_BRIDGE:
      case BACKHAUL_TYPE_AP_STA_ROUTER:
        backhaul_status->backhaul_type = QCMAP_MSGR_WLAN_BACKHAUL_V01;
        if (QCMAP_Backhaul_WLAN::IsSTAAvailableV4())
          backhaul_status->backhaul_v4_available = BACKHAUL_AVAILABLE;
        if (QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
          backhaul_status->backhaul_v6_available = BACKHAUL_AVAILABLE;
        break;

      case BACKHAUL_TYPE_CRADLE:
        backhaul_status->backhaul_type = QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01;
        if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4())
          backhaul_status->backhaul_v4_available = BACKHAUL_AVAILABLE;
        if (QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
          backhaul_status->backhaul_v6_available = BACKHAUL_AVAILABLE;
        break;
      case BACKHAUL_TYPE_WWAN:
        backhaul_status->backhaul_type = QCMAP_MSGR_WWAN_BACKHAUL_V01;
        if (QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)
          backhaul_status->backhaul_v4_available = BACKHAUL_AVAILABLE;
        if (QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)
          backhaul_status->backhaul_v6_available = BACKHAUL_AVAILABLE;
        break;
      case BACKHAUL_TYPE_ETHERNET:
        backhaul_status->backhaul_type = QCMAP_MSGR_ETHERNET_BACKHAUL_V01;
        if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
          backhaul_status->backhaul_v4_available = BACKHAUL_AVAILABLE;
        if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
          backhaul_status->backhaul_v6_available = BACKHAUL_AVAILABLE;
        break;

      case BACKHAUL_TYPE_BT:
        backhaul_status->backhaul_type = QCMAP_MSGR_BT_BACKHAUL_V01;
        if (QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4())
          backhaul_status->backhaul_v4_available = BACKHAUL_AVAILABLE;
        if (QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6())
          backhaul_status->backhaul_v6_available = BACKHAUL_AVAILABLE;
        break;

      default:
        LOG_MSG_ERROR("Invalid Backhaul", 0, 0, 0);
        return QCMAP_CM_ERROR;
        break;
    }
  }
  return QCMAP_CM_SUCCESS;
}


/*=====================================================
  FUNCTION GetDeviceName
======================================================*/
/*!
@brief
 Returns current active backhaul

@parameters
  - devname
  - IP family
  - Pointer to qmi_error_type_v01

@return
  int
@note
  Use the return values to indicate the type of backhaul being used.

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
int QCMAP_Backhaul::GetDeviceName
(
  char *devname,
  qcmap_msgr_ip_family_enum_v01 family_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_errno;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

  if( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if ( QcMapMgr && QcMapBackhaulWWAN && QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle, family_type, devname, &qcmap_cm_errno)
        != QCMAP_CM_SUCCESS )
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d", 0,0,0);
      *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
      return NO_BACKHAUL;
    }
    return BACKHAUL_TYPE_WWAN;
  }
  else if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
  {
    strlcpy(devname, BT_IFACE, strlen(BT_IFACE)+1);
    return BACKHAUL_TYPE_BT;
  }
  else if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    strlcpy(devname, ETH_IFACE, strlen(ETH_IFACE)+1);
    return BACKHAUL_TYPE_ETHERNET;
  }
  else if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
      strlcpy(devname, BRIDGE_IFACE, strlen(BRIDGE_IFACE)+1);
    else
      strlcpy(devname, ECM_IFACE, QCMAP_MSGR_INTF_LEN);

    return BACKHAUL_TYPE_CRADLE;
  }
  else if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE &&
           QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    strlcpy(devname, BRIDGE_IFACE, strlen(BRIDGE_IFACE)+1);
    return BACKHAUL_TYPE_AP_STA_BRIDGE;
  }
  else if (QcMapBackhaulWLANMgr &&
           (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER))
  {
    strlcpy( devname, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,\
             QCMAP_MSGR_INTF_LEN);
    return BACKHAUL_TYPE_AP_STA_ROUTER;
  }

  return NO_BACKHAUL;
}


/*=====================================================
  FUNCTION IsBackhaulRouterModeEnabled
======================================================*/
/*!
@brief
 Returns true if the current backhaul enabled needs Firewall/NAT.
 AKA: the backhaul is active and in router mode, not bridge mode

@parameters

@return
  - boolean

@note
  Use the return value to indicate if router mode is enabled.

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_Backhaul::IsBackhaulRouterModeEnabled()
{
  if (IS_DEFAULT_PROFILE(profileHandle))
  {
    /* Current_backhaul can be used for default PDN since it is a
       static variable */
    if(QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() ||
       QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() ||
       QCMAP_Backhaul::current_backhaul == NO_BACKHAUL)
    {
      return false;
    }
    return true;
  }
  /* Secondary PDN */
  /* Router mode is only enabled if v4 or v6 is connected, and VLAN is up */
  else if ((QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED ||
            QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED) &&
           QCMAP_Virtual_LAN::IsVLANIDUp(vlan_id, NULL))
  {
    /* For secondary PDNs we need to directly get the state of the PDN */
    return true;
  }
  return false;
}


/*=====================================================
  FUNCTION GetPdnRootXML
======================================================*/
/*!
@brief
 Returns the root XML node for the for the correct PDN

@parameters

@return
  - pugi::xml_node

@note
  Use the return value to indicate if router mode is enabled.

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
pugi::xml_node QCMAP_Backhaul::GetPdnRootXML(pugi::xml_document *xml_file)
{
  pugi::xml_node root, child, subchild;

  if (profileHandle == QCMAP_Backhaul::defaultProfileHandle)
  {
    root = xml_file->child(System_TAG).child(MobileAPCfg_TAG);
  }
  else
  {
    /* Secondary PDN Config */
    root = xml_file->child(System_TAG).child(ConcurrentMobileAPCfg_TAG);

    /*search through the MobileAPFirewallCfg_TAG's for the firewall
      corresponding to the correct PDN */
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if(atoi(child.child(MobileAPWanCfg_TAG).child(ProfileHandle_TAG).child_value()) == profileHandle)
        break;
    }

    if(child == NULL)
    {
      LOG_MSG_ERROR("Unable to find firewall corresponding to PDN",0,0,0);
      return child;
    }
    root = child;
  }

  return root;
}


/*=====================================================
  FUNCTION SwapNatAlgObjectWithDefault
======================================================*/
/*!
@brief
 Swap NatAlg reference in default and concurrent
 Backhaul object.

@parameters

@return
  - None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_Backhaul::SwapNatAlgObjectWithDefault()
{
  QCMAP_Backhaul          *pBackhaulDefault;
  QCMAP_NATALG            *pNatAlgDefault;

  pBackhaulDefault = GET_BACKHAUL_OBJECT(defaultProfileHandle);
  pNatAlgDefault = GET_NATALG_OBJECT(defaultProfileHandle);

  if (pBackhaulDefault == NULL || pNatAlgDefault == NULL ||
       pBackhaulDefault->QcMapNatAlg == NULL || QcMapNatAlg == NULL)
  {
    LOG_MSG_ERROR("Cannot swap, default Backhaul=%p, defaultALG=%p",
                    pBackhaulDefault, pNatAlgDefault, 0);
    return;
  }
  /* Swap Backhual reference in NatAlg Object */
  this->QcMapNatAlg->QcMapBackhaul = pBackhaulDefault;
  pBackhaulDefault->QcMapNatAlg->QcMapBackhaul = this;

  /* Swap NATALG Objects for default and Concurrent */
  pBackhaulDefault->QcMapNatAlg = this->QcMapNatAlg;
  QcMapNatAlg = pNatAlgDefault;
}


/*=====================================================
  FUNCTION SwapFirewallObjectWithDefault
======================================================*/
/*!
@brief
 Swap Firewall object reference in default and concurrent
 backahul object.

@parameters

@return
  - None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_Backhaul::SwapFirewallObjectWithDefault()
{
  QCMAP_Backhaul          *pBackhaulDefault;
  QCMAP_Firewall          *pFirewallDefault;

  pBackhaulDefault = GET_BACKHAUL_OBJECT(defaultProfileHandle);
  pFirewallDefault = GET_FIREWALL_OBJECT(defaultProfileHandle);

  if (pBackhaulDefault == NULL || pFirewallDefault == NULL ||
      pBackhaulDefault->QcMapFirewall == NULL || QcMapFirewall == NULL)
  {
    LOG_MSG_ERROR("Cannot swap, default Backhaul=%p, defaultFirewall=%p",
                    pBackhaulDefault, pFirewallDefault, 0);
    return;
  }

  /* Swap Backhual reference in Firewall Object */
  this->QcMapFirewall->QcMapBackhaul = pBackhaulDefault;
  pBackhaulDefault->QcMapFirewall->QcMapBackhaul = this;

  /* Swap Firewall Objects for default and Concurrent */
  pBackhaulDefault->QcMapFirewall = this->QcMapFirewall;
  QcMapFirewall = pFirewallDefault;
}

/*=====================================================
  FUNCTION SwapVlanIdWithDefault
======================================================*/
/*!
@brief
 Swap Vlan Id in default and concurrent backahul object.

@parameters

@return
  - None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_Backhaul::SwapVlanIdWithDefault()
{
  QCMAP_Backhaul          *pBackhaulDefault;

  pBackhaulDefault = GET_BACKHAUL_OBJECT(defaultProfileHandle);

  if (pBackhaulDefault == NULL)
  {
    LOG_MSG_ERROR("Cannot swap, default Backhaul=%p",
                    pBackhaulDefault, 0, 0);
    return;
  }

  /* Swap vlan_id, table_id of default and concurrent Object */
  pBackhaulDefault->vlan_id = vlan_id;

  this->vlan_id = QCMAP_MSGR_VLAN_ID_NONE_V01;
}
/*=====================================================
  FUNCTION SendBackhaulStatusInd
======================================================*/
/*!
@brief
 Send Backhaul Status Indication to clients if there is a change in backhaul availability,
 backhaul type and IP version.

@parameters
  - boolean backhaul_available,
  - qcmap_backhaul_type backhaul_type,
  - qcmap_msgr_ip_family_enum_v01 ip_family

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Backhaul::SendBackhaulStatusInd(
  int backhaul_v4_available,
  int backhaul_v6_available,
  qcmap_backhaul_type backhaul_type
)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;
  qcmap_cm_backhaul_status backhaul_status;

  LOG_MSG_INFO1("SendBackhaulStatusInd Enter \n", 0, 0, 0);

  if (backhaul_type == BACKHAUL_TYPE_WWAN)
  {
    if (QcMapBackhaulWWAN != GET_DEFAULT_BACKHAUL_WWAN_OBJECT())
      return;
  }

  backhaul_status.backhaul_v4_available = backhaul_v4_available;
  backhaul_status.backhaul_v6_available = backhaul_v6_available;

  switch (backhaul_type)
  {
    case BACKHAUL_TYPE_WWAN:
        backhaul_status.backhaul_type = QCMAP_MSGR_WWAN_BACKHAUL_V01;
        break;
    case BACKHAUL_TYPE_AP_STA_ROUTER:
    case BACKHAUL_TYPE_AP_STA_BRIDGE:
        backhaul_status.backhaul_type = QCMAP_MSGR_WLAN_BACKHAUL_V01;
        break;
    case BACKHAUL_TYPE_BT:
        backhaul_status.backhaul_type = QCMAP_MSGR_BT_BACKHAUL_V01;
        break;
    case BACKHAUL_TYPE_CRADLE:
        backhaul_status.backhaul_type = QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01;
        break;
    case BACKHAUL_TYPE_ETHERNET:
        backhaul_status.backhaul_type = QCMAP_MSGR_ETHERNET_BACKHAUL_V01;
        break;
    default:
        LOG_MSG_INFO1("SendBackhaulStatusInd: Invalid Backhaul",0,0,0);
        return;
        break;
  }

  cbPtr = (qmi_qcmap_msgr_status_cb_data *)QcMapMgr->GetQCMAPClientCBDatata();
  /* Send Backhaul Status IND to client */
  if (cbPtr && cbPtr->backhaul_status_cb)
    cbPtr->backhaul_status_cb(QcMapMgr->GetQCMAPClientCBDatata(), backhaul_status);
}

/*=====================================================
  FUNCTION SendPacketStatIndOnBackhaulSwitch
======================================================*/
/*!
@brief
 Send Packet Stat Indication to clients On Backhaul Switch.

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
void QCMAP_Backhaul::SendPacketStatIndOnBackhaulSwitch()
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  LOG_MSG_INFO1("SendPacketStatIndOnBackhaulSwitch Enter \n", 0, 0, 0);
  QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_BH_SWITCHED_V01,
                               QcMapMgr->conn_device_info.numOfNodes, NULL);

}

/*===========================================================================
  FUNCTION WritePMIPConfToXML
==========================================================================*/
/*!
@brief
  Write to XML from cache

@return
   true - sucess
   false - failure
/*=========================================================================*/
static boolean QCMAP_Backhaul::WritePMIPConfToXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child ;
  struct in_addr addr;
  uint8_t lma_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  char lma_v6_str[INET6_ADDRSTRLEN] = {0};

  boolean result=FALSE;
  const char *tag_ptr;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  LOG_MSG_INFO1("WritePMIPConfToXML.",0,0,0);

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PMIPV6_TAG);

  snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmip_debug_mode_enabled);
  root.child(EnablePMIPDebugmode).text() = data;


  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {

    snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled);
    root.child(EnablePMIPmode_TAG).text() = data;

    memset(&data, 0x0, MAX_STRING_LENGTH);

    snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmip_mode_type);
    root.child(PMIPmodeType_TAG).text() = data;

    snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode);
    root.child(PMIPTunnelType).text() = data;

    memset(&data, 0x0, MAX_STRING_LENGTH);
    if (QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip)
    {
      inet_ntop(AF_INET6, (void *)QCMAP_Backhaul::pmip_conf.LMA_v6_addr, lma_v6_str, INET6_ADDRSTRLEN);
                root.child(LMAv6Address_TAG).text() = lma_v6_str;
    }
    else if (QCMAP_Backhaul::pmip_conf.isValidLMAV4Ip)
    {
      addr.s_addr = QCMAP_Backhaul::pmip_conf.LMA_v4_addr;
      root.child(LMAv4Address_TAG).text() = inet_ntoa(addr);
    }
    else
    {
      LOG_MSG_ERROR("LMA v4/v6 ip write in XML failed for pmipv6 mode",0,0,0);
    }

  }
  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled);
    root.child(EnablePMIPmode_TAG).text() = data;

    memset(&data, 0x0, MAX_STRING_LENGTH);

    snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmip_mode_type);
    root.child(PMIPmodeType_TAG).text() = data;

    snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode);
    root.child(PMIPTunnelType).text() = data;

    memset(&data, 0x0, MAX_STRING_LENGTH);

    if (QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip)
    {
      inet_ntop(AF_INET6, (void *)QCMAP_Backhaul::pmip_conf.LMA_v6_addr, lma_v6_str, INET6_ADDRSTRLEN);
      root.child(LMAv6Address_TAG).text() = lma_v6_str;
    }
    else if (QCMAP_Backhaul::pmip_conf.isValidLMAV4Ip)
    {
      addr.s_addr = QCMAP_Backhaul::pmip_conf.LMA_v4_addr;
      root.child(LMAv4Address_TAG).text() = inet_ntoa(addr);
    }
    else
    {
      LOG_MSG_ERROR("LMA v4/v6 ip write in XML failed for pmipv4 mode",0,0,0);
    }

    root.child(PMIPV4WorkMode_TAG).text() = QCMAP_Backhaul::pmip_conf.pmipv4_work_mode;
    if (QCMAP_Backhaul::pmip_conf.pmipv4_work_mode == QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01)
    {
      LOG_MSG_INFO1("Pmipv4 mode is secondary router",0,0,0);
      addr.s_addr = QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.dmnp_prefix;
      root.child(PMIPV4SecRouterDMNPPrefix_TAG).text() = inet_ntoa(addr);
      snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.prefix_len);
      root.child(PMIPV4SecRouterdmnpPrefixLen_TAG).text() = data;
    }
  }

  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    memset(&data, 0x0, MAX_STRING_LENGTH);
    snprintf(data, sizeof(data), "%d", QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type);
    root.child(PMIPMobileNodeIdentifierType).text() = data;
    if (QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01)
    {
      LOG_MSG_INFO1("Store pmip mobile node identifier type string to xml",0,0,0);
      root.child(PMIPMobileNodeIdentifierString).text() = QCMAP_Backhaul::pmip_conf.pmip_mn_id_string;
    }

    LOG_MSG_INFO1("Store pmip service selection string to xml",0,0,0);
      root.child(PMIPServiceSelectionString).text() = QCMAP_Backhaul::pmip_conf.pmip_service_selection_string;

  }
  result = QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  LOG_MSG_INFO1("WritePMIPConfToXML: result:%d",result,0,0);

  return TRUE;
}


/*===========================================================================
  FUNCTION ReadPMIPConfFromXML
==========================================================================*/
/*!
@brief
  Read from XML to cahce

@return
   true - sucess
   false - failure
/*=========================================================================*/
boolean QCMAP_Backhaul::ReadPMIPConfFromXML(void)
{
  pugi::xml_document xml_file;
  pugi::xml_node parent, root, child, subchild;
  in_addr addr;
  char str[INET6_ADDRSTRLEN];
  bool is_valid_pmip_mode = false;

  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PMIPV6_TAG);

  QCMAP_Backhaul::pmip_conf.pmip_debug_mode_enabled= atoi(root.child(EnablePMIPDebugmode).child_value());
  QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode= atoi(root.child(PMIPTunnelType).child_value());
  LOG_MSG_ERROR("Pmip Tunnel Mode  %d",QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode,0,0);

  QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type = atoi(root.child(PMIPMobileNodeIdentifierType).child_value());
  LOG_MSG_INFO1("Pmip MobileNode identifier type %d ",QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type,0,0);

  if (QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01)
  {
    memcpy(QCMAP_Backhaul::pmip_conf.pmip_mn_id_string,
           root.child(PMIPMobileNodeIdentifierString).child_value(), strlen(root.child(PMIPMobileNodeIdentifierString).child_value()));
    LOG_MSG_INFO1("Pmip MobileNode identifier string is '%s'",QCMAP_Backhaul::pmip_conf.pmip_mn_id_string,0,0);
  }
    memcpy(QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,
           root.child(PMIPServiceSelectionString).child_value(), strlen(root.child(PMIPServiceSelectionString).child_value()));
  LOG_MSG_INFO1("Pmip Service selection string is '%s'",QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,0,0);


  if(QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    inet_pton(AF_INET6, root.child(LMAv6Address_TAG).child_value(),(struct in6_addr*)&QCMAP_Backhaul::pmip_conf.LMA_v6_addr);
    inet_ntop(AF_INET6, QCMAP_Backhaul::pmip_conf.LMA_v6_addr, (char *)&str, INET6_ADDRSTRLEN);
    if (strncmp (str, "::", sizeof("::")) == 0)
    {
      LOG_MSG_ERROR("Pmip not valid LMA ipv6 address %s",str,0,0);
      QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip = false;
    }
    else
    {
      LOG_MSG_ERROR("Pmip valid LMA ipv6 address %s",str,0,0);
      QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip = true;
    }
  }
  else if(QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
      if (inet_aton(root.child(LMAv4Address_TAG).child_value(), &addr))
      {
        QCMAP_Backhaul::pmip_conf.LMA_v4_addr = addr.s_addr;
        LOG_MSG_ERROR("Pmip valid LMA ipv4 address %s",root.child(LMAv4Address_TAG).child_value(),0,0);
        QCMAP_Backhaul::pmip_conf.isValidLMAV4Ip = true;
      }
      else
      {
        LOG_MSG_ERROR("Pmip Invalid LMA ipv4 address %s",str,0,0);
        QCMAP_Backhaul::pmip_conf.isValidLMAV4Ip = false;
      }
  }

  LOG_MSG_ERROR("Pmip Mode is %d",atoi(root.child(PMIPmodeType_TAG).child_value()),0,0);

  if(atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
     atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {

    QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled = atoi(root.child(EnablePMIPmode_TAG).child_value());
    QCMAP_Backhaul::pmip_conf.pmip_mode_type = atoi(root.child(PMIPmodeType_TAG).child_value());

    LOG_MSG_INFO1("\nPMIPV6 config: pmip_mode: %d & pmip_mode_type %d",
                  QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled,
                  QCMAP_Backhaul::pmip_conf.pmip_mode_type,0);
    is_valid_pmip_mode = true;
  }

  if(atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
     atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {

    QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled = atoi(root.child(EnablePMIPmode_TAG).child_value());
    QCMAP_Backhaul::pmip_conf.pmip_mode_type = atoi(root.child(PMIPmodeType_TAG).child_value());
    QCMAP_Backhaul::pmip_conf.pmipv4_work_mode = atoi(root.child(PMIPV4WorkMode_TAG).child_value());


    LOG_MSG_INFO1("\nPMIPV4 config: pmip_mode: %d & pmip_mode_type %d & pmipv4 work mode %d",
                  QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled,
                  QCMAP_Backhaul::pmip_conf.pmip_mode_type,QCMAP_Backhaul::pmip_conf.pmipv4_work_mode);
    if (QCMAP_Backhaul::pmip_conf.pmipv4_work_mode == QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01)
    {
      QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.prefix_len = atoi(root.child(PMIPV4SecRouterdmnpPrefixLen_TAG).child_value());
      if (inet_aton(root.child(PMIPV4SecRouterDMNPPrefix_TAG).child_value(), &addr))
        QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.dmnp_prefix = addr.s_addr;

      LOG_MSG_ERROR("DMNP prefix %s,len=%d:",root.child(PMIPV4SecRouterDMNPPrefix_TAG).child_value(),QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.prefix_len,0);
    }
    is_valid_pmip_mode = true;
  }

  if (is_valid_pmip_mode = false)
  {
    /*Error in config - set default values */
    LOG_MSG_ERROR("Invalid format for pmip_mode_type: (%d) & pmipv4_mode_type: (%d) in XML. Overwrite with default",
                    QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled,QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled,0);
    QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled = PMIP_MODE_DISABLED;
    QCMAP_Backhaul::pmip_conf.pmip_mode_type = QCMAP_MSGR_IP_FAMILY_V6_V01;
    QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled = PMIP_MODE_DISABLED;
    QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_MAC_V01;
    memcpy(QCMAP_Backhaul::pmip_conf.pmip_mn_id_string,
           PMIP_INVALID_ID_STRING, strlen(PMIP_INVALID_ID_STRING));
    memcpy(QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,
         PMIP_INVALID_ID_STRING, strlen(PMIP_INVALID_ID_STRING));
  }

  return true;
}

/*===========================================================================
  FUNCTION IsPmipV6ModeEnabled
==========================================================================*/
/*!
@brief
  Gives if the PMIP v6 mode is set in config or not

@return
   true - PMIP mode is enabled
   false - PMIP mode is disabled
/*=========================================================================*/
boolean QCMAP_Backhaul::IsPmipV6ModeEnabled()
{
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("IsPmipV6ModeEnabled: %d", QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled, 0, 0);
  return QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled;
}

/*===========================================================================
  FUNCTION IsPmipV4ModeEnabled
==========================================================================*/
/*!
@brief
  Gives if the PMIP V4 mode is set in config or not

@return
   true - PMIP mode is enabled
   false - PMIP mode is disabled
/*=========================================================================*/
boolean QCMAP_Backhaul::IsPmipV4ModeEnabled()
{
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();

  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("IsPmipV4ModeEnabled: %d", QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled, 0, 0);
  return QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled;
}

/*===========================================================================
  FUNCTION GetSetPmipConfigFromXML
==========================================================================*/
/*!
@brief
  Get or set the Pmip Config from XML.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul::GetSetPmipConfigFromXML
(
  qcmap_action_type action,
  qcmap_msgr_get_pmip_mode_resp_msg_v01 *resp_ptr
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root,child;
  const char *tag_ptr;
  in_addr addr;
  bool is_valid_pmip_mode = 0;

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);


  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(PMIPV6_TAG);

  if (action == GET_VALUE)
  {
    resp_ptr->pmip_mode = atoi(root.child(EnablePMIPmode_TAG).child_value());
    resp_ptr->pmip_mode_type = atoi(root.child(PMIPmodeType_TAG).child_value());
    LOG_MSG_INFO1("PMIPV6 config: pmip_mode: %d & pmip_mode_type %d",
                  resp_ptr->pmip_mode,
                  resp_ptr->pmip_mode_type,0);
    if(atoi(root.child(PMIPTunnelType).child_value()) == QCMAP_MSGR_IP_FAMILY_V6_V01)
    {
      resp_ptr->lma_ipv6_addr_valid = true;
      inet_pton(AF_INET6, root.child(LMAv6Address_TAG).child_value(),(struct in6_addr*)&resp_ptr->lma_ipv6_addr);
      if (strncmp (root.child(LMAv6Address_TAG).child_value(), "::", sizeof("::")) == 0)
      {
        LOG_MSG_ERROR("Pmip not valid LMA ipv6 address %s",root.child(LMAv6Address_TAG).child_value(),0,0);
        resp_ptr->lma_ipv6_addr_valid = false;
      }
      else
      {
        LOG_MSG_ERROR("Pmip valid LMA ipv6 address %s",root.child(LMAv6Address_TAG).child_value(),0,0);
        resp_ptr->lma_ipv6_addr_valid = true;
        inet_pton(AF_INET6, root.child(LMAv6Address_TAG).child_value(),(struct in6_addr*)&resp_ptr->lma_ipv6_addr);
      }
    }
    else if (atoi(root.child(PMIPTunnelType).child_value()) == QCMAP_MSGR_IP_FAMILY_V4_V01)
    {
      if (inet_aton(root.child(LMAv4Address_TAG).child_value(), &addr))
      {
        resp_ptr->lma_ipv4_addr_valid = true;
        resp_ptr->lma_ipv4_addr = addr.s_addr;
      }
    }
    else
    {
      LOG_MSG_ERROR("invalid Tunnel mode",0,0,0)
    }

    if (atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
        atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
    {
      LOG_MSG_INFO1("\nPMIP Mode is v6",0,0,0);
      is_valid_pmip_mode = 1;
    }
    if(atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
       atoi(root.child(PMIPmodeType_TAG).child_value()) == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
    {
      is_valid_pmip_mode = 1;
      LOG_MSG_INFO1("\nPMIP Mode is v4",0,0,0);
      resp_ptr->pmipv4_mode_type_valid= true;
      resp_ptr->pmipv4_mode_type = atoi(root.child(PMIPV4WorkMode_TAG).child_value());
      LOG_MSG_INFO1("\nPMIPV4 config: pmipv4 work mode %d",0,0,resp_ptr->pmipv4_mode_type);

      if (resp_ptr->pmipv4_mode_type == QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01)
      {
        resp_ptr->pmipv4_sec_router_param_valid = true;
        resp_ptr->pmipv4_sec_router_param.prefix_len= atoi(root.child(PMIPV4SecRouterdmnpPrefixLen_TAG).child_value());
        if (inet_aton(root.child(PMIPV4SecRouterDMNPPrefix_TAG).child_value(), &addr))
          resp_ptr->pmipv4_sec_router_param.dmnp_prefix = addr.s_addr;
        LOG_MSG_ERROR("DMNP prefix %s,len=%d:",root.child(PMIPV4SecRouterDMNPPrefix_TAG).child_value(),resp_ptr->pmipv4_sec_router_param.prefix_len,0);
      }
    }

    resp_ptr->pmip_mobile_node_identifier_type_valid = true;
    resp_ptr->pmip_mobile_node_identifier_type = atoi(root.child(PMIPMobileNodeIdentifierType).child_value());
    LOG_MSG_INFO1("pmip mode identifier is type %d", resp_ptr->pmip_mobile_node_identifier_type,0, 0);

    if (resp_ptr->pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01)
    {
      resp_ptr->pmip_mn_id_string_valid = true;
      memcpy(resp_ptr->pmip_mn_id_string, root.child(PMIPMobileNodeIdentifierString).child_value(),strlen(root.child(PMIPMobileNodeIdentifierString).child_value()));
      LOG_MSG_INFO1("pmip mode identifier string ='%s'", resp_ptr->pmip_mn_id_string,0, 0);
    }

    if (strncmp(root.child(PMIPServiceSelectionString).child_value(),
        PMIP_INVALID_ID_STRING,strlen(PMIP_INVALID_ID_STRING) != 0))
    {
      resp_ptr->pmip_service_selection_string_valid = true;
      memcpy(resp_ptr->pmip_service_selection_string,root.child(PMIPServiceSelectionString).child_value(),strlen(root.child(PMIPServiceSelectionString).child_value()));
      LOG_MSG_INFO1("pmip service selection string is '%s'.",resp_ptr->pmip_service_selection_string,0, 0);
    }
    if (is_valid_pmip_mode == 0)
    {
      LOG_MSG_ERROR("Invalid format for pmip_mode: (%d) & pmipv4_mode_type: (%d)",
                    resp_ptr->pmip_mode,resp_ptr->pmip_mode_type,0);

    }
  }
  return true;
}

/*===========================================================================
  FUNCTION GetPmipConfig
==========================================================================*/
/*!
@brief
  Gets the current PMIP configuration set

@return
   true - Sucess
   false -Failure
/*=========================================================================*/
boolean QCMAP_Backhaul::GetPmipConfig(qcmap_msgr_get_pmip_mode_resp_msg_v01 *resp_ptr,
                  qmi_error_type_v01 *qmi_err_num )
{
   QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
   if(resp_ptr == NULL)
   {
     *qmi_err_num = QMI_ERR_INTERNAL_V01;
     return false;
   }
   if (!QcMapBackhaulMgr)
   {
     LOG_MSG_ERROR("backhul object is not there",0,0, 0);
     GetSetPmipConfigFromXML(GET_VALUE, resp_ptr);
     return true;
   }

   LOG_MSG_INFO1("GetPmipConfig pmip mode: %d obj =%p", QCMAP_Backhaul::pmip_conf.pmip_mode_type, QcMapBackhaulMgr, 0);
   if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
       QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
   {
   resp_ptr->pmip_mode = QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled;
   }

   if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
       QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
   {
     resp_ptr->pmip_mode = QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled;
   }
   resp_ptr->pmip_mode_type = QCMAP_Backhaul::pmip_conf.pmip_mode_type;

   if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01)
   {
     if (QCMAP_Backhaul::pmip_conf.isValidLMAV4Ip)
     {
       LOG_MSG_INFO1("pmip v4 mode valid v4 ip",0,0, 0);
       resp_ptr->lma_ipv4_addr_valid = true;
       resp_ptr->lma_ipv4_addr = QCMAP_Backhaul::pmip_conf.LMA_v4_addr;
     }
     else
     {
       LOG_MSG_ERROR("LMA v4 ip is invalid",0,0, 0);
     }
   }
   else if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
   {
     if (QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip)
     {
       resp_ptr->lma_ipv6_addr_valid = true;
       memcpy(resp_ptr->lma_ipv6_addr, QCMAP_Backhaul::pmip_conf.LMA_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
     }
     else
     {
       LOG_MSG_ERROR("LMA v6 ip is invalid",0,0, 0);
     }
     }
     else
     {
     LOG_MSG_ERROR("Pmip tunnel mode is invalid",0,0, 0);
     }

   if(resp_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      resp_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
   {
     LOG_MSG_INFO1("pmip mode is v4",0,0, 0);
     resp_ptr->pmipv4_mode_type_valid = true;
     resp_ptr->pmipv4_mode_type = QCMAP_Backhaul::pmip_conf.pmipv4_work_mode;
     LOG_MSG_INFO1("pmip v4 work mode %d",resp_ptr->pmipv4_mode_type,0, 0);
     if(QCMAP_Backhaul::pmip_conf.pmipv4_work_mode == QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01)
     {
       LOG_MSG_INFO1("pmip v4 secondary Router work mode %d",resp_ptr->pmipv4_mode_type,0, 0);
       resp_ptr->pmipv4_sec_router_param_valid = true;
       resp_ptr->pmipv4_sec_router_param.dmnp_prefix = QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.dmnp_prefix;
       resp_ptr->pmipv4_sec_router_param.prefix_len = QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.prefix_len;
     }
   }
   if(resp_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
      resp_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
   {
     LOG_MSG_INFO1("pmip mode is v6",0,0, 0);
   }

   resp_ptr->pmip_mobile_node_identifier_type_valid= true;
   resp_ptr->pmip_mobile_node_identifier_type = QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type;
   if (QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01)
   {
     LOG_MSG_INFO1("pmip mode identifier is string.",0,0, 0);
     resp_ptr->pmip_mn_id_string_valid = true;
     memcpy(resp_ptr->pmip_mn_id_string, QCMAP_Backhaul::pmip_conf.pmip_mn_id_string,strlen(QCMAP_Backhaul::pmip_conf.pmip_mn_id_string));
   }

   if (strncmp(QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,
       PMIP_INVALID_ID_STRING,strlen(PMIP_INVALID_ID_STRING) != 0))
   {
     resp_ptr->pmip_service_selection_string_valid = true;
     memcpy(resp_ptr->pmip_service_selection_string,QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,strlen(QCMAP_Backhaul::pmip_conf.pmip_service_selection_string));
     LOG_MSG_INFO1("pmip service selection string is '%s'.",resp_ptr->pmip_service_selection_string,0, 0);
   }


   return true;
}


/*===========================================================================
  FUNCTION SetPmipMode
==========================================================================*/
/*!
@brief
   Set/modify the PMIP configuration

 @return
      true - Sucess
      false -Failure
 /*=========================================================================*/

 boolean QCMAP_Backhaul::SetPmipMode(
             qcmap_msgr_set_pmip_mode_req_msg_v01  *req_ptr,
             qmi_error_type_v01 *qmi_err_num )
{

  uint8_t LMA_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
//  uint32_t LMA_v4_addr;
  char lma_v6_str[INET6_ADDRSTRLEN] = {0};
  char lma_v4_str[INET_ADDRSTRLEN] = {0};
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  qmi_error_type_v01 qmi_err;
  bool result;
  qcmap_dynamic_dns_t ddns_conf;
  boolean  prefix_delegation = false;
  bool is_valid_pmip_mode = false;
  boolean  ret = false;

  memset(&ddns_conf, 0x0, sizeof(qcmap_dynamic_dns_t));

  if(req_ptr == NULL)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return FALSE;
  }
  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
    return false;
  }

  /*Add checks here as below,
    if any of the Prefix delegation/DDNS/WWAN is not first priority backhaul then reject the set*/

  if((QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(GET_VALUE, &ddns_conf) &&
       ddns_conf.enable == DDNS_ENABLED) ||
      (QCMAP_Backhaul_WWAN::GetPrefixDelegationConfig(&prefix_delegation, qmi_err_num) &&
       prefix_delegation))
  {
    LOG_MSG_ERROR("Other Features DDNS:% or Prefix delegation:%d enabled are enabled. reject PMIP set",
                  ddns_conf.enable, prefix_delegation,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return FALSE;
  }

  LOG_MSG_INFO1 ("SetPmipMode:%d",req_ptr->enable_pmip_mode,0,0);

  if(req_ptr->enable_pmip_mode == PMIP_MODE_ENABLED)
  {
    /*Write to the XML fille
      Call EnablePMIPmode()*/
    //pmip debug/Normal Mode run
    QCMAP_Backhaul::pmip_conf.pmip_debug_mode_enabled = req_ptr->enable_pmip_debug_mode;

    if (req_ptr->lma_ipv6_addr_valid == true)
    {
      if (QcMapBackhaulMgr && !QcMapBackhaulMgr->enable_ipv6)
      {
        LOG_MSG_ERROR("Ipv6 is disabled",0,0,0);
        *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
        return false;
      }
      if(!PS_IN6_IS_ADDR_UNSPECIFIED(req_ptr->lma_ipv6_addr))
      {
        inet_ntop(AF_INET6, (void *)req_ptr->lma_ipv6_addr, lma_v6_str, INET6_ADDRSTRLEN);
        LOG_MSG_INFO1("LMA v6 IP revceived :%s ",lma_v6_str,0,0);

        /*Update the cache*/
        memcpy(QCMAP_Backhaul::pmip_conf.LMA_v6_addr, req_ptr->lma_ipv6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip = true;
        QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode = QCMAP_MSGR_IP_FAMILY_V6_V01;
      }
      else
      {
        LOG_MSG_ERROR("LMA v6 IP not received for pmipv6 mode",0,0,0);
        QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip = false;
      }
    }
    else if (req_ptr->lma_ipv4_addr_valid == true)
    {
      if (QcMapBackhaulMgr && !QcMapBackhaulMgr->enable_ipv4)
      {
        LOG_MSG_ERROR("Ipv4 is disabled",0,0,0);
        *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
        return false;
      }
      inet_ntop(AF_INET, (void *)&req_ptr->lma_ipv4_addr, lma_v4_str, INET_ADDRSTRLEN);
      LOG_MSG_INFO1("LMA v4 IP revceived :%s for pmipv6 mode. pmip will use v4 tunnel",lma_v4_str,0,0);
      QCMAP_Backhaul::pmip_conf.LMA_v4_addr = req_ptr->lma_ipv4_addr ;
      QCMAP_Backhaul::pmip_conf.isValidLMAV4Ip = true;
      //pmip v4 Tunnel Type as v4 as we have LMA v4 IP
      QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode = QCMAP_MSGR_IP_FAMILY_V4_V01;
    }
    else
    {
      LOG_MSG_ERROR("LMA Ip was not provided",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }

    if(req_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
       req_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
    {
      LOG_MSG_INFO1 ("SetPmipMode: for IPv6",0,0,0);
      is_valid_pmip_mode = true;

      QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled = req_ptr->enable_pmip_mode;
      QCMAP_Backhaul::pmip_conf.pmip_mode_type = req_ptr->pmip_mode_type;
    }
    if(req_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
       req_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
    {
      is_valid_pmip_mode = true;
      LOG_MSG_INFO1 ("SetPmipMode: for IPv4",0,0,0);
      if (req_ptr->pmipv4_mode_type_valid == false)
      {
        *qmi_err_num = QMI_ERR_INVALID_DATA_FORMAT_V01;
        LOG_MSG_ERROR("pmipv4 mode but pmipv4 work mode (CPE or Secondary Router Not specified)",0,0,0);
        return FALSE;
      }

      QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled = req_ptr->enable_pmip_mode;
      QCMAP_Backhaul::pmip_conf.pmip_mode_type = req_ptr->pmip_mode_type;
      if (req_ptr->pmipv4_mode_type== QCMAP_MSGR_PMIPV4_MODE_CPE_V01)
      {
        LOG_MSG_ERROR("pmipv4 is CPE mode",0,0,0);
        QCMAP_Backhaul::pmip_conf.pmipv4_work_mode= QCMAP_MSGR_PMIPV4_MODE_CPE_V01;
      }
      else if (req_ptr->pmipv4_mode_type == QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01)
      {
        LOG_MSG_INFO1("pmipv4 is secondary router mode",0,0,0);
        QCMAP_Backhaul::pmip_conf.pmipv4_work_mode = QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01;
        QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.dmnp_prefix = req_ptr->pmipv4_sec_router_param.dmnp_prefix;
        QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.prefix_len= req_ptr->pmipv4_sec_router_param.prefix_len;
      }
    }

    if (is_valid_pmip_mode == false)
    {
      *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
      LOG_MSG_ERROR("Invalid PMIP mode type provided:%d",req_ptr->pmip_mode_type,0,0);
      return FALSE;
    }

    if (req_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
        req_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
        req_ptr->pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
    {
      if (req_ptr->pmip_mobile_node_identifier_type_valid == true)
      {
        //Pmip Mobile Node Identifier Type validation & storing
        if (req_ptr->pmip_mobile_node_identifier_type != QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01 &&
            req_ptr->pmip_mobile_node_identifier_type != QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_MAC_V01)
        {
          LOG_MSG_ERROR("Invalid pmip mobile node Identifier %d",req_ptr->pmip_mobile_node_identifier_type,0,0);
          *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
          return false;
        }
        QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type = req_ptr->pmip_mobile_node_identifier_type;
        if (req_ptr->pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01)
        {
          if (req_ptr->pmip_mn_id_string_valid)
          {
            memset(QCMAP_Backhaul::pmip_conf.pmip_mn_id_string, 0,
                   QCMAP_MSGR_PMIP_MN_ID_STRING_LENGTH_V01 + 1);
            memcpy(QCMAP_Backhaul::pmip_conf.pmip_mn_id_string,
                   req_ptr->pmip_mn_id_string,strlen(req_ptr->pmip_mn_id_string));
            LOG_MSG_INFO1("Successfully stored the MN id string '%s'",
                          QCMAP_Backhaul::pmip_conf.pmip_mn_id_string,0,0);
          }
          else
          {
            LOG_MSG_ERROR("pmip mobile node Identifier sring not provided",0,0,0);
            *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
            return false;
          }
        }
      }
      else
      {
        LOG_MSG_INFO1("pmip_mobile_node_identifier_type_valid flag is false."
                      " Setting Pmip MN identifier type to default as MAC address",0,0,0);
        QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type = QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_MAC_V01;
      }

      //Pmip Service Selection String
      if (req_ptr->pmip_service_selection_string_valid)
      {
        LOG_MSG_INFO1("Got Request To modify Service Selection String ",0,0,0);
        memset(QCMAP_Backhaul::pmip_conf.pmip_service_selection_string, 0,
               QCMAP_MSGR_PMIP_SERVICE_SELECTION_STRING_LENGTH_V01 + 1);
        memcpy(QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,
               req_ptr->pmip_service_selection_string,strlen(req_ptr->pmip_service_selection_string));
        LOG_MSG_INFO1("Successfully stored the Service Selection string '%s'",
                      QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,0,0);
      }
      else
      {
        LOG_MSG_ERROR("pmip Serivice Selection sring not provided",0,0,0);
      }

    }


    /*Update the XML*/
    QCMAP_Backhaul::WritePMIPConfToXML();
    LOG_MSG_INFO1("SetPmipMode config Updated Sucesfully for mode", QCMAP_Backhaul::pmip_conf.pmip_mode_type, 0, 0);

    if( QcMapBackhaulMgr && QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN)
    {
      if(!QcMapBackhaulMgr->EnablePmipMode(&qmi_err))
      {
        LOG_MSG_ERROR("EnablePmipMode failed with error:%d",qmi_err,0,0);
      }
    }
    else
    {
      LOG_MSG_ERROR("No approriate Backhaul for PMIP mode, will be enabled later",0,0,0);
    }
    return TRUE;
  }
  else if (req_ptr->enable_pmip_mode == PMIP_MODE_DISABLED)
  {
    LOG_MSG_INFO1("SetPmipMode to disable pmip", 0, 0, 0);
    if(QCMAP_Backhaul::pmip_conf.pmipv6_mode_active == PMIP_MODE_ENABLED ||
       QCMAP_Backhaul::pmip_conf.pmipv4_mode_active == PMIP_MODE_ENABLED)
    {
      ret = QcMapBackhaulMgr->DisablePmipMode(&qmi_err, PMIP_DISABLE_EV);
    }
    else
    {
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      LOG_MSG_ERROR("Pmip is Already Disabled\n",0,0,0);
      return false;
    }
    if (ret == TRUE)
    {
      QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled = req_ptr->enable_pmip_mode;
      QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled = req_ptr->enable_pmip_mode;
      QCMAP_Backhaul::WritePMIPConfToXML();
      LOG_MSG_INFO1("SetPmipMode config Updated Sucesfully", 0, 0, 0);
    }
    return ret;
  }
  else
  {
    LOG_MSG_ERROR("Invalid %d enable pmip mode request",req_ptr->enable_pmip_mode,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
}


/*===========================================================================
  FUNCTION EnablePmipMode
==========================================================================*/
/*!
@brief
   Enable PMIP mode

 @return
      true - Sucess
      false -Failure
 /*=========================================================================*/
boolean QCMAP_Backhaul::EnablePmipMode(
             qmi_error_type_v01 *qmi_err_num )
{

  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_LAN* default_lan_obj = GET_DEFAULT_LAN_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  boolean result;

/* 1 Sanity checks - wwan backhaul
     2 DisableNATonAPPs & DisableIPv6forwarding
     3 Kill Radish
     4 Bring down All LAN ifaces including Bridge
     5  Init Bridge interface
     6 Update MAG conf file & Start MAG process <pmip6d>
     7 Restart LAN ifaces (DO NOT allow any other old Backhaul connected)
     8 EnableNATonAPPs & EnableIPv6forwarding
     9 Install required rules
     10 Set the <pmipmodeactive> Flag     */

  LOG_MSG_INFO1("EnablePmipMode:",0, 0, 0);
  if (!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr pointer is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }
  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
    return false;
  }
  if(QcMapNatAlg == NULL)
  {
    LOG_MSG_ERROR("QcMapNatAlg object is NULL", 0, 0, 0);
    return false;
  }
  if(NULL == default_lan_obj)
  {
    LOG_MSG_ERROR("Default LAN object is NULL", 0, 0, 0);
    return false;
  }


  /* 1 Sanity checks - wwan backhaul*/
  if((QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 &&
     QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled != PMIP_MODE_ENABLED) ||
     (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01
      && QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled != PMIP_MODE_ENABLED) ||
     (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01
      && (QCMAP_Backhaul::pmip_conf.pmipv4_mode_enabled != PMIP_MODE_ENABLED &&
      QCMAP_Backhaul::pmip_conf.pmipv6_mode_enabled != PMIP_MODE_ENABLED)) ||
     !(QcMapBackhaulMgr && (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN)) ||
     (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01 &&
     (QcMapBackhaulWWANMgr->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED))||
     ((QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01) &&
     (QcMapBackhaulWWANMgr->GetState() != QCMAP_CM_WAN_CONNECTED)))
  {
    *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
    LOG_MSG_ERROR("WWAN Backhaul is NOT Active:%d",*qmi_err_num, 0, 0);
    return FALSE;
  }

  if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    if (QcMapBackhaulWWANMgr->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED)
    {
      *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
      LOG_MSG_ERROR("IPV6 WWAN Backhaul is NOT Active:%d for pmip v6 tunnel mode",*qmi_err_num, 0, 0);
      return FALSE;
    }
    else
    {
      LOG_MSG_INFO1("IPV6 WWAN Backhaul is Active for pmip v6 tunnel mode",0, 0, 0);
    }
  }

  LOG_MSG_INFO1("EnablePmipMode for Mode %d", QCMAP_Backhaul::pmip_conf.pmip_mode_type, 0, 0);
 if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
     QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
 {
  /* 2 DisableNATonAPPs & DisableIPv6forwarding*/
  QcMapBackhaulMgr->DisableIPV6Forwarding(false);
  LOG_MSG_INFO1("EnablePmipMode: DisableIPV6Forwarding", 0, 0, 0);


  /* 3 Kill Radish*/
  QcMapBackhaulMgr->StopRadish();
  LOG_MSG_INFO1("EnablePmipMode: StopRadish", 0, 0, 0);
  }

  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    LOG_MSG_INFO1("Pmip mode is v4 Disable pmip ",0,0,0)
    QcMapBackhaulMgr->DisablePmipMode(qmi_err_num, PMIP_DISABLE_EV);
  }

  /*   5  Bring down All LAN ifaces including Bridge
         6  Init Bridge interface
         7  Update MAG conf file & startPMIPprocess
         8  Restart LAN ifaces */
  if ((result = default_lan_obj->PmipRestartLAN(qmi_err_num, PMIP_ENABLE_EV)) == false)
  {
     LOG_MSG_ERROR("Failed to Re-Activate LAN on ProcessStaDHCPIPFail()",0,0,0);
  }

  LOG_MSG_INFO1("EnablePmipMode: PmipRestartLAN", 0, 0, 0);

  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {

   /*  9  EnableNATonAPPs & EnableIPv6forwarding*/
   QcMapBackhaulMgr->EnableIPV6Forwarding();

   LOG_MSG_INFO1("EnablePmipMode: EnableIPV6Forwarding", 0, 0, 0);
  }
  /*  11  Install required rules
       12  Set the <pmipmodeactive> Flag*/
  if(result == true)
  {
    if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
        QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
      QCMAP_Backhaul::pmip_conf.pmipv6_mode_active = PMIP_MODE_ENABLED;
    if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
        QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
      QCMAP_Backhaul::pmip_conf.pmipv4_mode_active = PMIP_MODE_ENABLED;
    *qmi_err_num = QMI_ERR_NONE_V01;
    LOG_MSG_INFO1("EnablePmipMode: Set pmip_mode_active to 1 for mode %d",
                  QCMAP_Backhaul::pmip_conf.pmip_mode_type, 0, 0);
  }
  else
  {
    if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
       QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
      QCMAP_Backhaul::pmip_conf.pmipv6_mode_active = PMIP_MODE_DISABLED;
    if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
        QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
      QCMAP_Backhaul::pmip_conf.pmipv4_mode_active = PMIP_MODE_DISABLED;
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
  }
  return result;
}

boolean QCMAP_Backhaul::checkProcessExist(char *proc)
{
  int i = 0;
  FILE *cmd=NULL;
  char pid_s[MAX_CMD_SIZE];
  int pid = 0;
  char process[MAX_CMD_SIZE] = {0};

  snprintf(process, sizeof(process), "%s %s", PROCESS_ID_OF_CMD, proc);
  while (i++ < 5)
  {
    usleep(PROCESS_CHECK_TIME);
    cmd = popen(process, "r");
    pid = 0;
    memset(pid_s, 0, MAX_CMD_SIZE);
    if(cmd)
    {
      fgets(pid_s, MAX_CMD_SIZE, cmd);
      pclose(cmd);
    }
    pid = atoi(pid_s);
    if(pid == 0)
    {
      return false;
    }
  }
  LOG_MSG_ERROR("Process %s is running\n", proc, 0, 0);
  return true;
}

boolean QCMAP_Backhaul::updatePMIPConfigFile()
{
  uint8_t public_ip_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t primary_dns_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t secondary_dns_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char v6add_str[INET6_ADDRSTRLEN] = {0};
  char public_v6_str[INET6_ADDRSTRLEN];
  char lma_v6_str[INET6_ADDRSTRLEN] = {0};
  char lma_v4_str[INET_ADDRSTRLEN] = {0};
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  qmi_error_type_v01 qmi_err_num;
  int qmi_error;
  int ret;
  struct in_addr public_ip;
  uint32 primary_dns;
  in_addr_t secondary_dns;
  uint32 default_gw = 0;
  uint32 netmask;

  //Update MAG conf file & Start MAG process <pmip6d>
  /*Update the MAG config file*/
  LOG_MSG_INFO1("updatePMIPConfigFile: enter", 0, 0, 0);

  if (!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr pointer is NULL ", 0, 0, 0);
    return false;
  }

  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
    return false;
  }

  QcMapBackhaulWWANMgr->GetIfaceLLAddrStr(v6add_str, BRIDGE_IFACE);
  LOG_MSG_INFO1("bridge0 LL:%s", v6add_str, 0, 0);

  if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    inet_ntop(AF_INET, &QCMAP_Backhaul::pmip_conf.LMA_v4_addr , lma_v4_str, INET_ADDRSTRLEN);
    LOG_MSG_ERROR("LMA v4 IP revceived :%s for v4 tunnel",lma_v4_str,0,0);
    snprintf(command,
             MAX_COMMAND_STR_LEN,
             "sed -i 's/LmaV4Address.*\;/LmaV4Address          %s\;/g' %s",lma_v4_str,PMIP_MAG_CONF_FILE);
    ds_system_call(command, strlen(command));

    if ( QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED )
    {
      ret = QcMapBackhaulMgr->GetDeviceName\
                            (devname,QCMAP_MSGR_IP_FAMILY_V4_V01, &qmi_err_num);
      qmi_error = QcMapBackhaulWWANMgr->GetIPv4NetworkConfig(&public_ip.s_addr,
                                           &primary_dns,&secondary_dns,&default_gw,
                                           &qmi_err_num);
      snprintf(command,
              MAX_COMMAND_STR_LEN,
              "sed -i 's/Magv4AddressEgress.*\;/Magv4AddressEgress        %s\;/g' %s",inet_ntoa(public_ip),PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/MagDeviceEgress.*\;/MagDeviceEgress          \"%s\"\;/g' %s",devname,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
      //Getting bridge Ip
      if ( !QcMapBackhaulMgr->GetIP(&public_ip.s_addr, &netmask, BRIDGE_IFACE))
      {
        LOG_MSG_ERROR("Unable to get the IP address of bridge",0,0,0);
        return false;
      }
      else
      {
        snprintf(command,
                 MAX_COMMAND_STR_LEN,
                 "sed -i 's/Magv4AddressIngress.*\;/Magv4AddressIngress        %s\;/g' %s",inet_ntoa(public_ip),PMIP_MAG_CONF_FILE);
        ds_system_call(command, strlen(command));
      }
    }
    else
    {
      LOG_MSG_ERROR("The WWAN is not connected, so it doesn't have an IP",0,0,0);

    }
  }
  if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    if (QCMAP_Backhaul::pmip_conf.isValidLMAV6Ip)
    {
      inet_ntop(AF_INET6, QCMAP_Backhaul::pmip_conf.LMA_v6_addr , lma_v6_str, INET6_ADDRSTRLEN);
      LOG_MSG_INFO1("LMA v6 IP revceived :%s by Pmip mode for v6 tunnel",lma_v6_str,0,0);
      /*edit the MAG conf file*/
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/LmaAddress.*\;/LmaAddress        %s\;/g' %s",lma_v6_str,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_INFO1("LMA v6 IP Not revceived by pmip mode",0,0,0);
    }
    if (QcMapBackhaulWWANMgr->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)
    {
      /*this checks if v6 is enabled and gets the IPv6 addr of current backhaul*/
      if( !(QcMapBackhaulMgr->GetIPv6NetworkConfig(public_ip_v6,
                                           primary_dns_v6,
                                           secondary_dns_v6,
                                           &qmi_err_num)))
      {
        LOG_MSG_ERROR("GetIPv6NetworkConfig fails:%d",qmi_err_num, 0, 0);
        return false;
      }
      if(QcMapBackhaulMgr->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V6_V01, &qmi_err_num) == 0)
      {
        LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d\n", qmi_err_num,0,0);
        return false;
      }

      inet_ntop(AF_INET6, (void *)public_ip_v6, public_v6_str, INET6_ADDRSTRLEN);
      LOG_MSG_INFO1("devaname: %s & public_ip_v6:%s",devname, public_v6_str, 0);

      inet_ntop(AF_INET6, QCMAP_Backhaul::pmip_conf.LMA_v6_addr , lma_v6_str, INET6_ADDRSTRLEN);
      LOG_MSG_INFO1("LMA v6 IP revceived :%s by ipv6 mode",lma_v6_str,0,0);
      /*edit the MAG conf file*/
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/LmaAddress.*\;/LmaAddress        %s\;/g' %s",lma_v6_str,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/MagAddressEgress.*\;/MagAddressEgress        %s\;/g' %s",(char*)public_v6_str,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/MagDeviceEgress.*\;/MagDeviceEgress          \"%s\"\;/g' %s",devname,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR("Not updating MagDeviceEgress/MagDeviceEgress as V6 WAN not there . Will update on v6 wan connect",0,0,0);
    }

      snprintf(command,
               MAX_COMMAND_STR_LEN,
             "sed -i 's/MagAddressIngress.*\;/MagAddressIngress        %s\;/g' %s",v6add_str,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/MagDeviceIngress.*\;/MagDeviceIngress          \"%s\"\;/g' %s",BRIDGE_IFACE,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));

  }

  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    snprintf(command,
             MAX_COMMAND_STR_LEN,
             "sed -i 's/MagDeviceIngress.*\;/MagDeviceIngress          \"%s\"\;/g' %s",BRIDGE_IFACE,PMIP_MAG_CONF_FILE);
    ds_system_call(command, strlen(command));

    if (QCMAP_Backhaul::pmip_conf.pmipv4_work_mode == QCMAP_MSGR_PMIPV4_MODE_SECONDARY_ROUTER_V01)
    {
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/PMIPV4SecRouterdmnpPrefixLen.*\;/PMIPV4SecRouterdmnpPrefixLen          %d\;/' %s",
               QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.prefix_len,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));

      inet_ntop(AF_INET, &QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.dmnp_prefix, lma_v4_str, INET_ADDRSTRLEN);
      LOG_MSG_ERROR("DMNP Prefix revceived :%s prefix len %d",lma_v4_str,QCMAP_Backhaul::pmip_conf.pmipv4_sec_router_param.prefix_len,0);
      snprintf(command,
               MAX_COMMAND_STR_LEN,
               "sed -i 's/PMIPV4SecRouterDMNPPrefix.*\;/PMIPV4SecRouterDMNPPrefix          %s\;/' %s",lma_v4_str,PMIP_MAG_CONF_FILE);
      ds_system_call(command, strlen(command));
    }

  }

  LOG_MSG_ERROR("Pmip mobile node identifier type %d",QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type,0,0);
  if (QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type == QCMAP_MSGR_PMIP_MOBILE_NODE_IDENTIFIER_STRING_V01)
  {
    snprintf(command,
             MAX_COMMAND_STR_LEN,
             "sed -i 's/MnIdString.*\;/MnIdString          \"%s\"\;/g' %s",QCMAP_Backhaul::pmip_conf.pmip_mn_id_string,PMIP_MAG_CONF_FILE);
    ds_system_call(command, strlen(command));
  }

  LOG_MSG_ERROR("Pmip Service Selection string '%s' storing to pmip conf",QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,0,0);
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i 's/ServiceSelectionIdentifier.*\;/ServiceSelectionIdentifier          \"%s\"\;/g' %s",QCMAP_Backhaul::pmip_conf.pmip_service_selection_string,PMIP_MAG_CONF_FILE);
  ds_system_call(command, strlen(command));

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "echo %s radius6server >> /data/hosts",lma_v6_str);
  ds_system_call(command, strlen(command));

  return true;
}


/*===========================================================================
  FUNCTION startPMIPprocess
==========================================================================*/
/*!
@brief
   Starts the PMIP deamon

 @return
      true - Sucess
      false -Failure
 /*=========================================================================*/
boolean QCMAP_Backhaul::startPMIPprocess()
{
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_LAN* QcMapMgrLan=GET_DEFAULT_LAN_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  struct in_addr sip,eip,smask;
  char startip[INET_ADDRSTRLEN],endip[INET_ADDRSTRLEN],netmask[INET_ADDRSTRLEN];
  char data[MAX_COMMAND_STR_LEN] = {0};
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;

  LOG_MSG_INFO1("startPMIPprocess: enter mode %d", QCMAP_Backhaul::pmip_conf.pmip_mode_type, 0, 0);

  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
    return false;
  }


  /*Update the PMIP config file
          Start PMIP process <pmip6d>
          Install basic rules*/
  if(!(QcMapBackhaulMgr && QcMapBackhaulMgr->updatePMIPConfigFile()))
  {
    LOG_MSG_ERROR("startPMIPprocess: Failed to update PMIP conf file",0, 0, 0);
    return false;
  }

  /*  Start PMIP process <pmip6d>*/
  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf %s", TEMP_PMIP_LOG_FILE);
  ds_system_call(command, strlen(command));

  ds_system_call("killall -15 pmip6d",
                 strlen("killall -15 pmip6d"));

  ds_system_call("killall -9 pmip6d",
                 strlen("killall -9 pmip6d"));

  //("/usr/sbin/pmip6d -c /etc/data/pmip-mag1.conf -l /tmp/pmip.log &"));

  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    if(QcMapMgrLan)
    {
      if((lan_cfg = QcMapMgrLan->GetLANConfig()) == NULL)
      {
        LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
        return false;
      }
      sip.s_addr=htonl(lan_cfg->dhcp_start_address) ;
      eip.s_addr=htonl(lan_cfg->dhcp_end_address) ;
      smask.s_addr = htonl(lan_cfg->sub_net_mask);
    } else {
      LOG_MSG_ERROR("LAN mgr is NULL", 0, 0, 0);
      return false;
    }
    inet_ntop(AF_INET, (void *)&(sip.s_addr), startip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, (void *)&(eip.s_addr), endip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, (void *)&(smask.s_addr), netmask, INET_ADDRSTRLEN);

    snprintf(command,
             MAX_COMMAND_STR_LEN,
             "%s -c %s -m %d -w %d -s %s -e %s -j %s -t %d", PMIP_PATH, PMIP_MAG_CONF_FILE,
             QCMAP_Backhaul::pmip_conf.pmip_mode_type,
             QCMAP_Backhaul::pmip_conf.pmipv4_work_mode,startip,
             endip, netmask, QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode);
    if (QCMAP_Backhaul::pmip_conf.pmipv4_work_mode == QCMAP_MSGR_PMIPV4_MODE_CPE_V01)
    {
      LOG_MSG_INFO1("CPE mode",0, 0, 0);
    }
    else
    {
      LOG_MSG_INFO1("Secondary Router mode",0, 0, 0);
    }
  }
  else
  {
   snprintf(command,
           MAX_COMMAND_STR_LEN,
             "%s -c %s -m %d -t %d", PMIP_PATH, PMIP_MAG_CONF_FILE,
             QCMAP_Backhaul::pmip_conf.pmip_mode_type, QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode);
  }

  //provide Mobile Node Identifier Type to pmip
  snprintf(data,
           MAX_COMMAND_STR_LEN,
             " -i %d ", QCMAP_Backhaul::pmip_conf.pmip_mobile_node_identifier_type);
  strlcat(command,data,MAX_COMMAND_STR_LEN);

  LOG_MSG_INFO1("data='%s',, com='%s'",data,command,0);

  //if debug mode start pmip in debug Mode
  if(QCMAP_Backhaul::pmip_conf.pmip_debug_mode_enabled)
  {
    memset(data,0,MAX_COMMAND_STR_LEN);
    snprintf(data, MAX_COMMAND_STR_LEN, " -l %s", TEMP_PMIP_LOG_FILE);
    strlcat(command,data,MAX_COMMAND_STR_LEN);
    LOG_MSG_INFO1("starting Pmip in debug mode",0, 0, 0);
  }
  else
  {
    LOG_MSG_INFO1("starting Pmip in Normal mode",0, 0, 0);
  }

  strlcat(command,"  > /dev/null 2>&1 &",MAX_COMMAND_STR_LEN);

  LOG_MSG_ERROR("startPMIPprocess: PMIP process args =%s",command, 0, 0);
  ds_system_call(command, strlen(command));

  if(QCMAP_Backhaul::checkProcessExist(PMIP_PROCESS) == false)
  {
    LOG_MSG_ERROR("startPMIPprocess: Failed to start PMIP process",0, 0, 0);
    return false;
  }

  //Pmip process Running
  //Remove SFE Modules
  LOG_MSG_INFO1("startPMIPprocess: Removing SFE Module",0, 0, 0);
  ds_system_call("rmmod shortcut_fe_cm",
                 strlen("rmmod shortcut-fe_cm"));

  ds_system_call("rmmod shortcut_fe_ipv6",
                 strlen("rmmod shortcut_fe_ipv6"));

  ds_system_call("rmmod shortcut_fe",
                 strlen("rmmod shortcut_fe"));

  ds_system_call("ip6tables -I FORWARD -i bridge0 -o ip6gre+ -j ACCEPT",
               strlen("ip6tables -I FORWARD -i bridge0 -o ip6gre+ -j ACCEPT"));

  ds_system_call("iptables -I FORWARD -i bridge0 -o ip6gre+ -j ACCEPT",
               strlen("iptables -I FORWARD -i bridge0 -o ip6gre+ -j ACCEPT"));

  ds_system_call("ip6tables -I FORWARD -i bridge0 -o ip4gre+ -j ACCEPT",
               strlen("ip6tables -I FORWARD -i bridge0 -o ip4gre+ -j ACCEPT"));

  ds_system_call("iptables -I FORWARD -i bridge0 -o ip4gre+ -j ACCEPT",
               strlen("iptables -I FORWARD -i bridge0 -o ip4gre+ -j ACCEPT"));

  //if Tunnel mode is v4 then add iptables post Routing rule to stop source port NAT by Masquarade rule
  if(QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,"iptables -t nat -D POSTROUTING -p udp --dport %d -j ACCEPT", PMIP_RESERVED_PORT);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,"iptables -t nat -I POSTROUTING -p udp --dport %d -j ACCEPT", PMIP_RESERVED_PORT);
    ds_system_call(command, strlen(command));
  }


  //table for pmipv4 uplink Route for ip/gre Tunnel
  ds_system_call("sed -i /'201 pmipv4'/d /data/iproute2/rt_tables",strlen("sed -i /'201 pmipv4'/d /data/iproute2/rt_tables"));
  ds_system_call("echo 201 pmipv4 >> /data/iproute2/rt_tables",strlen("echo 201 pmipv4 >> /data/iproute2/rt_tables"));

  LOG_MSG_INFO1("startPMIPprocess: conf complete",0, 0, 0);
  return true;
}


/*===========================================================================
  FUNCTION stopPMIPprocess
==========================================================================*/
/*!
@brief
   Stops the PMIP deamon

 @return
      true - Sucess
      false -Failure
 /*=========================================================================*/
boolean QCMAP_Backhaul::stopPMIPprocess()
{
    char command[MAX_COMMAND_STR_LEN] = {0};
    QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    qmi_error_type_v01 qmi_err_num;
    int i = 0;
    char Kernel_ver[KERNEL_VERSION_LENGTH];

    if(QcMapBackhaulMgr == NULL)
    {
      LOG_MSG_ERROR("QcMapBackhaulMgr object is NULL", 0, 0, 0);
      return false;
    }

    //Update MAG conf file & Start MAG process <pmip6d>
    /*Update the MAG config file*/

    LOG_MSG_INFO1("stopPMIPprocess: enter", 0, 0, 0);

    ds_system_call("ip6tables -D FORWARD -i bridge0 -o ip6gre+ -j ACCEPT",
              strlen("ip6tables -D FORWARD -i bridge0 -o ip6gre+ -j ACCEPT"));

    ds_system_call("iptables -D FORWARD -i bridge0 -o ip6gre+ -j ACCEPT",
              strlen("iptables -D FORWARD -i bridge0 -o ip6gre+ -j ACCEPT"));


    ds_system_call("ip6tables -D FORWARD -i bridge0 -o ip4gre+ -j ACCEPT",
              strlen("ip6tables -D FORWARD -i bridge0 -o ip4gre+ -j ACCEPT"));

    ds_system_call("iptables -D FORWARD -i bridge0 -o ip4gre+ -j ACCEPT",
              strlen("iptables -D FORWARD -i bridge0 -o ip4gre+ -j ACCEPT"));

    //if Tunnel mode is v4 then delete iptables post Routing rule to stop source port NAT by Masquarade rule
    if(QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,"iptables -t nat -D POSTROUTING -p udp --dport %d -j ACCEPT", PMIP_RESERVED_PORT);
      ds_system_call(command, strlen(command));
    }

    ds_system_call("killall -15 pmip6d",
              strlen("killall -15 pmip6d"));

     if(QCMAP_Backhaul::checkProcessExist(PMIP_PROCESS) == true)
     {
       ds_system_call("killall -9 pmip6d",
                 strlen("killall -9 pmip6d"));
     }

     snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf %s", TEMP_PMIP_LOG_FILE);
     ds_system_call(command, strlen(command));

   /*Delete the stale Tunnel interfaces created if any*/
   for(i=1; i<PMIP_MAG_MAX_TUNL_IF; i++)
   {
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 tun del ip6gre%d",i);
      ds_system_call(command, strlen(command));
   }

   //insert SFE module on pmip disable
   if ( false == QcMapMgr->GetKernelVer(Kernel_ver) )
   {
     LOG_MSG_ERROR("Unable to get the kernel version info", 0, 0, 0);
     return false;
   }
   LOG_MSG_INFO1("stopPMIPprocess: Inserting SFE Module",0, 0, 0);
   snprintf(command,MAX_COMMAND_STR_LEN,
            "insmod /usr/lib/modules/%s/extra/shortcut-fe.ko",Kernel_ver);
   ds_system_call(command, strlen(command));

   snprintf(command,MAX_COMMAND_STR_LEN,
            "insmod /usr/lib/modules/%s/extra/shortcut-fe-ipv6.ko",Kernel_ver);
   ds_system_call(command, strlen(command));

   snprintf(command,MAX_COMMAND_STR_LEN,
            "insmod /usr/lib/modules/%s/extra/shortcut-fe-cm.ko",Kernel_ver);
   ds_system_call(command, strlen(command));

   LOG_MSG_INFO1("stopPMIPprocess: conf complete",0, 0, 0);
  return true;
}

/*===========================================================================
  FUNCTION DisablePmipMode
==========================================================================*/
/*!
@brief
   Disables the PMIP mode

 @return
      true - Sucess
      false -Failure
 /*=========================================================================*/
boolean QCMAP_Backhaul::DisablePmipMode(qmi_error_type_v01 *qmi_err_num, pmip_event_type action)
{

 /*Bring down All LAN ifaces including Bridge
    Remove all rules installed for routes
    Remove tunnel
    Kill MAG process <pmip6d>
    Init Bridge interface
    Start LAN ifaces
    Reset the <pmipmodeactive> Flag
    start radish if needed */

  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_LAN* default_lan_obj = GET_DEFAULT_LAN_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  if (!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR( "QcMapBackhaulWWANMgr pointer is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if(!default_lan_obj)
  {
    LOG_MSG_ERROR("Default LAN object is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  LOG_MSG_INFO1 ("DisablePmipMode:%d",0,0,0);
  if(!QcMapBackhaulMgr)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if (QCMAP_Backhaul::pmip_conf.pmipv6_mode_active == PMIP_MODE_DISABLED &&
      QCMAP_Backhaul::pmip_conf.pmipv4_mode_active == PMIP_MODE_DISABLED)
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_ERROR("\nPmip is Already Disabled\n",0,0,0);
    return false;
  }

/*
Remove all rules installed for routes
Remove tunnels created
Stop process
*/
  if(action == PMIP_STOP_EV)
  {
    QcMapBackhaulMgr->stopPMIPprocess();
  }
  else if (action == PMIP_DISABLE_EV)
  {
    default_lan_obj->PmipRestartLAN(qmi_err_num, action);
    if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
        QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
    {
    //start radish if needed - scenario where PMIP disabled but WWAN connected
    if(QcMapBackhaulMgr && (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN) &&
       QcMapBackhaulWWANMgr->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)
    {
      QcMapBackhaulMgr->StartRadish();
    }
  }
  }
  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
    //Reset the <pmipmodeactive> Flag
    QCMAP_Backhaul::pmip_conf.pmipv4_mode_active = PMIP_MODE_DISABLED;
    LOG_MSG_INFO1("DisablePmipMode: Set pmipv4_mode_active to 0", 0, 0, 0);
  }
  if (QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V6_V01 ||
      QCMAP_Backhaul::pmip_conf.pmip_mode_type == QCMAP_MSGR_IP_FAMILY_V4V6_V01)
  {
  //Reset the <pmipmodeactive> Flag
    QCMAP_Backhaul::pmip_conf.pmipv6_mode_active = PMIP_MODE_DISABLED;
  }
  LOG_MSG_INFO1("DisablePmipMode: Set pmip_mode_active to 0", 0, 0, 0);

  return true;
}

/*===========================================================================
  FUNCTION PmipQueryDevice
==========================================================================*/
/*!
@brief
   Acknowledges to PMIP deamon if the Device exists in QCMAP CDI or not

 @return
      true - Sucess
      false -Failure
 /*=========================================================================*/
boolean QCMAP_Backhaul::PmipQueryDevice(qcmap_pmip_info_t* dev_info)
{
  //check devices list and sendto socket with relavent info
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_cm_prefix_iid_info_t tempPrefixIidptr;
  ds_dll_el_t * node = NULL;
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_cm_client_data_info_t *clientInfo = NULL;
  qcmap_pmip_sock_msg_t  send_dev_info;
  boolean is_dev_found = false;
  struct sockaddr_un pmip_qcmap;
  int numBytes=0, len;
  char ip6_addr[INET6_ADDRSTRLEN];
  char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01]={0};

  LOG_MSG_ERROR("PmipQueryDevice Enter",0, 0, 0);


/*take the IID and search match
 if found fill details and send
 if not set false and send details*/
  memset(&send_dev_info, 0x0, sizeof(qcmap_pmip_sock_msg_t));
  memset(ip6_addr, 0x0, INET6_ADDRSTRLEN);
  if (!dev_info)
  {
    LOG_MSG_ERROR("PmipQueryDevice dev_info is NULL",0, 0, 0);
    return false;
  }
  else if (dev_info->pmip_mode != QCMAP_MSGR_IP_FAMILY_V6_V01 && dev_info->pmip_mode != QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    LOG_MSG_ERROR("PmipQueryDevice Invalid pmip mode provided %d",dev_info->pmip_mode, 0, 0);
    return false;
  }
  if (dev_info && dev_info->pmip_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    LOG_MSG_ERROR("PmipQueryDevice V6 mode device query",0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("PmipQueryDevice V4 mode device query",0, 0, 0);
  }
    if(!dev_info && dev_info->isValidMac)
    {
      LOG_MSG_ERROR("PmipQueryDevice NO mac address",0, 0, 0);
      return false;
    }
    ds_mac_addr_ntop((uint8_t *)dev_info->mac_addr, (char *)&mac_addr_str);


    LOG_MSG_INFO1("PmipQueryDevice MAc addr rcvd:%s",mac_addr_str, 0, 0);
    //tempPrefixIidptr.iid_ptr = (struct ps_in6_addr *)dev_info->v6_addr;
    node = ds_dll_search (addrList->addrListHead , (void*)dev_info->mac_addr,
                          QcMapMgr->qcmap_match_mac_address);

  if(node == NULL)
  {
      LOG_MSG_ERROR("PmipQueryDevice NO such device with MAc as:%s",mac_addr_str, 0, 0);
    is_dev_found = false;
  }
  else
  {
    clientInfo = ( qcmap_cm_client_data_info_t* )ds_dll_data(node);
    if (clientInfo == NULL)
    {
        LOG_MSG_ERROR("PmipQueryDevice - NO such device with MAc as:%s ",
                      mac_addr_str, 0, 0);

      is_dev_found = false;
    }
    else
    {

        LOG_MSG_INFO1("PmipQueryDevice Found device with MAc as:%s",mac_addr_str, 0, 0);
      is_dev_found = true;
    }
  }

  if(is_dev_found)
  {
    send_dev_info.pmip_event = QCMAP_PMIP_QUERY_DEVICE_RESP;

      send_dev_info.dev_info.isValidMac= true;
    send_dev_info.dev_info.if_id = clientInfo->device_type; /*fill it with dev_type*/
    memcpy(send_dev_info.dev_info.mac_addr,
           clientInfo->mac_addr,
           QCMAP_MSGR_MAC_ADDR_LEN_V01);
  }
  else
  {
    send_dev_info.pmip_event = QCMAP_PMIP_QUERY_DEVICE_RESP;
      send_dev_info.dev_info.isValidMac = false;
    }

  pmip_qcmap.sun_family = AF_UNIX;
  strlcpy(pmip_qcmap.sun_path, PMIP_QCMAP_FILE, sizeof(PMIP_QCMAP_FILE));
  len = strlen(pmip_qcmap.sun_path) + sizeof(pmip_qcmap.sun_family);


    LOG_MSG_INFO1("PmipQueryDevice senddevinfo: valid:%d,mac:%s",
          send_dev_info.dev_info.isValidMac,
          mac_addr_str,0);
  if((numBytes = sendto(pmip_qcmap_sockfd, (void *)&send_dev_info, sizeof(qcmap_pmip_sock_msg_t), 0,(struct sockaddr *)&pmip_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed to pmip_qcmap_ind_sockfd, errno:%d",errno, 0, 0);
    return false;
  }
  return true;
}


/*===========================================================================
  FUNCTION PmipDeleteDevice
==========================================================================*/
/*!
@brief
   When the device is deleted from QCMAP CDI, PMIP deamon will be notified

 @return
      true - Sucess
      false -Failure
 /*=========================================================================*/
boolean QCMAP_Backhaul::PmipDeleteDevice(qcmap_cm_client_data_info_t* conn_dev)
{
  //sendto socket with relavent info
  qcmap_pmip_sock_msg_t  send_dev_info;
  struct sockaddr_un pmip_qcmap;
  int numBytes=0, len;

  memset(&send_dev_info, 0x0, sizeof(qcmap_pmip_sock_msg_t));

  if(conn_dev == NULL)
  {
    return false;
  }
    send_dev_info.pmip_event = QCMAP_PMIP_DEL_DEVICE;
    send_dev_info.dev_info.if_id = conn_dev->device_type; /*fill it with dev_type*/
    memcpy(send_dev_info.dev_info.mac_addr,
           conn_dev->mac_addr,
           QCMAP_MSGR_MAC_ADDR_LEN_V01);

  LOG_MSG_INFO1("PmipDeleteDevice senddevinfo: mac:%s",
          send_dev_info.dev_info.mac_addr, 0, 0);

  pmip_qcmap.sun_family = AF_UNIX;
  strlcpy(pmip_qcmap.sun_path, PMIP_QCMAP_IND_FILE, sizeof(PMIP_QCMAP_IND_FILE));
  len = strlen(pmip_qcmap.sun_path) + sizeof(pmip_qcmap.sun_family);

  if((numBytes = sendto(pmip_qcmap_ind_sockfd, (void *)&send_dev_info, sizeof(qcmap_pmip_sock_msg_t), 0,(struct sockaddr *)&pmip_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed to pmip_qcmap_ind_sockfd , errno:%d err msg %s",errno, strerror(errno), 0);
    return false;
  }

 return true;
}


/*=====================================================
  FUNCTION GetBackhaulObjectForInterface
======================================================*/
/*!
@brief
 Get's a Backhaul object related to rmnet_iface.

@parameters
  - rmnet_iface name

@return
  - Backhaul Object

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
QCMAP_Backhaul* QCMAP_Backhaul::GetBackhaulObjectForWWANInterface
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
  const char *rmnet_iface
 )
{
  QCMAP_Backhaul *pBackhaul = NULL;
  boolean flag = FALSE;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qmi_error_num;

  LOG_MSG_INFO1("Find Backhaul for %s", rmnet_iface, 0,0);

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaul = GET_BACKHAUL_OBJ_AND_INC;
    if (pBackhaul && pBackhaul->QcMapBackhaulWWAN &&
        (pBackhaul->QcMapBackhaulWWAN->GetDeviceName(qcmap_cm_get_handle(), ip_family,
                                          devname, &qmi_error_num) != QCMAP_CM_ERROR))
    {
      if (strncmp(rmnet_iface, devname, strlen(devname)) == 0)
      {
        flag = TRUE;
        break;
      }
    }
  }

  if (flag == TRUE)
  {
    LOG_MSG_INFO1("Found Backhaul object (%p) for %s", pBackhaul, rmnet_iface, 0);
    return pBackhaul;
  }
  else
  {
    LOG_MSG_ERROR("No Backhaul object found for %s",rmnet_iface, 0, 0);
    return NULL;
  }
}

