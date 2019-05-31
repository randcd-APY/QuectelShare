#ifndef MCM_CLIENT_STUBS_H
#define MCM_CLIENT_STUBS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2013 Qualcomm Technologies, Inc.
All Rights Reserved.Qualcomm Technologies International, Ltd. Confidential and Proprietary.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mcm_client.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32 mcm_client_init
(
  mcm_client_handle_type   *hndl,
  mcm_client_ind_cb         ind_cb,
  mcm_client_async_cb       default_resp_cb
)
{
  return MCM_SUCCESS_V01;
}

uint32 mcm_client_execute_command_async
(
  mcm_client_handle_type     hndl,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *resp_c_struct,
  int                        resp_c_struct_len,
  mcm_client_async_cb        async_resp_cb,
  void                      *token_id
)
{
  return MCM_SUCCESS_V01;
}

uint32 mcm_client_execute_command_sync
(
  mcm_client_handle_type      hndl,
  int                         msg_id,
  void                       *req_c_struct,
  int                         req_c_struct_len,
  void                       *resp_c_struct,
  int                         resp_c_struct_len
)
{
  return MCM_SUCCESS_V01;
}

uint32 mcm_client_release(mcm_client_handle_type hndl)
{
  return MCM_SUCCESS_V01;
}

#ifdef __cplusplus
}
#endif

#endif /* MCM_CLIENT_STUBS_H */

