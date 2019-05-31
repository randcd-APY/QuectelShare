/*====================================================

FILE:  QCMAP_Firewall.cpp

SERVICES:
   QCMAP Connection Manager Firewall Specific Implementation

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   vm         Created

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
#include "ds_qmi_qcmap_msgr.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"

#include "QCMAP_Firewall.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Backhaul_Ethernet.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"

/*===================================================================
                             Class Definitions
  ===================================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes Firewall variables.

@parameters
None

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

QCMAP_Firewall::QCMAP_Firewall(QCMAP_Backhaul* QcMapBackhaul)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  this->QcMapBackhaul = QcMapBackhaul;
  memset(&this->firewall_config,0,sizeof(this->firewall_config));
  this->firewall_config.firewall_enabled = false;
  this->firewall_config.firewall_pkts_allowed = false;
  this->firewall_config.upnp_inbound_pinhole = false;
  if(QcMapMgr && (IS_UL_FIREWALL_ALLOWED(QcMapMgr->target)))
    this->firewall_config.enable_ul_firewall = true;
  else
    this->firewall_config.enable_ul_firewall = false;
  this->firewall_config.num_firewall_entries=0;
  /* Read the configuration. */
  this->firewall_config.extd_firewall_entries.firewallEntryListHead = NULL;
  this->firewall_config.extd_firewall_entries.firewallEntryListTail = NULL;

  LOG_MSG_INFO1("QCMAP_Firewall: set enable_ul_firewall:%d",
                this->firewall_config.enable_ul_firewall,0,0);

  //initialize random number seed
  srand((unsigned) time(NULL));

  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
Destroys the class object

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

QCMAP_Firewall::~QCMAP_Firewall()
{
  qcmap_firewall_entry_list_t* firewallList = &(firewall_config.extd_firewall_entries);
  LOG_MSG_INFO1("Destroying Object: FIREWALL",0,0,0);
  /* Free memory allocated for firewall rules */
  if(firewallList->firewallEntryListHead && firewallList->firewallEntryListHead->next)
  {
    ds_dll_delete_all(firewallList->firewallEntryListHead);
    firewallList->firewallEntryListHead = NULL;
    firewallList->firewallEntryListTail = NULL;
  }
}

/*==========================================================
  FUNCTION Init
==========================================================*/
/*!
@brief
  Initialize Params for Firewall Object

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
void QCMAP_Firewall::Init()
{
  if (ReadFirewallXML())
  {
    LOG_MSG_INFO1("Reading QCMAP Firewall Read XML succeeded.",0,0,0);
    /* Reset NetDev in Firewall. */
    this->UpdateNetDevInXML(NULL);
  }
  else
  {
    LOG_MSG_ERROR("Reading QCMAP Firewall Read XML failed.",0,0,0);
  }
  return;
}


/*==========================================================
  FUNCTION GetProfileHandle
==========================================================*/
/*!
@brief
  Gets profile handle for Firewall Object

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
profile_handle_type_v01 QCMAP_Firewall::GetProfileHandle()
{
  return ((QcMapBackhaul != NULL) ? QcMapBackhaul->profileHandle : 0);
}

/*===========================================================================
  FUNCTION addFirewallEntryToXML
==========================================================================*/
/*!
@brief
  Adds firewall entry to XML

@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf
@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

int QCMAP_Firewall::addFirewallEntryToXML
(
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf
)
{
  struct in_addr addr;
  struct in6_addr ip6_addr;
  char ip6_addr_input[INET6_ADDRSTRLEN];
  int next_hdr_prot = -1;
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  pugi::xml_node parent, root, child, subchild;
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!(root = GetPdnRootXML(xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall cfg corresponding to PDN",0,0,0);
    return QCMAP_CM_ERROR;
  }

  // add new Firewall entry here.
  parent = root.append_child(Firewall_TAG);

  //add handle
  child = parent.append_child(FirewallHandle_TAG);
  snprintf(data, sizeof(data), "%d", extd_firewall_conf->firewall_handle);
  child.append_child(pugi::node_pcdata).set_value(data);
  LOG_MSG_INFO1("Add handle %d",extd_firewall_conf->firewall_handle,0,0);

  //Add Pinhole_Entry
  child = parent.append_child(PinholeEntry_TAG);
  snprintf(data, MAX_STRING_LENGTH, "%d", extd_firewall_conf->upnp_pinhole_handle);
  child.append_child(pugi::node_pcdata).set_value(data);

  //Add the direction
  LOG_MSG_INFO1("Firewall direction %d",extd_firewall_conf->firewall_direction,0,0);
  child = parent.append_child(FirewallDirection_TAG);
  snprintf(data, MAX_STRING_LENGTH, "%s",
          (extd_firewall_conf->firewall_direction == QCMAP_MSGR_UL_FIREWALL) ? "UL" : "DL");
  child.append_child(pugi::node_pcdata).set_value(data);
  LOG_MSG_INFO1("Add Firewall direction %d",extd_firewall_conf->firewall_direction,0,0);

  //add IP version
  child = parent.append_child(IPFamily_TAG);
  snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_vsn);
  child.append_child(pugi::node_pcdata).set_value(data);

  LOG_MSG_INFO1("IP Version: %d", extd_firewall_conf->filter_spec.ip_vsn, 0, 0);
  if(extd_firewall_conf->filter_spec.ip_vsn == IP_V4)
  {
    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
    {
      child = parent.append_child(IPV4SourceAddress_TAG);
      subchild = child.append_child(IPV4SourceIPAddress_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.src.addr.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

      subchild = child.append_child(IPV4SourceSubnetMask_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

      debug_string= extd_firewall_conf->filter_spec.ip_hdr.v4.src.addr.ps_s_addr;
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
      LOG_MSG_INFO1("\nv4 Source Socket address:: '%s'",str,0,0);


      debug_string=extd_firewall_conf->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr;
      readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
      LOG_MSG_INFO1("\nv4 Source Mask address: '%s'",str,0,0);
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
    {
      child = parent.append_child(IPV4DestinationAddress_TAG);
      subchild = child.append_child(IPV4DestinationIPAddress_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

      subchild = child.append_child(IPV4DestinationSubnetMask_TAG);
      addr.s_addr = extd_firewall_conf->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr;
      subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS)
    {
      child = parent.append_child(IPV4TypeOfService_TAG);

      subchild = child.append_child(TOSValue_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v4.tos.val);
      subchild.append_child(pugi::node_pcdata).set_value(data);

      subchild = child.append_child(TOSMask_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v4.tos.mask);
      subchild.append_child(pugi::node_pcdata).set_value(data);

      LOG_MSG_INFO1("Tos: %d mask: %d ", extd_firewall_conf->filter_spec.ip_hdr.v4.tos.val,
                    extd_firewall_conf->filter_spec.ip_hdr.v4.tos.mask, 0);
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT )
    {
      child = parent.append_child(IPV4NextHeaderProtocol_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot);
      child.append_child(pugi::node_pcdata).set_value(data);
      LOG_MSG_INFO1("Next Header: %d ",extd_firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot, 0, 0);
      next_hdr_prot = extd_firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot;
    }

  }
  if (extd_firewall_conf->filter_spec.ip_vsn == IP_V6)
  {
    if(extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
    {
      child = parent.append_child(IPV6SourceAddress_TAG);
      memcpy(ip6_addr.s6_addr,
             extd_firewall_conf->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      bzero(&ip6_addr_input,INET6_ADDRSTRLEN);
      if(inet_ntop(AF_INET6,&ip6_addr,ip6_addr_input,INET6_ADDRSTRLEN) != NULL)
      {
        subchild = child.append_child(IPV6SourceIPAddress_TAG);
        subchild.append_child(pugi::node_pcdata).set_value(ip6_addr_input);
      }

      subchild = child.append_child(IPV6SourcePrefix_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.src.prefix_len);
      subchild.append_child(pugi::node_pcdata).set_value(data);

      printf("\n In:%s @: %d Source IPv6: %s  Mask: %s \n", __FUNCTION__, __LINE__,
             child.child("IPV6SourceIPAddress").child_value(),
             child.child("IPV6SourcePrefix").child_value());
    }
    if(extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
    {
      child = parent.append_child(IPV6DestinationAddress_TAG);
      memcpy(ip6_addr.s6_addr,
             extd_firewall_conf->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
             QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
      bzero(&ip6_addr_input,INET6_ADDRSTRLEN);
      if(inet_ntop(AF_INET6,&ip6_addr,ip6_addr_input,INET6_ADDRSTRLEN) != NULL)
      {
        subchild = child.append_child(IPV6DestinationIPAddress_TAG);
        subchild.append_child(pugi::node_pcdata).set_value(ip6_addr_input);
      }

      subchild = child.append_child(IPV6DestinationPrefix_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.dst.prefix_len);
      subchild.append_child(pugi::node_pcdata).set_value(data);
     printf("\n In:%s @: %d Dest IPv6: %s  Mask: %s \n", __FUNCTION__, __LINE__,
             child.child("IPV6DestinationIPAddress").child_value(),
             child.child("IPV6DestinationPrefix").child_value());
    }

    if(extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
    {
      child = parent.append_child(IPV6TrafficClass_TAG);

      subchild = child.append_child(TrfClsValue_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val);
      subchild.append_child(pugi::node_pcdata).set_value(data);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.trf_cls.mask);
      subchild = child.append_child(TrfClsMask_TAG);
      subchild.append_child(pugi::node_pcdata).set_value(data);
    }

    if( extd_firewall_conf->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT )
    {
      child = parent.append_child(IPV6NextHeaderProtocol_TAG);
      snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot);
      child.append_child(pugi::node_pcdata).set_value(data);
      next_hdr_prot = extd_firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot;
    }
  }

  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
      if (extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        child = parent.append_child(TCPSource_TAG);

        subchild = child.append_child(TCPSourcePort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCPSourceRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        LOG_MSG_INFO1( "Source port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,
                       0 );

      }
      if(extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        child = parent.append_child(TCPDestination_TAG);

        subchild = child.append_child(TCPDestinationPort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCPDestinationRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "Dest port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range,
                       0 );
      }
    break;

    case PS_IPPROTO_UDP:
      if (extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
      {
        child = parent.append_child(UDPSource_TAG);

        subchild = child.append_child(UDPSourcePort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(UDPSourceRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        LOG_MSG_INFO1( "UDP port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.src.range,
                       0 );
      }

      if(extd_firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
      {
        child = parent.append_child(UDPDestination_TAG);

        subchild = child.append_child(UDPDestinationPort_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(UDPDestinationRange_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        LOG_MSG_INFO1( "UDP Dst port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.udp.dst.range,
                       0 );
      }
    break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
      if ( extd_firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask
           & IPFLTR_MASK_ICMP_MSG_TYPE)
      {
        child = parent.append_child(ICMPType_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.icmp.type);
        child.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "ICMP Type : %d  ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.icmp.type,
                       0,0 );

      }

      if (extd_firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE)
      {
        child = parent.append_child(ICMPCode_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.icmp.code);
        child.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "ICMP code : %d  ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.icmp.code,
                       0,0 );
      }
    break;

    case PS_IPPROTO_ESP:
      if (extd_firewall_conf->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI)
      {
        child = parent.append_child(ESPSPI_TAG);
        snprintf(data, sizeof(data), "%d", extd_firewall_conf->filter_spec.next_prot_hdr.esp.spi);
        child.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "ESP spi : %d  ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.esp.spi,
                       0,0 );
      }
    break;

    case PS_IPPROTO_TCP_UDP:
      if(extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
         & IPFLTR_MASK_TCP_UDP_SRC_PORT)
      {
        child = parent.append_child(TCP_UDPSource_TAG);

        subchild = child.append_child(TCP_UDPSourcePort_TAG);
        snprintf( data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port);

        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCP_UDPSourceRange_TAG);
        snprintf( data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range);

        subchild.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "TCP UDP Src port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range,
                       0 );
      }

      if( extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
          & IPFLTR_MASK_TCP_UDP_DST_PORT )
      {
        child = parent.append_child(TCP_UDPDestination_TAG);

        subchild = child.append_child(TCP_UDPDestinationPort_TAG);
        snprintf(data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port);
        subchild.append_child(pugi::node_pcdata).set_value(data);

        subchild = child.append_child(TCP_UDPDestinationRange_TAG);
        snprintf(data, sizeof(data), "%d",
                 extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range);
        subchild.append_child(pugi::node_pcdata).set_value(data);
        LOG_MSG_INFO1( "TCP UDP Src port: %d range: %d ",
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port,
                       extd_firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range,
                       0 );
       }
    break;

    default:
      break;
  }

  QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, xml_file);
  return 0;
}

/*===========================================================================
  FUNCTION addFirewallEntryToList
==========================================================================*/
/*!
@brief
  Adds firewall entry to List

@parameters
  qcmap_msgr_firewall_entry_conf_t* extd_firewall_conf,
  qcmap_msgr_firewall_entry_conf_t *new_firewall_entry

@return
   0 - on failure
   1 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Firewall::addFirewallEntryToList
(
  qcmap_msgr_firewall_entry_conf_t* extd_firewall_conf,
  qcmap_msgr_firewall_entry_conf_t *new_firewall_entry
)
{
  ds_dll_el_t * node = NULL;

  qcmap_firewall_entry_list_t* firewallList = &(this->firewall_config.extd_firewall_entries);

  /*****************************************************/

  /* save into the config */
  if (firewallList->firewallEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
           information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("addFirewallEntryToList - Error in allocating memory for node",
                     0,0,0);
      return false;
    }
    firewallList->firewallEntryListHead = node;
  }

  new_firewall_entry = (qcmap_msgr_firewall_entry_conf_t*)
                            ds_malloc(sizeof(qcmap_msgr_firewall_entry_conf_t));
  if( new_firewall_entry == NULL )
  {
    LOG_MSG_ERROR("addFirewallEntryToList - Error in allocating memory for"
                  "new Firewall entry",0,0,0);
    return false;
  }

  memset(new_firewall_entry, 0, sizeof(qcmap_msgr_firewall_entry_conf_t));

  memcpy(&(new_firewall_entry->filter_spec),
         &(extd_firewall_conf->filter_spec),
         sizeof(ip_filter_type));
  new_firewall_entry->firewall_handle = extd_firewall_conf->firewall_handle;
  new_firewall_entry->firewall_direction= extd_firewall_conf->firewall_direction;

  //Store the firewall entry in the linked list
  if ((node = ds_dll_enq(firewallList->firewallEntryListHead,
                         NULL, (void*)new_firewall_entry )) == NULL)
  {
    LOG_MSG_ERROR("AddFireWallEntry - Error in adding a node",0,0,0);
    ds_free(new_firewall_entry);
    return false;
  }
  firewallList->firewallEntryListTail = node;

  LOG_MSG_INFO1("Added Firewall entry added to list",0,0,0);

  this->firewall_config.num_firewall_entries++;
  return true;
}

/*===========================================================================
  FUNCTION DeleteConntrackEntryForDropFirewallEntries()
==========================================================================*/
/*!
@brief
  Delete the client conntrack when we are adding a DROP firewall entry for
  that IPv4 address and port combination

@parameters
  firewall_entry - firewall entry to add
  protocol_num   - protocol number

@return
  bool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

bool QCMAP_Firewall::DeleteConntrackEntryForDropIPv4FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  char *protocol=NULL,*saddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  char command[MAX_COMMAND_STR_LEN];
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  char *line = NULL;
  size_t len = 0;
  int read;
  bool check_ip = false;
  bool check_sport = false;
  bool check_dport = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port = 0;
  uint32 subnet_mask, ip_addr;

  if (firewall_entry == NULL)
  {
    LOG_MSG_ERROR("NULL Arguments passed",0,0,0);
    return false;
  }

  if (protocol_num != PS_IPPROTO_TCP && protocol_num != PS_IPPROTO_UDP)
  {
    if ( protocol_num == PS_IPPROTO_TCP_UDP)
    {
      DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry,PS_IPPROTO_TCP);
      DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry,PS_IPPROTO_UDP);
    }
    return true;
  }

  /*The logic is to grep for the conntrack entries using firewall params
  so as to reduce the number of conntrack entries to be checked to a
  minimum value. We write these entries to a txt file and then read the
  text file, check if the entry needs to be deleted and then delete the entry*/

  /*We only need certain columns of conntrack entries for deletion,
  so we cut those particular columns*/

  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,"conntrack -L | cut -f1,9,11,12 -d ' ' ");
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,"conntrack -L | cut -f1,10,12,13 -d ' ' ");

  if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
  {
    snprintf(tmp, MAX_COMMAND_STR_LEN,"| grep src=");
    strlcat(command,tmp,MAX_COMMAND_STR_LEN);
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    subnet_mask = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);

    ip_addr = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);

    if (subnet_mask < 0xff000000)
      check_ip = true;

    /*We need to grep according to the subnet mask. If subnet mask
    is 255.0.0.0 / 255.255.0.0 / 255.255.255.0 / 255.255.255.255 ,
    then we can grep straight-away for the match. Otherwise, we grep for
    the minimum pattern that matches perfectly and check later whether
    we need to delete that conntrack entry or not*/

    else if ((subnet_mask >= 0xff000000) && (subnet_mask < 0xffff0000))
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN,"%d. ",(ip_addr & 0xff000000) >> 24);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);

      if (subnet_mask != 0xff000000)
        check_ip = true;
    }

    else if ((subnet_mask >= 0xffff0000) && (subnet_mask < 0xffffff00))
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN,"%d.%d. ",
               (ip_addr & 0xff000000)>>24,
               (ip_addr & 0x00ff0000) >> 16);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);

      if (subnet_mask != 0xffff0000)
        check_ip = true;
    }

    else if ((subnet_mask >= 0xffffff00) && (subnet_mask < 0xffffffff))
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN,"%d.%d.%d. ",
               (ip_addr & 0xff000000)>>24,
               (ip_addr & 0x00ff0000) >> 16,
               (ip_addr & 0x0000ff00) >> 8);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);

      if (subnet_mask != 0xffffff00)
        check_ip = true;
    }
    else if (subnet_mask == 0xffffffff)
    {
      readable_addr(AF_INET,&(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr),tmp);
      strlcat(tmp," ",MAX_COMMAND_STR_LEN);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);

      if (subnet_mask != 0xffffffff)
        check_ip = true;
    }

  }

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      /*If sport / dport are defined, we use them directly, or else we set a flag
      to check the sport and dport later and delete conntrack entry only
      if matches the criterion*/

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
          check_sport = true;
        }
        else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
          check_sport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;
  }

  snprintf(tmp, MAX_COMMAND_STR_LEN,"> %s",CONNTRACK_ENTRIES);
  strlcat(command,tmp,MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("%s",command,0,0);
  ds_system_call(command, strlen(command));
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  memset(command,0,MAX_COMMAND_STR_LEN);

  /*Open the conntrack.txt file to fetch protocol,saddr,dport*/
  fd = fopen(CONNTRACK_ENTRIES,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("DeleteConntrackEntries - Error in opening %s",
                  CONNTRACK_ENTRIES,0,0);
    return false;
  }

  /*Reset the file ptr to start of file*/
  if (fseek(fd,0,SEEK_SET) !=0)
  {
    fclose (fd);
    LOG_MSG_ERROR("File pointer not reset to beginning of file\n",0,0,0);
    return false;
  }
  /*Read the file line-by-line and delete the entry if required*/
  while (((read = getline(&line, &len, fd)) != -1))
  {
    protocol = strtok_r(line, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);
    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);
    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);
    if(dport)
      *(dport + 5) = ' ';

    /*Check for subnet mask*/
    if (check_ip)
    {
      if(saddr)
        inet_pton(AF_INET, saddr+4, (void *)&ip_addr);
      if ((ip_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
          (firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr))
        continue;
    }

    /*Check for source port range*/
    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
        continue;
    }

    /*Check for destination port range*/
    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        continue;
    }

    memset(tmp,0,MAX_COMMAND_STR_LEN);

    /*Delete contrack entry*/
    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);

  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",CONNTRACK_ENTRIES);
  ds_system_call(command, strlen(command));

  return true;
}


