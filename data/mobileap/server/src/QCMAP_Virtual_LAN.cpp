/*====================================================

FILE:  QCMAP_Virtual_LAN.cpp

SERVICES:
   QCMAP Virtual LAN Specific Implementation

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
#include <sys/types.h>
#include <ifaddrs.h>

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
#include <linux/msm_ipa.h>
#include <sys/ioctl.h>
#endif

#include "ds_string.h"
#include "ds_util.h"
#include "QCMAP_Virtual_LAN.h"
#include "QCMAP_ConnectionManager.h"
#include "qcmap_cm_api.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_L2TP.h"
#include "qcmap_netlink.h"

#define IS_IFACE_TYPE_SAME(cmp1, cmp2) (cmp1 == cmp2)
#define IS_IFACE_NAME_SAME(iface_name1, iface_name2) (strcmp(iface_name1, iface_name2) == 0)

bool QCMAP_Virtual_LAN::flag = false;
QCMAP_Virtual_LAN* QCMAP_Virtual_LAN::object=NULL;

/*=====================================================
                     Class Definitions
  =====================================================*/

/*=====================================================
                  VLAN Helper functions
  =====================================================*/

/*=====================================================
  FUNCTION GetIfaceTypeFromIface
======================================================*/
/*!
@brief
  Gets PHY iface type, from iface name.

@parameters
  iface_name

@return
  PHY iface type

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
static QCMAP_Virtual_LAN_phy_iface GetIfaceTypeFromIface(const char *iface_name)
{
  QCMAP_Virtual_LAN_phy_iface ret_val = QCMAP_TETH_MIN;

  if (!strncmp(iface_name, ECM_IFACE, strlen(ECM_IFACE)))
  {
    ret_val = QCMAP_TETH_ECM;
    LOG_MSG_INFO1("ECM type", 0, 0, 0);
  } else if (!strncmp(iface_name, RNDIS_IFACE, strlen(RNDIS_IFACE))){
    ret_val = QCMAP_TETH_RNDIS;
    LOG_MSG_INFO1("RNDIS type", 0, 0, 0);
  } else if (!strncmp(iface_name, ETH_IFACE, strlen(ETH_IFACE))) {
    ret_val = QCMAP_TETH_ETH;
    LOG_MSG_INFO1("ETH type", 0, 0, 0);
  } else if (!strncmp(iface_name, BRIDGE_IFACE, strlen(BRIDGE_IFACE))) {
    ret_val = QCMAP_TETH_BRIDGE0;
    LOG_MSG_INFO1("BRIDGE type", 0, 0, 0);
  } else {
    LOG_MSG_ERROR("Couldn't determine type", 0, 0, 0);
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION IfaceNameToEnum
==========================================================================*/
/*!
@brief
  Returns Enum Type for an iface.

@parameters
  iface_name

@return
  qcmap_msgr_interface_type_enum_v01

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static qcmap_msgr_interface_type_enum_v01 IfaceNameToEnum(const char *iface_name)
{
  qcmap_msgr_interface_type_enum_v01 iface_type = QCMAP_MSGR_INTERFACE_TYPE_ENUM_MIN_ENUM_VAL_V01;

  if(!strcmp(iface_name, ETH_IFACE))
  {
    iface_type = QCMAP_MSGR_INTERFACE_TYPE_ETH_V01;
  }
  else if(!strcmp(iface_name, RNDIS_IFACE))
  {
    iface_type = QCMAP_MSGR_INTERFACE_TYPE_RNDIS_V01;
  }
  else if(!strcmp(iface_name, ECM_IFACE))
  {
    iface_type = QCMAP_MSGR_INTERFACE_TYPE_ECM_V01;
  }

  return iface_type;
}

/*=====================================================
                 Class Constructor, Destructors.
  =====================================================*/

/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and return instance of class QCMAP_Virtual_LAN

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
static QCMAP_Virtual_LAN* QCMAP_Virtual_LAN::Get_Instance(bool obj_create)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : Virtual LAN",0, 0, 0);
    object = new QCMAP_Virtual_LAN();
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
  Initializes Virtual LAN variables.

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
QCMAP_Virtual_LAN::QCMAP_Virtual_LAN()
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (QcMapMgr && !(IS_VLAN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("VLAN is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    return;
  }

  #if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
      !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
    this->ipa_offloader = new QCMAP_VLAN_IPA_Offloader();
  #endif
}

/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroyes the Virtual LAN Object.

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
QCMAP_Virtual_LAN::~QCMAP_Virtual_LAN()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  flag=false;
  object=NULL;

  #if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
      !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
    delete this->ipa_offloader;
  #endif
}

/*=====================================================
                 VLAN Private Functions.
  =====================================================*/

/*===========================================================================
  FUNCTION ConfigureVLAN
==========================================================================*/
/*!
@brief
  Configures VLAN with vconfig and ifconfig.

@parameters
 qcmap_vlan_item vlan_config_node

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Virtual_LAN::ConfigureVLAN(qcmap_vlan_item vlan_config_node) const
{
  char command[MAX_COMMAND_STR_LEN] = {0};

  if(VALIDATE_PHY_TYPE(vlan_config_node.phy_iface_type))
  {
    LOG_MSG_ERROR("Not a valid QCMAP_Virtual_LAN_phy_iface"
                   ",iface_type value: %d",vlan_config_node.phy_iface_type,0,0);
    return;
  }

  if(!vlan_config_node.is_up)
  {
    LOG_MSG_ERROR("PHY is not up: %d", vlan_config_node.phy_iface_type, 0, 0);
    return;
  }

  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "vconfig add %s %d", vlan_config_node.iface_name,
           vlan_config_node.vlan_id);
  ds_system_call(command, strlen(command));

  /* Disable proxing for other Local interfaces. */
  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv4/conf/%s.%d/arp_ignore",
           vlan_config_node.iface_name,
           vlan_config_node.vlan_id);
  ds_system_call(command, strlen(command));

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
  if(!(this->ipa_offloader))
  {
    LOG_MSG_ERROR("IPA Offloader object is NULL", 0, 0, 0);
    return;
  }

  if(vlan_config_node.ipa_offload)
  {
    /* Send IOCTL to Neutrino / EMAC if the underlying interface is ETH.
    If it succeds, Send IOCTL to IPA with vlan information*/
    if(QCMAP_TETH_ETH == vlan_config_node.phy_iface_type)
    {
      if(!(this->ipa_offloader->UpdateNeutrinoWithVlanIoctl(vlan_config_node.vlan_id, true)))
      {
        LOG_MSG_ERROR("%s,Failed VLAN IOCTL to Neutrino", __func__, 0, 0);
      }
    }

    if(!this->ipa_offloader->UpdateIPAWithVlanIOCTL((const char*)vlan_config_node.iface_name,
                                                      vlan_config_node.vlan_id, true))
    {
        LOG_MSG_ERROR("%s,Failed VLAN IOCTL to IPA", __func__, 0, 0);
    }
  }
#endif

  return;
}

/*===========================================================================
  FUNCTION DeleteVLAN
==========================================================================*/
/*!
@brief
  Deletes specific VLAN config with vconfig.

@parameters
 qcmap_vlan_item vlan_config_node
 bool deleteConfig if true, then remove runtime settings and update updateXML.
                   if false, then remove only runtime settings.

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Virtual_LAN::DeleteVLAN(qcmap_vlan_item vlan_config_node, bool deleteConfig) const
{
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN *QcMapBackhaulWWAN = NULL;
  QCMAP_Backhaul *QcMapBackhaul;
/*------------------------------------------------------------------------*/

  /* Loop through Backhaul objects till you get the one corresponding to correct vlan.
     If one exits... */
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while (END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    QcMapBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ;
    QcMapBackhaul = GET_BACKHAUL_OBJ_AND_INC;
    if (QcMapBackhaul->vlan_id == vlan_config_node.vlan_id)
    {
      LOG_MSG_INFO1("Found Backhaul tied to VLAN ID %d", vlan_config_node.vlan_id,0,0);
      break;
    }
  }

  //Secondary PDN
  if (QcMapBackhaulWWAN && !QcMapBackhaulWWAN->DisableVlanPdnRules(QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01))
  {
    LOG_MSG_ERROR("Unable to disable rules",0,0,0);
  }

  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "vconfig rem %s.%d", vlan_config_node.iface_name,
           vlan_config_node.vlan_id);
  ds_system_call(command, strlen(command));

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)

  if (this->ipa_offloader == NULL)
  {
    LOG_MSG_ERROR("oops!!! ipa_offloader is NULL",0,0,0);
    return;
  }

  /* Send IOCTL to Neutrino / EMAC if the underlying interface is ETH.
  If it succeds, Send IOCTL to IPA with vlan information*/
  if(QCMAP_TETH_ETH == vlan_config_node.phy_iface_type)
  {
    if(!(this->ipa_offloader->UpdateNeutrinoWithVlanIoctl(vlan_config_node.vlan_id, false)))
    {
      LOG_MSG_ERROR("Failed VLAN IOCTL to Neutrino", 0, 0, 0);
    }
  }

  if(!this->ipa_offloader->UpdateIPAWithVlanIOCTL((const char*)vlan_config_node.iface_name,
                                                    vlan_config_node.vlan_id, false))
  {
    LOG_MSG_ERROR("Failed VLAN IOCTL to IPA", 0, 0, 0);
  }

  if (deleteConfig == true)
  {
    this->ipa_offloader->UpdateDesiredOffloadInfo((const char*)vlan_config_node.iface_name,
                                                    vlan_config_node.vlan_id, false);

    if(this->ipa_offloader && QcMapMgr &&
       QcMapMgr->TriggerAutoReboot(this->ipa_offloader->GetAutoRebootFlag(), QCMAP_CM_LAST_VLAN_REMOVED_ON_PHY))
    {
      //Warn user, that we will be doing autoreboot
      LOG_MSG_ERROR("Auto reboot required to remove IPA offload (last VLAN removed)", 0, 0, 0);
    }
  }
#endif

  return;
}

