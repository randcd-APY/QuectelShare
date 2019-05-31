/*====================================================

FILE:  QCMAP_WLAN_SCM.cpp

SERVICES:
   QCMAP Connection Manager WLAN Specific Implementation to SCM

=====================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  02/19/16   tbw         Created

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_ConnectionManager.h"
#include "qti_wlan_scm_msgr_v01.h"
#include "QCMAP_GSB.h"

#include <sys/un.h>

bool QCMAP_WLAN_SCM::flag= false;
QCMAP_WLAN_SCM* QCMAP_WLAN_SCM::object=NULL;
extern unsigned int scm_qcmap_sockfd;

/*=====================================================
  FUNCTION UpdateWLANIFInfo
======================================================*/
/*!
@brief
 This is a utility function to update WLAN IF changes by SCM

@parameters
  - if number
  - state of If
  - type of WLAN if
  - wlan card type

@return
  void

@note

- Dependencies
   - WLAN should be initialized
   - Only available on Dual WIFI device

- Side Effects
- None
*/
/*=====================================================*/
static void UpdateWLANIFInfo(int if_num,
                             qcmap_msgr_wlan_iface_active_state_enum_v01 state,
                             qcmap_msgr_wlan_iface_index_enum_v01 ap_type,
                             qcmap_msgr_wlan_device_type_v01 dev_type)
{
  char cmd[MAX_CMD_LEN]  = {0};
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  snprintf(cmd, MAX_CMD_LEN,
           "if_num: %d, ap_Type: %d, card: %d, State: %d",
              if_num, ap_type, dev_type, state);
  LOG_MSG_INFO1("%s", cmd, 0, 0);

  if(!QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    SetGSBConfigforWLAN(if_num,state,ap_type,dev_type);
  }
  else
  {
    LOG_MSG_INFO1("No GSB cofig for AP+STA bridge mode",0 ,0 ,0);
  }

  if (NULL != QcMapWLANMgr)
  {
    if (QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01 == state)
    {
      QcMapWLANMgr->wlan_if_info[ap_type].wlan_ap_type = ap_type;
      snprintf(QcMapWLANMgr->wlan_if_info[ap_type].if_name,
                 QCMAP_MAX_IFACE_NAME_SIZE_V01, "wlan%d", if_num);
      QcMapWLANMgr->wlan_if_info[ap_type].wlan_dev_type = dev_type;
      if (QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01
              == QcMapWLANMgr->wlan_if_info[ap_type].state)
        QcMapWLANMgr->active_if_count++;
      QcMapWLANMgr->wlan_if_info[ap_type].state = state;
    }
    else
    {
      if(QcMapWLANMgr->active_if_count &&
           QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01
                == QcMapWLANMgr->wlan_if_info[ap_type].state)
      {
        /*only decrease active count if prev state was inactive*/
        QcMapWLANMgr->active_if_count--;
      }
      QcMapWLANMgr->wlan_if_info[ap_type].state = state;
    }
  }
  else
  {
    //should never be here
    LOG_MSG_ERROR("obj is null",0,0,0);
  }
}

/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and returns instance of class QCMAP_WLAN_SCM

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
QCMAP_WLAN_SCM* QCMAP_WLAN_SCM::Get_Instance(boolean obj_create)
{
  QCMAP_CM_LOG_FUNC_ENTRY();

  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : QCMAP_WLAN_SCM",0, 0, 0);
    object = new QCMAP_WLAN_SCM();
    flag = true;
    LOG_MSG_INFO1("created object", 0,0,0);
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

/*===========================================================================
  FUNCTION  scm_qmi_qcmap_ind
  ===========================================================================*/
/*!
  @brief
  Processes an incoming SCM Indication.

  @return
  void

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
void scm_qmi_qcmap_ind
(
 qmi_client_type user_handle,                    /* QMI user handle       */
 unsigned int    msg_id,                         /* Indicator message ID  */
 void           *ind_buf,                        /* Raw indication data   */
 unsigned int    ind_buf_len,                    /* Raw data length       */
 void           *ind_cb_data                     /* User call back handle */
 )
{
  qcmap_scm_buffer_t qcmap_scm_buffer;
  int numBytes=0, len;
  void *buf;
  struct sockaddr_un qcmap_un;

  LOG_MSG_INFO1("scm_qmi_qcmap_ind: user_handle %X msg_id %d ind_buf_len %d.",
          user_handle, msg_id, ind_buf_len);

  if(msg_id == QMI_SCM_MSGR_WLAN_IND_V01)
  {
    buf = malloc(ind_buf_len);
    ds_assert(buf != NULL);

    qcmap_scm_buffer.user_handle = user_handle;
    qcmap_scm_buffer.msg_id = msg_id;
    memcpy(buf, ind_buf, ind_buf_len);
    qcmap_scm_buffer.ind_buf = buf;
    qcmap_scm_buffer.ind_buf_len = ind_buf_len;

    /* ind_cb_data is not used, if in use, make a copy here */
    qcmap_scm_buffer.ind_cb_data = ind_cb_data;

    qcmap_un.sun_family = AF_UNIX;
    strlcpy(qcmap_un.sun_path, QCMAP_SCM_UDS_FILE, sizeof(QCMAP_SCM_UDS_FILE));
    len = strlen(qcmap_un.sun_path) + sizeof(qcmap_un.sun_family);

    if((numBytes = sendto(scm_qcmap_sockfd, (void *)&qcmap_scm_buffer, sizeof(qcmap_scm_buffer_t), 0,(struct sockaddr *)&qcmap_un, len)) == -1)
    {
      LOG_MSG_ERROR("Send Failed from QCMAP SCM thread context, errno:%d",errno, 0, 0);
      free(buf);
      buf = NULL;
      return;
    }
  }
  else
  {
    LOG_MSG_ERROR("Unknown MSG ID %d",msg_id,0,0);
  }

  return;
}

/*===========================================================================
  FUNCTION  scm_process_qmi_qcmap_ind
  ===========================================================================*/
