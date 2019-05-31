#ifndef CSR_BT_PANIC_H__
#define CSR_BT_PANIC_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_panic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_PANIC_MYSTERY                             0  /* PANIC_MYSTERY */
#define CSR_BT_PANIC_MBLK_MAP_ERROR                      1  /* PANIC_MBLK_MAP_ERROR */
#define CSR_BT_PANIC_L2CAP_HCI_DATA_CREDITS_INCONSISTENT 2  /* PANIC_L2CAP_HCI_DATA_CREDITS_INCONSISTENT */
#define CSR_BT_PANIC_INVALID_BLUESTACK_PRIMITIVE         3  /* PANIC_INVALID_BLUESTACK_PRIMITIVE */
#define CSR_BT_PANIC_INVALID_ESCO_TX_INTERVAL            4  /* PANIC_INVALID_ESCO_TX_INTERVAL */
#define CSR_BT_PANIC_HEAP_EXHAUSTION                     5  /* PANIC_HEAP_EXHAUSTION */
#define CSR_BT_PANIC_MBLK_MSGFRAG_COALESCE_FAILURE       6  /* PANIC_MBLK_MSGFRAG_COALESCE_FAILURE */
#define CSR_BT_PANIC_MBLK_DISCARD_TAIL_ERROR             7  /* PANIC_MBLK_DISCARD_TAIL_ERROR */
#define CSR_BT_PANIC_RFCOMM_TIMER_ALREADY_STARTED        8  /* PANIC_RFCOMM_TIMER_ALREADY_STARTED */
#define CSR_BT_PANIC_RFCOMM_INVALID_TIMER_TYPE           9  /* PANIC_RFCOMM_INVALID_TIMER_TYPE */
#define CSR_BT_PANIC_RFCOMM_INVALID_TIMER_CONTEXT        10 /* PANIC_RFCOMM_INVALID_TIMER_CONTEXT */
#define CSR_BT_PANIC_MBLK_CREATE_FAILURE                 11 /* PANIC_MBLK_CREATE_FAILURE */
#define CSR_BT_PANIC_RFCOMM_L2CAP_REGISTER_FAILED        12 /* PANIC_RFCOMM_L2CAP_REGISTER_FAILED */
#define CSR_BT_PANIC_RFCOMM_STREAM_MISMATCH              13 /* PANIC_RFCOMM_STREAM_MISMATCH */
#define CSR_BT_PANIC_DM_ACL_LOCKS_EXHAUSED               14 /* PANIC_DM_ACL_LOCKS_EXHAUSED */
#define CSR_BT_PANIC_INVALID_ULP_BUFFER_SIZE_RESPONSE    15 /* PANIC_INVALID_ULP_BUFFER_SIZE_RESPONSE */
#define CSR_BT_PANIC_SM_L2CAP_HANDLER                    16 /* PANIC_SM_L2CAP_HANDLER */

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PANIC_H__ */

