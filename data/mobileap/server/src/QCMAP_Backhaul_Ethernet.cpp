/*====================================================

FILE:  QCMAP_Backhaul_Ethernet.cpp

SERVICES:
   QCMAP Connection Manager Backhaul (Ethernet) Specific Implementation

=====================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  03/31/14   jc         Created

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
#include "QCMAP_Firewall.h"
#include "ds_qmi_qcmap_msgr.h"

#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul_Ethernet.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_LAN.h"
#include "QCMAP_Virtual_LAN.h"


bool QCMAP_Backhaul_Ethernet::flag= false;
QCMAP_Backhaul_Ethernet* QCMAP_Backhaul_Ethernet::object=NULL;


/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and return instance of class QCMAP_Backhaul_Ethernet

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

QCMAP_Backhaul_Ethernet* QCMAP_Backhaul_Ethernet::Get_Instance(boolean obj_create)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : ETH Backhaul",0, 0, 0);
    object = new QCMAP_Backhaul_Ethernet();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}
/*=====================================================
                             Class Definitions
  =====================================================*/

/*=====================================================
  FUNCTION Constructor
======================================================*/
/*!
@brief
  Initializes Backhaul Eth variables.

@parameters
none

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

QCMAP_Backhaul_Ethernet::QCMAP_Backhaul_Ethernet
(
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&this->eth_cfg,0,sizeof(this->eth_cfg));
  this->eth_v4_available = false;
  this->eth_v6_available = false;
  memset(&this->ipv6_gateway_addr,0,sizeof(in6_addr));
  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(
                                              GET_VALUE,
                                              QCMAP_TETH_ETH_BACKHAUL,
                                              (void *)&(this->eth_cfg.eth_mode));

  return;
}


/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroyes the Eth bachaul Object.

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
/*=====================================================*/


QCMAP_Backhaul_Ethernet::~QCMAP_Backhaul_Ethernet()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=
                                    GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=
                                    GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                                    GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  LOG_MSG_INFO1("Destroying Object: Eth Backhaul",0,0,0);
  flag=false;
  object=NULL;

  //Default Backhaul manager object should not be deleted
  //if(!QcMapBackhaulWLANMgr && !QcMapBackhaulWWANMgr && !QcMapBackhaulCradleMgr)
 //   delete QcMapBackhaulMgr;
}

