/*====================================================

FILE:  QCMAP_BT_Tethering.cpp

SERVICES:
   QCMAP Connection Manager BT Tethering Specific Implementation

=====================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  06/13/16   c_surajj         Created

===========================================================================*/
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"

#include "QCMAP_BT_Tethering.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_LAN.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_NATALG.h"
#include "ds_qmi_qcmap_msgr.h"

bool QCMAP_BT_TETHERING::flag= false;
QCMAP_BT_TETHERING* QCMAP_BT_TETHERING::object=NULL;

/*===================================================================
         Class Definitions
===================================================================*/

/*===========================================================================
FUNCTION  QCMAP_BT_TETHERING
==========================================================================*/
/*!
@brief
QCMAP_BT_TETHERING

@return
none

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

QCMAP_BT_TETHERING::QCMAP_BT_TETHERING()
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  this->bt_v4_available = false;
  this->bt_v6_available = false;
  memset(&this->ipv6_gateway_addr, 0, sizeof(in6_addr));
  return;
}

/*===========================================================================
FUNCTION  ~QCMAP_BT_TETHERING
==========================================================================*/
/*!
@brief
~QCMAP_BT_TETHERING.

@return
none

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

QCMAP_BT_TETHERING::~QCMAP_BT_TETHERING()
{
  flag=false;
  object=NULL;
  LOG_MSG_INFO1("Destroying Object: BT Tethering",0,0,0);
}

/*===========================================================================
FUNCTION  Get_Instance
==========================================================================*/
/*!
@brief
Get_Instance.

@parameters
boolean obj_create

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

QCMAP_BT_TETHERING* QCMAP_BT_TETHERING::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : BT Tethering",0, 0, 0);
    object = new QCMAP_BT_TETHERING();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}


/*===========================================================================
FUNCTION  BringupBTTethering
==========================================================================*/
/*!
@brief
Bring up BT tethering.

@parameters
qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_BT_TETHERING::BringupBTTethering(qcmap_bt_tethering_mode_enum_v01
                            bt_tethering_mode, qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];

  QCMAP_CM_LOG_FUNC_ENTRY();

  ds_system_call("echo QCMAP:Start Bring up BT tethering > /dev/kmsg",\
  strlen("echo QCMAP:Start Bring up BT tethering > /dev/kmsg"));

  /* If mobileap state state is invalid*/
  if ( QcMapMgr->qcmap_cm_handle <= 0 )
  {
    LOG_MSG_ERROR("\n MobileAP not enabled\n", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }


  if (QcMapBTTethMgr && QcMapBTTethMgr->bt_tethering_status ==
      QCMAP_MSGR_BT_TETHERING_MODE_UP_V01)
  {
    LOG_MSG_ERROR("\n BT Tethering is already UP \n", 0 ,0, 0);
    return true;
  }


  QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(true);//Creating BT Tethering object here

  if (bt_tethering_mode == QCMAP_MSGR_BT_MODE_LAN_V01)
  {
    LOG_MSG_INFO1("Bringing up Bluetooth Tethering for LAN Mode",0,0,0);
    /* Update Current BT Tethering configuration. */
  QcMapBTTethMgr->bt_tethering_status = QCMAP_MSGR_BT_TETHERING_MODE_UP_V01;
    QcMapBTTethMgr->bt_tethering_mode = QCMAP_MSGR_BT_MODE_LAN_V01;

  // bring up the BT Tethering
  snprintf(command,MAX_COMMAND_STR_LEN,"brctl addif bridge0 %s", BT_IFACE);
  ds_system_call(command,strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN,
           "ifconfig %s 169.254.5.1 netmask 255.255.255.0 up", BT_IFACE);
  ds_system_call(command,strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN,
           "route del -net 169.254.5.1 netmask 255.255.255.0 dev %s", BT_IFACE);
  ds_system_call(command,strlen(command));

  //remove ipv6 LL route
  snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev %s",
           BT_IFACE);
  ds_system_call(command, strlen(command));
  }
  else if (bt_tethering_mode == QCMAP_MSGR_BT_MODE_WAN_V01)
  {
     LOG_MSG_INFO1("Bringing up Bluetooth Tethering for WAN Mode",0,0,0);
    /* Update Current BT Tethering configuration. */
    QcMapBTTethMgr->bt_tethering_status = QCMAP_MSGR_BT_TETHERING_MODE_UP_V01;
    QcMapBTTethMgr->bt_tethering_mode = QCMAP_MSGR_BT_MODE_WAN_V01;
    //make BT interface UP
    snprintf(command,MAX_COMMAND_STR_LEN,
             "ifconfig %s  up", BT_IFACE);
    ds_system_call(command,strlen(command));

    /* Kill any stale dhcpcd intance */
    LOG_MSG_INFO1("Killing previous dhcpcd process for Bt interface",0,0,0);
    snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)", BT_IFACE);
    ds_system_call(command, strlen(command));

    snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", BT_IFACE);
    ds_system_call(command, strlen(command));
    //run dhcpcd on BT interface
    LOG_MSG_INFO1("Running DHCP client for Bluetooth interface",0,0,0);
    snprintf( command, MAX_COMMAND_STR_LEN, "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",BT_IFACE);
    ds_system_call(command, strlen(command));
  }
  else
  {
    LOG_MSG_ERROR("\n Invalid BT Tethering mode(%d) passed \n", bt_tethering_mode ,0, 0);
    return false;
  }

  snprintf(command,MAX_COMMAND_STR_LEN,
           "echo QCMAP:BT Tethering Brought UP in %d Mode  > /dev/kmsg", bt_tethering_mode);
  ds_system_call(command, strlen(command));

  return true;
}

