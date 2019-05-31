/*====================================================

FILE:  QCMAP_NATALG.cpp

SERVICES:
   QCMAP Connection Manager NAT ALG Specific Implementation

=====================================================

  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/21/14   vm         9x45
  06/12/15   rk         Offtarget support.
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
#include "QCMAP_NATALG.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_Tethering.h"


/*=============================================================*/
/* Local definitions                                           */
/*=============================================================*/
qcmap_cm_nat_conf_common_t QCMAP_NATALG::nat_config_common = {0};

/*==========================================================
                             Class Definitions
  =========================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes NAT variables and reads XML file for NAT related configuration.

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

QCMAP_NATALG::QCMAP_NATALG(QCMAP_Backhaul* QcMapBackhaul)
{
  this->QcMapBackhaul = QcMapBackhaul;

  memset(&(nat_config), 0, sizeof(nat_config));

  /*Symmetric NAT as default */
  nat_config.nat_type = QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01;
  nat_config.enable_ipsec_vpn_pass_through = true;
  nat_config.enable_l2tp_vpn_pass_through = true;
  nat_config.enable_pptp_vpn_pass_through = true;
  nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
  nat_config.dmz_ip = 0;

  /* Initialize alg enable disable mask to be zero */
  alg_enabled_disabled_mask = 0;
}


/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Frees memory allocated for NAT Entries.

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

QCMAP_NATALG::~QCMAP_NATALG()
{
  qcmap_nat_entry_list_t* natList = &(this->nat_config.port_fwding_entries);

  LOG_MSG_INFO1("Destroying NATALG object",0,0,0);
  /* Delete the NAT entry list */
  ds_dll_delete_all(natList->natEntryListHead);
  natList->natEntryListHead = NULL;
  natList->natEntryListTail = NULL;
}


/*==========================================================
  FUNCTION Init
==========================================================*/
/*!
@brief
  Initialize Params for Nat Object

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
void QCMAP_NATALG::Init()
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qmi_error_type_v01 qmi_err_num;

  if(ReadNATConfigFromXML(QcMapMgr->xml_path) == FALSE)
  {
    LOG_MSG_ERROR("Error in loading NAT Configuration\n", 0, 0, 0);
  }

  /* Set the timeout values (static configuration), required only once on boot */
  if (QcMapBackhaul == GET_DEFAULT_BACKHAUL_OBJECT())
  {
     /* Initialize natconfig common params */
    QCMAP_NATALG::nat_config_common.nat_entry_generic_timeout = QCMAP_NAT_ENTRY_DEFAULT_GENERIC_TIMEOUT;
    QCMAP_NATALG::nat_config_common.nat_entry_icmp_timeout = QCMAP_NAT_ENTRY_DEFAULT_ICMP_TIMEOUT;
    QCMAP_NATALG::nat_config_common.nat_entry_tcp_established_timeout = QCMAP_NAT_ENTRY_DEFAULT_TCP_TIMEOUT;
    QCMAP_NATALG::nat_config_common.nat_entry_udp_timeout = QCMAP_NAT_ENTRY_DEFAULT_UDP_TIMEOUT;
    QCMAP_NATALG::nat_config_common.initial_pkt_limit = 0;

    QCMAP_NATALG::SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01,
                                      QCMAP_NATALG::nat_config_common.nat_entry_generic_timeout, &qmi_err_num);
    QCMAP_NATALG::SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01,
                                      QCMAP_NATALG::nat_config_common.nat_entry_icmp_timeout, &qmi_err_num);
    QCMAP_NATALG::SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01,
                                      QCMAP_NATALG::nat_config_common.nat_entry_tcp_established_timeout, &qmi_err_num);
    QCMAP_NATALG::SetNatTimeoutOnApps(QCMAP_MSGR_NAT_TIMEOUT_UDP_V01,
                                      QCMAP_NATALG::nat_config_common.nat_entry_udp_timeout, &qmi_err_num);
  }

  return;
}


/*==========================================================
  FUNCTION GetProfileHandle
==========================================================*/
/*!
@brief
  Gets profile handle for NATALG Object

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
profile_handle_type_v01 QCMAP_NATALG::GetProfileHandle()
{
  return ((QcMapBackhaul != NULL) ? QcMapBackhaul->profileHandle : 0);
}

/*==========================================================
  FUNCTION GetNatTimeout
==========================================================*/
/*!
@brief
  Gets the NAT timeout value configured on A5. If NAT object is not present yet,
  obtain the values from XML

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          timeout_value

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
static boolean QCMAP_NATALG::GetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 *timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  if (!QCMAP_NATALG::GetNatTimeoutOnApps(timeout_type, timeout_value, qmi_err_num))
  {
    return false;
  }
  return true;
}


/*==========================================================
  FUNCTION SetNatTimeout
==========================================================*/
/*!
@brief
  Sets the NAT timeout on A5.

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          timeout_value

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
static boolean QCMAP_NATALG::SetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_NATALG* QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT();
  char data[MAX_STRING_LENGTH] = {0};

  if (timeout_value < QCMAP_NAT_ENTRY_MIN_TIMEOUT)
  {
    LOG_MSG_ERROR("Timeout value should be greater than: %d Got: %d.\n",
                  QCMAP_NAT_ENTRY_MIN_TIMEOUT, timeout_value, 0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }

  if(QcMapNatAlg == NULL)
  {
     LOG_MSG_ERROR("QcMapNatAlg is NULL", 0, 0, 0);
     *qmi_err_num = QMI_ERR_INTERNAL_V01;
     return false;
  }

  if (!QCMAP_NATALG::SetNatTimeoutOnApps(timeout_type, timeout_value, qmi_err_num))
  {
    return false;
  }

  snprintf(data, sizeof(data), "%d", timeout_value);

  switch ( timeout_type )
  {
    case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:

      QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_GENERIC_TIMEOUT, SET_VALUE, data, strlen(data));
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
      QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_TCP_ESTABLISHED_TIMEOUT, SET_VALUE, data, strlen(data));
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
      QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_UDP_TIMEOUT, SET_VALUE, data, strlen(data));
    break;

    case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
      QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_NAT_ENTRY_ICMP_TIMEOUT, SET_VALUE, data, strlen(data));
    break;

    default:
      LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
    break;
  }

  return true;
}

/*==========================================================
  FUNCTION GetNATEntryCountFromXML
==========================================================*/
/*!
@brief
  Returns the number of NAT entries present in XML.

@parameters
  pugi::xml_document *xml_file

@return
  int: Count of NAT entries.

*/
/*=========================================================*/

int QCMAP_NATALG::GetNATEntryCountFromXML(pugi::xml_document *xml_file)
{
  pugi::xml_node root;

  if (!(root = QcMapBackhaul->GetPdnRootXML(xml_file)))
  {
    LOG_MSG_ERROR("Unable to find NAT cfg corresponding to PDN",0,0,0);
    return false;
  }
  root = root.child(MobileAPNatCfg_TAG);

  return root.select_nodes(PortFwding_TAG).size();
}


/*==========================================================
  FUNCTION AddStaticNatEntry
==========================================================*/
/*!
@brief
  Checks nat entry for the redundancy and if unique adds a new snat entry.

@parameters
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

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
boolean QCMAP_NATALG::AddStaticNatEntry
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
   qmi_error_type_v01 *qmi_err_num
)
{
  int i;
  ds_dll_el_t * node = NULL;
  qcmap_nat_entry_list_t* natList = NULL;
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL;
  pugi::xml_document xml_file;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  *qmi_err_num = QMI_ERR_INTERNAL_V01;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  if (this->nat_config.num_port_fwding_entries ==
      QCMAP_MSGR_MAX_SNAT_ENTRIES_V01 )
  {
    /* we reach MAX entries */
    LOG_MSG_ERROR("\nReached Max limit on SNAT Entries!!",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }
  if((QCMAP_NATALG::GetNATEntryCountFromXML(&xml_file)) >=
                                           QCMAP_MSGR_MAX_SNAT_ENTRIES_V01)
  {
    LOG_MSG_ERROR("Exceeds maximum number of NAT entries\n",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }

  if (QCMAP_NATALG::searchNATEntryInXML(&xml_file, nat_entry))
  {
    /* we already added this one */
    LOG_MSG_ERROR("SNAT Entry already present!!",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
    natList = &(this->nat_config.port_fwding_entries);
    LOG_MSG_INFO1("Searching for NAT Entries in the list. ", 0, 0, 0);

    if (natList->natEntryListHead)
    {

      node = ds_dll_next(natList->natEntryListHead,
                         (&curr_nat_entry));
      if (node == NULL)
      {
        LOG_MSG_ERROR("AddStaticNatEntry - NAT info is NULL"
                      "Cant fetch NAT info",0,0,0);
        return false;
      }
      for ( i=0; i < this->nat_config.num_port_fwding_entries; i++ )
      {
        if ((nat_entry->port_fwding_global_port == curr_nat_entry->port_fwding_global_port) &&
            (nat_entry->port_fwding_protocol == curr_nat_entry->port_fwding_protocol ))
        {
          /* we already added this one */
          LOG_MSG_ERROR("\nSNAT Entry already present!!",0,0,0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }
        node = ds_dll_next(node, (&curr_nat_entry));
      }
    }

  if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    if (nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP)
    {
      if (!this->AddSNATEntryOnApps(nat_entry, qmi_err_num))
      {
        LOG_MSG_ERROR("\nAdding SNAT entry on A5 Failed!!",0,0,0);
        return false;
      }
    }
    else
    {
      nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
      if (!this->AddSNATEntryOnApps(nat_entry, qmi_err_num))
      {
        LOG_MSG_ERROR("\nAdding SNAT entry on A5 Failed!!",0,0,0);
        return false;
      }
      nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;

      if(!this->AddSNATEntryOnApps(nat_entry, qmi_err_num))
      {
        LOG_MSG_ERROR("\nAdding SNAT entry on A5 Failed!!",0,0,0);
        return false;
      }
    }
  }

  LOG_MSG_INFO1("Adding configuration to List", 0, 0, 0);
  if(this->AddNATEntryToList(nat_entry) == false)
  {
    LOG_MSG_ERROR("Failed to Add NAT Entry to List", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  /* If NAT is not enabled just add the entry to XML file. */
  LOG_MSG_INFO1("Adding configuration to XML file", 0, 0, 0);
  addNATEntryToXML(&xml_file, nat_entry);

  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*==========================================================
  FUNCTION DeleteStaticNatEntry
==========================================================*/
/*!
@brief
  Deletes the snat entry on A5 and in mobileap configuration file.

@parameters
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

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

boolean QCMAP_NATALG::DeleteStaticNatEntry
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  ds_dll_el_t * node = NULL;
  qcmap_nat_entry_list_t* natList = NULL;
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  pugi::xml_document xml_file;
  pugi::xml_node parent, child ;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (QcMapBackhaul)
  {
    natList = &(nat_config.port_fwding_entries);

    LOG_MSG_INFO1( "DeleteStaticNatEntry - head: %x tail: %x nat: %d",
                   natList->natEntryListHead,
                   natList->natEntryListTail,
                   nat_config.num_port_fwding_entries);

    if ((node = ds_dll_delete(natList->natEntryListHead, &(natList->natEntryListTail),
                              (void*)nat_entry, qcmap_compare_nat_entries))== NULL)
    {
      LOG_MSG_ERROR("DeleteStaticNatEntry - Error in finding the node",0,0,0);
      return false;
    }

    //Free the memory for the device details structure
    curr_nat_entry = (qcmap_cm_port_fwding_entry_conf_t*)node->data;

    if (curr_nat_entry == NULL)
    {
      LOG_MSG_ERROR("DeleteStaticNatEntry - NAT Info is NULL", 0,0,0);
      //Free the memory of the linked list node
      ds_dll_free(node);
      node = NULL;
      return false;
    }

    //Free the device information structure
    ds_free (curr_nat_entry);
    curr_nat_entry = NULL;

    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;

    // Add logic to check if there are no more nodes in the list and free the head
    if (natList->natEntryListHead->next == NULL)
    {
      LOG_MSG_INFO1("\n No NAT entry exist\n",0,0,0);
      ds_dll_free(natList->natEntryListHead);
      natList->natEntryListHead = NULL;
    }

    nat_config.num_port_fwding_entries--;

    if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
    {
      if ( nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP )
      {
        if (!DeleteSNATEntryOnApps(nat_entry, qmi_err_num))
          return false;
      }
      else
      {
        nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
        if (!DeleteSNATEntryOnApps(nat_entry, qmi_err_num))
          return false;

        nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;

        if (!DeleteSNATEntryOnApps(nat_entry, qmi_err_num))
          return false;
      }
    }
  }

  /* If NAT is not enabled, just delete the entry from XML file and return. */
  if (child = searchNATEntryInXML(&xml_file, nat_entry))
  {
    parent = child.parent();
    parent.remove_child(child);
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    LOG_MSG_INFO1("Deleted the NAT entry successfully.", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("DeleteStaticNatEntry - Error in finding the node",0,0,0);
    return false;
  }

  return true;
}

/*==========================================================
  FUNCTION addNATEntryToXML
==========================================================*/
/*!
@brief
  Add NAT Entry to the XML.

@parameters
  pugi::xml_document *xml_file,
  qcmap_cm_port_fwding_entry_conf_t* nat_entry
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

int QCMAP_NATALG::addNATEntryToXML(pugi::xml_document *xml_file,
                                   qcmap_cm_port_fwding_entry_conf_t* nat_entry)
{

  struct in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;
  pugi::xml_node root, child, subchild;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!(root = QcMapBackhaul->GetPdnRootXML(xml_file)))
  {
    LOG_MSG_ERROR("Unable to find NAT cfg corresponding to PDN",0,0,0);
    return false;
  }
  root = root.child(MobileAPNatCfg_TAG);

  // add new NAT entry here.
  child = root.append_child(PortFwding_TAG);

  //IP Address
  subchild = child.append_child(PortFwdingPrivateIP_TAG);
  addr.s_addr = htonl(nat_entry->port_fwding_private_ip);
  subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

  //Port forwarding IP
  subchild = child.append_child(PortFwdingPrivatePort_TAG);
  snprintf(data, sizeof(data), "%d", nat_entry->port_fwding_private_port);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  // Global Port
  subchild = child.append_child(PortFwdingGlobalPort_TAG);
  snprintf(data, sizeof(data), "%d", nat_entry->port_fwding_global_port);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  // Protocol
  subchild = child.append_child(PortFwdingProtocol_TAG);
  snprintf(data, sizeof(data), "%d", nat_entry->port_fwding_protocol);
  subchild.append_child(pugi::node_pcdata).set_value(data);


  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, xml_file);

  debug_string=htonl(nat_entry->port_fwding_private_ip);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("PortFwdingPrivateIP: '%s'",str,0,0);

  LOG_MSG_INFO1("PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d, PortFwdingProtocol: %d",
  nat_entry->port_fwding_private_port, nat_entry->port_fwding_global_port,nat_entry->port_fwding_protocol);

  return true;
}

/*==========================================================
  FUNCTION GetStaticNATEntriesFromXML
==========================================================*/
/*!
@brief
  Get Static NAT Entry from the XML.

@parameters
  qcmap_msgr_snat_entry_config_v01 *snat_config,
  unsigned int* num_entries

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

boolean QCMAP_NATALG::GetStaticNATEntriesFromXML
(
  qcmap_msgr_snat_entry_config_v01 *snat_config,
  unsigned int* num_entries
)
{
  int count = 0;
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;

  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  in_addr addr;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find NAT cfg corresponding to PDN",0,0,0);
    return false;
  }
  root = root.child(MobileAPNatCfg_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if ((!strcmp(child.name(),PortFwding_TAG)))
    {
      inet_aton(child.child_value(PortFwdingPrivateIP_TAG), &addr);
      snat_config[count].private_ip_addr = ntohl(addr.s_addr);

      snat_config[count].private_port = atoi(
                                    child.child_value(PortFwdingPrivatePort_TAG));

      snat_config[count].global_port = atoi(
                                     child.child_value(PortFwdingGlobalPort_TAG));

      snat_config[count].protocol = atoi(child.child_value(PortFwdingProtocol_TAG));

      debug_string= htonl(snat_config[count].private_ip_addr );
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
      LOG_MSG_INFO1("\PortFwdingPrivateIP: '%s'",str,0,0);
      LOG_MSG_INFO1("PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d, PortFwdingProtocol: %d",snat_config[count].private_port,
                    snat_config[count].global_port,snat_config[count].protocol );
      count++;


    }
  }

  *num_entries = count;
  return true;
}

/*==========================================================
  FUNCTION GetStaticNatEntries
==========================================================*/
/*!
@brief
  Gets all the snat entries configured.

@parameters
  qcmap_msgr_snat_entry_config_v01 *snat_config
  unsigned int                     *num_entries

@return
  true  - on Success - number of snat entries > 0
  false - on Failure - no snat entries

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::GetStaticNatEntries
(
  qcmap_msgr_snat_entry_config_v01 *snat_config,
  unsigned int* num_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i = 0;
  qcmap_nat_entry_list_t* natList = NULL;
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL;
  ds_dll_el_t * node = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  pugi::xml_document xml_file;

  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.\n",0,0,0);
    return false;
  }
  *qmi_err_num = QMI_ERR_NONE_V01;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if(!QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    LOG_MSG_INFO1("Obtaining NAT Entries from XML. ", 0, 0, 0);
    GetStaticNATEntriesFromXML(snat_config, num_entries);
    if (*num_entries <= 0)
    {
      LOG_MSG_ERROR("\nNo SNAT entries configured",0,0,0);
      *qmi_err_num = QMI_ERR_DISABLED_V01;
      return false;
    }
    return true;
  }

  LOG_MSG_INFO1("Obtaining NAT Entries from List. ", 0, 0, 0);
  natList = &(nat_config.port_fwding_entries);

  *num_entries = nat_config.num_port_fwding_entries;

  if (*num_entries <= 0)
  {
    LOG_MSG_ERROR("\nNo SNAT entries configured",0,0,0);
    *qmi_err_num = QMI_ERR_DISABLED_V01;
    return false;
  }

  if (natList->natEntryListHead)
  {
    node = natList->natEntryListHead;
    if (node == NULL)
    {
      LOG_MSG_ERROR("GetStaticNatEntries - NAT info is NULL"
                    "Cant fetch NAT info",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    node = ds_dll_next (node, (const void**)(&curr_nat_entry));

    for ( i=0; i < *num_entries && curr_nat_entry; i++ )
    {
      snat_config[i].private_ip_addr = curr_nat_entry->port_fwding_private_ip;
      snat_config[i].private_port = curr_nat_entry->port_fwding_private_port;
      snat_config[i].global_port = curr_nat_entry->port_fwding_global_port;
      snat_config[i].protocol = curr_nat_entry->port_fwding_protocol;
      node = ds_dll_next (node, (const void**)(&curr_nat_entry));
    }
  }
  return true;
}

/*==========================================================
  FUNCTION GetIpsecVpnPassthroughFlag
==========================================================*/
/*!
@brief
  Gets ipsec vpn pass through flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::GetSetPassthroughFlag
(
  qcmap_nat_config_enum pass_thru_type,
  qcmap_action_type action,
  uint8 *flag,
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (flag == NULL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (pass_thru_type < CONFIG_PPTP_VPN_PASSTHROUGH ||
      pass_thru_type > CONFIG_IPSEC_VPN_PASSTHROUGH)
  {
    LOG_MSG_ERROR(" Invalid pass through type. ", 0, 0, 0);
    return false;
  }

  if (action == SET_VALUE)
    snprintf(data, sizeof(data), "%d", *flag);


  switch (pass_thru_type)
  {
    case CONFIG_PPTP_VPN_PASSTHROUGH:
      if (action == SET_VALUE)
      {
        if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
        {
          if (!SetPPTPVpnPassThroughOnApps(*flag, qmi_err_num))
          {
            LOG_MSG_ERROR("Fail to set PPTP vpn passthrough", 0, 0, 0);
            return false;
          }
        }
        nat_config.enable_pptp_vpn_pass_through = *flag;
        GetSetNATConfigFromXML(CONFIG_PPTP_VPN_PASSTHROUGH, SET_VALUE, data, MAX_STRING_LENGTH);
      }
      else
      {
        if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
        {
          *flag = nat_config.enable_pptp_vpn_pass_through;
        }
        else
        {
          GetSetNATConfigFromXML(CONFIG_PPTP_VPN_PASSTHROUGH, GET_VALUE, data, MAX_STRING_LENGTH);
          *flag = atoi(data);
        }
      }
    break;
    case CONFIG_L2TP_VPN_PASSTHROUGH:
      if (action == SET_VALUE)
      {
        if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
        {
          if (!SetL2TPVpnPassThroughOnApps(*flag, qmi_err_num))
          {
            LOG_MSG_ERROR("Fail to set L2TP vpn passthrough", 0, 0, 0);
            return false;
          }
        }
        nat_config.enable_l2tp_vpn_pass_through = *flag;
        GetSetNATConfigFromXML(CONFIG_L2TP_VPN_PASSTHROUGH, SET_VALUE, data, MAX_STRING_LENGTH);
      }
      else
      {
        if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
        {
          *flag = nat_config.enable_l2tp_vpn_pass_through;
        }
        else
        {
          GetSetNATConfigFromXML(CONFIG_L2TP_VPN_PASSTHROUGH, GET_VALUE, data, MAX_STRING_LENGTH);
          *flag = atoi(data);
        }
      }
      break;

    case CONFIG_IPSEC_VPN_PASSTHROUGH:
      if (action == SET_VALUE)
      {
        if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
        {
          if (!SetIPSECVpnPassThroughOnApps(*flag, qmi_err_num))
          {
            LOG_MSG_ERROR("Fail to set IPSEC vpn passthrough", 0, 0, 0);
            return false;
          }
        }
        nat_config.enable_ipsec_vpn_pass_through = *flag;
        GetSetNATConfigFromXML(CONFIG_IPSEC_VPN_PASSTHROUGH,
                               SET_VALUE, data, MAX_STRING_LENGTH);
      }
      else
      {
        if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
        {
          *flag = nat_config.enable_ipsec_vpn_pass_through;
        }
        else
        {
          GetSetNATConfigFromXML(CONFIG_IPSEC_VPN_PASSTHROUGH, GET_VALUE, data, MAX_STRING_LENGTH);
          *flag = atoi(data);
        }
      }
    break;
  }
  return true;
}


/*==========================================================
  FUNCTION SetIPSECVpnPassThroughOnApps
==========================================================*/
/*!
@brief
  Sets ipsec vpn pass through.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetIPSECVpnPassThroughOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  if (!QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

   /* Delete the existing rule, only if it was added .*/
  if (!nat_config.enable_ipsec_vpn_pass_through)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -p esp -i %s -j %s", devname, "DROP" );
    ds_system_call(command, strlen(command));
  }

  /* Add entry only in case of disable rule, by default it's accept*/
  if( !enable )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -p esp -i %s -j %s", devname, "DROP" );
    ds_system_call(command, strlen(command));
  }

  return true;
}



/*==========================================================
  FUNCTION SetPPTPVpnPassThroughOnApps
==========================================================*/
/*!
@brief
  Configure the PPTP VPN pass through flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetPPTPVpnPassThroughOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  if (!QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

   /* Delete the existing rule, only if it was added .*/
  if (!nat_config.enable_pptp_vpn_pass_through)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -p gre -i %s -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
  }

  /* Add entry only in case of disabel rule, by default it's accept*/
  if( !enable )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -p gre -i %s -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
    LOG_MSG_INFO1("%s\n", command,0,0);
  }

  return true;
}

