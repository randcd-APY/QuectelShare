/*====================================================

FILE:  QCMAP_LAN.cpp

SERVICES:
   QCMAP Connection Manager LAN Specific Implementation

=====================================================

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when          who        what, where, why
  --------   ---        -------------------------------------------------------
  08/26/14     ka          Created
  06/12/15     rk          Offtarget support.
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
#include "qti_wlan_scm_msgr_v01.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_LAN.h"
#include "QCMAP_LAN_Manager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_BT_Tethering.h"
#include "QCMAP_Virtual_LAN.h"
#include "QCMAP_L2TP.h"
#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
#include <linux/msm_ipa.h>
#include <sys/ioctl.h>
#endif

#define DHCP_HOSTS_FILE "/data/dhcp_hosts"
#define DHCP_RECORD_LENGTH 100
#define MAX_DHCP_COMMAND_STR_LEN 4000
#define MTU_STRING_SIZE 5
#define DHCP_OPTION_MTU "26"

static FILE *dhcp_hosts_fp;



/*==========================================================
                             Class Definitions
  =========================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes LAN variables.

@parameters

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

QCMAP_LAN::QCMAP_LAN(int16_t bridge_vlan_id)
{
  in_addr addr;
  bool update_config = false;
  qcmap_msgr_lan_config_v01 lan_config;
  qcmap_msgr_station_mode_config_v01 station_config;
  qmi_error_type_v01 qmi_err_num;

  this->auto_reboot_flag = false;
  this->bridge_vlan_id = bridge_vlan_id;
  this->vlan_iface_mask = 0;
  this->profile_handle = 0;
  this->ipa_offload_enabled = false;

  memset(this->bridge_name, 0, sizeof(this->bridge_name));
  memset(this->dnsmasq_resolv_file_path, 0, sizeof(this->dnsmasq_resolv_file_path));
  memset(this->dnsmasq_lease_file_path, 0, sizeof(this->dnsmasq_lease_file_path));
  memset(this->dnsmasq_pid_file_path, 0, sizeof(this->dnsmasq_pid_file_path));
  memset(this->host_file_path,0,sizeof(this->host_file_path));

  qcmap_media_svc_status dlna_mode=MEDIA_SVC_MODE_DOWN;
  qcmap_media_svc_status upnp_mode=MEDIA_SVC_MODE_DOWN;
  qcmap_media_svc_status mdns_mode=MEDIA_SVC_MODE_DOWN;
  LOG_MSG_INFO1("Creating object: LAN for bridge/vlan id %d", this->bridge_vlan_id, 0, 0);

  memset(&this->prev_lan_config,0,sizeof(qcmap_msgr_lan_config_v01));
  memset(&(this->lan_cfg), 0,sizeof(this->lan_cfg));
  this->bridge_inited = false;
  this->enable_dns = true;
  this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active = false;
  this->lan_cfg.num_dhcp_reservation_records=0;
  memset(&this->lan_cfg.ip_passthrough_cfg,0,sizeof(qcmap_cm_ip_passthrough_conf_t));

  if(QCMAP_ConnectionManager::xml_path)
  {
    switch(this->bridge_vlan_id)
    {
      case(DEFAULT_BRIDGE_ID): //for bridge0 only
      {
        if(this->ReadLANConfigFromXML())
        {

         //fill in dnsmasq_resolv_file_path for this object
         snprintf(this->dnsmasq_resolv_file_path, sizeof(this->dnsmasq_resolv_file_path),
                  DNSMASQ_RESOLV_FILE);
         //fill in dnsmasq_lease_file_path for this object
         snprintf(this->dnsmasq_lease_file_path, sizeof(this->dnsmasq_lease_file_path),
                  DNSMASQ_LEASE_FILE);
         //fill in dnsmasq_pid_file_path for this object
         snprintf(this->dnsmasq_pid_file_path, sizeof(this->dnsmasq_pid_file_path),
                  DNSMASQ_PID_FILE);
         //fill in host_path for this object
         snprintf(this->host_file_path,sizeof(this->host_file_path),
                  QCMAP_HOST_PATH);

          /* DHCP config checks. */
          if((( this->lan_cfg.apps_ip_addr & this->lan_cfg.sub_net_mask) !=
              ( this->lan_cfg.dhcp_start_address & this->lan_cfg.sub_net_mask))||
              ( this->lan_cfg.dhcp_start_address <=this->lan_cfg.apps_ip_addr))
          {
            this->lan_cfg.dhcp_start_address = this->lan_cfg.apps_ip_addr + MAX_LAN_CLIENTS;
            addr.s_addr = htonl(this->lan_cfg.dhcp_start_address );
            LOG_MSG_INFO1( "SSID1 DHCP Start addr in XML is invalid."
                           "Setting it to default value %s\n",inet_ntoa(addr),0,0);
            update_config = true;
          }
          if((( this->lan_cfg.apps_ip_addr &this->lan_cfg.sub_net_mask) !=
             ( this->lan_cfg.dhcp_end_address & this->lan_cfg.sub_net_mask))||
             ( this->lan_cfg.dhcp_end_address < this->lan_cfg.dhcp_start_address))
          {
            this->lan_cfg.dhcp_end_address =this->lan_cfg.dhcp_start_address + MAX_LAN_CLIENTS;
            addr.s_addr = htonl(this->lan_cfg.dhcp_end_address );
            LOG_MSG_INFO1( "SSID1 DHCP End addr in XML is invalid."
                           "Setting it to default value %s\n",inet_ntoa(addr),0,0);
            update_config = true;
          }

          this->lan_cfg.ip_passthrough_cfg.rmnet_reserved_ip = inet_addr(WWAN_Reserved_IP);
          /*If dhcp lease time configured is less then 120 seconds, set it to default lease time */
          if(this->lan_cfg.dhcp_lease_time < MIN_DHCP_LEASE )
          {
            this->lan_cfg.dhcp_lease_time = DHCP_LEASE_TIME;
            update_config = true;
          }
          /* sync /data/dhcp-hosts with the existing DHCP Reservation records*/
          dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "wb");
          if (NULL == dhcp_hosts_fp)
          {
              /* File could not be opened for writing/append*/
              LOG_MSG_ERROR("\n Failed to create %s!!",DHCP_HOSTS_FILE,0,0);
          }
          else
          {
            this->sync_dhcp_hosts();
            fclose(dhcp_hosts_fp);
          }


          /* Create the bridge Interface if not already created. */
          if ( !this->bridge_inited )
          {
            this->InitBridge();
          }

          /*Check for address conflict between ap, guest ap and static sta configuration */
          lan_config.gw_ip = this->lan_cfg.apps_ip_addr;
          lan_config.netmask = this->lan_cfg.sub_net_mask;
          QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE, &station_config);
          if(CheckforAddrConflict(&lan_config,&station_config ))
          {
            station_config.conn_type = QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01;
            /*If there address conflict set STA mode to dynamic. */
            QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(SET_VALUE,
                                                           &station_config);
          }

          //update the xml if needed
          if (update_config)
          {
            //set the remaining default parameters, these are only needed so the xml is correct
            lan_config.enable_dhcp = this->lan_cfg.enable_dhcpd;
            lan_config.dhcp_config.lease_time = this->lan_cfg.dhcp_lease_time;
            lan_config.dhcp_config.dhcp_start_ip = this->lan_cfg.dhcp_start_address;
            lan_config.dhcp_config.dhcp_end_ip = this->lan_cfg.dhcp_end_address;
            //check if XLAT range
            if (this->SetLanInXlatRangeToDefault(&lan_config))
            {
              LOG_MSG_ERROR("LAN IPv4 Subnet collides with XLAT private subnet."
                            "Setting to default",0,0,0);
            }
            this->SetLANConfigToXML(&lan_config);
          }

          LOG_MSG_INFO1("LAN object with bridge name: %s has Bridge/VLAN ID %d",
                        this->bridge_name, this->bridge_vlan_id, 0);
          return;
        }
        LOG_MSG_INFO1("QCMAP LAN Read XML failed.\n",0,0,0);
        break;
      }

      default: //for non-default bridge
      {
        //see if pre-existing config existed
        if(!this->ReadLANConfigFromXML())
        { //no pre-existing LAN config

          LOG_MSG_INFO1("No pre-existing LAN config for bridge%d", this->bridge_vlan_id,
                        0, 0);

          //determine lan configurations based on bridge_vlan_id seed
          srand(bridge_vlan_id);

          //probability of duplicate subnet is 1 / 255 = 0.39% chance
          //additionally each bridge is segregated by VLAN at link layer anyways
          this->lan_cfg.apps_ip_addr = (0xc0a80001) | ((rand() % 255) << 8);
          this->lan_cfg.sub_net_mask = 0xffffff00; //255.255.255.0
          this->lan_cfg.enable_dhcpd = true;
          this->lan_cfg.dhcp_start_address = this->lan_cfg.apps_ip_addr + 20;
          this->lan_cfg.dhcp_end_address = this->lan_cfg.dhcp_start_address + MAX_LAN_CLIENTS;
          this->lan_cfg.dhcp_lease_time=DHCP_LEASE_TIME;
        }

        //fill in dnsmasq_resolv_file_path for this object
        if(IS_DEFAULT_PROFILE(this->profile_handle))
        {
          snprintf(this->dnsmasq_resolv_file_path, sizeof(this->dnsmasq_resolv_file_path),
                   DNSMASQ_RESOLV_FILE);
        } else {
          snprintf(this->dnsmasq_resolv_file_path, sizeof(this->dnsmasq_resolv_file_path), "%s%d",
                   RESOLV_FILE_PATH_PREFIX, this->bridge_vlan_id);
        }

        //fill in dnsmasq_lease_file_path for this object
        snprintf(this->dnsmasq_lease_file_path, sizeof(this->dnsmasq_lease_file_path), "%s%d",
                 LEASE_FILE_PATH_PREFIX, this->bridge_vlan_id);

        //fill in dnsmasq_pid_file_path for this object
        snprintf(this->dnsmasq_pid_file_path, sizeof(this->dnsmasq_pid_file_path), "%s%d",
                 PID_FILE_PATH_PREFIX, this->bridge_vlan_id);

        //fill in host_path for this object
        snprintf(this->host_file_path,sizeof(this->host_file_path),"%s%d",
                QCMAP_HOST_NAME_PREFIX,
                this->bridge_vlan_id);

        //Create the bridge Interface if not already created
        if(!(this->bridge_inited))
        {
          this->InitBridge();
        }

        LOG_MSG_INFO1("LAN object with bridge name: %s has Bridge/VLAN ID %d",
                      this->bridge_name, this->bridge_vlan_id, 0);

        return;
        break;
      }
    }
  } else {
    QCMAP_ConnectionManager::xml_path[0] = '\0';

    this->lan_cfg.apps_ip_addr = ntohl(inet_addr(APPS_LAN_IP_ADDR));
    this->lan_cfg.sub_net_mask = ntohl(inet_addr(APPS_SUB_NET_MASK));
    this->lan_cfg.enable_dhcpd = true;
    this->lan_cfg.dhcp_start_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20;
    this->lan_cfg.dhcp_end_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20 + MAX_LAN_CLIENTS;
    this->lan_cfg.dhcp_lease_time=DHCP_LEASE_TIME;


    if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
    {
      QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
      if(QcMapMediaServiceMgr)
      {
        if (QcMapMediaServiceMgr->srvc_cfg.dlna_config)
          dlna_mode = MEDIA_SVC_MODE_UP;
        if (QcMapMediaServiceMgr->srvc_cfg.upnp_config)
          upnp_mode = MEDIA_SVC_MODE_UP;
         if (QcMapMediaServiceMgr->srvc_cfg.mdns_config)
          mdns_mode = MEDIA_SVC_MODE_UP;

      }
      else
      {
        QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNA,GET_VALUE, &dlna_mode);
        QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_UPNP,GET_VALUE, &upnp_mode);
        QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_MDNS,GET_VALUE, &mdns_mode);
      }
      if (dlna_mode == MEDIA_SVC_MODE_UP)
      {
        if(!QCMAP_MediaService::EnableDLNA(&qmi_err_num))
        {
          LOG_MSG_ERROR("Cannot enable DLNA, error: %d.", qmi_err_num, 0, 0);
        }
      }
      if (upnp_mode == MEDIA_SVC_MODE_UP)
      {
        if(!QCMAP_MediaService::EnableUPNP(&qmi_err_num))
        {
          LOG_MSG_ERROR("Cannot enable UPnP, error: %d.", qmi_err_num, 0, 0);
        }
      }

      if (mdns_mode == MEDIA_SVC_MODE_UP)
      {
        if(!QCMAP_MediaService::EnableMDNS(&qmi_err_num))
        {
          LOG_MSG_ERROR("Cannot enable MDNS, error: %d.", qmi_err_num, 0, 0);
        }
      }
    }
  }

  //fill in dnsmasq_resolv_file_path for this object
  snprintf(this->dnsmasq_resolv_file_path, sizeof(this->dnsmasq_resolv_file_path),
           DNSMASQ_RESOLV_FILE);

  //fill in dnsmasq_lease_file_path for this object
  snprintf(this->dnsmasq_lease_file_path, sizeof(this->dnsmasq_lease_file_path),
           DNSMASQ_LEASE_FILE);

  //fill in dnsmasq_pid_file_path for this object
  snprintf(this->dnsmasq_pid_file_path, sizeof(this->dnsmasq_pid_file_path),
           DNSMASQ_PID_FILE);

  return;
}


/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroying the LAN Object.

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

QCMAP_LAN::~QCMAP_LAN()
{
  qcmap_dhcp_information_info_list_t *dhcp_list = NULL;

  LOG_MSG_INFO1("Destroying Object: LAN",0,0,0);

  this->DelBridge(); //delete bridge iface

  dhcp_list = &(this->lan_cfg.dhcp_reservation_records);
  ds_dll_delete_all(dhcp_list->dhcpInfoEntryListHead);
  dhcp_list->dhcpInfoEntryListHead = NULL;
  dhcp_list->dhcpInfoEntryListTail = NULL;
}

/*==========================================================
  FUNCTION IsBridgeInited
==========================================================*/
/*!
@brief
  Checks if this QCMAP_LAN object's bridge has been inited

@parameters
  None

@return
  bool bridge_inited

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/
bool QCMAP_LAN::IsBridgeInited(void) const
{
  if(this->bridge_inited)
  {
    LOG_MSG_INFO1("Bridge%d object has been inited", this->bridge_vlan_id, 0, 0);
  } else {
    LOG_MSG_INFO1("Bridge%d object has not been inited", this->bridge_vlan_id, 0, 0);
  }

  return this->bridge_inited;
}


/*=====================================================================
  FUNCTION check_non_empty_mac_addr
======================================================================*/
/*!
@brief
  Check for empty mac address

@return
  bool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static bool QCMAP_LAN::check_non_empty_mac_addr(uint8 *mac, char mac_addr_string[])
{
  memset(mac_addr_string,0,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  snprintf( mac_addr_string,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01,
            "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  if ( strncmp(mac_addr_string,MAC_NULL_STR,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01)
       == 0 )
     return false;
  else
    return true;
}



/*===========================================================================
  FUNCTION qcmap_compare_dhcp_entries
==========================================================================*/
/*!
@brief
  To compare dhcp enteries.

@parameters
  None.

@return
  true  - If enetries match.
  false - otherwise.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

bool qcmap_compare_dhcp_entries
(
  const void *first,
  const void *second
)
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  uint32 *entry_to_delete = (uint32*)first;
  qcmap_msgr_dhcp_reservation_v01*dhcp_entry =
                                       (qcmap_msgr_dhcp_reservation_v01*)second;

  if (dhcp_entry->client_reserved_ip == *entry_to_delete)
  {
    return false;//ds_dll expects zero if a match is found
  }
  return true;
}


/*=====================================================================
  FUNCTION find_record_delete_ip
======================================================================*/
/*!
@brief
  Search for a DHCP record based on IP and delete it from /data/dhcp_hosts

@return
  bool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*====================================================================*/
bool find_record_delete_ip(uint32_t client_reserved_ip)
{
  in_addr addr;
  char temp[DHCP_RECORD_LENGTH];
  bool match_result = false;
  FILE *tmpfile = NULL;

  addr.s_addr = htonl(client_reserved_ip);
  tmpfile = fopen(DHCP_RESERVATION_TMP_FILE,"wb");
  if ( NULL == tmpfile)
  {
    LOG_MSG_ERROR("\nUnable to open %s file!!",DHCP_RESERVATION_TMP_FILE,0,0);
    return false;
  }
  while (fgets(temp,DHCP_RECORD_LENGTH,dhcp_hosts_fp)!= NULL )
  {
    if (( strstr(temp,inet_ntoa(addr)))!= NULL)
    {
      LOG_MSG_INFO1("IP-address: %s found!!\n",inet_ntoa(addr),0,0);
      match_result = true;
    }
    else
    {
      fputs(temp,tmpfile);
    }
  }
  if (match_result == false)
  {
    LOG_MSG_INFO1("IP-address: %s NOT found!!\n",inet_ntoa(addr),0,0);
  }
  fclose(tmpfile);
  fclose(dhcp_hosts_fp);
  snprintf(temp, sizeof(temp), "mv %s %s",DHCP_RESERVATION_TMP_FILE,DHCP_HOSTS_FILE);
  ds_system_call(temp,strlen(temp));
  return true;
}

/*=====================================================================
  FUNCTION SetLanInXlatRangeToDefault
======================================================================*/
/*!
@brief
  This function will Check if lan Ip range is in xlat ip range.If yes then set to default

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
bool QCMAP_LAN::SetLanInXlatRangeToDefault(qcmap_msgr_lan_config_v01 *lan_config)
{

  struct in_addr xlat_ip,xlat_mask,tmp;
  inet_aton(XLATIP,&xlat_ip);
  inet_aton(XLATNETWORK,&xlat_mask);
  bool ret =false;

  if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
  {
    LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
    return false;
  }

  //check Gatway Ip if in xlat range
  if ((htonl(lan_config->gw_ip) & (htonl(lan_config->netmask))) == (xlat_ip.s_addr & xlat_mask.s_addr))
  {
    tmp.s_addr = htonl(lan_config->gw_ip);
    LOG_MSG_ERROR("Gateway Ip %s is in Xlat Ip range",inet_ntoa(tmp),0,0);
    ret = true;
  }
  //check if dhcp range start ip is in xlat range
  if ((htonl(lan_config->dhcp_config.dhcp_start_ip) & (htonl(lan_config->netmask))) == (xlat_ip.s_addr & xlat_mask.s_addr))
  {
    tmp.s_addr = htonl(lan_config->dhcp_config.dhcp_start_ip);
    LOG_MSG_ERROR("Range Start Ip %s is in Xlat Ip range.",inet_ntoa(tmp),0,0);
    ret = true;
  }
  //check if dhcp range end ip is in xlat range
  if ((htonl(lan_config->dhcp_config.dhcp_end_ip) & (htonl(lan_config->netmask))) == (xlat_ip.s_addr & xlat_mask.s_addr))
  {
    tmp.s_addr = htonl(lan_config->dhcp_config.dhcp_end_ip);
    LOG_MSG_ERROR ("Range end Ip %s is in Xlat Ip range.",inet_ntoa(tmp),0,0);
    ret = true;
  }


  if (ret == true)
  {
    LOG_MSG_ERROR ("LAN IPv4 Subnet collides with XLAT private subnet."
                   "setting to default value & storing in object",0,0,0);
    this->lan_cfg.sub_net_mask = lan_config->netmask = ntohl(inet_addr(APPS_SUB_NET_MASK));
    this->lan_cfg.apps_ip_addr = lan_config->gw_ip = ntohl(inet_addr(APPS_LAN_IP_ADDR));
    this->lan_cfg.dhcp_start_address = lan_config->dhcp_config.dhcp_start_ip =
                                              ntohl(inet_addr(APPS_LAN_IP_ADDR)) + MAX_LAN_CLIENTS ;
    this->lan_cfg.dhcp_end_address = lan_config->dhcp_config.dhcp_end_ip =
                                           lan_config->dhcp_config.dhcp_start_ip  + MAX_LAN_CLIENTS;
  }
  else
  {
    LOG_MSG_INFO1 ("None of Ip is Not in Xlat Ip range.Go ahead ",0,0,0);
  }

  return ret;

}


/*=====================================================================
  FUNCTION SetLANConfig
======================================================================*/
/*!
@brief
  This function will set the lan configuration.

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
bool QCMAP_LAN::SetLANConfig(qcmap_msgr_lan_config_v01 *lan_config,
                                qmi_error_type_v01 *qmi_err_num)
{
  in_addr addr;
  bool is_xlat_network = false;

  // Initialize QMI error
  *qmi_err_num = QMI_ERR_NONE_V01;

  /* sync /data/dhcp-hosts with the existing DHCP Reservation records*/
  dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "wb");
  if (NULL == dhcp_hosts_fp)
  {
      /* File could not be opened for writing/append*/
      LOG_MSG_ERROR("\n Failed to create %s!!",DHCP_HOSTS_FILE,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
  }

  /* check for address conflicts before setting the wlan configuration */
  if( this->CheckforAddrConflict(lan_config, NULL ))
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (lan_config != NULL)
  {
    this->lan_cfg.apps_ip_addr = lan_config->gw_ip;
    this->lan_cfg.sub_net_mask = lan_config->netmask;
    this->lan_cfg.enable_dhcpd = lan_config->enable_dhcp;

     // Checking DHCP Configuration.
    if (this->lan_cfg.enable_dhcpd )
    {
       this->lan_cfg.dhcp_start_address = lan_config->dhcp_config.dhcp_start_ip;
       this->lan_cfg.dhcp_end_address = lan_config->dhcp_config.dhcp_end_ip;

       this->lan_cfg.dhcp_lease_time=lan_config->dhcp_config.lease_time;

       /* DHCP config checks. */
       if ( ( ( this->lan_cfg.apps_ip_addr
            &this->lan_cfg.sub_net_mask) !=
            ( this->lan_cfg.dhcp_start_address &this->lan_cfg.sub_net_mask))||
            ( this->lan_cfg.dhcp_start_address <this->lan_cfg.apps_ip_addr))
      {
        this->lan_cfg.dhcp_start_address =this->lan_cfg.apps_ip_addr+MAX_LAN_CLIENTS;
        addr.s_addr =htonl(this->lan_cfg.dhcp_start_address );
        LOG_MSG_INFO1("SSID1 DHCP Start addr provide is invalid."
                     "Setting it to default value %s\n",inet_ntoa(addr),0,0);
      }

      if ( ( ( this->lan_cfg.apps_ip_addr & this->lan_cfg.sub_net_mask) !=
          ( this->lan_cfg.dhcp_end_address & this->lan_cfg.sub_net_mask))||
          ( this->lan_cfg.dhcp_end_address < this->lan_cfg.dhcp_start_address))
      {
        this->lan_cfg.dhcp_end_address =
       this->lan_cfg.dhcp_start_address + MAX_LAN_CLIENTS;
        addr.s_addr =
        htonl(this->lan_cfg.dhcp_end_address );
        LOG_MSG_INFO1("SSID1 DHCP End addr provided is invalid."
                     "Setting it to default value %s\n",inet_ntoa(addr),0,0);
      }
      //check if XLAT range
      if (this->SetLanInXlatRangeToDefault(lan_config))
      {
        LOG_MSG_ERROR("LAN IPv4 Subnet collides with XLAT private subnet."
                      "Done setting to default",0,0,0);
        is_xlat_network = true;
        *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      }

    }
  }
  if (is_xlat_network != true)
  {
    /*sync the /data/dhcp-hosts file now*/
    this->sync_dhcp_hosts();
    if ( dhcp_hosts_fp )
    {
      fclose(dhcp_hosts_fp);
    }
  }
  if(lan_config != NULL)
  {
    if(0 != this->bridge_vlan_id)
    {
      this->SetLANConfigToXML();
    }
    else
    {
      this->SetLANConfigToXML(lan_config);
    }
  }
  if(is_xlat_network)
  {
    return false;
  }
  return true;

}


/*=====================================================================
  FUNCTION GetLANConfig
======================================================================*/
/*!
@brief
  This function will get the lan configuration for QMI.

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
bool QCMAP_LAN::GetLANConfig(qcmap_msgr_lan_config_v01 *lan_config,
                                             qmi_error_type_v01 *qmi_err_num) const
{
  uint32_t value_debug=0; //used to print the values of individual LAN config as LOG MSGs
  char str[INET6_ADDRSTRLEN] = {0};

  if(NULL == lan_config)
  {
    LOG_MSG_ERROR("NULL params.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }

  memset(lan_config, 0, sizeof(lan_config));

  /* Populate AP Configuration. */
  lan_config->gw_ip = this->lan_cfg.apps_ip_addr;
  lan_config->netmask = this->lan_cfg.sub_net_mask;
  lan_config->enable_dhcp = this->lan_cfg.enable_dhcpd;

  value_debug= htonl(lan_config->gw_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Gateway Address : '%s'",str,0,0);

  value_debug= htonl(lan_config->netmask);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Subnet Mask : '%s'",str,0,0);

  LOG_MSG_INFO1("Enable DHCP: %b , ",lan_config->enable_dhcp,0,0)

  if ( lan_config->enable_dhcp == TRUE )
  {
    lan_config->dhcp_config.dhcp_start_ip = this->lan_cfg.dhcp_start_address;
    lan_config->dhcp_config.dhcp_end_ip = this->lan_cfg.dhcp_end_address;
    lan_config->dhcp_config.lease_time = this->lan_cfg.dhcp_lease_time;
    value_debug= htonl(lan_config->dhcp_config.dhcp_start_ip);
    readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
    LOG_MSG_INFO1("\nDHCP Start Address : '%s'",str,0,0);
    value_debug= htonl(lan_config->dhcp_config.dhcp_end_ip);
    readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
    LOG_MSG_INFO1("\nDHCP End Address : '%s'",str,0,0);
    LOG_MSG_INFO1("\nDHCP Lease Time: '%d'",lan_config->dhcp_config.lease_time,0,0);

  }

  return true;
}