/*=====================================================
  FUNCTION GetEthBackhaulMode
======================================================*/
/*!
@brief
  Retreives the EthBackhaul mode.

@parameters
  char *process

@return
  true  - Eth Backhaul mode found
  flase - Eth Backhaul mode not set

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
boolean QCMAP_Backhaul_Ethernet::GetEthBackhaulMode
(
  qcmap_msgr_ethernet_mode_v01 *mode,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                         GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.",0,0,0);
    return false;
  }

  if (mode == NULL)
  {
    LOG_MSG_ERROR("NULL mode Arg.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if(NULL != QcMapBackhaulEthMgr)
    *mode = QcMapBackhaulEthMgr->eth_cfg.eth_mode;
  else
  {
    QCMAP_Tethered_Backhaul::GetSetConfigFromXML(GET_VALUE,
                        QCMAP_TETH_ETH_BACKHAUL,
                        (void *)mode);
  }
  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*======================================================
  FUNCTION SetEthBackhaulMode
=====================================================*/
/*!
@brief
  Sets the Eth Backhaul mode.

@parameters
  char *process

@return
  true  - Eth Backhaul mode set.
  flase - Eth Backhaul mode set failed.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
boolean QCMAP_Backhaul_Ethernet::SetEthBackhaulMode
(
  qcmap_msgr_ethernet_mode_v01  mode,
  void                        *softApHandle,
  qmi_error_type_v01          *qmi_err_num
)
{
  QCMAP_Backhaul* QcMapBackhaulMgr=NULL;
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                  GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                                  GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();

  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=
                                GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_Virtual_LAN* QcMapVLAN = QCMAP_Virtual_LAN::Get_Instance(false);
  qcmap_msgr_ethernet_mode_v01 prev_mode;
  uint32 num_of_vlans = 0;
  qcmap_msgr_vlan_config_v01 vlan_config;
  char command[MAX_COMMAND_STR_LEN];
  qmi_qcmap_msgr_softap_handle_type *handle =
                     (qmi_qcmap_msgr_softap_handle_type*)softApHandle;
  int retval, err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(command,0,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("Entering Set EthBackhaul mode %d",mode,0,0);
  *qmi_err_num = QMI_ERR_NO_EFFECT_V01;

  if (handle == NULL)
  {
    LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if((NULL != QcMapBackhaulEthMgr)
      &&
     mode == QcMapBackhaulEthMgr->eth_cfg.eth_mode)
  {
    LOG_MSG_INFO1("Eth mode %d is already in effect", mode,0,0);
    return false;
  }

  //sanity check that VLAN feature is not enabled
  if(QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01 == mode)
  {
    if(QcMapVLAN && (QcMapVLAN->GetVLANConfig(&vlan_config, &num_of_vlans, qmi_err_num)))
    {
      if(num_of_vlans > 0)
      {
        LOG_MSG_ERROR("Please delete VLANs first", 0, 0, 0);
        *qmi_err_num = QMI_ERR_INCOMPATIBLE_STATE_V01;
        return false;
      }
      LOG_MSG_INFO1("Get VLAN Config succeeded.\n",0,0,0);
    } else {
      LOG_MSG_ERROR("Get VLAN Config failed", 0, 0, 0);
      //don't return false here, maybe VLANs were not configured
    }
  }

  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(GET_VALUE,
                                             QCMAP_TETH_ETH_BACKHAUL,
                                             (void *)&prev_mode);
  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(SET_VALUE,
                                             QCMAP_TETH_ETH_BACKHAUL,
                                             (void *)&mode);

  /* Case that is being checked here:
    1. If the TETHERED link is not UP (i.e. QcMapTetheringMgr is NULL)
    2. Tethered Link is UP but type is not QCMAP_QTI_TETHERED_LINK_ETH
  */
  if ((!QcMapTetheringMgr)
      ||
      (QcMapTetheringMgr
       &&
       QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2)
       &&
       QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type
                                         != QCMAP_QTI_TETHERED_LINK_ETH)
     )
  {
    LOG_MSG_INFO1("TETHERED link is not ETH, wait for eth connection",0,0,0);
    /* Update IPACM cfg , For JO there is no IPA. */
    #ifndef FEATURE_DATA_TARGET_MDM9607
    switch (mode)
    {
      case QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01:
        QCMAP_Backhaul_Ethernet::SetIPACMCategory(ETH_IFACE,
                                                  ETH_LAN_CATEGORY,
                                                  ROUTER_MODE);
      break;
      case QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01:
        QCMAP_Backhaul_Ethernet::SetIPACMCategory(ETH_IFACE,
                                                WAN_CATEGORY,
                                                ROUTER_MODE);
      break;
      default:
        LOG_MSG_ERROR("Unknown mode %d",mode,0,0);
      break;
    }
    #endif /* FEATURE_DATA_TARGET_MDM9607 */
    LOG_MSG_INFO1("Set Eth backhaul mode %d",mode,0,0);
    if(NULL != QcMapBackhaulEthMgr)
      QcMapBackhaulEthMgr->eth_cfg.eth_mode = mode;
    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
  }

  switch (mode)
  {
    case QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01:

      if (prev_mode == QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01 )
      {
        //Nothing needs to be done
        *qmi_err_num = QMI_ERR_NONE_V01;
        break;
      }

      if ( QcMapTetheringMgr && !(retval = QCMAP_Tethering::BringDownTetheredLink(
          QCMAP_QTI_TETHERED_LINK_ETH,
          handle, &err_num)))
      {
        LOG_MSG_ERROR("BringDownTetheredLink failed %d %d", retval, err_num,0);
      }

      if(!(retval = QCMAP_Tethering::SetupTetheredLink(
                                      QCMAP_QTI_TETHERED_LINK_ETH, &err_num)))
      {
        LOG_MSG_ERROR("SetupTetheredLink failed %d %d", retval, err_num,0);
      }
      #ifndef FEATURE_DATA_TARGET_MDM9607
      /* Update IPACM cfg , For JO there is no IPA. */
      QCMAP_Backhaul_Ethernet::SetIPACMCategory(ETH_IFACE,
                                                ETH_LAN_CATEGORY,
                                                ROUTER_MODE);
     #endif /* FEATURE_DATA_TARGET_MDM9607 */
      return true;
    break;
    case QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01:
      if (QcMapTetheringMgr)
      {
        if (!(retval = QCMAP_Tethering::BringDownTetheredLink(
              QCMAP_QTI_TETHERED_LINK_ETH,
              handle, &err_num)))
        {
          LOG_MSG_ERROR("BringDownTetheredLink failed %d %d", retval, err_num,0);
        }
      }
      QcMapBackhaulEthMgr=QCMAP_Backhaul_Ethernet::Get_Instance(true);


    /* For JO there is no IPA. */
#ifndef FEATURE_DATA_TARGET_MDM9607
      QCMAP_Backhaul_Ethernet::SetIPACMCategory(ETH_IFACE,
                                                  WAN_CATEGORY,
                                                  ROUTER_MODE);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
      if (!(retval = QCMAP_Tethering::SetupTetheredLink(QCMAP_QTI_TETHERED_LINK_ETH,
           &err_num)))
      {
        LOG_MSG_ERROR("SetupTetheredLink failed %d %d", retval, err_num,0);
        delete QcMapTetheringMgr;
        delete QcMapBackhaulEthMgr;

        if (!QcMapBackhaulWLANMgr && !QcMapBackhaulWWANMgr &&
            !QcMapBackhaulCradleMgr)
        {
          delete QcMapBackhaulMgr;
        }
      }
      /* Need to get the new object reference here as in BringDownTetheredLink object will be deleted. */
      QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
    break;
    default:
      /* Unknown mode */
      LOG_MSG_ERROR("Eth Backhaul mode unknown %d",mode,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    break;
  }
  LOG_MSG_INFO1("Set Eth backhaul mode %d",mode,0,0);
    if(NULL != QcMapBackhaulEthMgr)
      QcMapBackhaulEthMgr->eth_cfg.eth_mode = mode;
  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}