/*======================================================
  FUNCTION:  DeleteConntrackEntryForAcceptFirewallEntries
  =====================================================*/
  /*!
      @brief
      Delete the client conntrack when we are adding a ACCEPT firewall entry for
      that IPv4 address and port combination

      @params
         IPv6 address
         Protocol
         UDP Port start number
         UDP Port Range
         TCP Port start number
         TCP Port Range

      @return
      false - failure
      true - success
  */
/*====================================================*/
bool QCMAP_Firewall::DeleteConntrackEntryForAcceptIPv4FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  int read;
  bool check_sport = false;
  bool check_dport = false;
  bool check_ip = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port = 0;
  char *line = NULL;
  size_t len = 0;
  FILE *fd = NULL;
  uint32 subnet_mask, ip_addr;
  char *protocol=NULL,*saddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  QCMAP_LAN* QCMAPLANMgr = GET_LAN_OBJECT(QcMapBackhaul->vlan_id);
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  struct in_addr addr;
  char apps_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

  LOG_MSG_INFO1("DeleteConntrackEntryForAcceptIPv4FirewallEntries \n",0,0,0);

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    return;
  }

  if( QCMAPLANMgr == NULL )
  {
    LOG_MSG_ERROR("LAN object is NULL\n",0,0,0);
    return;
  }


  if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
  {
    LOG_MSG_INFO1("Failed to get LAN config", 0, 0, 0);
    return;
  }
  addr.s_addr = htonl(lan_cfg->apps_ip_addr);
  strlcpy(apps_ip, inet_ntoa(addr), INET_ADDRSTRLEN);

  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L | cut -f1,9,11,12 -d ' ' | grep -v src=%s ", apps_ip);
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,
            "conntrack -L | cut -f1,10,12,13 -d ' ' | grep -v src=%s ", apps_ip);

  if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
  {
    subnet_mask = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
    ip_addr = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
    check_ip = true;
  }

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        check_dport = true;
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        check_dport = true;
      }

      break;
  }

  if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
  {
    subnet_mask = ntohl(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
    check_ip = true;
  }

  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", CONNTRACK_ENTRIES);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  LOG_MSG_INFO1("%s",command,0,0);
  ds_system_call(command, strlen(command));

  /*Open the conntrack.txt file to fetch protocol,saddr,dport*/
  fd = fopen(CONNTRACK_ENTRIES,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("DeleteConntrackEntries - Error in opening %s",
                  CONNTRACK_ENTRIES,0,0);
    return false;
  }

  /*Reset the file ptr to start of file*/
  if (fseek(fd,0,SEEK_SET) !=0)
  {
    LOG_MSG_ERROR("File pointer not reset to beginning of file\n",0,0,0);
    return false;
  }

  /*Read the file line-by-line and delete the entry if required*/
  while (((read = getline(&line, &len, fd)) != -1))
  {
    protocol = strtok_r(line, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);

    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);

    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);

    if(dport)
      *(dport + 5) = ' ';

    /*Check for subnet mask*/
    if (check_ip)
    {
      if(saddr)
        inet_pton(AF_INET, saddr+4, (void *)&ip_addr);
      if ((ip_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
          (firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr & firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr))
        goto delete_entry;
    }

    /*Check for source port range*/
    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
        goto delete_entry;
    }

    /*Check for destination port range*/
    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        goto delete_entry;
    }

    /*check protocol*/
    if (firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP_UDP ||
        firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot == protocol_num ||
        firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot == 0)
      continue;

    /*Delete contrack entry*/
    delete_entry:
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);

  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",CONNTRACK_ENTRIES);
  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION SetFirewallV4
==========================================================================*/
/*!
@brief
  Adds firewall entry by preparing, the command to add firewall entry based on the
  firewall configuration value and executes the same.
  It also generates delete command for the same.

@parameters
  qcmap_msgr_firewall_conf_t *firewall
  uint8_t *del_command

@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::SetFirewallV4
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  boolean add_rule,
  qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN]={0}, devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2]={0};
  char final_command[MAX_COMMAND_STR_LEN]={0}, final_command_sec[MAX_COMMAND_STR_LEN]={0};
  char final_emb_command[MAX_COMMAND_STR_LEN]={0}, final_emb_command_sec[MAX_COMMAND_STR_LEN]={0};
  char command_sec[MAX_COMMAND_STR_LEN];
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char str[INET6_ADDRSTRLEN];
  int qcmap_cm_error;
  uint8 next_hdr_prot;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true ||
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true)
  {
    LOG_MSG_ERROR("Cannot add firewall entry in bridge mode",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return QCMAP_CM_ERROR;
  }

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }

  if (firewall_entry->filter_spec.ip_vsn != IP_V4) {
      LOG_MSG_ERROR( "Incorrect firewall_entry version. IP Version = %x\n",
                     firewall_entry->filter_spec.ip_vsn,0,0 );
     *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
      return QCMAP_CM_ERROR;
  }

  if(QcMapBackhaul && (QcMapBackhaul->QcMapBackhaulWWAN) &&
     (QcMapBackhaul->QcMapBackhaulWWAN->GetState()!= QCMAP_CM_WAN_CONNECTED) &&
    !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() &&
    !QCMAP_Backhaul_WLAN::IsSTAAvailableV4() &&
    !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() &&
     !QCMAP_BT_TETHERING::IsBTBackhaulAvailableV4())
  {
    strlcpy(devname, QcMapBackhaul->wan_cfg.ipv4_interface, sizeof(devname));
  }
  else if (QcMapBackhaul && !QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V4_V01, qmi_err_num))
  {
    LOG_MSG_ERROR("Failed to get WAN device name",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return QCMAP_CM_ERROR;
  }

  if(firewall_entry->firewall_direction == QCMAP_MSGR_DL_FIREWALL)
  {
    /*In comming traffic from wwan is handled in PREROUTING chain in mangle table */
    if ( add_rule )
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -I PREROUTING -i %s ",devname);
    }
    else
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D PREROUTING -i %s ",devname);
    }
  }
  else if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
  {
    /*Outgoing traffic from wwan is handled in POSTROUTING chain in mangle table by Default rules
      So mark the packets in FORWARD chain*/
    if ( add_rule )
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -I FORWARD -o %s ",devname);
      snprintf(final_emb_command,MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -I OUTPUT -o %s ",devname);

    }
    else
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D FORWARD -o %s ",devname);
      snprintf(final_emb_command,MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D OUTPUT -o %s ",devname);
    }
  }

  strlcpy(final_command_sec, final_command, MAX_COMMAND_STR_LEN);
  strlcpy(final_emb_command_sec, final_emb_command, MAX_COMMAND_STR_LEN);

   if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
   {
     readable_addr(AF_INET,&(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr),(char *)&str);
     strlcat(command," -s ",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
     readable_addr(AF_INET,&(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr),(char *)&str);
     strlcat(command,"/",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
   }

   if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS )
   {
     snprintf( tmp,MAX_COMMAND_STR_LEN," -m tos --tos %x/%x ",
               firewall_entry->filter_spec.ip_hdr.v4.tos.val,
               firewall_entry->filter_spec.ip_hdr.v4.tos.mask );

     strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     memset(tmp,0,MAX_COMMAND_STR_LEN);
   }
   next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;

  switch( next_hdr_prot )
  {
    case PS_IPPROTO_TCP:
      strlcat(command," -p tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
                  ( firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.src.range));
        }
        else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

     if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
     {
       if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
       {
         snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                  firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
                 (firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port + \
                  firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range));
       }else
       {
         snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                  firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
       }
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     break;
   case PS_IPPROTO_UDP:

     strlcat(command," -p udp ",MAX_COMMAND_STR_LEN);

     if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
          IPFLTR_MASK_UDP_SRC_PORT )
     {
       if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
       {
         snprintf( tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port,
                  ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range+\
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port ) );
       }else
       {
         snprintf( tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port );
       }
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
     {
       if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
       {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port,
                  (firewall_entry->filter_spec.next_prot_hdr.udp.dst.range + \
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port) );
       }else
       {
         snprintf( tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port );
       }
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     break;
   case PS_IPPROTO_TCP_UDP:

     strlcpy(command_sec, command, MAX_COMMAND_STR_LEN);
     strlcat(command, " -p tcp ", MAX_COMMAND_STR_LEN);
     strlcat(command_sec, " -p udp ", MAX_COMMAND_STR_LEN);

     if( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
         IPFLTR_MASK_TCP_UDP_SRC_PORT )
     {
       snprintf(tmp, MAX_COMMAND_STR_LEN, " --sport %d:%d ",
           firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
           firewall_entry->filter_spec.next_prot_hdr.tcp.src.range + \
           firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
       strlcat(command, tmp, MAX_COMMAND_STR_LEN);
       strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
          IPFLTR_MASK_TCP_UDP_DST_PORT )
     {
       snprintf(tmp, MAX_COMMAND_STR_LEN, " --dport %d:%d ",
           firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
           firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range + \
           firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
       strlcat(command, tmp, MAX_COMMAND_STR_LEN);
       strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }
     break;

   case PS_IPPROTO_ICMP:

     strlcat(command," -p icmp ",MAX_COMMAND_STR_LEN);

     if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask &
          IPFLTR_MASK_ICMP_MSG_TYPE )
     {
       snprintf(tmp,MAX_COMMAND_STR_LEN," --icmp-type %d",
                firewall_entry->filter_spec.next_prot_hdr.icmp.type);
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }

     if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask &
          IPFLTR_MASK_ICMP_MSG_CODE )
     {
       snprintf(tmp,MAX_COMMAND_STR_LEN, "/%d ",
                firewall_entry->filter_spec.next_prot_hdr.icmp.code);
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
     }
     break;

   case PS_IPPROTO_ESP:

     strlcat(command," -p esp ",MAX_COMMAND_STR_LEN);
     if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI )
     {
       snprintf(tmp,MAX_COMMAND_STR_LEN," --espspi %d ",
                firewall_entry->filter_spec.next_prot_hdr.esp.spi);
       strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     }
     break;
   default:
     LOG_MSG_ERROR("Unsupported protocol \n",next_hdr_prot,0,0);
     break;
  }

  LOG_MSG_INFO1("SET FIREWALL pkts allowed = %d",
                 firewall_config.firewall_pkts_allowed,0,0);

  if(firewall_entry->firewall_direction == QCMAP_MSGR_DL_FIREWALL)
  {
      snprintf(tmp,MAX_COMMAND_STR_LEN," -j CONNMARK --set-mark %d",FIREWALL_MARK);
  }

  if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
  {
    snprintf(tmp,MAX_COMMAND_STR_LEN," -j CONNMARK --set-mark %d", FIREWALL_MARK);
  }

  strlcat(command,tmp,MAX_COMMAND_STR_LEN);
  strlcat(final_command,command,MAX_COMMAND_STR_LEN);
  ds_system_call(final_command, strlen(final_command));

  if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
  {
    strlcat(final_emb_command,command,MAX_COMMAND_STR_LEN);
    ds_system_call(final_emb_command, strlen(final_emb_command));
  }

  if(next_hdr_prot == PS_IPPROTO_TCP_UDP)
  {
    strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
    strlcat(final_command_sec,command_sec,MAX_COMMAND_STR_LEN);
    ds_system_call(final_command_sec, strlen(final_command_sec));

    if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
    {
      strlcat(final_emb_command_sec,command_sec,MAX_COMMAND_STR_LEN);
      ds_system_call(final_emb_command_sec, strlen(final_emb_command_sec));
    }
  }

  /* The below API's are need to clear the conntrack entries which would in turn delete
    the SFE entries.
    firewall_pkt_allowed =  false we need to delete the conntrack entries for the firewall IP, Port
    and Protocol combination which is entered
    firewall_pkt_allowed =  true we need to delete all other conntrack entries except for the
    firewall IP, Port and Protocol combination which is entered
    This would be applicable for Only TCP, UDP and if no protocol is mentioned (if firewall is added
    only based on sport and dport)
   */
  if ( next_hdr_prot == PS_IPPROTO_UDP || next_hdr_prot == PS_IPPROTO_TCP ||
       next_hdr_prot == PS_IPPROTO_TCP_UDP || next_hdr_prot == PS_IPPROTO_NO_PROTO )
  {
    if (add_rule)
    {
      if (firewall_config.firewall_pkts_allowed)
      {
        DeleteConntrackEntryForAcceptIPv4FirewallEntries(firewall_entry, PS_IPPROTO_TCP);
        DeleteConntrackEntryForAcceptIPv4FirewallEntries(firewall_entry, PS_IPPROTO_UDP);
      }
      else
      {
        if (next_hdr_prot == PS_IPPROTO_NO_PROTO)
          DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry, PS_IPPROTO_TCP_UDP);
        else
          DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry, next_hdr_prot);
      }
    }
    else
    {
      if (firewall_config.firewall_enabled && firewall_config.firewall_pkts_allowed)
      {
        /*When the Allow firewall Rule is deleted, corresponding conntrack entry
          should be deleted to let IPA know*/
        if (next_hdr_prot == PS_IPPROTO_NO_PROTO)
          DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry, PS_IPPROTO_TCP_UDP);
        else
          DeleteConntrackEntryForDropIPv4FirewallEntries(firewall_entry, next_hdr_prot);
      }
    }
  }
  return QCMAP_CM_SUCCESS;
}