/*==========================================================
  FUNCTION SetL2TPVpnPassThroughOnApps
==========================================================*/
/*!
@brief
  Configures the L2TP VPN pass through flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetL2TPVpnPassThroughOnApps
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(command, 0, MAX_COMMAND_STR_LEN);

  if (!QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

   /* Delete the existing rule, only if it was added .*/
  if( !nat_config.enable_l2tp_vpn_pass_through )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -p esp -i %s -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
  }

  /* Add entry only in case of disable rule, by default it's accept*/
  if( !enable )
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -p esp -i %s -j %s", devname,"DROP" );
    ds_system_call(command, strlen(command));
  }

  return true;
}

/*==========================================================
  FUNCTION GetNatType
==========================================================*/
/*!
@brief
  Gets configured value of L2TP VPN passthrough flag.

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
/*=========================================================*/
boolean QCMAP_NATALG::GetNatType
(
  qcmap_msgr_nat_enum_v01 *cur_nat_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if ( cur_nat_type == NULL )
  {
    *qmi_err_num = QMI_ERR_DISABLED_V01;
    return false;
  }

  GetSetNATConfigFromXML(CONFIG_NAT_TYPE, GET_VALUE, data, MAX_STRING_LENGTH);
  if (strncasecmp(PRC_TAG, data, strlen(PRC_TAG)) == 0)
  {
    *cur_nat_type = QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01;
  }
  else if (strncasecmp(FULLCONE_TAG, data, strlen(FULLCONE_TAG)) == 0)
  {
    *cur_nat_type = QCMAP_MSGR_NAT_FULL_CONE_NAT_V01;
  }
  else if (strncasecmp(ARC_TAG, data, strlen(ARC_TAG)) == 0)
  {
    *cur_nat_type = QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01;
  }
  else
  {
    /* Default SYMMETRIC NAT. */
    *cur_nat_type = QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01;
  }

  return true;
}

/*==========================================================
  FUNCTION ReadNATConfigFromXML
==========================================================*/
/*!
@brief
  Reads the NAT Config from the XML

@parameters
  char *xml_path

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

boolean QCMAP_NATALG::ReadNATConfigFromXML(char *xml_path)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child, subchild;
  char str[INET6_ADDRSTRLEN];
  char data[MAX_STRING_LENGTH] = {0};
  uint32_t debug_string =0;
  qcmap_cm_port_fwding_entry_conf_t nat_entry;

  if (!xml_file.load_file(xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find NAT cfg corresponding to PDN",0,0,0);
    return false;
  }
  root = root.child(MobileAPNatCfg_TAG);

  if (root.child(NatEntryGenericTimeout_TAG))
  {
    QCMAP_NATALG::nat_config_common.nat_entry_generic_timeout =
                        atoi(root.child(NatEntryGenericTimeout_TAG).child_value());
  }

  if (root.child(NatEntryICMPTimeout_TAG))
  {
    QCMAP_NATALG::nat_config_common.nat_entry_icmp_timeout =
                           atoi(root.child(NatEntryICMPTimeout_TAG).child_value());
  }

  if (root.child(NatEntryTCPEstablishedTimeout_TAG))
  {
    QCMAP_NATALG::nat_config_common.nat_entry_tcp_established_timeout =
                 atoi(root.child(NatEntryTCPEstablishedTimeout_TAG).child_value());
  }

  if (root.child(NatEntryUDPTimeout_TAG))
  {
    QCMAP_NATALG::nat_config_common.nat_entry_udp_timeout =
                            atoi(root.child(NatEntryUDPTimeout_TAG).child_value());
  }

  this->nat_config.dmz_ip = inet_addr(root.child(DmzIP_TAG).child_value());
  this->nat_config.dmz_ip = ntohl(this->nat_config.dmz_ip);

  this->nat_config.enable_ipsec_vpn_pass_through =
                   atoi(root.child(EnableIPSECVpnPassthrough_TAG).child_value());

  this->nat_config.enable_pptp_vpn_pass_through =
                     atoi(root.child(EnablePPTPVpnPassthrough_TAG).child_value());

  this->nat_config.enable_l2tp_vpn_pass_through =
                    atoi(root.child(EnableL2TPVpnPassthrough_TAG).child_value());

  if (root.child(Initial_Pkt_Limit_TAG))
  {
    QCMAP_NATALG::nat_config_common.initial_pkt_limit=
                            atoi(root.child(Initial_Pkt_Limit_TAG).child_value());
  }

  if (root.child(ALGCfg_TAG))
  {
    if (atoi(root.child(ALGCfg_TAG).child(EnableRTSPAlg_TAG).child_value()) == 1)
      this->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_RTSP_ALG_V01;
    else
      this->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_RTSP_ALG_V01;
  }

  if (root.child(ALGCfg_TAG))
  {
    if (atoi(root.child(ALGCfg_TAG).child(EnableSIPAlg_TAG).child_value()) == 1)
      this->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
    else
      this->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_SIP_ALG_V01;
  }

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), PortFwding_TAG))
    {
      subchild=child;
      inet_aton(subchild.child(PortFwdingPrivateIP_TAG).child_value(),
                (in_addr *)&(nat_entry.port_fwding_private_ip));
      nat_entry.port_fwding_private_ip =
                                   ntohl(nat_entry.port_fwding_private_ip);
      nat_entry.port_fwding_private_port =
                        atoi(subchild.child(PortFwdingPrivatePort_TAG).child_value());

      nat_entry.port_fwding_global_port =
                         atoi(subchild.child(PortFwdingGlobalPort_TAG).child_value());

      nat_entry.port_fwding_protocol =
                           atoi(subchild.child(PortFwdingProtocol_TAG).child_value());

      debug_string= htonl(nat_entry.port_fwding_private_ip);
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);

      LOG_MSG_INFO1( "PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d, PortFwdingProtocol: %d",
                     nat_entry.port_fwding_private_port,
                     nat_entry.port_fwding_global_port,
                     nat_entry.port_fwding_protocol);

      if(this->AddNATEntryToList(&nat_entry) == false)
      {
        LOG_MSG_ERROR("Failed to Add NAT Entry to List", 0, 0, 0);
        return false;
      }
    }
  }

   if (strncasecmp(PRC_TAG, root.child(NATType_TAG).child_value(),
                   strlen(PRC_TAG)) == 0)
   {
     this->nat_config.nat_type = QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01;
   }
   else if (strncasecmp(FULLCONE_TAG,
                        root.child(NATType_TAG).child_value(),
                        strlen(FULLCONE_TAG)) == 0)
   {
     this->nat_config.nat_type = QCMAP_MSGR_NAT_FULL_CONE_NAT_V01;
   }
   else if (strncasecmp(ARC_TAG, root.child(NATType_TAG).child_value(),
                        strlen(ARC_TAG)) == 0)
   {
     this->nat_config.nat_type = QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01;
   }
   else
   {
     /* Default SYMMETRIC NAT. */
     this->nat_config.nat_type = QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01;
   }

  return true;
}


