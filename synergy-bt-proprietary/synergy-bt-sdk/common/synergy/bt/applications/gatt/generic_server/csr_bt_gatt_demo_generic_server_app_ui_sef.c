/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
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
#include "csr_bt_gatt_demo_generic_srv_prim.h"
#include "csr_bt_gatt_demo_generic_srv_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_generic_server_app.h"
#include "csr_bt_gatt_demo_generic_server_app_strings.h"
#include "csr_bt_gatt_demo_generic_server_app_ui_sef.h"
#include "csr_exceptionhandler.h"

void CsrBtGattAppHandleGenericSrvCsrUiActivateDeactivate(CsrBtGenericSrvAppInstData *inst, 
                                                   CsrBool activated, 
                                                   CsrBool success);
static void appCreateGenericSrvMainMenuUi(CsrBtGenericSrvAppInstData *inst);
static void appCreateGenericSrvWhiteListUi(CsrBtGenericSrvAppInstData *inst);
/* Helper functions */

CsrUtf8String *commonGenericSrvGetValueString( CsrUint16 value)
{
        switch(value)
        {
            case CSR_BT_GATT_APP_ALERT_LEVEL_OFF:
                {
                    return (CsrUtf8String *)CsrStrDup("Alert level changed to: Off");            
                }
            case CSR_BT_GATT_APP_ALERT_LEVEL_LOW:
                {
                    return (CsrUtf8String *)CsrStrDup("Alert level changed to: Low");
                }
            case CSR_BT_GATT_APP_ALERT_LEVEL_HIGH:
                {
                    return (CsrUtf8String *)CsrStrDup("Alert levet changed to: High");
                }
            default:
                {
                    return (CsrUtf8String *)CsrStrDup("Unknown");
                }
        }
}
void commonGenericSrvPopupSet(CsrBtGenericSrvAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonGenericSrvStopPopupSet(CsrBtGenericSrvAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_STOP_UCS2, 
                          NULL, 
                          inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk1EventHandle,
                          0,
                          0,
                          0);
}


void commonGenericSrvAcceptPopupSet(CsrBtGenericSrvAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_OK_UCS2, 
                          NULL, 
                          inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle,
                          0,
                          inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle,
                          0);
}

void commonGenericSrvPopupShow(CsrBtGenericSrvAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);
}

void commonGenericSrvAcceptPopupShow(CsrBtGenericSrvAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);
}

void commonGenericSrvPopupHide(CsrBtGenericSrvAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}

void CsrBtGattAppAcceptPopup(CsrBtGenericSrvAppInstData *inst, char *dialogHeading, char *dialogText)
{
    commonGenericSrvAcceptPopupSet(inst, dialogHeading, dialogText);
    commonGenericSrvAcceptPopupShow(inst);
}


void CsrBtGattAppHandleGenericSrvCsrUiMenuUpdate(CsrBtGenericSrvAppInstData *inst, CsrBool indication, CsrBool notify)
{
    CsrBtGenericSrvAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrBtGattAppHandleGenericSrvCsrUiActivateDeactivate(inst, TRUE, TRUE);

    if(indication == TRUE)
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                CSR_UI_LAST,
                                CSR_BT_GENERIC_SRV_APP_INDICATE_KEY,
                                CSR_UI_ICON_BELL,
                                TEXT_GENERIC_SRV_APP_INDICATE_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2("  "),
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk1EventHandle,
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk2EventHandle,
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle,
                                0);
    }

    if(notify == TRUE)
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                CSR_UI_LAST,
                                CSR_BT_GENERIC_SRV_APP_NOTIFY_KEY,
                                CSR_UI_ICON_BELL,
                                TEXT_GENERIC_SRV_APP_NOTIFY_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2("  "),
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk1EventHandle,
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk2EventHandle,
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle,
                                0);
    }

    commonGenericSrvPopupHide(inst);
                        
}
void CsrBtGattAppHandleGenericSrvCsrUiValueUpdate(CsrBtGenericSrvAppInstData *inst, 
                                                   CsrUint16 handle, 
                                                   CsrUint16 value)
{

    switch(handle - inst->dbStartHandle)
    {
        case CSR_BT_GENERIC_SRV_TEMP_MEASUREMENT_CC_HANDLE:
            {
                commonGenericSrvAcceptPopupSet(inst, "Temperature measurement", (char*)commonGenericSrvGetValueString(value));
                commonGenericSrvAcceptPopupShow(inst);
                break;
            }
        default:
            {
                CsrUint32 retSize = 16;
                CsrUtf8String *ret = CsrPmemAlloc(retSize);
                snprintf((char*)ret, retSize, "handle: 0x%04X", handle-inst->dbStartHandle);
                commonGenericSrvAcceptPopupSet(inst, "Error", (char*)ret);
                commonGenericSrvAcceptPopupShow(inst);
            }
    }

    
}