/*======================================================
FUNCTION ProcessEthBackhaulAddrAssign
======================================================*/
/*!
@brief
Process the address assignment from Ethernet Backhaul.

@parameters
devname
ip_vsn (ip versio)

@return
void
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

void QCMAP_Backhaul_Ethernet::ProcessEthBackhaulAddrAssign(char* devname,
                                                   ip_version_enum_type ip_vsn)
{
  in_addr craIp, craMask;
  boolean ret = false;
  boolean switch_backhaul_required = false;
  char gw[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char iface_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_ConnectionManager* QcMapMgr=
                         QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_NATALG* QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT();
  QCMAP_LAN *lan_obj = GET_DEFAULT_LAN_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(iface_ip, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(gw, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  LOG_MSG_INFO1("Entering ProcessEthBackhaulAddrAssign for further handling",
                0,0,0);

  if ( !QcMapBackhaulEthMgr || !QcMapBackhaulMgr || !lan_obj)
  {
    LOG_MSG_ERROR("QcMapBackhaulEthMgr value %p, QcMapBackhaulMgr value %p, lan_obj value %p", QcMapBackhaulEthMgr, QcMapBackhaulMgr,lan_obj);
    return;
  }

  /*Logic to decide whether backhaul needs to be switched or not*/
  if (QcMapBackhaulMgr->ComparePriority(BACKHAUL_TYPE_ETHERNET,
                                        QCMAP_Backhaul::current_backhaul))
  {
    switch_backhaul_required = true;
  }

  if (ip_vsn == IP_V4)
  {
    ret = QcMapBackhaulMgr->GetIP(&craIp.s_addr, &craMask.s_addr, devname);
    if ( true == ret )
    {
      LOG_MSG_INFO1("Device Connected to ETH for V4.\n",0,0,0);

      if (QcMapBackhaulEthMgr->eth_v4_available &&
          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
      {
        LOG_MSG_ERROR("ProcessETHAddrAssign : V4 Addr already assigned",0,0,0);
        return;
      }
      QcMapBackhaulEthMgr->eth_v4_available = true;


      /* Switch to Cradle interface. */
      /*Disconnect from WWAN */
      if (QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                         QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01
          &&
          QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false
          &&
          QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false
          &&
          switch_backhaul_required)
      {
         QCMAP_Tethered_Backhaul::DisableV4V6RulesOnOtherBackhaul(
                                                     BACKHAUL_TYPE_ETHERNET);

          lan_obj->AllowBackhaulAccessOnIface(IP_V4, devname);
          QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_ETHERNET;
          if(QcMapNatAlg)
            QcMapNatAlg->EnableNATonApps();
          else
            LOG_MSG_ERROR("QcMapNatAlg is NULL", 0, 0, 0)

          /* Enable IPv4 MCAST */
          LOG_MSG_INFO1(" Restart PIMD to enable MCAST forwarding for ETH \n",
                        0,0,0);
          QcMapMgr->StopMcastDaemon();
          QcMapMgr->StartMcastDaemon();
          LOG_MSG_INFO1("ProcessEthAddrAssign : V4 Addr Assigned successfully",
                        0,0,0);
      }
      else
      {
        LOG_MSG_ERROR("ProcessETHAddrAssign : failed because Mode %d"
                      "is not supported or Cradle/WLAN bridge mode is active"
                      " or Backhaul switch is not required "
                      ,QcMapBackhaulEthMgr->eth_cfg.eth_mode, 0, 0);
        return;
      }
    }
    else
    {
      LOG_MSG_ERROR("Error: GetIP Failed. Bailing out.\n",0,0,0);
      return;
    }
  }
  else if (ip_vsn == IP_V6)
  {
    //Add check to ensure that device got connected to sta
    /*Logic to decide whether backhaul needs to be switched or not*/
    LOG_MSG_INFO1("Device Connected to ETH for V6.\n",0,0,0);
    if (QcMapBackhaulEthMgr->eth_v6_available &&
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
    {
      LOG_MSG_ERROR("ProcessETHAddrAssign : V6 Addr already assigned",0,0,0);
      return;
    }
    QcMapBackhaulEthMgr->eth_v6_available = true;
    /* Switch to ETH interface. */
    /*Disconnect from WWAN */
    if (switch_backhaul_required
        &&
        QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                         QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01
        &&
        QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false
        &&
        QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
    {

      QCMAP_Tethered_Backhaul::DisableV4V6RulesOnOtherBackhaul(
                                                     BACKHAUL_TYPE_ETHERNET);
      lan_obj->AllowBackhaulAccessOnIface(IP_V6, devname);
      QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_ETHERNET;
      QcMapBackhaulMgr->EnableIPV6Forwarding();
      if( QcMapFirewall !=NULL)
        QcMapFirewall->EnableIPV6Firewall();
      else
      {
        LOG_MSG_ERROR("QcMapFirewall is NULL \n", 0, 0, 0);
      }

      LOG_MSG_INFO1("ProcessEthAddrAssign : V6 Addr Assigned successfully",0,0,0);
    }
  }

  qcmap_cm_process_eth_backhaul_ind(ETH_BACKHAUL_CONNECTED);

  return;
}
/*======================================================
FUNCTION ProcessEthBackhaulDisAssoc
======================================================*/
/*!
@brief
Bring down the Ethernet iface

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

void QCMAP_Backhaul_Ethernet::ProcessEthBackhaulDisAssoc()
{
  QCMAP_ConnectionManager* QcMapMgr=
                  QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                               GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1("Entering ProcessEthBackhaulDisAssoc",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }
  if ( !QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("\n Backhaul object not created.\n",0,0,0);
    return;
  }
  if ( !QcMapBackhaulEthMgr)
  {
    LOG_MSG_ERROR("\n ETH Backhaul object not created.\n",0,0,0);
    return;
  }

  this->ProcAddrDisAssocOnBackhaul(BACKHAUL_TYPE_ETHERNET);

  /* WAN Bridge mode is not supported right now. and can be included later*/
  if(QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    /* Delete external routes if needed */
    if (QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                                   QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01 &&
        QcMapBackhaulEthMgr->IsEthBackhaulAvailableV6())
    {
      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
    }

    QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_ETHERNET,
                                                   true,true);
  }

  qcmap_cm_process_eth_backhaul_ind(ETH_BACKHAUL_DISCONNECTED);

  return;
}