/*==========================================================
  FUNCTION SetNatType
==========================================================*/
/*!
@brief
  Configures the nat type to be used and enables the same.

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
/*=========================================================*/
boolean QCMAP_NATALG::SetNatType
(
  qcmap_msgr_nat_enum_v01 nat_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int ret;
  QCMAP_Firewall* QcMapFirewall = QcMapBackhaul->QcMapFirewall;
  QCMAP_LAN *lan_obj = GET_DEFAULT_LAN_OBJECT();
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  ret = QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num);

  if (nat_type < QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01 ||
       nat_type > QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01)
  {
    LOG_MSG_ERROR("Error: NAT type not supported.\n",0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if(lan_obj)
  {
    if((lan_cfg = lan_obj->GetLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
  } else {
      LOG_MSG_ERROR("Default LAN object is NULL", 0, 0, 0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
  }

  if(QcMapBackhaul == GET_DEFAULT_BACKHAUL_OBJECT() &&
     lan_cfg->ip_passthrough_cfg.ip_passthrough_active)
  {
    LOG_MSG_ERROR("IP Passthrough is Active. Cannot change NAT type.\n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;;
    return false;
  }


  //NAT is only running if we are not in bridge mode
  if(QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    DisableNATonApps();
    if (QcMapFirewall) //TODO: Check if this is needed
      QcMapFirewall->CleanIPv4MangleTable();
    this->nat_config.nat_type = nat_type;

    if (( ret == BACKHAUL_TYPE_AP_STA_ROUTER)
        || (ret == BACKHAUL_TYPE_CRADLE) || ( ret == BACKHAUL_TYPE_ETHERNET )
        || (ret == BACKHAUL_TYPE_BT))
    {
      lan_obj->AllowBackhaulAccessOnIface(IP_V4, devname);
    }
    EnableNATonApps();
  }
    //store NAT type
  this->nat_config.nat_type = nat_type;

  switch(nat_type)
  {
    case QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01:
      GetSetNATConfigFromXML(CONFIG_NAT_TYPE, SET_VALUE, PRC_TAG, strlen(PRC_TAG));
    break;

    case QCMAP_MSGR_NAT_FULL_CONE_NAT_V01:
      GetSetNATConfigFromXML(CONFIG_NAT_TYPE, SET_VALUE, FULLCONE_TAG, strlen(FULLCONE_TAG));
    break;

    case QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01:
      GetSetNATConfigFromXML(CONFIG_NAT_TYPE, SET_VALUE, ARC_TAG, strlen(ARC_TAG));
    break;

    case QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01:
      GetSetNATConfigFromXML(CONFIG_NAT_TYPE, SET_VALUE, SYMMETRIC_TAG, strlen(SYMMETRIC_TAG));
    break;

    default:
      LOG_MSG_INFO1("Nat Type:%d not supported.\n", nat_type,0,0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
    break;

  }

  return true;
}

/*==========================================================
  FUNCTION searchNATEntryInXML
==========================================================*/
/*!
@brief
  Searches the NAT Config in the XML

@parameters
  pugi::xml_document *xml_file,
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

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

pugi::xml_node QCMAP_NATALG::searchNATEntryInXML
(
  pugi::xml_document *xml_file,
  qcmap_cm_port_fwding_entry_conf_t* nat_entry
)
{
  pugi::xml_node root, child;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;

  if (!(root = QcMapBackhaul->GetPdnRootXML(xml_file)))
  {
    LOG_MSG_ERROR("Unable to find NAT cfg corresponding to PDN",0,0,0);
    return child;
  }
  root = root.child(MobileAPNatCfg_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(),PortFwding_TAG))
    {
      if(((atoi(child.child_value(PortFwdingGlobalPort_TAG)) ==
                                        nat_entry->port_fwding_global_port)) &&
          ((atoi(child.child_value(PortFwdingProtocol_TAG)) ==
                                             nat_entry->port_fwding_protocol)))
      {
        LOG_MSG_INFO1("Existing NAT Entry found", 0, 0, 0);
        return child;
      }
    }
  }

  debug_string= htonl(nat_entry->port_fwding_private_ip);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\PortFwdingPrivateIP: '%s'",str,0,0);

  LOG_MSG_INFO1("PortFwdingPrivatePort: %d , PortFwdingGlobalPort: %d,\
                 PortFwdingProtocol: %d",nat_entry->port_fwding_private_port,\
                 nat_entry->port_fwding_global_port,nat_entry->port_fwding_protocol);

  return child;
}

/*==========================================================
  FUNCTION GetSetNATConfigFromXML
==========================================================*/
/*!
@brief
  Gets the NAT Config from the XML

@parameters
  qcmap_nat_config_enum conf,
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

boolean QCMAP_NATALG::GetSetNATConfigFromXML
(
  qcmap_nat_config_enum conf,
  qcmap_action_type action,
  char *data,
  int data_len
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  const char *tag_ptr;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
     LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
     return false;
  }

  if (!(root = QcMapBackhaul->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find NAT cfg corresponding to PDN",0,0,0);
    return false;
  }
  root = root.child(MobileAPNatCfg_TAG);

  if (conf == CONFIG_RTSP_ALG)
  {
    if(action == SET_VALUE)
    {
      root.child(ALGCfg_TAG).child(EnableRTSPAlg_TAG).text() = data;
      QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    }
    else
    {
      strlcpy(data, root.child(ALGCfg_TAG).child(EnableRTSPAlg_TAG).child_value(),
              data_len);
    }
    return TRUE;
  }
  else if (conf == CONFIG_SIP_ALG)
  {
    if (action == SET_VALUE)
    {
      root.child(ALGCfg_TAG).child(EnableSIPAlg_TAG).text() = data;
      QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    }
    else
    {
      strlcpy(data, root.child(ALGCfg_TAG).child(EnableSIPAlg_TAG).child_value(),
              data_len);
    }
    return TRUE;
  }

  switch(conf)
  {
    case CONFIG_NAT_ENTRY_GENERIC_TIMEOUT:
    {
      tag_ptr = NatEntryGenericTimeout_TAG;
      break;
    }

    case CONFIG_NAT_ENTRY_ICMP_TIMEOUT:
    {
      tag_ptr = NatEntryICMPTimeout_TAG;
      break;
    }

    case CONFIG_NAT_ENTRY_TCP_ESTABLISHED_TIMEOUT:
    {
      tag_ptr = NatEntryTCPEstablishedTimeout_TAG;
      break;
    }

    case CONFIG_NAT_ENTRY_UDP_TIMEOUT:
    {
      tag_ptr = NatEntryUDPTimeout_TAG;
      break;
    }

    case CONFIG_DMZ_IP:
    {
      tag_ptr = DmzIP_TAG;
      break;
    }

    case CONFIG_PPTP_VPN_PASSTHROUGH:
    {
      tag_ptr = EnablePPTPVpnPassthrough_TAG;
      break;
    }

    case CONFIG_L2TP_VPN_PASSTHROUGH:
    {
      tag_ptr = EnableL2TPVpnPassthrough_TAG;
      break;
    }

    case CONFIG_IPSEC_VPN_PASSTHROUGH:
    {
      tag_ptr = EnableIPSECVpnPassthrough_TAG;
      break;
    }

    case CONFIG_SOCKSv5_PROXY:
    {
      tag_ptr = Enable_SOCKSv5_Proxy_TAG;
      break;
    }

    case CONFIG_SOCKSv5_CONF_FILE:
    {
      tag_ptr = SOCKSv5_Proxy_Conf_File_TAG;
      break;
    }

    case CONFIG_SOCKSv5_AUTH_FILE:
    {
      tag_ptr = SOCKSv5_Proxy_Auth_File_TAG;
      break;
    }

    case CONFIG_NAT_TYPE:
    {
      tag_ptr = NATType_TAG;
      break;
    }

    case CONFIG_PKT_LIMIT:
    {
      tag_ptr = Initial_Pkt_Limit_TAG;
      break;
    }

    default:
      LOG_MSG_ERROR("Invalid Config type to set.", 0, 0, 0);
      return FALSE;
    break;
  }

  //Set/Get the value based on action.
  if (action == SET_VALUE)
  {
    root.child(tag_ptr).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
    strlcpy(data, root.child(tag_ptr).child_value(), data_len);

  LOG_MSG_INFO1("Config: %d Action: %d value: %d" , conf, action, atoi(data));

  return TRUE;
}

/*============================================================
  FUNCTION EnableMiscNATTasks
==========================================================================*/
/*!
@brief
  Adds SNAT, DMZ, VPN passthrough and firewall configurations.

@parameters
  void

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
boolean QCMAP_NATALG::EnableMiscNATTasks()
{
  int i = 0;
  boolean ret = false;
  qmi_error_type_v01 qmi_err_num;
  ds_dll_el_t * node = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;

  qcmap_msgr_ip_family_enum_v01 ip_family;
  QCMAP_Firewall* QcMapFirewall = QcMapBackhaul->QcMapFirewall;
  QCMAP_Backhaul_WWAN *wwan_obj = QcMapBackhaul->QcMapBackhaulWWAN;

  qcmap_nat_entry_list_t* natList = &(this->nat_config.port_fwding_entries);
  qcmap_cm_port_fwding_entry_conf_t* curr_nat_entry = NULL;

  qcmap_firewall_entry_list_t* firewallList = &(QcMapFirewall->firewall_config.extd_firewall_entries);
  qcmap_msgr_firewall_entry_conf_t *firewall_entry = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  node = natList->natEntryListHead;
  /* Config SNAT */
  if (node)
  {
    node = ds_dll_next (node, (const void**)(&curr_nat_entry));
    if (node == NULL)
    {
      LOG_MSG_ERROR("EnableMiscNATTasks - NAT info is NULL"
                    "Cant fetch NAT info",0,0,0);
      return false;
    }
    for ( i=0; i < nat_config.num_port_fwding_entries && curr_nat_entry; i++ )
    {
      if (curr_nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP )
      {
        AddSNATEntryOnApps(curr_nat_entry, &qmi_err_num);
      }
      else
      {
        curr_nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
        AddSNATEntryOnApps(curr_nat_entry, &qmi_err_num);
        curr_nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;
        AddSNATEntryOnApps(curr_nat_entry, &qmi_err_num);
        curr_nat_entry->port_fwding_protocol = PS_IPPROTO_TCP_UDP;
      }
      //node = node->next;
      node = ds_dll_next (node, (const void**)(&curr_nat_entry));
    }
  }

  /* Config DMZ IP.*/
  if (nat_config.dmz_ip != 0)
  {
    AddDMZOnApps(nat_config.dmz_ip, &qmi_err_num);
  }

  // Adding default firewall rules
  ip_family = QCMAP_MSGR_IP_FAMILY_V4_V01;
  if (!QcMapFirewall->SetDefaultFirewallRule(ip_family))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV4",0,0,0);
  }

  // Adding Firewall rules for IPV4
  if (QcMapFirewall->firewall_config.firewall_enabled)
  {
    node = firewallList->firewallEntryListHead;
    if (node)
    {
      node = ds_dll_next (node, (const void**)(&firewall_entry));
      if (node == NULL)
      {
        LOG_MSG_ERROR("AddStaticNatEntry - NAT info is NULL"
                      "Cant fetch NAT info",0,0,0);
        return false;
      }
    }
    for (i = 0; i < QcMapFirewall->firewall_config.num_firewall_entries; i++) {
       if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V4 ) {
           QcMapFirewall->SetFirewallV4(firewall_entry, true, &qmi_err_num);
       }
       //node = node->next;
       node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }

  /* Configure VPN PassThrough. */
  SetIPSECVpnPassThroughOnApps(nat_config.enable_ipsec_vpn_pass_through, &qmi_err_num);
  SetL2TPVpnPassThroughOnApps(nat_config.enable_l2tp_vpn_pass_through, &qmi_err_num);
  SetPPTPVpnPassThroughOnApps(nat_config.enable_pptp_vpn_pass_through, &qmi_err_num);

  /* Configure Webserver WWAN Access. */
  QcMapBackhaul->SetWebserverWWANAccessOnApps(QcMapBackhaul->wan_cfg.enable_webserver_wwan_access,
                                              &qmi_err_num);

  /* Enable ALGs from XML file */
  if (nat_config.enable_alg_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01)
  {
    LOG_MSG_INFO1("QCMAP_NATALG::EnableRTSPAlg() Called",0,0,0);
    ret = EnableRTSPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to enable RTSP ALG, Ret value = %d error: %d.\n",
                     ret, qmi_err_num, 0);
    }
  }

  /*SIP Alg is enabled by default in kernel. So we to enable or disable based on
    the config explicitly*/
  if (nat_config.enable_alg_mask & QCMAP_MSGR_MASK_SIP_ALG_V01)
  {
    LOG_MSG_INFO1("QCMAP_NATALG::EnableSIPAlg() Called",0,0,0);
    ret = EnableSIPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to enable SIP ALG, Ret value = %d error: %d.\n",
                     ret, qmi_err_num, 0);
    }
  }
  else
  {
    LOG_MSG_INFO1("QCMAP_NATALG::DisableSIPAlg() Called",0,0,0);
    ret = DisableSIPAlg(&qmi_err_num);
    if(ret != true)
    {
      LOG_MSG_ERROR("Fail to Disable SIP ALG, Ret value%d Error: %d\n",
                     ret, qmi_err_num, 0);
    }
  }

  return ret;
}