/*===========================================================================
  FUNCTION SetFirewallV6
==========================================================================*/
/*!
@brief
  Adds firewall entry by preparing, the command to add firewall entry based on the
  firewall configuration value and executes the same.
  It also generates delete ommand for the same.

@parameters
  qcmap_msgr_firewall_conf_t *firewall
  uint8_t *del_command

@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::SetFirewallV6
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  boolean add_rule,
  qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN]={0}, devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2]={0};
  char command_sec[MAX_COMMAND_STR_LEN]={0};
  char final_command_sec[MAX_COMMAND_STR_LEN]={0};
  char final_command[MAX_COMMAND_STR_LEN]={0};
  char final_emb_command_sec[MAX_COMMAND_STR_LEN]={0};
  char final_emb_command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char str[INET6_ADDRSTRLEN]={0};
  int qcmap_cm_error;
  uint8 next_hdr_prot;

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true ||
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true)
  {
    LOG_MSG_ERROR("Cannot add firewall entry in bridge mode",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return QCMAP_CM_ERROR;
  }

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }

  if (firewall_entry->filter_spec.ip_vsn != IP_V6) {
      LOG_MSG_ERROR( "Incorrect firewall_entry version. IP Version = %x\n",
                     firewall_entry->filter_spec.ip_vsn,0,0 );
     *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
      return QCMAP_CM_ERROR;
  }

  if(QcMapBackhaul && (QcMapBackhaul->QcMapBackhaulWWAN) &&
     QcMapBackhaul->QcMapBackhaulWWAN->GetIPv6State() != QCMAP_CM_V6_WAN_CONNECTED &&
     !QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() &&
     !QCMAP_Backhaul_WLAN::IsSTAAvailableV6() &&
     !QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6() &&
     !QCMAP_BT_TETHERING::IsBTBackhaulAvailableV6())
  {
    strlcpy(devname, QcMapBackhaul->wan_cfg.ipv6_interface, sizeof(devname));
  }
  else if (QcMapBackhaul && !QcMapBackhaul->GetDeviceName(devname,QCMAP_MSGR_IP_FAMILY_V6_V01, qmi_err_num))
  {
    LOG_MSG_ERROR("Failed to get WAN device name",0,0,0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return QCMAP_CM_ERROR;
  }

  if(firewall_entry->firewall_direction == QCMAP_MSGR_DL_FIREWALL)
  {
    /*In comming traffic from wwan is handled in PREROUTING chain in mangle table */
    if ( add_rule )
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -I PREROUTING -i %s ",devname);
    }
    else
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -D PREROUTING -i %s ",devname);
    }
  }
  else if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
  {
    /*Outgoing traffic from wwan is handled in POStROUTING chain in mangle table by default rules
      So mark the packets in FORWARD chain*/
    if ( add_rule )
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -I FORWARD -o %s ",devname);
      snprintf(final_emb_command,MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -I OUTPUT -o %s ",devname);
    }
    else
    {
      snprintf(final_command,MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -D FORWARD -o %s ",devname);
      snprintf(final_emb_command,MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -D OUTPUT -o %s ",devname);

    }
  }

   strlcpy(final_command_sec, final_command, MAX_COMMAND_STR_LEN);
   strlcpy(final_emb_command_sec, final_emb_command, MAX_COMMAND_STR_LEN);
   if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
   {
     readable_addr(AF_INET6,(uint32 *)\
     &firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr32,(char *)&str);

     strlcat(command," -s ",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
     snprintf( tmp,MAX_COMMAND_STR_LEN,"/%d ",
               firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len);
     strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     memset(tmp,0,MAX_COMMAND_STR_LEN);
   }

   if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
   {
     readable_addr(AF_INET6,
                   (uint32 *)&firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr32,
                   (char *)&str);
     strlcat(command," -d ",MAX_COMMAND_STR_LEN);
     strlcat(command,str,MAX_COMMAND_STR_LEN);
     snprintf( tmp,MAX_COMMAND_STR_LEN,"/%d ", firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len);
     strlcat(command,tmp,MAX_COMMAND_STR_LEN);
     memset(tmp,0,MAX_COMMAND_STR_LEN);
   }

   if(firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
   {
     snprintf(tmp, MAX_COMMAND_STR_LEN," -m tos --tos %d/%d ",
              firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val,
              firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask);
     strlcat(command, tmp, MAX_COMMAND_STR_LEN);
       memset(tmp,0,MAX_COMMAND_STR_LEN);
   }
   next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
   switch( next_hdr_prot )
   {
     case PS_IPPROTO_TCP:
       strlcat(command," -p tcp ",MAX_COMMAND_STR_LEN);

        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT )
        {
          if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
                     ( firewall_entry->filter_spec.next_prot_hdr.tcp.src.port + \
                     firewall_entry->filter_spec.next_prot_hdr.tcp.src.range));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
        {
          if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
                    (firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port + \
                    firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        break;
      case PS_IPPROTO_UDP:

        strlcat(command," -p udp ",MAX_COMMAND_STR_LEN);

        if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT )
        {
          if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.src.port,
                    (firewall_entry->filter_spec.next_prot_hdr.udp.src.range+\
                    firewall_entry->filter_spec.next_prot_hdr.udp.src.port));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --sport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.src.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
        {
          if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d:%d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.dst.port,
                     (firewall_entry->filter_spec.next_prot_hdr.udp.dst.range + \
                     firewall_entry->filter_spec.next_prot_hdr.udp.dst.port));
          }else
          {
            snprintf(tmp,MAX_COMMAND_STR_LEN," --dport %d ",
                     firewall_entry->filter_spec.next_prot_hdr.udp.dst.port);
          }
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        break;
      case PS_IPPROTO_TCP_UDP:

        strlcpy(command_sec, command, MAX_COMMAND_STR_LEN);
        strlcat(command, " -p tcp ", MAX_COMMAND_STR_LEN);
        strlcat(command_sec, " -p udp ", MAX_COMMAND_STR_LEN);

        if( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
            IPFLTR_MASK_TCP_UDP_SRC_PORT )
        {
          snprintf(tmp, MAX_COMMAND_STR_LEN, " --sport %d:%d ",
              firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,
              firewall_entry->filter_spec.next_prot_hdr.tcp.src.range + \
              firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
          strlcat(command, tmp, MAX_COMMAND_STR_LEN);
          strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask &
             IPFLTR_MASK_TCP_UDP_DST_PORT )
        {
          snprintf(tmp, MAX_COMMAND_STR_LEN, " --dport %d:%d ",
              firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,
              firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range + \
              firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
          strlcat(command, tmp, MAX_COMMAND_STR_LEN);
          strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }
        break;

      case PS_IPPROTO_ICMP6:

        strlcat(command," -p icmpv6 ",MAX_COMMAND_STR_LEN);

        if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & \
             IPFLTR_MASK_ICMP_MSG_TYPE)
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --icmpv6-type %d",
                   firewall_entry->filter_spec.next_prot_hdr.icmp.type);
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }

        if( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & \
            IPFLTR_MASK_ICMP_MSG_CODE )
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN, "/%d ",
                   firewall_entry->filter_spec.next_prot_hdr.icmp.code);
          strlcat(command, tmp, MAX_COMMAND_STR_LEN);
          memset(tmp,0,MAX_COMMAND_STR_LEN);
        }
        break;
      case PS_IPPROTO_ESP:

        strlcat(command," -p esp ",MAX_COMMAND_STR_LEN);
        if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & \
             IPFLTR_MASK_ESP_SPI )
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN," --espspi %d ",
                   firewall_entry->filter_spec.next_prot_hdr.esp.spi);
          strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        }
        break;
      default:
        LOG_MSG_ERROR("Unsupported protocol %d \n",next_hdr_prot,0,0);
        break;
    }

  if ( firewall_entry->upnp_pinhole_handle )
    snprintf(tmp,MAX_COMMAND_STR_LEN," -j %s","ACCEPT");
  else
  {
    if(firewall_entry->firewall_direction == QCMAP_MSGR_DL_FIREWALL)
    {
      snprintf(tmp,MAX_COMMAND_STR_LEN," -j CONNMARK --set-mark %d", FIREWALL_MARK);
    }
    if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
    {
      snprintf(tmp,MAX_COMMAND_STR_LEN," -j CONNMARK --set-mark %d", FIREWALL_MARK);
    }
  }
  strlcat(command,tmp,MAX_COMMAND_STR_LEN);
  strlcat(final_command,command,MAX_COMMAND_STR_LEN);
  ds_system_call(final_command, strlen(final_command));

  if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
  {
    strlcat(final_emb_command,command,MAX_COMMAND_STR_LEN);
    ds_system_call(final_emb_command, strlen(final_emb_command));
  }

  if(next_hdr_prot == PS_IPPROTO_TCP_UDP)
  {
    strlcat(command_sec, tmp, MAX_COMMAND_STR_LEN);
    strlcat(final_command_sec,command_sec,MAX_COMMAND_STR_LEN);
    ds_system_call(final_command_sec, strlen(final_command_sec));

    if(firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL)
    {
      strlcat(final_emb_command_sec, command_sec, MAX_COMMAND_STR_LEN);
      ds_system_call(final_emb_command_sec, strlen(final_emb_command_sec));
    }
  }

  /* The below API's are need to clear the conntrack entries which would in turn delete
    the SFE entries.
    firewall_pkt_allowed =  false we need to delete the conntrack entries for the firewall IP, Port
    and Protocol combination which is entered
    firewall_pkt_allowed =  true we need to delete all other conntrack entries except for the
    firewall IP, Port and Protocol combination which is entered
    This would be applicable for Only TCP, UDP and if no protocol is mentioned (if firewall is added
    only based on sport and dport)
   */

  if ( next_hdr_prot == PS_IPPROTO_UDP || next_hdr_prot == PS_IPPROTO_TCP ||
       next_hdr_prot == PS_IPPROTO_TCP_UDP || next_hdr_prot == PS_IPPROTO_NO_PROTO )
  {
    if ( add_rule )
    {
      if (firewall_config.firewall_pkts_allowed)
      {
        DeleteConntrackEntryForAcceptIPv6FirewallEntries(firewall_entry, PS_IPPROTO_TCP);
        DeleteConntrackEntryForAcceptIPv6FirewallEntries(firewall_entry, PS_IPPROTO_UDP);
      }
      else if (!firewall_config.firewall_pkts_allowed)
      {
        if ( next_hdr_prot == PS_IPPROTO_NO_PROTO )
          DeleteConntrackEntryForDropIPv6FirewallEntries( firewall_entry, PS_IPPROTO_TCP_UDP);
        else
          DeleteConntrackEntryForDropIPv6FirewallEntries( firewall_entry, next_hdr_prot);
      }
    }
    else
    {
      if (firewall_config.firewall_enabled && firewall_config.firewall_pkts_allowed)
      {
        /*When the Allow firewall Rule is deleted, corresponding conntrack entry
          should be deleted to let IPA know*/
        if ( next_hdr_prot == PS_IPPROTO_NO_PROTO )
          DeleteConntrackEntryForDropIPv6FirewallEntries( firewall_entry, PS_IPPROTO_TCP_UDP);
        else
          DeleteConntrackEntryForDropIPv6FirewallEntries( firewall_entry, next_hdr_prot);
      }
    }
  }
  return QCMAP_CM_SUCCESS;
}

/*===========================================================================
  FUNCTION SetFirewallConfig
==========================================================================*/
/*!
@brief
  Sets the frewall configuration

@parameters
  boolean enable_firewall,
  boolean pkts_allowed,
  qmi_error_type_v01  *qmi_err_num

@return
   0 - on failure
   1- on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Firewall::SetFirewallConfig
(
  boolean enable_firewall,
  boolean pkts_allowed,
  boolean upnp_pinhole_flag,
  qmi_error_type_v01  *qmi_err_num,
  boolean pkts_allowed_valid,
  boolean upnp_pinhole_flag_valid
)
{
  uint i;
  ds_dll_el_t * node = NULL;
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL;
  qcmap_firewall_entry_list_t* firewallList = NULL;
  qcmap_msgr_firewall_entry_conf_t extd_firewall_entry;
  int firewall_count;
  pugi::xml_document xml_file;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL", 0,0,0);
    return false;
  }

  if (!(IS_DEFAULT_PROFILE(GetProfileHandle())))
  {
    if (!(IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(QcMapMgr->target)) )
    {
      LOG_MSG_ERROR("Firewall for current target(%d) not supported", QcMapMgr->target,0,0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }
  }

  if (!xml_file.load_file(QCMAP_FIREWALL_CONFIG))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if(QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    firewallList = &(firewall_config.extd_firewall_entries);
    if( enable_firewall )
    {
      if(!firewall_config.firewall_enabled)
      {
        LOG_MSG_INFO1("Firewall Was disabled Enabling the same",0,0,0);
        firewall_config.firewall_enabled = enable_firewall;
        firewall_config.firewall_pkts_allowed = pkts_allowed;
        SetDefaultFirewall();

        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        for (i = 0; i < firewall_config.num_firewall_entries && node; i++)
        {
          SetFirewall(curr_firewall_entry, true, qmi_err_num);
          //node = node->next;
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }
      }

      if(firewall_config.firewall_enabled &&
         (pkts_allowed_valid && (firewall_config.firewall_pkts_allowed != pkts_allowed)))
      {
        LOG_MSG_INFO1("\n Firewall Was enabled changing the action\n",0,0,0);
        CleanIPv4MangleTable();
        CleanIPv6MangleTable();

        firewall_config.firewall_pkts_allowed = pkts_allowed;

        SetDefaultFirewall();
        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        for (i = 0; i < firewall_config.num_firewall_entries && node; i++)
        {
          this->SetFirewall(curr_firewall_entry, true, qmi_err_num);
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }
      }
    }
    else
    {
      LOG_MSG_INFO1("In Disable firewall",0,0,0);
      if(firewall_config.firewall_enabled)
      {
        LOG_MSG_INFO1("Firewall was enabled disabling the same",0,0,0);
        firewall_config.firewall_enabled = enable_firewall;
        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        for (i = 0; i < firewall_config.num_firewall_entries && node; i++)
        {
          SetFirewall(curr_firewall_entry, false, qmi_err_num);
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }

        firewall_config.firewall_pkts_allowed = false;
        firewall_config.upnp_inbound_pinhole = false;

        SetDefaultFirewall();
      }
      else
        LOG_MSG_INFO1("Already disabled firewall",0,0,0);
    }
    firewall_config.upnp_inbound_pinhole = upnp_pinhole_flag;
    /*even if the TLV is not valid, disable firewall we need to do this*/
    if(upnp_pinhole_flag_valid || !enable_firewall)
    {
      if (!upnp_pinhole_flag)
      {
        LOG_MSG_INFO1("If upnp flag disable when object created",0,0,0);
        node = firewallList->firewallEntryListHead;
        node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        for (i = 0; i < firewall_config.num_firewall_entries && node; i++)
        {
          if(curr_firewall_entry->upnp_pinhole_handle)
          {
            DeleteFireWallEntry(curr_firewall_entry, curr_firewall_entry->firewall_handle, qmi_err_num);
          }
          node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
        }
      }
    }
  }
  else
  {
    /*backhaul is NOT active*/
    firewall_config.firewall_enabled = enable_firewall;
    if(pkts_allowed_valid && firewall_config.firewall_pkts_allowed != pkts_allowed)
       firewall_config.firewall_pkts_allowed = pkts_allowed;
    if(upnp_pinhole_flag_valid && firewall_config.upnp_inbound_pinhole != upnp_pinhole_flag)
      firewall_config.upnp_inbound_pinhole = upnp_pinhole_flag;

    if (upnp_pinhole_flag_valid && !upnp_pinhole_flag)
    {
      LOG_MSG_INFO1("UPNP flag is disabled when object NOT created",0,0,0);
      firewall_count = GetFirewallEntryCountFromXML(&xml_file);
      LOG_MSG_INFO1(" Firewall Count:%d ",firewall_count,0,0);
      for( i=0; i< firewall_count; i++)
      {
        if(searchFirewallByIndexInXML(&xml_file, &extd_firewall_entry, i))
        {
          LOG_MSG_INFO1(" Insearch by index ",0,0,0);
          if( extd_firewall_entry.upnp_pinhole_handle == 1)
          {
            DeleteFireWallEntry(&extd_firewall_entry, extd_firewall_entry.firewall_handle, qmi_err_num);
            LOG_MSG_INFO1("Firewall Handle num: %d ", extd_firewall_entry.firewall_handle, 0, 0);
          }
        }
      }
    }
  }
  LOG_MSG_INFO1("set to &enable_firewall=%d, &pkts_allowed=%d,&upnp_pinhole_flag=%d",
                 firewall_config.firewall_enabled, firewall_config.firewall_pkts_allowed,
                 firewall_config.upnp_inbound_pinhole);

  GetSetFirewallConfigFromXML(SET_VALUE, &enable_firewall, &pkts_allowed,&upnp_pinhole_flag);
  return true;
}