/*=====================================================================
FUNCTION BringdownBTTethering
======================================================================*/
/*!
@brief
Bring down BT Tethering.

@parameters
qmi_error_type_v01 *qmi_err_num

@return
true  - on success
false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*===================================================================*/

boolean QCMAP_BT_TETHERING::BringdownBTTethering( qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  if (!QcMapBTTethMgr || QcMapBTTethMgr->bt_tethering_status ==
      QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01)
  {
    LOG_MSG_ERROR("\n BT Tethering already down.\n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return true;
  }
  if (QcMapBTTethMgr->bt_tethering_mode == QCMAP_MSGR_BT_MODE_LAN_V01)
  {
    LOG_MSG_INFO1("Bringing down Bluetooth Tethering for LAN Mode",0,0,0);
    // bring down the BT Tethering
    snprintf(command,MAX_COMMAND_STR_LEN,"brctl delif bridge0 %s", BT_IFACE);
    ds_system_call(command,strlen(command));
    //bring down bt interface
    snprintf(command,MAX_COMMAND_STR_LEN,
             "ifconfig %s down", BT_IFACE);
    ds_system_call(command,strlen(command));
  }
  else if (QcMapBTTethMgr->bt_tethering_mode == QCMAP_MSGR_BT_MODE_WAN_V01)
  {
    LOG_MSG_INFO1("Bringing down Bluetooth Tethering for WAN Mode",0,0,0);
    //process BT WAN disassoc
    QcMapBTTethMgr->ProcessBTWANDisAssoc();

    //bring down bt interface
    snprintf(command,MAX_COMMAND_STR_LEN,
             "ifconfig %s down", BT_IFACE);
    ds_system_call(command,strlen(command));
  }
  else
  {
    LOG_MSG_ERROR("\n Invalid Brought DOWN BT Tethering mode(%d) request \n",
                  QcMapBTTethMgr->bt_tethering_mode ,0, 0);
    return false;
  }
  snprintf(command,MAX_COMMAND_STR_LEN,"echo QCMAP:BT Tethering Brought"
           " DOWN for %d Mode  > /dev/kmsg", QcMapBTTethMgr->bt_tethering_mode);
  ds_system_call(command, strlen(command));

  /* Reset BT Tethering mode */
  QcMapBTTethMgr->bt_tethering_status = QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01;

  delete QcMapBTTethMgr;//deleting bt tethering  object here
  return true;
}


/*=====================================================================
FUNCTION GetBTTetheringStatus
======================================================================*/
/*!
@brief
Gets the wlan configured mode.

@parameters
qcmap_msgr_wlan_mode_enum_v01 *wlan_mode

@return
true- on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_BT_TETHERING::GetBTTetheringStatus
(
  qcmap_msgr_bt_tethering_status_enum_v01 *bt_teth_status,
  qcmap_bt_tethering_mode_enum_v01 *bt_teth_mode,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  if (bt_teth_status == NULL || bt_teth_mode == NULL)
  {
    LOG_MSG_ERROR("NULL params passed for get BT status.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }
  if(QcMapBTTethMgr)
  {
    *bt_teth_status = QcMapBTTethMgr->bt_tethering_status;
    *bt_teth_mode = QcMapBTTethMgr->bt_tethering_mode;
  }
  else
  {
    *bt_teth_status = QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01 ;
    *bt_teth_mode = QCMAP_BT_TETHERING_MODE_ENUM_MIN_ENUM_VAL_V01;
  }

  return true;
}


/*======================================================
FUNCTION ProcessBTWANAddrAssign
======================================================*/
/*!
@brief
Process the address assignment for Bluetooth interface

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

void QCMAP_BT_TETHERING::ProcessBTWANAddrAssign
(
  char* devname,
  ip_version_enum_type ip_vsn
)
{
  in_addr btIp, btMask;
  boolean ret = false;
  qmi_error_type_v01 qmi_err_num;
  boolean switch_backhaul_required = false;
  char gw[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char iface_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_LAN *lan_obj = GET_DEFAULT_LAN_OBJECT();

  memset(iface_ip, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(gw, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  LOG_MSG_INFO1("Entering ProcessBTWANAddrAssign for further handling",0,0,0);

  if ( !QcMapBTTethMgr )
  {
    LOG_MSG_ERROR("Unable to get BT Tethering object",0,0,0);
    return;
  }

  /*Logic to decide whether backhaul needs to be switched or not*/
  if (QcMapBackhaulMgr && QcMapBackhaulMgr->ComparePriority(BACKHAUL_TYPE_BT,
                                        QCMAP_Backhaul::current_backhaul))
  {
    switch_backhaul_required = true;
  }

  if (ip_vsn == IP_V4)
  {
    if(QcMapBackhaulMgr)
      ret = QcMapBackhaulMgr->GetIP(&btIp.s_addr, &btMask.s_addr, devname);
    else
      LOG_MSG_ERROR("QcMapBackhaulMgr is NULL \n", 0, 0, 0);
    if ( true == ret )
    {
      LOG_MSG_INFO1("Device Connected to BT Backahul for V4.\n",0,0,0);

      if (QcMapBTTethMgr->bt_v4_available &&
          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
      {
        LOG_MSG_ERROR("ProcessBTWANAddrAssign : V4 Addr already assigned",0,0,0);
        return;
      }
      QcMapBTTethMgr->bt_v4_available = true;


      /* Switch to Cradle interface. */
      /*Disconnect from WWAN */
      if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false &&
          QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
          switch_backhaul_required)
      {
        /* Disconnect from WWAN */
        if (QCMAP_Backhaul::current_backhaul != BACKHAUL_TYPE_BT &&
            QCMAP_Backhaul::current_backhaul != NO_BACKHAUL)
        {
          if (QcMapNatAlg &&
              QcMapFirewall &&
              (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
              (QcMapBackhaulMgr && QcMapBackhaulMgr->QcMapBackhaulWWAN && QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED) ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV4()))
          {
            LOG_MSG_INFO1("Disabling V4 Backhaul Settings",0,0,0);
            QcMapNatAlg->DisableNATonApps();
            QcMapFirewall->CleanIPv4MangleTable();
          }
          if (QcMapBackhaulMgr &&
              QcMapFirewall &&
              (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
              (QcMapBackhaulMgr && QcMapBackhaulMgr->QcMapBackhaulWWAN && QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED) ||
              QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() ||
              QCMAP_Backhaul_WLAN::IsSTAAvailableV6()))
          {
            LOG_MSG_INFO1("Disabling V6 Backhaul Settings",0,0,0);
            QcMapBackhaulMgr->DisableIPV6Forwarding(true);
            QcMapFirewall->CleanIPv6MangleTable();
            if (QcMapBackhaulMgr && QcMapBackhaulWLANMgr && QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
            {
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
            }
            if (QcMapBackhaulMgr && QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
            {
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
            }
            if (QcMapBackhaulMgr && QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
            {
              QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
            }
          }
        }
        if(lan_obj)
          lan_obj->AllowBackhaulAccessOnIface(IP_V4, devname);
        else
          LOG_MSG_ERROR("OOPS!!! lan_obj is NULL \n", 0, 0, 0);
        QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_BT;
        if(QcMapNatAlg)
          QcMapNatAlg->EnableNATonApps();
        QcMapBTTethMgr->bt_connect_in_progress = false;

        /* Enable IPv4 MCAST */
        LOG_MSG_INFO1(" Restart PIMD to enable MCAST forwarding for BT \n",0,0,0);
        QcMapMgr->StopMcastDaemon();
        QcMapMgr->StartMcastDaemon();
        LOG_MSG_INFO1("ProcessBTWANAddrAssign : V4 Addr Assigned successfully",0,0,0);
      }
      else
      {
        LOG_MSG_ERROR("ProcessBTWANAddrAssign : failed ",0,0,0);
        return;
      }
    }
    else
    {
      LOG_MSG_INFO1("Error: GetIP Failed. Bailing out.\n",0,0,0);
      return;
    }
  }
  else if (ip_vsn == IP_V6)
  {
    //Add check to ensure that device got connected to sta
    /*Logic to decide whether backhaul needs to be switched or not*/
    LOG_MSG_INFO1("Device Connected to cradle for V6.\n",0,0,0);
    if (QcMapBTTethMgr->bt_v6_available &&
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
    {
      LOG_MSG_ERROR("ProcessBTWANAddrAssign : V6 Addr already assigned",0,0,0);
      return;
    }
    QcMapBTTethMgr->bt_v6_available = true;
    QcMapBTTethMgr->bt_connect_in_progress = false;
    /* Switch to Cradle interface. */
    /*Disconnect from WWAN */
    if (switch_backhaul_required ||
        (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false) &&
        (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false))
    {
       /* Disconnect from WWAN */
      if (QCMAP_Backhaul::current_backhaul != BACKHAUL_TYPE_BT &&
          QCMAP_Backhaul::current_backhaul != NO_BACKHAUL)
      {
        if (QcMapNatAlg &&
            QcMapFirewall &&
            (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
            (QcMapBackhaulMgr && (QcMapBackhaulMgr->QcMapBackhaulWWAN) && (QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED) ) ||
            QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() ||
            QCMAP_Backhaul_WLAN::IsSTAAvailableV4()))
        {
          QcMapNatAlg->DisableNATonApps();
          QcMapFirewall->CleanIPv4MangleTable();
        }
        if (QcMapBackhaulMgr &&
            QcMapFirewall &&
            (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
            (QcMapBackhaulMgr->QcMapBackhaulWWAN && QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED )||
            QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() ||
            QCMAP_Backhaul_WLAN::IsSTAAvailableV6()))
        {
          QcMapBackhaulMgr->DisableIPV6Forwarding(true);
          QcMapFirewall->CleanIPv6MangleTable();
		  if (QCMAP_Backhaul_WLAN::IsSTAAvailableV6())
		  {
			QcMapBackhaulMgr->DeleteAllV6ExternalRoute(WLAN_IFACE_STRING(QcMapMgr->sta_iface_index));
			QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
		  }
          if (QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
          {
            QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
            QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
          }
          if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
          {
            QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
            QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
          }
        }
      }
      if (lan_obj)
        lan_obj->AllowBackhaulAccessOnIface(IP_V6, devname);
      QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_BT;
      if (QcMapBackhaulMgr && QcMapFirewall)
      {
        QcMapBackhaulMgr->EnableIPV6Forwarding();
        QcMapFirewall->EnableIPV6Firewall();
      }

      LOG_MSG_INFO1("ProcessBTWANAddrAssign : V6 Addr Assigned successfully",0,0,0);
    }
  }

  qcmap_cm_process_bt_backhaul_ind(BT_BACKHAUL_CONNECTED);

  return;
}

/*======================================================
FUNCTION ProcessBTWANDisAssoc
======================================================*/
/*!
@brief
Process BT WAN dis associate

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

void QCMAP_BT_TETHERING::ProcessBTWANDisAssoc()
{
  QCMAP_ConnectionManager* QcMapMgr=
                  QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
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
  if ( !QcMapBTTethMgr)
  {
    LOG_MSG_ERROR("\n BT object not created.\n",0,0,0);
    return;
  }

  this->ProcAddrDisAssocOnBTWAN();

  /* Switch the backhaul. */
  if(QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
  {
    QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_BT,
                                          true,true);
  }

  qcmap_cm_process_bt_backhaul_ind(BT_BACKHAUL_DISCONNECTED);

  return;
}

/*=====================================================
  FUNCTION ProcAddrDisAssocOnBTWAN
======================================================*/
/*!
@brief
 Disable NAT, forwarding and Iface related V4 and V6 rules on the Backhaul
 as part of address Disallocation.

@parameters
  qcmap_backhaul_type backhaul_type

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_BT_TETHERING::ProcAddrDisAssocOnBTWAN()
{
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  boolean backhaul_v4_available = false;
  boolean backhaul_v6_available = false;
  char command[MAX_COMMAND_STR_LEN];
/*------------------------------------------------------------------------*/
  if ( !QcMapNatAlg || !QcMapFirewall)
  {
    LOG_MSG_ERROR(" QcMapNatAlg value: %p  QcMapFirewall value: %p",QcMapNatAlg, QcMapFirewall,0);
    return;
  }
  memset(command,0,MAX_COMMAND_STR_LEN);

  if (QcMapBTTethMgr)
  {
    backhaul_v4_available = QcMapBTTethMgr->bt_v4_available;
    backhaul_v6_available = QcMapBTTethMgr->bt_v6_available;
    /*Reset Cradle V4 Available flag*/
    QcMapBTTethMgr->bt_v4_available = false;
    /*Reset Cradle V6 Available flag*/
    QcMapBTTethMgr->bt_v6_available = false;
  }
  else
  {
    LOG_MSG_ERROR("ProcAddrDissalocOnBackhaul:"
                  "NULL BT object.",0,0,0);
    return;
  }
  if(QcMapBackhaulMgr == NULL)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr is NULL \n", 0, 0, 0);
    return;
  }

  if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
  {
    if (backhaul_v4_available)
    {
      /* Disable NAT on A5 and clean IPV4 firewall */
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }
    if (backhaul_v6_available)
    {
      /* Disable IPV6 Forwarding and firewall */
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(BT_IFACE);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BT_IFACE);
    }
  }

  /* stop dhcp client on bt interface */
  QcMapBackhaulMgr->StopDHCPCD(BT_IFACE);

  /*Set disable_ipv6 flag to 0 for BT Iface */
  snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
           BT_IFACE);
  ds_system_call(command, strlen(command));

  /*Set accept_ra flag to 1 for BT Iface */
  snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
           BT_IFACE);
  ds_system_call(command, strlen(command));

}

