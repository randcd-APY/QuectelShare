/*====================================================

FILE:  QCMAP_L2TP.cpp

SERVICES:
   QCMAP L2TP Specific Implementation

=====================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  03/15/17   jc         Created

======================================================*/
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
#include <linux/msm_ipa.h>
#include <sys/ioctl.h>
#endif

#include "ds_string.h"
#include "ds_util.h"
#include "QCMAP_L2TP.h"
#include "QCMAP_ConnectionManager.h"
#include "qcmap_cm_api.h"
#include "QCMAP_LAN.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_Virtual_LAN.h"

bool QCMAP_L2TP::flag = false;
int QCMAP_L2TP::num_c_tunnel;
QCMAP_L2TP* QCMAP_L2TP::object=NULL;
bool QCMAP_L2TP::l2tpEnableFlag = false;
uint8 QCMAP_L2TP::sessionIndex[MAX_SESSION_INDEX]= {0};
char QCMAP_L2TP::L2TP_xml_path[QCMAP_MSGR_MAX_L2TP_FILE_NAME_LENGTH_V01]
                                                               = {0};
boolean QCMAP_L2TP::is_iface_up[QCMAP_MAX_PHY_LAN_IFACE];
qcmap_session_id_info_list_t QCMAP_L2TP::sessionIDList;
qcmap_L2TP_Tunnel_info_list_t QCMAP_L2TP::tunnel_list;
phy_if_name QCMAP_L2TP::physical_iface_name[QCMAP_MAX_PHY_LAN_IFACE];

boolean QCMAP_L2TP::MTU_config_enabled = false;
boolean QCMAP_L2TP::TCP_MSS_config_enabled = false;
boolean QCMAP_L2TP::TCP_MSS_Kernel_Configs_Enabled = false;
boolean QCMAP_L2TP::eth_link_mtu_set_toggle = false;
/*=====================================================
                             Class Definitions
  =====================================================*/

/*=====================================================
                             L2TP Helper functions
  =====================================================*/

/*==========================================================
 FUNCTION    qcmap_match_tunnel_id_in_list
===========================================================*/
/*!
@brief
  Comparator function for match tunnel id in tunnel list

@parameters
  Two void pointers with tunnel list item type as first and tunnel id as second.

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
long int QCMAP_L2TP::qcmap_match_tunnel_id_in_list
(
  const void *first,
  const void *second
)
{
  uint32* tunnel_id = NULL;
/*------------------------------------------------------------------------*/

  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }

  qcmap_L2TP_Tunnel_list_item_t *tunnel_node =\
                                       (qcmap_L2TP_Tunnel_list_item_t*)first;

  tunnel_id = ( uint32*)second;

  return(\
     (*tunnel_id == tunnel_node->local_tunnel_id) ? false : true );
}

