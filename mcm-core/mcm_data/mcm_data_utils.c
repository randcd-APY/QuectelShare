/**
  @file
  mcm_data_utils.c

  @brief
  This file provides client functionality of Mobile Connection Manager(mcm) Data Services APIs.

  @details
  mcm_data APIs are used for general data call control, data call bring-up
  and tear-down, data call configuration purposes.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/13   vb      Initial module.

===========================================================================*/

#include "mcm_data_utils.h"
#include <stdarg.h>

long int isvalid_callid_calltbl_i
(
  const void *first,
  const void *second
)
{
  long int ret;
  mcm_data_call_table_entry *call_entry_totest, *call_entry_inlist;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_entry_totest = (mcm_data_call_table_entry *)first;
  call_entry_inlist = (mcm_data_call_table_entry *)second;

  if (is_call_valid_i(call_entry_inlist) == TRUE &&
      call_entry_inlist->call_id == call_entry_totest->call_id)
  {
    ret = MCM_DATA_SUCCESS;
    goto BAIL;
  }
  ret = MCM_DATA_ERROR;
  BAIL:
  return ret;
}

long int isvalid_qmi_clnt_hdnl_calltbl_i
(
  const void *first,
  const void *second
)
{
  long int ret;
  mcm_data_call_table_entry *call_entry_totest, *call_entry_inlist;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_entry_totest = (mcm_data_call_table_entry *)first;
  call_entry_inlist = (mcm_data_call_table_entry *)second;

  if (is_call_valid_i(call_entry_inlist) == TRUE &&
      call_entry_inlist->qmi_cli_handle == call_entry_totest->qmi_cli_handle)
  {
    ret = MCM_DATA_SUCCESS;
    goto BAIL;
  }
  ret = MCM_DATA_ERROR;
  BAIL:
  return ret;
}


long int isvalid_callid_txntbl_i
(
  const void *first,
  const void *second
)
{
  long int ret = 1;

  mcm_data_txn_table_entry *txn_entry_totest, *txn_entry_inlist;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  txn_entry_totest = (mcm_data_txn_table_entry *) first;
  txn_entry_inlist = (mcm_data_txn_table_entry *) second;

  if (is_txn_valid_i(txn_entry_inlist) == TRUE &&
      txn_entry_inlist->call_id == txn_entry_totest->call_id)
  {
    ret = MCM_DATA_SUCCESS;
    goto BAIL;
  }
  ret = MCM_DATA_ERROR;
  BAIL:
  return ret;
}

long int isvalid_qmi_clnt_hdnl_txntbl_i
(
  const void *first,
  const void *second
)
  {
    long int ret = 1;

    mcm_data_txn_table_entry *txn_entry_totest, *txn_entry_inlist;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    txn_entry_totest = (mcm_data_txn_table_entry *) first;
    txn_entry_inlist = (mcm_data_txn_table_entry *) second;

    if (is_txn_valid_i(txn_entry_inlist) == TRUE &&
        txn_entry_inlist->qmi_cli_handle == txn_entry_totest->qmi_cli_handle)
    {
      ret = MCM_DATA_SUCCESS;
      goto BAIL;
    }
    ret = MCM_DATA_ERROR;
    BAIL:
    return ret;
  }



long int txn_entry_callid_start_call_i
(
  const void *first,
  const void *second
)
{
  long int ret = 1;

  mcm_data_txn_table_entry *txn_entry_totest, *txn_entry_inlist;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  txn_entry_totest = (mcm_data_txn_table_entry *) first;
  txn_entry_inlist = (mcm_data_txn_table_entry *) second;

  if (is_txn_valid_i(txn_entry_inlist) == TRUE &&
      txn_entry_inlist->api_type == 1 &&
      txn_entry_inlist->call_id == txn_entry_totest->call_id)
  {
    ret = MCM_DATA_SUCCESS;
    goto BAIL;
  }
  ret = MCM_DATA_ERROR;
  BAIL:
  return ret;
}


long int txn_entry_callid_stop_call_i
(
  const void *first,
  const void *second
)
{
  long int ret = 1;

  mcm_data_txn_table_entry *txn_entry_totest, *txn_entry_inlist;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  txn_entry_totest = (mcm_data_txn_table_entry *) first;
  txn_entry_inlist = (mcm_data_txn_table_entry *) second;

  if (is_txn_valid_i(txn_entry_inlist) == TRUE &&
      txn_entry_inlist->api_type == 2 &&
      txn_entry_inlist->call_id == txn_entry_totest->call_id)
  {
    ret = MCM_DATA_SUCCESS;
    goto BAIL;
  }
  ret = MCM_DATA_ERROR;
  BAIL:
  return ret;
}

mcm_data_call_table_entry* get_calltbl_entry_i
(
  uint32_t call_id
)
{
  mcm_data_call_table_entry call_entry;
  ds_dll_el_t *call_table_node;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&call_entry, 0x00, sizeof(mcm_data_call_table_entry));
  call_entry.call_id = call_id;

  MCM_DATA_LOG_INFO("Searching for call entry with call_id %d",
                    call_entry.call_id);

  /* Check if call_id  entered is valid. */
  call_table_node = ds_dll_search(get_call_tbl_head(),
                                  (const void *) &call_entry,
                                  isvalid_callid_calltbl_i);
  if (call_table_node == NULL)
  {
    MCM_DATA_LOG_ERROR("Call entry not found with given call_id %d",
                       call_entry.call_id);
    return NULL;
  }

  return (mcm_data_call_table_entry *) (call_table_node->data);

}

