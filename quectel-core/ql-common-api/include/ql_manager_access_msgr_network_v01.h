#ifndef QL_MANAGER_ACCESS_MSGR_NETWORK_SERVICE_01_H
#define QL_MANAGER_ACCESS_MSGR_NETWORK_SERVICE_01_H
/**
  @file ql_manager_access_msgr_network_v01.h

  @brief This is the public header file which defines the ql_manager_access_msgr_network service Data structures.

  This header file defines the types and structures that were defined in
  ql_manager_access_msgr_network. It contains the constant values defined, enums, structures,
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
   From IDL File: ql_manager_access_msgr_network_v01.idl */

/** @defgroup ql_manager_access_msgr_network_qmi_consts Constant values defined in the IDL */
/** @defgroup ql_manager_access_msgr_network_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ql_manager_access_msgr_network_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ql_manager_access_msgr_network_qmi_messages Structures sent as QMI messages */
/** @defgroup ql_manager_access_msgr_network_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ql_manager_access_msgr_network_qmi_accessor Accessor for QMI service object */
/** @defgroup ql_manager_access_msgr_network_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "ql_manager_access_msgr_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ql_manager_access_msgr_network_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_NETWORK_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_NETWORK_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_NETWORK_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */


/** @addtogroup ql_manager_access_msgr_network_qmi_consts
    @{
  */
#define QMAM_MAX_SNAT_CNT_V01 32
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_enums
    @{
  */
typedef enum {
  ETHERNET_SPEED_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  ETHERNET_SPEED_10_V01 = 0, 
  ETHERNET_SPEED_100_V01 = 1, 
  ETHERNET_SPEED_1000_V01 = 2, 
  ETHERNET_SPEED_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ethernet_speed_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_enums
    @{
  */
typedef enum {
  ETHERNET_DUPLEX_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  ETHERNET_DUPLEX_HALF_V01 = 0, 
  ETHERNET_DUPLEX_FULL_V01 = 1, 
  ETHERNET_DUPLEX_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ethernet_duplex_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_enums
    @{
  */
typedef enum {
  SNAT_PROTOCOL_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNAT_PROTOCOL_TCP_V01 = 6, 
  SNAT_PROTOCOL_UDP_V01 = 17, 
  SNAT_PROTOCOL_TCP_UDP_V01 = 253, 
  SNAT_PROTOCOL_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}snat_protocol_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_enums
    @{
  */
typedef enum {
  ETHERNET_MODE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  ETH_LAN_ROUTE_V01 = 0, 
  ETH_WAN_ROUTE_V01 = 1, 
  ETHERNET_MODE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ethernet_mode_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint32_t ip;
}ql_manager_msgr_add_network_dmz_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_add_network_dmz_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_del_network_dmz_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_del_network_dmz_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_network_dmz_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint32_t ip;
}ql_manager_msgr_get_network_dmz_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t state;
}ql_manager_msgr_set_network_sgmii_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_network_sgmii_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_network_sgmii_enable_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t state;
}ql_manager_msgr_get_network_sgmii_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t autoneg;

  /* Mandatory */
  uint8_t speed;

  /* Mandatory */
  uint8_t duplex;
}ql_manager_msgr_set_network_sgmii_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_network_sgmii_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_network_sgmii_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t autoneg;

  /* Mandatory */
  uint8_t speed;

  /* Mandatory */
  uint8_t duplex;
}ql_manager_msgr_get_network_sgmii_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_network_sgmii_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t state;

  /* Mandatory */
  uint8_t running;

  /* Mandatory */
  uint8_t autoneg;

  /* Mandatory */
  uint8_t speed;

  /* Mandatory */
  uint8_t duplex;

  /* Mandatory */
  uint64_t tx_bytes;

  /* Mandatory */
  uint64_t tx_packets;

  /* Mandatory */
  uint64_t rx_bytes;

  /* Mandatory */
  uint64_t rx_packets;
}ql_manager_msgr_get_network_sgmii_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t gw_ip;

  uint32_t netmask;

  uint8_t enable_dhcp;

  uint32_t dhcp_start_ip;

  uint32_t dhcp_end_ip;

  uint32_t lease_time;
}ql_manager_msgr_network_lan_config_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  ql_manager_msgr_network_lan_config_v01 lan_config;
}ql_manager_msgr_set_network_lan_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_network_lan_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_network_lan_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  ql_manager_msgr_network_lan_config_v01 lan_config;
}ql_manager_msgr_get_network_lan_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t private_ip;

  uint16_t private_port;

  uint16_t global_port;

  uint8_t protocol;
}ql_manager_msgr_sant_confg_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  ql_manager_msgr_sant_confg_v01 sant_config;
}ql_manager_msgr_add_network_snat_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_add_network_snat_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  ql_manager_msgr_sant_confg_v01 sant_config;
}ql_manager_msgr_del_network_snat_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_network_snat_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_del_network_snat_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint32_t cnt;

  /* Mandatory */
  ql_manager_msgr_sant_confg_v01 snat_config[QMAM_MAX_SNAT_CNT_V01];
}ql_manager_msgr_get_network_snat_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  ethernet_mode_e_v01 mode;
}ql_manager_msgr_set_network_eth_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_network_eth_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_network_eth_mode_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_network_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  ethernet_mode_e_v01 mode;
}ql_manager_msgr_get_network_eth_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object ql_manager_access_msgr_network_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

