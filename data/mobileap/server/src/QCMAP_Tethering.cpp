/*====================================================

FILE:  QCMAP_Tethering.cpp

SERVICES:
   QCMAP Connection Manager USB Tethring Specific Implementation

=====================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when          who        what, where, why
  --------   ---        -------------------------------------------------------
  08/22/14   vm         9x45

==========================================================*/
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
#include "QCMAP_Backhaul_Ethernet.h"
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "ds_qmi_qcmap_msgr.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_Virtual_LAN.h"
#include "QCMAP_L2TP.h"

/*
#define TETH_DEBUG 1

#ifdef TETH_DEBUG
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
bool QCMAP_Tethering::flag= false;
QCMAP_Tethering* QCMAP_Tethering::object=NULL;


/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and returns instance of class QCMAP_Tethering

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
QCMAP_Tethering* QCMAP_Tethering::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : TETHERING",0, 0, 0);
    object = new QCMAP_Tethering();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}
/*==========================================================
                             Class Definitions
  =========================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes Tethering variables.

@parameters

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_Tethering::QCMAP_Tethering()
{
  char dunDongleMode[MAX_STRING_LENGTH] = {0};
  bzero(&this->tethered_conf,sizeof(this->tethered_conf));

  this->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr
               = ntohl(inet_addr(TETH_IFACE_LL_ADDR1));
  this->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].gateway_addr
               = ntohl(inet_addr(TETH_IFACE_LL_ADDR2));

  /* Initialize Remote USB IP */
  this->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].remote_link_ip4_address = 0;
  this->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].remote_link_ip4_address = 0;
  /* Initialize MAC address*/
  memset (tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].remote_link_mac_addr,0,
          QCMAP_MSGR_MAC_ADDR_LEN_V01);
  memset (tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].remote_link_mac_addr,0,
          QCMAP_MSGR_MAC_ADDR_LEN_V01);

  /* Initialize Subnet Mask. */
  this->ll_subnet_mask = ntohl(inet_addr(LL_SUBNET_MASK));

  /*Initialize dun dongle mode*/
  if (QCMAP_Tethering::SetGetDunDongleModeFromXML(GET_VALUE, dunDongleMode,
                                              MAX_STRING_LENGTH))
  {
    this->dun_dongle_mode = atoi(dunDongleMode);
  }
  else
  {
    LOG_MSG_ERROR("Get DunDongleMode status Failed and assigned default value",
                  0,0,0);
    this->dun_dongle_mode = QCMAP_DEFAULT_DUN_DONGLE_MODE;
  }

  memset(this->ppp_ipv6_addr, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  memset(this->ppp_ipv6_iid, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief

@parameters

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_Tethering::~QCMAP_Tethering()
{
  LOG_MSG_INFO1("Destroying Object: TETHRING",0,0,0);
  flag=false;
  object=NULL;
  return;
}


/*===========================================================================
  FUNCTION DelConntrack
==========================================================================*/
/*!
@brief
  Deletes the conntrack entries associated with USB/WLAN/ETH clients

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
/*=========================================================================*/
void QCMAP_Tethering::DelConntrack( uint32 ipv4addr )
{
  uint32 client_ip;
  char ipaddr[INET_ADDRSTRLEN];
  char command[MAX_COMMAND_STR_LEN];

  memset(ipaddr, 0, INET_ADDRSTRLEN);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  client_ip = ipv4addr;
  inet_ntop(AF_INET,(const void*)&client_ip, (char *)&ipaddr, INET_ADDRSTRLEN);

  LOG_MSG_INFO1("QCMAP_Tethering::DelConntrack(), client_ip %d",ipaddr,0,0);

  if (client_ip)
  {
    snprintf(command,MAX_COMMAND_STR_LEN,"echo Deleting Client Conntrack IP:%s > /dev/kmsg",(char *)&ipaddr);
    ds_system_call(command,strlen(command));
    snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D --orig-src %s",(char *)&ipaddr);
    ds_system_call(command, strlen(command));
    snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D --reply-src %s",(char *)&ipaddr);
    ds_system_call(command, strlen(command));
  }
}
/*===========================================================================
  FUNCTION SetTethRouteV6ParamsOnIface
==========================================================================*/
/*!
@brief
   Sets Router related IPv6 params on given iface

@parameters
  boolean enableV6
  char* iface

@return
  None
@note
- Dependencies
- None

- Side Effects
- None
*/
void QCMAP_Tethering::SetTethRouteV6ParamsOnIface(boolean enableV6, char* iface)
{
  char                        command[MAX_COMMAND_STR_LEN];
  qcmap_v6_sock_iface_t sock_iface_type;
/*------------------------------------------------------------------------*/
  if (!strcmp(iface,ECM_IFACE))
  {
    sock_iface_type = QCMAP_V6_SOCK_ECM;
  }
  else if (!strcmp(iface,ETH_IFACE))
  {
    sock_iface_type = QCMAP_V6_SOCK_ETH;
  }
  else
  {
    LOG_MSG_ERROR("iface %s is not supported\n",iface,0,0);
    return;
  }

  if (enableV6)
  {
      QCMAP_Backhaul::JoinMulticastGroup(sock_iface_type, iface);
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
               iface);
      ds_system_call(command, strlen(command));
      /* Dont set the dft route when information is recevied in an RA*/
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
               iface);
      ds_system_call(command, strlen(command));
    }
    else
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
               iface);
      ds_system_call(command, strlen(command));
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
               iface);
      ds_system_call(command, strlen(command));
      /* Set the dft route when information is recevied in an RA*/
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
               iface);
      ds_system_call(command, strlen(command));
    }
}
/*===========================================================================
  FUNCTION StartDHCPDOnIfae
==========================================================================*/
/*!
@brief
   Starts DHCP client on given iface

@parameters
  char* iface

@return
  None
@note
- Dependencies
- None

- Side Effects
- None
*/
void QCMAP_Tethering::AcceptIPAddrOnIfae(char* iface,
                                       boolean enableV4,
                                       boolean enableV6)
{
  char                        command[MAX_COMMAND_STR_LEN];
/*------------------------------------------------------------------------*/
   /* Obtain and assign IP address via dhcpcd daemon */
   LOG_MSG_INFO1("Running DHCP client on %s, enableV4 %d, enableV6 %d\n",iface,enableV4,enableV6);
   if(enableV4)
   {
     snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o\
               domain_name_servers --noipv4ll -b -G",iface);
     ds_system_call(command, strlen(command));
   }
     if(enableV6)
     {
       snprintf(command, MAX_COMMAND_STR_LEN,
                "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra", iface);
       ds_system_call(command, strlen(command));
       /* Dont set the dft route when information is recevied in an RA*/
       snprintf(command, MAX_COMMAND_STR_LEN,
                "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
              iface);
       ds_system_call(command, strlen(command));
     }
   }