/*!
  @brief
  Processes an incoming QMI QCMAP Indication.

  @parameters
  scm_msgr_wlan_ind_msg_v01 ind_data

  @return
  void

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
void QCMAP_WLAN_SCM::scm_process_qmi_qcmap_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(true);
  qmi_error_type_v01 qmi_err_num;
  qmi_client_error_type qmi_error;

  scm_msgr_wlan_ind_msg_v01 ind_data;
  qmi_error = qmi_client_message_decode(user_handle,
                                        QMI_IDL_INDICATION,
                                        msg_id,
                                        ind_buf,
                                        ind_buf_len,
                                        &ind_data,
                                        sizeof(scm_msgr_wlan_ind_msg_v01));
  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("scm_process_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error,0,0);
    return;
  }

  LOG_MSG_INFO1("Indication Type %d",ind_data.wlan_ind_type,0,0);
  /* Process SCM Indication Types*/
  switch (ind_data.wlan_ind_type)
  {
    case SCM_MSGR_WLAN_DYNAMIC_RECONFIG_IND_V01:
      if (ind_data.wlan_dyn_ind_valid)
      {
        LOG_MSG_INFO1("%d ifaces need to updated",ind_data.wlan_dyn_ind_len,0,0);

        for (int i=0; i < ind_data.wlan_dyn_ind_len; i++)
        {
          LOG_MSG_INFO1("Control message %d, type = %d, iface = %d", ind_data.wlan_dyn_ind[i].wlan_if_control,
                        ind_data.wlan_dyn_ind[i].wlan_if_type,ind_data.wlan_dyn_ind[i].wlan_if_num);
          LOG_MSG_INFO1("Control message %d, card type = %d", ind_data.wlan_dyn_ind[i].wlan_dev_type,0,0);
          if (ind_data.wlan_dyn_ind[i].wlan_if_control == SCM_MSGR_WLAN_IF_START_V01)
          {
            QcMapWLANSCMMgr->SCMProcessDynamicEnable(ind_data.wlan_dyn_ind[i].wlan_if_num,
                                                     ind_data.wlan_dyn_ind[i].wlan_if_type,
                                                     ind_data.wlan_dyn_ind[i].wlan_dev_type);
          }
          else if (ind_data.wlan_dyn_ind[i].wlan_if_control == SCM_MSGR_WLAN_IF_STOP_V01)
          {
            QcMapWLANSCMMgr->SCMProcessDynamicDisable(ind_data.wlan_dyn_ind[i].wlan_if_num,
                                                      ind_data.wlan_dyn_ind[i].wlan_if_type,
                                                      ind_data.wlan_dyn_ind[i].wlan_dev_type);
          }
          else
          {
            LOG_MSG_ERROR("Unknown control option %d",ind_data.wlan_dyn_ind[i].wlan_if_control,0,0);
          }
        }
        QCMAP_WLAN::UpdateWLANStatuswithSCMind(QCMAP_MSGR_SCM_DYNAMIC_RECONFIG_IND_V01);
      }
      else
        LOG_MSG_ERROR("WLAN_DYNAMIC_RECONFIG but flag is false",0,0,0);

      break;

    case SCM_MSGR_WLAN_STATION_STATE_IND_V01:
      if (ind_data.wlan_sta_ind_valid)
      {
        if(ind_data.wlan_sta_ind.wlan_sta_status == SCM_MSGR_WLAN_STATION_CONNECTED_V01)
        {
          /* In STA Reconfig there is a low chance of getting 2 CONNECT events in a row.
             if that happens we need to disable the old iface */
          if (QcMapBackhaulWLANMgr->sta_connected_event_received)
            QcMapBackhaulWLANMgr->DisableStaMode();

          /* Reset STA flags so Assoc/AddrAssign process for new access point */
          QcMapBackhaulWLANMgr->sta_connected_event_received  = false;
          QcMapBackhaulWLANMgr->sta_associated = false;
          QcMapBackhaulWLANMgr->sta_v4_available = false;
          QcMapBackhaulWLANMgr->sta_v6_available = false;

          QcMapMgr->sta_iface_index = ind_data.wlan_sta_ind.wlan_if_num;
          QcMapBackhaulWLANMgr->EnableStaMode();
          UpdateWLANIFInfo(ind_data.wlan_sta_ind.wlan_if_num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                           QCMAP_MSGR_WLAN_IFACE_STATION_V01,
                           (int)ind_data.wlan_sta_ind.wlan_dev_type);

          //send STA connect
          qcmap_cm_process_sta_ind (0xDCDCDCDC, STA_CONNECTED);
          //we may need to add a seperate cookie value for SCM
        }
        else if (ind_data.wlan_sta_ind.wlan_sta_status == SCM_MSGR_WLAN_STATION_DISCONNECTED_V01)
        {
          //send STA disconnect
          qcmap_cm_process_sta_ind (0xDCDCDCDC, STA_DISCONNECTED);
          if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
          {
            /*Increment STA disconnect event count*/
            this->sta_bridge_disc_evt_cnt++;
            /*Fallback to AP+STA Router mode if STA disconnect count exceeds
              or equal to max retry*/
            if (this->sta_bridge_disc_evt_cnt >= QCMAP_STA_BRDIGE_MAX_RETRY )
            {
              LOG_MSG_INFO1("STA Disconnect events in AP-STA bridge mode: %d\n",
                             this->sta_bridge_disc_evt_cnt, 0, 0);

              LOG_MSG_INFO1("Going back to AP+STA Router Mode since Association"
                            "failed in AP+STA Bridge mode.\n",0,0,0);
              if(QcMapMgr)
              {
                QcMapMgr->revert_ap_sta_router_mode = true;
                QcMapMgr->fallback_ap_sta_router_mode = true;
              }
              if(QCMAP_WLAN::ActivateWLAN(&qmi_err_num) == false)
              {
                LOG_MSG_ERROR("Activate WLAN Failed.\n",0,0,0);
              }
              return;
            }
          }
        }
        else
        {
          LOG_MSG_ERROR("Unknown status %d",ind_data.wlan_sta_ind.wlan_sta_status,0,0);
        }
        QCMAP_WLAN::UpdateWLANStatuswithSCMind(QCMAP_MSGR_SCM_STATION_STATE_IND_V01);

        if (QcMapWLANMgr && !QcMapWLANMgr->is_STA_GSB_conf)
        {
          /*Bind IF to GSB*/
          if ((QcMapWLANMgr->wifi_mode > QCMAP_MSGR_WLAN_MODE_AP_V01 &&
                                  QcMapWLANMgr->wifi_mode < QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01))
          {
            for (int ix = 0; ix < QCMAP_MAX_NUM_INTF; ix++)
            {
              if (QcMapWLANMgr->wlan_if_info[ix].state)
              {
                if (QcMapWLANMgr->wlan_if_info[ix].wlan_dev_type
                                    == QCMAP_MSGR_WLAN_DEV_TUF_V01 &&
                    QcMapWLANMgr->wlan_if_info[ix].wlan_ap_type
                                  == QCMAP_MSGR_WLAN_IFACE_STATION_V01)
                {
                  if (!QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
                  {
                    LOG_MSG_INFO1("setting GSB path for STA %s ",
                                    QcMapWLANMgr->wlan_if_info[ix].if_name, 0, 0);
                    QcMapWLANMgr->is_STA_GSB_conf = true;
                    QcMapWLANMgr->last_STA_GSB_IF = ind_data.wlan_sta_ind.wlan_if_num;
                  }
                  else
                  {
                    LOG_MSG_INFO1("No GSB cofig for AP+STA bridge mode",0 ,0 ,0);
                  }
                }
              }
            }
          }
        }

     /*GSB_TIMER_LOGIC
       Not using this for now and enable if needed later.*/
#if 0
        if (QcMapWLANMgr &&
            (QcMapWLANMgr->wifi_mode > QCMAP_MSGR_WLAN_MODE_AP_V01 &&
              QcMapWLANMgr->wifi_mode < QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01))
        {
          /*set timer for 30 sec, It takes more than 20 sec after station gets
              connected to get AP mode reconfigured*/
          setGSBTimer((GSB_TIMER_DEFAULT_VAL * GSB_TIMER_DEFAULT_MULTIPLIER), QCMAP_GSB::timerid);
        }
#endif
      }
      else
        LOG_MSG_ERROR("WLAN_DYNAMIC_RECONFIG but flag is false",0,0,0);
      break;

    case SCM_MSGR_WLAN_SYS_CONTROL_IND_V01:
      if (ind_data.wlan_sys_control_valid)
      {
        LOG_MSG_INFO1("Sys Control update %d",ind_data.wlan_sys_control,0,0);

        //The QCMAP_WLAN object does not need to be created for the priv client
        if (ind_data.wlan_sys_control == SCM_MSGR_WLAN_IF_START_V01)
        {
          //Start WLAN
          QCMAP_WLAN::EnableWLAN(&qmi_err_num,false,true);
        }
        else if (ind_data.wlan_sys_control == SCM_MSGR_WLAN_IF_STOP_V01)
        {
          //Stop WLAN
          QCMAP_WLAN::DisableWLAN(&qmi_err_num, true);
        }
        QCMAP_WLAN::UpdateWLANStatuswithSCMind(QCMAP_MSGR_SCM_SYS_CONTROL_IND_V01);

      }
      else
        LOG_MSG_ERROR("WLAN_DYNAMIC_RECONFIG but flag is false",0,0,0);
      break;

    default:
      LOG_MSG_ERROR("Unknown indication type %d",ind_data.wlan_ind_type,0,0);
      break;
  }
}

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes WLAN SCM variables.

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