mcm_data_txn_table_entry* get_txntbl_entry_i
(
  uint32_t call_id,
  condition condtion
)
{
  mcm_data_txn_table_entry txn_entry;
  ds_dll_el_t *txn_table_node;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&txn_entry, 0x00, sizeof(mcm_data_txn_table_entry));
  txn_entry.call_id = call_id;

  switch(condtion)
  {
    case ALL: MCM_DATA_LOG_INFO("Searching for txn entry with call_id %d",
                                txn_entry.call_id);
                      txn_table_node = ds_dll_search(get_txn_tbl_head(),
                                                     (const void *) &txn_entry,
                                                     isvalid_callid_txntbl_i);
                                       break;
    case START: MCM_DATA_LOG_INFO("Searching for Start Call txn entry with call_id %d",
                                  txn_entry.call_id);
                           txn_table_node = ds_dll_search(get_txn_tbl_head(),
                                                          (const void *) &txn_entry,
                                                          txn_entry_callid_start_call_i);
                                       break;
    case STOP: MCM_DATA_LOG_INFO("Searching for Stop Call txn entry with call_id %d",
                                 txn_entry.call_id);
                         txn_table_node = ds_dll_search(get_txn_tbl_head(),
                                                        (const void *) &txn_entry,
                                                        txn_entry_callid_stop_call_i);
                                       break;
    default: MCM_DATA_LOG_ERROR("%s", "Invalid delete condition");
  }

  /* Check if call_id  entered is valid. */
  if (txn_table_node == NULL)
  {
    MCM_DATA_LOG_ERROR("Txn entry with given callid [%d]not found",
                       txn_entry.call_id);
    return NULL;
  }

  return (mcm_data_txn_table_entry *) (txn_table_node->data);
}

int32_t del_txntbl_entry_callid_i
(
  uint32_t call_id,
  condition del_condtion
)
{
  ds_dll_el_t *txn_table_node;
  ds_dll_el_t *txn_table_tail;
  mcm_data_txn_table_entry txn_entry;
  mcm_data_txn_table_entry *txn_entry_temp;
  const void *dummy = NULL;
  int32_t rval;
  ds_dll_comp_f comp_func;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  txn_table_node = NULL;
  txn_table_tail = NULL;

  memset(&txn_entry, 0x00, sizeof(mcm_data_txn_table_entry));
  txn_entry.call_id = call_id;

  /*Find tail of transaction table list*/
  txn_table_node = ds_dll_next(get_txn_tbl_head(), &dummy);
  while (txn_table_node != NULL)
  {
    txn_table_tail = txn_table_node;
    txn_table_node = ds_dll_next(txn_table_node, &dummy);
  }
  switch(del_condtion)
  {
    case ALL: comp_func = isvalid_callid_txntbl_i;
              MCM_DATA_LOG_INFO("Searching all Txn table entries with call id [%d]",
                                txn_entry.call_id);
                                       break;
    case START: comp_func = txn_entry_callid_start_call_i;
                MCM_DATA_LOG_INFO("Searching Txn table start entries with call id [%d]",
                                  txn_entry.call_id);
                                       break;
    case STOP: comp_func = txn_entry_callid_stop_call_i;
               MCM_DATA_LOG_INFO("Searching Txn table stop entries with call id [%d]",
                                 txn_entry.call_id);
                                       break;
    default: MCM_DATA_LOG_ERROR("%s", "Invalid delete condition");
                 comp_func = isvalid_callid_txntbl_i;
  }

  do
  {
    txn_table_node = ds_dll_delete(get_txn_tbl_head(),
                                   &txn_table_tail,
                                   (const void *) &txn_entry,
                                   comp_func);
    if (txn_table_node != NULL)
    {
      /*Found a transaction table entry wih invalid call_id.*/
      txn_entry_temp = (mcm_data_txn_table_entry *) txn_table_node->data;
      MCM_DATA_LOG_INFO("Found Txn table entriy with call id [%d], deleting.@.[%p]->[%p]",
                        txn_entry_temp->call_id,
                        txn_table_node,
                        txn_entry_temp);
      ds_dll_free (txn_table_node->data);
      txn_table_node->data = NULL;
      ds_dll_free (txn_table_node);
      txn_table_node = NULL;
    }
  }while(txn_table_node != NULL);

  rval = MCM_DATA_SUCCESS;
  BAIL:
  return rval;
}