/*===========================================================================
  FUNCTION GetFirewallConfig
==========================================================================*/
/*!
@brief
  Gets the frewall configuration

@parameters
  boolean enable_firewall,
  boolean pkts_allowed,
  qmi_error_type_v01  *qmi_err_num

@return
  -0 - on failure
   1- on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Firewall::GetFirewallConfig
(
  boolean *enable_firewall,
  boolean *pkts_allowed,
  boolean *upnp_pinhole_flag,
  qmi_error_type_v01  *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL", 0,0,0);
    return false;
  }

  if (!(IS_DEFAULT_PROFILE(GetProfileHandle())))
  {
    if (!(IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(QcMapMgr->target)) )
    {
      LOG_MSG_ERROR("Firewall for current target(%d) not supported", QcMapMgr->target,0,0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }
  }

  if (QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    *enable_firewall = firewall_config.firewall_enabled;

    if(firewall_config.firewall_enabled)
      *pkts_allowed = firewall_config.firewall_pkts_allowed;

    *upnp_pinhole_flag = firewall_config.upnp_inbound_pinhole;
    LOG_MSG_INFO1("enable_firewall = %d firewall_pkts_allowed =%d upnp_inbound_pinhole=%d",
                  *enable_firewall,*pkts_allowed, *upnp_pinhole_flag);
    return true;
  }

  GetSetFirewallConfigFromXML(GET_VALUE, enable_firewall, pkts_allowed,upnp_pinhole_flag);
  LOG_MSG_INFO1("enable_firewall = %d firewall_pkts_allowed =%d upnp_inbound_pinhole=%d",
                *enable_firewall,*pkts_allowed, *upnp_pinhole_flag);

  return true;
}



/*===========================================================================
  FUNCTION DeleteFireWallEntry
==========================================================================*/
/*!
@brief
  Deletes the firewall entry on A5.

@parameters
  qcmap_msgr_firewall_conf_t* extd_firewall_conf
  int firewall_handle,
  qmi_error_type_v01  *qmi_err_num

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
boolean QCMAP_Firewall::DeleteFireWallEntry
(
   qcmap_msgr_firewall_entry_conf_t* extd_firewall_entry,
   int firewall_handle,
   qmi_error_type_v01  *qmi_err_num
)
{
  ds_dll_el_t * node = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_firewall_entry_list_t* firewallList = NULL;
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL;
  pugi::xml_document xml_file;
  pugi::xml_node parent, root, child;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }

  if (!(child = searchFirewallByHandleInXML(&xml_file, ( extd_firewall_entry),
                                                  firewall_handle)))
  {
    LOG_MSG_ERROR("DeleteFireWallEntry - Error in finding the node",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
    return false;
  }

  firewallList = &(firewall_config.extd_firewall_entries);
  node = ds_dll_next(firewallList->firewallEntryListHead, (void *)&curr_firewall_entry);

  while(curr_firewall_entry->firewall_handle != firewall_handle && node)
  {
    node = ds_dll_next(node, (void *)&curr_firewall_entry);
  }

  if(!node)
  {
    LOG_MSG_ERROR("Invalid handle",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
    return false;
  }

  if (node)
  {
    ds_dll_delete_node(firewallList->firewallEntryListHead,
                       &(firewallList->firewallEntryListTail),
                       node);

    //Free the firewall entry structure
    ds_free (curr_firewall_entry);
    curr_firewall_entry = NULL;

    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;
  }

  if(firewall_config.firewall_enabled && QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    if(SetFirewall(( extd_firewall_entry), FALSE, qmi_err_num) != QCMAP_CM_SUCCESS)
    {
      if(*qmi_err_num != QMI_ERR_INTERFACE_NOT_FOUND_V01)
      {
        LOG_MSG_ERROR("SetFirewall Entry failed\n",0,0,0);
        return  false;
      }
    }
  }
  // Add logic to check if there are no more nodes in the list and free the head
  if (firewallList->firewallEntryListHead->next == NULL)
  {
    LOG_MSG_INFO1("\n No Firewall entry exist\n",0,0,0);
    ds_dll_free(firewallList->firewallEntryListHead);
    firewallList->firewallEntryListHead = NULL;
  }

  firewall_config.num_firewall_entries--;

  /* Update the XML file and return back if the object is not yet created. */
  parent = child.parent();
  parent.remove_child(child);

  QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);


  /* For QMI_ERR_INTERFACE_NOT_FOUND_V01 we will delete but indicate user with the error message*/
  if( *qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01 )
  {
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDefaultFirewallRule
==========================================================================*/
/*!
@brief
  Set the default firewall rule based on pkts_allowed and firewall enable bits
  for various interfaces

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Firewall::SetDefaultFirewallRule(qcmap_msgr_ip_family_enum_v01 ip_family)
{
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2]={0};
  char command[MAX_COMMAND_STR_LEN]={0};
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if  (!QcMapBackhaul->GetDeviceName(devname, ip_family, &qmi_err_num))
  {
    LOG_MSG_ERROR("Could not get backhaul interface for ip_family %d", ip_family,0,0 );
    return false;
  }

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true ||
      QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true)
  {
    LOG_MSG_ERROR("Cannot add firewall rule in bridge mode",0,0,0);
    return false;
  }

  LOG_MSG_INFO1( "SetDefaultFirewallRule:: Enter for Interface %x and devname\= %s,"
                 " enable_ul_firewall: %d",
                 ip_family,devname, firewall_config.enable_ul_firewall);

  /* Default Firewall Configuration based on the mode*/
  switch (ip_family)
  {
    case QCMAP_MSGR_IP_FAMILY_V4_V01:
    {
      if ((firewall_config.firewall_enabled == false) ||
         ((firewall_config.firewall_enabled == true) &&
         (firewall_config.firewall_pkts_allowed == false)))
      {
        /*Clean up previous rules*/
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D PREROUTING -i %s -m connmark --mark %d -j ACCEPT", devname, FIREWALL_MARK);
        ds_system_call(command,strlen(command));
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D PREROUTING -i %s -j DROP",devname);
        ds_system_call(command,strlen(command));


        /*Default Rule for DL Firewall - Blacklisting*/
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -%s PREROUTING -i %s -m connmark --mark %d -j DROP",
                 (firewall_config.firewall_enabled == false)?"D":"I",devname,FIREWALL_MARK);
        ds_system_call(command,strlen(command));

        if((firewall_config.firewall_enabled == false)||
           (firewall_config.enable_ul_firewall == true))
        {
            snprintf(command, MAX_COMMAND_STR_LEN,
                     "iptables -t mangle -D POSTROUTING -o %s -j DROP",devname);
            ds_system_call(command,strlen(command));
            if(QcMapMgr && QcMapMgr->packet_stats_enabled)
            {
              snprintf(command, MAX_COMMAND_STR_LEN,
                       "iptables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j TRAFFIC_ACCT",
                       devname,FIREWALL_MARK);
            }
            else
            {
              snprintf(command, MAX_COMMAND_STR_LEN,
                      "iptables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j ACCEPT",
                      devname,FIREWALL_MARK);
            }
            ds_system_call(command,strlen(command));

            /*Default Rule for UL Firewall - Blacklisiting*/
            snprintf(command, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -%s POSTROUTING -o %s -m connmark --mark %d -j DROP",
                 (firewall_config.firewall_enabled == false)?"D":"I", devname,FIREWALL_MARK);
            ds_system_call(command,strlen(command));
        }
      }
      else if ((firewall_config.firewall_enabled == true) &&
               (firewall_config.firewall_pkts_allowed == true))
      {
        /*Clean up previous rules*/
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D PREROUTING -i %s -m connmark --mark %d -j DROP",
                 devname,FIREWALL_MARK);
        ds_system_call(command,strlen(command));

        /*Default Rule for DL Firewall - Whitelisiting*/
         snprintf(command, MAX_COMMAND_STR_LEN,
                  "iptables -t mangle -I PREROUTING -i %s  -j DROP",devname);
         ds_system_call(command,strlen(command));
         snprintf(command, MAX_COMMAND_STR_LEN,
                  "iptables -t mangle -I PREROUTING -i %s -m connmark --mark %d -j ACCEPT",
                   devname,FIREWALL_MARK);
         ds_system_call(command,strlen(command));

        if(firewall_config.enable_ul_firewall == true)
        {
            snprintf(command, MAX_COMMAND_STR_LEN,
                     "iptables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j DROP",
                     devname,FIREWALL_MARK);
            ds_system_call(command,strlen(command));

         /*Default Rule for UL Firewall - Whitelisiting*/
          snprintf(command, MAX_COMMAND_STR_LEN,
                   "iptables -t mangle -I POSTROUTING -o %s  -j DROP",devname);
          ds_system_call(command,strlen(command));

          if(QcMapMgr && QcMapMgr->packet_stats_enabled)
          {
            snprintf(command, MAX_COMMAND_STR_LEN,
                     "iptables -t mangle -I POSTROUTING -o %s -m connmark --mark %d -j TRAFFIC_ACCT",
                     devname,FIREWALL_MARK);
          }
          else
          {
            snprintf(command, MAX_COMMAND_STR_LEN,
                    "iptables -t mangle -I POSTROUTING -o %s -m connmark --mark %d -j ACCEPT",
                     devname,FIREWALL_MARK);
          }
           ds_system_call(command,strlen(command));
        }

        //Flush the Ipv4 WWAN conntrack entries
        if (firewall_config.num_firewall_entries == 0)
        {
          if(DeleteExternalIpv4Conntracks())
          {
            LOG_MSG_ERROR("Unable to delete external Ipv4 conntracks", 0, 0, 0);
            return false;
          }
        }
      }
    }
    break;
    case QCMAP_MSGR_IP_FAMILY_V6_V01:
    {
      if ((firewall_config.firewall_enabled == false) ||
         ((firewall_config.firewall_enabled == true) &&
          (firewall_config.firewall_pkts_allowed == false)))
      {
          /* Flush the default ipv6 firewall rules. */
        if(firewall_config.firewall_enabled == false)
        {
          UpdateIPv6FirewallDefaultRules(devname, true);
        }
        else
        {
          UpdateIPv6FirewallDefaultRules(devname, false);
        }
      }
      else if ((firewall_config.firewall_enabled == true) &&
               (firewall_config.firewall_pkts_allowed == true))
      {
          /* Add the default ipv6 firewall rules. */
          UpdateIPv6FirewallDefaultRules(devname, false);

          //Flush the Ipv6 WWAN conntrack entries
          if (firewall_config.num_firewall_entries == 0)
          {
            if (DeleteExternalIpv6Conntracks())
            {
              LOG_MSG_ERROR("Unable to delete external Ipv6 conntracks", 0, 0, 0);
              return false;
            }
          }
      }
    }
    break;
    default:
    {
      LOG_MSG_ERROR("Incorrect Interface passed ip_family = %d",ip_family,0,0);
    }
    break;
  }

  return true;
}


/*===========================================================================
  FUNCTION CleanIPv4MangleTable
==========================================================================*/
/*!
@brief
  Flush IPV4 firewall entries

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
void QCMAP_Firewall::CleanIPv4MangleTable(void)
{
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;
  qmi_error_type_v01 qmi_err_num;
  char command[MAX_COMMAND_STR_LEN]={0};
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int i;
  qcmap_firewall_entry_list_t* firewallList;
  ds_dll_el_t * node = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);


  LOG_MSG_INFO1("QCMAP_ConnectionManager::cleanIPv4MangleTable() Enter",0,0,0);
  if( QcMapBackhaul == NULL )
  {
    LOG_MSG_ERROR("NULL QcMapBackhaul object",0,0,0);
    return ;
  }

  /* Reset NetDev in Firewall. */
  this->UpdateNetDevInXML(NULL);

  // Delete default firewall rules
  strlcpy(devname, QcMapBackhaul->wan_cfg.ipv4_interface, sizeof(devname));
  snprintf(command,MAX_COMMAND_STR_LEN,"iptables -t mangle -D PREROUTING -i %s -j DROP",
           devname);
  ds_system_call(command,strlen(command));

  snprintf(command,MAX_COMMAND_STR_LEN,"iptables -t mangle -D POSTROUTING -o %s -j DROP",
           devname);
  ds_system_call(command,strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j DROP",
           devname,FIREWALL_MARK);
  ds_system_call(command,strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle -D PREROUTING -i %s -m connmark --mark %d -j DROP",
           devname,FIREWALL_MARK);
  ds_system_call(command,strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle -D PREROUTING -i %s -m connmark --mark %d -j ACCEPT",
           devname, FIREWALL_MARK);
  ds_system_call(command,strlen(command));

  if(QcMapMgr && QcMapMgr->packet_stats_enabled)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j TRAFFIC_ACCT",
             devname,FIREWALL_MARK);
  }
  else
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j ACCEPT",
            devname,FIREWALL_MARK);
  }
   ds_system_call(command,strlen(command));

  if (firewall_config.firewall_enabled)
  {
    firewallList = &(firewall_config.extd_firewall_entries);

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    for (i = 0; i < firewall_config.num_firewall_entries && node; i++)
    {
      if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V4 )
      {
        SetFirewallV4(firewall_entry,false, &qmi_err_num);
      }
      node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }
}

/*===========================================================================
  FUNCTION UpdateIPv6FirewallDefaultRules
==========================================================================*/
/*!
@brief
  Flush IPV6 default firewall rules

@parameters
  - devname
  - delete

@return
   none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Firewall::UpdateIPv6FirewallDefaultRules(char *devname, boolean del)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  LOG_MSG_INFO1("Updating default IPv6 rules: %s, decision: %d", devname, del, 0);

  /*====Whitelisting Rules=====*/
  /*Delete all if del. & install Default rules for Whitelisting*/
  if((del == true) ||
     (firewall_config.firewall_enabled == true &&
      firewall_config.firewall_pkts_allowed == true))
  {
    /*cleanup previous rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
               "ip6tables -t mangle -D PREROUTING -i %s -m connmark --mark %d -j DROP",
               devname,FIREWALL_MARK);
    ds_system_call(command,strlen(command));

    /*==== DL Whitelisting Rules=====*/
    /*Default Rule for DL Firewall - Whitelisiting*/
    snprintf(command, MAX_COMMAND_STR_LEN,
              "ip6tables -t mangle -%s PREROUTING -i %s -j DROP",
              (del == true) ? "D" : "I",devname);
    ds_system_call(command,strlen(command));

    snprintf(command, MAX_COMMAND_STR_LEN,
              "ip6tables -t mangle -%s PREROUTING -i %s -m connmark --mark %d -j ACCEPT",
              (del == true) ? "D" : "I",devname,FIREWALL_MARK);
    ds_system_call(command,strlen(command));

    /* NS for DL Firewall - Whitelisiting*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -t mangle -%s PREROUTING -i %s -p icmpv6 --icmpv6-type neighbor-solicitation -j ACCEPT",
             (del == true) ? "D" : "I", devname);
    ds_system_call(command,strlen(command));

    /* NA for DL Firewall - Whitelisiting*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -t mangle -%s PREROUTING -i %s -p icmpv6 --icmpv6-type neighbor-advertisement -j ACCEPT",
             (del == true) ? "D" : "I", devname);
    ds_system_call(command,strlen(command));

    /* RS for DL Firewall - Whitelisiting*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -t mangle -%s PREROUTING -i %s -p icmpv6 --icmpv6-type router-solicitation -j ACCEPT",
             (del == true) ? "D" : "I", devname);
    ds_system_call(command,strlen(command));

    /* RA for DL Firewall - Whitelisiting*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -t mangle -%s PREROUTING -i %s -p icmpv6 --icmpv6-type router-advertisement -j ACCEPT",
             (del == true) ? "D" : "I", devname);
    ds_system_call(command,strlen(command));

    /*==== UL Whitelisting Rules=====*/
    if(firewall_config.enable_ul_firewall == true)
    {
      /*Clean Up previous rules*/
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip6tables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j DROP",
               devname,FIREWALL_MARK);
      ds_system_call(command,strlen(command));

      /*Default Rule for UL Firewall - Whitelisiting*/
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip6tables -t mangle -%s POSTROUTING -o %s  -j DROP",
               (del == true) ? "D" : "I",devname);
      ds_system_call(command,strlen(command));

      if(QcMapMgr && QcMapMgr->packet_stats_enabled)
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -%s POSTROUTING -o %s -m connmark --mark %d -j TRAFFIC_ACCT",
                 (del == true) ? "D" : "I",devname,FIREWALL_MARK);
      else
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -%s POSTROUTING -o %s -m connmark --mark %d -j ACCEPT",
                (del == true) ? "D" : "I",devname,FIREWALL_MARK);
      ds_system_call(command,strlen(command));

       /* NS for UL Firewall - Whitelisiting*/
      snprintf(command, MAX_COMMAND_STR_LEN,
        "ip6tables -t mangle -%s POSTROUTING -o %s -p icmpv6 --icmpv6-type neighbor-solicitation -j ACCEPT",
              (del == true) ? "D" : "I", devname);
      ds_system_call(command,strlen(command));

      /* NA for UL Firewall - Whitelisiting*/
      snprintf(command, MAX_COMMAND_STR_LEN,
        "ip6tables -t mangle -%s POSTROUTING -o %s -p icmpv6 --icmpv6-type neighbor-advertisement -j ACCEPT",
               (del == true) ? "D" : "I", devname);
      ds_system_call(command,strlen(command));

      /* RS for UL Firewall - Whitelisiting*/
      snprintf(command, MAX_COMMAND_STR_LEN,
        "ip6tables -t mangle -%s POSTROUTING -o %s -p icmpv6 --icmpv6-type router-solicitation -j ACCEPT",
               (del == true) ? "D" : "I", devname);
      ds_system_call(command,strlen(command));

      /* RA for UL Firewall - Whitelisiting*/
      snprintf(command, MAX_COMMAND_STR_LEN,
        "ip6tables -t mangle -%s POSTROUTING -o %s -p icmpv6 --icmpv6-type router-advertisement -j ACCEPT",
              (del == true) ? "D" : "I", devname);
      ds_system_call(command,strlen(command));
    }
  }
  else
  {
    /*====Blacklisting Rules=====*/
    /*Clean Up previous rules*/
    snprintf(command, MAX_COMMAND_STR_LEN,
              "ip6tables -t mangle -D PREROUTING -i %s -m connmark --mark %d -j ACCEPT",
              devname,FIREWALL_MARK);
    ds_system_call(command,strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN,
              "ip6tables -t mangle -D PREROUTING -i %s -j DROP",devname);
    ds_system_call(command,strlen(command));

    /*==== DL Blacklisting Rules=====*/
    /*Default Rule for DL Firewall - Blacklisting*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -t mangle -%s PREROUTING -i %s -m connmark --mark %d -j DROP",
             (del == true) ? "D" : "I",devname,FIREWALL_MARK);
    ds_system_call(command,strlen(command));

    /*==== UL Blacklisting Rules=====*/
    if(firewall_config.enable_ul_firewall == true)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
                "ip6tables -t mangle -D POSTROUTING -o %s  -j DROP",devname);
      ds_system_call(command,strlen(command));
      if(QcMapMgr && QcMapMgr->packet_stats_enabled)
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j TRAFFIC_ACCT",
                 devname,FIREWALL_MARK);
      else
        snprintf(command, MAX_COMMAND_STR_LEN,
                "ip6tables -t mangle -D POSTROUTING -o %s -m connmark --mark %d -j ACCEPT",
                devname,FIREWALL_MARK);
      ds_system_call(command,strlen(command));

      /*Default Rule for UL Firewall - Blacklisiting*/
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip6tables -t mangle -%s POSTROUTING -o %s -m connmark --mark %d -j DROP",
               (del == true) ? "D" : "I",devname,FIREWALL_MARK);
      ds_system_call(command,strlen(command));
    }
  }
}
/*===========================================================================
  FUNCTION cleanIPv6MangleTable
==========================================================================*/
/*!
@brief
  Flush IPV6 firewall entries

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
void QCMAP_Firewall::CleanIPv6MangleTable(void)
{
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  qcmap_msgr_firewall_entry_conf_t *firewall_entry = NULL;
  qmi_error_type_v01 qmi_err_num;
  char command[MAX_COMMAND_STR_LEN]={0};
  int i;
  qcmap_firewall_entry_list_t* firewallList;
  ds_dll_el_t * node = NULL;

  LOG_MSG_INFO1("QCMAP_ConnectionManager::cleanIPv6MangleTable() Enter",0,0,0);

  if (!QcMapBackhaul->enable_ipv6)
  {
    LOG_MSG_ERROR("IPv6 backhaul not up",0,0,0);
    return false;
  }

  strlcpy(devname, QcMapBackhaul->wan_cfg.ipv6_interface, sizeof(devname));

  /* Flush the default ipv6 firewall rules. */
  UpdateIPv6FirewallDefaultRules(devname, true);

  /* Reset NetDev in Firewall. */
  this->UpdateNetDevInXML(NULL);

  // Enter IPV6 Firewall rules
  if (firewall_config.firewall_enabled)
  {
    firewallList = &(firewall_config.extd_firewall_entries);

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    for (i = 0; i < firewall_config.num_firewall_entries && node; i++)
    {
      if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V6 )
      {
        SetFirewallV6(firewall_entry, false, &qmi_err_num);
      }
      node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }
}

