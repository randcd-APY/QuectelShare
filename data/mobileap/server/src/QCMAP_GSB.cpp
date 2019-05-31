/*====================================================

FILE:  QCMAP_GSB.cpp

SERVICES:
   QCMAP GSB Specific Implementation

=====================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  06/01/17   gs         Created

======================================================*/
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds_string.h"
#include "ds_util.h"
#include "QCMAP_GSB.h"
#include "QCMAP_ConnectionManager.h"
#include "qcmap_cm_api.h"
#include "QCMAP_WLAN.h"
#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM)
#include <linux/msm_ipa.h>
#endif

bool QCMAP_GSB::flag = false;
QCMAP_GSB* QCMAP_GSB::object=NULL;
bool QCMAP_GSB::GSBEnableFlag = false;
timer_t QCMAP_GSB::timerid = QCMAP_CM_ERROR;

/*=============================================================*/
/* Extern definitions                                          */
/*=============================================================*/
extern struct sigevent sev;


/*=====================================================
                             Helper functions
  =====================================================*/


  /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607

boolean UpdateIPACMcfg(char* iface_name, char* tag)
{
  pugi::xml_document xml_file;
  pugi::xml_node parent, root, child;
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if(!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPACMIface_TAG);
  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strncmp(child.child(Name_TAG).child_value(),
                     iface_name, strlen(iface_name)))
    {
      child.child(Category_TAG).text() = tag;
      if (QcMapMgr) {
        QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
        LOG_MSG_INFO1("IPACM cfg file changed successfully for %s", iface_name, 0, 0);
      }
      return true;
    }
  }

  LOG_MSG_ERROR( "Failed to find iface %s",iface_name,0,0 );
  return false;
}

#endif /* FEATURE_DATA_TARGET_MDM9607 */

void SetGSBUnloadFlag()
{
  char command[MAX_COMMAND_STR_LEN];
  snprintf(command,MAX_COMMAND_STR_LEN,
            "data_path_opt -mGSB -o%d", (int)MSG_TYPE_UNLOAD);
  ds_system_call(command,strlen(command));
}

boolean SetGSBBootUpConfig(boolean flag)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  pugi::xml_document xml_file;
  pugi::xml_node root, entry_node;
  char data[MAX_STRING_LENGTH] = { 0 };
  char command[MAX_COMMAND_STR_LEN];

  if (!QcMapMgr)
  {
    return false;
  }

  if (!xml_file.load_file(QCMAP_DEFAULT_CONFIG))
  {
    LOG_MSG_ERROR("error loading XML file",0,0,0);
    return false;
  }

  LOG_MSG_INFO1( "Setting GSB bootup config=%d",flag, 0,0 );
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(GSB_CONFIG_TAG);
  entry_node = root.child(GSB_BOOTUP_CFG_TAG);

  snprintf(data, sizeof(data), "%d", flag);
  entry_node.first_child().set_value(data);

  xml_file.save_file(QCMAP_DEFAULT_CONFIG_TEMP);
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",QCMAP_DEFAULT_CONFIG_TEMP);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",QCMAP_DEFAULT_CONFIG_TEMP,
                                          QCMAP_DEFAULT_CONFIG);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",QCMAP_DEFAULT_CONFIG_TEMP);
  ds_system_call(command, strlen(command));
  return true;
}

uint8 GetGSBEntryCountFromXML(void)
{
  uint8 entries = 0;
  pugi::xml_document xml_file;


  if (!xml_file.load_file(QCMAP_DEFAULT_CONFIG))
  {
    LOG_MSG_ERROR("error loading XML file",0,0,0);
    return 0;
  }

  entries = atoi(xml_file.child(System_TAG).child(MobileAPCfg_TAG).
                 child(GSB_CONFIG_TAG).child(GSB_ENTRIES_TAG).first_child().value());
  LOG_MSG_INFO1("entries %d", entries,0,0);

  return entries;
}

