#ifndef CSR_BT_BLUESTACK_CONFIG_H_
#define CSR_BT_BLUESTACK_CONFIG_H_
/******************************************************************************
 Copyright (c) 2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 REVISION:      $Revision: #1 $
******************************************************************************/

/* This header file is used to override Bluestack configuration parameters which
 * are required to be calibrated for Synergy but not exposed to application developers */

#include "csr_synergy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Configurable timeout multiplier to keep the idle link alive post pairing */
#ifdef DM_CONFIG_ACL_IDLE_TIMEOUT_LE_TINY
#undef DM_CONFIG_ACL_IDLE_TIMEOUT_LE_TINY
#endif
#define DM_CONFIG_ACL_IDLE_TIMEOUT_LE_PAIRING       0   /* Do not delay LE ACL disconnection after pairing */

/* Configurable timeout for disconnection of ACL after LE fixed(for now) channel */
#ifdef DM_CONFIG_ACL_IDLE_TIMEOUT_LE_TINY
#undef DM_CONFIG_ACL_IDLE_TIMEOUT_LE_TINY
#endif
#define DM_CONFIG_ACL_IDLE_TIMEOUT_LE_TINY          0   /* Do not delay LE ACL disconnection when
                                                           there are no stakeholders of the link */

#include "dm_config.h"

#ifdef __cplusplus
}
#endif

#endif /* BLUESTACK_CONFIG */