/*===========================================================================
  FUNCTION SetupTetheredLink
==========================================================================*/
/*!
@brief
   Sets up the USB tethered link as RNDIS or ECM.

@parameters
  qcmap_qti_tethered_link_type    tethered_link
  int                        *err_num

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
 boolean QCMAP_Tethering::SetupTetheredLink
(
   qcmap_qti_tethered_link_type    tethered_link,
   int                        *err_num
)
{
  char                        command[MAX_COMMAND_STR_LEN];
  char                        temp_buf[MAX_COMMAND_STR_LEN];
  char                        link_type[16] = {0};
  char                        gwIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  in_addr                     subnet_mask, gateway,gateway_subnet, ppp_addr,apps_lan_addr;
  qmi_error_type_v01          qmi_err_num;
  int j = 0;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  uint32_t time_out = (PPP_DEV_INIT_DELAYS_MAX/PPP_DEV_INIT_DELAY);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  qcmap_msgr_lan_config_v01* prev_lan_cfg = NULL;
  QCMAP_Virtual_LAN* QcMapVLANMgr=QCMAP_Virtual_LAN::Get_Instance(false);
  /* Create USB Tethering Object here */
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(true);
  QCMAP_L2TP* QcMapL2TPMgr=QCMAP_L2TP::Get_Instance(false);
  char enable[MAX_STRING_LENGTH]={0};
  char enablev6[MAX_STRING_LENGTH]={0};
  qcmap_msgr_cradle_mode_v01 cradle_mode;
  qcmap_msgr_ethernet_mode_v01 eth_backhaul_mode;
  char iface[QCMAP_MSGR_DEVICE_NAME_MAX_V01];
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
  char iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  qcmap_nl_sock_msg_t nl_buf;
  bool is_vlan_enabled = false;