/*===========================================================================
  FUNCTION ConstructVLANNode
==========================================================================*/
/*!
@brief
  Constructs QCMAP VLAN item based on vlan config param

@parameters
  qcmap_msgr_vlan_config_v01 vconfig,
  qcmap_vlan_item *vlan_node

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
bool QCMAP_Virtual_LAN::ConstructVLANNode
(
 qcmap_msgr_vlan_config_v01   vconfig,
 qcmap_vlan_item              *vlan_node
) const
{
  if(NULL == vlan_node)
  {
    LOG_MSG_ERROR("%s Invalid VLAN Node passed", __func__, 0, 0);
    return false;
  }

  memset(vlan_node->iface_name, 0, sizeof(vlan_node->iface_name));

  //fill in iface
  snprintf(vlan_node->iface_name, sizeof(vlan_node->iface_name), "%s", vconfig.local_iface);

  vlan_node->vlan_id = vconfig.vlan_id; //fill in VLAN ID

  vlan_node->phy_iface_type = GetIfaceTypeFromIface(vconfig.local_iface); //fill in PHY type
  if((vlan_node->phy_iface_type <= QCMAP_TETH_MIN) ||
     (vlan_node->phy_iface_type >= QCMAP_TETH_BRIDGE0))
  {
    LOG_MSG_ERROR("Iface: %s not supported for VLAN", vlan_node->iface_name, 0, 0);
    return false;
  }

  //check PHY availability
  if(this->IsPhyLinkUp(vlan_node->phy_iface_type))
  {
    vlan_node->is_up = true;
  } else {
    vlan_node->is_up = false;
  }

  //check if IPA offload desired
  if(vconfig.ipa_offload)
  {
    vlan_node->ipa_offload = true;
  } else {
    vlan_node->ipa_offload = false;
  }

  return true;
}

/*=====================================================
  FUNCTION SetVLANConfigToXML
======================================================*/
/*!
@brief
  Set VLAN Config to XML.

@parameters
qcmap_msgr_vlan_config_v01 vconfig

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
bool QCMAP_Virtual_LAN::SetVLANConfigToXML(qcmap_msgr_vlan_config_v01 vconfig) const
{
  pugi::xml_document xml_file;
  pugi::xml_node root, subchild, temp;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  bool iface_match, vlan_id_match, found_ipa_offload;

  if(NULL == QcMapMgr)
  {
    LOG_MSG_ERROR("Connection Manager is NULL", 0, 0, 0);
    return false;
  }

  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  /* Append new vconfig to existing vlan tag*/
  if((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG)) == NULL)
  {
    root = root.append_child(VCONFIG_TAG);
  }

  //see if existing VLAN ID PHY type existed
  for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
  {
    iface_match = vlan_id_match = found_ipa_offload = false;

    for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
        grand_child = grand_child.next_sibling())
    {
      if(strcmp(grand_child.name(), VCONFIG_IFACE_TAG) == 0)
      {
        if(strcmp((char*)grand_child.text().get(), vconfig.local_iface) == 0)
        {
          iface_match = true;
        }
      }
      if(strcmp(grand_child.name(), VCONFIG_ID_TAG) == 0)
      {
        if(atoi((char*)grand_child.text().get()) == vconfig.vlan_id)
        {
          vlan_id_match = true;
        }
      }
      if(strcmp(grand_child.name(), IPAOffload_TAG) == 0)
      {
        found_ipa_offload = true;
        temp = grand_child;
      }
      if(iface_match && vlan_id_match && found_ipa_offload)
      {
        LOG_MSG_INFO1("Config already set for %s with vlan id %d with offload flag %d",
                      vconfig.local_iface, vconfig.vlan_id, vconfig.ipa_offload);
        temp.text().set(vconfig.ipa_offload);
        QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
        return true;
      }
    }
  }

  //append a new VCONFIG XML node
  subchild = root.append_child(VCONFIG_TAG);

  memset(data, 0, sizeof(data));
  snprintf(data, MAX_STRING_LENGTH, "%s",vconfig.local_iface);
  subchild = subchild.append_child(VCONFIG_IFACE_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);
  LOG_MSG_INFO1("Save VLAN config added first node", 0, 0, 0);

  subchild = subchild.parent();

  memset(data, 0, sizeof(data));
  snprintf(data,MAX_STRING_LENGTH,"%d",vconfig.vlan_id);
  subchild = subchild.append_child(VCONFIG_ID_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  subchild = subchild.parent();

  memset(data, 0, sizeof(data));
  snprintf(data,MAX_STRING_LENGTH,"%d",vconfig.ipa_offload);
  subchild = subchild.append_child(IPAOffload_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  LOG_MSG_INFO1("Save VLAN config added nodes", 0, 0, 0);
  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);

  return true;

}

/*=====================================================
  FUNCTION DeleteVLANConfigFromXML
======================================================*/
/*!
@brief
  Deletes VLAN Config from XML.

@parameters
qcmap_msgr_vlan_config_v01 vconfig

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
bool QCMAP_Virtual_LAN::DeleteVLANConfigFromXML(qcmap_msgr_vlan_config_v01 vconfig) const
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char xml_iface[MAX_STRING_LENGTH] = {0};
  char xml_vlan_id[MAX_STRING_LENGTH] = {0};
  bool found = false;

  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  /* Delete child with matching config*/
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG);

  snprintf(data,MAX_STRING_LENGTH,"%d",vconfig.vlan_id);
  for(child = root.first_child(); child; child = child.next_sibling())
  {
    if(!strncmp(child.name(), VCONFIG_TAG,strlen(VCONFIG_TAG)))
    {
      strlcpy(xml_iface,child.child(VCONFIG_IFACE_TAG).child_value(), MAX_STRING_LENGTH);
      strlcpy(xml_vlan_id,child.child(VCONFIG_ID_TAG).child_value(), MAX_STRING_LENGTH);
      if(!strncmp(xml_iface, vconfig.local_iface, strlen(vconfig.local_iface)) &&
         (atoi(xml_vlan_id) == vconfig.vlan_id))
      {
        LOG_MSG_INFO1("Deleting vlan config with iface=%s, vlan_id %s from xml", xml_iface, xml_vlan_id, 0);
        root.remove_child(child);
        QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
        found = true;
        break;
      }
    }
  }

  return found;
}




/*=====================================================
                 VLAN Public Functions.
  =====================================================*/
/*===========================================================================
  FUNCTION AddDeleteVLANOnIface
==========================================================================*/
/*!
@brief
  Configures VLAN on the iface passed.

@parameters
 char * iface_name

@return
- None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Virtual_LAN::AddDeleteVLANOnIface
(
  char   *iface_name,
  bool   link_up
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  bool iface_match, ipa_offload_found;
  int16_t vlan_id = 0;
  uint8_t temp_ipa_offload;
  qcmap_msgr_vlan_config_v01 vlan_config_for_add = {0};
  qcmap_vlan_item vlan_config_for_delete = {0};
  qmi_error_type_v01 qmi_err_num;
  bool is_ipa_offloaded = false;

  if(NULL == iface_name)
  {
    LOG_MSG_ERROR("iface_name arg is NULL", 0, 0, 0);
    return;
  }

  if(NULL == QcMapMgr) {
    LOG_MSG_ERROR("Connection Manager is NULL", 0, 0, 0);
    return;
  }

  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return;
  }

  /* Append new vconfig to existing vlan tag*/
  if((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG)) == NULL)
  {
    LOG_MSG_ERROR("Error finding VLAN tag in xml file", 0, 0, 0);
    return;
  }

  if(link_up)
  {
    LOG_MSG_INFO1("received link up event for: %s", iface_name, 0, 0);
    //for this physical iface bring up all VLANs
    for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
    {
      iface_match = ipa_offload_found= false;
      vlan_id = 0;
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        if(strcmp(grand_child.name(), VCONFIG_IFACE_TAG) == 0)
        {
          if(strcmp((char*)grand_child.text().get(), iface_name) == 0)
          {
            iface_match = true;
          }
        }
        if(strcmp(grand_child.name(), VCONFIG_ID_TAG) == 0)
        {
          vlan_id = atoi((char*)grand_child.text().get());
        }
        if(strcmp(grand_child.name(), IPAOffload_TAG) == 0)
        {
          ipa_offload_found = true;
          temp_ipa_offload = (uint8_t)atoi((char*)grand_child.text().get());
        }
        if(iface_match && (vlan_id > 0) && ipa_offload_found)
        {
          memset(&vlan_config_for_add, 0, sizeof(vlan_config_for_add));

          //vlan id
          vlan_config_for_add.vlan_id = vlan_id;

          //iface name
          snprintf(vlan_config_for_add.local_iface, sizeof(vlan_config_for_add.local_iface), "%s",
                   iface_name);

          //ipa offload flag
          vlan_config_for_add.ipa_offload = temp_ipa_offload;

          if(!this->SetVLANConfig(vlan_config_for_add, false, &qmi_err_num, &is_ipa_offloaded))
          {
            LOG_MSG_ERROR("error with setting VLAN config for %s.%d",
                          vlan_config_for_add.local_iface, vlan_config_for_add.vlan_id, 0);
            continue;
          }
          else if ((vlan_config_for_add.ipa_offload == 1) && !(is_ipa_offloaded))
          {
            LOG_MSG_ERROR("Setting VLAN config without offload for %s.%d as IPA offload count already reached max",
                          vlan_config_for_add.local_iface, vlan_config_for_add.vlan_id, 0);
            continue;
          }
          break;
        }
      }
    }

  } else {

    LOG_MSG_INFO1("received link down event for: %s", iface_name, 0, 0);

    //for this physical iface bring down all VLANs and keep bridge lingering
    for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
    {
      iface_match = false;
      vlan_id = 0;
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        if(strcmp(grand_child.name(), VCONFIG_IFACE_TAG) == 0)
        {
          if(strcmp((char*)grand_child.text().get(), iface_name) == 0)
          {
            iface_match = true;
          }
        }
        if(strcmp(grand_child.name(), VCONFIG_ID_TAG) == 0)
        {
          vlan_id = atoi((char*)grand_child.text().get());
        }
        if(iface_match && (vlan_id > 0))
        {
          memset(&vlan_config_for_delete, 0, sizeof(vlan_config_for_delete));

          //vlan id
          vlan_config_for_delete.vlan_id = vlan_id;

          //iface name
          snprintf(vlan_config_for_delete.iface_name, sizeof(vlan_config_for_delete.iface_name),
                   "%s", iface_name);

          //phy type
          if(!strcmp(vlan_config_for_delete.iface_name, ETH_IFACE))
          {
            vlan_config_for_delete.phy_iface_type = QCMAP_TETH_ETH;

          } else if(!strcmp(vlan_config_for_delete.iface_name, RNDIS_IFACE)) {

            vlan_config_for_delete.phy_iface_type = QCMAP_TETH_RNDIS;

          } else if(!strcmp(vlan_config_for_delete.iface_name, ECM_IFACE)) {

            vlan_config_for_delete.phy_iface_type = QCMAP_TETH_ECM;

          } else {
            LOG_MSG_ERROR("invalid PHY type: %s", vlan_config_for_delete.iface_name, 0, 0);
            vlan_config_for_delete.phy_iface_type = QCMAP_TETH_MIN;
            break;
          }

          this->DeleteVLAN(vlan_config_for_delete, false);
          break;
        }
      }
    }
  }

  QCMAP_L2TP* QcMapL2TPMgr= QCMAP_L2TP::Get_Instance(false);

  /* Install/Del L2TP on this VLAN */
  if(QcMapL2TPMgr)
  {
    /*Sleep is required here to give enough time for route setup to complete*/
    sleep(VLAN_SLEEP_INTERVAL);

    if(link_up)
    {
      snprintf(iface_name, QCMAP_MAX_IFACE_NAME_SIZE_V01, "%s.%d", vlan_config_for_add.local_iface,
               vlan_id);
    } else {
      snprintf(iface_name, QCMAP_MAX_IFACE_NAME_SIZE_V01, "%s.%d",
               vlan_config_for_delete.iface_name, vlan_id);
    }

    if(!QcMapL2TPMgr->InstallDelTunnelsOnVLANIface(iface_name, link_up))
    {
      LOG_MSG_ERROR("Install/Delete L2TP tunnels on VLAN id %d unsuccesful", vlan_id, 0, 0);
    }
  } else {
    LOG_MSG_ERROR("L2TP instance NULL", 0, 0, 0);
  }


  return;
}