/*==========================================================
 FUNCTION    qcmap_match_session_id
===========================================================*/
/*!
@brief
  Comparator function for match session id's in session ID list

@parameters
  Two void pointers both pointing to session id.

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
long int QCMAP_L2TP::qcmap_match_session_id
(
  const void *first,
  const void *second
)
{
  uint32* session_node = NULL;
  uint32* session_id = NULL;
/*------------------------------------------------------------------------*/
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  session_node = (uint32*)first;
  session_id = ( uint32*)second;

  return(\
     (*session_id == *session_node) ? false : true );
}
/*==========================================================
 FUNCTION    qcmap_match_session_id_from_list
===========================================================*/
/*!
@brief
  Comparator function for match session id in session list

@parameters
  Two void pointers with session list type as first and session id as second.

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
long int QCMAP_L2TP::qcmap_match_session_id_from_list
(
  const void *first,
  const void *second
)
{
  uint32* session_id = NULL;
/*------------------------------------------------------------------------*/
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }

  qcmap_L2TP_Session_list_item_t* session_node =\
                                       (qcmap_L2TP_Session_list_item_t*)first;
  session_id = ( uint32*)second;

  return(\
     (*session_id == session_node->sessionConfig.session_id) ? false : true );
}
/*=====================================================
  FUNCTION DeleteL2TPTunnelConfig
======================================================*/
/*!
@brief
  Deletes L2TP config and brings down tunnels/sessions if phy link is up.

@parameters
  qcmap_msgr_delete_l2tp_config_v01 l2tp_config,
  void *softApHandle,
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
/*====================================================*/
boolean QCMAP_L2TP::DeleteL2TPTunnelConfig
(
  qcmap_msgr_delete_l2tp_config_v01 l2tp_config,
  void                      *softApHandle,
  qmi_error_type_v01        *qmi_err_num
)
{
  QCMAP_L2TP* QcMapL2TPMgr= QCMAP_L2TP::Get_Instance(false);
  ds_dll_el_t *search_node = NULL;
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  QCMAP_ConnectionManager* QcMapMgr=
                        QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*------------------------------------------------------------------------*/
  *qmi_err_num = QMI_ERR_NONE_V01;

  if (QcMapMgr && !(IS_L2TP_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("L2TP is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapL2TPMgr)
  {
    LOG_MSG_ERROR("DeleteL2TPTunnelConfig(), L2TP instance is NULL",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if (QCMAP_L2TP::l2tpEnableFlag == false)
  {
    LOG_MSG_ERROR("DeleteL2TPTunnelConfig(),L2TP is not enabled\n",0,0,0);
    *qmi_err_num= QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  tunnel_list = &(QCMAP_L2TP::tunnel_list);
  search_node = ds_dll_search (
                tunnel_list->L2TP_TunnelListHead,
                (void*)&(l2tp_config.tunnel_id),
                qcmap_match_tunnel_id_in_list);

  if (NULL == search_node)
  {
    LOG_MSG_INFO1("DeleteL2TPTunnelConfig() - Tunnel ID %d is not configured",
                    l2tp_config.tunnel_id,0,0);
    *qmi_err_num= QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* Remove L2TP sessions and tunnel*/
  l2tp_config_node = (qcmap_L2TP_Tunnel_list_item_t*)ds_dll_data(search_node);
  QcMapL2TPMgr->RemoveL2TP(*l2tp_config_node);

  /* Clean up from local lists and xml file*/
  if (!QcMapL2TPMgr->CleanupL2TPTunnelConfig(l2tp_config_node))
  {
    LOG_MSG_ERROR("DeleteL2TPTunnelConfig()- Error in deleting tunnel config",
                  0,0,0);
    *qmi_err_num= QMI_ERR_INTERNAL_V01;
    return false;
  }

  /*Delete list item and free nodes*/
  ds_dll_delete (tunnel_list->L2TP_TunnelListHead,
                               &(tunnel_list->L2TP_TunnelListTail),
                             (void*)&(l2tp_config.tunnel_id),
                             qcmap_match_tunnel_id_in_list);

  ds_free(l2tp_config_node);
  ds_dll_free(search_node);

  return true;
}
/*=====================================================
  FUNCTION GetL2TPConfig
======================================================*/
/*!
@brief
  Retrieves L2TP Config

@parameters
  qcmap_msgr_l2tp_config_v01         *l2tp_config,
  uint32                             *length,
  qcmap_msgr_l2tp_mode_enum_v01      *mode,
  qcmap_msgr_l2tp_mtu_config_v01     *l2tp_mtu_config,
  qcmap_msgr_l2tp_TCP_MSS_config_v01 *l2tp_mss_config,
  qmi_error_type_v01                  *qmi_err_num

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
boolean QCMAP_L2TP::GetL2TPConfig
(
  qcmap_msgr_l2tp_config_v01         *l2tp_config,
  uint32                             *length,
  qcmap_msgr_l2tp_mode_enum_v01      *mode,
  qcmap_msgr_l2tp_mtu_config_v01     *l2tp_mtu_config,
  qcmap_msgr_l2tp_TCP_MSS_config_v01 *l2tp_mss_config,
  qmi_error_type_v01                  *qmi_err_num
)
{
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  QCMAP_L2TP* QcMapL2TPMgr= QCMAP_L2TP::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=
                        QCMAP_ConnectionManager::Get_Instance(NULL,false);
  uint32 num_tunnels = 0;
  *qmi_err_num = QMI_ERR_NONE_V01;
/*------------------------------------------------------------------------*/
  if (QcMapMgr && !(IS_L2TP_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("L2TP is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (NULL == l2tp_config || NULL == length || NULL == l2tp_mtu_config
      || NULL == l2tp_mss_config)
  {
    LOG_MSG_ERROR("GetL2TPConfig(),NULL Config passed\n",0,0,0);
    *qmi_err_num= QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (QCMAP_L2TP::l2tpEnableFlag == false)
  {
    LOG_MSG_ERROR("GetL2TPConfig(),L2TP is not enabled\n",0,0,0);
    *qmi_err_num= QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapL2TPMgr)
  {
    LOG_MSG_ERROR("GetL2TPConfig(), L2TP instance is NULL",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  *mode = QCMAP_MSGR_L2TP_ROUTER_MODE_V01;

  l2tp_mtu_config->enable = QCMAP_L2TP::MTU_config_enabled;
  l2tp_mss_config->enable = QCMAP_L2TP::TCP_MSS_config_enabled;
  tunnel_list = &QCMAP_L2TP::tunnel_list;

  if((*qmi_err_num = (QcMapL2TPMgr->GetTunnelConfigFromList(tunnel_list,
                                 &l2tp_config[0],
                                 &num_tunnels))) != QMI_ERR_NONE_V01)
  {
    LOG_MSG_ERROR("GetL2TPConfig() - "
                  "Error copying tunnel config",0,0,0);
    return false;
  }

  if (num_tunnels > QCMAP_MSGR_L2TP_MAX_TUNNELS_V01)
  {
    LOG_MSG_ERROR("GetL2TPConfig() - Max tunnels reached",0,0,0);
    *qmi_err_num =  QMI_ERR_INTERNAL_V01;
    return false;
  }

  *length = num_tunnels;
  LOG_MSG_ERROR("GetL2TPConfig()- returning length %d",*length,0,0);
  return true;
}
/*=====================================================
  FUNCTION GetSetL2TPEnableConfigFromXML
======================================================*/
/*!
@brief
  Get and sets the L2TP Config from XML.

@parameters
  qcmap_action_type action,
  QCMAP_L2TP_Config_type *l2tp_config_info

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
boolean QCMAP_L2TP::GetSetL2TPEnableConfigFromXML
(
  qcmap_action_type action,
  qcmap_msgr_set_unmanaged_l2tp_state_config_v01 *l2tp_config_info
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  uint8 enable;
  QCMAP_ConnectionManager* QcMapMgr =
                             QCMAP_ConnectionManager::Get_Instance(NULL, false);
/*------------------------------------------------------------------------*/
    if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root =
       xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(L2TPConfig_TAG);
  if(action == GET_VALUE)
  {
    enable = atoi(root.child(L2TPCONFIG_ENABLE_TAG).child_value());
    l2tp_config_info->enable = enable;
  }
  else if(action == SET_VALUE)
  {
    enable = l2tp_config_info->enable;
    snprintf(data,sizeof(data),"%d",enable);
    root.child(L2TPCONFIG_ENABLE_TAG).text() = data;

    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
  }
  else
  {
    LOG_MSG_ERROR("Get/Set L2TP config, Invalid Action: %d\n",action,0,0);
    return false;
  }
  LOG_MSG_INFO1("Get/Set L2TP info action %d, enable: %d\n",
                action,l2tp_config_info->enable,0);

  return true;
}

/*=====================================================
  FUNCTION GetSetMTUEnableConfigFromXML
======================================================*/
/*!
@brief
  Get and sets the L2TP MTU Enable Config from XML.

@parameters
  qcmap_action_type action,
  qcmap_msgr_l2tp_mtu_config_v01 *mtu_config

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
boolean QCMAP_L2TP::GetSetMTUEnableConfigFromXML
(
  qcmap_action_type action,
  qcmap_msgr_l2tp_mtu_config_v01 *mtu_config
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  uint8 enable;
  QCMAP_ConnectionManager* QcMapMgr =
                             QCMAP_ConnectionManager::Get_Instance(NULL, false);
/*------------------------------------------------------------------------*/
    if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root =
       xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(L2TPConfig_TAG);
  if(action == GET_VALUE)
  {
    enable = atoi(root.child(L2TPEnableMTU_TAG).child_value());
    mtu_config->enable = enable;
  }
  else if(action == SET_VALUE)
  {
    enable = mtu_config->enable;
    snprintf(data,sizeof(data),"%d",enable);
    root.child(L2TPEnableMTU_TAG).text() = data;

    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
  }
  else
  {
    LOG_MSG_ERROR("Get/Set L2TP MTU Enable config, Invalid Action: %d\n",
                                                              action,0,0);
    return false;
  }
  LOG_MSG_INFO1("Get/Set L2TP MTU Enable config action %d, enable: %d\n",
                action,mtu_config->enable,0);

  return true;
}

/*=====================================================
  FUNCTION GetSetTCPMSSEnableConfigFromXML
======================================================*/
/*!
@brief
  Get and sets the L2TP TCP MSS Enable Config from XML.

@parameters
  qcmap_action_type action,
  qcmap_msgr_l2tp_TCP_MSS_config_v01 *mss_config

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
boolean QCMAP_L2TP::GetSetTCPMSSEnableConfigFromXML
(
  qcmap_action_type action,
  qcmap_msgr_l2tp_TCP_MSS_config_v01 *mss_config
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  uint8 enable;
  QCMAP_ConnectionManager* QcMapMgr =
                             QCMAP_ConnectionManager::Get_Instance(NULL, false);
/*------------------------------------------------------------------------*/
    if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root =
       xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(L2TPConfig_TAG);
  if(action == GET_VALUE)
  {
    enable = atoi(root.child(L2TPEnableTCPMSS_TAG).child_value());
    mss_config->enable = enable;
  }
  else if(action == SET_VALUE)
  {
    enable = mss_config->enable;
    snprintf(data,sizeof(data),"%d",enable);
    root.child(L2TPEnableTCPMSS_TAG).text() = data;

    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
  }
  else
  {
    LOG_MSG_ERROR("Get/Set L2TP TCP MSS Enable config, Invalid Action: %d\n",
                                                              action,0,0);
    return false;
  }
  LOG_MSG_INFO1("Get/Set L2TP TCP MSS Enable config action %d, enable: %d\n",
                action,mss_config->enable,0);
  return true;
}

/*=====================================================
  FUNCTION SetL2TPState
======================================================*/
/*!
@brief
  Enables/ Disables L2TP Config.

@parameters
  qcmap_msgr_set_l2tp_state_v01 l2tp_config,
  void                           *softApHandle,
  qmi_error_type_v01              *qmi_err_num

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
boolean QCMAP_L2TP::SetL2TPState
(
  qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_config,
  void                           *softApHandle,
  qmi_error_type_v01              *qmi_err_num
)
{
  QCMAP_L2TP* QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(false);
  pugi::xml_document xml_file;
  *qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_msgr_l2tp_mtu_config_v01 l2tp_mtu_config;
  qcmap_msgr_l2tp_TCP_MSS_config_v01 l2tp_mss_config;
  QCMAP_ConnectionManager* QcMapMgr=
                        QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*------------------------------------------------------------------------*/
  if (QcMapMgr && !(IS_L2TP_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("L2TP is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (l2tp_config.enable == 1 && QCMAP_L2TP::l2tpEnableFlag == true)
  {
    LOG_MSG_ERROR("SetL2TPState() - L2TP Config already enabled "\
                                                                 ,0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  if (l2tp_config.enable == 0 && QCMAP_L2TP::l2tpEnableFlag == false)
  {
    LOG_MSG_ERROR("SetL2TPState() - L2TP Config already disabled "\
                                                                 ,0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  if(l2tp_config.enable == 1)
  {
    QCMAP_L2TP::l2tpEnableFlag = true;
    /* IF object was not already created, instantiate the object */
    if (!QcMapL2TPMgr)
    {
      LOG_MSG_INFO1("SetL2TPState() - L2TP object is NULL ",0,0,0);
      QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(true);
    }
  }
  else if (l2tp_config.enable == 0)
  {
    QCMAP_L2TP::l2tpEnableFlag = false;
    /* This object cannot be null as we create it if L2TP was enabled before.*/
    if (!QcMapL2TPMgr)
    {
      LOG_MSG_ERROR("SetL2TPState() - L2TP object is NULL ",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
    }
  }

  if( 1 == l2tp_config.enable)
  {
    /*Check if file path is a valid xml file*/
    if (!xml_file.load_file(L2TP_CFG_XML))
    {
      LOG_MSG_ERROR("SetL2TPState() - L2TP config file %s not present "\
                                                            ,L2TP_CFG_XML,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }

    /*save config xml*/
    strlcpy(QCMAP_L2TP::L2TP_xml_path,L2TP_CFG_XML,
                              QCMAP_MSGR_MAX_L2TP_FILE_NAME_LENGTH_V01);
    /* Read and install config from new xml*/
    QcMapL2TPMgr->ReadInstallL2TPConfigFromXML();
  }
  else
  {
    LOG_MSG_INFO1("SetL2TPState() - Delete L2TP Object for disable",0,0,0);
    delete QcMapL2TPMgr;
    /*Since user disabled L2TP state, MTU and MSS enable config are reset to 0
    and user sets them when enabled later.*/
    l2tp_mtu_config.enable =  0;
    l2tp_mss_config.enable = 0;
    QCMAP_L2TP::GetSetMTUEnableConfigFromXML(SET_VALUE,&l2tp_mtu_config);
    QCMAP_L2TP::GetSetTCPMSSEnableConfigFromXML(SET_VALUE,&l2tp_mss_config);
     /*Delete existing tunnel configs*/
  }

  QCMAP_L2TP::GetSetL2TPEnableConfigFromXML(SET_VALUE,&l2tp_config);
  return true;
}
/*=====================================================
  FUNCTION SetMTUConfigForL2TP
======================================================*/
/*!
@brief
  Sets MTU sie on all underlying interfaces of L2TP tunnels to avoid
   fragmentation.

@parameters
  qcmap_msgr_l2tp_mtu_config_v01 l2tp_config,
  void *softApHandle,
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
/*====================================================*/
boolean QCMAP_L2TP::SetMTUConfigForL2TP(
                              qcmap_msgr_l2tp_mtu_config_v01 l2tp_mtu_config,
                              void *softApHandle,
                              qmi_error_type_v01 *qmi_err_num )
{
  QCMAP_L2TP* QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(false);
  ds_dll_el_t * node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  QCMAP_ConnectionManager* QcMapMgr=
                        QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*------------------------------------------------------------------------*/
  if (QcMapMgr && !(IS_L2TP_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("L2TP is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapL2TPMgr)
  {
    LOG_MSG_ERROR("SetMTUConfigForL2TP() - L2TP object is NULL, "
                  "save config and return",0,0,0);
    QCMAP_L2TP::GetSetMTUEnableConfigFromXML(SET_VALUE,&l2tp_mtu_config);
    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
  }

  tunnel_list = &(QCMAP_L2TP::tunnel_list);

  if (QCMAP_L2TP::MTU_config_enabled != l2tp_mtu_config.enable)
  {
    /*Change MTU size on all interfaces on which L2TP tunnels are installed.*/
    node = tunnel_list->L2TP_TunnelListHead;
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
    while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
    {
      if (NULL == l2tp_config_node)
      {
        LOG_MSG_ERROR("SetMTUConfigForL2TP - Invalid tunnel config",0,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }

      if (l2tp_config_node->is_installed)
      {
        QcMapL2TPMgr->ConfigDelMTUSizeOnPhyVLANiface(l2tp_mtu_config.enable,
                                                     l2tp_config_node->phy_type,
                                                     l2tp_config_node->vlan_id);
      }
      node = ds_dll_next (node, (const void**)(&l2tp_config_node));
    }
  }
  /*Set and Save config to XML*/
  QCMAP_L2TP::MTU_config_enabled = l2tp_mtu_config.enable;
  QCMAP_L2TP::GetSetMTUEnableConfigFromXML(SET_VALUE,&l2tp_mtu_config);
  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}
/*=====================================================
  FUNCTION SetTCPMSSConfigForL2TP
======================================================*/
/*!
@brief
  Sets TCP MSS clamping rules in order to avoid segmentation of tunneled L2TP
   packets. This is common config for all L2TP tunnels and sessions.

@parameters
  qcmap_msgr_l2tp_TCP_MSS_config_v01 l2tp_config,
  void *softApHandle,
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
/*====================================================*/
boolean QCMAP_L2TP::SetTCPMSSConfigForL2TP(
                             qcmap_msgr_l2tp_TCP_MSS_config_v01 l2tp_mss_config,
                             void *softApHandle,
                             qmi_error_type_v01 *qmi_err_num )
{
  QCMAP_L2TP* QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(false);
  ds_dll_el_t * node = NULL, *session_node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  qcmap_L2TP_Session_list_item_t *session_config = NULL;
  char iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  QCMAP_ConnectionManager* QcMapMgr =
                             QCMAP_ConnectionManager::Get_Instance(NULL, false);
/*------------------------------------------------------------------------*/
  if (QcMapMgr && !(IS_L2TP_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("L2TP is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapL2TPMgr)
  {
    LOG_MSG_ERROR("SetTCPMSSConfigForL2TP() - L2TP object is NULL, "
                  "save config and return",0,0,0);
    QCMAP_L2TP::GetSetTCPMSSEnableConfigFromXML(SET_VALUE,&l2tp_mss_config);
    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
  }

  tunnel_list = &(QCMAP_L2TP::tunnel_list);

  if (QCMAP_L2TP::TCP_MSS_config_enabled != l2tp_mss_config.enable)
  {
    /*Install TCP MSS clamp rule on all installed tunnels, sessions*/
    node = tunnel_list->L2TP_TunnelListHead;
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
    while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
    {
      if (NULL == l2tp_config_node)
      {
        LOG_MSG_ERROR("SetTCPMSSConfigForL2TP - Invalid tunnel config",0,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }

      if (l2tp_config_node->is_installed)
      {
        session_node =  l2tp_config_node->session_list.L2TP_SessionListHead;
        session_node =
                 ds_dll_next (session_node, (const void**)(&session_config));
        while (session_node \
          && \
          session_node != l2tp_config_node->session_list.L2TP_SessionListHead)
        {
          if (NULL == session_config)
          {
            LOG_MSG_ERROR("SetTCPMSSConfigForL2TP-Invalid session config node",
                                                                   0,0,0);
            *qmi_err_num = QMI_ERR_INTERNAL_V01;
            return false;
          }
          snprintf(iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01,"l2tpeth%d",
                                                  session_config->SessionIndex);
          QcMapL2TPMgr->ConfigDelL2TPTCPMSS(l2tp_mss_config.enable,iface_name);
          session_node =
                 ds_dll_next (session_node, (const void**)(&session_config));
        }
      }

      l2tp_config_node = NULL;
      node = ds_dll_next (node, (const void**)(&l2tp_config_node));
    }
  }
  /*Set and Save config to XML*/
  QCMAP_L2TP::TCP_MSS_config_enabled = l2tp_mss_config.enable;
  QCMAP_L2TP::GetSetTCPMSSEnableConfigFromXML(SET_VALUE,&l2tp_mss_config);
  if (!QCMAP_L2TP::TCP_MSS_config_enabled)
  {
    ds_system_call("rmmod xt_physdev",
             strlen("rmmod xt_physdev"));
    if (QcMapMgr && !QcMapMgr->sfe_loaded)
      ds_system_call("rmmod br_netfilter",
              strlen("rmmod br_netfilter"));
    QCMAP_L2TP::TCP_MSS_Kernel_Configs_Enabled = false;
  }
  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}
/*=====================================================
  FUNCTION SetL2TPConfig
======================================================*/
/*!
@brief
  Sets L2TP config and brings up tunnels/sessions if phy link is up.

@parameters
  qcmap_msgr_l2tp_config_v01 l2tp_config,
  void *softApHandle,
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
/*====================================================*/
boolean QCMAP_L2TP::SetL2TPConfig
(
  qcmap_msgr_l2tp_config_v01 l2tp_config,
  void *softApHandle,
  qmi_error_type_v01        *qmi_err_num
)
{
  qcmap_L2TP_Tunnel_list_item_t tunnel_node,temp_node;
  qcmap_L2TP_Tunnel_list_item_t *match_tunnel_node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *tunnel_conf_node = NULL;
  QCMAP_L2TP* QcMapL2TPMgr= QCMAP_L2TP::Get_Instance(false);
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list;
  ds_dll_el_t * node = NULL, *search_node = NULL, *temp_search_node = NULL;
  QCMAP_Virtual_LAN_phy_iface phy_type = QCMAP_TETH_MIN;
  int16_t vlan_id = 0;
  qcmap_L2TP_Session_list_item_t *session_config = NULL;
  uint32 *session_id_ptr = NULL;
  qcmap_L2TP_Session_info_list_t *sessionList = NULL;
  qcmap_session_id_info_list_t* sessionIDList = &(QCMAP_L2TP::sessionIDList);
  QCMAP_ConnectionManager* QcMapMgr=
                        QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* lan_obj = NULL;
  qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_state_config;
/*------------------------------------------------------------------------*/
  if (QcMapMgr && !(IS_L2TP_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("L2TP is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapL2TPMgr)
  {
    LOG_MSG_ERROR("SetL2TPConfig(), L2TP instance is NULL",0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  memset(&tunnel_node,0,sizeof(qcmap_L2TP_Tunnel_list_item_t));
  memset(&temp_node,0,sizeof(qcmap_L2TP_Tunnel_list_item_t));

  if(!(QCMAP_Virtual_LAN::GetPhyIfaceVLANIDFromIface(l2tp_config.local_iface,\
                                                  &phy_type,&vlan_id)))
  {
    LOG_MSG_ERROR("SetL2TPConfig() - Invalid local_iface: %s"
                  " found in L2TP config",l2tp_config.local_iface,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

 /* Check if phy_type is having valid (0-3) interface ID */
  if(VALIDATE_PHY_TYPE(phy_type))
  {
    LOG_MSG_ERROR("SetL2TPConfig() Not a valid iface_type",0,0,0);
    return false;
  }

  tunnel_node.phy_type = phy_type;
  tunnel_node.vlan_id = vlan_id;

  if(0 != tunnel_node.vlan_id)
  {
    if((lan_obj = GET_LAN_OBJECT(vlan_id)) != NULL)
    {
      if(!(IS_DEFAULT_PROFILE(lan_obj->GetBackhaulProfileHandle())))
      {
        LOG_MSG_INFO1("VLAN ID: %d is currently mapped to profile handle: %d, please map to "
                      "default profile before configuring L2TP", vlan_id,
                      lan_obj->GetBackhaulProfileHandle(), 0);
        //Disable L2TP automatically so that VLAN to default PDN mapping can go smoothly
        l2tp_state_config.enable = 0;
        if(!QCMAP_L2TP::SetL2TPState(l2tp_state_config, NULL, qmi_err_num))
        {
          LOG_MSG_INFO1("Failed to disable L2TP", 0, 0, 0);
        }
        *qmi_err_num = QMI_ERR_INCOMPATIBLE_STATE_V01;
        return false;
      }
    }
  }

  if (!QcMapL2TPMgr->ConstructTunnelNode(l2tp_config, &tunnel_node))
  {
    LOG_MSG_ERROR("SetL2TPConfig(), Error creatig tunnel node",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if (!QcMapL2TPMgr->ValidateTunnelConfig(&tunnel_node,false))
  {
    LOG_MSG_ERROR("SetL2TPConfig(), Invalid config sent",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  /*This case should not happen*/
  if (QCMAP_L2TP::l2tpEnableFlag == false)
  {
    if(QCMAP_L2TP::num_c_tunnel < QCMAP_MSGR_L2TP_MAX_TUNNELS_V01)
    {
      QcMapL2TPMgr->AddL2TPNodeToXML(&tunnel_node);
      *qmi_err_num= QMI_ERR_NONE_V01;
    }
    else
    {
      LOG_MSG_ERROR("SetConfigL2TP()- Cannot further add tunnel, max value = %d",
                                                        QCMAP_L2TP::num_c_tunnel,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    return true;
  }

  /*Check if there is matching tunnel id on that interface.*/
  tunnel_list = &QCMAP_L2TP::tunnel_list;

  /*Search for matching tunnel on the tunnel list*/
  search_node = ds_dll_search (tunnel_list->L2TP_TunnelListHead,
                               (void*)&(tunnel_node.local_tunnel_id),
                               qcmap_match_tunnel_id_in_list);

  if (NULL != search_node)
  {
    LOG_MSG_ERROR("SetL2TPConfig(),matching tunnel found\n",0,0,0);
    match_tunnel_node =
                   (qcmap_L2TP_Tunnel_list_item_t*)ds_dll_data(search_node);
    if (match_tunnel_node == NULL)
    {
      LOG_MSG_ERROR("SetL2TPConfig() - Tunnel info NULL", 0,0,0);
      ds_dll_delete (tunnel_list->L2TP_TunnelListHead,
                     &tunnel_list->L2TP_TunnelListTail,
                     (void*)&(tunnel_node.local_tunnel_id),
                     qcmap_match_tunnel_id_in_list);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    /*Check for exact parameters match*/
    if (QcMapL2TPMgr->MatchTunnelConfig(match_tunnel_node,&tunnel_node))
    {
      /*exact match found, return no effect error*/
      LOG_MSG_ERROR("SetL2TPConfig() - Matching Tunnel and "
                    "session found, return", 0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
    }
    else
    {
      memcpy(&temp_node,match_tunnel_node,
                        sizeof(qcmap_L2TP_Tunnel_list_item_t));
      if (QcMapL2TPMgr->is_iface_up[phy_type]
          && !QcMapL2TPMgr->RemoveL2TP(temp_node))
      {
        LOG_MSG_ERROR("SetL2TPConfig() - Error removing tunnel", 0,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }

      QcMapL2TPMgr->DelL2TPNodeFromXML(match_tunnel_node);

      /* Delete and free linked list session and tunel nodes*/
      sessionList = &(temp_node.session_list);
      node = sessionList->L2TP_SessionListHead;
      node = ds_dll_next (node, (const void**)(&session_config));

      if (sessionList->L2TP_SessionListHead != NULL)
      {
        while (node \
          && \
          node != sessionList->L2TP_SessionListHead)
        {
          search_node = ds_dll_delete (
                            sessionIDList->SessionIDHead,
                           &(sessionIDList->SessionIDTail),
                           (void*)&(session_config->sessionConfig.session_id),
                           qcmap_match_session_id);

          if (NULL == search_node)
          {
            LOG_MSG_ERROR("SetL2TPConfig - Error deleting session"
                    " ID %d",session_config->sessionConfig.session_id,0,0);
            return false;
          }
          QCMAP_L2TP::sessionIndex[session_config->SessionIndex] = 0;

          node = ds_dll_next (node, (const void**)(&session_config));
        }
      }

      ds_dll_delete_all(sessionList->L2TP_SessionListHead);
      temp_search_node = ds_dll_delete (tunnel_list->L2TP_TunnelListHead,
                               &tunnel_list->L2TP_TunnelListTail,
                               (void*)&temp_node.local_tunnel_id,
                               qcmap_match_tunnel_id_in_list);
      ds_dll_free(temp_search_node);

    }

  } /*end of NULL != search_node*/

  if (!QcMapL2TPMgr->ValidateTunnelConfig(&tunnel_node,true))
  {
    LOG_MSG_ERROR("SetL2TPConfig(), Invalid config sent",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (QcMapL2TPMgr->is_iface_up[phy_type])
  {
    if(!QcMapL2TPMgr->InstallL2TP(tunnel_node))
    {
      LOG_MSG_ERROR("SetL2TPConfig() - Error installing tunnel", 0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    tunnel_node.is_installed = true;
  }
  else
  {
    LOG_MSG_INFO1("SetL2TPConfig() - Phy link not up. Saving config", 0,0,0);
  }

  node = tunnel_node.session_list.L2TP_SessionListHead;
  node = ds_dll_next (node, (const void**)(&session_config));
  while (node \
        && \
        node != tunnel_node.session_list.L2TP_SessionListHead)
  {
    if(!QcMapL2TPMgr->AddSessionIDEntryToList(
                                      session_config->sessionConfig.session_id,
                                      session_id_ptr))
    {
      LOG_MSG_ERROR("SetL2TPConfig() - Error in adding session"
                  " ID to list",0,0,0);
      QcMapL2TPMgr->RemoveL2TP(tunnel_node);
      goto delete_session;
    }
    node = ds_dll_next (node, (const void**)(&session_config));
    session_id_ptr = NULL;
  }

  if ((QCMAP_L2TP::num_c_tunnel >= QCMAP_MSGR_L2TP_MAX_TUNNELS_V01) ||
            (!QcMapL2TPMgr->AddL2TPTunnelEntryToList(tunnel_node,tunnel_conf_node)))
  {
    LOG_MSG_ERROR("SetL2TPConfig() - Error saving tunnel config", 0,0,0);
    QcMapL2TPMgr->RemoveL2TP(tunnel_node);
    goto delete_session;
  }

  QcMapL2TPMgr->AddL2TPNodeToXML(&tunnel_node);
  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;

  delete_session:

  node = tunnel_node.session_list.L2TP_SessionListHead;
  node = ds_dll_next (node, (const void**)(&session_config));
  while (node \
      && \
      node != tunnel_node.session_list.L2TP_SessionListHead)
  {
    ds_dll_delete (QCMAP_L2TP::sessionIDList.SessionIDHead,
                  &QCMAP_L2TP::sessionIDList.SessionIDTail,
                  (void*)&(session_config->sessionConfig.session_id),
                  qcmap_match_session_id);
    node = ds_dll_next (node, (const void**)(&session_config));
  }
  *qmi_err_num = QMI_ERR_INTERNAL_V01;
  return false;
}

/*=====================================================
                 Class Cunstuctor, Destructors.
  =====================================================*/

/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and return instance of class QCMAP_L2TP

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

QCMAP_L2TP* QCMAP_L2TP::Get_Instance(boolean obj_create)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : L2TP",0, 0, 0);
    object = new QCMAP_L2TP();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}
/*=====================================================
  FUNCTION Constructor
======================================================*/
/*!
@brief
  Initializes L2TP variables.

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
QCMAP_L2TP::QCMAP_L2TP()
{
  QCMAP_Virtual_LAN* QcMapVLANMgr=
                                  QCMAP_Virtual_LAN::Get_Instance(false);
  qcmap_msgr_l2tp_mtu_config_v01 mtu_config;
  qcmap_msgr_l2tp_TCP_MSS_config_v01 mss_config;
  QCMAP_ConnectionManager* QcMapMgr=
                        QCMAP_ConnectionManager::Get_Instance(NULL,false);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (QcMapMgr && !(IS_L2TP_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("L2TP is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    return;
  }

  mtu_config.enable = 0;
  mss_config.enable = 0;
  num_c_tunnel = 0;
  memset(&this->sessionIndex,0,sizeof(uint8)*MAX_SESSION_INDEX);

   for (int i = 0; i < QCMAP_MAX_PHY_LAN_IFACE; i++)
  {
    switch (i)
    {
      case QCMAP_TETH_ECM:
        strlcpy(this->physical_iface_name[i].if_name, ECM_IFACE,
                                          QCMAP_MAX_IFACE_NAME_SIZE_V01);
      break;
      case QCMAP_TETH_RNDIS:
        strlcpy(this->physical_iface_name[i].if_name, RNDIS_IFACE,
                                        QCMAP_MAX_IFACE_NAME_SIZE_V01);

      break;
      case QCMAP_TETH_ETH:
        strlcpy(this->physical_iface_name[i].if_name, ETH_IFACE,
                                       QCMAP_MAX_IFACE_NAME_SIZE_V01);

      break;
      case QCMAP_TETH_BRIDGE0:
        strlcpy(this->physical_iface_name[i].if_name, BRIDGE_IFACE,
                                         QCMAP_MAX_IFACE_NAME_SIZE_V01);

      break;
      default:
        LOG_MSG_ERROR("QCMAP_L2TP() Not a valid iface_type",0,0,0);
        return;
      break;
    }
  }

  /*Read L2TP enable config from mobileap_cfg.xml, enable flag is not read here
  because this constructor is called when user enables L2TP or when it is
  enabled during bootup.*/

  QCMAP_L2TP::l2tpEnableFlag = true;
  strlcpy(QCMAP_L2TP::L2TP_xml_path,L2TP_CFG_XML,
                                 QCMAP_MSGR_MAX_L2TP_FILE_NAME_LENGTH_V01);

    /*Initialize session ID list to NULL*/
  QCMAP_L2TP::sessionIDList.SessionIDHead = NULL;
  QCMAP_L2TP::sessionIDList.SessionIDTail = NULL;

   /*Initialize Tunnel list to NULL*/
  QCMAP_L2TP::tunnel_list.L2TP_TunnelListHead = NULL;
  QCMAP_L2TP::tunnel_list.L2TP_TunnelListTail = NULL;

  if (!QcMapVLANMgr)
  {
    for (int i =0; i < QCMAP_MAX_PHY_LAN_IFACE; i++)
    {
      this->is_iface_up[i] = false;
    }
  }
  else
  {
    for (int i =0; i < QCMAP_MAX_PHY_LAN_IFACE; i++)
    {
      this->is_iface_up[i] = QcMapVLANMgr->IsPhyLinkUp(i);
    }
  }

  QCMAP_L2TP::GetSetMTUEnableConfigFromXML(GET_VALUE,&mtu_config);
  QCMAP_L2TP::MTU_config_enabled = mtu_config.enable;

  QCMAP_L2TP::GetSetTCPMSSEnableConfigFromXML(GET_VALUE,&mss_config);
  QCMAP_L2TP::TCP_MSS_config_enabled = mss_config.enable;

  LOG_MSG_INFO1("QCMAP_L2TP() Enable MTU %d, Enable TCP MSS %d",
                     QCMAP_L2TP::MTU_config_enabled,
                     QCMAP_L2TP::TCP_MSS_config_enabled,0)

  /*Read L2TP Config from XML file if enabled on bootup*/
  if (QCMAP_L2TP::l2tpEnableFlag == true)
  {
    if (!this->ReadInstallL2TPConfigFromXML())
    {
      LOG_MSG_ERROR("QCMAP_L2TP() Error reading L2TP Config from xml"\
                    ,0,0,0);
    }
  }
}

/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroyes the L2TP Object.

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


QCMAP_L2TP::~QCMAP_L2TP()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_dll_delete_all(QCMAP_L2TP::sessionIDList.SessionIDHead);
  QCMAP_ConnectionManager* QcMapMgr =
                      QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!QcMapMgr)
  {
    LOG_MSG_ERROR("~QCMAP_L2TP Error fetching QcMapMgr object",0,0,0);
    TCP_MSS_Kernel_Configs_Enabled = false;
  }
  this->DeleteAllTunnels();
  l2tpEnableFlag = false;
  flag=false;
  if (TCP_MSS_Kernel_Configs_Enabled)
  {
    ds_system_call("rmmod xt_physdev",
              strlen("rmmod xt_physdev"));
    if (!QcMapMgr->sfe_loaded)
        ds_system_call("rmmod br_netfilter",
                 strlen("rmmod br_netfilter"));
  }
  TCP_MSS_Kernel_Configs_Enabled = false;
  object=NULL;
}
/*=====================================================
                 Public Functions.
  =====================================================*/

/*=====================================================
  FUNCTION AddL2TPNodeToXML
======================================================*/
/*!
@brief
  Adds L2TP node to XML.

@parameters
qcmap_L2TP_Tunnel_list_item_t *tunnel_node

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

void QCMAP_L2TP::AddL2TPNodeToXML
(
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child, new_node,subchild,subchild1, subchild2;
  int16_t vlan_id = 0;
  char local_iface[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  char temp[] = {0};
  char data[MAX_STRING_LENGTH] = {0};
  char v6_str[INET6_ADDRSTRLEN] = {0};
  char v4_str[INET_ADDRSTRLEN] = {0};
  uint32 temp_val = 0;
  qcmap_L2TP_Session_list_item_t *session_config;
  ds_dll_el_t * ll_node = NULL;
  char command[MAX_COMMAND_STR_LEN] = {0};
/*------------------------------------------------------------------------*/
  if (NULL == tunnel_node)
  {
    LOG_MSG_ERROR("NULL tunnel node passed.",0,0,0);
    return;
  }

  if (tunnel_node->phy_type <= QCMAP_TETH_MIN || tunnel_node->phy_type >= QCMAP_MAX_PHY_LAN_IFACE)
  {
    LOG_MSG_ERROR("AddL2TPNodeToXML() Not a valid iface_type",0,0,0);
    return;
  }

  if(!xml_file.load_file(QCMAP_L2TP::L2TP_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return;
  }
  vlan_id = tunnel_node->vlan_id;
  if (vlan_id == 0)
  {
    snprintf(local_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s",
                     this->physical_iface_name[tunnel_node->phy_type].if_name);
  }
  else
  {
    snprintf(local_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s.%d",
                     this->physical_iface_name[tunnel_node->phy_type].if_name,
                     vlan_id);
  }
  root = xml_file.child(System_TAG).child(L2TPXMLCfg_TAG);

  child = root.append_child(L2TPTunnel_TAG);

  subchild = child.append_child(L2TPTunnelIface_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(local_iface);

  snprintf(data,sizeof(data),"%d",tunnel_node->local_tunnel_id);
  subchild = child.append_child(L2TPTunnelID_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  snprintf(data,sizeof(data),"%d",tunnel_node->peer_tunnel_id);
  subchild = child.append_child(L2TPPeerTunnelID_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  if (tunnel_node->ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    snprintf(data,sizeof(data),"v4");
    subchild = child.append_child(L2TPTunnelIPVer_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(data);

    inet_ntop(AF_INET,&tunnel_node->peer_ipv4_addr,v4_str,INET_ADDRSTRLEN);
    subchild = child.append_child(L2TPTunnelRemoteIP_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(v4_str);

  }
  else if (tunnel_node->ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    snprintf(data,sizeof(data),"v6");
    subchild = child.append_child(L2TPTunnelIPVer_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(data);

    inet_ntop(AF_INET6,&tunnel_node->peer_ipv6_addr.s6_addr,v6_str,
                                                   INET6_ADDRSTRLEN);
    subchild = child.append_child(L2TPTunnelRemoteIP_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(v6_str);

  }
  else
  {
    LOG_MSG_ERROR("AddL2TPNodeFromXML: "
                  " Invalid IP version in tunnel node.",0,0,0);
  }

  if (tunnel_node->proto == QCMAP_MSGR_L2TP_ENCAP_IP_V01)
  {
    snprintf(data,sizeof(data),"IP");
    subchild = child.append_child(L2TPTunnelEncapProto_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(data);
  }
  else if (tunnel_node->proto == QCMAP_MSGR_L2TP_ENCAP_UDP_V01)
  {
    snprintf(data,sizeof(data),"UDP");
    subchild = child.append_child(L2TPTunnelEncapProto_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(data);

    temp_val = ntohs(tunnel_node->local_udp_port);
    snprintf(data,sizeof(data),"%d",temp_val);
    subchild = child.append_child(L2TPTunnelUDPPort_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(data);

    temp_val = ntohs(tunnel_node->peer_udp_port);
    snprintf(data,sizeof(data),"%d",temp_val);
    subchild = child.append_child(L2TPTunnelPeerUDPPort_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(data);
  }

  if (NULL != tunnel_node->session_list.L2TP_SessionListHead)
  {
    ll_node = tunnel_node->session_list.L2TP_SessionListHead;
    ll_node = ds_dll_next (ll_node, (const void**)(&session_config));
    while (ll_node \
            && \
            ll_node != tunnel_node->session_list.L2TP_SessionListHead)
    {
      if (NULL == session_config)
      {
        LOG_MSG_ERROR("ConfigureL2TP - Invalid session config retirved.",\
                                                                    0,0,0);
        break;
      }

      subchild1 = child.append_child(L2TPSession_Tag);
      snprintf(data,sizeof(data),"%d",session_config->sessionConfig.session_id);
      subchild2 = subchild1.append_child(L2TPSessionID_Tag);
      subchild2.append_child(pugi::node_pcdata).set_value(data);

      snprintf(data,sizeof(data),"%d",
                              session_config->sessionConfig.peer_session_id);
      subchild2 = subchild1.append_child(L2TPSessionPeerID_Tag);
      subchild2.append_child(pugi::node_pcdata).set_value(data);

      ll_node = ds_dll_next (ll_node, (const void**)(&session_config));
    }/* end of while*/
  }

  xml_file.save_file(TEMP_L2TP_CFG_XML);
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_L2TP_CFG_XML);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_L2TP_CFG_XML ,
              QCMAP_L2TP::L2TP_xml_path);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_L2TP_CFG_XML);
  ds_system_call(command, strlen(command));

  QCMAP_L2TP::num_c_tunnel++;
  return;
}
/*===========================================================================
  FUNCTION AddL2TPTunnelEntryToList
==========================================================================*/
/*!
@brief
  Adds L2TP Tunnel Entry to list

@parameters
  qcmap_L2TP_Tunnel_list_item_t tunnel_config_info,
  qcmap_L2TP_Tunnel_list_item_t *tunnel_config,

@return
  fasle- on failure
  true - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::AddL2TPTunnelEntryToList
(
  qcmap_L2TP_Tunnel_list_item_t tunnel_config_info,
  qcmap_L2TP_Tunnel_list_item_t *tunnel_config
)
{

  LOG_MSG_INFO1("Entering AddL2TPTunnelEntryToList",0,0,0);
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  ds_dll_el_t * node = NULL, *temp_node = NULL, *search_node = NULL;
/*------------------------------------------------------------------------*/
  if (tunnel_config_info.phy_type == QCMAP_TETH_MIN)
  {
    LOG_MSG_ERROR("AddL2TPTunnelEntryToList - Invalid phy type",0,0,0);
    return false;
  }

  tunnel_list = &QCMAP_L2TP::tunnel_list;

  if (tunnel_list->L2TP_TunnelListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
          information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddL2TPTunnelEntryToList - Error in allocating"
                    " memory for node",0,0,0);
      return false;
    }
    tunnel_list->L2TP_TunnelListHead = node;
  }

  tunnel_config = (qcmap_L2TP_Tunnel_list_item_t*)ds_malloc(
                                    sizeof(qcmap_L2TP_Tunnel_list_item_t));
  if( tunnel_config == NULL )
  {
    LOG_MSG_ERROR("AddL2TPTunnelEntryToList - Error in allocating memory for"
                 "Session Config entry",0,0,0);
    return false;
  }


  memset(tunnel_config, 0, sizeof(qcmap_L2TP_Tunnel_list_item_t));

  memcpy(tunnel_config,&tunnel_config_info,
                                 sizeof(qcmap_L2TP_Tunnel_list_item_t));

  //Store the Tunnel entry in the linked list
  if ((node = ds_dll_enq(tunnel_list->L2TP_TunnelListHead,
                         NULL, (void*)tunnel_config)) == NULL)
  {
    LOG_MSG_ERROR("AddL2TPTunnelEntryToList - Error in adding a node",0,0,0);
    ds_free(tunnel_config);
    return false;
  }
  tunnel_list->L2TP_TunnelListTail = node;

  return true;
}

/*===========================================================================
  FUNCTION AddL2TPSessionEntryToList
==========================================================================*/
/*!
@brief
  Adds L2TP Session Entry to list

@parameters
  qcmap_L2TP_Session_list_item_t session_config_info,
  qcmap_L2TP_Session_list_item_t *session_config,
  qcmap_L2TP_Session_info_list_t *session_list

@return
  fasle- on failure
  true - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::AddL2TPSessionEntryToList
(
  qcmap_L2TP_Session_list_item_t session_config_info,
  qcmap_L2TP_Session_list_item_t *session_config,
  qcmap_L2TP_Session_info_list_t *session_list
)
{

  LOG_MSG_INFO1("Entering AddL2TPSessionEntryToList",0,0,0);
  ds_dll_el_t * node = NULL, *temp_node = NULL;
/*------------------------------------------------------------------------*/

  if( session_list == NULL )
  {
    LOG_MSG_ERROR("AddL2TPSessionEntryToList: Device list passed is NULL",0,0,0);
    return false;
  }

  if (session_list && session_list->L2TP_SessionListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
          information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddL2TPSessionEntryToList - Error in allocating"
                    " memory for node",0,0,0);
      return false;
    }
    session_list->L2TP_SessionListHead = node;
  }

  session_config = (qcmap_L2TP_Session_list_item_t *)ds_malloc(
                                    sizeof(qcmap_L2TP_Session_list_item_t));
  if( session_config == NULL )
  {
    LOG_MSG_ERROR("AddL2TPSessionEntryToList - Error in allocating memory for"
                 "Session Config entry",0,0,0);
    return false;
  }


  memset(session_config, 0, sizeof(qcmap_L2TP_Session_list_item_t));

  memcpy(session_config,&session_config_info,
                                 sizeof(qcmap_L2TP_Session_list_item_t));

  //Store the Session entry in the linked list
  if ((node = ds_dll_enq(session_list->L2TP_SessionListHead,
                         NULL, (void*)session_config)) == NULL)
  {
    LOG_MSG_ERROR("AddL2TPSessionEntryToList - Error in adding a node",0,0,0);
    ds_free(session_config);
    return false;
  }
  session_list->L2TP_SessionListTail = node;

  return true;

}
/*===========================================================================
  FUNCTION AddSessionIDEntryToList
==========================================================================*/
/*!
@brief
  Adds Session ID entry to list

@parameters
  uint32 session_id,
  uint32 *session_id_node

@return
  fasle- on failure
  true - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::AddSessionIDEntryToList
(
  uint32 session_id,
  uint32 *session_id_node
)
{

  LOG_MSG_INFO1("Entering AddSessionEntryToList %d",session_id,0,0);
  ds_dll_el_t * node = NULL, *temp_node = NULL;

  qcmap_session_id_info_list_t* sessionIDList =
                         &(QCMAP_L2TP::sessionIDList);

  if (sessionIDList->SessionIDHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
          information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddSessionIDEntryToList - Error in allocating memory"
                    " for node",0,0,0);
      return false;
    }
    sessionIDList->SessionIDHead = node;
  }

  session_id_node = (uint32*)ds_malloc(
                                    sizeof(uint32));
  if( session_id_node == NULL )
  {
    LOG_MSG_ERROR("AddSessionIDEntryToList - Error in allocating memory for"
                 "Session ID entry",0,0,0);
    return false;
  }


  memset(session_id_node, 0, sizeof(uint32));

  *session_id_node = session_id;

  //Store the Session ID entry in the linked list
  if ((node = ds_dll_enq(sessionIDList->SessionIDHead,
                         NULL, (void*)session_id_node)) == NULL)
  {
    LOG_MSG_ERROR("AddSessionIDEntryToList - Error in adding a node",0,0,0);
    ds_free(session_id_node);
    return false;
  }
  sessionIDList->SessionIDTail = node;

  return true;

}
/*===========================================================================
  FUNCTION CleanupL2TPTunnelConfig
==========================================================================*/
/*!
@brief
  Clean up tunnel, sessio, tunnel ID's and session ID's from given l2tp config.

@parameters
 qcmap_L2TP_Tunnel_list_item_t *tunnel_config

@return
  true on success
  false otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::CleanupL2TPTunnelConfig
(
  qcmap_L2TP_Tunnel_list_item_t *tunnel_config
)
{
  ds_dll_el_t *session_node = NULL, *search_node = NULL;
  qcmap_L2TP_Session_list_item_t *session_config = NULL;
  qcmap_session_id_info_list_t* sessionIDList =
                         &(QCMAP_L2TP::sessionIDList);
/*------------------------------------------------------------------------*/
  if (NULL == tunnel_config)
  {
    LOG_MSG_ERROR("CleanupL2TPTunnelConfig - Invalid tunnel config",0,0,0);
    return false;
  }
  session_node =  tunnel_config->session_list.L2TP_SessionListHead;
  session_node = ds_dll_next (session_node, (const void**)(&session_config));
  while (session_node \
          && \
          session_node != tunnel_config->session_list.L2TP_SessionListHead)
  {
    search_node = ds_dll_delete (
                            sessionIDList->SessionIDHead,
                           &(sessionIDList->SessionIDTail),
                           (void*)&(session_config->sessionConfig.session_id),
                           qcmap_match_session_id);

    if (NULL == search_node)
    {
      LOG_MSG_ERROR("CleanupL2TPTunnelConfig - Error deleting session"
                    " ID %d",session_config->sessionConfig.session_id,0,0);
      return false;
    }
    QCMAP_L2TP::sessionIndex[session_config->SessionIndex] = 0;

    ds_dll_free(search_node);
    session_node = ds_dll_next (session_node, (const void**)(&session_config));

  }

  search_node = NULL;

  ds_dll_delete_all(tunnel_config->session_list.L2TP_SessionListHead);

  this->DelL2TPNodeFromXML(tunnel_config);
  return true;
}
/*===========================================================================
  FUNCTION ConfigDelMTUSizeOnPhyVLANiface
==========================================================================*/
/*!
@brief
  Configure/Deletes iptable rules to clamp TCP MSS

@parameters
 boolean config
 char *iface_name

@return
  true on success
  false otherwise

@note
- Dependencies
    This is currently supported only for Ethernet interface over neutrino.

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::ConfigDelMTUSizeOnPhyVLANiface
(
  boolean config,
  QCMAP_Virtual_LAN_phy_iface phy_type,
  int16_t                          vlan_id
)
{
  char command[MAX_L2TP_COMMAND_LEN] = {0};
  char iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  int mtu_size = 0;
/*------------------------------------------------------------------------*/
  if (QCMAP_TETH_ETH != phy_type)
  {
    LOG_MSG_ERROR("ConfigDelMTUSizeOnPhyVLANiface - "
                  "Not supported phy type %d",phy_type,0,0);
    return false;
  }

  snprintf(iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s",
           this->physical_iface_name[phy_type].if_name);

  if (config)
     mtu_size = L2TP_MTU_SIZE;
  else
     mtu_size = STD_MTU_SIZE;

  LOG_MSG_INFO1("ConfigDelMTUSizeOnPhyVLANiface - "
                  "phy type %d and toggle flag %d, config %d",phy_type,
                     QCMAP_L2TP::eth_link_mtu_set_toggle,config);

  if(!QCMAP_L2TP::eth_link_mtu_set_toggle)
  {
    if (config)
      QCMAP_L2TP::eth_link_mtu_set_toggle = true;

    snprintf(command,MAX_L2TP_COMMAND_LEN,
           "ifconfig %s mtu %d",this->physical_iface_name[phy_type].if_name,
                           mtu_size);
    ds_system_call(command, strlen(command));
    if (vlan_id != 0)
    {
      snprintf(command,MAX_L2TP_COMMAND_LEN,
           "ifconfig %s.%d mtu %d",this->physical_iface_name[phy_type].if_name,
                           vlan_id,mtu_size);
      ds_system_call(command, strlen(command));
    }
  }
  else
  {
    if (config)
    {
      QCMAP_L2TP::eth_link_mtu_set_toggle = false;
    }
  }

  return true;
}
/*===========================================================================
  FUNCTION ConfigDelL2TPTCPMSS
==========================================================================*/
/*!
@brief
  Configure/Deletes iptable rules to clamp TCP MSS

@parameters
 boolean config
 char *iface_name

@return
  true on success
  false otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::ConfigDelL2TPTCPMSS
(
  boolean config,
  char *iface_name
)
{
  char command[MAX_L2TP_COMMAND_LEN] = {0};
  char Kernel_ver[KERNEL_VERSION_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr =
                             QCMAP_ConnectionManager::Get_Instance(NULL, false);
/*------------------------------------------------------------------------*/
  if (!QcMapMgr)
  {
    LOG_MSG_ERROR("ConfigDelL2TPTCPMSS - Unable to find QcMapMgr Object",0,0,0);
    return false;
  }
  if ( false == QcMapMgr->GetKernelVer(Kernel_ver) ) {
    LOG_MSG_ERROR("Unable to get the kernel version info", 0, 0, 0);
    return false;
  }

  if (iface_name == NULL)
  {
    LOG_MSG_ERROR("ConfigDelL2TPTCPMSS - Invalid iface name recieved",0,0,0);
    return false;
  }

  if (config == true)
  {
    if(!QCMAP_L2TP::TCP_MSS_Kernel_Configs_Enabled)
    {
      snprintf(command,MAX_COMMAND_STR_LEN,
         "insmod /usr/lib/modules/%s/kernel/net/bridge/br_netfilter.ko",Kernel_ver);
      ds_system_call(command,strlen(command));
      snprintf(command,MAX_COMMAND_STR_LEN,
        "insmod /usr/lib/modules/%s/kernel/net/netfilter/xt_physdev.ko",Kernel_ver);
      ds_system_call(command,strlen(command));
      QCMAP_L2TP::TCP_MSS_Kernel_Configs_Enabled = true;
    }
    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "iptables -I FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-in %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v4_MSS_SIZE);
    ds_system_call(command, strlen(command));

    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "iptables -I FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-out %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v4_MSS_SIZE);
    ds_system_call(command, strlen(command));

    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "ip6tables -I FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-in %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v6_MSS_SIZE);
    ds_system_call(command, strlen(command));

    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "ip6tables -I FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-out %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v6_MSS_SIZE);
    ds_system_call(command, strlen(command));
  }
  else
  {
    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "iptables -D FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-in %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v4_MSS_SIZE);
    ds_system_call(command, strlen(command));

    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "iptables -D FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-out %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v4_MSS_SIZE);
    ds_system_call(command, strlen(command));

    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "ip6tables -D FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-in %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v6_MSS_SIZE);
    ds_system_call(command, strlen(command));

    snprintf(command,MAX_L2TP_COMMAND_LEN,
             "ip6tables -D FORWARD -i bridge0 -p tcp -m physdev "
             "--physdev-out %s --tcp-flags SYN,RST SYN -j "
             "TCPMSS --set-mss %d",iface_name,L2TP_v6_MSS_SIZE);
    ds_system_call(command, strlen(command));
  }

  return true;

}
/*===========================================================================
  FUNCTION ConstructTunnelNode
==========================================================================*/
/*!
@brief
  Constructs VLAN info based on iface and vlan_id

@parameters
  qcmap_msgr_l2tp_config_v01 tunnel_config,
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node

@return
   false - on failure
   true - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_L2TP::ConstructTunnelNode
(
  qcmap_msgr_l2tp_config_v01 tunnel_config,
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node
)
{
  qcmap_L2TP_Session_list_item_t session_config_info;
  qcmap_L2TP_Session_list_item_t *session_config;
/*------------------------------------------------------------------------*/
  memset(&session_config_info,0,sizeof(session_config_info));
  if (NULL == tunnel_node)
  {
    LOG_MSG_ERROR("ConstructTunnelNode(),NULL tunnel node sent"\
                    ,0,0,0);
    return false;
  }

  tunnel_node->local_tunnel_id = tunnel_config.local_tunnel_id;
  tunnel_node->peer_tunnel_id  = tunnel_config.peer_tunnel_id;
  tunnel_node->ip_family       = tunnel_config.ip_family;
  tunnel_node->local_udp_port  = tunnel_config.local_udp_port;
  tunnel_node->peer_udp_port   = tunnel_config.peer_udp_port;
  tunnel_node->peer_ipv4_addr  = tunnel_config.peer_ipv4_addr;
  tunnel_node->proto           = tunnel_config.proto;
  memcpy(&tunnel_node->peer_ipv6_addr, &tunnel_config.peer_ipv6_addr,
                                              sizeof(struct in6_addr ));

  for (int i = 0; i < QCMAP_MSGR_L2TP_MAX_SESSION_PER_TUNNEL_V01; i++)
  {
    /*Session ID 0 means there is no session at this index*/
    if (tunnel_config.session_config[i].session_id != 0)
    {
      session_config_info.sessionConfig.session_id =
                           tunnel_config.session_config[i].session_id;
      session_config_info.sessionConfig.peer_session_id =
                           tunnel_config.session_config[i].peer_session_id;

      tunnel_node->num_conf_sessions = tunnel_node->num_conf_sessions+1;
      if (!this->AddL2TPSessionEntryToList(\
                           session_config_info,
                           session_config,
                           &tunnel_node->session_list))
      {
        LOG_MSG_ERROR("Error in adding session:%d "
                       "to list",tunnel_config.session_config[i].session_id,
                                                                      0,0);
        return false;
      }
    }
  }
  return true;
}
/*=====================================================
  FUNCTION CopyTunnelConfig
======================================================*/
/*!
@brief
  Iterates through tunnel list item and copies tunnel config.

@parameters
   qcmap_msgr_l2tp_config_v01 *dest_l2tp,
   qcmap_L2TP_Tunnel_list_item_t *src_l2tp

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

boolean QCMAP_L2TP::CopyTunnelConfig
(
  qcmap_msgr_l2tp_config_v01 *dest_l2tp,
  qcmap_L2TP_Tunnel_list_item_t *src_l2tp
)
{
  char iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  ds_dll_el_t * node = NULL;
  qcmap_L2TP_Session_list_item_t *session_config;
  int num_sessions=0;
/*------------------------------------------------------------------------*/
  if (NULL == dest_l2tp || NULL == src_l2tp)
  {
     LOG_MSG_ERROR("CopyTunnelConfig - Invalid tunnel config",0,0,0);
     return false;
  }

  if (src_l2tp->vlan_id == 0)
  {
    snprintf(iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s",
           this->physical_iface_name[src_l2tp->phy_type].if_name);
    strlcpy(dest_l2tp->local_iface,iface_name,
                     QCMAP_MAX_IFACE_NAME_SIZE_V01);
  }
  else
  {
    snprintf(iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s.%d",
            this->physical_iface_name[src_l2tp->phy_type].if_name,
             src_l2tp->vlan_id);
    strlcpy(dest_l2tp->local_iface,iface_name,
                     QCMAP_MAX_IFACE_NAME_SIZE_V01);
  }
  dest_l2tp->ip_family = src_l2tp->ip_family;
  dest_l2tp->local_tunnel_id = src_l2tp->local_tunnel_id;
  dest_l2tp->peer_tunnel_id = src_l2tp->peer_tunnel_id;
  dest_l2tp->peer_ipv4_addr = src_l2tp->peer_ipv4_addr;
  memcpy((&dest_l2tp->peer_ipv6_addr),&src_l2tp->peer_ipv6_addr,
                 sizeof(dest_l2tp->peer_ipv6_addr));
  dest_l2tp->proto = src_l2tp->proto;
  dest_l2tp->local_udp_port = src_l2tp->local_udp_port;
  dest_l2tp->peer_udp_port = src_l2tp->peer_udp_port;

  node = src_l2tp->session_list.L2TP_SessionListHead;
  node = ds_dll_next (node, (const void**)(&session_config));
  while (node \
          && \
          node != src_l2tp->session_list.L2TP_SessionListHead)
  {
    if (NULL == session_config)
    {
      LOG_MSG_ERROR("CopyTunnelConfig - Invalid session config retirved.",\
                                                                  0,0,0);
      return false;
    }
    if (num_sessions == QCMAP_MSGR_L2TP_MAX_SESSION_PER_TUNNEL_V01)
    {
      LOG_MSG_ERROR("CopyTunnelConfig - Max sessions per tunnel reached.",\
                                                                  0,0,0);
      return false;
    }
    dest_l2tp->session_config[num_sessions].session_id =
                                  session_config->sessionConfig.session_id;
    dest_l2tp->session_config[num_sessions].peer_session_id =
                                  session_config->sessionConfig.peer_session_id;
    num_sessions++;
    node = ds_dll_next (node, (const void**)(&session_config));
  }
  return true;
}
/*=====================================================
  FUNCTION DeleteAllTunnels
======================================================*/
/*!
@brief
  Uninstall's(if phy link is up) and Deletes config of all tunnels in the
  system.

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
/*====================================================*/

boolean QCMAP_L2TP::DeleteAllTunnels()
{
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  ds_dll_el_t * node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  qcmap_L2TP_Tunnel_list_item_t l2tp_config;
/*------------------------------------------------------------------------*/

  tunnel_list = &QCMAP_L2TP::tunnel_list;
  node = tunnel_list->L2TP_TunnelListHead;
  node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  LOG_MSG_INFO1("DeleteAllTunnels - entering function",0,0,0);
  while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
  {
    if (NULL == l2tp_config_node)
    {
      LOG_MSG_ERROR("DeleteAllTunnels - Invalid tunnel config",0,0,0);
      return false;
    }
    if (l2tp_config_node->is_installed)
    {
      this->RemoveL2TP(*l2tp_config_node);
      l2tp_config_node->is_installed = false;
    }
    else
    {
      LOG_MSG_ERROR("DeleteAllTunnels - Tunnel:%d found, but not installed",
                                      l2tp_config_node->local_tunnel_id,0,0);
    }
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  }
  this->FreeTunnelList(tunnel_list);

  return true;
}
/*=====================================================
  FUNCTION DelL2TPNodeFromXML
======================================================*/
/*!
@brief
  Deletes L2TP node from XML.

@parameters
qcmap_L2TP_Tunnel_list_item_t *tunnel_node

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

void QCMAP_L2TP::DelL2TPNodeFromXML
(
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  int16_t vlan_id = 0;
  char local_iface[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  char command[MAX_COMMAND_STR_LEN] = {0};
/*------------------------------------------------------------------------*/
  if (NULL == tunnel_node)
  {
    LOG_MSG_ERROR("NULL tunnel node passed.",0,0,0);
    return;
  }

  if(!xml_file.load_file(QCMAP_L2TP::L2TP_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return;
  }
  vlan_id = tunnel_node->vlan_id;
  if (vlan_id == 0)
  {
    snprintf(local_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s",
                     this->physical_iface_name[tunnel_node->phy_type].if_name);
  }
  else
  {
    snprintf(local_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s.%d",
                       this->physical_iface_name[tunnel_node->phy_type].if_name,
                       vlan_id);
  }
  root =
       xml_file.child(System_TAG).child(L2TPXMLCfg_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (strncmp(child.name(),L2TPTunnel_TAG,strlen(L2TPTunnel_TAG)))
    {
      LOG_MSG_ERROR("Invalid node encountered.",0,0,0);
      return;
    }

    if (!strncmp(child.child(L2TPTunnelIface_TAG).child_value(),local_iface,
                                                           strlen(local_iface))
        &&
        tunnel_node->local_tunnel_id ==
                      atoi(child.child(L2TPTunnelID_TAG).child_value()))
    {
      root.remove_child(child);
    }
  }

  xml_file.save_file(TEMP_L2TP_CFG_XML);
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_L2TP_CFG_XML);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_L2TP_CFG_XML ,
              QCMAP_L2TP::L2TP_xml_path);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_L2TP_CFG_XML);
  ds_system_call(command, strlen(command));
  QCMAP_L2TP::num_c_tunnel--;
}
/*===========================================================================
  FUNCTION findLeastAvailSessionIndex
==========================================================================*/
/*!
@brief
  Finds least available session index

@parameters
  none

@return
  true on success
  false otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::FindLeastAvailSessionIndex
(
  uint8 *sessionIndex
)
{
/*------------------------------------------------------------------------*/
  if (sessionIndex == NULL)
  {
    LOG_MSG_ERROR("FindLeastAvailSessionIndex - "
                  "Invalid session config retirved.",0,0,0);
    return false;
  }

  for (int i = 0; i < MAX_SESSION_INDEX; i++)
  {
     if (QCMAP_L2TP::sessionIndex[i] == 0)
     {
        *sessionIndex = (uint8) i;
        QCMAP_L2TP::sessionIndex[i] = 1;
        return true;
     }
  }
  LOG_MSG_ERROR("FindLeastAvailSessionIndex - Max session Indexes reached.",\
                                                                   0,0,0);
  return false;
}
/*=====================================================
  FUNCTION FreeTunnelList
======================================================*/
/*!
@brief
  Free tunnels and sessions in the given tunnel list.

@parameters
qcmap_L2TP_Tunnel_info_list_t *tunnel_list

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

void QCMAP_L2TP::FreeTunnelList
(
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list
)
{
  ds_dll_el_t * node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
/*------------------------------------------------------------------------*/

  node = tunnel_list->L2TP_TunnelListHead;
  node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
  {
    if (NULL == l2tp_config_node)
    {
      LOG_MSG_ERROR("FreeTunnelList - Invalid tunnel config",\
                                                                       0,0,0);
      return false;
    }

    ds_dll_delete_all(l2tp_config_node->session_list.L2TP_SessionListHead);

    l2tp_config_node = NULL;
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  }
  ds_dll_delete_all(tunnel_list->L2TP_TunnelListHead);
}
/*=====================================================
  FUNCTION GetTunnelConfigFromList
======================================================*/
/*!
@brief
  Iterates through tunnel list and copies tunnel config.

@parameters
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list,
  qcmap_msgr_l2tp_config_v01 *l2tp_config,
  uint32                     *num_tunnels

@return
  qmi_error_type - error number

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/

qmi_error_type_v01 QCMAP_L2TP::GetTunnelConfigFromList
(
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list,
  qcmap_msgr_l2tp_config_v01 *l2tp_config,
  uint32                     *num_tunnels
)
{
  ds_dll_el_t * node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  qcmap_msgr_l2tp_config_v01 temp_l2tp_config;
/*------------------------------------------------------------------------*/
  if (NULL == tunnel_list || NULL == l2tp_config || NULL == num_tunnels)
  {
    LOG_MSG_ERROR("GetTunnelConfigFromList - Invalid tunnel list.",\
                                                                  0,0,0);
    return QMI_ERR_INTERNAL_V01;
  }

  *num_tunnels = 0;
  memset(&temp_l2tp_config,0,sizeof(qcmap_msgr_l2tp_config_v01));

  node = tunnel_list->L2TP_TunnelListHead;
  node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  while (node \
       && \
       node != tunnel_list->L2TP_TunnelListHead)
  {
    LOG_MSG_ERROR("ConfigDelL2TPTunnelsInList - Tunnel config found",\
                                                                     0,0,0);
    if (NULL == l2tp_config_node)
    {
      LOG_MSG_ERROR("ConfigDelL2TPTunnelsInList - Invalid tunnel config",\
                                                                     0,0,0);
      return QMI_ERR_INTERNAL_V01;
    }
    if (*num_tunnels == QCMAP_MSGR_L2TP_MAX_TUNNELS_V01)
    {
      LOG_MSG_ERROR("ConfigDelL2TPTunnelsInList - "
                    "Max tunnels per session reached",0,0,0);
      return QMI_ERR_NO_MEMORY_V01;
    }
    if (l2tp_config_node->num_conf_sessions >
             QCMAP_MSGR_L2TP_MAX_SESSION_PER_TUNNEL_V01)
    {
      LOG_MSG_ERROR("ConfigDelL2TPTunnelsInList - "
                    "Max sessions per tunnel reached",0,0,0);
      return QMI_ERR_NO_MEMORY_V01;
    }
    if(!(this->CopyTunnelConfig(&temp_l2tp_config,
                                        l2tp_config_node)))
    {
      LOG_MSG_ERROR("ConfigDelL2TPTunnelsInList - "
                    "Error copying tunnel config",0,0,0);
      return QMI_ERR_INTERNAL_V01;
    }

    memcpy(&l2tp_config[*num_tunnels],&temp_l2tp_config,
           sizeof(qcmap_msgr_l2tp_config_v01));
    *num_tunnels = (*num_tunnels)+1;
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  }
  LOG_MSG_INFO1("ConfigDelL2TPTunnelsInList - "
                    "returning num tunnels %d",*num_tunnels,0,0);
  return QMI_ERR_NONE_V01;
}
/*===========================================================================
  FUNCTION InstallDelTunnelsOnPhyIface
==========================================================================*/
/*!
@brief
  Installs/Deletes L2TP Tunnels and sessions on given phy interface.

@parameters
  char *iface_name,
  boolean install

@return
  true on success
  false otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::InstallDelTunnelsOnPhyIface
(
  char *iface_name,
  boolean install
)
{
  QCMAP_Virtual_LAN_phy_iface phy_type = QCMAP_TETH_MIN;
  int16_t vlan_id = 0;
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  ds_dll_el_t * node = NULL;
/*------------------------------------------------------------------------*/
  if(!(QCMAP_Virtual_LAN::GetPhyIfaceVLANIDFromIface(iface_name,\
                                                  &phy_type,&vlan_id)))
  {
    LOG_MSG_ERROR("InstallDelTunnelsOnPhyIface() - Invalid local_iface: %s"
                  " found in L2TP config",iface_name,0,0);
    return false;
  }

  if (vlan_id != 0)
  {
    LOG_MSG_ERROR("InstallDelTunnelsOnPhyIface() - Invalid iface %s passed",
                  iface_name,0,0);
    return false;
  }

  if(VALIDATE_PHY_TYPE(phy_type))
  {
    LOG_MSG_ERROR("phy_type is Not a valid QCMAP_Virtual_LAN_phy_iface phy_type value: %d",phy_type,0,0);
    return false;
  }

  this->is_iface_up[phy_type] = install;
  tunnel_list = &(QCMAP_L2TP::tunnel_list);
  node = tunnel_list->L2TP_TunnelListHead;
  node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
  {
    if (NULL == l2tp_config_node)
    {
      LOG_MSG_ERROR("InstallDelTunnelsOnPhyIface() - "
                    "Invalid tunnel config",0,0,0);
      return false;
    }

    if (l2tp_config_node->phy_type == phy_type
        &&
        l2tp_config_node->vlan_id == 0)
    {
      if (install)
      {
        if(this->InstallL2TP(*l2tp_config_node))
        {
          l2tp_config_node->is_installed = true;
        }
        else
        {
          LOG_MSG_ERROR("InstallDelTunnelsOnPhyIface() - error in installing "
                      "tunnel id %d",l2tp_config_node->local_tunnel_id,0,0);
          l2tp_config_node->is_installed = false;
        }
      }
      else
      {
        if(this->RemoveL2TP(*l2tp_config_node))
        {
          l2tp_config_node->is_installed = false;
        }
        else
        {
          LOG_MSG_ERROR("InstallDelTunnelsOnPhyIface() - error in removing "
                      "tunnel id %d",l2tp_config_node->local_tunnel_id,0,0);
          l2tp_config_node->is_installed = true;
        }
      }
    }
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  }
  return true;
}
/*===========================================================================
  FUNCTION InstallDelTunnelsOnVLANIface
==========================================================================*/
/*!
@brief
  Installs/Deletes L2TP Tunnels and sessions on given vlan iface.

@parameters
  char *iface_name,
  boolean install

@return
  true on success
  false otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::InstallDelTunnelsOnVLANIface
(
  char *iface_name,
  boolean install
)
{
  QCMAP_Virtual_LAN_phy_iface phy_type = QCMAP_TETH_MIN;
  int16_t vlan_id = 0;
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  ds_dll_el_t * node = NULL;
/*------------------------------------------------------------------------*/
  if(!(QCMAP_Virtual_LAN::GetPhyIfaceVLANIDFromIface(iface_name,\
                                                  &phy_type,&vlan_id)))
  {
    LOG_MSG_ERROR("InstallDelTunnelsOnVLANIface() - Invalid local_iface: %s"
                  " found in L2TP config",iface_name,0,0);
    return false;
  }

  if (vlan_id == 0)
  {
    LOG_MSG_ERROR("InstallDelTunnelsOnVLANIface() - Invalid iface %s passed "
                  "for vlan",iface_name,0,0);
    return false;
  }

  LOG_MSG_INFO1("InstallDelTunnelsOnVLANIface() - InstallDelTunnelsOnVLANIface"
                " iface %s passed ",iface_name,0,0);
  tunnel_list = &(QCMAP_L2TP::tunnel_list);
  node = tunnel_list->L2TP_TunnelListHead;
  node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
  {
    if (NULL == l2tp_config_node)
    {
      LOG_MSG_ERROR("InstallDelTunnelsOnVLANIface() - "
                    "Invalid tunnel config",0,0,0);
      return false;
    }

    if (l2tp_config_node->phy_type == phy_type
        &&
        l2tp_config_node->vlan_id == vlan_id)
    {
      if (install)
      {
        if(this->InstallL2TP(*l2tp_config_node))
        {
          l2tp_config_node->is_installed = true;
        }
        else
        {
          LOG_MSG_ERROR("InstallDelTunnelsOnPhyIface() - error in installing "
                      "tunnel id %d",l2tp_config_node->local_tunnel_id,0,0);
          l2tp_config_node->is_installed = false;
        }
      }
      else
      {
        if(this->RemoveL2TP(*l2tp_config_node))
        {
          l2tp_config_node->is_installed = false;
        }
        else
        {
          LOG_MSG_ERROR("InstallDelTunnelsOnPhyIface() - error in removing "
                      "tunnel id %d",l2tp_config_node->local_tunnel_id,0,0);
          l2tp_config_node->is_installed = true;
        }
      }
    }
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  }
  return true;
}
/*===========================================================================
  FUNCTION InstallL2TP
==========================================================================*/
/*!
@brief
  Installs L2TP Tunnel and Session.

@parameters
 qcmap_L2TP_Tunnel_list_item_t l2tp_config_node

@return
  true on success
  false otherwise

@note
- Dependencies
  Make sure to call this function only
  if underlying phy interface is up.
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::InstallL2TP
(
  qcmap_L2TP_Tunnel_list_item_t l2tp_config_node
)
{
  char command[MAX_L2TP_COMMAND_LEN] = {0};
  char tmp_cmd[MAX_L2TP_COMMAND_LEN] = {0};
  char ipv6_addr_str[INET6_ADDRSTRLEN] = {0};
  char ipv4_addr_str[INET_ADDRSTRLEN] = {0};
  char peer_ipv6_addr_str[INET6_ADDRSTRLEN] = {0};
  char peer_ipv4_addr_str[INET_ADDRSTRLEN] = {0};
  char phy_iface[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  uint32                      ipv4_addr = 0;
  struct in6_addr             ipv6_addr;
  int16_t                     vlan_id = 0;
  QCMAP_Virtual_LAN_phy_iface phy_type = QCMAP_TETH_MIN;
  qcmap_L2TP_Session_info_list_t *sessionList = NULL;
  ds_dll_el_t * node = NULL, *search_node = NULL;
  qcmap_L2TP_Session_list_item_t *session_config;
  char iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  uint8 sessionIndex = 0;
  QCMAP_Virtual_LAN* QcMapVLANMgr=
                                  QCMAP_Virtual_LAN::Get_Instance(false);
  qcmap_ip4_addr_subnet_mask_v01 ipv4_struct_addr;
  qcmap_ip6_addr_prefix_len_v01  ipv6_struct_addr;
/*------------------------------------------------------------------------*/
  phy_type = l2tp_config_node.phy_type;
  vlan_id = l2tp_config_node.vlan_id;

  if (VALIDATE_PHY_TYPE(phy_type))
  {
    LOG_MSG_ERROR("InstallL2TP() Not a valid iface_type",0,0,0);
    return false;
  }


  memset(&ipv4_struct_addr,0,sizeof(qcmap_ip4_addr_subnet_mask_v01));
  memset(&ipv6_struct_addr,0,sizeof(qcmap_ip6_addr_prefix_len_v01));
  if (QCMAP_TETH_MIN == phy_type)
  {
    LOG_MSG_ERROR("InstallL2TP - Invalid phy type found in tunnel config",\
                                                                       0,0,0);
    return false;
  }
  if (vlan_id == 0)
  {
    /*Get IPv4 and IPv6 addresses for phy iface*/
    if(!QcMapVLANMgr->GetIPAddrOfPhyLink(phy_type,&ipv4_addr,&ipv6_addr))
    {
      LOG_MSG_ERROR("InstallL2TP - Error getting IP address of phy iface",\
                                                                       0,0,0);
      return false;
    }
  }
  else
  {
    /*Get IPv4 and IPv6 addresses from specfic vlan iface.
      since vlan_id is unique, only VLAN ID is passed*/
    if(!QCMAP_Virtual_LAN::GetIPAddrForVLAN(vlan_id,&ipv4_struct_addr,
                                            &ipv6_struct_addr))
    {
      LOG_MSG_ERROR("InstallL2TP - Error getting IP address of vlan iface",\
                                                                       0,0,0);
      return false;
    }
    ipv4_addr = ipv4_struct_addr.addr;
    for (int i =0; i<QCMAP_MSGR_IPV6_ADDR_LEN_V01;i++)
    {
      ipv6_addr.s6_addr[i] = ipv6_struct_addr.addr[i];
    }
  }

  if (l2tp_config_node.ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    if(!inet_ntop(AF_INET, &ipv4_addr, ipv4_addr_str,INET_ADDRSTRLEN))
    {
      LOG_MSG_ERROR("InstallL2TP - Invalid IPV4 addr in tunnel config",\
                                                                      0,0,0);
      return false;
    }
    if(!inet_ntop(AF_INET, &l2tp_config_node.peer_ipv4_addr,
                                           peer_ipv4_addr_str,INET_ADDRSTRLEN))
    {
      LOG_MSG_ERROR("InstallL2TP - Invalid Peer IPV4 addr in tunnel config",\
                                                                      0,0,0);
      return false;
    }
  }
  else if (l2tp_config_node.ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    if(!inet_ntop(AF_INET6, &ipv6_addr.s6_addr, ipv6_addr_str,INET6_ADDRSTRLEN))
    {
      LOG_MSG_ERROR("InstallL2TP - Invalid IPV6 addr in tunnel config",\
                                                                      0,0,0);
      return false;
    }
    if(!inet_ntop(AF_INET6, &l2tp_config_node.peer_ipv6_addr.s6_addr,\
                                          peer_ipv6_addr_str,INET6_ADDRSTRLEN))
    {
      LOG_MSG_ERROR("InstallL2TP - Invalid Peer IPV6 addr in tunnel config",\
                                                                      0,0,0);
      return false;
    }
  }
  else
  {
    LOG_MSG_ERROR("InstallL2TP - Invalid IP Family in tunnel config",\
                                                                    0,0,0);
    return false;
  }

  /*Form Tunnel create command*/
  strlcat(command,"ip l2tp add tunnel ",MAX_L2TP_COMMAND_LEN);

  snprintf(tmp_cmd,MAX_L2TP_COMMAND_LEN,"tunnel_id %d peer_tunnel_id %d ",
                                             l2tp_config_node.local_tunnel_id,
                                             l2tp_config_node.peer_tunnel_id);
  strlcat(command,tmp_cmd,MAX_L2TP_COMMAND_LEN);


  snprintf(tmp_cmd,MAX_L2TP_COMMAND_LEN,"local %s remote %s ",
           (l2tp_config_node.ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01) ?\
                                                  ipv4_addr_str: ipv6_addr_str,
           (l2tp_config_node.ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01) ?\
              peer_ipv4_addr_str: peer_ipv6_addr_str);
  strlcat(command,tmp_cmd,MAX_L2TP_COMMAND_LEN);

  if (l2tp_config_node.proto == QCMAP_MSGR_L2TP_ENCAP_IP_V01)
  {
    strlcat(command,"encap ip",MAX_L2TP_COMMAND_LEN);
  }
  else if (l2tp_config_node.proto == QCMAP_MSGR_L2TP_ENCAP_UDP_V01)
  {
    snprintf(tmp_cmd,MAX_L2TP_COMMAND_LEN,
             "encap udp udp_sport %d udp_dport %d",
             l2tp_config_node.local_udp_port, l2tp_config_node.peer_udp_port);
    strlcat(command,tmp_cmd,MAX_L2TP_COMMAND_LEN);
  }

  /*Execute L2TP tunnel create command*/
  ds_system_call(command, strlen(command));

  if ((this->NumInstalledTunnelsWithPhyType(phy_type) == 0)
      &&
      (phy_type != QCMAP_TETH_BRIDGE0))
  {
    snprintf(command,MAX_L2TP_COMMAND_LEN,"brctl delif %s %s",
                     BRIDGE_IFACE,
                     this->physical_iface_name[phy_type].if_name);
    ds_system_call(command, strlen(command));
  }

  /*Iterate through sessions and make sure we have enough sessionIndexes,
                                                               valid config*/
  sessionList = &l2tp_config_node.session_list;
  if(NULL != sessionList->L2TP_SessionListHead)
  {
    node = sessionList->L2TP_SessionListHead;
    node = ds_dll_next (node, (const void**)(&session_config));
    while (node \
            && \
            node != sessionList->L2TP_SessionListHead)
    {
      if (NULL == session_config)
      {
        LOG_MSG_ERROR("InstallL2TP - Invalid session config retirved.",\
                                                                    0,0,0);
        goto err_delete_tunnel;
      }
      if (!this->FindLeastAvailSessionIndex(&sessionIndex))
      {
        LOG_MSG_ERROR("InstallL2TP - MAX session Indexes reached.",0,0,0);
        goto err_delete_tunnel;
      }
      /*Save Session Index in Session Config*/
      session_config->SessionIndex = sessionIndex;
      node = ds_dll_next (node, (const void**)(&session_config));
    }
  }

  /*Iterate through session list and create sessions*/
  sessionList = &l2tp_config_node.session_list;
  if(NULL != sessionList->L2TP_SessionListHead)
  {
    memset(command,0,sizeof(command));
    node = sessionList->L2TP_SessionListHead;
    node = ds_dll_next (node, (const void**)(&session_config));
    while (node \
            && \
            node != sessionList->L2TP_SessionListHead)
    {
      snprintf(iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01,
               "l2tpeth%d",session_config->SessionIndex);
      snprintf(command,MAX_L2TP_COMMAND_LEN,
               "ip l2tp add session tunnel_id %d session_id "
               "%d peer_session_id %d name %s",l2tp_config_node.local_tunnel_id,
                                 session_config->sessionConfig.session_id,
                                 session_config->sessionConfig.peer_session_id,
                                 iface_name);

      ds_system_call(command, strlen(command));
#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
      if (l2tp_config_node.vlan_id == 0)
      {
        snprintf(phy_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s",
                     this->physical_iface_name[phy_type].if_name);
      }
      else
      {
        snprintf(phy_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s.%d",
                     this->physical_iface_name[phy_type].if_name,vlan_id);
      }

      /* Send IOCTL to IPA only if encap proto is IP AND phy type is eth.
      UDP encapsulation offload is not supported currently.
      IPA offload for any other end point other than ethernet
       is not supported cuurently*/
      if (phy_type == QCMAP_TETH_ETH
          &&
          l2tp_config_node.proto == QCMAP_MSGR_L2TP_ENCAP_IP_V01)
      {
        if(!this->UpdateIPAWithL2TPIOCTL(iface_name,
                              phy_iface,l2tp_config_node.ip_family,
                              session_config->sessionConfig.session_id,true))
        {
          LOG_MSG_ERROR("InstallL2TP - IOCTL to IPA failed.",0,0,0);
        }
      }

#endif
      /*Bringup the interface and add it to bridge0*/
      snprintf(command,MAX_L2TP_COMMAND_LEN,"ip link set %s up",iface_name);
      ds_system_call(command, strlen(command));

      /* Ensure DAD does not itervene */
      snprintf(command, MAX_L2TP_COMMAND_LEN ,"echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",iface_name);
      ds_system_call(command,strlen(command));

      snprintf(command,MAX_L2TP_COMMAND_LEN,"brctl addif %s %s",
                                            BRIDGE_IFACE,iface_name);
      ds_system_call(command, strlen(command));

      snprintf(command, MAX_L2TP_COMMAND_LEN ,"echo 0 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",iface_name);
      ds_system_call(command,strlen(command));

      snprintf(command,MAX_L2TP_COMMAND_LEN,"ifconfig %s mtu 1500",
                                            iface_name);
      ds_system_call(command, strlen(command));

      /*install iptable rules to clamp TCP MSS if enabled*/
      if (QCMAP_L2TP::TCP_MSS_config_enabled)
      {
        this->ConfigDelL2TPTCPMSS(true,iface_name);
      }

      node = ds_dll_next (node, (const void**)(&session_config));
      sessionIndex = 0;
    } /*end of while*/
  }

  /*Change MTU Config for underlying interfaces if enabled.*/
  if (QCMAP_L2TP::MTU_config_enabled)
  {
    if (this->NumInstalledTunnelsWithPhyType(phy_type) == 0)
    {
      LOG_MSG_ERROR("InstallL2TP() - calling MTU size.",0,0,0);
      this->ConfigDelMTUSizeOnPhyVLANiface(true,
                                         l2tp_config_node.phy_type,
                                         l2tp_config_node.vlan_id);
    }
  }


  return true;

  /*Error case: Delete tunnel and revert bridge config on phy iface.*/
  err_delete_tunnel:
  {

    snprintf(command,MAX_L2TP_COMMAND_LEN,
                 "ip l2tp del tunnel tunnel_id %d",
                                 l2tp_config_node.local_tunnel_id);
    ds_system_call(command, strlen(command));

    if (this->NumInstalledTunnelsWithPhyType(phy_type) == 0)
    {
      snprintf(command,MAX_L2TP_COMMAND_LEN,"brctl addif %s %s",
                                   BRIDGE_IFACE,
                                   this->physical_iface_name[phy_type].if_name);
      ds_system_call(command, strlen(command));
    }
    return false;
  }
}
/*===========================================================================
  FUNCTION MatchTunnelConfig
==========================================================================*/
/*!
@brief
  Compares and matches tunnel nodes

@parameters
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node1
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node2

@return
  fasle- on failure
  true - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::MatchTunnelConfig
(
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node1,
  qcmap_L2TP_Tunnel_list_item_t *tunnel_node2
)
{
   ds_dll_el_t * search_node = NULL;
   ds_dll_el_t  *node;
   qcmap_L2TP_Session_list_item_t *session1_config = NULL;
   qcmap_L2TP_Session_list_item_t *session2_config = NULL;
/*------------------------------------------------------------------------*/
  if (NULL == tunnel_node1 || NULL == tunnel_node2)
  {
    LOG_MSG_ERROR("MatchTunnelConfig - Invalid parameters",0,0,0);
    return false;
  }

  if (tunnel_node1->local_tunnel_id != tunnel_node2->local_tunnel_id)
  {
    LOG_MSG_ERROR("MatchTunnelConfig - Tunnel ID mismatch",0,0,0);
    return false;
  }

  if (tunnel_node1->peer_tunnel_id != tunnel_node2->peer_tunnel_id)
  {
    LOG_MSG_ERROR("MatchTunnelConfig - Peer Tunnel ID mismatch",0,0,0);
    return false;
  }

  if (tunnel_node1->ip_family != tunnel_node2->ip_family)
  {
    LOG_MSG_ERROR("MatchTunnelConfig - IP family mismatch",0,0,0);
    return false;
  }

  if (tunnel_node1->proto != tunnel_node2->proto)
  {
    LOG_MSG_ERROR("MatchTunnelConfig - Encap proto mismatch",0,0,0);
    return false;
  }

  if (tunnel_node1->ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    if (tunnel_node1->peer_ipv4_addr != tunnel_node2->peer_ipv4_addr)
    {
      LOG_MSG_ERROR("MatchTunnelConfig - Peer IPv4 addr mismatch",0,0,0);
      return false;
    }
  }

  if (tunnel_node1->ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    if (memcmp(&tunnel_node1->peer_ipv6_addr,&tunnel_node2->peer_ipv6_addr,
               sizeof(struct in6_addr)))
    {
      LOG_MSG_ERROR("MatchTunnelConfig - Peer IPv6 addr mismatch",0,0,0);
      return false;
    }
  }

  if (tunnel_node1->proto == QCMAP_MSGR_L2TP_ENCAP_UDP_V01)
  {
    if (tunnel_node1->local_udp_port != tunnel_node2->local_udp_port)
    {
      LOG_MSG_ERROR("MatchTunnelConfig - local UDP port mismatch",0,0,0);
      return false;
    }
    if (tunnel_node1->peer_udp_port != tunnel_node2->peer_udp_port)
    {
      LOG_MSG_ERROR("MatchTunnelConfig - peer UDP port mismatch",0,0,0);
      return false;
    }
  }

  if (tunnel_node1->phy_type != tunnel_node2->phy_type)
  {
    LOG_MSG_ERROR("MatchTunnelConfig - Phy type mismatch",0,0,0);
    return false;
  }

  if (tunnel_node1->vlan_id != tunnel_node2->vlan_id)
  {
    LOG_MSG_ERROR("MatchTunnelConfig - vlan ID mismatch",0,0,0);
    return false;
  }

  if (tunnel_node1->num_conf_sessions == tunnel_node2->num_conf_sessions)
  {
    node = tunnel_node1->session_list.L2TP_SessionListHead;
    node = ds_dll_next (node, (const void**)(&session1_config));
    while (node \
            && \
            node != tunnel_node1->session_list.L2TP_SessionListHead)
    {
      if (NULL == session1_config)
      {
        LOG_MSG_ERROR("MatchTunnelConfig - Invalid session config retirved.",\
                                                                    0,0,0);
        return false;
      }
      search_node = ds_dll_search(\
                             tunnel_node2->session_list.L2TP_SessionListHead,
                             (void*)&(session1_config->sessionConfig.session_id),
                             qcmap_match_session_id_from_list);

      if (tunnel_node2->session_list.L2TP_SessionListHead\
          &&\
          NULL != search_node)
      {
        session2_config = (qcmap_L2TP_Session_list_item_t *)
                                         ds_dll_data(search_node);
        if (session1_config->sessionConfig.peer_session_id !=
                       session2_config->sessionConfig.peer_session_id)
        {
          LOG_MSG_ERROR("MatchTunnelConfig(),Peer Session ID do not match "
                      "between tunnels\n",0,0,0);
          return false;
        }
      }
      else
      {
        LOG_MSG_ERROR("MatchTunnelConfig(),Session ID do not match between "
                      "tunnels\n",0,0,0);
        return false;
      }
      node = ds_dll_next (node, (const void**)(&session1_config));
    }
  }
  else
  {
    LOG_MSG_ERROR("MatchTunnelConfig(),No of sessions do not match between "
                  "tunnels\n",0,0,0);
    return false;
  }
  return true;
}
/*===========================================================================
  FUNCTION NumInstalledTunnelsWithPhyType
==========================================================================*/
/*!
@brief
  Returns number of tunnels configured on this particular physical interface.

@parameters
  QCMAP_Virtual_LAN_phy_iface phy_type

@return
  fasle- on failure
  true - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_L2TP::NumInstalledTunnelsWithPhyType
(
  QCMAP_Virtual_LAN_phy_iface phy_type
)
{
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
    ds_dll_el_t * node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  int num_tunnels = 0;
/*------------------------------------------------------------------------*/
  tunnel_list = &QCMAP_L2TP::tunnel_list;
  node = tunnel_list->L2TP_TunnelListHead;
  node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
  {
    if (NULL == l2tp_config_node)
    {
      LOG_MSG_ERROR("NumInstalledTunnelsWithPhyType - Invalid tunnel config",\
                                                                       0,0,0);
      return -1;
    }

    if (l2tp_config_node->is_installed
        &&
        l2tp_config_node->phy_type == phy_type)
    {
      num_tunnels++;
    }
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  }

  LOG_MSG_ERROR("NumInstalledTunnelsWithPhyType - Returning Num Tunnels %d "
                "for phy iface type %d",num_tunnels,phy_type,0);
  return num_tunnels;
}
/*===========================================================================
  FUNCTION NumInstalledTunnelsWithVLANID
==========================================================================*/
/*!
@brief
  Returns number of tunnels configured on this VLAN ID.

@parameters
  QCMAP_Virtual_LAN_phy_iface phy_type

@return
  fasle- on failure
  true - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_L2TP::NumInstalledTunnelsWithVLANID
(
  int16_t vlan_id
)
{
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
    ds_dll_el_t * node = NULL;
  qcmap_L2TP_Tunnel_list_item_t *l2tp_config_node = NULL;
  int num_tunnels = 0;
/*------------------------------------------------------------------------*/
  tunnel_list = &QCMAP_L2TP::tunnel_list;
  node = tunnel_list->L2TP_TunnelListHead;
  node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  while (node \
         && \
         node != tunnel_list->L2TP_TunnelListHead)
  {
    if (NULL == l2tp_config_node)
    {
      LOG_MSG_ERROR("NumInstalledTunnelsWithVLANID - Invalid tunnel config",\
                                                                       0,0,0);
      return -1;
    }

    if (l2tp_config_node->is_installed
        &&
        l2tp_config_node->vlan_id == vlan_id)
    {
      num_tunnels++;
    }
    node = ds_dll_next (node, (const void**)(&l2tp_config_node));
  }

  LOG_MSG_ERROR("NumInstalledTunnelsWithVLANID - Returning Num Tunnels %d "
                "for vlan id %d",num_tunnels,vlan_id,0);
  return num_tunnels;
}
/*=====================================================
  FUNCTION ReadInstallL2TPConfigFromXML
======================================================*/
/*!
@brief
  Reads L2TP Config from XML.

@parameters

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

boolean QCMAP_L2TP::ReadInstallL2TPConfigFromXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  pugi::xml_node *rem_child = NULL;
  char local_iface[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  QCMAP_Virtual_LAN_phy_iface phy_type = QCMAP_TETH_MIN;
  int16_t vlan_id = 0;
  qcmap_L2TP_Tunnel_list_item_t tunnel_config;
  qcmap_L2TP_Tunnel_list_item_t *tunnel_config_ptr = NULL;
  ds_dll_el_t  *node = NULL, *search_node = NULL;
  qcmap_L2TP_Session_list_item_t *session_config = NULL;
  uint32 * session_id_ptr = NULL;
  qcmap_L2TP_Tunnel_info_list_t *tunnel_list = NULL;
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_LAN* lan_obj = NULL;
  qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_state_config;
  qmi_error_type_v01 qmi_err_num;
/*------------------------------------------------------------------------*/
  if(!xml_file.load_file(QCMAP_L2TP::L2TP_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  memset(&tunnel_config,0,sizeof(qcmap_L2TP_Tunnel_list_item_t));
  root =
       xml_file.child(System_TAG).child(L2TPXMLCfg_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (rem_child != NULL)
    {
      root.remove_child(*rem_child);
      rem_child = NULL;
    }
    if (strncmp(child.name(), L2TPTunnel_TAG, strlen(L2TPTunnel_TAG)))
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Invalid tag %s in %s",\
                               child.name(),QCMAP_L2TP::L2TP_xml_path,0);
      return false;
    }
    /*Get physical interface type from local interface name*/
    strlcpy(local_iface,child.child(L2TPTunnelIface_TAG).child_value(),\
            QCMAP_MAX_IFACE_NAME_SIZE_V01);
    if(!(QCMAP_Virtual_LAN::GetPhyIfaceVLANIDFromIface(local_iface,\
                                                  &phy_type,&vlan_id)))
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Invalid local_iface: %s"
                    " found in L2TP xml",local_iface,0,0);
      return false;
    }

    if(VALIDATE_PHY_TYPE(phy_type))
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() Not a valid iface_type",0,0,0);
      return false;
    }

    tunnel_config.phy_type = phy_type;
    tunnel_config.vlan_id = vlan_id;

    /* ensure vlan is not in use with PDN mapping */
    if(0 != tunnel_config.vlan_id)
    {
      if((lan_obj = GET_LAN_OBJECT(vlan_id)) != NULL)
      {
        if(!(IS_DEFAULT_PROFILE(lan_obj->GetBackhaulProfileHandle())))
        {
          LOG_MSG_INFO1("VLAN ID: %d is currently mapped to profile handle: %d, please map to "
                        "default profile before configuring L2TP", vlan_id,
                        lan_obj->GetBackhaulProfileHandle(), 0);
          //Disable L2TP automatically so that VLAN to default PDN mapping can go smoothly
          l2tp_state_config.enable = 0;
          if(!QCMAP_L2TP::SetL2TPState(l2tp_state_config, NULL, &qmi_err_num))
          {
            LOG_MSG_INFO1("Failed to disable L2TP", 0, 0, 0);
          }
          return false;
        }
      }
    }

    /*Read Tunnel Config*/
    if (!this->ReadL2TPTunnelConf(&child,&tunnel_config))
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - error in reading tunnel"
                    " config from L2TP xml",0,0,0);
      return false;
    }

    /*Validate Tunnel Config*/
    if (!QCMAP_L2TP::ValidateTunnelConfig(&tunnel_config,true))
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Not a valid tunnel config"
                    " from L2TP xml",0,0,0);
      return false;
    }

    tunnel_list = &(QCMAP_L2TP::tunnel_list);
    search_node = ds_dll_search (tunnel_list->L2TP_TunnelListHead,
                           (void*)&(tunnel_config.local_tunnel_id),
                            qcmap_match_tunnel_id_in_list);

    if (NULL != search_node)
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML(),Duplicate Tunnel ID : %d"
                  "found in l2tp xml. Delete this tunnel from XML\n",
                    tunnel_config.local_tunnel_id,0,0);
      rem_child = &child;
      continue;
    }

    node = tunnel_config.session_list.L2TP_SessionListHead;
    node = ds_dll_next (node, (const void**)(&session_config));
    while (node \
          && \
          node != tunnel_config.session_list.L2TP_SessionListHead)
    {
      if(!this->AddSessionIDEntryToList(
                                       session_config->sessionConfig.session_id,
                                       session_id_ptr))
      {
        LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Error in adding session"
                    " ID to list",0,0,0);
        goto delete_session_ids_from_list;
      }
      node = ds_dll_next (node, (const void**)(&session_config));
      session_id_ptr = NULL;
    }

    /*configure L2TP tunnels if interface is up*/
    if (this->is_iface_up[phy_type])
    {
      /*Check if VLAN ID is configured or not.*/
      if (tunnel_config.vlan_id != 0)
      {
        if(!QCMAP_Virtual_LAN::IsVLANIDUp(tunnel_config.vlan_id, NULL))
        {
          LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Vlan id %d is not"
                    "configured/up. Saving config.",tunnel_config.vlan_id,0,0);
        }
        else
        {
          if (!this->InstallL2TP(tunnel_config))
          {
            LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Error configuring"
                                                          "tunnel", 0,0,0);
            goto err_install;
          }
        }
      }
      else
      {
        if (!this->InstallL2TP(tunnel_config))
        {
          LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Error configuring"
                                                            "tunnel", 0,0,0);
          goto err_install;
        }
      }
      tunnel_config.is_installed = true;
    }
    else
    {
      LOG_MSG_INFO1("ReadInstallL2TPConfigFromXML() - phy iface %d is not up"
                           "Not installing tunnel",tunnel_config.phy_type,0,0);
    }

    /*Add Tunnel Config to list*/
    if(!this->AddL2TPTunnelEntryToList(tunnel_config,tunnel_config_ptr))
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Error in adding Tunnel"
                    " config to tunnel list",0,0,0);
      goto delete_session_ids_from_list;
    }

    /* to take bootup changes also into account */
    QCMAP_L2TP::num_c_tunnel++;

    /*Clear defaults for next iteration*/
    memset(&tunnel_config,0,sizeof(qcmap_L2TP_Tunnel_list_item_t));
    tunnel_config_ptr = NULL;
    phy_type = QCMAP_TETH_MIN;
    vlan_id =0;
  }

  xml_file.save_file(TEMP_L2TP_CFG_XML);
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_L2TP_CFG_XML);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_L2TP_CFG_XML ,
              QCMAP_L2TP::L2TP_xml_path);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_L2TP_CFG_XML);
  ds_system_call(command, strlen(command));
  return true;


  err_install:
    tunnel_config.is_installed = false;
      /*Add Tunnel Config to list*/
    if(!this->AddL2TPTunnelEntryToList(tunnel_config,tunnel_config_ptr))
    {
      LOG_MSG_ERROR("ReadInstallL2TPConfigFromXML() - Error in adding Tunnel"
                    " config to tunnel list",0,0,0);
      goto delete_session_ids_from_list;
    }
    return false;

  delete_session_ids_from_list:
    node = tunnel_config.session_list.L2TP_SessionListHead;
    node = ds_dll_next (node, (const void**)(&session_config));
    while (node \
          && \
          node != tunnel_config.session_list.L2TP_SessionListHead)
    {
      ds_dll_delete (QCMAP_L2TP::sessionIDList.SessionIDHead,
                    &QCMAP_L2TP::sessionIDList.SessionIDTail,
                    (void*)&(session_config->sessionConfig.session_id),
                    qcmap_match_session_id);
      node = ds_dll_next (node, (const void**)(&session_config));
    }
    xml_file.save_file(TEMP_L2TP_CFG_XML);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",TEMP_L2TP_CFG_XML);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_L2TP_CFG_XML ,
              QCMAP_L2TP::L2TP_xml_path);
    ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_L2TP_CFG_XML);
  ds_system_call(command, strlen(command));
    return false;

}
/*=====================================================
  FUNCTION ReadL2TPTunnelConf
======================================================*/
/*!
@brief
  Reads L2TP Tunnel config, saves from xml node

@parameters

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

boolean QCMAP_L2TP::ReadL2TPTunnelConf(pugi::xml_node *child,
                             qcmap_L2TP_Tunnel_list_item_t *tunnel_conf)
{
  pugi::xml_node itr_child;
  int temp_val = 0;
  ds_dll_el_t * search_node = NULL;
  qcmap_L2TP_Session_list_item_t session_config_info;
  qcmap_L2TP_Session_list_item_t *session_config = NULL;
/*------------------------------------------------------------------------*/
  for (itr_child = child->first_child(); itr_child;
                                 itr_child = itr_child.next_sibling())
  {
    if (!strncmp(itr_child.name(),L2TPTunnelID_TAG,strlen(L2TPTunnelID_TAG)))
    {
      /*Read Tunnel ID*/
      temp_val = atoi(itr_child.child_value());

      if (temp_val > MAX_UINT32_VAL)
      {
        LOG_MSG_ERROR("Invalid value for tunnel id",\
                                            0,0,0);
        return false;
      }
      tunnel_conf->local_tunnel_id = atoi(itr_child.child_value());
    }
    else if (!strncmp(itr_child.name(),L2TPPeerTunnelID_TAG,
                                            strlen(L2TPPeerTunnelID_TAG)))
    {
      /*Read Peer Tunnel ID*/
      temp_val = atoi(itr_child.child_value());
      if (temp_val > MAX_UINT32_VAL)
      {
        LOG_MSG_ERROR("Invalid value for "
                      "peer tunnel id",0,0,0);
        return false;
      }
      tunnel_conf->peer_tunnel_id = atoi(itr_child.child_value());

    }
    else if(!strncmp(itr_child.name(),L2TPTunnelRemoteIP_TAG,
                                         strlen(L2TPTunnelRemoteIP_TAG)))
    {
      /*Read Peer IP address according to IP version,
                    only v4 and v6 are supported*/
      if (!strncmp(child->child(L2TPTunnelIPVer_TAG).child_value(),"v6",
                                                             strlen("v6")))\
      {
        tunnel_conf->ip_family = QCMAP_MSGR_IP_FAMILY_V6_V01;
        if (!inet_pton(AF_INET6,itr_child.child_value(),\
                               &tunnel_conf->peer_ipv6_addr))
        {
          LOG_MSG_ERROR("Invalid value for "
                      "Peer IPv6 address %s",itr_child.child_value(),0,0);
          return false;
        }
      }
      else if (!strncmp(child->child(L2TPTunnelIPVer_TAG).child_value(),"v4",
                                                                strlen("v4")))\
      {
        tunnel_conf->ip_family = QCMAP_MSGR_IP_FAMILY_V4_V01;
        if (!inet_pton(AF_INET,itr_child.child_value(),\
                               &tunnel_conf->peer_ipv4_addr))
        {
          LOG_MSG_ERROR("Invalid value for "
                      "Peer IPv4 address %s",itr_child.child_value(),0,0);
          return false;
        }

      }
      else
      {
         LOG_MSG_ERROR("Invalid value for "
                      "IP version %s",
                       child->child(L2TPTunnelIPVer_TAG).child_value(),0,0);
        return false;
      }
    }
    else if (!strncmp(itr_child.name(),L2TPTunnelEncapProto_TAG,
                                           strlen(L2TPTunnelEncapProto_TAG)))
    {
      /*Read encapsulation protocol and port #'s if it is UDP*/
      if (!strncmp(itr_child.child_value(),"IP",strlen("IP")))
      {
        tunnel_conf->proto = QCMAP_MSGR_L2TP_ENCAP_IP_V01;

      }
      else if (!strncmp(itr_child.child_value(),"UDP",strlen("UDP")))
      {
        tunnel_conf->proto = QCMAP_MSGR_L2TP_ENCAP_UDP_V01;
        temp_val = 0;
        temp_val = atoi(child->child(L2TPTunnelUDPPort_TAG).child_value());
        if (temp_val == 0 || temp_val > MAX_UINT16_VAL)
        {
          LOG_MSG_ERROR("Invalid value for "
                      "Local UDP port %d",temp_val,0,0);
          return false;
        }
        tunnel_conf->local_udp_port = htons(temp_val);
        temp_val = 0;
        temp_val = atoi(child->child(L2TPTunnelPeerUDPPort_TAG).child_value());
        if (temp_val == 0 || temp_val > MAX_UINT16_VAL)
        {
          LOG_MSG_ERROR("Invalid value for "
                      "Peer UDP port %d",temp_val,0,0);
          return false;
        }
        tunnel_conf->peer_udp_port = htons(temp_val);
      }
      else
      {
        LOG_MSG_ERROR("Invalid value for "
                      "encap portocol %s",itr_child.child_value(),0,0);
        return false;
      }
    }

    else if (!strncmp(itr_child.name(),L2TPTunnelUDPPort_TAG,
                                            strlen(L2TPTunnelUDPPort_TAG))
             ||
             !strncmp(itr_child.name(),L2TPTunnelPeerUDPPort_TAG,
                                           strlen(L2TPTunnelPeerUDPPort_TAG))
             ||
             !strncmp(itr_child.name(),L2TPTunnelIPVer_TAG,
                                                strlen(L2TPTunnelIPVer_TAG))
             ||
             !strncmp(itr_child.name(),L2TPTunnelIface_TAG,
                                               strlen(L2TPTunnelIface_TAG)))
    {
      /*Ignore this as
        1. UDP ports are read if the encap proto is set to UDP
        2. IP version is read while reading IP addresses
        3. IFACE name is read before calling this function*/
    }
    else if (!strncmp(itr_child.name(),L2TPSession_Tag,strlen(L2TPSession_Tag)))
    {
      /*Read session config and save it into session list*/
      memset(&session_config_info,0,sizeof(qcmap_L2TP_Session_list_item_t));
      temp_val = 0;
      temp_val = atoi(itr_child.child(L2TPSessionID_Tag).child_value());
      if (temp_val == 0 || temp_val > MAX_UINT32_VAL)
      {
        LOG_MSG_ERROR("Invalid value for "
                    "local session ID %d",temp_val,0,0);
        return false;
      }
      session_config_info.sessionConfig.session_id = temp_val;
      temp_val = atoi(itr_child.child(L2TPSessionPeerID_Tag).child_value());
      if (temp_val == 0 || temp_val > MAX_UINT32_VAL)
      {
        LOG_MSG_ERROR("Invalid value for "
                    "local session ID %d",temp_val,0,0);
        return false;
      }
      session_config_info.sessionConfig.peer_session_id = temp_val;

      /*Search if it is a duplicate Session ID for this tunnel*/
      search_node = ds_dll_search(\
                               tunnel_conf->session_list.L2TP_SessionListHead,
                               (void*)&(temp_val),
                               qcmap_match_session_id_from_list);

      if (tunnel_conf->session_list.L2TP_SessionListHead\
          &&\
          NULL != search_node)
      {
        LOG_MSG_ERROR("Duplicate session id %d"\
                      " for tunnel\n",temp_val,0,0);
        return false;
      }

      /* Increment config sessions count*/
      tunnel_conf->num_conf_sessions = tunnel_conf->num_conf_sessions + 1;
      /*Add session config info to the list*/
      if (!this->AddL2TPSessionEntryToList(\
                           session_config_info,
                           session_config,
                           &tunnel_conf->session_list))
      {
         LOG_MSG_ERROR("Error in adding session:%d "
                       "to list",temp_val,0,0);
        return false;
      }
    }
    else
    {
      LOG_MSG_ERROR("Invalid tag: %s "
                    "present in L2TP xml",itr_child.name(),0,0);
      return false;
    }
  }
  LOG_MSG_INFO1("exiting",0,0,0);
  return true;
}
/*===========================================================================
  FUNCTION RemoveL2TP
==========================================================================*/
/*!
@brief
  Deletes L2TP Tunnel and Session

@parameters
 qcmap_L2TP_Tunnel_list_item_t l2tp_config_node

@return
  true on success
  false otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_L2TP::RemoveL2TP
(
  qcmap_L2TP_Tunnel_list_item_t l2tp_config_node
)
{
  char command[MAX_L2TP_COMMAND_LEN] = {0};
  int16_t                      vlan_id = 0;
  QCMAP_Virtual_LAN_phy_iface phy_type = QCMAP_TETH_MIN;
  qcmap_L2TP_Session_info_list_t *sessionList = NULL;
  ds_dll_el_t * node = NULL, *search_node = NULL;
  qcmap_L2TP_Session_list_item_t *session_config;
  char iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  uint8 sessionIndex = 0;
  char phy_iface[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
/*------------------------------------------------------------------------*/
  phy_type = l2tp_config_node.phy_type;
  vlan_id = l2tp_config_node.vlan_id;

  if(VALIDATE_PHY_TYPE(phy_type))
  {
    LOG_MSG_ERROR("RemoveL2TP() Not a valid iface_type",0,0,0);
    return false;
  }


  /*Iterate through session list and delete sessions*/
  sessionList = &l2tp_config_node.session_list;
  if(NULL != sessionList->L2TP_SessionListHead)
  {
    memset(command,0,sizeof(command));
    node = sessionList->L2TP_SessionListHead;
    node = ds_dll_next (node, (const void**)(&session_config));
    while (node \
            && \
            node != sessionList->L2TP_SessionListHead)
    {
      if (NULL == session_config)
      {
        LOG_MSG_ERROR("RemoveL2TP - Invalid session config retirved.",\
                                                                    0,0,0);
        return false;
      }

      sessionIndex = session_config->SessionIndex;
      snprintf(iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01,
                                        "l2tpeth%d",sessionIndex);

      /*Remove IP table TCP MSS rules for this particular iface if enabled*/
      if (QCMAP_L2TP::TCP_MSS_config_enabled)
      {
        this->ConfigDelL2TPTCPMSS(false,iface_name);
      }

      /*Unbridge interface, Delete session, delete config*/
      snprintf(command,MAX_L2TP_COMMAND_LEN,"brctl delif %s l2tpeth%d",
                                      BRIDGE_IFACE,
                                      session_config->SessionIndex);
      ds_system_call(command, strlen(command));

      QCMAP_L2TP::sessionIndex[sessionIndex] = 0;

      snprintf(command,MAX_L2TP_COMMAND_LEN,"ip l2tp del session "\
                        "tunnel_id %d session_id %d",
                                      l2tp_config_node.local_tunnel_id,
                                      session_config->sessionConfig.session_id);
      ds_system_call(command, strlen(command));

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
      if (vlan_id == 0)
      {
        snprintf(phy_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s",
                     this->physical_iface_name[phy_type].if_name);
      }
      else
      {
        snprintf(phy_iface,QCMAP_MAX_IFACE_NAME_SIZE_V01,"%s.%d",
                     this->physical_iface_name[phy_type].if_name,vlan_id);
      }

      /* end IOCTL to IPA only if encap proto is IP AND phy type is eth.
      UDP encapsulation offload is not supported currently.
      IPA offload for any other end point other than ethernet
       is not supported cuurently*/
      if (phy_type == QCMAP_TETH_ETH
          &&
          l2tp_config_node.proto == QCMAP_MSGR_L2TP_ENCAP_IP_V01)
      {
        if(!this->UpdateIPAWithL2TPIOCTL(iface_name,phy_iface,
                                     l2tp_config_node.ip_family,
                                     session_config->sessionConfig.session_id,
                                     false))
        {
          LOG_MSG_ERROR("RemoveL2TP - IOCTL to IPA failed.",0,0,0);
        }
      }

#endif

      node = ds_dll_next (node, (const void**)(&session_config));
    }/*end of while*/
  }

  /*Delete Tunnel, if it is last tunnel add underlying phy iface to bridge*/
  snprintf(command,MAX_L2TP_COMMAND_LEN,
           "ip l2tp del tunnel tunnel_id %d",l2tp_config_node.local_tunnel_id);
  ds_system_call(command, strlen(command));

  if (this->NumInstalledTunnelsWithPhyType(phy_type) == 1)
  {
    if (phy_type != QCMAP_TETH_BRIDGE0)
    {

      snprintf(command,MAX_L2TP_COMMAND_LEN,"brctl addif %s %s",
                                  BRIDGE_IFACE,
                                  this->physical_iface_name[phy_type].if_name);
      ds_system_call(command, strlen(command));
    }

    if (QCMAP_L2TP::MTU_config_enabled)
    {
      this->ConfigDelMTUSizeOnPhyVLANiface(false,
                                       l2tp_config_node.phy_type,
                                       l2tp_config_node.vlan_id);

    }

  }
  if (this->NumInstalledTunnelsWithVLANID(vlan_id) == 1
           && QCMAP_L2TP::MTU_config_enabled)
  {
    snprintf(command,MAX_L2TP_COMMAND_LEN,
           "ifconfig %s.%d mtu %d",this->physical_iface_name[phy_type].if_name,
                           vlan_id,STD_MTU_SIZE);
    ds_system_call(command, strlen(command));
  }

  return true;
}
#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
/*=====================================================
  FUNCTION UpdateIPAWithL2TPIOCTL
======================================================*/
/*!
@brief
  Sends IOCTL to IPA with L2TP info.

@parameters
 char *iface_name,
 char *phy_iface,
 qcmap_msgr_ip_family_enum_v01 ip_family,
 uint32 session_id,
 boolean is_up

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

boolean QCMAP_L2TP::UpdateIPAWithL2TPIOCTL(char *iface_name,
                                       char *phy_iface,
                                       qcmap_msgr_ip_family_enum_v01 ip_family,
                                       uint32 session_id,
                                       boolean is_up)
{
  int ioctl_file_fd = -1, ioctl_ret = -1;
  struct ipa_ioc_l2tp_vlan_mapping_info l2tp_ioctl_buffer;
/*------------------------------------------------------------------------*/
  if (iface_name == NULL || phy_iface == NULL)
  {
    LOG_MSG_ERROR("UpdateIPAWithIOCTL() Invalid parameters received",0,0,0);
    return false;
  }

  memset(&l2tp_ioctl_buffer,0,sizeof(ipa_ioc_l2tp_vlan_mapping_info));
  ioctl_file_fd = open(IPA_DEVICE_NAME, O_RDWR);
  if (ioctl_file_fd < 0)
  {
    LOG_MSG_ERROR("UpdateIPAWithIOCTL() Cannot open file for ioctl",0,0,0);
    return false;
  }

  strlcpy(l2tp_ioctl_buffer.l2tp_iface_name,iface_name,IPA_RESOURCE_NAME_MAX);
  strlcpy(l2tp_ioctl_buffer.vlan_iface_name,phy_iface,IPA_RESOURCE_NAME_MAX);

  if (ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    l2tp_ioctl_buffer.iptype = IPA_IP_v4;
  }
  else if (ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    l2tp_ioctl_buffer.iptype = IPA_IP_v6;
  }
  else
  {
    LOG_MSG_ERROR("UpdateIPAWithIOCTL() Unsupported IP type",0,0,0);
    close(ioctl_file_fd);
    return false;
  }

  l2tp_ioctl_buffer.l2tp_session_id = session_id;

  if (is_up)
  {
    ioctl_ret = ioctl(
              ioctl_file_fd,IPA_IOC_ADD_L2TP_VLAN_MAPPING,&l2tp_ioctl_buffer);
  }
  else
  {
    ioctl_ret = ioctl(
              ioctl_file_fd,IPA_IOC_DEL_L2TP_VLAN_MAPPING,&l2tp_ioctl_buffer);
  }

  if (ioctl_ret)
  {
    LOG_MSG_ERROR("UpdateIPAWithIOCTL() IOCTL to IPA failed",0,0,0);
    close(ioctl_file_fd);
    return false;
  }

  close(ioctl_file_fd);
  return true;
}
#endif
/*==========================================================
 FUNCTION    ValidateTunnelConfig
===========================================================*/
/*!
@brief
  Validates tunnel config passed and returns error number.

@parameters
  qcmap_L2TP_Tunnel_list_item_t *tunnel_conf,
  boolean                        match_session_id

@return
  fasle- on failure
  true - on success

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
boolean QCMAP_L2TP::ValidateTunnelConfig
(
  qcmap_L2TP_Tunnel_list_item_t *tunnel_conf,
  boolean                        match_session_id
)
{
  char v6addr_str[INET6_ADDRSTRLEN] = {0};
  char v4addr_str[INET_ADDRSTRLEN] = {0};
  ds_dll_el_t  *node, *search_node;
  qcmap_L2TP_Session_list_item_t *session_config = NULL;
  qcmap_session_id_info_list_t *sessionIDList;
/*------------------------------------------------------------------------*/
  if (NULL == tunnel_conf)
  {
    LOG_MSG_ERROR("ValidateTunnelConfig - Invalid parameters sent\n",0,0,0);
    return false;
  }

  if(tunnel_conf->local_tunnel_id == 0
     ||
     tunnel_conf->peer_tunnel_id == 0)
  {
    LOG_MSG_ERROR("AddL2TPSessionEntryToList - Invalid Tunnel ID\n",0,0,0);
    return false;
  }

  if (tunnel_conf->ip_family == QCMAP_MSGR_IP_FAMILY_V4_V01)
  {
    if (NULL == inet_ntop(AF_INET, &tunnel_conf->peer_ipv4_addr,\
                  v4addr_str, INET_ADDRSTRLEN))
    {
      LOG_MSG_ERROR("AddL2TPSessionEntryToList - Invalid Peer IPv4 address\n",\
                    0,0,0);
      return false;
    }
  }
  else if(tunnel_conf->ip_family == QCMAP_MSGR_IP_FAMILY_V6_V01)
  {
    if (NULL == inet_ntop(AF_INET6, &tunnel_conf->peer_ipv6_addr.s6_addr,\
                  v6addr_str, INET6_ADDRSTRLEN))
    {
      LOG_MSG_ERROR("AddL2TPSessionEntryToList - Invalid Peer IPv6 address\n",\
                    0,0,0);
      return false;
    }
  }
  else
  {
    LOG_MSG_ERROR("AddL2TPSessionEntryToList - Invalid IP family\n",0,0,0);
    return false;
  }

  if (tunnel_conf->proto == QCMAP_MSGR_L2TP_ENCAP_UDP_V01)
  {
    if (tunnel_conf->local_udp_port == 0
        ||
        tunnel_conf->peer_udp_port == 0)
    {
      LOG_MSG_ERROR("AddL2TPSessionEntryToList - Invalid UDP ports\n",0,0,0);
      return false;
    }
  }
  else if (tunnel_conf->proto != QCMAP_MSGR_L2TP_ENCAP_IP_V01)
  {
    LOG_MSG_ERROR("AddL2TPSessionEntryToList - Invalid Encap protocol\n",0,0,0);
    return false;
  }
  if (match_session_id)
  {
    node = tunnel_conf->session_list.L2TP_SessionListHead;
    node = ds_dll_next (node, (const void**)(&session_config));
    while (node \
          && \
          node != tunnel_conf->session_list.L2TP_SessionListHead)
    {
      sessionIDList = &(QCMAP_L2TP::sessionIDList);
      search_node = ds_dll_search (sessionIDList->SessionIDHead,
                             (void*)&(session_config->sessionConfig.session_id),
                              qcmap_match_session_id);

      if (NULL != search_node)
      {
        LOG_MSG_ERROR("ValidateTunnelConfig(),Session id %d is already "
                    "configured\n",session_config->sessionConfig.session_id,0,0);
        return false;
      }
      node = ds_dll_next (node, (const void**)(&session_config));
    }
  }

  return true;
}
