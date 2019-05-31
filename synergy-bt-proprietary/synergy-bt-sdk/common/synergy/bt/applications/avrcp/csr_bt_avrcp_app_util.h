#ifndef CSR_BT_AVRCP_APP_UTIL_H__
#define CSR_BT_AVRCP_APP_UTIL_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_avrcp.h"
#include "csr_util.h"
#include "csr_unicode.h"

#ifdef __cplusplus
extern "C" {
#endif




void CsrBtAvrcpRestoreSavedMessages(avrcpinstance_t * instData);

void CsrBtAvrcpAddItem(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 position,
                  CsrUint16 key, CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel);

void CsrBtAvrcpSetItem(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 key,
                                CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel);

void CsrBtAvrcpSetDialog(avrcpinstance_t * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtAvrcpSetInputDialog(avrcpinstance_t * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtAvrcpMenuSet(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 *heading,
                  CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtAvrcpShowUi(avrcpinstance_t * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority);

void CsrBtAvrcpHideUi(avrcpinstance_t * inst, CsrUint8 UiIndex);

void CsrBtAvrcpDisplaySetInputMode(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 inputMode);

void CsrBtAvrcpMenuRemoveAllItems(avrcpinstance_t * inst, CsrUint8 UiIndex);

void CsrBtAvrcpMenuRemoveItem(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 key);






#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_AVRCP_APP_UTIL_H__ */