int32_t del_calltbl_entry_callid_i
(
  uint32_t call_id
)
{
  ds_dll_el_t *call_table_node;
  ds_dll_el_t *call_table_tail;
  mcm_data_call_table_entry call_entry;
  mcm_data_call_table_entry *call_entry_temp;

  const void *dummy = NULL;
  int32_t rval;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_table_node = NULL;
  call_table_tail = NULL;

  MCM_DATA_LOG_INFO("Searching call table entry with call id [%d]",
                    call_id);

  memset(&call_entry, 0x00, sizeof(mcm_data_call_table_entry));
  call_entry.call_id = call_id;

  /*Find tail of transaction table list*/
  call_table_node = ds_dll_next(get_call_tbl_head(), &dummy);
  while (call_table_node != NULL)
  {
    call_table_tail = call_table_node;
    call_table_node = ds_dll_next(call_table_node, &dummy);
  }

  do
  {
    call_table_node = ds_dll_delete(get_call_tbl_head(),
                                    &call_table_tail,
                                    (const void *) &call_entry,
                                    isvalid_callid_calltbl_i);
    if (call_table_node != NULL)
    {
      /*Found a transaction table entry wih invalid call_id.*/
      call_entry_temp = (mcm_data_call_table_entry *) call_table_node->data;
     MCM_DATA_LOG_INFO("Deleting call table entry with call id [%d]",
                       call_entry_temp->call_id);
     MCM_DATA_LOG_INFO("Found call table entriy with call id [%d], deleting.@.[%p]->[%p]",
                       call_entry_temp->call_id,
                       call_table_node,
                       call_entry_temp);
      ds_dll_free (call_table_node->data);
      call_table_node->data = NULL;
      ds_dll_free (call_table_node);
      call_table_node = NULL;
    }
  }while(call_table_node != NULL);

  rval = MCM_DATA_SUCCESS;
  BAIL:

  return rval;
}

void print_calltbl_entries_i
(
  void
)
{
  ds_dll_el_t *call_table_node;
  const void *dummy = NULL;
  mcm_data_call_table_entry *call_entry;
  uint32_t index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  index = 0;
  /*Find tail of transaction table list*/
  call_table_node = ds_dll_next(get_call_tbl_head(), &dummy);

  while (call_table_node != NULL)
  {
    call_entry = (mcm_data_call_table_entry *) (call_table_node->data);

    MCM_DATA_LOG_INFO("call entry [%d] Call_id %d, dsi_hdnl %d",
                      index,
                      call_entry->call_id,
                      call_entry->dsi_handle);
    MCM_DATA_LOG_INFO("call entry [%d] QMI WDS hdnl %p",
                      index,
                      call_entry->qmi_wds_hndl,
                      0);
    MCM_DATA_LOG_INFO("call entry [%d] QMI CLI hdnl %p, is_valid %d",
                      index,
                      call_entry->qmi_cli_handle,
                      is_call_valid_i(call_entry));

    call_table_node = ds_dll_next(call_table_node, &dummy);
    index++;
  }
}

void print_txntbl_entries_i
(
  void
)
{
  ds_dll_el_t *txn_table_node;
  const void *dummy = NULL;
  mcm_data_txn_table_entry *txn_entry;
  uint32_t index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  index = 0;
  /*Find tail of transaction table list*/
  txn_table_node = ds_dll_next(get_txn_tbl_head(), &dummy);

  while (txn_table_node != NULL)
  {
    txn_entry = (mcm_data_txn_table_entry *) (txn_table_node->data);

    MCM_DATA_LOG_INFO("Txn entry [%d] Call_id %d",
                       index,
                       txn_entry->call_id,
                       0);
    MCM_DATA_LOG_INFO("Txn entry [%d] API type %d, is_valid %d",
                       index,
                       txn_entry->api_type,
                       is_txn_valid_i(txn_entry));
    MCM_DATA_LOG_INFO("Txn entry [%d] QMI clnt hdnl %p, req hndl %p",
                       index,
                       txn_entry->qmi_cli_handle,
                       txn_entry->req_handle);

    txn_table_node = ds_dll_next(txn_table_node, &dummy);
    index++;
  }
}

/*===========================================================================
  FUNCTION:  dsi_format_log_msg
===========================================================================*/
/*!
    @brief
    formats log messages

    @return
    none
*/
/*=========================================================================*/
void MCM_DATA_format_log_msg
(
  char * buf,
  int buf_size,
  char * fmt,
  ...
)
{
  va_list ap;

  va_start(ap, fmt);

  vsnprintf(buf, buf_size, fmt, ap);

  va_end(ap);
}

uint8_t is_call_valid_i
(
  mcm_data_call_table_entry *node
)
{
  if (node->self == node)
    return TRUE;
  else
    return FALSE;
}

uint8_t is_txn_valid_i
(
  mcm_data_txn_table_entry *node
)
{
  if (node->self == node)
    return TRUE;
  else
    return FALSE;
}

mcm_data_call_table_entry* get_calltbl_qmi_entry_i
(
  qmi_client_handle qmi_cli_handle
)
{
  mcm_data_call_table_entry call_entry;
  ds_dll_el_t *call_table_node;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&call_entry, 0x00, sizeof(mcm_data_call_table_entry));
  call_entry.qmi_cli_handle = qmi_cli_handle;

  MCM_DATA_LOG_INFO("Searching for call entry with QMI CLNT HDNL %p",
                    call_entry.qmi_cli_handle);

  /* Check if call_id  entered is valid. */
  call_table_node = ds_dll_search(get_call_tbl_head(),
                                  (const void *) &call_entry,
                                  isvalid_qmi_clnt_hdnl_calltbl_i);
  if (call_table_node == NULL)
  {
    MCM_DATA_LOG_ERROR("Call entry not found with given QMI CLNT HDNL %p",
                       call_entry.qmi_cli_handle);
    return NULL;
  }

  return (mcm_data_call_table_entry *) (call_table_node->data);

}

