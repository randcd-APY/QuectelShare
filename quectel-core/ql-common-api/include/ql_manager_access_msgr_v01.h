#ifndef QL_MANAGER_MSGR_SERVICE_01_H
#define QL_MANAGER_MSGR_SERVICE_01_H
/**
  @file ql_manager_access_msgr_v01.h

  @brief This is the public header file which defines the ql_manager_msgr service Data structures.

  This header file defines the types and structures that were defined in
  ql_manager_msgr. It contains the constant values defined, enums, structures,
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
  

  
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon Oct 22 2018 (Spin 0)
   From IDL File: ql_manager_access_msgr_v01.idl */

/** @defgroup ql_manager_msgr_qmi_consts Constant values defined in the IDL */
/** @defgroup ql_manager_msgr_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ql_manager_msgr_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ql_manager_msgr_qmi_messages Structures sent as QMI messages */
/** @defgroup ql_manager_msgr_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ql_manager_msgr_qmi_accessor Accessor for QMI service object */
/** @defgroup ql_manager_msgr_qmi_version Constant values for versioning information */

#include <stdint.h>
#include <qmi_idl_lib.h>
#include "ql_manager_access_msgr_common_v01.h"
#include "ql_manager_access_msgr_wifi_v01.h"
#include "ql_manager_access_msgr_data_call_v01.h"
#include "ql_manager_access_msgr_network_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ql_manager_msgr_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QL_MANAGER_MSGR_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QL_MANAGER_MSGR_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QL_MANAGER_MSGR_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QL_MANAGER_MSGR_V01_MAX_MESSAGE_ID 0x0041
/**
    @}
  */


/** @addtogroup ql_manager_msgr_qmi_consts
    @{
  */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Service Message Definition*/
/** @addtogroup ql_manager_msgr_qmi_msg_ids
    @{
  */
