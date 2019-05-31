/******************************************************************************

                        QBRIDGE_DPM.C

******************************************************************************/

/******************************************************************************

  @file    qbridge_dpm.c
  @brief   Handles DPM related QMI Interface Call

  ---------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when       who        what, where, why
--------   ---        -------------------------------------------------------
05/02/18    rv        Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"
#include "data_port_mapper_v01.h"
#include "qbridge_log_qxdm.h"
#include "qbridge_main.h"
#include "qbridge_dpm.h"

/*=============================================================================

  Private Function Definition

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_dpm_get_gps_port_info
===========================================================================*/
/*!
    @brief Gets control and data port interface IDs, etc. for QMI DPM

    @details

    @param qbridge_config_param
    @param control_port_name
    @param control_port_name_len
    @param data_ep_id
    @param bam_info Optional (can be NULL)

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbridge_dpm_get_gps_port_info
(
  qbridge_rmnet_param  *qbridge_config_param,
  char  *control_port_name,
  uint32  control_port_name_len,
  data_ep_id_type_v01  *data_ep_id,
  hardware_accl_port_details_v01  *bam_info
)
{
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qbridge_config_param);
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(control_port_name);

  strlcpy(control_port_name, QBRDIGE_DPM_CONTROL_PORT, control_port_name_len);

  return qbridge_dpm_get_data_port_info(qbridge_config_param, data_ep_id, 
                                        bam_info);
} /* qbridge_dpm_get_gps_port_info() */

/*===========================================================================
  FUNCTION: qbridge_dpm_send_qmi_req
===========================================================================*/
/*!
    @brief Sends a QMI DPM request (synchronous)

    @details

    @param msg_id
    @param req_data
    @param req_data_len
    @param rsp_data
    @param rsp_data_len

    @return boolean TRUE if the request was sent and a response was received
*/
/*=========================================================================*/
static boolean qbridge_dpm_send_qmi_req
(
  uint32  msg_id,
  void  *req_data,
  uint32  req_data_len,
  void   *rsp_data,
  uint32  rsp_data_len
)
{
  qmi_client_error_type  err = QMI_NO_ERR;
  qmi_client_os_params   os_params;
  qmi_client_type        handle;
  boolean                success = FALSE;
  int                    clientRetry = 0;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(req_data);
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(rsp_data);

  QBRIDGE_LOG_D_0("QMUXBRIDGE :: Fetching DPM client ID");

  qbridge_os_qcci_os_params_init(&os_params);

  while (clientRetry < QBRIDGE_QMI_CLIENT_MAX_RETRY)
  {
    err = qmi_client_init_instance(
          dpm_get_service_object_v01(), QMI_CLIENT_INSTANCE_ANY, NULL, NULL,
          &os_params, QBRIDGE_OS_LINUX_QCCI_TIMEOUT_MS, &handle);
    if (err != QMI_NO_ERR)
    {
      QBRIDGE_LOG_E_1("QMUXBRIDGE :: Error %d while trying to allocate a "
                      "client for QMI_DPM", err);
      clientRetry++;
      continue;
    }
    else
    {
      QBRIDGE_LOG_D_1("QMUXBRIDGE :: Sending request (msg_id 0x%02x)", msg_id);
      err = qmi_client_send_msg_sync(
        handle, msg_id, req_data, req_data_len, rsp_data, rsp_data_len,
        QBRIDGE_QMI_MSG_TIMEOUT_VALUE_MS);
      if (err == QMI_TIMEOUT_ERR ||
          err != QMI_NO_ERR)
      {
        QBRIDGE_LOG_E_1("QMUXBRIDGE :: Error %d while sending QMI_DPM "
                        "request", err);
      }
      else
      {
        success = DPM_TRUE;
      }

      QBRIDGE_LOG_D_0("QMUXBRIDGE :: Releasing client ID");
      err = qmi_client_release(handle);
      if (err == QMI_TIMEOUT_ERR ||
          err != QMI_NO_ERR)
      {
        QBRIDGE_LOG_E_1("QMUXBRIDGE :: Error %d while trying to release "
                        "QMI_DPM client", err);
      }
      QBRIDGE_LOG_D_0("Done");
      break;
    }
  }

  return success;
} /* qbridge_dpm_send_qmi_req() */

