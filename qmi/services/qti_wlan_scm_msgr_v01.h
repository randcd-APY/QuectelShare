#ifndef SCM_MSGR_SERVICE_01_H
#define SCM_MSGR_SERVICE_01_H
/**
  @file qti_wlan_scm_msgr_v01.h

  @brief This is the public header file which defines the scm_msgr service Data structures.

  This header file defines the types and structures that were defined in
  scm_msgr. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2016,2018 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/scm_msgr/api/qti_wlan_scm_msgr_v01.h#5 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Apr 13 2018 (Spin 0)
   From IDL File: qti_wlan_scm_msgr_v01.idl */

/** @defgroup scm_msgr_qmi_consts Constant values defined in the IDL */
/** @defgroup scm_msgr_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup scm_msgr_qmi_enums Enumerated types used in QMI messages */
/** @defgroup scm_msgr_qmi_messages Structures sent as QMI messages */
/** @defgroup scm_msgr_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup scm_msgr_qmi_accessor Accessor for QMI service object */
/** @defgroup scm_msgr_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup scm_msgr_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SCM_MSGR_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SCM_MSGR_V01_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SCM_MSGR_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SCM_MSGR_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup scm_msgr_qmi_consts
    @{
  */
#define SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01 3
#define SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01 80
#define SCM_MSGR_WLAN_CMD_LINE_ARG_MAX_V01 100
#define SCM_MSGR_WLAN_IF_TYPES_MAX_V01 7
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_enums
    @{
  */
typedef enum {
  SCM_MSGR_WLAN_CONCURRENCY_CFG_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SCM_MSGR_WLAN_CFG_INVALID_V01 = 0x00, 
  SCM_MSGR_WLAN_CFG_1_STA_INT_V01 = 0x01, 
  SCM_MSGR_WLAN_CFG_1_SAP_V01 = 0x02, 
  SCM_MSGR_WLAN_CFG_2_SAP_V01 = 0x03, 
  SCM_MSGR_WLAN_CFG_3_SAP_V01 = 0x04, 
  SCM_MSGR_WLAN_CFG_4_SAP_V01 = 0x05, 
  SCM_MSGR_WLAN_CFG_1_SAP_1_STA_ROUTER_MODE_V01 = 0x06, 
  SCM_MSGR_WLAN_CFG_2_SAP_1_STA_ROUTER_MODE_V01 = 0x07, 
  SCM_MSGR_WLAN_CFG_1_SAP_1_STA_BRIDGE_MODE_V01 = 0x08, 
  SCM_MSGR_WLAN_CFG_2_SAP_1_STA_BRIDGE_MODE_V01 = 0x09, 
  SCM_MSGR_WLAN_CFG_MODE_MAX_V01 = 0x0A, 
  SCM_MSGR_WLAN_CONCURRENCY_CFG_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}scm_msgr_wlan_concurrency_cfg_enum_v01;
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_enums
    @{
  */
typedef enum {
  SCM_MSGR_WLAN_IF_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SCM_MSGR_WLAN_IF_TYPE_INVALID_V01 = 0x00, 
  SCM_MSGR_WLAN_IF_SAP_1_V01 = 0x01, 
  SCM_MSGR_WLAN_IF_SAP_2_V01 = 0x02, 
  SCM_MSGR_WLAN_IF_SAP_3_V01 = 0x03, 
  SCM_MSGR_WLAN_IF_SAP_4_V01 = 0x04, 
  SCM_MSGR_WLAN_IF_STA_1_V01 = 0x05, 
  SCM_MSGR_WLAN_IF_STA_2_V01 = 0x06, 
  SCM_MSGR_WLAN_IF_STA_1_INT_V01 = 0x07, 
  SCM_MSGR_WLAN_IF_TYPE_MAX_V01 = 0x08, 
  SCM_MSGR_WLAN_IF_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}scm_msgr_wlan_if_type_enum_v01;
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_enums
    @{
  */
typedef enum {
  SCM_MSGR_WLAN_IF_CONTROL_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SCM_MSGR_WLAN_IF_CTRL_INVALID_V01 = 0x00, 
  SCM_MSGR_WLAN_IF_STOP_V01 = 0x01, 
  SCM_MSGR_WLAN_IF_START_V01 = 0x02, 
  SCM_MSGR_WLAN_IF_RESTART_V01 = 0x03, 
  SCM_MSGR_WLAN_IF_CTRL_MAX_V01 = 0x04, 
  SCM_MSGR_WLAN_IF_CONTROL_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}scm_msgr_wlan_if_control_enum_v01;
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_enums
    @{
  */
typedef enum {
  SCM_MSGR_WLAN_IND_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SCM_MSGR_WLAN_IND_INVALID_V01 = 0x00, 
  SCM_MSGR_WLAN_DYNAMIC_RECONFIG_IND_V01 = 0x01, 
  SCM_MSGR_WLAN_STATION_STATE_IND_V01 = 0x02, 
  SCM_MSGR_WLAN_SYS_CONTROL_IND_V01 = 0x03, 
  SCM_MSGR_WLAN_IND_TYPE_MAX_V01 = 0x04, 
  SCM_MSGR_WLAN_IND_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}scm_msgr_wlan_ind_type_enum_v01;
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_enums
    @{
  */
typedef enum {
  SCM_MSGR_WLAN_SYS_CONTROL_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SCM_MSGR_WLAN_SYS_CTRL_INVALID_V01 = 0x00, 
  SCM_MSGR_WLAN_DISABLE_V01 = 0x01, 
  SCM_MSGR_WLAN_ENABLE_V01 = 0x02, 
  SCM_MSGR_WLAN_SYS_CTRL_MAX_V01 = 0x03, 
  SCM_MSGR_WLAN_SYS_CONTROL_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}scm_msgr_wlan_sys_control_enum_v01;
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_enums
    @{
  */
typedef enum {
  SCM_MSGR_WLAN_STATION_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SCM_MSGR_WLAN_STATION_STATUS_INVALID_V01 = 0x00, 
  SCM_MSGR_WLAN_STATION_DISCONNECTED_V01 = 0x01, 
  SCM_MSGR_WLAN_STATION_CONNECTED_V01 = 0x02, 
  SCM_MSGR_WLAN_STATION_STATUS_MAX_V01 = 0x03, 
  SCM_MSGR_WLAN_STATION_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}scm_msgr_wlan_station_status_enum_v01;
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_enums
    @{
  */
typedef enum {
  SCM_MSGR_WLAN_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SCM_MSGR_WLAN_DEV_TYPE_INVALID_V01 = 0x00, 
  SCM_MSGR_WLAN_DEV_ROME_V01 = 0x01, 
  SCM_MSGR_WLAN_DEV_TUF_V01 = 0x02, 
  SCM_MSGR_WLAN_DEVICE_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}scm_msgr_wlan_device_type_enum_v01;
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_aggregates
    @{
  */
/**  Internal station only mode 
 One SoftAP Interface 
 Two SoftAP Concurrent Interfaces 
 Three SoftAP Concurrent Interfaces 
 Four SoftAP Concurrent Interfaces 
 Soft AP and Station Concurrent Interfaces in Router mode 
 Two Soft AP and Station Concurrent Interfaces in Router mode 
 Soft AP and Station Concurrent Interfaces in bridge mode 
 Two Soft AP and Station Concurrent Interfaces in bridge mode 
 First SoftAP Concurrent Interface 
 Second SoftAP Concurrent Interface 
 Third SoftAP Concurrent Interface 
 Fourth SoftAP Concurrent Interface 
 First Station Concurrent Interface 
 Second Station Concurrent Interface 
 Internal Station only mode Interface 
 WLAN interface stop 
 WLAN interface start 
 WLAN Interface restart 
 SCM dynamic reconfiguration indication on specified interfaces 
 Station interface connection update indiation 
 SCM gives this indication to disable wlan due wlan inactivity, SSR 
 WLAN system enable 
 WLAN system disable 
 Station interface is not connected to external AP 
 Station interface is successfully connected to external AP 
 WLAN ROME device 
 WLAN TUF device 
 Data Structure used to provide WLAN Interface Type and Number 
 */
typedef struct {

  scm_msgr_wlan_if_type_enum_v01 wlan_if_type;

  /*  WLAN Interface Type in supported concurrent configuration  */
  uint8_t wlan_if_num;

  /*  WLAN Interface number  */
  scm_msgr_wlan_device_type_enum_v01 wlan_dev_type;
}scm_msgr_wlan_if_data_v01;  /* Type */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_aggregates
    @{
  */
/**  Data Structure used to provide WLAN Supplicant config 
 */
typedef struct {

  scm_msgr_wlan_if_type_enum_v01 wlan_if_type;

  /*  WLAN Interface Type in supported concurrent configuration  */
  char conf_file[SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01];

  /*  WLAN wpa_supplicant / hostapd config file path  */
  char pid_file[SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01];

  /*  WLAN wpa_supplicant / hostapd pid file path  */
  char entropy_file[SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01];

  /*  WLAN wpa_supplicant / hostapd entropy file path  */
  char extra_cmd_line_args[SCM_MSGR_WLAN_CMD_LINE_ARG_MAX_V01];
}scm_msgr_wlan_cfg_v01;  /* Type */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_aggregates
    @{
  */
typedef struct {

  scm_msgr_wlan_if_control_enum_v01 wlan_if_control;

  /*  wlan interface status  */
  scm_msgr_wlan_if_type_enum_v01 wlan_if_type;

  /*  WLAN Interface Type in supported concurrent configuration  */
  uint8_t wlan_if_num;

  /*  WLAN Interface number  */
  scm_msgr_wlan_device_type_enum_v01 wlan_dev_type;
}scm_msgr_wlan_dyn_ind_notify_v01;  /* Type */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_aggregates
    @{
  */
typedef struct {

  scm_msgr_wlan_station_status_enum_v01 wlan_sta_status;

  /*  WLAN station connection status  */
  scm_msgr_wlan_if_type_enum_v01 wlan_if_type;

  /*  WLAN Interface Type in supported concurrent configuration  */
  uint8_t wlan_if_num;

  /*  WLAN Interface number  */
  scm_msgr_wlan_device_type_enum_v01 wlan_dev_type;
}scm_msgr_wlan_sta_con_notify_v01;  /* Type */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Request Message; Enables the WLAN in given configuration. */
typedef struct {

  /* Mandatory */
  /*  WLAN concurrency configuration */
  scm_msgr_wlan_concurrency_cfg_enum_v01 wlan_concurrency_cfg;
}scm_msgr_wlan_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Response Message; Enables the WLAN in given configuration. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  WLAN concurrency Interface Data */
  uint8_t wlan_concurrency_cfg_valid;  /**< Must be set to true if wlan_concurrency_cfg is being passed */
  uint32_t wlan_concurrency_cfg_len;  /**< Must be set to # of elements in wlan_concurrency_cfg */
  scm_msgr_wlan_if_data_v01 wlan_concurrency_cfg[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];
}scm_msgr_wlan_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Request Message; Disables the WLAN. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}scm_msgr_wlan_disable_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Response Message; Disables the WLAN. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}scm_msgr_wlan_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Request Message; Reconfigure provided WLAN interface. */
typedef struct {

  /* Mandatory */
  /*  WLAN interface type */
  uint32_t wlan_if_type_len;  /**< Must be set to # of elements in wlan_if_type */
  scm_msgr_wlan_if_type_enum_v01 wlan_if_type[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];

  /* Mandatory */
  /*  WLAN Interfaces types to be reconfigured. One or more concurrent
         Interface can be provided to be reconfigured.
     */
  uint32_t wlan_if_control_len;  /**< Must be set to # of elements in wlan_if_control */
  scm_msgr_wlan_if_control_enum_v01 wlan_if_control[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];
}scm_msgr_wlan_if_reconfig_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Response Message; Reconfigure provided WLAN interface. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  WLAN concurrency Interface Data */
  uint8_t wlan_concurrency_cfg_valid;  /**< Must be set to true if wlan_concurrency_cfg is being passed */
  uint32_t wlan_concurrency_cfg_len;  /**< Must be set to # of elements in wlan_concurrency_cfg */
  scm_msgr_wlan_if_data_v01 wlan_concurrency_cfg[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];
}scm_msgr_wlan_if_reconfig_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Request Message; Registers or deregisters the control point to receive QMI_SCM_MSGR_WLAN_IND. */
typedef struct {

  /* Mandatory */
  /*  Register Indication */
  uint8_t register_indication;
  /**<   Specifies the registration. Values: 
       - 0 -- Do not register or deregister the control point if already registered
       - 1 -- Register for the indication; ignore if already registered
  */

  /* Optional */
  uint8_t wlan_cfg_valid;  /**< Must be set to true if wlan_cfg is being passed */
  uint32_t wlan_cfg_len;  /**< Must be set to # of elements in wlan_cfg */
  scm_msgr_wlan_cfg_v01 wlan_cfg[SCM_MSGR_WLAN_IF_TYPES_MAX_V01];
}scm_msgr_wlan_setup_ind_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Response Message; Registers or deregisters the control point to receive QMI_SCM_MSGR_WLAN_IND. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}scm_msgr_wlan_setup_ind_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup scm_msgr_qmi_messages
    @{
  */
/** Indication Message; Indicates a change in the current wlan configuration due to operating condition changes
	       or station connection status. */
typedef struct {

  /* Mandatory */
  /*  WLAN indication type */
  scm_msgr_wlan_ind_type_enum_v01 wlan_ind_type;

  /* Optional */
  /*  WLAN dynamic indication notification structure */
  uint8_t wlan_dyn_ind_valid;  /**< Must be set to true if wlan_dyn_ind is being passed */
  uint32_t wlan_dyn_ind_len;  /**< Must be set to # of elements in wlan_dyn_ind */
  scm_msgr_wlan_dyn_ind_notify_v01 wlan_dyn_ind[SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01];

  /* Optional */
  /*  Provides the WLAN interface types reconfigured dynamically due to changes in
     WIFI environment, with name mapping 
	
 WLAN Station interface conncetion update notification structure */
  uint8_t wlan_sta_ind_valid;  /**< Must be set to true if wlan_sta_ind is being passed */
  scm_msgr_wlan_sta_con_notify_v01 wlan_sta_ind;

  /* Optional */
  /*  WLAN Module enabled disable due Autoshutdown SSR etc. */
  uint8_t wlan_sys_control_valid;  /**< Must be set to true if wlan_sys_control is being passed */
  scm_msgr_wlan_sys_control_enum_v01 wlan_sys_control;
}scm_msgr_wlan_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_SCM_MSGR_IND_V01 
//#define REMOVE_QMI_SCM_MSGR_WLAN_DISABLE_V01 
//#define REMOVE_QMI_SCM_MSGR_WLAN_ENABLE_V01 
//#define REMOVE_QMI_SCM_MSGR_WLAN_IF_RECONFIG_V01 
//#define REMOVE_QMI_SCM_MSGR_WLAN_SETUP_IND_REG_V01 

/*Service Message Definition*/
/** @addtogroup scm_msgr_qmi_msg_ids
    @{
  */
#define QMI_SCM_MSGR_WLAN_ENABLE_REQ_V01 0x0020
#define QMI_SCM_MSGR_WLAN_ENABLE_RESP_V01 0x0020
#define QMI_SCM_MSGR_WLAN_DISABLE_REQ_V01 0x0021
#define QMI_SCM_MSGR_WLAN_DISABLE_RESP_V01 0x0021
#define QMI_SCM_MSGR_WLAN_IF_RECONFIG_REQ_V01 0x0022
#define QMI_SCM_MSGR_WLAN_IF_RECONFIG_RESP_V01 0x0022
#define QMI_SCM_MSGR_WLAN_SETUP_IND_REG_REQ_V01 0x0023
#define QMI_SCM_MSGR_WLAN_SETUP_IND_REG_RESP_V01 0x0023
#define QMI_SCM_MSGR_WLAN_IND_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro scm_msgr_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type scm_msgr_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define scm_msgr_get_service_object_v01( ) \
          scm_msgr_get_service_object_internal_v01( \
            SCM_MSGR_V01_IDL_MAJOR_VERS, SCM_MSGR_V01_IDL_MINOR_VERS, \
            SCM_MSGR_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

