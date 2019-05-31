/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_app_prim.h"
#include "csr_bt_hfg_app_util.h"
#include "csr_bt_hfg_app_ui_sef.h"
#include "csr_bt_util.h"
#include "csr_pmem.h"



void CsrBtHfgSetDialog(hfgInstance_t * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a Dialog Menu on the UI                                            */
    CsrUieHandle dialog, sk1Handle, sk2Handle, backHandle, delHandle;

    dialog      = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiDialogSetReqSend(dialog, heading, message, textSK1, textSK2,
                            sk1Handle, sk2Handle, backHandle, delHandle);
}

void CsrBtHfgSetInputDialog(hfgInstance_t * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a input Dialog Menu on the UI                                      */
    CsrUieHandle dialog, sk1Handle, sk2Handle, backHandle, delHandle;

    dialog      = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiInputdialogSetReqSend(dialog, heading, message, icon, text, textLength, keyMap,
                            textSK1, textSK2, sk1Handle, sk2Handle, backHandle, delHandle);
}



void CsrBtHfgShowUi(hfgInstance_t * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority)
{ /* Show a given display/window from the UI                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiUieShowReqSend(displayHandle, CsrSchedTaskQueueGet(), inputMode, priority);
}


void CsrBtHfgMenuRemoveAllItems(hfgInstance_t * inst, CsrUint8 UiIndex)
{ /* Remove move all item from a menu display                               */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiMenuRemoveallitemsReqSend(displayHandle);
}

void CsrBtHfgMenuRemoveItem(hfgInstance_t * inst, CsrUint8 UiIndex, CsrUint16 key)
{ /* Remove move all item from a menu display                               */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiMenuRemoveitemReqSend(displayHandle, key);
}