/*=====================================================================
  FUNCTION GetLANConfig
======================================================================*/
/*!
@brief
  This function will get the lan configuration for this LAN object

@return
  const qcmap_cm_lan_conf_t* lan_cfg

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
const qcmap_cm_lan_conf_t* QCMAP_LAN::GetLANConfig(void) const
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  return (const qcmap_cm_lan_conf_t*)(&(this->lan_cfg));
}

/*=====================================================================
  FUNCTION GetPrevLANConfig
======================================================================*/
/*!
@brief
  This function will get the saved prev lan configuration for this LAN object

@return
  const qcmap_cm_lan_conf_t* lan_cfg

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
const qcmap_msgr_lan_config_v01* QCMAP_LAN::GetPrevLANConfig(void) const
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  return (const qcmap_msgr_lan_config_v01*)(&(this->prev_lan_config));
}


/*=====================================================================
  FUNCTION ActivateLAN
======================================================================*/
/*!
@brief
  Brings up the lan with the latest wlan configuration and mode.

@parameters
  admin - If set to true forcefully re-activate LAN, even if no change in cfg.

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
bool QCMAP_LAN::ActivateLAN
(
  qmi_error_type_v01 *qmi_err_num,
  bool admin
)
{
  LOG_MSG_INFO1("QCMAP_ConnectionManager::ActivateLAN",0,0,0);
  bool teth_link1_enable = false, teth_link2_enable = false;
  bool wlan_enable = false;
  char teth_intf_name1 [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char teth_intf_name2 [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char a5_guest_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char a5_guest2_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char a5_guest3_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE], subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_gw_ip1[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_gw_ip2[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  int num_ifaces = 0;
  scm_msgr_wlan_if_type_enum_v01 mode[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(FALSE);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);

  qcmap_msgr_ethernet_mode_v01 eth_backhaul_mode =
                                     QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01;
  QCMAP_Virtual_LAN* QcMapVLANMgr=QCMAP_Virtual_LAN::Get_Instance(false);
  QCMAP_L2TP* QcMapL2TPMgr=QCMAP_L2TP::Get_Instance(false);
/*------------------------------------------------------------------------*/
  QCMAP_Tethered_Backhaul::GetSetConfigFromXML(GET_VALUE,
                                             QCMAP_TETH_ETH_BACKHAUL,
                                             (void *)&eth_backhaul_mode);

  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;

  switch(this->bridge_vlan_id)
  {
    case(DEFAULT_BRIDGE_ID): //default LAN object
    {
      // Wlan related addresses
      if (QcMapWLANMgr)
      {
        addr.s_addr =
        htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].ll_ip_addr);
        strlcpy(a5_ip, inet_ntoa(addr), 16);

        addr.s_addr =
        htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr);
        strlcpy(a5_guest_ip, inet_ntoa(addr), 16);

        addr.s_addr =
        htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_2_INDEX].ll_ip_addr);
        strlcpy(a5_guest2_ip, inet_ntoa(addr), 16);

        addr.s_addr =
        htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_3_INDEX].ll_ip_addr);
        strlcpy(a5_guest3_ip, inet_ntoa(addr), 16);

        addr.s_addr =
        htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
        strlcpy(netmask, inet_ntoa(addr), 16);
      }
      break;
    }
    default: //for non-default LAN object
    {
      break;
    }
  }

  // USB related link local address
  if(QcMapTetheringMgr)
  {
    // TETH related link local address
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr);
    strlcat(teth_gw_ip1, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].gateway_addr);
    strlcat(teth_gw_ip2, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapTetheringMgr->ll_subnet_mask);
    strlcpy(teth_netmask, inet_ntoa(addr), 16);
  }
  // Sanity check to ensure that we only proceed if either the mode or AP configuration
  //has changed
  // Added check to ensure when in AP-STA bridge mode; ActivateLAN is also called from
  //DHCP Fail Case
  if (!this->IsLanCfgUpdated() && !admin)
  {
    LOG_MSG_INFO1(" There is no change in the LAN Config",0,0,0);
    return true;
  }
  memset(teth_intf_name1, 0, sizeof(teth_intf_name1));
  memset(teth_intf_name2, 0, sizeof(teth_intf_name2));
  // AP Configuration has changed and Bridge interface was up, need to apply the new configuration.
  // This will also take care if the mode has changed
  if(this->bridge_inited)
  {
    // Bring down USB TE
    if (QcMapTetheringMgr)
    {
      if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
           (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type !=
                    QCMAP_QTI_TETHERED_LINK_PPP))
      {
         teth_link1_enable = true;
         if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type
                                     == QCMAP_QTI_TETHERED_LINK_RNDIS )
         {
           strlcpy(teth_intf_name1, RNDIS_IFACE,sizeof(teth_intf_name1));
         }
         else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type
                                     == QCMAP_QTI_TETHERED_LINK_ECM )
         {
           strlcpy(teth_intf_name1, ECM_IFACE,sizeof(teth_intf_name1));
         }
         else
         {
           LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
           *qmi_err_num = QMI_ERR_INTERNAL_V01;
           return false;
         }
         if (DS_TARGET_LE_CHIRON == ds_get_target())
         {
           ds_system_call("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC ",
                   strlen("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC "));
         }
         else
         {
           ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",strlen("echo 0 > /sys/class/android_usb/android0/enable "));
         }
         snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif bridge%d %s",this->bridge_vlan_id,
                   teth_intf_name1);
         ds_system_call(command, strlen(command));
         if (QcMapVLANMgr)
         {
           QcMapVLANMgr->AddDeleteVLANOnIface(teth_intf_name1, false);
         }
         if (QcMapL2TPMgr)
         {
           QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(teth_intf_name1,false);
         }
      }
     else if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
               (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                    QCMAP_QTI_TETHERED_LINK_PPP))
      {
        /* Kill the PPP Daemon. Right now there is no way we can make PPP to renegotiate. */
        ds_system_call("killall -TERM pppd", strlen("killall -TERM pppd"));
      }

     if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) &&
          eth_backhaul_mode != QCMAP_MSGR_ETHERNET_WAN_ROUTER_V01)
     {
         teth_link2_enable = true;
         if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type == QCMAP_QTI_TETHERED_LINK_ETH )
         {
             strlcpy(teth_intf_name2, ETH_IFACE, sizeof(teth_intf_name2));
         } else
         {
            LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                          QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type,0,0);
           *qmi_err_num = QMI_ERR_INTERNAL_V01;
           return false;
         }
         snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s down",teth_intf_name2);
         ds_system_call(command, strlen(command));
         snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif bridge%d %s",this->bridge_vlan_id,
                   teth_intf_name2);
         ds_system_call(command, strlen(command));
         if (QcMapVLANMgr)
         {
           QcMapVLANMgr->AddDeleteVLANOnIface(teth_intf_name2, false);
         }
         if (QcMapL2TPMgr)
         {
           QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(teth_intf_name2,false);
         }
     }
    }

    switch(this->bridge_vlan_id)
    {
      case(DEFAULT_BRIDGE_ID): //default LAN object
      {
        // Disable WLAN if we are not in STA mode and WLAN is enabled
        // Also Disable WLAN if we are in STA mode and admin=true; DHCP Fail case
        if (QcMapWLANMgr && ( (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_V01) ||
            (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) ||
            (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01) ||
            (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_AP_AP_V01) ||
            (admin == true)))
        {
          wlan_enable = true;
          QCMAP_WLAN::DisableWLAN(qmi_err_num);
        }
        else if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
                                  QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01))
        {
          wlan_enable = true;

          if (!QcMapMgr->dual_wifi_mode)
          {
            /* Stop Hostapd first so client disconnect events are generated */
            QcMapWLANMgr->StopHostAPD();

            /* Disable WLAN and remove wlan from bridge Iface */
            snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d down",QcMapMgr->ap_dev_num1);
            ds_system_call(command, strlen(command));

            if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
            {
              snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d down",QcMapMgr->ap_dev_num2);
              ds_system_call(command, strlen(command));
            }

            snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
                      BRIDGE_IFACE,QcMapMgr->ap_dev_num1);
            ds_system_call(command,strlen(command));

            if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
            {
              snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
                      BRIDGE_IFACE,QcMapMgr->ap_dev_num2);
              ds_system_call(command,strlen(command));
            }
          }
        }
        // Bring down BT Tethering & Send Indication If BT Tethering is UP
        if (QcMapBTTethMgr && QcMapBTTethMgr->bt_tethering_status ==
                 QCMAP_MSGR_BT_TETHERING_MODE_UP_V01)
        {
          if (!QCMAP_BT_TETHERING::BringdownBTTethering(qmi_err_num))
          {
            LOG_MSG_ERROR("Error in Bring down BT tethering",0,0,0);
            return false;
          }
          else
          {
            cbPtr = (qmi_qcmap_msgr_status_cb_data *)QcMapMgr->GetQCMAPClientCBDatata();
            //send indication
            if (cbPtr && cbPtr->bt_tethering_status_cb)
            {
              LOG_MSG_INFO1(" Sending bt TETHERING BRING DOWN Indication to BT module. ",0,0,0);
              cbPtr->bt_tethering_status_cb(cbPtr, QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01);
            }
          }
        }
        break;
      }
      default: //for non-default LAN object
      {
        break;
      }
    }

    // Disable Bridge force-fully.
    if(this->bridge_inited)
    {
      LOG_MSG_INFO1(" Delete the bridge interface. forcefully \n",0,0,0);
      this->DelBridge();
    }

    this->InitBridge();

    //Add the default v6 route back if backhaul is up
    if(QcMapBackhaulMgr)
    {
      QCMAP_Backhaul::SwitchToBackhaul(QCMAP_Backhaul::current_backhaul);
    }

    switch(this->bridge_vlan_id)
    {
      case(DEFAULT_BRIDGE_ID): //default LAN object
      {
        // Enable WLAN
        if(wlan_enable)
        {
          sleep(QcMapMgr->wlan_restart_delay);

          if ((!QcMapWLANMgr || (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 &&
                                 QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 &&
                                 QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)) ||
              (admin == true))
          {
            QCMAP_WLAN::EnableWLAN(qmi_err_num);
          }
          else
          {
            if (!QcMapMgr->dual_wifi_mode)
            {
              /* In legacy only 1 AP needs to be restarted here(AP-STA), otherwise the EnableWLAN()
                 above should be hit */

              // Start Hostapd
              QcMapWLANMgr->StartHostAPD(QCMAP_MSGR_INTF_AP_INDEX, QcMapMgr->ap_dev_num1);

              /* Config WLAN iface IP's, and add to bridge */
              snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
                       QcMapMgr->ap_dev_num1, a5_ip, netmask);
              ds_system_call(command, strlen(command));

              addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface
                                  [QCMAP_MSGR_INTF_AP_INDEX].ll_ip_addr &
                                  QcMapWLANMgr->wlan_cfg.interface
                                  [QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);

              strlcpy(subnet, inet_ntoa(addr), 16);
              snprintf(command, MAX_COMMAND_STR_LEN,
                       "route del -net %s netmask %s dev wlan%d", subnet, netmask,
                       QcMapMgr->ap_dev_num1);
              ds_system_call(command, strlen(command));
              snprintf( command, MAX_COMMAND_STR_LEN, "brctl addif %s wlan%d",
                        BRIDGE_IFACE,QcMapMgr->ap_dev_num1);
              ds_system_call(command,strlen(command));
            }
            else
            {
              switch (QcMapWLANMgr->wifi_mode)
              {
                 case QCMAP_MSGR_WLAN_MODE_AP_AP_AP_V01:
                   mode[2] = SCM_MSGR_WLAN_IF_SAP_3_V01;
                   num_ifaces++;
                 case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
                 case QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01:
                   mode[1] = SCM_MSGR_WLAN_IF_SAP_2_V01;
                   num_ifaces++;
                 case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
                 case QCMAP_MSGR_WLAN_MODE_AP_V01:
                   mode[0] = SCM_MSGR_WLAN_IF_SAP_1_V01;
                   num_ifaces++;
              }
              if(!QcMapWLANSCMMgr->SCMReconfig(num_ifaces, mode, SCM_MSGR_WLAN_IF_RESTART_V01))
              {
                LOG_MSG_ERROR("Unable to reconfig AP's",0,0,0);
               *qmi_err_num = QMI_ERR_INTERNAL_V01;
               return false;
              }
            }
          }
        }
        break;
      }

      default:
      {
        break;
      }
    }

    // Enable USB if it was enabled before
    if (teth_link1_enable)
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
      snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
                teth_intf_name1,teth_gw_ip1,teth_netmask);
      ds_system_call(command, strlen(command));

      /*If vlan is enabled for rndis/ecm interface and mapped to bridge0,
        then skip adding this interface to bridge0 */
      if (strncmp(this->bridge_name, BRIDGE_IFACE, strlen(BRIDGE_IFACE)) == 0)
      {
        CheckInterfacesWithVLAN();
        AddPhyInterfaceToDefaultBridge(QCMAP_MSGR_INTERFACE_TYPE_RNDIS_V01);
        AddPhyInterfaceToDefaultBridge(QCMAP_MSGR_INTERFACE_TYPE_ECM_V01);
      }

      if (QcMapVLANMgr)
      {
        QcMapVLANMgr->AddDeleteVLANOnIface(teth_intf_name1, true);
      }
      if (QcMapL2TPMgr)
      {
        QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(teth_intf_name1,true);
      }
    }

    if (teth_link2_enable)
    {
      snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
                teth_intf_name2,teth_gw_ip2,teth_netmask);
      ds_system_call(command, strlen(command));

      /*If vlan is enabled for eth interface and mapped to bridge0,
        then skip adding this interface to bridge0 */
      if (strncmp(this->bridge_name, BRIDGE_IFACE, strlen(BRIDGE_IFACE)) == 0)
      {
        CheckInterfacesWithVLAN();
        AddPhyInterfaceToDefaultBridge(QCMAP_MSGR_INTERFACE_TYPE_ETH_V01);
      }
      QCMAP_LAN::ToggleAutoNegForEth(teth_intf_name2);
      if (QcMapVLANMgr)
      {
        QcMapVLANMgr->AddDeleteVLANOnIface(teth_intf_name2, true);
      }
      if (QcMapL2TPMgr)
      {
        QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(teth_intf_name2,true);
      }
    }
  }
  return true;
}


/*=====================================================================
  FUNCTION IsLanCfgUpdated
======================================================================*/
/*!
@brief
  Check is current applied LAN config and the prev LAN config are the same

@parameters
   none

@return
   returns succesful if prev and current lan config are different

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
bool QCMAP_LAN::IsLanCfgUpdated(void) const
{

  if ((this->lan_cfg.apps_ip_addr != this->prev_lan_config.gw_ip) ||
       (this->lan_cfg.sub_net_mask != this->prev_lan_config.netmask) ||
       (this->lan_cfg.enable_dhcpd != this->prev_lan_config.enable_dhcp)) {
     return true;
  }

  if (this->lan_cfg.enable_dhcpd) {
    if ((this->lan_cfg.dhcp_start_address != this->prev_lan_config.
         dhcp_config.dhcp_start_ip) ||
        (this->lan_cfg.dhcp_end_address != this->prev_lan_config.
         dhcp_config.dhcp_end_ip ) ||
        (this->lan_cfg.dhcp_lease_time!= this->prev_lan_config.
         dhcp_config.lease_time)){
       return true;
    }
    /* Check if the reservations are updated. */
    if (this->lan_cfg.dhcp_reservations_updated == TRUE)
    {
      return true;
    }
  }
  return false;
}




/*=====================================================================
  FUNCTION StartDHCPD
======================================================================*/
/*!
@brief
  This function will start the dhcp service with the configured values
  for the enabled interfaces.

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
/*====================================================================*/
bool QCMAP_LAN::StartDHCPD()
{
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr = GET_BACKHAUL_WWAN_OBJECT(this->profile_handle);
  char command[MAX_COMMAND_STR_LEN] = {0};
  char scratch_buf[MAX_COMMAND_STR_LEN] = {0};
  in_addr start, end, subnet_mask, gw_addr;

  ds_system_call("echo QCMAP:Start DHCP server > /dev/kmsg",
                 strlen("echo QCMAP:Start DHCP server > /dev/kmsg"));

  if (this->lan_cfg.dhcp_start_address == NULL || this->lan_cfg.dhcp_end_address == NULL
      || this->lan_cfg.sub_net_mask == NULL || this->lan_cfg.dhcp_lease_time == NULL)
  {

    ds_system_call("echo QCMAP:Start DHCP server FAILED > /dev/kmsg",
          strlen("echo QCMAP:Start DHCP server FAILED > /dev/kmsg"));
    return false;
  }

  memset(command, 0, sizeof(command));
  memset(scratch_buf, 0, sizeof(scratch_buf));
  if (this->lan_cfg.enable_dhcpd)
  {
    /* INET_NTOA_SIZE is the size of static buff used in inet_ntoa function */
    start.s_addr = htonl(this->lan_cfg.dhcp_start_address);

    switch(this->bridge_vlan_id)
    {
      case(DEFAULT_BRIDGE_ID): //default bridge0 case
      {
        snprintf(scratch_buf, MAX_COMMAND_STR_LEN, "dnsmasq --conf-file=%s --dhcp-leasefile=%s "
                 "--addn-hosts=%s --pid-file=%s -i bridge%d -I lo -z "
                 "--dhcp-range=bridge%d,%s,",
                 DNSMASQ_CONFIG_FILE, DNSMASQ_LEASE_FILE, QCMAP_HOST_PATH, DNSMASQ_PID_FILE,
                 this->bridge_vlan_id, this->bridge_vlan_id, inet_ntoa(start));
        break;
      }
      default: //bridgeX case, need to point to specific resolv file
      {
        snprintf(scratch_buf, MAX_COMMAND_STR_LEN, "dnsmasq --conf-file=%s --dhcp-leasefile=%s "
                 "--addn-hosts=%s --pid-file=%s -r %s -i bridge%d -I lo -z "
                 "--dhcp-range=bridge%d,%s,",
                 DNSMASQ_CONFIG_FILE, this->dnsmasq_lease_file_path, this->host_file_path,
                 this->dnsmasq_pid_file_path, this->dnsmasq_resolv_file_path,
                 this->bridge_vlan_id, this->bridge_vlan_id, inet_ntoa(start));
        break;
      }
    }
    strlcpy(command, scratch_buf, MAX_COMMAND_STR_LEN);

    /* Reserve IP for PPP only if we have more than 1 IP available.
       Also check if it is in DUN+SoftAp Mode*/
    if (( this->lan_cfg.dhcp_end_address >
          this->lan_cfg.dhcp_start_address) && (QCMAP_ConnectionManager::CheckDUNSoftAPMode()))
    {
      end.s_addr =
             htonl(this->lan_cfg.dhcp_end_address - 1);
      this->lan_cfg.ppp_reserved_ip = this->lan_cfg.dhcp_end_address;
    }
    else
    {
      LOG_MSG_INFO1("No IP reserved for DUN client", 0, 0, 0);
      end.s_addr =htonl(this->lan_cfg.dhcp_end_address);
      this->lan_cfg.ppp_reserved_ip = 0;
    }

    /* inet_ntoa returns in a static buffer subsequent calls will overwrite the buffer,
       DO NOT combine them into 1 print */
    snprintf( scratch_buf, MAX_COMMAND_STR_LEN,"%s,", inet_ntoa(end));
    strlcat(command , scratch_buf , MAX_COMMAND_STR_LEN);

    subnet_mask.s_addr = htonl(this->lan_cfg.sub_net_mask);
    snprintf(scratch_buf, MAX_COMMAND_STR_LEN,"%s,%d", inet_ntoa(subnet_mask),
             this->lan_cfg.dhcp_lease_time);
    strlcat(command , scratch_buf , MAX_COMMAND_STR_LEN);

    /*if dhcp-hostsfile exits append this to the dnsmasq option*/
    dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "rb");
    /* File could not be opened for writing/append*/
    if (NULL == dhcp_hosts_fp)
    {
        LOG_MSG_INFO1("\nFile %s does not exists!!",DHCP_HOSTS_FILE,0,0);
    }
    else
    {
        /* Insert the DHCP hosts file.*/
        snprintf(scratch_buf, MAX_COMMAND_STR_LEN, " --dhcp-hostsfile=%s",
                 DHCP_HOSTS_FILE);
        strlcat(command, scratch_buf, MAX_COMMAND_STR_LEN);
        fclose(dhcp_hosts_fp);
    }

    if (this->lan_cfg.apps_ip_addr != NULL)
    {
      /* DNS Gateway address, to used in DHCP ACK message*/
      gw_addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
      snprintf(scratch_buf, MAX_COMMAND_STR_LEN, " --dhcp-option-force=6,%s",
               inet_ntoa(gw_addr));
      strlcat(command, scratch_buf, MAX_COMMAND_STR_LEN);
    }

    /* Copy DHCP command. Used when LAN config has not been re-activated
       with changed values, but few other components of DHCPD change. */
    strlcpy(this->dhcp_command_str, command, MAX_COMMAND_STR_LEN);

    /*Update DHCPD daemon with network SIP server, MTU info */
    this->UpdateDHCPDNetworkInfo();
  }
  //if dhcp is disabled add support for dhcpv6 proxy
  else if(QcMapBackhaulWWANMgr &&
          QcMapBackhaulWWANMgr->dhcpv6_dns_conf.dhcpv6_enable_state  ==
          QCMAP_MSGR_DHCPV6_MODE_UP_V01 )
  {

    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), " kill -9 $(cat %s)", this->dnsmasq_pid_file_path);
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "dnsmasq -i bridge%d -I lo -z -r %s",
             this->bridge_vlan_id, this->dnsmasq_resolv_file_path);
    //add dhcpv6 dns proxy options
    AddDHCPV6ProxyDNSOption(command);
    ds_system_call(command, strlen(command));
    ds_system_call("echo QCMAP:DHCP server is running on DHCPV6 proxy mode only > /dev/kmsg",
                 strlen("echo QCMAP:DHCP server is running on DHCPV6 proxy mode only > /dev/kmsg"));
  }

  ds_system_call("echo QCMAP:DHCP server started > /dev/kmsg",
          strlen("echo QCMAP:DHCP server started > /dev/kmsg"));
  return true;
}


/*=====================================================================
  FUNCTION StopDHCPD
======================================================================*/
/*!
@brief
  Stops the dhcp services started.

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
/*====================================================================*/

bool QCMAP_LAN::StopDHCPD()
{
  char command[MAX_COMMAND_STR_LEN] = {0};

  memset(command, 0, sizeof(command));
  snprintf(command, sizeof(command), " kill -9 $(cat %s)", this->dnsmasq_pid_file_path);
  ds_system_call(command, strlen(command));

/*-------------------------------------------------------------------------
  Delete the DNS masq leases which were allocated earlier. This is
  required because
  - the dnsmasq lease file stores the MAC address to IP address mapping
    and does not allocate the stored IP addresses to DHCP requests
    coming in with different MAC address
  - but with every reboot in case of USB the host uses a different MAC address
  - thus in order to assign the one constant IP address to the USB tethered
    device we delete this mapping.
--------------------------------------------------------------------------*/
  memset(command, 0, sizeof(command));
  snprintf(command, sizeof(command), "rm %s", this->dnsmasq_lease_file_path);
  ds_system_call(command, strlen(command));

  /* Clear off the reservations flag. */
  this->lan_cfg.dhcp_reservations_updated = FALSE;

  return true;
}


/*=====================================================================
  FUNCTION AddDHCPReservRecord
======================================================================*/
/*!
@brief
  Checks existing DHCP Reservation Records for the redundancy and if unique
  adds a new DHCP Reservation Record.

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record

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
bool QCMAP_LAN::AddDHCPReservRecord
(
qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
qmi_error_type_v01 *qmi_err_num
)
{
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  int i;
  in_addr addr;
  bool mac_addr_non_empty = true, client_ip_match = false;
  int  client_mac_match = 0, client_name_match = 0;
  qcmap_msgr_dhcp_reservation_v01 *local_rec = NULL;
  uint32 num_records = 0;

  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_dhcp_information_info_list_t* dhcpList = NULL;
  ds_dll_el_t * node = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
  {
    LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
    return false;
  }

  dhcpList = &(this->lan_cfg.dhcp_reservation_records);

  num_records = this->lan_cfg.num_dhcp_reservation_records;

   /* Check if we have already reached a max limit on the records*/
  if ( num_records  == QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01 )
  {
    /* we reach MAX entries */
    LOG_MSG_ERROR("\nReached Max limit on DHCP Reservation Record Entries!!",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }

  node = dhcpList->dhcpInfoEntryListHead;
  node = ds_dll_next (node, (const void**)(&dhcp_record));

  mac_addr_non_empty = check_non_empty_mac_addr(dhcp_reserv_record->client_mac_addr,
                                                mac_addr_string);

  /*check for duplicate entries*/
  for ( i = 0; i < num_records && dhcp_record; i++ )
  {

    local_rec = dhcp_record;
    client_ip_match = ( dhcp_reserv_record->client_reserved_ip ==
                        local_rec->client_reserved_ip );
    client_mac_match = memcmp(dhcp_reserv_record->client_mac_addr,
                              local_rec->client_mac_addr,
                              sizeof(dhcp_reserv_record->client_mac_addr));
    client_name_match = memcmp(dhcp_reserv_record->client_device_name,
                               local_rec->client_device_name,
                               QCMAP_MSGR_DEVICE_NAME_MAX_V01);

    if ( ( ( 0 == client_mac_match )&& mac_addr_non_empty ) ||
         ( ( 0 == client_name_match )&&
            dhcp_reserv_record->client_device_name[0] != '\0' )||
         ( client_ip_match ) )
    {
      /* we already added this one */
      LOG_MSG_ERROR("\n IP Address already reserved for another Client!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    node = ds_dll_next (node, (const void**)(&dhcp_record));
  }

  /* if  <ipaddr>  is gievn outside the AP dhcp-range , throw an error */
  if ( ( this->lan_cfg.dhcp_start_address >
                   dhcp_reserv_record->client_reserved_ip) ||
        ( (this->lan_cfg.dhcp_end_address - 1) <
                  dhcp_reserv_record->client_reserved_ip) )
  {
      LOG_MSG_ERROR("\nIP address is outside the dhcp range!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
  }

  /* if DHCP enable flag is FALSE for a record during addition, then the corresponding
     record would be not added in the dhcp-hosts file.
     however, we would be still adding the record in the XML */
  if ( dhcp_reserv_record->enable_reservation )
  {
   /* If we have reached this far, then check if /data/dhcp_hosts file exists.
     if it does not, then create a file and add the following per line
     for AP clients:
     [<hwaddr>] [,<hostname>] [,<ipaddr>]
     for USB client:
     [,<hostname>] [,<ipaddr>]
   */
   dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "ab");

   /* File could not be opened for writing/append*/
   if (NULL == dhcp_hosts_fp)
   {
      LOG_MSG_ERROR("\nUnable to open %s file!!",DHCP_HOSTS_FILE,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
   }
   else
   {
     if ( dhcp_reserv_record->client_device_name[0] != '\0' )
     {
         fprintf(dhcp_hosts_fp,"%s,",dhcp_reserv_record->client_device_name);
     }
     /* mac address is not provided for USB client*/
     if ( mac_addr_non_empty )
     {
       fprintf(dhcp_hosts_fp,"%s,",mac_addr_string);
     }
     addr.s_addr = htonl(dhcp_reserv_record->client_reserved_ip);
     fprintf(dhcp_hosts_fp,"%s\n",inet_ntoa(addr));
     fclose(dhcp_hosts_fp);
     this->lan_cfg.dhcp_reservations_updated = TRUE;
   }
  }

  /* save into the config */
  /* TODO: Have to move the below code to firewall constructor to avoid it from doing
    while adding a Firewall entry and then while fetching from XML file. */
   if (dhcpList->dhcpInfoEntryListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
             information. This is done to make use of the doubly linked list framework which
             is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for node",
                      0,0,0);
        return false;
      }
      dhcpList->dhcpInfoEntryListHead = node;
    }

   dhcp_record = (qcmap_msgr_dhcp_reservation_v01*)ds_malloc
                                      (sizeof(qcmap_msgr_dhcp_reservation_v01));

   if( dhcp_record == NULL )
   {
     LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for"
                   "dhcp reservation entry",0,0,0);
     return false;
   }

   memset(dhcp_record, 0, sizeof(qcmap_msgr_dhcp_reservation_v01));

  memcpy(dhcp_record, dhcp_reserv_record,
         sizeof(qcmap_msgr_dhcp_reservation_v01));

  //Store the dhcp entry in the linked list
  if ((node = ds_dll_enq(dhcpList->dhcpInfoEntryListHead,
                           NULL, (void*)dhcp_record)) == NULL)
  {
      LOG_MSG_ERROR("AddDHCPReservRecord - Error in adding a node",0,0,0);
      ds_free(dhcp_record);
      return false;
  }
  dhcpList->dhcpInfoEntryListTail = node;
  this->lan_cfg.num_dhcp_reservation_records++;

  LOG_MSG_INFO1("\n Writing the DHCP record to XML config!!",0,0,0);
  this->AddDHCPResRecToXML(dhcp_record);
  //Adding to XML here if all the conditions are met.
  return true;
}

