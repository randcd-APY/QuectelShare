#ifndef QL_MANAGER_ACCESS_MSGR_DATA_CALL_SERVICE_01_H
#define QL_MANAGER_ACCESS_MSGR_DATA_CALL_SERVICE_01_H
/**
  @file ql_manager_access_msgr_data_call_v01.h

  @brief This is the public header file which defines the ql_manager_access_msgr_data_call service Data structures.

  This header file defines the types and structures that were defined in
  ql_manager_access_msgr_data_call. It contains the constant values defined, enums, structures,
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
   It was generated on: Fri May 25 2018 (Spin 0)
   From IDL File: ql_manager_access_msgr_data_call_v01.idl */

/** @defgroup ql_manager_access_msgr_data_call_qmi_consts Constant values defined in the IDL */
/** @defgroup ql_manager_access_msgr_data_call_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ql_manager_access_msgr_data_call_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ql_manager_access_msgr_data_call_qmi_messages Structures sent as QMI messages */
/** @defgroup ql_manager_access_msgr_data_call_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ql_manager_access_msgr_data_call_qmi_accessor Accessor for QMI service object */
/** @defgroup ql_manager_access_msgr_data_call_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "ql_manager_access_msgr_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ql_manager_access_msgr_data_call_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_DATA_CALL_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_DATA_CALL_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_DATA_CALL_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_enums
    @{
  */