QCMAP_WLAN_SCM::QCMAP_WLAN_SCM()
{
  qmi_client_error_type qmi_error;

  QCMAP_CM_LOG_FUNC_ENTRY();  //why isn't this showing in QXDM?

  this->qmi_scm_handle = 0;
  this->sta_bridge_disc_evt_cnt = 0;

  memset(&qmi_scm_os_params, 0, sizeof(qmi_client_os_params));
  /* Create the QCCI client */
  qmi_error = qmi_client_init_instance(scm_msgr_get_service_object_v01(),
                                QMI_CLIENT_INSTANCE_ANY,
                                scm_qmi_qcmap_ind,
                                NULL,
                                &qmi_scm_os_params,
                                QCMAP_CM_QMI_SCM_TIMEOUT_VALUE,
                                &this->qmi_scm_handle);

  LOG_MSG_INFO1("qmi_client_init: %d",
                 qmi_error,0,0);

  if (qmi_error != QMI_NO_ERR)
  {
    qmi_client_release(this->qmi_scm_notifier);
    this->qmi_scm_notifier = NULL;
    LOG_MSG_ERROR("Can not init SCM client %d",
        qmi_error,0,0);
    return;
  }

  //register for WLAN indications
  scm_msgr_wlan_setup_ind_register_req_msg_v01 ind_req;
  scm_msgr_wlan_setup_ind_register_resp_msg_v01 ind_resp;

  memset(&ind_req, 0, sizeof(scm_msgr_wlan_setup_ind_register_req_msg_v01));
  memset(&ind_resp, 0, sizeof(scm_msgr_wlan_setup_ind_register_resp_msg_v01));

  qcmap_cm_wlan_paths_conf_t wlan_paths;
  memset(&wlan_paths, 0, sizeof(qcmap_cm_wlan_paths_conf_t));

  /* If config paths are not set SCM will use default values, no need to fail */
  if (QCMAP_WLAN::ReadWLANConfigPathsFromXML(&wlan_paths))
  {
    LOG_MSG_INFO1("Setting config paths",0,0,0);

    /* Primary AP config */
    ind_req.wlan_cfg[0].wlan_if_type = SCM_MSGR_WLAN_IF_SAP_1_V01;
    strlcpy(ind_req.wlan_cfg[0].conf_file, wlan_paths.hostapd.path_to_hostapd_conf,
            SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);
    strlcpy(ind_req.wlan_cfg[0].entropy_file, wlan_paths.hostapd.path_to_hostapd_entropy,
            SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);

    if (wlan_paths.debug.hostapd_debug)
    {
      strlcpy(ind_req.wlan_cfg[0].extra_cmd_line_args, wlan_paths.debug.hostapd_dbg_cmd,
              SCM_MSGR_WLAN_CMD_LINE_ARG_MAX_V01);
      ds_log_med("Debug %s", ind_req.wlan_cfg[0].extra_cmd_line_args);
    }

    /* Guest AP config */
    ind_req.wlan_cfg[1].wlan_if_type = SCM_MSGR_WLAN_IF_SAP_2_V01;
    strlcpy(ind_req.wlan_cfg[1].conf_file, wlan_paths.hostapd_guest.path_to_hostapd_conf,
            SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);
    strlcpy(ind_req.wlan_cfg[1].entropy_file, wlan_paths.hostapd_guest.path_to_hostapd_entropy,
            SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);

    if (wlan_paths.debug.hostapd_guest_debug)
    {
      strlcpy(ind_req.wlan_cfg[1].extra_cmd_line_args, wlan_paths.debug.hostapd_guest_dbg_cmd,
              SCM_MSGR_WLAN_CMD_LINE_ARG_MAX_V01);
    }


    /* Guest AP 2 config */
    ind_req.wlan_cfg[2].wlan_if_type = SCM_MSGR_WLAN_IF_SAP_3_V01;
    strlcpy(ind_req.wlan_cfg[2].conf_file, wlan_paths.hostapd_guest_2.path_to_hostapd_conf,
            SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);
    strlcpy(ind_req.wlan_cfg[2].entropy_file, wlan_paths.hostapd_guest_2.path_to_hostapd_entropy,
            SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);

    if (wlan_paths.debug.hostapd_guest_2_debug)
    {
      strlcpy(ind_req.wlan_cfg[2].extra_cmd_line_args, wlan_paths.debug.hostapd_guest_2_dbg_cmd,
              SCM_MSGR_WLAN_CMD_LINE_ARG_MAX_V01);
    }

    /* STA iface config */
    ind_req.wlan_cfg[3].wlan_if_type = SCM_MSGR_WLAN_IF_STA_1_V01;
    strlcpy(ind_req.wlan_cfg[3].conf_file, wlan_paths.supplicant, SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);

    ds_log_med("Supp path %s", wlan_paths.supplicant);

    if (wlan_paths.debug.supplicant_debug)
    {
      strlcpy(ind_req.wlan_cfg[3].extra_cmd_line_args, wlan_paths.debug.supplicant_dbg_cmd,
              SCM_MSGR_WLAN_CMD_LINE_ARG_MAX_V01);
    }

    /* Internal STA Iface, used for CNE */
    ind_req.wlan_cfg[4].wlan_if_type = SCM_MSGR_WLAN_IF_STA_1_INT_V01;
    strlcpy(ind_req.wlan_cfg[4].conf_file, wlan_paths.supplicant, SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01);
    strlcpy(ind_req.wlan_cfg[4].extra_cmd_line_args, "-d", strlen("-d"));
  }

  ind_req.wlan_cfg_valid = true;
  ind_req.register_indication = 1;
  ind_req.wlan_cfg_len = 5;

  qmi_error = qmi_client_send_msg_sync(this->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_SETUP_IND_REG_REQ_V01,
                                       &ind_req,
                                       sizeof(scm_msgr_wlan_setup_ind_register_req_msg_v01),
                                       &ind_resp,
                                       sizeof(scm_msgr_wlan_setup_ind_register_resp_msg_v01),
                                       QCMAP_CM_QMI_SCM_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d", qmi_error,
               ind_resp.resp.result,0);

  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (ind_resp.resp.result != QMI_NO_ERR))
  {
    LOG_MSG_ERROR("Failed to enable ind %d : %d",  qmi_error,
               ind_resp.resp.error,0);
  }

  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroying the WLAN SCM Object.

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
/*=========================================================*/