/*------------------------------------------------------------------------*/
  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(GET_VALUE,
                                             QCMAP_TETH_CRADLE_BACKHAUL,
                                             (void *)&cradle_mode);
  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(GET_VALUE,
                                             QCMAP_TETH_ETH_BACKHAUL,
                                             (void *)&eth_backhaul_mode);

  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,GET_VALUE,enable,MAX_STRING_LENGTH);
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,GET_VALUE,enablev6,MAX_STRING_LENGTH);
  qmi_err_num = QMI_ERR_NONE_V01;

  if(NULL == QCMAPLANMgr)
  {
    LOG_MSG_ERROR("Default LAN object is NULL", 0, 0, 0);
    *err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
  {
    if (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == true)
    {
      LOG_MSG_ERROR("Ethernet Link Already Enabled",0,0,0);
      *err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }
  }
  else
  {
    if (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true)
    {
      LOG_MSG_ERROR("USB Link Already Enabled",0,0,0);
      *err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }
  }

  LOG_MSG_INFO1("Tethered link up impl",0,0,0);

  /* PPP should not be enabled in AP-STA Bridge Mode */
  if ( (tethered_link == QCMAP_QTI_TETHERED_LINK_PPP) && (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
  {
    if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
      *err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    /* Check if we have reserved IP. if not return error.*/
    if(lan_cfg->ppp_reserved_ip == 0)
    {
      LOG_MSG_ERROR("Reserved IP not available",0,0,0);
      *err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
      return false;
    }

    if((prev_lan_cfg = QCMAPLANMgr->GetPrevLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get prev LAN config", 0, 0, 0);
      *err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    apps_lan_addr.s_addr = htonl(prev_lan_cfg->gw_ip);
    strlcpy(gwIP, inet_ntoa(apps_lan_addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    QcMapTetheringMgr->SetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1, true);
    QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type = QCMAP_QTI_TETHERED_LINK_PPP;

    memset(command, 0, MAX_COMMAND_STR_LEN);

    ppp_addr.s_addr = htonl(lan_cfg->ppp_reserved_ip);

    /* Start the PPP Daemon. */
    /* Refer pppd man page for more info on arguements. */
    /*
     * crtscts --> Specifies that pppd should set the serial port to use
     * hardware flow control using the RTS and CTS signals in the RS-232
     * interface.
     * lock --> Locks the serial device /dev/ttyGS1.
     * %s:%s --> Our IP:Host IP.
     * %s --> Serial device.
     * 115200 --> Baud rate.
     * silent --> PPP negotiation will be started by host and not by us.
     * debug --> Enable debugging.
     * +ipv6 --> Enable IPV6.
     * disconnect <script> --> execute the script when the dun call is
     * disconnected.
     */
    snprintf(command, MAX_COMMAND_STR_LEN, "pppd crtscts lock \
           %s:%s %s 115200 ms-dns %s \
           silent debug +ipv6 disconnect \"/usr/sbin/chat -V -f /etc/disconnect\"",
           TETH_IFACE_LL_ADDR1, inet_ntoa(ppp_addr),USB_DUN_TTY_PORT,gwIP);
    ds_system_call( command, strlen(command));

    /* Wait till PPP interface is created.. */
    snprintf(command, MAX_COMMAND_STR_LEN, "/proc/sys/net/ipv4/conf/ppp0");
    while ( j++ < time_out )
    {
      std::ifstream ifile( command );
      if ( ifile )
      {
        break;
      }
      usleep( PPP_DEV_INIT_DELAY );
    }

    /* Restart RADISH. */
    if(QcMapBackhaulMgr)
    {
      QcMapBackhaulMgr->RestartRadish();
    }
    /* Store the IPV4 address. */
    /* Update the connected devices information. */
    memset(&nl_buf,0,sizeof(qcmap_nl_sock_msg_t));
    nl_buf.nl_addr.isValidIPv4address = true;
    nl_buf.nl_addr.isValidIPv6address = false;
    nl_buf.nl_addr.ip_addr = htonl(lan_cfg->ppp_reserved_ip);
    //Convert the MAC from char to hex
    if(!ds_mac_addr_pton((char*)PPP_CLIENT_MAC_ADDR, nl_buf.nl_addr.mac_addr))
    {
      LOG_MSG_ERROR("Store PPP IPv4 address - Error in MAC address conversion",
                    0,0,0);
    }

    QcMapTetheringMgr->SetUSBMac(nl_buf.nl_addr.mac_addr);
    if(QcMapMgr->MatchMacAddrInList(&nl_buf) == 0)
    {
      LOG_MSG_INFO1("No match found for the USB MAC,"
                    "so add a linked list node\n",0, 0, 0);
      if (!(QCMAP_ConnectionManager::AddNewDeviceEntry(
           (void*)nl_buf.nl_addr.mac_addr,
           QCMAP_MSGR_DEVICE_TYPE_USB_V01,
           &nl_buf.nl_addr.ip_addr,
           nl_buf.nl_addr.ip_v6_addr,
           0,
           nl_buf.nl_addr.isValidIPv4address,
           nl_buf.nl_addr.isValidIPv6address)))
      {
        LOG_MSG_ERROR("Error in adding a new device entry ",
                      0, 0, 0);
      }
    }
    return true;
  }

  QCMAP_Backhaul_Cradle::GetCradleMode(&cradle_mode,&qmi_err_num);
  if ((tethered_link == QCMAP_QTI_TETHERED_LINK_PPP ||
       (tethered_link == QCMAP_QTI_TETHERED_LINK_ECM
        && cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01) )
       && (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true))
  {
    LOG_MSG_ERROR("Can't enable DUN/ETH/cradle wan bridge when in AP-STA Brigde Mode",0,0,0);
    *err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

/*-----------------------------------------------------------------------
  Assign the interface names
------------------------------------------------------------------------*/
  switch(tethered_link)
  {
    case QCMAP_QTI_TETHERED_LINK_RNDIS:
    {
      QcMapTetheringMgr->SetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1, true);
      QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type = QCMAP_QTI_TETHERED_LINK_RNDIS;
      strlcpy(link_type, "rndis",16);

      /* Check if VLAN is enabled for RNDIS interface */
      if (QCMAPLANMgr->GetVLANIfaceMask() & RNDIS_VLAN_ENABLED)
      {
        LOG_MSG_INFO1("VLAN is enabled for RNDIS in bridge0", 0,0,0);
        is_vlan_enabled = true;
      }
      break;
    }
    case QCMAP_QTI_TETHERED_LINK_ECM:
    {
      QcMapTetheringMgr->SetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1, true);
      QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type = QCMAP_QTI_TETHERED_LINK_ECM;
      strlcpy(link_type, "ecm",16);

      /* Check if VLAN is enabled for ECM interface */
      if (QCMAPLANMgr->GetVLANIfaceMask() & ECM_VLAN_ENABLED)
      {
        LOG_MSG_INFO1("VLAN is enabled for ECM in bridge0", 0,0,0);
        is_vlan_enabled = true;
      }

      /* Enable v6 for ecm0, we disable this in bringDownUSB for cradle */
      snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
               ECM_IFACE);
      ds_system_call(command, strlen(command));
      if (cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01)
      {
        QCMAP_Tethering::SetTethRouteV6ParamsOnIface((boolean)atoi(enablev6),
                                                     ECM_IFACE);
      }

      if (cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01)
      {
        //flush IPv6 address
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 BRIDGE_IFACE);
        ds_system_call(command, strlen(command));

        if ((boolean)atoi(enablev6))
        {
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
          QCMAP_Backhaul::JoinMulticastGroup(QCMAP_V6_SOCK_ECM, BRIDGE_IFACE);
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
        else
        {
          snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                   BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
      }
      break;
    }
    case QCMAP_QTI_TETHERED_LINK_ETH:
    {
      QcMapTetheringMgr->SetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2, true);
      QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type = QCMAP_QTI_TETHERED_LINK_ETH;
      strlcpy(link_type, ETH_LINK,16);

      /* Check if VLAN is enabled for ETH interface */
      if (QCMAPLANMgr->GetVLANIfaceMask() & ETH_VLAN_ENABLED)
      {
        LOG_MSG_INFO1("VLAN is enabled for Eth in bridge0", 0,0,0);
        is_vlan_enabled = true;
      }

      /* Enable v6 for eth0 */
      snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
               ETH_IFACE);
      ds_system_call(command, strlen(command));
      /* Enable v6 for eth0 */
      snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
               ETH_IFACE);
      ds_system_call(command, strlen(command));

      /*Stop any dhcpcd running on eth0 and flush IP address*/
      snprintf(command, MAX_COMMAND_STR_LEN,
               "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
               ETH_IFACE);
      ds_system_call(command, strlen(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",
               ETH_IFACE);
      ds_system_call(command, strlen(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -4 addr flush dev %s", ETH_IFACE);
      ds_system_call(command, strlen(command));

      if (eth_backhaul_mode == QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
      {
        QCMAP_Tethering::SetTethRouteV6ParamsOnIface((boolean)atoi(enablev6),
                                                     ETH_IFACE);
      }
      break;
    }
    default:
     break;
  }

  if (tethered_link == QCMAP_QTI_TETHERED_LINK_RNDIS ||
      (tethered_link == QCMAP_QTI_TETHERED_LINK_ETH
       && eth_backhaul_mode == (int) QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01) ||
      ((cradle_mode == QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01 || cradle_mode == QCMAP_MSGR_CRADLE_LAN_ROUTER_V01
       || cradle_mode == QCMAP_MSGR_CRADLE_DISABLED_V01)
       && tethered_link == QCMAP_QTI_TETHERED_LINK_ECM))
  {

    /**
     * Get IP Passthrough flag from XML
     * If the flag is set then enable passthrough
    */
    memset(&passthrough_config, 0, sizeof(passthrough_config));
    if(QCMAPLANMgr->GetIPPassthroughConfig(&enable_state,&passthrough_config,&qmi_err_num))
    {
      if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_UP_V01)
      {
        /* For Ethernet client, it is mandatory that mac address is non-empty
         * otherwise passthrough is not enabled.
         */
        if ((tethered_link == QCMAP_QTI_TETHERED_LINK_RNDIS) ||
            (tethered_link == QCMAP_QTI_TETHERED_LINK_ECM) ||
            (tethered_link == QCMAP_QTI_TETHERED_LINK_ETH &&
            QCMAP_LAN::check_non_empty_mac_addr
            (passthrough_config.mac_addr, mac_addr_string)))
        {
          /* If mac addr is not empty*/
          if (QCMAPLANMgr->EnableIPPassthrough(mac_addr_string,
                                               passthrough_config.client_device_name,
                                               passthrough_config.device_type))
          {
            LOG_MSG_INFO1("IP Passthrough started\n",0,0,0);
          }
          else
          {
            LOG_MSG_INFO1("Enable IP Passthrough Error: 0x%x", qmi_err_num,0,0);
          }
          QcMapMgr->dont_bringdown_backhaul = false;
        }
      }
      else
      {
        LOG_MSG_INFO1("Passthrough Not started\n",0,0,0);
      }
    }
    else
    {
      LOG_MSG_INFO1("Get IP Passthrough Flag failed\n",0,0,0);
    }

    /*------------------------------------------------------------------------
      Assign a pre-defined IP address to the RNDIS/ECM gateway interface on A5
      -------------------------------------------------------------------------*/
    strlcpy(command, "ifconfig ", MAX_COMMAND_STR_LEN);

    snprintf(temp_buf, MAX_COMMAND_STR_LEN, "%s0 ", link_type);
    strlcat(command, temp_buf, MAX_COMMAND_STR_LEN);

    if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
      gateway.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].gateway_addr);
    else
      gateway.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr);
    strlcat(command, inet_ntoa(gateway), MAX_COMMAND_STR_LEN);

    strlcat(command, " netmask ", MAX_COMMAND_STR_LEN);

    subnet_mask.s_addr = htonl(QcMapTetheringMgr->ll_subnet_mask);
    strlcat(command, inet_ntoa(subnet_mask), MAX_COMMAND_STR_LEN);

    ds_system_call( command, strlen(command));
    /*-----------------------------------------------------------------------
      Delete the subnet based route to USB interface
    --------------------------------------------------------------------------*/
    strlcpy(command, "route del -net ", MAX_COMMAND_STR_LEN);

    if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
      gateway_subnet.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].gateway_addr &
                                    QcMapTetheringMgr->ll_subnet_mask);
    else
      gateway_subnet.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr &
                                    QcMapTetheringMgr->ll_subnet_mask);
    strlcat(command, inet_ntoa(gateway_subnet), MAX_COMMAND_STR_LEN);

    strlcat(command, " netmask ", MAX_COMMAND_STR_LEN);

    subnet_mask.s_addr = htonl(QcMapTetheringMgr->ll_subnet_mask);
    strlcat(command, inet_ntoa(subnet_mask), MAX_COMMAND_STR_LEN);

    snprintf(temp_buf, MAX_COMMAND_STR_LEN, " dev %s0 ", link_type);
    strlcat(command, temp_buf, MAX_COMMAND_STR_LEN);

    ds_system_call(command, strlen(command));
    printf("command %s \n",command);

    /* First delete the link-local route. */
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev %s0",
             link_type);
    ds_system_call(command, strlen(command));

    /* Add phy interface iff vlan interface is enabled on bridge0 */
    if (is_vlan_enabled == false)
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s0",
               BRIDGE_IFACE, link_type);
      ds_system_call(command, strlen(command));
    }
    else
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s0",
               BRIDGE_IFACE, link_type);
      ds_system_call(command, strlen(command));
    }

    if(tethered_link == QCMAP_QTI_TETHERED_LINK_RNDIS)
    {
      /*After starting the dhcp server, we need to indicate to the kernel to
        start accepting dhcp requestes from host pc. This is required to avoid
        dhcp request going for a backoff timer at the host PC*/
      ds_system_call("echo 1 > /sys/class/android_usb/f_rndis/rx_trigger",\
                     strlen("echo 1 > /sys/class/android_usb/f_rndis/rx_trigger"));
    }

    /*------------------------------------------------------------------------
       Enable IP forwarding
    ------------------------------------------------------------------------*/
    ds_system_call("echo 1 > /proc/sys/net/ipv4/ip_forward",
                   strlen("echo 1 > /proc/sys/net/ipv4/ip_forward"));

    /* Enter Default Rules for AP-STA Bridge Mode */

    if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true)
    {
      QCMAPLANMgr->AddEbtablesUSBRulesForBridgeMode();
    }

    /* Dont enable Multicast services for AP-STA Bridge mode */
    if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.dlna_config &&
      (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
      if(!QcMapMediaServiceMgr->EnableDLNA(&qmi_err_num))
      {
        LOG_MSG_ERROR("Cannot enable DLNA, error: %d.", qmi_err_num, 0, 0);
      }
    }

    if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.upnp_config &&\
      (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
      if(!QcMapMediaServiceMgr->EnableUPNP(&qmi_err_num))
      {
        LOG_MSG_ERROR("Cannot enable UPnP, error: %d.", qmi_err_num, 0, 0);
      }
    }

    snprintf(iface_name, QCMAP_MAX_IFACE_NAME_SIZE_V01, "%s0", link_type);
    if(QcMapVLANMgr)
    {
      QcMapVLANMgr->AddDeleteVLANOnIface(iface_name, true);
    }
    if(QcMapL2TPMgr)
    {
      QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(iface_name, true);
    }
  }
  else if ((tethered_link == QCMAP_QTI_TETHERED_LINK_ETH)
            &&
           (eth_backhaul_mode == (int) QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01))
  {
    //unbridge eth0 and bridge0
     LOG_MSG_INFO1("Teth link %d, eth_backhaul_mode %d\n",
                   tethered_link,eth_backhaul_mode, 0);
     QcMapBackhaulEthMgr=QCMAP_Backhaul_Ethernet::Get_Instance(true);
     snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s %s0",
                BRIDGE_IFACE, link_type);
     ds_system_call(command, strlen(command));
     QCMAP_Tethering::AcceptIPAddrOnIfae(ETH_IFACE,
                                        (boolean)atoi(enable),
                                        (boolean)atoi(enablev6));
  }
  else //WAN Mode
  {
    QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(true);
    switch (cradle_mode)
    {
    case QCMAP_MSGR_CRADLE_WAN_ROUTER_V01:
      //unbridge ecm0 and bridge0
      snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s %s0",
                BRIDGE_IFACE, link_type);
      ds_system_call(command, strlen(command));
      QCMAP_Tethering::AcceptIPAddrOnIfae(ECM_IFACE,
                                        (boolean)atoi(enable),
                                        (boolean)atoi(enablev6));
      break;
    case QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01:
      QCMAPLANMgr->StopDHCPD();
      /* Stop RADISH. */
      QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
      if (QcMapBackhaulMgr)
      {
        QcMapBackhaulMgr->StopRadish();
      }

      snprintf( command, MAX_COMMAND_STR_LEN, "brctl addif %s %s0",
                BRIDGE_IFACE, link_type);
      ds_system_call(command, strlen(command));

      /* Delete old IP address and default route*/
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -4 addr flush dev %s", BRIDGE_IFACE);
      ds_system_call(command, strlen(command));

      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s 169.254.3.1 netmask 255.255.255.0 up", ECM_IFACE);
      ds_system_call(command, strlen(command));

      snprintf(command, MAX_COMMAND_STR_LEN, "ip route del 169.254.3.1/24 dev %s",ECM_IFACE);
      ds_system_call(command, strlen(command));

      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev %s",ECM_IFACE);
      ds_system_call(command, strlen(command));

      /* Obtain and assign IP address via dhcpcd daemon */
      LOG_MSG_INFO1("Running DHCP client on bridge0\n",0,0,0);
      if((boolean)atoi(enable))
      {
        snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o\
                  domain_name_servers --noipv4ll -b -G", BRIDGE_IFACE);
        ds_system_call(command, strlen(command));

        if((boolean)atoi(enablev6))
        {
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra", BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
        }
      }
      break;
    default:
        LOG_MSG_ERROR("Unsupported Cradle Mode %d, in usb mode %d",cradle_mode, tethered_link, 0);
        delete QcMapBackhaulCradleMgr;
    }
  }

  if (tethered_link == QCMAP_QTI_TETHERED_LINK_RNDIS ||
      (( cradle_mode == QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01 ||
         cradle_mode == QCMAP_MSGR_CRADLE_LAN_ROUTER_V01 ||
         cradle_mode == QCMAP_MSGR_CRADLE_DISABLED_V01)
       && tethered_link == QCMAP_QTI_TETHERED_LINK_ECM))
  {
    /*Populating CDI based on bridge fdb info and neighbor table*/
    if ( tethered_link == QCMAP_QTI_TETHERED_LINK_RNDIS )
    {
      strlcpy ( iface, RNDIS_IFACE , QCMAP_MSGR_INTF_LEN);
    }
    else if( tethered_link == QCMAP_QTI_TETHERED_LINK_ECM )
    {
      strlcpy ( iface, ECM_IFACE , QCMAP_MSGR_INTF_LEN);
    }
    QcMapTetheringMgr->UpdateMACandIPOnPlugIn( iface );
  }

  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr is NULL ", 0, 0, 0);
    return false;
  }

  if ( QcMapMgr && QcMapMgr->IsDynamicClockVotingNeeded())
  {
    if(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
    {
      if((((QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)||
          (QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)) &&
          (QcMapMgr->IsRatLteFdd()) && (QcMapMgr->isBWLteCAT4())) ||
          (QcMapMgr->CheckWlanClientConnected()))
      {
        QcMapMgr->SetUSBClockFreq(true);
      }
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION BringDownTetheredLink
==========================================================================*/
/*!
@brief
  Brings down the USB tethered RNDIS or ECM link.

@parameters
  qcmap_qti_tethered_link_type    tethered_link
  int                        *err_num
  void                     *softApHandle

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
boolean QCMAP_Tethering::BringDownTetheredLink
(
   qcmap_qti_tethered_link_type  tethered_link,
   void                     *softApHandle,
   int                      *err_num
)
{
  char                        command[MAX_COMMAND_STR_LEN];
  char                        link_type[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2] = {0};
  uint8_t                     *mac_addr;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Virtual_LAN* QcMapVLANMgr=QCMAP_Virtual_LAN::Get_Instance(false);
  QCMAP_L2TP* QcMapL2TPMgr=QCMAP_L2TP::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  qcmap_cm_teth_index_type other_index = QCMAP_MSGR_TETH_LINK_INDEX_NONE;
  char temp[MAX_COMMAND_STR_LEN]="";  /*for storing string readed from file*/
  FILE *fp1,*fp2;
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_msgr_ip_passthrough_config_v01 passthrough_config;
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state;
  boolean mac_addr_non_empty = false;
  boolean wan_mode = false;
  char    iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  /*------------------------------------------------------------------------*/

  if (QcMapTetheringMgr == NULL)
  {
    LOG_MSG_ERROR("Tethering is Not enabled yet.", 0, 0, 0);
    *err_num = QMI_ERR_INTERNAL_V01;
    return false;
  } else if(NULL == QCMAPLANMgr) {
    LOG_MSG_ERROR("Default LAN object is NULL", 0, 0, 0);
    *err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if ( tethered_link == QCMAP_QTI_TETHERED_LINK_PPP)
  {
    /* Kill the PPP Daemon. */
    ds_system_call("killall -TERM pppd", strlen("killall -TERM pppd"));
    if(!QCMAP_Backhaul::IsPPPkilled())
    {
      LOG_MSG_ERROR("PPP still running check. Kill forcefully", 0, 0, 0);
      /* Kill forcefully. */
      ds_system_call("killall -KILL pppd", strlen("killall -KILL pppd"));
    }

    QcMapTetheringMgr->SetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1, false);
    QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type = QCMAP_QTI_TETHERED_LINK_NONE;

    /* Restart RADISH. */
    if(QcMapBackhaulMgr)
    {
      QcMapBackhaulMgr->RestartRadish();
    }
    /* Stop services before bringing down bridge so byebye messages are sent */
    if((QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01) &&
       (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == false))
    {
      if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.upnp_config)
      {
        QcMapMediaServiceMgr->InterimDisableUPNP(&qmi_err_num);
      }

      if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.dlna_config)
      {
        QcMapMediaServiceMgr->InterimDisableDLNA(&qmi_err_num);
      }
    }

    /* Delete delegated prefix. */
    if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated &&
        (QCMAP_Backhaul_WWAN::SendDeleteDelegatedPrefix(true,
                                               QcMapTetheringMgr->ppp_ipv6_addr,
                                               &qmi_err_num) == QCMAP_CM_ERROR))
    {
      LOG_MSG_ERROR("Error: Unable flush prefix's %d", qmi_err_num, 0, 0);
    }

    /* Delete Device Info. */
    mac_addr = QcMapTetheringMgr->GetUSBMac();
    QcMapMgr->DeleteDeviceEntryInfo(QcMapTetheringMgr->GetUSBMac(),NULL);
    if (mac_addr)
    {
      /* Clear USB MAC */
      memset(mac_addr, 0, QCMAP_MSGR_MAC_ADDR_LEN_V01);
    }

    /* Clear PPP IPV6 address. */
    memset(QcMapTetheringMgr->ppp_ipv6_addr, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    memset(QcMapTetheringMgr->ppp_ipv6_iid, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

  if (!QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2))
    delete QcMapTetheringMgr;

    return true;
  }

  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true && (tethered_link != QCMAP_QTI_TETHERED_LINK_ETH))
  {
    QCMAPLANMgr->DelEbtablesUSBRulesForBridgeMode();
  }

  if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
  {
    other_index = QCMAP_MSGR_TETH_LINK_INDEX1;
    QcMapTetheringMgr->SetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2, false);
  }
  else
  {
    other_index = QCMAP_MSGR_TETH_LINK_INDEX2;
    QcMapTetheringMgr->SetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1, false);
  }

  /* Stop services before bringing down bridge so byebye messages are sent */
  if((QcMapWLANMgr&& QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01) &&
      (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false) &&
      (QcMapTetheringMgr->GetTethLinkEnable(other_index) == false))
  {
    if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.upnp_config)
    {
      QcMapMediaServiceMgr->InterimDisableUPNP(&qmi_err_num);
    }

    if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.dlna_config)
    {
      QcMapMediaServiceMgr->InterimDisableDLNA(&qmi_err_num);
    }
  }

