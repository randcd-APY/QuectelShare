/******************************************************************************

                        QBRIDGE_DPM.H

******************************************************************************/

/******************************************************************************

  @file    qbridge_dpm.h
  @brief   Handles DPM related QMI Interface Call

  DESCRIPTION
  Implementation of Tethering Interface module.

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
05/02/18    rv         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#define DPM_SUCCESS                               (0)
#define DPM_FAILURE                               (-1)
#define DPM_TRUE                                  (1)
#define DPM_FALSE                                 (0)

#define QBRIDGE_HC_LINUX_EP_LOOKUP_RETRY_COUNT    (10)
#define QBRIDGE_HC_LINUX_EP_LOOKUP_RETRY_DELAY_US (50000)

/*! Timeout for synchronous QCCI requests */
#define QBRIDGE_OS_LINUX_QCCI_TIMEOUT_MS          (30000)

/*! Timeout for QMI requests */
#define QBRIDGE_QMI_MSG_TIMEOUT_VALUE_MS          (30000)

#define QBRIDGE_HC_LINUX_IOCTL_EP_LOOKUP \
  _IOR(QBRIDGE_HC_LINUX_IOCTL_MAGIC, 4, hardware_accl_port_details_v01)

#define QBRIDGE_HC_LINUX_IOCTL_MAGIC 'o'

/*! DPM Control Port name */
#define QBRDIGE_DPM_CONTROL_PORT                  "DATA6_CNTL"

/*===========================================================================
  FUNCTION: qbridge_dpm_open
===========================================================================*/
/*!
    @brief Opens QBRIDGE's control+data ports via data port mapper

    @details

    @param ctx

    @return
*/
/*=========================================================================*/
boolean qbridge_dpm_open
(
  qbridge_rmnet_param  *qbridge_config_param
);

/*===========================================================================
  FUNCTION: qbridge_dpm_close
===========================================================================*/
/*!
    @brief Closes QBRIDGE's control+data ports via data port mapper

    @details

    @param ctx

    @return
*/
/*=========================================================================*/
boolean qbridge_dpm_close
(
  qbridge_rmnet_param  *qbridge_config_param
);

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
);