/*===========================================================================
  FUNCTION EnableIPv6Firewall
==========================================================================*/
/*!
@brief
  EnableIPv6Firewall based on the WAN Connect

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Firewall::EnableIPV6DefaultFirewall(void)
{
  LOG_MSG_INFO1("QCMAP_Firewall::EnableIPV6DefaultFirewall() Enter",0,0,0);
  qcmap_msgr_ip_family_enum_v01 ip_family;

  ip_family = QCMAP_MSGR_IP_FAMILY_V6_V01;
  if (!SetDefaultFirewallRule(ip_family))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV6",0,0,0);
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION EnableIPv6Firewall
==========================================================================*/
/*!
@brief
  EnableIPv6Firewall based on the WAN Connect

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Firewall::EnableIPV6Firewall(void)
{
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();

  qcmap_msgr_ip_family_enum_v01  ip_family = QCMAP_MSGR_IP_FAMILY_V6_V01;
  qcmap_msgr_firewall_entry_conf_t *firewall_entry = NULL;
  int i =0;
  qmi_error_type_v01 qmi_err_num;
  ds_dll_el_t * node = NULL;
  qcmap_firewall_entry_list_t* firewallList = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN *wwan_obj = QcMapBackhaul->QcMapBackhaulWWAN;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;

  if (QcMapBackhaulWLANMgr)
  {
    LOG_MSG_INFO1("QCMAP_Firewall::EnableIPV6Firewall() Enter is_sta_enabled  = %x",
                  QcMapBackhaulWLANMgr->IsSTAAvailableV6(),0,0);

    if (QcMapBackhaulWLANMgr->IsAPSTABridgeActivated())
    {
      LOG_MSG_INFO1("QCMAP_Firewall::EnableIPV6Firewall() Not needed in Bridge Mode",0,0,0);
      return true;
    }
  }

  // Flush firewall rules
  CleanIPv6MangleTable();

  // Enter default firewall rule only for IPV6
  ip_family = QCMAP_MSGR_IP_FAMILY_V6_V01;

  if (!SetDefaultFirewallRule(ip_family))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV6",0,0,0);
  }

  if (firewall_config.firewall_enabled)
  {
    firewallList = &(firewall_config.extd_firewall_entries);

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    for (i = 0; i < firewall_config.num_firewall_entries && node; i++)
    {
      if (firewall_entry && firewall_entry->filter_spec.ip_vsn == IP_V6 )
      {
        SetFirewallV6(firewall_entry,true, &qmi_err_num);
      }
      node = ds_dll_next (node, (const void**)(&firewall_entry));
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDefaultFirewall(void)
==========================================================================*/
/*!
@brief
  EnableIPv6Firewall based on the WAN Connect

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Firewall::SetDefaultFirewall(void)
{
  qcmap_msgr_ip_family_enum_v01 ip_family;

  LOG_MSG_INFO1("QCMAP_Firewall::SetDefaultFirewall() Enter",0,0,0);

  // Adding default firewall rules for sta and non-sta mode
  ip_family = QCMAP_MSGR_IP_FAMILY_V4_V01;
  if (!SetDefaultFirewallRule(ip_family))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV4",0,0,0);
  }
  ip_family = QCMAP_MSGR_IP_FAMILY_V6_V01;
  if (!SetDefaultFirewallRule(ip_family))
  {
    LOG_MSG_ERROR("Default firewall rules not added for IPV6",0,0,0);
  }
}

/*===========================================================================
  FUNCTION GetFireWallEntry
==========================================================================*/
/*!
@brief
  Gets the firewall entry pointed by the handle.

@parameters
  qcmap_msgr_get_firewall_entry_resp_msg_v01 *resp_msg
  uint32_t handle

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
boolean QCMAP_Firewall::GetFireWallEntry
(
  qcmap_msgr_get_firewall_entry_resp_msg_v01 *resp_msg,
  uint32_t handle,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_firewall_entry_conf_t *firewall_entry;
  uint8_t ip_vsn,next_hdr_prot=0;
  ds_dll_el_t * node = NULL;
  qcmap_firewall_entry_list_t* firewallList = NULL;
  qcmap_msgr_firewall_entry_conf_t curr_firewall_entry;
  pugi::xml_document xml_file;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL", 0,0,0);
    return false;
  }

  if (!(IS_DEFAULT_PROFILE(GetProfileHandle())))
  {
    if (!(IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(QcMapMgr->target)) )
    {
      LOG_MSG_ERROR("Firewall for current target(%d) not supported", QcMapMgr->target,0,0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }
  }

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  /* Update the XML file and return back if the object is not yet created. */
  if (!QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    LOG_MSG_INFO1("Obtaining Firewall entry from XML ", 0, 0, 0);
    if(!(searchFirewallByHandleInXML(&xml_file, &curr_firewall_entry, handle)))
    {
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }
    firewall_entry = &curr_firewall_entry;
  }
  else
  {
    LOG_MSG_INFO1("Obtaining Firewall entry from List", 0, 0, 0);
    firewallList = &(firewall_config.extd_firewall_entries);
    if (firewall_config.num_firewall_entries == 0)
    {
      LOG_MSG_ERROR("No entries configured",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }

    node = firewallList->firewallEntryListHead;
    node = ds_dll_next (node, (const void**)(&firewall_entry));

    while(firewall_entry->firewall_handle != handle && node)
      node = ds_dll_next (node, (const void**)(&firewall_entry));

    if(!node)
    {
      LOG_MSG_ERROR("Invalid handle",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }
  }

  Dump_firewall_conf(firewall_entry);

  ip_vsn = firewall_entry->filter_spec.ip_vsn;

  resp_msg->ip_version_valid = TRUE;
  resp_msg->direction_valid = TRUE;
  resp_msg->direction = (qcmap_msgr_firewall_rule_direction_v01)firewall_entry->firewall_direction;
  LOG_MSG_INFO1("Direction %d", firewall_entry->firewall_direction, 0, 0);
  switch(ip_vsn)
  {
    LOG_MSG_INFO1("IP_VSN %d", ip_vsn , 0, 0);

    case IP_V4:

      resp_msg->ip_version = QCMAP_MSGR_IP_FAMILY_V4_V01;
      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_SRC_ADDR )
      {
        resp_msg->ip4_src_addr_valid = TRUE;
        resp_msg->ip4_src_addr.addr =
          (firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
        resp_msg->ip4_src_addr.subnet_mask =
          (firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
        LOG_MSG_INFO1("IP4 source address is:",0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr);
        LOG_MSG_INFO1("IP4 subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_DST_ADDR )
      {
        resp_msg->ip4_dst_addr_valid = TRUE;
        resp_msg->ip4_dst_addr.addr =
          (firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr);
        resp_msg->ip4_dst_addr.subnet_mask =
          (firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr);
        LOG_MSG_INFO1("IP4 dst address is:",0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr);
        LOG_MSG_INFO1("IP4 dst subnet mask is:", 0, 0, 0);
        IPV4_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_TOS )
      {
        resp_msg->ip4_tos_valid = TRUE;
        resp_msg->ip4_tos.value =
          firewall_entry->filter_spec.ip_hdr.v4.tos.val;
        resp_msg->ip4_tos.mask =
          firewall_entry->filter_spec.ip_hdr.v4.tos.mask;
        LOG_MSG_INFO1("IP4 TOS val %d mask %d",
            resp_msg->ip4_tos.value ,
            resp_msg->ip4_tos.mask, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask &
           IPFLTR_MASK_IP4_NEXT_HDR_PROT )
      {
        LOG_MSG_INFO1("IP4 next header protocol %d ", resp_msg->next_hdr_prot, 0, 0);
        resp_msg->next_hdr_prot_valid = TRUE;
        resp_msg->next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
      }
      break;

    case IP_V6:

      resp_msg->ip_version = QCMAP_MSGR_IP_FAMILY_V6_V01;
      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_SRC_ADDR )
      {
        memcpy(resp_msg->ip6_src_addr.addr,
               firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        resp_msg->ip6_src_addr.prefix_len =
          firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len;
        resp_msg->ip6_src_addr_valid = TRUE;

        LOG_MSG_INFO1("IPV6 src_addr is:", 0, 0, 0);
        IPV6_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IP6 prefix length %d  ",
                      resp_msg->ip6_src_addr.prefix_len, 0, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_DST_ADDR )
      {

        memcpy(resp_msg->ip6_dst_addr.addr,
               firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
        resp_msg->ip6_dst_addr.prefix_len =
          firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len;
        resp_msg->ip6_dst_addr_valid = TRUE;
        LOG_MSG_INFO1("IPV6 dst_addr is:", 0, 0, 0);
        IPV6_ADDR_MSG(firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr64);
        LOG_MSG_INFO1("IP6 prefix length %d  ",
                      resp_msg->ip6_dst_addr.prefix_len, 0, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_TRAFFIC_CLASS )
      {
        resp_msg->ip6_trf_cls.value =
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val;
        resp_msg->ip6_trf_cls.mask =
          firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask;
        resp_msg->ip6_trf_cls_valid = TRUE;
        LOG_MSG_INFO1("trf class val %d  mask %d",
                      resp_msg->ip6_trf_cls.value,
                      resp_msg->ip6_trf_cls.mask, 0);
      }

      if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask &
           IPFLTR_MASK_IP6_NEXT_HDR_PROT )
      {
        resp_msg->next_hdr_prot_valid = TRUE;
        next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
        resp_msg->next_hdr_prot = next_hdr_prot;
        LOG_MSG_INFO1("IP6 next header protocol  %d  ", next_hdr_prot,0, 0);
      }
      break;
    default:
      LOG_MSG_ERROR("Unsupported IP protocol %d", ip_vsn, 0, 0);
  }

  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        resp_msg->tcp_udp_src.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        resp_msg->tcp_udp_src.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        resp_msg->tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("TCP src port  %d  range %d",
                      resp_msg->tcp_udp_src.port,
                      resp_msg->tcp_udp_src.range, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_DST_PORT )
      {
        resp_msg->tcp_udp_dst.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
        resp_msg->tcp_udp_dst.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        resp_msg->tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("TCP dst port  %d  range %d",
                      resp_msg->tcp_udp_dst.port,
                      resp_msg->tcp_udp_dst.range, 0);
      }
      break;

    case PS_IPPROTO_UDP:

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_UDP_SRC_PORT )
      {
        resp_msg->tcp_udp_src.port =
          (firewall_entry->filter_spec.next_prot_hdr.udp.src.port);
        resp_msg->tcp_udp_src.range =
          firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        resp_msg->tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("UDP src port  %d  range %d",
                      resp_msg->tcp_udp_src.port,
                      resp_msg->tcp_udp_src.range, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_UDP_DST_PORT )
      {
        resp_msg->tcp_udp_dst.port =
          (firewall_entry->filter_spec.next_prot_hdr.udp.dst.port);
        resp_msg->tcp_udp_dst.range =
          firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        resp_msg->tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("UDP dst port  %d  range %d",
                      resp_msg->tcp_udp_dst.port ,
                      resp_msg->tcp_udp_dst.range, 0);
      }
      break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:

      if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask
           & IPFLTR_MASK_ICMP_MSG_TYPE )
      {
        resp_msg->icmp_type =
         firewall_entry->filter_spec.next_prot_hdr.icmp.type;
        resp_msg->icmp_type_valid = TRUE;
        LOG_MSG_INFO1("ICMP type  %d  ", resp_msg->icmp_type , 0, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask
           & IPFLTR_MASK_ICMP_MSG_CODE )
      {
        resp_msg->icmp_code =
         firewall_entry->filter_spec.next_prot_hdr.icmp.code;
        resp_msg->icmp_code_valid = TRUE;
        LOG_MSG_INFO1("ICMP code  %d  ", resp_msg->icmp_code , 0, 0);
      }
      break;

    case PS_IPPROTO_ESP:

      if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask
           & IPFLTR_MASK_ESP_SPI )
      {
        resp_msg->esp_spi =
         (firewall_entry->filter_spec.next_prot_hdr.esp.spi);
        resp_msg->esp_spi_valid = TRUE;
        LOG_MSG_INFO1("ESP spi %d", resp_msg->esp_spi , 0, 0);
      }
      break;

    case PS_IPPROTO_TCP_UDP:

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
           & IPFLTR_MASK_TCP_UDP_SRC_PORT )
      {
        resp_msg->tcp_udp_src.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port);
        resp_msg->tcp_udp_src.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range;
        resp_msg->tcp_udp_src_valid = TRUE;
        LOG_MSG_INFO1("TCP_UDP port %d range %d",
                      resp_msg->tcp_udp_src.port ,
                      resp_msg->tcp_udp_src.range, 0);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask
           & IPFLTR_MASK_TCP_UDP_DST_PORT )
      {
        resp_msg->tcp_udp_dst.port =
          (firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port);
        resp_msg->tcp_udp_dst.range =
          firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range;
        resp_msg->tcp_udp_dst_valid = TRUE;
        LOG_MSG_INFO1("TCP_UDP port %d range %d",
                      resp_msg->tcp_udp_dst.port ,
                      resp_msg->tcp_udp_dst.range, 0);
      }
     break;

    default:
      LOG_MSG_ERROR("Unsupported next header protocol %d",next_hdr_prot, 0, 0);
  }

  return true;
}


/*===========================================================================
  FUNCTION Dump_firewall_conf
==========================================================================*/
/*!
@brief
  Displays the values of filrewall entry.

@parameters
  qcmap_msgr_firewall_entry_conf_t *firewall_entry

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static void QCMAP_Firewall::Dump_firewall_conf( qcmap_msgr_firewall_entry_conf_t *firewall_entry)
{
  char str[INET6_ADDRSTRLEN];
  int next_hdr_prot = 0;

  if( firewall_entry !=NULL )
  {
    LOG_MSG_INFO1("Start Displaying firewall configuration of handle =%d",
                  firewall_entry->firewall_handle,0,0);

      if ( firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL )
      {
        LOG_MSG_INFO1("Direction : UL", 0, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1("Direction : DL", 0, 0, 0);
      }

      if ( firewall_entry->filter_spec.ip_vsn == IP_V4 )
      {
        LOG_MSG_INFO1("Ip version : IPv4",0,0,0);

        if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR )
        {
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.src.addr.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("SRC Addr : %s",str,0,0);
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("SRC Addr Mask : %s",str,0,0);
        }

        if ( firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR )
        {
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("Dest Addr : %s",str,0,0);
          readable_addr(AF_INET,(const uint32_t *)&firewall_entry->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr,(char *)&str);
          LOG_MSG_INFO1("Dest Addr Mask : %s",str,0,0);
        }

        if (  firewall_entry->filter_spec.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS )
        {
          LOG_MSG_INFO1("Tos value : %x ",firewall_entry->filter_spec.ip_hdr.v4.tos.val,0,0);
          LOG_MSG_INFO1("Tos Mask : %x ",firewall_entry->filter_spec.ip_hdr.v4.tos.mask,0,0);
        }

        if( firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot & IPFLTR_MASK_IP4_NEXT_HDR_PROT )
        {
          next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v4.next_hdr_prot;
        }
      }
      else
      {
        LOG_MSG_INFO1("Ip version : Ipv6",0,0,0);

        if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR )
        {
          readable_addr(AF_INET6,(uint32 *)&firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr32,(char *)&str);
          LOG_MSG_INFO1("Src Addr : %s ",str,0,0);
          LOG_MSG_INFO1("Src Prefixlen : %d ",firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len,0,0);
        }

        if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR )
        {
          readable_addr(AF_INET6,(uint32 *)&firewall_entry->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr32,(char *)&str);
          LOG_MSG_INFO1("Dst Addr : %s ",str,0,0);
          LOG_MSG_INFO1("Dst Prefixlen : %d ",firewall_entry->filter_spec.ip_hdr.v6.dst.prefix_len,0,0);
        }

        if( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS )
        {
          LOG_MSG_INFO1("IPV6 Traffic class value: %d", firewall_entry->filter_spec.ip_hdr.v6.trf_cls.val, 0, 0);
          LOG_MSG_INFO1("IPV6 Traffic class mask: %d", firewall_entry->filter_spec.ip_hdr.v6.trf_cls.mask, 0, 0);
        }

        if( firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot & IPFLTR_MASK_IP6_NEXT_HDR_PROT )
        {
          next_hdr_prot = firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot;
        }
      }

       switch( next_hdr_prot )
      {
        case PS_IPPROTO_TCP:

          LOG_MSG_INFO1("Protocol : TCP",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT )
          {
            LOG_MSG_INFO1("Src port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.src.port,0,0);
            LOG_MSG_INFO1("Src portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.src.range,0,0);
          }

          if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
          {
            LOG_MSG_INFO1("Dst port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port,0,0);
            LOG_MSG_INFO1("Dst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range,0,0);
          }
          break;

        case PS_IPPROTO_UDP:

          LOG_MSG_INFO1("Protocol: UDP",0,0,0);

          if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT )
          {
            LOG_MSG_INFO1("Src port : %d",firewall_entry->filter_spec.next_prot_hdr.udp.src.port,0,0);
            LOG_MSG_INFO1("Dst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.udp.src.range,0,0);
          }

          if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
          {
            LOG_MSG_INFO1("Dst port : %d",firewall_entry->filter_spec.next_prot_hdr.udp.dst.port,0,0);
            LOG_MSG_INFO1("Dst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.udp.dst.range,0,0);
          }
          break;

        case PS_IPPROTO_TCP_UDP:

          LOG_MSG_INFO1("Protocol: TCP_UDP",0,0,0);

          if( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT )
          {
            LOG_MSG_INFO1("Src port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port,0,0);
            LOG_MSG_INFO1("Dst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range,0,0);
          }

          if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_TCP_UDP_DST_PORT )
          {
            LOG_MSG_INFO1("Dst port : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port,0,0);
            LOG_MSG_INFO1("Dst portrange : %d",firewall_entry->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range,0,0);
          }
          break;
        case PS_IPPROTO_ICMP:

          LOG_MSG_INFO1("Protocol : ICMP",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE )
          {
            LOG_MSG_INFO1("Icmp Type: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.type,0,0);
          }

          if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE )
          {
            LOG_MSG_INFO1("Icmp Code: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.code,0,0);
          }
          break;

        case PS_IPPROTO_ICMP6:

          LOG_MSG_INFO1("Protocol : ICMP6",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE )
          {
            LOG_MSG_INFO1("ICMPv6 Type: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.type,0,0);
          }

          if ( firewall_entry->filter_spec.next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE )
          {
            LOG_MSG_INFO1("ICMPv6 Code: %d ",firewall_entry->filter_spec.next_prot_hdr.icmp.code,0,0);
          }
          break;

        case PS_IPPROTO_ESP:

          LOG_MSG_INFO1("Protocol : ESP",0,0,0);

          if ( firewall_entry->filter_spec.next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI )
          {
            LOG_MSG_INFO1("ESP spi : %d",firewall_entry->filter_spec.next_prot_hdr.esp.spi,0,0);
          }
          break;

        default:

          LOG_MSG_INFO1("Unsupported protocol \n",0,0,0);
          break;
      }
    }
  if(firewall_entry)
  {
    LOG_MSG_INFO1("End of Firewall configuration of handle =%d ",firewall_entry->firewall_handle,0,0);
  }
}


/*===========================================================================
  FUNCTION AddFireWallEntry
==========================================================================*/
/*!
@brief
  Checks for the duplicate entries of firewall entryi, if unique
  adds the firewall entry and update the same in the mobileap configuration file.

@parameters
  qcmap_msgr_firewall_conf_t* extd_firewall_conf

@return
On success, returns QCMAP_CM_SUCCESS.
On error, return QCMAP_CM_ERROR and places the error condition value in
*qcmap_cm_errno.

 qcmap_cm_errno Values
 ----------------
 QCMAP_MSGR_ENTRY_FULL          max number of firewall entries are added
 QCMAP_MSGR_ENTRY_PRESENT       if the firewall entry is already added


@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::AddFireWallEntry
(
  qcmap_msgr_firewall_conf_t* extd_firewall_conf,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i, add_firewall_entry = 0;
  ds_dll_el_t * node = NULL;
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL, *new_firewall_entry = NULL;
  qcmap_firewall_entry_list_t* firewallList = NULL;
  int firewall_count = 0;
  boolean enable_firewall, pkts_allowed, upnp_pinhole_flag;
  pugi::xml_document xml_file;
  boolean retVal = false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  *qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01;

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL", 0,0,0);
    return false;
  }

  if (!(IS_DEFAULT_PROFILE(GetProfileHandle())))
  {
    if (!(IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(QcMapMgr->target)) )
    {
      LOG_MSG_ERROR("Firewall for current target(%d) not supported", QcMapMgr->target,0,0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }
  }

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return QCMAP_CM_ERROR;
  }
  LOG_MSG_ERROR("upnp_pinhole_handle: %d ip vsn=%d",
                extd_firewall_conf->extd_firewall_entry.upnp_pinhole_handle,extd_firewall_conf->extd_firewall_entry.filter_spec.ip_vsn,0);

  retVal = GetSetFirewallConfigFromXML(GET_VALUE, &enable_firewall, &pkts_allowed,&upnp_pinhole_flag);
  if (retVal == false)
  {
    LOG_MSG_ERROR("Unable to add firewall entry", 0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return QCMAP_CM_ERROR;
  }

/* CHECK FOR PINHOLE*/
  if(extd_firewall_conf->extd_firewall_entry.upnp_pinhole_handle &&
     extd_firewall_conf->extd_firewall_entry.filter_spec.ip_vsn == IP_V6)
  {
    if(upnp_pinhole_flag == FALSE )
    {
      LOG_MSG_ERROR("The UPNP Inbound Pinhole flag is not set. Adding Entry not allowed\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return QCMAP_CM_ERROR;
    }
  }
  LOG_MSG_INFO1("Firewall direction %d",
                            extd_firewall_conf->extd_firewall_entry.firewall_direction,0,0);

  extd_firewall_conf->extd_firewall_entry.firewall_handle = rand();
  LOG_MSG_INFO1("Adding Firewall entry to List ", 0, 0, 0);
  firewallList = &(firewall_config.extd_firewall_entries);

  if (firewall_config.num_firewall_entries >= QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01)
  {
    /* we reach MAX entries */
    LOG_MSG_ERROR("Exceeds maximum number of firewall entries\n",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return QCMAP_MSGR_ENTRY_FULL;
  }

  node = firewallList->firewallEntryListHead;
  node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
  for ( i = 0; i < firewall_config.num_firewall_entries && node; i++ )
  {
    if ( (memcmp(&(extd_firewall_conf->extd_firewall_entry.filter_spec),
          &(curr_firewall_entry->filter_spec),
          sizeof(ip_filter_type)) == 0) )
    {
      LOG_MSG_ERROR("Firewall entry is already present\n",0,0,0);
      /* we already added this one */
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return QCMAP_MSGR_ENTRY_PRESENT;
    }
    node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
  }

  // Add firewall entry only if firewall is enabled or UPNP Pinhole is being added
  if (((firewall_config.firewall_enabled && QcMapBackhaul->IsBackhaulRouterModeEnabled()) ||
       (extd_firewall_conf->extd_firewall_entry.upnp_pinhole_handle &&
        extd_firewall_conf->extd_firewall_entry.filter_spec.ip_vsn == IP_V6))
        && SetFirewall(&(extd_firewall_conf->extd_firewall_entry),
                       TRUE, qmi_err_num ) != QCMAP_CM_SUCCESS )
  {
     if( *qmi_err_num != QMI_ERR_INTERFACE_NOT_FOUND_V01 )
     {
       LOG_MSG_ERROR("SetFirewall Entry failed\n",0,0,0);
       return  QCMAP_CM_ERROR;
     }
  }
  if( addFirewallEntryToList(&(extd_firewall_conf->extd_firewall_entry),
      new_firewall_entry) == false)
  {
    LOG_MSG_ERROR("Adding Firewall Entry to Firewall list failed.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return QCMAP_CM_ERROR;
  }
  /* If Firewall enabled then set qmi_err_num to NONE. */
  *qmi_err_num = QMI_ERR_NONE_V01;
  add_firewall_entry = 1;

  LOG_MSG_INFO1("Applied firewall entry.", 0, 0, 0);

  /* if it is OK, call write to XML */
  /* Check if we have already added 50 Entries */
  firewall_count = GetFirewallEntryCountFromXML(&xml_file);
  QCMAP_CM_LOG_FUNC_ENTRY();

  if(firewall_count >= QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01)
  {
    LOG_MSG_ERROR("Exceeds maximum number of firewall entries\n",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return QCMAP_MSGR_ENTRY_FULL;
  }

  if((add_firewall_entry ||!(searchFirewallByIndexInXML(&xml_file,
      &(extd_firewall_conf->extd_firewall_entry),SEARCH_FIREWALL_ENTRY))))
  {
    LOG_MSG_INFO1("Adding Firewall entry to XML file.", 0, 0, 0);
    this->addFirewallEntryToXML(&xml_file,
                                   &(extd_firewall_conf->extd_firewall_entry));
    Dump_firewall_conf(&(extd_firewall_conf->extd_firewall_entry));
  }
  else
  {
    LOG_MSG_ERROR("Firewall entry is already present\n",0,0,0);
    /* we already added this one */
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return QCMAP_MSGR_ENTRY_PRESENT;
  }


  /*we will indicate the user that interface was down but entry is stored */
  if( *qmi_err_num == QMI_ERR_INTERFACE_NOT_FOUND_V01 )
  {
    LOG_MSG_ERROR("Entry is added but not reflected in kernel iptables yet\n",0,0,0);
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_SUCCESS;
}


/*===========================================================================
  FUNCTION SetFirewall
==========================================================================*/
/*!
@brief
  Adds firewall entry to the appropriate IP tables based on IPV4 and IPV6

@parameters
  qcmap_msgr_firewall_conf_t *firewall
  boolean add_rule

@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_Firewall::SetFirewall
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  boolean add_rule,
  qmi_error_type_v01 *qmi_err_num)
{

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }

  /*If firewall is in UL direction and UL firewall is disabled then skip installing it*/
  if(add_rule && (firewall_entry->firewall_direction == QCMAP_MSGR_UL_FIREWALL) && !firewall_config.enable_ul_firewall)
    return QCMAP_CM_SUCCESS;

  if (firewall_entry->filter_spec.ip_vsn == IP_V4 )
  {
    if ( SetFirewallV4( firewall_entry, add_rule, qmi_err_num ) != QCMAP_CM_SUCCESS )
    {
      return QCMAP_CM_ERROR;
    }
  }
  else if ( firewall_entry->filter_spec.ip_vsn == IP_V6 )
  {
    if( SetFirewallV6( firewall_entry, add_rule, qmi_err_num ) != QCMAP_CM_SUCCESS )
    {
      return QCMAP_CM_ERROR;
    }
  }
  else
  {
    LOG_MSG_ERROR("Invalid IP Version %x",firewall_entry->filter_spec.ip_vsn,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_SUCCESS;
}


/*===========================================================================
  FUNCTION GetFirewallEntryCountFromXML
==========================================================================*/
/*!
@brief
  Get the firewall entry count from the XML.

@parameters
  pugi::xml_document *xml_file

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

int QCMAP_Firewall::GetFirewallEntryCountFromXML(pugi::xml_document *xml_file)
{
  pugi::xml_node root, child;

  if (!(root = GetPdnRootXML(xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall cfg corresponding to PDN",0,0,0);
    return false;
  }

  return root.select_nodes(Firewall_TAG).size();
}


/*===========================================================================
  FUNCTION GetFireWallHandleList
==========================================================================*/
/*!
@brief
  Get all the handles of firewall entries.

@parameters
  qcmap_msgr_firewall_conf_t *get_handle_list

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
boolean QCMAP_Firewall::GetFireWallHandleList
(
  qcmap_msgr_firewall_conf_t *get_handle_list,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i,len = 0;

  ds_dll_el_t * node = NULL;
  qcmap_msgr_firewall_entry_conf_t *curr_firewall_entry = NULL;
  int family;
  family=get_handle_list->ip_family;
  qcmap_msgr_firewall_entry_conf_t extd_firewall_entry;
  pugi::xml_document xml_file;
  qcmap_firewall_entry_list_t* firewallList = NULL;
  int firewall_count = 0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(GetProfileHandle());

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL", 0,0,0);
    return false;
  }

  if (!(IS_DEFAULT_PROFILE(GetProfileHandle())))
  {
    if (!(IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(QcMapMgr->target)) )
    {
      LOG_MSG_ERROR("Firewall for current target(%d) not supported", QcMapMgr->target,0,0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }
  }

  if (!xml_file.load_file(QCMAP_FIREWALL_CONFIG))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  /* Update the XML file and return back if the object is not yet created. */
  if (!QcMapBackhaul->IsBackhaulRouterModeEnabled())
  {
    firewall_count = QCMAP_Firewall::GetFirewallEntryCountFromXML(&xml_file);

    for( i=0; i< firewall_count; i++)
    {
      if(QCMAP_Firewall::searchFirewallByIndexInXML(&xml_file, &extd_firewall_entry, i))
      {
        if( family == extd_firewall_entry.filter_spec.ip_vsn)
        {
          get_handle_list->extd_firewall_handle_list.handle_list[len++] = extd_firewall_entry.firewall_handle;
          LOG_MSG_INFO1("Firewall Handle num: %d ", extd_firewall_entry.firewall_handle, 0, 0);
        }
      }
    }
    get_handle_list->extd_firewall_handle_list.num_of_entries = len;
    return true;
  }

  firewallList = &(firewall_config.extd_firewall_entries);

  node = firewallList->firewallEntryListHead;
  node = ds_dll_next (node, (const void**)(&curr_firewall_entry));

  for( i=0; i< firewall_config.num_firewall_entries && node; i++)
  {
    if( family == curr_firewall_entry->filter_spec.ip_vsn)
    {
      get_handle_list->extd_firewall_handle_list.handle_list[len++] = curr_firewall_entry->firewall_handle;
    }
    //node = node->next;
    node = ds_dll_next (node, (const void**)(&curr_firewall_entry));
  }
  get_handle_list->extd_firewall_handle_list.num_of_entries = len;
  return true;
}


/*===========================================================================
  FUNCTION GetFirewallEntryFromNode
==========================================================================*/
/*!
@brief
  Get the firewall entries from Node.

@parameters
  pugi::xml_node child,
  qcmap_msgr_firewall_entry_conf_t *firewall_conf

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

boolean QCMAP_Firewall::GetFirewallEntryFromXML
(
  pugi::xml_node child,
  qcmap_msgr_firewall_entry_conf_t *firewall_conf
)
{
  pugi::xml_node subchild;
  int next_hdr = -1;
  int type = 0, code = 0;
  uint32_t debug_string=0;
  struct in6_addr ip6_addr;
  firewall_conf->filter_spec.ip_vsn = atoi(child.child_value(IPFamily_TAG));
  firewall_conf->firewall_handle = atoi(child.child(FirewallHandle_TAG).child_value());
  firewall_conf->upnp_pinhole_handle= atoi(child.child_value(PinholeEntry_TAG));
  if (strncasecmp(child.child_value(FirewallDirection_TAG), "UL",
                  strlen("UL")) == 0)
  {
    firewall_conf->firewall_direction = QCMAP_MSGR_UL_FIREWALL;
  }
  else
  {
    /* Default is DL firewall. */
    firewall_conf->firewall_direction = QCMAP_MSGR_DL_FIREWALL;
  }
     if(firewall_conf->filter_spec.ip_vsn == IP_V4)
     {
       subchild = child.child(IPV4SourceAddress_TAG);

       if (subchild)
       {
         firewall_conf->filter_spec.ip_hdr.v4.field_mask|=IPFLTR_MASK_IP4_SRC_ADDR;
         firewall_conf->filter_spec.ip_hdr.v4.src.addr.ps_s_addr =\
           inet_addr(subchild.child(IPV4SourceIPAddress_TAG).child_value());

         firewall_conf->filter_spec.ip_hdr.v4.src.subnet_mask.ps_s_addr =\
           inet_addr(subchild.child(IPV4SourceSubnetMask_TAG).child_value());
       }

       subchild = child.child(IPV4DestinationAddress_TAG);

       if (subchild)
       {
         firewall_conf->filter_spec.ip_hdr.v4.field_mask |=IPFLTR_MASK_IP4_DST_ADDR;

         firewall_conf->filter_spec.ip_hdr.v4.dst.addr.ps_s_addr =
          inet_addr(subchild.child(IPV4DestinationIPAddress_TAG).child_value());

         firewall_conf->filter_spec.ip_hdr.v4.dst.subnet_mask.ps_s_addr =
         inet_addr(subchild.child(IPV4DestinationSubnetMask_TAG).child_value());
       }

       subchild = child.child(IPV4TypeOfService_TAG);

       if (subchild)
       {
         firewall_conf->filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
         firewall_conf->filter_spec.ip_hdr.v4.tos.val =
                               atoi(subchild.child(TOSValue_TAG).child_value());
         firewall_conf->filter_spec.ip_hdr.v4.tos.mask =
                                atoi(subchild.child(TOSMask_TAG).child_value());
        LOG_MSG_INFO1("TOS Value: %d, TOS Mask: %d",
                       firewall_conf->filter_spec.ip_hdr.v4.tos.val,
                       firewall_conf->filter_spec.ip_hdr.v4.tos.mask,0);
       }

       firewall_conf->filter_spec.ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
       firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot =
                    atoi(child.child(IPV4NextHeaderProtocol_TAG).child_value());
       next_hdr = firewall_conf->filter_spec.ip_hdr.v4.next_hdr_prot;

     }

    if(firewall_conf->filter_spec.ip_vsn == IP_V6)
    {
      subchild = child.child(IPV6SourceAddress_TAG);

      if (subchild)
      {
        firewall_conf->filter_spec.ip_hdr.v6.field_mask |=IPFLTR_MASK_IP6_SRC_ADDR;

        inet_pton(AF_INET6,subchild.child(IPV6SourceIPAddress_TAG).\
                  child_value(), &ip6_addr);

        memcpy(firewall_conf->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr8,
               ip6_addr.s6_addr,QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        firewall_conf->filter_spec.ip_hdr.v6.src.prefix_len =
                   atoi(subchild.child(IPV6SourcePrefix_TAG).child_value());
        LOG_MSG_INFO1("IPV6SourcePrefix: %d",
                      firewall_conf->filter_spec.ip_hdr.v6.src.prefix_len,0,0);
      }

      subchild = child.child(IPV6DestinationAddress_TAG);
      if (subchild)
      {
        firewall_conf->filter_spec.ip_hdr.v6.field_mask |=
               IPFLTR_MASK_IP6_DST_ADDR;
        inet_pton(AF_INET6,subchild.child(IPV6DestinationIPAddress_TAG).\
                  child_value(), &ip6_addr);
        memcpy(firewall_conf->filter_spec.ip_hdr.v6.dst.addr.in6_u.u6_addr8,
               ip6_addr.s6_addr,
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

        firewall_conf->filter_spec.ip_hdr.v6.dst.prefix_len =
         atoi(subchild.child(IPV6DestinationPrefix_TAG).child_value());

        LOG_MSG_INFO1( "IPV6DestinationPrefix: %d",
                       firewall_conf->filter_spec.ip_hdr.v6.dst.prefix_len,0,0 );
      }

      subchild = child.child(IPV6TrafficClass_TAG);
      if (subchild)
      {
        firewall_conf->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
        firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val =
                            atoi(subchild.child(TrfClsValue_TAG).child_value());

        firewall_conf->filter_spec.ip_hdr.v6.trf_cls.mask =
                            atoi(subchild.child(TrfClsMask_TAG).child_value());
        LOG_MSG_INFO1( "TrfClsValue: %d , TrfClsMask: %d",
                       firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val ,
                       firewall_conf->filter_spec.ip_hdr.v6.trf_cls.val,0 );
      }

      firewall_conf->filter_spec.ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot =
                    atoi(child.child(IPV6NextHeaderProtocol_TAG).child_value());
      next_hdr = firewall_conf->filter_spec.ip_hdr.v6.next_hdr_prot;
    }

    switch(next_hdr)
    {
      case PS_IPPROTO_TCP:
      {
        subchild = child.child("TCPSource");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask |=
              IPFLTR_MASK_TCP_SRC_PORT;
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.port =
                           atoi(subchild.child("TCPSourcePort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.range =
                           atoi(subchild.child("TCPSourceRange").child_value());
          LOG_MSG_INFO1("TCPSourcePort: %d , TCPSourceRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,0);
        }

        subchild = child.child("TCPDestination");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.tcp.field_mask |=
              IPFLTR_MASK_TCP_DST_PORT;

          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port =
                      atoi(subchild.child("TCPDestinationPort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range =
                     atoi(subchild.child("TCPDestinationRange").child_value());
          LOG_MSG_INFO1( "TCPDestinationPort: %d , TCPDestinationRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range,
                         0 );
        }
      }
      break;
      case PS_IPPROTO_UDP:
      {
        subchild = child.child("UDPSource");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.udp.field_mask |=
              IPFLTR_MASK_UDP_SRC_PORT;
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.port =
                           atoi(subchild.child("UDPSourcePort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.src.range =
                           atoi(subchild.child("UDPSourceRange").child_value());
          LOG_MSG_INFO1("UDPSourcePort: %d , UDPSourceRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.src.range,0);
        }

        subchild = child.child("UDPDestination");
        if (subchild)
        {
          firewall_conf->filter_spec.next_prot_hdr.udp.field_mask |=
              IPFLTR_MASK_UDP_DST_PORT;
          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port =
                      atoi(subchild.child("UDPDestinationPort").child_value());
          firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range =
                      atoi(subchild.child("UDPDestinationRange").child_value());

          LOG_MSG_INFO1("UDPDestinationPort: %d , UDPDestinationRange: %d",
                         firewall_conf->filter_spec.next_prot_hdr.tcp.dst.port ,
                         firewall_conf->filter_spec.next_prot_hdr.tcp.dst.range,0);
        }
      }
      break;

       case PS_IPPROTO_ICMP:
       case PS_IPPROTO_ICMP6:
       {
         subchild = child.child(ICMPType_TAG);
         if (subchild)
         {
           type = atoi(child.child(ICMPType_TAG).child_value());
           firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask
                 |= IPFLTR_MASK_ICMP_MSG_TYPE;
           firewall_conf->filter_spec.next_prot_hdr.icmp.type = type;
           LOG_MSG_INFO1("ICMP Type : %d ",
                        firewall_conf->filter_spec.next_prot_hdr.icmp.type,0,0);
         }

         subchild = child.child(ICMPCode_TAG);
         if (subchild)
         {
           code = atoi(child.child(ICMPCode_TAG).child_value());
           firewall_conf->filter_spec.next_prot_hdr.icmp.field_mask
                 |= IPFLTR_MASK_ICMP_MSG_CODE;
           firewall_conf->filter_spec.next_prot_hdr.icmp.code = code;
           LOG_MSG_INFO1("ICMP Code : %d ",
                        firewall_conf->filter_spec.next_prot_hdr.icmp.code,0,0);
         }
       }
       break;

       case PS_IPPROTO_ESP:
       {
         subchild = child.child(ESPSPI_TAG);
         if(subchild)
         {
           firewall_conf->filter_spec.next_prot_hdr.esp.spi =
                                    atoi(child.child(ESPSPI_TAG).child_value());
           firewall_conf->filter_spec.next_prot_hdr.esp.field_mask
                 |= IPFLTR_MASK_ESP_SPI;
           LOG_MSG_INFO1("ESP spi : %d ",
                        firewall_conf->filter_spec.next_prot_hdr.esp.spi,0,0);
         }
       }
       break;

       case PS_IPPROTO_TCP_UDP:
       {
         subchild = child.child(TCP_UDPSource_TAG);
         if (subchild)
         {
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |=
                                                   IPFLTR_MASK_TCP_UDP_SRC_PORT;
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port
                    = atoi(subchild.child(TCP_UDPSourcePort_TAG).child_value());

           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range
                   = atoi(subchild.child(TCP_UDPSourceRange_TAG).child_value());

           LOG_MSG_INFO1("TCP_UDPSourcePort: %d , TCP_UDPSourceRange: %d",
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.port,
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.src.range,0);

         }
         subchild = child.child(TCP_UDPDestination_TAG);
         if (subchild)
         {
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.field_mask |=
                                                   IPFLTR_MASK_TCP_UDP_DST_PORT;
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port
                    = atoi(subchild.child(TCP_UDPDestinationPort_TAG).child_value());

           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range
                   = atoi(subchild.child(TCP_UDPDestinationRange_TAG).child_value());

           LOG_MSG_INFO1("TCP_UDPDestinationPort: %d , TCP_UDPDestinationRange: %d",
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.port,
           firewall_conf->filter_spec.next_prot_hdr.tcp_udp_port_range.dst.range,0);

         }
       }
       break;

       default:
       break;
     }

  return;
}

/*===========================================================================
  FUNCTION searchFirewallByHandleInXML
==========================================================================*/
/*!
@brief
  This API both searches and retrives a Firewall entry based on the handle.

@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf,
  int handle

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

pugi::xml_node QCMAP_Firewall::searchFirewallByHandleInXML
(
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf,
  int handle
)
{
  pugi::xml_node root, child ;
  qcmap_msgr_firewall_entry_conf_t firewall_conf;
  bzero(&firewall_conf,sizeof(firewall_conf));

  if (!(root = GetPdnRootXML(xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall cfg corresponding to PDN",0,0,0);
    return child;
  }

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), "Firewall"))
    {
      memset(&firewall_conf, 0, sizeof(firewall_conf));
      GetFirewallEntryFromXML(child, &firewall_conf);
      if(handle == firewall_conf.firewall_handle)
      {
        memcpy(extd_firewall_conf, &firewall_conf, sizeof(firewall_conf));
        return child;
      }
    }
  }
  return child;
}

/*===========================================================================
  FUNCTION searchFirewallByIndexInXML
==========================================================================*/
/*!
@brief
  This API both searches and retrives a Firewall entry based on the "entry"
variable. If entry = SEARCH_FIREWALL_ENTRY, it will try to search for an entry indicated by
extd_firewall_conf.

If entry != SEARCH_FIREWALL_ENTRY, this API would retrieve the corresponding Firewall entry
as indicated by the count.


@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf,
  int entry

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
pugi::xml_node QCMAP_Firewall::searchFirewallByIndexInXML
(
  pugi::xml_document *xml_file,
  qcmap_msgr_firewall_entry_conf_t *extd_firewall_conf,
  int entry)
{
  pugi::xml_node root, child ;
  qcmap_msgr_firewall_entry_conf_t firewall_conf;
  int count = 0;
  bzero(&firewall_conf,sizeof(firewall_conf));

  root = xml_file->child("system");

  if (!(root = GetPdnRootXML(xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall cfg corresponding to PDN",0,0,0);
    return child;
  }

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), "Firewall"))
    {
      if((entry == SEARCH_FIREWALL_ENTRY) || (entry == count))
      {
        memset(&firewall_conf, 0, sizeof(firewall_conf));
        GetFirewallEntryFromXML(child, &firewall_conf);
        if(entry == count)
        {
          memcpy(extd_firewall_conf, &firewall_conf, sizeof(firewall_conf));
          return child;
        }
        else
        {
          if(memcmp(&(extd_firewall_conf->filter_spec), &(firewall_conf.filter_spec), sizeof(ip_filter_type)) == 0)
          {
            LOG_MSG_ERROR("Found entry with same spec",0,0,0);
            return child;
          }
        }
      }
      count++;
    }
  }
  return child;
}


/*===========================================================================
  FUNCTION ReadFirewallXML
==========================================================================*/
/*!
@brief
  Reads the firewall XML


@parameters
  None

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

boolean QCMAP_Firewall::ReadFirewallXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child, parent;
  char data[MAX_STRING_LENGTH] = {0};
  qcmap_msgr_firewall_conf_t firewall_conf;
  qcmap_msgr_firewall_entry_conf_t *new_firewall_entry = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL", 0,0,0);
    return false;
  }

  if (!(IS_DEFAULT_PROFILE(GetProfileHandle())))
  {
    if (!(IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(QcMapMgr->target)) )
    {
      LOG_MSG_ERROR("Firewall for current target(%d) not supported", QcMapMgr->target,0,0);
      return false;
    }
  }

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
    return false;
  }

  if (!(root = GetPdnRootXML(&xml_file)) )
  {
    LOG_MSG_ERROR("Unable to find firewall corresponding to PDN, generate default",0,0,0);
    root = xml_file.child(System_TAG);

    // add new Firewall config here.
    parent = root.append_child(MobileAPFirewallCfg_TAG);

    //PDN which this firewall is tied to
    child = parent.append_child(ProfileHandle_TAG);
    snprintf(data, sizeof(data), "%d", QcMapBackhaul->profileHandle);
    child.append_child(pugi::node_pcdata).set_value(data);

    //NetDev which this firewall is tied to
    child = parent.append_child(NetDev_TAG);
    snprintf(data, sizeof(data), "0");
    child.append_child(pugi::node_pcdata).set_value(data);

    //Firewall enabled
    child = parent.append_child(FirewallEnabled_TAG);
    snprintf(data, sizeof(data), "0");
    child.append_child(pugi::node_pcdata).set_value(data);

    //Packets Allowed enabled
    child = parent.append_child(FirewallPktsAllowed_TAG);
    snprintf(data, sizeof(data), "0");
    child.append_child(pugi::node_pcdata).set_value(data);

    //UPNP Inbound Pinhole enabled
    child = parent.append_child(UPNPInboundPinhole_TAG);
    snprintf(data, sizeof(data), "0");
    child.append_child(pugi::node_pcdata).set_value(data);

    QCMAP_ConnectionManager::WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);
    return false;
  }

  this->firewall_config.firewall_enabled = atoi(root.child(FirewallEnabled_TAG).child_value());
  this->firewall_config.firewall_pkts_allowed = atoi(root.child(FirewallPktsAllowed_TAG).child_value());
  if(root.child(UPNPInboundPinhole_TAG))
  {
    this->firewall_config.upnp_inbound_pinhole= atoi(root.child(UPNPInboundPinhole_TAG).child_value());
  }

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), Firewall_TAG))
    {
      memset(&firewall_conf, 0, sizeof(qcmap_msgr_firewall_conf_t));
      this->GetFirewallEntryFromXML(child, &(firewall_conf.extd_firewall_entry));
      if(this->addFirewallEntryToList(&(firewall_conf.extd_firewall_entry), new_firewall_entry) == false)
      {
        LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
        return false;
      }
    }
  }
  LOG_MSG_INFO1("Firewall enabled: %d Pkts allowed: %d UPNP Pinhole Allowed: %d",this->firewall_config.firewall_enabled,
                this->firewall_config.firewall_pkts_allowed, this->firewall_config.upnp_inbound_pinhole);

  return true;
}


/*===========================================================================
  FUNCTION GetSetFirewallConfigFromXML
==========================================================================*/
/*!
@brief
  Get or set the firewall XML


@parameters
  qcmap_action_type action,
  boolean *enable_firewall,
  boolean *pkts_allowed

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

boolean QCMAP_Firewall::GetSetFirewallConfigFromXML
(
  qcmap_action_type action,
  boolean *enable_firewall,
  boolean *pkts_allowed,
  boolean *upnp_pinhole_allow
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (!(root = GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall cfg corresponding to PDN",0,0,0);
    return false;
  }

  switch (action)
  {
    case SET_VALUE:
      snprintf(data, sizeof(data), "%d", *enable_firewall);
      root.child(FirewallEnabled_TAG).text() = data;
      if(*enable_firewall!=0)
      {
        snprintf(data, sizeof(data), "%d", *pkts_allowed);
        root.child(FirewallPktsAllowed_TAG).text() = data;
      }
      snprintf(data, MAX_STRING_LENGTH, "%d", *upnp_pinhole_allow);
      root.child(UPNPInboundPinhole_TAG).text() = data;
      QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);
    break;
    case GET_VALUE:
      LOG_MSG_INFO1("GET VALUE",0,0,0);
      *enable_firewall = atoi(root.child(FirewallEnabled_TAG).child_value());
      *pkts_allowed = atoi(root.child(FirewallPktsAllowed_TAG).child_value());
      *upnp_pinhole_allow = atoi(root.child(UPNPInboundPinhole_TAG).child_value());
    break;

    default:
      LOG_MSG_ERROR("Invalid action", 0, 0, 0);
      return false;
    break;
  }

  LOG_MSG_INFO1("action: %d enable: %d pkts: %d ", action, *enable_firewall, *pkts_allowed);
  return TRUE;
}


/*======================================================
  FUNCTION:  WriteDefaultsToFirewallXML
  =====================================================*/
  /*!
      @brief
      Writes default values to XML in case if XML is not present

      @params

      @return
      QCMAP_CM_ERROR
      QCMAP_CM_SUCCESS
  */
/*====================================================*/
static void QCMAP_Firewall::WriteDefaultsToFirewallXML()
{
  FILE *fp = NULL;
  char *xml_file=QCMAP_ConnectionManager::firewall_xml_path;

  LOG_MSG_INFO1("Creating XML file using default values",0,0,0);
  fp = fopen(xml_file, "wb");

  /* File not found at the given path */
  if (NULL == fp)
  {
    return QCMAP_CM_ERROR;
  }

  fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(fp,
      "<system xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"mobileap_firewall_cfg.xsd\">\n");
  fprintf(fp, "\t<DefaultProfile>%d</DefaultProfile>\n", 1);
  fprintf(fp, "\t<MobileAPFirewallCfg>\n");
  fprintf(fp, "\t\t<Profile>%d</Profile>\n", 1);
  fprintf(fp, "\t\t<NetDev>%d</NetDev>\n", 0);
  fprintf(fp, "\t\t<FirewallEnabled>%d</FirewallEnabled>\n", 0);
  fprintf(fp, "\t\t<FirewallPktsAllowed>%d</FirewallPktsAllowed>\n",0);
  fprintf(fp, "\t\t<UPNPInboundPinhole>%d</UPNPInboundPinhole>\n",0);

  fprintf(fp, "\t</MobileAPFirewallCfg>\n");
  fprintf(fp, "</system>\n");
  fclose(fp);
  return (QCMAP_CM_SUCCESS);
}


/*======================================================
  FUNCTION:  DeleteConfigFromFirewallXML
  =====================================================*/
  /*!
      @brief
      Delete's firewall config from XML.

      @params

      @return
      false - failure
      true - success
  */
/*====================================================*/
boolean  QCMAP_Firewall::DeleteConfigFromFirewallXML( )
{
  pugi::xml_document xml_file;
  pugi::xml_node firewallNode;

  LOG_MSG_INFO1("Removing firewall config from xml.",0,0,0);

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
    return false;
  }

  if (!(firewallNode = GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall corresponding to PDN, generate default",0,0,0);
    return false;
  }

  if (!xml_file.child(System_TAG).remove_child(firewallNode))
  {
    LOG_MSG_ERROR("Unable to remove firewall, profile(%p)", QcMapBackhaul->profileHandle,0,0);
    return false;
  }

  QCMAP_ConnectionManager::WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);
  LOG_MSG_INFO1("Removed firewall node from xml", 0,0,0);
  return true;
}

/*======================================================
  FUNCTION:  ConntrackDeleteForDropFirewallEntries
  =====================================================*/
  /*!
      @brief
      Delete the client conntrack when we are adding a firewall entry for
      that IPv6 address and port combination

      @params
         IPv6 address
         Protocol
         UDP Port start number
         UDP Port Range
         TCP Port start number
         TCP Port Range

      @return
      void
  */
/*====================================================*/
void QCMAP_Firewall::DeleteConntrackEntryForDropIPv6FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  uint16 prefixbitvalue = 0;
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char v6str[INET6_ADDRSTRLEN]={0};
  uint8 prefixWords = 0;
  uint8 prefixBits = 0;
  bool check_sport = false;
  bool check_dport = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port = 0;
  FILE *fd = NULL;
  char *protocol=NULL,*saddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  in6_addr tmpipv6, curr_ipv6_addr;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  bool processV6addr =  false;
  bool skipPrefixBitProcess = false;

  LOG_MSG_INFO1("DeleteConntrackEntryForDropIPv6FirewallEntries \n",0,0,0);

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    return;
  }

  if (protocol_num != PS_IPPROTO_TCP && protocol_num != PS_IPPROTO_UDP)
  {
    if ( protocol_num == PS_IPPROTO_TCP_UDP)
    {
      DeleteConntrackEntryForDropIPv6FirewallEntries(firewall_entry,PS_IPPROTO_TCP);
      DeleteConntrackEntryForDropIPv6FirewallEntries(firewall_entry,PS_IPPROTO_UDP);
    }
    return true;
  }

  /*
   */
  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,9,11,12 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,10,12,13 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        if( firewall_entry->filter_spec.next_prot_hdr.tcp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
          check_sport = true;
        }
        else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.src.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port);
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.src.range !=0 )
        {
          min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
          max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
          check_sport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep sport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.src.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        if ( firewall_entry->filter_spec.next_prot_hdr.udp.dst.range !=0 )
        {
          min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
          max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                      firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
          check_dport = true;
        }else
        {
          snprintf(tmp,MAX_COMMAND_STR_LEN,"| grep dport=%d ",
                   firewall_entry->filter_spec.next_prot_hdr.udp.dst.port );
        }
        strlcat(command,tmp,MAX_COMMAND_STR_LEN);
        memset(tmp,0,MAX_COMMAND_STR_LEN);
      }

      break;
  }

  if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
  {
    prefixWords = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len/(sizeof(uint16)*8);
    prefixBits = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len%(sizeof(uint16)*8);

    memcpy (&curr_ipv6_addr.s6_addr ,
            firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr16,
            QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    prefixbitvalue = curr_ipv6_addr.s6_addr[prefixWords] >> (16 - prefixBits);

    if ( prefixBits == 0 && prefixWords == 8 )
    {
      processV6addr = false;

      inet_ntop(AF_INET6,
               (void*)&curr_ipv6_addr, v6str, sizeof(v6str));
      snprintf(tmp, MAX_COMMAND_STR_LEN,"| grep src=%s", (char*)v6str);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    else
    {
      processV6addr = true;
    }
  }


  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPv6_CONNTRACK_FILTER_PATH);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  printf("Grep : %s \n", command);
  ds_system_call(command, strlen(command));

  fd = fopen(IPv6_CONNTRACK_FILTER_PATH,"r");


  if(fd == NULL)
  {
    LOG_MSG_ERROR("FetchPortInfoFromV6Conntrack - Error in opening ",0,0,0);
    return;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    skipPrefixBitProcess = false;

    protocol = strtok_r(stringline, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);

    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);

    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);

    if(dport)
      *(dport + 5) = ' ';

      //Copy to temp str then pton and then check prefixword byte value after right shifting

    if (processV6addr)
    {
      if(saddr)
        inet_pton(AF_INET6, saddr + 4, (void *)&tmpipv6);

      if (memcmp (&curr_ipv6_addr.s6_addr16, &tmpipv6.s6_addr16,
                  (sizeof (uint16))*prefixWords) != 0)
      {
        skipPrefixBitProcess = true;
      }

      if ((skipPrefixBitProcess)||
           ( tmpipv6.s6_addr16[prefixWords] >> ((sizeof(uint16)*8)-prefixBits) != prefixbitvalue))
      {
        continue;
      }
    }

    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
      continue;
    }

    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        continue;
    }

    //Delete contrack entry
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);
  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPv6_CONNTRACK_FILTER_PATH);
  ds_system_call(command, strlen(command));
}


/*======================================================
  FUNCTION:  ConntrackDeleteForAcceptFirewallEntries
  =====================================================*/
  /*!
      @brief
      Delete the client conntrack when we are adding a firewall entry for
      that IPv6 address and port combination

      @params
         IPv6 address
         Protocol
         UDP Port start number
         UDP Port Range
         TCP Port start number
         TCP Port Range

      @return
      void
  */
/*====================================================*/
void QCMAP_Firewall::DeleteConntrackEntryForAcceptIPv6FirewallEntries
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  uint8 protocol_num
)
{
  uint16 prefixbitvalue = 0;
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  uint8 prefixWords = 0;
  uint8 prefixBits = 0;
  bool check_sport = false;
  bool check_dport = false;
  uint16 min_sport;
  uint16 max_sport;
  uint16 min_dport;
  uint16 max_dport;
  uint16 port = 0;
  FILE *fd = NULL;
  char *protocol=NULL,*saddr=NULL, *sport=NULL, *dport=NULL, *ptr;
  in6_addr tmpipv6, curr_ipv6_addr;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  bool processV6addr =  false;
  bool skipPrefixBitProcess = false;

  LOG_MSG_INFO1("DeleteConntrackEntryForAcceptIPv6FirewallEntries \n",0,0,0);

  if( firewall_entry == NULL )
  {
    LOG_MSG_ERROR("NULL firewall_entry\n",0,0,0);
    return;
  }

  if ( protocol_num == PS_IPPROTO_UDP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,9,11,12 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);
  else if ( protocol_num == PS_IPPROTO_TCP)
    snprintf(command, MAX_COMMAND_STR_LEN,
             "conntrack -L -f ipv6 | cut -f1,10,12,13 -d ' ' | grep -v dport=%d ",
             PS_IPPROTO_DNS);

  switch(protocol_num)
  {
    case PS_IPPROTO_TCP:

      strlcat(command,"| grep tcp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask
           & IPFLTR_MASK_TCP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.tcp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.tcp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.tcp.dst.range;
        check_dport = true;
      }

      break;

    case PS_IPPROTO_UDP:

      strlcat(command,"| grep udp ",MAX_COMMAND_STR_LEN);

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask &
           IPFLTR_MASK_UDP_SRC_PORT )
      {
        min_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port;
        max_sport = firewall_entry->filter_spec.next_prot_hdr.udp.src.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.src.range;
        check_sport = true;
      }

      if ( firewall_entry->filter_spec.next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT )
      {
        min_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port;
        max_dport = firewall_entry->filter_spec.next_prot_hdr.udp.dst.port +
                    firewall_entry->filter_spec.next_prot_hdr.udp.dst.range;
        check_dport = true;
      }

      break;
  }

  if ( firewall_entry->filter_spec.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
  {
    prefixWords = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len/(sizeof(uint16)*8);
    prefixBits = firewall_entry->filter_spec.ip_hdr.v6.src.prefix_len%(sizeof(uint16)*8);

    memcpy (&curr_ipv6_addr.s6_addr ,
            firewall_entry->filter_spec.ip_hdr.v6.src.addr.in6_u.u6_addr16,
            QCMAP_MSGR_IPV6_ADDR_LEN_V01);

    if (prefixWords < 8)
    {
      prefixbitvalue = curr_ipv6_addr.s6_addr[prefixWords] >> (16 - prefixBits);
    }

    processV6addr = true;
  }


  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPv6_CONNTRACK_FILTER_PATH);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  printf("Grep : %s \n", command);
  ds_system_call(command, strlen(command));

  fd = fopen(IPv6_CONNTRACK_FILTER_PATH,"r");


  if(fd == NULL)
  {
    LOG_MSG_ERROR("FetchPortInfoFromV6Conntrack - Error in opening ",0,0,0);
    return;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    skipPrefixBitProcess = false;

    protocol = strtok_r(stringline, " ", &ptr);

    saddr = strtok_r(NULL, " ", &ptr);

    if(saddr)
      *(saddr + 3) = ' ';

    sport = strtok_r(NULL, " ", &ptr);

    if(sport)
      *(sport + 5) = ' ';

    dport = strtok_r(NULL, " ", &ptr);

    if(dport)
      *(dport + 5) = ' ';
      //Copy to temp str then pton and then check prefixword byte value after right shifting

    if (processV6addr)
    {
      if(saddr)
        inet_pton(AF_INET6, saddr + 4, (void *)&tmpipv6);
      if (memcmp (&curr_ipv6_addr.s6_addr16, &tmpipv6.s6_addr16,
                  (sizeof (uint16))*prefixWords) != 0)
      {
        goto delete_entry_v6;
      }

      if ((prefixWords < 8) &&
           (tmpipv6.s6_addr16[prefixWords] >> ((sizeof(uint16)*8)-prefixBits) != prefixbitvalue))
      {
        goto delete_entry_v6;
      }
    }

    if (check_sport)
    {
      if(sport)
        port = ds_atoi((const)(sport+6));
      if (port < min_sport || port > max_sport)
        goto delete_entry_v6;
    }

    if (check_dport)
    {
      if(dport)
        port = ds_atoi((const)(dport+6));
      if (port < min_dport || port > max_dport)
        goto delete_entry_v6;
    }

    /*check protocol*/
    if (firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP_UDP ||
        firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot == protocol_num ||
        firewall_entry->filter_spec.ip_hdr.v6.next_hdr_prot == 0)
      continue;

    //Delete contrack entry
    delete_entry_v6:
    memset(tmp,0,MAX_COMMAND_STR_LEN);

    snprintf(command, MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6");
    if(protocol)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " -p %s", protocol);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(saddr)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, "  --orig-%s", saddr);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(sport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", sport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    if(dport)
    {
      snprintf(tmp, MAX_COMMAND_STR_LEN, " --%s", dport);
      strlcat(command,tmp,MAX_COMMAND_STR_LEN);
      memset(tmp,0,MAX_COMMAND_STR_LEN);
    }
    ds_system_call(command, strlen(command));
    memset(command,0,MAX_COMMAND_STR_LEN);

  }

  fclose(fd);

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPv6_CONNTRACK_FILTER_PATH);
  ds_system_call(command, strlen(command));
}