QCMAP_WLAN_SCM::~QCMAP_WLAN_SCM()
{
  qmi_client_error_type qmi_error;

  QCMAP_CM_LOG_FUNC_ENTRY();

  flag=false;
  object=NULL;
  LOG_MSG_INFO1("Destroying Object: QCMAP_WLAN_SCM",0,0,0);

  qmi_error = qmi_client_release(this->qmi_scm_notifier);
  this->qmi_scm_notifier = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Can not release client scm notifier %d",qmi_error,0,0);
  }

  qmi_error = qmi_client_release(this->qmi_scm_handle);
  this->qmi_scm_handle = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Can not release client scm handle %d",
        qmi_error,0,0);
  }
}


/*=====================================================================
  FUNCTION SCMEnableWLAN
======================================================================*/
/*!
@brief
  Return true if SCM was able to enable WLAN in mode

@parameters
  int mode

@return
  true  - Enable WLAN succeeded
  false - Enable WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN_SCM::SCMEnableWLAN(int mode)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(true);

  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qmi_client_error_type qmi_error;
  scm_msgr_wlan_enable_req_msg_v01 req;
  scm_msgr_wlan_enable_resp_msg_v01 resp;
  int num, type, i;
  int dev_type = 0;
  QcMapWLANSCMMgr->sta_bridge_disc_evt_cnt = 0;

  if(QcMapWLANMgr == NULL)
  {
    LOG_MSG_ERROR("WLAN object is NULL", 0,0,0);
    return false;
  }

  memset(&req, 0, sizeof(scm_msgr_wlan_enable_req_msg_v01));
  memset(&resp, 0, sizeof(scm_msgr_wlan_enable_resp_msg_v01));

  LOG_MSG_INFO1("SCM Enable WLAN mode %d ", mode, 0,0);
  req.wlan_concurrency_cfg = mode;
  qmi_error = qmi_client_send_msg_sync(QcMapWLANSCMMgr->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_ENABLE_REQ_V01,
                                       &req,
                                       sizeof(scm_msgr_wlan_enable_req_msg_v01),
                                       &resp,
                                       sizeof(scm_msgr_wlan_enable_resp_msg_v01),
                                       QCMAP_CM_QMI_SCM_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d", qmi_error, resp.resp.result, 0);
  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (resp.resp.result != QMI_NO_ERR))
  {
    LOG_MSG_ERROR("Failed enable wlan %d : %d", qmi_error,resp.resp.error,0);
    return false;
  }

  LOG_MSG_INFO1("Number of new Wlan interfaces %d", resp.wlan_concurrency_cfg_len,0,0);
  for (i = 0; i < resp.wlan_concurrency_cfg_len; i++)
  {
    LOG_MSG_INFO1("WLAN IF Num: %d, Type: %d, card: %d",
                  resp.wlan_concurrency_cfg[i].wlan_if_num,
                  resp.wlan_concurrency_cfg[i].wlan_if_type,
                  resp.wlan_concurrency_cfg[i].wlan_dev_type);

    //need to store if_name correctly based on type
    num = resp.wlan_concurrency_cfg[i].wlan_if_num;
    type = resp.wlan_concurrency_cfg[i].wlan_if_type;
    dev_type =  resp.wlan_concurrency_cfg[i].wlan_dev_type;

    //store first AP num in QcMapMgr->ap_dev_num, all other AP's should immediately follow

    //Since QMI_SCM_MSGR_WLAN_IF_RECONFIG_REQ_V01 request taking ~5 sec .So there can
    // be possibility that within this time some client connects . At this time we can miss the
    // the device entry add . Added logic to manually query the connected client MAc address
    //.from hostapd

    switch (type)
    {
    case SCM_MSGR_WLAN_IF_SAP_1_V01:
      QcMapMgr->ap_dev_num1 = num;
      QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
      QcMapWLANMgr->ParseMacAndAddDeviceEntry(QCMAP_MSGR_PRIMARY_AP_V01);
      UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                       QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01,
                       dev_type);
      break;
    case SCM_MSGR_WLAN_IF_SAP_2_V01:
      QcMapMgr->ap_dev_num2 = num;
      QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
      QcMapWLANMgr->ParseMacAndAddDeviceEntry(QCMAP_MSGR_GUEST_AP_V01);
      UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                       QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01,
                       dev_type);
      break;
    case SCM_MSGR_WLAN_IF_SAP_3_V01:
      QcMapMgr->ap_dev_num3 = num;
      QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_2_V01);
      QcMapWLANMgr->ParseMacAndAddDeviceEntry(QCMAP_MSGR_GUEST_AP_2_V01);
      UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                       QCMAP_MSGR_WLAN_IFACE_GUEST_AP_TWO_V01,
                       dev_type);
      break;
    case SCM_MSGR_WLAN_IF_STA_1_V01:
    case SCM_MSGR_WLAN_IF_STA_1_INT_V01:
      QcMapMgr->sta_iface_index = num;
      UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                       QCMAP_MSGR_WLAN_IFACE_STATION_V01,
                       dev_type);
      break;
    default:
      LOG_MSG_ERROR("Unsupported WLAN iface type %d", resp.wlan_concurrency_cfg[i].wlan_if_type,0,0);
      break;
    }

    if (type == SCM_MSGR_WLAN_IF_SAP_1_V01 || type == SCM_MSGR_WLAN_IF_SAP_2_V01
         || type == SCM_MSGR_WLAN_IF_SAP_3_V01)
    {
      /* Add the interfaces to the bridge. */
      /* This action is done by SCM. So removing from QCMAP */
      //if(!QcMapWLANMgr->AssociateWLANtoBridge(num))
      //  LOG_MSG_ERROR("Failed to associate wlan%d to bridge", num,0,0);

      /* Config WLAN interface IP */
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[i].ll_ip_addr);
      strlcpy(a5_ip, inet_ntoa(addr), 16);
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg
                    .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
               num, a5_ip, netmask);
      ds_system_call(command, strlen(command));
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[i].ll_ip_addr
       & QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev wlan%d",
               subnet, netmask, num);
      ds_system_call(command, strlen(command));
      /* First delete the link-local route. */
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev wlan%d",num);
      ds_system_call(command, strlen(command));
    }
  }

  if(mode == SCM_MSGR_WLAN_CFG_1_SAP_1_STA_BRIDGE_MODE_V01 ||
     mode == SCM_MSGR_WLAN_CFG_2_SAP_1_STA_BRIDGE_MODE_V01)
    start_wlan_sta_timer(STA_ASSOC_FAIL);

  return true;
}

