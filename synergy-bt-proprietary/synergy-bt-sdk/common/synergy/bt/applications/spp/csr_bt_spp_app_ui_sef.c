/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
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
#include "csr_bt_spp_lib.h"
#include "csr_bt_spp_app.h"
#include "csr_bt_spp_strings.h"


/* Helper functions */
void commonSppPopupSet(CsrBtSppAppGlobalInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, CONVERT_TEXT_STRING_2_UCS2(dialogHeading), CONVERT_TEXT_STRING_2_UCS2(dialogText),
        NULL, NULL, 0,0,0,0);
}

void commonSppPopupShow(CsrBtSppAppGlobalInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_BLOCK, CSR_BT_SPP_APP_CSR_UI_PRI);
}

void commonSppPopupHide(CsrBtSppAppGlobalInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}

void CsrBtSppAppHandleSppCsrUiActivateDeactivate(CsrBtSppAppGlobalInstData *inst, CsrBool activated)
{
    CsrBtSppAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(activated)
    {
        /* Update main menu so it is no longer possible to activate but just to deactivate */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_SPP_APP_DEACTIVATE_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_DEACTIVATE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, CSR_BT_SPP_APP_ACTIVATE_KEY);
    }
    else
    {
        /* Update main menu so it is no longer possible to deactivate but just to activate */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_SPP_APP_ACTIVATE_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_ACTIVATE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, CSR_BT_SPP_APP_DEACTIVATE_KEY);
    }
    /* Remove popup */
    commonSppPopupHide(inst);
}

void CsrBtSppAppHandleSppCsrUiConnectDisconnect(CsrBtSppAppGlobalInstData *inst, CsrBool connected, CsrBool success)
{
    CsrBtSppAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(connected)
    {
        if(success)
        {
            /* Update main menu so it is no longer possible to connect and activate but just to disconnect and CsrBtSppAppSend file */
            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_FIRST, CSR_BT_SPP_APP_DISCONNECT_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_DISCONNECT_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

            /* Add option to CsrBtSppAppSend file */
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_SPP_APP_START_FILE_TRANSFER_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_START_FILE_TRANSFER_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
        }
    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just to connect and activate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_FIRST, CSR_BT_SPP_APP_CONNECT_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_CONNECT_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_SPP_APP_ACTIVATE_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_ACTIVATE_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;

        /* Remove popup */
        commonSppPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}

void CsrBtSppAppHandleSppCsrUiServiceNameList(CsrBtSppAppGlobalInstData *inst, CsrUint16 listEntries, CsrBtSppServiceName *serviceNameList)
{
    CsrBtSppAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUint16 i;

    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hSelectComInstanceMenu);

    for(i=0;i<listEntries;i++)
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hSelectComInstanceMenu, CSR_UI_LAST, i, CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(serviceNameList[i].serviceName), NULL,
            csrUiVar->hSk1Event, 0, csrUiVar->hBackEvent, 0);
    }

    CsrUiUieShowReqSend(csrUiVar->hSelectComInstanceMenu, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_AUTO, CSR_BT_SPP_APP_CSR_UI_PRI);
}

void CsrBtSppAppHandleSppCsrUiSendFile(CsrBtSppAppGlobalInstData *inst, CsrBool CsrBtSppAppStarted)
{
    CsrBtSppAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(CsrBtSppAppStarted)
    {
        /* Add option to cancel file transfer and remove CsrBtSppAppSend file */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_SPP_APP_STOP_FILE_TRANSFER_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_STOP_FILE_TRANSFER_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, CSR_BT_SPP_APP_START_FILE_TRANSFER_KEY);
    }
    else
    {
        /* Add option to CsrBtSppAppStart file transfer and remove cancel file transfer */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, CSR_UI_LAST, CSR_BT_SPP_APP_START_FILE_TRANSFER_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_START_FILE_TRANSFER_UCS2, NULL,
            csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, CSR_BT_SPP_APP_STOP_FILE_TRANSFER_KEY);
    }
}

