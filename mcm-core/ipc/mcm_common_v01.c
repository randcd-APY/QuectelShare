/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ C O M M O N _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_common service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2
   It was generated on: Tue Sep  3 2013 (Spin 0)
   From IDL File: mcm_common_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_common_v01.h"


/*Type Definitions*/
static const uint8_t mcm_response_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_response_t_v01, result),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_response_t_v01, error),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/* Type Table */
static const qmi_idl_type_table_entry  mcm_common_type_table_v01[] = {
  {sizeof(mcm_response_t_v01), mcm_response_t_data_v01}
};

/* Message Table */
/* No Messages Defined in IDL */

/* Range Table */
/* Predefine the Type Table Object */
const qmi_idl_type_table_object mcm_common_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_common_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object mcm_common_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_common_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  0,
  1,
  mcm_common_type_table_v01,
  NULL,
  mcm_common_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

