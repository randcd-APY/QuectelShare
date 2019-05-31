/*================================================================
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  =================================================================*/
/*=================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ---------------------------------------
  12/10/2018       laurence    Init.

  =================================================================*/

#ifndef __QL_ATC_H__
#define __QL_ATC_H__




typedef uint32 atc_client_handle_type;

/* Send AT Cmd */
int QL_ATC_Send_Cmd
(
    atc_client_handle_type  h_atc, 
    char*                   req_atc_cmd, 
    char*                   resp_buf, 
    int                     resp_len
);

int QL_ATC_Client_Init(atc_client_handle_type  *ph_atc);

/* Release handle */
int QL_ATC_Client_Deinit(atc_client_handle_type h_atc);

#endif // __QL_ATC_H__