/*-----------------------------------------------------------------------
  Assign the interface names
------------------------------------------------------------------------*/
  if ( tethered_link == QCMAP_QTI_TETHERED_LINK_RNDIS )
  {
    strlcpy(link_type, "rndis", DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  }
  else if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ECM )
  {
    strlcpy(link_type, "ecm",DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  }
  else if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
  {
    strlcpy(link_type, ETH_LINK,DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  }

  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s0",
           BRIDGE_IFACE, link_type);
  ds_system_call(command, strlen(command));

  if ((NULL != QcMapBackhaulEthMgr) &&
         QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                        QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
  {
    /* Process Disalloc and delete Eth Backhaul object*/
    if (tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
    {
      LOG_MSG_INFO1("Disconnect Eth when prev mode is %d\n",
                    QcMapBackhaulEthMgr->eth_cfg.eth_mode,0,0);
      QcMapBackhaulEthMgr->ProcessEthBackhaulDisAssoc();
      wan_mode = true;
    }
    delete QcMapBackhaulEthMgr;
  }
  if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
  {
   /* Remove IP address from ETH iface*/
   snprintf(command, MAX_COMMAND_STR_LEN,
           "ip -4 addr flush dev %s",
           ETH_IFACE);
   ds_system_call(command, strlen(command));

   snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
           ETH_IFACE);
    ds_system_call(command, strlen(command));
  }
/*------------------------------------------------------------------------
 Clear the pre-defined IP address if not in Cradle Mode, otherwise process
 cradle discAssoc
-------------------------------------------------------------------------*/
  /* Remove all v4 IP information from tethered interfaces */
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s 0",
           ECM_IFACE);
  ds_system_call(command, strlen(command));

  /* Remove all v6 IP information from ecm0 */
  snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
           ECM_IFACE);
  ds_system_call(command, strlen(command));

  if ((NULL != QcMapBackhaulCradleMgr) &&
      (QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_BRIDGE_V01 ||
      QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode == QCMAP_MSGR_CRADLE_WAN_ROUTER_V01))
  {
    /* this state is hit in following conditions:
     1. we are in Cradle WAN mode and switching to Cradle LAN mode
     2. in Cradle WAN mode and switching to another Cradle WAN mode
     3. if one sets from LAN->WAN ( no ip from cradle)->LAN mode
     in all above cases, one should call ProcessCradleDisAssoc()*/

    if (tethered_link == QCMAP_QTI_TETHERED_LINK_ECM )
    {
      LOG_MSG_INFO1("Disconnect Cradle when prev mode is %d\n",
                    QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode,0,0);
      QcMapBackhaulCradleMgr->ProcessCradleDisAssoc();
      wan_mode = true;
    }
    delete QcMapBackhaulCradleMgr;
  }