/*=============================================================================

  Public Function Definition

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_mem_malloc_clear
===========================================================================*/
/*!
    @brief Allocate and zero out memory

    @details
    Equivalent to using calloc for a single element of the given size. Also
    logs an error message on allocation failures.

    @param size Size of memory to allocate in bytes

    @return void* Pointer to newly allocated memory, or NULL on failure
*/
/*=========================================================================*/
void *qbridge_mem_malloc_clear
(
  uint32  size
)
{
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  if (!size)
  {
    QBRIDGE_LOG_E_1("QMUXBRIDGE :: Memory allocation failure! "
                    "Tried to allocate %d bytes", size);
  }
  else
  {
    data = QBRIDGE_OS_MALLOC(size);
    if (data == NULL)
    {
      QBRIDGE_LOG_E_1("QMUXBRIDGE :: Memory allocation failure! "
                      "Tried to allocate %d bytes", size);
    }
    else
    {
      QBRIDGE_MEMSET(data, 0, size);
    }
  }

  return data;
} /* qbridge_mem_malloc_clear() */

/*===========================================================================
  FUNCTION: qbridge_os_qcci_os_params_init
===========================================================================*/
/*!
    @brief Initializes OS-specific parameters for use in QCCI synchronous
    APIs such as qmi_client_init()

    @details

    @param os_params
*/
/*=========================================================================*/
void qbridge_os_qcci_os_params_init
(
  qmi_client_os_params  *os_params
)
{
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET(os_params);

  QBRIDGE_MEMSET(os_params, 0, sizeof(qmi_client_os_params));

  return;
} /* qbridge_os_qcci_os_params_init() */

/*===========================================================================
  FUNCTION: qbridge_dpm_get_data_port_info
===========================================================================*/
/*!
    @brief Retrieves port details to use with DPM-related functionality

    @details

    @param qbridge_config_param
    @param data_ep_id
    @param ep_info Hardware-accelerated data port info. Optional, can be
    NULL

    @return boolean TRUE if data populated successfully, FALSE otherwise
*/
/*=========================================================================*/
boolean qbridge_dpm_get_data_port_info
(
  qbridge_rmnet_param  *qbridge_config_param,
  data_ep_id_type_v01  *data_ep_id,
  hardware_accl_port_details_v01  *ep_info
)
{
  int ret = 0;
  int status = FALSE;
  boolean data_copied = FALSE;
  uint32 retry_count = 0;
  hardware_accl_port_details_v01 local_ep_info;
/*-------------------------------------------------------------------------*/

  while (DPM_TRUE)
  {
    QBRIDGE_LOG_D_1("QMUXBRIDGE :: Triggering LOKUP IOCTL on node %d",
                    qbridge_config_param->ph_iface.ph_iface_fd);

    ret = ioctl(qbridge_config_param->ph_iface.ph_iface_fd,
                QBRIDGE_HC_LINUX_IOCTL_EP_LOOKUP, &local_ep_info);
    if (ret == DPM_FAILURE)
    {
      if (errno == EINVAL)
      {
        QBRIDGE_LOG_I_0("QMUXBRIDGE :: Driver does not support EP_LOOKUP "
                        "(DPM not used)");
        break;
      }
      else
      {
        /* It's possible that USB was not ready for the IOCTL yet, so retry */
        QBRIDGE_LOG_E_2("QMUXBRIDGE :: Couldn't get endpoint details from "
                        "driver: %d (retry %d)",errno, retry_count);
        if (retry_count++ > QBRIDGE_HC_LINUX_EP_LOOKUP_RETRY_COUNT)
        {
          QBRIDGE_LOG_E_0("QMUXBRIDGE :: Reached maximum number of retries "
                          "- giving up! Will sleep here indefinetly.");
          /*! An indefinite sleep is required to avoid killing 
              and respawning of qbridge in non mbim usb compositions. */
          sleep(-1);
        }
        usleep(QBRIDGE_HC_LINUX_EP_LOOKUP_RETRY_DELAY_US);
      }
    }
    else
    {
      status = DPM_TRUE;
      break;
    }
  }

  if (status == DPM_TRUE && data_ep_id != NULL)
  {
    QBRIDGE_MEMSCPY(data_ep_id, sizeof(data_ep_id_type_v01),
                &local_ep_info.ep_id, sizeof(local_ep_info.ep_id));
    data_copied = DPM_TRUE;
  }
  if (status == DPM_TRUE && ep_info != NULL)
  {
    QBRIDGE_MEMSCPY(ep_info, sizeof(hardware_accl_port_details_v01),
                &local_ep_info, sizeof(local_ep_info));
    QBRIDGE_LOG_D_2("QMUXBRIDGE :: ep_type %d iface_id %d",
         ep_info->ep_id.ep_type,ep_info->ep_id.iface_id);
    data_copied = DPM_TRUE;
  }

  return data_copied;
} /* qbridge_hc_dpm_get_data_port_info() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_dpm_open
===========================================================================*/
/*!
    @brief Opens QBRIDGE's control+data ports via data port mapper

    @details

    @param qbridge_config_param

    @return TRUE or FLASE
*/
/*=========================================================================*/
boolean qbridge_dpm_open
(
  qbridge_rmnet_param  *qbridge_config_param
)
{
  dpm_open_port_req_msg_v01 *qmi_req_gps = NULL;
  dpm_open_port_resp_msg_v01 *qmi_rsp_gps = NULL;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/

  qmi_req_gps = qbridge_mem_malloc_clear(sizeof(dpm_open_port_req_msg_v01));
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qmi_req_gps);

  if (!qbridge_dpm_get_gps_port_info(
        qbridge_config_param,
        qmi_req_gps->control_port_list[0].port_name,
        sizeof(qmi_req_gps->control_port_list[0].port_name),
        &qmi_req_gps->control_port_list[0].default_ep_id,
        &qmi_req_gps->hardware_data_port_list[0]))
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Couldn't get port details!");
  }
  else
  {
    qmi_rsp_gps = qbridge_mem_malloc_clear(sizeof(dpm_open_port_resp_msg_v01));
    QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qmi_rsp_gps);

    qmi_req_gps->control_port_list_valid = DPM_TRUE;
    qmi_req_gps->control_port_list_len = 1;
    qmi_req_gps->hardware_data_port_list_valid = DPM_FALSE;

    QBRIDGE_LOG_D_0("QMUXBRIDGE :: Sending QMI_DPM_OPEN_PORT_REQ for GPS");
    success = qbridge_dpm_send_qmi_req(
      QMI_DPM_OPEN_PORT_REQ_V01, qmi_req_gps, sizeof(dpm_open_port_req_msg_v01),
      qmi_rsp_gps, sizeof(dpm_open_port_resp_msg_v01));
    if (success && qmi_rsp_gps->resp.result != QMI_RESULT_SUCCESS_V01)
    {
      QBRIDGE_LOG_E_1("QMUXBRIDGE :: Port mapper open failed with error "
                      "%d", qmi_rsp_gps->resp.error);
    }
    free(qmi_rsp_gps);
  }
  free(qmi_req_gps);

  /* Response allocated on the stack since it's just the result code */
  return success;
} /* qbridge_dpm_open() */