int convert_wds_ce_reason_code_to_mcm
(
  dsi_ce_reason_t ce_reason
)
{
  int return_val = MCM_DATA_CE_INVALID_V01;
  switch(ce_reason.reason_type)
  {
    case DSI_CE_TYPE_MOBILE_IP:
      switch(ce_reason.reason_code)
      {
        case WDS_VCER_MIP_FA_REASON_UNSPECIFIED_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_REASON_UNSPECIFIED_V01;
          break;
        case WDS_VCER_MIP_FA_ADMIN_PROHIBITED_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_ADMINISTRATIVELY_PROHIBITED_V01;
          break;
        case WDS_VCER_MIP_FA_INSUFFICIENT_RESOURCES_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_INSUFFICIENT_RESOURCES_V01;
          break;
        case WDS_VCER_MIP_FA_MOBILE_NODE_AUTH_FAILURE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE_V01;
          break;
        case WDS_VCER_MIP_FA_HA_AUTH_FAILURE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_HA_AUTHENTICATION_FAILURE_V01;
          break;
        case WDS_VCER_MIP_FA_REQ_LIFETIME_TOO_LONG_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_REQUESTED_LIFETIME_TOO_LONG_V01;
          break;
        case WDS_VCER_MIP_FA_MALFORMED_REQUEST_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_MALFORMED_REQUEST_V01;
          break;
        case WDS_VCER_MIP_FA_MALFOMED_REPLY_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_MALFORMED_REPLY_V01;
          break;
        case WDS_VCER_MIP_FA_ENCAPSULATION_UNAVAILABLE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_ENCAPSULATION_UNAVAILABLE_V01;
          break;
        case WDS_VCER_MIP_FA_VJHC_UNAVAILABLE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_VJHC_UNAVAILABLE_V01;
          break;
        case WDS_VCER_MIP_FA_REV_TUNNEL_UNAVAILABLE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_REVERSE_TUNNEL_UNAVAILABLE_V01;
          break;
        case WDS_VCER_MIP_FA_REV_TUNNEL_IS_MAND_AND_T_BIT_NOT_SET_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET_V01;
          break;
        case WDS_VCER_MIP_FA_DELIVERY_STYLE_NOT_SUPP_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_DELIVERY_STYLE_NOT_SUPPORTED_V01;
          break;
        case WDS_VCER_MIP_FA_MISSING_NAI_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_MISSING_NAI_V01;
          break;
        case WDS_VCER_MIP_FA_MISSING_HA_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_MISSING_HA_V01;
          break;
        case WDS_VCER_MIP_FA_MISSING_HOME_ADDR_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_MISSING_HOME_ADDR_V01;
          break;
        case WDS_VCER_MIP_FA_UNKNOWN_CHALLENGE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_UNKNOWN_CHALLENGE_V01;
          break;
        case WDS_VCER_MIP_FA_MISSING_CHALLENGE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_MISSING_CHALLENGE_V01;
          break;
        case WDS_VCER_MIP_FA_STALE_CHALLENGE_V01:
          return_val = MCM_DATA_CE_MIP_FA_ERR_STALE_CHALLENGE_V01;
          break;
        case WDS_VCER_MIP_HA_REASON_UNSPECIFIED_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_REASON_UNSPECIFIED_V01;
          break;
        case WDS_VCER_MIP_HA_ADMIN_PROHIBITED_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_ADMINISTRATIVELY_PROHIBITED_V01;
          break;
        case WDS_VCER_MIP_HA_INSUFFICIENT_RESOURCES_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_INSUFFICIENT_RESOURCES_V01;
          break;
        case WDS_VCER_MIP_HA_MOBILE_NODE_AUTH_FAILURE_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_MOBILE_NODE_AUTHENTICATION_FAILURE_V01;
          break;
        case WDS_VCER_MIP_HA_FA_AUTH_FAILURE_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_FA_AUTHENTICATION_FAILURE_V01;
          break;
        case WDS_VCER_MIP_HA_REGISTRATION_ID_MISMATCH_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_REGISTRATION_ID_MISMATCH_V01;
          break;
        case WDS_VCER_MIP_HA_MALFORMED_REQUEST_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_MALFORMED_REQUEST_V01;
          break;
        case WDS_VCER_MIP_HA_UNKNOWN_HA_ADDR_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_UNKNOWN_HA_ADDR_V01;
          break;
        case WDS_VCER_MIP_HA_REV_TUNNEL_UNAVAILABLE_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_REVERSE_TUNNEL_UNAVAILABLE_V01;
          break;
        case WDS_VCER_MIP_HA_REV_TUNNEL_IS_MAND_AND_T_BIT_NOT_SET_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_REVERSE_TUNNEL_IS_MANDATORY_AND_T_BIT_NOT_SET_V01;
          break;
        case WDS_VCER_MIP_HA_ENCAPSULATION_UNAVAILABLE_V01:
          return_val = MCM_DATA_CE_MIP_HA_ERR_ENCAPSULATION_UNAVAILABLE_V01;
          break;
        case WDS_VCER_MIP_HA_REASON_UNKNOWN_V01:
          return_val = MCM_DATA_CE_MIP_ERR_REASON_UNKNOWN_V01;
          break;
      }
      break;
    case DSI_CE_TYPE_INTERNAL:
      switch(ce_reason.reason_code)
      {
        case WDS_VCER_INTERNAL_INTERNAL_ERROR_V01:
          return_val = MCM_DATA_CE_INTERNAL_ERROR_V01;
          break;
        case WDS_VCER_INTERNAL_CALL_ENDED_V01:
          return_val = MCM_DATA_CE_CALL_ENDED_V01;
          break;
        case WDS_VCER_INTERNAL_INTERNAL_UNKNOWN_CAUSE_CODE_V01:
          return_val = MCM_DATA_CE_INTERNAL_UNKNOWN_CAUSE_CODE_V01;
          break;
        case WDS_VCER_INTERNAL_UNKNOWN_CAUSE_CODE_V01:
          return_val = MCM_DATA_CE_UNKNOWN_CAUSE_CODE_V01;
          break;
        case WDS_VCER_INTERNAL_CLOSE_IN_PROGRESS_V01:
          return_val = MCM_DATA_CE_CLOSE_IN_PROGRESS_V01;
          break;
        case WDS_VCER_INTERNAL_NW_INITIATED_TERMINATION_V01:
          return_val = MCM_DATA_CE_NW_INITIATED_TERMINATION_V01;
          break;
        case WDS_VCER_INTERNAL_APP_PREEMPTED_V01:
          return_val = MCM_DATA_CE_APP_PREEMPTED_V01;
          break;
      }
      break;
    case DSI_CE_TYPE_CALL_MANAGER_DEFINED:
      switch(ce_reason.reason_code)
      {
        case WDS_VCER_CM_CDMA_LOCK_V01:
          return_val = MCM_DATA_CE_CDMA_LOCK_V01;
          break;
        case WDS_VCER_CM_INTERCEPT_V01:
          return_val = MCM_DATA_CE_INTERCEPT_V01;
          break;
        case WDS_VCER_CM_REORDER_V01:
          return_val = MCM_DATA_CE_REORDER_V01;
          break;
        case WDS_VCER_CM_REL_SO_REJ_V01:
          return_val = MCM_DATA_CE_REL_SO_REJ_V01;
          break;
        case WDS_VCER_CM_INCOM_CALL_V01:
          return_val = MCM_DATA_CE_INCOM_CALL_V01;
          break;
        case WDS_VCER_CM_ALERT_STOP_V01:
          return_val = MCM_DATA_CE_ALERT_STOP_V01;
          break;
        case WDS_VCER_CM_ACTIVATION_V01:
          return_val = MCM_DATA_CE_ACTIVATION_V01;
          break;
        case WDS_VCER_CM_MAX_ACCESS_PROBE_V01:
          return_val = MCM_DATA_CE_MAX_ACCESS_PROBE_V01;
          break;
        case WDS_VCER_CM_CCS_NOT_SUPP_BY_BS_V01:
          return_val = MCM_DATA_CE_CCS_NOT_SUPPORTED_BY_BS_V01;
          break;
        case WDS_VCER_CM_NO_RESPONSE_FROM_BS_V01:
          return_val = MCM_DATA_CE_NO_RESPONSE_FROM_BS_V01;
          break;
        case WDS_VCER_CM_REJECTED_BY_BS_V01:
          return_val = MCM_DATA_CE_REJECTED_BY_BS_V01;
          break;
        case WDS_VCER_CM_INCOMPATIBLE_V01:
          return_val = MCM_DATA_CE_INCOMPATIBLE_V01;
          break;
        case WDS_VCER_CM_ALREADY_IN_TC_V01:
          return_val = MCM_DATA_CE_ALREADY_IN_TC_V01;
          break;
        case WDS_VCER_CM_USER_CALL_ORIG_DURING_GPS_V01:
          return_val = MCM_DATA_CE_USER_CALL_ORIG_DURING_GPS_V01;
          break;
        case WDS_VCER_CM_USER_CALL_ORIG_DURING_SMS_V01:
          return_val = MCM_DATA_CE_USER_CALL_ORIG_DURING_SMS_V01;
          break;
        case WDS_VCER_CM_NO_CDMA_SRV_V01:
          return_val = MCM_DATA_CE_NO_CDMA_SRV_V01;
          break;
        case WDS_VCER_CM_CONF_FAILED_V01:
          return_val = MCM_DATA_CE_CONF_FAILED_V01;
          break;
        case WDS_VCER_CM_INCOM_REJ_V01:
          return_val = MCM_DATA_CE_INCOM_REJ_V01;
          break;
        case WDS_VCER_CM_NEW_NO_GW_SERV_V01:
          return_val = MCM_DATA_CE_NO_GW_SRV_V01;
          break;
        case WDS_VCER_CM_NEW_NO_GPRS_CONTEXT_V01:
          return_val = MCM_DATA_CE_NO_GPRS_CONTEXT_V01;
          break;
        case WDS_VCER_CM_NEW_ILLEGAL_MS_V01:
          return_val = MCM_DATA_CE_ILLEGAL_MS_V01;
          break;
        case WDS_VCER_CM_NEW_ILLEGAL_ME_V01:
          return_val = MCM_DATA_CE_ILLEGAL_ME_V01;
          break;
        case WDS_VCER_CM_NEW_GPRS_SERV_AND_NON_GPRS_SERV_NOT_ALLOWED_V01:
          return_val = MCM_DATA_CE_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED_V01;
          break;
        case WDS_VCER_CM_NEW_GPRS_SERV_NOT_ALLOWED_V01:
          return_val = MCM_DATA_CE_GPRS_SERVICES_NOT_ALLOWED_V01;
          break;
        case WDS_VCER_CM_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK_V01:
          return_val = MCM_DATA_CE_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK_V01;
          break;
        case WDS_VCER_CM_IMPLICITLY_DETACHED_V01:
          return_val = MCM_DATA_CE_IMPLICITLY_DETACHED_V01;
          break;
        case WDS_VCER_CM_PLMN_NOT_ALLOWED_V01:
          return_val = MCM_DATA_CE_PLMN_NOT_ALLOWED_V01;
          break;
        case WDS_VCER_CM_LA_NOT_ALLOWED_V01:
          return_val = MCM_DATA_CE_LA_NOT_ALLOWED_V01;
          break;
        case WDS_VCER_CM_GPRS_SERV_NOT_ALLOWED_IN_THIS_PLMN_V01:
          return_val = MCM_DATA_CE_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN_V01;
          break;
        case WDS_VCER_CM_PDP_DUPLICATE_V01:
          return_val = MCM_DATA_CE_PDP_DUPLICATE_V01;
          break;
        case WDS_VCER_CM_UE_RAT_CHANGE_V01:
          return_val = MCM_DATA_CE_UE_RAT_CHANGE_V01;
          break;
        case WDS_VCER_CM_CONGESTION_V01:
          return_val = MCM_DATA_CE_CONGESTION_V01;
          break;
        case WDS_VCER_CM_NO_PDP_CONTEXT_ACTIVATED_V01:
          return_val = MCM_DATA_CE_NO_PDP_CONTEXT_ACTIVATED_V01;
          break;
        case WDS_VCER_CM_ACCESS_CLASS_DSAC_REJECTION_V01:
          return_val = MCM_DATA_CE_ACCESS_CLASS_DSAC_REJECTION_V01;
          break;
        case WDS_VCER_CM_CD_GEN_OR_BUSY_V01:
          return_val = MCM_DATA_CE_CD_GEN_OR_BUSY_V01;
          break;
        case WDS_VCER_CM_CD_BILL_OR_AUTH_V01:
          return_val = MCM_DATA_CE_CD_BILL_OR_AUTH_V01;
          break;
        case WDS_VCER_CM_CHG_HDR_V01:
          return_val = MCM_DATA_CE_CHG_HDR_V01;
          break;
        case WDS_VCER_CM_EXIT_HDR_V01:
          return_val = MCM_DATA_CE_EXIT_HDR_V01;
          break;
        case WDS_VCER_CM_HDR_NO_SESSION_V01:
          return_val = MCM_DATA_CE_HDR_NO_SESSION_V01;
          break;
        case WDS_VCER_CM_HDR_ORIG_DURING_GPS_FIX_V01:
          return_val = MCM_DATA_CE_HDR_ORIG_DURING_GPS_FIX_V01;
          break;
        case WDS_VCER_CM_HDR_CS_TIMEOUT_V01:
          return_val = MCM_DATA_CE_HDR_CS_TIMEOUT_V01;
          break;
        case WDS_VCER_CM_HDR_RELEASED_BY_CM_V01:
          return_val = MCM_DATA_CE_HDR_RELEASED_BY_CM_V01;
          break;
        case WDS_VCER_CM_CLIENT_END_V01:
          return_val = MCM_DATA_CE_CLIENT_END_V01;
          break;
        case WDS_VCER_CM_NO_SRV_V01:
          return_val = MCM_DATA_CE_NO_SRV_V01;
          break;
        case WDS_VCER_CM_FADE_V01 :
          return_val = MCM_DATA_CE_FADE_V01;
          break;
        case WDS_VCER_CM_REL_NORMAL_V01:
          return_val = MCM_DATA_CE_REL_NORMAL_V01;
          break;
        case WDS_VCER_CM_ACC_IN_PROG_V01:
          return_val = MCM_DATA_CE_ACC_IN_PROG_V01;
          break;
        case WDS_VCER_CM_ACC_FAIL_V01:
          return_val = MCM_DATA_CE_ACC_FAIL_V01;
          break;
        case WDS_VCER_CM_REDIR_OR_HANDOFF_V01:
          return_val = MCM_DATA_CE_REDIR_OR_HANDOFF_V01;
          break;
      }
      break;
    case DSI_CE_TYPE_3GPP_SPEC_DEFINED:
      switch(ce_reason.reason_code)
      {
        case WDS_VCER_3GPP_OPERATOR_DETERMINED_BARRING_V01:
          return_val = MCM_DATA_CE_OPERATOR_DETERMINED_BARRING_V01;
          break;
        case WDS_VCER_3GPP_LLC_SNDCP_FAILURE_V01:
          return_val = MCM_DATA_CE_LLC_SNDCP_FAILURE_V01;
          break;
        case WDS_VCER_3GPP_INSUFFICIENT_RESOURCES_V01:
          return_val = MCM_DATA_CE_INSUFFICIENT_RESOURCES_V01;
          break;
        case WDS_VCER_3GPP_UNKNOWN_APN_V01:
          return_val = MCM_DATA_CE_UNKNOWN_APN_V01;
          break;
        case WDS_VCER_3GPP_UNKNOWN_PDP_V01:
          return_val = MCM_DATA_CE_UNKNOWN_PDP_V01;
          break;
        case WDS_VCER_3GPP_AUTH_FAILED_V01:
          return_val = MCM_DATA_CE_AUTH_FAILED_V01;
          break;
        case WDS_VCER_3GPP_GGSN_REJECT_V01:
          return_val = MCM_DATA_CE_GGSN_REJECT_V01;
          break;
        case WDS_VCER_3GPP_ACTIVATION_REJECT_V01:
          return_val = MCM_DATA_CE_ACTIVATION_REJECT_V01;
          break;
        case WDS_VCER_3GPP_OPTION_NOT_SUPPORTED_V01:
          return_val = MCM_DATA_CE_OPTION_NOT_SUPPORTED_V01;
          break;
        case WDS_VCER_3GPP_OPTION_UNSUBSCRIBED_V01:
          return_val = MCM_DATA_CE_OPTION_UNSUBSCRIBED_V01;
          break;
        case WDS_VCER_3GPP_OPTION_TEMP_OOO_V01:
          return_val = MCM_DATA_CE_OPTION_TEMP_OOO_V01;
          break;
        case WDS_VCER_3GPP_NSAPI_ALREADY_USED_V01:
          return_val = MCM_DATA_CE_NSAPI_ALREADY_USED_V01;
          break;
        case WDS_VCER_3GPP_REGULAR_DEACTIVATION_V01:
          return_val = MCM_DATA_CE_REGULAR_DEACTIVATION_V01;
          break;
        case WDS_VCER_3GPP_QOS_NOT_ACCEPTED_V01:
          return_val = MCM_DATA_CE_QOS_NOT_ACCEPTED_V01;
          break;
        case WDS_VCER_3GPP_NETWORK_FAILURE_V01:
          return_val = MCM_DATA_CE_NETWORK_FAILURE_V01;
          break;
        case WDS_VCER_3GPP_UMTS_REACTIVATION_REQ_V01:
          return_val = MCM_DATA_CE_UMTS_REACTIVATION_REQ_V01;
          break;
        case WDS_VCER_3GPP_FEATURE_NOT_SUPP_V01:
          return_val = MCM_DATA_CE_FEATURE_NOT_SUPPORTED_V01;
          break;
        case WDS_VCER_3GPP_TFT_SEMANTIC_ERROR_V01:
          return_val = MCM_DATA_CE_TFT_SEMANTIC_ERROR_V01;
          break;
        case WDS_VCER_3GPP_TFT_SYTAX_ERROR_V01:
          return_val = MCM_DATA_CE_TFT_SYNTAX_ERROR_V01;
          break;
        case WDS_VCER_3GPP_UNKNOWN_PDP_CONTEXT_V01:
          return_val = MCM_DATA_CE_UNKNOWN_PDP_CONTEXT_V01;
          break;
        case WDS_VCER_3GPP_FILTER_SEMANTIC_ERROR_V01:
          return_val = MCM_DATA_CE_FILTER_SEMANTIC_ERROR_V01;
          break;
        case WDS_VCER_3GPP_FILTER_SYTAX_ERROR_V01:
          return_val = MCM_DATA_CE_FILTER_SYNTAX_ERROR_V01;
          break;
        case WDS_VCER_3GPP_PDP_WITHOUT_ACTIVE_TFT_V01:
          return_val = MCM_DATA_CE_PDP_WITHOUT_ACTIVE_TFT_V01;
          break;
        case WDS_VCER_3GPP_IP_V4_ONLY_ALLOWED_V01:
          return_val = MCM_DATA_CE_IP_V4_ONLY_ALLOWED_V01;
          break;
        case WDS_VCER_3GPP_IP_V6_ONLY_ALLOWED_V01:
          return_val = MCM_DATA_CE_IP_V6_ONLY_ALLOWED_V01;
          break;
        case WDS_VCER_3GPP_SINGLE_ADDR_BEARER_ONLY_V01:
          return_val = MCM_DATA_CE_SINGLE_ADDR_BEARER_ONLY_V01;
          break;
        case WDS_VCER_3GPP_INVALID_TRANSACTION_ID_V01:
          return_val = MCM_DATA_CE_INVALID_TRANSACTION_ID_V01;
          break;
        case WDS_VCER_3GPP_MESSAGE_INCORRECT_SEMANTIC_V01:
          return_val = MCM_DATA_CE_MESSAGE_INCORRECT_SEMANTIC_V01;
          break;
        case WDS_VCER_3GPP_INVALID_MANDATORY_INFO_V01:
          return_val = MCM_DATA_CE_INVALID_MANDATORY_INFO_V01;
          break;
        case WDS_VCER_3GPP_MESSAGE_TYPE_UNSUPPORTED_V01:
          return_val = MCM_DATA_CE_MESSAGE_TYPE_UNSUPPORTED_V01;
          break;
        case WDS_VCER_3GPP_MSG_TYPE_NONCOMPATIBLE_STATE_V01:
          return_val = MCM_DATA_CE_MSG_TYPE_NONCOMPATIBLE_STATE_V01;
          break;
        case WDS_VCER_3GPP_UNKNOWN_INFO_ELEMENT_V01:
          return_val = MCM_DATA_CE_UNKNOWN_INFO_ELEMENT_V01;
          break;
        case WDS_VCER_3GPP_CONDITIONAL_IE_ERROR_V01:
          return_val = MCM_DATA_CE_CONDITIONAL_IE_ERROR_V01;
          break;
        case WDS_VCER_3GPP_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE_V01:
          return_val = MCM_DATA_CE_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE_V01;
          break;
        case WDS_VCER_3GPP_PROTOCOL_ERROR_V01:
          return_val = MCM_DATA_CE_PROTOCOL_ERROR_V01;
          break;
        case WDS_VCER_3GPP_APN_TYPE_CONFLICT_V01:
          return_val = MCM_DATA_CE_APN_TYPE_CONFLICT_V01;
          break;
      }
      break;
    case DSI_CE_TYPE_PPP:
      switch(ce_reason.reason_code)
      {
        case WDS_VCER_PPP_TIMEOUT_V01:
          return_val = MCM_DATA_CE_PPP_TIMEOUT_V01;
          break;
        case WDS_VCER_PPP_AUTH_FAILURE_V01:
          return_val = MCM_DATA_CE_PPP_AUTH_FAILURE_V01;
          break;
        case WDS_VCER_PPP_OPTION_MISMATCH_V01:
          return_val = MCM_DATA_CE_PPP_OPTION_MISMATCH_V01;
          break;
        case WDS_VCER_PPP_PAP_FAILURE_V01:
          return_val = MCM_DATA_CE_PPP_PAP_FAILURE_V01;
          break;
        case WDS_VCER_PPP_CHAP_FAILURE_V01:
          return_val = MCM_DATA_CE_PPP_CHAP_FAILURE_V01;
          break;
        case WDS_VCER_PPP_UNKNOWN_V01:
          return_val = MCM_DATA_CE_PPP_UNKNOWN_V01;
          break;
      }
      break;
    case DSI_CE_TYPE_EHRPD:
      switch(ce_reason.reason_code)
      {
        case WDS_VCER_EHRPD_SUBS_LIMITED_TO_V4_V01:
          return_val = MCM_DATA_CE_EHRPD_SUBS_LIMITED_TO_V4_V01;
          break;
        case WDS_VCER_EHRPD_SUBS_LIMITED_TO_V6_V01:
          return_val = MCM_DATA_CE_EHRPD_SUBS_LIMITED_TO_V6_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_TIMEOUT_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_TIMEOUT_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_FAILURE_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_FAILURE_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_GEN_ERROR_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_GEN_ERROR_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_UNAUTH_APN_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_UNAUTH_APN_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_PDN_LIMIT_EXCEED_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_PDN_LIMIT_EXCEED_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_NO_PDN_GW_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_NO_PDN_GW_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_PDN_GW_UNREACH_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_PDN_GW_UNREACH_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_PDN_GW_REJ_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_PDN_GW_REJ_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_INSUFF_PARAM_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_INSUFF_PARAM_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_RESOURCE_UNAVAIL_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_RESOURCE_UNAVAIL_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_ADMIN_PROHIBIT_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_ADMIN_PROHIBIT_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_PDN_ID_IN_USE_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_PDN_ID_IN_USE_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_SUBSCR_LIMITATION_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_SUBSCR_LIMITATION_V01;
          break;
        case WDS_VCER_EHRPD_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN_V01:
          return_val = MCM_DATA_CE_EHRPD_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN_V01;
          break;
      }
      break;
    case DSI_CE_TYPE_IPV6:
      switch(ce_reason.reason_code)
      {
        case WDS_VCER_IPV6_PREFIX_UNAVAILABLE_V01:
          return_val = MCM_DATA_CE_PREFIX_UNAVAILABLE_V01;
          break;
        case WDS_VCER_IPV6_ERR_HRPD_IPV6_DISABLED_V01:
          return_val = MCM_DATA_CE_IPV6_ERR_HRPD_IPV6_DISABLED_V01;
          break;
      }
      break;
    case DSI_CE_TYPE_INVALID:
    case DSI_CE_TYPE_UNINIT:
    default:
      MCM_DATA_LOG_ERROR("%s","Unknown qmi_wds ce_reason type");
      return_val = MCM_DATA_CE_INVALID_V01;
      break;
  }
  return return_val;
}