/*==================================================================
  FUNCTION:  DeleteExternalIpv4Conntracks
  =================================================================*/
  /*!
  @brief
  - Delete all the conntrack entries for external Ipv4 communication

  @params
  - None

  @return
  - bool

  @note
  - Dependencies
  - None

  - Side Effects
  - None

  */
/*====================================================*/
boolean QCMAP_Firewall::DeleteExternalIpv4Conntracks()
{
  char command[MAX_COMMAND_STR_LEN]={0};
  qmi_error_type_v01 qmi_err_num;
  in_addr_t public_ip = 0,  sec_dns_addr = 0;
  uint32 pri_dns_addr = 0;
  struct in_addr addr;
  char ipaddr[INET_ADDRSTRLEN];

  memset(&public_ip,0,sizeof(in_addr));
  memset(&pri_dns_addr,0,sizeof(in_addr));
  memset(&sec_dns_addr,0,sizeof(in_addr));
  memset (command, 0, MAX_COMMAND_STR_LEN);
  qmi_err_num = QMI_ERR_NONE_V01;

  if (QcMapBackhaul->GetNetworkConfig(&public_ip, &pri_dns_addr,
                                      &sec_dns_addr, &qmi_err_num) == true)
  {
    addr.s_addr = public_ip;

    inet_ntop(AF_INET,(const void*)&addr.s_addr, (char *)&ipaddr,
              INET_ADDRSTRLEN);

    snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D --orig-dst %s",(char *)&ipaddr);
    ds_system_call(command, strlen(command));

    snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D --reply-dst %s",(char *)&ipaddr);
    ds_system_call(command, strlen(command));
  }
  else
  {
    LOG_MSG_ERROR("Couldn't get WWAN ipv4 address. error %d",
                  qmi_err_num, 0, 0);
    return false;
  }
  return true;
}