/*===========================================================================
  FUNCTION IsPhyLinkUp
==========================================================================*/
/*!
@brief
  Returns if the physical interface is up.

@parameters
 QCMAP_Virtual_LAN_phy_iface iface_type

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_Virtual_LAN::IsPhyLinkUp(QCMAP_Virtual_LAN_phy_iface iface_type) const
{
  bool phy_up = false;
  int netlink_sock_fd = 0;
  int ioctl_sock_fd = 0;
  int num_bytes_recv;
  struct sockaddr_nl kernel_addr;
  struct ifreq ifr;
  qcmap_nl_req_type nlrmsg;
  struct iovec iov_req, iov_reply;
  struct msghdr msg_req, msg_reply;
  unsigned char buffer[MAX_NL_PAYLOAD] = {0};
  struct nlmsghdr* nlmhdr = NULL;
  struct ifinfomsg* iface_info = NULL;


  if((iface_type <= QCMAP_TETH_MIN) || (iface_type >= QCMAP_MAX_PHY_LAN_IFACE))
  {
    LOG_MSG_ERROR("Invalid iface type passed: %d", iface_type, 0, 0);
    return false;
  }

   if((netlink_sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
  {
    LOG_MSG_ERROR("error creating netlink socket: %s", strerror(errno), 0, 0);
    return false;
  }

  //set the kernelspace addr
  memset(&kernel_addr, 0, sizeof(kernel_addr));
  memset(&nlrmsg, 0, sizeof(nlrmsg));
  memset(&iov_req, 0, sizeof(iov_req));
  memset(&msg_req, 0, sizeof(msg_req));


  kernel_addr.nl_family = AF_NETLINK;
  //already memset default PID 0 to mean kernel space

  //set the netlink message header and payload
  nlrmsg.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(nlrmsg));
  nlrmsg.hdr.nlmsg_type = RTM_GETLINK;
  //is a get request, root, and return all matching
  nlrmsg.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  nlrmsg.hdr.nlmsg_seq = 1;
  nlrmsg.hdr.nlmsg_pid = getpid();

  nlrmsg.gen.rtgen_family = AF_INET | AF_INET6; //IPv4 and IPv6 ifaces

  //fill out iov and msg
  iov_req.iov_base = (void*)&(nlrmsg);
  iov_req.iov_len = nlrmsg.hdr.nlmsg_len;

  msg_req.msg_name = (void*)&kernel_addr;
  msg_req.msg_namelen = sizeof(kernel_addr);
  msg_req.msg_iov = &iov_req;
  msg_req.msg_iovlen = 1;

  if(sendmsg(netlink_sock_fd, (struct msghdr*)&msg_req, 0) < 0)
  {
    LOG_MSG_ERROR("error sending netlink message: %s", strerror(errno), 0, 0);
    phy_up = false;
    goto close_netlink_sock_fd;
  }

  /* Receive netlink reply */

  //clear
  memset(&msg_reply, 0, sizeof(msg_reply));
  memset(&iov_reply, 0, sizeof(iov_reply));

  iov_reply.iov_base = (void*)buffer;
  iov_reply.iov_len = sizeof(buffer);

  msg_reply.msg_name = (void*)&kernel_addr;
  msg_reply.msg_namelen = sizeof(kernel_addr);
  msg_reply.msg_iov = &iov_reply;
  msg_reply.msg_iovlen = 1;

  while(1)
  {
    //recv netlink msg
    if((num_bytes_recv = recvmsg(netlink_sock_fd, &msg_reply, 0)) < 0)
    {
      LOG_MSG_ERROR("error receiving from netlink socket: %s", strerror(errno), 0, 0);
      phy_up = false;
      goto close_netlink_sock_fd;
    }

    LOG_MSG_INFO1("num_bytes_recv = %d", num_bytes_recv, 0, 0);

    if((nlmhdr = (struct nlmsghdr*)buffer) == NULL)
    {
      LOG_MSG_ERROR("nlmhdr is NULL", 0, 0, 0);
      phy_up = false;
      goto close_netlink_sock_fd;
    }

    while(NLMSG_OK(nlmhdr, num_bytes_recv))
    {
      if((iface_info = (struct ifinfomsg*)NLMSG_DATA(nlmhdr)) == NULL)
      {
        LOG_MSG_ERROR("iface info from nlmsg is NULL", 0, 0, 0);
        nlmhdr = NLMSG_NEXT(nlmhdr, num_bytes_recv);
        continue;
      }

      switch(nlmhdr->nlmsg_type)
      {
        case(RTM_NEWLINK):
        {
          LOG_MSG_INFO1("Received a netlink msg for index: %u", iface_info->ifi_index, 0, 0);

          if((ioctl_sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
          {
            LOG_MSG_ERROR("error with creating ioctl socket: %s", strerror(errno), 0, 0);
            phy_up = false;
            goto close_netlink_sock_fd;
          }

          memset(&ifr, 0, sizeof(ifr));
          ifr.ifr_ifindex = iface_info->ifi_index;
          if(ioctl(ioctl_sock_fd, SIOCGIFNAME, &ifr) < 0)
          {
            LOG_MSG_ERROR("error with ioctl: %s", strerror(errno), 0, 0);
            phy_up = false;
            goto close_ioctl_sock_fd;
          }
          LOG_MSG_INFO1("Received a netlink msg for iface name: %s", ifr.ifr_name, 0, 0);


          //check IFF_UP and IFF_LOWER_UP flags
          if(!(iface_info->ifi_flags & IFF_UP) || !(iface_info->ifi_flags & IFF_LOWER_UP))
          {
            LOG_MSG_INFO1("%s is physically down", ifr.ifr_name, 0, 0);
          } else {
            LOG_MSG_INFO1("%s is physically up", ifr.ifr_name, 0, 0);

            switch(iface_type)
            {
              case(QCMAP_TETH_ECM):
              {
                if(!strncmp(ifr.ifr_name, ECM_IFACE, strlen(ECM_IFACE)))
                {
                  LOG_MSG_INFO1("Verified ECM is physically up", 0, 0, 0);
                  phy_up = true;
                  goto close_ioctl_sock_fd;
                }
                break;
              }
              case(QCMAP_TETH_RNDIS):
              {
                if(!strncmp(ifr.ifr_name, RNDIS_IFACE, strlen(RNDIS_IFACE)))
                {
                  LOG_MSG_INFO1("Verified RNDIS is physically up", 0, 0, 0);
                  phy_up = true;
                  goto close_ioctl_sock_fd;
                }
                break;
              }
              case(QCMAP_TETH_ETH):
              {
                if(!strncmp(ifr.ifr_name, ETH_IFACE, strlen(ETH_IFACE)))
                {
                  LOG_MSG_INFO1("Verified ETH is physically up", 0, 0, 0);
                  phy_up = true;
                  goto close_ioctl_sock_fd;
                }
                break;
              }
              default:
              {
                LOG_MSG_ERROR("Invalid iface type: %d", iface_type, 0, 0);
                break;
              }
            }
          }

          //close socket
          if((ioctl_sock_fd > 0) && close(ioctl_sock_fd))
          {
            LOG_MSG_ERROR("error closing ioctl socket: %s", strerror(errno), 0, 0);
          }
          ioctl_sock_fd = 0;

          break;
        }
        case(NLMSG_DONE):
        {
          LOG_MSG_INFO1("Received NLMSG_DONE", 0, 0, 0);
          goto close_ioctl_sock_fd;
          break;
        }
        default:
        {
          LOG_MSG_INFO1("Received a netlink msg type: %u", nlmhdr->nlmsg_type, 0, 0);
          break;
        }
      }

      nlmhdr = NLMSG_NEXT(nlmhdr, num_bytes_recv);
    }
  }


  close_ioctl_sock_fd:
    if((ioctl_sock_fd > 0) && close(ioctl_sock_fd))
    {
      LOG_MSG_ERROR("error closing ioctl socket: %s", strerror(errno), 0, 0);
    }

  close_netlink_sock_fd:
    if((netlink_sock_fd > 0) && close(netlink_sock_fd))
    {
      LOG_MSG_ERROR("error closing netlink socket: %s", strerror(errno), 0, 0);
    }

  return phy_up;
}

/*===========================================================================
  FUNCTION GetIPAddrOfPhyLink
==========================================================================*/
/*!
@brief
  Returns IP addresses of physical interface.

@parameters
  QCMAP_Virtual_LAN_phy_iface iface_type,
  uint32 *ipv4_addr,
  struct in6_addr *ipv6_addr

@return
  true if phy interface is up.
  false otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_Virtual_LAN::GetIPAddrOfPhyLink
(
  QCMAP_Virtual_LAN_phy_iface iface_type,
  uint32 *ipv4_addr,
  struct in6_addr *ipv6_addr
) const
{
  struct ifaddrs* ifaddr = NULL;
  char buffer[INET6_ADDRSTRLEN] = {0};
  bool ipv4_found = false;
  bool ipv6_found = false;

  if(NULL == ipv4_addr)
  {
    LOG_MSG_ERROR("ipv4_addr is NULL", 0, 0, 0);
    return false;
  } else if(NULL == ipv6_addr) {
    LOG_MSG_ERROR("ipv6_addr is NULL", 0, 0, 0);
    return false;
  } else if(iface_type <= QCMAP_TETH_MIN || iface_type >= QCMAP_MAX_PHY_LAN_IFACE) {
    LOG_MSG_ERROR("iface_type out of bounds: %d", iface_type, 0, 0);
    return false;
  }

  if(getifaddrs(&ifaddr))
  {
    LOG_MSG_ERROR("error with getifaddrs: %s", strerror(errno), 0, 0);
    return false;
  }

  //ifaddr will be a linked list we have to traverse
  for(struct ifaddrs* ifa = ifaddr; NULL != ifa; ifa = ifa->ifa_next)
  {
    if(NULL == ifa)
    {
      break;
    } else if(NULL == ifa->ifa_addr) {
      continue;
    }

    memset(buffer, 0, sizeof(buffer));
    switch(ifa->ifa_addr->sa_family)
    {
      case(AF_INET):
      {
        if(getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), buffer, INET6_ADDRSTRLEN, NULL, 0,
                       NI_NUMERICHOST)) //return the IPv4 address into buffer
        {
          LOG_MSG_ERROR("%s", gai_strerror(errno), 0, 0);
          continue;
        }
        LOG_MSG_INFO1("%s has address %s", ifa->ifa_name, buffer, 0);

        //copy IPv4 addr
        if ( (IS_IFACE_TYPE_SAME(iface_type, QCMAP_TETH_ECM) &&
            IS_IFACE_NAME_SAME(ifa->ifa_name, ECM_IFACE) ) ||
            (IS_IFACE_TYPE_SAME(iface_type, QCMAP_TETH_RNDIS) &&
            IS_IFACE_NAME_SAME(ifa->ifa_name, RNDIS_IFACE) ) ||
            (IS_IFACE_TYPE_SAME(iface_type, QCMAP_TETH_ETH) &&
            IS_IFACE_NAME_SAME(ifa->ifa_name, ETH_IFACE) ) )
        {
          *ipv4_addr = ntohl(((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr);
          LOG_MSG_INFO1("Found IPv4 address", 0, 0, 0);
          ipv4_found = true;
        }
        else
        {
          LOG_MSG_ERROR("Invalid iface:%d, or if_name:%s", iface_type, ifa->ifa_name, 0);
        }
        break;
      }
      case(AF_INET6):
      {
        if(getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), buffer, INET6_ADDRSTRLEN, NULL, 0,
                       NI_NUMERICHOST)) //return the IPv6 address into buffer
        {
          LOG_MSG_ERROR("%s", gai_strerror(errno), 0, 0);
          continue;
        }
        LOG_MSG_INFO1("%s has address %s", ifa->ifa_name, buffer, 0);

        //copy IPv6 addr
        if ( (IS_IFACE_TYPE_SAME(iface_type, QCMAP_TETH_ECM) &&
            IS_IFACE_NAME_SAME(ifa->ifa_name, ECM_IFACE) ) ||
            (IS_IFACE_TYPE_SAME(iface_type, QCMAP_TETH_RNDIS) &&
            IS_IFACE_NAME_SAME(ifa->ifa_name, RNDIS_IFACE) ) ||
            (IS_IFACE_TYPE_SAME(iface_type, QCMAP_TETH_ETH) &&
            IS_IFACE_NAME_SAME(ifa->ifa_name, ETH_IFACE) ) )
        {
          *ipv6_addr = ((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr;
          LOG_MSG_INFO1("Found IPv6 address", 0, 0, 0);
          ipv6_found = true;
        }
        else
        {
          LOG_MSG_ERROR("Invalid iface:%d, or if_name:%s", iface_type, ifa->ifa_name, 0);
        }
        break;
      }
      default:
      {
        continue;
        break;
      }
    }
  }

  freeifaddrs(ifaddr);

  return (ipv4_found && ipv6_found);
}

/*=====================================================
  FUNCTION SetVLANConfig
======================================================*/
/*!
@brief
  Sets VLAN Config and brings up VLAN for a given config list.

@parameters
  qcmap_msgr_vlan_config_v01 vlan_config,
  bool createConfig if true, then update XML, else update only
                    runtime settings.
  qmi_error_type_v01* qmi_err_num,
  bool* is_ipa_offloaded

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
bool QCMAP_Virtual_LAN::SetVLANConfig
(
  qcmap_msgr_vlan_config_v01 vlan_config,
  bool createConfig,
  qmi_error_type_v01 *qmi_err_num,
  bool *is_ipa_offloaded
)
{
  char vlan_id_str[MAX_VLAN_STR_LEN] = {0};
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_Virtual_LAN_phy_iface phy_iface_type = QCMAP_TETH_MIN;
  qcmap_vlan_item vlan_item;
  QCMAP_LAN* lan_obj = NULL;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_L2TP* QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = NULL;
  QCMAP_Backhaul* QcMapBackhaul = NULL;
  qcmap_msgr_cradle_mode_v01 cradle_mode;
  qcmap_msgr_ethernet_mode_v01 eth_mode;
  qcmap_msgr_interface_type_enum_v01 intf_type;
  bool packet_stats_enabled = false;

  if(NULL == qmi_err_num)
  {
    LOG_MSG_ERROR("qmi_err_num is NULL", 0, 0, 0);
    return false;
  } else if(NULL == lan_mgr) {
    LOG_MSG_ERROR("lan mgr is NULL", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if(QcMapMgr && !(IS_VLAN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("VLAN is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }
  else if (NULL == is_ipa_offloaded)
  {
    LOG_MSG_ERROR("Invalid parameters passed",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  //ensure that Cradle is in LAN router and ETH is in LAN router
  if((QCMAP_Backhaul_Cradle::GetCradleMode(&cradle_mode, qmi_err_num)) &&
     (QCMAP_MSGR_CRADLE_LAN_ROUTER_V01 != cradle_mode))
  {
    LOG_MSG_INFO1("Cradle in mode %d, must be in LAN router mode", cradle_mode, 0, 0);
    *qmi_err_num = QMI_ERR_INCOMPATIBLE_STATE_V01;
    return false;
  } else if((QCMAP_Backhaul_Ethernet::GetEthBackhaulMode(&eth_mode, qmi_err_num)) &&
            (QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01 != eth_mode)) {
    LOG_MSG_INFO1("Ethernet in mode %d, must be in LAN router mode", eth_mode, 0, 0);
    *qmi_err_num = QMI_ERR_INCOMPATIBLE_STATE_V01;
    return false;
  }

  if((phy_iface_type = GetIfaceTypeFromIface(vlan_config.local_iface)) == QCMAP_TETH_MIN)
  {
    LOG_MSG_ERROR("Iface: %s not supported for VLAN", vlan_config.local_iface, 0, 0);
    *qmi_err_num= QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  *qmi_err_num= QMI_ERR_NONE_V01;

  if(!IS_VLAN_ID_VALID(vlan_config.vlan_id))
  {
    LOG_MSG_ERROR("Unsupported VLAN-ID: %d passed in config", vlan_config.vlan_id, 0, 0);
    *qmi_err_num= QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  /*If phy link is enabled, configure VLAN*/
  memset(&vlan_item, 0, sizeof(qcmap_vlan_item));
  vlan_item.phy_iface_type = phy_iface_type;
  if(!(this->ConstructVLANNode(vlan_config, &vlan_item)))
  {
    LOG_MSG_ERROR("Error in constructing VLAN Item", 0, 0, 0);
    *qmi_err_num= QMI_ERR_INTERNAL_V01;
    return false;
  }

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
      !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
  //check if IPA offload was allowed
  if((NULL == this->ipa_offloader) ||
     !(this->ipa_offloader->IsVLANIfaceOffloadAllowed(vlan_config.local_iface, vlan_config.vlan_id)))
  {
    LOG_MSG_ERROR("Over-riding user-configuration to disable IPA offload",0,0,0);
    vlan_config.ipa_offload = 0; //manual intervention to set ipa offload in xml to false
    vlan_item.ipa_offload = false;
  }
#endif

  this->ConfigureVLAN(vlan_item);

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
      !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
  if(vlan_item.ipa_offload && this->ipa_offloader)
  {
    this->ipa_offloader->UpdateDesiredOffloadInfo((const char*)vlan_item.iface_name,
                                                    vlan_item.vlan_id, true);
  }
#endif

  if(vlan_config.ipa_offload)
  {
    sleep(VLAN_SLEEP_INTERVAL); //sleep to allow IPA HW to configure itself
    *is_ipa_offloaded = true;
  }
  else
  {
    *is_ipa_offloaded = false;
  }

  //L2TP class expects entire VLAN iface name
  strlcat(vlan_item.iface_name, ".", sizeof(vlan_item.iface_name));
  if(snprintf(vlan_id_str, sizeof(vlan_id_str), "%u", vlan_config.vlan_id) < 0)
  {
    LOG_MSG_ERROR("error with snprintf: %s", strerror(errno), 0, 0);
  } else {
    strlcat(vlan_item.iface_name, vlan_id_str, sizeof(vlan_item.iface_name));
  }

  //ask LAN Mgr to associate vlan iface to its corresponding bridge
  intf_type = IfaceNameToEnum((const char*)vlan_config.local_iface);
  if (intf_type != QCMAP_MSGR_INTERFACE_TYPE_ENUM_MIN_ENUM_VAL_V01)
  {
    if(((lan_obj = lan_mgr->AssociateIface(intf_type, vlan_config, createConfig)) == NULL))
    {
      LOG_MSG_ERROR("failed to associate to bridge: %s.%d",
                    vlan_config.local_iface,
                    vlan_config.vlan_id, 0);
      *qmi_err_num= QMI_ERR_INTERNAL_V01;
      return false;
    }
  }
  else
  {
    LOG_MSG_ERROR("Failed to get interface type for %s", vlan_config.local_iface, 0,0);
    *qmi_err_num= QMI_ERR_INTERNAL_V01;
    return false;
  }

  if(vlan_item.is_up && QcMapL2TPMgr->InstallDelTunnelsOnVLANIface(vlan_item.iface_name, true))
  {
    LOG_MSG_ERROR("Install/Delete L2TP tunnels"
                  "on VLAN id %d not succeded",vlan_item.vlan_id,0,0);
  }

  /* Loop through Backhaul objects till you get the one corresponding to correct vlan.
     If one exits... */
  DECLARE_HASH_MAP_ITERATOR_FOR_BACKHAUL;
  START_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash);
  while(END_OF_HASH_MAP_FOR_BACKHAUL(QCMAP_ConnectionManager::QCMAP_Backhaul_Hash))
  {
    QcMapBackhaulWWAN = GET_BACKHAUL_WWAN_OBJ;
    QcMapBackhaul = GET_BACKHAUL_OBJ_AND_INC;
    if (QcMapBackhaul->vlan_id == vlan_config.vlan_id)
    {
      LOG_MSG_INFO1("Found Backhaul tied to VLAN ID %d", vlan_config.vlan_id,0,0);
      break;
    }
  }

  if(QcMapBackhaulWWAN && !QcMapBackhaulWWAN->EnableVlanPdnRules())
  {
    LOG_MSG_ERROR("Unable to install rules",0,0,0);
  }

  if (createConfig)
  {
    if(!this->SetVLANConfigToXML(vlan_config))
    {
      LOG_MSG_ERROR("Failed to write vlan config to xml", 0, 0, 0);
      *qmi_err_num= QMI_ERR_INTERNAL_V01;
      return false;
    }
  }

  /*Disable per client stats on adding VLAN interface*/
  if (QcMapMgr && QcMapMgr->GetPacketStatsStatus(qmi_err_num))
  {
    packet_stats_enabled = true;
    if (!QcMapMgr->DisablePacketStats(qmi_err_num))
    {
      LOG_MSG_INFO1("Disable Packet Stats fails.\n",0,0,0);
    }
  }

  #if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
      !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
  if(this->ipa_offloader && QcMapMgr &&
     QcMapMgr->TriggerAutoReboot(this->ipa_offloader->GetAutoRebootFlag(), QCMAP_CM_VLAN_ADDED_ON_PHY))
  {
    //Warn user, that we will be doing autoreboot
    LOG_MSG_ERROR("Auto reboot required for IPA HW offload to take affect", 0, 0, 0);
    *qmi_err_num = QMI_ERR_OP_IN_PROGRESS_V01;
    return true;
  }

  if (QcMapMgr && QcMapMgr->TriggerAutoReboot(packet_stats_enabled, QCMAP_CM_VLAN_ADD_WITH_PKT_STATS_ON))
  {
    //Warn user, that we will be doing autoreboot
    LOG_MSG_ERROR("Auto reboot required for Disable packet stats to take affect", 0, 0, 0);
    *qmi_err_num = QMI_ERR_OP_IN_PROGRESS_V01;
    return true;
  }
  #endif

  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*=====================================================
  FUNCTION GetVLANConfig