/*=====================================================================
  FUNCTION EditDHCPReservRecord
======================================================================*/
/*!
@brief
  Edit a DHCP record

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record

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
bool QCMAP_LAN::EditDHCPReservRecord
( uint32_t *client_ip,
  qcmap_msgr_dhcp_reservation_v01 *record,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i,j;
  in_addr addr;
  bool mac_addr_non_empty = true;
  bool match_result = false, client_ip_match = false;
  uint32 num_records = 0;
  int  client_mac_match = 0, client_name_match = 0;
  qcmap_msgr_dhcp_reservation_v01 *local_rec = NULL;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/
  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_dhcp_information_info_list_t* dhcpList = NULL;

  ds_dll_el_t * node = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
  {
    LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
    return false;
  }

  dhcpList = &(this->lan_cfg.dhcp_reservation_records);


  node = dhcpList->dhcpInfoEntryListHead;
  node = ds_dll_next (node, (const void**)(&dhcp_record));

  /*check if record is existing*/
  num_records = this->lan_cfg.num_dhcp_reservation_records;
  for ( i = 0; i < num_records && dhcp_record; i++ )
  {
    if (*client_ip == dhcp_record->client_reserved_ip )
      break;
    node = ds_dll_next (node, (const void**)(&dhcp_record));
  }
  if ( i == num_records  || !dhcp_record)
  {
      /* Record not found */
      LOG_MSG_ERROR("\nDHCP record not found!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
  }

  /* if  updated <ipaddr>  is gievn outside the AP dhcp-range , throw an error */
  if ( record->client_reserved_ip != 0 )
  {
    if ( ( this->lan_cfg.dhcp_start_address >
                   record->client_reserved_ip) ||
        ( (this->lan_cfg.dhcp_end_address - 1) <
                  record->client_reserved_ip) )
    {
      LOG_MSG_ERROR("\nIP address is outside the dhcp range!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
  }
  /*Check if the mac address is not entered by the user
  If its empty then we have don't have edit the mac address
  for AP client*/
  mac_addr_non_empty = check_non_empty_mac_addr(record->client_mac_addr,
                                                mac_addr_string);

  /*mac address should not be edited for USB clients.Return error*/
  if ( ( check_non_empty_mac_addr(dhcp_record->client_mac_addr, mac_addr_string) == false ) &&
        ( mac_addr_non_empty == true ) )
  {
      LOG_MSG_ERROR("\nMAC address cannot be entered for USB client!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
  }


  node = ds_dll_next (dhcpList->dhcpInfoEntryListHead, (const void**)(&local_rec));

  /*check for duplicate entries*/
  /* We need to increment J to skip the node below, but should go till node is NULL */
  for ( j = 0; node != NULL; j++ )
  {
    /*skip the record which has been matched already*/
    if ( j == i )
    {
      continue;
    }
    client_ip_match = ( local_rec->client_reserved_ip == record->client_reserved_ip );
    client_mac_match = memcmp(record->client_mac_addr,local_rec->client_mac_addr,
                              sizeof(record->client_mac_addr));
    client_name_match = strncmp(record->client_device_name,local_rec->client_device_name,
                                strlen(record->client_device_name));

    if ( ( ( 0 == client_mac_match )&& mac_addr_non_empty ) ||
         ( ( 0 == client_name_match )&& record->client_device_name[0] != '\0') ||
         client_ip_match )
    {
      /* we already added this one */
      LOG_MSG_ERROR("\n IP Address already reserved for another Client!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    node = ds_dll_next (node, (const void**)(&local_rec));
  }
  local_rec = dhcp_record;
   /*replace the existing record with the records that were entered*/
  if ( mac_addr_non_empty )
  {
     memcpy(local_rec->client_mac_addr,record->client_mac_addr,
            sizeof(local_rec->client_mac_addr));
  }
  if ( record->client_reserved_ip != 0)
  {
     local_rec->client_reserved_ip = record->client_reserved_ip;
  }
  if ( record->client_device_name[0] != '\0')
  {
     memcpy(local_rec->client_device_name,record->client_device_name,
            sizeof(local_rec->client_device_name));
  }
  if (local_rec->enable_reservation != record->enable_reservation)
  {
     local_rec->enable_reservation = record->enable_reservation;
  }

  /* check if dhcp-host files exists. it could be possible that dhcp-host files
      has been removed from the user space. if thats the case, create the file */
  dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "rb+");
  /* File could not be opened for writing/append*/
  if (NULL == dhcp_hosts_fp)
  {
       /*dhcp-hosts files might have been removed from user space.
      creating a new file below*/
      LOG_MSG_INFO1("\nFile %s does not exists!!",DHCP_HOSTS_FILE,0,0);
  }
  else
  {
     /* if dhcp-hosts files exits, search for the record based on ip-address.
     if found, delete the same. If not found , it could be possible that
     the entry might have been removed from the user space.
     Go ahead and add the record if dhcp-reservation flag is true*/
     if ( false == ( match_result = find_record_delete_ip(*client_ip)) )
     {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      fclose(dhcp_hosts_fp);
      return false;
     }
  }

  if ( match_result == true ||record->enable_reservation == true)
  {
    dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "ab");
    /* File could not be opened for writing/append*/
    if (NULL == dhcp_hosts_fp)
    {
        LOG_MSG_ERROR("\nUnable to open /data/dhcp_hosts file!!",0,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
    }
    else
    {
       if ( record->enable_reservation == true )
       {
           /* If we have reached this far, then edit as below
             for AP clients:
             [<hwaddr>] [,<hostname>] [,<ipaddr>]
             for USB client:
             [,<hostname>] [,<ipaddr>]
            */
         if ( local_rec->client_device_name[0] != '\0')
         {
           fprintf(dhcp_hosts_fp,"%s,",local_rec->client_device_name);
         }
         /* mac address is not provided for USB client*/
         if ( check_non_empty_mac_addr(local_rec->client_mac_addr, mac_addr_string))
         {
             fprintf(dhcp_hosts_fp,"%s,",mac_addr_string);
         }
         addr.s_addr = htonl(local_rec->client_reserved_ip);
         fprintf(dhcp_hosts_fp,"%s\n",inet_ntoa(addr));
       }
       fclose(dhcp_hosts_fp);
       this->lan_cfg.dhcp_reservations_updated = TRUE;
    }

  /* if it is OK, call write to XML */

  this->EditDHCPResRecToXML(*client_ip, local_rec);
  }

  return true;
}


/*=====================================================================
  FUNCTION DeleteDHCPReservRecord
======================================================================*/
/*!
@brief
  Delete a DHCP record

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record

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
bool QCMAP_LAN::DeleteDHCPReservRecord
( uint32_t *client_reserved_ip,
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_dhcp_information_info_list_t* dhcpList = NULL;
  ds_dll_el_t * node = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
  {
    LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
    return false;
  }

  dhcpList = &(this->lan_cfg.dhcp_reservation_records);

  LOG_MSG_INFO1("client IP is 0x%x",*client_reserved_ip,0,0);

  if (!(node = ds_dll_delete( dhcpList->dhcpInfoEntryListHead,
      &(dhcpList->dhcpInfoEntryListTail),
        (void*)client_reserved_ip, qcmap_compare_dhcp_entries)))

  {
    LOG_MSG_ERROR("DeleteDHCPReservRecord - Error in finding the entry",0,0,0);
    return false;
  }



 /* check if dhcp-host files exists. if
      it does not exists its fine. this might have
      been removed from the user space. Return TRUE*/
    dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "rb+");
  /* File could not be opened for writing/append*/
  if (NULL == dhcp_hosts_fp)
  {
      LOG_MSG_INFO1("\nFile /data/dhcp_hosts does not exists!!",0,0,0);
  }
  else
  {
     /* if dhcp-hosts files exits, seach for the record based on ip-address.
     if found, delete the same. If not found , it could be possible that
     the entry might have been already removed from the user space.*/
     if ( false == find_record_delete_ip(*client_reserved_ip) )
     {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      fclose(dhcp_hosts_fp);
      return false;
     }
     this->lan_cfg.dhcp_reservations_updated = TRUE;
  }


  dhcp_record = (qcmap_msgr_dhcp_reservation_v01*)ds_dll_data(node);
  if (dhcp_record == NULL)
  {
    LOG_MSG_ERROR("DeleteStaticNatEntry - NAT Info is NULL", 0,0,0);
    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;
    return false;
  }

  //Free the device information structure
  ds_free (dhcp_record);
  dhcp_record = NULL;

  //Free the memory of the linked list node
  ds_dll_free(node);
  node = NULL;

  this->lan_cfg.num_dhcp_reservation_records--;

  /* if it is OK, call write to XML */

  this->DeleteDHCPResRecFromXML(*client_reserved_ip);
  return true;
}


/*=====================================================================
  FUNCTION GetDHCPReservRecords
======================================================================*/
/*!
@brief
  Displays all the DHCP Reservation Records.

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_records

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
bool QCMAP_LAN::GetDHCPReservRecords
( qcmap_msgr_dhcp_reservation_v01* dhcp_reservation_record,
  unsigned int* num_entries,
  qmi_error_type_v01 *qmi_err_num
) const
{
  int i;

  QCMAP_CM_LOG_FUNC_ENTRY();
  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_dhcp_information_info_list_t* dhcpList = NULL;
  ds_dll_el_t * node = NULL;

  if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
  {
    LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
    return false;
  }

  dhcpList = &(this->lan_cfg.dhcp_reservation_records);

  node = dhcpList->dhcpInfoEntryListHead;
  node = ds_dll_next (node, (const void**)(&dhcp_record));


  *num_entries = this->lan_cfg.num_dhcp_reservation_records;
  for ( i=0; i < *num_entries && dhcp_record; i++ )
  {
    memcpy(&dhcp_reservation_record[i], dhcp_record,
           sizeof(qcmap_msgr_dhcp_reservation_v01));
    node = ds_dll_next (node, (const void**)(&dhcp_record));
  }
  return true;
}

/*===========================================================================
  FUNCTION AddDHCPV6ProxyDNSOption
==========================================================================*/
/*!
@brief
  Add dhcpv6 proxy DNS option on dnsmasq

@parameters
  char dhcp_command

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

bool QCMAP_LAN::AddDHCPV6ProxyDNSOption(char dhcp_command[]) const
{
  char lladdr[INET6_ADDRSTRLEN] = {0};
  char command[MAX_DHCP_COMMAND_STR_LEN];
  qcmap_msgr_ipv6_sip_server_info_v01
  network_v6_sip_server_info[QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01];
  int count_network_v6_sip_server_info=0;
  qcmap_msgr_pcscf_info_type_v01 v6_pcscf_info_type;
  int cnt;
  char ipv6_addr_buf[INET6_ADDRSTRLEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN=GET_BACKHAUL_WWAN_OBJECT(this->profile_handle);
  const char *bridge_iface = GET_ASSOCIATED_BRIDGE_IFACE_NAME(this->bridge_vlan_id);

  if (QcMapBackhaulWWAN == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! BackhaulWWAN is NULL", 0,0,0);
    return false;
  }

  if (QcMapBackhaulWWAN->GetIfaceLLAddrStr(lladdr, bridge_iface))
  {
    snprintf(command, MAX_COMMAND_STR_LEN," --dhcp-range=%s,%s"
             " --dhcp-option=option6:dns-server,[%s] " , "::","::",lladdr);
    strlcat(dhcp_command, command, MAX_DHCP_COMMAND_STR_LEN);
  }
  else
  {
    LOG_MSG_ERROR("Unable to get LL address for iface.to start DHCPV6 DNS",0,0,0);
  }

  //add ipv6 SIp server Info
  memset(network_v6_sip_server_info, 0, sizeof(network_v6_sip_server_info));
  if ((QcMapBackhaulWWAN->GetV6SIPServerInfo(network_v6_sip_server_info,
                            &count_network_v6_sip_server_info,
                            &qmi_err_num)))
  {
    if (count_network_v6_sip_server_info > 0)
    {
      strlcat(dhcp_command, " --dhcp-option-force=option6:sip-server",
              MAX_DHCP_COMMAND_STR_LEN);
      LOG_MSG_INFO1("Number of network assigned IPV6 SIP server info is %d",
                    count_network_v6_sip_server_info, 0, 0);
      for (cnt=0; cnt<count_network_v6_sip_server_info; cnt++)
      {
        strlcat(dhcp_command, ",", MAX_DHCP_COMMAND_STR_LEN);
        if (network_v6_sip_server_info[cnt].pcscf_info_type ==
            QCMAP_MSGR_PCSCF_IP_ADDRESS_V01)
        {
          v6_pcscf_info_type = QCMAP_MSGR_PCSCF_IP_ADDRESS_V01;
          LOG_MSG_INFO1("Adding network assigned PCSCF address", 0, 0, 0);
          snprintf(command, MAX_COMMAND_STR_LEN,"[%s]" ,inet_ntop(AF_INET6,
                   (in6_addr *)&network_v6_sip_server_info[cnt].pcscf_ipv6_addr,
                   ipv6_addr_buf,sizeof(ipv6_addr_buf)));
          strlcat(dhcp_command, command, MAX_DHCP_COMMAND_STR_LEN);
        }
        else if (network_v6_sip_server_info[cnt].pcscf_info_type ==
                 QCMAP_MSGR_PCSCF_FQDN_V01)
        {
          v6_pcscf_info_type = QCMAP_MSGR_PCSCF_IP_ADDRESS_V01;
          strlcat(dhcp_command, "\"", MAX_DHCP_COMMAND_STR_LEN);
          strlcat(dhcp_command,
                  network_v6_sip_server_info[cnt].pcscf_fqdn,
                  MAX_DHCP_COMMAND_STR_LEN);
          strlcat(dhcp_command, "\"", MAX_DHCP_COMMAND_STR_LEN);
        }
      }
    }
  }
  else
  {
    LOG_MSG_ERROR("Get IPV6 SIP server failed Error: %d ",
                  qmi_err_num, 0, 0);
  }

  return true;
}


/*===========================================================================
  FUNCTION UpdateDHCPDNetworkInfo
==========================================================================*/
/*!
@brief
  This function will re-start the dhcpd service with the configured SIP server, mtu
  information.

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
bool QCMAP_LAN::UpdateDHCPDNetworkInfo()
{
  char dhcp_command[MAX_DHCP_COMMAND_STR_LEN] = {0};
  char scratch_buf[MAX_COMMAND_STR_LEN] = {0};
  struct in_addr addr_pub;
  in_addr pcscf_addr;
  int mtu_size_wwan_ipv4;

  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr = GET_BACKHAUL_WWAN_OBJECT(this->profile_handle);
  qcmap_sip_server_info_list_t *sip_server_info_list = NULL;
  qcmap_cm_sip_server_info_t *sip_entry = NULL;
  qcmap_cm_sip_server_info_t default_sip_entry;
  ds_dll_el_t * node = NULL;
  /*variable for storing network sip server type*/
  qcmap_cm_sip_server_type_e  network_sip_server_type= QCMAP_CM_SIP_SERVER_MIN;

  QCMAP_CM_LOG_FUNC_ENTRY_AND_PROFILE_INFO(this->profile_handle);
  if (this->lan_cfg.enable_dhcpd)
  {
    //kill any old dnsmasq instances, then prep command
    memset(scratch_buf, 0, sizeof(scratch_buf));
    snprintf(scratch_buf, sizeof(scratch_buf), " kill -9 $(cat %s)", this->dnsmasq_pid_file_path);
    ds_system_call(scratch_buf, strlen(scratch_buf));

    memset(scratch_buf, 0, sizeof(scratch_buf));
    memset(dhcp_command, 0, sizeof(dhcp_command));
    memset(&default_sip_entry, 0, sizeof(default_sip_entry));
    strlcpy(dhcp_command,this->dhcp_command_str, MAX_DHCP_COMMAND_STR_LEN);

    /* if either sta backhaul connected or cradle backhaul connected ignore mtu wwanipv4*/
    if ( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      if( QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->GetWWANMTUInfo(&mtu_size_wwan_ipv4) == true )
      {
        /* add mtu wwan ipv4 as dhcp option 26 starts */
        if(mtu_size_wwan_ipv4 > 0 )
        {
          snprintf(scratch_buf, MAX_COMMAND_STR_LEN," --dhcp-option-force=%s,%d",
                   DHCP_OPTION_MTU, (int) mtu_size_wwan_ipv4);
          strlcat(dhcp_command, scratch_buf, MAX_DHCP_COMMAND_STR_LEN);
        }
        else
        {
          LOG_MSG_ERROR( "Couldn't set .mtu i.e not got it from wwan  %d\n",
                         mtu_size_wwan_ipv4,0,0 );
        }
        /* add mtu wwan ipv4 as dhcp option 26 ends */
      }
    }

    /*If default SIP Server configuration is not yet available get the
      configuraiton directly from XML*/
    if (QcMapBackhaulWWANMgr)
    {
      sip_server_info_list = &(QcMapBackhaulWWANMgr->sip_server_info_list);
      memcpy(&default_sip_entry,
             &(QcMapBackhaulWWANMgr->default_sip_server_info),
             sizeof(default_sip_entry));
    }
    else
    {
      QCMAP_Backhaul_WWAN::GetSetDefaultSIPServerConfigFromXML(GET_VALUE,
                                                               &default_sip_entry);
    }

    if (default_sip_entry.sip_serv_type != QCMAP_CM_SIP_SERVER_MIN ||
        (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->sip_server_list_count > 0))
    {
      strlcat(dhcp_command, " --dhcp-option-force=120", MAX_DHCP_COMMAND_STR_LEN);
    }

    if (QcMapBackhaulWWANMgr)
    {
      sip_server_info_list = &(QcMapBackhaulWWANMgr->sip_server_info_list);
      LOG_MSG_INFO1("Sip server count:%d network assigned PCSCF info",
                     QcMapBackhaulWWANMgr->sip_server_list_count, 0, 0);

      if (QcMapBackhaulWWANMgr->sip_server_list_count > 0)
      {

        node = sip_server_info_list->sipServerEntryListHead;
        node = ds_dll_next (node, (const void**)(&sip_entry));

        for (int i = 0; i < QcMapBackhaulWWANMgr->sip_server_list_count; i++)
        {
          strlcat(dhcp_command, ",", MAX_DHCP_COMMAND_STR_LEN);
          if (sip_entry->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
          {
            network_sip_server_type = QCMAP_CM_SIP_SERVER_ADDR;
            LOG_MSG_INFO1("Adding network assigned PCSCF address", 0, 0, 0);
            pcscf_addr.s_addr = sip_entry->sip_serv_info.sip_serv_ip;
            /* INET_NTOA_SIZE is the size of static buff used in inet_ntoa function */
            strlcat(dhcp_command, inet_ntoa(pcscf_addr), MAX_DHCP_COMMAND_STR_LEN);
          }
          else if (sip_entry->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
          {
            network_sip_server_type = QCMAP_CM_SIP_SERVER_FQDN;
            snprintf(scratch_buf, MAX_COMMAND_STR_LEN,"\"%s\"", sip_entry->sip_serv_info.sip_fqdn);
            strlcat(dhcp_command, scratch_buf, MAX_DHCP_COMMAND_STR_LEN);
          }
          node = ds_dll_next (node, (const void**)(&sip_entry));
        }
      }
    }

    /*adding default sip server info on basis of network sip server type*/
    if ((default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR) &&
        ((network_sip_server_type == QCMAP_CM_SIP_SERVER_ADDR) ||
        (network_sip_server_type == QCMAP_CM_SIP_SERVER_MIN )))
    {
      pcscf_addr.s_addr = default_sip_entry.sip_serv_info.sip_serv_ip;
      snprintf(scratch_buf, MAX_COMMAND_STR_LEN,",%s", inet_ntoa(pcscf_addr));
      strlcat(dhcp_command, scratch_buf, MAX_DHCP_COMMAND_STR_LEN);
    }
    else if ((default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN) &&
              ((network_sip_server_type == QCMAP_CM_SIP_SERVER_FQDN) ||
             (network_sip_server_type == QCMAP_CM_SIP_SERVER_MIN)))
    {
      snprintf(scratch_buf, MAX_COMMAND_STR_LEN,",\"%s\"", default_sip_entry.sip_serv_info.sip_fqdn);
      strlcat(dhcp_command, scratch_buf, MAX_DHCP_COMMAND_STR_LEN);
    }

    ds_system_call("chown nobody:nogroup /bin/dnsmasq_script.sh",
                   strlen("chown nobody:nogroup /bin/dnsmasq_script.sh"));


    strlcat (dhcp_command," --dhcp-script=/bin/dnsmasq_script.sh",
             MAX_DHCP_COMMAND_STR_LEN);

    if(this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
    {
      addr_pub.s_addr = this->lan_cfg.ip_passthrough_cfg.public_ip;
      snprintf(scratch_buf, MAX_COMMAND_STR_LEN," --dhcp-range=%s,%s,%s ",
               inet_ntoa(addr_pub),inet_ntoa(addr_pub), APPS_SUB_NET_MASK);
      strlcat(dhcp_command, scratch_buf , MAX_COMMAND_STR_LEN);
    }

    if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->dhcpv6_dns_conf.dhcpv6_enable_state ==
        QCMAP_MSGR_DHCPV6_MODE_UP_V01)
    {
      AddDHCPV6ProxyDNSOption(dhcp_command);

    }
    ds_system_call(dhcp_command, strlen(dhcp_command));
  }
  return true;
}


/*===========================================================================
  FUNCTION EnableDNS
==========================================================================*/
/*!
@brief
  Adds the IP addresses of nameservers available, received from network.

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
bool QCMAP_LAN::EnableDNS()
{
  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 pri_dns_addr = 0;
  struct in_addr addr;
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = GET_BACKHAUL_WWAN_OBJECT(this->profile_handle);

  if(QcMapBackhaulWWAN && QcMapBackhaulWWAN->GetIPv4NetworkConfig(&public_ip, &pri_dns_addr,
                                                                  &sec_dns_addr, &default_gw_addr,
                                                                  &qmi_err_num) != QCMAP_CM_SUCCESS)
  {
    LOG_MSG_ERROR("Error in GetIPv4NetworkConfig() %d\n", qmi_err_num,0,0);
  }

  if ( !pri_dns_addr && !sec_dns_addr )
  {
    LOG_MSG_INFO1("QCMAP DNS not enabled \n",0,0,0);
    return false;
  }

  if ( pri_dns_addr )
  {
    #ifndef FEATURE_QTIMAP_OFFTARGET
      addr.s_addr = pri_dns_addr;
      if((QcMapBackhaulWWAN && (QCMAP_CM_V6_WAN_CONNECTED == QcMapBackhaulWWAN->GetIPv6State())) &&
         (QCMAP_MSGR_DHCPV6_MODE_UP_V01 != QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_enable_state))
      {
        //append if IPv6 call is already up
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 'nameserver %s' >> %s", inet_ntoa(addr), this->dnsmasq_resolv_file_path);
      } else {
        //overwrite if IPv6 call is not up
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 'nameserver %s' > %s", inet_ntoa(addr), this->dnsmasq_resolv_file_path);
      }
      LOG_MSG_INFO1("QCMAP PRI DNS %s\n", inet_ntoa(addr),0,0);
      ds_system_call(command, strlen(command));

     //update resolve.conf to add ipv6 dns server
     if(QcMapBackhaulWWAN &&  QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_enable_state  ==
        QCMAP_MSGR_DHCPV6_MODE_UP_V01 )
     {
       QcMapBackhaulWWAN->Update_resolv_file(ADD_V6DNS_SERVER,
                                             QcMapBackhaulWWAN->dhcpv6_dns_conf.pri_dns_addr,
                                             QcMapBackhaulWWAN->dhcpv6_dns_conf.sec_dns_addr);
     }
    #endif
  }

  if ( sec_dns_addr )
  {
    #ifndef FEATURE_QTIMAP_OFFTARGET
      addr.s_addr = sec_dns_addr;
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 'nameserver %s' >> %s", inet_ntoa(addr), this->dnsmasq_resolv_file_path);
      LOG_MSG_INFO1("QCMAP SEC DNS %s\n", inet_ntoa(addr),0,0);
      ds_system_call(command, strlen(command));
    #endif
  }

  LOG_MSG_INFO1("QCMAP DNS Enabled \n",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION AddDNSNameServers
==========================================================================*/
/*!
@brief
  Adds the IP addresses of nameservers

@parameters
- char* primary dns addr
- char* secondary dns addr

@return
- None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_LAN::AddDNSNameServers(char* pri_dns_addr, char* sec_dns_addr)
{
  char command[MAX_COMMAND_STR_LEN] = {0};

  if(NULL == pri_dns_addr)
  {
    LOG_MSG_INFO1("primary dns addr given NULL!", 0, 0, 0);
    return;
  } else if(NULL == sec_dns_addr) {
    LOG_MSG_INFO1("secondary dns addr given NULL!", 0, 0, 0);
    return;
  }

  LOG_MSG_INFO1("LAN Object ID = %d, resolve_file = %s", this->bridge_vlan_id,
                this->dnsmasq_resolv_file_path, 0);

  if(strlen(pri_dns_addr))
  {
    #ifndef FEATURE_QTIMAP_OFFTARGET
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "echo 'nameserver %s' >> %s",
               pri_dns_addr, this->dnsmasq_resolv_file_path);
      LOG_MSG_INFO1("Added QCMAP PRI DNS %s", pri_dns_addr, 0, 0);
      ds_system_call(command, strlen(command));
    #endif
  }

  if(strlen(sec_dns_addr))
  {
    #ifndef FEATURE_QTIMAP_OFFTARGET
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "echo 'nameserver %s' >> %s",
               sec_dns_addr, this->dnsmasq_resolv_file_path);
      LOG_MSG_INFO1("Added QCMAP PRI DNS %s", sec_dns_addr, 0, 0);
      ds_system_call(command, strlen(command));
    #endif
  }

  return;
}

/*===========================================================================
  FUNCTION DeleteDNSNameServers
==========================================================================*/
/*!
@brief
  Deletes the IP addresses of nameservers

@parameters
- char* primary dns addr
- char* secondary dns addr

@return
- None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_LAN::DeleteDNSNameServers(char* pri_dns_addr, char* sec_dns_addr)
{
  FILE* fp = NULL;
  char buffer[strlen("nameserver ") + INET6_ADDRSTRLEN + 1] = {0};
  std::string temp, filtered_str;

  if(NULL == pri_dns_addr)
  {
    LOG_MSG_INFO1("primary dns addr given NULL!", 0, 0, 0);
    return;
  } else if(NULL == sec_dns_addr) {
    LOG_MSG_INFO1("secondary dns addr given NULL!", 0, 0, 0);
    return;
  }

  #ifndef FEATURE_QTIMAP_OFFTARGET
    //first filter out the IPv6 DNS addrs into filtered_str string
    if((fp = fopen(this->dnsmasq_resolv_file_path, "r")) == NULL)
    {
      LOG_MSG_INFO1("error opening %s: %s", this->dnsmasq_resolv_file_path, strerror(errno), 0);
      return;
    }

    filtered_str.clear();
    memset(buffer, 0, sizeof(buffer));
    while(fgets(buffer, strlen("nameserver ") + INET6_ADDRSTRLEN + 1, fp) != NULL)
    {
      temp.clear();
      temp = buffer;

      if(!((strlen(pri_dns_addr) &&
           (std::string::npos != temp.find(pri_dns_addr))) ||
           (strlen(sec_dns_addr) &&
           (std::string::npos != temp.find(sec_dns_addr)))))
      {
        //save this line
        filtered_str.append(temp);
      }
      memset(buffer, 0, sizeof(buffer));
    }

    if(fclose(fp))
    {
      LOG_MSG_INFO1("error closing %s: %s", this->dnsmasq_resolv_file_path, strerror(errno), 0);
    }
    fp = NULL;

    //write the filtered lines
    if((fp = fopen(this->dnsmasq_resolv_file_path, "w")) == NULL)
    {
      LOG_MSG_INFO1("error opening %s: %s", this->dnsmasq_resolv_file_path, strerror(errno), 0);
      return;
    }

    if(fputs(filtered_str.c_str(), fp) < 0)
    {
      LOG_MSG_INFO1("error fputs %s: %s", this->dnsmasq_resolv_file_path, strerror(errno), 0);
      goto end;
    }

    end:
    if(fclose(fp))
    {
      LOG_MSG_INFO1("error closing %s: %s", this->dnsmasq_resolv_file_path, strerror(errno), 0);
    }
  #endif

  return;
}

/*=====================================================================
  FUNCTION IsDNSEnabled
======================================================================*/
/*!
@brief
  Get this LAN object's dns_enable flag

@parameters

@return
  true - enabled
  false - disabled

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
bool QCMAP_LAN::IsDNSEnabled(void) const
{
  if(this->enable_dns)
  {
    LOG_MSG_INFO1("Bridge object %d has DNS enabled", this->bridge_vlan_id, 0, 0);
  } else {
    LOG_MSG_INFO1("Bridge object %d has DNS disabled", this->bridge_vlan_id, 0, 0);
  }
  return this->enable_dns;
}

/*=====================================================================
  FUNCTION SetDNSEnableFlag
======================================================================*/
/*!
@brief
  Set this LAN object's dns_enable flag

@parameters
  true - enabled
  false - disabled


@return

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_LAN::SetDNSEnableFlag(bool flag)
{
  this->enable_dns = flag;

  if(this->enable_dns)
  {
    LOG_MSG_INFO1("Bridge object %d has DNS enabled", this->bridge_vlan_id, 0, 0);
  } else {
    LOG_MSG_INFO1("Bridge object %d has DNS disabled", this->bridge_vlan_id, 0, 0);
  }

  return;
}

/*===========================================================================
FUNCTION CheckforAddrConflict
==========================================================================*/
/*!
@brief
This function will check if ap, guest ap and sta are in different subnets.
To avoid address conflicts.

@return
true - on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::CheckforAddrConflict
(
  qcmap_msgr_lan_config_v01 *lan_config,
  qcmap_msgr_station_mode_config_v01 *station_config
) const
{
  qcmap_msgr_station_mode_config_v01 station_cfg;

  /* Check for LAN Configuration for address conflicts */
  if (lan_config != NULL)
  {
    /* Check with sta configuratin for address conflicts */
    if (station_config !=NULL)
    {
      if ((station_config->conn_type == QCMAP_MSGR_STA_CONNECTION_STATIC_V01) &&
          ((lan_config->gw_ip & lan_config->netmask) ==
           (station_config->static_ip_config.gw_ip & station_config->static_ip_config.netmask)))
      {
        return true;
      }
    }
    /* Check with default sta configuratin for address conflicts */
    QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,&station_cfg);

    if (station_cfg.conn_type == QCMAP_MSGR_STA_CONNECTION_STATIC_V01 )
    {
      if ((lan_config->gw_ip & lan_config->netmask) ==
          (station_cfg.static_ip_config.gw_ip
           & station_cfg.static_ip_config.netmask))//read from xml check full api
      {
        LOG_MSG_INFO1("\n LAN configured address overlapped with STA IP \n",0,0,0);
        return true;
      }
    }
  }

  /* Check for sta Configuratin for address conflicts with AP and Guest AP mode*/
  if (station_config != NULL && (station_config->conn_type ==
      QCMAP_MSGR_STA_CONNECTION_STATIC_V01))
  {
    if ((station_config->static_ip_config.gw_ip
         & station_config->static_ip_config.netmask) ==
        (this->lan_cfg.apps_ip_addr &
         this->lan_cfg.sub_net_mask ))
    {
      LOG_MSG_INFO1("\n STA configured address overlapped with LAN IP\n",0,0,0);
      return true;
    }
  }
  return false;
}


/*===========================================================================
  FUNCTION sync_dhcp_hosts
==========================================================================*/
/*!
@brief
  Sync /data/dhcp-hosts with the current DHCP reservation records
  on boot or when SetLANConfig API is called

@return
  bool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void QCMAP_LAN::sync_dhcp_hosts(void)
{
  in_addr addr;

  qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_record = NULL;
  bool mac_addr_non_empty = true;
  uint32_t client_ip;
  uint32 num_records = 0;
  qcmap_dhcp_information_info_list_t* dhcpList = &(this->lan_cfg.dhcp_reservation_records);
  ds_dll_el_t * node = NULL, *temp_node = NULL;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/

  /*-------------------------------------------------------------------------*/


  num_records = this->lan_cfg.num_dhcp_reservation_records;
  this->lan_cfg.num_dhcp_reservation_records = 0;
  node = dhcpList->dhcpInfoEntryListHead;

  node = ds_dll_next (node, (const void**)(&dhcp_reserv_record));

  while(node)
  {
    client_ip = dhcp_reserv_record->client_reserved_ip;
    addr.s_addr = htonl(client_ip);
    if ( ( this->lan_cfg.dhcp_start_address <= client_ip ) &&
         ( (this->lan_cfg.dhcp_end_address - 1)   >= client_ip ) )
    {
      this->lan_cfg.num_dhcp_reservation_records++;

      /*sync /data/dhcp-hosts file*/
      if ( dhcp_reserv_record->enable_reservation )
      {
        if ( dhcp_reserv_record->client_device_name[0] != '\0' )
        {
          fprintf(dhcp_hosts_fp,"%s,",dhcp_reserv_record->client_device_name);
        }
        /* mac address is not provided for USB client*/
        mac_addr_non_empty = check_non_empty_mac_addr(dhcp_reserv_record->client_mac_addr,
                                                      mac_addr_string);
        if ( mac_addr_non_empty )
        {
          fprintf(dhcp_hosts_fp,"%s,",mac_addr_string);
        }
        fprintf(dhcp_hosts_fp,"%s\n",inet_ntoa(addr));
      }
      node = ds_dll_next (node, (const void**)(&dhcp_reserv_record));
    }
    else
    {
      temp_node = node->next;
      ds_dll_delete_node(dhcpList->dhcpInfoEntryListHead,
                         &(dhcpList->dhcpInfoEntryListTail), node);

      //Free the device information structure
      ds_free (dhcp_reserv_record);
      dhcp_reserv_record = NULL;

      //Free the memory of the linked list node
      ds_dll_free(node);

      node = temp_node;
      dhcp_reserv_record = ds_dll_data(node);
    }
  }
}