void CsrBtGattAppHandleGenericSrvCsrUiActivateDeactivate(CsrBtGenericSrvAppInstData *inst, 
                                                   CsrBool activated, 
                                                   CsrBool success)
{
    CsrBtGenericSrvAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(activated)
    {
        if(success)
        {
            CsrUint32 addrSize = 15;
            /* Update main menu so it is no longer possible to connect and activate but just to deactivate */
            char *addr = CsrPmemAlloc(addrSize);
            snprintf(addr, addrSize, "%04X:%02X:%06X", inst->connectedDeviceAddr.addr.nap, inst->connectedDeviceAddr.addr.uap, inst->connectedDeviceAddr.addr.lap);

            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_FIRST, 
                                    CSR_BT_GENERIC_SRV_APP_DEACTIVATE_KEY, 
                                    CSR_UI_ICON_DISCONNECT, 
                                    TEXT_GENERIC_SRV_APP_DEACTIVATE_UCS2, 
                                    CsrUtf82Utf16String((CsrUtf8String *) addr),
                                    inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk1EventHandle, 
                                    inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk2EventHandle, 
                                    inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle, 
                                    0);
            commonGenericSrvStopPopupSet(inst, "Advertising", "Please wait");
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_GENERIC_SRV_APP_WL_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_GENERIC_SRV_APP_WL_UCS2,
                                    NULL,
                                    csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].sk1EventHandle,
                                    CSR_UI_DEFAULTACTION,
                                    inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle,
                                    0);
            commonGenericSrvAcceptPopupShow(inst);
            CsrPmemFree(addr);
        }
    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just to connect and activate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_GENERIC_SRV_APP_ACTIVATE_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_GENERIC_SRV_APP_ACTIVATE_UCS2, 
                                NULL,
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk1EventHandle, 
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].sk2EventHandle, 
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                CSR_UI_LAST,
                                CSR_BT_GENERIC_SRV_APP_WL_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_GENERIC_SRV_APP_WL_UCS2,
                                NULL,
                                csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].sk1EventHandle,
                                CSR_UI_DEFAULTACTION,
                                inst->csrUiVar.displayesHandlers[inst->csrUiVar.uiIndex].backEventHandle,
                                0);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;

        /* Remove popup */
        commonGenericSrvPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}