======================================================*/
/*!
@brief
  Gets VLAN Config.

@parameters
  qcmap_msgr_vlan_config_v01* vlan_config,
  uint32_t* vlan_config_list_len,
  qmi_error_type_v01* qmi_err_num

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
bool QCMAP_Virtual_LAN::GetVLANConfig
(
  qcmap_msgr_vlan_config_v01* vlan_config_list,
  uint32_t* vlan_config_list_len,
  qmi_error_type_v01* qmi_err_num
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  qcmap_msgr_vlan_config_v01 temp_vlan_config;
  bool found_iface, found_vlan_id, found_ipa_offload;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(NULL == qmi_err_num)
  {
    LOG_MSG_ERROR("qmi_err_num is NULL", 0, 0, 0);
    return false;
  }

  if(QcMapMgr && !(IS_VLAN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("VLAN is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  } else if (NULL == vlan_config_list || NULL == vlan_config_list_len) {
    LOG_MSG_ERROR("Invalid parameters passed",0,0,0);
    *qmi_err_num= QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  *vlan_config_list_len = 0;

  //read VLAN config and fill out information
  if(!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.", 0, 0, 0);
    *qmi_err_num= QMI_ERR_INTERNAL_V01;
    return false;
  }

  if((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG)) == NULL)
  {
    LOG_MSG_ERROR("No VLANs configured", 0, 0, 0);
    *qmi_err_num= QMI_ERR_INTERNAL_V01;
    return false;
  }



  for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
  {
    memset(&temp_vlan_config, 0, sizeof(temp_vlan_config));
    found_iface = found_vlan_id = found_ipa_offload = false;

    for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
        grand_child = grand_child.next_sibling())
    {
      //iface name
      if(strcmp(grand_child.name(), VCONFIG_IFACE_TAG) == 0)
      {
        if(sizeof(temp_vlan_config.local_iface) > (strlen((char*)grand_child.text().get()) - 1))
        {
          strlcpy(temp_vlan_config.local_iface, (char*)grand_child.text().get(),
                 sizeof(temp_vlan_config.local_iface));
          found_iface = true;
        }
      }

      //vlan id
      if(strcmp(grand_child.name(), VCONFIG_ID_TAG) == 0)
      {
        temp_vlan_config.vlan_id = (int16_t)atoi((char*)grand_child.text().get());
        found_vlan_id = true;
      }

      //ipa offload flag
      if(strcmp(grand_child.name(), IPAOffload_TAG) == 0)
      {
        temp_vlan_config.ipa_offload = (uint8_t)atoi((char*)grand_child.text().get());
        found_ipa_offload = true;
      }
    }

    //fill out cached information if found all configuration items
    if(found_iface && found_vlan_id && found_ipa_offload)
    {
      if(*vlan_config_list_len >= (QCMAP_MSGR_MAX_VLAN_ENTRIES_V01 - 1))
      {
        LOG_MSG_INFO1("Number of VLAN ifaces configured: %d", *vlan_config_list_len, 0, 0);
        *qmi_err_num= QMI_ERR_NONE_V01;
        return true;
      }

      memset(vlan_config_list[*vlan_config_list_len].local_iface, 0,
             sizeof(vlan_config_list[*vlan_config_list_len].local_iface));
      strlcpy(vlan_config_list[*vlan_config_list_len].local_iface, temp_vlan_config.local_iface,
             sizeof(vlan_config_list[*vlan_config_list_len].local_iface));

      vlan_config_list[*vlan_config_list_len].vlan_id = temp_vlan_config.vlan_id;

      vlan_config_list[*vlan_config_list_len].ipa_offload = temp_vlan_config.ipa_offload;

      (*vlan_config_list_len) = ((*vlan_config_list_len) + 1) % QCMAP_MSGR_MAX_VLAN_ENTRIES_V01;
    }
  }

  LOG_MSG_INFO1("Number of VLAN ifaces configured: %d", *vlan_config_list_len, 0, 0);
  *qmi_err_num= QMI_ERR_NONE_V01;
  return true;
}

/*=====================================================
  FUNCTION DeleteVLANConfig
======================================================*/
/*!
@brief
  Deletes VLAN Config and brings down VLAN from given config list.

@parameters
  qcmap_msgr_vlan_config_v01 vlan_config,
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
bool QCMAP_Virtual_LAN::DeleteVLANConfig
(
  qcmap_msgr_vlan_config_v01 vlan_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  char vlan_id_str[MAX_VLAN_STR_LEN] = {0};
  QCMAP_Virtual_LAN_phy_iface phy_iface_type = QCMAP_TETH_MIN;
  qcmap_vlan_item vlan_item;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_L2TP* QcMapL2TPMgr = QCMAP_L2TP::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  qcmap_msgr_interface_type_enum_v01 intf_type;
  QCMAP_LAN *lan_obj = NULL;

  if(NULL == qmi_err_num)
  {
    LOG_MSG_ERROR("qmi_err_num is NULL", 0, 0, 0);
    return false;
  } else if(NULL == lan_mgr) {
    LOG_MSG_ERROR("lan mgr is NULL", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if(QcMapMgr && !(IS_VLAN_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_ERROR("VLAN is not allowed on this target(%d)",
                  QcMapMgr->target,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if((phy_iface_type = GetIfaceTypeFromIface(vlan_config.local_iface)) ==
      QCMAP_TETH_MIN)
  {
    LOG_MSG_ERROR("Iface: %s not supported for VLAN", vlan_config.local_iface, 0, 0);
    *qmi_err_num= QMI_ERR_INVALID_ARG_V01;
    return false;
  }


  if(!IS_VLAN_ID_VALID(vlan_config.vlan_id))
  {
    LOG_MSG_ERROR("Unsupported VLAN-ID: %d passed in config", vlan_config.vlan_id,
                  0, 0);
    *qmi_err_num= QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  /* Check if VLAN object exists */
  lan_obj = GET_LAN_OBJECT(vlan_config.vlan_id);
  if (!lan_obj)
  {
    LOG_MSG_ERROR("OOPS!!! lan_obj is NULL, nothing to do.",0,0,0);
    *qmi_err_num= QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  memset(&vlan_item, 0, sizeof(qcmap_vlan_item));
  vlan_item.phy_iface_type = phy_iface_type;
  if(!(this->ConstructVLANNode(vlan_config, &vlan_item)))
  {
    LOG_MSG_ERROR("Error in constructing VLAN List node", 0, 0, 0);
    *qmi_err_num= QMI_ERR_INTERNAL_V01;
    return false;
  }
  this->DeleteVLAN(vlan_item, true);

  //ask LAN Mgr to disassociate vlan iface to its corresponding bridge
  intf_type = IfaceNameToEnum((const char*)vlan_config.local_iface);
  if (intf_type != QCMAP_MSGR_INTERFACE_TYPE_ENUM_MIN_ENUM_VAL_V01)
  {
    if(!(lan_mgr->DisassociateIface(intf_type, vlan_config.vlan_id)))
    {
      LOG_MSG_ERROR("failed to disassociate from bridge: %s.%d",
                    vlan_config.local_iface,
                    vlan_config.vlan_id, 0);
      *qmi_err_num= QMI_ERR_INTERNAL_V01;
      return false;
    }
  }
  else
  {
    LOG_MSG_ERROR("Failed to get interface type for %s", vlan_config.local_iface, 0,0);
    *qmi_err_num= QMI_ERR_INTERNAL_V01;
    return false;
  }

  //L2TP class expects entire VLAN iface
  strlcat(vlan_item.iface_name, ".", sizeof(vlan_item.iface_name));
  if(snprintf(vlan_id_str, sizeof(vlan_id_str), "%u", vlan_config.vlan_id) < 0)
  {
    LOG_MSG_ERROR("error with snprintf: %s", strerror(errno), 0, 0);
  } else {
    strlcat(vlan_item.iface_name, vlan_id_str, sizeof(vlan_item.iface_name));
  }
  if(QcMapL2TPMgr->InstallDelTunnelsOnVLANIface(vlan_item.iface_name, false))
  {
    LOG_MSG_ERROR("Install/Delete L2TP tunnels"
                  "on VLAN id %d not succeded",vlan_item.vlan_id,0,0);
  }

  if(!this->DeleteVLANConfigFromXML(vlan_config))
  {
    LOG_MSG_ERROR("Failed to write vlan config to xml", 0, 0, 0);
  }

  LOG_MSG_INFO1("%s succesful", __func__, 0, 0);

  *qmi_err_num= QMI_ERR_NONE_V01;
  return true;
}

