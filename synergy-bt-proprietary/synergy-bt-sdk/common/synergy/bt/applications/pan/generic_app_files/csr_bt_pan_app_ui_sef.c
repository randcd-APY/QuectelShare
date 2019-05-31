/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_bsl_lib.h"
#include "csr_bt_pan_app.h"
#include "csr_bt_pan_strings.h"


/* Helper functions */
void commonPanPopupSet(CsrBtPanAppGlobalInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialogForwaiting, CONVERT_TEXT_STRING_2_UCS2(dialogHeading), CONVERT_TEXT_STRING_2_UCS2(dialogText),
        NULL, NULL, 0,0,0,0);
}

void commonPanPopupShow(CsrBtPanAppGlobalInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialogForwaiting, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_AUTO, 
    CSR_BT_PAN_APP_CSR_UI_PRI);
}

void commonPanPopupHide(CsrBtPanAppGlobalInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialogForwaiting);
}
void CsrBtPanSetDialog(CsrBtPanAppGlobalInstData * inst,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a Dialog Menu on the UI                                            */
    
    CsrUieHandle sk1Handle, sk2Handle, backHandle;
    CsrBtPanAppUiInstData *csrUiVar = &inst->csrUiVar;

    sk1Handle   = csrUiVar->hSelectcommonDialogSk1Event;
    sk2Handle   = csrUiVar->hSk2Event;
    backHandle  = csrUiVar->hSelectcommonDialogBackEvent;
    
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, heading, message, textSK1, textSK2,
                            sk1Handle, sk2Handle, backHandle, 0);
}

void CsrBtPanShowUi(CsrBtPanAppGlobalInstData * inst,
                                CsrUint16 inputMode, CsrUint16 priority)
{ /* Show a given display/window from the UI                                */
     CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, CsrSchedTaskQueueGet(), inputMode, priority);
}

void CsrBtPanAppHandlePanCsrUiActivateDeactivate(CsrBtPanAppGlobalInstData *inst,CsrBool connected)
{
    CsrBtPanAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(!connected)
    {
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_FIRST, CSR_BT_PAN_APP_ROLE_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_ROLE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_CONNECT_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_CONNECT_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        
      
    }
     else
    {
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_FIRST, CSR_BT_PAN_APP_ROLE_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_ROLE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_DISCONNECT_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_DISCONNECT_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        
        
    }
     /* Remove popup */
        commonPanPopupHide(inst);
    
}
void CsrBtPanAppHandlePanCsrUiRoleSelected(CsrBtPanAppGlobalInstData *inst, CsrBool selected)
{
    CsrBtPanAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hSelectRoleInstanceMenu);

    CsrUiMenuAdditemReqSend(csrUiVar->hSelectRoleInstanceMenu, CSR_UI_LAST, CSR_BT_PAN_APP_ROLE_NAP_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_NAP_ROLE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event1, csrUiVar->hBackEvent1, 0);

    CsrUiMenuAdditemReqSend(csrUiVar->hSelectRoleInstanceMenu, CSR_UI_LAST, CSR_BT_PAN_APP_ROLE_PANU_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_PANU_ROLE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event1, csrUiVar->hBackEvent1, 0);

    CsrUiUieShowReqSend(csrUiVar->hSelectRoleInstanceMenu, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_AUTO, CSR_BT_PAN_APP_CSR_UI_PRI); 
}

void CsrBtPanAppHandlePanCsrUiConnectSelected(CsrBtPanAppGlobalInstData *inst)
{
    CsrBtPanAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);
    CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_REMOTE_ROLE_PANU_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_CONNECT_TO_PANU_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2EventforConnectOptions, csrUiVar->hBackEventforConnectOptions, 0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_REMOTE_ROLE_NAP_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_CONNECT_TO_NAP_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2EventforConnectOptions, csrUiVar->hBackEventforConnectOptions, 0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_REMOTE_ROLE_GN_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_CONNECT_TO_GN_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2EventforConnectOptions, csrUiVar->hBackEventforConnectOptions, 0);
}