/*==================================================================
  FUNCTION:  DeleteExternalIpv6Conntracks
  ================================================================*/
  /*!
  @brief
  - Delete all the conntrack entries for external Ipv6 communication

  @params
  - None

  @return
  - bool

  @note
  - Dependencies
  - None

  - Side Effects
  - None

  */
/*====================================================*/
boolean QCMAP_Firewall::DeleteExternalIpv6Conntracks()
{
  in6_addr tmpipv6;
  char command[MAX_COMMAND_STR_LEN]={0};
  qmi_error_type_v01 qmi_err_num;
  unsigned char ip6_addr_buf[MAX_IPV6_PREFIX];
  uint8_t public_ip_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t primary_dns_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t secondary_dns_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t null_global_v6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  qcmap_addr_info_list_t* addrList;
  ds_dll_el_t * node;
  qcmap_cm_client_data_info_t* node_data = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  memset(public_ip_v6, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  memset(primary_dns_v6, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  memset(secondary_dns_v6, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  memset(null_global_v6, 0, QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
  memset (ip6_addr_buf, 0, MAX_IPV6_PREFIX);
  memset (command, 0, MAX_COMMAND_STR_LEN);
  memset(&tmpipv6, 0, sizeof(tmpipv6));
  qmi_err_num = QMI_ERR_NONE_V01;

  if (QcMapBackhaul->GetIPv6NetworkConfig(public_ip_v6, primary_dns_v6,
                                          secondary_dns_v6,&qmi_err_num) == true)

  {
    QcMapMgr->FetchIpv6Conntrack((char*)&public_ip_v6);
  }
  else
  {
    LOG_MSG_ERROR("Couldn't get Backhaul ipv6 address. error %d\n",
                   qmi_err_num, 0, 0);
    return false;
  }

  if (QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER ||
      QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_CRADLE)
  {
    addrList = &(QcMapBackhaul->external_client_addr_list);
    if (addrList->addrListHead == NULL)
    {
      LOG_MSG_ERROR("External client List head is NULL\n", 0, 0, 0);
      return false;
    }

    node = addrList->addrListHead;
    node = ds_dll_next (node, (const void**)(&node_data));
    if ( node == NULL)
    {
      LOG_MSG_ERROR("Ext client info is NULL", 0, 0, 0);
      return false;
    }
    while (node)
    {
      for (int i = 0; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
      {
        if (memcmp (node_data->ipv6[i].addr,
                    null_global_v6,QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0)
        {
          break;
        }
        memcpy(&tmpipv6.s6_addr,
               node_data->ipv6[i].addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
        inet_ntop(AF_INET6,
                  (void*)&tmpipv6, ip6_addr_buf, MAX_IPV6_PREFIX);

        ds_log_med ("Ext client ipv6 %s\n",ip6_addr_buf);

        snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --orig-src %s",ip6_addr_buf);
        ds_system_call(command, strlen(command));

        snprintf(command,MAX_COMMAND_STR_LEN, "conntrack -D -f ipv6 --orig-dst %s",ip6_addr_buf);
        ds_system_call(command, strlen(command));
      }
      //Point to the next node
      node = ds_dll_next (node, (const void**)(&node_data));
    }
  }
  return true;
}


/*=====================================================
  FUNCTION GetPdnRootXML
======================================================*/
/*!
@brief
 Returns hte root XML node of the firewall cfg for the correct
 PDN

@parameters

@return
  - pugi::xml_node

@note
  Use the return value to indicate if router mode is enabled.

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
pugi::xml_node QCMAP_Firewall::GetPdnRootXML(pugi::xml_document *xml_file)
{
  pugi::xml_node root, child, subchild;

  if (IS_DEFAULT_PROFILE(GetProfileHandle()))
  {
    root = xml_file->child(System_TAG).child(MobileAPFirewallCfg_TAG);
  }
  else
  {
    root = xml_file->child(System_TAG);

    /*search through the MobileAPFirewallCfg_TAG's for the firewall
      corresponding to the correct PDN */
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if(atoi(child.child(ProfileHandle_TAG).child_value()) == QcMapBackhaul->profileHandle)
        break;
    }

    if(child == NULL)
    {
      LOG_MSG_ERROR("Unable to find firewall corresponding to PDN",0,0,0);
      return child;
    }
    root = child;
  }

  return root;
}


/*=====================================================
  FUNCTION SwapFirewallWithDefault
======================================================*/
/*!
@brief
 Swap default and concurrent profile handle in firewall.xml
 file.

@parameters

@return
  - None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_Firewall::SwapFirewallWithDefault()
{
  char data[MAX_PROFILE_HANDLE_LENGTH]={0};

  pugi::xml_document xml_file;
  pugi::xml_node firewallNode, firewallNodeDefault;
  QCMAP_Firewall  *pFirewallDefault = GET_DEFAULT_FIREWALL_OBJECT();
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (pFirewallDefault == NULL)
  {
    LOG_MSG_ERROR("Default firewall object is NULL", 0,0,0);
    return;
  }

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr is NULL", 0,0,0);
    return;
  }

  if (!(IS_DEFAULT_PROFILE(GetProfileHandle())))
  {
    if (!(IS_FIREWALL_CONFIG_FOR_SECONDARY_PDN_ALLOWED(QcMapMgr->target)) )
    {
      LOG_MSG_ERROR("Firewall for current target(%d) not supported", QcMapMgr->target,0,0);
      return;
    }
  }

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
    return;
  }

  if (!(firewallNodeDefault = pFirewallDefault->GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall default PDN",0,0,0);
    return;
  }

  /* Only Profile Handle Reference need to swapped */
  /* Firewall Object swap is done in QCMAP_Backhaul::SwapFirewallObjectWithDefault() */
  /* Swap profile handle for concurrent with default profile handle and vice-versa */
  snprintf(data, sizeof(data), "%d", pFirewallDefault->QcMapBackhaul->profileHandle);
  firewallNode.child(ProfileHandle_TAG).text() = data;

  snprintf(data, sizeof(data), "%d", QcMapBackhaul->profileHandle);
  firewallNodeDefault.child(ProfileHandle_TAG).text() = data;
  /* Update default profile in XML */
  xml_file.child(System_TAG).child(DefaultProfile_TAG).text() = data;

  QCMAP_ConnectionManager::WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);
}

/*=====================================================
  FUNCTION UpdateNetDevInXML
======================================================*/
/*!
@brief
 Update NetDev associated for this profile.

@parameters

@return
  - None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_Firewall::UpdateNetDevInXML(const char *devname)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child, parent;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*
   * 1. IPACM needs <NetDev>device</NetDev> info from QCMAP in Multi-PDN scenarios.
   * 2. If devname is NULL and if any IPv4 or IPv6 PDN for this profile is UP
   *    then do not reset <NetDev>
   */
  if (QcMapBackhaul == NULL || QcMapBackhaul->QcMapBackhaulWWAN == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! Backhaul is NULL",0,0,0);
    return;
  }

  if (devname == NULL &&
        (QcMapBackhaul->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED ||
          QcMapBackhaul->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED) )
  {
    LOG_MSG_INFO1("IPv4 or IPv6 PDN is UP, skip resetting devname for profile %d", QcMapBackhaul->profileHandle, 0,0);
    return;
  }

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (!(root = GetPdnRootXML(&xml_file)))
  {
    LOG_MSG_ERROR("Unable to find firewall cfg corresponding to PDN",0,0,0);
    return false;
  }

  //Update NetDev for this profile
  if (devname)
  {
    snprintf(data, sizeof(data), "%s", devname);
  }
  else
  {
    snprintf(data, sizeof(data), "0");
  }

  LOG_MSG_INFO1("Updating NetDev  as %s for profile %d ", data, QcMapBackhaul->profileHandle, 0);
  if (root.child(NetDev_TAG))
    root.child(NetDev_TAG).text() = data;
  else
  {
    parent = root.child(MobileAPFirewallCfg_TAG);
    child = parent.append_child(NetDev_TAG);
    child.append_child(pugi::node_pcdata).set_value(data);
  }
  QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);
}

