/******************************************************************************

                        QBRIDGE_QMI.C

******************************************************************************/

/******************************************************************************

  @file    qbridge_qmi.c
  @brief   Handle initialization of basic routine for communication with
           modem via QMI interface

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
06/06/18   rv         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "qbridge_log_qxdm.h"
#include "qbridge_qmux.h"
#include "qbridge_main.h"
#include "qbridge_qmi.h"
#include "wireless_data_administrative_service_v01.h"

/*=============================================================================

  Private Function Prototypes

=============================================================================*/
static void qbridge_qmi_client_error_cb
(
 qmi_client_type  user_handle,
 qmi_client_error_type  error,
 void  *err_cb_data
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_qmi_ind_cb
===========================================================================*/
/*!
    @brief Callback handling

    @details
    Runs outside of QBRIDGE task, so no QBRIDGE internal data should be
    accessed in this function without a mutex.

    @param user_handle
    @param msg_id
    @param ind_buf
    @param ind_buf_len
    @param ind_cb_data

    @return void
*/
/*=========================================================================*/
static void qbridge_qmi_ind_cb
(
  qmi_client_type user_handle,
  unsigned int    msg_id,
  void           *ind_buf,
  unsigned int    ind_buf_len,
  void           *ind_cb_data
)
{
  QBRIDGE_LOG_D_0("QMUXBRIDGE :: Indication Callback Received");
  return;
}/* qbridge_qmi_ind_cb*/

/*===========================================================================
  FUNCTION: qbridge_qmi_client_error_cb
===========================================================================*/
/*!
    @brief Error callback registered with QCCI during client initialization

    @details
    Used to handle modem subsystem restart

    @param user_handle
    @param error
    @param err_cb_data

    @return void
*/
/*=========================================================================*/
static void qbridge_qmi_client_error_cb
(
 qmi_client_type  user_handle,
 qmi_client_error_type  error,
 void  *err_cb_data
)
{
  qbridge_rmnet_param *qbridge_config_param = NULL;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET(err_cb_data);

  QBRIDGE_LOG_D_0("QMUXBRIDGE :: Received error callback");
  qbridge_config_param = (qbridge_rmnet_param  *)err_cb_data;

  if (error == QMI_SERVICE_ERR)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Received modem restart notification!");
    qbridge_config_param->ssr_in_progress = TRUE;
    qbridge_close_interface_towards_modem();
  }
  else
  {
    QBRIDGE_LOG_W_1("QMUXBRIDGE :: Ignoring QCCI error %d", error);
  }

  return;
} /* qbridge_qmi_client_error_cb() */

/*===========================================================================
  FUNCTION: qbridge_qmi_wait_for_svc_then_get_info
===========================================================================*/
/*!
    @brief Waits for a QMI service to become ready, then fetches the
    qmi_service_info data associated with the QMI service on the connection
    QBI uses

    @details
 
    @param svc_obj
    @param qmi_svc_info Will be populated with QMI service information to
    be used in a call to qmi_client_init()

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbridge_qmi_wait_for_svc_then_get_info
(
  qmi_idl_service_object_type  svc_obj,
  qmi_service_info  *qmi_svc_info
)
{
  boolean success = FALSE;
  qmi_client_os_params os_params;
  qmi_client_type notifier_handle;
  qmi_client_error_type err;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qmi_svc_info);

  QBRIDGE_MEMSET(&os_params, 0, sizeof(qmi_client_os_params));
  err = qmi_client_notifier_init(svc_obj, &os_params, &notifier_handle);
  if (err != QMI_NO_ERR)
  {
    QBRIDGE_LOG_E_1("QMUXBRIDGE :: Error %d while trying to initialize "
                    "client notifier", err);
  }
  else
  {
    /* QMI QMUX may not be available when QMI_CCI_OS_SIGNAL_WAIT is unblocked
       by signal for non-QMUX transport. Additional attempts are necessary in
       order to get qmi service instance over QMUX. */
    while (success != TRUE)
    {
      QMI_CCI_OS_SIGNAL_WAIT(&os_params, QBRIDGE_QMI_SVC_INIT_TIMEOUT_MS);
      if (QMI_CCI_OS_SIGNAL_TIMED_OUT(&os_params))
      {
        QBRIDGE_LOG_E_0("QMUXBRIDGE :: Timeout occured while waiting "
                        "for signal");
        break;
      }
      QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
      err = qmi_client_get_service_instance(
       svc_obj, (qmi_service_instance)QMI_CLIENT_INSTANCE_ANY, qmi_svc_info);
      if (err != QMI_NO_ERR)
      {
        QBRIDGE_LOG_E_1("QMUXBRIDGE :: Error %d while trying to get "
                        "service info", err);
      }
      else
      {
        success = TRUE;
      }
    }
    /*! @note The notifier handle is similar to a client but it has very limited
        functionality, which we only use to wait for the service to become
        ready. Once we are done waiting, we need to release it using the same
        function as used for a regular client handle. This must happen after
        calling qmi_client_get_service_instance(), otherwise that function will
        fail with QMI_SERVICE_ERR. */
    err = qmi_client_release(notifier_handle);
    if (err != QMI_NO_ERR)
    {
      QBRIDGE_LOG_W_1("QMUXBRIDGE :: Error %d while releasing client notifier"
                      " handle", err);
    }
  }

  return success;
}

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_qmi_init_client
===========================================================================*/
/*!
    @brief Initializes a client handle with a QMI service

    @details
    Performs actual initialization of the QMI service's client handle with
    the underlying QMI client API. It is assumed that before entering this
    function, the QMI service handle's reference count is 0. On success,
    the reference count will be set to 1.

    @return boolean TRUE on success, FALSE on failure

    @see qbridge_qmi_release_client
*/
/*=========================================================================*/
boolean qbridge_qmi_init_client
(
  qbridge_rmnet_param  *qbridge_config_param
)
{
  qmi_client_error_type err = 0;
  qmi_service_info qmi_svc_info;
  qmi_idl_service_object_type svc_obj;
  boolean success = FALSE;
  qmi_client_os_params os_params;
  qmi_client_type handle;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qbridge_config_param);

  svc_obj = wda_get_service_object_v01();
  if (NULL == svc_obj)
  {
    QBRIDGE_LOG_E_2("QMUXBRIDGE :: %s:%d]: wda_get_service_object_v01 failed\n" ,
                     __func__, __LINE__ );
  }
  else if (!qbridge_qmi_wait_for_svc_then_get_info(
            svc_obj, &qmi_svc_info))
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Error getting info for QMI service");
  }
  else
  {
    qbridge_os_qcci_os_params_init(&os_params);
    err = qmi_client_init(
      &qmi_svc_info, svc_obj, qbridge_qmi_ind_cb, (void *) qbridge_config_param,
      &os_params, &handle);
    if (err != QMI_NO_ERR)
    {
      QBRIDGE_LOG_E_1("QMUXBRIDGE :: Error %d while trying to initialize "
                      "client", err);
    }
    else
    {
      QBRIDGE_LOG_I_0("QMUXBRIDGE :: Successfully allocated client");
      success = TRUE;
      err = qmi_client_register_error_cb(
          handle, qbridge_qmi_client_error_cb, (void *) qbridge_config_param);
      if (err != QMI_NO_ERR)
      {
        QBRIDGE_LOG_E_1("QMUXBRIDGE :: Error %d while trying to initialize "
                        "client", err);
      }
    }
  }

  return success;
} /* qbridge_qmi_init_client() */