/*=====================================================
                 VLAN Static Functions.
  =====================================================*/

/*=====================================================
  FUNCTION GetPhyIfaceVLANIDFromIface
======================================================*/
/*!
@brief
  Gets physical iface type, vlan ID from iface name.

@parameters
  char *iface_name,\
  QCMAP_Virtual_LAN_phy_iface *phy_type,\
  int16_t *vlan_id @output pram

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
static bool QCMAP_Virtual_LAN::GetPhyIfaceVLANIDFromIface
(
char *iface_name,
QCMAP_Virtual_LAN_phy_iface *phy_type,
int16_t *vlan_id
)
{
  QCMAP_Virtual_LAN_phy_iface phy_iface_type = QCMAP_TETH_MIN;
  char  phy_iface_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  char  vlan_id_text[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  char *ptr = NULL;
/*------------------------------------------------------------------------*/
  if (iface_name == NULL || phy_type == NULL || vlan_id == NULL)
  {
    LOG_MSG_ERROR("GetPhyIfaceVLANIDFromIface() - Invalid input params",\
                                                0,0,0);
    return false;
  }

  ptr = strchr(iface_name,'.');
  if (ptr == NULL)
  {
    strlcpy(phy_iface_name,iface_name,QCMAP_MAX_IFACE_NAME_SIZE_V01);
    *vlan_id = 0;
  }
  else
  {
    strlcpy(phy_iface_name,iface_name,(ptr-iface_name+1));
    strlcpy(vlan_id_text,ptr+1,QCMAP_MAX_IFACE_NAME_SIZE_V01);
    *vlan_id = atoi(vlan_id_text);
  }
  phy_iface_type = GetIfaceTypeFromIface(phy_iface_name);

  if (phy_iface_type == QCMAP_TETH_MIN)
  {
    LOG_MSG_ERROR("GetPhyIfaceVLANIDFromIface() - Invalid physical interface:"
                  "%s in iface_name %s",phy_iface_name,iface_name,0);
    return false;
  }

  *phy_type = phy_iface_type;
  return true;
}

