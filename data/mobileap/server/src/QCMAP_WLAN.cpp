/*====================================================

FILE:  QCMAP_WLAN.cpp

SERVICES:
   QCMAP Connection Manager WLAN Specific Implementation

=====================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   ka         Created

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
#include "qti_wlan_scm_msgr_v01.h"

#include "QCMAP_WLAN.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_NATALG.h"
#include "ds_qmi_qcmap_msgr.h"
#include "qcmap_packet_stats.h"
#include "QCMAP_GSB.h"


#define GUEST_AP_IFACE_LL_ADDR "169.254.2.1"
#define GUEST_AP_2_IFACE_LL_ADDR "169.254.6.1"
#define GUEST_AP_3_IFACE_LL_ADDR "169.254.7.1"

#define P2P_DEFAULT_IFACE_LL_ADDR "169.254.8.1"
#define ZERO_IP_ADDRESS "0.0.0.0"

#define HOSTAPD_PID_FILE "/var/run/hostapd_ssid1.pid"
#define AP_IFACE_LL_ADDR       "169.254.1.1"
#define QCMAP_MULTIPLE_SYSTEM_CALL_FILE "/tmp/qcmap_multiple_system_call.sh"
#define WLAN_INI_CFG_FILE "/lib/firmware/wlan/qca_cld/WCNSS_qcom_cfg.ini"

#define HOSTAPD_SSID2_PID_FILE "/var/run/hostapd_ssid2.pid"
#define HOSTAPD_SSID3_PID_FILE "/var/run/hostapd_ssid3.pid"
#define HOSTAPD_SSID4_PID_FILE "/var/run/hostapd_ssid4.pid"

#define STA_ONLY_MODE_SUPPLICANT_FILE "/tmp/wpa_supplicant.conf"

#define BRCTL_TMP_FILE "/tmp/brctl.tmp"

bool QCMAP_WLAN::flag= false;
QCMAP_WLAN* QCMAP_WLAN::wlan_object=NULL;
bool QCMAP_WLAN::priv_client_restart_wlan=false;

/*=====================================================================
  FUNCTION CheckAPStatus
======================================================================*/
/*!
@brief
  Return true if wlan AP interface started beaconing

@parameters
  int iface

@return
  true  - AP is beaconing
  false - AP is not beaconing

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
static int CheckAPStatus(int iface)
{
  char cmd_res_buf[MAX_COMMAND_STR_LEN] = {0}, cmd_buf[MAX_CMD_SIZE] = {0};
  int ptr = 0, ap_status = 0, cmd_res_len = 0;
  FILE *cmd_pipe;

  snprintf(cmd_buf, MAX_CMD_SIZE, "(iwpriv wlan%d getchannel 2>&1)", iface);
  cmd_pipe = popen(cmd_buf, "r");
  if(cmd_pipe) {
    fgets(cmd_res_buf, MAX_COMMAND_STR_LEN, cmd_pipe);
    cmd_res_len = strlen(cmd_res_buf);
    pclose(cmd_pipe);
  }

  if (cmd_res_buf && cmd_res_len < MAX_COMMAND_STR_LEN) {
    char *a = cmd_res_buf;
    while(*a != ':' && ptr < cmd_res_len) {
      a++; ptr++;
    }
    if (*a == ':' && *(a+1) >= '0' && *(a+1) <= '9')
      ap_status = atoi(&cmd_res_buf[ptr+1]);
  }

  return (ap_status == 0 ? 0 : 1);
}

/*=====================================================================
  FUNCTION KillHostapd
======================================================================*/
/*!
@brief
  Kill hostapd and wait for driver cleanup

@parameters
  int pid, wlan interface index

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/

static void KillHostapd(int pid, int wlan_dev_index)
{

  char command[MAX_COMMAND_STR_LEN];
  int retry = 0;

  if (pid != 0) {
    snprintf(command, MAX_COMMAND_STR_LEN, "kill %d", pid);
    ds_system_call(command, strlen(command));
  }
  /* Though hostapd application has terminated, the wlan driver is not yet
   * done with AP Stop and cleanup operation. Check before exit
   */
  /* Check Interface stop */
  while (CheckAPStatus(wlan_dev_index) == 1 &&
           retry < QCMAP_HOSTAPD_END_POLL_MAX_COUNT) {
    retry++;
    usleep(QCMAP_HOSTAPD_POLL_DELAY_MS);
  }
  /* Wait for 100ms interface cleanup */
  usleep(QCMAP_HOSTAPD_POLL_DELAY_MS * 5);
  snprintf(command, MAX_COMMAND_STR_LEN,
         "echo \"QCMAP: Wlan%d Hostapd kill complete\" > /dev/kmsg",
          wlan_dev_index);

  ds_system_call(command, strlen(command));

  /* New requirement from WLAN to bring down interface after hostapd is killed. */
  snprintf(command, MAX_COMMAND_STR_LEN,"ifconfig wlan%d down", wlan_dev_index);
  ds_system_call(command, strlen(command));
}


/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and returns instance of class QCMAP_WLAN

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
QCMAP_WLAN* QCMAP_WLAN::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : WLAN",0, 0, 0);
    wlan_object = new QCMAP_WLAN();
    flag = true;
    return wlan_object;
  }
  else
  {
    return wlan_object;
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
  Initializes QCMAP_WLAN variables.

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
/*=========================================================*/

QCMAP_WLAN::QCMAP_WLAN()
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  memset(&this->wlan_cfg,0,sizeof(this->wlan_cfg));
  memset(&this->wlan_if_info, 0, sizeof(this->wlan_if_info));
  this->active_if_count = 0;
  this->last_STA_GSB_IF = -1;
  this->is_STA_GSB_conf = false;

  /*Convert string to lower case, to keep the comparision uniform */
  this->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  this->activate_wlan_in_progress = false;
  this->disable_wlan_in_progress = false;
  this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_V01;
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile =
                                                                  QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile =
                                                                  QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile =
                                                                  QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].ll_ip_addr = ntohl(inet_addr(AP_IFACE_LL_ADDR));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr = ntohl(inet_addr(GUEST_AP_IFACE_LL_ADDR));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].ll_ip_addr = ntohl(inet_addr(GUEST_AP_2_IFACE_LL_ADDR));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].ll_ip_addr = ntohl(inet_addr(GUEST_AP_3_IFACE_LL_ADDR));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].ll_subnet_mask = ntohl(inet_addr(LL_SUBNET_MASK));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask = ntohl(inet_addr(LL_SUBNET_MASK));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].ll_subnet_mask = ntohl(inet_addr(LL_SUBNET_MASK));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].ll_subnet_mask = ntohl(inet_addr(LL_SUBNET_MASK));
  this->prev_guest_1_profile = QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  this->prev_guest_2_profile = QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  this->prev_guest_3_profile = QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  this->debug_config.supplicant_debug = false;
  this->debug_config.hostapd_debug= false;
  this->debug_config.hostapd_guest_debug= false;
  this->debug_config.hostapd_guest_2_debug= false;
  this->debug_config.hostapd_guest_3_debug= false;
  this->is_sta_only_mode_enabled = false;
  this->debug_config.p2p_supplicant_debug = false;
  memset(&(this->p2p_config), 0, sizeof(qcmap_p2p_config));
  char dont_bringdown_wlan[MAX_STRING_LENGTH] ={0};

  memset(&this->current_wlan_status, 0, sizeof(this->current_wlan_status));

  if (QcMapMgr->xml_path)
  {

    /*Initialize Always on WLAN*/
    if (QCMAP_WLAN::GetSetAlwaysOnWLANFromXML(GET_VALUE, dont_bringdown_wlan,
                                                  MAX_STRING_LENGTH))
    {
      this->always_on_wlan =  atoi(dont_bringdown_wlan);
    }
    else
    {
      LOG_MSG_ERROR("Get Always on WLAN status Failed and assigned default value",0,0,0);
      this->always_on_wlan = QCMAP_DEFAULT_ALWAYS_ON_WLAN;
    }
    if (this->ReadWLANConfigFromXML())
    {

      return;
    }
    /* Read Config from XML failed. Use defaults. */
    LOG_MSG_ERROR("QCMAP Read XML failed.",0,0,0);
  }
  //after xml read
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].path_to_hostapd_conf, HOSTAPD_CFG_PATH, QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].path_to_sta_mode_hostapd_conf, STA_MODE_HOSTAPD_CFG_PATH, QCMAP_CM_MAX_FILE_LEN);

  return;
}


/*======================================================
  FUNCTION Destroy_Instance
======================================================*/
/*!
@brief
  Destroying the QCMAP_WLAN Object.

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
static void QCMAP_WLAN::Destroy_Instance()
{
  flag=false;
  delete wlan_object;
}


/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroying the QCMAP_WLAN Object.

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
QCMAP_WLAN::~QCMAP_WLAN()
{
  wlan_object=NULL;
  LOG_MSG_INFO1("Destroying Object: WLAN",0,0,0);
}

/*=====================================================================
  FUNCTION AssociateWLANtoBridge
======================================================================*/
/*!
@brief
  This fuction will associate wlan with bridge

@parameters
  void

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::AssociateWLANtoBridge(int wlan_index)
{
  char command[MAX_COMMAND_STR_LEN];
  int ret = 0;
  int retry = 0;

  snprintf(command, MAX_COMMAND_STR_LEN, "brctl show | grep wlan%d > %s",
           wlan_index, BRCTL_TMP_FILE);
  ds_system_call(command, strlen(command));

  FILE *fp = NULL;
  char temp_str[100]="";
  /* Get the Primary AP PID. */
  fp = fopen(BRCTL_TMP_FILE, "r");
  if ( fp == NULL )
  {
    LOG_MSG_ERROR("Error opening brctl show file: %d.", errno, 0, 0);
  }
  else
  {
    ret = fgets(temp_str, 100, fp);
    fclose(fp);
  }
  snprintf( command, MAX_COMMAND_STR_LEN,
            "rm -f %s", BRCTL_TMP_FILE);
  ds_system_call( command, strlen(command));

  if (ret != NULL)
  {
    LOG_MSG_ERROR("BRCTL entry already for wlan%d present", wlan_index, 0, 0);
    return true;
  }


  snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s wlan%d",
           BRIDGE_IFACE, wlan_index);

  while(retry < QCMAP_ASSOC_WLAN_TO_BRIDGE_MAX_RETRIES)
  {
    ret = system(command);
    if(ret < 0)
    {
      LOG_MSG_ERROR("system: bridge wlan association failed ", 0, 0, 0);
    }
    else
    {
      if( WIFEXITED(ret) && (WEXITSTATUS(ret) == 0))
      {
        LOG_MSG_INFO1("system: bridge wlan association passed ", 0, 0, 0);
        return true;
      }
    }
    LOG_MSG_ERROR("system: Retry bridge wlan association", 0, 0, 0);
    usleep(QCMAP_ASSOC_WLAN_TO_BRIDGE_TIMEOUT_US);
    retry++;
  }
  return false;
}

/*=====================================================================
  FUNCTION DisAssociateWLANtoBridge
======================================================================*/
/*!
@brief
  This fuction will disassociate wlan with bridge

@parameters
  void

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void DisAssociateWLANtoBridge(int wlan_index)
{
  char command[MAX_COMMAND_STR_LEN];
  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
           BRIDGE_IFACE, wlan_index);
  ds_system_call(command, strlen(command));
}


/*=====================================================
  FUNCTION hostapdclicmd
======================================================*/
/*!
@brief
 This function execute hostapd_cli commands, if the command fails it will retry

@parameters
  - qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean hostapdclicmd(int dev)
{
  char command[MAX_COMMAND_STR_LEN];
  int retry;
  int ret;
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "hostapd_cli -i wlan%d -s /var/run -p /var/run/hostapd -B -a %s",
           dev, STA_INTERFACE_CTRL_PATH);
  retry = 0;
  while(retry < QCMAP_HOSTAPDCLI_MAX_RETRIES)
  {
    ret = system(command);
    if(ret < 0)
    {
      LOG_MSG_ERROR("system: hostapd_cli failed ", 0, 0, 0);
    }
    else
    {
      if( WIFEXITED(ret) && (WEXITSTATUS(ret) == 0))
      {
        LOG_MSG_INFO1("system: hostapd_cli passed for iface wlan%d", dev, 0, 0);
        return true;
      }
    }

    usleep(QCMAP_HOSTAPDCLI_TIMEOUT_US);
    retry++;
  }
  return false;
}

/*=====================================================
  FUNCTION PopulateIFInfoforLegacy
======================================================*/
/*!
@brief
 This is a utility function to maintain WLAN IF changes in legacy mode

@parameters
  - void

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
static void PopulateIFInfoforLegacy(void)
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);


  if(!QcMapWLANMgr || !QcMapMgr)
  {
    LOG_MSG_ERROR("obj is null",0,0,0);
    return;
  }

  switch (QcMapWLANMgr->wlan_cfg.wlan_mode)
  {
    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].state
                                              = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].wlan_ap_type
                                              = QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].wlan_dev_type
                                                            = QCMAP_MSGR_WLAN_DEV_ROME_V01;
      snprintf(QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].if_name,
               QCMAP_MAX_IFACE_NAME_SIZE_V01, "wlan%d", QcMapMgr->ap_dev_num2);
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                              = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].wlan_ap_type
                                              = QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].wlan_dev_type
                                                            = QCMAP_MSGR_WLAN_DEV_ROME_V01;
      snprintf(QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].if_name,
               QCMAP_MAX_IFACE_NAME_SIZE_V01, "wlan%d", QcMapMgr->ap_dev_num1);
      break;

    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                              = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].wlan_ap_type
                                              = QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].wlan_dev_type
                                                            = QCMAP_MSGR_WLAN_DEV_ROME_V01;
      snprintf(QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].if_name,
               QCMAP_MAX_IFACE_NAME_SIZE_V01, "wlan%d", QcMapMgr->ap_dev_num1);

      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_STATION_V01].state
                                              = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_STATION_V01].wlan_ap_type
                                              = QCMAP_MSGR_WLAN_IFACE_STATION_V01;
      QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_STATION_V01].wlan_dev_type
                                                            = QCMAP_MSGR_WLAN_DEV_ROME_V01;
      snprintf(QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_STATION_V01].if_name,
               QCMAP_MAX_IFACE_NAME_SIZE_V01, "wlan0");
      break;

    default:
      break;
  }
}

/*Only need to fillin the info during Enable since obj is deleted on Disable*/

/*=====================================================
  FUNCTION FillinWLANStatusIND
======================================================*/
/*!
@brief
 This function fills the WLAN status info during enable

@parameters
  - valid scm indication is passed if SCM is managing WLAN

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

static void FillinWLANStatusIND(qcmap_msgr_scm_ind_enum_v01 ind)
{
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  if(!QcMapWLANMgr)
    return;

  QcMapWLANMgr->current_wlan_status.scm_ind_valid = false;

  if ((ind > QCMAP_MSGR_SCM_IND_ENUM_MIN_ENUM_VAL_V01
              && ind < QCMAP_MSGR_SCM_IND_ENUM_MAX_ENUM_VAL_V01)
                  && ind != QCMAP_MSGR_SCM_INVALID_RECONFIG_IND_V01)
  {
    QcMapWLANMgr->current_wlan_status.scm_ind_valid = true;
  }
  QcMapWLANMgr->current_wlan_status.scm_ind = ind;

  if (!QcMapWLANMgr->current_wlan_status.scm_ind_valid)
  {

    QcMapWLANMgr->current_wlan_status.wlan_mode =  QcMapWLANMgr->wifi_mode;
    snprintf(QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_name,
             QCMAP_MSGR_DEVICE_NAME_MAX_V01, "wlan%d", QcMapMgr->ap_dev_num1);
    snprintf(QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_name,
             QCMAP_MSGR_DEVICE_NAME_MAX_V01, "wlan%d", QcMapMgr->ap_dev_num2);
    snprintf(QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_name,
             QCMAP_MSGR_DEVICE_NAME_MAX_V01, "wlan%d", QcMapMgr->ap_dev_num3);
    snprintf(QcMapWLANMgr->current_wlan_status.wlan_state[3].wlan_iface_name,
             QCMAP_MSGR_DEVICE_NAME_MAX_V01, "wlan%d", QcMapMgr->ap_dev_num4);

    if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_V01)
    {
      QcMapWLANMgr->current_wlan_status.wlan_state_len = 1;
      QcMapWLANMgr->current_wlan_status.wlan_status =
                                            QCMAP_MSGR_WLAN_ENABLED_V01;
      /* WLAN State */
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
    }
    else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
    {
      QcMapWLANMgr->current_wlan_status.wlan_state_len = 2;
      QcMapWLANMgr->current_wlan_status.wlan_status =
                                            QCMAP_MSGR_WLAN_ENABLED_V01;
      /* WLAN State */
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
    }
    else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
    {
      QcMapWLANMgr->current_wlan_status.wlan_state_len = 3;
      QcMapWLANMgr->current_wlan_status.wlan_status =
                                            QCMAP_MSGR_WLAN_ENABLED_V01;
      /* WLAN State */
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[2].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;

    }
    else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
             QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
    {
      QcMapWLANMgr->current_wlan_status.wlan_state_len = 2;
      QcMapWLANMgr->current_wlan_status.wlan_status =
                                            QCMAP_MSGR_WLAN_ENABLED_V01;
      /* WLAN State */
      /* STA State */
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_DISCONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_STA_V01;
      /* Primary AP State */
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
    }
    else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
    {
      QcMapWLANMgr->current_wlan_status.wlan_state_len = 4;
      QcMapWLANMgr->current_wlan_status.wlan_status =
                                            QCMAP_MSGR_WLAN_ENABLED_V01;
      /* WLAN State */
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[2].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[3].wlan_iface_state =
                                         QCMAP_MSGR_WLAN_CONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[3].ip_type =
                                   QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[3].wlan_iface_type =
                               QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;

      if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
      {
        QcMapWLANMgr->current_wlan_status.wlan_state_len = 3;
        /* Guest AP State */
        QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_state =
                                           QCMAP_MSGR_WLAN_CONNECTED_V01;
        QcMapWLANMgr->current_wlan_status.wlan_state[2].ip_type =
                                     QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
        QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_type =
                                 QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
      }

      QcMapWLANMgr->current_wlan_status.bridge_mode_valid = true;

      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
        QcMapWLANMgr->current_wlan_status.bridge_mode = true;
      else
        QcMapWLANMgr->current_wlan_status.bridge_mode = false;
    }
    else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
    {
      QcMapWLANMgr->current_wlan_status.wlan_state_len = 1;
      QcMapWLANMgr->current_wlan_status.wlan_status =
                                            QCMAP_MSGR_WLAN_ENABLED_V01;
      /* WLAN State */
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                            QCMAP_MSGR_WLAN_DISCONNECTED_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                            QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
      QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                                            QCMAP_MSGR_WLAN_IFACE_TYPE_STA_V01;
      QcMapWLANMgr->current_wlan_status.bridge_mode_valid = true;

      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
        QcMapWLANMgr->current_wlan_status.bridge_mode = true;
      else
        QcMapWLANMgr->current_wlan_status.bridge_mode = false;
    }
  }
}

