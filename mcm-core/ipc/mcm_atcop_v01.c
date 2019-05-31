/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ A T C O P _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_atcop service Data structures.

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2
   It was generated on: Tue Dec  3 2013 (Spin 0)
   From IDL File: mcm_atcop_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_atcop_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"

/*Type Definitions*/
/*Message Definitions*/
static const uint8_t mcm_atcop_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_atcop_req_msg_v01, cmd_req),
  ((MCM_ATCOP_MAX_REQ_MSG_SIZE_V01) & 0xFF), ((MCM_ATCOP_MAX_REQ_MSG_SIZE_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_atcop_req_msg_v01, cmd_len)
};

static const uint8_t mcm_atcop_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_atcop_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_atcop_resp_msg_v01, cmd_resp) - QMI_IDL_OFFSET8(mcm_atcop_resp_msg_v01, cmd_resp_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_atcop_resp_msg_v01, cmd_resp),
  ((MCM_ATCOP_MAX_RESP_MSG_SIZE_V01) & 0xFF), ((MCM_ATCOP_MAX_RESP_MSG_SIZE_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_atcop_resp_msg_v01, resp_len) - QMI_IDL_OFFSET16RELATIVE(mcm_atcop_resp_msg_v01, resp_len_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_atcop_resp_msg_v01, resp_len)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry mcm_atcop_message_table_v01[] = {
  {sizeof(mcm_atcop_req_msg_v01), mcm_atcop_req_msg_data_v01},
  {sizeof(mcm_atcop_resp_msg_v01), mcm_atcop_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mcm_atcop_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_atcop_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_atcop_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mcm_atcop_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(mcm_atcop_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  mcm_atcop_message_table_v01,
  mcm_atcop_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mcm_atcop_service_command_messages_v01[] = {
  {MCM_ATCOP_REQ_V01, QMI_IDL_TYPE16(0, 0), 523}
};

static const qmi_idl_service_message_table_entry mcm_atcop_service_response_messages_v01[] = {
  {MCM_ATCOP_RESP_V01, QMI_IDL_TYPE16(0, 1), 4118}
};

/*Service Object*/
struct qmi_idl_service_object mcm_atcop_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x403,
  4118,
  { sizeof(mcm_atcop_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_atcop_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { mcm_atcop_service_command_messages_v01, mcm_atcop_service_response_messages_v01, NULL},
  &mcm_atcop_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mcm_atcop_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MCM_ATCOP_V01_IDL_MAJOR_VERS != idl_maj_version || MCM_ATCOP_V01_IDL_MINOR_VERS != idl_min_version
       || MCM_ATCOP_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&mcm_atcop_qmi_idl_service_object_v01;
}

