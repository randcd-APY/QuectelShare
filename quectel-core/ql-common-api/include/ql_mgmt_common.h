/**
 * @file ql_mgmt_common.h
 * @brief Quectel manager common interface function declarations.
 *
 * @note
 */
/*=============================================================================
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=============================================================================*/

/*============================================================================

                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ----------------------------------------
  11/20/2017       Mike        Initial creation.
  07/17/2018       Mike        Adds WiFi ap station connected/disconnected event.
============================================================================*/
#ifndef __QL_MGMT_COMMON_H__
#define __QL_MGMT_COMMON_H__

#include "qmi_client.h"

#define COMMAND_SET 1
#define COMMAND_GET 2

/*<! Timeout of gets or sets command */
#define TIMEOUT_DEFAULT 5000

typedef struct {
        qmi_client_type qmi_qmam_handle;
	qmi_client_type qmi_qmam_notifiy;
	qmi_cci_os_signal_type qmi_qmam_os_params;
	ql_manager_ind_cb ind_cb;
} ql_mgmt_client_s;

#ifdef FEATURE_QL_WIFI
/*========================================================================
  FUNCTION: wifi_command_handle
=========================================================================*/
/*! @brief 
     Gets or Sets WiFi configuration.

    @param[in] handle          Handle used by the infrastructure to
                               identify different clients.
    @param[in] command_flag    The gets or sets flag.
    @param[in] command         The commands of manager client.
    @param[in] wifi            The WiFi configuration.

    @return
       On success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int wifi_command_handle(ql_mgmt_client_s *handle, s32 command_flag,
			       u32 command, ql_mgmt_client_wifi_s *wifi);
#endif /* end of FEATURE_QL_WIFI */

#ifdef FEATURE_QL_DATA_CALL
/*========================================================================
  FUNCTION: data_call_command_handle
=========================================================================*/
/*! @brief 
     Gets or Sets data call configuration.

    @param[in] handle          Handle used by the infrastructure to
                               identify different clients.
    @param[in] command_flag    The gets or sets flag.
    @param[in] command         The commands of manager client.
    @param[in] data_call       The data call configuration.

    @return
       On success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int data_call_command_handle(ql_mgmt_client_s *handle, s32 command_flag,
				    u32 command, ql_mgmt_client_data_call_s *data_call);
#endif /* end of FEATURE_QL_DATA_CALL */

#ifdef FEATURE_QL_NETWORK
/*========================================================================
  FUNCTION: network_command_handle
=========================================================================*/
/*! @brief 
     Gets or Sets network configuration.

    @param[in] handle          Handle used by the infrastructure to
                               identify different clients.
    @param[in] command_flag    The gets or sets flag.
    @param[in] command         The commands of manager client.
    @param[in] network         The network configuration.

    @return
       On success, 0 is return. On error, -1 is return.
*/
/*=======================================================================*/
extern int network_command_handle(ql_mgmt_client_s *handle, s32 command_flag,
				  u32 command, ql_mgmt_client_network_s *network);
#endif /* end of FEATURE_QL_NETWORK */

/*========================================================================
  FUNCTION: ql_mgmt_set_errno
=========================================================================*/
/*! @brief 
      Sets errno.

    @return
       None
*/
/*=======================================================================*/
extern void ql_mgmt_set_errno(int err);

/*========================================================================
  FUNCTION: data_call_status_event_ind_msg
  =========================================================================*/
/*! @brief 
    Processes an QL_MANAGER_MSGR_DATA_CALL_STATUS_EVENT_IND_V01 
    incoming QMI QMAM Indication.

  @return
    None
*/
/*=======================================================================*/
extern void data_call_status_event_ind_msg(qmi_client_type user_handle,
					   void *ind_buf,
					   u32 ind_buf_len,
					   ql_mgmt_client_s *qmc);

/*========================================================================
  FUNCTION: wifi_event_ind_msg
  =========================================================================*/
/*! @brief 
    Processes an WIFI indication message.
    incoming QMI QMAM Indication.

  @return
    None
*/
/*=======================================================================*/
extern void wifi_event_ind_msg(qmi_client_type user_handle, s32 ind_id, void *ind_buf,
			       u32 ind_buf_len, ql_mgmt_client_s *qmc); 

#endif /* end of __QL_MGMT_COMMON_H__ */