/*===========================================================================
FUNCTION  cleanUpP2PRole
==========================================================================*/
/*!
@brief
CleanUp the p2p role set

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

void cleanUpP2PRole()
{
  char command[MAX_COMMAND_STR_LEN] = {'\0'};
  QCMAP_WLAN *QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);

  if(NULL == QcMapWLANMgr)
  {
    LOG_MSG_ERROR("Please enable wlan with p2p config", 0, 0, 0);
    return FALSE;
  }
  memset(&(QcMapWLANMgr->p2p_config), 0, sizeof(qcmap_p2p_config));

  /* removing P2P-Go role */
  LOG_MSG_INFO1("removing %s from %s", P2P_DEFAULT_IFACE, BRIDGE_IFACE, 0);
  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",
           BRIDGE_IFACE, P2P_DEFAULT_IFACE);
  ds_system_call(command, strlen(command));

  /* removing P2P-Go and P2P-CLI role */
  memset(command, 0, MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s",
           P2P_DEFAULT_IFACE, ZERO_IP_ADDRESS);
  ds_system_call(command, strlen(command));

  /* removing P2P-CLI role*/
  LOG_MSG_INFO1("Killing dhcpcd on %s interface", P2P_DEFAULT_IFACE, 0, 0);
  memset(command, 0, MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",P2P_DEFAULT_IFACE);
  ds_system_call(command, strlen(command));

  /* removing P2P-CLI role*/
  memset(command, 0, MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid", P2P_DEFAULT_IFACE);
  ds_system_call(command, strlen(command));

}

/*===========================================================================
FUNCTION  cleanUpP2PAll
==========================================================================*/
/*!
@brief
CleanUp all the processes which were running when peer-to-peer mode was enabled.

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

void cleanUpP2PAll()
{
  char command[MAX_COMMAND_STR_LEN] = {'\0'};

  /* Kill wpa_supplicant on p2p0 gracefully*/
  memset(command, 0, MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat %s)", P2P_PID_PATH);
  ds_system_call(command, strlen(command));

  /* rm pid file of wpa_supplicant on p2p0*/
  memset(command, 0, MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf %s", P2P_PID_PATH);
  ds_system_call(command, strlen(command));

  /* bring down p2p0 interface */
  LOG_MSG_INFO1("Bringing down Peer-to-peer interface (p2p0)", 0, 0, 0);
  memset(command, 0, MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s down", P2P_DEFAULT_IFACE );
  ds_system_call(command, strlen(command));

  cleanUpP2PRole();

}

/*===========================================================================
FUNCTION  EnableWLAN
==========================================================================*/
/*!
@brief
Enables the wlan based on the wlan chip and  mode configured.

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

boolean QCMAP_WLAN::EnableWLAN( qmi_error_type_v01 *qmi_err_num,
                         boolean enable_wlan_onbootup,
                         boolean privileged_client )
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_MediaService* QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  qcmap_msgr_wlan_mode_enum_v01 mode;
  qcmap_media_svc_status dlna_mode=MEDIA_SVC_MODE_DOWN;
  qcmap_media_svc_status upnp_mode=MEDIA_SVC_MODE_DOWN;
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_msgr_station_mode_config_v01 station_config;
  int qmi_err_num_internal;
  char command[MAX_COMMAND_STR_LEN] = {'\0'};
  char devmode[16] = { '\0' };
  QCMAP_CM_LOG_FUNC_ENTRY();

  ds_system_call("echo QCMAP:Start WLAN Enable > /dev/kmsg",\
  strlen("echo QCMAP:Start WLAN Enable > /dev/kmsg"));

  if( privileged_client && !QCMAP_WLAN::priv_client_restart_wlan)
  {
    LOG_MSG_ERROR("Privileged, not privileged restart, can't enable wlan", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* If wlan is enabled during bootup, we don't need to check mobileap state*/
  if ( !enable_wlan_onbootup && QcMapMgr->qcmap_cm_handle <= 0 )
  {
    LOG_MSG_ERROR("MobileAP not enabled", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  /* If STA Only Mode is enabled; disable it if we are not in Dual wifi mode
     otherwise SCM will handle it*/
  if (QcMapWLANMgr && QcMapWLANMgr->is_sta_only_mode_enabled &&
      !QcMapMgr->dual_wifi_mode)
  {
    /* Kill wpa_supplicant gracefully*/
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
    ds_system_call(command, strlen(command));

    if (!QCMAP_Backhaul_WLAN::IsWpaSupplicantkilled())
    {
      /* Kill wpa_supplicant forcefully. */
      snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
      ds_system_call(command, strlen(command));
    }

    /* Remove Temp Wpa_suplicant File */
    snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf %s", STA_ONLY_MODE_SUPPLICANT_FILE);
    ds_system_call(command, strlen(command));

    /* Disable WLAN */
    LOG_MSG_INFO1("Disable WLAN in STA Only Mode",0,0,0);
    snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
    ds_system_call(command, strlen(command));

    QcMapWLANMgr->is_sta_only_mode_enabled = false;
  }

   if ( (QcMapWLANMgr) &&
    ( QcMapWLANMgr->wifi_mode >= QCMAP_MSGR_WLAN_MODE_AP_V01 &&
     QcMapWLANMgr->wifi_mode <= QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ||
     QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
     QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01) )
  {
    LOG_MSG_ERROR("WLAN is already enabled. Current Mode: %d",QcMapWLANMgr->wifi_mode ,0, 0);
    return true;
  }

#ifdef FEATURE_MOBILEAP_APQ_PLATFORM

    cleanUpP2PAll();

  /* For APQ devices stop already running wpa_supplicant & dhcpcd started by wlan script*/

  /* Kill wpa_supplicant gracefully*/
  snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
  ds_system_call(command, strlen(command));

  if (!QCMAP_Backhaul_WLAN::IsWpaSupplicantkilled())
  {
    /* Kill wpa_supplicant forcefully. */
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
    ds_system_call(command, strlen(command));
  }

  /* Kill dhcpcd*/
  snprintf(command, MAX_COMMAND_STR_LEN, "kill -15"
           " $(cat /var/run/dhcpcd-wlan0.pid)");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-wlan0.pid");
  ds_system_call(command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 dhcpcd" );
  ds_system_call(command, strlen(command));

  /* Disable WLAN */
  LOG_MSG_INFO1("Disable WLAN",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
  ds_system_call(command, strlen(command));

  LOG_MSG_INFO1("MSM8909 device .wpa_supplicant & dhcpcd forcefully killed",0,0,0);

#endif

  /* STA mode is tied to mobile ap handle as all the NAT configuration is
  * tied to mobile ap handle. */

  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,(uint32*)&mode);

#ifndef FEATURE_MOBILEAP_APQ_PLATFORM
  /* STA only Mode is not supported on MDM's. */
  if (mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
      mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
      mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01)
  {
    LOG_MSG_ERROR("Mode:%d is not supported: %d, change the mode first",
                  mode, QcMapMgr->target, 0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return false;
  }
#endif

  if ( (mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 || mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
        mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 || mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
        mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01) && (QcMapMgr->qcmap_tear_down_in_progress ))
  {
    LOG_MSG_ERROR("Cannot enable STA mode when mobileap is in not enabled.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (!QcMapMgr->dual_wifi_mode && (mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
       mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))
  {
    LOG_MSG_ERROR("Cannot enable AP-AP-AP or AP-AP-STA in legacy mode.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
  QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,&station_config);
  if (station_config.ap_sta_bridge_mode &&
      ((QCMAP_MSGR_WLAN_MODE_AP_STA_V01 == mode) ||
       (QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 == mode) ||
       (QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 == mode)) &&
      QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
  {
    LOG_MSG_ERROR("Can't enable AP-STA Bridge mode with "
     "Cradle WAN Bridge mode.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* Not supporting sta only mode in Dual wifi chips. If needed revisit this again.*/
  if ((QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 == mode) && QcMapMgr->dual_wifi_mode)
  {
    LOG_MSG_ERROR("Cannot enable sta only mode in Dual Wifi chip", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  QcMapWLANMgr=QCMAP_WLAN::Get_Instance(true);//Creating WLAN object here

  if(NULL == QcMapWLANMgr)
  {
    LOG_MSG_ERROR("QcMapWLANMgr is NULL", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_MEMORY_V01;
    return false;
  }

  /* Intialize. */
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = false;

  /* Set STA iface index in interface array in lan_cfg */
  QcMapMgr->sta_iface_index = 0;
  /* Set AP device index default values. */
  /* In dual wifi mode this may be changed in EnableWLANModule */
  /* Set defaul AP nums for legacy mode */
  QcMapMgr->ap_dev_num1 = 0;
  QcMapMgr->ap_dev_num2 = 1;
  QcMapMgr->ap_dev_num3 = 2;
  QcMapMgr->ap_dev_num4 = 3;

  /* Verify that the devmode is supported. Allowed ones are:
  AP, AP+AP, AP+STA. */
  LOG_MSG_INFO1("Enable WLAN mode %d", QcMapWLANMgr->wlan_cfg.wlan_mode, 0, 0);
  switch (QcMapWLANMgr->wlan_cfg.wlan_mode)
  {

    case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01:
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].enable = true;
      strlcpy(devmode, "ap-ap-ap", sizeof(devmode));
    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].enable = true;
      if (QcMapWLANMgr->wlan_cfg.wlan_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
        strlcpy(devmode, "ap,ap", sizeof(devmode));
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      if (QcMapWLANMgr->wlan_cfg.wlan_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_V01 &&
            QcMapWLANMgr->wlan_cfg.wlan_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
        strlcpy(devmode, "ap", sizeof(devmode));
      break;
    case QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01:
      strlcpy(devmode, "sta", sizeof(devmode));
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = false;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = true;
      QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(true);
      /* The backhaul instance might have changed, get it again here */
      QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
      QcMapMgr->ap_dev_num1 = -1;
      /* Disable WLAN-IPA interface if AP+STA Bridge Configuration is Active */
      if (QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode &&
          (QcMapMgr->revert_ap_sta_router_mode == false))
      {
        /*Reset fallback flag*/
        QcMapMgr->fallback_ap_sta_router_mode = false;
        /*Take Backup of the WLAN Ini File before modification*/
        snprintf(command,
                 MAX_COMMAND_STR_LEN,
                 "cp %s %s_bak",
                 WLAN_INI_CFG_FILE,
                 WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        /*Modify the INI file*/
        snprintf(command,
                 MAX_COMMAND_STR_LEN,
                 "sed -i '/gIPAConfig=/c\gIPAConfig=0x00' %s",
                 WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        snprintf(command,
                 MAX_COMMAND_STR_LEN,
                 "sed -i '/IpaUcOffloadEnabled=/c\IpaUcOffloadEnabled=0' %s",
                 WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "sed -i '/gIpaUcStaOffload=/c\gIpaUcStaOffload=0' %s",
                 WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        QcMapBackhaulWLANMgr->sta_bridge_mode_activated = true;

        /* Disable DUN and ETH Tethering in AP+STA bridging. */
        if ( QcMapTetheringMgr != NULL )
        {
          if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
             (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                                         QCMAP_QTI_TETHERED_LINK_PPP))
          {
            QCMAP_Tethering::BringDownTetheredLink(QCMAP_QTI_TETHERED_LINK_PPP,
                                                   (void *)NULL,
                                                   &qmi_err_num_internal);
          }
          if ((QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_enabled &&
              (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
                          QCMAP_QTI_TETHERED_LINK_ETH)))
          {
            QCMAP_Tethering::BringDownTetheredLink(QCMAP_QTI_TETHERED_LINK_ETH,
                                                   (void *)NULL,
                                                   &qmi_err_num_internal);
          }
        }
      }
      break;

    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
      strlcpy(devmode, "sta,ap", sizeof(devmode));
    case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01:
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = true;
      QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(true);
      /* The backhaul instance might have changed, get it again here */
      QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();

      if (QcMapWLANMgr->wlan_cfg.wlan_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
        strlcpy(devmode, "sta,ap,ap", sizeof(devmode));

      /* In dual wifi mode this may be changed in EnableWLANModule */
      QcMapMgr->ap_dev_num1 = 1;

      /* Disable WLAN-IPA interface if AP+STA Bridge Configuration is Active */
      if (QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode &&
          (QcMapMgr->revert_ap_sta_router_mode == false))
      {
        /*Reset fallback flag*/
        QcMapMgr->fallback_ap_sta_router_mode = false;
        /*Take Backup of the WLAN Ini File before modification*/
        snprintf(command, MAX_COMMAND_STR_LEN,"cp %s %s_bak",WLAN_INI_CFG_FILE, WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        /*Modify the INI file*/
        snprintf(command, MAX_COMMAND_STR_LEN,"sed -i '/gIPAConfig=/c\gIPAConfig=0x00' %s",WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        snprintf(command, MAX_COMMAND_STR_LEN,"sed -i '/IpaUcOffloadEnabled=/c\IpaUcOffloadEnabled=0' %s",WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        snprintf(command, MAX_COMMAND_STR_LEN,"sed -i '/gIpaUcStaOffload=/c\gIpaUcStaOffload=0' %s",WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        QcMapBackhaulWLANMgr->sta_bridge_mode_activated = true;

        /* Disable DUN and ETH Tethering in AP+STA bridging. */
        if ( QcMapTetheringMgr != NULL )
        {
          if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
             (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                                         QCMAP_QTI_TETHERED_LINK_PPP))
          {
            QCMAP_Tethering::BringDownTetheredLink(QCMAP_QTI_TETHERED_LINK_PPP,
                                                   (void *)NULL,
                                                   &qmi_err_num_internal);
          }
          if ((QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_enabled &&
              (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
                          QCMAP_QTI_TETHERED_LINK_ETH)))
          {
            QCMAP_Tethering::BringDownTetheredLink(QCMAP_QTI_TETHERED_LINK_ETH,
                                                   (void *)NULL,
                                                   &qmi_err_num_internal);
          }
        }
      }
      break;

    case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01:
    {
      /* Check if the target is MDM9607*/
      if (QcMapMgr->target != DS_TARGET_LYKAN)
      {
        *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
         LOG_MSG_ERROR("AP-AP-AP-AP mode supported only for MDM9x07",0,0,0);
         return false;
      }
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].enable = true;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].enable = true;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].enable = true;
      strlcpy(devmode, "ap,ap,ap,ap", sizeof(devmode));
      break;
    }
#ifdef FEATURE_MOBILEAP_APQ_PLATFORM
    case QCMAP_MSGR_WLAN_MODE_AP_P2P_V01:
    {
      strlcpy(devmode, "ap,p2p", sizeof(devmode));
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      break;
    }
    case QCMAP_MSGR_WLAN_MODE_STA_P2P_V01:
    {
      strlcpy(devmode, "sta,p2p", sizeof(devmode));
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = false;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = true;
      QcMapMgr->ap_dev_num1 = -1;
      QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(true);
      break;
    }
#endif
    default:
    {
      /* Incorrect WLAN Mode. Restore it to default mode and continue. */
      LOG_MSG_ERROR("Incorrect WLAN Mode: %d, Start with default Mode.",QcMapWLANMgr->wlan_cfg.wlan_mode, 0, 0);
      QcMapWLANMgr->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_V01;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      strlcpy(devmode, "ap", sizeof(devmode));
      break;
    }
  }

  if(!QcMapBackhaulWLANMgr ||
     ((QcMapBackhaulWLANMgr && !QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode) ||
      QcMapMgr->revert_ap_sta_router_mode == true))
  {
    /* Restore the previous WLAN INI file, command will fail if _bak file does not exist */
    snprintf(command, MAX_COMMAND_STR_LEN,"mv %s_bak %s",WLAN_INI_CFG_FILE, WLAN_INI_CFG_FILE);
    ds_system_call(command, strlen(command));
  }

  /* Update Current WIFI mode. */
  QcMapWLANMgr->wifi_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;

  if (!QcMapMgr->dual_wifi_mode)
  {
    /* Reset the revert ap_sta router mode flag to ensure that next Enable/Disable/Bootup
       AP-STA is started with bridge mode*/
    QcMapMgr->revert_ap_sta_router_mode = false;
  }

  LOG_MSG_INFO1("ap_dev_num1: %d",QcMapMgr->ap_dev_num1, 0, 0);

  // Enable the wlan module
  if(!QcMapWLANMgr->EnableWLANModule(devmode,enable_wlan_onbootup))
  {
    LOG_MSG_ERROR("Cannot enable WLAN Module.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    /*revert the WCNSS_qcom_cfg.ini file if changed.*/
    if (QcMapBackhaulWLANMgr && QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode &&
          (QcMapMgr->revert_ap_sta_router_mode == false))
    {
      snprintf(command, MAX_COMMAND_STR_LEN,"mv %s_bak %s",WLAN_INI_CFG_FILE, WLAN_INI_CFG_FILE);
      ds_system_call(command, strlen(command));
      QcMapBackhaulWLANMgr->sta_bridge_mode_activated = false;
    }

    /* Clear wifi mode */
    QcMapWLANMgr->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
    return false;
  }

  if(!QcMapWLANMgr->activate_wlan_in_progress && !QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if(QcMapMediaServiceMgr)
    {
      if (QcMapMediaServiceMgr->srvc_cfg.dlna_config)
        dlna_mode = MEDIA_SVC_MODE_UP;
      if (QcMapMediaServiceMgr->srvc_cfg.upnp_config)
        upnp_mode = MEDIA_SVC_MODE_UP;
    }
    else
    {
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNA,GET_VALUE, &dlna_mode);
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_UPNP,GET_VALUE, &upnp_mode);
    }
    if (dlna_mode == MEDIA_SVC_MODE_UP)
    {
      if(!QCMAP_MediaService::EnableDLNA(qmi_err_num))
      {
        LOG_MSG_ERROR("Cannot enable DLNA, error: %d.", *qmi_err_num, 0, 0);
      }
    }
    if (upnp_mode == MEDIA_SVC_MODE_UP)
    {
      if(!QCMAP_MediaService::EnableUPNP(qmi_err_num))
      {
        LOG_MSG_ERROR("Cannot enable UPnP, error: %d.", *qmi_err_num, 0, 0);
      }
    }
  }

  /* Reset priviledge client flag. Priviledge client needs to first disable
  * wlan, inorder to request enable wlan */
  if( privileged_client )
  {
    QCMAP_WLAN::priv_client_restart_wlan = false;
    LOG_MSG_INFO1("EnableWLAN is called from a privilaged client",0,0,0);
  }
  ds_system_call("echo QCMAP:WLAN Enabled > /dev/kmsg",
          strlen("echo QCMAP:WLAN Enabled > /dev/kmsg"));

  /* Fill in the WLAN Status IND Struct and Send IND to Clients*/
  FillinWLANStatusIND(QCMAP_MSGR_SCM_INVALID_RECONFIG_IND_V01);
  QcMapWLANMgr->SendWLANStatusIND();

  if(QcMapBackhaulMgr && (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true))
  {
    QcMapBackhaulMgr->StopRadish();
  }

  /*GSB_TIMER_LOGIC
    Not using this for now and enable if needed later.*/
#if 0
  /*Bind IF to GSB*/
  if (QcMapWLANMgr && QcMapMgr->dual_wifi_mode &&
        (QcMapWLANMgr->wifi_mode > QCMAP_MSGR_WLAN_MODE_AP_V01 &&
               QcMapWLANMgr->wifi_mode < QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01))
  {
    QCMAP_GSB::timerid = createGSBTimer();
    for (int ix = 0; ix < QCMAP_MAX_NUM_INTF; ix++)
    {
      if (QcMapWLANMgr->wlan_if_info[ix].state)
      {
        if (QcMapWLANMgr->wlan_if_info[ix].wlan_dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01 &&
              QcMapWLANMgr->wlan_if_info[ix].wlan_ap_type != QCMAP_MSGR_WLAN_IFACE_STATION_V01)
        {
          LOG_MSG_INFO1("setting GSB path for AP %s ",QcMapWLANMgr->wlan_if_info[ix].if_name, 0, 0);
        }
      }
    }
  }
#endif

  return true;
}

/*===========================================================================
FUNCTION  EnableWLANModule
==========================================================================*/
/*!
@brief
This function is invoked, to bringup the wlan module.
This function will load the appropriate driver and brings up the wlan in
the mode configured. Enables dhcpd service for the wlan interfaces.

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::EnableWLANModule
(
char *wlanmode,
boolean enable_wlan_onbootup
)
{
  char command[MAX_COMMAND_STR_LEN] = {'\0'};
  char temp_command[MAX_COMMAND_STR_LEN] = {'\0'};
  struct in_addr addr;
  char devmode[16] = { '\0' };
  int devnum = 0;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  int i, dev;
  uint32_t time_out = (WIFI_DEV_INIT_DELAYS_MAX/WIFI_DEV_INIT_DELAY);
  int j = 0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_gsb_config_v01 config[GSB_MAX_IF_SUPPORT];
  uint8 entries = 0;
  memset(config, 0, sizeof(qcmap_msgr_gsb_config_v01)*GSB_MAX_IF_SUPPORT);

  strlcpy(devmode,wlanmode,sizeof(devmode));

#ifdef FEATURE_MOBILEAP_APQ_PLATFORM
  /* For peer-to-peer modes we do not use the script and it is taken care by QCMAP itself, i.e:
    1)AP+P2P: Enable legacy AP through the script, and then P2P mode without script.
    2)STA+P2P: Enable legacy STA through script and then P2P mode without script */
  if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01)
  {
    strlcpy(devmode,"ap",sizeof(devmode));
  }
  else if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01)
  {
    strlcpy(devmode,"sta",sizeof(devmode));
  }
#endif

  if (strncmp("ap", wlanmode, strlen(wlanmode)) == 0)
    devnum = SCM_MSGR_WLAN_CFG_1_SAP_V01;
  else if (strncmp("sta,ap", wlanmode, strlen(wlanmode)) == 0)
  {
    /* If revert flag was set bridge mode will not be set from EnableWLAN() */
    if (QcMapBackhaulWLANMgr->sta_bridge_mode_activated)
      devnum = SCM_MSGR_WLAN_CFG_1_SAP_1_STA_BRIDGE_MODE_V01;
    else
      devnum = SCM_MSGR_WLAN_CFG_1_SAP_1_STA_ROUTER_MODE_V01;
  }
  else if (strncmp("ap-ap-ap", wlanmode, strlen(wlanmode)) == 0)
    devnum = SCM_MSGR_WLAN_CFG_3_SAP_V01;
  else if (strncmp("ap,ap", wlanmode, strlen(wlanmode)) == 0)
    devnum = SCM_MSGR_WLAN_CFG_2_SAP_V01;
  else if (strncmp("sta,ap,ap", wlanmode, strlen(wlanmode)) == 0)
  {
    /* If revert flag was set bridge mode will not be set from EnableWLAN() */
    if (QcMapBackhaulWLANMgr->sta_bridge_mode_activated)
      devnum = SCM_MSGR_WLAN_CFG_2_SAP_1_STA_BRIDGE_MODE_V01;
    else
      devnum = SCM_MSGR_WLAN_CFG_2_SAP_1_STA_ROUTER_MODE_V01;
  }

  /* This function stops all currently running HostAPD daemons.
  * Since hostapd is not running on bootup, to improve the wlan
  * service availablity, we are spending time stopping hostapd */
  /* In Dual WiFi SCM is in charge of hostapd */
  if(!enable_wlan_onbootup && !QcMapMgr->dual_wifi_mode)
  {
    QcMapWLANMgr->StopHostAPD();
  }

  /*Its better to check if GSB config is cleaned up? before enabling WLAN. We do not
  do this if WLAN is enabled during bootup with AP mode set to avoid having impact
  on boot up KPIs*/

  if (QcMapMgr->dual_wifi_mode &&
        !(enable_wlan_onbootup && (devnum == SCM_MSGR_WLAN_CFG_1_SAP_V01)))
  {
    /*In case of a non graceful shutdown, GSB config is not cleaned. So better clean up
          for WLAN dev type entries to avoid any undesirable state*/
    if (QCMAP_GSB::GetGSBConfig(config , &entries, &qmi_err_num))
    {
      LOG_MSG_INFO1("Got GSB Config successfully",0,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Get GSB Config Failed",0,0,0);
    }

    if (entries > 0)
    {
      LOG_MSG_INFO1("IF configured with GSB %d",entries,0,0);
      for (int i = 0; i < entries; i++)
      {
        if (QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01 == config[i].if_type ||
              QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01 == config[i].if_type)
        {
          LOG_MSG_INFO1("Deleting %s from GSB config", config[i].if_name, 0, 0);
          if (QCMAP_GSB::SetCustomGSBDataPath(config[i].if_name,
                                                QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01, false))
          {
            LOG_MSG_INFO1("Delete GSB config for %s succeeded",config[i].if_name,0,0);
          }
          else
          {
            LOG_MSG_ERROR("Delete GSB config %s failed!!",config[i].if_name,0,0);
          }
        }
      }
    }
  }

  /* Enable IP forwarding */
  ds_system_call("echo 1 > /proc/sys/net/ipv4/ip_forward",
  strlen("echo 1 > /proc/sys/net/ipv4/ip_forward"));

  if (!QcMapMgr->dual_wifi_mode)/* Legacy mode */
  {
    /* Run the WLAN Script to load the WLAN driver. */
    snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan start %s",
      devmode);
    LOG_MSG_INFO1("Bringup WLAN",0,0,0);

    ds_system_call(command, strlen(command));

    for ( i = 0, dev = 0; i < QCMAP_MAX_NUM_INTF; i++ )
    {
      if ( !this->wlan_cfg.interface[i].enable )
        continue;
      snprintf(command, MAX_COMMAND_STR_LEN, "/proc/sys/net/ipv4/conf/wlan%d",dev++);
      while ( j++ < time_out )
      {
        std::ifstream ifile( command );
        if ( ifile )
        {
          break;
        }
        usleep( WIFI_DEV_INIT_DELAY );
      }
    }

    LOG_MSG_INFO1("Wlan card brought up in mode %d.", this->wifi_mode,0,0);
    /* Bring up interfaces.
    This part needs to be done only for AP interfaces.
    Proceed only upto STA iface index since STA is always last in the
    list (ap; ap,ap; ap,sta; ap,ap,sta supported). */
    /* AP2 num is guaranteed to be higher than AP1 */
    for ( i=0, dev = QcMapMgr->ap_dev_num1; i <= QcMapMgr->ap_dev_num4;i++,dev++)
    {
      if ( !this->wlan_cfg.interface[i].enable )
        break;
      /* Start the HostAP daemon. */
      if(!this->StartHostAPD(i, dev))
      {
        LOG_MSG_ERROR("Failed to start hostapd for wlan%d", dev,0,0);
        QcMapWLANMgr->StopHostAPD();
        QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_ALL_AP_V01);
        /* Disable WLAN */
        LOG_MSG_INFO1("Disable WLAN",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
        ds_system_call(command, strlen(command));
        return false;
      }

      /* Add the interfaces to the bridge. */
      if(!this->AssociateWLANtoBridge(dev))
      {
        LOG_MSG_ERROR("Failed to associate wlan%d to bridge", dev,0,0);
        QcMapWLANMgr->StopHostAPD();
        QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_ALL_AP_V01);
        /* Disable WLAN */
        LOG_MSG_INFO1("Disable WLAN",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
        ds_system_call(command, strlen(command));
        return false;
      }

      /* Config WLAN interface IP */
      addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr);
      strlcpy(a5_ip, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg
                    .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
                dev, a5_ip, netmask);
      ds_system_call(command, strlen(command));
      addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr
       & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev wlan%d", subnet, netmask, dev);
      ds_system_call(command, strlen(command));
      /* First delete the link-local route. */
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev wlan%d",
      dev);
      ds_system_call(command, strlen(command));

      this->active_if_count++;
    }

    /* Enable Station Mode. */
    if(NULL != QcMapBackhaulWLANMgr)
      QcMapBackhaulWLANMgr->EnableStaMode();

    PopulateIFInfoforLegacy();
  }
  else /* Dual wifi mode */
  {
    if(!QcMapWLANSCMMgr->SCMEnableWLAN(devnum))
    {
      LOG_MSG_ERROR("SCM Enable Failed", 0, 0, 0);
      return false;
    }
  }

  /* Install access restrictions for the Guest SSID if the profile is
  INTERNETONLY.*/
  if ((this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
      this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
      this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ||
      (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 &&
       !QcMapBackhaulWLANMgr->sta_bridge_mode_activated)) &&
       !QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
  {
    if (this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile ==
       QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
    {
      /* Disable InterBSS in WLAN Driver.. */
      this->InstallGuestAPAccessRules(QcMapMgr->ap_dev_num2);
    }
    else
    {
      /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
      this->SetIPAWLANMode(QcMapMgr->ap_dev_num2, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
    }

    if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
        this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 )
    {
      if (this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile ==
         QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01
         &&
         !QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
      {
        /* Disable InterBSS in WLAN Driver.. */
        this->InstallGuestAPAccessRules(QcMapMgr->ap_dev_num3);
      }
      else
      {
      /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
        this->SetIPAWLANMode(QcMapMgr->ap_dev_num3, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
      }
    }
      //Guest ap3
    if ( this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 )
    {
      if (this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile ==
         QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
          /* Disable InterBSS in WLAN Driver.. */
          this->InstallGuestAPAccessRules(QcMapMgr->ap_dev_num4);
      }
      else
      {
      /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
        this->SetIPAWLANMode(QcMapMgr->ap_dev_num4, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
      }
    }
  }

  // Store the current active configuration in the backup DS
  this->prev_guest_1_profile =
                  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile;
  this->prev_guest_2_profile =
                  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile;
  this->prev_guest_3_profile =
                  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile;
  if(NULL != QcMapBackhaulWLANMgr)
  {
    QcMapBackhaulWLANMgr->prev_station_mode_config.conn_type =
      QcMapBackhaulWLANMgr->apsta_cfg.conn_type;
    memcpy(&QcMapBackhaulWLANMgr->prev_station_mode_config.static_ip_config,
    &QcMapBackhaulWLANMgr->apsta_cfg.static_ip_config,
      sizeof(qcmap_msgr_sta_static_ip_config_v01));
    QcMapBackhaulWLANMgr->prev_sta_bridge_mode = QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode;
  }

#ifdef FEATURE_MOBILEAP_APQ_PLATFORM
  if(QcMapWLANMgr->wlan_cfg.wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
     QcMapWLANMgr->wlan_cfg.wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01)
  {
    memset(command, 0, MAX_COMMAND_STR_LEN);

    /* Start wpa_supplicant on p2p0 interface */
    snprintf( command, MAX_COMMAND_STR_LEN, "wpa_supplicant -i %s -Dnl80211 -B -c %s -P %s",
              P2P_DEFAULT_IFACE, P2P_CFG_PATH, P2P_PID_PATH );
    if (QcMapWLANMgr && (QcMapWLANMgr->debug_config.p2p_supplicant_debug))
    {
      snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command,
                 QcMapWLANMgr->debug_config.p2p_supplicant_dbg_cmd);
      memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
  }
#endif
  return true;
}

/*===========================================================================
FUNCTION  set_p2p_role
==========================================================================*/
/*!
@brief
Enables the wlan based on the wlan chip and  mode configured.

@return
true  - on success
false - on failure

@note

- Dependencies
- MobileAP should be enabled

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::set_p2p_role
(
  qcmap_p2p_config p2p_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  LOG_MSG_INFO1("Entered function %s", __func__, 0, 0);
  boolean ret = TRUE;
  char command[MAX_COMMAND_STR_LEN] = {0};

  if(NULL == qmi_err_num)
  {
    LOG_MSG_ERROR("NULL parameter passed", 0, 0, 0);
    return FALSE;
  }
  *qmi_err_num = QMI_ERR_NONE_V01;

  QCMAP_WLAN *QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);

  if(NULL == QcMapWLANMgr)
  {
    LOG_MSG_ERROR("EnableWLAN in p2p config before setting p2p role", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return FALSE;
  }

  if(QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 != QcMapWLANMgr->wifi_mode &&
     QCMAP_MSGR_WLAN_MODE_STA_P2P_V01 != QcMapWLANMgr->wifi_mode)
  {
    LOG_MSG_ERROR("WLAN config NOT set in p2p mode", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return FALSE;
  }

  if(P2P_ROLE_ENABLE == p2p_config.p2p_status)
  {
    if(TRUE == p2p_config.p2p_role_valid)
    {
      if(QcMapWLANMgr->p2p_config.p2p_status &&
         p2p_config.p2p_role == QcMapWLANMgr->p2p_config.p2p_role)
      {
        LOG_MSG_INFO1(" Already in %d role", p2p_config.p2p_role, 0, 0);
        return TRUE;
      }
      /* Disable the previously set roles*/
      cleanUpP2PRole();

      if(QCMAP_P2P_ROLE_GO_V01 == p2p_config.p2p_role)
      {
        snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s",
                  P2P_DEFAULT_IFACE, P2P_DEFAULT_IFACE_LL_ADDR);
        ds_system_call(command, strlen(command));
        /*Linux won't let you bridge a wireless interface in managed mode at all
          unless you enable the 4addr option */
        memset(command, 0,MAX_COMMAND_STR_LEN);
        snprintf(command, MAX_COMMAND_STR_LEN, "iw dev %s set 4addr on", P2P_DEFAULT_IFACE);
        ds_system_call(command, strlen(command));

        LOG_MSG_INFO1("Adding %s to %s", P2P_DEFAULT_IFACE, BRIDGE_IFACE, 0);
        memset(command, 0,MAX_COMMAND_STR_LEN);
        snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
                BRIDGE_IFACE, P2P_DEFAULT_IFACE);
        ds_system_call(command, strlen(command));

        QcMapWLANMgr->p2p_config.p2p_status = p2p_config.p2p_status;
        QcMapWLANMgr->p2p_config.p2p_role_valid = p2p_config.p2p_role_valid;
        QcMapWLANMgr->p2p_config.p2p_role = p2p_config.p2p_role;
      }
      else if(QCMAP_P2P_ROLE_CLI_V01 == p2p_config.p2p_role)
      {
        LOG_MSG_INFO1("Running DHCP client on %s interface\n",P2P_DEFAULT_IFACE,0,0);
        snprintf( command, MAX_COMMAND_STR_LEN,
                "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",
                 P2P_DEFAULT_IFACE);
        ds_system_call(command, strlen(command));

        QcMapWLANMgr->p2p_config.p2p_status = p2p_config.p2p_status;
        QcMapWLANMgr->p2p_config.p2p_role_valid = p2p_config.p2p_role_valid;
        QcMapWLANMgr->p2p_config.p2p_role = p2p_config.p2p_role;
      }
      else
      {
        LOG_MSG_ERROR("p2p_role is invalid. p2p_role: %d",p2p_config.p2p_role, 0, 0);
        *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
        ret = FALSE;
      }
    }
    else
    {
      LOG_MSG_ERROR("p2p_role_valid cannot be false when p2p_status is enabled. \
                     p2p_role: %d", p2p_config.p2p_role, 0, 0);
      *qmi_err_num = QMI_ERR_MISSING_ARG_V01;
      ret = FALSE;
    }
  }
  else if(P2P_ROLE_DISABLE == p2p_config.p2p_status)
  {
    /* Disable the previously set roles*/
    cleanUpP2PAll();
  }
  LOG_MSG_INFO1("Exiting function: %s", __func__, 0, 0);
  return ret;
}

/*===========================================================================
FUNCTION  get_p2p_role
==========================================================================*/
/*!
@brief
Get p2p role configured

@return
true  - on success
false - on failure

@note

- Dependencies
- MobileAP should be enabled

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::get_p2p_role
(
  qcmap_p2p_config* p2p_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  LOG_MSG_INFO1("Entered function %s", __func__, 0, 0);
  boolean ret = TRUE;

  if(NULL == qmi_err_num || NULL == p2p_config)
  {
    LOG_MSG_ERROR("Null param passed p2p_config: %p, qmi_err_num: %p", p2p_config, qmi_err_num, 0);
    return FALSE;
  }

  *qmi_err_num = QMI_ERR_NONE_V01;

  QCMAP_WLAN *QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);
  if(NULL == QcMapWLANMgr)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("P2P role is diabled", 0, 0, 0);
    return false;
  }

  if (P2P_ROLE_ENABLE == QcMapWLANMgr->p2p_config.p2p_status)
  {
    LOG_MSG_INFO1("P2P role is enabled. P2P-ROLE: %d",QcMapWLANMgr->p2p_config.p2p_role, 0, 0);
    p2p_config->p2p_status = P2P_ROLE_ENABLE;
    p2p_config->p2p_role_valid = TRUE;
    p2p_config->p2p_role = QcMapWLANMgr->p2p_config.p2p_role;
  }
  else if(P2P_ROLE_DISABLE == QcMapWLANMgr->p2p_config.p2p_status)
  {
     /* memset will disable all p2p_status and p2p_role */
     memset(p2p_config, 0, sizeof(qcmap_p2p_config));
     LOG_MSG_INFO1("P2P role is diabled", 0, 0, 0);
  }
  else
  {
    ret = false;
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Could-not get p2p-config",0 ,0 , 0);
  }

  LOG_MSG_INFO1("Exiting function: %s", __func__, 0, 0);
  return ret;
}



/*=====================================================================
FUNCTION DisableWLAN
======================================================================*/
/*!
@brief
Brings down the wlan and stops the dhcp server.

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

boolean QCMAP_WLAN::DisableWLAN( qmi_error_type_v01 *qmi_err_num,
                          boolean privileged_client )
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  int err_num;
  qcmap_msgr_wlan_mode_enum_v01     prev_wifi_mode;
  qcmap_msgr_device_type_enum_v01 device_type =
    QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Firewall* QcMapFirewall=GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_MediaService* QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN * lan_obj = GET_DEFAULT_LAN_OBJECT();
  qmi_error_type_v01 qmi_err;
  qcmap_msgr_gsb_config_v01 config[GSB_MAX_IF_SUPPORT];
  uint8 entries, entries_left = 0;
  boolean gsb_unload = true;
  memset(config, 0, sizeof(qcmap_msgr_gsb_config_v01)*GSB_MAX_IF_SUPPORT);


  /* Check iface management if drivers are unloaded first */

  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_addr_info_list_t* addrList_prev = &(QcMapMgr->addrList_prev);
  ds_dll_el_t * node = NULL;
  ds_dll_el_t * nodeF = NULL;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;

  if (!QcMapWLANMgr || QcMapWLANMgr->wifi_mode ==
       QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("WLAN is already disabled.",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return true;
  }
  //From here on QCMapWLANMgr object is not null and hence can be used without null checks

  /* Stop services before bringing down bridge so byebye messages are sent */
  if( !(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)) &&
      !(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2)) &&
      !QcMapWLANMgr->activate_wlan_in_progress)
  {
    if(QcMapMediaServiceMgr)
    {
      if(QcMapMediaServiceMgr->srvc_cfg.upnp_config)
      {
        LOG_MSG_INFO1("Interim disable UPnP daemon", 0, 0, 0);
        QcMapMediaServiceMgr->InterimDisableUPNP(qmi_err_num);
      }
      if(QcMapMediaServiceMgr->srvc_cfg.dlna_config)
      {
        LOG_MSG_INFO1("Interim disable DLNA daemon", 0, 0, 0);
        QcMapMediaServiceMgr->InterimDisableDLNA(qmi_err_num);
      }
    }
  }

  QcMapWLANMgr->disable_wlan_in_progress = true;

  /* hostapd_cli is needed for CDI in dual wifi and legacy modes */
  /*Stop all instances of hostapd_cli */
  if (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
    QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_ALL_AP_V01);

  if (!QcMapMgr->dual_wifi_mode)
  {
    /* Shut down interfaces.
    Proceed only upto STA iface index since STA is always last in the
    list (ap; ap,ap; ap,sta; ap,ap,sta supported).*/
    QcMapWLANMgr->StopHostAPD();
  }

  /* Disable station mode iface. */
  if(NULL != QcMapBackhaulWLANMgr)
  {
    QcMapBackhaulWLANMgr->DisableStaMode();
  }

  /* Copy connected device list so we can send deprecate RA's if we are
  switching to STA */
  if (addrList->addrListHead != NULL && addrList->addrListHead->next != NULL)
  {
    nodeF = addrList->addrListHead->next;

    if (addrList_prev->addrListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
          information. This is done to make use of the doubly linked list framework which
          is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("Error in allocating memory for node",0,0,0);
        *qmi_err_num = QMI_ERR_NO_MEMORY_V01;
        QcMapWLANMgr->disable_wlan_in_progress = false;
        return false;
      }
      addrList_prev->addrListHead = node;
    }

    while (nodeF != NULL)
    {
      //Copy the mac address, interface name in the linked list
      connectedDevicesList = ( qcmap_cm_client_data_info_t* )nodeF->data;
      if ((node = ds_dll_enq(addrList_prev->addrListHead,
               NULL, (void*)connectedDevicesList )) == NULL)
      {
        LOG_MSG_ERROR("Error in adding a node",0,0,0);
        *qmi_err_num = QMI_ERR_NO_MEMORY_V01;
        QcMapWLANMgr->disable_wlan_in_progress = false;
        return false;
      }
      nodeF = nodeF->next;
    }
    addrList_prev->addrListTail = node;
  }

  else
    LOG_MSG_ERROR("list is empty",0,0,0);

  if(!QcMapWLANMgr->DisableWLANModule())
  {
    LOG_MSG_ERROR("Cannot disable WLAN Module", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    QcMapWLANMgr->disable_wlan_in_progress = false;
    return false;
  }

  if(QcMapMgr->packet_stats_enabled)
  {
    QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_WLAN_DISABLED_V01,
                                  QcMapMgr->conn_device_info.numofWlanclients,NULL);
  }

  /* Delete all the WLAN clients from the Connected Devices List */
  switch (QcMapWLANMgr->wifi_mode)
  {
    case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01:
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01:
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
      break;

    case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01:
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
      break;
  }

  /* AP-STA Bridge Mode cleanup*/
  if (QcMapBackhaulWLANMgr != NULL && QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    /* Restore the previous WLAN INI file */
    snprintf(command, MAX_COMMAND_STR_LEN,"mv %s_bak %s",WLAN_INI_CFG_FILE, WLAN_INI_CFG_FILE);
    ds_system_call(command, strlen(command));

    QcMapMgr->CleanEbtables();
    QcMapMgr->CleanIPtables();

     /* Delete the default route*/
     snprintf(command, MAX_COMMAND_STR_LEN, "route del default gw %s dev %s",
              inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw),
              BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /* Enable ETH link*/
    LOG_MSG_ERROR("SetupTetheredLink on ETH iface",0,0,0);
    if (!QCMAP_Tethering::SetupTetheredLink(QCMAP_QTI_TETHERED_LINK_ETH,
         &err_num))
    {
      LOG_MSG_ERROR("SetupTetheredLink failed %d",err_num,0,0);
    }
    if(QcMapBackhaulMgr)
    {
      QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE);
    }

    if (lan_obj)
    {
      lan_obj->BlockIPv4WWANAccess();//Need to put blocking rules since all have been deleted.
    }

    /*Set accept_ra flag to 1*/
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
    ds_system_call("ip6tables --flush FORWARD",strlen("ip6tables --flush FORWARD")); //need to remove icmp6 rules
    if(lan_obj)
      lan_obj->BlockIPv6WWANAccess();
  }

  /* Corner case where if we just processed a revert and disable is called before
     STA ASSOC we need to reset the revert flag so we come back up in Bridge mode.
     This would happen if user triggers disable before ASSOC */
  if (!QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() && QcMapMgr->dual_wifi_mode)
    QcMapMgr->revert_ap_sta_router_mode = false;

  /* Save the previous wifi mode. */
  prev_wifi_mode = QcMapWLANMgr->wifi_mode;

  /* Reset wifi mode */
  QcMapWLANMgr->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  /* Reset STA mode specific parameters */
  if( QcMapBackhaulWLANMgr && QcMapBackhaulMgr)
  {
    QcMapBackhaulWLANMgr->sta_v4_available = false;
    QcMapBackhaulWLANMgr->sta_v6_available = false;
    QcMapBackhaulWLANMgr->sta_bridge_mode_activated = false;
    QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    delete QcMapBackhaulWLANMgr;
  }
  QcMapMgr->sta_iface_index = QCMAP_LAN_INVALID_IFACE_INDEX;

  QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();

  /* Try switching to backhaul if previous mode is AP-STA. */
  if ((NULL != QcMapBackhaulMgr) &&
      ((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
      || (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE)))
  {
    if ( prev_wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
         prev_wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
         prev_wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
    {
      // Clean up stale entries if previous mode was AP-STA, i.e. WLAN0 was backhaul
      if (QcMapFirewall)
      {
        QcMapFirewall->CleanIPv4MangleTable();
        QcMapFirewall->CleanIPv6MangleTable();
      }
    }

    /* Switch to Cradle/WWAN mode if present */
    if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
    {
      QCMAP_Backhaul::SwitchToOtherBackhaul(QCMAP_Backhaul::current_backhaul,
                                            true,true);
    }
  }

  /* Set all the WLAN enabled boolean values to false */
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = false;

  /* Priviledge client needs to disable wlan and only than, enable wlan is
  * permitted */
  if( privileged_client )
  {
    QCMAP_WLAN::priv_client_restart_wlan = true;
    /* Reset the revert ap_sta_router_mode flag to ensure the for SSR
       we revert back to AP-BRIDGE mode */
    QcMapMgr->revert_ap_sta_router_mode = false;
    LOG_MSG_INFO1("DisableWLAN is called from a privilaged client",0,0,0);
  }

  /* For Activate WLAN Dont Send a Disable IND; other send WLAN IND*/
  if (!QcMapWLANMgr->activate_wlan_in_progress)
  {
    QcMapWLANMgr->current_wlan_status.wlan_status =
                                  QCMAP_MSGR_WLAN_DISABLED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_mode = prev_wifi_mode;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                     QCMAP_MSGR_WLAN_DISCONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                     QCMAP_MSGR_WLAN_DISCONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[2].wlan_iface_state =
                                     QCMAP_MSGR_WLAN_DISCONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[3].wlan_iface_state =
                                     QCMAP_MSGR_WLAN_DISCONNECTED_V01;

    QcMapWLANMgr->current_wlan_status.wlan_state[0].ip4_addr = 0;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].ip4_addr = 0;
    QcMapWLANMgr->current_wlan_status.wlan_state[2].ip4_addr = 0;
    QcMapWLANMgr->current_wlan_status.wlan_state[3].ip4_addr = 0;

    memset(QcMapWLANMgr->current_wlan_status.wlan_state[0].ip6_addr, 0,
                sizeof(QcMapWLANMgr->current_wlan_status.wlan_state[0].ip6_addr));
    memset(QcMapWLANMgr->current_wlan_status.wlan_state[1].ip6_addr, 0,
                sizeof(QcMapWLANMgr->current_wlan_status.wlan_state[1].ip6_addr));
    memset(QcMapWLANMgr->current_wlan_status.wlan_state[2].ip6_addr, 0,
                sizeof(QcMapWLANMgr->current_wlan_status.wlan_state[2].ip6_addr));
    memset(QcMapWLANMgr->current_wlan_status.wlan_state[3].ip6_addr, 0,
                sizeof(QcMapWLANMgr->current_wlan_status.wlan_state[3].ip6_addr));

    QcMapWLANMgr->SendWLANStatusIND();
  }

  /*Reset the fallback flag based on revert flag*/
  if (QcMapMgr && QcMapMgr->dual_wifi_mode && !QcMapMgr->revert_ap_sta_router_mode )
  {
    QcMapMgr->fallback_ap_sta_router_mode = false;
  }

  /*cleanup GSB  & unload if possible*/
  if (QcMapMgr->dual_wifi_mode &&
       (prev_wifi_mode > QCMAP_MSGR_WLAN_MODE_AP_V01 &&
          prev_wifi_mode < QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01))
  {
    if (QCMAP_GSB::GetGSBConfig(config , &entries, qmi_err_num))
    {
      LOG_MSG_INFO1("Got GSB Config successfully",0,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Get GSB Config Failed",0,0,0);
    }
    entries_left = entries;

    if (entries > 0)
    {
      LOG_MSG_INFO1("IF configured with GSB %d",entries,0,0);
      for (int i = 0; i < entries; i++)
      {
        if (QCMAP_MSGR_INTERFACE_TYPE_ETHERNET_V01 == config[i].if_type)
        {
          gsb_unload = false;
        }

        if (QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01 == config[i].if_type ||
              QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01 == config[i].if_type)
        {
          LOG_MSG_INFO1("Deleting %sfrom GSB config", config[i].if_name, 0, 0);
          if (QCMAP_GSB::SetCustomGSBDataPath(config[i].if_name,
                                                QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01, false))
          {
            LOG_MSG_INFO1("Delete GSB config succeeded",0,0,0);
            entries_left--;
          }
          else
          {
            LOG_MSG_ERROR("Delete GSB config failed!!",0,0,0);
          }
        }
      }
    }

    if ((entries_left == 0) || gsb_unload)
    {
      if (QCMAP_GSB::UnLoadGSB(qmi_err_num))
      {
        LOG_MSG_INFO1("Unload GSB succeeded",0,0,0);
      }
      else
      {
        LOG_MSG_INFO1("Unload GSB failed",0,0,0);
      }
    }

    /*GSB_TIMER_LOGIC
      Not using this for now and enable if needed later.*/
#if 0
    destroyGSBTimer(QCMAP_GSB::timerid);
#endif
  }
  QcMapWLANMgr->Destroy_Instance();
  return true;
}


/*=====================================================================
FUNCTION DisableWLANModule
======================================================================*/
/*!
@brief
  Brings down the module for wlan chip  and
  deletes the related routing informations.

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::DisableWLANModule(void)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  boolean usb_enable=false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);

  /*-----------------------------------------------------------------------
  Delete iptables rules.
  -----------------------------------------------------------------------*/
  /* Flush Guest AP access rules*/
  if (this->wifi_mode != QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
  {
    this->DeleteGuestAPAccessRules();
  /* Delete the interfaces from bridge. */
  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
  BRIDGE_IFACE, QcMapMgr->ap_dev_num1);
  ds_system_call(command, strlen(command));
  }

  if ( this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
       this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
       this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ||
       this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
    BRIDGE_IFACE, QcMapMgr->ap_dev_num2);
    ds_system_call(command, strlen(command));

    if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
        this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
                 BRIDGE_IFACE, QcMapMgr->ap_dev_num3);
      ds_system_call(command, strlen(command));

      if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
      {
        snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
                   BRIDGE_IFACE, QcMapMgr->ap_dev_num4);
        ds_system_call(command, strlen(command));
      }
    }
  }
  if(QcMapBackhaulCradleMgr)
  {
    cradle_mode = QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode;
  }
  /* If bridge mode is activated Delete sta interface from bridge */
  /* If USB was enabled restart USB*/
  /* Restart DHCP On bridge, assign IP Address to Bridge*/
  if ((NULL != QcMapBackhaulWLANMgr) && QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",
    BRIDGE_IFACE, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
    if ( QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
         !this->IsSSRInProgress() && (cradle_mode != QCMAP_MSGR_CRADLE_WAN_ROUTER_V01) )
    {
      usb_enable=true;
      if (ds_get_target() == DS_TARGET_LE_CHIRON)
      {
        ds_system_call("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC ",
                strlen("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC "));
      }
      else
      {
        ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",
                     strlen("echo 0 > /sys/class/android_usb/android0/enable "));
      }
    }

    // Assign IP to bridge
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    if(QCMAPLANMgr)
    {
      if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
      {
        LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
        return false;
      }
    } else {
        LOG_MSG_ERROR("Default LAN object is NULL", 0, 0, 0);
        return false;
    }
    addr.s_addr = htonl(lan_cfg->apps_ip_addr);
    strlcpy(a5_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(lan_cfg->sub_net_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
      BRIDGE_IFACE, a5_ip, netmask);
    ds_system_call(command, strlen(command));
    // Restart DHCP Server
    QCMAPLANMgr->StopDHCPD();
    QCMAPLANMgr->StartDHCPD();

    // Assign Original MAC Address to Bridge
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig bridge0 hw ether %s",
               ether_ntoa(&QCMAPLANMgr->GetBridgeMacAddr()));
    ds_system_call(command, strlen(command));

    /* Start USB*/
    if (usb_enable)
    {
      if (DS_TARGET_LE_CHIRON == ds_get_target())
      {
        ds_system_call("echo a600000.dwc3 > /sys/kernel/config/usb_gadget/g1/UDC ",
                strlen("echo a600000.dwc3 > /sys/kernel/config/usb_gadget/g1/UDC "));
      }
      else
      {
        ds_system_call("echo 1 > /sys/class/android_usb/android0/enable ",
                     strlen("echo 1 > /sys/class/android_usb/android0/enable "));
      }
    }
    /* Stop timers if running */
    if (QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running)
      stop_wlan_sta_timer(STA_ASSOC_FAIL);
    if (QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running)
      stop_wlan_sta_timer(STA_DHCP_FAIL);
  }

  if (!QcMapMgr->dual_wifi_mode) //if we change to creating the SCM client at boot, we can always just validate against that client != NULL instead of QcMapMgr var
  {
    snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
    LOG_MSG_INFO1("Bring down WLAN ",0,0,0);
    ds_system_call(command, strlen(command));
  }
  else if (QcMapMgr->dual_wifi_mode)
  {
    //Call SCM stop WIFI
    if(!QcMapWLANSCMMgr->SCMDisableWLAN())
    {
      LOG_MSG_ERROR("SCM Disable WLAN failed", 0, 0, 0);
      return false;
    }
  }

  return true;
}
/*==========================================================
  FUNCTION GetSetAlwaysOnWLANFromXML
==========================================================*/
/*!
@brief
  Set Always On WLAN flag to the XML
  Get Always On WLAN flag from the XML

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

boolean QCMAP_WLAN::GetSetAlwaysOnWLANFromXML
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

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child\
                   (MobileAPLanCfg_TAG).child(AlwaysOnWLAN_TAG);
  if (root == NULL)
  {
    LOG_MSG_ERROR("ALways On WLAN tag not found in %s" , QCMAP_DEFAULT_CONFIG, 0, 0);
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
  LOG_MSG_INFO1("Config: %s Action: %d value: %d" , AlwaysOnWLAN_TAG,
                 action, atoi(data));
  return true;
}

/*==========================================================
  FUNCTION    SetAlwaysOnWLAN
===========================================================*/
/*!
@brief
  set the Always On WLAN

@parameters
  boolean always_on_wlan_state
  qmi_error_type_01 *qmi_err_num

@return
  boolean

@ Dependencies
  - None

@ Side Effects
  - None
*/
/*==========================================================*/
boolean QCMAP_WLAN::SetAlwaysOnWLAN
(
  boolean always_on_wlan_state,
  qmi_error_type_v01 *qmi_err_num
)
{
  char alwaysOnWlan[MAX_STRING_LENGTH] = {0};
  QCMAP_WLAN *QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }

  if (QcMapWLANMgr)
  {
    QcMapWLANMgr->always_on_wlan = always_on_wlan_state;
  }

  snprintf(alwaysOnWlan, MAX_STRING_LENGTH, "%d", always_on_wlan_state);
  if (GetSetAlwaysOnWLANFromXML(SET_VALUE, alwaysOnWlan, MAX_STRING_LENGTH))
  {
    return true;
  }
  *qmi_err_num = QMI_ERR_INTERNAL_V01;
  return false;
}