/*----------------------------------------------------------------------
  Delete IPv4 and IPv6 forwarding if WLAN is disabled
-----------------------------------------------------------------------*/
  if(QcMapWLANMgr&& QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 &&
     (QcMapTetheringMgr->GetTethLinkEnable(other_index) == false))
  {
    ds_system_call("echo 0 > /proc/sys/net/ipv4/ip_forward",
                 strlen("echo 0 > /proc/sys/net/ipv4/ip_forward"));
  }

  if (tethered_link == QCMAP_QTI_TETHERED_LINK_ETH || tethered_link ==
      QCMAP_QTI_TETHERED_LINK_ECM || tethered_link == QCMAP_QTI_TETHERED_LINK_RNDIS)
  {
    if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
      *err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    if(lan_cfg->ip_passthrough_cfg.ip_passthrough_active)
    {
       QCMAPLANMgr->DisableIPPassthrough(true);
    }

    snprintf(iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s0",link_type);
    if (!wan_mode && QcMapVLANMgr)
    {
      QcMapVLANMgr->AddDeleteVLANOnIface(iface_name, false);
    }
    if (!wan_mode && QcMapL2TPMgr)
    {
      QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(iface_name,false);
    }
  }

  if ( tethered_link != QCMAP_QTI_TETHERED_LINK_ETH )
  {
    mac_addr = QcMapTetheringMgr->GetUSBMac();
    if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
    {
      /*updating lease file & restarting dnsmasq*/
      memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
      if (mac_addr)
      {
        ds_mac_addr_ntop(mac_addr, mac_addr_char);
      }

      fp1=fopen (DNSMASQ_LEASE_FILE,"r");
      fp2=fopen (TEMP_LEASE_FILE, "w");
      if ((fp1 == NULL) || (fp2 == NULL))
      {
        LOG_MSG_ERROR("unable to open file", 0, 0, 0);
      }
      else
      {
        while (fgets(temp, MAX_COMMAND_STR_LEN, fp1) != NULL)
        {
          if ((strstr(temp, mac_addr_char) == NULL))
          {
            fputs(temp, fp2);
          }
        }

        snprintf(command, sizeof(command), "mv %s %s",TEMP_LEASE_FILE,
                 DNSMASQ_LEASE_FILE);
        ds_system_call(command, strlen(command));
      }

      if (fp1 != NULL)
        fclose (fp1);
      if (fp2 != NULL)
        fclose (fp2);

      /*Removing entry for dnsmasq script cointaining this mac address*/
      snprintf(command, sizeof(command), "sed -i \"/%s/d\" %s", mac_addr_char, TEMP_HOST_FILE);
      ds_system_call(command, strlen(command));

      snprintf(command, MAX_COMMAND_STR_LEN, "killall -HUP dnsmasq");
      ds_system_call(command, strlen(command));
      QCMAPLANMgr->StartDHCPD();
      /* end of updating lease file & restarting dnsmasq*/
    }
    /* Delete delegated prefix */
    if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated &&
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN)
    {
      int num_entries, i;
      qcmap_msgr_connected_device_info_v01 connected_devices[QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01];

      if(QcMapMgr->GetConnectedDevicesInfo(connected_devices,
                                            &num_entries, &qmi_err_num))
      {
        for (i =0; i < num_entries; i++)
        {
          if (mac_addr && memcmp(connected_devices[i].client_mac_addr, mac_addr, sizeof(mac_addr)) == 0)
            break;
        }
        if (QCMAP_Backhaul_WWAN::SendDeleteDelegatedPrefix(true, connected_devices[i].ipv6[0].addr, &qmi_err_num) == QCMAP_CM_ERROR)
        {
          LOG_MSG_ERROR("Error: Unable flush prefix's %d", qmi_err_num, 0, 0);
        }
      }
      else
        LOG_MSG_ERROR("Error: Unable get connectedDevices %d", qmi_err_num, 0, 0);
    }

    QcMapMgr->DeleteConnectedDevicesClients(QCMAP_MSGR_DEVICE_TYPE_USB_V01);
  }

  if ( tethered_link == QCMAP_QTI_TETHERED_LINK_ETH )
  {
    if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false ||
        QCMAP_Backhaul_Cradle::IsCradleBridgeActivated() == false )
    {
      if(QCMAPLANMgr->GetIPPassthroughConfig(&enable_state,&passthrough_config,&qmi_err_num))
      {
        mac_addr_non_empty = QCMAP_LAN::check_non_empty_mac_addr(passthrough_config.mac_addr, mac_addr_char);
        if (mac_addr_non_empty)
        {
          /*updating lease file & restarting dnsmasq*/
          /*Removing entry for dnsmasq script cointaining this mac address*/
          snprintf(command, sizeof(command), "sed -i \"/%s/d\" %s",
                   mac_addr_char, DNSMASQ_LEASE_FILE);
          ds_system_call(command, strlen(command));

          snprintf(command, MAX_COMMAND_STR_LEN, "killall -HUP dnsmasq");
          ds_system_call(command, strlen(command));
          QCMAPLANMgr->StartDHCPD();
          /* end of updating lease file & restarting dnsmasq*/

        }
      }
    }
    QcMapMgr->DeleteConnectedDevicesClients(QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01);
  }

  if (QcMapMgr->IsDynamicClockVotingNeeded())
  {
    if (QcMapMgr->CheckUsbClockAtNom())
    {
      QcMapMgr->SetUSBClockFreq(false);
    }
  }

  if (!QcMapTetheringMgr->GetTethLinkEnable(other_index))
    delete QcMapTetheringMgr;

  return true;
}