/*=====================================================================
  FUNCTION SCMDisableWLAN
======================================================================*/
/*!
@brief
  Return true if SCM was able to enable WLAN in mode

@parameters

@return
  true  - Enable WLAN succeeded
  false - Enable WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int QCMAP_WLAN_SCM::SCMDisableWLAN()
{
  qmi_client_error_type qmi_error;
  scm_msgr_wlan_disable_req_msg_v01 req;
  scm_msgr_wlan_disable_resp_msg_v01 resp;

  memset(&req, 0, sizeof(scm_msgr_wlan_disable_req_msg_v01));
  memset(&resp, 0, sizeof(scm_msgr_wlan_disable_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_DISABLE_REQ_V01,
                                       &req,
                                       sizeof(scm_msgr_wlan_disable_req_msg_v01),
                                       &resp,
                                       sizeof(scm_msgr_wlan_disable_resp_msg_v01),
                                       QCMAP_CM_QMI_SCM_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
          qmi_error, resp.resp.result,0);

  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (resp.resp.result != QMI_NO_ERR)) {
    LOG_MSG_ERROR("Failed to disable wlan %d : %d",  qmi_error,
            resp.resp.error,0);
    return false;
  }

  return true;
}


/*=====================================================================
  FUNCTION DualWifiEnabled
======================================================================*/
/*!
@brief
  Return true if Dual wifi mode is enabled

@parameters

@return
  true  - Dual Wifi enabled
  false - Legacy (Rome) solution

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN_SCM::DualWifiEnabled()
{
  FILE *stream = NULL, *device_type = NULL;
  char subtype = ' ', platform[PLATFORM_STRING_LENGTH] = {0};
  boolean ret = false;

#ifdef FEATURE_MOBILEAP_APQ_PLATFORM
  return false;
#endif

  stream = fopen("/sys/devices/soc0/platform_subtype_id", "rt");
  device_type = fopen("/sys/devices/soc0/hw_platform", "rt");

  if(NULL == stream || NULL == device_type)
  {
    if (NULL != stream)
    {
      LOG_MSG_INFO1("Failed to open platform",0,0,0);
      fclose(stream);
    }
    if (NULL != device_type)
    {
      LOG_MSG_INFO1("Failed to open CDT config",0,0,0);
      fclose(device_type);
    }
    return ret;
  }

  subtype = fgetc(stream);
  if (EOF != subtype)
  {
    LOG_MSG_INFO1("Sub type : %c", subtype, 0, 0);
    if (DS_TARGET_LE_CHIRON == ds_get_target())
    {
      if (fgets(platform, PLATFORM_STRING_LENGTH, device_type) != NULL)
      {
        LOG_MSG_INFO1("Platform type : %s subtype : %c", platform, subtype, 0);
        if (((strncmp(platform, PLATFORM_MTP, strlen(PLATFORM_MTP)) == 0) &&
              (CDT_DUAL_WIFI_MTP_CHIRON == atoi(&subtype))) ||
             ((strncmp(platform, PLATFORM_CDP, strlen(PLATFORM_CDP)) == 0) &&
              (CDT_DUAL_WIFI_CDP_CHIRON == atoi(&subtype))))
        {
          LOG_MSG_INFO1("Dual Wifi Enabled", 0, 0, 0);
          ret = true;
        }
      }
    }
    else
    {
      if(CDT_DUAL_WIFI == atoi(&subtype))
      {
        LOG_MSG_INFO1("Dual Wifi Enabled",0,0,0);
        ret = true;
      }
    }
  }

  fclose(device_type);
  fclose(stream);
  return ret;

}



/*=====================================================================
  FUNCTION SCMReconfig
======================================================================*/
/*!
@brief
  Return true if SCM was able to reconfigure to new mode

@parameters

@return
  true  - Reconfig WLAN succeeded
  false - Reconfig WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int QCMAP_WLAN_SCM::SCMReconfig
(
   int num_ifaces,
   scm_msgr_wlan_if_type_enum_v01 mode[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01],
   scm_msgr_wlan_if_control_enum_v01 control_option)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  scm_msgr_wlan_if_reconfig_req_msg_v01 req;
  scm_msgr_wlan_if_reconfig_resp_msg_v01 resp;
  qmi_client_error_type qmi_error;
  qcmap_cm_intf_index_type cfg_index;
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  int i, num = 0, type, old_num = -1;
  int dev_type = 0;
  char bridge_mac[ETH_ALEN];
  char ap_mac[ETH_ALEN];
  qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
  int active_if[QCMAP_MAX_NUM_INTF];

  LOG_MSG_INFO1("QCMAP_WLAN_SCM::SCMReconfig Enter",0,0,0);

  active_if[QCMAP_MSGR_INTF_AP_INDEX] = QcMapMgr->ap_dev_num1;
  active_if[QCMAP_MSGR_INTF_GUEST_AP_INDEX] = QcMapMgr->ap_dev_num2;
  active_if[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX] = QcMapMgr->ap_dev_num3;
  active_if[QCMAP_MSGR_INTF_STATION_INDEX] = QcMapMgr->sta_iface_index;

  memset(&req, 0, sizeof(scm_msgr_wlan_if_reconfig_req_msg_v01));
  memset(&resp, 0, sizeof(scm_msgr_wlan_if_reconfig_resp_msg_v01));

  req.wlan_if_type_len = num_ifaces;
  req.wlan_if_control_len = num_ifaces;
  for (int i=0; i < num_ifaces; i++)
  {
    req.wlan_if_type[i] = mode[i];
    req.wlan_if_control[i] = control_option;

    switch (mode[i])
    {
    case SCM_MSGR_WLAN_IF_SAP_1_V01:
      if (control_option == SCM_MSGR_WLAN_IF_STOP_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
      {
        LOG_MSG_INFO1("QCMAP_WLAN_SCM:: Stopping HostAPD for SAP_1,Iface wlan%d",
                      QcMapMgr->ap_dev_num1,0,0);
        QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
        active_if[QCMAP_MSGR_INTF_AP_INDEX] = QCMAP_LAN_INVALID_IFACE_INDEX;
        UpdateWLANIFInfo(QcMapMgr->ap_dev_num1, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                         QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01,
                         dev_type);
      }
      break;

    case SCM_MSGR_WLAN_IF_SAP_2_V01:
      if (control_option == SCM_MSGR_WLAN_IF_STOP_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
      {
        LOG_MSG_INFO1("QCMAP_WLAN_SCM:: Stopping HostAPD for SAP_2,Iface wlan%d",
                      QcMapMgr->ap_dev_num2,0,0);
        QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
        active_if[QCMAP_MSGR_INTF_GUEST_AP_INDEX] = QCMAP_LAN_INVALID_IFACE_INDEX;
        UpdateWLANIFInfo(QcMapMgr->ap_dev_num2, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                         QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01,
                         dev_type);
      }
      break;

    case SCM_MSGR_WLAN_IF_SAP_3_V01:
      if (control_option == SCM_MSGR_WLAN_IF_STOP_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
      {
        LOG_MSG_INFO1("QCMAP_WLAN_SCM:: Stopping HostAPD for SAP_3,Iface wlan%d",
                      QcMapMgr->ap_dev_num3,0,0);
        QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_GUEST_AP_2_V01);
        active_if[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX] = QCMAP_LAN_INVALID_IFACE_INDEX;
        UpdateWLANIFInfo(QcMapMgr->ap_dev_num3, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                         QCMAP_MSGR_WLAN_IFACE_GUEST_AP_TWO_V01,
                         dev_type);
      }
      break;
    }
  }

  qmi_error = qmi_client_send_msg_sync(this->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_IF_RECONFIG_REQ_V01,
                                       &req,
                                       sizeof(scm_msgr_wlan_if_reconfig_req_msg_v01),
                                       &resp,
                                       sizeof(scm_msgr_wlan_if_reconfig_resp_msg_v01),
                                       QCMAP_CM_QMI_SCM_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
          qmi_error, resp.resp.result,0);

  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (resp.resp.result != QMI_NO_ERR)) {
    LOG_MSG_ERROR("Failed WLAN IF recfg %d : %d",qmi_error,resp.resp.error,0);
    return false;
  }

  LOG_MSG_INFO1("Number of new Wlan interfaces %d", resp.wlan_concurrency_cfg_len,0,0);
  for (i = 0; i < resp.wlan_concurrency_cfg_len; i++)
  {
    LOG_MSG_INFO1("%d %d", resp.wlan_concurrency_cfg_len,i,0);
    LOG_MSG_INFO1("WLAN IF Num: %d, Type: %d, card: %d",
                  resp.wlan_concurrency_cfg[i].wlan_if_num,
                  resp.wlan_concurrency_cfg[i].wlan_if_type,
                  resp.wlan_concurrency_cfg[i].wlan_dev_type);

    //need to store if_name correctly based on type
    num = resp.wlan_concurrency_cfg[i].wlan_if_num;
    type = resp.wlan_concurrency_cfg[i].wlan_if_type;
    dev_type = resp.wlan_concurrency_cfg[i].wlan_dev_type;

    //store first AP num in QcMapMgr->ap_dev_num, all other AP's should immediately follow

    //Since QMI_SCM_MSGR_WLAN_IF_RECONFIG_REQ_V01 request taking ~5 sec .So there can
    // be possibility that within this time some client connects . At this time we can miss the
    // the device entry add . Added logic to manually query the connected client MAc address
    //.from hostapd
    switch (type)
    {
    case SCM_MSGR_WLAN_IF_SAP_1_V01:
      old_num = QcMapMgr->ap_dev_num1;
      QcMapMgr->ap_dev_num1 = num;
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;

      if (control_option == SCM_MSGR_WLAN_IF_START_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
      {
        LOG_MSG_INFO1("StartHostapdCli on SAP_1 old_Iface:%d, new_Iface:%d",
                      old_num, num,0);
        QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
        active_if[QCMAP_MSGR_INTF_AP_INDEX] = QcMapMgr->ap_dev_num1;
        QcMapWLANMgr->ParseMacAndAddDeviceEntry(QCMAP_MSGR_PRIMARY_AP_V01);
        UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                         QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01,
                         dev_type);
      }
      break;

    case SCM_MSGR_WLAN_IF_SAP_2_V01:
      old_num = QcMapMgr->ap_dev_num2;
      QcMapMgr->ap_dev_num2 = num;
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;

      if (control_option == SCM_MSGR_WLAN_IF_START_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
      {
        LOG_MSG_INFO1("StartHostapdCli on SAP_2 old_Iface:%d, new_Iface:%d",
                      old_num, num,0);
        QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
        active_if[QCMAP_MSGR_INTF_GUEST_AP_INDEX] = QcMapMgr->ap_dev_num2;
        QcMapWLANMgr->ParseMacAndAddDeviceEntry(QCMAP_MSGR_GUEST_AP_V01);
        UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                         QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01,
                         dev_type);
      }
      break;

    case SCM_MSGR_WLAN_IF_SAP_3_V01:
      old_num = QcMapMgr->ap_dev_num3;
      QcMapMgr->ap_dev_num3 = num;

      if (control_option == SCM_MSGR_WLAN_IF_START_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
      {
        LOG_MSG_INFO1("StartHostapdCli on SAP_3 old_Iface:%d, new_Iface:%d",
                      old_num, num,0);
        QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_2_V01);
        active_if[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX] = QcMapMgr->ap_dev_num3;
        QcMapWLANMgr->ParseMacAndAddDeviceEntry(QCMAP_MSGR_GUEST_AP_2_V01);
        UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                         QCMAP_MSGR_WLAN_IFACE_GUEST_AP_TWO_V01,
                         dev_type);
      }
      break;

    case SCM_MSGR_WLAN_IF_STA_1_INT_V01:
      /* For internal sta we never bring up the iface, used for CNE */
      QcMapMgr->sta_iface_index = num;
      active_if[QCMAP_MSGR_INTF_STATION_INDEX] = QcMapMgr->sta_iface_index;
      UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                       QCMAP_MSGR_WLAN_IFACE_STATION_V01,
                       dev_type);
      break;

    case SCM_MSGR_WLAN_IF_STA_1_V01:
      /* Restart should be the only supported mode for STA */
      if (control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
      {
        if(!QcMapBackhaulWLANMgr)
        {
          LOG_MSG_ERROR("QcMapBackhaulWLANMgr does not exist but STA restart attempted",0,0,0);
          return false;
        }
        QcMapBackhaulWLANMgr->DisableStaMode();
        UpdateWLANIFInfo(num, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                         QCMAP_MSGR_WLAN_IFACE_STATION_V01,
                         dev_type);
        //reset the revert_ap_sta_router_mode flag
        QcMapMgr->revert_ap_sta_router_mode = false;
        LOG_MSG_INFO1("Reset revert_ap_sta_router_mode flag",0,0,0);

        /* Reset STA V4 available flag */
        QcMapBackhaulWLANMgr->sta_v4_available = false;

        /* Reset STA V6 available flag */
        QcMapBackhaulWLANMgr->sta_v6_available = false;

      }
      break;

    default:
      LOG_MSG_ERROR("Unsupported WLAN iface type %d", resp.wlan_concurrency_cfg[i].wlan_if_type,0,0);
      break;
    }

    //Config the new iface
    if (type == SCM_MSGR_WLAN_IF_SAP_1_V01 || type == SCM_MSGR_WLAN_IF_SAP_2_V01 ||
	 	type == SCM_MSGR_WLAN_IF_SAP_3_V01 )
    {
      /* If the iface changed clean up the old one, add the new one to the bridge */
      if (old_num != num)
      {
        /* Remove the iface from the bridge and delete the IP
           We can not bring down iface as SCM might need it up */
        LOG_MSG_INFO1("Active IF SAP_1 %d, SAP_2 %d, SAP_3 %d",
                      active_if[QCMAP_MSGR_INTF_AP_INDEX],
                      active_if[QCMAP_MSGR_INTF_GUEST_AP_INDEX],
                      active_if[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX]);
        if (old_num != active_if[QCMAP_MSGR_INTF_AP_INDEX] &&
            old_num != active_if[QCMAP_MSGR_INTF_GUEST_AP_INDEX] &&
            old_num != active_if[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX])
        {
          snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
                   BRIDGE_IFACE, old_num);
          ds_system_call(command, strlen(command));
          QcMapMgr->unused_wlan_if = old_num;
        }
        /* Add the new interface to the bridge. */
        if(!QcMapWLANMgr->AssociateWLANtoBridge(num))
          LOG_MSG_ERROR("Failed to associate wlan%d to bridge", num,0,0);

        /* If in bridge mode move the ebtables to the new AP iface */
        if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
        {
          if(!QcMapBackhaulWLANMgr)
          {
            LOG_MSG_ERROR("QcMapBackhaulWLANMgr does not exist but STA restart attempted",0,0,0);
            return false;
          }

          memcpy(bridge_mac, QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac, ETH_ALEN);
          memcpy(ap_mac, QcMapBackhaulWLANMgr->ap_sta_bridge.ap_mac, ETH_ALEN);

          /* Remove MAC_SNAT rule for 3-addr scheme on old iface and add new iface */
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "ebtables -t nat -D POSTROUTING -o wlan%d -j snat --to-source %02x:%02x:%02x:%02x:%02x:%02x",
                   old_num, ap_mac[0],ap_mac[1],ap_mac[2],ap_mac[3], ap_mac[4], ap_mac[5]);
          ds_system_call(command, strlen(command));
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "ebtables -t nat -A POSTROUTING -o wlan%d -j snat --to-source %02x:%02x:%02x:%02x:%02x:%02x",
                   num, ap_mac[0],ap_mac[1],ap_mac[2],ap_mac[3], ap_mac[4], ap_mac[5]);
          ds_system_call(command, strlen(command));

          /* Default Subnet Based ARP Reply Rules for WLAN Interface*/
          char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
          strlcpy(netmask, inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_netmask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "ebtables -t nat -D PREROUTING -i wlan%d -p arp --arp-ip-src=%s/%s --arp-opcode Request -j"\
                   " arpreply --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x --arpreply-target ACCEPT",
                   old_num, inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw), netmask,
                   bridge_mac[0],bridge_mac[1],bridge_mac[2],bridge_mac[3],bridge_mac[4],bridge_mac[5]);
          ds_system_call(command, strlen(command));
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "ebtables -t nat -A PREROUTING -i wlan%d -p arp --arp-ip-src=%s/%s --arp-opcode Request -j" \
                   " arpreply --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x --arpreply-target ACCEPT",
                   num, inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw), netmask,
                   bridge_mac[0],bridge_mac[1],bridge_mac[2],bridge_mac[3],bridge_mac[4],bridge_mac[5]);
          ds_system_call(command, strlen(command));
        }
      }

      if (type == SCM_MSGR_WLAN_IF_SAP_2_V01)
        cfg_index = QCMAP_MSGR_INTF_GUEST_AP_INDEX;
      else if (type == SCM_MSGR_WLAN_IF_SAP_3_V01)
        cfg_index = QCMAP_MSGR_INTF_GUEST_AP_2_INDEX;
      else
        cfg_index = QCMAP_MSGR_INTF_AP_INDEX;

      /* Config WLAN interface IP */
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[cfg_index].ll_ip_addr);
      strlcpy(a5_ip, inet_ntoa(addr), 16);
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg
                    .interface[cfg_index].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
               num, a5_ip, netmask);
      ds_system_call(command, strlen(command));
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[cfg_index].ll_ip_addr
       & QcMapWLANMgr->wlan_cfg.interface[cfg_index].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev wlan%d",
               subnet, netmask, num);
      ds_system_call(command, strlen(command));
      /* First delete the link-local route. */
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev wlan%d",num);
      ds_system_call(command, strlen(command));
    }

      /* Set the bridge mode if cfg is set */
  if (QcMapBackhaulWLANMgr && QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode &&
      QcMapMgr->revert_ap_sta_router_mode == false &&
      QcMapMgr->fallback_ap_sta_router_mode == false)
    QcMapBackhaulWLANMgr->sta_bridge_mode_activated = true;

    /* Install access restrictions for the Second and third SSID if the profile is
    INTERNETONLY.*/
    if ((!QcMapBackhaulWLANMgr ||
            (QcMapBackhaulWLANMgr &&
             !QcMapBackhaulWLANMgr->sta_bridge_mode_activated))
        && (!QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated()))
    {
      QcMapWLANMgr->UpdateAccessProfileRules();
    }
  }

  LOG_MSG_INFO1("QCMAP_WLAN_SCM::SCMReconfig EXIT ",0,0,0);
  return true;
}