/*==========================================================
  FUNCTION    GetAlwaysOnWLAN
===========================================================*/
/*!
@brief
  get the Always on WLAN

@parameters
  boolean always_on_wlan_status
  qmi_error_type_01 *qmi_err_num

@return
  boolean

@ Dependencies
  - None

@ Side Effects
  - None
*/
/*==========================================================*/
boolean QCMAP_WLAN::GetAlwaysOnWLAN
(
  boolean *always_on_wlan_status,
  qmi_error_type_v01 *qmi_err_num
)
{
  char alwaysOnWlan[MAX_STRING_LENGTH] = {0};
  QCMAP_WLAN *QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);

  if (!qmi_err_num)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    return false;
  }

  if (!always_on_wlan_status)
  {
    LOG_MSG_ERROR("Invalid parameter passed ",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (QcMapWLANMgr)
  {
    *always_on_wlan_status = QcMapWLANMgr->always_on_wlan;
  }
  else
  {
    if(!GetSetAlwaysOnWLANFromXML(GET_VALUE, alwaysOnWlan, MAX_STRING_LENGTH))
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    *always_on_wlan_status = atoi(alwaysOnWlan);
  }
  return true;
}
/*=====================================================================
FUNCTION SetWLANConfig
======================================================================*/
/*!
@brief
This function will set the wlan configuration.

@return
true  - on Success
false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::SetWLANConfig
(
qcmap_msgr_wlan_mode_enum_v01 wlan_mode,
qcmap_msgr_guest_profile_config_v01*  guest_ap_access_profile,
qcmap_msgr_station_mode_config_v01 *station_config,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  // Initialize QMI error
  *qmi_err_num = QMI_ERR_NONE_V01;
  /* check for address conflicts before setting the wlan configuration */

  if (QCMAPLANMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! LAN Object is NULL", 0, 0, 0);
    return false;
  }

  if( QCMAPLANMgr->CheckforAddrConflict( NULL,station_config ))
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (!QcMapMgr->dual_wifi_mode &&
      (wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
       wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Unable to set AP-AP-AP or AP-AP-STA unless in dual wifi mode",0,0,0);
    return false;
  }

#ifdef FEATURE_MOBILEAP_APQ_PLATFORM
  if( wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 )
  {
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    LOG_MSG_ERROR(" AP+P2P is not supported by the device",0,0,0);
    return false;
  }
  if (QcMapMgr->dual_wifi_mode &&
      (wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
       wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01))
  {
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    LOG_MSG_ERROR(" Cannot set Peer-to-Peer modes when dual wifi is enabled.",0,0,0);
    return false;
  }
#endif

  if (QcMapMgr->target != DS_TARGET_LYKAN &&
      wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("AP-AP-AP-AP mode supported only for 9x07",0,0,0);
    return false;
  }

#ifndef FEATURE_MOBILEAP_APQ_PLATFORM
  /* STA only Mode is not supported on MDM's. */
  if (wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
      wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
      wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01)
  {
    LOG_MSG_ERROR("STA only mode is not supported: %d",
                  QcMapMgr->target, 0, 0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return false;
  }
#endif

  if(QcMapWLANMgr)
  {
    if ( (wlan_mode >= QCMAP_MSGR_WLAN_MODE_AP_V01 &&
         wlan_mode <= QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) ||
         wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_P2P_V01 ||
         wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01)
    {
      QcMapWLANMgr->wlan_cfg.wlan_mode = wlan_mode;
    }

    if (guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] !=
         QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
        .access_profile = guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01];
    }

    if (guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] !=
         QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX]
        .access_profile = guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01];
    }

    if (guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] !=
        QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX]
        .access_profile = guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01];
    }
  }

  if ( station_config != NULL )
  {
    if(NULL != QcMapBackhaulWLANMgr)
    {
      QcMapBackhaulWLANMgr->apsta_cfg.conn_type = station_config->conn_type;
      if (QcMapBackhaulWLANMgr->apsta_cfg.conn_type ==
           QCMAP_MSGR_STA_CONNECTION_STATIC_V01 )
      {
        QcMapBackhaulWLANMgr->apsta_cfg.static_ip_config =
         station_config->static_ip_config;
      }
      QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode =
       station_config->ap_sta_bridge_mode;
    }
    QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(SET_VALUE,station_config);
  }

  /* if it is OK, call write to XML */
  LOG_MSG_INFO1("Set WLAN mode %d",wlan_mode,0,0);
  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,SET_VALUE,
  (uint32 *)&wlan_mode);

  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_ACCESS_PROFILE,SET_VALUE,
  (uint32 *)&guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01]);

  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_2_ACCESS_PROFILE,SET_VALUE,
  (uint32 *)&guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01]);

  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_3_ACCESS_PROFILE,SET_VALUE,
  (uint32 *)&guest_ap_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01]);

  return true;

}