/*===========================================================================
  FUNCTION ReadLANConfigFromXML
==========================================================================*/
/*!
@brief
  Reads the LAN config from XML

@return
  bool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
bool QCMAP_LAN::ReadLANConfigFromXML(void)
{
  pugi::xml_document xml_file;
  pugi::xml_node parent, root, child, subchild;
  struct in_addr ip_addr;
  uint32_t value_debug=0;
  char str[INET6_ADDRSTRLEN];
  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_msgr_dhcp_reservation_v01 dhcp_reserv_record;
  uint8 mac[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  char content_buf[MAX_STRING_LENGTH];
  bool vlan_id_match = false;
  QCMAP_Backhaul_WWAN* default_wwan_obj = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  memset(&dhcp_reserv_record,0,sizeof(qcmap_msgr_dhcp_reservation_v01));

  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  switch(this->bridge_vlan_id)
  {
    case(DEFAULT_BRIDGE_ID): //for default LAN object bridge0
    {
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

      this->lan_cfg.enable_dhcpd = atoi(root.child(EnableDHCPServer_TAG).child_value());

      if (inet_aton(root.child(APIPAddr_TAG).child_value(), &ip_addr))
        this->lan_cfg.apps_ip_addr = ntohl(ip_addr.s_addr);

      if (inet_aton(root.child(SubNetMask_TAG).child_value(), &ip_addr))
         this->lan_cfg.sub_net_mask = ntohl(ip_addr.s_addr);

      strlcpy(this->lan_cfg.gateway_url, root.child(GatewayURL_TAG).child_value(),
              QCMAP_MSGR_MAX_GATEWAY_URL_V01);


      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG).
             child(DHCPCfg_TAG);

      if (inet_aton(root.child(DHCPStartIP_TAG).child_value(), &ip_addr))
        this->lan_cfg.dhcp_start_address = ntohl(ip_addr.s_addr);

      if (inet_aton(root.child(DHCPEndIP_TAG).child_value(), &ip_addr))
        this->lan_cfg.dhcp_end_address = ntohl(ip_addr.s_addr);

      this->lan_cfg.dhcp_lease_time = atoi(root.child(DHCPLeaseTime_TAG).child_value());

      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
      for (child = root.first_child(); child; child = child.next_sibling())
      {
        if (!strcmp(child.name(), DHCPReservationRecord_TAG))
        {
          /*memset to NULL*/
          memset(&dhcp_reserv_record,0,sizeof(qcmap_msgr_dhcp_reservation_v01));
          if(child.child(ClientMACAddr_TAG))
          {
            strlcpy(content_buf,child.child(ClientMACAddr_TAG).child_value(),MAX_STRING_LENGTH);
            for (int i = 0; i < QCMAP_MSGR_MAC_ADDR_LEN_V01; i++)
            {
              mac[i] = (ds_hex_to_dec(content_buf[i * 3]) << 4) | ds_hex_to_dec(content_buf[i * 3
                                                                                            + 1]);
            }
            LOG_MSG_INFO1("content_buf: %s",content_buf,0,0);
            strlcpy(dhcp_reserv_record.client_mac_addr,mac,QCMAP_MSGR_MAC_ADDR_LEN_V01+1);
            inet_aton( child.child(ClientReservedIP_TAG).child_value(),
                      (in_addr *)&(dhcp_reserv_record.client_reserved_ip) );
            dhcp_reserv_record.client_reserved_ip = ntohl(dhcp_reserv_record.client_reserved_ip);

            dhcp_reserv_record.enable_reservation =
                                   atoi(child.child(Reservation_TAG).child_value());
          }


           else if(child.child(ClientName_TAG))
           {
             LOG_MSG_INFO1("here: ",0,0,0);
             strlcpy( dhcp_reserv_record.client_device_name,
                      child.child(ClientName_TAG).child_value(),
                      QCMAP_MSGR_DEVICE_NAME_MAX_V01+1 );

             inet_aton(child.child(ClientReservedIP_TAG).child_value(),
                      (in_addr *)&(dhcp_reserv_record.client_reserved_ip));

             dhcp_reserv_record.client_reserved_ip =
                                       ntohl(dhcp_reserv_record.client_reserved_ip);

             dhcp_reserv_record.enable_reservation =
                                atoi(child.child(Reservation_TAG).child_value());
           }

           if(this->AddDHCPResRecEntryToList( &dhcp_reserv_record, dhcp_record) == false)
          {
            LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
            return false;
          }
        }
      }

      value_debug= htonl(this->lan_cfg.dhcp_start_address);
      readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
      LOG_MSG_INFO1("\nDHCP Start Address : '%s'",str,0,0);

      value_debug= htonl(this->lan_cfg.dhcp_end_address);
      readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
      LOG_MSG_INFO1("\nDHCP End Address : '%s'",str,0,0);


      LOG_MSG_INFO1("\nDHCP Lease Time: '%d'",this->lan_cfg.dhcp_lease_time,0,0);

      value_debug= htonl(this->lan_cfg.apps_ip_addr);
      readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
      LOG_MSG_INFO1("\nDHCP API Address : '%s'",str,0,0);

      value_debug= htonl(this->lan_cfg.sub_net_mask);
      readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
      LOG_MSG_INFO1("\nDHCP Subnet Mask : '%s'",str,0,0);

      LOG_MSG_INFO1("Enable DHCP: %b , ",this->lan_cfg.enable_dhcpd,0,0);

      //parse the profile handle
      if((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG).
                 child(ProfileHandle_TAG)) != NULL)
      {
        this->profile_handle = atoi(root.text().get());
      }

      LOG_MSG_INFO1("Bridge%d has profile handle %d", this->bridge_vlan_id,
                    this->profile_handle, 0);
      break;
    }

    default: //non default bridge
    {
      LOG_MSG_INFO1("Attempt to parse for non default bridge: %d", this->bridge_vlan_id, 0, 0);

      /* Concurrent Tag */
      if((root = xml_file.child(System_TAG).child(ConcurrentMobileAPCfg_TAG)) == NULL)
      {
        LOG_MSG_INFO1("Concurrent MobileAP Cfg Tag does not exist", 0, 0, 0);
        return false;
      }

      vlan_id_match = false;
      //see if existing Config existed
      for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
      {
        if(vlan_id_match)
        {
          break;
        }

        for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
            grand_child = grand_child.next_sibling())
        {
          //check if BridgeCfg
          if(strcmp(grand_child.name(), BridgeCfg_TAG) == 0)
          {
            for(pugi::xml_node great_grand_child = grand_child.first_child();
                great_grand_child != NULL;
                great_grand_child = great_grand_child.next_sibling())
            {
              //check VLAN ID
              if(!vlan_id_match && (strcmp(great_grand_child.name(), VLAN_ID_TAG) == 0))
              {
                if(atoi((char*)great_grand_child.text().get()) == this->bridge_vlan_id)
                {
                  LOG_MSG_INFO1("pre-existing config bridge%d found", this->bridge_vlan_id, 0, 0);
                  vlan_id_match = true;
                  //reset this loop, update all other config
                  great_grand_child = grand_child.first_child();
                }
              }

              //APIP addr tag
              if(vlan_id_match && (grand_child.child(APIPAddr_TAG) != NULL))
              {
                memset(&ip_addr, 0, sizeof(ip_addr));
                if(!inet_aton(grand_child.child(APIPAddr_TAG).text().get(), &ip_addr))
                {
                  LOG_MSG_INFO1("Error with inet_aton: %s", strerror(errno), 0, 0);
                } else {
                  this->lan_cfg.apps_ip_addr = ntohl(ip_addr.s_addr);
                }
              }

              //Subnet Mask tag
              if(vlan_id_match && (grand_child.child(SubNetMask_TAG) != NULL))
              {
                memset(&ip_addr, 0, sizeof(ip_addr));
                if(!inet_aton(grand_child.child(SubNetMask_TAG).text().get(), &ip_addr))
                {
                  LOG_MSG_INFO1("Error with inet_aton: %s", strerror(errno), 0, 0);
                } else {
                  this->lan_cfg.sub_net_mask = ntohl(ip_addr.s_addr);
                }
              }

              //Enable DHCP Server tag
              if(vlan_id_match && (grand_child.child(EnableDHCPServer_TAG) != NULL))
              {
                this->lan_cfg.enable_dhcpd =
                                         atoi(grand_child.child(EnableDHCPServer_TAG).text().get());
              }

              //DHCPCfg tag
              if(vlan_id_match && (grand_child.child(DHCPCfg_TAG) != NULL))
              {
                //DHCPStartIP tag
                if(grand_child.child(DHCPCfg_TAG).child(DHCPStartIP_TAG) != NULL)
                {
                  memset(&ip_addr, 0, sizeof(ip_addr));
                  if(!inet_aton(grand_child.child(DHCPCfg_TAG).child(DHCPStartIP_TAG).text().get(),
                               &ip_addr))
                  {
                    LOG_MSG_INFO1("Error with inet_aton: %s", strerror(errno), 0, 0);
                  } else {
                    this->lan_cfg.dhcp_start_address = ntohl(ip_addr.s_addr);
                  }
                }

                //DHCPEndIP tag
                if(grand_child.child(DHCPCfg_TAG).child(DHCPEndIP_TAG) != NULL)
                {
                  memset(&ip_addr, 0, sizeof(ip_addr));
                  if(!inet_aton(grand_child.child(DHCPCfg_TAG).child(DHCPEndIP_TAG).text().get(),
                               &ip_addr))
                  {
                    LOG_MSG_INFO1("Error with inet_aton: %s", strerror(errno), 0, 0);
                  } else {
                    this->lan_cfg.dhcp_end_address = ntohl(ip_addr.s_addr);
                  }
                }

                //DHCPLeaseTime tag
                if(grand_child.child(DHCPCfg_TAG).child(DHCPLeaseTime_TAG) != NULL)
                {
                  this->lan_cfg.dhcp_lease_time =
                         atoi(grand_child.child(DHCPCfg_TAG).child(DHCPLeaseTime_TAG).text().get());
                }
              }

              //try to find profile handle if existing
              if(vlan_id_match && (child.child(MobileAPWanCfg_TAG).child(ProfileHandle_TAG) !=
                                   NULL))
              {
                this->profile_handle =
                        atoi(child.child(MobileAPWanCfg_TAG).child(ProfileHandle_TAG).text().get());
              }
            }
          }
        }
      }

      if(vlan_id_match)
      {
        //check if profile handle is for default
        root = xml_file.child(System_TAG).child(MobileAPCfg_TAG);
        if(root.child(VLAN_ID_TAG) != NULL)
        {
          if(atoi(root.child(VLAN_ID_TAG).text().get()) == this->bridge_vlan_id)
          {
            if(!default_wwan_obj)
            {
              LOG_MSG_INFO1("Default WWAN obj NULL", 0, 0, 0)  ;
            } else {
              this->profile_handle = default_wwan_obj->GetProfileHandle();
            }
          }
        }

        if(this->profile_handle)
        {
          LOG_MSG_INFO1("Bridge%d pre-existing config: profile_handle: %d",
                        this->bridge_vlan_id, this->profile_handle, 0);
        }

        LOG_MSG_INFO1("Bridge%d pre-existing config: GW IP Addr 0x%x",
                      this->bridge_vlan_id, this->lan_cfg.apps_ip_addr, 0);
        LOG_MSG_INFO1("Bridge%d pre-existing config: Subnet 0x%x",
                      this->bridge_vlan_id, this->lan_cfg.sub_net_mask, 0);
        LOG_MSG_INFO1("Bridge%d pre-existing config: EnableDHCPServer: %d",
                      this->bridge_vlan_id, this->lan_cfg.enable_dhcpd, 0);

        if(this->lan_cfg.enable_dhcpd)
        {
          LOG_MSG_INFO1("Bridge%d pre-existing config: DHCPStartAddr 0x%x",
                        this->bridge_vlan_id, this->lan_cfg.dhcp_start_address, 0);
          LOG_MSG_INFO1("Bridge%d pre-existing config: DHCPEndAddr 0x%x",
                        this->bridge_vlan_id, this->lan_cfg.dhcp_end_address, 0);
          LOG_MSG_INFO1("Bridge%d pre-existing config: DHCPLeaseTime %d(seconds)",
                        this->bridge_vlan_id, this->lan_cfg.dhcp_lease_time, 0);
        }
      } else {
        LOG_MSG_INFO1("Bridge%d pre-existing config does not exist", this->bridge_vlan_id, 0, 0);
        return false;
      }
      break;
    }
  }

  return true;
}
/*===========================================================================
  FUNCTION AddDHCPResRecEntryToList
==========================================================================*/
/*!
@brief
  Adds DHCP entry to List

@parameters
qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
  qcmap_msgr_dhcp_reservation_v01* dhcp_record

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

bool QCMAP_LAN::AddDHCPResRecEntryToList
(
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
  qcmap_msgr_dhcp_reservation_v01* dhcp_record
)
{

  LOG_MSG_INFO1("Entering AddDHCPResRecEntryToList",0,0,0);
  ds_dll_el_t * node = NULL;

  qcmap_dhcp_information_info_list_t* dhcpList = &(this->lan_cfg.dhcp_reservation_records);

  if (dhcpList->dhcpInfoEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
          information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for node",
                    0,0,0);
      return false;
    }
    dhcpList->dhcpInfoEntryListHead = node;
  }

  dhcp_record = (qcmap_msgr_dhcp_reservation_v01*)ds_malloc(sizeof(qcmap_msgr_dhcp_reservation_v01));
  if( dhcp_record == NULL )
  {
    LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for"
                 "dhcp reservation entry",0,0,0);
    return false;
  }


  memset(dhcp_record, 0, sizeof(qcmap_msgr_dhcp_reservation_v01));

  memcpy(dhcp_record, dhcp_reserv_record,
         sizeof(qcmap_msgr_dhcp_reservation_v01));

  //Store the firewall entry in the linked list
  if ((node = ds_dll_enq(dhcpList->dhcpInfoEntryListHead,
                         NULL, (void*)dhcp_record)) == NULL)
  {
    LOG_MSG_ERROR("AddDHCPReservRecord - Error in adding a node",0,0,0);
    ds_free(dhcp_record);
    return false;
  }
  dhcpList->dhcpInfoEntryListTail = node;
  this->lan_cfg.num_dhcp_reservation_records++;

  return true;

}

/*===========================================================================
  FUNCTION SetLANConfigToXML
==========================================================================*/
/*!
@brief
  Get /set LAN config from XML

@return
  bool

@note

@Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
bool QCMAP_LAN::SetLANConfigToXML(qcmap_msgr_lan_config_v01 *lan_config)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child ;
  struct in_addr addr;
  uint32_t value_debug=0;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  snprintf(data, sizeof(data), "%d", lan_config->enable_dhcp);
  root.child(EnableDHCPServer_TAG).text() = data;

  addr.s_addr = htonl(lan_config->gw_ip);
  root.child(APIPAddr_TAG).text() = inet_ntoa(addr);

  addr.s_addr = htonl(lan_config->netmask);
  root.child(SubNetMask_TAG).text() = inet_ntoa(addr);

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).
         child(MobileAPLanCfg_TAG).child(DHCPCfg_TAG);

  addr.s_addr = htonl(lan_config->dhcp_config.dhcp_start_ip);
  root.child(DHCPStartIP_TAG).text() = inet_ntoa(addr);

  addr.s_addr = htonl(lan_config->dhcp_config.dhcp_end_ip);
  root.child(DHCPEndIP_TAG).text() = inet_ntoa(addr);

  snprintf(data, sizeof(data), "%d", lan_config->dhcp_config.lease_time);
  root.child(DHCPLeaseTime_TAG).text() = data;

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  value_debug= htonl(lan_config->dhcp_config.dhcp_start_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Start Address : '%s'",str,0,0);

  value_debug= htonl(lan_config->dhcp_config.dhcp_end_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP End Address : '%s'",str,0,0);

  LOG_MSG_INFO1("\nDHCP Lease Time: '%d'",lan_config->dhcp_config.lease_time,0,0);

  value_debug= htonl(lan_config->gw_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP API Address : '%s'",str,0,0);

  value_debug= htonl(lan_config->netmask);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Subnet Mask : '%s'",str,0,0);

  LOG_MSG_INFO1("Enable DHCP: %d",lan_config->enable_dhcp,0,0);

  return TRUE;
}


/*===========================================================================
  FUNCTION SetLANConfigToXML
==========================================================================*/
/*!
@brief
  Set LAN config to xml for this QCMAP_LAN obj

@return
  bool

@note

@Dependencies
  - None

  - Side Effects
    - None
*/
/*=========================================================================*/
bool QCMAP_LAN::SetLANConfigToXML(void)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, bridge_node, temp_node;
  struct in_addr ip_addr;
  char data[MAX_STRING_LENGTH] = {0};
  bool vlan_id_match, profile_handle_match;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if(!QcMapMgr)
  {
    LOG_MSG_INFO1("QcMapMgr is NULL", 0, 0, 0);
    return false;
  }

  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  /* Concurrent Tag */
  if((root = xml_file.child(System_TAG).child(ConcurrentMobileAPCfg_TAG)) == NULL)
  {
    root = xml_file.child(System_TAG).append_child(ConcurrentMobileAPCfg_TAG);
  }

  //see if existing Config existed
  for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
  {
    vlan_id_match = false;
    profile_handle_match = false;

    for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
        grand_child = grand_child.next_sibling())
    {
      //check if MobileAPWANCfg
      if(!profile_handle_match && (strcmp(grand_child.name(), MobileAPWanCfg_TAG) == 0))
      {
        if((grand_child.child(ProfileHandle_TAG) != NULL) &&
           (atoi((char*)grand_child.child(ProfileHandle_TAG).text().get()) == this->profile_handle))
        {
          LOG_MSG_INFO1("Will update bridge config for profile %d", this->profile_handle, 0, 0);
          profile_handle_match = true;
        }
      }

      //check if BridgeCfg
      if(strcmp(grand_child.name(), BridgeCfg_TAG) == 0)
      {
        for(pugi::xml_node great_grand_child = grand_child.first_child(); great_grand_child != NULL;
            great_grand_child = great_grand_child.next_sibling())
        {
          //check VLAN ID
          if(!vlan_id_match && (strcmp(great_grand_child.name(), VLAN_ID_TAG) == 0))
          {
            if(atoi((char*)great_grand_child.text().get()) == this->bridge_vlan_id)
            {
              LOG_MSG_INFO1("Will update config for bridge%d", this->bridge_vlan_id, 0, 0);
              vlan_id_match = true;
              //reset this loop, update all other config
              great_grand_child = grand_child.first_child();
            }
          }

          //APIP addr tag
          if(vlan_id_match && (grand_child.child(APIPAddr_TAG) != NULL))
          {
            memset(&ip_addr, 0, sizeof(ip_addr));
            ip_addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
            grand_child.child(APIPAddr_TAG).text().set(inet_ntoa(ip_addr));
          }

          //Subnet Mask tag
          if(vlan_id_match && (grand_child.child(SubNetMask_TAG) != NULL))
          {
            memset(&ip_addr, 0, sizeof(ip_addr));
            ip_addr.s_addr = htonl(this->lan_cfg.sub_net_mask);
            grand_child.child(SubNetMask_TAG).text().set(inet_ntoa(ip_addr));
          }

          //Enable DHCP Server tag
          if(vlan_id_match && (grand_child.child(EnableDHCPServer_TAG) != NULL))
          {
            grand_child.child(EnableDHCPServer_TAG).text().set(this->lan_cfg.enable_dhcpd);
          }

          //DHCPCfg tag
          if(vlan_id_match && (grand_child.child(DHCPCfg_TAG) != NULL))
          {
            //DHCPStartIP tag
            if(grand_child.child(DHCPCfg_TAG).child(DHCPStartIP_TAG) != NULL)
            {
              memset(&ip_addr, 0, sizeof(ip_addr));
              ip_addr.s_addr = htonl(this->lan_cfg.dhcp_start_address);
              grand_child.child(DHCPCfg_TAG).child(DHCPStartIP_TAG).text().set(inet_ntoa(ip_addr));
            }

            //DHCPEndIP tag
            if(grand_child.child(DHCPCfg_TAG).child(DHCPEndIP_TAG) != NULL)
            {
              memset(&ip_addr, 0, sizeof(ip_addr));
              ip_addr.s_addr = htonl(this->lan_cfg.dhcp_end_address);
              grand_child.child(DHCPCfg_TAG).child(DHCPEndIP_TAG).text().set(inet_ntoa(ip_addr));
            }

            //DHCPLeaseTime tag
            if(grand_child.child(DHCPCfg_TAG).child(DHCPLeaseTime_TAG) != NULL)
            {
              grand_child.child(DHCPCfg_TAG).child(DHCPLeaseTime_TAG).text().set(
                                                                     this->lan_cfg.dhcp_lease_time);
            }
          }

          //save updates
          if(vlan_id_match)
          {
            LOG_MSG_INFO1("Config set for bridge%d", this->bridge_vlan_id, 0, 0);
            QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

            return true;
          }
        }
      }
    }
    /* Append a new bridge config */
    if(profile_handle_match && !vlan_id_match)
    {
      LOG_MSG_INFO1("Configuration for bridge%d not existing, will append for profile %d...",
                    this->bridge_vlan_id, this->profile_handle, 0);
      //make a new bridge config for this profile
      bridge_node = child.append_child(BridgeCfg_TAG);
      goto append_bridge_cfg;
    }
  }

  /* Append new config */
  LOG_MSG_INFO1("Configuration for bridge%d and profile %d previously not existing, will append "
                "new config...", this->bridge_vlan_id, this->profile_handle, 0);
  bridge_node = root.append_child(Cfg_TAG).append_child(BridgeCfg_TAG);

  append_bridge_cfg:

  //VLAN ID tag
  memset(data, 0, sizeof(data));
  temp_node = bridge_node.append_child(VLAN_ID_TAG);
  snprintf(data, sizeof(data), "%d", this->bridge_vlan_id);
  temp_node.append_child(pugi::node_pcdata).set_value(data);

  //APIP addr tag
  memset(&ip_addr, 0, sizeof(ip_addr));
  ip_addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
  memset(data, 0, sizeof(data));
  temp_node = bridge_node.append_child(APIPAddr_TAG);
  snprintf(data, sizeof(data), "%s", inet_ntoa(ip_addr));
  temp_node.append_child(pugi::node_pcdata).set_value(data);

  //Subnet Mask tag
  memset(&ip_addr, 0, sizeof(ip_addr));
  ip_addr.s_addr = htonl(this->lan_cfg.sub_net_mask);
  memset(data, 0, sizeof(data));
  temp_node = bridge_node.append_child(SubNetMask_TAG);
  snprintf(data, sizeof(data), "%s", inet_ntoa(ip_addr));
  temp_node.append_child(pugi::node_pcdata).set_value(data);

  //Enable DHCP Server tag
  memset(data, 0, sizeof(data));
  temp_node = bridge_node.append_child(EnableDHCPServer_TAG);
  snprintf(data, sizeof(data), "%d", this->lan_cfg.enable_dhcpd);
  temp_node.append_child(pugi::node_pcdata).set_value(data);

  //DHCPCfg tag
  if((root = bridge_node.child(DHCPCfg_TAG)) == NULL)
  {
    root = bridge_node.append_child(DHCPCfg_TAG);
  }

  //DHCPStartIP tag
  memset(&ip_addr, 0, sizeof(ip_addr));
  ip_addr.s_addr = htonl(this->lan_cfg.dhcp_start_address);
  memset(data, 0, sizeof(data));
  temp_node = root.append_child(DHCPStartIP_TAG);
  snprintf(data, sizeof(data), "%s", inet_ntoa(ip_addr));
  temp_node.append_child(pugi::node_pcdata).set_value(data);

  //DHCPEndIP tag
  memset(&ip_addr, 0, sizeof(ip_addr));
  ip_addr.s_addr = htonl(this->lan_cfg.dhcp_end_address);
  memset(data, 0, sizeof(data));
  temp_node = root.append_child(DHCPEndIP_TAG);
  snprintf(data, sizeof(data), "%s", inet_ntoa(ip_addr));
  temp_node.append_child(pugi::node_pcdata).set_value(data);

  //DHCPLeaseTime tag
  memset(data, 0, sizeof(data));
  temp_node = root.append_child(DHCPLeaseTime_TAG);
  snprintf(data, sizeof(data), "%d", this->lan_cfg.dhcp_lease_time);
  temp_node.append_child(pugi::node_pcdata).set_value(data);

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  return true;
}