void CsrBtGattAppHandleGenericSrvSetMainMenuHeader(CsrBtGenericSrvAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("Generic Server"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtGenericSrvAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtGenericSrvAppUiInstData *csrUiVar = &inst->csrUiVar;

    if (confirm->elementType == CSR_UI_UIETYPE_EVENT)
    {
        if (!csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle = confirm->handle;
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
                                CSR_BT_GENERIC_SRV_APP_ACTIVATE_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_GENERIC_SRV_APP_ACTIVATE_UCS2,
                                NULL,
                                csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle,
                                csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle,
                                csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle, 0);

        CsrUiMenuAdditemReqSend(confirm->handle,
                                CSR_UI_LAST,
                                CSR_BT_GENERIC_SRV_APP_WL_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_GENERIC_SRV_APP_WL_UCS2,
                                NULL,
                                csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle,
                                csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle,
                                csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle, 0);

        csrUiVar->hMainMenu = confirm->handle;
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtGenericSrvAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrBtGenericSrvAppUiInstData *csrUiVar = &inst->csrUiVar;

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(csrUiVar->hSelectFileInputDialog);

    CsrPmemFree(confirm->text);

}

static void handleCsrUiUieEventInd(CsrBtGenericSrvAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtGenericSrvAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUint8     i;

    for ( i = 0; i < NUMBER_OF_CSR_BT_GENERIC_SRV_APP_UI; i++)
    { 
        if (indication->event == csrUiVar->displayesHandlers[i].backEventHandle)
        {
           /*hide current shown displayelement*/
            CsrUiUieHideReqSend(indication->displayElement);

            if(indication->displayElement == csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle)
            {
                csrUiVar->inGenericSrvMenu = TRUE;
                                /*update UI*/
                CsrUiUieShowReqSend(csrUiVar->hMainMenu,
                                    CsrSchedTaskQueueGet(),
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);
            }
            else if(indication->displayElement == csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_MAIN_MENU_UI].displayHandle)
            {
                csrUiVar->inGenericSrvMenu = FALSE;
            }

            if (csrUiVar->inGenericSrvMenu)
            {
                CsrUiDisplayGethandleReqSend(CsrSchedTaskQueueGet());
            }
        }
        else if(indication->event == csrUiVar->hSelectFileInputSk1Event)
        {
            CsrUiInputdialogGetReqSend(CsrSchedTaskQueueGet(), csrUiVar->hSelectFileInputDialog);
        }
        else if (indication->event == csrUiVar->displayesHandlers[i].sk1EventHandle)
        {
            if(indication->displayElement == csrUiVar->hCommonDialog)
            {
                /* Stop peripheral req */
                CsrUiUieHideReqSend(indication->displayElement);
                CsrBtGenericSrvAppStartDisconnect(inst);
                CsrUiUieShowReqSend(csrUiVar->hMainMenu,
                                    CsrSchedTaskQueueGet(),
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);
            }
            else if(indication->displayElement == csrUiVar->hErrorDialog)
            {
                CsrUiUieHideReqSend(indication->displayElement);
                CsrUiUieShowReqSend(csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle,
                                    CsrSchedTaskQueueGet(),
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);
            }
            else
            {
                /* Main Menu handling */
                switch(indication->key)
                {
                    case CSR_BT_GENERIC_SRV_APP_ACTIVATE_KEY:
                        {
                            commonGenericSrvStopPopupSet(inst, "Advertising", "Please wait");
                            commonGenericSrvAcceptPopupShow(inst);
                            CsrBtGenericSrvAppStartActivate(inst);
                            inst->csrUiVar.localInitiatedAction = TRUE;
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_DEACTIVATE_KEY:
                        {
                            commonGenericSrvPopupSet(inst, "Deactivating", "Please wait");
                            commonGenericSrvPopupShow(inst);
                            CsrBtGenericSrvAppStartDeactivate(inst);
                            inst->csrUiVar.localInitiatedAction = TRUE;
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_WL_KEY:
                        {
                            CsrUieHandle displayHandle = csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle;
                            CsrUiUieShowReqSend(displayHandle, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_AUTO, CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_ENABLE_WL_KEY:
                        {
                            CsrUieHandle displayHandle = csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle;
                            CsrUieHandle sk1Handle = csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].sk1EventHandle;
                            CsrUieHandle backHandle = csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].backEventHandle;

                            if (!csrBtWlDbCheckEmpty())
                            {
                                inst->enable_whitelist = TRUE;
                            }
                            else
                            {
                                CsrUiDialogSetReqSend(inst->csrUiVar.hErrorDialog, 
                                                      CONVERT_TEXT_STRING_2_UCS2("Error Enable WhiteList"), 
                                                      CONVERT_TEXT_STRING_2_UCS2("Please pairing first!!"),
                                                      TEXT_OK_UCS2, 
                                                      NULL, 
                                                      sk1Handle,
                                                      0,
                                                      0,
                                                      0);

                                CsrUiUieShowReqSend(inst->csrUiVar.hErrorDialog, 
                                                    CsrSchedTaskQueueGet(), 
                                                    CSR_UI_INPUTMODE_AUTO, 
                                                    CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);

                                break;
                            }
                            CsrUiMenuRemoveallitemsReqSend(displayHandle);

                            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GENERIC_SRV_APP_DISABLE_WL_KEY,
                                                    CSR_UI_ICON_NONE, TEXT_GENERIC_SRV_APP_DISABLE_WL_UCS2, NULL, sk1Handle,
                                                    CSR_UI_DEFAULTACTION, backHandle, CSR_UI_DEFAULTACTION);
                            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GENERIC_SRV_APP_DELETE_FROM_WL_KEY,
                                                    CSR_UI_ICON_NONE, TEXT_GENERIC_SRV_APP_DELETE_FROM_WL_UCS2, NULL, sk1Handle,
                                                    CSR_UI_DEFAULTACTION, backHandle, CSR_UI_DEFAULTACTION);

                            CsrUiUieShowReqSend(csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle,
                                                CsrSchedTaskQueueGet(),
                                                CSR_UI_INPUTMODE_AUTO,
                                                CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);  
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_DISABLE_WL_KEY:
                        {
                            CsrUieHandle displayHandle = csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle;
                            CsrUieHandle sk1Handle = csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].sk1EventHandle;
                            CsrUieHandle backHandle = csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].backEventHandle;

                            inst->enable_whitelist = FALSE;

                            CsrUiMenuRemoveallitemsReqSend(displayHandle);

                            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GENERIC_SRV_APP_ENABLE_WL_KEY,
                                                    CSR_UI_ICON_NONE, TEXT_GENERIC_SRV_APP_ENABLE_WL_UCS2, NULL, sk1Handle,
                                                    CSR_UI_DEFAULTACTION, backHandle, CSR_UI_DEFAULTACTION);
                            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GENERIC_SRV_APP_DELETE_FROM_WL_KEY,
                                                    CSR_UI_ICON_NONE, TEXT_GENERIC_SRV_APP_DELETE_FROM_WL_UCS2, NULL, sk1Handle,
                                                    CSR_UI_DEFAULTACTION, backHandle, CSR_UI_DEFAULTACTION);
                            CsrUiUieShowReqSend(csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle,
                                                CsrSchedTaskQueueGet(),
                                                CSR_UI_INPUTMODE_AUTO,
                                                CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_DELETE_FROM_WL_KEY:
                        {
                            CsrBtWlDbRemove(&inst->connectedDeviceAddr);
                            CsrUiUieShowReqSend(csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].displayHandle,
                                                CsrSchedTaskQueueGet(),
                                                CSR_UI_INPUTMODE_AUTO,
                                                CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI);  
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_NOTIFY_KEY:
                        {
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_INDICATE_KEY:
                        {
                            break;
                        }
                    case CSR_BT_GENERIC_SRV_APP_OK_KEY:
                        {
                            commonGenericSrvPopupHide(inst);
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }
        }
        else if (indication->event == csrUiVar->displayesHandlers[i].sk2EventHandle)
        {
            CsrUiUieHideReqSend(indication->displayElement);
        }
    }
}

static const CsrBtGenericSrvEventType csrUiGenericSrvUieCreateCfmHandlers[NUMBER_OF_CSR_BT_GENERIC_SRV_APP_UI] =
{
    appCreateGenericSrvMainMenuUi,                              /* CSR_BT_GENERIC_SRV_MAIN_MENU_UI */
    appCreateGenericSrvWhiteListUi,                             /* CSR_BT_GENERIC_SRV_WHITELIST_UI */
};

void CsrBtGattAppHandleGenericSrvCsrUiPrim(CsrBtGenericSrvAppInstData *inst)
{
    switch(*((CsrUint16 *) inst->recvMsgP))
    {
        case CSR_UI_UIE_CREATE_CFM:
            {
                if (inst->csrUiVar.uiIndex < NUMBER_OF_CSR_BT_GENERIC_SRV_APP_UI)
                {
                     csrUiGenericSrvUieCreateCfmHandlers[inst->csrUiVar.uiIndex](inst);
                }
                else
                { /* An unexpected number of CSR_UI CsrUiUieCreateCfm messages
                     has been received                                                      */
                }
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

static void appUiKeyHandlerEnableDisableWLMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{
    CSR_UNUSED(instData);
    CSR_UNUSED(displayHandle);
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appUiKeyHandlerHideMics(void * instData,
                                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* An key event function used to hide the showed UI. This function is used
 by many different APP UI's                                              */
   CsrUiUieHideReqSend(displayHandle);
   CSR_UNUSED(eventHandle);
   CSR_UNUSED(instData);
   CSR_UNUSED(key);
}

static void appCreateGenericSrvMainMenuUi(CsrBtGenericSrvAppInstData *inst)
{

    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;
    CsrBtGenericSrvAppUiInstData *csrUiVar = &inst->csrUiVar;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerEnableDisableWLMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GENERIC_SRV_WHITELIST_UI*/
        CsrUiMenuSetReqSend(prim->handle, CONVERT_TEXT_STRING_2_UCS2(inst->profileName), TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event*/
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        if (!csrUiVar->hCommonDialog)
        {
            csrUiVar->eventState    = CSR_BT_GENERICSRV_CREATE_SK1_EVENT;
            csrUiVar->hCommonDialog = prim->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
        else if (!csrUiVar->hErrorDialog)
        {
            csrUiVar->eventState   = CSR_BT_GENERICSRV_CREATE_SK1_EVENT;
            csrUiVar->hErrorDialog = prim->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
    }
    else if(prim->elementType == CSR_UI_UIETYPE_EVENT)
    {
        if (csrUiVar->eventState == CSR_BT_GENERICSRV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked*/

            csrUiVar->eventState                                           = CSR_BT_GENERICSRV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event*/
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        {
         /* CSR_BT_GENERIC_SRV_CREATE_BACK_EVENT*/

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked*/
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle,
                                    CSR_UI_LAST, 
                                    CSR_BT_GENERIC_SRV_APP_ACTIVATE_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_GENERIC_SRV_APP_ACTIVATE_UCS2,
                                    NULL,
                                    sk1Handle,
                                    CSR_UI_DEFAULTACTION,
                                    prim->handle, 
                                    0);

            CsrUiMenuAdditemReqSend(displayHandle,
                                    CSR_UI_LAST, 
                                    CSR_BT_GENERIC_SRV_APP_WL_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_GENERIC_SRV_APP_WL_UCS2,
                                    NULL,
                                    csrUiVar->displayesHandlers[CSR_BT_GENERIC_SRV_WHITELIST_UI].sk1EventHandle,
                                    CSR_UI_DEFAULTACTION,
                                    prim->handle, 
                                    0);

            csrUiVar->hMainMenu = displayHandle;
        }
    }
}

static void appCreateGenericSrvWhiteListUi(CsrBtGenericSrvAppInstData *inst)
{
    CsrBtGenericSrvAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;
    /* handler functions */

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                               = CSR_BT_GENERICSRV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerEnableDisableWLMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GENERIC_SRV_MAIN_MENU_UI*/
        CsrUiMenuSetReqSend(prim->handle, TEXT_GENERIC_SRV_APP_WL_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event*/
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GENERICSRV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked*/

            csrUiVar->eventState                                           = CSR_BT_GENERICSRV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        {
             /* CSR_BT_GENERIC_SRV_MAIN_MENU_UI  */
            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            if (inst->enable_whitelist == FALSE)
            {
                CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GENERIC_SRV_APP_ENABLE_WL_KEY,
                        CSR_UI_ICON_NONE, TEXT_GENERIC_SRV_APP_ENABLE_WL_UCS2, NULL, sk1Handle,
                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            else
            {
                CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GENERIC_SRV_APP_DISABLE_WL_KEY,
                        CSR_UI_ICON_NONE, TEXT_GENERIC_SRV_APP_DISABLE_WL_UCS2, NULL, sk1Handle,
                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GENERIC_SRV_APP_DELETE_FROM_WL_KEY,
                        CSR_UI_ICON_NONE, TEXT_GENERIC_SRV_APP_DELETE_FROM_WL_UCS2, NULL, sk1Handle,
                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_GENERIC_SRV_MAIN_MENU_UI */
            csrUiVar->uiIndex = CSR_BT_GENERIC_SRV_MAIN_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
   }
}