/*=====================================================================
FUNCTION GetWLANConfig
======================================================================*/
/*!
@brief
This function will get the wlan configuration.

@return
true  - on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::GetWLANConfig(qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
                            qcmap_msgr_guest_profile_config_v01 *guest_access_profile,
                            qcmap_msgr_station_mode_config_v01 *station_config,
                            qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);


  if ( wlan_mode == NULL || guest_access_profile == NULL ||
      station_config == NULL )
  {
    LOG_MSG_ERROR("NULL params.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }
  if(QcMapWLANMgr)
  {
    *wlan_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;
    guest_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01] =
    QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile;
    guest_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01] =
    QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile;
    guest_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01] =
    QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile;
  }
  else
  {
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,
                                                          (uint32*)(wlan_mode));
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_ACCESS_PROFILE,GET_VALUE,
    (uint32 *)&guest_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_ONE_V01]);
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_2_ACCESS_PROFILE,GET_VALUE,
    (uint32 *)&guest_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_TWO_V01]);
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_3_ACCESS_PROFILE,GET_VALUE,
    (uint32 *)&guest_access_profile->guest_ap_profile[QCMAP_MSGR_GUEST_AP_THREE_V01]);
  }
  if(NULL != QcMapBackhaulWLANMgr)
  {
    station_config->static_ip_config =
                               QcMapBackhaulWLANMgr->apsta_cfg.static_ip_config;
    station_config->ap_sta_bridge_mode =
                             QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode;
    station_config->conn_type = QcMapBackhaulWLANMgr->apsta_cfg.conn_type;
  }
  else
  {
    memset(station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
    QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,station_config);
  }
  return true;
}


/*=====================================================================
FUNCTION ActivateWLAN
======================================================================*/
/*!
@brief
  Brings up the lan with the latest wlan configuration, wlan mode and access
  profile.

@parameters
  void

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::ActivateWLAN
(
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
  qcmap_msgr_station_mode_config_v01 station_config;


  LOG_MSG_INFO1("QCMAP_WLAN::ActivateWLAN",0,0,0);
  /* STA mode is tied to mobile ap handle as all the NAT configuration is
  * tied to mobile ap handle. */
  if (QcMapWLANMgr)
  {
    wlan_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;
  }
  else
  {
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,
                                        (uint32*)(&wlan_mode));
  }
  LOG_MSG_INFO1("Activate WLAN mode %d",wlan_mode,0,0);


  if ((wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
       wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
       wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01) &&
      ( QcMapMgr->qcmap_tear_down_in_progress || QcMapMgr->qcmap_cm_handle <= 0 ) )
  {
    LOG_MSG_ERROR("Cannot enable STA mode when mobileap is not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
  QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,&station_config);

  if (station_config.ap_sta_bridge_mode &&
      ((QCMAP_MSGR_WLAN_MODE_AP_STA_V01 == wlan_mode) ||
       (QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 == wlan_mode) ||
       (QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 == wlan_mode)) &&
      QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
  {
    LOG_MSG_ERROR("Can't activate AP-STA Bridge mode with Cradle WAN Bridge mode.",
                      0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
      QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
      QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ||
      QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
      &&
      ((!QcMapBackhaulWLANMgr) ||
        (QcMapBackhaulWLANMgr && !QcMapBackhaulWLANMgr->sta_bridge_mode_activated))
      &&
      !QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
  {
    QcMapWLANMgr->UpdateAccessProfileRules();
  }

  /*We need to disable WLAN, re-enable it, if either WLAN MODE or
     Access Profile or Station Mode config has changed*/
  if ( (QcMapWLANMgr && QcMapWLANMgr->IsWlanModeUpdated() &&
       (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01))
       ||(((NULL != QcMapBackhaulWLANMgr) && QcMapBackhaulWLANMgr->IsStaCfgUpdated())&&
       (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
                         QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
                         QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))) ||
       (((NULL != QcMapBackhaulWLANMgr) && QcMapMgr->revert_ap_sta_router_mode == true) &&
        (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
                          QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
                          QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))))
  {
    /* Disable WLAN. */
    QcMapWLANMgr->activate_wlan_in_progress = true;
    QCMAP_WLAN::DisableWLAN(qmi_err_num);

    sleep(QcMapMgr->wlan_restart_delay);
  }

  /* Enable WLAN. */
  if ( !QCMAP_WLAN::EnableWLAN(qmi_err_num) )
  {
    return false;
  }

  return true;
}


/*=====================================================================
FUNCTION RestartWLAN
======================================================================*/
/*!
@brief
  Restarts WLAN.

@parameters
  void

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::RestartWLAN(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
  qmi_error_type_v01 qmi_err_num;

  if (!QcMapWLANMgr)
  {
    LOG_MSG_ERROR("WLAN not enabled.",0,0,0);
    return false;
  }

  LOG_MSG_INFO1("QCMAP_WLAN::RestartWLAN",0,0,0);

  // We need to disable WLAN, re-enable it, if either WLAN MODE or
  //Access Profile or Station Mode config has changed
  if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("WLAN is in disabled state.",0,0,0);
    return false;
  }

  /* STA mode is tied to mobile ap handle as all the NAT configuration is
  * tied to mobile ap handle. */
  if (QcMapWLANMgr)
  {
    wlan_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;
  }
  else
  {
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,
                                        (uint32*)(&wlan_mode));
  }

  if ((wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
       wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
       wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01) &&
  ( QcMapMgr->qcmap_tear_down_in_progress || QcMapMgr->qcmap_cm_handle <= 0 ) )
  {
    LOG_MSG_ERROR("Cannot enable STA mode when mobileap is not enabled.",0,0,0);
    return false;
  }

  /* Disable WLAN. */
  QcMapWLANMgr->activate_wlan_in_progress = true;
  QCMAP_WLAN::DisableWLAN(&qmi_err_num);

  sleep(QcMapMgr->wlan_restart_delay);

  /* Enable WLAN. */
  if ( !QCMAP_WLAN::EnableWLAN(&qmi_err_num) )
  {
    return false;
  }

  return true;
}

/*=====================================================================
FUNCTION GetActiveAPCount
======================================================================*/
/*!
@brief
  Gets the number of active APs

@parameters
  wifi mode

@return
  int
*/
/*====================================================================*/
int QCMAP_WLAN::GetActiveAPCount(qcmap_msgr_wlan_mode_enum_v01 wifi_mode)
{
  switch (wifi_mode)
  {
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_STA_BRIDGE_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_P2P_V01:
      return 1;

    case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_BRIDGE_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
      return 2;

    case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01:
      return 3;

    case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01:
      return 4;
  }
  return QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
}

