#ifndef CSR_BT_HFG_APP_UTIL_H__
#define CSR_BT_HFG_APP_UTIL_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_hfg_demo_app.h"
#include "csr_util.h"
#include "csr_unicode.h"
#include "csr_bt_hfg_app_ui_strings.h"
#include "csr_bt_hfg_app_ui_sef.h"
#include "csr_bt_hfg_app_task.h"
#include "csr_app_lib.h"
#include "csr_exceptionhandler.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrBtHfgSetDialog(hfgInstance_t * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtHfgSetInputDialog(hfgInstance_t * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2);

void CsrBtHfgShowUi(hfgInstance_t * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority);

void CsrBtHfgMenuRemoveAllItems(hfgInstance_t * inst, CsrUint8 UiIndex);

void CsrBtHfgMenuRemoveItem(hfgInstance_t * inst, CsrUint8 UiIndex, CsrUint16 key);


#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_HFG_APP_UTIL_H__ */