int convert_wds_ce_reason_type_to_mcm
(
  dsi_ce_reason_t ce_reason
)
{
  switch(ce_reason.reason_type)
  {
    case DSI_CE_TYPE_UNINIT:
    case DSI_CE_TYPE_INVALID:
      return MCM_DATA_TYPE_UNSPECIFIED_V01;
    case DSI_CE_TYPE_MOBILE_IP:
      return MCM_DATA_TYPE_MOBILE_IP_V01;
    case DSI_CE_TYPE_INTERNAL:
      return MCM_DATA_TYPE_INTERNAL_V01;
    case DSI_CE_TYPE_CALL_MANAGER_DEFINED:
      return MCM_DATA_TYPE_CALL_MANAGER_DEFINED_V01;
    case DSI_CE_TYPE_3GPP_SPEC_DEFINED:
      return MCM_DATA_TYPE_3GPP_SPEC_DEFINED_V01;
    case DSI_CE_TYPE_PPP:
      return MCM_DATA_TYPE_PPP_V01;
    case DSI_CE_TYPE_EHRPD:
      return MCM_DATA_TYPE_EHRPD_V01;
    case DSI_CE_TYPE_IPV6:
      return MCM_DATA_TYPE_IPV6_V01;
    default:
      MCM_DATA_LOG_ERROR("%s", "Unknown qmi_wds_ce_reason_type");
      return MCM_DATA_TYPE_UNSPECIFIED_V01;
  }
}
