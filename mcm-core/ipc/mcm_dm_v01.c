/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ D M _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_dm service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5
   It was generated on: Wed Sep 11 2013 (Spin 0)
   From IDL File: mcm_dm_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_dm_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/*
 * mcm_dm_get_radio_mode_req_msg is empty
 * static const uint8_t mcm_dm_get_radio_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_dm_get_radio_mode_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_dm_get_radio_mode_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_dm_get_radio_mode_resp_msg_v01, radio_mode) - QMI_IDL_OFFSET8(mcm_dm_get_radio_mode_resp_msg_v01, radio_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_dm_get_radio_mode_resp_msg_v01, radio_mode)
};

static const uint8_t mcm_dm_set_radio_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_dm_set_radio_mode_req_msg_v01, radio_mode)
};

static const uint8_t mcm_dm_set_radio_mode_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_dm_set_radio_mode_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_dm_set_radio_mode_resp_msg_v01, no_change) - QMI_IDL_OFFSET8(mcm_dm_set_radio_mode_resp_msg_v01, no_change_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_dm_set_radio_mode_resp_msg_v01, no_change)
};

static const uint8_t mcm_dm_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_dm_event_register_req_msg_v01, register_radio_mode_changed_event) - QMI_IDL_OFFSET8(mcm_dm_event_register_req_msg_v01, register_radio_mode_changed_event_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_dm_event_register_req_msg_v01, register_radio_mode_changed_event)
};

static const uint8_t mcm_dm_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_dm_event_register_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_dm_radio_mode_changed_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_dm_radio_mode_changed_event_ind_msg_v01, radio_mode) - QMI_IDL_OFFSET8(mcm_dm_radio_mode_changed_event_ind_msg_v01, radio_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_dm_radio_mode_changed_event_ind_msg_v01, radio_mode)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry mcm_dm_message_table_v01[] = {
  {sizeof(mcm_dm_get_radio_mode_req_msg_v01), 0},
  {sizeof(mcm_dm_get_radio_mode_resp_msg_v01), mcm_dm_get_radio_mode_resp_msg_data_v01},
  {sizeof(mcm_dm_set_radio_mode_req_msg_v01), mcm_dm_set_radio_mode_req_msg_data_v01},
  {sizeof(mcm_dm_set_radio_mode_resp_msg_v01), mcm_dm_set_radio_mode_resp_msg_data_v01},
  {sizeof(mcm_dm_event_register_req_msg_v01), mcm_dm_event_register_req_msg_data_v01},
  {sizeof(mcm_dm_event_register_resp_msg_v01), mcm_dm_event_register_resp_msg_data_v01},
  {sizeof(mcm_dm_radio_mode_changed_event_ind_msg_v01), mcm_dm_radio_mode_changed_event_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
const qmi_idl_type_table_object mcm_dm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
const qmi_idl_type_table_object *mcm_dm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_dm_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object mcm_dm_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(mcm_dm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  mcm_dm_message_table_v01,
  mcm_dm_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