/*===========================================================================
  FUNCTION EnableNATonApps
==========================================================================*/
/*!
@brief
  Enables NAT on A5 based on the NAT type configured.

@parameters
  void

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
boolean QCMAP_NATALG::EnableNATonApps()
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char gwIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE], subnetIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netMaskIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char tmp[MAX_COMMAND_STR_LEN];
  int qcmap_cm_error = 0;
  in_addr_t public_ip = 0, sec_dns_addr = 0;
  in_addr_t default_gw_addr = 0, net_mask = 0, public_ip_wwan = 0;
  uint32 pri_dns_addr = 0;
  qmi_error_type_v01 qmi_err_num;
  int qcmap_cm_errno;
  int ret;
  char enable[MAX_STRING_LENGTH];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = NULL;
  QCMAP_Firewall* QcMapFirewall = NULL;
  /*-------------------------------------------------------------------------*/

  if(NULL != QcMapBackhaul)
  {
    QcMapBackhaulWWAN = QcMapBackhaul->QcMapBackhaulWWAN;
    QcMapFirewall = QcMapBackhaul->QcMapFirewall;
  }
  else
  {
    LOG_MSG_ERROR("QcMapBackhaul is NULL", 0, 0, 0);
    return false;
  }

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr || !QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }

  /* Check for STA mode, before we start setting up NAT on A5 */
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  memset(publicIpAddr, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(gwIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(subnetIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(netMaskIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  /* 1. For default profile check if current backahul is WWAN
   * 2. For non-default profile above check not required.
   */
  if (IS_DEFAULT_PROFILE(this->QcMapBackhaul->profileHandle))
  {
    ret = QcMapBackhaul->GetDeviceName(devname, QCMAP_MSGR_IP_FAMILY_V4_V01, &qmi_err_num);
  }
  else
  {
    if (QcMapBackhaulWWAN && QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle,
          QCMAP_MSGR_IP_FAMILY_V4_V01, devname, &qcmap_cm_errno) == QCMAP_CM_ERROR)
    {
      LOG_MSG_ERROR("Failed to GetDeviceName for profile=%d", GetProfileHandle(), 0,0);
      return false;
    }
    ret = BACKHAUL_TYPE_WWAN; //set ret for non-default profile
  }

  if (ret == BACKHAUL_TYPE_WWAN)
  {
    if(QcMapBackhaulWWAN && (QCMAP_CM_SUCCESS !=
       QcMapBackhaulWWAN->GetIPv4NetworkConfig(&public_ip, &pri_dns_addr,
                                               &sec_dns_addr, &default_gw_addr, &qmi_err_num)))
    {
      LOG_MSG_ERROR("Couldn't get public ip address. error %d", qmi_err_num,0,0);
      return false;
    }

    if ( false == QcMapBackhaul->GetIP(&public_ip_wwan, &net_mask, devname) )
    {
      LOG_MSG_ERROR("Unable to get the IP address",0,0,0);
      return false;
    }
    /* Update NetDev in Firewall. */
    if (QcMapBackhaul->QcMapFirewall)
    {
      QcMapBackhaul->QcMapFirewall->UpdateNetDevInXML((const char*) devname);
    }
  }
  else if(ret > BACKHAUL_TYPE_WWAN)
  {
    if (false == QcMapBackhaul->GetIP(&public_ip, &net_mask, devname) )
    {
      LOG_MSG_ERROR("Unable to get the IP address",0,0,0);
      return false;
    }
    if ( false == QcMapMgr->GetGatewayIP(&default_gw_addr) )
    {
      LOG_MSG_ERROR("Unable to get the Gateway IP",0,0,0);
      /* continue by adding just the default route. */
    }
  }
  else
  {
    LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", qcmap_cm_error,0,0);
    return false;
  }

  addr.s_addr = public_ip;
  strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = default_gw_addr;
  strlcpy(gwIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = public_ip & net_mask;
  strlcpy(subnetIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = net_mask;
  strlcpy(netMaskIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  /* Store ipv4 address info. */
  QcMapBackhaul->ipv4_public_ip = public_ip;
  QcMapBackhaul->ipv4_default_gw_addr = default_gw_addr;
  //store Netmask
  QcMapBackhaul->ipv4_public_subnet_mask = net_mask;
  bzero(QcMapBackhaul->wan_cfg.ipv4_interface, QCMAP_MSGR_INTF_LEN);
  strlcpy(QcMapBackhaul->wan_cfg.ipv4_interface, devname, QCMAP_MSGR_INTF_LEN);

  /* Delete the default route to backhaul. */
#ifndef FEATURE_QTIMAP_OFFTARGET
  snprintf( command, MAX_COMMAND_STR_LEN, "ip route del default dev %s", devname);
  ds_system_call(command, strlen(command));

  /* Delete the subnet base route */
  snprintf( command, MAX_COMMAND_STR_LEN, "ip route del %s/%s dev %s",\
              subnetIP, netMaskIP, devname);
  ds_system_call(command, strlen(command));

  /* Remove PDN specific subnet rout if needed as well */
  if ((!IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle)) &&
      (QcMapBackhaul->vlan_id !=  QCMAP_MSGR_VLAN_ID_NONE_V01))
  {
    snprintf(tmp,MAX_COMMAND_STR_LEN," table pdn_table_%d", QcMapBackhaul->profileHandle);
    strlcat(command, tmp, sizeof (command));
    ds_system_call(command, strlen(command));
  }
#endif

  /*-----------------------------------------------------------------------
    To allow embedded call on same PDN, traffic from MobileAP clients should
    not be forwarded to WWAN interface. So we install a firewall rule in
    DisableNATonApps to not to forward traffic from bridge interface to WWAN
    interface. So if we flush the rules here even that entry will be deleted.
    To avoid it we do the flushing only when in WWANmode. When we are swithing
    between WWAN and station mode, we anyway first flush all the existing
    iptables rules in DisableNATonApps. So not doing it for station mode should
    affect anything.
  -----------------------------------------------------------------------*/
  if (ret == BACKHAUL_TYPE_WWAN)
  {
    CleanIPv4FilterTableFwdChain();
  }

  /* Flush Mangle table */
  if (QcMapFirewall)
    QcMapFirewall->CleanIPv4MangleTable();
  CleanIPv4NatTable();

   /* Check for enable ipv4 must be here to remove the default routes which
     DHCPCD adds */
  QCMAP_Backhaul::GetSetBackhaulConfigFromXML(CONFIG_ENABLE_IPV4,GET_VALUE,\
                                              enable,MAX_STRING_LENGTH);
  if ( !((boolean)atoi(enable)) )
  {
    LOG_MSG_ERROR("Error: IPV4 not enabled. Not continuing further\n",0,0,0);
    return false;
  }

  /*-----------------------------------------------------------------------
    Set up forwarding and masquerading.
  -----------------------------------------------------------------------*/
  switch (nat_config.nat_type)
  {
    case QCMAP_MSGR_NAT_FULL_CONE_NAT_V01:
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
                "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE --random", devname );
      ds_system_call(command, strlen(command));

      snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -A PREROUTING -i %s -j NATTYPE --mode dnat --type 1", devname );
      ds_system_call(command, strlen(command));
      /*
      * When UL data path is through IPA, only one packet traverses
      * through SW. NATTYPE module requires at least 2 packets to create
      * a valid NATTYPE entry. As the NATTYPE entry is not created, DL
      * data fails in ARCN and FCN. Move NATTYPE forward mode to
      * POSTROUTING chain to create the NATTYPE entry with only one packet.
      */
      snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -A POSTROUTING -o %s -j NATTYPE --mode forward_out"
                 " --type 1", devname );
      ds_system_call(command, strlen(command));
      break;
    }
    case QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01:
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE --random", devname);
      ds_system_call(command, strlen(command));
      break;
    }
    case QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01:
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "iptables -t nat -A POSTROUTING -o %s -j SNAT --to-source %s",
               devname, publicIpAddr);
      ds_system_call(command, strlen(command));
      break;
    }
    case QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01:
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
               "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE --random",
               devname );
      ds_system_call(command, strlen(command));

      snprintf( command, MAX_COMMAND_STR_LEN,
               "iptables -t nat -A PREROUTING -i %s -j NATTYPE --mode dnat"
               " --type 2", devname );
      ds_system_call(command, strlen(command));

      /*
      * When UL data path is through IPA, only one packet traverses
      * through SW. NATTYPE module requires at least 2 packets to create
      * a valid NATTYPE entry. As the NATTYPE entry is not created, DL
      * data fails in ARCN and FCN. Move NATTYPE forward mode to
      * POSTROUTING chain to create the NATTYPE entry with only one packet.
      */
      snprintf( command, MAX_COMMAND_STR_LEN,
               "iptables -t nat -A POSTROUTING -o %s -j NATTYPE --mode forward_out"
               " --type 2", devname );
      ds_system_call(command, strlen(command));
      break;
    }
  }

  /*-----------------------------------------------------------------------
    Enable packet forwarding
  -----------------------------------------------------------------------*/
  snprintf( command, MAX_COMMAND_STR_LEN,
            "echo 1 > /proc/sys/net/ipv4/ip_forward");
  ds_system_call(command, strlen(command));

  /*-----------------------------------------------------------------------
    Enable ARP Proxy on WAN interface
  -----------------------------------------------------------------------*/
  snprintf( command, MAX_COMMAND_STR_LEN,
            "echo 1 > /proc/sys/net/ipv4/conf/%s/proxy_arp", devname );
  ds_system_call(command, strlen(command));

  LOG_MSG_INFO1("NAT Enabled.",0,0,0);

  /* Drop unstatefull tcp packets */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -t filter -A FORWARD -i %s -p tcp -m state --state INVALID -j DROP",
            GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id) );
  ds_system_call(command, strlen(command));

  /*-----------------------------------------------------------------------
    Install the subnet route..
  -----------------------------------------------------------------------*/
  snprintf( command, MAX_COMMAND_STR_LEN, "ip route add %s/%s dev %s",\
            subnetIP, netMaskIP, devname);
  ds_system_call(command, strlen(command));

  /* Subnet rule must be added to default table and vlan table.
     This is so the route lookup packets can resolve the network
     for the defualt route below */
  if (QcMapBackhaul->vlan_id !=  QCMAP_MSGR_VLAN_ID_NONE_V01)
  {
    snprintf(tmp,MAX_COMMAND_STR_LEN," table pdn_table_%d", QcMapBackhaul->profileHandle);
    strlcat(command, tmp, sizeof (command));
    ds_system_call(command, strlen(command));
  }

  /*-----------------------------------------------------------------------
    Install the default route..
  -----------------------------------------------------------------------*/
  #ifndef FEATURE_QTIMAP_OFFTARGET
    if ( default_gw_addr != 0 )
    {
      snprintf( command, MAX_COMMAND_STR_LEN, "ip route add default via %s dev %s",
                gwIP ,devname);
    }
    else
    {
      snprintf( command, MAX_COMMAND_STR_LEN, "route add default dev %s",
                devname);
    }

    if (! (IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle)) &&
        QcMapBackhaul->vlan_id !=  QCMAP_MSGR_VLAN_ID_NONE_V01)
    {
      snprintf(tmp,MAX_COMMAND_STR_LEN," table pdn_table_%d", QcMapBackhaul->profileHandle);
      strlcat(command, tmp, sizeof (command));
    }

    ds_system_call(command, strlen(command));
  #endif
  EnableMiscNATTasks();
  /* Clear the existing connection track entries. */
  /* Delete the conntrack connections based on source NAT and destination NAT flags. */
  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -q %s", publicIpAddr);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -d %s", publicIpAddr);
  ds_system_call(command, strlen(command));
  /*clear the specific backhaul adress conntrack */
  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -d %s -p udp", publicIpAddr);
  ds_system_call(command,strlen(command));

  if (IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle))
  {
    if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
                         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
                         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
                         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01))
    {
      if (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile
          == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        addr.s_addr = QcMapBackhaul->ipv4_public_ip;
        strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 "
                  " --ip-source %s -j DROP", QcMapMgr->ap_dev_num2, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4 "
                  " --ip-source %s -j DROP", QcMapMgr->ap_dev_num2, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 "
                  " --ip-destination %s -j DROP", QcMapMgr->ap_dev_num2, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN, "ebtables -A INPUT -i wlan%d -p IPv4 "
                  " --ip-destination %s -j DROP", QcMapMgr->ap_dev_num2, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 "
                 " -j DROP", QcMapMgr->ap_dev_num2);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 "
                  " -j DROP", QcMapMgr->ap_dev_num2);
        ds_system_call(command, strlen(command));
      }

      /*AP-AP-AP-AP or AP-AP-AP mode : GuestAP_2*/
      if(QcMapWLANMgr &&(QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
         QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ) &&
         QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile
          == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 "
                  " --ip-source %s -j DROP", QcMapMgr->ap_dev_num3, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4 "
                  " --ip-source %s -j DROP", QcMapMgr->ap_dev_num3, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 "
                  " --ip-destination %s -j DROP", QcMapMgr->ap_dev_num3, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv4 "
                  " --ip-destination %s -j DROP", QcMapMgr->ap_dev_num3, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 "
                  " -j DROP \n", QcMapMgr->ap_dev_num3);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 "
                  " -j DROP",QcMapMgr->ap_dev_num3);
        ds_system_call(command, strlen(command));

      }

  //AP-AP-AP-AP mode: GuestAP-3
      if(QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 &&
         QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile
          == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 "
                  " --ip-source %s -j DROP", QcMapMgr->ap_dev_num4, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4 "
                  " --ip-source %s -j DROP", QcMapMgr->ap_dev_num4, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 "
                  " --ip-destination %s -j DROP", QcMapMgr->ap_dev_num4, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv4 "
                  " --ip-destination %s -j DROP", QcMapMgr->ap_dev_num4, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 "
                  " -j DROP", QcMapMgr->ap_dev_num4);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 "
                  " -j DROP", QcMapMgr->ap_dev_num4);
        ds_system_call(command, strlen(command));
      }
    }

    if( IS_DEFAULT_PROFILE(GetProfileHandle()) && !QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      if(QcMapBackhaulWWAN && (QCMAP_Backhaul_WWAN::ddns_conf.enable == DDNS_ENABLED) &&
          (QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED) )
      {
        QcMapBackhaulWWAN->StartDDNS(&qmi_err_num);
      }

      /*Enable PMIP mode */
      if(QcMapBackhaulWWAN && (QCMAP_Backhaul::IsPmipV4ModeEnabled() || QCMAP_Backhaul::IsPmipV6ModeEnabled()))
      {
        if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01)
        {
          LOG_MSG_INFO1("Pmip is in pmipv4 tunnel mode start PMIP As WAN connected",0,0,0);
          if(!QcMapBackhaul->EnablePmipMode(&qmi_err_num))
          {
            LOG_MSG_ERROR("EnablePmipMode failed with error:%d",qmi_err_num,0,0);
          }
        }
        else if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
        {
          LOG_MSG_INFO1("Pmip is in pmipv6 tunnel mode start PMIP will be on IPV6 WAN connected",0,0,0);
        }
        else
        {
          LOG_MSG_ERROR("Pmip tunnel Mode is invalid %d",QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode,0,0);
          return false;
        }
      }

    }

    if(QcMapBackhaul->tiny_proxy_enable_state == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01)
    {
      QCMAP_Backhaul_WWAN::EnableTinyProxy(&qmi_err_num);
    }
  }

  /*Send Backhaul Status Indication for only default WWAN backhaul object*/
  LOG_MSG_INFO1("Send IPV4 backhaul connect indication", 0, 0, 0);
  QcMapBackhaul->SendBackhaulStatusInd(BACKHAUL_AVAILABLE,BACKHAUL_NOT_APPLICABLE,QcMapBackhaul->current_backhaul);

  return true;
}

/*===========================================================================
  FUNCTION DisableNATonApps
==========================================================================*/
/*!
@brief
  This function will delete NAT entries on A5.

@parameters
  void

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
boolean QCMAP_NATALG::DisableNATonApps()
{
  char command[MAX_COMMAND_STR_LEN] = {0};
  struct in_addr addr;
  boolean ret = false;
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char gwIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char tmp[MAX_COMMAND_STR_LEN];
  char subnetIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netMaskIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = QcMapBackhaul->QcMapBackhaulWWAN;
  QCMAP_WLAN* QcMapWLANMgr = QCMAP_WLAN::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg = QcMapBackhaul->QcMapNatAlg;
  QCMAP_LAN *lan_obj = GET_LAN_OBJECT(QcMapBackhaul->vlan_id);
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  qcmap_msgr_device_type_enum_v01 dev_type;

  if(!QcMapBackhaulWWAN || !lan_obj)
  {
    LOG_MSG_ERROR("DisableNATonApps: NULL pointer objects",0,0,0);
    return false;
  }

  /*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  /* Cant continue if SoftAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }

  if ( !QCMAP_Backhaul::enable_ipv4)
  {
    LOG_MSG_ERROR("Error: IPV4 not enabled.\n",0,0,0);
    return false;
  }

  /*-----------------------------------------------------------------------
    Delete iptables entries and flush tables.
    -----------------------------------------------------------------------*/
  CleanIPv4FilterTableFwdChain();
  CleanIPv4FilterTableInChain();
  CleanIPv4NatTable();

  /* Install the rule to block WWAN access. */
  lan_obj->BlockIPv4WWANAccess();

  if (IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle))
  {
    if((lan_cfg = lan_obj->GetLANConfig()) == NULL)
    {
      LOG_MSG_ERROR("Failed to get LAN config", 0, 0, 0);
      return false;
    }

    if(lan_cfg->ip_passthrough_cfg.ip_passthrough_active)
    {
      dev_type = lan_cfg->ip_passthrough_cfg.device_type;
      lan_obj->DisableIPPassthrough(false);
      lan_obj->RestartTetheredClient(dev_type);
    }

    /*-----------------------------------------------------------------------
      Disable ALGs with enable bit set in XML.
    -----------------------------------------------------------------------*/
    if (nat_config.enable_alg_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01)
    {
      LOG_MSG_INFO1("QCMAP_NATALG::DisableRTSPAlg() Called",0,0,0);
      ret = DisableRTSPAlg(&qmi_err_num);
      if(ret != true)
      {
        LOG_MSG_ERROR("Fail to Disable RTSP ALG, Ret value%d Error: %d\n",
                       ret, qmi_err_num, 0);
      }
    }

    if (nat_config.enable_alg_mask & QCMAP_MSGR_MASK_SIP_ALG_V01)
    {
      LOG_MSG_INFO1("QCMAP_NATALG::DisableSIPAlg() Called",0,0,0);
      ret = DisableSIPAlg(&qmi_err_num);
      if(ret != true)
      {
        LOG_MSG_ERROR("Fail to Disable SIP ALG, Ret value%d Error: %d\n",
                       ret, qmi_err_num, 0);
      }
    }
  }

  /* Check for STA mode, before we start setting up NAT on A5 */
  memset(publicIpAddr, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(gwIP, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  /* Gateway address of a Connected Interface, set in function EnableNATonApps */
  if(QcMapBackhaul->ipv4_default_gw_addr != 0)
  {
    addr.s_addr = QcMapBackhaul->ipv4_default_gw_addr;
    strlcpy(gwIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    /* First delete the default route. */
  #ifndef FEATURE_QTIMAP_OFFTARGET
    snprintf( command, MAX_COMMAND_STR_LEN, "ip route del default via %s", gwIP);
    if (!IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle) && QcMapBackhaul->vlan_id !=  QCMAP_MSGR_VLAN_ID_NONE_V01)
    {
      snprintf(tmp,MAX_COMMAND_STR_LEN," table pdn_table_%d", QcMapBackhaul->profileHandle);
      strlcat(command, tmp, sizeof (command));
    }
    ds_system_call(command, strlen(command));
  #endif
  }
  else
  {
  #ifndef FEATURE_QTIMAP_OFFTARGET
    snprintf( command, MAX_COMMAND_STR_LEN, "ip route del default via 0.0.0.0");
    if (!IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle) && QcMapBackhaul->vlan_id !=  QCMAP_MSGR_VLAN_ID_NONE_V01)
    {
      snprintf(tmp,MAX_COMMAND_STR_LEN," table pdn_table_%d", QcMapBackhaul->profileHandle);
      strlcat(command, tmp, sizeof (command));
    }
    ds_system_call(command, strlen(command));
  #endif
  }

  ret = QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, &qmi_err_num);
  if (ret)
  {
    /* Delete the default route to backhaul. */
    snprintf( command, MAX_COMMAND_STR_LEN, "ip route del default dev %s", devname);
    if ( ! (IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle)) &&
        QcMapBackhaul->vlan_id !=  QCMAP_MSGR_VLAN_ID_NONE_V01)
    {
      snprintf(tmp,MAX_COMMAND_STR_LEN," table pdn_table_%d", QcMapBackhaul->profileHandle);
      strlcat(command, tmp, sizeof (command));
    }
    ds_system_call(command, strlen(command));
  }
  else
  {
    LOG_MSG_ERROR("Get Device Name Failed. So Default route Will not be deletd", 0, 0, 0);
  }

  if(QcMapBackhaul->ipv4_public_ip == 0 || QcMapBackhaul->ipv4_public_subnet_mask == 0)
  {
    LOG_MSG_ERROR("v4 public ip or subnet mask is 0 ."
                  " Subnet based route will not be deleted",0, 0, 0);
  }
  else
  {
    strlcpy(devname,QcMapBackhaul->wan_cfg.ipv4_interface,sizeof(devname));
    addr.s_addr = QcMapBackhaul->ipv4_public_ip & QcMapBackhaul->ipv4_public_subnet_mask;
    strlcpy(subnetIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    addr.s_addr = QcMapBackhaul->ipv4_public_subnet_mask;
    strlcpy(netMaskIP, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    snprintf(command, MAX_COMMAND_STR_LEN, "ip route del %s/%s dev %s",\
             subnetIP, netMaskIP, devname);
    ds_system_call(command, strlen(command));
  }

  addr.s_addr = QcMapBackhaul->ipv4_public_ip;
  strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  /* Delete the conntrack connections based on source NAT and destination NAT flags. */
  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -q %s", publicIpAddr);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN, "conntrack -D -d %s", publicIpAddr);
  ds_system_call(command, strlen(command));

  if (IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle))
  {
    if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 ||
        QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
        QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
        QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 ))
    {
      if (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile
          == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        //addr.s_addr = QcMapBackhaul->ipv4_public_ip;
        //strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 --ip-source %s -j DROP \n",
                    QcMapMgr->ap_dev_num2, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 --ip-destination %s -j DROP \n",
                   QcMapMgr->ap_dev_num2, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv4 -j DROP \n",
                    QcMapMgr->ap_dev_num2);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4 -j DROP \n",
                    QcMapMgr->ap_dev_num2);
        ds_system_call(command, strlen(command));
      }

      //AP-AP-AP or AP-AP-AP-AP Guest AP2
      if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01 ||
          QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) &&
          (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].access_profile
          == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 --ip-source %s -j DROP \n",
                 QcMapMgr->ap_dev_num3, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 --ip-destination %s -j DROP \n",
                 QcMapMgr->ap_dev_num3, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv4 -j DROP \n",
                  QcMapMgr->ap_dev_num3);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4 -j DROP \n",
                 QcMapMgr->ap_dev_num3);
        ds_system_call(command, strlen(command));

      }

      //Guest AP3
      if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01 &&
         (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].access_profile
          == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01))
      {
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D OUTPUT -o wlan%d -p IPv4 --ip-source %s -j DROP \n",
                  QcMapMgr->ap_dev_num4, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -D INPUT -i wlan%d -p IPv4 --ip-destination %s -j DROP \n",
                  QcMapMgr->ap_dev_num4, publicIpAddr);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A INPUT -i wlan%d -p IPv4 -j DROP \n",
                  QcMapMgr->ap_dev_num4);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"ebtables -A OUTPUT -o wlan%d -p IPv4 -j DROP \n",
                  QcMapMgr->ap_dev_num4);
        ds_system_call(command, strlen(command));
      }

    }

    /* Clear the ipv4 address info since disconnecting and the same
       will be reset in EnableNATonApps*/
    QcMapBackhaul->ipv4_public_ip = 0;
    QcMapBackhaul->ipv4_default_gw_addr = 0;

    if( IS_DEFAULT_PROFILE(GetProfileHandle()) && !QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      if(QcMapBackhaulWWAN &&
         QCMAP_Backhaul_WWAN::ddns_conf.enable == DDNS_ENABLED &&
         QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED
         && QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED)
      {
        QcMapBackhaulWWAN->StopDDNS();
      }

      if (QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED)
      {
        if(QCMAP_Backhaul::IsPmipV4ModeEnabled() || QCMAP_Backhaul::IsPmipV6ModeEnabled())
        {
          if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V4_V01)
          {
            LOG_MSG_INFO1("Pmip tunnel mode is v4 Disable pmip on WAN disconnect",0,0,0)
            QcMapBackhaul->DisablePmipMode(&qmi_err_num, PMIP_DISABLE_EV);
          }
          else if (QCMAP_Backhaul::pmip_conf.pmip_tunnel_mode == QCMAP_MSGR_IP_FAMILY_V6_V01)
          {
            LOG_MSG_ERROR("Pmip tunnel mode is v6. Do not Disable pmip."
                        " It will be disconnected On IPV6 WAN disconnect",0,0,0);
          }
          else
          {
            LOG_MSG_ERROR("Pmip v4 tunnel mode is Invalid.",0,0,0);
            return false;
          }
        }
      }


      if (QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED)
      {
          //cleanup only the IPv4 DNS addresses
          if(lan_obj &&
            (IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle)) &&
            (QcMapBackhaulWWAN->GetProfileHandle() != 0))
          {
            lan_obj->DeleteDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv4_addr,
                                              QcMapBackhaulWWAN->sec_dns_ipv4_addr);
          } else {
            LOG_MSG_INFO1("LAN object NULL, could not clean up IPv4 DNS addrs", 0, 0, 0);
          }
      }
    }
    if(QcMapBackhaul->tiny_proxy_enable_state == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01)
    {
      QCMAP_Backhaul_WWAN::StopTinyProxy(&qmi_err_num);
    }
  }

  /*Send Backhaul Status Indication for only default WWAN backhaul object*/
  LOG_MSG_INFO1("Send IPV4 backhaul disconnect indication", 0, 0, 0);
  QcMapBackhaul->SendBackhaulStatusInd(BACKHAUL_NOT_AVAILABLE,BACKHAUL_NOT_APPLICABLE,QcMapBackhaul->current_backhaul);

  return true;
}


