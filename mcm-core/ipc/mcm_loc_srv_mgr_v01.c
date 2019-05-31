/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ L O C _ S R V _ M G R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_loc_srv_mgr service Data structures.

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.7
   It was generated on: Fri Mar 14 2014 (Spin 0)
   From IDL File: mcm_loc_srv_mgr_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_loc_srv_mgr_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"

/*Type Definitions*/
/*Message Definitions*/
/*
 * mcm_loc_srv_mgr_unload_req_msg is empty
 * static const uint8_t mcm_loc_srv_mgr_unload_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_loc_srv_mgr_unload_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_srv_mgr_unload_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry mcm_loc_srv_mgr_message_table_v01[] = {
  {sizeof(mcm_loc_srv_mgr_unload_req_msg_v01), 0},
  {sizeof(mcm_loc_srv_mgr_unload_resp_msg_v01), mcm_loc_srv_mgr_unload_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mcm_loc_srv_mgr_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_loc_srv_mgr_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_loc_srv_mgr_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mcm_loc_srv_mgr_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(mcm_loc_srv_mgr_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  mcm_loc_srv_mgr_message_table_v01,
  mcm_loc_srv_mgr_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mcm_loc_srv_mgr_service_command_messages_v01[] = {
  {MCM_LOC_SRV_MGR_UNLODE_REQ_V01, QMI_IDL_TYPE16(0, 0), 0}
};

static const qmi_idl_service_message_table_entry mcm_loc_srv_mgr_service_response_messages_v01[] = {
  {MCM_LOC_SRV_MGR_UNLODE_RESP_V01, QMI_IDL_TYPE16(0, 1), 11}
};

/*Service Object*/
struct qmi_idl_service_object mcm_loc_srv_mgr_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x409,
  11,
  { sizeof(mcm_loc_srv_mgr_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_loc_srv_mgr_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { mcm_loc_srv_mgr_service_command_messages_v01, mcm_loc_srv_mgr_service_response_messages_v01, NULL},
  &mcm_loc_srv_mgr_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mcm_loc_srv_mgr_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MCM_LOC_SRV_MGR_V01_IDL_MAJOR_VERS != idl_maj_version || MCM_LOC_SRV_MGR_V01_IDL_MINOR_VERS != idl_min_version
       || MCM_LOC_SRV_MGR_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&mcm_loc_srv_mgr_qmi_idl_service_object_v01;
}

