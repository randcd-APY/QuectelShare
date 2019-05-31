/*!
  @file
  qbi_hc.h

  @brief
  QBI Common Host Communications layer
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
03/19/18  ha   Add support for mhi uevent and deprecate pcie_enabled NV
01/30/18  ha   Add support to monitor mhi_ctrl
01/05/18  ha   Add support for PCIe enablement
10/28/13  bd   Add support for Data Port Mapper (DPM)
03/13/13  hz   Add support for multiple data sessions
01/25/13  bd   Support configuration of data path in qbi_hc layer
11/20/12  bd   Differentiate active and next diag config
05/18/12  bd   Add function to get wMaxSegmentSize
01/30/12  cy   Add NTB max num/size config and Reset function
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_HC_H
#define QBI_HC_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_txn.h"

#include "data_port_mapper_v01.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! @brief Defines whether a USB composition includes DIAG
    @details Only two preconfigured compositions are supported, one with a DIAG
    endpoint and one without. If the composition does not match one of the two
    preconfigured settings, no attempt is made to determine whether it includes
    DIAG or not - it is considered UNKNOWN.
*/
typedef enum {
  QBI_HC_DIAG_CONFIG_EXCLUDED = 0,
  QBI_HC_DIAG_CONFIG_INCLUDED = 1,
  QBI_HC_DIAG_CONFIG_UNKNOWN  = 2
} qbi_hc_diag_config_e;

/*! Status codes for qbi_hc_diag_config_set_next() operation */
typedef enum {
  QBI_HC_DIAG_CONFIG_SET_STATUS_SUCCESS        = 0,
    /*!< The operation completed successfully */

  QBI_HC_DIAG_CONFIG_SET_STATUS_INVALID_PARAM  = 1,
    /*!< Invalid requested configuration */

  QBI_HC_DIAG_CONFIG_SET_STATUS_READ_FAILURE   = 2,
    /*!< Unable to read current composition */

  QBI_HC_DIAG_CONFIG_SET_STATUS_UNKNOWN_CONFIG = 3,
    /*!< Current configuration is not a known diag included/excluded
         composition */

  QBI_HC_DIAG_CONFIG_SET_STATUS_WRITE_FAILURE  = 4,
    /*!< Unable to write the new composition */
} qbi_hc_diag_config_set_status_e;

/*! mhi ctrl possible states. These states reflect the possible
    mhi pipe connectivity states between the mhi LE layer in the
    modem and mhi driver in the host. */
typedef enum {
  QBI_HC_MHI_CTRL_UNKNOWN = 0,
  /*< mhi_ctrl file content is not MHI_STATE=DISCONNECTED, is not
      MHI_STATE=CONNECTED, is not MHI_STATE=CONFIGURED */

  QBI_HC_MHI_CTRL_DISCONNECTED,
  /*< mhi_ctrl file content is MHI_STATE=DISCONNECTED */

  QBI_HC_MHI_CTRL_CONNECTED,
  /*< mhi_ctrl file content is MHI_STATE=CONNECTED */

  QBI_HC_MHI_CTRL_CONFIGURED,
  /*< mhi_ctrl file content is MHI_STATE=CONFIGURED */
} qbi_hc_mhi_ctrl_state_e;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_hc_data_format_configure
===========================================================================*/
/*!
    @brief Configures the data format for MBIM using platform-specific means

    @details
    This function must not be called if
    qbi_hc_data_format_is_configured_by_hc() returns FALSE.

    @param ctx
    @param listener_txn Pointer to a configured listener transaction to be
    notified of the result of asynchronous operations

    @return boolean TRUE on success, FALSE on failure. Note that a return
    value of TRUE does not mean that configuration is complete - if any async
    operations are required, their result will be communicated through the
    listener transaction.
*/
/*=========================================================================*/
boolean qbi_hc_data_format_configure
(
  qbi_ctx_s *ctx,
  qbi_txn_s *listener_txn
);

/*===========================================================================
  FUNCTION: qbi_hc_data_format_is_configured_by_hc
===========================================================================*/
/*!
    @brief Determines whether the data format is configured by the host
    communications platform layer, and QMI_WDA_SET_DATA_FORMAT should not
    be used

    @details

    @param ctx

    @return boolean TRUE if the data format should be configured by the
    host communications platform layer, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_hc_data_format_is_configured_by_hc
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_hc_diag_config_get_active
===========================================================================*/
/*!
    @brief Determines whether a DIAG endpoint is included in the active USB
    composition

    @details

    @param active_diag_config

    @return boolean TRUE if active_diag_config was set, FALSE if a failure
    occurred and active_diag_config was not modified
*/
/*=========================================================================*/
boolean qbi_hc_diag_config_get_active
(
  qbi_hc_diag_config_e *active_diag_config
);

