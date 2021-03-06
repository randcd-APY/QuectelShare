#ifndef QL_MANAGER_ACCESS_MSGR_COMMON_SERVICE_01_H
#define QL_MANAGER_ACCESS_MSGR_COMMON_SERVICE_01_H
/**
  @file ql_manager_access_msgr_common_v01.h

  @brief This is the public header file which defines the ql_manager_access_msgr_common service Data structures.

  This header file defines the types and structures that were defined in
  ql_manager_access_msgr_common. It contains the constant values defined, enums, structures,
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
   From IDL File: ql_manager_access_msgr_common_v01.idl */

/** @defgroup ql_manager_access_msgr_common_qmi_consts Constant values defined in the IDL */
/** @defgroup ql_manager_access_msgr_common_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ql_manager_access_msgr_common_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ql_manager_access_msgr_common_qmi_messages Structures sent as QMI messages */
/** @defgroup ql_manager_access_msgr_common_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ql_manager_access_msgr_common_qmi_accessor Accessor for QMI service object */
/** @defgroup ql_manager_access_msgr_common_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ql_manager_access_msgr_common_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_COMMON_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_COMMON_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_COMMON_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_common_qmi_enums
    @{
  */
typedef enum {
  ERROR_TYPE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMAM_ERR_NONE_V01 = 0, 
  QMAM_ERR_ERROR_V01 = 1, 
  QMAM_ERR_INVALID_PARAMETERS_V01 = 2, 
  QMAM_ERR_NOT_SUPPORT_V01 = 3, 
  QMAM_ERR_SYSTEM_ERROR_V01 = 4, 
  QMAM_ERR_QMI_ERROR_V01 = 5, 
  QMAM_ERR_QCMAP_MANAGER_FAILED_V01 = 6, 
  QMAM_ERR_NOT_ENOUGH_RESOURCES_V01 = 7, 
  QMAM_ERR_NOT_FOUND_RESOURCES_V01 = 8, 
  QMAM_ERR_MALLOC_FAILED_V01 = 9, 
  ERROR_TYPE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}error_type_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_common_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t result_code;

  uint16_t error_code;
}result_v01;  /* Type */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object ql_manager_access_msgr_common_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

