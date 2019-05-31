/*!
  @file
  qbi_svc_bc_ext_dssa.h

  @brief
  Basic Connectivity Extension device service definitions, to implement Dual
  SIM Single Active requirements.
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

#ifndef QBI_SVC_BC_EXT_DSSA_H
#define QBI_SVC_BC_EXT_DSSA_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_qmi_txn.h"


/*=============================================================================

  Constants and Macros

=============================================================================*/

#define QBI_SVC_BC_EXT_DSSA_CARD_STATUS_INDEX  (0)

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/

qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_info_uim32_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_q_req
(
  qbi_txn_s *txn
);

qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_info_uim48_ind_cb
(
    const qbi_svc_qmi_ind_data_s *ind
);


qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_s_req
(
  qbi_txn_s *txn
);

qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_info_status_q_req
(
  qbi_txn_s *txn
);

#endif /* QBI_SVC_BC_EXT_DSSA_H */