void CsrBtSppAppHandleSppSetMainMenuHeader(CsrBtSppAppGlobalInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, CONVERT_TEXT_STRING_2_UCS2(inst->profileName), TEXT_SELECT_UCS2, NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtSppAppGlobalInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtSppAppUiInstData *csrUiVar = &inst->csrUiVar;

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
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSelectFileInputSk1Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hSelectFileInputSk1Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        if (!csrUiVar->hSelectFileInputDialog)
        {
            CsrUiInputdialogSetReqSend(confirm->handle, TEXT_SPP_APP_FILE_TRANSFER_UCS2, TEXT_SPP_APP_INPUT_FILE_NAME_UCS2, CSR_UI_ICON_NONE,
                NULL, MAX_FILE_NAME_LEN, CSR_UI_KEYMAP_ALPHANUMERIC, TEXT_OK_UCS2, NULL, csrUiVar->hSelectFileInputSk1Event,0,csrUiVar->hBackEvent,0);
            csrUiVar->hSelectFileInputDialog = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
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
        if (!csrUiVar->hSelectComInstanceMenu)
        {
            CsrUiMenuSetReqSend(confirm->handle, TEXT_SPP_APP_SELECT_COM_UCS2, TEXT_SELECT_UCS2, NULL);
            csrUiVar->hSelectComInstanceMenu = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
        else if (!csrUiVar->hMainMenu)
        {
            CsrUiMenuSetReqSend(confirm->handle, CONVERT_TEXT_STRING_2_UCS2(inst->profileName), TEXT_SELECT_UCS2, NULL);
            CsrUiMenuAdditemReqSend(confirm->handle, CSR_UI_LAST, CSR_BT_SPP_APP_CONNECT_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_CONNECT_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);
            CsrUiMenuAdditemReqSend(confirm->handle, CSR_UI_LAST, CSR_BT_SPP_APP_ACTIVATE_KEY, CSR_UI_ICON_NONE, TEXT_SPP_APP_ACTIVATE_UCS2, NULL,
                csrUiVar->hSk1Event, csrUiVar->hSk2Event, csrUiVar->hBackEvent, 0);

            csrUiVar->hMainMenu = confirm->handle;
        }
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtSppAppGlobalInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrBtSppAppUiInstData *csrUiVar = &inst->csrUiVar;
    char *fileName = (char *) CsrUtf16String2Utf8(confirm->text);

    CsrStrNCpyZero(inst->sppInstance.fileName, fileName, MAX_FILE_NAME_LEN);

    CsrPmemFree (fileName);

    CsrBtSppAppOpenTheSelectedFile(inst);

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(csrUiVar->hSelectFileInputDialog);

    CsrPmemFree(confirm->text);

}

static void handleCsrUiUieEventInd(CsrBtSppAppGlobalInstData *inst, CsrUiEventInd *indication)
{
    CsrBtSppAppUiInstData *csrUiVar = &inst->csrUiVar;;

    if (indication->event == csrUiVar->hBackEvent)
    {
        if(csrUiVar->hSelectComInstanceMenu == indication->displayElement)
        { /* Select COM port menu handling */
            CsrBtSppServiceNameResSend(inst->queueId, FALSE, 0);
            inst->serviceNameListSize = 0;
            CsrPmemFree(inst->serviceNameList);
            inst->serviceNameList = NULL;
        }
        CsrUiUieHideReqSend(indication->displayElement);
    }
    else if(indication->event == csrUiVar->hSelectFileInputSk1Event)
    {
        CsrUiInputdialogGetReqSend(CsrSchedTaskQueueGet(), csrUiVar->hSelectFileInputDialog);
    }
    else if (indication->event == csrUiVar->hSk1Event)
    {
        if(csrUiVar->hSelectComInstanceMenu == indication->displayElement)
        { /* Select COM port menu handling */
            CsrBtSppServiceNameResSend(inst->queueId, TRUE, inst->serviceNameList[indication->key].serviceHandle);
            inst->serviceNameListSize = 0;
            CsrPmemFree(inst->serviceNameList);
            inst->serviceNameList = NULL;
            CsrUiUieHideReqSend(csrUiVar->hSelectComInstanceMenu);
        }
        else
        { /* Main Menu handling */
            switch(indication->key)
            {
                case CSR_BT_SPP_APP_ACTIVATE_KEY:
                {
                    commonSppPopupSet(inst, "Activating", "Please wait");
                    commonSppPopupShow(inst);
                    CsrBtSppAppStartActivate(inst);
                    break;
                }
                case CSR_BT_SPP_APP_DEACTIVATE_KEY:
                {
                    commonSppPopupSet(inst, "Deactivating", "Please wait");
                    commonSppPopupShow(inst);
                    CsrBtSppAppStartDeactivate(inst);
                    break;
                }
                case CSR_BT_SPP_APP_CONNECT_KEY:
                {
                    commonSppPopupSet(inst, "Connecting", "Please wait");
                    commonSppPopupShow(inst);
                    CsrBtSppAppStartConnecting(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
                case CSR_BT_SPP_APP_DISCONNECT_KEY:
                {
                    commonSppPopupSet(inst, "Disconnecting", "Please wait");
                    commonSppPopupShow(inst);
                    CsrBtSppAppStartDisconnect(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
                case CSR_BT_SPP_APP_START_FILE_TRANSFER_KEY:
                {
                    CsrUiUieShowReqSend(csrUiVar->hSelectFileInputDialog, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_AUTO, CSR_BT_SPP_APP_CSR_UI_PRI);
                    break;
                }
                case CSR_BT_SPP_APP_STOP_FILE_TRANSFER_KEY:
                {
                    CsrBtSppAppCancelFileTransfer(inst);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
    else if (indication->event == csrUiVar->hSk2Event)
    {
        CsrUiUieHideReqSend(indication->displayElement);
    }
}


void CsrBtSppAppHandleSppCsrUiPrim(CsrBtSppAppGlobalInstData *inst)
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