/*===========================================================================
  FUNCTION DeleteLANConfigFromXML
==========================================================================*/
/*!
@brief
  Delete LAN config from xml for this QCMAP_LAN obj

@return
  bool

@note

@Dependencies
  - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void QCMAP_LAN::DeleteLANConfigFromXML(bool updateIPA)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  bool removed_bridge_cfg;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  LOG_MSG_INFO1("%s for bridge: %d", __func__, this->bridge_vlan_id, 0);

  if(!QcMapMgr)
  {
    LOG_MSG_INFO1("QcMapMgr is NULL", 0, 0, 0);
    return;
  }

  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return;
  }

  if((root = xml_file.child(System_TAG).child(ConcurrentMobileAPCfg_TAG)) == NULL)
  {
    LOG_MSG_INFO1("Couldn't find concurrent mobileap tag", 0, 0, 0);
    return;
  }

  //find the config for this LAN object
  for(pugi::xml_node child = root.first_child(); child != NULL; child = child.next_sibling())
  {
    removed_bridge_cfg = false;
    for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
        grand_child = grand_child.next_sibling())
    {
      if(strcmp(grand_child.name(), BridgeCfg_TAG) == 0)
      {
        for(pugi::xml_node great_grand_child = grand_child.first_child(); great_grand_child != NULL;
            great_grand_child = great_grand_child.next_sibling())
        {
          if(strcmp(great_grand_child.name(), VLAN_ID_TAG) == 0)
          {
            if(atoi((char*)great_grand_child.text().get()) == this->bridge_vlan_id)
            {
              LOG_MSG_INFO1("%s found the config for this LAN object: %d", __func__,
                            this->bridge_vlan_id, 0);
                grand_child.parent().remove_child(grand_child);
                removed_bridge_cfg = true;;
            }
          }
        }
      }

      if(removed_bridge_cfg && (child.child(MobileAPWanCfg_TAG) == NULL) &&
         (child.child(MobileAPNatCfg_TAG) == NULL))
      {
        child.parent().remove_child(child);
      }
    }
  }

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  if (updateIPA == true)
  {
    /* Update IPACM_cfg.xml file */
    if (!UpdateIPAWithVLANIOCTL(false))
    {
      LOG_MSG_ERROR("IPA to VLAN IOCTL failed!!!",0,0,0);
    }
  }

  LOG_MSG_INFO1("%s returning", __func__, 0, 0);

  return;
}

/*===========================================================================
  FUNCTION SetGatewayConfigToXML
==========================================================================*/
/*!
@brief
  Sets the Gateway to the XML

@parameters
  char *url

@return
  true  - Get URL success
  flase - Get URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::SetGatewayConfigToXML(const char *url) const
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  root.child(GatewayURL_TAG).text() = url;

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  return TRUE;
}


/*===========================================================================
  FUNCTION GetGatewayUrl
==========================================================================*/
/*!
@brief
  Retreives the Gateway url.

@parameters
  char *url

@return
  true  - Get URL success
  flase - Get URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::GetGatewayUrl
(
  char *url,
  uint32_t *url_len,
  qmi_error_type_v01 *qmi_err_num
) const
{

  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT(); /* Will be always set on default bridge */

  if( url != NULL && url_len != NULL )
  {
    memcpy(url, QCMAPLANMgr->lan_cfg.gateway_url, QCMAP_MSGR_MAX_GATEWAY_URL_V01);
    *url_len = strlen(url);
  }
  else
  {
    LOG_MSG_ERROR("Invalid argument\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  return true;
}

/*===========================================================================
  FUNCTION EnableGatewayUrl
==========================================================================*/
/*!
@brief
  Enables the Gateway url.

@parameters
  None

@return
  true  - Get URL success
  false - Get URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

bool QCMAP_LAN::EnableGatewayUrl(void) const
{
  char command[MAX_COMMAND_STR_LEN];
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  struct sockaddr_in6 pub_ip6;
  char addr_pub_str[INET6_ADDRSTRLEN] = {0};
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  QCMAP_LAN* QCMAPLANMgr = GET_DEFAULT_LAN_OBJECT();

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "echo 127.0.0.1 localhost.localdomain localhost > %s",this->host_file_path);
  ds_system_call(command, strlen(command));

  addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
  strlcpy(a5_ip, inet_ntoa(addr),QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  if ((strlen(this->lan_cfg.gateway_url) == 0) && (QCMAPLANMgr != NULL))
  {
     memcpy(this->lan_cfg.gateway_url, QCMAPLANMgr->lan_cfg.gateway_url, QCMAP_MSGR_MAX_GATEWAY_URL_V01);
  }

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "echo %s %s >> %s",a5_ip,this->lan_cfg.gateway_url,this->host_file_path);
  ds_system_call(command, strlen(command));

  if(QcMapBackhaulWWANMgr &&  QcMapBackhaulWWANMgr->dhcpv6_dns_conf.dhcpv6_enable_state  ==
     QCMAP_MSGR_DHCPV6_MODE_UP_V01 )
  {
    memcpy(pub_ip6.sin6_addr.s6_addr, QcMapBackhaulWWANMgr->dhcpv6_dns_conf.pub_ip,
           QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));

    //Add ipv6 host entry for gateway URL
    inet_ntop(AF_INET6, &pub_ip6.sin6_addr, addr_pub_str, sizeof addr_pub_str);
    snprintf(command,
             MAX_COMMAND_STR_LEN,
             "echo %s %s >> %s", addr_pub_str, this->lan_cfg.gateway_url,this->host_file_path);
    ds_system_call(command, strlen(command));
  }

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "killall -s SIGHUP dnsmasq");
  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION SetGatewayUrl
==========================================================================*/
/*!
@brief
  Set the Gateway url.

@parameters
  char *url

@return
  true  - Set URL success
  flase - Set URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::SetGatewayUrl(const char *url,
                              uint32_t url_len,
                              qmi_error_type_v01 *qmi_err_num)
{
  boolean ret_val = false;
  QCMAP_LAN_Manager* lan_mgr = QCMAP_LAN_Manager::GetInstance();
  QCMAP_Virtual_LAN* QcMapVLANMgr = QCMAP_Virtual_LAN::Get_Instance(false);
  qcmap_msgr_vlan_config_v01 vlan_config_arr[QCMAP_MSGR_MAX_VLAN_ENTRIES_V01];
  uint32 len = 0;
  qmi_error_type_v01 qmi_err;
  int16_t  vlan_id = 0;
  QCMAP_LAN* QCMAPLANMgr = NULL;

  do
  {
    if((url == NULL) || (url_len == 0))
    {
      LOG_MSG_ERROR("Invalid argument\n", 0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
    {
      LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      break;
    }

    bzero(this->lan_cfg.gateway_url,QCMAP_MSGR_MAX_GATEWAY_URL_V01);
    memcpy(this->lan_cfg.gateway_url,url,url_len);

    this->EnableGatewayUrl();
    this->SetGatewayConfigToXML(url);

    ret_val = true;
    /*---------------------------------------------------------------------
    Configure GatewayURL for every VLAN configuration that exists.
    No need to write it into the XML file
    -----------------------------------------------------------------------*/
    if(QcMapVLANMgr->GetVLANConfig(vlan_config_arr,&len,&qmi_err))
    {
      for(int i = 0; i < len; i++)
      {
        vlan_id = vlan_config_arr[i].vlan_id;
        QCMAPLANMgr = lan_mgr->GetLANBridge(vlan_id);
        if(QCMAPLANMgr != NULL)
        {
          bzero(QCMAPLANMgr->lan_cfg.gateway_url,QCMAP_MSGR_MAX_GATEWAY_URL_V01);
          memcpy(QCMAPLANMgr->lan_cfg.gateway_url,url,url_len);
          QCMAPLANMgr->EnableGatewayUrl();
        }
      }
    }
  }while(0);

  return ret_val;
}

/*===========================================================================
 FUNCTION AddEbtablesRuleForBridgeMode
==========================================================================*/
/*!
@brief
  - Match the MAC address to find the node
  - If a match is found, add the default Ebtables rules for various interfaces

@parameters
  Pointer to qcmap_cm_client_data_info_t
@return
  none
@note
  - Dependencies
  - None
  - Side Effects
  - None
*/
/*=========================================================================*/
bool QCMAP_LAN::AddEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t* data)
{
  in_addr addr;
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_ConnectionManager::AddEbtablesRuleForBridgeMode()",0,0,0);

  if ( data == NULL)
  {
    LOG_MSG_ERROR("AddEbtablesRuleForBridgeMode - NULL pointer passed \n",0,0,0);
    return false;
  }

  addr.s_addr = data->ip_addr;
  ds_log_med(" AddEbtablesRuleForBridgeMode for IP address %s \n",inet_ntoa(addr));
  /* If the Update for IP address is for Primary AP or USB TE*
   * Update Ebtables WLAN-STA side for ARP-REPLY target for this entry
   * Update Ebtables WLAN-AP, USB side for ARP-ACCEPT target for local LAN*/
  if ((data->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
      (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01) ||
      (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01) ||
      (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01) ||
      (data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01))
  {
    /* WLAN-STA Side Entry */
    if(NULL != QcMapBackhaulWLANMgr)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
                "ebtables -t nat -A PREROUTING -i %s -p arp --arp-opcode Request\
                --arp-ip-dst %s -j arpreply \
                --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x\
                --arpreply-target ACCEPT",
                 QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, inet_ntoa(addr),
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
      ds_system_call(command, strlen(command));
    }

    /* AP Side Entry */
    snprintf(command, MAX_COMMAND_STR_LEN,
              "ebtables -t nat -I PREROUTING -i wlan%d -p arp --arp-opcode\
              Request --arp-ip-dst %s -j ACCEPT",
               QcMapMgr->ap_dev_num1, inet_ntoa(addr));
    ds_system_call(command, strlen(command));

    /* Guest AP Side Entry */
    if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ebtables -t nat -I PREROUTING -i wlan%d -p arp --arp-opcode\
               Request --arp-ip-dst %s -j ACCEPT",
               QcMapMgr->ap_dev_num2, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }

    /* USB Side Entry */
    bzero(usb_intf_name,sizeof(usb_intf_name));
    if ( QcMapTetheringMgr && QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
    {
      if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
      {
        strlcpy(usb_intf_name, RNDIS_IFACE, sizeof(usb_intf_name));
      }
      else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
      {
        strlcpy(usb_intf_name, ECM_IFACE, sizeof(usb_intf_name));
      }
      else
      {
        LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                       QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
        return false;
      }
      snprintf(command, MAX_COMMAND_STR_LEN,
                "ebtables -t nat -I PREROUTING -i %s -p arp --arp-opcode Request\
                --arp-ip-dst %s -j ACCEPT",
                 usb_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
  }
  return true;
}


/*===========================================================================
 FUNCTION DelEbtablesRuleForBridgeMode
==========================================================================*/
/*!
@brief
  - Match the MAC address to find the node
  - If a match is found, delete the Ebtables rules for various interfaces

@parameters
  Pointer to qcmap_cm_client_data_info_t
@return
  none
@note
 - Dependencies
 - None
 - Side Effects
 - None
*/
/*=========================================================================*/
bool QCMAP_LAN::DelEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t* data)
{
  in_addr addr;
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  LOG_MSG_INFO1("QCMAP_ConnectionManager::DelEbtablesRuleForBridgeMode()",0,0,0);
  if ( data == NULL)
  {
    LOG_MSG_ERROR("DelEbtablesRuleForBridgeMode - NULL pointer passed \n",0,0,0);
    return false;
  }

  addr.s_addr = data->ip_addr;
  /* If the Update for IP address is for Primary AP or USB TE*
   * Update Ebtables WLAN-STA side for ARP-REPLY target for this entry
   * Update Ebtables WLAN-AP, USB side for ARP-ACCEPT target for local LAN
   * Also Delete the ARP-Entry Manually, instead of wating for it to time.
   * This ensures imediate NEWNEIGH Event on re-connection*/
 if ((data->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
     (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01) ||
     (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01) ||
     (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_3_V01) ||
     (data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01))
 {
   /* WLAN-STA Side Entry */
   if(NULL != QcMapBackhaulWLANMgr)
   {
     snprintf(command, MAX_COMMAND_STR_LEN,
               "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode Request\
               --arp-ip-dst %s -j arpreply --arpreply-mac\
               %02x:%02x:%02x:%02x:%02x:%02x --arpreply-target ACCEPT",
                QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, inet_ntoa(addr),
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
     ds_system_call(command, strlen(command));
   }
   /* AP Side Entry */
   snprintf(command, MAX_COMMAND_STR_LEN,
            "ebtables -t nat -D PREROUTING -i wlan%d -p arp --arp-opcode Request\
            --arp-ip-dst %s -j ACCEPT",
             QcMapMgr->ap_dev_num1, inet_ntoa(addr));
   ds_system_call(command, strlen(command));

   if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01)
   {
     snprintf(command, MAX_COMMAND_STR_LEN,
              "ebtables -t nat -D PREROUTING -i wlan%d -p arp --arp-opcode Request\
              --arp-ip-dst %s -j ACCEPT",
               QcMapMgr->ap_dev_num2, inet_ntoa(addr));
     ds_system_call(command, strlen(command));
   }

   /* USB Side Entry */
   bzero(usb_intf_name, sizeof(usb_intf_name));
   if ( QcMapTetheringMgr && QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
   {
     if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
     {
       strlcpy(usb_intf_name, RNDIS_IFACE, sizeof(usb_intf_name));
     }
     else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
     {
       strlcpy(usb_intf_name, ECM_IFACE, sizeof(usb_intf_name));
     }
     else
     {
       LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                      QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
       return false;
     }
     snprintf(command, MAX_COMMAND_STR_LEN,
              "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode Request\
              --arp-ip-dst %s -j ACCEPT",
              usb_intf_name, inet_ntoa(addr));
     ds_system_call(command, strlen(command));
   }

   /* Delete ARP Entry */
   snprintf(command, MAX_COMMAND_STR_LEN,"arp -d %s",inet_ntoa(addr));
   ds_system_call(command, strlen(command));
 }
  return true;
}


/*===========================================================================
 FUNCTION DelEbtablesUSBRulesForBridgeMode
==========================================================================*/
/*!
@brief
 Del Etables rules for LAN clients in AP-STA Bridge Mode

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
/*=========================================================================*/
bool QCMAP_LAN::DelEbtablesUSBRulesForBridgeMode(void)
{
  char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  in_addr addr;
  qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;


  LOG_MSG_INFO1("QCMAP_Tethering::DelEbtablesUSBRulesForBridgeMode",0,0,0);
  bzero(usb_intf_name,sizeof(usb_intf_name));
  if (QcMapTetheringMgr && QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
  {
    strlcpy(usb_intf_name, RNDIS_IFACE,sizeof(usb_intf_name));
  }
  else if (QcMapTetheringMgr && QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
  {
    strlcpy(usb_intf_name, ECM_IFACE,sizeof(usb_intf_name));
  }
  else if (QcMapTetheringMgr)
  {
    LOG_MSG_ERROR("Incorrect USB LINK Detected: %d",
                  QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,
                  0,0);
    return false;
  }
  else
  {
    LOG_MSG_ERROR("Incorrect USB LINK Detected: & QcMapTetheringMgr"
                 "is NULL",0,0,0);
    return false;
  }
  /* Add the default Rule for LAN Side Reply */
  if(NULL != QcMapBackhaulWLANMgr)
  {
    strlcpy(netmask, inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_netmask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    snprintf(command, MAX_COMMAND_STR_LEN,
                       "ebtables -t nat -D PREROUTING -i %s -p arp --arp-ip-src\
                       =%s/%s --arp-opcode Request -j arpreply \
                       --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x \
                       --arpreply-target ACCEPT",
                       usb_intf_name,
                       inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw),
                       netmask, QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
    ds_system_call(command, strlen(command));
  }
  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Empty List: No rules to be deleted, per WLAN Iface\n",0,0,0);
    return true;
  }

  if (QcMapMgr->dual_wifi_mode)
  {
    /* Del Rules for WLAN Clients connected to Secondary AP 2*/
    device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
    node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                          qcmap_match_device_type);
    while(node)
    {
      list_data = (qcmap_cm_client_data_info_t*)node->data;
      if (list_data != NULL)
      {
        addr.s_addr = list_data->ip_addr;
        ds_log_med(" DelEbtablesUSBRulesForBridgeMode() for IP address %s \n",inet_ntoa(addr));
        snprintf(command, MAX_COMMAND_STR_LEN,
                    "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode\
                    Request --arp-ip-dst %s -j ACCEPT",
                     usb_intf_name, inet_ntoa(addr));
        ds_system_call(command, strlen(command));
      }
      else
      {
        LOG_MSG_ERROR(" The device information data  node is NULL\n",0,0,0);
      }
      node = ds_dll_search (node, (void*)&device_type, qcmap_match_device_type);
    }
  }

  /* Del Rules for WLAN Clients connected to Secondary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" DelEbtablesUSBRulesForBridgeMode() for IP address %s \n",inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode\
                  Request --arp-ip-dst %s -j ACCEPT",
                   usb_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information data  node is NULL\n",0,0,0);
    }
    node = ds_dll_search (node, (void*)&device_type, qcmap_match_device_type);
  }

  /* Del Rules for WLAN Clients connected to Primary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" DelEbtablesUSBRulesForBridgeMode() for IP address %s \n",
                   inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode\
                  Request --arp-ip-dst %s -j ACCEPT",
                   usb_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information node data is NULL\n",0,0,0);
    }
    node = ds_dll_search (node , (void*)&device_type, qcmap_match_device_type);
  }
  return true;
}


/*===========================================================================
  FUNCTION AddEbtablesUSBRulesForBridgeMode
==========================================================================*/
/*!
@brief
  Add Etables rules for LAN clients in AP-STA Bridge Mode

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
/*=========================================================================*/
bool QCMAP_LAN::AddEbtablesUSBRulesForBridgeMode(void)
{
  char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=GET_DEFAULT_BACKHAUL_WLAN_OBJECT();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  in_addr addr;
  qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;


  LOG_MSG_INFO1("QCMAP_Tethering::AddEbtablesUSBRulesForBridgeMode",0,0,0);
  snprintf(command,MAX_COMMAND_STR_LEN,"AddEbtablesUSBRulesForBridgeMode() > /dev/kmsg");
  ds_system_call(command,strlen(command));
  bzero(teth_intf_name,sizeof(teth_intf_name));
  if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
  {
    strlcpy(teth_intf_name, RNDIS_IFACE,sizeof(teth_intf_name));
  }
  else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
  {
    strlcpy(teth_intf_name, ECM_IFACE,sizeof(teth_intf_name));
  }
  else
  {
    LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x", QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
    return false;
  }
  if(NULL != QcMapBackhaulWLANMgr)
  {
    strlcpy(netmask, inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_netmask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    //here no null check for APSTA since bridge mode check by caller ensures object is created
    snprintf(command, MAX_COMMAND_STR_LEN,
                       "ebtables -t nat -A PREROUTING -i %s -p arp \
                       --arp-ip-src=%s/%s --arp-opcode Request -j arpreply\
                       --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x\
                       --arpreply-target ACCEPT",
                       teth_intf_name,
                       inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw),
                       netmask,QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
    ds_system_call(command, strlen(command));
  }
  /* Iterate through the list and get check for any WLAN-AP based entries */
  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("\nMatchMacAddrList() - Linked list head is NULL \n",0,0,0);
    return false;
  }

  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Empty List: No rules to be entered, per WLAN Iface\n",0,0,0);
    return true;
  }

  if (QcMapMgr->dual_wifi_mode)
  {
    /* Add Rules for WLAN Clients connected to Secondary AP 2 */
    device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_2_V01;
    node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                          qcmap_match_device_type);
    while(node)
    {
      list_data = (qcmap_cm_client_data_info_t*)node->data;
      if (list_data != NULL)
      {
        addr.s_addr = list_data->ip_addr;
        ds_log_med(" AddEbtablesUSBRulesForBridgeMode() for IP address %s \n",
                     inet_ntoa(addr));
        snprintf(command, MAX_COMMAND_STR_LEN,
                    "ebtables -t nat -I PREROUTING -i %s -p arp --arp-opcode\
                    Request --arp-ip-dst %s -j ACCEPT",
                     teth_intf_name, inet_ntoa(addr));
        ds_system_call(command, strlen(command));
      }
      else
      {
        LOG_MSG_ERROR(" The device information node data is NULL\n",0,0,0);
      }
      node = ds_dll_search (node , (void*)&device_type, qcmap_match_device_type);
    }
  }

  /* Add Rules for WLAN Clients connected to Secondary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" AddEbtablesUSBRulesForBridgeMode() for IP address %s \n",
                   inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -I PREROUTING -i %s -p arp --arp-opcode\
                  Request --arp-ip-dst %s -j ACCEPT",
                   teth_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information node data is NULL\n",0,0,0);
    }
    node = ds_dll_search (node , (void*)&device_type, qcmap_match_device_type);
  }

  /* Add Rules for WLAN Clients connected to Primary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" AddEbtablesUSBRulesForBridgeMode() for IP address %s \n",
                 inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -I PREROUTING -i %s -p arp --arp-opcode\
                   Request --arp-ip-dst %s -j ACCEPT",
                   teth_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information node is NULL\n",0,0,0);
    }
    node = ds_dll_search (node , (void*)&device_type, qcmap_match_device_type);
  }
  return true;
}

/*==========================================================
 FUNCTION    qcmap_match_device_type
===========================================================*/
/*!
@brief
  Comparator function for match ConnectedDevices device type

@parameters
  Two void pointers with the device type to be compared

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
static long int QCMAP_LAN::qcmap_match_device_type(const void *first, const void *second)
{
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  qcmap_msgr_device_type_enum_v01* device_type = (qcmap_msgr_device_type_enum_v01*)first;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  return((*device_type == connectedDevicesList->device_type) ? 0 : 1 );
}


/*==========================================================
  FUNCTION AddDHCPResRecToXML
==========================================================*/
/*!
@brief
  Add DHCP Reservation Record to the XML.

@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record
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

bool QCMAP_LAN::AddDHCPResRecToXML(qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record)
{

  struct in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  uint8 *mac = NULL;
  pugi::xml_document xml_file;
  pugi::xml_node root, child, subchild;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);


  // add new DHCP Reservation Record here.
  child = root.append_child(DHCPReservationRecord_TAG);
  if(dhcp_reserv_record->client_device_name[0]=='\0')//AP client
  {
  //MAC Address
  mac=dhcp_reserv_record->client_mac_addr;
  snprintf(mac_addr_str,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01,"%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  subchild = child.append_child(ClientMACAddr_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(mac_addr_str);
  }
  //IP to reserve
  subchild = child.append_child(ClientReservedIP_TAG);
  addr.s_addr = htonl(dhcp_reserv_record->client_reserved_ip);
  subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

  if(dhcp_reserv_record->client_device_name[0]!='\0')//USB client
  {
    subchild = child.append_child(ClientName_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(dhcp_reserv_record->client_device_name);
  }

  subchild = child.append_child(Reservation_TAG);
  snprintf(data, sizeof(data), "%d", dhcp_reserv_record->enable_reservation);
  subchild.append_child(pugi::node_pcdata).set_value(data);


  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  return true;
}

/*==========================================================
  FUNCTION EditDHCPResRecToXML
==========================================================*/
/*!
@brief
  Edit DHCP Reservation Record to the XML.

@parameters
  uint32_t client_ip,
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record
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

bool QCMAP_LAN::EditDHCPResRecToXML(uint32_t client_ip,
                                            qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record)
{
    char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/
    char data[MAX_STRING_LENGTH] = {0};
    in_addr addr;
    pugi::xml_document xml_file;
    pugi::xml_node root, child;
    char *main_c,*main_t;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    addr.s_addr = htonl(client_ip);
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      return false;
    }
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strcmp(child.name(), DHCPReservationRecord_TAG))
      {
        main_t=child.child(ClientReservedIP_TAG).child_value();  /*xml stored  ip address*/
        main_c=inet_ntoa(addr);  /* client address to delete*/
        if (strcmp (main_t,main_c)==0) /*if mach found*/
        {
          LOG_MSG_ERROR("Editing client with ip=%s to xml", main_c, 0, 0);
          if ( dhcp_reserv_record->client_device_name[0] != '\0')
          {
            child.child(ClientName_TAG).text()=dhcp_reserv_record->client_device_name;
          }
          /* mac address is not provided for USB client*/
          if ( check_non_empty_mac_addr(dhcp_reserv_record->client_mac_addr, mac_addr_string))
          {
            LOG_MSG_ERROR("Editing mac=%s to xml", mac_addr_string, 0, 0);
            child.child(ClientMACAddr_TAG).text()=mac_addr_string;
          }
          addr.s_addr = htonl(dhcp_reserv_record->client_reserved_ip);
          child.child(ClientReservedIP_TAG).text()=inet_ntoa(addr);
          snprintf(data, sizeof(data), "%d", dhcp_reserv_record->enable_reservation);
          child.child(Reservation_TAG).text()=data;

          QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
          break;
        }
      }
    }
    return true;
}


/*==========================================================
  FUNCTION DeleteDHCPResRecFromXML
==========================================================*/
/*!
@brief
  Delete DHCP Reservation Record from  the XML.

@parameters
  uint32_t client_ip

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

bool QCMAP_LAN::DeleteDHCPResRecFromXML(uint32_t client_ip)
{
    in_addr addr;
    pugi::xml_document xml_file;
    pugi::xml_node root, child;
    char *main_c,*main_t;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    addr.s_addr = htonl(client_ip);
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      return false;
    }
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strcmp(child.name(), DHCPReservationRecord_TAG))
      {
        main_t=child.child(ClientReservedIP_TAG).child_value();  /*xml stored  ip address*/
        main_c=inet_ntoa(addr);  /* client address to delete*/
        if (strcmp (main_t,main_c)==0) /*if mach found*/
        {
          LOG_MSG_ERROR("deleting client with ip=%s from xml", main_c, 0, 0);
          root.remove_child(child);
          QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
          break;
        }

      }
    }

    return true;
}