typedef enum {
  DATA_CALL_PROFILE_TYPE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_CALL_PROFILE_TYPE_3GPP_V01 = 0, 
  DATA_CALL_PROFILE_TYPE_3GPP2_V01 = 1, 
  DATA_CALL_PROFILE_TYPE_EPC_V01 = 2, 
  DATA_CALL_PROFILE_TYPE_INVALID_V01 = 0xff, 
  DATA_CALL_PROFILE_TYPE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_call_profile_type_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_enums
    @{
  */
typedef enum {
  DATA_CALL_IP_FAMILY_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_CALL_IPV4_V01 = 0, 
  DATA_CALL_IPV6_V01 = 1, 
  DATA_CALL_IPV4V6_V01 = 2, 
  DATA_CALL_IP_FAMILY_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_call_ip_family_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_enums
    @{
  */
typedef enum {
  DATA_CALL_PDP_TYPE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_CALL_PDP_TYPE_IPV4_V01 = 0, 
  DATA_CALL_PDP_TYPE_PPP_V01 = 1, 
  DATA_CALL_PDP_TYPE_IPV6_V01 = 2, 
  DATA_CALL_PDP_TYPE_IPV4V6_V01 = 3, 
  DATA_CALL_PDP_TYPE_INVALID_V01 = 0xff, 
  DATA_CALL_PDP_TYPE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_call_pdp_type_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_enums
    @{
  */
typedef enum {
  DATA_CALL_PROFILE_AUTH_PROTOCOL_TYPE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_CALL_AUTH_PROTOCOL_NONE_V01 = 0, 
  DATA_CALL_AUTH_PROTOCOL_PAP_V01 = 1, 
  DATA_CALL_AUTH_PROTOCOL_CHAP_V01 = 2, 
  DATA_CALL_AUTH_PROTOCOL_PAP_CHAP_V01 = 3, 
  DATA_CALL_AUTH_PROTOCOL_INVALID_V01 = 0xff, 
  DATA_CALL_PROFILE_AUTH_PROTOCOL_TYPE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_call_profile_auth_protocol_type_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_enums
    @{
  */
typedef enum {
  DATA_CALL_QCMAP_TECH_PREF_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_CALL_QCMAP_TECH_PREF_ANY_V01 = 0, 
  DATA_CALL_QCMAP_TECH_PREF_UTMS_V01 = 1, 
  DATA_CALL_QCMAP_TECH_PREF_CDMA_V01 = 2, 
  DATA_CALL_QCMAP_TECH_PREF_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}data_call_qcmap_tech_pref_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_index;

  /* Mandatory */
  uint8_t ip_family;

  /* Mandatory */
  uint8_t reconnect;

  /* Mandatory */
  char apn_name[151];

  /* Mandatory */
  char username[128];

  /* Mandatory */
  char passwd[128];
}ql_manager_msgr_set_data_call_up_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_data_call_up_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_index;

  /* Mandatory */
  uint8_t ip_family;
}ql_manager_msgr_set_data_call_down_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_data_call_down_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_index;

  /* Mandatory */
  uint8_t ip_family;
}ql_manager_msgr_get_data_call_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t profile_index;

  /* Mandatory */
  uint8_t state;

  /* Mandatory */
  uint8_t ip_family;

  /* Mandatory */
  uint8_t reconnect;

  /* Mandatory */
  char name[16];

  /* Mandatory */
  char ip_address[64];

  /* Mandatory */
  char gateway_address[64];

  /* Mandatory */
  char pri_dns[64];

  /* Mandatory */
  char sec_dns[64];

  /* Mandatory */
  uint32_t pkts_tx;

  /* Mandatory */
  uint32_t pkts_rx;

  /* Mandatory */
  uint64_t bytes_tx;

  /* Mandatory */
  uint64_t bytes_rx;

  /* Mandatory */
  uint32_t pkts_dropped_tx;

  /* Mandatory */
  uint32_t pkts_dropped_rx;
}ql_manager_msgr_get_data_call_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_index;

  /* Mandatory */
  uint8_t ip_family;

  /* Mandatory */
  uint8_t state;

  /* Mandatory */
  char name[16];

  /* Mandatory */
  int32_t reason_type;

  /* Mandatory */
  int32_t reason_code;

  /* Mandatory */
  char ip_address[64];

  /* Mandatory */
  char gateway_address[64];

  /* Mandatory */
  char pri_dns[64];

  /* Mandatory */
  char sec_dns[64];
}ql_mgmager_msgr_data_call_status_event_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_type;

  /* Mandatory */
  uint8_t profile_index;

  /* Mandatory */
  uint8_t pdp_type;

  /* Mandatory */
  uint8_t auth_protocol;

  /* Mandatory */
  char apn_name[151];

  /* Mandatory */
  char username[128];

  /* Mandatory */
  char passwd[128];
}ql_manager_msgr_set_data_call_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_data_call_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_type;

  /* Mandatory */
  uint8_t profile_index;
}ql_manager_msgr_get_data_call_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t profile_type;

  /* Mandatory */
  uint8_t profile_index;

  /* Mandatory */
  uint8_t pdp_type;

  /* Mandatory */
  uint8_t auth_protocol;

  /* Mandatory */
  char apn_name[151];

  /* Mandatory */
  char username[128];

  /* Mandatory */
  char passwd[128];
}ql_manager_msgr_get_data_call_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_type;
}ql_manager_msgr_get_data_call_apn_list_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t len;

  /* Mandatory */
  char profile_index_lists[256];
}ql_manager_msgr_get_data_call_apn_list_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_type;

  /* Mandatory */
  uint8_t profile_index;
}ql_manager_msgr_del_data_call_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_del_data_call_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t profile_type;

  /* Mandatory */
  uint8_t pdp_type;

  /* Mandatory */
  uint8_t auth_protocol;

  /* Mandatory */
  char apn_name[151];

  /* Mandatory */
  char username[128];

  /* Mandatory */
  char passwd[128];
}ql_manager_msgr_add_data_call_apn_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t profile_index;
}ql_manager_msgr_add_data_call_apn_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t qcmap_profile_index;

  /* Mandatory */
  uint8_t tech_pref;

  /* Mandatory */
  uint8_t ip_family;
}ql_manager_msgr_set_data_call_default_profile_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_data_call_default_profile_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_data_call_default_profile_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_data_call_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t qcmap_profile_index;

  /* Mandatory */
  uint8_t tech_pref;

  /* Mandatory */
  uint8_t ip_family;
}ql_manager_msgr_get_data_call_default_profile_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object ql_manager_access_msgr_data_call_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