/*===========================================================================
  FUNCTION GetNatTimeoutOnApps
==========================================================================*/
/*!
@brief
  Get the NAT timeout value for the requested nat type.

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          *timeout_value

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
static boolean QCMAP_NATALG::GetNatTimeoutOnApps
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 *timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  FILE *fp = NULL;
  int ret = QCMAP_CM_ERROR;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  if ( timeout_value == NULL )
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  ret = QcMapMgr->CompareKernelVer(KERNEL_VERSION_4_9);
  if (ret == FALSE)
  {
    /*If Kernel Version is below 4.9*/
    switch ( timeout_type )
    {
      case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:
        fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_generic_timeout", "r");
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
        fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_icmp_timeout", "r");
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
        fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established", "r");
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
        fp = fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout", "r");
        break;
      default:
        LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
    }
  }
  else
  {
    switch ( timeout_type )
    {
    /*Default and if Kernel Version is >= 4.9*/
       case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:
         fp = fopen("/proc/sys/net/netfilter/nf_conntrack_generic_timeout", "r");
         break;
       case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
         fp = fopen("/proc/sys/net/netfilter/nf_conntrack_icmp_timeout", "r");
         break;
       case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
         fp = fopen("/proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established", "r");
         break;
       case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
         fp = fopen("/proc/sys/net/netfilter/nf_conntrack_udp_timeout", "r");
         break;
       default:
         LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
         *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
         return false;
    }
  }
  if ( fp == NULL )
  {
     LOG_MSG_ERROR("Error opening timeout file: %d.\n", timeout_type,0,0);
     #ifdef FEATURE_QCMAP_OFFTARGET
       LOG_MSG_ERROR("Error opening timeout file skipping offtarget test: %d.\n", timeout_type,0,0);
       return true;
     #endif

     *qmi_err_num = QMI_ERR_INTERNAL_V01;
     return false;
  }

  if (fscanf(fp, "%d", timeout_value) != 1) {
    LOG_MSG_ERROR("Error reading timeout file: %d.\n", timeout_type,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  LOG_MSG_INFO1("Timeout Type: %d Timeout Value: %d.\n", timeout_type, *timeout_value,0);

  fclose(fp);

  return true;
}

/*===========================================================================
  FUNCTION AddNATEntryToList
==========================================================================*/
/*!
@brief
  Adds the NAT entry to the List.

@parameters
  qcmap_cm_port_fwding_entry_conf_t *nat_entry

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


boolean QCMAP_NATALG::AddNATEntryToList(qcmap_cm_port_fwding_entry_conf_t *nat_entry)
{


  ds_dll_el_t * node = NULL;
  qcmap_nat_entry_list_t* natList = &(this->nat_config.port_fwding_entries);
  qcmap_cm_port_fwding_entry_conf_t *new_nat_entry;

  if (natList->natEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
            information. This is done to make use of the doubly linked list framework which
            is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddNATEntryToList - Error in allocating memory for node",
                    0,0,0);
      return false;
    }
    natList->natEntryListHead = node;
  }

  new_nat_entry = (qcmap_cm_port_fwding_entry_conf_t*)ds_malloc(sizeof(qcmap_cm_port_fwding_entry_conf_t));

  if( new_nat_entry == NULL )
  {
    LOG_MSG_ERROR("AddNATEntryToList - Error in allocating memory for"
                  "new NAT entry",0,0,0);
    return false;
  }

  memset(new_nat_entry, 0, sizeof(qcmap_cm_port_fwding_entry_conf_t));
  /* save into the config */
  memcpy(new_nat_entry, nat_entry, sizeof(qcmap_cm_port_fwding_entry_conf_t));

  //Enque the node
  if ((node = ds_dll_enq(natList->natEntryListHead,
                          NULL, (void*)new_nat_entry )) == NULL)
  {
    LOG_MSG_ERROR("AddStaticNatEntry - Error in adding a node",0,0,0);
    ds_free(new_nat_entry);
    new_nat_entry = NULL;
    return false;
  }
  natList->natEntryListTail = node;

  this->nat_config.num_port_fwding_entries++;
  LOG_MSG_INFO1("AddNATEntryToList - Added Snat entry",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION SetNatTimeoutOnApps
==========================================================================*/
/*!
@brief
  Will set the NAT timeout value for the identified nat type.

@parameters
  qcmap_msgr_nat_timeout_enum_v01 timeout_type
  uint32                          timeout_value

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
static boolean QCMAP_NATALG::SetNatTimeoutOnApps
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 timeout_value,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  int ret = QCMAP_CM_ERROR;
  QCMAP_NATALG* QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  if(QcMapNatAlg == NULL)
  {
    LOG_MSG_ERROR("QcMapNatAlg is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }
  ret = QcMapMgr->CompareKernelVer(KERNEL_VERSION_4_9);
  if (ret == FALSE)
  {
    /*If Kernel Version is below 4.9*/
    switch ( timeout_type )
    {
      case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_generic_timeout", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_generic_timeout = timeout_value; //todo change timeouts to static
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
             "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_icmp_timeout", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_icmp_timeout = timeout_value;
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
             "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_tcp_established_timeout = timeout_value;
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
             "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout", timeout_value);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,
             "echo %d > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout_stream", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_udp_timeout = timeout_value;
        break;
      default:
        LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
    }
  }
  else
  {
    /*Default and if Kernel Version is >= 4.9*/
    switch ( timeout_type )
    {
      case QCMAP_MSGR_NAT_TIMEOUT_GENERIC_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/netfilter/nf_conntrack_generic_timeout", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_generic_timeout = timeout_value; //todo change timeouts to static
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_ICMP_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/netfilter/nf_conntrack_icmp_timeout", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_icmp_timeout = timeout_value;
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_TCP_ESTABLISHED_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_tcp_established_timeout = timeout_value;
        break;
      case QCMAP_MSGR_NAT_TIMEOUT_UDP_V01:
        snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/netfilter/nf_conntrack_udp_timeout", timeout_value);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/netfilter/nf_conntrack_udp_timeout_stream", timeout_value);
        QCMAP_NATALG::nat_config_common.nat_entry_udp_timeout = timeout_value;
        break;
      default:
        LOG_MSG_INFO1("Timeout Type:%d not supported.\n", timeout_type,0,0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
    }
  }
  ds_system_call(command, strlen(command));
  return true;
}


/*===========================================================================
  FUNCTION AddSNATEntryOnApps
==========================================================================*/
/*!
@brief
  Prepares  command based on the nat configuration to add SNAT
  entries to kernel and execute the same.

@parameters
  qcmap_cm_port_fwding_entry_conf_t *nat_entry

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
boolean QCMAP_NATALG::AddSNATEntryOnApps
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char tmp[MAX_COMMAND_STR_LEN];

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if (!QCMAP_Backhaul::enable_ipv4)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  /* Verify ARGs */
  if( nat_entry == NULL )
  {
    LOG_MSG_ERROR("NULL nat_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true ||
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true)
  {
    LOG_MSG_ERROR("Cannot apply snat entry in bridge mode",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  if (!QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return FALSE;
  }

  bzero(command,MAX_COMMAND_STR_LEN);
  bzero(tmp,MAX_COMMAND_STR_LEN);

  if( ( nat_config.nat_type == QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01 )||
      ( nat_config.nat_type == QCMAP_MSGR_NAT_FULL_CONE_NAT_V01 ))
  {
    /*The below rule will add SNAT rules after the first rule in PREROUTING chain
      which will make sure that ARC nat rule will be at the begining of the
      PREROUTING chain rules */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -t nat -I PREROUTING 2 -i %s ",devname);
  }
  else
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -I PREROUTING -i %s ",devname);
  }

  if ( nat_entry->port_fwding_protocol!=0 )
  {
    snprintf(tmp,MAX_COMMAND_STR_LEN," -p %d ",nat_entry->port_fwding_protocol);
    strlcat(command, tmp, sizeof (command));
  }

  if ( nat_entry->port_fwding_global_port!=0 )
  {
    bzero(tmp,MAX_COMMAND_STR_LEN);
    snprintf( tmp, MAX_COMMAND_STR_LEN," --dport %d ",nat_entry->port_fwding_global_port);
    strlcat(command, tmp, sizeof (command));
  }

  if ( nat_entry->port_fwding_private_ip!=0 )
  {
    strlcat(command, " -j DNAT ", sizeof (command));
    bzero(tmp,MAX_COMMAND_STR_LEN);
    addr.s_addr = htonl(nat_entry->port_fwding_private_ip);
    strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));
    snprintf(tmp, MAX_COMMAND_STR_LEN," --to-destination %s",tempIP);
    strlcat(command, tmp, sizeof (command));
    if ( nat_entry->port_fwding_private_port!=0 )
    {
      bzero(tmp,MAX_COMMAND_STR_LEN);
      snprintf( tmp, MAX_COMMAND_STR_LEN,":%d ",nat_entry->port_fwding_private_port);
      strlcat(command, tmp, sizeof (command));
    }
  }
  ds_system_call(command, strlen(command));
  return true;
}

/*===========================================================================
  FUNCTION DeleteSNATEntryOnApps
==========================================================================*/
/*!
@brief
  Prepares command based on the nat entry to be deleted.

@parameters
  qcmap_cm_port_fwding_entry_conf_t *nat_entry

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
boolean QCMAP_NATALG::DeleteSNATEntryOnApps
(
  qcmap_cm_port_fwding_entry_conf_t* nat_entry,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  char tmp[MAX_COMMAND_STR_LEN];
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (!QCMAP_Backhaul::enable_ipv4)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  /* Verify ARGs */
  if( nat_entry == NULL )
  {
    LOG_MSG_ERROR("NULL nat_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if(QcMapBackhaul->QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED &&
     !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
     !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
     !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() &&
     !QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4())
  {
    strlcpy(devname, QcMapBackhaul->wan_cfg.ipv4_interface, sizeof(devname));
  }
  else if (!QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    return false;
  }

  bzero(command,MAX_COMMAND_STR_LEN);
  bzero(tmp,MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN,
      "iptables -t nat -D PREROUTING -i %s ",devname);

  if( nat_entry->port_fwding_protocol!=0 )
  {
    snprintf(tmp,MAX_COMMAND_STR_LEN," -p %d ",nat_entry->port_fwding_protocol);
    strlcat(command, tmp, sizeof (command));
  }

  if( nat_entry->port_fwding_global_port !=0 )
  {
    bzero(tmp,MAX_COMMAND_STR_LEN);
    snprintf( tmp, MAX_COMMAND_STR_LEN," --dport %d ",nat_entry->port_fwding_global_port);
    strlcat(command, tmp, sizeof (command));
  }

  if( nat_entry->port_fwding_private_ip !=0 )
  {
    strlcat(command, " -j DNAT ", sizeof (command));
    bzero(tmp,MAX_COMMAND_STR_LEN);
    addr.s_addr = htonl(nat_entry->port_fwding_private_ip);
    strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));
    snprintf(tmp, MAX_COMMAND_STR_LEN," --to-destination %s",tempIP);
    strlcat(command, tmp, sizeof (command));
    if( nat_entry->port_fwding_private_port !=0 )
    {
      bzero(tmp,MAX_COMMAND_STR_LEN);
      snprintf( tmp, MAX_COMMAND_STR_LEN,":%d ",nat_entry->port_fwding_private_port);
      strlcat(command, tmp, sizeof (command));
    }
  }
  ds_system_call(command, strlen(command));
  return true;
}

/*===========================================================================
  FUNCTION AddDMZOnApps
==========================================================================*/
/*!
@brief
  Adds DMZ IP address. Adding DMZ ipaddress will allow incomming packets,
  which are not intended to be received by softap clients to be
  captured on DMZ interface.

@parameters
  uint32       dmzIP

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
boolean QCMAP_NATALG::AddDMZOnApps
(
  uint32 dmzIP,
 qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  int qcmap_cm_error;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (!QcMapMgr || !QCMAP_Backhaul::enable_ipv4)
  {
    LOG_MSG_ERROR("Backhaul not up",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  /* Verify DMZ IP */
  if( dmzIP == 0 )
  {
    LOG_MSG_ERROR("Invalid DMZ IP\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true ||
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true)
  {
    LOG_MSG_ERROR("Cannot add dmz in bridge mode",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* Convert IP to ASCII format */
  addr.s_addr = htonl(dmzIP);
  strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));
  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
      QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",
             ECM_IFACE, tempIP );
  }
  else if(QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",\
              QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
              tempIP );
  }
  else if(QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() &&
          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",\
              ETH_IFACE,
              tempIP );
  }
  else if(QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4() &&
          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",\
              BT_IFACE,
              tempIP );
  }
  else
  {
    if( QcMapBackhaul->QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED &&
        !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
        !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
        !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() &&
        !QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4())
    {
        strlcpy(devname, QcMapBackhaul->wan_cfg.ipv4_interface,sizeof(devname));
    }
    else if(QcMapBackhaul->QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle,QCMAP_MSGR_IP_FAMILY_V4_V01,\
        devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS)
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d", qcmap_cm_error,0,0);
      *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
      return false;
    }
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -A PREROUTING -i %s -j DNAT --to-destination %s",
              devname, tempIP );
  }
  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION AddDMZ