/*===========================================================================
  FUNCTION InitBridge
==========================================================================*/
/*!
@brief
  Initialize bridge interface.

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_LAN::InitBridge()
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char apps_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qmi_error_type_v01 qmi_err_num;
  int ret;
  uint8 i = 0;
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  QCMAP_Virtual_LAN* QcMapVLANMgr=QCMAP_Virtual_LAN::Get_Instance(false);
  QCMAP_L2TP* QcMapL2TPMgr=QCMAP_L2TP::Get_Instance(false);
  qcmap_msgr_ethernet_mode_v01 ethernet_mode = QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01;

  /* Create the Bridge Interface. only if not a VLAN tied to default PDN */
  if(!(this->bridge_vlan_id && (IS_DEFAULT_PROFILE(this->profile_handle))))
  {
    //assign bridge_name to bridge<vlan_id>
    snprintf(this->bridge_name, sizeof(this->bridge_name), "bridge%d", this->bridge_vlan_id);

    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "brctl addbr %s", this->bridge_name);
    ds_system_call(command, strlen(command));
  } else {

    //assign bridge_name to bridge0
    snprintf(this->bridge_name, sizeof(this->bridge_name), "bridge0");

    //assign ULA address
    memset(this->lan_cfg.ipv6_ula_addr, 0, sizeof(this->lan_cfg.ipv6_ula_addr));
    snprintf(this->lan_cfg.ipv6_ula_addr, sizeof(this->lan_cfg.ipv6_ula_addr), "%s:%x::%s",
             VLAN_BASE_IPV6_ADDR, this->bridge_vlan_id, STATIC_IID);

    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 addr add %s dev %s", this->lan_cfg.ipv6_ula_addr,
             this->bridge_name);
    ds_system_call(command, strlen(command));

    //Add IPv6 prefix based route for embedded VLAN ifaces
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route add %s:%x::/%d dev %s",
             VLAN_BASE_IPV6_ADDR, this->bridge_vlan_id, VLAN_IPV6_PREFIX_LEN_BITS,
             this->bridge_name);
    ds_system_call(command, strlen(command));

    this->bridge_inited = TRUE;

    // Update Prev LAN Config with the currrent config with which the bridge is created
    this->prev_lan_config.gw_ip = this->lan_cfg.apps_ip_addr;
    this->prev_lan_config.netmask =this->lan_cfg.sub_net_mask;
    this->prev_lan_config.enable_dhcp =this->lan_cfg.enable_dhcpd;
    this->prev_lan_config.dhcp_config.dhcp_start_ip =this->lan_cfg.dhcp_start_address;
    this->prev_lan_config.dhcp_config.dhcp_end_ip = this->lan_cfg.dhcp_end_address;
    this->prev_lan_config.dhcp_config.lease_time = this->lan_cfg.dhcp_lease_time;

    /* Create a socket over the bridge interface, to send RA's for prefix deperecation. */
    this->bridge_sock = this->CreateRawSocket(this->bridge_name);

    return true;
  }

  /* Bring up and configure the bridge interface. */
  /* Configure Static IP*/
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
      QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
  {
    addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
    strlcpy(apps_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(this->lan_cfg.sub_net_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
             this->bridge_name, apps_ip, netmask);
    /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
    if(this->bridge_vlan_id == 0 )
    {
      if (!this->UpdateLANConfigToIPACM())
        LOG_MSG_INFO1("Subnet Update to IPACM_CFG.xml failed ",0,0,0);
    }
#endif /* FEATURE_DATA_TARGET_MDM9607 */

    if (QcMapL2TPMgr)
    {
      QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(this->bridge_name,true);
    }
  }
  else /*Get IP via DHCP*/
  {
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s up", this->bridge_name);
  }
  ds_system_call(command, strlen(command));

  if(QCMAP_CM_ERROR == QCMAP_ConnectionManager::GetHWAddr((char*)
                                              &bridge_local_mac_addr.ether_addr_octet,
                                              this->bridge_name))
  {
    LOG_MSG_INFO1("Error in fetching Bridge MAC ",0,0,0);
  }
  else
  {
    for ( i = 0 ; i < 3 ; i++)
    {
      bridge_local_mac_addr.ether_addr_octet[i+3] += RANDOM_MAC_KEY;
    }
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s hw ether %s", this->bridge_name,
             ether_ntoa(&bridge_local_mac_addr));
    ds_system_call(command, strlen(command));
  }


  if(IS_VLAN_ID_VALID(this->bridge_vlan_id))
  {
    //assign ULA address
    memset(this->lan_cfg.ipv6_ula_addr, 0, sizeof(this->lan_cfg.ipv6_ula_addr));
    snprintf(this->lan_cfg.ipv6_ula_addr, sizeof(this->lan_cfg.ipv6_ula_addr), "%s:%x::%s",
             VLAN_BASE_IPV6_ADDR, this->bridge_vlan_id, STATIC_IID);

    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 addr add %s dev %s", this->lan_cfg.ipv6_ula_addr,
             this->bridge_name);
    ds_system_call(command, strlen(command));

    //Add IPv6 prefix based route for embedded VLAN ifaces
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route add %s:%x::/%d dev bridge%d",
             VLAN_BASE_IPV6_ADDR, this->bridge_vlan_id, VLAN_IPV6_PREFIX_LEN_BITS,
             this->bridge_vlan_id);
    ds_system_call(command, strlen(command));
  }

  /* Restart DHCPD */
  this->StopDHCPD();
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
  {
    this->StartDHCPD();
  }

  this->EnableGatewayUrl();
  //Enable the bridge forwarding for Ipv6
  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"echo 1 > /proc/sys/net/ipv6/conf/%s/forwarding",
           this->bridge_name);
  //Set the Garbage Collector threshold to 1
  strlcat(command, " && echo 1 > /proc/sys/net/ipv4/neigh/default/gc_thresh1", MAX_COMMAND_STR_LEN);
  strlcat(command, " && echo 1 > /proc/sys/net/ipv6/neigh/default/gc_thresh1", MAX_COMMAND_STR_LEN);
  ds_system_call(command, strlen(command));

  if (strncmp(this->bridge_name, BRIDGE_IFACE, strlen(BRIDGE_IFACE)) == 0)
  {
    CheckInterfacesWithVLAN();
    AddPhyInterfaceToDefaultBridge(QCMAP_MSGR_INTERFACE_TYPE_ETH_V01);
    AddPhyInterfaceToDefaultBridge(QCMAP_MSGR_INTERFACE_TYPE_ECM_V01);
    AddPhyInterfaceToDefaultBridge(QCMAP_MSGR_INTERFACE_TYPE_RNDIS_V01);
  }

  /* Enable Proxy ARP. */
  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"echo 1 > /proc/sys/net/ipv4/conf/%s/proxy_arp",
           this->bridge_name);
  /* Enable v4 Neigh Probe. */
  strlcat(command, " && echo 1 > /proc/sys/net/ipv4/neigh/default/neigh_probe", MAX_COMMAND_STR_LEN);
  /* Enable v6 Neigh Probe. */
  strlcat(command, " && echo 1 > /proc/sys/net/ipv6/neigh/default/neigh_probe", MAX_COMMAND_STR_LEN);
  ds_system_call( command, strlen(command));

  /* Bring up rndis and ecm interfaces up on bootup,so that it improves ip
        address assignment time during bootup */
  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s up", RNDIS_IFACE);
  ds_system_call(command, strlen(command));

  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s up", ECM_IFACE);
  ds_system_call(command, strlen(command));

  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig bridge%d promisc", this->bridge_vlan_id);
  ds_system_call(command, strlen(command));

  if ( QcMapBackhaulMgr &&
       (QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED) &&
       (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false) &&
       !QCMAP_Backhaul::IsPmipV6ModeEnabled())
  {
    /* Just Restart RADISH. */
    QcMapBackhaulMgr->StopRadish();
    QcMapBackhaulMgr->StartRadish();
  }

  this->bridge_inited = TRUE;

  // Update Prev LAN Config with the currrent config with which the bridge is created
  this->prev_lan_config.gw_ip = this->lan_cfg.apps_ip_addr;
  this->prev_lan_config.netmask =this->lan_cfg.sub_net_mask;
  this->prev_lan_config.enable_dhcp =this->lan_cfg.enable_dhcpd;
  this->prev_lan_config.dhcp_config.dhcp_start_ip =this->lan_cfg.dhcp_start_address;
  this->prev_lan_config.dhcp_config.dhcp_end_ip = this->lan_cfg.dhcp_end_address;
  this->prev_lan_config.dhcp_config.lease_time = this->lan_cfg.dhcp_lease_time;

  /* Create a socket over the bridge interface, to send RA's for prefix deperecation. */
  this->bridge_sock = this->CreateRawSocket(this->bridge_name);

  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 1 > /sys/devices/virtual/net/%s/bridge/multicast_querier",
           this->bridge_name);
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION DelBridge
==========================================================================*/
/*!
@brief
  Deletes the bridge interface.

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_LAN::DelBridge()
{
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_Virtual_LAN* QcMapVLANMgr=QCMAP_Virtual_LAN::Get_Instance(false);
  QCMAP_L2TP* QcMapL2TPMgr=QCMAP_L2TP::Get_Instance(false);


  /* Flush the BROUTING chain. */
  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ebtables -t broute -D BROUTING -F");
  ds_system_call(command, strlen(command));

  /* Flush the Filter chain. */
  memset(command, 0, sizeof(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ebtables -F");
  ds_system_call(command, strlen(command));

  /* Delete the Bridge Interface. only if not a VLAN tied to default PDN */
  if(!(this->bridge_vlan_id && (IS_DEFAULT_PROFILE(this->profile_handle))))
  {
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN,
             "echo 0 > /sys/devices/virtual/net/%s/bridge/multicast_querier", this->bridge_name);
    ds_system_call(command, strlen(command));

    /* Stop DHCP */
    this->StopDHCPD();

    /* Bring down the bridge interface. */
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", this->bridge_name);
    ds_system_call(command, strlen(command));

    /* Clean-up iptables for this bridge interface */
    this->BlockIPv4WWANAccess(true);
    this->BlockIPv6WWANAccess(true);

    /* Deletes the Bridge Interface. */
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "brctl delbr %s", this->bridge_name);
    ds_system_call(command, strlen(command));
  }

  //clean up dnsmasq resolv file, lease file, pid file
  if(this->bridge_vlan_id && (!IS_DEFAULT_PROFILE(this->profile_handle)))
  {
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "rm %s", this->dnsmasq_resolv_file_path);
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "rm %s", this->dnsmasq_lease_file_path);
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "rm %s", this->dnsmasq_pid_file_path);
    ds_system_call(command, strlen(command));

    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "rm %s", this->host_file_path);
    ds_system_call(command, strlen(command));
  }

  this->bridge_inited = false;

  if (QcMapL2TPMgr)
  {
    QcMapL2TPMgr->InstallDelTunnelsOnPhyIface(this->bridge_name,false);
  }

  if(this->bridge_sock >= 0)
  {
    close(this->bridge_sock);
  }
}


/*===========================================================================
  FUNCTION AllowBackhaulAccessOnIface
==========================================================================*/
/*!
@brief
  This function allows IPv4/IPv6 Access on provided Iface for MobileAP clients.

@parameters
  ip_vsn : IP type for which Access has to be allow
  devname : device (interface) for which access has to be allowed

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
void QCMAP_LAN::AllowBackhaulAccessOnIface(ip_version_enum_type ip_vsn, char *devname)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=GET_DEFAULT_BACKHAUL_ETHERNET_OBJECT();

  if (ip_vsn == IP_V4)
  {
    /*===========================================================================
      Allow IPv4 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing IPv4 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*Add rule to allow access */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*===========================================================================
      Allow IPv4 Access between PPP Iface and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing IPv4 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*Add rule to allow access */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "iptables -A FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }
  else if (ip_vsn == IP_V6)
  {
    /*===========================================================================
      Allow IPv6 Access between Bridge and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing Bridge IPv6 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -j DROP", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*Add rule to allow access */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s ! -o %s -j DROP", BRIDGE_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*===========================================================================
      Allow IPv4 Access between PPP Iface and Iface mentioned.
    ==========================================================================*/
    LOG_MSG_INFO1("Allowing PPP IPv6 Access for STA/Cradle/ETH Iface",0,0,0);

    /* Delete rule which is blocking the access. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    /*Delete duplicate rule*/
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));

    /*Add rule to allow access */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s ! -o %s -j DROP", PPP_IFACE,
             devname);
    ds_system_call(command, strlen(command));
  }
}

/*===========================================================================
  FUNCTION BlockIPv4WWANAccess
==========================================================================*/
/*!
@brief
  This function blocks IPv4 WWAN access for MobileAP clients.

@parameters
  bool clean_up, if true then it'll remove all rules.

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
boolean QCMAP_LAN::BlockIPv4WWANAccess(bool clean_up) //clean_up is defaulted to false
{
  char command[MAX_COMMAND_STR_LEN];
  const char *bridgeIface;

  bridgeIface = GET_ASSOCIATED_BRIDGE_IFACE_NAME(this->bridge_vlan_id);
/*===========================================================================
  Allow access between Bridge and Bridge.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -D FORWARD -i %s -o %s -j ACCEPT", bridgeIface, ALL_BRIDGES);
  ds_system_call(command, strlen(command));

  if (clean_up == false)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -A FORWARD -i %s -o %s -j ACCEPT", bridgeIface, ALL_BRIDGES);
    ds_system_call(command, strlen(command));
  }

/*===========================================================================
  Allow access between Bridge and PPP.
==========================================================================*/
  if (IS_DEFAULT_BRIDGE(this->bridge_vlan_id))
  {
    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -D FORWARD -i %s -o %s -j ACCEPT", bridgeIface, PPP_IFACE);
    ds_system_call(command, strlen(command));

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, bridgeIface);
    ds_system_call(command, strlen(command));

    if (clean_up == false)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
                "iptables -A FORWARD -i %s -o %s -j ACCEPT", bridgeIface, PPP_IFACE);
      ds_system_call(command, strlen(command));


      snprintf(command, MAX_COMMAND_STR_LEN,
                "iptables -A FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, bridgeIface);
      ds_system_call(command, strlen(command));
    }
  }

  /*===========================================================================
    Block WWAN access to MobileAP clients.
  ==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
          "iptables -D FORWARD -i %s -j DROP", bridgeIface);
  ds_system_call(command, strlen(command));

  if (clean_up == false)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -A FORWARD -i %s -j DROP", bridgeIface);
    ds_system_call(command, strlen(command));
  }

  /*===========================================================================
    Block WWAN access to PPP client.
    ==========================================================================*/
  if (IS_DEFAULT_BRIDGE(this->bridge_vlan_id))
  {
    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    if (clean_up == false)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
              "iptables -A FORWARD -i %s -j DROP", PPP_IFACE);
      ds_system_call(command, strlen(command));
    }
  }

  return true;
}


/*===========================================================================
  FUNCTION BlockIPv6WWANAccess
=========================================================================*/
/*!
@brief
  This function blocks IPv6 WWAN access for MobileAP clients.

@parameters
  bool clean_up, if true then it'll remove all rules.

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
boolean QCMAP_LAN::BlockIPv6WWANAccess(bool clean_up) //clean_up is defaulted to false
{
  char command[MAX_COMMAND_STR_LEN];
  const char *bridgeIface;

  bridgeIface = GET_ASSOCIATED_BRIDGE_IFACE_NAME(this->bridge_vlan_id);
/*===========================================================================
  Allow access between Bridge and Bridge.
==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", bridgeIface, ALL_BRIDGES);
  ds_system_call(command, strlen(command));

  if (clean_up == false)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s -o %s -j ACCEPT", bridgeIface, ALL_BRIDGES);
    ds_system_call(command, strlen(command));
  }

  /*===========================================================================
    Allow access between Bridge and PPP.
    ==========================================================================*/
  if (IS_DEFAULT_BRIDGE(this->bridge_vlan_id))
  {
    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", bridgeIface, PPP_IFACE);
    ds_system_call(command, strlen(command));

    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -D FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, bridgeIface);
    ds_system_call(command, strlen(command));

    if (clean_up == false)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip6tables -A FORWARD -i %s -o %s -j ACCEPT", bridgeIface, PPP_IFACE);
      ds_system_call(command, strlen(command));

      snprintf(command, MAX_COMMAND_STR_LEN,
               "ip6tables -A FORWARD -i %s -o %s -j ACCEPT", PPP_IFACE, bridgeIface);
      ds_system_call(command, strlen(command));
    }
  }

  /*===========================================================================
    Block WWAN access to MobileAP clients.
    ==========================================================================*/
  /* First delete duplicate rule if any. */
  snprintf(command, MAX_COMMAND_STR_LEN,
          "ip6tables -D FORWARD -i %s -j DROP", bridgeIface);
  ds_system_call(command, strlen(command));

  if (clean_up == false)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
            "ip6tables -A FORWARD -i %s -j DROP", bridgeIface);
    ds_system_call(command, strlen(command));
  }

  /*===========================================================================
    Block WWAN access to PPP client.
    ==========================================================================*/
  if (IS_DEFAULT_BRIDGE(this->bridge_vlan_id))
  {
    /* First delete duplicate rule if any. */
    snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -D FORWARD -i %s -j DROP", PPP_IFACE);
    ds_system_call(command, strlen(command));

    if (clean_up == false)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
             "ip6tables -A FORWARD -i %s -j DROP", PPP_IFACE);
      ds_system_call(command, strlen(command));
    }
  }

  return true;
}


/*===========================================================================
  FUNCTION AddPhyInterfaceToDefaultBridge
==========================================================================*/
/*!
@brief
  Adds either default phy interface or VLAN interface to default bridge0.

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void  QCMAP_LAN::AddPhyInterfaceToDefaultBridge
(
  qcmap_msgr_interface_type_enum_v01 iface_type
 )
{
  char command[MAX_COMMAND_STR_LEN] = {0};
  int ret;
  qcmap_msgr_ethernet_mode_v01 ethernet_mode = QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01;
  qcmap_msgr_cradle_mode_v01 cradle_mode = QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  qmi_error_type_v01 qmi_err_num;

  switch(iface_type)
  {
    case QCMAP_MSGR_INTERFACE_TYPE_ETH_V01:
      {
        /* Check for eth interface is enabled, if enabled bind it to bridge, so that dhcp request
           message are honoured earliest*/
        ret = QCMAP_ConnectionManager::IsInterfaceEnabled(ETH_IFACE);
        if( ret > 0 )
        {
          QCMAP_Backhaul_Ethernet::GetEthBackhaulMode(&ethernet_mode, &qmi_err_num);
          if (ethernet_mode == QCMAP_MSGR_ETHERNET_LAN_ROUTER_V01)
          {
            /* If vlan is enabled for eth interface and mapped to bridge0,
               then skip adding this interface to bridge0
             */
            if (bridge_vlan_id == 0 && (vlan_iface_mask & ETH_VLAN_ENABLED))
            {
              LOG_MSG_ERROR("eth interface on bridge0 is enabled for VLAN,"
                              "skip adding eth0 to bridge0",0,0,0);
            }
            else if (bridge_vlan_id == 0)
            {
              LOG_MSG_INFO1("Ethernet is enabled in LAN Router",0,0,0);
              snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
                       BRIDGE_IFACE, ETH_IFACE);
              ds_system_call(command, strlen(command));
            }
          }
        }
      }
      break;

    case QCMAP_MSGR_INTERFACE_TYPE_RNDIS_V01:
      {
        /* Check for rndis interface is enabled, if enabled bind it to bridge, so that dhcp request
           message are honoured earliest*/
        ret = QCMAP_ConnectionManager::IsInterfaceEnabled(RNDIS_IFACE);
        if( ret > 0 )
        {
          /* If vlan is enabled for rndis interface and mapped to bridge0,
             then skip adding this interface to bridge0
           */
          if (bridge_vlan_id == 0 && (vlan_iface_mask & RNDIS_VLAN_ENABLED))
          {
            LOG_MSG_ERROR("rndis interface on bridge0 is enabled for VLAN,"
                            "skip adding rndis0 to bridge0",0,0,0);
          }
          else if (bridge_vlan_id == 0)
          {
            LOG_MSG_INFO1("rndis is enabled ",0,0,0);
            memset(command, 0, sizeof(command));
            snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
                     BRIDGE_IFACE, RNDIS_IFACE);
            ds_system_call(command, strlen(command));
          }
        }
      }
      break;

    case QCMAP_MSGR_INTERFACE_TYPE_ECM_V01:
      {
        /* Check for ecm interface is enabled, if enabled bind it to bridge, so that dhcp request
           message are honoured earliest*/
        ret = QCMAP_ConnectionManager::IsInterfaceEnabled(ECM_IFACE);

        if( ret > 0)
        {
          QCMAP_Backhaul_Cradle::GetCradleMode(&cradle_mode,&qmi_err_num);

          if(cradle_mode == QCMAP_MSGR_CRADLE_LAN_ROUTER_V01 ||
             cradle_mode == QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01 ||
             cradle_mode == QCMAP_MSGR_CRADLE_DISABLED_V01)
          {
            /* If vlan is enabled for ecm interface and mapped to bridge0,
               then skip adding this interface to bridge0
             */
            if (bridge_vlan_id == 0 && (vlan_iface_mask & ECM_VLAN_ENABLED))
            {
              LOG_MSG_ERROR("ecm interface on bridge0 is enabled for VLAN,"
                              "skip adding ecm0 to bridge0",0,0,0);
            }
            else if (bridge_vlan_id == 0)
            {
              memset(command, 0, sizeof(command));
              snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
                       BRIDGE_IFACE, ECM_IFACE);
              ds_system_call(command, strlen(command));
            }
          }
        }
      }
      break;

    default:
      LOG_MSG_ERROR("Not valid interface %d", iface_type, 0, 0);
      break;
  }
}


/*===========================================================================
  FUNCTION CheckInterfacesWithVLAN
==========================================================================*/
/*!
@brief
  For bridge0, find all vlan interfaces (eth, ecm or rndis).

@return
  None

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_LAN::CheckInterfacesWithVLAN()
{
  pugi::xml_document xml_file;
  pugi::xml_node root, vconfig_node, id_node, iface_node, vlan_node;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  int16_t vlan_id = 0, vlan_id_in_bridge0 = 0;

  if(!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return;
  }

  /* Check if VLAN is enabled for bridge0 */
  vlan_node = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_ID_TAG);
  if (vlan_node == NULL)
  {
    LOG_MSG_ERROR("No VLAN found in default bridge",0,0,0);
    return;
  }
  else
  {
    vlan_id_in_bridge0 = atoi((char *)vlan_node.text().get());
    if (vlan_id_in_bridge0 == 0)
    {
      LOG_MSG_ERROR("Invalid VLAN configured in default bridge0", 0,0,0);
      return;
    }
  }

  /* Check all interfaces with above found vlan-id */
  if((root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(VLAN_TAG)) == NULL)
  {
    LOG_MSG_ERROR("Error finding VLAN tag in xml file", 0, 0, 0);
    return;
  }

  for(vconfig_node = root.first_child();
      vconfig_node != NULL;
      vconfig_node = vconfig_node.next_sibling())
  {
    id_node = vconfig_node.child(VCONFIG_ID_TAG);
    iface_node = vconfig_node.child(VCONFIG_IFACE_TAG);
    vlan_id = atoi((char*)id_node.text().get());

    if (vlan_id == vlan_id_in_bridge0)
    {
      if(strcmp((char*)iface_node.text().get(), ETH_IFACE) == 0)
      {
        this->vlan_iface_mask |= ETH_VLAN_ENABLED;
      }
      else if (strcmp((char*)iface_node.text().get(), ECM_IFACE) == 0)
      {
        this->vlan_iface_mask |= ECM_VLAN_ENABLED;
      }
      else if (strcmp((char*)iface_node.text().get(), RNDIS_IFACE) == 0)
      {
        this->vlan_iface_mask |= RNDIS_VLAN_ENABLED;
      }
    }
  }
  LOG_MSG_INFO1("bridge=%d, vlan_iface_mask = %d", this->bridge_vlan_id, this->vlan_iface_mask, 0);
}

