#ifndef CTL_SERVICE_H
#define CTL_SERVICE_H
/**
  @file control_service_v01.h

  @brief This is the public header file which defines the ctl service Data structures.

  This header file defines the types and structures that were defined in
  ctl. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

  $Header: //source/qcom/qct/interfaces/qmi/ctl/main/latest/api/control_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.1
   It requires encode/decode library version 4 or later
   It was generated on: Fri Mar 16 2012
   From IDL File: control_service_v01.idl */

/** @defgroup ctl_qmi_consts Constant values defined in the IDL */
/** @defgroup ctl_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ctl_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ctl_qmi_messages Structures sent as QMI messages */
/** @defgroup ctl_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ctl_qmi_accessor Accessor for QMI service object */
/** @defgroup ctl_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ctl_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define CTL_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define CTL_V01_IDL_MINOR_VERS 0x06
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define CTL_V01_IDL_TOOL_VERS 0x05
/** Maximum Defined Message ID */
#define CTL_V01_MAX_MESSAGE_ID 0x002B;
/**
    @}
  */


/** @addtogroup ctl_qmi_consts
    @{
  */
#define QMI_CTL_MAX_SERVICES_V01 255
#define QMI_CTL_MAX_ADDENDUM_LABEL_V01 255
#define QMI_CTL_MAX_IND_SET_V01 10
/**
    @}
  */

/** @addtogroup ctl_qmi_enums
    @{
  */
typedef enum {
  CTL_DATA_FORMAT_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CTL_DATA_FORMAT_NO_QOS_V01 = 0,
  CTL_DATA_FORMAT_QOS_V01 = 1,
  CTL_DATA_FORMAT_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ctl_data_format_enum_v01;
/**
    @}
  */

typedef uint16_t ctl_link_prot_mask_v01;
#define CTL_MASK_ETHER_V01 ((ctl_link_prot_mask_v01)0x1)
#define CTL_MASK_IP_V01 ((ctl_link_prot_mask_v01)0x2)
/** @addtogroup ctl_qmi_enums
    @{
  */
typedef enum {
  CTL_DATA_AGG_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CTL_DATA_AGG_DISABLE_V01 = 0x0,
  CTL_DATA_AGG_TLP_V01 = 0x1,
  CTL_DATA_AGG_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ctl_data_agg_enum_v01;
/**
    @}
  */

/** @addtogroup ctl_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t qmi_svc_type;
  /**<   QMI service type, as defined in [Q1]
   */

  uint16_t major_ver;
  /**<   Major version number of the QMI service specified by qmi_svc_type
   */

  uint16_t minor_ver;
  /**<   Minor version number of the QMI service specified by qmi_svc_type
   */
}ctl_service_version_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ctl_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t qmi_svc_type;
  /**<   QMI service type, as defined in [Q1]
   */

  uint16_t addendum_major_ver;
  /**<   Addendum major version number of the QMI service
       specified by qmi_svc_type
   */

  uint16_t addendum_minor_ver;
  /**<   Addendum minor version number of the QMI service
       specified by qmi_svc_type
   */
}ctl_addendum_version_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ctl_qmi_aggregates
    @{
  */
typedef struct {

  char addendum_label[QMI_CTL_MAX_ADDENDUM_LABEL_V01 + 1];
  /**<   Label describing the addendum
   */

  uint32_t addendum_version_list_len;  /**< Must be set to # of elements in addendum_version_list */
  ctl_addendum_version_type_v01 addendum_version_list[QMI_CTL_MAX_SERVICES_V01];
}ctl_addendum_ver_list_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ctl_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t qmi_svc_type;
  /**<   QMI Service type */

  uint8_t client_id;
  /**<   Client ID */
}ctl_svc_clid_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ctl_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t current_state;
  /**<   Handle specified by control point representing Power Save state;
       indicates state after transition
   */

  uint32_t previous_state;
  /**<   Handle specified by control point representing Power Save state;
       indicates state prior to transition
   */
}ctl_pwr_save_state_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ctl_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t pwrsave_state;
  /**<   Handle specified by control point representing Power Save state;
       client-specified value should match that used in
       QMI_CTL_SET_PWR_SAVE_MODE_REQ; 0x00 through 0xFF are reserved
   */

  uint8_t qmi_service;
  /**<   Valid QMI service identifier
   */
}ctl_pwr_save_desc_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Request Message; Generates a unique ID to distinguish the QMI link over which
           the message is sent. */
typedef struct {

  /* Mandatory */
  /*  Host Driver Instance */
  uint8_t host_driver_instance;
  /**<   Host-unique QMI instance for this device driver
   */
}ctl_set_instance_id_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Generates a unique ID to distinguish the QMI link over which
           the message is sent. */
