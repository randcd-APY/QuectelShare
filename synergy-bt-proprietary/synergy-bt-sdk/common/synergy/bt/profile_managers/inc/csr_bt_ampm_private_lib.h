#ifndef CSR_BT_AMPM_PRIVATE_LIB_H__
#define CSR_BT_AMPM_PRIVATE_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_types.h"
#include "rfcomm_prim.h"
#include "hci_prim.h"
#include "csr_bt_ampm_prim.h"
#include "csr_bt_tasks.h"
#include "csr_mblk.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CSR_AMP_ENABLE

/* Common put_message function to reduce code size */
void CsrBtAmpmMsgTransport(void* msg);

/* Construct and send the CsrBtAmpmRegisterReq primitive */
void CsrBtAmpmPrfMultiConnReqSend(CsrBtConnId master, CsrUint8 slavesCount, const CsrBtConnId *slaves);

#else

/* AMP not enabled, do not use this feature */
#define CsrBtAmpmPrfMultiConnReqSend(_m,_c,_s)

#endif

#ifdef __cplusplus
}
#endif

#endif