/*===========================================================================
  FUNCTION IsVLANIDUp
==========================================================================*/
/*!
@brief
  Returns if vlan id is up or not.

@parameters
 int16_t vlan_id
 char *iface_name - buffer to fill interface name

@return
  true if vlan id is configured
  fasle if otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static bool QCMAP_Virtual_LAN::IsVLANIDUp(int16_t vlan_id, char *iface_name)
{
  bool vlan_id_configured = false;
  pugi::xml_document xml_file;
  pugi::xml_node root;

  if(!IS_VLAN_ID_VALID(vlan_id))
  {
    LOG_MSG_ERROR("Invalid input parameters: %d", vlan_id, 0, 0);
    return false;
  }

  if(!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.", 0, 0, 0);
    return false;
  }

  //check for VLAN ID
  if((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG)) == NULL)
  {
    LOG_MSG_ERROR("VLAN not configured yet", 0, 0, 0);
    return false;
  }
  for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
  {
    vlan_id_configured = false;
    if(!strcmp(child.name(), VCONFIG_TAG))
    {
      if(child.child(VCONFIG_ID_TAG))
      {
        if(atoi(child.child(VCONFIG_ID_TAG).text().get()) == vlan_id)
        {
          LOG_MSG_INFO1("VLAN id: %d is configured", vlan_id, 0, 0);
          vlan_id_configured = true;
        }
      }

      if(vlan_id_configured && child.child(VCONFIG_IFACE_TAG))
      {
        //if iface name is not null fill it in
        if(NULL != iface_name)
        {
          snprintf(iface_name, sizeof(iface_name), "%s",
                   child.child(VCONFIG_IFACE_TAG).text().get());
          LOG_MSG_INFO1("Returning phy iface name %s", iface_name, 0, 0);
        }
        break;
      }
    }
  }

  return vlan_id_configured;
}

/*===========================================================================
  FUNCTION GetIPAddrforVLAN
==========================================================================*/
/*!
@brief
  Returns IP addresses of VLAN iface corresponding to vlan id. passed

@parameters
  int16_t vlan_id,
  qcmap_ip4_addr_subnet_mask_v01 *ipv4_addr,
  qcmap_ip6_addr_prefix_len_data_v01 *ipv6_addr

@return
  true if vlan id is configured
  fasle if otherwise

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static bool QCMAP_Virtual_LAN::GetIPAddrForVLAN
(
  int16_t vlan_id,
  qcmap_ip4_addr_subnet_mask_v01 *ipv4_addr,
  qcmap_ip6_addr_prefix_len_v01 *ipv6_addr
)
{
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_LAN* lan_obj = NULL;
  QCMAP_LAN* default_lan_obj = NULL;
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  struct in_addr in4;
  struct in6_addr in6;
  char buffer[INET6_ADDRSTRLEN] = {0};

  if(NULL == ipv4_addr)
  {
    LOG_MSG_ERROR("ipv4_addr NULL", 0, 0, 0);
    return false;
  } else if(NULL == ipv6_addr) {
    LOG_MSG_ERROR("ipv6_addr NULL", 0, 0, 0);
    return false;
  } else if(!IS_VLAN_ID_VALID(vlan_id)){
    LOG_MSG_ERROR("vlan_id out of bounds: %d", vlan_id, 0, 0);
    return false;
  } else if(NULL == lan_mgr) {
    LOG_MSG_ERROR("lan mgr is NULL", 0, 0, 0);
    return false;
  } else if((lan_obj = lan_mgr->GetLANBridge(vlan_id)) == NULL) {
    LOG_MSG_ERROR("lan obj for vlan id %d is NULL", vlan_id, 0, 0);
    return false;
  }

  //IPv4 information, but if tied to default profile handle need to give default LAN obj's IPv4 info
  if(IS_DEFAULT_PROFILE(lan_obj->GetBackhaulProfileHandle()))
  {
    if((default_lan_obj = GET_DEFAULT_LAN_OBJECT()) == NULL)
    {
      LOG_MSG_ERROR("default lan obj is NULL", 0, 0, 0);
      return false;
    }
    if((lan_cfg = default_lan_obj->GetLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
      return false;
    }
  } else {
    if((lan_cfg = lan_obj->GetLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
      return false;
    }
  }

  ipv4_addr->addr = ntohl(lan_cfg->apps_ip_addr);
  ipv4_addr->subnet_mask = ntohl(lan_cfg->sub_net_mask);

  memset(&in4, 0, sizeof(in4));
  in4.s_addr = ipv4_addr->addr;
  LOG_MSG_INFO1("VLAN%d has IPv4 addr: %s", vlan_id, inet_ntoa(in4), 0);

  memset(&in4, 0, sizeof(in4));
  in4.s_addr = ipv4_addr->subnet_mask;
  LOG_MSG_INFO1("VLAN%d has IPv4 subnet mask: %s", vlan_id, inet_ntoa(in4), 0);

  //IPv6 information
  snprintf(buffer, sizeof(buffer), "%s:%x::%s", VLAN_BASE_IPV6_ADDR, vlan_id, STATIC_IID);
  if(inet_pton(AF_INET6, buffer, &in6) != 1)
  {
    LOG_MSG_ERROR("error with inet_pton: %s", strerror(errno), 0, 0);
    return false;
  }
  memcpy(ipv6_addr->addr, in6.s6_addr, sizeof(ipv6_addr->addr));
  ipv6_addr->prefix_len = VLAN_IPV6_PREFIX_LEN_BITS;

  if(inet_ntop(AF_INET6, ipv6_addr->addr, buffer, INET6_ADDRSTRLEN) == NULL)
  {
    LOG_MSG_ERROR("error with inet_ntop: %s", strerror(errno), 0, 0);
    return false;
  }

  LOG_MSG_INFO1("VLAN%d has IPv6 addr: %s and IPv6 prefix len in bits: %d", vlan_id, buffer,
                ipv6_addr->prefix_len);

  return true;
}



#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)

/*=====================================================
           QCMAP_VLAN_IPA_Offloader Class Definitions
  =====================================================*/

/*=====================================================
                  Private functions
  =====================================================*/

