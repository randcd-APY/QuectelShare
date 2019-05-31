/*!
  @file
  qbi_svc_bc_ext_lte.h

  @brief
  Basic Connectivity Extension device service definitions, based on LTE ATTACH
  feature requirements.
*/

/*=============================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
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
10/28/17  rv   Avoid deregister in case of 3G
10/27/17  nk   Added header files and moved function prototypes from bc_ext.h
=============================================================================*/

#ifndef QBI_SVC_BC_EXT_LTE_H
#define QBI_SVC_BC_EXT_LTE_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_qmi_txn.h"

/*=============================================================================

  Function Prototypes

=============================================================================*/


qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_static_ind_e_wds95_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_e_dsd26_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_reg_ind_dsd25_rsp
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_CONFIG
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_config_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_LTE_ATTACH_CONFIG query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_req
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_STATUS
@{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_STATUS query
    request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_req
(
  qbi_txn_s *txn
);

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_build_nas33_req
===========================================================================*/
/*!
  @brief Allocates and populates
         QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01 request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_nas33_req
(
  qbi_txn_s *txn,
  boolean flag
);

/*! @} */

#endif /* QBI_SVC_BC_EXT_LTE_H */