/*=====================================================================
  FUNCTION SCMProcessDynamicEnable
======================================================================*/
/*!
@brief
  Process the indication data for Enable iface

@parameters

@return
  true  - Reconfig WLAN succeeded
  false - Reconfig WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN_SCM::SCMProcessDynamicEnable(int iface_num, int type, int dev_type)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  int index = 0;
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

  LOG_MSG_INFO1("SCMProcessDynamicEnable enter for wlan%d, %d",iface_num,type,0);
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() && QcMapMgr &&
      (QcMapMgr->revert_ap_sta_router_mode == true))
  {
    LOG_MSG_ERROR("Ignore the SCM dynamic enable indication since fallback to"
                   "router mode is in progress.\n",0,0,0);
    return;
  }

  /* Enable IP forwarding */
  ds_system_call("echo 1 > /proc/sys/net/ipv4/ip_forward",
                 strlen("echo 1 > /proc/sys/net/ipv4/ip_forward")); //may not be needed...***********************

  //store first AP num in QcMapMgr->ap_dev_num, all other AP's should immediately follow
  //Check with Mani, STA should either be before all AP's or after, otherwise all logic needs to change
  switch (type)
  {
  case SCM_MSGR_WLAN_IF_SAP_1_V01:
    QcMapMgr->ap_dev_num1 = iface_num;
    LOG_MSG_INFO1("Dynamic StartHostAPD for SAP_%d with Iface wlan%d", type,iface_num,0);
    QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
    UpdateWLANIFInfo(iface_num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                     QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01,
                     dev_type);
    /* Primary AP index will always be 0 in wlan_cfg.interface[] */
    index = 0;
    break;
  case SCM_MSGR_WLAN_IF_SAP_2_V01:
    LOG_MSG_INFO1("Dynamic StartHostAPD for SAP_%d with Iface wlan%d", type,iface_num,0);
    QcMapMgr->ap_dev_num2 = iface_num;
    QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
    UpdateWLANIFInfo(iface_num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                     QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01,
                     dev_type);

    /* Guest AP index will always be 1 in wlan_cfg.interface[] */
    index = 1;
    break;

  case SCM_MSGR_WLAN_IF_SAP_3_V01:
    LOG_MSG_INFO1("Dynamic StartHostAPD for SAP_%d with Iface wlan%d", type,iface_num,0);
    QcMapMgr->ap_dev_num3 = iface_num;
    QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_2_V01);
    UpdateWLANIFInfo(iface_num, QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01,
                     QCMAP_MSGR_WLAN_IFACE_GUEST_AP_TWO_V01,
                     dev_type);

    /* Guest AP 2 index will always be 2 in wlan_cfg.interface[] */
    index = 2;
    break;

  case SCM_MSGR_WLAN_IF_STA_1_V01:
  case SCM_MSGR_WLAN_IF_STA_1_INT_V01:
    QcMapMgr->sta_iface_index = iface_num;
    break;
  default:
    LOG_MSG_ERROR("Unsupported WLAN iface type %d", type,0,0);
    return;
  }

  if (type == SCM_MSGR_WLAN_IF_SAP_1_V01 || type == SCM_MSGR_WLAN_IF_SAP_2_V01 ||
      type == SCM_MSGR_WLAN_IF_SAP_3_V01)
  {
    /* Add the interfaces to the bridge. */
    if(!QcMapWLANMgr->AssociateWLANtoBridge(iface_num))
      LOG_MSG_ERROR("Failed to associate wlan%d to bridge", iface_num,0,0);

    /* Config WLAN interface IP */
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[index].ll_ip_addr);
    strlcpy(a5_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg
                  .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
             iface_num, a5_ip, netmask);
    ds_system_call(command, strlen(command));
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[index].ll_ip_addr
     & QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
    strlcpy(subnet, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev wlan%d",
             subnet, netmask, iface_num);
    ds_system_call(command, strlen(command));
    /* First delete the link-local route. */
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev wlan%d",iface_num);
    ds_system_call(command, strlen(command));
  }

  if (type == SCM_MSGR_WLAN_IF_STA_1_V01)
  {
    /* Set the bridge mode if cfg is set */
    if (QcMapBackhaulWLANMgr && QcMapBackhaulWLANMgr->apsta_cfg.sta_bridge_mode &&
        (QcMapMgr->revert_ap_sta_router_mode == false) &&
        (QcMapMgr->fallback_ap_sta_router_mode == false))
    {
      QcMapBackhaulWLANMgr->sta_bridge_mode_activated = true;

      /* Start the wlan ASSOC timer for bridge mode only */
      start_wlan_sta_timer(STA_ASSOC_FAIL);
    }

    /* Reset revert flag here since we don't change it in EnableWLAN */
    QcMapMgr->revert_ap_sta_router_mode = false;
  }

  /* Install access restrictions for the Second and third SSID if the profile is
  INTERNETONLY.*/

  if ((!QcMapBackhaulWLANMgr
           || (QcMapBackhaulWLANMgr && !QcMapBackhaulWLANMgr->sta_bridge_mode_activated))
      && (!QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated()))
  {
    QcMapWLANMgr->UpdateAccessProfileRules();
  }

  LOG_MSG_INFO1("SCMProcessDynamicEnable exit",0,0,0);

  return;
}

