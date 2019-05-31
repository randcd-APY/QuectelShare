/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#ifndef EXCLUDE_CSR_BT_PROX_SRV_MODULE

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_prox_srv_prim.h"
#include "csr_bt_prox_srv_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_prox_server_app.h"
#include "csr_bt_gatt_demo_prox_server_app_strings.h"

/* Helper functions */

CsrUtf8String *commonProxsGetValueString( CsrUint16 value)
{
        switch(value)
        {
            case CSR_BT_PROX_SRV_ALERT_LEVEL_OFF:
                {
                    return (CsrUtf8String *)CsrStrDup("Alert level changed to: Off");            
                }
            case CSR_BT_PROX_SRV_ALERT_LEVEL_LOW:
                {
                    return (CsrUtf8String *)CsrStrDup("Akert level changed to: Low");
                }
            case CSR_BT_PROX_SRV_ALERT_LEVEL_HIGH:
                {
                    return (CsrUtf8String *)CsrStrDup("Alert levet changed to: High");
                }
            default:
                {
                    return (CsrUtf8String *)CsrStrDup("Unknown");
                }
        }
}
void commonProxsPopupSet(CsrBtProxsAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonProxsAcceptPopupSet(CsrBtProxsAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_PROXS_APP_OK_UCS2, 
                          NULL, 
                          inst->csrUiVar.hBackEvent,
                          0,
                          inst->csrUiVar.hBackEvent,
                          0);
}

void commonProxsPopupShow(CsrBtProxsAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_PROXS_APP_CSR_UI_PRI);
}

void commonProxsAcceptPopupShow(CsrBtProxsAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_PROXS_APP_CSR_UI_PRI);
}

void commonProxsPopupHide(CsrBtProxsAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}



void CsrBtGattAppHandleProxsCsrUiMenuUpdate(CsrBtProxsAppInstData *inst)
{
       /*Could be used to show more info in the menu */
}
void CsrBtGattAppHandleProxsCsrUiValueUpdate(CsrBtProxsAppInstData *inst, 
                                                   CsrBtProxSrvAlertType type, 
                                                   CsrBtProxSrvAlertLevel level)
{

    switch(type) /* -2 because we get the value handle, not the service handle*/
    {
        case (CSR_BT_PROX_SRV_ALERT_LINKLOSS):
            {
                commonProxsAcceptPopupSet(inst, "Link Loss Alert", (char*)commonProxsGetValueString(level));
                commonProxsAcceptPopupShow(inst);
                break;
            }
        case (CSR_BT_PROX_SRV_ALERT_IMMEDIATE):
            {
                commonProxsAcceptPopupSet(inst, "Immediate Alert", (char*)commonProxsGetValueString(level));
                commonProxsAcceptPopupShow(inst);
                break;
            }
        default:
            {
                CsrUint32 retSize = 16;
                CsrUtf8String *ret = CsrPmemAlloc(retSize);
                snprintf((char*)ret, retSize, "handle: 0x%04X", type);
                commonProxsAcceptPopupSet(inst, "Error", (char*)ret);
                commonProxsAcceptPopupShow(inst);
            }
    }

    
}


void CsrBtGattAppHandleProxsCsrUiActivateDeactivate(CsrBtProxsAppInstData *inst, 
                                                   CsrBool activated, 
                                                   CsrBool success)
{
    CsrBtProxsAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(activated)
    {
        /* Update main menu so it is no longer possible to connect and activate but just to deactivate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);
        if(success)
        {
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_FIRST, 
                                    CSR_BT_PROXS_APP_DEACTIVATE_KEY, 
                                    CSR_UI_ICON_NONE, 
                                    TEXT_PROXS_APP_DEACTIVATE_UCS2, 
                                    TEXT_PROXS_APP_CONNECTED_UCS2,
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            /*FIXME
            commonProxsPopupSet(inst, "Advertising", "Please wait");
            commonProxsPopupShow(inst);*/
        }
        else
        {
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                 CSR_UI_FIRST,
                                 CSR_BT_PROXS_APP_DEACTIVATE_KEY,
                                 CSR_UI_ICON_NONE,
                                 TEXT_PROXS_APP_DEACTIVATE_UCS2,
                                 TEXT_PROXS_APP_ACTIVATING_UCS2,
                                 csrUiVar->hSk1Event,
                                 csrUiVar->hSk2Event,
                                 csrUiVar->hBackEvent,
                                 0);

        }
    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just to connect and activate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_PROXS_APP_ACTIVATE_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_PROXS_APP_ACTIVATE_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}



