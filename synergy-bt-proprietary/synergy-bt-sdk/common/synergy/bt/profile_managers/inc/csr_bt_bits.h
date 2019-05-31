#ifndef CSR_BT_CSR_BT_BITS_H__
#define CSR_BT_CSR_BT_BITS_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_panic.h"
#include "csr_pmem.h"
#include "csr_bt_tasks.h"
#include "csr_bt_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_BIT_CLEAR(a, nth)  ((a) &= ~((CsrUint8) (1 << (nth))))
#define CSR_BT_BIT_ENABLE(a, nth) ((a) |= ((CsrUint8) (1 << (nth))))
#define CSR_BT_BIT_TOGGLE(a, nth) ((a) ^= ((CsrUint8) (1 << (nth))))
#define CSR_BT_BIT_GET(a, nth)    (((a) & ((CsrUint8) (1 << (nth)))) ? 1 : 0)

#ifdef __cplusplus
}
#endif

#endif /* ndef _CM_MAIN_H */