/*===========================================================================
  FUNCTION StorePPPIPv6IID
==========================================================================*/
/*!
@brief
  Adds the destination based route to ppp interface.

@parameters
  uint8_t ppp_ip[]

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
void QCMAP_Tethering::StorePPPIPv6IID
(
  uint8_t            ppp_ip[]
)
{
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  memset(ip6_addr, 0, MAX_IPV6_PREFIX+1);
  char command[MAX_COMMAND_STR_LEN];
  struct in6_addr null_ipv6_address;
  qmi_error_type_v01 qmi_err_num;
  qcmap_nl_addr_t nl_addr;
  qcmap_nl_sock_msg_t nl_buf;
  char enable[MAX_STRING_LENGTH];
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();

  memset(&null_ipv6_address, 0, sizeof(struct in6_addr));
  memset (&nl_buf, 0, sizeof (qcmap_nl_sock_msg_t));

  if (!QcMapTetheringMgr ||
      !(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
       (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                                   QCMAP_QTI_TETHERED_LINK_PPP)))
  {
    /* QCMAP PPP not enabled */
    LOG_MSG_ERROR("QCMAP PPP not enabled\n",0,0,0);
    return;
  }

  /* Update the txqueuelength for PPP interface. */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "ifconfig %s txqueuelen 1000",
            PPP_IFACE);
  ds_system_call( command, strlen(command));

  /* Restart UPNP/DLNA */
  if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.dlna_config)
  {
    if(!QcMapMediaServiceMgr->EnableDLNA(&qmi_err_num))
    {
      LOG_MSG_ERROR("Cannot enable DLNA, error: %d.", qmi_err_num, 0, 0);
    }
  }

  if(QcMapMediaServiceMgr && QcMapMediaServiceMgr->srvc_cfg.upnp_config)
  {
    if(!QcMapMediaServiceMgr->EnableUPNP(&qmi_err_num))
    {
      LOG_MSG_ERROR("Cannot enable UPnP, error: %d.", qmi_err_num, 0, 0);
    }
  }

  if (!memcmp ((struct in6_addr *)ppp_ip, &null_ipv6_address, sizeof(struct in6_addr)))
  {
    /* QCMAP PPP NULL address recieved */
    LOG_MSG_ERROR("QCMAP PPP NULL address recieved\n",0,0,0);
    return;
  }
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV6,GET_VALUE,enable,MAX_STRING_LENGTH);
  if (!((boolean)atoi(enable)))
  {
    /* QCMAP IPV6 is not enabled */
    LOG_MSG_ERROR("QCMAP IPV6 not enabled\n",0,0,0);
    return;
  }

  {
    /* Store the IID. */
    memset(QcMapTetheringMgr->ppp_ipv6_iid, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    memcpy(QcMapTetheringMgr->ppp_ipv6_iid, ppp_ip, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    /* Update the connected devices information. */
    nl_buf.nl_addr.isValidIPv6address = true;
    memcpy(nl_buf.nl_addr.ip_v6_addr, ppp_ip, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    //Convert the MAC from char to hex
    if(!ds_mac_addr_pton((char*)PPP_CLIENT_MAC_ADDR, nl_buf.nl_addr.mac_addr))
    {
      LOG_MSG_ERROR("StorePPPIPv6IID - Error in MAC address conversion",
                    0,0,0);
    }

    if(QcMapMgr->MatchMacAddrInList(&nl_buf) == 0)
    {
      LOG_MSG_INFO1("No match found for the USB MAC,"
                    "so add a linked list node\n",0, 0, 0);
      if (!(QCMAP_ConnectionManager::AddNewDeviceEntry(
           (void*)nl_buf.nl_addr.mac_addr,
           QCMAP_MSGR_DEVICE_TYPE_USB_V01,
           &nl_buf.nl_addr.ip_addr,
           nl_buf.nl_addr.ip_v6_addr,
           0,
           nl_buf.nl_addr.isValidIPv4address,
           nl_buf.nl_addr.isValidIPv6address)))
      {
        LOG_MSG_ERROR("Error in adding a new device entry ",
                      0, 0, 0);
      }
    }
  }

  if (!memcmp ((struct in6_addr *)QcMapTetheringMgr->ppp_ipv6_addr,
                &null_ipv6_address, sizeof(struct in6_addr)))
  {
    /* QCMAP PPP Global IPV6 address not yet recieved. Silently return. */
    return;
  }

  /* Add the IPV6 route. If not done already. */
  memset(&nl_addr, 0 ,sizeof(nl_addr));
  memcpy(nl_addr.ipv6_dst_addr, QcMapTetheringMgr->ppp_ipv6_iid, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  memcpy(nl_addr.ip_v6_addr,
         QcMapTetheringMgr->ppp_ipv6_addr,
         QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  nl_addr.isValidIPv6address = true;
  if(QcMapBackhaulMgr)
  QcMapBackhaulMgr->UpdateGlobalV6addr(&nl_addr, false);

  return;
}

/*===========================================================================
  FUNCTION SetUSBNeighIP
==========================================================================*/
/*!
@brief
  Sets the neighbor IP

@parameters
  uint8_t ip

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

void QCMAP_Tethering::SetUSBNeighIP(uint32 ip)
{
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (QcMapTetheringMgr)
    QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1]. \
                     remote_link_ip4_address = ip;
}

/*===========================================================================
  FUNCTION GetUSBNeighIP
==========================================================================*/
/*!
@brief
  Gets the neighbour IP

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

uint32 QCMAP_Tethering::GetUSBNeighIP(void )
{
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (QcMapTetheringMgr)
    return QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].remote_link_ip4_address;
  return 0;
}

 /*===========================================================================
   FUNCTION SetUSBMac
 ==========================================================================*/
 /*!
 @brief
  Set USB MAC address

 @parameters
   uint8* usbMac

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

void QCMAP_Tethering::SetUSBMac(uint8* usbMac)
{
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (QcMapTetheringMgr)
  {
     memcpy(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].remote_link_mac_addr,
            usbMac,
            QCMAP_MSGR_MAC_ADDR_LEN_V01);
  }
}

/*===========================================================================
  FUNCTION GetUSBMac
==========================================================================*/
/*!
@brief
  Get USB MAC.Address

@parameters
  none

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

uint8* QCMAP_Tethering::GetUSBMac(void)
{
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (QcMapTetheringMgr)
    return QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].remote_link_mac_addr;
  return 0;
}

/*===========================================================================
  FUNCTION SetTethLinkEnable
==========================================================================*/
/*!
@brief
  Set Tethering Link Enable

@parameters
  bool isLinkEnabled

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
void QCMAP_Tethering::SetTethLinkEnable (int index, bool isLinkEnabled)
{
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (QcMapTetheringMgr)
    QcMapTetheringMgr->tethered_conf[index].link_enabled = isLinkEnabled;
}

/*===========================================================================
  FUNCTION GetTethLinkEnable
==========================================================================*/
/*!
@brief
  Get Tethered Link enable status

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

bool QCMAP_Tethering::GetTethLinkEnable (int index)
{
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if (QcMapTetheringMgr)
    return QcMapTetheringMgr->tethered_conf[index].link_enabled;

  return 0;
}


/*==========================================================
  FUNCTION    UpdateMACandIP
===========================================================*/
/*!
@brief
  On USB plug-in, fetch the MAC address of the client using bridge fdb show
  and update the IP based on the neighbor table entry

@parameters
  Interface Name

@return
  void

@ Dependencies
  - None

@ Side Effects
  - None
*/
/*==========================================================*/
void QCMAP_Tethering:: UpdateMACandIPOnPlugIn
(
  char *iface
)
{
  char logmsg[MAX_COMMAND_STR_LEN];
  char temp[MAX_COMMAND_STR_LEN]="",*ptr;
  char *mac_ptr;
  char mac_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  uint8_t    mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  qcmap_nl_sock_msg_t qcmap_nl_buffer;
  FILE *fd = NULL;
  int retry =0;
  boolean deviceInfoUpdated = false;

  memset ( &qcmap_nl_buffer, 0, sizeof (qcmap_nl_sock_msg_t));

  for ( retry = 0; retry < QCMAP_MAX_FDB_RETRY ; retry ++ )
  {
    snprintf((char *)logmsg, MAX_COMMAND_STR_LEN, "rm %s",STATE_FDB_TABLE);
    ds_system_call(logmsg, strlen(logmsg));

    snprintf((char *)logmsg, MAX_COMMAND_STR_LEN, "bridge fdb show > %s",
             STATE_FDB_TABLE);
    ds_system_call(logmsg, strlen(logmsg));

    fd = fopen(STATE_FDB_TABLE, "r");

    if(fd == NULL)
    {
      LOG_MSG_ERROR("Error in opening file %s", STATE_FDB_TABLE,0,0);
      return;
    }

    while (fgets(temp, MAX_COMMAND_STR_LEN, fd) != NULL)
    {
      if ( (strstr ( temp, iface) != NULL) && (strstr (temp, PERMANENT) == NULL) )
      {
        mac_ptr=strtok_r (temp," ", &ptr);
        deviceInfoUpdated = true;
        LOG_MSG_INFO1("client mac address = %s",mac_ptr, 0, 0);
        break;
      }
    }

    fclose (fd);
    if ( deviceInfoUpdated )
    {
      break;
    }
    else
    {
      usleep( QCMAP_RETRY_FDB_TIMEOUT_US );
    }
  }

  if ( deviceInfoUpdated )
  {
    strlcpy  (mac_str,mac_ptr,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
    ds_mac_addr_pton(mac_str, mac_addr);
    memcpy( qcmap_nl_buffer.nl_addr.mac_addr, mac_addr,
            QCMAP_MSGR_MAC_ADDR_LEN_V01);
    if(QCMAP_ConnectionManager::MatchMacAddrInList(&qcmap_nl_buffer) == 0)
    {
      LOG_MSG_INFO1("No match found for the  MAC,"
                    "so add a linked list node\n",0, 0, 0);

      QCMAP_Tethering::SetUSBMac(qcmap_nl_buffer.nl_addr.mac_addr);

      if(!(QCMAP_ConnectionManager::AddNewDeviceEntry(
                 (void*)qcmap_nl_buffer.nl_addr.mac_addr,
                  QCMAP_MSGR_DEVICE_TYPE_USB_V01,
                  NULL,
                  NULL)))
      {
        LOG_MSG_ERROR("Error in adding a new device entry ", 0, 0, 0);
      }
      else
      {
        Getclientaddr(&qcmap_nl_buffer.nl_addr, qcmap_nl_buffer.vlan_id);
        if(QCMAP_ConnectionManager::MatchMacAddrInList(&qcmap_nl_buffer) == 0)
        {
          ds_mac_addr_ntop(qcmap_nl_buffer.nl_addr.mac_addr, mac_str);
          ds_log_med("No match found for the USB Client MAC %s", mac_str, 0, 0);
        }
      }
    }
  }

  snprintf((char *)logmsg, MAX_COMMAND_STR_LEN, "rm %s",STATE_FDB_TABLE);
  ds_system_call(logmsg, strlen(logmsg));
}

/*==========================================================
  FUNCTION SetGetDunDongleModeFromXML
==========================================================*/
/*!
@brief
  Set dundonglemode flag to the XML
  Get dundonglemode flag from the XML

@parameters
  qcmap_action_type action,
  char *data,
  int data_len

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

boolean QCMAP_Tethering::SetGetDunDongleModeFromXML
(
  qcmap_action_type action,
  char *data,
  int data_len
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  if (!data || data_len <= 0)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance
                                                                (NULL, false);

  if (!QcMapMgr)
  {
    return false;
  }

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
     LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
     return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(Dun_Dongle_Mode_Tag);
  if (root == NULL)
  {
    LOG_MSG_ERROR("DUN Dongle Mode tag not found in %s" , QCMAP_DEFAULT_CONFIG, 0, 0);
    return false;
  }
  if (action == SET_VALUE)
  {
    root.text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    strlcpy(data, root.child_value(), data_len);
  }
  LOG_MSG_INFO1("Config: %s Action: %d value: %d" , Dun_Dongle_Mode_Tag,
                 action, atoi(data));
  return true;
}

/*==========================================================
  FUNCTION    SetDunDongleMode
===========================================================*/
/*!
@brief
  set the Dun dongle mode

@parameters
  boolean dun_dongle_mode_state
  qmi_error_type_01 *qmi_err_num

@return
  boolean

@ Dependencies
  - None

@ Side Effects
  - None
*/
/*==========================================================*/
boolean QCMAP_Tethering::SetDunDongleMode
(
  boolean dun_dongle_mode_state,
  qmi_error_type_v01 *qmi_err_num
)
{
  char dunDongleMode[MAX_STRING_LENGTH] = {0};
  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  if(!SetGetDunDongleModeFromXML(GET_VALUE, dunDongleMode, MAX_STRING_LENGTH))
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if (dun_dongle_mode_state == atoi(dunDongleMode))
  {
    LOG_MSG_INFO1("Dun Dongle Mode is already set to %b", dun_dongle_mode_state, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  if (QcMapTetheringMgr)
    QcMapTetheringMgr->dun_dongle_mode = dun_dongle_mode_state;

  snprintf(dunDongleMode, MAX_STRING_LENGTH, "%d", dun_dongle_mode_state);
  if (SetGetDunDongleModeFromXML(SET_VALUE, dunDongleMode, MAX_STRING_LENGTH))
  {
    return true;
  }
  *qmi_err_num = QMI_ERR_INTERNAL_V01;
  return false;
}

/*==========================================================
  FUNCTION    GetDunDongleMode
===========================================================*/
/*!
@brief
  get the Dun dongle mode

@parameters
  boolean dun_dongle_mode_state
  qmi_error_type_01 *qmi_err_num

@return
  boolean

@ Dependencies
  - None

@ Side Effects
  - None
*/
/*==========================================================*/
boolean QCMAP_Tethering::GetDunDongleMode
(
  boolean *dun_dongle_mode_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }

  if (!dun_dongle_mode_status)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  if (QcMapTetheringMgr)
  {
    *dun_dongle_mode_status = QcMapTetheringMgr->dun_dongle_mode;
  }
  else
  {
    char dunDongleMode[MAX_STRING_LENGTH] = {0};
    if(!SetGetDunDongleModeFromXML(GET_VALUE, dunDongleMode, MAX_STRING_LENGTH))
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    *dun_dongle_mode_status = atoi(dunDongleMode);
  }
  return true;
}

/*==========================================================
  FUNCTION    BringDownPPPD
===========================================================*/
/*!
@brief
  Bring down pppd when backhaul is down if dun dongle mode is enabled

@parameters
  None

@return
  boolean

@ Dependencies
  - None

@ Side Effects
  - None
*/
/*==========================================================*/
boolean QCMAP_Tethering::BringDownPPPD()
{
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  if (QcMapTetheringMgr)
  {
    if ((QcMapTetheringMgr->dun_dongle_mode) &&
        (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
        QCMAP_QTI_TETHERED_LINK_PPP))
    {
      /*Kill ppp daemon*/
      ds_system_call("killall -15 pppd", strlen("killall -15 pppd"));
      if(!QCMAP_Backhaul::IsPPPkilled())
      {
        LOG_MSG_ERROR("PPP still running check. Kill forcefully", 0, 0, 0);
        /* Kill forcefully. */
        ds_system_call("killall -KILL pppd", strlen("killall -KILL pppd"));
      }
      return true;
    }
  }
  return false;
}
