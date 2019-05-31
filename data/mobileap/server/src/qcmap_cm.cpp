/******************************************************************************

                           QCMAP_CM.CPP

******************************************************************************/

/******************************************************************************

  @file    qcmap_cm.c
  @brief   Mobile AP Connection Manager Lib Implementation

  DESCRIPTION
  Mobile AP Connection Manager Lib Implementation.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        --------------------------------------------------------
07/11/12   gk         9x25
10/26/12   cp         Added support for Dual AP and different types of NAT.
03/28/13   mp         Added support to get IPv6 WWAN configuration.
01/03/14   vm         Changes to support IoE on 9x25
01/16/14   cp         Added support for modem loopback call.
01/05/15   rk         qtimap offtarget support.
03/28/17   spr        Added support for Multi-PDN.
******************************************************************************/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
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
#include "comdef.h"
#ifdef FEATURE_QTIMAP_OFFTARGET
#include "linux/msg.h"
#else
#include "msg.h"
#endif
#include "ds_util.h"
#include "qmi_client.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "network_access_service_v01.h"
#include "wireless_data_service_v01.h"
#include "qmi_client_instance_defs.h"
#include "qcmap_cm_api.h"

#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_Tethering.h"

extern unsigned int dsi_qcmap_sockfd;
extern unsigned int cmdq_qcmap_sockfd;
extern unsigned int nas_qcmap_sockfd;
extern unsigned int dsd_qcmap_sockfd;
extern unsigned int qmi_service_qcmap_sockfd;
extern unsigned int timer_qcmap_sockfd;

extern unsigned int modem_loopback_mode;
//extern QCMAP_ConnectionManager *QcMapMgr;


#define QCMAP_STA_COOKIE 0xDCDCDCDC
#define TEMP_SED_INPLACE_FILE "/var/run/sed_inplace"

int
qcmap_cm_qmi_timer_ind
(
  qcmap_timer_enum_t  timer_id,                         /* Indicator message ID  */
  uint32              pUserData                       /* user_data */
);
/*===========================================================================
                     FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
   Type representing an QCMAP CM CB (QCMAP CM control block); this captures
   all state and other data related to the QCMAP CM
---------------------------------------------------------------------------*/
typedef struct
{
  int                       handle;

  /* libqcmap_cm library callback info */
  qcmap_cm_cb_fcn           qcmap_cm_callback;
  void                     *qcmap_cm_callback_user_data;

  /* Whether it is init */
  boolean                   init;

  /* Whether WWAN Backhaul is in roaming*/
  uint8                     roaming_indicator;

  /*Whether disable is in progress*/
  boolean                   disable_in_process;

  /* STA Cookie Value. */
  uint32                    sta_cookie;

  /* MobileAP config read from XML file or default.*/
  qcmap_cm_conf_t          *cfg;

  /* QMI NAS service info */
  qmi_client_type           qmi_nas_handle;
  qmi_client_type           qmi_nas_notifier;
  qmi_cci_os_signal_type    qmi_nas_os_params;

  /* QMI WDS service info */
  qmi_client_type           qmi_wds_handle;
  qmi_client_type           qmi_wds_notifier;
  qmi_cci_os_signal_type    qmi_wds_os_params;

  /* QMI DSD service info */
  qmi_client_type           qmi_dsd_handle;
  qmi_client_type           qmi_dsd_notifier;
  qmi_cci_os_signal_type    qmi_dsd_os_params;
}qcmap_cm_cb_t;

/*---------------------------------------------------------------------------
   Mobile AP CM control block
---------------------------------------------------------------------------*/

#if 1
static qcmap_cm_cb_t qcmap_cm_cb = {
 1,      //handle
 NULL,   //qcmap_cm_callback
 NULL,   //qcmap_cm_callback_user_data
 false,  //init
 0,      //roaming_indicatior
 false,  //disable_in_process
 0       //sta_cookie
 };

#else
static qcmap_cm_cb_t qcmap_cm_cb;

  qcmap_cm_cb.state                       = QCMAP_CM_DISABLE;
  qcmap_cm_cb.ipv6_state                  = QCMAP_CM_V6_DISABLE;
  qcmap_cm_cb.handle                      = 1;
  qcmap_cm_cb.mobile_ap_handle            = QCMAP_WAN_INVALID_QCMAP_HANDLE;
 qcmap_cm_cb.ipv6_mobile_ap_handle       = QCMAP_WAN_INVALID_QCMAP_HANDLE;
  qcmap_cm_cb.qcmap_cm_callback           = NULL;
  qcmap_cm_cb.qcmap_cm_callback_user_data = NULL;
  qcmap_cm_cb.init                        = false;
  qcmap_cm_cb.backhaul_service            = false;
  qcmap_cm_cb.roaming_indicator           = 0;
  qcmap_cm_cb.eri_roam_data_len           = 0;
  qcmap_cm_cb.disable_in_process          = false;
  qcmap_cm_cb.sta_cookie                    = 0;
#endif

/*===========================================================================
  FUNCTION qcmap_cm_get_handle
===========================================================================*/
/*!
@brief
  Get qcmap_cm handle

@return
  int

@note

- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/
int qcmap_cm_get_handle()
{
  return qcmap_cm_cb.handle;
}

/*===========================================================================
  FUNCTION qcmap_cm_is_disable_in_process
===========================================================================*/
/*!
@brief
  Get disable_in_process.

@return
  boolean

@note
- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/
boolean qcmap_cm_is_disable_in_process()
{
  return qcmap_cm_cb.disable_in_process;
}

/*===========================================================================
  FUNCTION qcmap_cm_send_event
===========================================================================*/
/*!
@brief
  Send qcmap_cm_send_event to client

@return
  None

@note

- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/
void qcmap_cm_send_event
(
  qcmap_cm_event_e event,
  void *wwanObj,
  dsi_ce_reason_t *dsi_reason,
  qcmap_msgr_wwan_info_v01 *wwan_info
  )
{
  qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, event, (void *)wwanObj,
        qcmap_cm_cb.qcmap_cm_callback_user_data, dsi_reason, wwan_info);
}

/*===========================================================================
  FUNCTION qcmap_cm_register_nas_sys_info
===========================================================================*/
/*!
@brief
  Register for NAS sys_info indication.

@return
  void

@note

- Dependencies
  - None

- Side Effects
  - None
*/
/*=========================================================================*/
void qcmap_cm_register_nas_sys_info(boolean enable)
{
  qmi_client_error_type                qmi_error;
    nas_indication_register_req_msg_v01  qcmap_nas_indication_register_req_msg_v01;
  nas_indication_register_resp_msg_v01 qcmap_nas_indication_register_resp_msg_v01;

  memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));

  if (enable == TRUE)
  {
    /*optional TLV so set the valid flag */
    qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;
    LOG_MSG_INFO1("Registering from serving system NAS indications",0,0,0);
  }
  else
  {
    qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.sys_info = 0x00;
    LOG_MSG_INFO1("De-Registering from serving system NAS indications",0,0,0);
  }

#ifndef FEATURE_QTIMAP_OFFTARGET
  qmi_error = qmi_client_send_msg_sync(qcmap_cm_cb.qmi_nas_handle,
                                    QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                    &qcmap_nas_indication_register_req_msg_v01,
                                    sizeof(qcmap_nas_indication_register_req_msg_v01),
                                    &qcmap_nas_indication_register_resp_msg_v01,
                                    sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                    QCMAP_CM_QMI_TIMEOUT_VALUE);
  if(qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to deregister from NAS indications %d", qmi_error,0,0);
    return;
  }
#endif
}

