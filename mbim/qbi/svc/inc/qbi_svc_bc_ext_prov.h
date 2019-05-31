/*!
  @file
  qbi_svc_bc_ext_prov.h

  @brief
  Basic Connectivity Extension device service definitions, based on PROVISIONING
  CONTEXT feature requirements.
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
10/27/17  nk   Added header files and moved function prototypes from bc_ext.h
=============================================================================*/

#ifndef QBI_SVC_BC_EXT_PROV_H
#define QBI_SVC_BC_EXT_PROV_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_qmi_txn.h"

/*=============================================================================

  Function Prototypes

=============================================================================*/

qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_wdsa8_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_req
(
  qbi_txn_s *txn
);

qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_req
(
  qbi_txn_s *txn
);

qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_req
(
  qbi_qmi_txn_s *qmi_txn
);

qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_pdc2f_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_configure_nv_for_operator
===========================================================================*/
/*!
    @brief Configures NV for specific operator if required

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_configure_nv_for_operator
(
  qbi_txn_s *txn
);

#endif /* QBI_SVC_BC_EXT_PROV_H */