==========================================================================*/
/*!
@brief
  Configures DMZ ipaddress and updates the configuration file
  with the DMZ IP .

@parameters
  uint32       dmzIP

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
boolean QCMAP_NATALG::AddDMZ(uint32 dmz_ip, qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  in_addr addr;
  uint32 ip_addr;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  GetSetNATConfigFromXML(CONFIG_DMZ_IP, GET_VALUE, data, MAX_STRING_LENGTH);

  inet_aton(data, (in_addr *)&(ip_addr));
  ip_addr = ntohl(ip_addr);
  if (!ip_addr)
  {
    addr.s_addr = htonl(dmz_ip);
    GetSetNATConfigFromXML(CONFIG_DMZ_IP, SET_VALUE, inet_ntoa(addr), strlen(inet_ntoa(addr)));
  }
  else
  {
    LOG_MSG_ERROR("DMZ is already configured",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  //store DMZ IP
  nat_config.dmz_ip = dmz_ip;

  if(QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    if( !AddDMZOnApps(dmz_ip, qmi_err_num) )
    {
      if( *qmi_err_num != QMI_ERR_INTERFACE_NOT_FOUND_V01 )
      {
        LOG_MSG_ERROR("Add DMZ on Apps failed",0,0,0);
        return false;
      }
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION DeleteDMZOnApps
==========================================================================*/
/*!
@brief
  Ths is function will remove the DMZ ipaddress entry.

@parameters
  uint32       dmzIP

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
boolean QCMAP_NATALG::DeleteDMZOnApps
(
  uint32 dmzIP,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN], devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  struct in_addr addr;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  int qcmap_cm_error;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if(!QcMapMgr)
  {
    LOG_MSG_ERROR("QcMapMgr is NULL",0,0,0);
    return false;
  }

  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  /* Verify DMZ IP */
  if( dmzIP == 0 )
  {
    LOG_MSG_ERROR("Invalid DMZ IP\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  /* Convert IP to ASCII format */
  addr.s_addr = htonl(dmzIP);
  strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));

  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
      QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s", ECM_IFACE, tempIP );
  }
  else if (QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
           QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s",\
            QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
            tempIP );
  }
  else if(QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() &&
          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_ETHERNET)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
             "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s",\
              ETH_IFACE,
              tempIP );
  }
  else if(QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4() &&
          QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_BT)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,
             "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s",\
              BT_IFACE,
              tempIP );
  }
  else
  {
    if(QcMapBackhaul->QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED)
    {
      strlcpy(devname, QcMapBackhaul->wan_cfg.ipv4_interface, sizeof(devname));
    }
    else if (QcMapBackhaul->QcMapBackhaulWWAN->GetDeviceName(QcMapMgr->qcmap_cm_handle, QCMAP_MSGR_IP_FAMILY_V4_V01,\
         devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS)
    {
      LOG_MSG_ERROR("Couldn't get rmnet name. error %d\n", qcmap_cm_error,0,0);
      *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
      return false;
    }
    snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -D PREROUTING -i %s -j DNAT --to-destination %s",
              devname,
              tempIP );
  }

  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION DeleteDMZ
==========================================================================*/
/*!
@brief
  Will disable DMZ feature.

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
/*=========================================================================*/
boolean QCMAP_NATALG::DeleteDMZ(qmi_error_type_v01 *qmi_err_num)
{
  in_addr addr;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (QcMapBackhaul)
  {
    if(QcMapBackhaul->IsBackhaulRouterModeEnabled())
    {
      if ( !DeleteDMZOnApps(nat_config.dmz_ip, qmi_err_num) )
      {
        if(*qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01 ||
         *qmi_err_num == QMI_ERR_INVALID_ARG_V01)
        {
          return false;
         }
      }
    }
    nat_config.dmz_ip=0;

  }

  addr.s_addr = 0;
  GetSetNATConfigFromXML(CONFIG_DMZ_IP, SET_VALUE, inet_ntoa(addr), strlen(inet_ntoa(addr)));

  return true;
}

/*===========================================================================
  FUNCTION GetDMZ
==========================================================================*/
/*!
@brief
  Gets the DMZ ipaddress configured.

@parameters
  uint32_t *dmz_ip

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
boolean QCMAP_NATALG::GetDMZ(uint32_t *dmz_ip, qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if(QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    if ( nat_config.dmz_ip == 0 )
    {
      LOG_MSG_ERROR("\nDMZ IP not configured!!",0,0,0);
      *qmi_err_num = QMI_ERR_DISABLED_V01;
      return false;
    }
    *dmz_ip = nat_config.dmz_ip;
    return true;
  }

  GetSetNATConfigFromXML(CONFIG_DMZ_IP, GET_VALUE, data, MAX_STRING_LENGTH);
  inet_aton(data, (in_addr *)dmz_ip);
  *dmz_ip=ntohl(*dmz_ip);
  if (*dmz_ip == 0)
  {
    LOG_MSG_ERROR("\nDMZ IP not configured!!",0,0,0);
    *qmi_err_num = QMI_ERR_DISABLED_V01;
    return false;
  }

  return true;
}



/*===========================================================================
  FUNCTION EnableAlg
==========================================================================*/
/*!
@brief
  Enables Algs Functionality.

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
static boolean QCMAP_NATALG::EnableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  boolean ret_rtsp = false;
  boolean ret_sip = false;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  char data[MAX_STRING_LENGTH] = {0};
  boolean enable = TRUE;

  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  snprintf(data, sizeof(data), "%d", enable);

  if(QcMapNatAlg == NULL)
  {
    LOG_MSG_ERROR("QcMapNatAlg is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }
  if(alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
  {
    QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_RTSP_ALG, SET_VALUE, data, strlen(data));
  }
  else
  {
    QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SIP_ALG, SET_VALUE, data, strlen(data));
  }

  if (QcMapNatAlg)
  {
    if( !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() )
    {
      if ( QcMapBackhaulWWAN && QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED )
      {
        LOG_MSG_ERROR("IPv4 backhaul down:cannot enable ALG now, but will change cfg",
                        0, 0, 0);
        *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;

        if (alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_RTSP_ALG_V01 ;
        }
        else if (alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
        }
        return false;
      }
   }

    if( (alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01) ==
         QCMAP_MSGR_MASK_RTSP_ALG_V01 )
    {
      /*-----------------------------------------------------------------------
           Install RTSP ALG Kernel Module
          ----------------------------------------------------------------------*/
      ret_rtsp = EnableRTSPAlg(qmi_err_num);
      if ( ret_rtsp != true )
      {
        LOG_MSG_ERROR("Fail to enable RTSP ALG: = %d.\n",
                     ret_rtsp, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully enabled RTSP ALG", 0, 0, 0);
        // Set mask bit for RTSP ALG module enabled
        QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_RTSP_ALG_V01 ;
      }
    }

    if(alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
    {
      ret_sip = EnableSIPAlg(qmi_err_num);
      if ( ret_sip != true )
      {
        LOG_MSG_ERROR("Fail to enable SIP ALG, Ret value = %d",
                      ret_sip, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully enabled SIP ALG", 0, 0, 0);
        // Set mask for ALG module enabled
        QcMapNatAlg->nat_config.enable_alg_mask |= QCMAP_MSGR_MASK_SIP_ALG_V01;
      }
    }

     /*check if any of the requested ALG failed */
    if (((alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01) && (ret_rtsp == false)) ||
        ((alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01) && (ret_sip == false)))
    {
     LOG_MSG_ERROR("EnableAlg(): Failed for ret_rtsp:%d & ret_sip:%d", ret_rtsp, ret_sip, 0);
     /*check if it is a partial failure in case of multi*/
      if(((alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01) && (alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)) &&
           ret_sip != ret_rtsp)
      {
        *qmi_err_num = QMI_ERR_OP_PARTIAL_FAILURE_V01;
        return false;
      }
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
  }
  return true;
}


/*===========================================================================
  FUNCTION DisableAlg
==========================================================================*/
/*!
@brief
  Disables Algs Functionality.

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

boolean QCMAP_NATALG::DisableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_type,
  qmi_error_type_v01 *qmi_err_num
)
{
  boolean ret_rtsp = false;
  boolean ret_sip = false;
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  char data[MAX_STRING_LENGTH] = {0};
  boolean enable = false;
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapMgr->qcmap_enable)
  {
    /* QCMAP is not enabled */
    LOG_MSG_ERROR("QCMAP not enabled\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  snprintf(data, sizeof(data), "%d", enable);

  if(QcMapNatAlg == NULL)
  {
    LOG_MSG_ERROR("QcMapNatAlg is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if(alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
  {
    QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_RTSP_ALG, SET_VALUE, data, strlen(data));
  }
  else
  {
    QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SIP_ALG, SET_VALUE, data, strlen(data));
  }


  if(QcMapNatAlg)
  {
    if( !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() )
    {
      if ( QcMapBackhaulWWAN && QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED )
      {
        LOG_MSG_ERROR(" IPv4 backhaul down: cannot disable ALG", 0,0,0);
        *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;

        if (alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_RTSP_ALG_V01;
        }
        else if (alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
        {
          QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_SIP_ALG_V01;
        }
        return false;
      }
    }

    if(alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01)
    {
      /*-----------------------------------------------------------------------
             Unload the RTSP ALG module from Kernel
             -----------------------------------------------------------------------*/
      ret_rtsp = DisableRTSPAlg(qmi_err_num);
      if ( ret_rtsp != true )
      {
        LOG_MSG_ERROR("Fail to disable RTSP ALG. error = %d.\n",
                      ret_rtsp, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully disabled RTSP ALG", 0, 0, 0);
        // Reset mask bit for RTSP ALG module
        QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_RTSP_ALG_V01 ;
      }
    }

    if(alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)
    {
      ret_sip = DisableSIPAlg(qmi_err_num);
      if ( ret_sip != true )
      {
        LOG_MSG_ERROR("Fail to disable SIP ALG, Ret value = %d.\n",
                      ret_sip, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Successfully disabled SIP ALG", 0, 0, 0);
        // Reset mask bit for SIP ALG module
        QcMapNatAlg->nat_config.enable_alg_mask &= ~QCMAP_MSGR_MASK_SIP_ALG_V01 ;
      }
    }

    /*check if any of the requested ALG failed */
    if (((alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01) && (ret_rtsp == false)) ||
        ((alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01) && (ret_sip == false)))
    {
      LOG_MSG_ERROR("DisableAlg(): Failed for ret_rtsp:%d & ret_sip:%d", ret_rtsp, ret_sip, 0);
     /*check if it is a partial failure in case of multi*/
      if(((alg_type & QCMAP_MSGR_MASK_RTSP_ALG_V01) && (alg_type & QCMAP_MSGR_MASK_SIP_ALG_V01)) &&
           ret_sip != ret_rtsp)
      {
        *qmi_err_num = QMI_ERR_OP_PARTIAL_FAILURE_V01;
        return false;
      }
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
  }
  return true;
}
/*===========================================================================
  FUNCTION EnableRTSPAlg
==========================================================================*/
/*!
@brief
  Enables RTSP Alg Functionality.

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
static boolean QCMAP_NATALG::EnableRTSPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  int enable = TRUE;
  char Kernel_ver[KERNEL_VERSION_LENGTH] = {0};
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (QcMapNatAlg)
  {
    snprintf(data, sizeof(data), "%d", enable);
    QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_RTSP_ALG, SET_VALUE , data, strlen(data));
    if ((QcMapNatAlg->alg_enabled_disabled_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01) ==
        QCMAP_MSGR_MASK_RTSP_ALG_V01)
    {
      LOG_MSG_INFO1("RTSP ALG is already enabled.\n", 0, 0, 0);
      return true;
    }
    else
    {
      if ( false == QcMapMgr->GetKernelVer(Kernel_ver) )
      {
        LOG_MSG_ERROR("Unable to get the kernel version info", 0, 0, 0);
        *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
        return false;
      }
      /*-----------------------------------------------------------------------
          Install RTSP ALG Kernel Module
       -----------------------------------------------------------------------*/
      snprintf(command,MAX_COMMAND_STR_LEN, "insmod /usr/lib/modules/%s/kernel/drivers/net/nf_conntrack_rtsp.ko",Kernel_ver);
      ds_system_call(command, strlen(command));

      snprintf(command,MAX_COMMAND_STR_LEN, "insmod /usr/lib/modules/%s/kernel/drivers/net/nf_nat_rtsp.ko",Kernel_ver);
      ds_system_call(command, strlen(command));

      QcMapNatAlg->alg_enabled_disabled_mask = QcMapNatAlg->alg_enabled_disabled_mask |
                                        QCMAP_MSGR_MASK_RTSP_ALG_V01;
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION DisableRTSPAlg
==========================================================================*/
/*!
@brief
  Disables RTSP Alg Functionality.

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

boolean QCMAP_NATALG::DisableRTSPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  int enable = false;
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();

  snprintf(data, sizeof(data), "%d", enable);

  if (QcMapNatAlg)
  {
    if ((QcMapNatAlg->alg_enabled_disabled_mask & QCMAP_MSGR_MASK_RTSP_ALG_V01) ==
         QCMAP_MSGR_MASK_RTSP_ALG_V01)
    {
      snprintf(command,MAX_COMMAND_STR_LEN, "rmmod  nf_nat_rtsp");
      ds_system_call(command, strlen(command));

      snprintf(command,MAX_COMMAND_STR_LEN, "rmmod nf_conntrack_rtsp");
      ds_system_call(command, strlen(command));

      QcMapNatAlg->alg_enabled_disabled_mask = (QcMapNatAlg->alg_enabled_disabled_mask &
                                       ~(QCMAP_MSGR_MASK_RTSP_ALG_V01));
    }
    else
    {
      LOG_MSG_INFO1("RTSP ALG is already disabled.\n", 0, 0, 0);
      return true;
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION EnableSIPAlg
==========================================================================*/
/*!
@brief
  - Enables SIP Alg Functionality.
  - Writes to a proc/sys entry which indicates netfilter to enable SIP ALG
    processing (corresponding code added in kernel)

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
static boolean QCMAP_NATALG::EnableSIPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};
  int enable = TRUE;
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();

  if (QcMapNatAlg)
  {
    snprintf(data, sizeof(data), "%d", enable);
    QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SIP_ALG, SET_VALUE, data, strlen(data));
    ds_system_call("echo 0 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg",
                   strlen("echo 0 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg"));

    /*Enable media streaming between non signalling endpoints */
    ds_system_call("echo 0 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media",
                   strlen("echo 0 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media"));

    QcMapNatAlg->alg_enabled_disabled_mask = QcMapNatAlg->alg_enabled_disabled_mask |
                                      QCMAP_MSGR_MASK_SIP_ALG_V01;
  }
  return true;
}


/*===========================================================================
  FUNCTION DisableSIPAlg
==========================================================================*/
/*!
@brief
  - Disables SIP Alg Functionality.
  - Writes to a proc/sys entry which indicates netfilter to bypass SIP ALG
    processing (corresponding code added in kernel)

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

boolean QCMAP_NATALG::DisableSIPAlg
(
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_NATALG* QcMapNatAlg=GET_DEFAULT_NATALG_OBJECT();

  ds_system_call("echo 1 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg",
                 strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_disable_sip_alg"));

  /*Disable media streaming between non signalling endpoints */
  ds_system_call("echo 1 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media",
                 strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_sip_direct_media"));

  LOG_MSG_INFO1("SIP ALG disabled", 0, 0, 0);
  if(QcMapNatAlg != NULL)
    QcMapNatAlg->alg_enabled_disabled_mask = (QcMapNatAlg->alg_enabled_disabled_mask &
                                     ~(QCMAP_MSGR_MASK_SIP_ALG_V01));
  else
  {
    LOG_MSG_ERROR("QcMapNatAlg is NULL ", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION SetInitialPacketThreshold
==========================================================================*/
/*!
@brief
  Set the packet threshold count to delay the time for the initial
  packets to flow through HW path. Until the packet threshold is reached
  packets will take the software path

@parameters
  uint32                          packet_count

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
boolean QCMAP_NATALG::SetInitialPacketThreshold
(
  uint32 packet_count,
  qmi_error_type_v01 *qmi_err_num
)
{

  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_NATALG* QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT();
  char command[MAX_COMMAND_STR_LEN];

  if (packet_count < QCMAP_MIN_INITIAL_PKT_THRESHOLD)
  {
    LOG_MSG_ERROR("Packet threshold value should be greater than: %d Got: %d.\n",
                                QCMAP_NAT_ENTRY_MIN_TIMEOUT, packet_count, 0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }

  if(QcMapNatAlg)
  {
    QCMAP_NATALG::nat_config_common.initial_pkt_limit= packet_count;

    snprintf(data, sizeof(data), "%d", packet_count);

    QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_PKT_LIMIT, SET_VALUE, data, strlen(data));
    ds_system_call("echo 1 > /proc/sys/net/netfilter/nf_conntrack_acct",
                strlen("echo 1 > /proc/sys/net/netfilter/nf_conntrack_acct"));

    snprintf( command, MAX_COMMAND_STR_LEN,
            "echo %d > /proc/sys/net/netfilter/nf_conntrack_pkt_threshold", packet_count);
    ds_system_call(command, strlen(command));
  }
  return true;
}

/*===========================================================================
  FUNCTION GetInitialPacketThreshold
==========================================================================*/
/*!
@brief
  Get the packet threshold count to delay the time for the initial
  packets to flow through HW path. Until the packet threshold is reached
  packets will take the software path

@parameters
  uint32                          packet_count

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
boolean QCMAP_NATALG::GetInitialPacketThreshold
(
  uint32 *packet_count,
  qmi_error_type_v01 *qmi_err_num
)
{

  FILE *fp = NULL;
  char command[MAX_COMMAND_STR_LEN];

  fp = fopen("/proc/sys/net/netfilter/nf_conntrack_pkt_threshold", "r");
  if ( fp == NULL )
  {
     LOG_MSG_ERROR("Error opening threshold file.\n", 0,0,0);
     *qmi_err_num = QMI_ERR_INTERNAL_V01;
     return false;
  }

  if (fscanf(fp, "%d", packet_count) != 1) {
    LOG_MSG_ERROR("Error reading timeout file: %d.\n", *packet_count,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  LOG_MSG_INFO1("Packet Count: %d \n", *packet_count, 0,0);
  fclose(fp);

  return true;

}

/*===========================================================================
  FUNCTION cleanIPv4FilterTableFwdChain
==========================================================================*/
/*!
@brief
  Deleted all the ipv4 forward chain rules added.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_NATALG::CleanIPv4FilterTableFwdChain()
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

  bzero(command, MAX_COMMAND_STR_LEN);
  bzero(devname, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if (QcMapBackhaul == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapBackhaul is NULL", 0,0,0);
    return false;
  }

  /* Drop unstatefull tcp packets */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -t filter -D FORWARD -i %s -p tcp -m state --state INVALID -j DROP",
            GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id) );
  ds_system_call(command, strlen(command));

  if (QcMapBackhaul)
  {
    strlcpy(devname,QcMapBackhaul->wan_cfg.ipv4_interface,sizeof(devname));
    /* Configure VPN PassThrough. */
    /* Delete the existing rule, only if it was added .*/
    if ( !nat_config.enable_l2tp_vpn_pass_through )
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -p esp -i %s -j %s", devname, "DROP" );
      ds_system_call(command, strlen(command));
    }

    if ( !nat_config.enable_ipsec_vpn_pass_through )
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -p esp -i %s -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
    }

    if ( !nat_config.enable_pptp_vpn_pass_through )
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -p gre -i %s -j %s", devname,"DROP" );
      ds_system_call(command, strlen(command));
    }
  }

  /*===========================================================================
  Allow access between Bridge and Bridge.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
            ALL_BRIDGES);
  ds_system_call(command, strlen(command));


/*===========================================================================
  Allow access between Bridge and PPP.
==========================================================================*/
  if (IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle))
  {
    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -D FORWARD -i %s -o %s -j ACCEPT", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
              PPP_IFACE);
    ds_system_call(command, strlen(command));


    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE,
              GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id));
    ds_system_call(command, strlen(command));
  }


