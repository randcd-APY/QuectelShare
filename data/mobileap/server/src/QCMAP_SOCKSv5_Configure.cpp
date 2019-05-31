/*====================================================

FILE:  QCMAP_SOCKSv5_Configure.cpp

SERVICES:
   QCMAP SOCKSv5 Configuration Specific Implementation

=====================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/

/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse
  chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------
  03/30/17   jt         initial SOCKSv5 support
==========================================================*/

#include <net/if.h>
#include "QCMAP_NATALG.h"
#include "QCMAP_ConnectionManager.h"

/*==========================================================
     QC_SOCKSv5 Namespace Definitions
===========================================================*/

namespace QC_SOCKSv5_Configure
{
  /*===========================================================================
    FUNCTION GetSOCKSv5ProxyConfig
  ==========================================================================*/
  /*!
  @brief
    Get socksv5 proxy config

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

  bool GetSOCKSv5ProxyConfig
  (
    qmi_error_type_v01 *qmi_err_num,
    qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01 *resp_msg
  )
  {
    unsigned char count = 0;
    pugi::xml_document doc;
    pugi::xml_node node;
    qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

    if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
    {
      LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }

    //find out where are configuration files
    if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
    {
      LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    //load conf file
    if(!doc.load_file(socksv5_file_path.conf_file))
    {
      LOG_MSG_INFO1("Error loading the config file: %s\n", socksv5_file_path.conf_file,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    //extract lan_iface
    node = doc.child("PROXY_CONFIG").child("LAN_CFG");
    for(int i = 0; (i < strlen(node.child("LAN_IFACE").child_value())) && (i < IFNAMSIZ); i++)
    {
      resp_msg->lan_iface[i] = node.child("LAN_IFACE").child_value()[i];
    }

    //extract wan_iface
    node = doc.child("PROXY_CONFIG").child("WAN_CFG");
    //go through each wan iface
    for(pugi::xml_node child = node.first_child(); child != NULL; child = child.next_sibling())
    {
      if(strcmp(child.name(), "WAN_IFACE") == 0)
      {
        memset(&(resp_msg->socksv5_wan_service[count]), 0,
               sizeof(resp_msg->socksv5_wan_service[count]));

        //extract name and profile
        for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
            grand_child = grand_child.next_sibling())
        {
          if(strcmp(grand_child.name(), "NAME") == 0)
          {
            for(int i = 0; (i < strlen(grand_child.child_value())) && (i < IFNAMSIZ); i++)
            {
              resp_msg->socksv5_wan_service[count].wan_iface[i] = grand_child.child_value()[i];
            }
          }
          if(strcmp(grand_child.name(), "PROFILE") == 0)
          {
              resp_msg->socksv5_wan_service[count].service_no =
                                                              atoi((char*)grand_child.text().get());
          }
        }

        if(++count >= QCMAP_MAX_NUM_BACKHAULS_V01)
        {
          LOG_MSG_INFO1("Reached maximum number of backhauls in the SOCKSv5 config: %s\n",
                         socksv5_file_path.auth_file,0,0);
          break;
        }
      }
    }

    //load auth file
    if(!doc.load_file(socksv5_file_path.auth_file))
    {
      LOG_MSG_INFO1("Error loading the auth file: %s\n", socksv5_file_path.auth_file,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    //extract auth_method
    node = doc.child("PROXY_CONFIG").child("SOCKSV5_AUTH_METHOD_CFG");
    if(strcmp(node.child("SOCKSV5_AUTH_METHOD").child_value(), "NO_AUTHENTICATION") == 0)
    {
      resp_msg->auth_method = QCMAP_SOCKSV5_NO_AUTHENTICATION_V01;
    } else if(strcmp(node.child("SOCKSV5_AUTH_METHOD").child_value(), "USERNAME_PASSWORD") == 0) {
      resp_msg->auth_method = QCMAP_SOCKSV5_UNAME_PASSWD_V01;
    } else {
      resp_msg->auth_method = -1;
    }

    //fill out config file paths
    if(sizeof(resp_msg->config_file_paths.conf_file) <= strlen(socksv5_file_path.conf_file))
    {
      LOG_MSG_INFO1("Preventing buffer overflow: %d <= %d\n",
                    sizeof(resp_msg->config_file_paths.conf_file),
                    strlen(socksv5_file_path.conf_file),0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    memcpy(resp_msg->config_file_paths.conf_file, socksv5_file_path.conf_file,
           strlen(socksv5_file_path.conf_file));

    if(sizeof(resp_msg->config_file_paths.auth_file) <= strlen(socksv5_file_path.auth_file))
    {
      LOG_MSG_INFO1("Preventing buffer overflow: %d <= %d\n",
                    sizeof(resp_msg->config_file_paths.auth_file),
                    strlen(socksv5_file_path.auth_file),0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    memcpy(resp_msg->config_file_paths.auth_file, socksv5_file_path.auth_file,
           strlen(socksv5_file_path.auth_file));

    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
  }


  /*===========================================================================
    FUNCTION SetSOCKSv5ProxyAuth
  ==========================================================================*/
  /*!
  @brief
    Set socksv5 proxy auth method

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

  bool SetSOCKSv5ProxyAuth
  (
    qmi_error_type_v01 *qmi_err_num,
    unsigned char auth_method
  )
  {
    pugi::xml_document doc;
    pugi::xml_node node;
    qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    char command[MAX_COMMAND_STR_LEN] = {0};

    if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
    {
      LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }

    //find out where are configuration files
    if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
    {
      LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    if(!doc.load_file(socksv5_file_path.auth_file))
    {
      LOG_MSG_INFO1("Error loading the auth file: %s\n", socksv5_file_path.auth_file,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    node = doc.child("PROXY_CONFIG").child("SOCKSV5_AUTH_METHOD_CFG");

    switch(auth_method)
    {
      case QCMAP_SOCKSV5_NO_AUTHENTICATION_V01:
      {
        node.child("SOCKSV5_AUTH_METHOD").text() = "NO_AUTHENTICATION";
        node.child("SOCKSV5_AUTH_METHOD_VALUE").text() = QCMAP_SOCKSV5_NO_AUTHENTICATION_V01;
        break;
      }

      case QCMAP_SOCKSV5_UNAME_PASSWD_V01:
      {
        node.child("SOCKSV5_AUTH_METHOD").text() = "USERNAME_PASSWORD";
        node.child("SOCKSV5_AUTH_METHOD_VALUE").text() = QCMAP_SOCKSV5_UNAME_PASSWD_V01;
        break;
      }

      default:
      {

        printf("Invalid set SOCKSv5 auth response: %u\n", auth_method);
        break;
      }
    }

    doc.save_file(TEMP_SOCKSV5_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_SOCKSV5_CFG ,
              socksv5_file_path.auth_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", socksv5_file_path.auth_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));


    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
  }

  /*===========================================================================
    FUNCTION EditSOCKSv5ProxyLANIface
  ==========================================================================*/
  /*!
  @brief
    Edit socksv5 proxy LAN iface

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

  bool EditSOCKSv5ProxyLANIface
  (
    qmi_error_type_v01 *qmi_err_num,
    char* lan_iface
  )
  {
    pugi::xml_document doc;
    pugi::xml_node node;
    qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    char command[MAX_COMMAND_STR_LEN] = {0};

    if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
    {
      LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }

    //find out where are configuration files
    if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
    {
      LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    if(!doc.load_file(socksv5_file_path.conf_file))
    {
      LOG_MSG_INFO1("Error loading the config file: %s\n", socksv5_file_path.conf_file,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    node = doc.child("PROXY_CONFIG").child("LAN_CFG");

    node.child("LAN_IFACE").text().set(lan_iface);

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


    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
  }

  /*===========================================================================
    FUNCTION AddSOCKSv5ProxyUnamePasswd
  ==========================================================================*/
  /*!
  @brief
    Add socksv5 proxy username and password

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

  bool AddSOCKSv5ProxyUnameAssoc
  (
    qmi_error_type_v01 *qmi_err_num,
    char* uname,
    unsigned int service_no
  )
  {
    pugi::xml_document doc;
    pugi::xml_node node, subchild, grandchild;
    qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    char command[MAX_COMMAND_STR_LEN] = {0};


    if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
    {
      LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }

    //find out where are configuration files
    if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
    {
      LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    if(!doc.load_file(socksv5_file_path.auth_file))
    {
      LOG_MSG_INFO1("Error loading the auth file: %s\n", socksv5_file_path.auth_file,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    LOG_MSG_INFO1("Username: %s\n", uname,0,0);
    LOG_MSG_INFO1("Profile: %u\n", service_no,0,0);

    node = doc.child("PROXY_CONFIG").child("USERNAME_PASSWORD_CFG");
    subchild = node.append_child("USER");
    grandchild = subchild.append_child("NAME");
    grandchild.text().set(uname);
    grandchild = subchild.append_child("PROFILE");
    grandchild.text().set(service_no);

    doc.save_file(TEMP_SOCKSV5_CFG);
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_SOCKSV5_CFG ,
              socksv5_file_path.auth_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", socksv5_file_path.auth_file);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_SOCKSV5_CFG);
    ds_system_call(command, strlen(command));

    *qmi_err_num = QMI_ERR_NONE_V01;
    return true;
  }

  /*===========================================================================
    FUNCTION DeleteSOCKSv5ProxyUnameAssoc
  ==========================================================================*/
  /*!
  @brief
    Delete socksv5 proxy username association

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

  bool DeleteSOCKSv5ProxyUnameAssoc
  (
    qmi_error_type_v01 *qmi_err_num,
    char* uname
  )
  {
    unsigned char uname_valid;
    pugi::xml_document doc;
    pugi::xml_node node;
    qcmap_msgr_socksv5_config_file_paths_v01 socksv5_file_path;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    char command[MAX_COMMAND_STR_LEN] = {0};

    if(QcMapMgr && !(IS_SOCKSV5_ALLOWED(QcMapMgr->target)))
    {
      LOG_MSG_INFO1("SOCKSv5 not allowed on target: %d", QcMapMgr->target, 0, 0);
      *qmi_err_num = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      return false;
    }

    //find out where are configuration files
    if(!QCMAP_NATALG::GetSOCKSv5ConfigFilePathFromXML(&socksv5_file_path))
    {
      LOG_MSG_INFO1("Error extracting the socksv5 config file locations\n",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    if(!doc.load_file(socksv5_file_path.auth_file))
    {
      LOG_MSG_INFO1("Error loading the auth file: %s\n", socksv5_file_path.auth_file,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    node = doc.child("PROXY_CONFIG").child("USERNAME_PASSWORD_CFG");

    //go through each user
    for(pugi::xml_node child = node.first_child(); child != NULL; child = child.next_sibling())
    {
      uname_valid = 0;

      if(strcmp(child.name(), "USER") == 0)
      {
        //validate username and password for this user
        for(pugi::xml_node grand_child = child.first_child(); grand_child != NULL;
            grand_child = grand_child.next_sibling())
        {
          if(strcmp(grand_child.name(), "NAME") == 0)
          {
            if(strcmp(grand_child.child_value(), uname) == 0) //valid uname?
            {
              uname_valid = 1;
              break;
            }
          }
        }
      }

      //delete that uname entry
      if(uname_valid)
      {
        child.parent().remove_child(child);
        doc.save_file(TEMP_SOCKSV5_CFG);
        snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", TEMP_SOCKSV5_CFG);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"cp %s %s ",TEMP_SOCKSV5_CFG ,
                  socksv5_file_path.auth_file);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"fsync -d %s", socksv5_file_path.auth_file);
        ds_system_call(command, strlen(command));
        snprintf( command, MAX_COMMAND_STR_LEN,"rm %s ",TEMP_SOCKSV5_CFG);
        ds_system_call(command, strlen(command));
        return true;
      }
    }

   LOG_MSG_INFO1("Invalid username\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;;
    return false;
  }
}