/*=====================================================
  FUNCTION PrintPhyIfaceUseHashTable
======================================================*/
/*!
@brief
  Logs information on the current state of the PHY iface
  use hashtable

@parameters

@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::PrintPhyIfaceUseHashTable(void) const
{
  LOG_MSG_INFO1("Will print phy iface use for VLAN IPA Offload...", 0, 0, 0);

  for(std::unordered_map<QCMAP_Virtual_LAN_phy_iface,std::vector<int16_t>,
                         std::hash<int>>::const_iterator
      it = this->phy_iface_use_ht.cbegin(); it != this->phy_iface_use_ht.cend(); ++it)
  {
    switch(it->first)
    {
      case(QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_ECM):
      {
        for(std::vector<int16_t>::const_iterator it2 = it->second.cbegin();
            it2 != it->second.cend(); ++it2)
        {
          //KW doesn't like *it
          LOG_MSG_INFO1("ECM configured for offload VLAN: %d", it2[0], 0, 0);
        }
        break;
      }
      case(QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_RNDIS):
      {
        for(std::vector<int16_t>::const_iterator it2 = it->second.cbegin();
            it2 != it->second.cend(); ++it2)
        {
          if(it2 != it->second.cend())
          {
            //KW doesn't like *it
            LOG_MSG_INFO1("RNDIS configured for offload VLAN: %d", it2[0], 0, 0);
          }
        }
        break;
      }
      case(QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_ETH):
      {
        for(std::vector<int16_t>::const_iterator it2 = it->second.cbegin();
            it2 != it->second.cend(); ++it2)
        {
          if(it2 != it->second.cend())
          {
            //KW doesn't like *it
            LOG_MSG_INFO1("ETH configured for offload VLAN: %d", it2[0], 0, 0);
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

/*=====================================================
  FUNCTION IsVLANIfacePreviouslyOffloaded
======================================================*/
/*!
@brief
  Checks if already offloaded the VLAN iface to IPA
  Note: IPA HW currently is a one-shot deal, once called
        IPA_IOC_ADD_VLAN_IFACE, user can not dynamically
        change

@parameters

@return
  true  - Already offloaded
  false - Not offloaded yet

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
bool QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::IsVLANIfacePreviouslyOffloaded
(
  const char *iface_name,
  int16_t vlan_id
) const
{

  for(int i = 0; i < MAX_IPA_OFFLOAD; i++)
  {
    if((!strcmp(this->vlan_iface[i].local_iface, iface_name)) &&
       (vlan_id == this->vlan_iface[i].vlan_id))
    {
      LOG_MSG_INFO1("%s.%d is already offloaded", iface_name, vlan_id, 0);
      return true;
    }
  }

  LOG_MSG_INFO1("%s.%d is not offloaded", iface_name, vlan_id, 0);
  return false;
}

/*=====================================================
  FUNCTION GetOffloadCount
======================================================*/
/*!
@brief
  Retrieves current IPA offload count

@parameters

@return
  unsigned char offload_count

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
unsigned char QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::GetOffloadCount(void) const
{
  LOG_MSG_INFO1("Current ipa offload count: %u", this->offload_count, 0, 0);

  return this->offload_count;
}

/*=====================================================
  FUNCTION UpdateIPACfgFile
======================================================*/
/*!
@brief
  Updates IPA config file for user configured VLAN offload
  Will create file if not created before.

@parameters

@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::UpdateIPACfgFile(void) const
{
  FILE* fp = NULL;
  char iface_str_list[MAX_PHY_IFACE_LIST_STRLEN] = {0};

  LOG_MSG_INFO1("Will update IPA Cfg file: %s", IPA_CFG_FILE_PATH, 0, 0);

  //check for all previous PHY configured for VLAN offload
  for(std::unordered_map<QCMAP_Virtual_LAN_phy_iface,std::vector<int16_t>,
                         std::hash<int>>::const_iterator
      it = this->phy_iface_use_ht.cbegin(); it != this->phy_iface_use_ht.cend(); ++it)
  {
    switch(it->first)
    {
      case(QCMAP_TETH_ECM):
      {
        if(it->second.size() > 0)
        {
          strlcat(iface_str_list, ECM_STR, sizeof(iface_str_list));
        }
        break;
      }
      case(QCMAP_TETH_RNDIS):
      {
        if(it->second.size() > 0)
        {
          strlcat(iface_str_list, RNDIS_STR, sizeof(iface_str_list));
        }
        break;
      }
      case(QCMAP_TETH_ETH):
      {
        if(it->second.size() > 0)
        {
          strlcat(iface_str_list, ETH_STR, sizeof(iface_str_list));
        }
        break;
      }
      default:
      {
        break;
      }
    }
  }

  //overwrite and create file if necessary
  if((fp = fopen(IPA_CFG_FILE_PATH, "w")) == NULL)
  {
    LOG_MSG_ERROR("Error opening %s for writing: %s", IPA_CFG_FILE_PATH, strerror(errno), 0);
    return;
  }

  if (strlen(iface_str_list) == 0)
  {
    //write empty file.
    fprintf(fp, "%s", iface_str_list);
  }
  else
  {
    fprintf(fp, "vlan:%s", iface_str_list);
  }

  if(fclose(fp))
  {
    LOG_MSG_ERROR("Error closing %s for writing: %s", IPA_CFG_FILE_PATH, strerror(errno), 0);
    return;
  }

  LOG_MSG_INFO1("Finished updating IPA Cfg file: %s", IPA_CFG_FILE_PATH, 0, 0);
  return;
}

/*=====================================================
                  Public functions
  =====================================================*/