/*=====================================================================
FUNCTION IsGuestProfile1Updated
======================================================================*/
/*!
@brief
  Checks for Guest AP 1 profile, if updated return status

@parameters
  void

@return
  updated status - on Success

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::IsGuestProfile1Updated()
{
  return  (this->prev_guest_1_profile != this->wlan_cfg.
    interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile);
}

/*=====================================================================
FUNCTION IsGuestProfile2Updated
======================================================================*/
/*!
@brief
  Checks for Guest AP 2 profile, if updated return status

@parameters
  void

@return
  updated status - on Success

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/

boolean QCMAP_WLAN::IsGuestProfile2Updated()
{
  return  (this->prev_guest_2_profile != this->wlan_cfg.
    interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile);
}

/*=====================================================================
FUNCTION GetWLANStatus
======================================================================*/
/*!
@brief
  Gets the wlan configured mode.

@parameters
  qcmap_msgr_wlan_mode_enum_v01 *wlan_mode

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::GetWLANStatus
(
qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  if (wlan_mode == NULL )
  {
    LOG_MSG_ERROR("NULL params.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }
  if(QcMapWLANMgr)
  {
    if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
    {
      if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
        *wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_STA_BRIDGE_V01;
      else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
        *wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_AP_STA_BRIDGE_V01;
      else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
        *wlan_mode = QCMAP_MSGR_WLAN_MODE_STA_ONLY_BRIDGE_V01;
    }
    else
      *wlan_mode = QcMapWLANMgr->wifi_mode;
  }

  return true;
}

/*=====================================================================
FUNCTION ActivateHostapdConfig
======================================================================*/
/*!
@brief
  Restarts Hostapd with the latest configuration.

@parameters
  void

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdConfig
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
qcmap_msgr_activate_hostapd_action_enum_v01 action_type,
qmi_error_type_v01 *qmi_err_num
)
{
  FILE *fp = NULL;
  int ap_pid =0, guest_ap_pid = 0,guest_ap2_pid = 0,guest_ap3_pid = 0, active_ap_cnt = 0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  LOG_MSG_INFO1("ActivateHostapdConfig ap_type = %d, action = %d ", ap_type,action_type,0);

  if (!QcMapWLANMgr || QcMapWLANMgr->wifi_mode ==
        QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("Cannot activate hostapd when WLAN is disabled.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
  {
    LOG_MSG_ERROR("Cannot activate hostapd when WLAN isin sta only mode.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }


  if ( ap_type < QCMAP_MSGR_PRIMARY_AP_V01 ||
       ap_type > QCMAP_MSGR_ALL_AP_V01 )
  {
    LOG_MSG_ERROR("Cannot activate hostapd incorrect ap_type: %d.",ap_type, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  /* Check if the target is MDM9607*/
  if (QcMapMgr->target != DS_TARGET_LYKAN &&
      ap_type == QCMAP_MSGR_GUEST_AP_3_V01)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    LOG_MSG_ERROR("Guest AP3 only supported for AP-AP-AP-AP mode in MDM9x07",0,0,0);
    return false;
  }

  if ( action_type < QCMAP_MSGR_HOSTAPD_START_V01 ||
       action_type > QCMAP_MSGR_HOSTAPD_RESTART_V01 )
  {
    LOG_MSG_ERROR("Cannot activate hostapd incorrect action_type: %d.",action_type, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if ((( ap_type == QCMAP_MSGR_ALL_AP_V01 ||
         ap_type == QCMAP_MSGR_GUEST_AP_V01 ) &&
        (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_V01 &&
         QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 &&
         QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 &&
         QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)) ||
        (ap_type == QCMAP_MSGR_GUEST_AP_2_V01 &&
         QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 &&
         QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) ||
         (ap_type == QCMAP_MSGR_GUEST_AP_3_V01 &&
         QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01))
  {
    LOG_MSG_ERROR("Cannot activate hostapd when AP-AP/AP-AP-STA/AP-AP-AP/AP-AP-AP-AP is not enabled: %d.",
                               QcMapWLANMgr->wifi_mode, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapMgr->dual_wifi_mode)
  {
    /* Get the Primary AP PID. */
    fp = fopen(HOSTAPD_PID_FILE, "r");
    if ( fp == NULL )
    {
      LOG_MSG_ERROR("Error opening hostapd pid file: %d.", errno, 0, 0);
    }
    else if (fscanf(fp, "%d", &ap_pid) != 1)
    {
      LOG_MSG_ERROR("Error reading hostapd file: %d.", errno, 0, 0);
      ap_pid = 0;
      fclose(fp);
    }
    else
    {
      LOG_MSG_INFO1("Primary AP PID = %d",ap_pid,0,0);
      fclose(fp);
    }

    /* Get the Guest AP PID. */
    /* AP-AP-STA check not needed as it can only run in dual wifi */
    if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 )
    {
      fp = fopen(HOSTAPD_SSID2_PID_FILE, "r");
      if ( fp == NULL )
      {
        LOG_MSG_ERROR("Error opening Guest AP hostapd pid file: %d.",errno,0,0);
      }
      else if (fscanf(fp, "%d", &guest_ap_pid) != 1)
      {
        LOG_MSG_ERROR("Error reading Guest AP hostapd file: %d.", errno, 0, 0);
        guest_ap_pid = 0;
        fclose(fp);
      }
      else
      {
        LOG_MSG_INFO1("Guest AP PID = %d",guest_ap_pid,0,0);
        fclose(fp);
      }
    }

    if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 )
    {
      /* Get the Guest AP2 PID. */

      fp = fopen(HOSTAPD_SSID3_PID_FILE, "r");
      if ( fp == NULL )
      {
        LOG_MSG_ERROR("Error opening Guest AP2 hostapd pid file: %d.",errno,0,0);
      }
      else if (fscanf(fp, "%d", &guest_ap2_pid) != 1)
      {
        LOG_MSG_ERROR("Error reading Guest AP hostapd file: %d.", errno, 0, 0);
        guest_ap2_pid = 0;
        fclose(fp);
      }
      else
      {
        LOG_MSG_INFO1("Guest AP2 PID = %d",guest_ap2_pid,0,0);
        fclose(fp);
      }

      /* Get the Guest AP3 PID. */

      fp = fopen(HOSTAPD_SSID4_PID_FILE, "r");
      if ( fp == NULL )
      {
        LOG_MSG_ERROR("Error opening Guest AP hostapd pid file: %d.\n",errno,0,0);
      }
      else if (fscanf(fp, "%d", &guest_ap3_pid) != 1)
      {
        LOG_MSG_ERROR("Error reading Guest AP hostapd file: %d.\n", errno, 0, 0);
        guest_ap3_pid = 0;
        fclose(fp);
      }
      else
      {
        LOG_MSG_INFO1("Guest AP3 PID = %d",guest_ap3_pid,0,0);
        fclose(fp);
      }
    }
  }

  /*Get Active AP count. QcMapWLANMgr should be valid here*/
  active_ap_cnt = QcMapWLANMgr->GetActiveAPCount(QcMapWLANMgr->wifi_mode);

  if (QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 == active_ap_cnt)
  {
    LOG_MSG_ERROR("Cannot determine active APs.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;

  }
  if ( action_type == QCMAP_MSGR_HOSTAPD_START_V01 )
  {
    return QcMapWLANMgr->ActivateHostapdActionStart(ap_type, ap_pid,guest_ap_pid,guest_ap2_pid,
                                                    guest_ap3_pid,active_ap_cnt,qmi_err_num);
  }
  else if ( action_type == QCMAP_MSGR_HOSTAPD_STOP_V01 )
  {
    return QcMapWLANMgr->ActivateHostapdActionStop(ap_type, ap_pid,guest_ap_pid,guest_ap2_pid,
                                                   guest_ap3_pid, qmi_err_num);
  }
  else if ( action_type == QCMAP_MSGR_HOSTAPD_RESTART_V01 )
  {
    return QcMapWLANMgr->ActivateHostapdActionRestart(ap_type, ap_pid,guest_ap_pid,guest_ap2_pid,
                                                      guest_ap3_pid,active_ap_cnt, qmi_err_num);
  }

  return true;
}


/*=====================================================================
FUNCTION ActivateHostapdActionStart
======================================================================*/
/*!
@brief
Starts Hostapd with the latest configuration.

@parameters
void

@return
true  - on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdActionStart
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
int ap_pid,
int guest_ap_pid,
int guest_ap2_pid,
int guest_ap3_pid,
int active_ap_num,
qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  int i, dev;
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  scm_msgr_wlan_if_type_enum_v01 mode[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      /* Start Hostapd for Primary AP. */
      if (!QcMapMgr->dual_wifi_mode)
      {
        if ( ap_pid != 0 )
        {
          LOG_MSG_ERROR("Hostapd already running for Primary AP. PID : %d.",ap_pid, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
        if (!this->StartHostAPD(QCMAP_MSGR_INTF_AP_INDEX, QcMapMgr->ap_dev_num1))
        {
          *qmi_err_num = QMI_ERR_INTERNAL_V01;
          return false;
        }
        addr.s_addr = htonl(this->wlan_cfg
                     .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(netmask, inet_ntoa(addr), 16);
        addr.s_addr = htonl(this->wlan_cfg
                     .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr
                     &this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(subnet, inet_ntoa(addr), 16);
        snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
          wlan%d", subnet, netmask, QcMapMgr->ap_dev_num1);
        ds_system_call(command, strlen(command));
        if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num1))
        {
          LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }
        if (QcMapWLANMgr)
        {
          QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                                    = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
          QcMapWLANMgr->active_if_count++;
        }
      }
      else
      {
        //CALL to SCM start Primary AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_1_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_START_V01);
      }
    }
    break;
    case QCMAP_MSGR_GUEST_AP_V01:
    {
      /* Start Hostapd for GuestAP. */
      if (!QcMapMgr->dual_wifi_mode)
      {
        if ( guest_ap_pid != 0 )
        {
          LOG_MSG_ERROR("Hostapd already running for Guest AP. PID : %d.",guest_ap_pid, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
        if (!this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_INDEX, QcMapMgr->ap_dev_num2))
        {
          *qmi_err_num = QMI_ERR_INTERNAL_V01;
          return false;
        }
        addr.s_addr = htonl(this->wlan_cfg.
                      interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(netmask, inet_ntoa(addr), 16);
        addr.s_addr = htonl(this->wlan_cfg.
                      interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr &this->
                      wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(subnet, inet_ntoa(addr), 16);
        snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s \
                   dev wlan%d", subnet, netmask, QcMapMgr->ap_dev_num2);
        ds_system_call(command, strlen(command));
        if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num2))
        {
          LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }

        if (QcMapWLANMgr)
        {
          QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].state
                                                    = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
          QcMapWLANMgr->active_if_count++;
        }
      }
      else
      {
        //CALL to SCM start Guest AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_2_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_START_V01);
      }
    }
    break;
    case QCMAP_MSGR_GUEST_AP_2_V01:
      if (QcMapMgr->dual_wifi_mode &&
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
      {
        //CALL to SCM start Guest AP 2
        mode[0] = SCM_MSGR_WLAN_IF_SAP_3_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_START_V01);
      }
      /* If dual wifi mode is not active and wlan mode is AP-AP-AP, do nothing*/
      else if ((!QcMapMgr->dual_wifi_mode) &&
                 QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
      {
        LOG_MSG_ERROR("Not in dual Wifi mode",0 , 0, 0);
      }
      else if ((!QcMapMgr->dual_wifi_mode) &&
                 QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
      {
        /* Start Hostapd for GuestAP2. */
        if ( guest_ap2_pid != 0 )
        {
          LOG_MSG_ERROR("Hostapd already running for Guest AP2 PID : %d.",guest_ap2_pid, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
        if (!this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_2_INDEX, QcMapMgr->ap_dev_num3))
        {
          *qmi_err_num = QMI_ERR_INTERNAL_V01;
          return false;
        }
        addr.s_addr = htonl(this->wlan_cfg.
                           interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(netmask, inet_ntoa(addr), 16);
        addr.s_addr = htonl(this->wlan_cfg.
                           interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].ll_ip_addr &this->
                           wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(subnet, inet_ntoa(addr), 16);
        snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s\
                   dev wlan%d", subnet, netmask, QcMapMgr->ap_dev_num3);
        ds_system_call(command, strlen(command));
        if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num3))
        {
          LOG_MSG_ERROR("Associatewlantobridge failed: %d.",0 , 0, 0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }
      }
      else
      {
        LOG_MSG_ERROR("Cannot enable guest AP2 . Improper mode",0 , 0, 0);
      }
    break;

    case QCMAP_MSGR_GUEST_AP_3_V01:
    {
      /* Start Hostapd for GuestAP3. */
      if ( guest_ap3_pid != 0 )
      {
        LOG_MSG_ERROR("Hostapd already running for Guest AP. PID : %d.",guest_ap3_pid, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      if (!this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_3_INDEX, QcMapMgr->ap_dev_num4))
      {
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }
      addr.s_addr = htonl(this->wlan_cfg.
                    interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg.
                    interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].ll_ip_addr &this->
                    wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s\
                 dev wlan%d", subnet, netmask, QcMapMgr->ap_dev_num4);
      ds_system_call(command, strlen(command));
      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num4))
      {
        LOG_MSG_ERROR("Associatewlantobridge failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;

    case QCMAP_MSGR_ALL_AP_V01:
    {
      if (!QcMapMgr->dual_wifi_mode)
      {
        /* Start HostAP for both the AP's.
        This part needs to be done only for AP interfaces.
        Proceed only upto STA iface index since STA is always last in the
        list (ap; ap,ap; ap,sta; ap,ap,sta supported). */
        for ( i=0, dev = QcMapMgr->ap_dev_num1; i < active_ap_num;
             i++, dev++ )
        {
          if ( !this->wlan_cfg.interface[i].enable )
            break;
          /* Start only the non-running hostapd process. */
          if ( i == QCMAP_MSGR_INTF_AP_INDEX && ap_pid != 0 )
            continue;
          else if ( i == QCMAP_MSGR_INTF_GUEST_AP_INDEX && guest_ap_pid != 0 )
            continue;
          else if ( i == QCMAP_MSGR_INTF_GUEST_AP_2_INDEX && guest_ap2_pid != 0 )
            continue;
          else if ( i == QCMAP_MSGR_INTF_GUEST_AP_3_INDEX && guest_ap3_pid != 0 )
            continue;
          /* Start the HostAP daemon. */
          if (!this->StartHostAPD(i, dev))
          {
            *qmi_err_num = QMI_ERR_INTERNAL_V01;
            return false;
          }
          addr.s_addr = htonl(this->wlan_cfg.
            interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(netmask, inet_ntoa(addr), 16);
          addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr
            & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(subnet, inet_ntoa(addr), 16);
          snprintf(command, MAX_COMMAND_STR_LEN,
            "route del -net %s netmask %s dev wlan%d", subnet, netmask, dev);
          ds_system_call(command, strlen(command));
          if(!this->AssociateWLANtoBridge(dev))
          {
            LOG_MSG_ERROR("AssociateWLANtoBridge failed: WLAN%d.",dev , 0, 0);
            *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
            return false;
          }

          if (QcMapWLANMgr && (i < QCMAP_MSGR_INTF_GUEST_AP_2_INDEX))
          {
            QcMapWLANMgr->wlan_if_info[i].state = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
            QcMapWLANMgr->active_if_count++;
          }
        }
      }
      else
      {
        //CALL to SCM start AP-AP-AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_1_V01;
        mode[1] = SCM_MSGR_WLAN_IF_SAP_2_V01;
        mode[2] = SCM_MSGR_WLAN_IF_SAP_3_V01;
        QcMapWLANSCMMgr->SCMReconfig(3, mode, SCM_MSGR_WLAN_IF_START_V01);
      }
    }
    break;
  }

  /*GSB_TIMER_LOGIC
    Not using this for now and enable if needed later.*/
#if 0
  /* start a timer for 5 sec to bimd GSB after SCM reconfigures.*/
  if (QcMapWLANMgr && QcMapMgr->dual_wifi_mode &&
       (QcMapWLANMgr->wifi_mode > QCMAP_MSGR_WLAN_MODE_AP_V01 &&
          QcMapWLANMgr->wifi_mode < QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01))
  {
    /*set timer */
    setGSBTimer(GSB_TIMER_DEFAULT_VAL, QCMAP_GSB::timerid);
  }
#endif

  return true;
}


/*=====================================================================
FUNCTION ActivateHostapdActionStop
======================================================================*/
/*!
@brief
Stops Hostapd for the required AP.

@parameters
void

@return
true  - on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdActionStop
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
int ap_pid,
int guest_ap_pid,
int guest_ap2_pid,
int guest_ap3_pid,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  scm_msgr_wlan_if_type_enum_v01 mode[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];
  qcmap_msgr_device_type_enum_v01 device_type =
    QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      if ( !QcMapMgr->dual_wifi_mode && ap_pid == 0 )
      {
        LOG_MSG_ERROR("Hostapd already stopped for Primary AP. PID : %d.",ap_pid, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the hostapd for Primary AP. */
        KillHostapd(ap_pid, QcMapMgr->ap_dev_num1);
        if ( !IsHostapdkilled(guest_ap_pid,guest_ap2_pid,guest_ap3_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        if (QcMapWLANMgr)
        {
          QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                                    = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
          if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
        }
      }
      else
      {
        //CALL to SCM stop Primary AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_1_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_STOP_V01);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    }
    break;
    case QCMAP_MSGR_GUEST_AP_V01:
    {
      if ( !QcMapMgr->dual_wifi_mode && guest_ap_pid == 0 )
      {
        LOG_MSG_ERROR("Hostapd already stopped for Guest AP. PID : %d.",guest_ap_pid, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the hostapd for Guest AP. */
        KillHostapd(guest_ap_pid, QcMapMgr->ap_dev_num2);
        if ( !IsHostapdkilled(ap_pid,guest_ap2_pid,guest_ap3_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        if (QcMapWLANMgr)
        {
          QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].state
                                                    = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
           if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
        }
      }
      else
      {
        //CALL to SCM stop Guest AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_2_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_STOP_V01);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    }
    break;

    case QCMAP_MSGR_GUEST_AP_2_V01:
    {
      if (QcMapMgr->dual_wifi_mode)
      {
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num3);
        //CALL to SCM stop Guest AP 2
        mode[0] = SCM_MSGR_WLAN_IF_SAP_3_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_STOP_V01);
        LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }
      else
      {
        if ( guest_ap2_pid == 0 )
        {
          LOG_MSG_ERROR("Hostapd already stopped for Guest AP2. PID : %d.",guest_ap2_pid, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num3);

        this->StopHostapdCli(ap_type);
        /* Stop the hostapd for Guest AP. */
        KillHostapd(guest_ap2_pid, QcMapMgr->ap_dev_num3);
        if ( !IsHostapdkilled(ap_pid,guest_ap_pid,guest_ap3_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_3_V01:
    {
      if (( !QcMapMgr->dual_wifi_mode) || ( guest_ap3_pid == 0 ))
      {
        LOG_MSG_ERROR("Hostapd already stopped for Guest AP3. PID : %d.",guest_ap2_pid, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num4);
      this->StopHostapdCli(ap_type);
      /* Stop the hostapd for Guest AP. */
      KillHostapd(guest_ap3_pid, QcMapMgr->ap_dev_num4);
      if ( !IsHostapdkilled(ap_pid,guest_ap_pid,guest_ap2_pid) )
      {
        LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    }
    break;

    case QCMAP_MSGR_ALL_AP_V01:
    {
      if (!QcMapMgr->dual_wifi_mode)
      {
        /* pid validation in this case is only valid for legacy mode*/
        if ( ap_pid == 0 && guest_ap_pid == 0 && guest_ap2_pid == 0 && guest_ap3_pid == 0 )
        {
          LOG_MSG_ERROR("Hostapd already stopped for All AP's",0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
        {
          DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num4);
          DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num3);
        }

      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01))
          DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);

      this->StopHostapdCli(ap_type);
      /* Stop the Hostapd for all the AP's. */
      StopHostAPD();

      if ( !IsHostapdkilled(0,0,0) )
      {
        LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }

      if (QcMapWLANMgr)
      {
        QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].state
                                                  = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
        if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
        QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                                  = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
        if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
      }

      if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
      {
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }

      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01))
      {
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }

      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }
      else
      {
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num3);
        //CALL to SCM stop AP-AP-AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_1_V01;
        mode[1] = SCM_MSGR_WLAN_IF_SAP_2_V01;
        mode[2] = SCM_MSGR_WLAN_IF_SAP_3_V01;
        QcMapWLANSCMMgr->SCMReconfig(3, mode, SCM_MSGR_WLAN_IF_STOP_V01);

        LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
        device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }
    }
    break;
  }
  return true;
}

/*=====================================================================
FUNCTION ActivateHostapdActionRestart
======================================================================*/
/*!
@brief
Restarts Hostapd with the latest configuration.

@parameters
void

@return
true  - on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdActionRestart
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
int ap_pid,
int guest_ap_pid,
int guest_ap2_pid,
int guest_ap3_pid,
int active_ap_num,
qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  int i, dev;
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  scm_msgr_wlan_if_type_enum_v01 mode[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];
  qcmap_msgr_device_type_enum_v01 device_type =
                                   QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  int num_ifaces = 0;

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      /* This function was required, when wlan is configured in ht_capable mode,
          to restart hostapd, wlan interface needs to be disassociate with bridge */
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);
      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the process only if it is not already killed. */
        if ( ap_pid != 0 )
        {
          KillHostapd(ap_pid, QcMapMgr->ap_dev_num1);
        }
        if ( !IsHostapdkilled(guest_ap_pid,guest_ap2_pid,guest_ap3_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Sleep so clients will disconnect */
        sleep(QcMapMgr->wlan_restart_delay);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);

      /* Restart Hostapd for Primary AP. */
      if (QcMapWLANMgr && !QcMapMgr->dual_wifi_mode)
      {
        QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                                  = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
        if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
        if (!this->StartHostAPD(QCMAP_MSGR_INTF_AP_INDEX, QcMapMgr->ap_dev_num1))
        {
          *qmi_err_num = QMI_ERR_INTERNAL_V01;
          return false;
        }
        addr.s_addr = htonl(this->wlan_cfg.
                            interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(netmask, inet_ntoa(addr), 16);
        addr.s_addr = htonl(this->wlan_cfg.
        interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr & this->wlan_cfg.
        interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(subnet, inet_ntoa(addr), 16);
        snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
          wlan%d", subnet, netmask, QcMapMgr->ap_dev_num1);
        ds_system_call(command, strlen(command));
        if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num1))
        {
          LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }

        if (QcMapWLANMgr)
        {
          QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                                    = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
          QcMapWLANMgr->active_if_count++;
        }
      }
      else
      {
        //CALL to SCM start Primary AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_1_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_RESTART_V01);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_V01:
    {
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the process only if it not already killed. */
        if ( guest_ap_pid != 0 )
        {
          /* Stop the hostapd for Guest AP. */
          KillHostapd(guest_ap_pid, QcMapMgr->ap_dev_num2);
        }
        if ( !IsHostapdkilled(ap_pid,guest_ap2_pid,guest_ap3_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Sleep so clients will disconnect */
        sleep(QcMapMgr->wlan_restart_delay);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);

      /* Restart Hostapd for GuestAP. */
      if (QcMapWLANMgr && !QcMapMgr->dual_wifi_mode)
      {
        QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].state
                                                  = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
        if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
        if (!this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_INDEX, QcMapMgr->ap_dev_num2))
        {
          *qmi_err_num = QMI_ERR_INTERNAL_V01;
          return false;
        }

        addr.s_addr = htonl(this->wlan_cfg.
          interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(netmask, inet_ntoa(addr), 16);
        addr.s_addr = htonl(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
          .ll_ip_addr & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
          .ll_subnet_mask);
        strlcpy(subnet, inet_ntoa(addr), 16);
        snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
                    wlan%d", subnet, netmask, QcMapMgr->ap_dev_num2);
        ds_system_call(command, strlen(command));
        if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num2))
        {
          LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }

        if (QcMapWLANMgr)
        {
          QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].state
                                                    = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
          QcMapWLANMgr->active_if_count++;
        }
      }
      else
      {
        //CALL to SCM start Guest AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_2_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_RESTART_V01);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_2_V01:
    {
      if (QcMapMgr->dual_wifi_mode)
      {
        //CALL to SCM start Guest AP
        mode[0] = SCM_MSGR_WLAN_IF_SAP_3_V01;
        QcMapWLANSCMMgr->SCMReconfig(1, mode, SCM_MSGR_WLAN_IF_RESTART_V01);
      }
      else
      {
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num3);
        this->StopHostapdCli(ap_type);

        /* Stop the process only if it not already killed. */
        if ( guest_ap2_pid != 0 )
        {
          /* Stop the hostapd for Guest AP. */
          KillHostapd(guest_ap2_pid, QcMapMgr->ap_dev_num3);
        }
        if ( !IsHostapdkilled(ap_pid,guest_ap_pid,guest_ap3_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Sleep so clients will disconnect */
        sleep(QcMapMgr->wlan_restart_delay);

        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
        /* Restart Hostapd for GuestAP. */
        if (!this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_2_INDEX, QcMapMgr->ap_dev_num3))
        {
          *qmi_err_num = QMI_ERR_INTERNAL_V01;
          return false;
        }
        addr.s_addr = htonl(this->wlan_cfg.
          interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(netmask, inet_ntoa(addr), 16);
        addr.s_addr = htonl(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX]
          .ll_ip_addr & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
          .ll_subnet_mask);
        strlcpy(subnet, inet_ntoa(addr), 16);
        snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
          wlan%d", subnet, netmask, QcMapMgr->ap_dev_num3);
        ds_system_call(command, strlen(command));
        if(!AssociateWLANtoBridge(QcMapMgr->ap_dev_num3))
        {
          LOG_MSG_ERROR("Associatewlantobridge failed: %d.",0 , 0, 0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_3_V01:
    {
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num4);
      this->StopHostapdCli(ap_type);

      /* Stop the process only if it not already killed. */
      if ( guest_ap3_pid != 0 )
      {
        /* Stop the hostapd for Guest AP. */
        KillHostapd(guest_ap3_pid, QcMapMgr->ap_dev_num4);
      }
      if ( !IsHostapdkilled(ap_pid,guest_ap_pid,guest_ap2_pid) )
      {
        LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }

      /* Sleep so clients will disconnect */
      sleep(QcMapMgr->wlan_restart_delay);

      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
      /* Restart Hostapd for GuestAP. */
      if (!this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_3_INDEX, QcMapMgr->ap_dev_num4))
      {
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }
      addr.s_addr = htonl(this->wlan_cfg.
        interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX]
        .ll_ip_addr & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
        .ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
        wlan%d", subnet, netmask, QcMapMgr->ap_dev_num4);
      ds_system_call(command, strlen(command));
      if(!AssociateWLANtoBridge(QcMapMgr->ap_dev_num4))
      {
        LOG_MSG_ERROR("Associatewlantobridge failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;

    case QCMAP_MSGR_ALL_AP_V01:
    {
      if (!QcMapMgr->dual_wifi_mode)
      {
        if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
        {
          DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num4);
          DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num3);
        }
        if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
          DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);

        this->StopHostapdCli(ap_type);

        /* Stop Hostapd. */
        StopHostAPD();

        if ( !IsHostapdkilled(0,0,0) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Sleep so clients will disconnect */
        sleep(QcMapMgr->wlan_restart_delay);

        if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
        {
          device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
          QcMapMgr->DeleteConnectedDevicesClients(device_type);
          device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01;
          QcMapMgr->DeleteConnectedDevicesClients(device_type);
        }

        if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
        {
          device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
          QcMapMgr->DeleteConnectedDevicesClients(device_type);
        }

        device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }
      else
      {
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);
        DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num3);
      }

      if (QcMapWLANMgr && !QcMapMgr->dual_wifi_mode)
      {
         QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01].state
                                                  = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
        if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
        QcMapWLANMgr->wlan_if_info[QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01].state
                                                  = QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01;
        if(QcMapWLANMgr->active_if_count) QcMapWLANMgr->active_if_count--;
        /* Bring up interfaces.
          This part needs to be done only for AP interfaces.
          Proceed only upto STA iface index since STA is always last in the
          list (ap; ap,ap; ap,sta; ap,ap,sta;ap,ap,ap,ap supported). */
        for ( i=0, dev = QcMapMgr->ap_dev_num1; i < active_ap_num; i++,
              dev++ )
        {
          if ((i >= QCMAP_MAX_NUM_INTF) || (!this->wlan_cfg.interface[i].enable) )
          break;

          /* Start the HostAP daemon. */
          if (!this->StartHostAPD(i, dev))
          {
            *qmi_err_num = QMI_ERR_INTERNAL_V01;
            return false;
          }
          addr.s_addr = htonl(this->wlan_cfg.
            interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(netmask, inet_ntoa(addr), 16);
          addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr & this->wlan_cfg
           .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(subnet, inet_ntoa(addr), 16);
          snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
            wlan%d", subnet, netmask, dev);
          ds_system_call(command, strlen(command));

          if(!this->AssociateWLANtoBridge(dev))
          {
            LOG_MSG_ERROR("AssociateWLANtoBridge dev1 failed: wlan%d.",dev , 0, 0);
            *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
            return false;
          }

          if (QcMapWLANMgr && (i < QCMAP_MSGR_INTF_GUEST_AP_2_INDEX))
          {
            QcMapWLANMgr->wlan_if_info[i].state = QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01;
            QcMapWLANMgr->active_if_count++;
          }
        }
      }
      else
      {
        LOG_MSG_INFO1("Delete CDI information before SCM restart", 0, 0, 0);
        device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
        mode[0] = SCM_MSGR_WLAN_IF_SAP_1_V01;
        num_ifaces = 1;

        if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
            QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
        {
          device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
          QcMapMgr->DeleteConnectedDevicesClients(device_type);
          mode[1] = SCM_MSGR_WLAN_IF_SAP_2_V01;
          num_ifaces = 2;
        }

        if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
        {
          device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
          QcMapMgr->DeleteConnectedDevicesClients(device_type);
          mode[2] = SCM_MSGR_WLAN_IF_SAP_3_V01;
          num_ifaces = 3;
        }

        //CALL to SCM start AP-AP-AP
        QcMapWLANSCMMgr->SCMReconfig(num_ifaces, mode, SCM_MSGR_WLAN_IF_RESTART_V01);
      }
    }
    break;
  }

  /*GSB_TIMER_LOGIC
    Not using this for now and enable if needed later.*/
#if 0
  if (QcMapWLANMgr && QcMapMgr->dual_wifi_mode &&
       (QcMapWLANMgr->wifi_mode > QCMAP_MSGR_WLAN_MODE_AP_V01 &&
          QcMapWLANMgr->wifi_mode < QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01))
  {
    /*set timer */
    setGSBTimer(GSB_TIMER_DEFAULT_VAL, QCMAP_GSB::timerid);
  }
#endif

  return true;
}

/*==========================================================
 FUNCTION    GetActiveWLANIFInfo
===========================================================*/
/*!
@brief
  Gets Active WLAN IF Info

@parameters

@return
  1 - on success
  0 - on failure

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
boolean QCMAP_WLAN::GetActiveWLANIFInfo
(
  qcmap_msgr_wlan_if_info_v01 *wlan_info,
  uint8 *num_of_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  uint8 ix = 0;

  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapWLANMgr)
  {
    *num_of_entries = 0;
    LOG_MSG_ERROR("WLAN not activated?",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  *num_of_entries = QcMapWLANMgr->active_if_count;
  LOG_MSG_INFO1("entries %d", QcMapWLANMgr->active_if_count , 0 ,0);

  if (*num_of_entries <= QCMAP_MAX_NUM_INTF)
  {
    for (ix = 0; ix < QCMAP_MAX_NUM_INTF; ix++)
    {
      if (QcMapWLANMgr->wlan_if_info[ix].state)
      {
        strlcpy(wlan_info[ix].if_name, QcMapWLANMgr->wlan_if_info[ix].if_name,
                                      QCMAP_MAX_IFACE_NAME_SIZE_V01);
        wlan_info[ix].state = QcMapWLANMgr->wlan_if_info[ix].state;
        wlan_info[ix].wlan_ap_type = QcMapWLANMgr->wlan_if_info[ix].wlan_ap_type;
        wlan_info[ix].wlan_dev_type = QcMapWLANMgr->wlan_if_info[ix].wlan_dev_type;
      }
    }
  }
  else
  {
    //we should never be here
    LOG_MSG_ERROR("Invalid num of entries",0,0,0);
    *num_of_entries = 0;
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  LOG_MSG_INFO1("Entries %d",*num_of_entries,0,0);
  for ( ix = 0; ix < QCMAP_MAX_NUM_INTF; ix++)
  {
    LOG_MSG_INFO1("%d, state %d", ix, wlan_info[ix].state, 0);
    if (wlan_info[ix].state)
    {
      LOG_MSG_INFO1("%d, %s", ix, wlan_info[ix].if_name, 0);
    }
  }

  return true;
}
/*==========================================================
 FUNCTION    GetWLANDeviceTypeFromAP
===========================================================*/
/*!
@brief
  Gets Active WLAN IF Info

@parameters

@return
  1 - on success
  0 - on failure

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
boolean QCMAP_WLAN::GetWLANDeviceTypeFromAP
(
  qcmap_msgr_device_type_enum_v01 ap_type,
  qcmap_msgr_wlan_device_type_v01 *wlan_dev_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_wlan_if_info_v01 wlan_if_info[QCMAP_MSGR_MAX_WLAN_IFACE_V01];
  qcmap_msgr_wlan_iface_index_enum_v01 wlan_ap_type;
  uint8 num_of_entries = 0;
  int i = 0;

  if (QCMAP_WLAN::GetActiveWLANIFInfo(wlan_if_info, &num_of_entries, qmi_err_num))
  {
    if (num_of_entries > 0)
    {
      switch (ap_type)
      {
        case QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01:
          wlan_ap_type = QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01;
          break;
        case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01:
          wlan_ap_type = QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01;
          break;
        case QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01:
          wlan_ap_type = QCMAP_MSGR_WLAN_IFACE_GUEST_AP_TWO_V01;
          break;
        default:
          *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
          return false;
      }
      for (int i =0; i < num_of_entries; i++)
      {
        if (wlan_if_info[i].wlan_ap_type == wlan_ap_type )
        {
          *wlan_dev_type = wlan_if_info[i].wlan_dev_type;
          break;
        }
      }
    }
  }

  return true;
}
/*==========================================================
 FUNCTION    IsAPUpOnTUF
===========================================================*/
/*!
@brief
  Gets Active WLAN IF Info

@parameters

@return
  1 - on success
  0 - on failure

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
boolean QCMAP_WLAN::IsAPUpOnTUF(void)
{
  qcmap_msgr_wlan_if_info_v01 wlan_if_info[QCMAP_MSGR_MAX_WLAN_IFACE_V01];
  uint8 num_of_entries = 0;
  int i = 0;
  qmi_error_type_v01 qmi_err_num;

  if (QCMAP_WLAN::GetActiveWLANIFInfo(wlan_if_info, &num_of_entries, &qmi_err_num))
  {
    if (num_of_entries > 0)
    {
      for (int i =0; i < num_of_entries; i++)
      {
        if ((wlan_if_info[i].wlan_ap_type != QCMAP_MSGR_WLAN_IFACE_STATION_V01) &&
            (wlan_if_info[i].wlan_dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01 ))
          return true;
      }
    }
  }

  return false;
}

/*=====================================================================
FUNCTION StopHostapdCli
======================================================================*/
/*!
@brief
Kills appropriate hostapd_cli process

@parameters
- qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
void

@note

- Dependencies
- None

- Side Effects
- None

/*====================================================================*/
void QCMAP_WLAN::StopHostapdCli
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);


  LOG_MSG_INFO1("QCMAP_WLAN::StopHostapdCli", 0, 0, 0);
  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "kill -15 `ps -ef | grep hostapd_cli | grep wlan%d"
      "| awk '{print $1}'`",QcMapMgr->ap_dev_num1);
      ds_system_call(command, strlen(command));
    }
    break;

    case QCMAP_MSGR_GUEST_AP_V01:
    {
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "kill -15 `ps -ef | grep hostapd_cli | grep wlan%d"
      "| awk '{print $1}'`",QcMapMgr->ap_dev_num2);
      ds_system_call(command, strlen(command));
    }
    break;

    case QCMAP_MSGR_GUEST_AP_2_V01:
    {
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "kill -15 `ps -ef | grep hostapd_cli | grep wlan%d"
      "| awk '{print $1}'`",QcMapMgr->ap_dev_num3);
      ds_system_call(command, strlen(command));
    }
    break;

     case QCMAP_MSGR_GUEST_AP_3_V01:
    {
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "kill -15 `ps -ef | grep hostapd_cli | grep wlan%d"
      "| awk '{print $1}'`",QcMapMgr->ap_dev_num4);
      ds_system_call(command, strlen(command));
    }
    break;

    case QCMAP_MSGR_ALL_AP_V01:
    {
      /*Kill the hostapd_cli */
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "killall hostapd_cli");
      ds_system_call(command, strlen(command));
    }
    break;
    default:
    {
      LOG_MSG_ERROR("QCMAP_WLAN::StopHostapdCli-Invalid ap type %d ",ap_type, 0, 0);
    }
    break;
  }
}