/*===========================================================================
  Block WWAN access to MobileAP clients.
==========================================================================*/
  /* First delete duplicate rule if any. */
  if (QcMapBackhaulWLANMgr && IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle))
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s ! -o %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
             QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s ! -o %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
            ECM_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s ! -o %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
            ETH_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s ! -o %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
           GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id));
  ds_system_call(command, strlen(command));

  /* First delete rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id));
  ds_system_call(command, strlen(command));

/*===========================================================================
  Block WWAN access to PPP client.
==========================================================================*/
  /* First delete duplicate rule if any. */
  if ( IS_DEFAULT_PROFILE(QcMapBackhaul->profileHandle))
  {
    if (QcMapBackhaulWLANMgr)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "iptables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
               QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
      ds_system_call(command, strlen(command));
    }
    snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
              ECM_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
              ETH_IFACE);
    ds_system_call(command, strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id));
    ds_system_call(command, strlen(command));

    /* First delete rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));
  }
  return true ;
}

/*===========================================================================
  FUNCTION cleanIPv4FilterTableInChain
==========================================================================*/
/*!
@brief
  Deleted all the ipv4 Input tabel rules added.

@parameters
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::CleanIPv4FilterTableInChain()
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];

  bzero(command, MAX_COMMAND_STR_LEN);
  bzero(devname, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  if (QcMapBackhaul)
  {
    #ifndef FEATURE_QTIMAP_OFFTARGET
      strlcpy(devname,QcMapBackhaul->wan_cfg.ipv4_interface,sizeof(devname));
      /* Delete the existing rule, only if it was added .*/
      snprintf( command, MAX_COMMAND_STR_LEN,
                  "iptables -t filter -D INPUT -i %s -p tcp --dport 80 -j DROP", devname);
      ds_system_call(command, strlen(command));
      LOG_MSG_INFO1("%s\n", command,0,0);
      snprintf( command, MAX_COMMAND_STR_LEN,
                  "iptables -t filter -D INPUT -i %s -p tcp --dport 443 -j DROP", devname);
      ds_system_call(command, strlen(command));
      LOG_MSG_INFO1("%s\n", command,0,0);
    #endif
    return true ;
  }

  return false;
}

/*===========================================================================
  FUNCTION cleanIPv4NatTable
==========================================================================*/
/*!
@brief
  Deleted all the ipv4 nat table rules added.

@parameters

@return
  false - fail
  true  - succeed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_NATALG::CleanIPv4NatTable()
{
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  int i=0;
  ds_dll_el_t * node;
  pugi::xml_document xml_file;
  pugi::xml_node parent, child;
  qcmap_cm_port_fwding_entry_conf_t* nat_entry;

  bzero(command, MAX_COMMAND_STR_LEN);
  bzero(devname, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);

  qcmap_nat_entry_list_t* natList = &(nat_config.port_fwding_entries);

  memset(publicIpAddr, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  addr.s_addr = QcMapBackhaul->ipv4_public_ip;
  strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  strlcpy(devname,QcMapBackhaul->wan_cfg.ipv4_interface,sizeof(devname));
   /* Delete SNAT entries */
  switch (nat_config.nat_type)
  {
     case QCMAP_MSGR_NAT_FULL_CONE_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE --random", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D PREROUTING -i %s -j NATTYPE --mode dnat --type 1", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j NATTYPE --mode forward_out --type 1", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
     case QCMAP_MSGR_NAT_SYMMETRIC_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE --random", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
     case QCMAP_MSGR_NAT_PORT_RESTRICTED_CONE_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j SNAT --to-source %s",
                 devname, publicIpAddr);
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
     case QCMAP_MSGR_NAT_ADDRESS_RESTRICTED_NAT_V01:
     {
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE --random", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D PREROUTING -i %s -j NATTYPE --mode dnat --type 2", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       snprintf( command, MAX_COMMAND_STR_LEN,
                 "iptables -t nat -D POSTROUTING -o %s -j NATTYPE --mode forward_out --type 2", devname );
       ds_system_call(command, strlen(command));
       LOG_MSG_INFO1("%s\n", command,0,0);
       break;
     }
  }

  if (natList->natEntryListHead)
  {
    node = ds_dll_next(natList->natEntryListHead, (&nat_entry));
    if (node == NULL)
    {
      LOG_MSG_ERROR("HeadNode is empty",0,0,0);
      return false;
    }

    for( i = 0; i < nat_config.num_port_fwding_entries; i++ )
    {
      if ( nat_entry->port_fwding_protocol != PS_IPPROTO_TCP_UDP )
      {
        if (!DeleteSNATEntryOnApps(nat_entry, &qmi_err_num))
          return false;
      }
      else
      {
        nat_entry->port_fwding_protocol = PS_IPPROTO_TCP;
        if (!DeleteSNATEntryOnApps(nat_entry, &qmi_err_num))
          return false;

        nat_entry->port_fwding_protocol = PS_IPPROTO_UDP;

        if (!DeleteSNATEntryOnApps(nat_entry, &qmi_err_num))
          return false;

      }
      node = ds_dll_next(node, (&nat_entry));
    }
  }

  /* Config DMZ IP.*/
  if( nat_config.dmz_ip != 0 )
  {
    DeleteDMZOnApps(nat_config.dmz_ip, &qmi_err_num);
  }

  return true ;
}

/*===========================================================================
  FUNCTION CleanIPv6FilterTableFwdChain
==========================================================================*/
/*!
@brief
  Deleted all the ipv6 forward table  rules added.

@parameters
  true  - success
  false - fail
@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::CleanIPv6FilterTableFwdChain()
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

  bzero(command, MAX_COMMAND_STR_LEN);

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
            ALL_BRIDGES);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id), PPP_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id));
  ds_system_call(command, strlen(command));

  if (QcMapBackhaulWLANMgr)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id),
             QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s ! -o %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id), ECM_IFACE);
  ds_system_call(command, strlen(command));

  if (QcMapBackhaulWLANMgr)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE, ECM_IFACE);
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s -j DROP", GET_ASSOCIATED_BRIDGE_IFACE_NAME(QcMapBackhaul->vlan_id));
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s -j DROP", PPP_IFACE);
  ds_system_call(command, strlen(command));

  return true ;
}

/*===========================================================================
  FUNCTION GetSOCKSv5ProxyEnableCfg
==========================================================================*/
/*!
@brief
  Gets socksv5 proxy status

@parameters

@return
  0 - disabled
  otherwise - enabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static unsigned char QCMAP_NATALG::GetSOCKSv5ProxyEnableCfg(qmi_error_type_v01 *qmi_err_num)
{
  char data[2];
  QCMAP_NATALG* QcMapNatAlg = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return 0;
  }

  if((QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT()) == NULL)
  {
    LOG_MSG_INFO1("Default NATALG object NULL", 0, 0, 0);
    return 0;
  }

  if(!(QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SOCKSv5_PROXY, GET_VALUE, data, sizeof(data))))
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
  } else
  {
    *qmi_err_num = QMI_ERR_NONE_V01;
  }

  return atoi(data);
}

/*===========================================================================
  FUNCTION EnableSOCKSv5Proxy
==========================================================================*/
/*!
@brief
  Enable socksv5 proxy

@parameters
  qmi_err_num  - error no.

@return
  true  - enabled
  false - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static boolean QCMAP_NATALG::EnableSOCKSv5Proxy(qmi_error_type_v01 *qmi_err_num)
{
  char* status = "0";
  qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
  QCMAP_NATALG* QcMapNatAlg = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return false;
  }

  if((QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT()) == NULL)
  {
    LOG_MSG_INFO1("Default NATALG object NULL", 0, 0, 0);
    return false;
  }

  /*kill socksv5 proxy */
  if(!KillSOCKSv5Proxy())
  {
    status = "0";
    goto set_socksv5_status_xml;
  }

  //find out where are configuration files
  if(!QcMapNatAlg->GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
  {
    LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  /* invoke socksv5 proxy */
  if(InvokeSOCKSv5Proxy(&socksv5_file_path))
  {
    status = "1";
  } else
  {
    status = "0";
  }

  set_socksv5_status_xml:

    if(!QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SOCKSv5_PROXY, SET_VALUE,
       status, strlen(status)) || (strcmp(status, "0") == 0))
    {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
}

/*===========================================================================
  FUNCTION DisableSOCKSv5Proxy
==========================================================================*/
/*!
@brief
  Disable SOCKSv5 proxy

@parameters
  qmi_err_num  - error no.

@return
  false - enabled
  true  - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static boolean QCMAP_NATALG::DisableSOCKSv5Proxy(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_NATALG* QcMapNatAlg = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    return false;
  }

  if((QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT()) == NULL)
  {
    LOG_MSG_INFO1("Default NATALG object NULL", 0, 0, 0);
    return false;
  }

  /*kill socksv5 proxy */
  if(!KillSOCKSv5Proxy())
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  } else if(!QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SOCKSv5_PROXY, SET_VALUE, "0",
                                                  strlen("0")))
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*===========================================================================
  FUNCTION InvokeSOCKSv5Proxy
