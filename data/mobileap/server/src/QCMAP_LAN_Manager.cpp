/*====================================================

FILE:  QCMAP_LAN_Manager.cpp

SERVICES:
   QCMAP LAN Manager Class Implementation

=====================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/

/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse
  chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------
  03/05/18   jt         initial LAN Manager creation
==========================================================*/

#include "QCMAP_LAN_Manager.h"

QCMAP_LAN_Manager* QCMAP_LAN_Manager::lan_mgr_object = NULL;

/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and returns singleton instance

@parameters

@return
  object - singleton object created

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================*/
static QCMAP_LAN_Manager* QCMAP_LAN_Manager::GetInstance(void)
{
  LOG_MSG_INFO1("%s entry", __func__, 0, 0);
  if(NULL == lan_mgr_object)
  {
    lan_mgr_object = new QCMAP_LAN_Manager();
    lan_mgr_object->CreateLANBridge(); //Create LAN Object for bridge0
  }
  return lan_mgr_object;
}

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes default QCMAP_LAN object

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
QCMAP_LAN_Manager::QCMAP_LAN_Manager(void)
{
  this->bridge_vlan_id_ht.clear();
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroying the LAN Manager Object.

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
static void QCMAP_LAN_Manager::DestroyInstance(void)
{
  delete lan_mgr_object;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroying all created LAN objects

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
QCMAP_LAN_Manager::~QCMAP_LAN_Manager(void)
{
  if(this->bridge_vlan_id_ht.size())
  {
    for(std::unordered_map<int16_t, QCMAP_LAN*>::iterator it = bridge_vlan_id_ht.begin();
        it != bridge_vlan_id_ht.end(); ++it)
    {
      delete it->second;
    }
  }
  lan_mgr_object = NULL;
}

/*==========================================================
  FUNCTION GetLANBridge
==========================================================*/
/*!
@brief
  Retrieves LAN object by vlan id

@parameters
  vlan id

@return
  QCMAP_LAN* ptr to LAN object

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
QCMAP_LAN* QCMAP_LAN_Manager::GetLANBridge(int16_t bridge_vlan_id)
{
  std::unordered_map<int16_t, QCMAP_LAN*>::const_iterator it;

  LOG_MSG_INFO1("Request to find Bridge/VLAN ID: %d", bridge_vlan_id, 0, 0);

  if((it = bridge_vlan_id_ht.find(bridge_vlan_id)) == bridge_vlan_id_ht.end())
  {
    LOG_MSG_INFO1("Could not find Bridge/VLAN ID: %d", bridge_vlan_id, 0, 0);
    return NULL;
  }

  return it->second;
}

/*==========================================================
  FUNCTION GetNumOfBridges
==========================================================*/
/*!
@brief
  Gets number of LAN objects created

@parameters

@return
  int - number of LAN objects created

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
int QCMAP_LAN_Manager::GetNumOfBridges(void)
{
  LOG_MSG_INFO1("There are %d bridges", bridge_vlan_id_ht.size(), 0, 0);
  return this->bridge_vlan_id_ht.size();
}

/*==========================================================
  FUNCTION GetBridgeIDs
==========================================================*/
/*!
@brief
  Gets vector of configured VLANs

@parameters

@return
  vector of configured VLANs

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
std::vector<int16_t> QCMAP_LAN_Manager::GetBridgeIDs(void)
{
  std::vector<int16_t> bridge_vlan_vec;

  LOG_MSG_INFO1("There are %d bridges", bridge_vlan_id_ht.size(), 0, 0);

  for(std::unordered_map<int16_t, QCMAP_LAN*>::iterator it = this->bridge_vlan_id_ht.begin();
      it != this->bridge_vlan_id_ht.end(); ++it)
  {
    bridge_vlan_vec.push_back(it->first);
  }

  return bridge_vlan_vec;
}

/*==========================================================
  FUNCTION AssociateIface
==========================================================*/
/*!
@brief
  Associates physical iface to a VLAN ID

@parameters
  iface_type
  vlan_id

@return
  ptr to LAN object associated with vlan_id

@note
  Implicitly creates LAN objects if vlan_id non-existent

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
QCMAP_LAN* QCMAP_LAN_Manager::AssociateIface
(
  qcmap_msgr_interface_type_enum_v01 iface_type,
  qcmap_msgr_vlan_config_v01 vlan_config,
  bool createConfig
)
{
  QCMAP_LAN* lan_obj = NULL;

  LOG_MSG_INFO1("Associating iface type: %d, to Bridge/VLAN ID: %d", iface_type, vlan_config.vlan_id, 0);

  if(!IS_VLAN_ID_VALID(vlan_config.vlan_id))
  {
    LOG_MSG_ERROR("Bridge VLAN ID out of bounds! %d", vlan_config.vlan_id, 0, 0);
    return NULL;
  }

  if((lan_obj = GetLANBridge(vlan_config.vlan_id)) == NULL)
  {
    lan_obj = this->CreateLANBridge(vlan_config);
  }

  if(lan_obj && lan_obj->AssociateVLANIface(iface_type))
  {
    if (createConfig == true)
    {
      //update the mobileap_cfg.xml
      if(!lan_obj->SetLANConfigToXML())
      {
        LOG_MSG_ERROR("Failed to update xml for associating iface type: %d, to Bridge/VLAN ID: %d",
                      iface_type, vlan_config.vlan_id, 0);
      }
    }
  }

  return lan_obj;
}


/*==========================================================
  FUNCTION CreateLANBridge
==========================================================*/
/*!
@brief
  Creates a new LAN object for bridge0

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
void QCMAP_LAN_Manager::CreateLANBridge()
{
  QCMAP_LAN *lan_obj = NULL;

  LOG_MSG_INFO1("Creating LAN Object for bridge0",0,0,0);
  lan_obj = new QCMAP_LAN(0);
  ds_assert(lan_obj != NULL); //assert if lan_obj creation failed.

  this->bridge_vlan_id_ht.insert(std::pair<int16_t, QCMAP_LAN*>(0, lan_obj));
  lan_obj->SetIPAoffload(1); // IPA will always be enabled for bridge0
}


/*==========================================================
  FUNCTION CreateLANBridge
==========================================================*/
/*!
@brief
  Creates a new LAN object with VLAN ID

@parameters
  vlan_id

@return
  ptr to LAN object associated with vlan_id

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
QCMAP_LAN* QCMAP_LAN_Manager::CreateLANBridge(qcmap_msgr_vlan_config_v01 vlan_config)
{
  QCMAP_LAN *lan_obj = NULL;
  std::unordered_map<int16_t, QCMAP_LAN*>::iterator it;

  if(!IS_VLAN_ID_VALID(vlan_config.vlan_id))
  {
    LOG_MSG_ERROR("Bridge VLAN ID out of bounds! %d", vlan_config.vlan_id, 0, 0);
    return NULL;
  }

  LOG_MSG_INFO1("%s: %d", __func__, vlan_config.vlan_id, 0);

  lan_obj = new QCMAP_LAN(vlan_config.vlan_id);
  ds_assert (lan_obj != NULL); //Assert if lan_obj creation failed.
  this->bridge_vlan_id_ht.insert(std::pair<int16_t, QCMAP_LAN*>(vlan_config.vlan_id, lan_obj));

  lan_obj->BlockIPv4WWANAccess();
  lan_obj->BlockIPv6WWANAccess();
  lan_obj->SetIPAoffload(vlan_config.ipa_offload);

  return lan_obj;
}

/*==========================================================
  FUNCTION DeleteLANBridge
==========================================================*/
/*!
@brief
  Deletes a LAN object with VLAN ID

@parameters
  vlan_id

@return
  true - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
bool QCMAP_LAN_Manager::DeleteLANBridge(int16_t bridge_vlan_id)
{
  QCMAP_LAN* lan_obj = NULL;

  LOG_MSG_INFO1("%s for %d", __func__, bridge_vlan_id, 0);

  if((lan_obj = this->GetLANBridge(bridge_vlan_id)) == NULL)
  {
    LOG_MSG_ERROR("Failed to get lan bridge: %d", bridge_vlan_id, 0, 0);
    return false;
  }

  lan_obj->DeleteLANConfigFromXML(true);
  delete lan_obj;

  this->bridge_vlan_id_ht.erase(bridge_vlan_id);

  return true;
}

/*==========================================================
  FUNCTION DisassociateIface
==========================================================*/
/*!
@brief
  Disassociates physical iface from a VLAN ID

@parameters
  iface_type
  vlan_id

@return
  true - on Success
  false - on Failure

@note
  Implicitly deletes LAN objects if vlan_id is no longer in use

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
bool QCMAP_LAN_Manager::DisassociateIface
(
qcmap_msgr_interface_type_enum_v01 iface_type,
int16_t bridge_vlan_id
)
{
  QCMAP_LAN* lan_obj = NULL;
  QCMAP_Backhaul_WWAN* wwan_obj = NULL;
  qmi_error_type_v01 qmi_err_num;

  LOG_MSG_INFO1("Disassociating iface type: %d, from Bridge/VLAN ID: %d", iface_type,
                bridge_vlan_id, 0);

  if(!IS_VLAN_ID_VALID(bridge_vlan_id))
  {
    LOG_MSG_INFO1("Bridge VLAN ID out of bounds! %d", bridge_vlan_id, 0, 0);
    return;
  }

  if((lan_obj = GetLANBridge(bridge_vlan_id)) == NULL)
  {
    LOG_MSG_INFO1("Bridge %d does not exist", bridge_vlan_id, 0, 0);
    return;
  }


  if(lan_obj && lan_obj->DisassociateVLANIface(iface_type))
  {
    //check if any other VLAN ifaces up
    if(!lan_obj->GetVLANIfaceMask())
    {
      if((wwan_obj = GET_BACKHAUL_WWAN_OBJECT(lan_obj->GetBackhaulProfileHandle())) != NULL)
      {
        if(!(wwan_obj->DeleteVlanMapping(bridge_vlan_id, &qmi_err_num)))
        {
          LOG_MSG_ERROR("failed to notify WWAN object %d of VLAN deletion %d",
                        lan_obj->GetBackhaulProfileHandle(), bridge_vlan_id, 0);
        }
      }

      if(!this->DeleteLANBridge(bridge_vlan_id))
      {
        LOG_MSG_ERROR("Error deleting lan bridge: %d", bridge_vlan_id, 0, 0);
      } else {
        LOG_MSG_INFO1("Cleaned up lan bridge%d", bridge_vlan_id, 0, 0);
      }
    } else {
      LOG_MSG_INFO1("bridge%d vlan iface use mask: 0x%x", bridge_vlan_id,
                    lan_obj->GetVLANIfaceMask(), 0);
    }

    return true;
  }

  return false;
}

/*==========================================================
  FUNCTION IsVLANToPDNMappingAllowed
==========================================================*/
/*!
@brief
  Feature check to see if VLAN <-> PDN mapping will be supported

@parameters

@return
  true - VLAN <-> PDN mapping will be supported
  false - VLAN <-> PDN mapping can not be supported

@note
  If (L2TP || SOCKSv5) are enabled, then we can not allow
  VLAN <-> PDN mapping to be simultaneously supported

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
bool QCMAP_LAN_Manager::IsVLANToPDNMappingAllowed(void)
{
  qmi_error_type_v01 qmi_err_num;
  qcmap_msgr_set_unmanaged_l2tp_state_config_v01 l2tp_config_info;

  memset(&l2tp_config_info, 0, sizeof(l2tp_config_info));
  if(!QCMAP_L2TP::GetSetL2TPEnableConfigFromXML(GET_VALUE, &l2tp_config_info))
  {
    LOG_MSG_ERROR("Error getting L2TP config info", 0, 0, 0);
    return false;
  }

  //if L2TP is enabled
  if(l2tp_config_info.enable)
  {
    LOG_MSG_ERROR("L2TP enabled, VLAN to PDN mapping not allowed", 0, 0, 0);
    return false;
  } else if(QCMAP_NATALG::GetSOCKSv5ProxyEnableCfg(&qmi_err_num)) { //if SOCKSv5 is enabled
    LOG_MSG_ERROR("SOCKSv5 enabled, VLAN to PDN mapping not allowed", 0, 0, 0);
    return false;
  }

  return true;
}