/*=====================================================================
FUNCTION StartHostapdCli
======================================================================*/
/*!
@brief
Start appropriate hostapd_cli process

@parameters
- qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN::StartHostapdCli
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_2_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num3))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_3_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num4))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_ALL_AP_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
      if(!hostapdclicmd(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
      if(!hostapdclicmd(QcMapMgr->ap_dev_num3))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
      if(!hostapdclicmd(QcMapMgr->ap_dev_num4))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    default:
    {
      LOG_MSG_ERROR("QCMAP_WLAN::StartHostapdCli-Invalid ap type %d",ap_type, 0, 0);
    }
    break;
  }
}

/*=====================================================================
FUNCTION StopHostAPD
======================================================================*/
/*!
@brief
Stops hostapd service.

@parameters
int interface

@return
true  - on success
false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::StopHostAPD(void)
{
  FILE *fp = NULL;
  int ap_pid = 0, guest_ap_pid = 0,guest_ap2_pid = 0,guest_ap3_pid = 0;
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  ds_system_call("killall hostapd",
   strlen("killall hostapd"));

  switch (this->wifi_mode)
  {
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
      /* AP-AP-STA check not needed as this function is legacy only */
      /* Only one instance of hostapd is running */
      if (!IsHostapdkilled(0,0,0) )
      {
        LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
        ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
      }
      snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_PID_FILE);
      ds_system_call(command, strlen(command));
      KillHostapd(0, QcMapMgr->ap_dev_num1);
    break;

    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01:
      /* Get the Primary AP PID. */
      fp = fopen(HOSTAPD_PID_FILE, "r");
      if ( fp == NULL )
      {
        LOG_MSG_ERROR("Error opening hostapd pid file: %d.", errno, 0, 0);
      }
      else if (fscanf(fp, "%d", &ap_pid) != 1)
      {
        LOG_MSG_ERROR("primary AP hostapd is stopped: %d.", errno, 0, 0);
        ap_pid = 0;
        fclose(fp);
      }
      else
      {
        LOG_MSG_INFO1("Primary AP PID = %d",ap_pid,0,0);
        fclose(fp);
      }

      /* Get the Guest AP PID. */
      fp = fopen(HOSTAPD_SSID2_PID_FILE, "r");
      if ( fp == NULL )
      {
        LOG_MSG_ERROR("Error opening Guest AP hostapd pid file: %d.\n",errno,0,0);
      }
      else if (fscanf(fp, "%d", &guest_ap_pid) != 1)
      {
        LOG_MSG_ERROR("Guest AP1 hostapd is stopped: %d.\n", errno, 0, 0);
       guest_ap_pid = 0;
       fclose(fp);
      }
      else
      {
        LOG_MSG_INFO1("Guest AP1 PID = %d",guest_ap_pid,0,0);
        fclose(fp);
      }

     /*If wlan mode is AP-AP-AP-AP mode*/
      if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
      {
        /* Get the Guest AP2 PID. */
        fp = fopen(HOSTAPD_SSID3_PID_FILE, "r");
        if ( fp == NULL )
        {
          LOG_MSG_ERROR("Error opening Guest AP 2 hostapd pid file: %d.\n",errno,0,0);
        }
        else if (fscanf(fp, "%d", &guest_ap2_pid) != 1)
        {
          LOG_MSG_ERROR("Guest AP 2 hostapd is stopped: %d.\n", errno, 0, 0);
          guest_ap2_pid = 0;
          fclose(fp);
        }
        else
        {
          LOG_MSG_INFO1("Guest AP 2 PID = %d",guest_ap2_pid,0,0);
          fclose(fp);
        }

        /* Get the Guest AP3 PID. */
        fp = fopen(HOSTAPD_SSID4_PID_FILE, "r");
        if ( fp == NULL )
        {
          LOG_MSG_ERROR("Error opening Guest AP 3 hostapd pid file: %d.\n",errno,0,0);
        }
        else if (fscanf(fp, "%d", &guest_ap3_pid) != 1)
        {
          LOG_MSG_ERROR("Guest AP 3 hostapd is stopped: %d.\n", errno, 0, 0);
          guest_ap3_pid = 0;
          fclose(fp);
        }
        else
        {
          LOG_MSG_INFO1("Guest AP 3 PID = %d",guest_ap3_pid,0,0);
          fclose(fp);
        }

         /* Check if Guest ap 2 hostapd is killed */
         if(guest_ap2_pid !=0 )
         {
           if ( !IsHostapdkilled(ap_pid,guest_ap_pid,guest_ap3_pid) )
           {
             LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
             ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
           }
         }

         KillHostapd(0, QcMapMgr->ap_dev_num3);
         snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_SSID3_PID_FILE);
         ds_system_call(command, strlen(command));

       /* Check if Guest ap3 hostapd is killed */
       if(guest_ap3_pid !=0 )
       {
         if ( !IsHostapdkilled(ap_pid,guest_ap_pid,guest_ap2_pid) )
         {
           LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
           ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
         }
       }

       KillHostapd(0, QcMapMgr->ap_dev_num4);
       snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_SSID4_PID_FILE);
       ds_system_call(command, strlen(command));
     }

     /* Check if Primary ap hostapd is killed */
     if(ap_pid !=0 )
     {
       if (!IsHostapdkilled(guest_ap_pid,guest_ap2_pid,guest_ap3_pid) )
       {
         LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
         ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
       }
     }

     KillHostapd(0, QcMapMgr->ap_dev_num1);
     snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_PID_FILE);
     ds_system_call(command, strlen(command));

     /* Check if Guest AP1 hostapd is killed */
     if(guest_ap_pid !=0 )
     {
       if ( !IsHostapdkilled(ap_pid,guest_ap2_pid,guest_ap3_pid) )
       {
         LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
         ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
       }
     }
     KillHostapd(0, QcMapMgr->ap_dev_num2);
     snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_SSID2_PID_FILE);
     ds_system_call(command, strlen(command));

    break ;

    default:
    break;

  }
  return true;
}