#define QL_MANAGER_MSGR_SET_WIFI_WORK_MODE_REQ_V01 0x0001
#define QL_MANAGER_MSGR_SET_WIFI_WORK_MODE_RESP_V01 0x0001
#define QL_MANAGER_MSGR_GET_WIFI_WORK_MODE_REQ_V01 0x0002
#define QL_MANAGER_MSGR_GET_WIFI_WORK_MODE_RESP_V01 0x0002
#define QL_MANAGER_MSGR_SET_WIFI_ESSID_REQ_V01 0x0003
#define QL_MANAGER_MSGR_SET_WIFI_ESSID_RESP_V01 0x0003
#define QL_MANAGER_MSGR_GET_WIFI_ESSID_REQ_V01 0x0004
#define QL_MANAGER_MSGR_GET_WIFI_ESSID_RESP_V01 0x0004
#define QL_MANAGER_MSGR_SET_WIFI_COUNTRY_CODE_REQ_V01 0x0005
#define QL_MANAGER_MSGR_SET_WIFI_COUNTRY_CODE_RESP_V01 0x0005
#define QL_MANAGER_MSGR_GET_WIFI_COUNTRY_CODE_REQ_V01 0x0006
#define QL_MANAGER_MSGR_GET_WIFI_COUNTRY_CODE_RESP_V01 0x0006
#define QL_MANAGER_MSGR_SET_WIFI_MODE_REQ_V01 0x0007
#define QL_MANAGER_MSGR_SET_WIFI_MODE_RESP_V01 0x0007
#define QL_MANAGER_MSGR_GET_WIFI_MODE_REQ_V01 0x0008
#define QL_MANAGER_MSGR_GET_WIFI_MODE_RESP_V01 0x0008
#define QL_MANAGER_MSGR_SET_WIFI_CHANNEL_REQ_V01 0x0009
#define QL_MANAGER_MSGR_SET_WIFI_CHANNEL_RESP_V01 0x0009
#define QL_MANAGER_MSGR_GET_WIFI_CHANNEL_REQ_V01 0x000A
#define QL_MANAGER_MSGR_GET_WIFI_CHANNEL_RESP_V01 0x000A
#define QL_MANAGER_MSGR_SET_WIFI_BANDWIDTH_REQ_V01 0x000B
#define QL_MANAGER_MSGR_SET_WIFI_BANDWIDTH_RESP_V01 0x000B
#define QL_MANAGER_MSGR_GET_WIFI_BANDWIDTH_REQ_V01 0x000C
#define QL_MANAGER_MSGR_GET_WIFI_BANDWIDTH_RESP_V01 0x000C
#define QL_MANAGER_MSGR_SET_WIFI_MAX_NUM_STA_REQ_V01 0x000D
#define QL_MANAGER_MSGR_SET_WIFI_MAX_NUM_STA_RESP_V01 0x000D
#define QL_MANAGER_MSGR_GET_WIFI_MAX_NUM_STA_REQ_V01 0x000E
#define QL_MANAGER_MSGR_GET_WIFI_MAX_NUM_STA_RESP_V01 0x000E
#define QL_MANAGER_MSGR_SET_WIFI_REQUIRE_HT_REQ_V01 0x000F
#define QL_MANAGER_MSGR_SET_WIFI_REQUIRE_HT_RESP_V01 0x000F
#define QL_MANAGER_MSGR_GET_WIFI_REQUIRE_HT_REQ_V01 0x0010
#define QL_MANAGER_MSGR_GET_WIFI_REQUIRE_HT_RESP_V01 0x0010
#define QL_MANAGER_MSGR_SET_WIFI_REQUIRE_VHT_REQ_V01 0x0011
#define QL_MANAGER_MSGR_SET_WIFI_REQUIRE_VHT_RESP_V01 0x0011
#define QL_MANAGER_MSGR_GET_WIFI_REQUIRE_VHT_REQ_V01 0x0012
#define QL_MANAGER_MSGR_GET_WIFI_REQUIRE_VHT_RESP_V01 0x0012
#define QL_MANAGER_MSGR_SET_WIFI_AP_ISOLATE_REQ_V01 0x0013
#define QL_MANAGER_MSGR_SET_WIFI_AP_ISOLATE_RESP_V01 0x0013
#define QL_MANAGER_MSGR_GET_WIFI_AP_ISOLATE_REQ_V01 0x0014
#define QL_MANAGER_MSGR_GET_WIFI_AP_ISOLATE_RESP_V01 0x0014
#define QL_MANAGER_MSGR_SET_WIFI_AUTH_REQ_V01 0x0015
#define QL_MANAGER_MSGR_SET_WIFI_AUTH_RESP_V01 0x0015
#define QL_MANAGER_MSGR_GET_WIFI_AUTH_REQ_V01 0x0016
#define QL_MANAGER_MSGR_GET_WIFI_AUTH_RESP_V01 0x0016
#define QL_MANAGER_MSGR_SET_WIFI_DFS_REQ_V01 0x0017
#define QL_MANAGER_MSGR_SET_WIFI_DFS_RESP_V01 0x0017
#define QL_MANAGER_MSGR_GET_WIFI_DFS_REQ_V01 0x0018
#define QL_MANAGER_MSGR_GET_WIFI_DFS_RESP_V01 0x0018
#define QL_MANAGER_MSGR_SET_WIFI_RSSI_REQ_V01 0x0019
#define QL_MANAGER_MSGR_SET_WIFI_RSSI_RESP_V01 0x0019
#define QL_MANAGER_MSGR_GET_WIFI_RSSI_REQ_V01 0x001A
#define QL_MANAGER_MSGR_GET_WIFI_RSSI_RESP_V01 0x001A
#define QL_MANAGER_MSGR_SET_WIFI_SSID_HIDE_REQ_V01 0x001B
#define QL_MANAGER_MSGR_SET_WIFI_SSID_HIDE_RESP_V01 0x001B
#define QL_MANAGER_MSGR_GET_WIFI_SSID_HIDE_REQ_V01 0x001C
#define QL_MANAGER_MSGR_GET_WIFI_SSID_HIDE_RESP_V01 0x001C
#define QL_MANAGER_MSGR_SET_WIFI_ENABLE_REQ_V01 0x001D
#define QL_MANAGER_MSGR_SET_WIFI_ENABLE_RESP_V01 0x001D
#define QL_MANAGER_MSGR_GET_WIFI_ENABLE_REQ_V01 0x001E
#define QL_MANAGER_MSGR_GET_WIFI_ENABLE_RESP_V01 0x001E
#define QL_MANAGER_MSGR_SET_WIFI_ACTIVE_REQ_V01 0x001F
#define QL_MANAGER_MSGR_SET_WIFI_ACTIVE_RESP_V01 0x001F
#define QL_MANAGER_MSGR_SET_WIFI_ACTIVE_IND_V01 0x001F
#define QL_MANAGER_MSGR_GET_WIFI_STA_STATUS_REQ_V01 0x0020
#define QL_MANAGER_MSGR_GET_WIFI_STA_STATUS_RESP_V01 0x0020
#define QL_MANAGER_MSGR_GET_WIFI_STA_STATUS_EVENT_IND_V01 0x0020
#define QL_MANAGER_MSGR_GET_WIFI_AP_STA_INFO_REQ_V01 0x0021
#define QL_MANAGER_MSGR_GET_WIFI_AP_STA_INFO_RESP_V01 0x0021
#define QL_MANAGER_MSGR_GET_WIFI_AP_STA_INFO_IND_V01 0x0021
#define QL_MANAGER_MSGR_SET_WIFI_MAC_ACL_RULE_REQ_V01 0x0022
#define QL_MANAGER_MSGR_SET_WIFI_MAC_ACL_RULE_RESP_V01 0x0022
#define QL_MANAGER_MSGR_GET_WIFI_MAC_ACL_RULE_REQ_V01 0x0023
#define QL_MANAGER_MSGR_GET_WIFI_MAC_ACL_RULE_RESP_V01 0x0023
#define QL_MANAGER_MSGR_ADD_WIFI_MAC_ACL_REQ_V01 0x0024
#define QL_MANAGER_MSGR_ADD_WIFI_MAC_ACL_RESP_V01 0x0024
#define QL_MANAGER_MSGR_DEL_WIFI_MAC_ACL_REQ_V01 0x0025
#define QL_MANAGER_MSGR_DEL_WIFI_MAC_ACL_RESP_V01 0x0025
#define QL_MANAGER_MSGR_GET_WIFI_MAC_ACL_REQ_V01 0x0026
#define QL_MANAGER_MSGR_GET_WIFI_MAC_ACL_RESP_V01 0x0026
#define QL_MANAGER_MSGR_SET_WIFI_RESTORE_DEFAULT_CONFIG_REQ_V01 0x0027
#define QL_MANAGER_MSGR_SET_WIFI_RESTORE_DEFAULT_CONFIG_RESP_V01 0x0027
#define QL_MANAGER_MSGR_SET_DATA_CALL_UP_REQ_V01 0x0028
#define QL_MANAGER_MSGR_SET_DATA_CALL_UP_RESP_V01 0x0028
#define QL_MANAGER_MSGR_SET_DATA_CALL_DOWN_REQ_V01 0x0029
#define QL_MANAGER_MSGR_SET_DATA_CALL_DOWN_RESP_V01 0x0029
#define QL_MANAGER_MSGR_GET_DATA_CALL_INFO_REQ_V01 0x002A
#define QL_MANAGER_MSGR_GET_DATA_CALL_INFO_RESP_V01 0x002A
#define QL_MANAGER_MSGR_DATA_CALL_STATUS_EVENT_IND_V01 0x002B
#define QL_MANAGER_MSGR_SET_DATA_CALL_APN_REQ_V01 0x002C
#define QL_MANAGER_MSGR_SET_DATA_CALL_APN_RESP_V01 0x002C
#define QL_MANAGER_MSGR_GET_DATA_CALL_APN_REQ_V01 0x002D
#define QL_MANAGER_MSGR_GET_DATA_CALL_APN_RESP_V01 0x002D
#define QL_MANAGER_MSGR_GET_DATA_CALL_APN_LIST_REQ_V01 0x002E
#define QL_MANAGER_MSGR_GET_DATA_CALL_APN_LIST_RESP_V01 0x002E
#define QL_MANAGER_MSGR_DEL_DATA_CALL_APN_REQ_V01 0x002F
#define QL_MANAGER_MSGR_DLE_DATA_CALL_APN_RESP_V01 0x002F
#define QL_MANAGER_MSGR_ADD_DATA_CALL_APN_REQ_V01 0x0030
#define QL_MANAGER_MSGR_ADD_DATA_CALL_APN_RESP_V01 0x0030
#define QL_MANAGER_MSGR_SET_DATA_CALL_DEFAULT_PROFILE_REQ_V01 0x0031
#define QL_MANAGER_MSGR_SET_DATA_CALL_DEFAULT_PROFILE_RESP_V01 0x0031
#define QL_MANAGER_MSGR_GET_DATA_CALL_DEFAULT_PROFILE_REQ_V01 0x0032
#define QL_MANAGER_MSGR_GET_DATA_CALL_DEFAULT_PROFILE_RESP_V01 0x0032
#define QL_MANAGER_MSGR_ADD_NETWORK_DMZ_REQ_V01 0x0033
#define QL_MANAGER_MSGR_ADD_NETWORK_DMZ_RESP_V01 0x0033
#define QL_MANAGER_MSGR_DEL_NETWORK_DMZ_REQ_V01 0x0034
#define QL_MANAGER_MSGR_DEL_NETWORK_DMZ_RESP_V01 0x0034
#define QL_MANAGER_MSGR_GET_NETWORK_DMZ_REQ_V01 0x0035
#define QL_MANAGER_MSGR_GET_NETWORK_DMZ_RESP_V01 0x0035
#define QL_MANAGER_MSGR_SET_NETWORK_SGMII_ENABLE_REQ_V01 0x0036
#define QL_MANAGER_MSGR_SET_NETWORK_SGMII_ENABLE_RESP_V01 0x0036
#define QL_MANAGER_MSGR_GET_NETWORK_SGMII_ENABLE_REQ_V01 0x0037
#define QL_MANAGER_MSGR_GET_NETWORK_SGMII_ENABLE_RESP_V01 0x0037
#define QL_MANAGER_MSGR_SET_NETWORK_SGMII_CONFIG_REQ_V01 0x0038
#define QL_MANAGER_MSGR_SET_NETWORK_SGMII_CONFIG_RESP_V01 0x0038
#define QL_MANAGER_MSGR_GET_NETWORK_SGMII_CONFIG_REQ_V01 0x0039
#define QL_MANAGER_MSGR_GET_NETWORK_SGMII_CONFIG_RESP_V01 0x0039
#define QL_MANAGER_MSGR_GET_NETWORK_SGMII_INFO_REQ_V01 0x003A
#define QL_MANAGER_MSGR_GET_NETWORK_SGMII_INFO_RESP_V01 0x003A
#define QL_MANAGER_MSGR_SET_NETWORK_LAN_CONFIG_REQ_V01 0x003B
#define QL_MANAGER_MSGR_SET_NETWORK_LAN_CONFIG_RESP_V01 0x003B
#define QL_MANAGER_MSGR_GET_NETWORK_LAN_CONFIG_REQ_V01 0x003C
#define QL_MANAGER_MSGR_GET_NETWORK_LAN_CONFIG_RESP_V01 0x003C
#define QL_MANAGER_MSGR_ADD_NETWORK_SNAT_REQ_V01 0x003D
#define QL_MANAGER_MSGR_ADD_NETWORK_SNAT_RESP_V01 0x003D
#define QL_MANAGER_MSGR_DEL_NETWORK_SNAT_REQ_V01 0x003E
#define QL_MANAGER_MSGR_DEL_NETWORK_SNAT_RESP_V01 0x003E
#define QL_MANAGER_MSGR_GET_NETWORK_SNAT_REQ_V01 0x003F
#define QL_MANAGER_MSGR_GET_NETWORK_SNAT_RESP_V01 0x003F
#define QL_MANAGER_MSGR_SET_NETWORK_ETH_MODE_REQ_V01 0x0040
#define QL_MANAGER_MSGR_SET_NETWORK_ETH_MODE_RESP_V01 0x0040
#define QL_MANAGER_MSGR_GET_NETWORK_ETH_MODE_REQ_V01 0x0041
#define QL_MANAGER_MSGR_GET_NETWORK_ETH_MODE_RESP_V01 0x0041
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ql_manager_msgr_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ql_manager_msgr_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define ql_manager_msgr_get_service_object_v01( ) \
          ql_manager_msgr_get_service_object_internal_v01( \
            QL_MANAGER_MSGR_V01_IDL_MAJOR_VERS, QL_MANAGER_MSGR_V01_IDL_MINOR_VERS, \
            QL_MANAGER_MSGR_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