==========================================================================*/
/*!
@brief
  Invoke SOCKSv5 proxy

@parameters
  qmi_err_num  - error no.

@return
  false - enabled
  true  - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::InvokeSOCKSv5Proxy
(
qcmap_msgr_socksv5_config_file_paths_v01 *socksv5_file_path
)
{
  char buffer[2];
  char* invoke_cmd = NULL;
  const char* base_invoke = "insmod /lib/modules/$(uname -r)/kernel/drivers/net/tcp_splice.ko ;"
                            " qti_socksv5 -c ";
  const char* check_cmd = "ps -aef | grep qti_socksv5 | grep -v grep";
  FILE* fp = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    return false;
  }

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(NULL == socksv5_file_path)
  {
    LOG_MSG_INFO1("socksv5_file_path NULL", 0, 0, 0);
    return false;
  }

  if((invoke_cmd = (char*)calloc(1, strlen(base_invoke) + strlen(socksv5_file_path->conf_file) +
                                    strlen(socksv5_file_path->auth_file) + strlen(" -u ") +
                                    strlen(" &") + 1)) == NULL)
  {
    LOG_MSG_ERROR("Error with calloc: %s", strerror(errno), 0, 0);
    return false;
  }

  snprintf(invoke_cmd, strlen(base_invoke) + strlen(socksv5_file_path->conf_file) +
                       strlen(socksv5_file_path->auth_file) + strlen(" -u ") + strlen(" &") + 1,
                       "insmod /lib/modules/$(uname -r)/kernel/drivers/net/tcp_splice.ko ;"
                       " qti_socksv5 -c %s -u %s &", socksv5_file_path->conf_file,
                       socksv5_file_path->auth_file);

  LOG_MSG_INFO1("SOCKSv5 Invoke Cmd: %s", invoke_cmd, 0, 0);

  if((fp = popen(invoke_cmd, "w")) == NULL)
  {
    LOG_MSG_INFO1("Error invoking SOCKSv5 Proxy: ", errno, 0, 0);
    free(invoke_cmd);
    return false;
  }
  pclose(fp);

  //wait for SOCKSv5 proxy to run/parse
  sleep(1);

  //ps grep for the socksv5 process
  if((fp = popen(check_cmd, "r")) == NULL)
  {
    LOG_MSG_INFO1("Error checking SOCKSv5 Proxy: ", errno, 0, 0);
    free(invoke_cmd);
    return false;
  }

  //if there was an STDOUT from ps grep, then qti_socksv5 is running
  if(fgets(buffer, sizeof(buffer), fp))
  {
    pclose(fp);
    free(invoke_cmd);
    return true;
  }
  pclose(fp);

  free(invoke_cmd);
  return false;
}

/*===========================================================================
  FUNCTION KillSOCKSv5Proxy
==========================================================================*/
/*!
@brief
  Kill SOCKSv5 proxy

@parameters
  qmi_err_num  - error no.

@return
  false - enabled
  true  - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::KillSOCKSv5Proxy(void)
{
  char buffer;
  const char* kill_cmd = "killall -s 15 qti_socksv5; sleep 1; rmmod tcp_splice";
  FILE* fp;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    return false;
  }

  QCMAP_CM_LOG_FUNC_ENTRY();

  if((fp = popen(kill_cmd, "w")) == NULL)
  {
    LOG_MSG_INFO1("Error killing SOCKSv5 Proxy: ", errno, 0, 0);
    return false;
  }
  pclose(fp);

  return true;
}

/*===========================================================================
  FUNCTION GetSOCKSv5ConfigFilePathFromXML
==========================================================================*/
/*!
@brief
  Gets configuration file paths for SOCKSv5

@parameters
  qmi_err_num  - error no.

@return
  false - enabled
  true  - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static boolean QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML
(
qcmap_msgr_socksv5_config_file_paths_v01 *config_file_path
)
{
  char data[MAX_STRING_LENGTH];
  QCMAP_NATALG* QcMapNatAlg = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    return false;
  }

  if((QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT()) == NULL)
  {
    LOG_MSG_INFO1("Default NATALG object NULL", 0, 0, 0);
    return false;
  }

  memset(config_file_path, 0, sizeof(qcmap_msgr_socksv5_config_file_paths_v01));

  memset(data, 0, MAX_STRING_LENGTH * sizeof(char));
  if(!QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SOCKSv5_CONF_FILE, GET_VALUE, data,
                                       MAX_STRING_LENGTH))
  {
    LOG_MSG_INFO1("Error getting socksv5 conf file",0,0,0);
    return false;
  }

  if(sizeof(config_file_path->conf_file) <= strlen(data))
  {
    LOG_MSG_INFO1("Preventing buffer overflow: %d <= %d\n",
                  sizeof(config_file_path->conf_file), strlen(data), 0);
    return false;
  }
  memcpy(config_file_path->conf_file, data, strlen(data));

  memset(data, 0, MAX_STRING_LENGTH * sizeof(char));
  if(!QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SOCKSv5_AUTH_FILE, GET_VALUE, data,
                                       MAX_STRING_LENGTH))
  {
    LOG_MSG_INFO1("Error getting socksv5 auth file",0,0,0);
    return false;
  }

  if(sizeof(config_file_path->auth_file) <= strlen(data))
  {
    LOG_MSG_INFO1("Preventing buffer overflow: %d <= %d\n",
                  sizeof(config_file_path->auth_file), strlen(data), 0);
    return false;
  }
  memcpy(config_file_path->auth_file, data, strlen(data));

  return true;
}

/*===========================================================================
  FUNCTION SetSOCKSv5ConfigFilePath
==========================================================================*/
/*!
@brief
  Sets configuration file paths for SOCKSv5

@parameters
  qmi_err_num  - error no.

@return
  false - enabled
  true  - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static boolean QCMAP_NATALG::SetSOCKSv5ConfigFilePath(const char *conf_file, const char *auth_file)
{
  char data[MAX_STRING_LENGTH];
  QCMAP_NATALG* QcMapNatAlg = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    return false;
  }

  if((QcMapNatAlg = GET_DEFAULT_NATALG_OBJECT()) == NULL)
  {
    LOG_MSG_INFO1("Default NATALG object NULL", 0, 0, 0);
    return false;
  }

  memset(data, 0, MAX_STRING_LENGTH * sizeof(char));

  if(sizeof(data) <= strlen(conf_file))
  {
    LOG_MSG_INFO1("Preventing buffer overflow: %d <= %d\n",
                  sizeof(data), strlen(conf_file), 0);
    return false;
  }
  memcpy(data, conf_file, strlen(conf_file));
  if(!QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SOCKSv5_CONF_FILE, SET_VALUE, data,
                                       MAX_STRING_LENGTH))
  {
    LOG_MSG_INFO1("Error setting socksv5 conf file",0,0,0);
    return false;
  }

  memset(data, 0, MAX_STRING_LENGTH * sizeof(char));
  if(sizeof(data) <= strlen(auth_file))
  {
    LOG_MSG_INFO1("Preventing buffer overflow: %d <= %d\n",
                  sizeof(data), strlen(auth_file), 0);
    return false;
  }
  memcpy(data, auth_file, strlen(auth_file));
  if(!QcMapNatAlg->GetSetNATConfigFromXML(CONFIG_SOCKSv5_AUTH_FILE, SET_VALUE, data,
                                       MAX_STRING_LENGTH))
  {
    LOG_MSG_INFO1("Error setting socksv5 auth file",0,0,0);
    return false;
  }

  return true;

}

/*===========================================================================
  FUNCTION SetSOCKSv5Backhaul
==========================================================================*/
/*!
@brief
  Sets Backhaul in SOCKSv5 proxy conf

@parameters
  qmi_err_num  - error no.

@return
  false - enabled
  true  - disabled

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::SetSOCKSv5Backhaul
(
unsigned int service_no,
int wan_ip_ver,
const char* wan_iface,
const char* pri_dns_ip_addr,
const char* sec_dns_ip_addr
)
{
  pugi::xml_document doc;
  pugi::xml_node node, child, temp_node;
  boolean set_wan_iface = false;
  boolean set_pri_dns_ip_addr = false;
  boolean set_sec_dns_ip_addr = false;
  boolean set_wan_ip_ver = false;
  qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char command[MAX_COMMAND_STR_LEN] = {0};

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    return false;
  }

  if((IPV4 != wan_ip_ver) && (IPV6 != wan_ip_ver))
  {
    LOG_MSG_INFO1("%d is invalid wan IP version\n", wan_ip_ver, 0, 0);
    return false;
  } else if(NULL == wan_iface)
  {
    LOG_MSG_INFO1("wan iface NULL\n", 0, 0, 0);
    return false;
  } else if(strlen(wan_iface) > IFNAMSIZ || !(strlen(wan_iface))) //check wan_iface is valid len
  {
    LOG_MSG_INFO1("%s is invalid wan iface name length\n", wan_iface, 0, 0);
    return false;
  } else if(NULL == pri_dns_ip_addr)
  {
    LOG_MSG_INFO1("pri_dns_ip_addr NULL\n", 0, 0, 0);
    return false;
  } else if(NULL == sec_dns_ip_addr)
  {
    LOG_MSG_INFO1("sec_dns_ip_addr NULL\n", 0, 0, 0);
    return false;
  }

  //find out where are configuration files
  if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
  {
    LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
    return false;
  }

  if(!doc.load_file(socksv5_file_path.conf_file))
  {
    LOG_MSG_INFO1("Error loading the conf file: %s\n", socksv5_file_path.conf_file,0,0);
    return false;
  }

  //extract wan_iface
  node = doc.child("PROXY_CONFIG").child("WAN_CFG");

  //delete all previous associations of the wan iface
  for(child = node.first_child(); child != NULL; child = child.next_sibling())
  {
    if(strcmp(child.name(), "WAN_IFACE") == 0)
    {
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        //find the profile to update with iface name
        if((strcmp(grand_child.name(), "NAME") == 0) &&
           (strcmp(grand_child.child_value(), wan_iface) == 0))
        {
          grand_child.text().set("");
        }
      }
    }
  }

  //go through each wan iface and find the profile id to update
  for(child = node.first_child(); child != NULL; child = child.next_sibling())
  {
    if(strcmp(child.name(), "WAN_IFACE") == 0)
    {
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        //find the profile to update with iface name
        if((strcmp(grand_child.name(), "PROFILE") == 0) &&
           (atoi(grand_child.child_value()) == service_no))
        {
          temp_node = child;

          //set this WAN iface's name
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "NAME") == 0)
            {
              temp_node.text().set(wan_iface);
              set_wan_iface = true;
            }
          }

          //there is not an associated NAME yet
          if(!set_wan_iface)
          {
            temp_node = child.append_child("NAME");
            temp_node.text().set(wan_iface);
            set_wan_iface = true;
          }

          switch(wan_ip_ver)
          {
            case(IPV4):
            {
              //set this WAN iface's primary dns server ip
              for(temp_node = child.first_child(); temp_node != NULL;
                  temp_node = temp_node.next_sibling())
              {
                if(strlen(pri_dns_ip_addr) &&
                   (strcmp(temp_node.name(), "PRI_DNS_IPV4_ADDR") == 0))
                {
                  temp_node.text().set(pri_dns_ip_addr);
                  set_pri_dns_ip_addr = true;
                }
              }

              //there is not an associated PRI DNS IP yet
              if(strlen(pri_dns_ip_addr) && !(set_pri_dns_ip_addr))
              {
                temp_node = child.append_child("PRI_DNS_IPV4_ADDR");
                temp_node.text().set(pri_dns_ip_addr);
                set_pri_dns_ip_addr = true;
              }

              //set this WAN iface's secondary dns server ip
              for(temp_node = child.first_child(); temp_node != NULL;
                  temp_node = temp_node.next_sibling())
              {
                if(strlen(sec_dns_ip_addr) &&
                   (strcmp(temp_node.name(), "SEC_DNS_IPV4_ADDR") == 0))
                {
                  temp_node.text().set(sec_dns_ip_addr);
                  set_sec_dns_ip_addr = true;
                }
              }

              //there is not an associated SEC DNS IP yet
              if(strlen(sec_dns_ip_addr) && (!set_sec_dns_ip_addr))
              {
                temp_node = child.append_child("SEC_DNS_IPV4_ADDR");
                temp_node.text().set(sec_dns_ip_addr);
                set_sec_dns_ip_addr = true;
              }
              break;
            }
            case(IPV6):
            {
              //set this WAN iface's primary dns server ip
              for(temp_node = child.first_child(); temp_node != NULL;
                  temp_node = temp_node.next_sibling())
              {
                if(strlen(pri_dns_ip_addr) &&
                   (strcmp(temp_node.name(), "PRI_DNS_IPV6_ADDR") == 0))
                {
                  temp_node.text().set(pri_dns_ip_addr);
                  set_pri_dns_ip_addr = true;
                }
              }
              //there is not an associated PRI DNS IP yet
              if(strlen(pri_dns_ip_addr) && (!set_pri_dns_ip_addr))
              {
                temp_node = child.append_child("PRI_DNS_IPV6_ADDR");
                temp_node.text().set(pri_dns_ip_addr);
                set_pri_dns_ip_addr = true;
              }

              //set this WAN iface's secondary dns server ip
              for(temp_node = child.first_child(); temp_node != NULL;
                  temp_node = temp_node.next_sibling())
              {
                if(strlen(sec_dns_ip_addr) &&
                   (strcmp(temp_node.name(), "SEC_DNS_IPV6_ADDR") == 0))
                {
                  temp_node.text().set(sec_dns_ip_addr);
                  set_sec_dns_ip_addr = true;
                }
              }

              //there is not an associated SEC DNS IP yet
              if(strlen(sec_dns_ip_addr) && (!set_sec_dns_ip_addr))
              {
                temp_node = child.append_child("SEC_DNS_IPV6_ADDR");
                temp_node.text().set(sec_dns_ip_addr);
                set_sec_dns_ip_addr = true;
              }
              break;
            }
            default:
            {
              LOG_MSG_INFO1("Invalid WAN IP Ver: %d", wan_ip_ver, 0, 0);
              break;
            }
          }

          //set this WAN iface's ip version
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "IP") == 0)
            {
              switch(wan_ip_ver)
              {
                case(IPV4):
                {
                  if((strcmp(temp_node.child_value(), "IPV6") == 0) ||
                     (strcmp(temp_node.child_value(), "IPV4V6") == 0))
                  {
                    temp_node.text().set("IPV4V6");
                  } else {
                    temp_node.text().set("IPV4");
                  }

                  set_wan_ip_ver = true;
                  break;
                }
                case(IPV6):
                {
                  if((strcmp(temp_node.child_value(), "IPV4") == 0) ||
                     (strcmp(temp_node.child_value(), "IPV4V6") == 0))
                  {
                    temp_node.text().set("IPV4V6");
                  } else {
                    temp_node.text().set("IPV6");
                  }
                  set_wan_ip_ver = true;
                  break;
                }
                default:
                {
                  temp_node.text().set("");
                  set_wan_ip_ver = true;
                  break;
                }
              }
            }
          }
          //there is not an associated IP version yet
          if(!set_wan_ip_ver)
          {
            temp_node = child.append_child("IP");
            switch(wan_ip_ver)
            {
              case(IPV4):
              {
                temp_node.text().set("IPV4");
                set_wan_ip_ver = true;
                break;
              }
              case(IPV6):
              {
                temp_node.text().set("IPV6");
                set_wan_ip_ver = true;
                break;
              }
              default:
              {
                temp_node.text().set("");
                set_wan_ip_ver = true;
                break;
              }
            }
            set_wan_ip_ver = true;
          }
        }
      }
    }
  }

  if(set_wan_iface || set_pri_dns_ip_addr || set_sec_dns_ip_addr || set_wan_ip_ver)
  {
    doc.save_file(TEMP_SOCKSV5_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_SOCKSV5_CFG ,
              socksv5_file_path.conf_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", socksv5_file_path.conf_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));


    return true;
  }

  LOG_MSG_INFO1("Profile Number not provisioned for SOCKSv5: %d", service_no, 0 , 0);

  //still save file if we deleted previous associations of the profile index
  doc.save_file(TEMP_SOCKSV5_CFG);
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", TEMP_SOCKSV5_CFG);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_SOCKSV5_CFG ,
            socksv5_file_path.conf_file);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", socksv5_file_path.conf_file);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_SOCKSV5_CFG);
  ds_system_call(command, strlen(command));

  return false;
}

/*===========================================================================
  FUNCTION DeleteSOCKSv5Backhaul
==========================================================================*/
/*!
@brief
  Deletes Backhaul in SOCKSv5 proxy conf

@parameters
  wan_iface  - wan iface to delete from socks config

@return
  false - unsuccesful
  true  - succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::DeleteSOCKSv5Backhaul(const char* wan_iface)
{
  pugi::xml_document doc;
  pugi::xml_node node, child, temp_node;
  qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char command[MAX_COMMAND_STR_LEN] = {0};

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    return false;
  }

  if(NULL == wan_iface)
  {
    LOG_MSG_INFO1("wan iface NULL\n", 0, 0, 0);
    return false;
  } else if(strlen(wan_iface) > IFNAMSIZ || !(strlen(wan_iface))) //check wan_iface is valid len
  {
    LOG_MSG_INFO1("%s is invalid wan iface name length\n", wan_iface, 0, 0);
    return false;
  }

  //find out where are configuration files
  if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
  {
    LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
    return false;
  }

  if(!doc.load_file(socksv5_file_path.conf_file))
  {
    LOG_MSG_INFO1("Error loading the conf file: %s\n", socksv5_file_path.conf_file,0,0);
    return false;
  }

  //extract wan_iface
  node = doc.child("PROXY_CONFIG").child("WAN_CFG");

  //delete all previous associations of the wan iface
  for(child = node.first_child(); child != NULL; child = child.next_sibling())
  {
    if(strcmp(child.name(), "WAN_IFACE") == 0)
    {
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        //find the profile to update with iface name
        if((strcmp(grand_child.name(), "NAME") == 0) &&
           (strcmp(grand_child.child_value(), wan_iface) == 0))
        {
          grand_child.text().set("");

          //delete this WAN iface's ip version
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "IP") == 0)
            {
              temp_node.text().set("");
            }
          }

          //delete this WAN iface's DNS IP Addr Info
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "PRI_DNS_IPV4_ADDR") == 0)
            {
              temp_node.text().set("");
            }
          }
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "SEC_DNS_IPV4_ADDR") == 0)
            {
              temp_node.text().set("");
            }
          }
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "PRI_DNS_IPV6_ADDR") == 0)
            {
              temp_node.text().set("");
            }
          }
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "SEC_DNS_IPV6_ADDR") == 0)
            {
              temp_node.text().set("");
            }
          }
        }
      }
    }
  }

  doc.save_file(TEMP_SOCKSV5_CFG);
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", TEMP_SOCKSV5_CFG);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_SOCKSV5_CFG ,
            socksv5_file_path.conf_file);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", socksv5_file_path.conf_file);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_SOCKSV5_CFG);
  ds_system_call(command, strlen(command));


  return true;
}

/*===========================================================================
  FUNCTION SetSOCKSv5DNSServerIPVer
==========================================================================*/
/*!
@brief
  Sets WAN IP version for Backhaul in SOCKSv5 proxy conf

@parameters
  wan_iface  - wan iface to update the IP version info
  wan_ip_ver - 4 or 6

@return
  false - unsuccesful
  true  - succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_NATALG::SetSOCKSv5WANIPVer
(
const char* wan_iface,
int wan_ip_ver
)
{
  pugi::xml_document doc;
  pugi::xml_node node, child, temp_node;
  qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
  boolean set_wan_ip_ver = false;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char command[MAX_COMMAND_STR_LEN] = {0};

  if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
  {
    LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
    return false;
  }

  if((IPV4 != wan_ip_ver) && (IPV6 != wan_ip_ver))
  {
    LOG_MSG_INFO1("%d is invalid wan IP version\n", wan_ip_ver, 0, 0);
    return false;
  } else if(NULL == wan_iface)
  {
    LOG_MSG_INFO1("wan iface null!\n", wan_iface, 0, 0);
    return false;
  }

  //find out where are configuration files
  if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
  {
    LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
    return false;
  }

  if(!doc.load_file(socksv5_file_path.conf_file))
  {
    LOG_MSG_INFO1("Error loading the conf file: %s\n", socksv5_file_path.conf_file,0,0);
    return false;
  }

  //extract wan_iface
  node = doc.child("PROXY_CONFIG").child("WAN_CFG");

  //go through each wan iface and find the profile id to update
  for(child = node.first_child(); child != NULL; child = child.next_sibling())
  {
    if(strcmp(child.name(), "WAN_IFACE") == 0)
    {
      for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
          grand_child = grand_child.next_sibling())
      {
        //find the iface name
        if((strcmp(grand_child.name(), "NAME") == 0) &&
           (strcmp(grand_child.child_value(), wan_iface) == 0))
        {
          temp_node = child;

          //set this WAN iface's ip version
          for(temp_node = child.first_child(); temp_node != NULL;
              temp_node = temp_node.next_sibling())
          {
            if(strcmp(temp_node.name(), "IP") == 0)
            {
              switch(wan_ip_ver)
              {
                case(IPV4):
                {
                  temp_node.text().set("IPV4");
                  set_wan_ip_ver = true;
                  break;
                }
                case(IPV6):
                {
                  temp_node.text().set("IPV6");
                  set_wan_ip_ver = true;
                  break;
                }
                default:
                {
                  temp_node.text().set("");
                  set_wan_ip_ver = true;
                  break;
                }
              }
            }
          }
        }
      }
    }
  }

  if(set_wan_ip_ver)
  {
    doc.save_file(TEMP_SOCKSV5_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_SOCKSV5_CFG ,
              socksv5_file_path.conf_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", socksv5_file_path.conf_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));
    return true;
  }

  LOG_MSG_INFO1("WAN Iface not found for SOCKSv5: %s", wan_iface, 0 , 0);
  return false;
}

/*===========================================================================
  FUNCTION qcmap_compare_nat_entries
==========================================================================*/
/*!
@brief
  - To compare two NAT enteries

@param
  const void *first,
  const void *second

@return
  false  - on success
  true - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

long int qcmap_compare_nat_entries
(
    const void *first,
    const void *second
)
{
  qcmap_cm_port_fwding_entry_conf_t* entry_to_delete = (qcmap_cm_port_fwding_entry_conf_t*)first;
  qcmap_cm_port_fwding_entry_conf_t* nat_entry = (qcmap_cm_port_fwding_entry_conf_t*)second;

  if((entry_to_delete->port_fwding_private_ip) ==
     nat_entry->port_fwding_private_ip &&
     entry_to_delete->port_fwding_private_port ==
     nat_entry->port_fwding_private_port &&
     entry_to_delete->port_fwding_global_port ==
     nat_entry->port_fwding_global_port &&
     entry_to_delete->port_fwding_protocol ==
     nat_entry->port_fwding_protocol)
  {
    return 0;
  }
  return 1;
}