/*=====================================================
  FUNCTION ResetNetDevInXML
======================================================*/
/*!
@brief
 Reset NetDev associated for all profiles.

@parameters

@return
  - None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
void QCMAP_Firewall::ResetNetDevInXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root, firewallCfg, netDevCfg;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /*
   * 1. IPACM needs <NetDev>device</NetDev> info from QCMAP in Multi-PDN scenarios.
   * 2. On device power-up reset all <NetDev> info
   */

  LOG_MSG_INFO1("Reset NetDev for all Firewall configs", 0,0,0);

  if (!xml_file.load_file(QCMAP_ConnectionManager::firewall_xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  strlcpy(data, "0", MAX_STRING_LENGTH);

  root = xml_file.child(System_TAG);
  for (firewallCfg = root.first_child(); firewallCfg; firewallCfg = firewallCfg.next_sibling())
  {
    if (strncmp(MobileAPFirewallCfg_TAG, firewallCfg.name(), strlen(MobileAPFirewallCfg_TAG)) == 0)
    {
      if (firewallCfg.child(NetDev_TAG))
      {
        firewallCfg.child(NetDev_TAG).text() = data;
      }
      else
      {
        netDevCfg = firewallCfg.append_child(NetDev_TAG);
        netDevCfg.append_child(pugi::node_pcdata).set_value(data);
      }
    }
  }
  QcMapMgr->WriteConfigToXML(UPDATE_FIREWALL_XML, &xml_file);
}