typedef struct {

  /* Mandatory */
  /*  QMI Link ID */
  uint16_t qmi_id;
  /**<   Unique QMI link ID assigned to the link over which the message is
       exchanged; the upper byte is assigned by the QMI_CTL service and the
       lower byte is assigned by the host (the value passed in the request)
   */

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_set_instance_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/*
 * ctl_get_version_info_req_msg is empty
 * typedef struct {
 * }ctl_get_version_info_req_msg_v01;
 */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Queries the versions of all QMI services supported by the device. */
typedef struct {

  /* Mandatory */
  /*  QMUX Service Version List */
  uint32_t service_version_list_len;  /**< Must be set to # of elements in service_version_list */
  ctl_service_version_type_v01 service_version_list[QMI_CTL_MAX_SERVICES_V01];

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Addendum version list */
  uint8_t addendum_ver_list_valid;  /**< Must be set to true if addendum_ver_list is being passed */
  ctl_addendum_ver_list_type_v01 addendum_ver_list;
}ctl_get_version_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Request Message; Requests a client ID for the specified QMI service type. */
typedef struct {

  /* Mandatory */
  /*  QMI Service Type */
  uint8_t qmi_svc_type;
  /**<   QMI service type for which a client ID is requested
   */
}ctl_get_client_id_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Requests a client ID for the specified QMI service type. */
typedef struct {

  /* Mandatory */
  /*  Assigned client ID */
  ctl_svc_clid_type_v01 svc_client_id;

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_get_client_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Request Message; Releases a previously assigned client ID. */
typedef struct {

  /* Mandatory */
  /*  Client ID to release */
  ctl_svc_clid_type_v01 svc_client_id;
}ctl_release_client_id_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Releases a previously assigned client ID. */
typedef struct {

  /* Mandatory */
  /*  Released client ID */
  ctl_svc_clid_type_v01 svc_client_id;

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_release_client_id_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Indication Message; Indicates that a client ID has been revoked by the service. */
typedef struct {

  /* Mandatory */
  /*  Revoked client ID */
  ctl_svc_clid_type_v01 svc_client_id;
}ctl_revoke_client_id_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Indication Message; Indicates that a client ID/service type pair specified in a
           QMUX header is invalid. */
typedef struct {

  /* Mandatory */
  /*  Invalid client ID */
  ctl_svc_clid_type_v01 svc_client_id;
}ctl_invalid_client_id_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Request Message; Indicates the MSM device of the data format used by the driver. */
typedef struct {

  /* Mandatory */
  /*  Data format */
  ctl_data_format_enum_v01 data_format;
  /**<   Data format used by the driver \n
       - 0 -- No QoS flow header \n
       - 1 -- QoS flow header present
   */

  /* Optional */
  /*  Underlying link layer protocol */
  uint8_t link_prot_valid;  /**< Must be set to true if link_prot is being passed */
  ctl_link_prot_mask_v01 link_prot;
  /**<   Bitmask of link protocols supported by the driver; if multiple
       protocols are supported, they are OR’ed together as a mask \n
       - 0x1 -- 802.3 \n
       - 0x2 -- IP
   */

  /* Optional */
  /*  Uplink data aggregation protocol */
  uint8_t ul_data_agg_setting_valid;  /**< Must be set to true if ul_data_agg_setting is being passed */
  ctl_data_agg_enum_v01 ul_data_agg_setting;
  /**<   Data aggregation protocol to be used for uplink data transfer \n
       - 0x0 -- Disable data aggregation \n
       - 0x1 -- TLP (Thin Layer Protocol)
   */
}ctl_set_data_format_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Indicates the MSM device of the data format used by the driver. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Underlying link layer protocol */
  uint8_t link_prot_valid;  /**< Must be set to true if link_prot is being passed */
  ctl_link_prot_mask_v01 link_prot;
  /**<   The link protocol used by the driver; only one protocol in the
       response indicates the mode that should be used \n
       - 0x1 -- 802.3 \n
       - 0x2 -- IP
   */

  /* Optional */
  /*  Configured uplink data aggregation protocol */
  uint8_t ul_data_agg_setting_valid;  /**< Must be set to true if ul_data_agg_setting is being passed */
  ctl_data_agg_enum_v01 ul_data_agg_setting;
  /**<   Data aggregation protocol that got configured on the device \n
       - 0x0 -- Disabled \n
       - 0x1 -- TLP (Thin Layer Protocol)
   */
}ctl_set_data_format_resp_msg_v01;  /* Message */
/**
    @}
  */

/*
 * ctl_sync_req_msg is empty
 * typedef struct {
 * }ctl_sync_req_msg_v01;
 */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Synchronizes the service provider and service consumer. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_sync_resp_msg_v01;  /* Message */
/**
    @}
  */

/*
 * ctl_sync_ind_msg is empty
 * typedef struct {
 * }ctl_sync_ind_msg_v01;
 */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Request Message; Sets the Control Power Save mode state reporting conditions
           for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Power Save Normal state report */
  uint8_t report_pwrsnormal_state;
  /**<   Values: \n
       - 0 -- Do not report \n
       - 1 -- Report when Power Save state changes to Normal
   */
}ctl_reg_pwr_save_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Sets the Control Power Save mode state reporting conditions
           for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_reg_pwr_save_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Indication Message; Indicates a Power Save mode state change update was sent to
           the control point. */
typedef struct {

  /* Optional */
  /*  Power Save state report */
  uint8_t pwr_save_state_valid;  /**< Must be set to true if pwr_save_state is being passed */
  ctl_pwr_save_state_type_v01 pwr_save_state;
}ctl_pwr_save_mode_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Request Message; Configures the event indication filter by the Power Save state
           for each QMI service. */
typedef struct {

  /* Mandatory */
  /*  Power Save Descriptor */
  ctl_pwr_save_desc_type_v01 pwr_save_desc;

  /* Optional */
  /*  Permitted Indication Set */
  uint8_t indication_set_valid;  /**< Must be set to true if indication_set is being passed */
  uint32_t indication_set_len;  /**< Must be set to # of elements in indication_set */
  uint16_t indication_set[QMI_CTL_MAX_IND_SET_V01];
  /**<   A sequence of indication message identifiers (2 bytes each)
    */
}ctl_config_pwr_save_settings_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Configures the event indication filter by the Power Save state
           for each QMI service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_config_pwr_save_settings_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Request Message; Sets the QMI framework Power Save mode. */
typedef struct {

  /* Mandatory */
  /*  Power Save State */
  uint32_t pwrsave_state;
  /**<   Handle specified by control point representing power save state;
       client specified; value should match that used in message
       QMI_CTL_CONFIG_PWR_SAVE_SAVINGS
   */
}ctl_set_pwr_save_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Sets the QMI framework Power Save mode. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_set_pwr_save_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/*
 * ctl_get_pwr_save_mode_req_msg is empty
 * typedef struct {
 * }ctl_get_pwr_save_mode_req_msg_v01;
 */

/** @addtogroup ctl_qmi_messages
    @{
  */
/** Response Message; Gets the current QMI framework Power Save mode. */
typedef struct {

  /* Mandatory */
  /*  Power Save State */
  uint32_t pwrsave_state;
  /**<   Handle specified by control point representing Power Save state
   */

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}ctl_get_pwr_save_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup ctl_qmi_msg_ids
    @{
  */
#define QMI_CTL_SET_INSTANCE_ID_REQ_V01 0x0020
#define QMI_CTL_SET_INSTANCE_ID_RESP_V01 0x0020
#define QMI_CTL_GET_VERSION_INFO_REQ_V01 0x0021
#define QMI_CTL_GET_VERSION_INFO_RESP_V01 0x0021
#define QMI_CTL_GET_CLIENT_ID_REQ_V01 0x0022
#define QMI_CTL_GET_CLIENT_ID_RESP_V01 0x0022
#define QMI_CTL_RELEASE_CLIENT_ID_REQ_V01 0x0023
#define QMI_CTL_RELEASE_CLIENT_ID_RESP_V01 0x0023
#define QMI_CTL_REVOKE_CLIENT_ID_IND_V01 0x0024
#define QMI_CTL_INVALID_CLIENT_ID_IND_V01 0x0025
#define QMI_CTL_SET_DATA_FORMAT_REQ_V01 0x0026
#define QMI_CTL_SET_DATA_FORMAT_RESP_V01 0x0026
#define QMI_CTL_SYNC_REQ_V01 0x0027
#define QMI_CTL_SYNC_RESP_V01 0x0027
#define QMI_CTL_SYNC_IND_V01 0x0027
#define QMI_CTL_REG_PWR_SAVE_MODE_REQ_V01 0x0028
#define QMI_CTL_REG_PWR_SAVE_MODE_RESP_V01 0x0028
#define QMI_CTL_PWR_SAVE_MODE_IND_V01 0x0028
#define QMI_CTL_CONFIG_PWR_SAVE_SETTINGS_REQ_V01 0x0029
#define QMI_CTL_CONFIG_PWR_SAVE_SETTINGS_RESP_V01 0x0029
#define QMI_CTL_SET_PWR_SAVE_MODE_REQ_V01 0x002A
#define QMI_CTL_SET_PWR_SAVE_MODE_RESP_V01 0x002A
#define QMI_CTL_GET_PWR_SAVE_MODE_REQ_V01 0x002B
#define QMI_CTL_GET_PWR_SAVE_MODE_RESP_V01 0x002B
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ctl_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ctl_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define ctl_get_service_object_v01( ) \
          ctl_get_service_object_internal_v01( \
            CTL_V01_IDL_MAJOR_VERS, CTL_V01_IDL_MINOR_VERS, \
            CTL_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

