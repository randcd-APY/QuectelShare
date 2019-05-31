#ifndef CSR_BT_AV_APP_UTIL_H__
#define CSR_BT_AV_APP_UTIL_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_av2.h"
#include "csr_util.h"
#include "csr_unicode.h"

#ifdef __cplusplus
extern "C" {
#endif




void CsrBtAvRestoreSavedMessages(av2instance_t * instData);

void CsrBtAvAddItem(av2instance_t * inst, CsrUint8 UiIndex, CsrUint16 position,
                  CsrUint16 key, CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel);

void CsrBtAvSetItem(av2instance_t * inst, CsrUint8 UiIndex, CsrUint16 key,
                                CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel);

void CsrBtAvSetDialog(av2instance_t * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtAvSetInputDialog(av2instance_t * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtAvpMenuSet(av2instance_t * inst, CsrUint8 UiIndex, CsrUint16 *heading,
                  CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtAvShowUi(av2instance_t * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority);

void CsrBtAvHideUi(av2instance_t * inst, CsrUint8 UiIndex);

void CsrBtAvDisplaySetInputMode(av2instance_t * inst, CsrUint8 UiIndex, CsrUint16 inputMode);

void CsrBtAvMenuRemoveAllItems(av2instance_t * inst, CsrUint8 UiIndex);

void CsrBtAvMenuRemoveItem(av2instance_t * inst, CsrUint8 UiIndex, CsrUint16 key);






#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_AV_APP_UTIL_H__ */