/*=====================================================================
FUNCTION StartHostAPD
======================================================================*/
/*!
@brief
Starts hostapd service with the appropriate configuration
files based on the wlan mode configured.

@parameters
int interface

@return
true  - on success
false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::StartHostAPD(int interface, int wlan_dev_index)
{
  char command[MAX_COMMAND_STR_LEN];
  char temp_command[MAX_COMMAND_STR_LEN];
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type;
  int retry = 0, ret = true;
  QCMAP_CM_LOG_FUNC_ENTRY();
  ap_type = QCMAP_MSGR_ACTIVATE_HOSTAPD_AP_ENUM_MIN_ENUM_VAL_V01;

  ds_system_call("echo QCMAP:Start Hostapd > /dev/kmsg",\
  strlen("echo QCMAP:Start Hostapd > /dev/kmsg"));

  snprintf(command, MAX_COMMAND_STR_LEN, "sh /etc/cld-hostapd-wlan%d.sh",
  wlan_dev_index);
  ds_system_call(command, strlen(command));

  /* Changes should not be needed here as AP-AP-STA will always be on dual wifi */
  if ( interface == QCMAP_MSGR_INTF_AP_INDEX &&
  this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_sta_mode_hostapd_conf,
    HOSTAPD_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);
    ap_type = QCMAP_MSGR_PRIMARY_AP_V01;
  }
  else if ( interface == QCMAP_MSGR_INTF_AP_INDEX )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_hostapd_conf,
    HOSTAPD_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);
    LOG_MSG_INFO1("AP MODE: Hostapdconf: %s entropy: %s interface: %d",
        this->wlan_cfg.interface[interface].path_to_hostapd_conf,
        this->wlan_cfg.interface[interface].path_to_hostapd_entropy,interface);

    ap_type = QCMAP_MSGR_PRIMARY_AP_V01;
  }

  //AP-AP-AP-AP mode Guest Config 2
  else if ( interface == QCMAP_MSGR_INTF_GUEST_AP_2_INDEX )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_hostapd_conf,
    HOSTAPD_SSID3_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);

    LOG_MSG_INFO1("Hostapdconf: %s entropy: %s interface: %d",
            this->wlan_cfg.interface[interface].path_to_hostapd_conf,
            this->wlan_cfg.interface[interface].path_to_hostapd_entropy,interface);
    ap_type = QCMAP_MSGR_GUEST_AP_2_V01;
  }

  //AP-AP-AP-AP mode Guest Config 3
  else if ( interface == QCMAP_MSGR_INTF_GUEST_AP_3_INDEX )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_hostapd_conf,
    HOSTAPD_SSID4_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);

    LOG_MSG_INFO1("Hostapdconf: %s entropy: %s interface: %d",
            this->wlan_cfg.interface[interface].path_to_hostapd_conf,
            this->wlan_cfg.interface[interface].path_to_hostapd_entropy,interface);
    ap_type = QCMAP_MSGR_GUEST_AP_3_V01;
  }

  //AP-AP mode or AP-AP-AP-AP mode Guest 1 config
  else if ( interface == QCMAP_MSGR_INTF_GUEST_AP_INDEX )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_hostapd_conf,
    HOSTAPD_SSID2_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);
    LOG_MSG_INFO1("AP Guest1 MODE: Hostapdconf: %s entropy: %s interface: %d",
        this->wlan_cfg.interface[interface].path_to_hostapd_conf,
        this->wlan_cfg.interface[interface].path_to_hostapd_entropy,interface);
    ap_type = QCMAP_MSGR_GUEST_AP_V01;
  }

  /* If debug mode is enabled add concat dbg cmd  */
  if (interface == QCMAP_MSGR_INTF_AP_INDEX && this->debug_config.hostapd_debug)
  {
    snprintf(temp_command,
    MAX_COMMAND_STR_LEN, "%s %s", command,
    this->debug_config.hostapd_dbg_cmd);
    memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
  }
  else if (interface == QCMAP_MSGR_INTF_GUEST_AP_INDEX
            && this->debug_config.hostapd_guest_debug)
  {
    snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command,
                    this->debug_config.hostapd_guest_dbg_cmd);
    memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
  }

  else if (interface == QCMAP_MSGR_INTF_GUEST_AP_2_INDEX
            && this->debug_config.hostapd_guest_2_debug)
  {
    snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command,
                    this->debug_config.hostapd_guest_2_dbg_cmd);
    memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
  }

  else if (interface == QCMAP_MSGR_INTF_GUEST_AP_3_INDEX
            && this->debug_config.hostapd_guest_3_debug)
  {
    snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command,
                    this->debug_config.hostapd_guest_3_dbg_cmd);
    memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
  }

  ds_system_call(command, strlen(command));


  if(IsHostapdRunning(interface))
  {
    /* Though hostapd application has started, the wlan driver is not yet
     * done with AP Start and beacon operation. Check to confirm
     */
    while (CheckAPStatus(wlan_dev_index) == 0 &&
             retry < QCMAP_HOSTAPD_START_POLL_MAX_COUNT) {
      retry++;
      usleep(QCMAP_HOSTAPD_POLL_DELAY_MS);
    }
  } else
    retry = QCMAP_HOSTAPD_START_POLL_MAX_COUNT;

  if (retry >= QCMAP_HOSTAPD_START_POLL_MAX_COUNT) {
    LOG_MSG_ERROR("hostapd start failed", 0, 0, 0);
    ret = false;
  }
  //Start Hostapd_cli on the interface
  this->StartHostapdCli(ap_type);

  snprintf(temp_command, MAX_COMMAND_STR_LEN,
            "echo \"QCMAP: Wlan%d Hostapd Start %s\" > /dev/kmsg",
            wlan_dev_index, (ret == true ? "Success" : "Fail"));
  ds_system_call(temp_command, strlen(temp_command));

  return ret;
}


/*=====================================================================
FUNCTION UpdateAccessProfileRules
======================================================================*/
/*!
@brief
Updates Ebtables rules for changes in access profile.

@parameters
int dev interface number

@return
returns succesful if prev and current sta config are different

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN::UpdateAccessProfileRules()
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (QcMapWLANMgr)
  {
    QcMapWLANMgr->DeleteGuestAPAccessRules();
  }

  if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
                                  QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
                                  QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
                                  QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01))
  {
    if (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile ==
        QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
    {
      QcMapWLANMgr->InstallGuestAPAccessRules(QcMapMgr->ap_dev_num2);
    }
    else
    {
      /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
     if (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
       QcMapWLANMgr->SetIPAWLANMode(QcMapMgr->ap_dev_num2, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
    }
  }

  if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
                                  QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01))
  {
    if (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile ==
        QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
    {
      QcMapWLANMgr->InstallGuestAPAccessRules(QcMapMgr->ap_dev_num3);
    }
    else
    {
      /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
     if (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
       QcMapWLANMgr->SetIPAWLANMode(QcMapMgr->ap_dev_num3, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
#endif /* FEATURE_DATA_TARGET_MDM9607 */
    }
  }

//Guest AP 3
  if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
  {
    if (QcMapWLANMgr && QcMapWLANMgr->wlan_cfg.\
        interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile ==
        QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
    {
      QcMapWLANMgr->InstallGuestAPAccessRules(QcMapMgr->ap_dev_num4);
    }
  }
}
/*=====================================================================
FUNCTION InstallGuestAPAccessRules
======================================================================*/
/*!
@brief
Install Ebtables/iptables rules for Guest AP access.

@parameters
int dev interface number

@return
none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN::InstallGuestAPAccessRules(int ap_dev_num)
{
  char command[MAX_COMMAND_STR_LEN];
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  char prefix_v6[MAX_IPV6_PREFIX + 1];
  struct in6_addr *prefix_ptr = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=GET_DEFAULT_BACKHAUL_CRADLE_OBJECT();
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                        GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();
  FILE *access_fd = NULL;
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qmi_error_type_v01 qmi_err_num;

  qcmap_cm_nl_prefix_info_t* ipv6_prefix_info_ptr=NULL;

  if(QcMapBackhaulMgr)
  {
    ipv6_prefix_info_ptr = &QcMapBackhaulMgr->ipv6_prefix_info;
    memset(prefix_v6, 0, MAX_IPV6_PREFIX + 1);
    prefix_ptr = &((struct sockaddr_in6 *)&(ipv6_prefix_info_ptr->prefix_addr))->sin6_addr;
    inet_ntop(AF_INET6, (struct in6_addr *)prefix_ptr->s6_addr, prefix_v6,
      MAX_IPV6_PREFIX );
  }

  if(QCMAPLANMgr)
  {
    if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
      return;
    }
  } else {
      LOG_MSG_ERROR("Default LAN object is NULL", 0, 0, 0);
      return;
  }
  addr.s_addr = htonl(lan_cfg->apps_ip_addr);
  strlcpy(a5_ip, inet_ntoa(addr), 16);
  addr.s_addr = htonl(lan_cfg->sub_net_mask);
  strlcpy(netmask, inet_ntoa(addr), 16);
  access_fd = fopen(QCMAP_MULTIPLE_SYSTEM_CALL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("Cannot open %s file for writing",QCMAP_MULTIPLE_SYSTEM_CALL_FILE,0,0);
    return;
  }
  fprintf( access_fd,"#!/bin/sh \n");

  /* ----------------------------------------------------- */
  /* ------------------ IPv4 Rules ----------------------- */
  /* ----------------------------------------------------- */

  /* Rules to allow DNS/DHCP and block the rest of the traffic from Guest AP Client
  (interface via which frame is received)  to Apps local subnet*/
  fprintf( access_fd,"ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto udp\
    --ip-dport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto tcp\
    --ip-dport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto udp\
    --ip-dport 67 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv4 --ip-destination\
    %s/%s -j DROP \n",ap_dev_num,a5_ip,netmask);
  fprintf( access_fd,"ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto tcp\
    --ip-dport 8888 -j ACCEPT \n",ap_dev_num);

  /* Rules to allow DNS/DHCP and block the rest of the traffic from A5/local subnet
   to GuestAPClient(interface via which a frame is going to be sent )*/
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv4 --ip-proto udp\
    --ip-sport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv4 --ip-proto tcp\
    --ip-sport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv4 --ip-proto udp\
    --ip-sport 67 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv4 --ip-source\
    %s/%s -j DROP \n",ap_dev_num,a5_ip,netmask);

  /* Rules to block traffic to pubilc ip on A5 */
  if(QcMapBackhaulMgr && ( (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_WAN_CONNECTED) ||
      QcMapBackhaulCradleMgr|| QcMapBackhaulWLANMgr || QcMapBackhaulEthMgr))
  {
      addr.s_addr = QcMapBackhaulMgr->ipv4_public_ip;
      strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
      fprintf( access_fd, "ebtables -D OUTPUT -o wlan%d -p IPv4 --ip-source %s\
      -j DROP \n",ap_dev_num, publicIpAddr);
      fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv4 --ip-source %s\
        -j DROP \n",ap_dev_num, publicIpAddr);
      fprintf( access_fd, "ebtables -D INPUT -i wlan%d -p IPv4 --ip-destination\
        %s -j DROP \n",ap_dev_num, publicIpAddr);
      fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv4 --ip-destination\
        %s -j DROP \n",ap_dev_num, publicIpAddr);
  }
  else
  {
    /* This will make sure to drop all data, since backhaul in not connected.
       It will make sure to drop packet during embedded call also. */
    fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv4 -j DROP \n",\
             ap_dev_num);
    fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv4 -j DROP \n",\
             ap_dev_num);
  }

  /* ----------------------------------------------------- */
  /* ------------------ IPv6 Rules ----------------------- */
  /* ----------------------------------------------------- */
  /* Rule to allow ICMP/DNS messages and block rest of the traffic to A5 or local LAN*/
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv6 --ip6-proto ipv6-icmp\
    --ip6-icmp-type 133:136/0 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv6 --ip6-proto udp\
    --ip6-dport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv6 --ip6-proto tcp \
  --ip6-dport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv6 --ip6-destination \
    fe80::/64 -j DROP \n",ap_dev_num);

    // If a global IPv6 prefix is available, also add prefix based rules.
  if (QcMapBackhaulMgr && QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid )
  {
    fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv6 --ip6-destination\
      %s/%d -j DROP \n",ap_dev_num,prefix_v6,ipv6_prefix_info_ptr->prefix_len);
    // Set the flag to indicate ipv6 prefix based rules are added
    if(QcMapBackhaulWWANMgr)
    {
      if (ap_dev_num == QcMapMgr->ap_dev_num2)
      {
        QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added_for_guest_ap_1 = true;
      }
      else if (ap_dev_num == QcMapMgr->ap_dev_num3)
      {
        QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added_for_guest_ap_2 = true;
      }
       else if (ap_dev_num == QcMapMgr->ap_dev_num4)
      {
        QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added_for_guest_ap_3 = true;
      }
    }
  }

  /* Rule to Drop traffic routed from local subnet to guest AP. Allow DNS/ICMP responses*/
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv6 --ip6-proto ipv6-icmp\
    --ip6-icmp-type 133:136/0 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv6 --ip6-proto udp --ip6\
    -sport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv6 --ip6-proto tcp --ip6\
    -sport 53 -j ACCEPT \n",ap_dev_num);
  fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv6 --ip6-source \
    fe80::/64 -j DROP \n",ap_dev_num);

  // If a global IPv6 prefix is available, also add prefix based rules.
  if (QcMapBackhaulMgr && QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid)
  {
    fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv6 --ip6-source %s/%d\
    -j DROP \n",ap_dev_num,prefix_v6,ipv6_prefix_info_ptr->prefix_len);
  }

  /* Add Drop rules if there is no backhaul avaulable/connected */
  if ( QcMapBackhaulWWANMgr && (QcMapBackhaulWWANMgr->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED) &&
       !QcMapBackhaulCradleMgr && !QcMapBackhaulWLANMgr && !QcMapBackhaulEthMgr)
  {
      fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv6 -j DROP \n",\
               ap_dev_num);
      fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv6 -j DROP \n",\
               ap_dev_num);
  }


  /* Rules to drop briged traffic to/from LAN from/to guest AP*/
  fprintf( access_fd, "ebtables -A FORWARD -o wlan%d -j DROP \n",ap_dev_num);
  fprintf( access_fd, "ebtables -A FORWARD -i wlan%d -j DROP \n",ap_dev_num);

  fclose(access_fd);

  /* Changing permissions */
  snprintf( command, MAX_COMMAND_STR_LEN,
  "chmod 755 %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,
  "rm -f %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));

  /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
  /* Update IPACM_cfg*/
  this->SetIPAWLANMode(ap_dev_num, QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01);
#endif /* FEATURE_DATA_TARGET_MDM9607 */

}


/*=====================================================================
FUNCTION IsHostapdkilled
======================================================================*/
/*!
@brief
Waits 1 second for the process to die.

@parameters
char *process

@return
true  - process killed
flase - process running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::IsHostapdkilled(int omit_pid1, int omit_pid2, int omit_pid3)
{
  char process[MAX_COMMAND_STR_LEN];
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  if ( omit_pid1== 0 && omit_pid2 == 0 && omit_pid3 == 0)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof hostapd");
  }
  else  if ( omit_pid1== 0 && omit_pid2 == 0 && omit_pid3 == 1)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d hostapd",omit_pid3);
  }
  else  if ( omit_pid1== 0 && omit_pid2 == 1 && omit_pid3 == 0)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d hostapd",omit_pid2);
  }
  else  if ( omit_pid1== 1 && omit_pid2 == 0 && omit_pid3 == 0)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d hostapd", omit_pid1);
  }
  else  if ( omit_pid1== 0 && omit_pid2 == 1 && omit_pid3 == 1)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d -o %d hostapd",
               omit_pid2,omit_pid3);
  }
  else  if ( omit_pid1== 1 && omit_pid2 == 0 && omit_pid3 == 1)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d -o %d hostapd",
               omit_pid1,omit_pid3);
  }
  else  if ( omit_pid1== 1 && omit_pid2 == 1 && omit_pid3 == 0)
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d -o %d hostapd",
               omit_pid1,omit_pid2);
  }
  else
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d -o %d -o %d hostapd",
               omit_pid1,omit_pid2,omit_pid3);
  }

  while (i++ < QCMAP_PROCESS_KILL_RETRY)
  {
    usleep(QCMAP_PROCESS_KILL_WAIT_MS);/*50 mini second*/
    cmd = popen(process, "r");
    pid = 0;
    memset(pid_s, 0, MAX_CMD_SIZE);
    if(cmd)
    {
      fgets(pid_s, MAX_CMD_SIZE, cmd);
      pclose(cmd);
    }
    pid = atoi(pid_s);
    /* If pid is zero we break from while*/
    if(pid == 0)
    {
      return true;
    }
  }

  LOG_MSG_ERROR("PID still running after waiting 2 second", 0, 0, 0);
  return false;
}


/*=====================================================================
  FUNCTION DeleteGuestAPAccessRules
======================================================================*/
/*!
@brief
  Delete Ebtables/iptables rules for Guest AP access.

@parameters
   int dev interface number

@return
   none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN::DeleteGuestAPAccessRules()
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /* Flush complete Ebtables  */
  snprintf( command, MAX_COMMAND_STR_LEN,
             "ebtables -t filter --flush");
  ds_system_call( command, strlen(command));

  if(QcMapBackhaulWWANMgr)
  {
    QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added_for_guest_ap_1 = false;
    QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added_for_guest_ap_2 = false;
    QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added_for_guest_ap_3 = false;
  }

  /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
  /* Update IPACM_cfg*/
  if (QcMapMgr->dual_wifi_mode)
  {
    if (QCMAP_LAN_INVALID_IFACE_INDEX != QcMapMgr->unused_wlan_if)
    {
      this->SetIPAWLANMode(QcMapMgr->unused_wlan_if, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
    }
    this->SetIPAWLANMode(QcMapMgr->ap_dev_num1, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
  }

  if (this->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
      this->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
  {
    this->SetIPAWLANMode(QcMapMgr->ap_dev_num2, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
  }

  if (this->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 )
  {
    this->SetIPAWLANMode(QcMapMgr->ap_dev_num3, QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
  }
#endif

}

/*=====================================================================
  FUNCTION CheckHostapd
======================================================================*/
/*!
@brief
  Return true if the hostapd is running.

@parameters
  char *process

@return
  true  - process killed
  flase - process running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean CheckHostapd(int omit_pid)
{
  char process[MAX_COMMAND_STR_LEN];
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  if ( omit_pid == 0 )
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof hostapd");
  }
  else
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d hostapd", omit_pid);
  }
  cmd = popen(process, "r");
  pid = 0;
  memset(pid_s, 0, MAX_CMD_SIZE);
  if(cmd)
  {
    fgets(pid_s, MAX_CMD_SIZE, cmd);
    pid = atoi(pid_s);
    LOG_MSG_INFO1("pid =%d", pid, 0, 0);
    pclose(cmd);
    /* If pid is zero we break from while*/
    if(pid != 0)
    {
      return true;
    }
  }
  LOG_MSG_ERROR("hostapd not running!!", 0, 0, 0);
  return false;
}

/*=====================================================================
  FUNCTION IsSupplicantRunning
======================================================================*/
/*!
@brief
  Return true if the wpa_supplicant is running.

@parameters
  char *process

@return
  true  - process killed
  flase - process running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::IsSupplicantRunning(void)
{
  char process[MAX_COMMAND_STR_LEN];
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  snprintf(process, MAX_COMMAND_STR_LEN, "pidof wpa_supplicant");
  cmd = popen(process, "r");
  pid = 0;
  memset(pid_s, 0, MAX_CMD_SIZE);
  if(cmd)
  {
    fgets(pid_s, MAX_CMD_SIZE, cmd);
    pid = atoi(pid_s);
    LOG_MSG_INFO1("pid =%d", pid, 0, 0);
    pclose(cmd);
    /* If pid is zero then process is not running*/
    if(pid != 0)
    {
      return true;
    }
  }

  LOG_MSG_ERROR("wpa_supplicant not running!!",0,0,0);
  return false;
}

/*=====================================================================
  FUNCTION IsHostapdRunning
======================================================================*/
/*!
@brief
  Checks if the hostapd is running.

@parameters
  void

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::IsHostapdRunning(int iface)
{
  FILE *fp = NULL;
  int ap_pid = 0;

  switch ( iface )
  {
    case QCMAP_MSGR_INTF_AP_INDEX:
     {
        LOG_MSG_INFO1("Primary AP mode %d",iface,0,0);
        /* Only instance of hostapd will be running for the first time */
        if ( CheckHostapd(0) )
        {
          LOG_MSG_INFO1("Hostapd is Running", 0, 0, 0);
          return true;
        }
        else
        {
          return false;
        }
     }
     break;
    case QCMAP_MSGR_INTF_GUEST_AP_INDEX:
     {
        LOG_MSG_INFO1("Guest AP mode %d",iface,0,0);
        /* Get the Primary AP PID. */
        fp = fopen(HOSTAPD_PID_FILE, "r");

        if ( fp == NULL )
        {
          LOG_MSG_ERROR("Error opening hostapd pid file: %d.", errno, 0, 0);
        }
        else if (fscanf(fp, "%d", &ap_pid) != 1)
        {
          LOG_MSG_ERROR("Error reading hostapd file: %d.", errno, 0, 0);
          ap_pid = 0;
          fclose(fp);
        }
        else
        {
          LOG_MSG_INFO1("Primary AP PID = %d",ap_pid,0,0);
          fclose(fp);
        }

        if ( ap_pid == 0 )
        {
          LOG_MSG_ERROR("Hostapd for AP is not yet started", 0, 0, 0);
        }

        if ( CheckHostapd(ap_pid) )
        {
          LOG_MSG_INFO1("Guest AP mode Hostapd is Running", 0, 0, 0);
          return true;
        }
        else
        {
         return false;
        }
     }
     break;
    default:
          LOG_MSG_ERROR("No Hostapd of this mode %d", iface, 0, 0);
     break;
  }
  return true;
}

/*=====================================================================
  FUNCTION GetsetWLANConfigFromXML
======================================================================*/
/*!
@brief
  Get / sets the wlan config from XML.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================================*/

