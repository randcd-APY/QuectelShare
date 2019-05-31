/*!
  @file
  qbi_svc_msfwid.c

  @brief
  Microsoft Firmware ID device service implementation, based on the "Mobile
  Broadband Device Firmware Update on Windows 8" document from Microsoft.
  This device service allows host to query Firmware ID from device.
*/

/*=============================================================================

  Copyright (c) 2012-2014, 2017 Qualcomm Technologies, Inc.
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
06/24/13  bd   Updated to use NV store API
08/17/12  hz   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_msfwid.h"
#include "qbi_svc_msfwid_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

/*! @addtogroup MBIM_CID_MSFIRMWAREID
    @{ */

static qbi_svc_action_e qbi_svc_msfwid_firmwareid_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_msfwid_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_msfwid_cmd_hdlr_tbl[] = {
  /* MBIM_CID_MSFIRMWAREID */
  {qbi_svc_msfwid_firmwareid_q_req, 0, NULL, 0}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*! @addtogroup MBIM_CID_MSFIRMWAREID
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_msfwid_firmwareid_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MSFIRMWAREID query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msfwid_firmwareid_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_msfwid_firmwareid_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  rsp = (qbi_svc_msfwid_firmwareid_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_msfwid_firmwareid_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  /*! @note The OEM is expected to provision a Firmware ID to QBI's storage
      area in the factory to enable firmware updates, or otherwise customize
      this implementation to suit their firmware update requirements. */
  if (qbi_nv_store_cfg_item_read(
        txn->ctx, QBI_NV_STORE_CFG_ITEM_FIRMWARE_ID, rsp->firmware_id,
        sizeof(rsp->firmware_id)))
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    txn->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
  }

  return action;
} /* qbi_svc_msfwid_firmwareid_q_req() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_msfwid_init
===========================================================================*/
/*!
    @brief One-time initialization of the MSFIRMWAREID device service

    @details
*/
/*=========================================================================*/
void qbi_svc_msfwid_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_msfwid_cfg = {
    {
      0xe9, 0xf7, 0xde, 0xa2, 0xfe, 0xaf, 0x40, 0x09,
      0x93, 0xce, 0x90, 0xa3, 0x69, 0x41, 0x03, 0xb6
    },
    QBI_SVC_ID_MSFWID,
    FALSE,
    qbi_svc_msfwid_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_msfwid_cmd_hdlr_tbl),
    NULL,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_msfwid_cfg);
} /* qbi_svc_msfwid_init() */