/*=====================================================================
  FUNCTION SCMProcessDynamicDisable
======================================================================*/
/*!
@brief
  Process the indication data for Disable iface

@parameters

@return
  true  - Reconfig WLAN succeeded
  false - Reconfig WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN_SCM::SCMProcessDynamicDisable(int iface_num, int type, int dev_type)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN *lan_obj = GET_DEFAULT_LAN_OBJECT();
  char command[MAX_COMMAND_STR_LEN];
  qcmap_msgr_device_type_enum_v01 device_type =
    QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;

  LOG_MSG_INFO1("SCMProcessDynamicDisable enter wlan%d %d",iface_num,type,0);

  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() && QcMapMgr &&
      (QcMapMgr->revert_ap_sta_router_mode == true))
  {
    LOG_MSG_ERROR("Ignore the SCM dynamic disable indication since fallback to"
                   "router mode is in progress.\n",0,0,0);
    return;
  }

  /* hostapd_cli is needed for CDI in dual wifi and legacy modes */
  /*Stop all instances of hostapd_cli */
  if (type == SCM_MSGR_WLAN_IF_SAP_1_V01)
  {
    LOG_MSG_INFO1("Dynamic StopHostAPD on SAP_1 Iface wlan%d",iface_num,0,0);
    QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
    UpdateWLANIFInfo(iface_num, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                     QCMAP_MSGR_WLAN_IFACE_PRIMARY_AP_V01,
                     dev_type);
  }
  else if (type == SCM_MSGR_WLAN_IF_SAP_2_V01)
  {
    LOG_MSG_INFO1("Dynamic StopHostAPD on SAP_2 Iface wlan%d",iface_num,0,0);
    QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
    UpdateWLANIFInfo(iface_num, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                     QCMAP_MSGR_WLAN_IFACE_GUEST_AP_ONE_V01,
                     dev_type);
  }
  else if (type == SCM_MSGR_WLAN_IF_SAP_3_V01)
  {
    LOG_MSG_INFO1("Dynamic StopHostAPD on SAP_3 Iface wlan%d",iface_num,0,0);
    QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_GUEST_AP_2_V01);
    UpdateWLANIFInfo(iface_num, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                     QCMAP_MSGR_WLAN_IFACE_GUEST_AP_TWO_V01,
                     dev_type);
  }

  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
           BRIDGE_IFACE, iface_num);
  ds_system_call(command, strlen(command));
  QcMapMgr->unused_wlan_if = iface_num;

  /* Disable station mode iface. */
  if(NULL != QcMapBackhaulWLANMgr && (type == SCM_MSGR_WLAN_IF_STA_1_V01||
                                      type == SCM_MSGR_WLAN_IF_STA_1_INT_V01))
  {
    QcMapBackhaulWLANMgr->DisableStaMode();
    QcMapMgr->sta_iface_index = QCMAP_LAN_INVALID_IFACE_INDEX;
    UpdateWLANIFInfo(iface_num, QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01,
                     QCMAP_MSGR_WLAN_IFACE_STATION_V01,
                     dev_type);
  }

  /* Delete all the WLAN clients from the Connected Devices List */
  if (type == SCM_MSGR_WLAN_IF_SAP_1_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }
  else if (type == SCM_MSGR_WLAN_IF_SAP_2_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }
  else if (type == SCM_MSGR_WLAN_IF_SAP_3_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }

  /* AP-STA Bridge Mode cleanup*/
  if (type == SCM_MSGR_WLAN_IF_STA_1_V01 && QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if(!QcMapBackhaulWLANMgr)
    {
      LOG_MSG_ERROR("QcMapBackhaulWLANMgr does not exist but STA Disable attempted",0,0,0);
      return false;
    }

    QcMapMgr->CleanEbtables();
    QcMapMgr->CleanIPtables();

    /* Delete the default route*/
    snprintf(command, MAX_COMMAND_STR_LEN, "route del default gw %s dev %s",
             inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw),
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

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

    if (lan_obj)
    {
      lan_obj->BlockIPv6WWANAccess();
    }

    /* Stop timers if running */
    if (QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_assoc_running)
      stop_wlan_sta_timer(STA_ASSOC_FAIL);
    if (QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_sta_dhcp_running)
      stop_wlan_sta_timer(STA_DHCP_FAIL);
  }

  /* Reset STA mode specific parameters */
  if(type == SCM_MSGR_WLAN_IF_STA_1_V01 &&  NULL != QcMapBackhaulWLANMgr && NULL != QcMapBackhaulMgr)
  {
    QcMapBackhaulWLANMgr->sta_v4_available = false;
    QcMapBackhaulWLANMgr->sta_v6_available = false;
    QcMapBackhaulWLANMgr->sta_bridge_mode_activated = false;
    QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(WLAN_IFACE_STRING(iface_num));
  }

  LOG_MSG_INFO1("SCMProcessDynamicDisable exit",0,0,0);

  return;
}