void CsrBtPanAppHandlePanCsrUiConnectDisconnect(CsrBtPanAppGlobalInstData *inst, CsrBool connected, CsrBool success)
{
    CsrBtPanAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(connected)
    {
        if(success)
        {
            /* Update main menu so it is no longer possible to connect and activate but just to disconnect and 
            CsrBtPanAppSend file */
            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, 
            CSR_BT_PAN_APP_DISCONNECT_KEY, CSR_UI_ICON_NONE, TEXT_PAN_APP_DISCONNECT_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_FIRST, 
            CSR_BT_PAN_APP_ROLE_KEY, CSR_UI_ICON_NONE, TEXT_PAN_APP_ROLE_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        }
    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just to connect and activate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

       CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_CONNECT_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_CONNECT_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_FIRST, CSR_BT_PAN_APP_ROLE_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_ROLE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;
        /* Remove popup */
        commonPanPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}

void CsrBtPanAppHandlePanSetMainMenuHeader(CsrBtPanAppGlobalInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, CONVERT_TEXT_STRING_2_UCS2(PROFILE_PAN), TEXT_SELECT_UCS2, NULL);
    }
    
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtPanAppGlobalInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtPanAppUiInstData *csrUiVar = &inst->csrUiVar;

    if (confirm->elementType == CSR_UI_UIETYPE_EVENT)
    {
        if (!csrUiVar->hBackEvent)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hBackEvent = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hBackEvent1)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hBackEvent1 = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hBackEventforConnectOptions)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hBackEventforConnectOptions = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSelectcommonDialogBackEvent)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hSelectcommonDialogBackEvent = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSelectcommonDialogSk1Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hSelectcommonDialogSk1Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSk1Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hSk1Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSk2EventforConnectOptions)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            csrUiVar->hSk2EventforConnectOptions = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSk2Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            csrUiVar->hSk2Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
    }
    
    if (confirm->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        if (!csrUiVar->hCommonDialog)
        {
            csrUiVar->hCommonDialog = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
        else if (!csrUiVar->hCommonDialogForwaiting)
        {
            csrUiVar->hCommonDialogForwaiting = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_MENU)
    {
        if (!csrUiVar->hSelectRoleInstanceMenu)
        {
            CsrUiMenuSetReqSend(confirm->handle, TEXT_PAN_APP_PANU_ROLE_UCS2, TEXT_SELECT_UCS2, NULL);
            csrUiVar->hSelectRoleInstanceMenu = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
        if (!csrUiVar->hMainMenu)
        {
            CsrUiMenuSetReqSend(confirm->handle, CONVERT_TEXT_STRING_2_UCS2(PROFILE_PAN), TEXT_SELECT_UCS2, NULL);
            CsrUiMenuAdditemReqSend(confirm->handle, CSR_UI_LAST, CSR_BT_PAN_APP_ROLE_KEY, CSR_UI_ICON_NONE, 
            TEXT_PAN_APP_ROLE_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
            
            CsrUiMenuAdditemReqSend(confirm->handle, CSR_UI_LAST, CSR_BT_PAN_APP_CONNECT_KEY, CSR_UI_ICON_NONE, 
            TEXT_PAN_APP_CONNECT_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
            csrUiVar->hMainMenu = confirm->handle;
        }
     }
}


static void handleCsrUiUieEventInd(CsrBtPanAppGlobalInstData *inst, CsrUiEventInd *indication)
{
    CsrBtPanAppUiInstData *csrUiVar = &inst->csrUiVar;;

    if (indication->event == csrUiVar->hBackEvent1 ||
        indication->event == csrUiVar->hSk2Event1)
    {
         CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);
        
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_ROLE_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_ROLE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        if(!inst->connected)
        {
           CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_CONNECT_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_CONNECT_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        }
        else
        {
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_DISCONNECT_KEY, CSR_UI_ICON_NONE, 
            TEXT_PAN_APP_DISCONNECT_UCS2, NULL, csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        
        }
        
    }
    if (indication->event == csrUiVar->hBackEventforConnectOptions ||
        indication->event == csrUiVar->hSk2EventforConnectOptions)
    {
         CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_PAN_APP_CONNECT_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_CONNECT_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_FIRST, CSR_BT_PAN_APP_ROLE_KEY, CSR_UI_ICON_NONE, 
        TEXT_PAN_APP_ROLE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
    }
    else if(indication->event == csrUiVar->hBackEvent)
    {
         CsrUiUieHideReqSend(indication->displayElement);
         
    }
    else if(indication->event==csrUiVar->hSelectcommonDialogBackEvent)
    {   
        CsrUiUieHideReqSend(indication->displayElement);
        
    }
    else if(indication->event==csrUiVar->hSelectcommonDialogSk1Event)
    {   
        CsrUiUieHideReqSend(indication->displayElement);
        
    }
    else if (indication->event == csrUiVar->hSk1Event)
    {

            /* Main Menu handling */
            switch(indication->key)
            {
                case CSR_BT_PAN_APP_ACTIVATE_KEY:
                {
                    commonPanPopupSet(inst, "Activating panu role for incoming connections", "Please wait");
                    commonPanPopupShow(inst);
                    CsrBtPanAppStartActivate(inst);
                    break;
                }
                case CSR_BT_PAN_APP_ROLE_KEY:
                {
                    CsrBtPanAppStartSelect(inst);
                    break;
                }

                case CSR_BT_PAN_APP_ROLE_NAP_KEY:
                {
                    inst->localRole = CSR_BT_PAN_NAP_ROLE;
                    CsrBtPanAppStartActivate(inst);
                    break;
                }

                case CSR_BT_PAN_APP_ROLE_PANU_KEY:
                {
                    inst->localRole = CSR_BT_PAN_PANU_ROLE;
                    CsrBtPanAppStartActivate(inst);
                    break;
                }

                case CSR_BT_PAN_APP_CONNECT_KEY:
                {
                    if((inst->selectedDeviceAddr.nap == 0) &&
                        (inst->selectedDeviceAddr.uap == 0) &&
                         (inst->selectedDeviceAddr.lap == 0))
                    {
                        CsrBtPanSetDialog(inst,  TEXT_FAILED_UCS2, TEXT_PAN_APP_TARGET_DEVICE_UCS2, TEXT_OK_UCS2, NULL);
                        CsrBtPanShowUi(inst,  CSR_UI_INPUTMODE_AUTO, CSR_BT_PAN_APP_CSR_UI_PRI);
                    }
                    else
                    {
                       CsrBtPanAppHandlePanCsrUiConnectSelected(inst);
                    }
                    break;
                }
                case CSR_BT_PAN_APP_DISCONNECT_KEY:
                {
                    commonPanPopupSet(inst, "Disconnecting", "Please wait");
                    commonPanPopupShow(inst);
                    CsrBtPanAppStartDisconnect(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
                case CSR_BT_PAN_APP_REMOTE_ROLE_PANU_KEY:
                {
                    CsrBtPanAppStartConnecting(inst,CSR_BT_PAN_PANU_ROLE);
                    break;
                }
                case CSR_BT_PAN_APP_REMOTE_ROLE_NAP_KEY:
                {
                    CsrBtPanAppStartConnecting(inst,CSR_BT_PAN_NAP_ROLE);
                    break;
                }
                case CSR_BT_PAN_APP_REMOTE_ROLE_GN_KEY:
                {
                    CsrBtPanAppStartConnecting(inst,CSR_BT_PAN_GN_ROLE);
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


void CsrBtPanAppHandlePanCsrUiPrim(CsrBtPanAppGlobalInstData *inst)
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

