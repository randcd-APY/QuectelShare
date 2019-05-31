/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_app_prim.h"
#include "csr_bt_avrcp_app_util.h"
#include "csr_bt_avrcp_app_ui_sef.h"
#include "csr_bt_util.h"
#include "csr_pmem.h"

void CsrBtAvrcpAddItem(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 position,
                  CsrUint16 key, CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel)
{ /* Add a Menu item to an UI                                               */
    CsrUieHandle menu, sk1Handle, sk2Handle, backHandle, delHandle;

    menu        = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiMenuAdditemReqSend(menu, position, key, icon, label, sublabel,
                                sk1Handle, sk2Handle, backHandle, delHandle);
}

void CsrBtAvrcpSetItem(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 key,
                                CsrUint16 icon, CsrUint16 *label, CsrUint16 *sublabel)
{ /* Set a Menu item to an UI                                               */
    CsrUieHandle menu, sk1Handle, sk2Handle, backHandle, delHandle;

    menu        = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiMenuSetitemReqSend(menu, key, icon, label, sublabel,
                    sk1Handle, sk2Handle, backHandle, delHandle);
}


void CsrBtAvrcpSetDialog(avrcpinstance_t * inst, CsrUint8 UiIndex,
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

void CsrBtAvrcpSetInputDialog(avrcpinstance_t * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a input Dialog Menu on the UI                                      */
    CsrUieHandle dialog, sk1Handle, sk2Handle, backHandle, delHandle;

    inst->csrUiVar.uiIndex = UiIndex;
    dialog      = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiInputdialogSetReqSend(dialog, heading, message, icon, text, textLength, keyMap,
                            textSK1, textSK2, sk1Handle, sk2Handle, backHandle, delHandle);
}

void CsrBtAvrcpMenuSet(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 *heading,
                  CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set the Menu on the UI                                                 */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiMenuSetReqSend(displayHandle, heading, textSK1, textSK2);
}


void CsrBtAvrcpShowUi(avrcpinstance_t * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority)
{ /* Show a given display/window from the UI                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrUiUieShowReqSend(displayHandle, CsrSchedTaskQueueGet(), inputMode, priority);
#endif
}

void CsrBtAvrcpHideUi(avrcpinstance_t * inst, CsrUint8 UiIndex)
{ /* Hide a given display/window from the UI                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrUiUieHideReqSend(displayHandle);
#endif
}

void CsrBtAvrcpDisplaySetInputMode(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 inputMode)
{ /* Set the display mode on a given display                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrUiDisplaySetinputmodeReqSend(displayHandle, inputMode);
#endif

}

void CsrBtAvrcpMenuRemoveAllItems(avrcpinstance_t * inst, CsrUint8 UiIndex)
{ /* Remove move all item from a menu display                               */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrUiMenuRemoveallitemsReqSend(displayHandle);
#endif
}

void CsrBtAvrcpMenuRemoveItem(avrcpinstance_t * inst, CsrUint8 UiIndex, CsrUint16 key)
{ /* Remove move all item from a menu display                               */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrUiMenuRemoveitemReqSend(displayHandle, key);
#endif
}