/*===========================================================================
  FUNCTION CreateRawSocket
==========================================================================*/
/*!
@brief
  Creates the socket

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
int QCMAP_LAN::CreateRawSocket(const char *dev_name) const
{
  int retry_count = 0;
  int sock = -1;
  /* According to RFC Hop Limit must be set to 255 in order
    to protect against off-link packets.
   */
  int hop_limit = 255;

  while(retry_count < QCMAP_BRIDGE_MAX_RETRY )
  {
    if (retry_count)
    {
      if (sock >= 0)
        close(sock);
      usleep(QCMAP_BRIDGE_MAX_TIMEOUT_MS);
    }

    if( retry_count == QCMAP_BRIDGE_MAX_RETRY )
    {
      LOG_MSG_ERROR("Reached maximum retry attempts for CreateRawSocket %d",
                    retry_count, 0, 0);
      LOG_MSG_ERROR("Error: Bridge Sock cannot be created: errno=%d, retry=%d",
                  errno, retry_count, 0);
      sock = -1;
      break;
    }

    retry_count++;
    /* Create socket and set required options */
    sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    if (sock < 0)
    {
      LOG_MSG_ERROR("%s: Unable to create socket %d", __func__, errno, 0);
      continue;
    }

    if (setsockopt(sock,
                   IPPROTO_IPV6,
                   IPV6_MULTICAST_HOPS,
                   (char *)&hop_limit,
                   sizeof(hop_limit)) < 0)
    {
      LOG_MSG_ERROR("%s: Set sockopt failed %d", __func__, errno, 0);
      continue;
    }

    if (setsockopt(sock,
                   IPPROTO_IPV6,
                   IPV6_UNICAST_HOPS,
                   (char *)&hop_limit,
                   sizeof(hop_limit)) < 0)
    {
      LOG_MSG_ERROR("%s: Set sockopt failed %d", __func__, errno, 0);
      continue;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, dev_name, strlen(dev_name)+1) < 0)
    {
      LOG_MSG_ERROR("%s: Unable to bind socket to interface %s", __func__, dev_name, 0);
      continue;
    }

    LOG_MSG_INFO1("Created Raw Socket, sock=%d", this->bridge_sock, 0,0);
    break;
  }

  return sock;
}


/*===========================================================================
  FUNCTION EnableIPPassthrough
==========================================================================*/
/*!
@brief
  Enables IP Passthrough.

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
bool QCMAP_LAN::EnableIPPassthrough
(
  char *mac,
  char *host,
  qcmap_msgr_device_type_enum_v01 dev_type
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  qmi_error_type_v01 qmi_err_num;
  struct ifreq buffer;
  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 pri_dns_addr = 0;
  struct in_addr addr;
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_COMMAND_STR_LEN];
  FILE *in=NULL;
  in_addr netip, netmask;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  if(!QcMapBackhaulMgr)
  {
    LOG_MSG_ERROR("QcMapBackhaulMgr is NULL \n", 0, 0, 0);
    return false;
  }

  QCMAP_CM_LOG_FUNC_ENTRY();

  memset(&buffer, 0x00, sizeof(buffer));
  this->lan_cfg.ip_passthrough_cfg.device_type= dev_type;
  memset(this->lan_cfg.ip_passthrough_cfg.rmnet_def_route , 0, MAX_COMMAND_STR_LEN);

  if (this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
  {
    LOG_MSG_INFO1("\nIP Passthrough is Already Running\n",0,0,0);
    return false;
  }

  LOG_MSG_INFO1("\nDevice type %d",dev_type,0,0);

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    if(!( QcMapTetheringMgr &&
           QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true))
    {
      LOG_MSG_ERROR("\nUSB Link Not Enabled",0,0,0);
      return false;
    }
  }

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    if(!( QcMapTetheringMgr &&
           QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == true))
    {
      LOG_MSG_ERROR("\nEthernet Link Not Enabled",0,0,0);
      return false;
    }
  }
  /*Check if backhaul is connected*/
  if (QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED )
  {
    LOG_MSG_INFO1("WWAN is not Connected.",0,0,0);
    return false;
  }

  /*Check if current backhaul is WWAN*/
  if (QCMAP_Backhaul::current_backhaul != BACKHAUL_TYPE_WWAN )
  {
    LOG_MSG_INFO1("Current backhaul mode is not WWAN.",0,0,0);
    return false;
  }

  if ( QcMapBackhaulMgr->QcMapBackhaulWWAN->GetIPv4NetworkConfig(&public_ip, &pri_dns_addr,
                                                                 &sec_dns_addr, &default_gw_addr, &qmi_err_num)
       != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_INFO1("Error in GetIPv4NetworkConfig() %d", qmi_err_num,0,0);
    return false;
  }

  /*getting network netmask*/
  if ( !QcMapBackhaulMgr->GetIP(&netip.s_addr, &netmask.s_addr,
      QcMapBackhaulMgr->wan_cfg.ipv4_interface))
  {
    LOG_MSG_ERROR("Unable to get Netmask",0,0,0);
    return false;
  }
  /*Use the current public IP*/
  addr.s_addr = public_ip;

  /*add dhcp reservation record*/
  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    /*Remove any other dhcp reservation from file for this ip & hostname*/
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", host, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));

  /*dhcp reservation entry in file*/
    snprintf(command, MAX_COMMAND_STR_LEN,
               "echo %s,%s >> %s", host, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    /*Remove any other dhcp reservation from file for this ip & mac*/
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", mac, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));

    /*dhcp reservation entry in file*/
    snprintf(command, MAX_COMMAND_STR_LEN,
               "echo %s,%s >> %s", mac,inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  /*Set the IP Passthrough Enable state*/
  this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active = true;

  /* Removing conntrack entry for public ip. */
  QcMapTetheringMgr->DelConntrack(this->lan_cfg.ip_passthrough_cfg.public_ip);

  /*storing WWAN Interface default route*/
  snprintf(data, MAX_COMMAND_STR_LEN,
           "ip route show | grep default | grep %s",
           QcMapBackhaulMgr->wan_cfg.ipv4_interface);
  if ((in = popen(data, "r")))
  {
    /*reading buffer return by popen*/
    if(fgets(data, sizeof(data), in)!=NULL)
    {
      data[strlen(data) - 1] = '\0' ;
      snprintf(this->lan_cfg.ip_passthrough_cfg.rmnet_def_route,
               MAX_COMMAND_STR_LEN,"%s", data);
    }
    pclose(in);
  }

  /* Assign a link local ip to WWAN Interface iface. */
  addr.s_addr = this->lan_cfg.ip_passthrough_cfg.rmnet_reserved_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,"ifconfig %s %s netmask %s up",
           QcMapBackhaulMgr->wan_cfg.ipv4_interface, inet_ntoa(addr), "255.255.255.255");
  ds_system_call( command, strlen(command));

  /* Add WWAN Interface gw ip to bridge0. */
  addr.s_addr = default_gw_addr;
  snprintf(command, MAX_COMMAND_STR_LEN,"ip addr add %s/32 dev %s", inet_ntoa(addr),BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /*Add wan ipv4 interface as the default route*/
  snprintf(command, MAX_COMMAND_STR_LEN,"ip route add default dev %s",
             QcMapBackhaulMgr->wan_cfg.ipv4_interface);
  ds_system_call( command, strlen(command));

  /* Saving public ip*/
  this->lan_cfg.ip_passthrough_cfg.public_ip = public_ip;
  this->StartDHCPD();

  /*
    Marking DHCP packets so that only dhcp packets can be handled by bridge
    creating custom routing table 'ippastbl' for dhcp request handle
  */
  addr.s_addr = default_gw_addr;
  snprintf(data, MAX_COMMAND_STR_LEN,
           "ip route show table local | grep \"local %s\"",inet_ntoa(addr));
  if ((in = popen(data, "r")))
  {
     /* Reading buffer return by popen. */
    if(fgets(data, sizeof(data), in)!=NULL)
    {
      data[strlen(data) - 1] = '\0' ;
      ds_system_call("iptables -A PREROUTING -i bridge0 -t nat -p udp  --dport 67  -j"
                           " MARK --set-mark 1", strlen("iptables -A PREROUTING -i bridge0"
                           " -t nat -p udp  --dport 67  -j MARK --set-mark 1"));

      /* Adding custom routing table ippastbl. */
      snprintf(command, sizeof(command), "echo 200 ippastbl >> %s", QCMAP_ROUTE_TABLE_PATH);
      ds_system_call(command, strlen(command));

      /* Removing gatway ip route from local table. */
      snprintf(command, MAX_COMMAND_STR_LEN,"ip route del %s table local" ,
                 data);
      ds_system_call(command, strlen(command));

      /* ip rule to handle marked packets. */
      snprintf(command, MAX_COMMAND_STR_LEN, "ip rule add fwmark 1 table ippastbl");
      ds_system_call(command, strlen(command));

      /* Handling gatway route from ippastbl table.*/
      snprintf(command, MAX_COMMAND_STR_LEN, "ip route add %s table ippastbl",data);
      ds_system_call(command, strlen(command));
      pclose(in);


    }
    else
    {
      LOG_MSG_ERROR("\nUnable to get gatway ip route from local table\n",0,0,0);
    }
  }

  /* Add route for the the IP. */
  addr.s_addr = public_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,
             " ip route add %s dev %s", inet_ntoa(addr), BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /* Add SNAT entry. */
  addr.s_addr = public_ip;
  snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -I POSTROUTING -o %s -j SNAT --to %s",
              QcMapBackhaulMgr->wan_cfg.ipv4_interface, inet_ntoa(addr));
  ds_system_call(command, strlen(command));

  /* Saving netmask and gateway ip*/
  this->lan_cfg.ip_passthrough_cfg.public_netmask= netmask.s_addr;
  this->lan_cfg.ip_passthrough_cfg.rmnet_gateway_ip = default_gw_addr;

  /* Drop the duplicate address detection for the public IP*/
  addr.s_addr = public_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ebtables -t nat -I PREROUTING -p arp --arp-ip-src=0.0.0.0 --arp-"
           "ip-dst=%s --arp-opcode Request -j DROP", inet_ntoa(addr));
  ds_system_call(command, strlen(command));

#ifndef FEATURE_DATA_TARGET_MDM9607
  /* Updating the IPACM XML with the wwan iface IP. */
  if(!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPPassthroughFlag_TAG);
  root.child(IPPassthroughMode_TAG).text() = 1;
  /* If the device type is ethernet, update the mac address of the client for
   * which passthrough is enabled. For usb client, mac will be updated
   * dynamically.
   */
  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
    root.child(IPPassthroughMacAddr_TAG).text() = mac;
  else
    root.child(IPPassthroughMacAddr_TAG).text() = 0;
  QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif

  snprintf(command,MAX_COMMAND_STR_LEN,"echo IP Passthrough Activated > /dev/kmsg");
  ds_system_call(command,strlen(command));

  LOG_MSG_INFO1("IP Passthrough Enabled \n",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DisableIPPassthrough
==========================================================================*/
/*!
@brief
  Disables IP Passthrough.

@parameters
  bool - Boolean to indicate whether default route is added or not.

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
bool QCMAP_LAN::DisableIPPassthrough(bool default_route)
{
  qmi_error_type_v01 qmi_err_num;
  struct in_addr addr, netmask;
  char command[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  qcmap_msgr_device_type_enum_v01 dev_type ;
  char host_str[MAX_STRING_LENGTH]={0};
  char mac_str[MAX_STRING_LENGTH]={0};
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();


  QCMAP_CM_LOG_FUNC_ENTRY();

  if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
  {
    LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
    return false;
  }


  if (!this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
  {
    LOG_MSG_ERROR("\nIP Passthrough is Already Disabled\n",0,0,0);
    return false;
  }

  this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active = false;

  /* Delete the WWAN gw ip from bridge. */
  addr.s_addr = this->lan_cfg.ip_passthrough_cfg.rmnet_gateway_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,
             " ip addr delete %s/32 dev %s", inet_ntoa(addr), BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /* Delete route for the the IP. */
  addr.s_addr = this->lan_cfg.ip_passthrough_cfg.public_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,
             " ip route delete %s dev %s", inet_ntoa(addr), BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /* Delete SNAT entry. */
  addr.s_addr = this->lan_cfg.ip_passthrough_cfg.public_ip;
  snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -D POSTROUTING -o %s -j SNAT --to %s",
              QcMapBackhaulMgr->wan_cfg.ipv4_interface,
              inet_ntoa(addr));
  ds_system_call(command, strlen(command));

  /* Remove the ebtables rules for DAD of the public IP*/
  addr.s_addr = this->lan_cfg.ip_passthrough_cfg.public_ip;
  snprintf( command, MAX_COMMAND_STR_LEN,
              "ebtables -t nat -D PREROUTING -p arp --arp-ip-src=0.0.0.0 --arp-"
              "ip-dst=%s --arp-opcode Request -j DROP", inet_ntoa(addr));
  ds_system_call(command, strlen(command));

  /*Remove  dhcp reservation from file*/
  dev_type =  this->lan_cfg.ip_passthrough_cfg.device_type;

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", host_str, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", mac_str, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  /* Sending signal to dnsmasq to read reservervation file again. */
  snprintf(command, MAX_COMMAND_STR_LEN, "killall -1 dnsmasq");
  ds_system_call(command, strlen(command));
  QcMapTetheringMgr->DelConntrack(this->lan_cfg.ip_passthrough_cfg.rmnet_reserved_ip);

  /* Remove marking for DHCP packets.*/
  ds_system_call("iptables -D PREROUTING -i bridge0 -t nat -p udp  --dport 67  -j"
                       " MARK --set-mark 1", strlen("iptables -A PREROUTING -i bridge0"
                       " -t nat -p udp  --dport 67  -j MARK --set-mark 1"));

  /* Delete ip rule to handle marked packets */
  snprintf(command, MAX_COMMAND_STR_LEN, "ip rule del fwmark 1 table ippastbl");
  ds_system_call(command, strlen(command));

  /* Flush ippastbl  table. */
  snprintf(command, MAX_COMMAND_STR_LEN, "ip route flush table ippastbl");
  ds_system_call(command, strlen(command));

  /* Remove custom routing table ippastbl. */
  snprintf(command, sizeof(command), "sed -i '/200 ippastbl/d' %s", QCMAP_ROUTE_TABLE_PATH);
  qcmap_edit_file(command, strlen(command), QCMAP_ROUTE_TABLE_PATH, strlen(QCMAP_ROUTE_TABLE_PATH));

  if (QcMapBackhaulMgr &&
      QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN &&
      QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED )
  {
    //Assign public ip to WWAN Interface iface
    addr.s_addr = this->lan_cfg.ip_passthrough_cfg.public_ip;
    netmask.s_addr = this->lan_cfg.ip_passthrough_cfg.public_netmask;
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s ",
             QcMapBackhaulMgr->wan_cfg.ipv4_interface, inet_ntoa(addr));
    snprintf(temp_buf, MAX_COMMAND_STR_LEN, " netmask %s up",inet_ntoa(netmask));
    strlcat(command, temp_buf, MAX_COMMAND_STR_LEN);
    ds_system_call( command, strlen(command));

    //route for gateway ip .reachable from WAN Interface
    addr.s_addr = this->lan_cfg.ip_passthrough_cfg.rmnet_gateway_ip;

    /*addding stored default route as adding ip to WAN Interface removes default route */
    if ((strlen(this->lan_cfg.ip_passthrough_cfg.rmnet_def_route) > 0) &&
        default_route)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,"ip route add %s ",
               this->lan_cfg.ip_passthrough_cfg.rmnet_def_route);
      ds_system_call( command, strlen(command));
    }
  }

  memset(&this->lan_cfg.ip_passthrough_cfg.rmnet_def_route, 0,
         MAX_COMMAND_STR_LEN);

#ifndef FEATURE_DATA_TARGET_MDM9607
/* Update IPACM XML. */
  if (!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPPassthroughFlag_TAG);
  root.child(IPPassthroughMode_TAG).text() = 0;
  root.child(IPPassthroughMacAddr_TAG).text() = 0;
  QCMAP_ConnectionManager::WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif

  snprintf(command,MAX_COMMAND_STR_LEN,"echo IP Passthrough Deactivated > /dev/kmsg");
  ds_system_call(command,strlen(command));

  LOG_MSG_INFO1("IP Passthrough Disabled \n",0,0,0);
  return true;
}

/*==========================================================
  FUNCTION SetIPPassthroughConfig
==========================================================*/
/*!
@brief
  Sets the IP Passthrough configuration

@parameters
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
  bool new_config ,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 * qmi_err_num

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

bool QCMAP_LAN::SetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
  bool new_config ,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 * qmi_err_num
)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root, parent,child, subchild;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  bool mac_addr_non_empty = true;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_msgr_ip_passthrough_mode_enum_v01 old_enable_state;

  QCMAP_CM_LOG_FUNC_ENTRY();

  mac_addr_non_empty = check_non_empty_mac_addr(passthrough_config->mac_addr,
                                                mac_addr_string);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG)\
            .child(MobileAPLanCfg_TAG).child(IPPassthroughCfg_TAG);
  snprintf(data, sizeof(data), "%d", enable_state);

  if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_UP_V01)
  {
    LOG_MSG_INFO1("SetIP passthrough mode up",0,0,0);

    if (this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
    {
        LOG_MSG_ERROR("\nAlready enabled IP Passthrough",0,0,0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
    }

    if (new_config)// Give a new config for IP Passthrough
    {
      /*
      Update the XML with the new ip passthrough config
      */
      switch (passthrough_config->device_type)
      {
        case QCMAP_MSGR_DEVICE_TYPE_USB_V01:
          root.child(IPPassthroughDeviceType_TAG).text() = "USB";
          root.child(IPPassthroughHostName_TAG).text() = passthrough_config->client_device_name;
          LOG_MSG_INFO1("\nDevice Type = USB",0,0,0);
        break;

        case QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01:
          root.child(IPPassthroughDeviceType_TAG).text() = "ETH";
          if (mac_addr_non_empty) {
             root.child(IPPassthroughMacAddr_TAG).text() = mac_addr_string;
          }
          else {
          LOG_MSG_ERROR("Empty MAC Address.", 0, 0, 0);
          return false;
          }
          LOG_MSG_INFO1("\nDevice Type = ETH", 0, 0, 0);
        break;

        default:
          LOG_MSG_ERROR("Invalid Device type.", 0, 0, 0);
          return false;
      }
    }
    else
    {
      // Check if there is an existing configuration
      if(this->GetIPPassthroughConfig
        (&old_enable_state, passthrough_config,qmi_err_num))
      {
        if (!(passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01 ||
            passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01))
        {
          LOG_MSG_ERROR("No existing configuration\n",0,0,0);
          *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
          return false;
        }
      }
      else
      {
        LOG_MSG_ERROR("Get IP Passthrough Flag failed\n",0,0,0);
        return false;
      }
    }

    // Set the XML flag with the ip passthrough state
    root.child(IPPassthroughEnable_TAG).text() = data;

    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

    if (passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
    {
      if (!(QcMapTetheringMgr &&
          QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true))
      {
        LOG_MSG_ERROR("\nUSB Link Not Enabled",0,0,0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return true;
      }
    }
    if (passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
    {
      if (!(QcMapTetheringMgr &&
            QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == true))
      {
        LOG_MSG_ERROR("\nEthernet Link Not Enabled",0,0,0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return true;
      }
    }

    //Check if backhaul is connected
    if ( QcMapBackhaulMgr && (QcMapBackhaulMgr->QcMapBackhaulWWAN) && QcMapBackhaulMgr->QcMapBackhaulWWAN->GetState() != QCMAP_CM_WAN_CONNECTED )
    {
      LOG_MSG_ERROR("WWAN is not Connected.",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }

    //Check if current backhaul is WWAN
    if (!(QcMapBackhaulMgr &&
        QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN ))
    {
      LOG_MSG_INFO1("Current backhaul mode is not WWAN.",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }

    LOG_MSG_INFO1("\nRestart the tethered client ",0,0,0);
    this->RestartTetheredClient(passthrough_config->device_type);
  }

  else if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_DOWN_V01)
  {
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG)\
            .child(MobileAPLanCfg_TAG).child(IPPassthroughCfg_TAG);
    snprintf(data, sizeof(data), "%d", enable_state);

    // Set the XML flag with the ip passthrough state
    root.child(IPPassthroughEnable_TAG).text() = data;
    /* Update the XML. */
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

    /* Restart the tethered client if passthrough active. */
    if(this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
    {
      this->RestartTetheredClient(
      this->lan_cfg.ip_passthrough_cfg.device_type);
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION GetIPPassthroughConfig
==========================================================================*/
/*!
@brief
  Get IP Passthrough Status/Configuration

@parameters
  qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 *qmi_err_num

@return
  true  -on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::GetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 *qmi_err_num
) const
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  char content_buf[MAX_STRING_LENGTH];
  QCMAP_CM_LOG_FUNC_ENTRY();
  LOG_MSG_INFO1("\nGetIPPassthrough in API",0,0,0);
  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG)\
          .child(MobileAPLanCfg_TAG).child(IPPassthroughCfg_TAG);
  *enable_state = atoi(root.child(IPPassthroughEnable_TAG ).child_value());

  strlcpy(content_buf,root.child(IPPassthroughDeviceType_TAG).child_value(),MAX_STRING_LENGTH);
  if (strncmp(content_buf,"USB",MAX_STRING_LENGTH) == 0)
  {
    passthrough_config->device_type = QCMAP_MSGR_DEVICE_TYPE_USB_V01;
    strlcpy(passthrough_config->client_device_name,
              root.child(IPPassthroughHostName_TAG).child_value(),
              QCMAP_MSGR_DEVICE_NAME_MAX_V01);
  }
  else if (strncmp(content_buf,"ETH",MAX_STRING_LENGTH) == 0)
  {
    passthrough_config->device_type = QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01;
    strlcpy(content_buf,
              root.child(IPPassthroughMacAddr_TAG).child_value(), MAX_STRING_LENGTH);
    for (int i = 0; i < QCMAP_MSGR_MAC_ADDR_LEN_V01; i++)
    {
      passthrough_config->mac_addr[i] =
      (ds_hex_to_dec(content_buf[i * 3]) << 4) |ds_hex_to_dec(content_buf[i * 3 + 1]);
    }
  }
  else
  {
    passthrough_config->device_type = -1;
  }

  return true;
}

/*===========================================================================
  FUNCTION RestartTetheredClient
==========================================================================*/
/*!
@brief
  Restart the tethered client

@parameters
  qcmap_msgr_device_type_enum_v01 dev_type

@return
  true  -on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::RestartTetheredClient(qcmap_msgr_device_type_enum_v01 dev_type) const
{
  int iface_added = -1;
  QCMAP_CM_LOG_FUNC_ENTRY();
  FILE *cmd = NULL;
  char *process =NULL;
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();

  /*Restart USB client*/
  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {

    process = "brctl show | grep -c 'rndis0\\|ecm0' ";
    cmd = popen(process, "r");
    if(cmd)
    {
      iface_added = fgetc(cmd);
      pclose(cmd);
    }

    //Check if USB is enabled
    if ((!(QcMapTetheringMgr &&
          QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true))
          && iface_added == '0' )
    {
      LOG_MSG_ERROR("USB Link Not Enabled\n",0,0,0);
      return false;
    }

    if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
        QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
    {
      LOG_MSG_ERROR("Cradle Active. Do not Restart. \n",0,0,0);
      return false;
    }

    LOG_MSG_INFO1("RestartTetheredClient: USB",0,0,0);

    //Check if current backhaul is WWAN and PDN is default PDN
    if ((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN) &&
        QcMapBackhaulMgr && (QcMapBackhaulMgr->QcMapBackhaulWWAN) &&
        (QcMapBackhaulMgr->QcMapBackhaulWWAN == GET_DEFAULT_BACKHAUL_WWAN_OBJECT()))
    {
      //Check if ip passthough is active
      if(!this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
      {
        QcMapMgr->dont_bringdown_backhaul = true;
      }
    }
    if (DS_TARGET_LE_CHIRON == ds_get_target())
    {
      ds_system_call("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC ",
              strlen("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC "));
      usleep(500000);
      /* Delay for 1 secs since USB UP/DOWN in immediate instance causes issues*/
      ds_system_call("echo a600000.dwc3 > /sys/kernel/config/usb_gadget/g1/UDC ",
              strlen("echo a600000.dwc3 > /sys/kernel/config/usb_gadget/g1/UDC "));
    }
    else
    {
      ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",
                          strlen("echo 0 > /sys/class/android_usb/android0/enable "));
      usleep(500000);
      /* Delay for 1 secs since USB UP/DOWN in immediate instance causes issues*/
      ds_system_call("echo 1 > /sys/class/android_usb/android0/enable ",
                          strlen("echo 1 > /sys/class/android_usb/android0/enable "));
    }
  }

  /* Restart Ethernet client*/
  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    //Check if ETHERNET is enabled
    if (!(QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == true))
    {
      //Check if Ethernet is enabled
      LOG_MSG_ERROR("\nEthernet Link Not Enabled",0,0,0);
      return false;
    }

    if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() ||
        QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
    {
      LOG_MSG_ERROR("Ethernet Backhaul Active. Do not Restart. \n",0,0,0);
      return false;
    }

    LOG_MSG_INFO1("RestartTetheredClient: Ethernet",0,0,0);

    //Check if current backhaul is WWAN and PDN is default PDN
    if ((QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN) &&
        QcMapBackhaulMgr && (QcMapBackhaulMgr->QcMapBackhaulWWAN) &&
        (QcMapBackhaulMgr->QcMapBackhaulWWAN == GET_DEFAULT_BACKHAUL_WWAN_OBJECT()))
    {
      //Check if ip passthough is active
      if(!this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
      {
        QcMapMgr->dont_bringdown_backhaul = true;
      }
    }

    /*
      restart ethernet interface
      setting autoneg off then on to restart ethernet
    */
     QCMAP_LAN::ToggleAutoNegForEth(ETH_IFACE);
  }
  return true;
}

/*===========================================================================
  FUNCTION GetIPPassthroughState
==========================================================================*/
/*!
@brief
  Get IP Passthrough State

@parameters
  qmi_err_num

@return
  active_state

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::GetIPPassthroughState(qmi_error_type_v01 *qmi_err_num) const
{
  if(this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
  {
    LOG_MSG_INFO1("IP Passthrough state is active", 0, 0, 0);
  } else {
    LOG_MSG_INFO1("IP Passthrough state is not active", 0, 0, 0);
  }

  return this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active;
}


/*===========================================================================
  FUNCTION UpdateIPAWithVLANIOCTL
==========================================================================*/
/*!
@brief
  Update IPA VLAN IOCTL with ipa_ioc_bridge_vlan_mapping_info

@parameters
  bool

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
bool QCMAP_LAN::UpdateIPAWithVLANIOCTL(bool isAdd)
{
  bool ret_val = false;

#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
        !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM) && false
  struct in_addr addr;
  pugi::xml_document xml_file;
  pugi::xml_node parent, root, subnetChild, subnetNode;
  char value[QCMAP_LAN_MAX_IPV4_ADDR_SIZE], data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  int ioctl_file_fd = -1, ioctl_ret = -1;
  ipa_ioc_bridge_vlan_mapping_info ipa_vlan_buffer;

  /*------------------------------------------------------------------------*/

  LOG_MSG_INFO1("Add config in IPACM_cfg vlan_id=%d, ipa_offload=%d",
                  this->bridge_vlan_id, this->ipa_offload_enabled, 0);

  if (this->ipa_offload_enabled == false)
  {
    LOG_MSG_ERROR("IPA offload not enabled for bridge=%d", this->bridge_vlan_id, 0,0);
    return false;
  }

  memset(&ipa_vlan_buffer, 0, sizeof(ipa_ioc_bridge_vlan_mapping_info));
  ioctl_file_fd = open(IPA_DEVICE_NAME, O_RDWR);
  if (ioctl_file_fd < 0)
  {
    LOG_MSG_ERROR("Cannot open file for ioctl",0,0,0);
    return false;
  }

  strlcpy(ipa_vlan_buffer.bridge_name, this->bridge_name, IPA_RESOURCE_NAME_MAX);
  ipa_vlan_buffer.vlan_id = this->bridge_vlan_id;
  ipa_vlan_buffer.bridge_ipv4 = this->lan_cfg.apps_ip_addr;
  ipa_vlan_buffer.subnet_mask = this->lan_cfg.sub_net_mask;

  if (isAdd)
  {
    ioctl_ret = ioctl(ioctl_file_fd, IPA_IOC_ADD_BRIDGE_VLAN_MAPPING, &ipa_vlan_buffer);
    LOG_MSG_INFO1("Calling IPA_IOC_ADD_BRIDGE_VLAN_MAPPING for bridge_name=%s, vlan_id=%d, bridge_ip4=0x%x",
                  ipa_vlan_buffer.bridge_name, ipa_vlan_buffer.vlan_id, ipa_vlan_buffer.bridge_ipv4);
  }
  else
  {
    ioctl_ret = ioctl(ioctl_file_fd, IPA_IOC_DEL_BRIDGE_VLAN_MAPPING, &ipa_vlan_buffer);
    LOG_MSG_INFO1("Calling IPA_IOC_DEL_BRIDGE_VLAN_MAPPING for bridge_name=%s, vlan_id=%d, bridge_ip4=0x%x",
              ipa_vlan_buffer.bridge_name, ipa_vlan_buffer.vlan_id, ipa_vlan_buffer.bridge_ipv4);
  }

  if (ioctl_ret)
  {
    LOG_MSG_ERROR("IOCTL to IPA failed",0,0,0);
    ioctl_ret = false;
  }
  else
  {
    ret_val = true;
  }

  close(ioctl_file_fd);
#endif

  return ret_val;
}

/*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607

/*===========================================================================
  FUNCTION UpdateLANConfigToIPACM
==========================================================================*/
/*!
@brief
  Updates Subnet ID and Subnet mask to IPACM config file.

@parameters
  none

@return
  true  -on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
bool QCMAP_LAN::UpdateLANConfigToIPACM(void) const
{
  struct in_addr addr;
  pugi::xml_document xml_file;
  pugi::xml_node parent, root, child;
  char value[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_ConnectionManager* QcMapMgr =
              QCMAP_ConnectionManager::Get_Instance(NULL, false);
  bool ret_val = false;
  /*------------------------------------------------------------------------*/
  /* Updating the IPACM XML with the Subnet IP. */
  if(!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return ret_val;
  }

  root = xml_file.child(System_TAG).child(IPACM_TAG).
            child(PrivateSubnetFlag_TAG).child(SubnetFlag_TAG);

  addr.s_addr = htonl(this->lan_cfg.sub_net_mask & this->lan_cfg.apps_ip_addr);
  strlcpy(value, inet_ntoa(addr), 16);
  root.child(SubnetAddress_TAG).text() = value;

  addr.s_addr = htonl(this->lan_cfg.sub_net_mask);
  strlcpy(value, inet_ntoa(addr), 16);
  root.child(SubnetMask_TAG).text() = value;
  ret_val = QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
  return ret_val;
}

#endif /* FEATURE_DATA_TARGET_MDM9607 */

/*=====================================================================
  FUNCTION PmipRestartLAN
======================================================================*/
/*!
@brief
  Restarts all available LAN interfaces and along with starting PMIP Deamon

@parameters
   Event -defines event when this restart is done

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
bool QCMAP_LAN::PmipRestartLAN
(
  qmi_error_type_v01 *qmi_err_num,
  pmip_event_type event
)
{
  LOG_MSG_INFO1("QCMAP_ConnectionManager::PmipRestartLAN",0,0,0);
  bool teth_link1_enable = false, teth_link2_enable = false;
  bool wlan_enable = false;
  bool result = true;
  char teth_intf_name1 [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char teth_intf_name2 [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  char teth_gw_ip1[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_gw_ip2[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type = QCMAP_MSGR_ACTIVATE_HOSTAPD_AP_ENUM_MIN_ENUM_VAL_V01;
  qcmap_msgr_activate_hostapd_action_enum_v01 action_type = QCMAP_MSGR_ACTIVATE_HOSTAPD_ACTION_ENUM_MIN_ENUM_VAL_V01;
  struct in_addr addr;
  int i, dev;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(FALSE);
  QCMAP_Backhaul* QcMapBackhaulMgr = GET_DEFAULT_BACKHAUL_OBJECT();
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;


  if(DEFAULT_BRIDGE_ID != this->bridge_vlan_id)
  {
    LOG_MSG_INFO1("Not supported with non default LAN object", 0, 0, 0);
    return false;
  }

  /*there should be some backhaul already to do this*/
  if( !QcMapBackhaulMgr)
  {
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    LOG_MSG_ERROR("QcMapBackhaulMgr in NULL",0,0,0);
    return false;
  }
  // USB related link local address
  if(QcMapTetheringMgr)
  {
    // TETH related link local address
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr);
    strlcat(teth_gw_ip1, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].gateway_addr);
    strlcat(teth_gw_ip2, inet_ntoa(addr), 16);
    addr.s_addr =
    htonl(QcMapTetheringMgr->ll_subnet_mask);
    strlcpy(teth_netmask, inet_ntoa(addr), 16);
  }

  bzero(teth_intf_name1,sizeof(teth_intf_name1));
  bzero(teth_intf_name2,sizeof(teth_intf_name2));
  // AP Configuration has changed and Bridge interface was up, need to apply the new configuration.
  // This will also take care if the mode has changed
  if (this->bridge_inited)
  {
    if(event == PMIP_DISABLE_EV)
      QcMapBackhaulMgr->stopPMIPprocess();
  // Bring down USB TE
    if (QcMapTetheringMgr)
    {
      if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
           (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type !=
                    QCMAP_QTI_TETHERED_LINK_PPP))
      {
         teth_link1_enable = true;
         if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type
                                     == QCMAP_QTI_TETHERED_LINK_RNDIS )
         {
           strlcpy(teth_intf_name1, RNDIS_IFACE,sizeof(teth_intf_name1));
         }
         else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type
                                     == QCMAP_QTI_TETHERED_LINK_ECM )
         {
           strlcpy(teth_intf_name1, ECM_IFACE,sizeof(teth_intf_name1));
         }
         else
         {
           LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
           *qmi_err_num = QMI_ERR_INTERNAL_V01;
           return false;
         }
         if (DS_TARGET_LE_CHIRON == ds_get_target())
         {
           ds_system_call("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC ",
                   strlen("echo \"\" > /sys/kernel/config/usb_gadget/g1/UDC "));
         }
         else
         {
           ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",
                         strlen("echo 0 > /sys/class/android_usb/android0/enable "));
         }
         snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",BRIDGE_IFACE,teth_intf_name1);
         ds_system_call(command, strlen(command));
      }
     else if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
               (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                    QCMAP_QTI_TETHERED_LINK_PPP))
      {
        /* Kill the PPP Daemon. Right now there is no way we can make PPP to renegotiate. */
        ds_system_call("killall -TERM pppd", strlen("killall -TERM pppd"));
      }

     if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2))
     {
         teth_link2_enable = true;
         if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
                                                                    QCMAP_QTI_TETHERED_LINK_ETH )
         {
             strlcpy(teth_intf_name2, ETH_IFACE, sizeof(teth_intf_name2));
         }
         else
         {
            LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                          QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type,0,0);
           *qmi_err_num = QMI_ERR_INTERNAL_V01;
           return false;
         }
         snprintf( command, MAX_COMMAND_STR_LEN, "ethtool -s %s autoneg off",teth_intf_name2);
         ds_system_call(command, strlen(command));
         snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",BRIDGE_IFACE,teth_intf_name2);
         ds_system_call(command, strlen(command));
     }
    }
    // Only restart the LAN interfaces (AP modes)
    if (QcMapWLANMgr &&
        ((QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_V01) ||
         (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) ||
         (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)) )
    {
      /* Stop Hostapd first so client disconnect events are generated */
      wlan_enable = true;
      /* Disable WLAN and remove wlan from bridge Iface */
      if(QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
      {
        ap_type = QCMAP_MSGR_GUEST_AP_V01;
      }
      else
      {
        ap_type = QCMAP_MSGR_PRIMARY_AP_V01;
      }
      action_type =QCMAP_MSGR_HOSTAPD_STOP_V01;
      if(!QCMAP_WLAN::ActivateHostapdConfig(ap_type, action_type, qmi_err_num))
      {
        LOG_MSG_ERROR("PmipRestartLAN: ActivateHostapdConfig-stop failed: err:%d",qmi_err_num, 0, 0);
      }
    }

    // Disable Bridge force-fully.
    if (this->bridge_inited)
    {
      LOG_MSG_INFO1(" Delete the bridge interface. forcefully \n",0,0,0);
      this->DelBridge();
    }

    this->InitBridge();

    if(event == PMIP_ENABLE_EV)
    {
      result = QcMapBackhaulMgr->startPMIPprocess();

      LOG_MSG_INFO1(" PmipRestartLAN : startPMIPprocess", 0, 0, 0);
    }

    //setting bridge to promisc mode
    snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s promisc",BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    // Enable WLAN
    if (wlan_enable)
    {
      /*ap_type is already set while disable*/
      action_type =QCMAP_MSGR_HOSTAPD_START_V01;
      if(!QCMAP_WLAN::ActivateHostapdConfig(ap_type, action_type, qmi_err_num))
      {
        LOG_MSG_ERROR("PmipRestartLAN: ActivateHostapdConfig -Start failed: err:%d",qmi_err_num, 0, 0);
      }
      else
      {
        LOG_MSG_INFO1(" PmipRestartLAN : WLAN AP enabled", 0, 0, 0);
      }
    }

    // Enable USB if it was enabled before
    if (teth_link1_enable)
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
      snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
                teth_intf_name1,teth_gw_ip1,teth_netmask);
      ds_system_call(command, strlen(command));
      snprintf( command, MAX_COMMAND_STR_LEN,
                "brctl addif %s %s",BRIDGE_IFACE,teth_intf_name1);
      ds_system_call(command, strlen(command));
    }
    if (teth_link2_enable)
    {
      snprintf( command, MAX_COMMAND_STR_LEN, "ethtool -s %s autoneg on",
                teth_intf_name2);
      ds_system_call(command, strlen(command));
      snprintf( command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
                BRIDGE_IFACE,teth_intf_name2);
      ds_system_call(command, strlen(command));
      QCMAP_LAN::ToggleAutoNegForEth(teth_intf_name2);
    }
  }
  return result;
}


