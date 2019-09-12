#ifndef MCM_DATA_UTILS_H
#define MCM_DATA_UTILS_H
/**
  @file
  mcm_data_utils.h

  @brief
  This file provides utlities for Mobile Connection Manager(mcm)
  Data Services APIs.

  @details
  This file provides logging macros and function declarations
  of utilities to be used by Mobile Connection Manager(mcm)
  Data Services APIs

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

#include "ds_util.h"
#include "mcm_data_srv.h"

#define MCM_DATA_SUCCESS 0
#define MCM_DATA_ERROR -1

#ifdef FEATURE_MCM_DATA_EXE

#include <stdio.h>

#define MCM_DATA_LOG_DEBUG(fmt, ...) \
  printf("%s:%d ",__FILE__, __LINE__); \
  printf(fmt, __VA_ARGS__); \
  printf("\n")

#define MCM_DATA_LOG_ERROR(fmt, ...) \
  printf("%s:%d ",__FILE__, __LINE__); \
  printf(fmt, __VA_ARGS__); \
  printf("\n")

#define MCM_DATA_LOG_FATAL(fmt, ...) \
  printf("%s:%d ",__FILE__, __LINE__); \
  printf(fmt, __VA_ARGS__); \
  printf("\n")

#define MCM_DATA_LOG_VERBOSE(fmt, ...) \
  printf("%s:%d ",__FILE__, __LINE__); \
  printf(fmt, __VA_ARGS__); \
  printf("\n")

#define MCM_DATA_LOG_INFO(fmt, ...) \
  printf("%s:%d ",__FILE__, __LINE__); \
  printf(fmt, __VA_ARGS__); \
  printf("\n")

#else

#include "msg.h"

#define MCM_DATA_MAX_DIAG_LOG_MSG_SIZE 512

void MCM_DATA_format_log_msg(char * buf, int buf_size, char * fmt, ...);

#define MCM_DATA_LOG_HIGH(...) \
  { \
    char _buf[MCM_DATA_MAX_DIAG_LOG_MSG_SIZE]; \
    MCM_DATA_format_log_msg(_buf, MCM_DATA_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__); \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, "%s", _buf); \
  }

#define MCM_DATA_LOG_VERBOSE(...) \
  { \
    char _buf[MCM_DATA_MAX_DIAG_LOG_MSG_SIZE]; \
    MCM_DATA_format_log_msg(_buf, MCM_DATA_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__); \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, "%s", _buf); \
  }

#define MCM_DATA_LOG_INFO(...) \
  { \
    char _buf[MCM_DATA_MAX_DIAG_LOG_MSG_SIZE]; \
    MCM_DATA_format_log_msg(_buf, MCM_DATA_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__); \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, "%s", _buf); \
  }

#define MCM_DATA_LOG_ERROR(...) \
  { \
    char _buf[MCM_DATA_MAX_DIAG_LOG_MSG_SIZE]; \
    MCM_DATA_format_log_msg(_buf, MCM_DATA_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__); \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, "%s", _buf); \
  }

#define MCM_DATA_LOG_FATAL(...) \
  { \
    char _buf[MCM_DATA_MAX_DIAG_LOG_MSG_SIZE]; \
    MCM_DATA_format_log_msg(_buf, MCM_DATA_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__); \
    MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_FATAL, "%s", _buf); \
  }
#endif

#define MCM_DATA_LOG_FUNC_ENTRY() \
  MCM_DATA_LOG_INFO("%s: ENTRY", __FUNCTION__)

#define MCM_DATA_LOG_FUNC_EXIT() \
  MCM_DATA_LOG_INFO("%s: EXIT", __FUNCTION__)

/*---------------------------------------------------------------------------
  Condition for searching in ds_dll
---------------------------------------------------------------------------*/

typedef enum condition_e
{
  ALL = 0,
  START,
  STOP
} condition;

/*---------------------------------------------------------------------------
  Function declarations for mcm_data_utils
---------------------------------------------------------------------------*/

long int isvalid_callid_calltbl_i
(
  const void *first,
  const void *second
);

long int isvalid_callid_txntbl_i
(
  const void *first,
  const void *second
);

long int txn_entry_callid_start_call_i
(
  const void *first,
  const void *second
);

long int txn_entry_callid_stop_call_i
(
  const void *first,
  const void *second
);

mcm_data_call_table_entry* get_calltbl_entry_i
(
  uint32_t call_id
);

mcm_data_txn_table_entry* get_txntbl_entry_i
(
  uint32_t call_id,
  condition condtion
);

int32_t del_txntbl_entry_callid_i
(
  uint32_t call_id,
  condition del_condtion
);

int32_t del_calltbl_entry_callid_i
(
  uint32_t call_id
);

void print_txntbl_entries_i
(
  void
);

void print_calltbl_entries_i
(
  void
);

uint8_t is_call_valid_i
(
  mcm_data_call_table_entry *node
);

uint8_t is_txn_valid_i
(
  mcm_data_txn_table_entry *node
);

long int isvalid_qmi_clnt_hdnl_calltbl_i
(
  const void *first,
  const void *second
);

long int isvalid_qmi_clnt_hdnl_txntbl_i
(
  const void *first,
  const void *second
);

mcm_data_call_table_entry* get_calltbl_qmi_entry_i
(
  qmi_client_handle qmi_cli_handle
);

int convert_wds_ce_reason_code_to_mcm
(
  dsi_ce_reason_t ce_reason
);

int convert_wds_ce_reason_type_to_mcm
(
  dsi_ce_reason_t ce_reason
);

#endif