boolean GetGSBCustomConfig(qcmap_msgr_gsb_config_v01 *conf)
{
  uint8 i = 0;
  pugi::xml_node root, child;
  pugi::xml_node conf_root, cust_root,cust_child;
  pugi::xml_document xml_file;


  if (!xml_file.load_file(QCMAP_DEFAULT_CONFIG))
  {
    LOG_MSG_ERROR("error loading XML file",0,0,0);
    return 0;
  }

  if (root)
  {
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (strncmp(child.name(), GSB_CONFIG_TAG,strlen(child.name())) == 0 )
      {
        conf_root = child;
        for (child = conf_root.first_child(); child; child = child.next_sibling())
        {
          if (strncmp(child.name(), GSB_CUSTOM_TAG,strlen(child.name())) == 0 )
          {
            cust_root = child;
            for (cust_child = cust_root.first_child(); cust_child;
                   cust_child = cust_child.next_sibling())
            {
              if (!strncmp(cust_child.name(), GSB_BW_REQD_TAG,strlen(cust_child.name())))
              {
                LOG_MSG_INFO1("bw reqd %d", atoi(cust_child.first_child().value()),0,0);
                conf->bw_reqd_in_mb = atoi(cust_child.first_child().value());
                i++;
              }

              if (!strncmp(cust_child.name(), GSB_IF_HIGH_WM_TAG,strlen(cust_child.name())))
              {
                LOG_MSG_INFO1("high wm %d", atoi(cust_child.first_child().value()),0,0);
                conf->if_high_watermark = atoi(cust_child.first_child().value());
                i++;
              }

              if (!strncmp(cust_child.name(), GSB_IF_LOW_WM_TAG, strlen(cust_child.name())))
              {
                LOG_MSG_INFO1("low wm %d", atoi(cust_child.first_child().value()),0,0);
                conf->if_low_watermark = atoi(cust_child.first_child().value());
                i++;
              }

              if (i == 3)
              {
                /* we except max 3 entries */
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}

uint8 GetGSBConfigFromXML(qcmap_msgr_gsb_config_v01 *conf)
{
  uint8 entries = 0;
  uint8 i = 0;
  pugi::xml_node root, child;
  pugi::xml_node conf_root, conf_child;
  pugi::xml_node if_root;
  pugi::xml_document xml_file;
  struct in_addr ip_addr;
  memset(&ip_addr, 0 , sizeof(ip_addr));

  if (!xml_file.load_file(QCMAP_DEFAULT_CONFIG))
  {
    LOG_MSG_ERROR("error loading XML file",0,0,0);
    return 0;
  }

  root = xml_file.child("system").first_child();

  if (root)
  {
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (strncmp(child.name(), GSB_CONFIG_TAG,strlen(child.name())) == 0 )
      {
        conf_root = child;

        for (child = conf_root.first_child(); child; child = child.next_sibling())
        {
          if (strncmp(child.name(), GSB_ENTRIES_TAG,strlen(child.name())) == 0 )
          {
            LOG_MSG_INFO1("entries %d", atoi(child.first_child().value()),0,0);
            entries = atoi(child.first_child().value());
            if (entries == 0) break;
          }
          if (strncmp(child.name(), GSB_INTERFACE_ENTRY_TAG,strlen(child.name())) == 0 )
          {
            if_root = child;
            for (conf_child = if_root.first_child(); conf_child; conf_child = conf_child.next_sibling())
            {
              if (!strncmp(conf_child.name(), GSB_INTERFACE_NAME_TAG,strlen(conf_child.name())))
              {
                LOG_MSG_INFO1("if_name: %s", conf_child.first_child().value(),0,0);
                strlcpy(conf[i].if_name, conf_child.first_child().value(),
                                                  QCMAP_MAX_IFACE_NAME_SIZE_V01);
              }

              if (!strncmp(conf_child.name(), GSB_INTERFACE_TYPE_TAG,strlen(conf_child.name())))
              {
                conf[i].if_type = (qcmap_msgr_gsb_interface_type_enum_v01)atoi(conf_child.first_child().value());
              }


              if (!strncmp(conf_child.name(), GSB_BW_REQD_TAG,strlen(conf_child.name())))
              {
                conf[i].bw_reqd_in_mb = atoi(conf_child.first_child().value());
              }


              if (!strncmp(conf_child.name(), GSB_IF_HIGH_WM_TAG,strlen(conf_child.name())))
              {
                conf[i].if_high_watermark = atoi(conf_child.first_child().value());
              }


              if (!strncmp(conf_child.name(), GSB_IF_LOW_WM_TAG, strlen(conf_child.name())))
              {
                conf[i].if_low_watermark = atoi(conf_child.first_child().value());
              }

              if (!strncmp(conf_child.name(), GSB_IF_AP_IP, strlen(conf_child.name())))
              {
                 if (inet_aton(conf_child.first_child().value(), &ip_addr))
                 {
                   conf[i].ap_ip =  ntohl(ip_addr.s_addr);
                 }
              }
            }
            i++;
          }
        }
      }
    }
  }

  if (i != entries)
  {
    LOG_MSG_ERROR("config is not correct",0,0,0);
  }
  return entries;
}

int IsDuplicateEntry(char * iface_name)
{
  uint8 entries = 0;
  pugi::xml_node root, child;
  pugi::xml_node conf_child;
  pugi::xml_node if_root;
  pugi::xml_document xml_file;

  if (iface_name == "" || iface_name == NULL)
  {
    LOG_MSG_ERROR("NULL ifname passed", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  if (!xml_file.load_file(QCMAP_DEFAULT_CONFIG))
  {
    LOG_MSG_INFO1("error loading XML file",0,0,0);
    return QCMAP_CM_ERROR;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(GSB_CONFIG_TAG);

  if (root)
  {
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strncmp(child.name(), GSB_ENTRIES_TAG, strlen(child.name())))
      {
        entries = atoi(child.first_child().value());
        if (entries == 0)
        {
          LOG_MSG_ERROR("No entry present",0,0,0);
          return false;
        }
      }

      if (!strncmp(child.name(), GSB_INTERFACE_ENTRY_TAG,strlen(child.name())))
      {
        if_root = child;

        for (conf_child = if_root.first_child(); conf_child; conf_child = conf_child.next_sibling())
        {
          if (!strncmp(conf_child.name(), GSB_INTERFACE_NAME_TAG,strlen(conf_child.name())))
          {
            if ((strncmp(conf_child.first_child().value(), iface_name,
                                    strlen(conf_child.first_child().value())) == 0))
            {
              LOG_MSG_INFO1("duplicate entry for if_name %s found, requested: %s",
                                    conf_child.first_child().value(),iface_name,0);
              return true;
            }
          }
        }
      }
    }
  }
  else
  {
    LOG_MSG_ERROR("Error in obtaining GSB node in cfg file", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  return QCMAP_CM_ENOERROR;
}

boolean SetGSBConfigToXML(qcmap_msgr_gsb_config_v01 *conf)
{
  uint8 curr_entries = 0;
  uint8 new_entry = 0;
  struct in_addr addr;
  uint8 i = 0;
  pugi::xml_node root, child, subchild;
  pugi::xml_document xml_file;
  char data[MAX_STRING_LENGTH] = { 0 };
  char command[MAX_COMMAND_STR_LEN];


  if (!xml_file.load_file(QCMAP_DEFAULT_CONFIG))
  {
    LOG_MSG_INFO1("error loading XML file",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(GSB_CONFIG_TAG);

  if (root)
  {
    curr_entries = atoi(root.child(GSB_ENTRIES_TAG).first_child().value());
    LOG_MSG_INFO1("curr entries %d", curr_entries,0,0);
  }

  child = root.append_child(GSB_INTERFACE_ENTRY_TAG);

  subchild = child.append_child(GSB_INTERFACE_NAME_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(conf->if_name);

  snprintf(data, sizeof(data), "%d", conf->if_type);
  subchild = child.append_child(GSB_INTERFACE_TYPE_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  snprintf(data, sizeof(data), "%d", conf->bw_reqd_in_mb);
  subchild = child.append_child(GSB_BW_REQD_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  snprintf(data, sizeof(data), "%d", conf->if_high_watermark);
  subchild = child.append_child(GSB_IF_HIGH_WM_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  snprintf(data, sizeof(data), "%d", conf->if_low_watermark);
  subchild = child.append_child(GSB_IF_LOW_WM_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  if (conf->ap_ip != 0)
  {
    memset(&addr, 0, sizeof(addr));
    subchild = child.append_child(GSB_IF_AP_IP);
    addr.s_addr = htonl(conf->ap_ip);
    subchild.text() = inet_ntoa(addr);
  }
  else
  {
    memset(&addr, 0, sizeof(addr));
    subchild = child.append_child(GSB_IF_AP_IP);
    subchild.text() = inet_ntoa(addr);
  }

  curr_entries++;
  snprintf(data, sizeof(data), "%d", curr_entries);
  root.child(GSB_ENTRIES_TAG).first_child().set_value(data);

  new_entry = atoi(root.child(GSB_ENTRIES_TAG).first_child().value());
  LOG_MSG_INFO1("new entriess %d", new_entry,0,0);


  xml_file.save_file(QCMAP_DEFAULT_CONFIG_TEMP);
  snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",QCMAP_DEFAULT_CONFIG_TEMP);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",QCMAP_DEFAULT_CONFIG_TEMP,
                                          QCMAP_DEFAULT_CONFIG);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",QCMAP_DEFAULT_CONFIG_TEMP);
  ds_system_call(command, strlen(command));

  return true;
}

boolean RemoveGSBConfigFromXML(char * iface_name)
{
  uint8 entries = 0;
  uint8 i = 0;
  pugi::xml_node root, child;
  pugi::xml_node conf_root, conf_child, itr_child;
  pugi::xml_node if_root;
  pugi::xml_document xml_file;
  boolean if_found = false;
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_STRING_LENGTH] = { 0 };


  if (iface_name == "" || iface_name == NULL)
  {
    return false;
  }

  if (!xml_file.load_file(QCMAP_DEFAULT_CONFIG))
  {
    LOG_MSG_ERROR("error loading XML file",0,0,0);
    return false;
  }

  root = xml_file.child("system").first_child();

  if (root)
  {
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strncmp(child.name(), GSB_CONFIG_TAG,strlen(child.name())))
      {
        conf_root = child;

        for (child = conf_root.first_child(); child; child = child.next_sibling())
        {
          if (!strncmp(child.name(), GSB_ENTRIES_TAG, strlen(child.name())))
          {
            entries = atoi(child.first_child().value());
            if (entries == 0)
            {
              LOG_MSG_ERROR("No entry to delete",0,0,0);
              return false;
            }
            else
            {
              LOG_MSG_INFO1("Total entries available %d",entries,0,0);
            }
          }

          if (!strncmp(child.name(), GSB_INTERFACE_ENTRY_TAG,strlen(child.name())))
          {
            if_root = child;
            for (conf_child = if_root.first_child(); conf_child;
                   conf_child = conf_child.next_sibling())
            {
              if (!strncmp(conf_child.name(), GSB_INTERFACE_NAME_TAG,strlen(conf_child.name())))
              {
                if ((strncmp(conf_child.first_child().value(), iface_name,
                                        strlen(conf_child.first_child().value())) == 0))
                {
                  /* update IPACM cfg to set default category for the provided IF type*/
                  for (itr_child = if_root.first_child(); itr_child;
                         itr_child = itr_child.next_sibling())
                  {
                    if (!strncmp(itr_child.name(), GSB_INTERFACE_TYPE_TAG,strlen(itr_child.name())))
                    {
                      LOG_MSG_INFO1("if type %d", atoi(itr_child.first_child().value()),0,0);
                      if ((atoi(itr_child.first_child().value())
                            == QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01) ||
                          (atoi(itr_child.first_child().value())
                            == QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01)  )
                      {
                       /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
                        if (!UpdateIPACMcfg(iface_name, IPACM_DEFAULT_TAG))
                        {
                          LOG_MSG_ERROR("Error setting config to IPA cfg XML for iface %s",
                                          iface_name,0,0);
                          return false;
                        }
#endif /* FEATURE_DATA_TARGET_MDM9607 */
                       }
                    }
                  }

                  LOG_MSG_INFO1("removing if_name %s, requested: %s",
                                        conf_child.first_child().value(),iface_name,0);
                  if_found = true;
                  if_root.parent().remove_child(if_root);
                }
              }
            }
          }
        }
      }
    }
  }

  if (if_found)
  {
    entries--;
    LOG_MSG_INFO1("Node with %s name is removed successfully, entry %d",iface_name,entries,0);
    snprintf(data, sizeof(data), "%d", entries);
    xml_file.child(System_TAG).child(MobileAPCfg_TAG).
    child(GSB_CONFIG_TAG).child(GSB_ENTRIES_TAG).first_child().set_value(data);
    xml_file.save_file(QCMAP_DEFAULT_CONFIG_TEMP);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s",QCMAP_DEFAULT_CONFIG_TEMP);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",QCMAP_DEFAULT_CONFIG_TEMP,
                                            QCMAP_DEFAULT_CONFIG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",QCMAP_DEFAULT_CONFIG_TEMP);
    ds_system_call(command, strlen(command));
    return true;
  }
  else
  {
    LOG_MSG_ERROR("Node with %s name is not found",iface_name,0,0);
    return false;
  }
}

int isInterfaceUP(char* if_name)
{
  char buf[MAX_COMMAND_STR_LEN] = {0};
  char command[MAX_COMMAND_STR_LEN]  = {0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *stream = NULL;


  snprintf( command, MAX_COMMAND_STR_LEN,NET_DEV_FILE_ROOT_PATH);
  snprintf(tmp, MAX_COMMAND_STR_LEN, "/%s/operstate", if_name);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);

  LOG_MSG_INFO1("file path %s", command,0,0);

  stream = fopen(command, "r");
  if(NULL == stream)
  {
    LOG_MSG_ERROR("Failed to open if file",0,0,0);
    return QCMAP_CM_ERROR;
  }

  fread(buf, sizeof(char), MAX_COMMAND_STR_LEN, stream);
  fclose(stream);

  if ((strncmp(buf, IF_STATE_UP, strlen(IF_STATE_UP)) == 0))
  {
    LOG_MSG_INFO1("if %s state is up, buf in %s state",if_name,buf,0);
    return IF_STATUS_UP;
  }
  else
  {
    LOG_MSG_INFO1("if %s state is down, buf in %s state",if_name,buf,0);
    return IF_STATUS_DOWN;
  }
}

void ChangeIFState(char* if_name, char* state)
{
  char command[MAX_COMMAND_STR_LEN];
  snprintf( command, MAX_COMMAND_STR_LEN,"ifconfig %s %s", if_name, state);
  ds_system_call( command, strlen(command));
}

int SendMSGToGSB(qcmap_msgr_gsb_config_v01 *conf, int code)
{
  int ret = QCMAP_CM_ERROR;
  char command[MAX_COMMAND_STR_LEN];
  const qcmap_cm_lan_conf_t* lan_cfg = NULL;
  QCMAP_LAN* QCMAPLANMgr=GET_DEFAULT_LAN_OBJECT();
  struct in_addr addr;
  memset(&addr, 0, sizeof(addr));

  if(QCMAPLANMgr == NULL )
  {
    LOG_MSG_ERROR("NULL obj found", 0, 0, 0);
    return ret;
  }

  if((lan_cfg = QCMAPLANMgr->GetLANConfig()) == NULL)
  {
    LOG_MSG_ERROR("NULL obj found", 0, 0, 0);
    return ret;
  }

  if (conf->ap_ip == 0)
  {
    conf->ap_ip = htonl(lan_cfg->apps_ip_addr);
    addr.s_addr = conf->ap_ip;
    LOG_MSG_INFO1("updating with QCMAP lan value 0x%X, %s",conf->ap_ip, inet_ntoa(addr), 0);
  }
  else
  {
    conf->ap_ip = htonl(conf->ap_ip);
    addr.s_addr = conf->ap_ip;
    LOG_MSG_INFO1("updating with configured value 0x%X, %s",conf->ap_ip, inet_ntoa(addr), 0);
  }

  snprintf(command,MAX_COMMAND_STR_LEN,
           "data_path_opt -mGSB -i%s -b%d -h%d -w%d -t%d -a%d -o%d",
           conf->if_name,
           conf->bw_reqd_in_mb,
           conf->if_high_watermark,
           conf->if_low_watermark,
           conf->if_type,
           conf->ap_ip,
           code);
  ds_system_call(command,strlen(command));
  return QCMAP_CM_SUCCESS;
}

/*=====================================================================
  FUNCTION GSBBindHandler
======================================================================*/
/*!
@brief
  Binds appropriate IF to GSB

@parameters
  signal info

@return
  none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
static void GSBBindHandler(int sig, siginfo_t *si, void *uc)
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  if (QcMapWLANMgr == NULL)
  {
    LOG_MSG_ERROR("Null WLAN obj", 0, 0, 0);
    return;
  }

  /*Bind IF to GSB*/
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

/*=====================================================================
  FUNCTION destroyGSBTimer
======================================================================*/
/*!
@brief
  destroys the timer created for GSB binding

@parameters
  timer id

@return
  none
@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void destroyGSBTimer(timer_t timerid)
{
  if (timer_delete(timerid) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("delete timer FAILED", 0, 0, 0);
    return;
  }
  LOG_MSG_INFO1("Timer deleted!", 0, 0, 0);
}

/*=====================================================================
  FUNCTION destroyGSBTimer
======================================================================*/
/*!
@brief
  Creates the timer for GSB binding

@parameters
  none

@return
  timer id
@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
timer_t createGSBTimer()
{
  timer_t timerid;

  /* Create the timer */

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_ptr = &timerid;
  if (timer_create(CLOCKID, &sev, &timerid) == QCMAP_CM_ERROR)
  {
    LOG_MSG_ERROR("timer_create FAILED", 0, 0, 0);
    return QCMAP_CM_ERROR;
  }

  LOG_MSG_INFO1("GSB Timer established sucessfully id 0x%X", timerid, 0, 0);
  return timerid;
}

/*=====================================================================
  FUNCTION setGSBTimer
======================================================================*/
/*!
@brief
  Sets gsb timer

@parameters
  time in sec
  timer id

@return
  none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void setGSBTimer(int time_in_sec, timer_t timerid)
{
  struct itimerspec its;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec =0;
  its.it_value.tv_sec = time_in_sec;

  if (timerid != QCMAP_CM_ERROR)
  {
    if (timer_settime(timerid, 0, &its, NULL) == QCMAP_CM_ERROR)
    {
      LOG_MSG_ERROR("set Timer FAILED", 0, 0, 0);
      return;
    }
  }
  else
  {
    LOG_MSG_ERROR("Cannot set timer for GSB ", 0, 0, 0);
    return;
  }
  LOG_MSG_INFO1("setting timer id 0x%X with %d sec ", timerid, time_in_sec, 0);
}

/*=====================================================
  FUNCTION SetGSBConfigforWLAN
======================================================*/
/*!
@brief
 This is a utility function set GSB config for supported WLAN card

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
void SetGSBConfigforWLAN(int if_num,
                                qcmap_msgr_wlan_iface_active_state_enum_v01 state,
                                qcmap_msgr_wlan_iface_index_enum_v01 ap_type,
                                qcmap_msgr_wlan_device_type_v01 dev_type)
{
  qcmap_msgr_gsb_config_v01 config[GSB_MAX_IF_SUPPORT];
  uint8 entries = 0;
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
  int gsb_conf_if_num = 0;
  boolean do_not_config = false;
  char if_name[QCMAP_MAX_IFACE_NAME_SIZE_V01] = {0};
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  memset(config, 0, sizeof(qcmap_msgr_gsb_config_v01)*GSB_MAX_IF_SUPPORT);

  if (QcMapWLANMgr == NULL)
  {
    LOG_MSG_ERROR("WLAN Obj is null",0,0,0);
  }

  /*we do not want to check anything with GSB if WIFI mode is AP mode
  as AP mode will always come on Rome. This will help ith KPI*/
  if (QcMapWLANMgr && (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_V01))
  {
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
      for (int i = 0; i < entries; i++)
      {
        if ((QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01 == config[i].if_type) ||
              (QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01 == config[i].if_type))
        {
          gsb_conf_if_num = atoi(config[i].if_name+WLAN_IF_NUM_OFFSET);
          if (gsb_conf_if_num == if_num)
          {
            if ((dev_type == QCMAP_MSGR_WLAN_DEV_ROME_V01) &&
                    (state == QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01))
            {
              LOG_MSG_INFO1("IF %d is activating on Rome", gsb_conf_if_num, 0, 0);
              /*remove config from GSB. In this case type does not matter. Can be set ot STA or AP*/
              if (QCMAP_GSB::SetCustomGSBDataPath(config[i].if_name,
                                                  QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01,
                                                  false))
              {
                LOG_MSG_INFO1("Delete GSB config succeeded.\n",0,0,0);
              }
              else
              {
                LOG_MSG_ERROR("Delete GSB config failed!!",0,0,0);
              }
            }

            if ((dev_type == QCMAP_MSGR_WLAN_DEV_INVALID_V01) &&
                  (state == QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01))
            {
              LOG_MSG_INFO1("IF name %s is deactivating on unknown card", if_name, 0, 0);
              if (QCMAP_GSB::SetCustomGSBDataPath(config[i].if_name,
                                                  QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01,
                                                  false))
              {
                LOG_MSG_INFO1("Delete GSB config succeeded.\n",0,0,0);
              }
              else
              {
                LOG_MSG_ERROR("Delete GSB config failed!!",0,0,0);
              }
            }

            if ((dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01) &&
                  (state == QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01))
            {
              LOG_MSG_INFO1("IF %s is activating on Tuf, already present", if_name, 0, 0);
              if (!((ap_type == QCMAP_MSGR_WLAN_IFACE_STATION_V01) && QcMapWLANMgr &&
                   (QcMapWLANMgr->last_STA_GSB_IF == if_num) && QcMapWLANMgr->is_STA_GSB_conf))
              {
                if (QCMAP_GSB::SetCustomGSBDataPath(config[i].if_name,
                                                    QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01,
                                                    false))
                {
                  LOG_MSG_INFO1("Delete GSB config succeeded.\n",0,0,0);
                }
                else
                {
                  LOG_MSG_ERROR("Delete GSB config failed!!",0,0,0);
                }
              }
              else
              {
                LOG_MSG_INFO1("STA %s was activating, bind avail", config[i].if_name, 0, 0);
                do_not_config = true;
              }
            }

            if ((dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01) &&
                  (state == QCMAP_MSGR_WLAN_IFACE_INACTIVE_V01))
            {
              LOG_MSG_INFO1("IF wlan%d is stopping on Tuf", if_num, 0, 0);
              if (!((ap_type == QCMAP_MSGR_WLAN_IFACE_STATION_V01) && QcMapWLANMgr &&
                   (QcMapWLANMgr->last_STA_GSB_IF == if_num) && QcMapWLANMgr->is_STA_GSB_conf))
              {
                snprintf(if_name, QCMAP_MAX_IFACE_NAME_SIZE_V01,"wlan%d", if_num);
                LOG_MSG_INFO1("IF name %s is deactivating on Tuf", if_name, 0, 0);
                if (QCMAP_GSB::SetCustomGSBDataPath(if_name,
                                                    QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01,
                                                    false))
                {
                  LOG_MSG_INFO1("remove GSB config succeeded",0,0,0);
                }
                else
                {
                  LOG_MSG_ERROR("remove GSB config failed!!",0,0,0);
                }

                if (QcMapWLANMgr && (ap_type == QCMAP_MSGR_WLAN_IFACE_STATION_V01))
                {
                  LOG_MSG_INFO1("STA %d is stopping on Tuf", if_num, 0, 0);
                  QcMapWLANMgr->is_STA_GSB_conf = false;
                  QcMapWLANMgr->last_STA_GSB_IF = -1;
                }
              }
            }
          }
        }
      }
    }

    if (!do_not_config && (dev_type == QCMAP_MSGR_WLAN_DEV_TUF_V01)
                            && (state == QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01))
    {
      snprintf(if_name, QCMAP_MAX_IFACE_NAME_SIZE_V01,"wlan%d", if_num);
      LOG_MSG_INFO1("IF name %s is activating on Tuf", if_name, 0, 0);
      /*For AP+STA bridge mode we want ODU category for STA because
              STA is also bridged*/
      if (ap_type != QCMAP_MSGR_WLAN_IFACE_STATION_V01)
      {
        if (QCMAP_GSB::SetCustomGSBDataPath(if_name,
                                            QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01,
                                            true))
        {
          LOG_MSG_INFO1("Add GSB config succeeded",0,0,0);
        }
        else
        {
          LOG_MSG_ERROR("Add GSB config failed!!",0,0,0);
        }
      }
      else
      {
        if (QCMAP_GSB::SetCustomGSBDataPath(if_name,
                                            QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01,
                                            true))
        {
          LOG_MSG_INFO1("Add GSB config succeeded",0,0,0);
        }
        else
        {
          LOG_MSG_ERROR("Add GSB config failed!!",0,0,0);
        }
      }
    }
    else
    {
      if (do_not_config)
      {
        LOG_MSG_INFO1("STA IF wlan%d was activating, bind avail",if_num,0,0);
      }
    }

    if ((dev_type == QCMAP_MSGR_WLAN_DEV_ROME_V01) &&
         (state == QCMAP_MSGR_WLAN_IFACE_ACTIVE_V01))
    {
      LOG_MSG_INFO1("IF %d is activating on Rome, not updating GSB", if_num, 0, 0);
      if ((ap_type == QCMAP_MSGR_WLAN_IFACE_STATION_V01) &&
           (QcMapWLANMgr->last_STA_GSB_IF != -1))
      {
        snprintf(if_name, QCMAP_MAX_IFACE_NAME_SIZE_V01,"wlan%d",
                 QcMapWLANMgr->last_STA_GSB_IF);
        LOG_MSG_INFO1("invalidate last STA IF wlan%d", if_name, 0, 0);
        if (QCMAP_GSB::SetCustomGSBDataPath(if_name,
                                            QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01,
                                            false))
        {
          LOG_MSG_INFO1("remove GSB config succeeded",0,0,0);
        }
        else
        {
          LOG_MSG_ERROR("remove GSB config failed!!",0,0,0);
        }
      }
    }
  }
}

/*=====================================================
              Class Implementation
  =====================================================*/
/*=====================================================
  FUNCTION Constructor
======================================================*/
/*!
@brief
  Initializes GSB OBject.

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
/*====================================================*/
QCMAP_GSB::QCMAP_GSB()
{

}

/*======================================================
  FUNCTION Destructor
======================================================*/
/*!
@brief
  Destroyes the GSB Object.

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


QCMAP_GSB::~QCMAP_GSB()
{
  QCMAP_GSB* QcMapGSBObj = QCMAP_GSB::Get_Instance(false);
  flag=false;
  if (QcMapGSBObj != NULL)
  {
    delete QcMapGSBObj;
  }
  object = NULL;
}


/*=====================================================
  FUNCTION Get_Instance
======================================================*/
/*!
@brief
  Gets and return instance of class QCMAP_GSB

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

QCMAP_GSB* QCMAP_GSB::Get_Instance(boolean obj_create)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : GSB",0, 0, 0);
    object = new QCMAP_GSB();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}

/*==========================================================
 FUNCTION    LoadGSB
===========================================================*/
/*!
@brief
  Loads GSB module

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
boolean QCMAP_GSB::LoadGSB
(
  qmi_error_type_v01 *qmi_err_num
)
{
  if (QCMAP_GSB::GSBEnableFlag)
  {
    LOG_MSG_ERROR("GSB already enabled", 0, 0, 0);
    /* this does not stop the config process so  not an error*/
    *qmi_err_num = QMI_ERR_NONE_V01;
    return false;
  }
  ds_system_call("data_path_opt -lGSB",strlen("data_path_opt -lGSB"));

  /*set enable flag for gsb*/
  QCMAP_GSB::GSBEnableFlag = true;

  return true;
}

/*==========================================================
 FUNCTION   UnLoadGSB
===========================================================*/
/*!
@brief
  UnLoads GSB module

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
boolean QCMAP_GSB::UnLoadGSB
(
  qmi_error_type_v01 *qmi_err_num
)
{
  if (!QCMAP_GSB::GSBEnableFlag)
  {
    LOG_MSG_ERROR("GSB is not enabled", 0, 0, 0);
    /* this does not stop the config process so  not an error*/
    *qmi_err_num = QMI_ERR_NONE_V01;
    return false;
  }
  ds_system_call("data_path_opt -uGSB",strlen("data_path_opt -uGSB"));

  /*set enable flag for gsb*/
  QCMAP_GSB::GSBEnableFlag = false;

  return true;
}

/*==========================================================
 FUNCTION    ConfigureGSB
===========================================================*/
/*!
@brief
  Sends the configuration to GSB module

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
boolean QCMAP_GSB::ConfigureGSB
(
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  qcmap_msgr_gsb_config_v01 config[QCMAP_MSGR_MAX_IF_SUPPORTED_V01];
  uint8 entries = 0;
  int ret = 0;
  boolean if_stopped = false;


  //read the configuration
  entries = GetGSBConfigFromXML(config);
  //if no configuration entry found then return
  if (!entries)
  {
    LOG_MSG_ERROR("No configuration found",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  /*if there is even a single config then load GSB*/
  QCMAP_GSB::LoadGSB(qmi_err_num);
  usleep(QCMAP_GSB_DELAY_COUNT * 50);

  for (int j = 0 ; j < entries; j++)
  {
    if (config[j].if_type == QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01 ||
        config[j].if_type == QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01)
    {
      continue;
    }
    if_stopped = false;
    //Pass the configuration to GSB
    if(SendMSGToGSB(&config[j], MSG_TYPE_ADD)<0)
    {
      LOG_MSG_ERROR("Failed to send msg to GSB", 0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    usleep(QCMAP_GSB_DELAY_COUNT * 50);

    /*stop all the  interfaces who are configured to work with GSB
     if interfaces are not up then  skip*/
    if (!strncmp(config[j].if_name,ETH_IFACE,strlen(ETH_IFACE)))
    {
      /* Check if eth interface is enabled. */
      ret = QCMAP_ConnectionManager::IsInterfaceEnabled(ETH_IFACE);
    }
    else
    {
      ret = isInterfaceUP(config[j].if_name);
    }
    if (ret > IF_STATUS_DOWN)
    {
      ChangeIFState(config[j].if_name, IF_STATE_DOWN);
      if_stopped = true;
      snprintf(command, MAX_COMMAND_STR_LEN, "echo [GSB]: if %s disabled > /dev/kmsg",config[j].if_name);
      ds_system_call(command, strlen(command));
    }

    if(if_stopped)
    {
      ChangeIFState(config[j].if_name, IF_STATE_UP);
      snprintf(command, MAX_COMMAND_STR_LEN, "echo [GSB]: if %s enabled > /dev/kmsg",config[j].if_name);
      ds_system_call(command, strlen(command));
    }
  }

  if (!QcMapMgr->cfg.bootup_config.enable_gsb_at_bootup)
  {
    if(!SetGSBBootUpConfig(true))
    {
      LOG_MSG_ERROR("Error enabling bootup config", 0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
  }

  return true;
}

/*==========================================================
 FUNCTION    DisableGSB
===========================================================*/
/*!
@brief
  Disables GSB

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
boolean QCMAP_GSB::DisableGSB
(
  qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_msgr_gsb_config_v01 config[QCMAP_MSGR_MAX_IF_SUPPORTED_V01];
  uint8 entries = 0;
  boolean if_stopped[QCMAP_MSGR_MAX_IF_SUPPORTED_V01];
  int ret = 0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode =
                                  QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  memset(if_stopped,0, QCMAP_MSGR_MAX_IF_SUPPORTED_V01);

  if (QcMapMgr == NULL)
  {
    LOG_MSG_ERROR("Null Obj", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  //if not enabled return 0
  if (!QCMAP_GSB::GSBEnableFlag)
  {
    LOG_MSG_ERROR("GSB not enabled",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  // if enabled, then read configuration
  entries = GetGSBConfigFromXML(config);
  //if no configuration entry found then return
  if (!entries)
  {
    LOG_MSG_ERROR("No configuration found",0,0,0);
    //unload GSB module
   //unload GSB module
    QCMAP_GSB::UnLoadGSB(qmi_err_num);
    usleep(QCMAP_GSB_DELAY_COUNT * 50);
    return true;
  }
  else if (QcMapMgr->dual_wifi_mode)
  {
    /* Find if WLAN is enabled*/
    if (QCMAP_WLAN::GetWLANStatus(&wlan_mode, qmi_err_num))
    {
      if ((QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 == wlan_mode)  ||
              QCMAP_MSGR_WLAN_MODE_ENUM_MAX_ENUM_VAL_V01 == wlan_mode)
      {
        LOG_MSG_INFO1("WLAN is not UP",0,0,0);
      }
      else
      {
        /* only disable WLAN IF*/
        /*Stop all the enabled IF (the one which are configured  only)
          If not enabled then move to next step*/
        for (int j = 0 ; j < entries; j++)
        {
          if (config[j].if_type == QCMAP_MSGR_INTERFACE_TYPE_ETHERNET_V01)
          {
            if (QCMAP_GSB::DeleteGSBConfig(config[j].if_name, qmi_err_num, false))
            {
              LOG_MSG_INFO1("Delete GSB config succeeded for if %s",config[j].if_name, 0, 0);
            }
            else
            {
              LOG_MSG_ERROR("Delete GSB config failed!!",0,0,0);
            }
          }
          else
          {
            LOG_MSG_INFO1("%s is dynamically managed for dual wifi config, skip",config[j].if_name, 0, 0);
          }
        }
        /*return here*/
        return true;
      }
    }
    else
    {
      LOG_MSG_ERROR("Get WLAN Status failed!!",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
  }


  SetGSBUnloadFlag();
  /*Stop all the enabled IF (the one which are configured  only)
          If not enabled then move to next step*/
  for (int j = 0 ; j < entries; j++)
  {
    if (!strncmp(config[j].if_name,ETH_IFACE,strlen(ETH_IFACE)))
    {
      /* Check if eth interface is enabled. */
      ret = QCMAP_ConnectionManager::IsInterfaceEnabled(ETH_IFACE);
    }
    else
    {
      ret = isInterfaceUP(config[j].if_name);
    }

    if (ret > IF_STATUS_DOWN)
    {
      //stop IF
      ChangeIFState(config[j].if_name, IF_STATE_DOWN);
      if_stopped [j]= true;
    }
  }
 //unload GSB module
  QCMAP_GSB::UnLoadGSB(qmi_err_num);
  usleep(QCMAP_GSB_DELAY_COUNT * 50);

  //restart the interfaces if stopped.
  for (int j = 0 ; j < entries; j++)
  {
    if (if_stopped[j])
    {
      ChangeIFState(config[j].if_name, IF_STATE_UP);
    }
  }

  return true;
}

/*==========================================================
 FUNCTION    DeleteGSBConfig
===========================================================*/
/*!
@brief
  Delete GSB Config

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
boolean QCMAP_GSB::DeleteGSBConfig
(
  char* if_name,
  qmi_error_type_v01 *qmi_err_num,
  boolean update_xml
)
{
  qcmap_msgr_gsb_config_v01 gsb_conf;
  int ret = 0;
  boolean if_stopped = false;
  char command[MAX_COMMAND_STR_LEN];
  uint8 *num_of_entries = 0;


  LOG_MSG_INFO1("removing %s IFACE from GSB",if_name,0,0);

  if (update_xml && !(RemoveGSBConfigFromXML(if_name)))
  {
    LOG_MSG_ERROR("Error removing config from XML",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  strlcpy(gsb_conf.if_name, if_name, QCMAP_MAX_IFACE_NAME_SIZE_V01);

  /*if GSB is enabled, it can be assumed that its cache is updated
  with the configuration. So we need to remove the IF from GSB cache too.*/
  if (QCMAP_GSB::GSBEnableFlag)
  {
    if (!strncmp(if_name,ETH_IFACE,strlen(ETH_IFACE)))
    {
      /* Check if eth interface is enabled. */
      ret = QCMAP_ConnectionManager::IsInterfaceEnabled(ETH_IFACE);
    }
    else
    {
      ret = isInterfaceUP(if_name);
    }

    if (ret > IF_STATE_DOWN)
    {
      //stop IF
      ChangeIFState(if_name, IF_STATE_DOWN);
      if_stopped = true;
    }
    usleep(QCMAP_GSB_DELAY_COUNT * 10);
    //Pass the configuration to GSB.
    //We are only interested in if_name in this case.
    if(SendMSGToGSB(&gsb_conf, MSG_TYPE_DEL)<0)
    {
      LOG_MSG_ERROR("Failed to send msg to GSB", 0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    else
    {
      LOG_MSG_INFO1("sent message to gsb\n",0,0,0);
    }
    /* requires cleaning so adding some time*/
    usleep(QCMAP_GSB_DELAY_COUNT * 100);

    num_of_entries = GetGSBEntryCountFromXML();
    if (num_of_entries == 0)
    {
      LOG_MSG_INFO1("No entry found , Unloading GSB",0,0,0);
      //unload GSB module
      QCMAP_GSB::UnLoadGSB(qmi_err_num);
      usleep(QCMAP_GSB_DELAY_COUNT * 50);

      /*no reason to have GSB enabled at bootup either*/
      LOG_MSG_INFO1("Disabling GSB boot up configuration",0,0,0);
      if(!SetGSBBootUpConfig(false))
      {
        LOG_MSG_ERROR("Error disabling bootup config ",0,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }
    }

    // restart stopped interfaces, if they were stopped
    if(if_stopped)
    {
      ChangeIFState(if_name, IF_STATE_UP);
    }
  }
  else
  {
    LOG_MSG_INFO1("cannot send msg to gsb as it is not enabled",0,0,0);
  }

  return true;
}

/*==========================================================
 FUNCTION    SetGSBConfig
===========================================================*/
/*!
@brief
  Sets GSB Configuration

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
boolean QCMAP_GSB::SetGSBConfig
(
  qcmap_msgr_gsb_config_v01 *gsb_conf,
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  uint8 *num_of_entries = 0;

  num_of_entries = GetGSBEntryCountFromXML();
  if ((num_of_entries+1) > GSB_MAX_IF_SUPPORT)
  {
    LOG_MSG_ERROR("Cannot add %s to GSB config. Max iface reached",gsb_conf->if_name,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  if (IsDuplicateEntry(gsb_conf->if_name) == 0)
  {
    /*IPA is not supported for 9607*/
#ifndef FEATURE_DATA_TARGET_MDM9607
    if (gsb_conf->if_type == QCMAP_MSGR_INTERFACE_TYPE_WLAN_AP_V01)
    {
      if (!UpdateIPACMcfg(gsb_conf->if_name, IPACM_ODU_TAG))
      {
        LOG_MSG_ERROR("Error setting config to IPA cfg XML for iface %s",gsb_conf->if_name,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }
    }
    else if (gsb_conf->if_type == QCMAP_MSGR_INTERFACE_TYPE_WLAN_STA_V01)
    {
      if (!UpdateIPACMcfg(gsb_conf->if_name, IPACM_ODU_STA_TAG))
      {
        LOG_MSG_ERROR("Error setting config to IPA cfg XML for iface %s",gsb_conf->if_name,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
      }
    }
#endif /* FEATURE_DATA_TARGET_MDM9607 */

    if (!SetGSBConfigToXML(gsb_conf))
    {
      LOG_MSG_ERROR("Error setting config to XML",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
  }
  else
  {
    if (IsDuplicateEntry(gsb_conf->if_name) == 1)
    {
      LOG_MSG_ERROR("Entry already present", 0, 0, 0);
    }
    else
    {
      LOG_MSG_ERROR("Problem accessing gsb config", 0, 0, 0);
    }

    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  return true;
}

/*==========================================================
 FUNCTION    GetGSBConfig
===========================================================*/
/*!
@brief
  Gets GSB Configuration

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
boolean QCMAP_GSB::GetGSBConfig
(
  qcmap_msgr_gsb_config_v01 *gsb_conf,
  uint8 *num_of_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  *num_of_entries = GetGSBConfigFromXML(gsb_conf);

  if (!*num_of_entries )
  {
    LOG_MSG_ERROR("No configured entries found",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  return true;
}

/*==========================================================
 FUNCTION    SetCustomGSBDataPath
===========================================================*/
/*!
@brief
  Sets GSB data path for the given IF

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
boolean QCMAP_GSB::SetCustomGSBDataPath
(
  char* if_name,
  qcmap_msgr_gsb_interface_type_enum_v01 if_type,
  boolean flag
)
{
  qcmap_msgr_gsb_config_v01 config;
  int ret = 0;
  boolean if_stopped = false;
  char command[MAX_COMMAND_STR_LEN] = {0};
  memset(&config,0, sizeof(qcmap_msgr_gsb_config_v01));
  qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

  strlcpy(config.if_name, if_name, QCMAP_MAX_IFACE_NAME_SIZE_V01);
  config.if_type = if_type;


  if (flag)
  {
    /* Read any custom config*/
    ret = GetGSBCustomConfig(&config);
    if (!ret)
    {
      /*set default config if no custom config provied*/
      config.bw_reqd_in_mb = GSB_DEFAULT_BW;
      config.if_high_watermark = GSB_DEFAULT_HIGH_WM;
      config.if_low_watermark = GSB_DEFAULT_LOW_WM;
    }

    /*set this config to xml*/
    if (QCMAP_GSB::SetGSBConfig(&config, &qmi_err_num))
    {
      LOG_MSG_INFO1("GSB config set successfully",0,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Set GSB config failed!!",0,0,0);
    }

    /* is GSB already loaded*/
    if (!QCMAP_GSB::GSBEnableFlag)
    {
      QCMAP_GSB::LoadGSB(&qmi_err_num);
    }
    else
    {
      LOG_MSG_INFO1("GSB already loaded", 0, 0 ,0);
    }
    usleep(QCMAP_GSB_DELAY_COUNT * 50);

    /* enable GSB for provided IF */
    if(SendMSGToGSB(&config, MSG_TYPE_ADD)<0)
    {
      LOG_MSG_ERROR("Failed to send msg to GSB", 0,0,0);
      return false;
    }
    usleep(QCMAP_GSB_DELAY_COUNT * 50);

    /* Send the message to IPACM. */
    QCMAP_GSB::BindIFwithGSB(if_name, if_type);
    usleep(QCMAP_GSB_DELAY_COUNT * 50);
  }
  else
  {
    LOG_MSG_INFO1("removing %s IFACE from GSB",if_name,0,0);
    if (!RemoveGSBConfigFromXML(if_name))
    {
      LOG_MSG_ERROR("Error removing config from XML",0,0,0);
      return false;
    }
    strlcpy(config.if_name, if_name, QCMAP_MAX_IFACE_NAME_SIZE_V01);

    if (QCMAP_GSB::GSBEnableFlag)
    {
      /*Pass the configuration to GSB.
      We are only interested in if_name in this case.*/
      if(SendMSGToGSB(&config, MSG_TYPE_DEL)<0)
      {
        LOG_MSG_ERROR("Failed to send msg to GSB", 0,0,0);
        return false;
      }
      else
      {
        LOG_MSG_INFO1("sent msg to gsb",0,0,0);
      }
      /* requires cleaning so adding some time*/
      usleep(QCMAP_GSB_DELAY_COUNT * 100);
    }
    else
    {
      LOG_MSG_INFO1("cannot send msg to gsb as it is not enabled",0,0,0);
    }
  }

  return true;
}

/*==========================================================
 FUNCTION    BindIFwithGSB
===========================================================*/
/*!
@brief
  Binds the configured IF with GSB

@parameters

@return
   -None


@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
void QCMAP_GSB::BindIFwithGSB
(
  char* if_name,
  qcmap_msgr_gsb_interface_type_enum_v01 iface_type
)
{
#if !defined(FEATURE_DATA_TARGET_MDM9607) && !defined(FEATURE_MOBILEAP_LV_PLATFORM) && \
    !defined(FEATURE_QCMAP_OFFTARGET) && !defined(FEATURE_MOBILEAP_APQ_PLATFORM) && false

  int ret = 0;
  int ioctl_file_fd = -1, ioctl_ret = -1;
  struct ipa_ioc_gsb_info gsb_ioctl_buffer;

  if (if_name == NULL)
  {
    LOG_MSG_ERROR("BindIFwithGSB() Invalid parameters received",0,0,0);
    return;
  }

  memset(&gsb_ioctl_buffer, 0, sizeof(struct ipa_ioc_gsb_info));
  ioctl_file_fd = open(IPA_DEVICE_NAME, O_RDWR);
  if (ioctl_file_fd < 0)
  {
    LOG_MSG_ERROR("BindIFwithGSB() Cannot open file for ioctl",0,0,0);
    return;
  }

  snprintf(gsb_ioctl_buffer.name, sizeof(gsb_ioctl_buffer.name), "%s", if_name);

  LOG_MSG_INFO1("IOCTL to IPA (IPA_IOC_GSB_CONNECT) for iface=%s",
                  gsb_ioctl_buffer.name, 0, 0);
  ioctl_ret = ioctl(ioctl_file_fd, IPA_IOC_GSB_CONNECT, &gsb_ioctl_buffer);

  if (ioctl_ret)
  {
    LOG_MSG_ERROR("IOCTL to IPA failed with err %d, errno %s",
                  ioctl_ret, strerror(errno), 0);
    close(ioctl_file_fd);
    return;
  }

  close(ioctl_file_fd);

#endif
}