/*=====================================================================
  FUNCTION SetBackhaulProfileHandle
======================================================================*/
/*!
@brief
  Sets this LAN object's backhaul profile handle, a profile handle of
  0 means this LAN object is not yet associated to a backhaul profile
  handle

@parameters
   profile_handle

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
bool QCMAP_LAN::SetBackhaulProfileHandle(profile_handle_type_v01 profile_handle)
{
  char command[MAX_COMMAND_STR_LEN] = {0};
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("OOPS!!! QcMapMgr object is NULL", 0,0,0);
    return false;
  }

  //if bridge0 or previously set the profile handle to a valid handle, then don't mutate
  if((0 == this->bridge_vlan_id) || ((0 != this->profile_handle) && (0 != profile_handle)))
  {
    LOG_MSG_INFO1("Bridge/VLAN %d already mapped to profile: %d",
                  this->bridge_vlan_id, this->profile_handle, 0);
    return false;
  }

  if((IS_DEFAULT_PROFILE(profile_handle)) && (0 != this->bridge_vlan_id))
  {
    //default profile handle case (mapping a VLAN to bridge0)
    LOG_MSG_INFO1("Mapping VLAN %d for default profile handle", this->bridge_vlan_id, 0, 0);

    this->DelBridge();
    this->auto_reboot_flag = true;

    /* Adding a vlan interface to bridge0 requires force-reboot.
     * In future, if this reboot is not required then enable below code snippet.
     */
#ifdef ALLOW_DYANAMIC_VLAN_ON_BRIDGE0
    //set bridge_name
    snprintf(this->bridge_name, sizeof(this->bridge_name), "bridge0");

    //set resolve file name
    snprintf(this->dnsmasq_resolv_file_path, sizeof(this->dnsmasq_resolv_file_path),
             DNSMASQ_RESOLV_FILE);

    //set lease file name
    snprintf(this->dnsmasq_lease_file_path, sizeof(this->dnsmasq_lease_file_path),
             DNSMASQ_LEASE_FILE);

    //set pid file name
    snprintf(this->dnsmasq_pid_file_path, sizeof(this->dnsmasq_pid_file_path),
             DNSMASQ_PID_FILE);

    //fill in host_path for this object
    snprintf(this->host_file_path,sizeof(this->host_file_path),
            QCMAP_HOST_PATH);

    //brctl addif all vlan ifaces of this LAN object to the bridge0
    if(this->vlan_iface_mask & ETH_VLAN_ENABLED)
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, ETH_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));
    }
    if(this->vlan_iface_mask & ECM_VLAN_ENABLED)
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, ECM_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));
    }
    if(this->vlan_iface_mask & RNDIS_VLAN_ENABLED)
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, RNDIS_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));
    }

    //add ULA IPv6 addr to bridge0
    memset(this->lan_cfg.ipv6_ula_addr, 0, sizeof(this->lan_cfg.ipv6_ula_addr));
    snprintf(this->lan_cfg.ipv6_ula_addr, sizeof(this->lan_cfg.ipv6_ula_addr), "%s:%x::%s",
             VLAN_BASE_IPV6_ADDR, this->bridge_vlan_id, STATIC_IID);

    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 addr add %s dev %s", this->lan_cfg.ipv6_ula_addr,
             this->bridge_name);
    ds_system_call(command, strlen(command));

    //Add IPv6 prefix based route
    memset(command, 0, sizeof(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route add %s:%x::/%d dev %s",
             VLAN_BASE_IPV6_ADDR, this->bridge_vlan_id, VLAN_IPV6_PREFIX_LEN_BITS,
             this->bridge_name);
    ds_system_call(command, strlen(command));
#endif

    this->profile_handle = profile_handle;

  }
  else
  {
    //non-default profile handle case
    LOG_MSG_INFO1("Mapping VLAN %d for profile handle %d", this->bridge_vlan_id, profile_handle, 0);

    /* Removing a vlan interface to bridge0 requires force-reboot.
     * In future, if this reboot is not required then enable below code snippet.
     */
#ifdef ALLOW_DYANAMIC_VLAN_ON_BRIDGE0
    //if previously associated with bridge0
    if(IS_DEFAULT_PROFILE(this->profile_handle))
    {
      //brctl delif all vlan ifaces of this LAN object to the bridge0
      if(this->vlan_iface_mask & ETH_VLAN_ENABLED)
      {
        memset(command, 0, sizeof(command));
        snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s.%d", this->bridge_name, ETH_IFACE,
                 this->bridge_vlan_id);
        ds_system_call(command, strlen(command));
      }
      if(this->vlan_iface_mask & ECM_VLAN_ENABLED)
      {
        memset(command, 0, sizeof(command));
        snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s.%d", this->bridge_name, ECM_IFACE,
                 this->bridge_vlan_id);
        ds_system_call(command, strlen(command));
      }
      if(this->vlan_iface_mask & RNDIS_VLAN_ENABLED)
      {
        memset(command, 0, sizeof(command));
        snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s.%d", this->bridge_name,
                 RNDIS_IFACE, this->bridge_vlan_id);
        ds_system_call(command, strlen(command));
      }

      //del IPv6 prefix based route
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del %s:%x::/%d dev %s",
               VLAN_BASE_IPV6_ADDR, this->bridge_vlan_id, VLAN_IPV6_PREFIX_LEN_BITS,
               this->bridge_name);
      ds_system_call(command, strlen(command));

      //delete ULA IPv6 addr from bridge0
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 addr del %s dev %s",
               this->lan_cfg.ipv6_ula_addr, this->bridge_name);
      ds_system_call(command, strlen(command));
    }
#endif
    if (IS_DEFAULT_PROFILE(this->profile_handle))
    {
      this->auto_reboot_flag = true;
    }

    this->profile_handle = profile_handle;

    //set resolve file name
    snprintf(this->dnsmasq_resolv_file_path, sizeof(this->dnsmasq_resolv_file_path),
             "%s%d", RESOLV_FILE_PATH_PREFIX, this->bridge_vlan_id);

    //set lease file name
    snprintf(this->dnsmasq_lease_file_path, sizeof(this->dnsmasq_lease_file_path),
             "%s%d", LEASE_FILE_PATH_PREFIX, this->bridge_vlan_id);

    //set pid file name
    snprintf(this->dnsmasq_pid_file_path, sizeof(this->dnsmasq_pid_file_path),
             "%s%d", PID_FILE_PATH_PREFIX, this->bridge_vlan_id);

    //fill in host_path for this object
    snprintf(this->host_file_path,sizeof(this->host_file_path),"%s%d",
            QCMAP_HOST_NAME_PREFIX,
            this->bridge_vlan_id);


    if(!this->bridge_inited)
    {
      this->InitBridge();
    }

    //set bridge name
    snprintf(this->bridge_name, sizeof(this->bridge_name), "bridge%d", this->bridge_vlan_id);

    //brctl addif all vlan ifaces of this LAN object to this bridge
    if(this->vlan_iface_mask & ETH_VLAN_ENABLED)
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, ETH_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));
    }
    if(this->vlan_iface_mask & ECM_VLAN_ENABLED)
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, ECM_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));
    }
    if(this->vlan_iface_mask & RNDIS_VLAN_ENABLED)
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d",
               this->bridge_name, RNDIS_IFACE, this->bridge_vlan_id);
      ds_system_call(command, strlen(command));
    }

    QcMapBackhaulWWAN = GET_BACKHAUL_WWAN_OBJECT(this->profile_handle);

    if(QcMapBackhaulWWAN)
    {
      //add IPv4 nameservers if v4 call up
      if(QCMAP_CM_WAN_CONNECTED == QcMapBackhaulWWAN->GetState())
      {
        this->AddDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv4_addr,
                                QcMapBackhaulWWAN->sec_dns_ipv4_addr);
      }

      //add IPv6 nameservers if v6 call up
      if((QCMAP_CM_V6_WAN_CONNECTED == QcMapBackhaulWWAN->GetIPv6State()) &&
         (QCMAP_MSGR_DHCPV6_MODE_UP_V01 != QcMapBackhaulWWAN->dhcpv6_dns_conf.dhcpv6_enable_state))
      {
        this->AddDNSNameServers(QcMapBackhaulWWAN->pri_dns_ipv6_addr,
                                QcMapBackhaulWWAN->sec_dns_ipv6_addr);
      }
    }
  }

  this->DeleteLANConfigFromXML(false); //erase any previous instance of this Bridge Config
  this->SetLANConfigToXML();
  if (QcMapMgr->TriggerAutoReboot(this->auto_reboot_flag, QCMAP_CM_VLAN_ADD_REMOVE_ON_BRIDGE0))
  {
    LOG_MSG_ERROR("Mapping changed for default bridge0, reboot required.",0,0,0);
  }

  LOG_MSG_INFO1("LAN object %d assigned to profile handle: %d", this->bridge_vlan_id,
                this->profile_handle, 0);

  return true;
}


/*=====================================================================
  FUNCTION GetBackhaulProfileHandle
======================================================================*/
/*!
@brief
  Gets this LAN object's backhaul profile handle, a profile handle of
  0 means this LAN object is not yet associated to a backhaul profile
  handle

@parameters

@return
  profile_handle

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
profile_handle_type_v01 QCMAP_LAN::GetBackhaulProfileHandle(void) const
{
  LOG_MSG_INFO1("Bridge%d has backhaul profile handle set as: %d", this->bridge_vlan_id,
                this->profile_handle, 0);
  return this->profile_handle;
}

/*=====================================================================
  FUNCTION GetVLANID
======================================================================*/
/*!
@brief
  Gets this LAN object's VLAN ID, a VLAN ID of 0 means this LAN object
  is associated with default bridge0

@parameters

@return
  vlan_id

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int16_t QCMAP_LAN::GetVLANID(void) const
{
  return this->bridge_vlan_id;
}

/*=====================================================================
  FUNCTION GetVLANIfaceMask
======================================================================*/
/*!
@brief
  Gets this LAN object's VLAN iface mask, determined by bitmask:

  ETH_VLAN_ENABLED   0b00000100
  ECM_VLAN_ENABLED   0b00000010
  RNDIS_VLAN_ENABLED 0b00000001

@parameters

@return
  vlan_iface_mask

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
uint8_t QCMAP_LAN::GetVLANIfaceMask(void) const
{
  LOG_MSG_INFO1("Bridge%d has vlan iface mask as: 0x%x", this->bridge_vlan_id,
                this->vlan_iface_mask, 0);
  return this->vlan_iface_mask;
}


/*=====================================================================
  FUNCTION AssociateVLANIface
======================================================================*/
/*!
@brief
  Sets this LAN object's VLAN iface mask, determined by bitmask:

  ETH_VLAN_ENABLED   0b00000100
  ECM_VLAN_ENABLED   0b00000010
  RNDIS_VLAN_ENABLED 0b00000001

  Bridges VLAN iface to this object's bridge.

@parameters
  iface_type

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
bool QCMAP_LAN::AssociateVLANIface(qcmap_msgr_interface_type_enum_v01 iface_type)
{
  char command[MAX_COMMAND_STR_LEN] = {0};

  /* First Update IPA about the vlan association. */
  if (!UpdateIPAWithVLANIOCTL(true))
  {
    LOG_MSG_ERROR("IPA to VLAN IOCTL failed!!!",0,0,0);
  }

  //set bridge_name
  if(IS_DEFAULT_PROFILE(this->profile_handle))
  {
    snprintf(this->bridge_name, sizeof(this->bridge_name), "bridge0");
  }
  else
  {
    snprintf(this->bridge_name, sizeof(this->bridge_name), "bridge%d", this->bridge_vlan_id);
  }

  switch(iface_type)
  {
    case(QCMAP_MSGR_INTERFACE_TYPE_ETH_V01):
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, ETH_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));

      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s.%d up", ETH_IFACE, this->bridge_vlan_id);
      ds_system_call(command,strlen(command));

      if(this->vlan_iface_mask & ETH_VLAN_ENABLED)
      {
        LOG_MSG_INFO1("ETH already associated with Bridge/VLAN %d", this->bridge_vlan_id, 0, 0);
        return false;
      }
      this->vlan_iface_mask |= ETH_VLAN_ENABLED;
      break;
    }
    case(QCMAP_MSGR_INTERFACE_TYPE_ECM_V01):
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, ECM_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));

      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s.%d up", ECM_IFACE, this->bridge_vlan_id);
      ds_system_call(command,strlen(command));

      if(this->vlan_iface_mask & ECM_VLAN_ENABLED)
      {
        LOG_MSG_INFO1("ECM already associated with Bridge/VLAN %d", this->bridge_vlan_id, 0, 0);
        return false;
      }
      this->vlan_iface_mask |= ECM_VLAN_ENABLED;
      break;
    }
    case(QCMAP_MSGR_INTERFACE_TYPE_RNDIS_V01):
    {
      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s.%d", this->bridge_name, RNDIS_IFACE,
               this->bridge_vlan_id);
      ds_system_call(command, strlen(command));

      memset(command, 0, sizeof(command));
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s.%d up", RNDIS_IFACE, this->bridge_vlan_id);
      ds_system_call(command,strlen(command));

      if(this->vlan_iface_mask & RNDIS_VLAN_ENABLED)
      {
        LOG_MSG_INFO1("RNDIS already associated with Bridge/VLAN %d", this->bridge_vlan_id, 0, 0);
        return false;
      }
      this->vlan_iface_mask |= RNDIS_VLAN_ENABLED;
      break;
    }
    default:
    {
      LOG_MSG_INFO1("Illegal vlan iface to associate: %d", iface_type, 0, 0);
      return false;
      break;
    }
  }
  LOG_MSG_INFO1("bridge/vlan%d vlan iface use mask: 0x%x", this->bridge_vlan_id,
                this->vlan_iface_mask, 0);
  return true;
}

/*=====================================================================
  FUNCTION DisassociateVLANIface
======================================================================*/
/*!
@brief
  Sets this LAN object's VLAN iface mask, determined by bitmask:

  ETH_VLAN_ENABLED   0b00000100
  ECM_VLAN_ENABLED   0b00000010
  RNDIS_VLAN_ENABLED 0b00000001

  Explicit unbridging VLAN iface from this object's bridge is unnecessary
  when removing a VLAN iface.

@parameters
  iface_type

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
bool QCMAP_LAN::DisassociateVLANIface(qcmap_msgr_interface_type_enum_v01 iface_type)
{
  switch(iface_type)
  {
    case(QCMAP_MSGR_INTERFACE_TYPE_ETH_V01):
    {
      this->vlan_iface_mask &= ~(ETH_VLAN_ENABLED);
      break;
    }
    case(QCMAP_MSGR_INTERFACE_TYPE_ECM_V01):
    {
      this->vlan_iface_mask &= ~(ECM_VLAN_ENABLED);
      break;
    }
    case(QCMAP_MSGR_INTERFACE_TYPE_RNDIS_V01):
    {
      this->vlan_iface_mask &= ~(RNDIS_VLAN_ENABLED);
      break;
    }
    default:
    {
      LOG_MSG_ERROR("Illegal vlan iface to disassociate: %d", iface_type, 0, 0);
      return false;
      break;
    }
  }
  LOG_MSG_INFO1("bridge%d vlan iface use mask: 0x%x", this->bridge_vlan_id,
                this->vlan_iface_mask, 0);
  return true;
}


/*=====================================================================
  FUNCTION SetIPAoffload
======================================================================*/
/*!
@brief
  Sets ipa_offload flag for LAN Object

@parameters
  ipa_ofload

@return
  none

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_LAN::SetIPAoffload(bool ipa_offload)
{
  if (ipa_offload)
    this->ipa_offload_enabled = true;
}


/*=====================================================================
  FUNCTION GetBridgeAsCStr
======================================================================*/
/*!
@brief
  Get this LAN object's bridge name as a C style string

@parameters

@return
  bridge_name

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
const char* QCMAP_LAN::GetBridgeNameAsCStr(void) const
{
  LOG_MSG_INFO1("LAN object with Bridge/VLAN ID %d has bridge name: %s", this->bridge_vlan_id,
                this->bridge_name, 0);

  return (const char*)(this->bridge_name);
}

/*=====================================================================
  FUNCTION GetBridgeMacAddr
======================================================================*/
/*!
@brief
  Get this LAN object's bridge MAC addr

@parameters

@return
  struct ether_addr bridge_local_mac_addr

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
struct ether_addr QCMAP_LAN::GetBridgeMacAddr(void) const
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  return this->bridge_local_mac_addr;
}

/*=====================================================================
  FUNCTION GetBridgesockFd
======================================================================*/
/*!
@brief
  Get this LAN object's bridge sock fd

@parameters

@return
  int bridge_sock

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int QCMAP_LAN::GetBridgeSockFd(void) const
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  return this->bridge_sock;
}