/*=====================================================
FUNCTION DisableETHBackhaul
======================================================*/
/*!
@brief
Bring down the ETH iface

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
/*=====================================================*/

void QCMAP_Backhaul_Ethernet::DisableEthBackhaul(
                                             qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=
                            QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                  GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!QcMapNatAlg || !QcMapFirewall || !QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("QcMapNatAlg value %p, QcMapFirewall value: %p , QcMapBackhaulMgr value: %p ", QcMapNatAlg, QcMapFirewall, QcMapBackhaulMgr);
    return;
  }
  LOG_MSG_INFO1("Entering Disable Eth backhaul",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }
  if(!QcMapBackhaulEthMgr)
  {
    LOG_MSG_ERROR("\n Eth Backhaul object not created.\n",0,0,0);
    return;
  }
  if ( !(QcMapBackhaulEthMgr->eth_v4_available ||
         QcMapBackhaulEthMgr->eth_v6_available))
  {
    LOG_MSG_ERROR("\n Eth Backhaul is not connected.\n",0,0,0);
    return;
  }
  /* WAN Bridge mode is not supported right now. and can be included later*/
  if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    if (QcMapBackhaulEthMgr->eth_v4_available)
    {
      /* Disable NAT on A5 and clean IPV4 firewall */
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }

    if (QcMapBackhaulEthMgr->eth_v6_available)
    {
      /* Disable IPV6 Forwarding and firewall */
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);

      QcMapFirewall->CleanIPv6MangleTable();

      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
    }
  }

  /*Reset eth backhaul available flags*/
  QcMapBackhaulEthMgr->eth_v4_available = false;
  QcMapBackhaulEthMgr->eth_v6_available = false;

  if (QcMapBackhaulEthMgr->eth_cfg.eth_mode ==
                                QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
            ETH_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             ETH_IFACE);
    ds_system_call(command, strlen(command));
  }

  /* stop dhcp client on eth0 */
  LOG_MSG_INFO1("Killing dhcpcd process.\n",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN,
           "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
           ETH_IFACE);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "rm -rf /var/run/dhcpcd-%s.pid", ETH_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "ip addr flush dev %s", ETH_IFACE);
  ds_system_call(command, strlen(command));

  /* Disable IPv4 MCAST */
  LOG_MSG_INFO1(" Stop PIMD to disable MCAST forwarding on "
                "ETH interface.\n",0,0,0);
  QcMapMgr->StopMcastDaemon();
  delete QcMapBackhaulEthMgr;
  return;
}