/*===========================================================================
  FUNCTION qcmap_cm_dsi_net_init_cb
===========================================================================*/
/*!
@brief
  This function is a call back function to dsi_init_ex. After the dsi module
  is initialized, this call back is called and it notifies QCMAP that dsi is
  initilialized.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_cm_dsi_net_init_cb(void)
{
  qcmap_dsi_buffer_t qcmap_dsi_buffer;
  int numBytes = 0, len;
  struct sockaddr_un dsi_qcmap;
  uint32_t *user_data = NULL;

  dsi_qcmap.sun_family = AF_UNIX;
  strlcpy(dsi_qcmap.sun_path, QCMAP_DSI_UDS_FILE,sizeof(dsi_qcmap.sun_path));
  len = strlen(dsi_qcmap.sun_path) + sizeof(dsi_qcmap.sun_family);
  bzero(&qcmap_dsi_buffer, sizeof(qcmap_dsi_buffer_t));

  user_data = (uint32_t *)malloc(sizeof(uint32_t));
  if (user_data == NULL)
  {
    LOG_MSG_ERROR("Out of Memory in dsi_init_cb!!", 0,0,0);
    return;
  }
  /* DSI initialized */
  *user_data = DSI_INITED;
  qcmap_dsi_buffer.user_data = user_data;

  if ((numBytes = sendto(dsi_qcmap_sockfd, (void *)&qcmap_dsi_buffer, sizeof(qcmap_dsi_buffer_t), 0,
             (struct sockaddr *)&dsi_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from dsi callback context", 0, 0, 0);
    free(user_data);
    user_data = NULL;
    return;
  }
  LOG_MSG_INFO1("Send succeeded in dsi callback context", 0, 0, 0);
  return;
}

/*===========================================================================
  FUNCTION qcmap_timer_handler
===========================================================================*/
/*!
@brief
  Function used to handle various QCMAP timers

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_timer_handler(int sig, siginfo_t *si, void *uc)
{

  qcmap_timer_enum_t event = -1;
  boolean send_event = true;
  QCMAP_ConnectionManager* QcMapMgr  =QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  qcmap_timer_data_t *pTimerData = (qcmap_timer_data_t *)si->si_value.sival_ptr;
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  uint32 user_data;

  if (pTimerData == NULL)
  {
     LOG_MSG_ERROR("qcmap_timer_handler pUserData is NULL",0 ,0, 0);
     return;
  }

  pBackhaulWWAN = pTimerData->pUserData;
  if ( (pBackhaulWWAN == NULL) &&
       (pTimerData->timer_id == AUTO_CONNECT_V4 || pTimerData->timer_id == AUTO_CONNECT_V6)
       )
    {
        LOG_MSG_ERROR("pTimerData->pUserData is NULL", 0,0,0);
        return;
    }

  if(QcMapMgr)
  {
    switch (pTimerData->timer_id)
    {
      case AUTO_CONNECT_V4:
      {
        pBackhaulWWAN->auto_connect_timer_running_v4 = false;
        event = AUTO_CONNECT_V4;
        user_data = (uint32)pBackhaulWWAN;
      }
      break;

      case AUTO_CONNECT_V6:
      {

        event = AUTO_CONNECT_V6;
        pBackhaulWWAN->auto_connect_timer_running_v6 = false;
        user_data = (uint32)pBackhaulWWAN;
      }
      break;

      case STA_ASSOC_FAIL:
      {

        if ((NULL != QcMapBackhaulWLANMgr) && QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running)
        {
          event = STA_ASSOC_FAIL;
          QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running = false;
          user_data = (uint32)QcMapBackhaulWLANMgr;
        }
        else
        {
          LOG_MSG_ERROR("WLAN Assoc Timer Expired: Event recieved even though timer is not running",0,0,0);
          send_event=false;
        }
      }
      break;

      case STA_DHCP_FAIL:
      {

        if ((NULL != QcMapBackhaulWLANMgr) && QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running)
        {
          event = STA_DHCP_FAIL;
          QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running = false;
          user_data = (uint32)QcMapBackhaulWLANMgr;
        }
        else
        {
          LOG_MSG_ERROR("WLAN DHCP Timer Expired: Event recieved even though timer is not running",0,0,0);
          send_event=false;
        }
      }
      break;

      case NS_NA_PROXY_CLEANUP:
      {

        if ((NULL != QcMapBackhaulWLANMgr) && QcMapBackhaulWLANMgr->IsAPSTABridgeActivated() &&
            QcMapBackhaulWLANMgr->sta_v6_available)
        {
           QcMapBackhaulWLANMgr->CleanV6ProxyEntries();
           user_data = (uint32)QcMapBackhaulWLANMgr;
        }
        else
        {
            LOG_MSG_ERROR("WLAN Proxy Cleanup Timer Expired: even though we are not in v6 bridge mode",0,0,0);
            send_event=false;
        }
      }
      break;

      default:
        send_event = false;
      break;
    }
  }

  if (send_event)
    qcmap_cm_qmi_timer_ind(event, user_data);
}

/*===========================================================================
 FUNCTION start_wlan_sta_timer
===========================================================================*/
/*!
@brief
  Used to start the WLAN STA timer(s)

@return
  void

@note

  - Dependencies
  - None

  - Side Effects
  - None
*/
/*=========================================================================*/
int start_wlan_sta_timer(qcmap_timer_enum_t sta_event)
{
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

  LOG_MSG_INFO1("start_wlan_sta_timer() event=%d",sta_event,0,0);
  struct itimerspec its;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec =0;

  if (!QcMapBackhaulWLANMgr)
  {
    LOG_MSG_ERROR("WLAN is not backhaul",0,0,0);
    return QCMAP_CM_ERROR;
  }

  if( (sta_event == STA_ASSOC_FAIL) && (QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running == false))
  {
    QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running=true;
    LOG_MSG_INFO1("WLAN STA Assoc Timer Started for timerid %d",QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_assoc,0,0);
    its.it_value.tv_sec = QCMAP_WLAN_STA_ASSOC_TIMEOUT;
    if (timer_settime(QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_assoc, 0, &its, NULL) == -1)
    {
       LOG_MSG_INFO1("start_wlan_sta_timer timer_settime failed error:%s",strerror(errno),0,0);
       return QCMAP_CM_ERROR;
    }
  }
  else if ( (sta_event == STA_DHCP_FAIL) && (QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running == false) )
  {
   //no checks for WLAN object since this function is called by APSTA obj which will only come up when WLAN is enabled.
    QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running=true;
    its.it_value.tv_sec = QCMAP_WLAN_STA_DHCP_TIMEOUT;
    LOG_MSG_INFO1("WLAN STA DHCP IP Assignment Timer Started for timerid %d",QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_dhcp,0,0);
    if (timer_settime(QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_dhcp, 0, &its, NULL) == -1 )
    {
       LOG_MSG_INFO1("start_wlan_sta_timer timer_settime failed error:%s",strerror(errno),0,0);
       return QCMAP_CM_ERROR;
    }
  }
  else if (sta_event == NS_NA_PROXY_CLEANUP)
  {
    LOG_MSG_INFO1("stopping WLAN STA timer for timer %d \n",QcMapBackhaulWLANMgr->apsta_cfg.timerid_proxy_cleanup,0,0);
    its.it_value.tv_sec = QCMAP_BRIDGE_PROXY_TIMEOUT;
    if (timer_settime(QcMapBackhaulWLANMgr->apsta_cfg.timerid_proxy_cleanup, 0, &its, NULL) == -1 )
    {
       ds_log_med("stop_wlan_sta_timer timer_settime failed error:%s",strerror(errno));
       return QCMAP_CM_ERROR;
    }
  }
  else
  {
    LOG_MSG_INFO1("Incorrect Event passed in start_wlan_sta_timer() event =%d",sta_event,0,0);
    return QCMAP_CM_ERROR;
  }
  return QCMAP_CM_SUCCESS;
}

/*===========================================================================
 FUNCTION stop_wlan_sta_timer
===========================================================================*/
/*!
@brief
  Used to stop the WLAN STA timer(s)

@return
  void

@note

  - Dependencies
  - None

  - Side Effects
  - None
*/
/*=========================================================================*/
int stop_wlan_sta_timer(qcmap_timer_enum_t sta_event)
{
  struct itimerspec its;
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

  LOG_MSG_INFO1("stopping wlan_sta timer for event:%d\n",sta_event,0,0);

  if (!QcMapBackhaulWLANMgr)
  {
    LOG_MSG_ERROR("WLAN is not backhaul",0,0,0);
    return QCMAP_CM_ERROR;
  }

  if(sta_event == STA_ASSOC_FAIL && QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running)
  {
    LOG_MSG_INFO1("stopping WLAN STA timer for timer %d \n",QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_assoc,0,0);
    if (timer_settime(QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_assoc, 0, &its, NULL) == -1 )
    {
       ds_log_med("stop_wlan_sta_timer timer_settime failed error:%s",strerror(errno));
       return QCMAP_CM_ERROR;
    }
    QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running=false;
  }
  else if (sta_event == STA_DHCP_FAIL && QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running )
  {
    LOG_MSG_INFO1("stopping WLAN STA timer for timer %d \n",QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_dhcp,0,0);
    if (timer_settime(QcMapBackhaulWLANMgr->apsta_cfg.timerid_sta_dhcp, 0, &its, NULL) == -1 )
    {
       ds_log_med("stop_wlan_sta_timer timer_settime failed error:%s",strerror(errno));
       return QCMAP_CM_ERROR;
    }
    QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running=false;
  }
  else if (sta_event == NS_NA_PROXY_CLEANUP)
  {
    LOG_MSG_INFO1("stopping WLAN STA timer for timer %d \n",QcMapBackhaulWLANMgr->apsta_cfg.timerid_proxy_cleanup,0,0);
    if (timer_settime(QcMapBackhaulWLANMgr->apsta_cfg.timerid_proxy_cleanup, 0, &its, NULL) == -1 )
    {
       ds_log_med("stop_wlan_sta_timer timer_settime failed error:%s",strerror(errno));
       return QCMAP_CM_ERROR;
    }
  }
  else
  {
    LOG_MSG_ERROR("Incorrect Event passed in stop_wlan_sta_timer() event =%d, sta_assoc_running =%d, sta_dhcp_running=%d",
                    sta_event,QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running,QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running);
    return QCMAP_CM_ERROR;
  }
  return QCMAP_CM_SUCCESS;
}

/*============================================================
  FUNCTION qcmap_cm_nas_init
==============================================================
@brief
  API to register QMI NAS Service available call back

@return
  QCMAP_CM_ERROR - Could not register QMI NAS service callback
  QCMAP_CM_SUCCESS - Successfully registered QMI NAS service call back
@note

  - Dependencies
    - None

  - Side Effects
    - None
/*==========================================================*/


int qcmap_cm_nas_init()
{
  qmi_idl_service_object_type nas_qmi_idl_service_object;
  qmi_client_error_type qmi_error;

  nas_qmi_idl_service_object = nas_get_service_object_v01();
  if (nas_qmi_idl_service_object == NULL)
  {
    LOG_MSG_INFO1("qcmap service object(nas) not available.",0,0,0);
    return QCMAP_CM_ERROR;
  }

  qmi_error = qmi_client_notifier_init(nas_qmi_idl_service_object,
                                       &qcmap_cm_cb.qmi_nas_os_params,
                                       &qcmap_cm_cb.qmi_nas_notifier);
  if (qmi_error < 0)
  {
    LOG_MSG_INFO1("qmi_client_notifier_init(nas) returned %d",
                  qmi_error,0,0);
    return QCMAP_CM_ERROR;
  }

  qmi_error =  qmi_client_register_notify_cb(qcmap_cm_cb.qmi_nas_notifier,
                                             qmi_nas_service_available_cb,
                                             NULL);

  if (qmi_error < 0)
  {
    LOG_MSG_INFO1("qmi_client_register_notify_cb(nas) returned %d",
                  qmi_error,0,0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1(" qcmap_cm_nas_init : QMI NAS init successful", 0, 0, 0);
  return QCMAP_CM_SUCCESS;

}


/*============================================================
  FUNCTION qmi_nas_service_available_cb
==============================================================
@brief
 QMI NAS Service Available callback handler

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
/*==========================================================*/

void qmi_nas_service_available_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
)
{
  qcmap_qmi_service_buffer_t qcmap_qmi_service_buffer;
  int numBytes=0, len;
  struct sockaddr_un qmi_service_qcmap;

  LOG_MSG_INFO1(" qmi_nas_service_available_cb", 0, 0, 0);

  switch (service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      qmi_service_qcmap.sun_family = AF_UNIX;
      strlcpy(qmi_service_qcmap.sun_path, QCMAP_QMI_SERVICE_UDS_FILE, sizeof(QCMAP_QMI_SERVICE_UDS_FILE));
      len = strlen(qmi_service_qcmap.sun_path) + sizeof(qmi_service_qcmap.sun_family);

      qcmap_qmi_service_buffer.qmi_service_id = QCMAP_QMI_NAS_IN_SERVICE;

      if ((numBytes = sendto(qmi_service_qcmap_sockfd, (void *)&qcmap_qmi_service_buffer,
                              sizeof(qcmap_qmi_service_buffer_t), MSG_DONTWAIT,
                             (struct sockaddr *)&qmi_service_qcmap, len)) == -1)
      {
        LOG_MSG_ERROR("Send Failed from qmi_nas_service_available_cb context", 0, 0, 0);
        return;
      }
      break;
    default:
      LOG_MSG_INFO1(" qmi_nas_service_available_cb: Invalid Event %d",
                    service_event, 0, 0);
      break;
  }
  return;
}

/*============================================================
  FUNCTION qmi_nas_error_cb
=============================================================

@brief
 QMI NAS Error callback handler

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
/*==========================================================*/
void qmi_nas_error_cb()
{
  qcmap_qmi_service_buffer_t qcmap_qmi_service_buffer;
  int numBytes=0, len;
  struct sockaddr_un qmi_service_qcmap;

  LOG_MSG_INFO1(" qmi_nas_error_cb", 0, 0, 0);

  qmi_service_qcmap.sun_family = AF_UNIX;
  strlcpy(qmi_service_qcmap.sun_path, QCMAP_QMI_SERVICE_UDS_FILE, sizeof(QCMAP_QMI_SERVICE_UDS_FILE));
  len = strlen(qmi_service_qcmap.sun_path) + sizeof(qmi_service_qcmap.sun_family);

  qcmap_qmi_service_buffer.qmi_service_id = QCMAP_QMI_NAS_NOT_IN_SERVICE;

  if ((numBytes = sendto(qmi_service_qcmap_sockfd, (void *)&qcmap_qmi_service_buffer,
                          sizeof(qcmap_qmi_service_buffer_t), 0,
                         (struct sockaddr *)&qmi_service_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from qmi_nas_error_cb context", 0, 0, 0);
    return;
  }
  return;
}

/*===========================================================================
  FUNCTION qmi_dsd_client_init
==========================================================================*/
/*!
@brief
  Initialize DSD client

@parameters
  -None

@return
QCMAP_CM_ERROR - Initialize DSD client failed
QCMAP_CM_SUCCESS - Successfully Initialized DSD client

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int qmi_dsd_client_init()
{
  qmi_client_error_type qmi_error;
  qmi_cci_os_signal_type qmi_dsd_os_params;
  qmi_idl_service_object_type qmi_dsd_service_object;
  dsd_system_status_change_req_msg_v01 dsdreq;
  dsd_system_status_change_resp_msg_v01 dsdresp;

  memset(&dsdreq, 0, sizeof(dsdreq));
  memset(&dsdresp, 0, sizeof(dsdresp));

  /* DSD Client registration */
  qmi_dsd_service_object = dsd_get_service_object_v01();
  if(qmi_dsd_service_object == NULL)
  {
    LOG_MSG_ERROR("DSD object is not available\n", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }
  LOG_MSG_INFO1("Got DSD object\n", 0, 0, 0);

  LOG_MSG_INFO1("Start initing DSD qmi client with timeout: %d s\n", QMI_MAX_TIMEOUT_MS/1000, 0, 0);


  qmi_error = qmi_client_init_instance(qmi_dsd_service_object,
                                       QMI_CLIENT_INSTANCE_ANY,
                                       qcmap_cm_qmi_dsd_ind,
                                       NULL,
                                       &qmi_dsd_os_params,
                                       QMI_MAX_TIMEOUT_MS,
                                       &qcmap_cm_cb.qmi_dsd_handle);


  if(qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to init DSD qmi client %d\n", qmi_error, 0, 0);
    if(qcmap_cm_cb.qmi_dsd_handle != NULL)
    {
      qmi_client_release(qcmap_cm_cb.qmi_dsd_handle);
      qcmap_cm_cb.qmi_dsd_handle = NULL;
    }
    return QCMAP_CM_ERROR;
  }
  LOG_MSG_INFO1("DSD qmi client is inited with handle %d\n",
                qcmap_cm_cb.qmi_dsd_handle, 0, 0);

  /* Register for QMI_DSD_SYSTEM_STATUS_IND_V01 indication*/
  dsdreq.limit_so_mask_change_ind_valid=1;
  dsdreq.limit_so_mask_change_ind = 1;

  qmi_error = qmi_client_send_msg_sync(qcmap_cm_cb.qmi_dsd_handle,
                                       QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                                       (void *)&dsdreq,
                                       sizeof(dsdreq),
                                       (void*)&dsdresp,
                                       sizeof(dsdresp),
                                       QMI_MAX_TIMEOUT_MS);

  if (qmi_error != QMI_NO_ERR || dsdresp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LOG_MSG_ERROR("Failed to register indication with qmi_error=%d result=%d\n",
                  qmi_error, dsdresp.resp.result, 0);
    qmi_client_release(qcmap_cm_cb.qmi_dsd_handle);
    qcmap_cm_cb.qmi_dsd_handle = NULL;
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("DSD indication registration success\n", 0, 0, 0);

  return QCMAP_CM_SUCCESS;
}


/*============================================================
  FUNCTION qmi_nas_client_init
==============================================================
@brief
 QMI NAS Client Init

@return
  QCMAP_CM_ERROR in case of error
  QCMAP_CM_SUCCESS for sucess

@note

  - Dependencies
    - None

  - Side Effects
    - None
/*==========================================================*/

int qmi_nas_client_init()
{
  int ret_val;
  qmi_client_error_type qmi_error;
  qmi_error_type_v01 qmi_err_num;
  int qcmap_cm_errno;
  qmi_idl_service_object_type nas_qmi_idl_service_object;
  nas_indication_register_req_msg_v01 qcmap_nas_indication_register_req_msg_v01;
  nas_indication_register_resp_msg_v01 qcmap_nas_indication_register_resp_msg_v01;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  char val[MAX_STRING_LENGTH]={0};//to save current state of enable_ipv4 and enable_ipv6 variables
  int retry_count = 0;
  boolean prev_auto_state = false;

  if (qcmap_cm_cb.qmi_nas_handle != NULL)
  {
    /* Client already initialized.*/
    LOG_MSG_INFO1("NAS client already initialized.", 0, 0, 0);
    return QCMAP_CM_SUCCESS;
  }

  nas_qmi_idl_service_object = nas_get_service_object_v01();
  if (nas_qmi_idl_service_object == NULL)
  {
    LOG_MSG_ERROR("QCMAP service object(nas) not available.", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("Releasing QMI NAS notifier",0,0,0);

  qmi_error = qmi_client_release(qcmap_cm_cb.qmi_nas_notifier);
  qcmap_cm_cb.qmi_nas_notifier = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Cannot release client nas notifier %d",
                   qmi_error, 0, 0);
    return QCMAP_CM_ERROR;
  }

  qmi_error = qmi_client_init_instance(nas_qmi_idl_service_object,
                                       QMI_CLIENT_INSTANCE_ANY,
                                       qcmap_cm_qmi_nas_ind,
                                       NULL,
                                       &qcmap_cm_cb.qmi_nas_os_params,
                                       QCMAP_CM_QMI_TIMEOUT_VALUE,
                                       &qcmap_cm_cb.qmi_nas_handle);

  if(qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to init NAS qmi client %d\n", qmi_error, 0, 0);
    if(qcmap_cm_cb.qmi_nas_handle != NULL)
    {
      qmi_client_release(qcmap_cm_cb.qmi_nas_handle);
      qcmap_cm_cb.qmi_nas_handle = NULL;
    }
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("QCMAP: Nas client inited %d", qcmap_cm_cb.qmi_nas_handle, 0, 0);
  /*deregister from all NAS indications*/
  memset(&qcmap_nas_indication_register_req_msg_v01, 0, sizeof(nas_indication_register_req_msg_v01));

  qcmap_nas_indication_register_req_msg_v01.dual_standby_pref_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.dual_standby_pref = 0x00;
  qcmap_nas_indication_register_req_msg_v01.err_rate_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.err_rate = 0x00;
  qcmap_nas_indication_register_req_msg_v01.network_reject_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.network_reject.reg_network_reject = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_csp_plmn_mode_bit_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_csp_plmn_mode_bit =0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_current_plmn_name_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_current_plmn_name = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_ddtm_events_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_ddtm_events = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_embms_status_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_embms_status = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_session_close_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_session_close = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_uati_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_hdr_uati = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_managed_roaming_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_managed_roaming = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_network_time_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_network_time = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_operator_name_data_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_operator_name_data = 0x00;
  if (QcMapMgr &&
      (QcMapMgr->target == DS_TARGET_LE_MDM9x06 ||
       QcMapMgr->target ==  DS_TARGET_LYKAN))
  {
    qcmap_nas_indication_register_req_msg_v01.reg_rf_band_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.reg_rf_band_info = 0x01;
  }
  qcmap_nas_indication_register_req_msg_v01.reg_rtre_cfg_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_rtre_cfg = 0x00;
  qcmap_nas_indication_register_req_msg_v01.reg_sys_sel_pref_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.reg_sys_sel_pref = 0x00;
  qcmap_nas_indication_register_req_msg_v01.sig_info_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.sig_info = 0x00;
  qcmap_nas_indication_register_req_msg_v01.subscription_info_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.subscription_info = 0x00;
  qcmap_nas_indication_register_req_msg_v01.req_serving_system_valid = TRUE;
  qcmap_nas_indication_register_req_msg_v01.req_serving_system = 0x00;

  /*register for serving system NAS indication if autoconnect is enabled*/
  if(QCMAP_Backhaul_WWAN::IsAutoConnectOnForAnyPDN())
  {
    LOG_MSG_INFO1("Registering for NAS Sys Info indications",0,0,0);
    qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.sys_info = 0x01;
  }
  else
  {
    LOG_MSG_INFO1("Deregistering from NAS Sys info indications",0,0,0);
    qcmap_nas_indication_register_req_msg_v01.sys_info_valid = TRUE;
    qcmap_nas_indication_register_req_msg_v01.sys_info = 0x00;
  }

  qmi_error = qmi_client_send_msg_sync(qcmap_cm_cb.qmi_nas_handle,
                                       QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                       &qcmap_nas_indication_register_req_msg_v01,
                                       sizeof(qcmap_nas_indication_register_req_msg_v01),
                                       &qcmap_nas_indication_register_resp_msg_v01,
                                       sizeof(qcmap_nas_indication_register_resp_msg_v01),
                                       QCMAP_CM_QMI_TIMEOUT_VALUE);
  if (qmi_error != QMI_NO_ERR)
  {
    qmi_client_release(qcmap_cm_cb.qmi_nas_handle);
    qcmap_cm_cb.qmi_nas_handle = NULL;
    LOG_MSG_ERROR( "Can not perform NAS indication register %d",
                  qmi_error, 0, 0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("NAS indication registration success\n", 0, 0, 0);

/*-----------------------------------------------------------------------------
  Initialize the WDS client
------------------------------------------------------------------------------*/
  if ( qmi_wds_client_init() == QCMAP_CM_ERROR )
  {
    LOG_MSG_ERROR("Not able to Init QMI WDS client",0, 0, 0);
    if(qcmap_cm_cb.qmi_nas_handle != NULL)
    {
      qmi_client_release(qcmap_cm_cb.qmi_nas_handle);
      qcmap_cm_cb.qmi_nas_handle = NULL;
    }
    return QCMAP_CM_ERROR;
  }

  /*-----------------------------------------------------------------------------
  Initialize the Qmi dsd  client
------------------------------------------------------------------------------*/
  if (qmi_dsd_client_init() == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("Not able to Init qmi DSD client",0, 0, 0);
    if(qcmap_cm_cb.qmi_nas_handle != NULL)
    {
      qmi_client_release(qcmap_cm_cb.qmi_nas_handle);
      qcmap_cm_cb.qmi_nas_handle = NULL;
    }
    return QCMAP_CM_ERROR;
  }

  if (QcMapMgr && QcMapMgr->is_ssr_in_progress)
  {
     QCMAP_Backhaul_WWAN::DisconnectAllPDNs();

     while (retry_count < QCMAP_SSR_DSI_INIT_MAX_RETRY)
     {
         if (dsi_init(DSI_MODE_SSR) == DSI_SUCCESS)
         {
            break;
         }
         sleep(QCMAP_SSR_DSI_INIT_MAX_TIMEOUT);
         retry_count++;
         LOG_MSG_ERROR("Cannot init DSI after SSR, retry: %d", retry_count, 0, 0);
     }

     if (retry_count == QCMAP_SSR_DSI_INIT_MAX_RETRY)
     {
        if(qcmap_cm_cb.qmi_nas_handle != NULL)
        {
            qmi_client_release(qcmap_cm_cb.qmi_nas_handle);
            qcmap_cm_cb.qmi_nas_handle = NULL;
        }
        if(qcmap_cm_cb.qmi_wds_handle != NULL)
        {
            qmi_client_release(qcmap_cm_cb.qmi_wds_handle);
            qcmap_cm_cb.qmi_wds_handle = NULL;
        }
        LOG_MSG_ERROR("DSI Init failed after SSR", 0, 0, 0);
        return QCMAP_CM_ERROR;
     }

     QcMapMgr->is_ssr_in_progress = false;
     LOG_MSG_INFO1("DSI Init Success after SSR", 0, 0, 0);
   }

   /*-----------------------------------------------------------------------------
     Register for err callback
   ------------------------------------------------------------------------------*/
   (void) qmi_client_register_error_cb(qcmap_cm_cb.qmi_nas_handle,
                                      qmi_nas_error_cb,
                                      NULL);

   QCMAP_Backhaul_WWAN::ConnectAllPDNs();
   return QCMAP_CM_SUCCESS;
}

/*============================================================
  FUNCTION qmi_nas_not_in_service
=============================================================

@brief
   Function invoked during SSR functionality, when modem is not in service.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
/*==========================================================*/
int qmi_nas_not_in_service()
{
  qmi_client_error_type                 qmi_error;
  qmi_idl_service_object_type nas_qmi_idl_service_object;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  qmi_error = qmi_client_release(qcmap_cm_cb.qmi_nas_handle);
  qcmap_cm_cb.qmi_nas_handle = NULL;
  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Cannot release NAS client %d", qmi_error, 0, 0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("Successfully deregistered NAS client", 0, 0, 0);

  qmi_error = qmi_client_release(qcmap_cm_cb.qmi_wds_handle);
  qcmap_cm_cb.qmi_wds_handle = NULL;
  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Cannot release WDS client %d", qmi_error, 0, 0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("Successfully deregistered WDS client", 0, 0, 0);

  //release DSD handle
  qmi_error = qmi_client_release(qcmap_cm_cb.qmi_dsd_handle);
  qcmap_cm_cb.qmi_dsd_handle = NULL;
  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Cannot release DSD client %d", qmi_error, 0, 0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("Successfully deregistered DSD client", 0, 0, 0);

  QcMapMgr->is_ssr_in_progress = true;

  nas_qmi_idl_service_object = nas_get_service_object_v01();
  if (nas_qmi_idl_service_object == NULL)
  {
    LOG_MSG_ERROR("qcpmap service object(nas) not available.", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

/*----------------------------------------------------------------------------
    Register callback to get notified when modem is in service
---------------------------------------------------------------------------- */
  (void) qmi_client_notifier_init(nas_qmi_idl_service_object,
                                  &qcmap_cm_cb.qmi_nas_os_params,
                                  &qcmap_cm_cb.qmi_nas_notifier);

  (void) qmi_client_register_notify_cb(qcmap_cm_cb.qmi_nas_notifier,
                                       qmi_nas_service_available_cb,
                                       NULL);
  return QCMAP_CM_SUCCESS;
}

/*============================================================
  FUNCTION qmi_wds_client_init
=============================================================

@brief
 QMI WDS Client Init

@return
  QCMAP_CM_ERROR in case of error
  QCMAP_CM_SUCCESS for sucess

@note

  - Dependencies
    - None

  - Side Effects
    - None
/*==========================================================*/
int qmi_wds_client_init()
{
  qmi_idl_service_object_type wds_qmi_idl_service_object;
  qmi_client_error_type qmi_error;

  wds_qmi_idl_service_object = wds_get_service_object_v01();
  if(wds_qmi_idl_service_object == NULL)
  {
    LOG_MSG_ERROR(" wds get service object failed",0,0,0);
    return QCMAP_CM_ERROR;
  }

  qmi_error = qmi_client_init_instance(wds_qmi_idl_service_object,
                                       QMI_CLIENT_INSTANCE_ANY,
                                       NULL,
                                       NULL,
                                       &qcmap_cm_cb.qmi_wds_os_params,
                                       QCMAP_CM_QMI_TIMEOUT_VALUE,
                                       &qcmap_cm_cb.qmi_wds_handle);

  if(qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to init WDS qmi client %d\n", qmi_error, 0, 0);
    if(qcmap_cm_cb.qmi_wds_handle != NULL)
    {
      qmi_client_release(qcmap_cm_cb.qmi_wds_handle);
      qcmap_cm_cb.qmi_wds_handle = NULL;
    }
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("QCMAP: Wds client inited %d", qcmap_cm_cb.qmi_wds_handle, 0, 0);
  return QCMAP_CM_SUCCESS;
}

/*===========================================================================
  FUNCTION qcmap_cm_process_dsi_init_ind
===========================================================================*/
/*!
@brief
 This Fuction will trigger backhaul request, if auto connect is enabled.
 It is called as a part of handling dsi_net initialization.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_cm_process_dsi_init_ind(void)
{
  int qcmap_cm_errno = 0;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_Backhaul_WWAN::ConnectAllPDNs();
}

/*===========================================================================
  FUNCTION  qcmap_cm_process_qmi_nas_ind
===========================================================================*/
/*!
@brief
 Processes an incoming QMI NAS Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_qmi_nas_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{
  qmi_client_error_type qmi_error = QMI_NO_ERR;
  nas_sys_info_ind_msg_v01 sys_info_ind;
  nas_rf_band_info_ind_msg_v01 band_info_ind;
  nas_service_domain_enum_type_v01 srv_domain         = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_lte     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_hdr     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_cdma    = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_wcdma   = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_gsm     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_tdscdma = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status         = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_lte     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_hdr     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_cdma    = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_wcdma   = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_gsm     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_tdscdma = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_roam_status_enum_type_v01 roam_status = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  int               err;
  int              *qcmap_cm_errno = &err;
  qcmap_cm_event_e  event;
  uint8             old_roam_status;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  ds_target_t target = DS_TARGET_INVALID;
  LOG_MSG_INFO1("qcmap_cm_process_qmi_nas_ind: user_handle %d msg_id %d ind_buf_len %d.",
                user_handle,
                msg_id,
                ind_buf_len);

  memset(&sys_info_ind, 0, sizeof(nas_sys_info_ind_msg_v01));
  memset(&band_info_ind, 0, sizeof(nas_rf_band_info_ind_msg_v01));

  /* Process based on indication type */
  switch (msg_id)
  {
    case QMI_NAS_SYS_INFO_IND_MSG_V01:
      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &sys_info_ind,
                                            sizeof(nas_sys_info_ind_msg_v01));
      if (qmi_error == QMI_NO_ERR)
      {
        /* Local domain and status variables */
        srv_domain_lte     = sys_info_ind.lte_sys_info.common_sys_info.srv_domain;
        srv_domain_hdr     = sys_info_ind.hdr_sys_info.common_sys_info.srv_domain;
        srv_domain_cdma    = sys_info_ind.cdma_sys_info.common_sys_info.srv_domain;
        srv_domain_wcdma   = sys_info_ind.wcdma_sys_info.common_sys_info.srv_domain;
        srv_domain_gsm     = sys_info_ind.gsm_sys_info.common_sys_info.srv_domain;
        srv_domain_tdscdma = sys_info_ind.tdscdma_sys_info.common_sys_info.srv_domain;
        srv_status_lte     = sys_info_ind.lte_srv_status_info.srv_status;
        srv_status_hdr     = sys_info_ind.hdr_srv_status_info.srv_status;
        srv_status_cdma    = sys_info_ind.cdma_srv_status_info.srv_status;
        srv_status_wcdma   = sys_info_ind.wcdma_srv_status_info.srv_status;
        srv_status_gsm     = sys_info_ind.gsm_srv_status_info.srv_status;
        srv_status_tdscdma = sys_info_ind.tdscdma_srv_status_info.srv_status;

        /* First Get the Service Domain. */
        /* If the LTE System Info is valid, check the LTE Service Domain. */
        if (sys_info_ind.lte_sys_info_valid == TRUE &&
            sys_info_ind.lte_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_lte == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_lte == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_lte == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.lte_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.lte_srv_status_info.srv_status;
          if (sys_info_ind.lte_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.lte_sys_info.common_sys_info.roam_status;
          }
          LOG_MSG_INFO1("qcmap_cm_connect_backhaul: lte Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the HDR System Info is valid, check the HDR Service Domain. */
        else if (sys_info_ind.hdr_sys_info_valid == TRUE &&
            sys_info_ind.hdr_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_hdr == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_hdr == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_hdr == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.hdr_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.hdr_srv_status_info.srv_status;
          if (sys_info_ind.hdr_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.hdr_sys_info.common_sys_info.roam_status;
          }
          LOG_MSG_INFO1("qcmap_cm_connect_backhaul: HDR Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the CDMA System Info is valid, check the CDMA Service Domain. */
        else if (sys_info_ind.cdma_sys_info_valid == TRUE &&
            sys_info_ind.cdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_cdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_cdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_cdma == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.cdma_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.cdma_srv_status_info.srv_status;
          if (sys_info_ind.cdma_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.cdma_sys_info.common_sys_info.roam_status;
          }
          LOG_MSG_INFO1("qcmap_cm_connect_backhaul: CDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the WCDMA System Info is valid, check the WCDMA Service Domain. */
        else if (sys_info_ind.wcdma_sys_info_valid == TRUE &&
            sys_info_ind.wcdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_wcdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_wcdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_wcdma == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.wcdma_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.wcdma_srv_status_info.srv_status;
          if (sys_info_ind.wcdma_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.wcdma_sys_info.common_sys_info.roam_status;
          }
          LOG_MSG_INFO1("qcmap_cm_connect_backhaul: WCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the GSM System Info is valid, check the GSM Service Domain. */
        else if (sys_info_ind.gsm_sys_info_valid == TRUE &&
            sys_info_ind.gsm_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_gsm == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_gsm == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_gsm == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.gsm_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.gsm_srv_status_info.srv_status;
          if (sys_info_ind.gsm_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.gsm_sys_info.common_sys_info.roam_status;
          }
          LOG_MSG_INFO1("qcmap_cm_connect_backhaul: GSM Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }
        /* If the TDSCDMA System Info is valid, check the TDSCDMA Service Domain. */
        else if (sys_info_ind.tdscdma_sys_info_valid == TRUE &&
            sys_info_ind.tdscdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
            srv_status_tdscdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
            (srv_domain_tdscdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_tdscdma == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          srv_domain = sys_info_ind.tdscdma_sys_info.common_sys_info.srv_domain;
          srv_status = sys_info_ind.tdscdma_srv_status_info.srv_status;
          if (sys_info_ind.tdscdma_sys_info.common_sys_info.roam_status_valid == TRUE)
          {
            roam_status = sys_info_ind.tdscdma_sys_info.common_sys_info.roam_status;
          }
          LOG_MSG_INFO1("qcmap_cm_connect_backhaul: TDSCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
        }

        if ((srv_status == NAS_SYS_SRV_STATUS_SRV_V01) &&
            (srv_domain == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain == SYS_SRV_DOMAIN_CS_PS_V01))
        {
          ds_system_call("echo QCMAP: Modem in service NAS indication received > /dev/kmsg",
                          strlen("echo QCMAP: Modem in service NAS indication received > /dev/kmsg"));

          QCMAP_Backhaul_WWAN::ConnectAllPDNs();
          old_roam_status = qcmap_cm_cb.roaming_indicator;
          if (roam_status != NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01)
          {
            qcmap_cm_cb.roaming_indicator = roam_status;
          }else
          {
            qcmap_cm_cb.roaming_indicator = 0;
          }
          /*check if Roaming status has changed*/
          if (old_roam_status != qcmap_cm_cb.roaming_indicator)
          {
            LOG_MSG_INFO1("WWAN Roaming status changed from %d to %d. Send event",
                          old_roam_status, qcmap_cm_cb.roaming_indicator, 0);
            event = QCMAP_CM_EVENT_WWAN_ROAMING_STATUS_CHANGED;
            qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, event,NULL,
                                          qcmap_cm_cb.qcmap_cm_callback_user_data, NULL, NULL);
          }
        }
      }
      else
      {
        LOG_MSG_INFO1("qcmap_cm_process_qmi_nas_ind: could not decode message %d", qmi_error, 0, 0);
      }
      break;
    case QMI_NAS_RF_BAND_INFO_IND_V01:
      LOG_MSG_INFO1("Receive QMI_NAS_RF_BAND_INFO_IND_V01\n",
                    0, 0, 0);
      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &band_info_ind,
                                            sizeof(nas_rf_band_info_ind_msg_v01));
      if(qmi_error != QMI_NO_ERR)
      {
        LOG_MSG_ERROR("Failed to decode qmi nas indication\n", 0, 0, 0);
        return;
      }

      target = ds_get_target();
      if (target == DS_TARGET_LE_MDM9x06)
      {
        if (band_info_ind.nas_rf_bandwidth_info_valid &&
            band_info_ind.nas_rf_bandwidth_info.bandwidth == NAS_LTE_BW_NRB_100_V01)
        {
          LOG_MSG_INFO1("CAT4 BW reported %d\n",
                        band_info_ind.nas_rf_bandwidth_info.bandwidth, 0, 0);
          QcMapMgr->dyn_clk_info.lte_cat4_bw = true;

          if ( QcMapBackhaulWWAN && (QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)) &&
               ((QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)||
               (QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)) &&
               (QcMapMgr->IsRatLteFdd()) &&
               !(QcMapMgr->CheckUsbClockAtNom()))
          {
            QcMapMgr->SetUSBClockFreq(true);
          }
        }
        else if (QcMapMgr->CheckUsbClockAtNom())
        {
          LOG_MSG_INFO1("NON CAT4 BW reported %d\n",
                       band_info_ind.nas_rf_bandwidth_info.bandwidth, 0, 0);
          QcMapMgr->dyn_clk_info.lte_cat4_bw = false;
          QcMapMgr->SetUSBClockFreq(false);
        }
      }
      break;
    default:
      /* Ignore all other indications */
      break;
   }

}

/*============================================================
  FUNCTION  qcmap_cm_process_qmi_service_availabilty
=============================================================
@brief
 Initiate QMI Client for the QMI service which is available

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None

/* ==========================================================*/
void qcmap_cm_process_qmi_service_availabilty
(
  qcmap_qmi_service_t  qmi_service_id
)
{
  switch (qmi_service_id)
  {
    case QCMAP_QMI_NAS_IN_SERVICE:
      if ( qmi_nas_client_init() == QCMAP_CM_ERROR )
      {
        LOG_MSG_ERROR("Not able to Init QMI NAS client",0, 0, 0);
      }
      break;

    case QCMAP_QMI_NAS_NOT_IN_SERVICE:
      if ( qmi_nas_not_in_service() == QCMAP_CM_ERROR )
      {
        LOG_MSG_ERROR("Not able to clean up QMI NAS context during SSR",0, 0, 0);
      }
      break;

    default:
     LOG_MSG_ERROR("Invalid QMI service =%d",qmi_service_id, 0, 0);
     break;
  }
  return;
}

/*===========================================================================
  FUNCTION  qcmap_cm_process_qmi_dsd_ind
===========================================================================*/
/*!
@brief
 Processes an incoming QMI DSDIndication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_qmi_dsd_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{
  qmi_error_type_v01 qmi_err_num;
  qmi_client_error_type qmi_error = QMI_NO_ERR;
  dsd_system_status_ind_msg_v01 system_status_ind;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  LOG_MSG_INFO1("qcmap_cm_process_qmi_dsd_ind: user_handle %d msg_id %d ind_buf_len %d.",
                user_handle,
                msg_id,
                ind_buf_len);

  memset ( &system_status_ind, 0, sizeof(dsd_system_status_ind_msg_v01));

  switch (msg_id)
  {
    case QMI_DSD_SYSTEM_STATUS_IND_V01:
      LOG_MSG_INFO1("Receive QMI_DSD_SYSTEM_STATUS_IND_V01\n",
                    0, 0, 0);
      qmi_error = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            &system_status_ind,
                                            sizeof(dsd_system_status_ind_msg_v01));
      if(qmi_error != QMI_NO_ERR)
      {
        LOG_MSG_ERROR("Failed to decode qmi dsd indication\n", 0, 0, 0);
        return;
      }
      if (system_status_ind.avail_sys_valid && system_status_ind.avail_sys_len > 0 &&
          system_status_ind.avail_sys[0].technology == DSD_SYS_NETWORK_3GPP_V01 &&
          system_status_ind.avail_sys[0].rat_value == DSD_SYS_RAT_EX_3GPP_LTE_V01 &&
          system_status_ind.avail_sys[0].so_mask == QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01)
      {
        LOG_MSG_INFO1("LTE FDD detected %x\n",
                      system_status_ind.avail_sys[0].so_mask , 0, 0);

        QcMapMgr->dyn_clk_info.rat_lte_fdd = true;

        if ( QcMapBackhaulWWAN && (QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)) &&
            ((QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)||
            (QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED)) &&
            (QcMapMgr->isBWLteCAT4()) &&
            !(QcMapMgr->CheckUsbClockAtNom()))
        {
          QcMapMgr->SetUSBClockFreq(true);
        }
      }
      else if (QcMapMgr->CheckUsbClockAtNom())
      {
        LOG_MSG_INFO1(" NO LTE FDD detected %x\n",
                      system_status_ind.avail_sys[0].so_mask , 0, 0);
        QcMapMgr->dyn_clk_info.rat_lte_fdd = false;
        QcMapMgr->SetUSBClockFreq(false);
      }
      break;
    default:
     /* Ignore all other indications */
    break;
  }

}

/*===========================================================================
  FUNCTION  qcmap_cm_process_timer_ind
===========================================================================*/
/*!
@brief
 Processes an incoming QMI timer Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_qmi_timer_ind
(
 unsigned int    msg_id,                         /* Indicator message ID  */
 uint32          user_data                      /* User data */
 )
{
  int qcmap_cm_errno,ret_val;
  qmi_error_type_v01 qmi_err_num;
  qcmap_cm_event_e  qcmap_event;
  QCMAP_Backhaul_WWAN *pBackhaulWWAN = (QCMAP_Backhaul_WWAN *)user_data;

  if (pBackhaulWWAN == NULL)
  {
     LOG_MSG_ERROR("QCMAP_Backhaul_WWAN is NULL in %s", __func__, 0, 0);
     return;
  }

  LOG_MSG_INFO1("qcmap_cm_process_qmi_timer_ind() msg=%d",msg_id,0,0);
  switch(msg_id)
  {
    case AUTO_CONNECT_V4:
      LOG_MSG_INFO1("Calling qcmap_cm_connect_backhaul has a part of auto connect for IPv4 call",0, 0, 0);

      if(pBackhaulWWAN->GetState() == QCMAP_CM_DISABLE || pBackhaulWWAN->auto_connect == false)
      {
        LOG_MSG_ERROR("%s: Autoconnect is disabled, stop v4 timer", __func__, 0, 0);
        if(pBackhaulWWAN->auto_connect_timer_running_v4)
          pBackhaulWWAN->StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V4_V01);
        return;
      }
      ret_val = pBackhaulWWAN->ConnectBackHaul(qcmap_cm_cb.handle,
                                        QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01, &qcmap_cm_errno, &qmi_err_num);
      if (ret_val == QCMAP_CM_SUCCESS)
      {
        LOG_MSG_INFO1("Call to Connect backhaul Success \n",0,0,0);
      }

      if (ret_val == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK)
      {
        LOG_MSG_INFO1("QCMAP WAN Connecting Inprogress \n",0,0,0);
      }
      else if( ret_val == QCMAP_CM_ERROR )
      {
        LOG_MSG_INFO1( "QCMAP WAN Connecting Fail, ret_val %d qcmap_cm_errno %d\n",
            ret_val, qcmap_cm_errno,0 );
      }
      break;

    case AUTO_CONNECT_V6:
      LOG_MSG_INFO1("Calling qcmap_cm_connect_backhaul has a part of auto connect for IPv6 call",0, 0, 0);

      if(pBackhaulWWAN->GetIPv6State() == QCMAP_CM_DISABLE || pBackhaulWWAN->auto_connect == false)
      {
        LOG_MSG_ERROR("%s: Autoconnect is disabled, stop v6 timer", __func__, 0, 0);
        if(pBackhaulWWAN->auto_connect_timer_running_v6)
          pBackhaulWWAN->StopAutoTimer(QCMAP_MSGR_IP_FAMILY_V6_V01);
        return;
      }

      ret_val = pBackhaulWWAN->ConnectBackHaul(qcmap_cm_cb.handle,
                                    QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01, &qcmap_cm_errno, &qmi_err_num);
      if (ret_val == QCMAP_CM_SUCCESS)
      {
        LOG_MSG_INFO1("Call to Connect backhaul Success \n",0,0,0);
      }

      if (ret_val == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK)
      {
        LOG_MSG_INFO1("QCMAP IPV6 WAN Connecting Inprogress \n",0,0,0);
      }
      else if( ret_val == QCMAP_CM_ERROR )
      {
        LOG_MSG_INFO1( "QCMAP IPV6 WAN Connecting Fail, ret_val %d qcmap_cm_errno %d\n",
            ret_val, qcmap_cm_errno,0 );
      }
      break;

    case STA_ASSOC_FAIL:
      {
         qcmap_event = QCMAP_CM_EVENT_STA_ASSOCIATION_FAIL;
         qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, qcmap_event, NULL,
                                qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);
      }
      break;

    case STA_DHCP_FAIL:
      {
        qcmap_event = QCMAP_CM_EVENT_STA_DHCP_IP_ASSIGNMENT_FAIL;
        qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, qcmap_event, NULL,
                                qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);
      }
      break;

    default:
      LOG_MSG_INFO1("Unknown Message id %d\n",msg_id,0,0);
  }

  return;
}

/*===========================================================================
  FUNCTION  qcmap_cm_process_sta_ind
===========================================================================*/
/*!
@brief
 Processes an incoming QMI STA Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_sta_ind
(
  uint32 sta_cookie,
  qcmap_sta_event_t event
)
{
  qcmap_cm_event_e  qcmap_event;

  LOG_MSG_INFO1("qcmap_cm_process_qmi_sta_ind: sta_cookie 0x%x event %d .",
                sta_cookie,
                event, 0);

  if ( sta_cookie != qcmap_cm_cb.sta_cookie )
  {
    LOG_MSG_ERROR("qcmap_cm_process_qmi_sta_ind: Cookie invalid. .", 0, 0, 0);
    return;
  }

  switch ( event )
  {
    case STA_CONNECTED:
      qcmap_event = QCMAP_CM_EVENT_STA_CONNECTED;
      break;
    case STA_DISCONNECTED:
      qcmap_event = QCMAP_CM_EVENT_STA_DISCONNECTED;
      break;
    default:
      LOG_MSG_ERROR("qcmap_cm_process_qmi_sta_ind: Unsupported STA Event : %d.", event, 0, 0);
      return;
  }

  qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, qcmap_event, NULL,
                                qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);


  return;
}

/*===========================================================================
  FUNCTION  qcmap_cm_process_cradle_ind
===========================================================================*/
/*!
@brief
 Processes an incoming QMI Cradle Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_cradle_ind
(
  qcmap_cradle_event_t event
)
{
  qcmap_cm_event_e  qcmap_event;

  switch ( event )
  {
    case CRADLE_CONNECTED:
      qcmap_event = QCMAP_CM_EVENT_CRADLE_CONNECTED;
      break;
    case CRADLE_DISCONNECTED:
      qcmap_event = QCMAP_CM_EVENT_CRADLE_DISCONNECTED;
      break;
    default:
      LOG_MSG_ERROR("qcmap_cm_process_qmi_cradle_ind: Unsupported Cradle Event : %d.", event, 0, 0);
      return;
  }

  qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, qcmap_event, NULL,
                                qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);


  return;
}


/*===========================================================================
  FUNCTION  qcmap_cm_process_eth_backhaul_ind
===========================================================================*/
/*!
@brief
 Processes an incoming ETH Backhaul Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_eth_backhaul_ind
(
  qcmap_eth_backhaul_event_t event
)
{
  qcmap_cm_event_e  qcmap_event;

  switch ( event )
  {
    case ETH_BACKHAUL_CONNECTED:
      qcmap_event = QCMAP_CM_EVENT_ETH_BACKHAUL_CONNECTED;
      break;
    case ETH_BACKHAUL_DISCONNECTED:
      qcmap_event = QCMAP_CM_EVENT_ETH_BACKHAUL_DISCONNECTED;
      break;
    default:
      LOG_MSG_ERROR("qcmap_cm_process_qmi_eth_backhaul_ind:"
                    "Unsupported ETH Backhaul Event : %d.", event, 0, 0);
      return;
  }

  qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, qcmap_event, NULL,
                                qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);


  return;
}

/*===========================================================================
  FUNCTION  qcmap_cm_process_bt_backhaul_ind
===========================================================================*/
/*!
@brief
 Processes an incoming BT Backhaul Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_bt_backhaul_ind
(
  qcmap_bt_backhaul_event_t event
)
{
  qcmap_cm_event_e  qcmap_event;

  switch ( event )
  {
    case BT_BACKHAUL_CONNECTED:
      qcmap_event = QCMAP_CM_EVENT_BT_BACKHAUL_CONNECTED;
      break;
    case BT_BACKHAUL_DISCONNECTED:
      qcmap_event = QCMAP_CM_EVENT_BT_BACKHAUL_DISCONNECTED;
      break;
    default:
      LOG_MSG_ERROR("qcmap_cm_process_qmi_btbackhaul_ind:"
                    "Unsupported BT Backhaul Event : %d.", event, 0, 0);
      return;
  }

  qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, qcmap_event, NULL,
                                qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);


  return;
}



/*===========================================================================
  FUNCTION  qcmap_cm_qmi_nas_ind
===========================================================================*/
/*!
@brief
 Receives an incoming QMI NAS Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
qcmap_cm_qmi_nas_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{
  qcmap_nas_buffer_t qcmap_nas_buffer;
  int numBytes=0, len;
  struct sockaddr_un nas_qcmap;
  void *buf;

  LOG_MSG_INFO1("qcmap_cm_qmi_nas_ind: user_handle %d msg_id %d ind_buf_len %d.",
                user_handle,
                msg_id,
                ind_buf_len);

  nas_qcmap.sun_family = AF_UNIX;
  strlcpy(nas_qcmap.sun_path, QCMAP_NAS_UDS_FILE, sizeof(QCMAP_NAS_UDS_FILE));
  len = strlen(nas_qcmap.sun_path) + sizeof(nas_qcmap.sun_family);

  buf = malloc(ind_buf_len);
  ds_assert(buf != NULL);

  qcmap_nas_buffer.user_handle = user_handle;
  qcmap_nas_buffer.msg_id = msg_id;
  memcpy(buf, ind_buf, ind_buf_len);
  qcmap_nas_buffer.ind_buf = buf;
  qcmap_nas_buffer.ind_buf_len = ind_buf_len;

  /* ind_cb_data is not used, if in use, make a copy here */
  qcmap_nas_buffer.ind_cb_data = ind_cb_data;


  if ((numBytes = sendto(nas_qcmap_sockfd, (void *)&qcmap_nas_buffer, sizeof(qcmap_nas_buffer_t), 0,
             (struct sockaddr *)&nas_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from nas_callback context", 0, 0, 0);
    free(buf);
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_ENOERROR;
}


/*===========================================================================
  FUNCTION  qcmap_cm_qmi_dsd_ind
===========================================================================*/
/*!
@brief
 Receives an incoming QMI DSDIndication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
qcmap_cm_qmi_dsd_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{
  qcmap_dsd_buffer_t qcmap_dsd_buffer;
  int numBytes=0, len;
  struct sockaddr_un dsd_qcmap;
  void *buf;

  LOG_MSG_INFO1("qcmap_cm_qmi_dsd_ind: user_handle %d msg_id %d ind_buf_len %d.",
                user_handle,
                msg_id,
                ind_buf_len);

  dsd_qcmap.sun_family = AF_UNIX;
  strlcpy(dsd_qcmap.sun_path, QCMAP_DSD_UDS_FILE, sizeof(QCMAP_DSD_UDS_FILE));
  len = strlen(dsd_qcmap.sun_path) + sizeof(dsd_qcmap.sun_family);

  buf = malloc(ind_buf_len);
  ds_assert(buf != NULL);

  qcmap_dsd_buffer.user_handle = user_handle;
  qcmap_dsd_buffer.msg_id = msg_id;
  memcpy(buf, ind_buf, ind_buf_len);
  qcmap_dsd_buffer.ind_buf = buf;
  qcmap_dsd_buffer.ind_buf_len = ind_buf_len;

  /* ind_cb_data is not used, if in use, make a copy here */
  qcmap_dsd_buffer.ind_cb_data = ind_cb_data;


  if ((numBytes = sendto(dsd_qcmap_sockfd, (void *)&qcmap_dsd_buffer, sizeof(qcmap_dsd_buffer_t), 0,
             (struct sockaddr *)&dsd_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from dsd_callback context", 0, 0, 0);
    free(buf);
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_ENOERROR;
}

/*===========================================================================
  FUNCTION  qcmap_cm_qmi_timer_ind
===========================================================================*/
/*!
@brief
 Receives an incoming QMI TIMER Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
qcmap_cm_qmi_timer_ind
(
  qcmap_timer_enum_t timer_id,                          /* Indicator message ID  */
  uint32             user_data                          /* User Data */
)
{
  qcmap_timer_buffer_t qcmap_timer_buffer;
  int numBytes=0, len;
  struct sockaddr_un timer_qcmap;

  timer_qcmap.sun_family = AF_UNIX;
  strlcpy(timer_qcmap.sun_path, QCMAP_TIMER_UDS_FILE, sizeof(QCMAP_TIMER_UDS_FILE));
  len = strlen(timer_qcmap.sun_path) + sizeof(timer_qcmap.sun_family);

  qcmap_timer_buffer.msg_id = timer_id;
  qcmap_timer_buffer.user_data = user_data;

  if ((numBytes = sendto(timer_qcmap_sockfd, (void *)&qcmap_timer_buffer, sizeof(qcmap_timer_buffer_t), 0,
          (struct sockaddr *)&timer_qcmap, len)) == -1)
  {
    LOG_MSG_ERROR("Send Failed from nas_callback context", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_ENOERROR;
}
/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION qcmap_edit_file
===========================================================================*/
/*!
@brief
 This function executes the command and copies console
 content to temporary file. Copies temporary file into
 the file provided.
 This function is kind of replacement for sed -i by
 doing cat and then cp.

@parameters
  command to be executed
  command length
  file path where the content has to be copied
  file path length

@return
  int - numeric value 0 on success, -1 otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int qcmap_edit_file(
  const char*         command_to_execute,
  unsigned int        cmdlen,
  const char*         file_path,
  unsigned int        pathlen
)
{
  char command[MAX_COMMAND_STR_LEN];
  int result = -1;
  unsigned int vallen = (unsigned int)strlen( command_to_execute );
  unsigned int filelen = (unsigned int)strlen( file_path );

  if( vallen != cmdlen ) {
    LOG_MSG_ERROR( "system call length mismatch: %d != %d", cmdlen, vallen, 0);
    return -1;
  }

  if( filelen != pathlen ) {
    LOG_MSG_ERROR( "file path length mismatch: %d != %d", pathlen, filelen,0);
    return -1;
  }

  snprintf(command, MAX_COMMAND_STR_LEN,"%s > %s", command_to_execute, TEMP_SED_INPLACE_FILE);
  result = ds_system_call(command, strlen(command));
  if (result != 0)
  {
    ds_system_call("rm " TEMP_SED_INPLACE_FILE, strlen("rm " TEMP_SED_INPLACE_FILE));
    LOG_MSG_INFO1("command failed: result %d", result, 0, 0);
    return result;
  }

  snprintf(command, MAX_COMMAND_STR_LEN,"cp %s %s", TEMP_SED_INPLACE_FILE, file_path);
  result = ds_system_call(command, strlen(command));
  ds_system_call("rm " TEMP_SED_INPLACE_FILE, strlen("rm " TEMP_SED_INPLACE_FILE));

  return result;
}

/*===========================================================================

FUNCTION QCMAP_CM_ENABLE()

DESCRIPTION

  Enable MobileAP CM based on the config.
  It will register MobileAP event callback.
  It will configure Modem in MobileAP Mode and bring up RmNet between Q6 and A5.
  It will also bring up LAN if it is config.

DEPENDENCIES
  None.

RETURN VALUE
  Returns MobileAP CM application ID on success.

  On error, return 0 and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_enable
(
  qcmap_cm_conf_t *qcmap_cm_cfg,                  /* Config for QCMAP CM   */
  qcmap_cm_cb_fcn  qcmap_cm_callback,             /* Callback function     */
  void            *qcmap_cm_callback_user_data,   /* Callback user data    */
  int             *qcmap_cm_errno,                /* Error condition value */
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_handle = 0;
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  boolean flag = false;

  LOG_MSG_INFO1("qcmap_cm_enable: enter",0,0,0);

  ds_assert(qcmap_cm_callback != NULL);
  ds_assert(qcmap_cm_cfg != NULL);
  ds_assert(qcmap_cm_errno != NULL);
  ds_assert(qmi_err_num != NULL);

  if (qcmap_cm_cb.disable_in_process)
  {
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    LOG_MSG_ERROR( "qcmap_cm_enable fail because disable still in progress", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_OPERATION_V01;
    return QCMAP_CM_ERROR;
  }

  qcmap_cm_handle = qcmap_cm_cb.handle;

  flag = false;
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;

    if ( ((pBackhaulWWAN->dsi_net_hndl.handle != NULL) &&
         (pBackhaulWWAN->GetState() > QCMAP_CM_DISABLE)) ||
         ((pBackhaulWWAN->ipv6_dsi_net_hndl.handle != NULL) &&
         (pBackhaulWWAN->GetIPv6State() > QCMAP_CM_V6_DISABLE)))
    {
      *qcmap_cm_errno = QCMAP_CM_ENOERROR;
      LOG_MSG_INFO1( "qcmap_cm_enable : already enabled",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      continue;
    }

    flag = true;
    memset( &(pBackhaulWWAN->dsi_net_hndl), 0x0, sizeof(pBackhaulWWAN->dsi_net_hndl) );
    memset( &(pBackhaulWWAN->ipv6_dsi_net_hndl), 0x0, sizeof(pBackhaulWWAN->ipv6_dsi_net_hndl) );

    pBackhaulWWAN->SetState(QCMAP_CM_ENABLE);
    pBackhaulWWAN->SetIPv6State(QCMAP_CM_V6_ENABLE);

    LOG_MSG_INFO1("Enable: STATES V4 %d V6 %d", pBackhaulWWAN->GetState(), pBackhaulWWAN->GetIPv6State(),0);
    qcmap_cm_handle = qcmap_cm_cb.handle;
  }

  if (flag == false)
  {
     *qcmap_cm_errno = QCMAP_CM_ENOERROR;
      LOG_MSG_INFO1( "qcmap_cm_enable : already enabled",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return qcmap_cm_cb.handle;
  }

  qcmap_cm_cb.qcmap_cm_callback = qcmap_cm_callback;
  qcmap_cm_cb.qcmap_cm_callback_user_data = qcmap_cm_callback_user_data;

#ifndef FEATURE_QTIMAP_OFFTARGET
#ifndef FEATURE_MOBILEAP_APQ_PLATFORM
    if (qcmap_cm_nas_init() == QCMAP_CM_ERROR)
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      LOG_MSG_ERROR("Unable to initialize QMI NAS service in QCMAP.",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      qcmap_cm_handle = 0;

      //DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
      START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
      while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
      {
         pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;

         pBackhaulWWAN->SetState(QCMAP_CM_DISABLE);
         pBackhaulWWAN->SetIPv6State(QCMAP_CM_V6_DISABLE);
      }
      return qcmap_cm_handle;
    }
#endif
#endif

  /* Initialize STA Cookie value. */
  qcmap_cm_cb.sta_cookie = QCMAP_STA_COOKIE;

  LOG_MSG_INFO1("QCMAP AP Handle 0x%04x Enabled", qcmap_cm_handle,0,0);
  qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle,
                                QCMAP_CM_EVENT_ENABLED, NULL,
                                qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);

  return qcmap_cm_handle;
}

/*===========================================================================

FUNCTION QCMAP_CM_DISABLE()

DESCRIPTION

  Disable MobileAP CM.
  It will teardown LAN.
  It will configure Modem in non-MobileAP mode.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_cm_errno Values
  ----------------
  QCMAP_CM_EBADAPP           invalid application ID specified

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_disable
(
  int  qcmap_cm_handle,                          /* Handle for MobileAP CM */
  int *qcmap_cm_errno,                           /* Error condition value  */
  qmi_error_type_v01 *qmi_err_num
)
{
  int ret = QCMAP_CM_SUCCESS;
  int ret_val;
  qmi_client_error_type qmi_error = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN *pBackhaulWWAN;
  boolean is_all_backhaul_down = TRUE;

  LOG_MSG_INFO1("qcmap_cm_disable: enter", 0, 0, 0);
  ds_assert(qcmap_cm_errno != NULL);

  if (qcmap_cm_handle != qcmap_cm_cb.handle)
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    LOG_MSG_INFO1( "Wrong QCMAP CM Handle",
                   0, 0, 0);
    return ret;
  }

  //This is used for IP passthrough for default pdn
  if(QcMapMgr->dont_bringdown_backhaul == true)
  {
    ds_system_call("echo QCMAP:WAN not disconnected > /dev/kmsg",strlen("echo QCMAP:WAN not disconnected > /dev/kmsg"));
    return true;
  }

  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    pBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ_AND_INC;

    if (pBackhaulWWAN->GetState() == QCMAP_CM_DISABLE &&
        pBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_DISABLE)
    {
      *qcmap_cm_errno = QCMAP_CM_EALDDISCONN;
      ret = QCMAP_CM_SUCCESS;
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      LOG_MSG_ERROR( "Already disconnected...",0,0,0);
      continue;
    }
    if(pBackhaulWWAN->GetState() > QCMAP_CM_ENABLE ||
        pBackhaulWWAN->GetIPv6State() > QCMAP_CM_V6_ENABLE)
    {
      qcmap_cm_cb.disable_in_process = TRUE;
      is_all_backhaul_down  = FALSE;

      if ( pBackhaulWWAN->GetState() > QCMAP_CM_ENABLE &&
           pBackhaulWWAN->GetState() !=  QCMAP_CM_WAN_DISCONNECTING)
      {
        ret_val = pBackhaulWWAN->DisconnectBackHaul(qcmap_cm_cb.handle,
                                                    QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01,
                                                    qcmap_cm_errno,
                                                    qmi_err_num);
        if (ret_val == QCMAP_CM_ERROR && *qcmap_cm_errno != QCMAP_CM_EWOULDBLOCK)
        {
          /* IPV4 Backhaul disconnecting failed. */
          LOG_MSG_ERROR("IPV4 backhaul disconnecting failed: Error %x", *qmi_err_num, 0, 0);
          return ret_val;
        }
      }
      if ( pBackhaulWWAN->GetIPv6State() > QCMAP_CM_V6_ENABLE &&
           pBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_DISCONNECTING)
      {
        ret_val = pBackhaulWWAN->DisconnectBackHaul(qcmap_cm_cb.handle,
                                                    QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01,
                                                    qcmap_cm_errno,
                                                    qmi_err_num);
        if (ret_val == QCMAP_CM_ERROR && *qcmap_cm_errno != QCMAP_CM_EWOULDBLOCK)
        {
          /* IPV6 Backhaul disconnecting failed. */
          LOG_MSG_ERROR("IPV6 backhaul disconnecting failed: Error %x", *qmi_err_num, 0, 0);
          return ret_val;
        }
      }
    }
    else if(pBackhaulWWAN->GetState() <= QCMAP_CM_ENABLE &&
            pBackhaulWWAN->GetIPv6State() <= QCMAP_CM_V6_ENABLE)
    {
       pBackhaulWWAN->SetState(QCMAP_CM_DISABLE);
       pBackhaulWWAN->SetIPv6State(QCMAP_CM_V6_DISABLE);
       LOG_MSG_INFO1("Disable: STATES V4 %d V6 %d", pBackhaulWWAN->GetState(),
                                                    pBackhaulWWAN->GetIPv6State(), 0);

       if (pBackhaulWWAN->ipv6_dsi_net_hndl.handle != NULL)
       {
         dsi_rel_data_srvc_hndl(pBackhaulWWAN->ipv6_dsi_net_hndl.handle);
         pBackhaulWWAN->ipv6_dsi_net_hndl.handle = NULL;
         memset(&(pBackhaulWWAN->ipv6_dsi_net_hndl), 0x0, sizeof(pBackhaulWWAN->ipv6_dsi_net_hndl));
       }

       if (pBackhaulWWAN->dsi_net_hndl.handle != NULL )
       {
         dsi_rel_data_srvc_hndl(pBackhaulWWAN->dsi_net_hndl.handle);
         pBackhaulWWAN->dsi_net_hndl.handle = NULL;
         memset(&(pBackhaulWWAN->dsi_net_hndl), 0x0, sizeof(pBackhaulWWAN->dsi_net_hndl));
       }
     }
   }

   if(is_all_backhaul_down == FALSE)
   {
     LOG_MSG_INFO1("qcmap_cm_disable(): wait for all backhaul to be down",0, 0, 0);
     return ret;
   }

   LOG_MSG_INFO1("qcmap_cm_disable(): All backhaul are down. cleanup", 0, 0, 0);
   if(qcmap_cm_cb.qmi_nas_notifier != NULL)
   {
     qmi_error = qmi_client_release(qcmap_cm_cb.qmi_nas_notifier);
     qcmap_cm_cb.qmi_nas_notifier = NULL;
   }

   if (qmi_error != QMI_NO_ERR)
   {
     LOG_MSG_ERROR("Can not release client nas notifier %d",
                 qmi_error, 0, 0);
   }
   qmi_error = qmi_client_release(qcmap_cm_cb.qmi_nas_handle);
   qcmap_cm_cb.qmi_nas_handle = NULL;

   if (qmi_error != QMI_NO_ERR)
   {
     LOG_MSG_ERROR("Can not release client nas handle %d",
                 qmi_error, 0, 0);
   }

   qmi_error = qmi_client_release(qcmap_cm_cb.qmi_wds_handle);
   qcmap_cm_cb.qmi_wds_handle = NULL;

   if (qmi_error != QMI_NO_ERR)
   {
     LOG_MSG_ERROR("Can not release client wds handle %d",
                 qmi_error, 0, 0);
   }

   qmi_error = qmi_client_release(qcmap_cm_cb.qmi_dsd_handle);
   qcmap_cm_cb.qmi_dsd_handle = NULL;

   if (qmi_error != QMI_NO_ERR)
   {
     LOG_MSG_ERROR("Can not release client dsd handle %d",
                   qmi_error, 0, 0);
   }

   qcmap_cm_cb.disable_in_process = false;
   qcmap_cm_cb.sta_cookie = 0;

   /* qmi_err_num is set to QCMAP_CM_DISCONNECTED to indicate clients that MobileAP is
   is disabled. This is being used for IoE 9x25. */
   *qcmap_cm_errno = QCMAP_CM_DISCONNECTED;

   qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle,
                               QCMAP_CM_EVENT_DISABLED, NULL,
                               qcmap_cm_cb.qcmap_cm_callback_user_data,NULL,NULL);

   LOG_MSG_INFO1("QCMAP AP Handle 0x%04x Disabled",
               qcmap_cm_handle, 0, 0);
   return ret;
}

/*===========================================================================
FUNCTION get_nas_config
===========================================================================
@brief
  gets nas configuration values

@input
  pBackhaulWWAN   - Reference to Backhaul WWAN Object
  qcmap_cm_handle - Handle for MobileAP CM
  qcmap_cm_errno  - Error condition value

@return
  0  - success
- 1 - failure

@dependencies
  It depends on inet_ntop()

@sideefects
None
=========================================================================*/
int get_nas_config
(
  QCMAP_Backhaul_WWAN  *pBackhaulWWAN,      /* Reference to Backhaul WWAN */
  int                   qcmap_cm_handle,    /* Handle for MobileAP CM */
  int                  *qcmap_cm_errno,     /* Error condition value  */
  qmi_error_type_v01   *qmi_err_num
)
{
  int ret = QCMAP_CM_SUCCESS;
  qmi_client_error_type qmi_error;
  char data[MAX_STRING_LENGTH];
  nas_get_sys_info_resp_msg_v01 get_sys_info_resp_msg;
  nas_roam_status_enum_type_v01 roam_status = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_roam_status_enum_type_v01 roam_status_hdr  = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_roam_status_enum_type_v01 roam_status_cdma = NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  qcmap_cm_event_e    event;
  uint8               old_roam_status;
  // check whether had backhaul service

  /* Call QMI NAS service to get system info. */
  nas_service_domain_enum_type_v01 srv_domain         = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_lte     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_hdr     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_cdma    = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_wcdma   = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_gsm     = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_domain_enum_type_v01 srv_domain_tdscdma = NAS_SERVICE_DOMAIN_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status         = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_lte     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_hdr     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_cdma    = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_wcdma   = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_gsm     = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;
  nas_service_status_enum_type_v01 srv_status_tdscdma = NAS_SERVICE_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01;

  memset(&get_sys_info_resp_msg, 0 , sizeof(nas_get_sys_info_resp_msg_v01));
  qmi_error = qmi_client_send_msg_sync( qcmap_cm_cb.qmi_nas_handle,
                                        QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
                                        NULL,
                                        0,
                                        &get_sys_info_resp_msg,
                                        sizeof(get_sys_info_resp_msg),
                                        QCMAP_CM_QMI_TIMEOUT_VALUE);
  if (qmi_error != QMI_NO_ERR)
  {
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    ret = QCMAP_CM_ERROR;
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR( "QCMAP CM nas get sys info req msg fail %d",
                    qmi_error,0,0);
    return ret;
  }
  else
  {
    /* Local domain and status variables */
    srv_domain_lte     = get_sys_info_resp_msg.lte_sys_info.common_sys_info.srv_domain;
    srv_domain_hdr     = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.srv_domain;
    srv_domain_cdma    = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.srv_domain;
    srv_domain_wcdma   = get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.srv_domain;
    srv_domain_gsm     = get_sys_info_resp_msg.gsm_sys_info.common_sys_info.srv_domain;
    srv_domain_tdscdma = get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.srv_domain;
    srv_status_lte     = get_sys_info_resp_msg.lte_srv_status_info.srv_status;
    srv_status_hdr     = get_sys_info_resp_msg.hdr_srv_status_info.srv_status;
    srv_status_cdma    = get_sys_info_resp_msg.cdma_srv_status_info.srv_status;
    srv_status_wcdma   = get_sys_info_resp_msg.wcdma_srv_status_info.srv_status;
    srv_status_gsm     = get_sys_info_resp_msg.gsm_srv_status_info.srv_status;
    srv_status_tdscdma = get_sys_info_resp_msg.tdscdma_srv_status_info.srv_status;
    /* First Get the Service Domain. */
    /* If the LTE System Info is valid, check the LTE Service Domain. */
    if (get_sys_info_resp_msg.lte_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.lte_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_lte == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_lte == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_lte == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.lte_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.lte_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.lte_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.lte_sys_info.common_sys_info.roam_status;
      }
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: lte Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the HDR System Info is valid, check the HDR Service Domain. */
    else if (get_sys_info_resp_msg.hdr_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.hdr_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_hdr == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_hdr == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_hdr == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.hdr_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.hdr_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.roam_status;
        roam_status_hdr = get_sys_info_resp_msg.hdr_sys_info.common_sys_info.roam_status;
      }
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: HDR Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the CDMA System Info is valid, check the CDMA Service Domain. */
    else if (get_sys_info_resp_msg.cdma_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.cdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_cdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_cdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_cdma == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.cdma_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.cdma_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.roam_status;
        roam_status_cdma = get_sys_info_resp_msg.cdma_sys_info.common_sys_info.roam_status;
      }
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: CDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the WCDMA System Info is valid, check the WCDMA Service Domain. */
    else if (get_sys_info_resp_msg.wcdma_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_wcdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_wcdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_wcdma == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.wcdma_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.wcdma_sys_info.common_sys_info.roam_status;
      }
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: WCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the GSM System Info is valid, check the GSM Service Domain. */
    else if (get_sys_info_resp_msg.gsm_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.gsm_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_gsm == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_gsm == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_gsm == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.gsm_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.gsm_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.gsm_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.gsm_sys_info.common_sys_info.roam_status;
      }
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: GSM Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }
    /* If the TDSCDMA System Info is valid, check the TDSCDMA Service Domain. */
    else if (get_sys_info_resp_msg.tdscdma_sys_info_valid == TRUE &&
        get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.srv_domain_valid == TRUE &&
        srv_status_tdscdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
        (srv_domain_tdscdma == SYS_SRV_DOMAIN_PS_ONLY_V01 || srv_domain_tdscdma == SYS_SRV_DOMAIN_CS_PS_V01))
    {
      srv_domain = get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.srv_domain;
      srv_status = get_sys_info_resp_msg.tdscdma_srv_status_info.srv_status;
      if (get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.roam_status_valid == TRUE)
      {
        roam_status = get_sys_info_resp_msg.tdscdma_sys_info.common_sys_info.roam_status;
      }
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: TDSCDMA Service Domain %d Status %d, roam status %d", srv_domain, srv_status, roam_status);
    }

    if ((srv_status != NAS_SYS_SRV_STATUS_SRV_V01) ||
        (srv_domain != SYS_SRV_DOMAIN_PS_ONLY_V01 && srv_domain != SYS_SRV_DOMAIN_CS_PS_V01))
    {
      *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
      ret = QCMAP_CM_ERROR;
      *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
      LOG_MSG_ERROR("QCMAP CM has no backhaul service",0,0,0);
      return ret;
    }

    /* QCMAP is on an active network. */
    pBackhaulWWAN->backhaul_service = TRUE;
    old_roam_status = qcmap_cm_cb.roaming_indicator;

    /*Do Not remove KPI Log*/
    ds_system_call("echo QCMAP: Modem in service NAS indication received > /dev/kmsg",
                    strlen("echo QCMAP: Modem in service NAS indication received > /dev/kmsg"));

    if(roam_status == NAS_ROAM_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01)
    {
      roam_status = 0;
    }
    qcmap_cm_cb.roaming_indicator = roam_status;

    /*check if Roaming status has changed*/
    if (old_roam_status != qcmap_cm_cb.roaming_indicator)
    {
      LOG_MSG_INFO1("WWAN Roaming status changed from %d to %d. Send event",
                     old_roam_status, qcmap_cm_cb.roaming_indicator, 0);
      event = QCMAP_CM_EVENT_WWAN_ROAMING_STATUS_CHANGED;
      qcmap_cm_cb.qcmap_cm_callback(qcmap_cm_cb.handle, event,NULL,
                                    qcmap_cm_cb.qcmap_cm_callback_user_data, NULL, NULL);
    }
  }

  if (roam_status >= NAS_SYS_ROAM_STATUS_ON_V01)
  {
    /* ...read the ERI config file for valid roam_status values. */
    QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ERI_CONFIG,GET_VALUE,data,MAX_STRING_LENGTH);
    if (qcmap_cm_eri_read_config(qcmap_cm_handle,data, qcmap_cm_errno) != QCMAP_CM_SUCCESS)
    {
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: qcmap_cm_eri_read_config: qcmap_cm_errno %d", qcmap_cm_errno,0,0);
    }
  }

  /* Check to see whether we can connect backhaul during roam. */
  /* If we're connected to a network...*/
  if (pBackhaulWWAN->backhaul_service)
  {
    /* If the config indicates roaming data calls not allowed and we're
       connected to a roaming network...*/
    char val[MAX_STRING_LENGTH] = {0};
    if (!(pBackhaulWWAN->GetRoaming())&&
        (roam_status >= NAS_SYS_ROAM_STATUS_ON_V01))
    {
      LOG_MSG_INFO1("qcmap_cm_connect_backhaul: qcmap_cm_cb.eri_roam_data_len %d", QCMAP_Backhaul_WWAN::eri_roam_data_len,0,0);
      ret = QCMAP_CM_ERROR;
      /* If ERI roam data, check if roaming status matches eri data. */
      if (QCMAP_Backhaul_WWAN::eri_roam_data_len > 0)
      {
        uint8 i = 0;

        /* Look to see if the current roaming status matched an eri data entry. */
        for (i = 0; i < QCMAP_Backhaul_WWAN::eri_roam_data_len; i++ )
        {
          if ( ( srv_status_hdr == NAS_SYS_SRV_STATUS_SRV_V01 &&
                 (srv_domain_hdr == SYS_SRV_DOMAIN_PS_ONLY_V01 ||
                 srv_domain_hdr == SYS_SRV_DOMAIN_CS_PS_V01) &&
                 roam_status_hdr == QCMAP_Backhaul_WWAN::eri_roam_data[i] ) ||
               ( srv_status_cdma == NAS_SYS_SRV_STATUS_SRV_V01 &&
                 (srv_domain_cdma == SYS_SRV_DOMAIN_PS_ONLY_V01 ||
                 srv_domain_hdr == SYS_SRV_DOMAIN_CS_PS_V01) &&
                 roam_status_cdma == QCMAP_Backhaul_WWAN::eri_roam_data[i]) )
          {
            /* We have a match, this is a romaing exception. */
            ret = QCMAP_CM_SUCCESS;
            break;
          }
        }
      }
      /* If a roaming exception was not found, error out. */
      if (ret == QCMAP_CM_ERROR)
      {
        *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
        LOG_MSG_ERROR("QCMAP CM can not connect backhaul while it is roaming",0,0,0);
        *qmi_err_num = QMI_ERR_NO_NETWORK_FOUND_V01;
        return ret;
      }
    }
  }
  return QCMAP_CM_SUCCESS;
}

/*===========================================================================

FUNCTION QCMAP_CM_ERI_READ_CONFIG()

DESCRIPTION
  This function reads the passed file name to store the ERI config for
  processing.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

=============================================================================*/
int
qcmap_cm_eri_read_config
(
  int    qcmap_cm_handle,                           /* Mobile AP CM Handle   */
  char  *file_name,                                 /* ERI config file       */
  int   *qcmap_cm_errno                             /* error condition value */
)
{
  int file_handle, ret;
  uint8 bytes_read = 0;
  struct stat stat_buf;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  ds_assert(file_name != NULL);
  ds_assert(qcmap_cm_errno != NULL);

  LOG_MSG_INFO1("qcmap_cm_eri_read_config: enter", 0, 0, 0);

  if (qcmap_cm_handle != qcmap_cm_cb.handle)
  {
    *qcmap_cm_errno = QCMAP_CM_EBADAPP;
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "Wrong QCMAP CM Handle", 0, 0, 0);
    return ret;
  }

  if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->GetState() == QCMAP_CM_DISABLE)
  {
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    ret = QCMAP_CM_ERROR;
    LOG_MSG_ERROR( "QCMAP CM disable state", 0, 0, 0);
    return ret;
  }


  file_handle = open(file_name, O_RDONLY);

  if (file_handle < 0)
  {
    *qcmap_cm_errno = errno;
    LOG_MSG_ERROR( "Couldn't open config file %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }

  if (fstat(file_handle, &stat_buf) < 0)
  {
    *qcmap_cm_errno = errno;
    LOG_MSG_ERROR( "Couldn't stat config file %d", errno, 0, 0);
    return QCMAP_CM_ERROR;
  }

  if (stat_buf.st_size > QCMAP_WAN_MAX_ERI_DATA_SIZE)
  {
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    LOG_MSG_ERROR( "Config file too big. size %d", stat_buf.st_size, 0, 0);
    return QCMAP_CM_ERROR;
  }

  do
  {
    ret = read(file_handle, &QCMAP_Backhaul_WWAN::eri_roam_data[bytes_read], (QCMAP_WAN_MAX_ERI_DATA_SIZE - bytes_read));
  } while (ret > 0 && (bytes_read += ret) < QCMAP_WAN_MAX_ERI_DATA_SIZE);

  close(file_handle);

  if (stat_buf.st_size != bytes_read)
  {
    *qcmap_cm_errno = QCMAP_CM_EOPNOTSUPP;
    LOG_MSG_ERROR( "eri_roam_data_len %d != bytes_read %d", stat_buf.st_size, bytes_read, 0);
    return QCMAP_CM_ERROR;
  }

  QCMAP_Backhaul_WWAN::eri_roam_data_len = (uint16)stat_buf.st_size;

  return QCMAP_CM_SUCCESS;
}

/*=====================================================
  FUNCTION qcmap_cm_check_ltefdd_cat4_bw
======================================================*/
/*!
@brief
  Check whether the current RAT is LTE in FDD mode and CAT4 BW (20Mhz)

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean qcmap_cm_check_ltefdd_cat4_bw()
{
  dsd_get_system_status_req_msg_v01 get_system_status_req_msg;
  dsd_get_system_status_resp_msg_v01 get_system_status_resp_msg;
  nas_get_rf_band_info_req_msg_v01 get_rf_band_info_req_msg;
  nas_get_rf_band_info_resp_msg_v01 get_rf_band_info_resp_msg;
  qmi_client_error_type qmi_error;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qmi_error_type_v01 qmi_err_num;
  uint32_t bw_info_len = 0, index =0;
  boolean is_LTE_in_FDDMode = false;
  boolean is_bw_cat4 = false;

  LOG_MSG_INFO1("qcmap_cm_check_ltefdd_cat4_bw\n",0,0,0);

  if ( QcMapMgr == NULL )
  {
    LOG_MSG_ERROR("ConnectionMgr Object is NULL\n", 0, 0, 0);
    return false;
  }

  /*Check whether LTE is in FDD mode*/
  qmi_error = qmi_client_send_msg_sync(qcmap_cm_cb.qmi_dsd_handle,
                                       QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                       &get_system_status_req_msg,
                                       sizeof(dsd_get_system_status_req_msg_v01),
                                       &get_system_status_resp_msg,
                                       sizeof(dsd_get_system_status_resp_msg_v01),
                                       QMI_MAX_TIMEOUT_MS);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_system_status_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get current RAT %d : %d",
        qmi_error, get_system_status_resp_msg.resp.error,0);
    qmi_err_num = get_system_status_resp_msg.resp.error;
    return false;
  }

  LOG_MSG_INFO1("avail_sys_valid %d, avail_sys_len %d\n",
                    get_system_status_resp_msg.avail_sys_valid, get_system_status_resp_msg.avail_sys[0].rat_value, 0);

  LOG_MSG_INFO1("avail_sys[0].technology %d, avail_sys[0].rat_value %d\n",
                    get_system_status_resp_msg.avail_sys[0].technology, get_system_status_resp_msg.avail_sys[0].rat_value, 0);


  if (get_system_status_resp_msg.avail_sys_valid &&
      get_system_status_resp_msg.avail_sys_len > 0 &&
      get_system_status_resp_msg.avail_sys[0].technology == DSD_SYS_NETWORK_3GPP_V01 &&
      get_system_status_resp_msg.avail_sys[0].rat_value == DSD_SYS_RAT_EX_3GPP_LTE_V01 &&
      get_system_status_resp_msg.avail_sys[0].so_mask == QMI_DSD_3GPP_SO_MASK_LTE_FDD_V01)
  {
    LOG_MSG_INFO1("LTE FDD detected %d\n",
                    get_system_status_resp_msg.avail_sys[0].so_mask , 0, 0);
    QcMapMgr->dyn_clk_info.rat_lte_fdd = true;
    is_LTE_in_FDDMode = true;
  }
  else
  {
    LOG_MSG_INFO1("NO LTE FDD detected %d\n",
                    get_system_status_resp_msg.avail_sys[0].so_mask , 0, 0);
    QcMapMgr->dyn_clk_info.rat_lte_fdd = false;
    return false;
  }

  qmi_error = qmi_client_send_msg_sync(qcmap_cm_cb.qmi_nas_handle,
                                       QMI_NAS_GET_RF_BAND_INFO_REQ_MSG_V01,
                                       &get_rf_band_info_req_msg,
                                       sizeof(nas_get_rf_band_info_req_msg_v01),
                                       &get_rf_band_info_resp_msg,
                                       sizeof(nas_get_rf_band_info_resp_msg_v01),
                                       QMI_MAX_TIMEOUT_MS);

  if ( ( qmi_error == QMI_TIMEOUT_ERR ) ||
       ( qmi_error != QMI_NO_ERR ) ||
       ( get_rf_band_info_resp_msg.resp.result != QMI_NO_ERR ) )
  {
    LOG_MSG_ERROR("Can not get current Cell BW %d : %d",
        qmi_error, get_rf_band_info_resp_msg.resp.error,0);
    qmi_err_num = get_rf_band_info_resp_msg.resp.error;
    return false;
  }

  QcMapMgr->dyn_clk_info.lte_cat4_bw = false;
  if ( get_rf_band_info_resp_msg.nas_rf_bandwidth_info_valid )
  {
    bw_info_len = get_rf_band_info_resp_msg.nas_rf_bandwidth_info_len;

    for ( index = 0; index < bw_info_len ; index++ )
    {
      LOG_MSG_INFO1("radio_if %d , bandwidth %d\n",
                    get_rf_band_info_resp_msg.nas_rf_bandwidth_info[index].radio_if,
                    get_rf_band_info_resp_msg.nas_rf_bandwidth_info[index].bandwidth, 0);

      if ( (get_rf_band_info_resp_msg.nas_rf_bandwidth_info[index].radio_if == NAS_RADIO_IF_LTE_V01) &&
           (get_rf_band_info_resp_msg.nas_rf_bandwidth_info[index].bandwidth == NAS_LTE_BW_NRB_100_V01))
      {
        LOG_MSG_INFO1("CAT4 BW reported %d\n",
                       get_rf_band_info_resp_msg.nas_rf_bandwidth_info[index].bandwidth, 0, 0);
        QcMapMgr->dyn_clk_info.lte_cat4_bw = true;
        is_bw_cat4 = true;
        break;
      }
    }
  }

  if ( is_LTE_in_FDDMode && is_bw_cat4 )
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*=====================================================
  FUNCTION qcmap_cm_get_wwan_roam_status
======================================================*/
/*!
@brief
  Gets the current value of Roaming status at QCMAP

@return
  boolean
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
uint8
qcmap_cm_get_wwan_roam_status()
{
  return qcmap_cm_cb.roaming_indicator;
}