/*===========================================================================
  FUNCTION IsBTBackhaulAvailableV4
==========================================================================*/
/*!
@brief
  Utility API that Checks if BT Backhaul is Enabled and BT Backhaul is
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
boolean QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4()
{
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL != QcMapBTTethMgr) && QcMapBTTethMgr->bt_v4_available)
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
  Utility API that Checks if BT Backhaul is Enabled and BT Backhaul is
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
boolean QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6()
{
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL != QcMapBTTethMgr) && QcMapBTTethMgr->bt_v6_available)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*=====================================================
  FUNCTION SwitchToBTBackhaul()
======================================================*/
/*!
@brief
  Switches current backhaul to BT WAN

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================*/

void QCMAP_BT_TETHERING::SwitchToBTBackhaul()
{
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_LAN *lan_obj = GET_DEFAULT_LAN_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr =
                         QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL != QcMapBackhaulMgr)
    QCMAP_Backhaul::current_backhaul = BACKHAUL_TYPE_BT;

  if (QcMapBTTethMgr->bt_v4_available)
  {
    if (lan_obj)
      lan_obj->AllowBackhaulAccessOnIface(IP_V4, BT_IFACE);
    if(QcMapNatAlg !=NULL)
      QcMapNatAlg->EnableNATonApps();
    else
    {
      LOG_MSG_ERROR("QcMapNatAlg is NULL \n", 0, 0, 0);
      return;
    }
  }
  if (QcMapBTTethMgr->bt_v6_available)
  {
    if (lan_obj)
      lan_obj->AllowBackhaulAccessOnIface(IP_V6, BT_IFACE);

    if(QcMapBackhaulMgr)
      QcMapBackhaulMgr->EnableIPV6Forwarding();

    if(QcMapFirewall)
      QcMapFirewall->EnableIPV6Firewall();
  }
  /* Enable IPv4 MCAST */
  LOG_MSG_INFO1(" Restart PIMD to enable MCAST "
                "forwarding for BT Backhaul \n",0,0,0);
  QcMapMgr->StopMcastDaemon();
  QcMapMgr->StartMcastDaemon();
}