/*===========================================================================
  FUNCTION: qbridge_dpm_close
===========================================================================*/
/*!
    @brief Closes QBRIDGE's control+data ports via data port mapper

    @details

    @param qbridge_config_param

    @return TRUE or FLASE
*/
/*=========================================================================*/
boolean qbridge_dpm_close
(
  qbridge_rmnet_param  *qbridge_config_param
)
{
  dpm_close_port_req_msg_v01 *qmi_req_gps = NULL;
  dpm_close_port_resp_msg_v01 *qmi_rsp_gps = NULL;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/

  qmi_req_gps = qbridge_mem_malloc_clear(sizeof(dpm_close_port_req_msg_v01));
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qmi_req_gps);

  if (!qbridge_dpm_get_gps_port_info(
        qbridge_config_param,
        qmi_req_gps->control_port_list[0].port_name,
        sizeof(qmi_req_gps->control_port_list[0].port_name),
        &qmi_req_gps->data_port_list[0], NULL))
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Couldn't get port details!");
  }
  else
  {
    qmi_rsp_gps = qbridge_mem_malloc_clear(sizeof(dpm_close_port_resp_msg_v01));
    QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qmi_rsp_gps);

    qmi_req_gps->control_port_list_valid = DPM_TRUE;
    qmi_req_gps->control_port_list_len = 1;
    qmi_req_gps->data_port_list_valid = DPM_TRUE;
    qmi_req_gps->data_port_list_len = 1;

    QBRIDGE_LOG_D_0("QMUXBRIDGE :: Sending QMI_DPM_CLOSE_PORT_REQ for GPS");
    success = qbridge_dpm_send_qmi_req(
      QMI_DPM_CLOSE_PORT_REQ_V01, qmi_req_gps, sizeof(dpm_close_port_req_msg_v01),
      qmi_rsp_gps, sizeof(dpm_close_port_req_msg_v01));
    if (success && qmi_rsp_gps->resp.result != QMI_RESULT_SUCCESS_V01)
    {
      QBRIDGE_LOG_E_1("QMUXBRIDGE :: Port mapper close failed with "
                      "error %d", qmi_rsp_gps->resp.error);
    }
    free(qmi_rsp_gps);
  }
  free(qmi_req_gps);

  /* Response allocated on the stack since it's just the result code */
  return success;
} /* qbridge_dpm_open() */