/*=====================================================
  FUNCTION QCMAP_VLAN_IPA_Offloader
======================================================*/
/*!
@brief
  Constructor

@parameters

@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::QCMAP_VLAN_IPA_Offloader()
{
  qcmap_msgr_vlan_config_v01 temp_vlan_config;
  bool found_iface, found_vlan_id, found_ipa_offload;
  pugi::xml_document xml_file;
  pugi::xml_node root;

  //set everything to zero
  memset(this->vlan_iface, 0, sizeof(vlan_iface));
  this->offload_count = 0; //this will be incremented only when the IOCTL is called
  this->auto_reboot_flag = false;

  this->phy_iface_use_ht.clear();

  //create the buckets and vector sizes to zero
  this->phy_iface_use_ht.insert(std::pair<QCMAP_Virtual_LAN_phy_iface, std::vector<int16_t>>(
  QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_ECM, std::vector<int16_t>()));
  this->phy_iface_use_ht.insert(std::pair<QCMAP_Virtual_LAN_phy_iface, std::vector<int16_t>>(
  QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_RNDIS, std::vector<int16_t>()));
  this->phy_iface_use_ht.insert(std::pair<QCMAP_Virtual_LAN_phy_iface, std::vector<int16_t>>(
  QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_ETH, std::vector<int16_t>()));


  //read VLAN config and fill out information
  if(!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.", 0, 0, 0);
    return;
  }

  if((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG)) == NULL)
  {
    LOG_MSG_ERROR("No VLANs configured", 0, 0, 0);
    return;
  }

  for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
  {
    memset(&temp_vlan_config, 0, sizeof(temp_vlan_config));
    found_iface = found_vlan_id = found_ipa_offload = false;

    for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
        grand_child = grand_child.next_sibling())
    {
      //iface name
      if(strcmp(grand_child.name(), VCONFIG_IFACE_TAG) == 0)
      {
        if(sizeof(temp_vlan_config.local_iface) > (strlen((char*)grand_child.text().get()) - 1))
        {
          strlcpy(temp_vlan_config.local_iface, (char*)grand_child.text().get(),
                 sizeof(temp_vlan_config.local_iface));
          found_iface = true;
        }
      }

      //vlan id
      if(strcmp(grand_child.name(), VCONFIG_ID_TAG) == 0)
      {
        temp_vlan_config.vlan_id = (int16_t)atoi((char*)grand_child.text().get());
        found_vlan_id = true;
      }

      //ipa offload flag
      if(strcmp(grand_child.name(), IPAOffload_TAG) == 0)
      {
        temp_vlan_config.ipa_offload = (uint8_t)atoi((char*)grand_child.text().get());
        found_ipa_offload = true;
      }
    }

    //fill out cached information if found all configuration items
    if(found_iface && found_vlan_id && found_ipa_offload)
    {
      if(temp_vlan_config.ipa_offload)
      {
        switch(GetIfaceTypeFromIface(temp_vlan_config.local_iface))
        {
          case(QCMAP_TETH_ECM):
          {
            this->phy_iface_use_ht.at(QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_ECM).push_back(
                                                                          temp_vlan_config.vlan_id);
            break;
          }
          case(QCMAP_TETH_RNDIS):
          {
            this->phy_iface_use_ht.at(QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_RNDIS).push_back(
                                                                          temp_vlan_config.vlan_id);
            break;
          }
          case(QCMAP_TETH_ETH):
          {
            this->phy_iface_use_ht.at(QCMAP_Virtual_LAN_phy_iface::QCMAP_TETH_ETH).push_back(
                                                                          temp_vlan_config.vlan_id);
            break;
          }
          default:
          {
            LOG_MSG_ERROR("Invalid iface type", 0, 0, 0);
            break;;
          }
        }
      }
    }
  }

  this->PrintPhyIfaceUseHashTable();
  LOG_MSG_INFO1("QCMAP_VLAN_IPA_Offloader object created" , 0, 0, 0);
}

/*=====================================================
  FUNCTION GetAutoRebootFlag
======================================================*/
/*!
@brief
  Get auto reboot flag status which is required for IPA HW offload to take affect

@parameters

@return
  true  - reboot required
  false - reboot not required

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
bool QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::GetAutoRebootFlag(void)
{
  return this->auto_reboot_flag;
}

/*=====================================================
  FUNCTION IsVLANIfaceOffloadAllowed
======================================================*/
/*!
@brief
  Checks if VLAN iface param will be allowed for IPA offload.
  As of current, only 4 one-shot IOCTL for IPA offload is allowed.
  We will check not only if ipa_offload count will be violated, but
  also if given vlan iface has been chosen for offload before.

@parameters
  const char* iface_name
  int16_t vlan_id

@return
  true  - IPA offload allowed
  false - IPA offload not allowed

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
bool QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::IsVLANIfaceOffloadAllowed
(
  const char *iface_name,
  int16_t vlan_id
) const
{
  if(this->IsVLANIfacePreviouslyOffloaded(iface_name, vlan_id) ||
     !(this->GetOffloadCount() >= MAX_IPA_OFFLOAD))
  {
    LOG_MSG_INFO1("%s.%d allowed for offload", iface_name, vlan_id, 0);
    return true;
  }

  LOG_MSG_ERROR("%s.%d not allowed for offload, offload_count=%d",
                iface_name, vlan_id, this->GetOffloadCount());
  return false;
}



/*=====================================================
  FUNCTION UpdateDesiredOffloadInfo
======================================================*/
/*!
@brief
  Updates auto_reboot_flag based on user desired offload
  configuration

@parameters
  bool offload_flag //whether or not user wants the vlan iface to be offloaded

@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
void QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::UpdateDesiredOffloadInfo
(
 const char  *iface_name,
 int16_t     vlan_id,
 bool        offload_flag
)
{
  bool found_ht_entry = false;
  QCMAP_Virtual_LAN_phy_iface iface_type;

  LOG_MSG_INFO1("Updating IPA offload info for phy type: %s.%d, offload_flag = %d", iface_name,
                vlan_id, offload_flag);

  iface_type = GetIfaceTypeFromIface(iface_name);

  try
  {
    this->phy_iface_use_ht.at(iface_type);
  } catch(const std::out_of_range& oor) {
    LOG_MSG_INFO1("%s", oor.what(), 0, 0);
    return;
  }

  if(offload_flag)
  { //user wants offload
    for(std::vector<int16_t>::const_iterator it = this->phy_iface_use_ht.at(iface_type).cbegin();
        it != this->phy_iface_use_ht.at(iface_type).cend();
        ++it)
    {
      //KW doesn't like *it
      if((it != this->phy_iface_use_ht.at(iface_type).cend()) && (vlan_id == it[0]))
      {
        LOG_MSG_INFO1("%s.%d already added to hash table", iface_name, vlan_id, 0);
        found_ht_entry = true;
      }
    }

    if(!found_ht_entry)
    {
      this->phy_iface_use_ht.at(iface_type).push_back(vlan_id);

      //if size of vector is one, which is int16_t we just push_backed
      if(this->phy_iface_use_ht.at(iface_type).size() == 1)
      {
        //here is where we update the ipacfg file
        this->UpdateIPACfgFile();
        LOG_MSG_INFO1("User desires to enable VLAN offload on %s for the first time", iface_name,
                      0, 0);
        this->auto_reboot_flag = true;
      }
    }
  }
  else
  { //user does not want offload
    if(this->phy_iface_use_ht.at(iface_type).size())
    {
      //does vland id exist in vector?
      for(std::vector<int16_t>::iterator it = this->phy_iface_use_ht.at(iface_type).begin();
          it != this->phy_iface_use_ht.at(iface_type).end(); ++it)
      {
        //KW doesn't like *it
        if((it != this->phy_iface_use_ht.at(iface_type).end()) && (vlan_id == it[0]))
        {
          //KW doesn't like *it
          LOG_MSG_INFO1("%s.%d previously configured for offload VLAN: %d", iface_name, it[0],
                        it[0]);
          this->phy_iface_use_ht.at(iface_type).erase(it);
          //here is where we update the ipacfg file
          this->UpdateIPACfgFile();
          if (this->phy_iface_use_ht.at(iface_type).size() == 0)
            this->auto_reboot_flag = true;
          break;
        }
      }
    }
  }

  LOG_MSG_INFO1("auto_reboot_flag = %d", this->auto_reboot_flag, 0, 0);
  return;
}

/*=====================================================
  FUNCTION UpdateIPAWithVlanIOCTL
======================================================*/
/*!
@brief
  Sends IOCTL to IPA with VLAN info.

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
bool QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::UpdateIPAWithVlanIOCTL
(
  const char   *iface_name,
  int16_t      vlan_id,
  bool         is_up
)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  int ioctl_file_fd = -1, ioctl_ret = -1;
  struct ipa_ioc_vlan_iface_info vlan_ioctl_buffer;

  if (iface_name == NULL || vlan_id == 0)
  {
    LOG_MSG_ERROR("UpdateIPAWithIOCTL() Invalid parameters received",0,0,0);
    return false;
  }

  if (QcMapMgr && !IS_EMAC_IPA_SUPPORTED(QcMapMgr->target))
  {
    LOG_MSG_ERROR("EMAC IPA offload not supported for this target", 0, 0, 0);
    return false;
  }

  memset(&vlan_ioctl_buffer, 0, sizeof(ipa_ioc_vlan_iface_info));
  ioctl_file_fd = open(IPA_DEVICE_NAME, O_RDWR);
  if (ioctl_file_fd < 0)
  {
    LOG_MSG_ERROR("UpdateIPAWithIOCTL() Cannot open file for ioctl",0,0,0);
    return false;
  }

  snprintf(vlan_ioctl_buffer.name, sizeof(vlan_ioctl_buffer.name), "%s.%d", iface_name,vlan_id);
  vlan_ioctl_buffer.vlan_id = vlan_id;

  if (is_up)
  {
    LOG_MSG_INFO1("IOCTL to IPA (IPA_IOC_ADD_VLAN_IFACE) for iface=%s, vlan_id=%d",
                  vlan_ioctl_buffer.name, vlan_ioctl_buffer.vlan_id, 0);
    ioctl_ret = ioctl(ioctl_file_fd, IPA_IOC_ADD_VLAN_IFACE, &vlan_ioctl_buffer);
  }
  else
  {
    LOG_MSG_INFO1("IOCTL to IPA (IPA_IOC_DEL_VLAN_IFACE) for iface=%s, vlan_id=%d",
                  vlan_ioctl_buffer.name, vlan_ioctl_buffer.vlan_id, 0);
    ioctl_ret = ioctl(ioctl_file_fd, IPA_IOC_DEL_VLAN_IFACE, &vlan_ioctl_buffer);
  }

  if (ioctl_ret)
  {
    LOG_MSG_ERROR("IOCTL to IPA failed with err %d, errno %s",
                  ioctl_ret, strerror(errno), 0);
    close(ioctl_file_fd);
    return false;
  }

  close(ioctl_file_fd);

  if(is_up)
  {
    if(this->IsVLANIfacePreviouslyOffloaded(iface_name, vlan_id))
    {
      LOG_MSG_INFO1("IPA offload already configured: %s.%d", iface_name, vlan_id, 0);
      return true;
    }
    else if(this->offload_count >= MAX_IPA_OFFLOAD)
    {
      LOG_MSG_INFO1("IPA offload count already reached max: %u", MAX_IPA_OFFLOAD, 0, 0);
      return false;
    }

    if(sizeof(this->vlan_iface[this->offload_count].local_iface) > (strlen(iface_name) - 1))
    {
      memcpy(this->vlan_iface[this->offload_count].local_iface, iface_name, strlen(iface_name));
    } else {
      LOG_MSG_ERROR("Invalid iface name length", 0, 0, 0);
      return false;
    }
    this->vlan_iface[this->offload_count].vlan_id = vlan_id;

    this->offload_count++; //increment offload count

    LOG_MSG_INFO1("%s.%d configured for IPA offload", iface_name, vlan_id, 0);

  }

  this->PrintPhyIfaceUseHashTable();

  return true;
}

/*=====================================================
  FUNCTION UpdateNeutrinoWithVlanIoctl
======================================================*/
/*!
@brief
  Sends IOCTL to Neutrino with VLAN info.

@parameters
  int16_t vlan_id,
  bool is_up

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
    This IOCTL packs forllowing data and sends over. If
    there is a change in data structure(struct ifr_data_struct_ipa)
    used by Neutrino, this needs to be updated.

- Side Effects
- None
*/
/*====================================================*/
bool QCMAP_Virtual_LAN::QCMAP_VLAN_IPA_Offloader::UpdateNeutrinoWithVlanIoctl
(
  int16_t vlan_id,
  bool is_up
) const
{
  int ioctl_fd = -1,ioctl_ret = -1;
  unsigned char ioctl_buffer[NEUTRINO_IOCTL_BUFFER_SIZE_BYTES] = {0};
  struct   ifreq ifr;
  uint32 dma_c2 = NTN_TX_DMA_CH_2;
  uint32 dma_c0 = NTN_RX_DMA_CH_0;
  uint32 vlan_ipa_enable = NEUTRINO_IPA_VLAN_DISABLE;
  int    retry = 0;
/*------------------------------------------------------------------------*/

  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (QcMapMgr && !IS_NEUTRINO_IPA_SUPPORTED(QcMapMgr->target))
  {
    LOG_MSG_ERROR("Neutrino IPA offload not supported for this target", 0, 0, 0);
    return false;
  }

  if (vlan_id == 0)
  {
    LOG_MSG_ERROR("UpdateNeutrinoWithIoctl() Invalid parameters received",
                   0,0,0);
    return false;
  }

  ioctl_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (ioctl_fd < 0)
  {
    LOG_MSG_ERROR("UpdateNeutrinoWithIoctl() Cannot open file for ioctl to"
                  "neutrnio",0,0,0);
    return false;
  }

  memset(&ifr, 0, sizeof(struct ifreq));
  strlcpy((char *)ifr.ifr_name, ETH_IFACE, IFNAMSIZ);

  memcpy(&ioctl_buffer[NEUTRION_IOCTL_TX_IPA_DMA_INDEX],&dma_c2,sizeof(dma_c2));
  memcpy(&ioctl_buffer[NEUTRION_IOCTL_RX_IPA_DMA_INDEX],&dma_c0,sizeof(dma_c0));
  vlan_ipa_enable = is_up ? NEUTRINO_IPA_VLAN_ENABLE:NEUTRINO_IPA_VLAN_DISABLE;

  memcpy(&ioctl_buffer[NEUTRION_IOCTL_COMMAND_INDEX],&vlan_ipa_enable,
                                                       sizeof(vlan_ipa_enable));
  memcpy(&ioctl_buffer[NEUTRION_IOCTL_VLAN_ID_INDEX],&vlan_id,sizeof(vlan_id));

  ifr.ifr_ifru.ifru_data = (void *)&ioctl_buffer;

  /* Retry IOCTL if the return value is DWC_ETH_QOS_CONFIG_FAIL (-3) for
     predefine MAX retries*/
  do
  {
    ioctl_ret = ioctl(ioctl_fd,SIOCDEVPRIVATE+1, &ifr);
    retry++;

    if (ioctl_ret && errno == VLAN_NTN_CONFIG_FAIL_ERRNO)
    {
      LOG_MSG_ERROR("UpdateNeutrinoWithIoctl() failed with errno %d, sleep",
                       VLAN_NTN_IOCTL_SLEEP_INTERVAL,0,0);
      sleep(VLAN_NTN_IOCTL_SLEEP_INTERVAL);
    }
    else
    {
      LOG_MSG_ERROR("UpdateNeutrinoWithIoctl() IOCTL ret val %d, errno %d",
                     ioctl_ret,errno,0);
      break;
    }

  }while(retry <= VLAN_NTN_MAX_RETRIES);


  if (ioctl_ret)
  {
    LOG_MSG_ERROR("UpdateNeutrinoWithIoctl() IOCTL to Neutrino failed",0,0,0);
    close(ioctl_fd);
    return false;
  }

  close(ioctl_fd);
  return true;

}
#endif