void CsrBtGattAppHandleProxsSetMainMenuHeader(CsrBtProxsAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("Proximity Srv"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtProxsAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtProxsAppUiInstData *csrUiVar = &inst->csrUiVar;

    if (confirm->elementType == CSR_UI_UIETYPE_EVENT)
    {
        if (!csrUiVar->hBackEvent)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hBackEvent = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSk1Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hSk1Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSk2Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            csrUiVar->hSk2Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        /* not actually needed for this demo */
    }
    if (confirm->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        if (!csrUiVar->hCommonDialog)
        {
            csrUiVar->hCommonDialog = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_MENU)
    {
        CsrUiMenuSetReqSend(confirm->handle,
                            CONVERT_TEXT_STRING_2_UCS2(inst->profileName),
                            TEXT_SELECT_UCS2, 
                            NULL);
        CsrUiMenuAdditemReqSend(confirm->handle, 
                                CSR_UI_LAST, 
                                CSR_BT_PROXS_APP_ACTIVATE_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_PROXS_APP_ACTIVATE_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);

        csrUiVar->hMainMenu = confirm->handle;
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtProxsAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrBtProxsAppUiInstData *csrUiVar = &inst->csrUiVar;

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(csrUiVar->hSelectFileInputDialog);

    CsrPmemFree(confirm->text);

}

static void handleCsrUiUieEventInd(CsrBtProxsAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtProxsAppUiInstData *csrUiVar = &inst->csrUiVar;;

    if (indication->event == csrUiVar->hBackEvent)
    {
       /*hide current shown displayelement*/
        CsrUiUieHideReqSend(indication->displayElement);
    }
    else if(indication->event == csrUiVar->hSelectFileInputSk1Event)
    {
        CsrUiInputdialogGetReqSend(CsrSchedTaskQueueGet(), csrUiVar->hSelectFileInputDialog);
    }
    else if (indication->event == csrUiVar->hSk1Event)
    {
        /* Main Menu handling */
        switch(indication->key)
        {
            case CSR_BT_PROXS_APP_ACTIVATE_KEY:
                {
                    CsrBtProxsAppStartActivate(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
            case CSR_BT_PROXS_APP_DEACTIVATE_KEY:
                {
                    CsrBtProxsAppStartDeactivate(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
            case CSR_BT_PROXS_APP_OK_KEY:
                {
                    commonProxsPopupHide(inst);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    else if (indication->event == csrUiVar->hSk2Event)
    {
        CsrUiUieHideReqSend(indication->displayElement);
    }
}


void CsrBtGattAppHandleProxsCsrUiPrim(CsrBtProxsAppInstData *inst)
{
    switch(*((CsrUint16 *) inst->recvMsgP))
    {
        case CSR_UI_UIE_CREATE_CFM:
            {
                handleCsrUiUieCreateCfm(inst, inst->recvMsgP);
                break;
            }

        case CSR_UI_INPUTDIALOG_GET_CFM:
            {
                handleCsrUiInputDialogGetCfm(inst, inst->recvMsgP);
                break;
            }

        case CSR_UI_KEYDOWN_IND:
            {
                break;
            }

        case CSR_UI_EVENT_IND:
            {
                handleCsrUiUieEventInd(inst, inst->recvMsgP);
                break;
            }
        default:
            {
                break;
            }
    }
}

#endif