boolean QCMAP_WLAN::GetsetWLANConfigFromXML
(
  qcmap_wlan_config_enum conf,
  qcmap_action_type action,
  uint32 *value
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  switch (conf)
  {
    case CONFIG_WLAN_MODE:
      if (action == GET_VALUE)
      {
        if (strncasecmp(AP_STA_TAG, root.child(WlanMode_TAG).child_value(),\
            strlen(AP_STA_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_STA_V01;
        else if (strncasecmp(AP_AP_STA_TAG, root.child(WlanMode_TAG).child_value(),\
                   strlen(AP_AP_STA_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01;
        else if (strncasecmp(AP_AP_AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
                   strlen(AP_AP_AP_AP_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01;
        else if (strncasecmp(AP_AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
          strlen(AP_AP_AP_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01;
        else if (strncasecmp(AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
          strlen(AP_AP_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_AP_V01;
        else if (strncasecmp(STA_P2P_TAG, root.child(WlanMode_TAG).child_value(),\
            strlen(STA_P2P_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_STA_P2P_V01;
        else if (strncasecmp(STA_ONLY_TAG, root.child(WlanMode_TAG).child_value(),\
          strlen(STA_ONLY_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01;
        else
          /* Default Value. */
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_V01;
      }
      else
      {
        if (*value  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01)
          root.child(WlanMode_TAG).text() = AP_AP_AP_AP_TAG;
        else if (*value  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
          root.child(WlanMode_TAG).text() = AP_AP_TAG;
        else if (*value  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01)
          root.child(WlanMode_TAG).text() = AP_AP_AP_TAG;
        else if (*value  == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
          root.child(WlanMode_TAG).text() = AP_STA_TAG;
        else if (*value  == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
          root.child(WlanMode_TAG).text() = AP_AP_STA_TAG;
        else if (*value  == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01)
          root.child(WlanMode_TAG).text() = STA_ONLY_TAG;
        else if(*value  == QCMAP_MSGR_WLAN_MODE_STA_P2P_V01)
          root.child(WlanMode_TAG).text() = STA_P2P_TAG;
        else
          root.child(WlanMode_TAG).text() = AP_TAG;
      }
    break;

    case CONFIG_GUEST_ACCESS_PROFILE:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
        child(MobileAPLanCfg_TAG).child(GuestAPCfg_TAG);
      if (action == GET_VALUE)
      {
        if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),\
              MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).\
              child_value()))) == 0)
        {
          *value = (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
        }
        else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
                 child_value(), MIN(sizeof(INTERNET_TAG) - 1,\
                 strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
        {
          *value = (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
        }
      }
      else
      {
        if(*value == (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)
        {
          root.child(AccessProfile_TAG).text() = FULL_TAG;
        }
        else if(*value == (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
        {
          root.child(AccessProfile_TAG).text() = INTERNET_TAG;
        }
      }
    break;

  case CONFIG_GUEST_2_ACCESS_PROFILE:
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
      child(MobileAPLanCfg_TAG).child(GuestAP2Cfg_TAG);
    if (action == GET_VALUE)
    {
      if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),\
          MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).\
          child_value()))) == 0)
      {
        *value = (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
      }
      else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
               child_value(), MIN(sizeof(INTERNET_TAG) - 1,\
               strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
      {
        *value = (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
      }
    }
    else
    {
      if(*value == (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)
      {
        root.child(AccessProfile_TAG).text() = FULL_TAG;
      }
      else if(*value == (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        root.child(AccessProfile_TAG).text() = INTERNET_TAG;
      }
    }
    break;

    case CONFIG_GUEST_3_ACCESS_PROFILE:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
      child(MobileAPLanCfg_TAG).child(GuestAP3Cfg_TAG);
      if (action == GET_VALUE)
      {
        if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),\
             MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).\
              child_value()))) == 0)
        {
          *value = (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
        }
      else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
                 child_value(), MIN(sizeof(INTERNET_TAG) - 1,\
                 strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
        {
          *value = (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
        }
      }
      else
      {
        if(*value == (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)
       {
          root.child(AccessProfile_TAG).text() = FULL_TAG;
        }
        else if(*value == (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
        {
          root.child(AccessProfile_TAG).text() = INTERNET_TAG;
        }
      }
    break;
  }

  if (action == SET_VALUE)
     QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  return TRUE;
}

/*===========================================================================
  FUNCTION ReadWLANConfigFromXML
==========================================================================*/
/*!
@brief
  Reads WLAN Config from XML.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::ReadWLANConfigFromXML()
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  pugi::xml_document xml_file;
  pugi::xml_node  root, child;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  if (strncasecmp(AP_STA_TAG, root.child(WlanMode_TAG).child_value(),\
        strlen(AP_STA_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_STA_V01;
  }
  else if (strncasecmp(AP_AP_STA_TAG, root.child(WlanMode_TAG).child_value(),\
        strlen(AP_AP_STA_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01;
  }
  else if (strncasecmp(AP_AP_AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
             strlen(AP_AP_AP_AP_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01;
  }
  else if (strncasecmp(AP_AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
           strlen(AP_AP_AP_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01;
  }
  else if (strncasecmp(AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
           strlen(AP_AP_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_AP_V01;
  }
#ifdef FEATURE_MOBILEAP_APQ_PLATFORM
  else if (strncasecmp(STA_P2P_TAG, root.child(WlanMode_TAG).child_value(),\
           strlen(STA_P2P_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_STA_P2P_V01;
  }
  else if (strncasecmp(STA_ONLY_TAG, root.child(WlanMode_TAG).child_value(),\
           strlen(STA_ONLY_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01;
  }
#endif
  else
  {
    /* Default Value. */
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_V01;
  }

  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
          .path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
          .path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG).child_value(),\
           QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
          .path_to_sta_mode_hostapd_conf, root.child(STAModeHostAPDCfg_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1("READ: Hostapdconf: %s entropy: %s interface: %d",\
                this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].path_to_hostapd_conf,\
                this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
                .path_to_hostapd_entropy, QCMAP_MSGR_INTF_AP_INDEX);

//GUEST AP1
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAPCfg_TAG);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]//
          .path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]\
          .path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]\
                 .path_to_hostapd_conf,
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
                 .path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_INDEX);

  if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),
        MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).
        child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile =\
                                             QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
  }
  else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
            child_value(), MIN(sizeof(INTERNET_TAG) - 1,
            strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile =\
                                           QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  }
    LOG_MSG_INFO1("WLAN Mode: %d Guest AP 1 Access profile: %d" , this->wlan_cfg.wlan_mode,\
      this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile, 0);

  //GUEST AP2
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAP2Cfg_TAG);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX]\
            .path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
            QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX]\
            .path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
            .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX]\
                 .path_to_hostapd_conf,
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX]
                 .path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_2_INDEX);

  if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),
        MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).
        child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile =\
                                             QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
  }
  else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
            child_value(), MIN(sizeof(INTERNET_TAG) - 1,
            strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile =\
                                           QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  }

  LOG_MSG_INFO1("WLAN Mode: %d Access profile: %d" , this->wlan_cfg.wlan_mode,\
                this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile, 0);

  //GUEST AP3
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAP3Cfg_TAG);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX]\
          .path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX]\
          .path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX]\
                 .path_to_hostapd_conf,
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX]
                 .path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_3_INDEX);

  if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),
        MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).
        child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile =\
                                             QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
  }
  else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
            child_value(), MIN(sizeof(INTERNET_TAG) - 1,
            strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile =\
                                           QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  }
    LOG_MSG_INFO1("WLAN Mode: %d Guest AP 3 Access profile: %d" , this->wlan_cfg.wlan_mode,\
      this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile, 0);

  root= xml_file.child(System_TAG).child(Debug_TAG);
  if(root)
  {
    if (root.child(SUPPLICANT_DEBUG_TAG).child_value())
    {
      this->debug_config.supplicant_debug = 1;
      strlcpy(this->debug_config.supplicant_dbg_cmd,\
      root.child(SUPPLICANT_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_DEBUG_TAG).child_value())
    {
      this->debug_config.hostapd_debug= 1;
      strlcpy(this->debug_config.hostapd_dbg_cmd,\
      root.child(HOSTAPD_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value())
    {
      this->debug_config.hostapd_guest_debug= 1;
      strlcpy(this->debug_config.hostapd_guest_dbg_cmd,\
      root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_2_DEBUG_TAG).child_value())
    {
      this->debug_config.hostapd_guest_2_debug= 1;
      strlcpy(this->debug_config.hostapd_guest_2_dbg_cmd,\
      root.child(HOSTAPD_GUEST_2_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_3_DEBUG_TAG).child_value())
    {
      this->debug_config.hostapd_guest_3_debug= 1;
      strlcpy(this->debug_config.hostapd_guest_3_dbg_cmd,\
      root.child(HOSTAPD_GUEST_3_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
#ifdef FEATURE_MOBILEAP_APQ_PLATFORM
    if (root.child(P2P_SUPPLICANT_DEBUG_TAG).child_value())
    {
      this->debug_config.p2p_supplicant_debug = 1;
      strlcpy(this->debug_config.p2p_supplicant_dbg_cmd,\
      root.child(P2P_SUPPLICANT_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
#endif
  }
  return true;
}

/*===========================================================================
  FUNCTION ReadWLANConfigPathsFromXML
==========================================================================*/
/*!
@brief
  Reads WLAN Config from XML.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::ReadWLANConfigPathsFromXML(qcmap_cm_wlan_paths_conf_t *wlan_paths)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  pugi::xml_document xml_file;
  pugi::xml_node  root, child;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  /* Copy Primary AP Path */
  strlcpy(wlan_paths->hostapd.path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd.path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG).child_value(),\
           QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd.path_to_sta_mode_hostapd_conf, root.child(STAModeHostAPDCfg_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1("READ: Hostapdconf: %s entropy: %s interface: %d",\
                wlan_paths->hostapd.path_to_hostapd_conf,\
                wlan_paths->hostapd.path_to_hostapd_entropy, QCMAP_MSGR_INTF_AP_INDEX);

  /* Copy Guest AP Path */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAPCfg_TAG);
  strlcpy(wlan_paths->hostapd_guest.path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd_guest.path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 wlan_paths->hostapd_guest.path_to_hostapd_conf,
                 wlan_paths->hostapd_guest.path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_INDEX);


  /* Copy Guest AP 2 Path */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAP2Cfg_TAG);
  strlcpy(wlan_paths->hostapd_guest_2.path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd_guest_2.path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 wlan_paths->hostapd_guest_2.path_to_hostapd_conf,
                 wlan_paths->hostapd_guest_2.path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_2_INDEX);

  /* Copy Guest AP 3 Path */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAP3Cfg_TAG);
  strlcpy(wlan_paths->hostapd_guest_3.path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd_guest_3.path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 wlan_paths->hostapd_guest_3.path_to_hostapd_conf,
                 wlan_paths->hostapd_guest_3.path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_2_INDEX);

  /* Copy Supplicant Path */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
  strlcpy( wlan_paths->supplicant,
           root.child(StationModeCfg_TAG).child(SupplicantCfg_TAG).child_value(),
           QCMAP_CM_MAX_FILE_LEN );

  root= xml_file.child(System_TAG).child(Debug_TAG);
  if(root)
  {
    if (root.child(SUPPLICANT_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.supplicant_debug = true;
      strlcpy(wlan_paths->debug.supplicant_dbg_cmd,\
      root.child(SUPPLICANT_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.hostapd_debug = true;
      strlcpy(wlan_paths->debug.hostapd_dbg_cmd,\
      root.child(HOSTAPD_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.hostapd_guest_debug = true;
      strlcpy(wlan_paths->debug.hostapd_guest_dbg_cmd,\
      root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_2_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.hostapd_guest_2_debug = true;
      strlcpy(wlan_paths->debug.hostapd_guest_2_dbg_cmd,\
      root.child(HOSTAPD_GUEST_2_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_3_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.hostapd_guest_3_debug= 1;
      strlcpy(wlan_paths->debug.hostapd_guest_3_dbg_cmd,\
      root.child(HOSTAPD_GUEST_3_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
  }

  return true;
}

  /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607

/*=====================================================
  FUNCTION SetIPAWLANMode
======================================================*/
/*!
@brief
  This function sets the WLAN mode in IPACM XML

@parameters
  int mode, int dev interface number

@return
  true  - on success
  flase - on failure


- Side Effects
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_WLAN::SetIPAWLANMode(int ap_dev_num, int mode)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char guest_iface[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  LOG_MSG_INFO1("Updating IPACM rules : wlan%d, mode=%d", ap_dev_num, mode,0);

  snprintf(guest_iface, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2, "wlan%d", ap_dev_num);
  if (!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPACMIface_TAG);
  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strncasecmp(child.child(Name_TAG).child_value(),
                     guest_iface, strlen(guest_iface)))
    {
      if (mode == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        // if new mode is same as last saved mode, dont need to change.
        if (strncasecmp(WLAN_Internet_TAG, child.child(WLANMode_TAG).child_value(),\
                        strlen(WLAN_Internet_TAG)) == 0)
          return true;

        child.child(WLANMode_TAG).text() = WLAN_Internet_TAG;
      }
      else
      {
        // if new mode is same as last saved mode, dont need to change.
        if (strncasecmp(WLAN_Full_TAG, child.child(WLANMode_TAG).child_value(),\
                        strlen(WLAN_Full_TAG)) == 0)
          return true;

        child.child(WLANMode_TAG).text() = WLAN_Full_TAG;
      }
      QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
      return true;
    }
  }
  return false;
}

#endif /* FEATURE_DATA_TARGET_MDM9607 */

/*===========================================================================
 FUNCTION EnableSTAOnlyMode
==========================================================================*/
/*!
@brief
  Brings up WLAN in STA only Mode. If WLAN is enabled it returns error.

@parameters
  void

@return
  true  - on Success
  false - on Failure

@note
  This API is only going to be used by eCNE-QCMAP client.
  Other QCMAP Clients should not use this API.

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_WLAN::EnableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  uint32_t time_out = (WIFI_DEV_INIT_DELAYS_MAX/WIFI_DEV_INIT_DELAY);
  int i = 0;

  LOG_MSG_INFO1("Entering EnableSTAOnlyMode",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if ( !QcMapMgr->qcmap_enable )
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (QcMapWLANMgr)
  {
    LOG_MSG_ERROR("WLAN is already enabled. Current Mode:- %d",QcMapWLANMgr->wifi_mode ,0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  else
  {
    QcMapWLANMgr = QCMAP_WLAN::Get_Instance(true);//Creating WLAN object here
    if (!QcMapWLANMgr)
    {
      LOG_MSG_ERROR("Could not create WLAN obj",0,0,0);
      *qmi_err_num = QMI_ERR_NO_MEMORY_V01;
      return false;
    }
  }

  /* Enable WLAN */
  LOG_MSG_INFO1("Bringup WLAN",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan start");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "/proc/sys/net/ipv4/conf/wlan0");
  while ( i++ < time_out )
  {
    std::ifstream ifile( command );
    if ( ifile )
    {
      break;
    }
    usleep( WIFI_DEV_INIT_DELAY );
  }

  LOG_MSG_INFO1("Wlan card brought up", 0,0,0);


  snprintf(command, MAX_COMMAND_STR_LEN,
              "echo \"ctrl_interface=\/var\/run\/wpa_supplicant\" > %s",
               STA_ONLY_MODE_SUPPLICANT_FILE);
  ds_system_call( command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "wpa_supplicant -i wlan0 -Dnl80211 -c %s -B",
               STA_ONLY_MODE_SUPPLICANT_FILE); //should be changed to STA_ONLY_FILE or internal
  ds_system_call( command, strlen(command));

  if (QcMapWLANMgr->IsSupplicantRunning() == false)
  {
    LOG_MSG_ERROR("Could not start wpa_supplicant",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  /* Ensure WiFi is not enabled for other clients */
  QcMapWLANMgr->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  /* Set STA Mode only variable */
  QcMapWLANMgr->is_sta_only_mode_enabled = true;

  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*===========================================================================
 FUNCTION DisableSTAOnlyMode
==========================================================================*/
/*!
@brief
  Brings down WLAN in STA only Mode.
  If WLAN is enabled in any other mode like AP,AP+AP or AP+STA - returns error.

@parameters
  void

@return
  true  - on Success
  false - on Failure

@note
  This API is only going to be used by eCNE-QCMAP client.
  Other QCMAP Clients should not use this API.

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_WLAN::DisableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];

  LOG_MSG_INFO1("Entering DisableSTAOnlyMode",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if ( !QcMapMgr->qcmap_enable )
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (QcMapWLANMgr && (QcMapWLANMgr->is_sta_only_mode_enabled != true))
  {
    LOG_MSG_ERROR("WLAN is not enabled in STAOnly Mode.",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  else if (!QcMapWLANMgr)
  {
    LOG_MSG_ERROR("WLAN is already disabled", 0,0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* Kill wpa_supplicant gracefully*/
  snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
  ds_system_call(command, strlen(command));

  if (!QCMAP_Backhaul_WLAN::IsWpaSupplicantkilled())
  {
    /* Kill wpa_supplicant forcefully. */
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
    ds_system_call(command, strlen(command));
  }

  /* Remove Temp Wpa_suplicant File */
  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf %s", STA_ONLY_MODE_SUPPLICANT_FILE);
  ds_system_call(command, strlen(command));

  /* Disable WLAN */
  LOG_MSG_INFO1("Disable WLAN in STA Only Mode",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
  ds_system_call(command, strlen(command));

  /* Set STA Mode only variable */
  QcMapWLANMgr->is_sta_only_mode_enabled = false;

  LOG_MSG_INFO1("Wlan card brought down",0,0,0);

  /* Ensure WiFi is not enabled for other clients */
  QcMapWLANMgr->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;

  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*===========================================================================
 FUNCTION UpdateWLANStatuswithSCMind
==========================================================================*/
/*!
@brief
  Updtaes WLAN status and send ind to client about SCM changes

@parameters
  void

@return
  void

@note

- Dependencies
- None

- Side Effects
==========================================================================*/
void QCMAP_WLAN::UpdateWLANStatuswithSCMind(qcmap_msgr_scm_ind_enum_v01 ind)
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  FillinWLANStatusIND(ind);
  if (QcMapWLANMgr)
  {
    QcMapWLANMgr->SendWLANStatusIND();
  }
  else
  {
    LOG_MSG_ERROR("null obj",0,0,0);
  }
}

/*===========================================================================
 FUNCTION SendWLANStatusIND
==========================================================================*/
/*!
@brief
  Send WLAN Status IND to registered clients

@parameters
  void

@return
  void

@note

- Dependencies
- None

- Side Effects
==========================================================================*/
void QCMAP_WLAN::SendWLANStatusIND(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_msgr_wlan_status_ind_msg_v01 wlan_ind_msg;
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;

  if (!QcMapWLANMgr || !QcMapMgr)
  {
    LOG_MSG_ERROR("Invalid Objects!!! WLAN %p, QCMAP %p",QcMapWLANMgr, QcMapMgr, 0);
    return;
  }

  memset(&wlan_ind_msg, 0, sizeof(wlan_ind_msg));
  memcpy(&wlan_ind_msg, &QcMapWLANMgr->current_wlan_status, sizeof(wlan_ind_msg));

  cbPtr = (qmi_qcmap_msgr_status_cb_data *)QcMapMgr->GetQCMAPClientCBDatata();
  /* Send WLAN Status IND to client */
  if (cbPtr && cbPtr->wlan_status_cb)
    cbPtr->wlan_status_cb(QcMapMgr->GetQCMAPClientCBDatata(), &wlan_ind_msg);
}

/*===========================================================================
 FUNCTION AddDeviceEntryForMac
==========================================================================*/
/*!
@brief
  Add the device entry for given MAC address
  And device Type

@parameters
  char *mac_str
  qcmap_msgr_device_type_enum_v01 device_type

@return
  void


- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

void QCMAP_WLAN::AddDeviceEntryForMac
(
  char *mac_str,
  qcmap_msgr_device_type_enum_v01 device_type
)
{
  qcmap_nl_sock_msg_t qcmap_nl_wlan_buffer;
  uint8_t             mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];

  memset(&qcmap_nl_wlan_buffer, 0, sizeof(qcmap_nl_wlan_buffer));
  memset(mac_addr, 0, QCMAP_MSGR_MAC_ADDR_LEN_V01);
  ds_mac_addr_pton(mac_str, mac_addr);
  memcpy(qcmap_nl_wlan_buffer.nl_addr.mac_addr, mac_addr, QCMAP_MSGR_MAC_ADDR_LEN_V01);
  if(!(QCMAP_ConnectionManager::MatchMacAddrInList(&qcmap_nl_wlan_buffer)))
  {
    LOG_MSG_INFO1("No match found for the WLAN MAC,"
                  "so add a linked list node device type %d",device_type, 0, 0);

    if (!QCMAP_ConnectionManager::AddNewDeviceEntry( (void*)mac_addr,device_type,NULL, NULL))
    {
      LOG_MSG_ERROR("Error in adding a new device entry ", 0, 0, 0);
    }
    else
    {
      Getclientaddr(&qcmap_nl_wlan_buffer.nl_addr, qcmap_nl_wlan_buffer.vlan_id);
      LOG_MSG_INFO1("WLAN Client conected",0, 0, 0);
      if(!(QCMAP_ConnectionManager::MatchMacAddrInList(&qcmap_nl_wlan_buffer)))
      {
         LOG_MSG_ERROR("No match found for the WLAN MAC -"
                       "Recived a NEWNEIGH Event before AP-STA-CONNECTED",0, 0, 0);
      }
    }
  }
  else
  {
    LOG_MSG_INFO1("Update device entry info if needed", 0, 0, 0);
                  QCMAP_ConnectionManager::UpdateDeviceEntryInfo(&qcmap_nl_wlan_buffer,
                                                                device_type);
  }
}

/*===========================================================================
 FUNCTION HostapdCliCmdParseMac
==========================================================================*/
/*!
@brief
  Parse the MAC address from the info got using hostapd_cli
  command

@parameters
  int dev
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
  true  - on Success
  false - on Failure

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::HostapdCliCmdParseMac
(
  int dev,
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
)
{
  char command[MAX_COMMAND_STR_LEN] = {0};
  int retry;
  int ret;
  FILE *fp;
  char res[MAX_COMMAND_STR_LEN] = {0};
  qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
  bool got_sta_from_hostapd = false;

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
    }
    break;

    case QCMAP_MSGR_GUEST_AP_V01:
    {
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;

    }
    break;

    case QCMAP_MSGR_GUEST_AP_2_V01:
    {
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;

    }
    break;

    case QCMAP_MSGR_GUEST_AP_3_V01:
    {
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01;
    }
    break;

    default:
    {
      LOG_MSG_ERROR("Invalid ap type %d return ",ap_type, 0, 0);
      return false;
    }
    break;
  }

  //Get Conected client mac address from hostapd
  //below command parse the MAC adress from the info.
  // "sort -u" is used to remove the duplicate entry
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "hostapd_cli -i wlan%d all_sta | grep -o -E \'([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}\' | sort -u",
           dev);

    /* Open the command for reading. */
  fp = popen(command, "r");
  if (fp == NULL)
  {
    LOG_MSG_ERROR("Cannot execute command : %s", command , 0, 0);
    return false;
  }
  LOG_MSG_INFO1("Executed command : %s", command, 0, 0);

  /* Read the output a line at a time - output it. */
  while (fgets(res, sizeof(res)-1, fp) != NULL)
  {
    got_sta_from_hostapd = true;
    LOG_MSG_INFO1("Got Mac address %s . Add device entry for Mac %s ap type %d", res, res, ap_type);
    QCMAP_WLAN::AddDeviceEntryForMac(res,device_type);
  }
  if (!got_sta_from_hostapd)
  {
    LOG_MSG_ERROR("Haven't Got Any client info from hostapd_cli for ap %d", ap_type , 0, 0);
  }
  pclose(fp);
  return true;
}

/*===========================================================================
 FUNCTION ParseMacAndAddDeviceEntry
==========================================================================*/
/*!
@brief
  Parse the MAC address from the info get from hostapd
  using doing query from hostapd_cli & add the device entry
  using the MAC address.

@parameters
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
  void

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

void QCMAP_WLAN::ParseMacAndAddDeviceEntry
(
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num1, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_V01:
    {
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num2, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_2_V01:
    {
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num3, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_3_V01:
    {
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num4, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_ALL_AP_V01:
    {
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num1, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num2, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num3, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
      if(!HostapdCliCmdParseMac(QcMapMgr->ap_dev_num4, ap_type))
      {
        LOG_MSG_ERROR("HostapdCliCmdParseMac failed %d",ap_type, 0, 0);
      }
    }
    break;

    default:
    {
      LOG_MSG_ERROR("HostapdCliCmdParseMac failed-Invalid ap type %d",ap_type, 0, 0);
    }
    break;
  }
}