/*===========================================================================
  FUNCTION: qbi_hc_diag_config_get_next
===========================================================================*/
/*!
    @brief Determine if the USB composition that will be used on next boot
    matches one of two preconfigured settings known to include or exclude
    a DIAG endpoint

    @details

    @param next_diag_config Will be set to the composition type

    @return boolean TRUE if next_diag_config was set, FALSE if a failure
    occurred and next_diag_config was not modified
*/
/*=========================================================================*/
boolean qbi_hc_diag_config_get_next
(
  qbi_hc_diag_config_e *next_diag_config
);

/*===========================================================================
  FUNCTION: qbi_hc_diag_config_set_next
===========================================================================*/
/*!
    @brief Set the USB composition that will be used on next boot to one of
    two preconfigured settings, which either include or exclude a DIAG
    endpoint

    @details
    The new USB composition may require a device reset to take effect

    @param requested_diag_config

    @return qbi_hc_diag_config_set_status_e
*/
/*=========================================================================*/
qbi_hc_diag_config_set_status_e qbi_hc_diag_config_set_next
(
  qbi_hc_diag_config_e requested_diag_config
);

/*===========================================================================
  FUNCTION: qbi_hc_dpm_is_used
===========================================================================*/
/*!
    @brief Determines whether Data Port Mapper (DPM) is supported and
    required by the platform (e.g. MDM9x35)

    @details
    Note that this does not necessarily mean that QBI must send the DPM
    messages, only that DPM is used on the platform.

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_hc_dpm_is_used
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_hc_dpm_get_data_port_info
===========================================================================*/
/*!
    @brief Retrieves port details to use with DPM-related functionality

    @details

    @param ctx
    @param data_ep_id
    @param ep_info Hardware-accelerated data port info. Optional, can be
    NULL

    @return boolean TRUE if data populated successfully, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_hc_dpm_get_data_port_info
(
  const qbi_ctx_s                *ctx,
  data_ep_id_type_v01            *data_ep_id,
  hardware_accl_port_details_v01 *ep_info
);

/*===========================================================================
  FUNCTION: qbi_hc_dpm_get_mux_id
===========================================================================*/
/*!
    @brief Returns the mux_id to be used for data calls on this session.

    @details

    @param ctx
    @param session_id session id for this data call.

    @return mux_id to be used.
*/
/*=========================================================================*/
uint8_t qbi_hc_dpm_get_mux_id
(
  const qbi_ctx_s *ctx,
  uint8_t          session_id
);

/*===========================================================================
  FUNCTION: qbi_hc_get_bind_data_port
===========================================================================*/
/*!
    @brief Retrieve data port based on port index for binding operation

    @details

    @param wds_svc_id
*/
/*=========================================================================*/
uint16 qbi_hc_get_bind_data_port
(
  uint32 index
);

/*===========================================================================
  FUNCTION: qbi_hc_get_max_xfer
===========================================================================*/
/*!
    @brief Returns the maximum control transfer size supported by the device

    @details

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_hc_get_max_xfer
(
  void
);

/*===========================================================================
  FUNCTION: qbi_hc_get_dl_ntb_max_size
===========================================================================*/
/*!
    @brief Returns the maximum size of an input NTB. This is the
    dwNtbInMaxSize value set by the host via SetNtbInputSize (refer to USB
    CDC NCM 1.0 specification).

    @details

    @param ctx

    @return uint32 IN NTB maximum size in bytes. This function may return
    zero to indicate that host did not set this value via SetNtbInputSize,
    so the device should use its default value.
*/
/*=========================================================================*/
uint32 qbi_hc_get_dl_ntb_max_size
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_hc_get_dl_ntb_max_datagrams
===========================================================================*/
/*!
    @brief Returns the maximum number of datagrams allowed in each input NTB.
    This is the wNtbInMaxDatagrams value set by the host via SetNtbInputSize
    (refer to USB CDC NCM 1.0 specification).

    @details

    @param ctx

    @return uint32 Maximum number of datagrams within the IN NTB. Zero means
    no limit.
*/
/*=========================================================================*/
uint32 qbi_hc_get_dl_ntb_max_datagrams
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_hc_get_max_segment_size
===========================================================================*/
/*!
    @brief Returns the maximum segment size supported by the device

    @details
    This is the wMaxSegmentSize value reported in the MBIM functional
    descriptor. The device must never report an MTU that is larger than
    this value.

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_hc_get_max_segment_size
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_hc_init
===========================================================================*/
/*!
    @brief Perform one-time initialization of the host communications layer

    @details

*/
/*=========================================================================*/
void qbi_hc_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_hc_tx
===========================================================================*/
/*!
    @brief Transmit data to the host

    @details

    @param ctx
    @param data
    @param len
*/
/*=========================================================================*/
void qbi_hc_tx
(
  qbi_ctx_s *ctx,
  void      *data,
  uint32     len
);

/*===========================================================================
  FUNCTION: qbi_hc_pcie_is_enabled
===========================================================================*/
/*!
    @brief Checks if PCIe is enabled

    @details Reads pcie_enabled field from qbi_hc_linux_info_s

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_hc_pcie_is_enabled
(
  qbi_ctx_s *ctx
);

#endif /* QBI_HC_H */