/*===========================================================================
  FUNCTION IsEthBackhaulAvailableV4
==========================================================================*/
/*!
@brief
  Utility API that Checks if ETH Backhaul is Enabled and ETH Backhaul is
  available for v4 connectivity

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
boolean QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4()
{
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr =
     GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL != QcMapBackhaulEthMgr) && QcMapBackhaulEthMgr->eth_v4_available)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*===========================================================================
  FUNCTION IsETHBackhaulAvailableV6
==========================================================================*/
/*!
@brief
  Utility API that Checks if ETH Backhaul is Enabled and ETH Backhaul is
  available for v6 connectivity

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
boolean QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6()
{
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr =
           GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL != QcMapBackhaulEthMgr) && QcMapBackhaulEthMgr->eth_v6_available)
  {
    return true;
  }
  else
  {
    return false;
  }
}
/*=====================================================
  FUNCTION SwitchToEthBackhaul()
======================================================*/
/*!
@brief
  Switches current backhaul to Ethernet

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================*/

void QCMAP_Backhaul_Ethernet::SwitchToEthBackhaul()
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                               GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr =
                         QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_LAN *lan_obj = GET_DEFAULT_LAN_OBJECT();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* WAN Bridge mode is not supported right now. and can be included later*/
  if (NULL != QcMapBackhaulMgr)
    QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_ETHERNET;

  if(QcMapNatAlg == NULL)
  {
    LOG_MSG_ERROR("QcMapNatAlg is NULL \n", 0, 0, 0);
    return;
  }
  if (QcMapBackhaulEthMgr->eth_v4_available)
  {
    if (lan_obj)
      lan_obj->AllowBackhaulAccessOnIface(IP_V4, ETH_IFACE);

    QcMapNatAlg->EnableNATonApps();
  }
  if (QcMapBackhaulEthMgr->eth_v6_available)
  {
    if (lan_obj)
      lan_obj->AllowBackhaulAccessOnIface(IP_V6, ETH_IFACE);

    if(QcMapBackhaulMgr)
      QcMapBackhaulMgr->EnableIPV6Forwarding();

    if(QcMapFirewall)
      QcMapFirewall->EnableIPV6Firewall();
  }
  /* Enable IPv4 MCAST */
  LOG_MSG_INFO1(" Restart PIMD to enable MCAST "
                "forwarding for ETH Backhaul \n",0,0,0);
  QcMapMgr->StopMcastDaemon();
  QcMapMgr->StartMcastDaemon();
}
