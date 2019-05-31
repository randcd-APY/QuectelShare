/****************************************************************************

Copyright (c) 2011-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
*****************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_le_browser_app.h"
#include "csr_bt_gatt_demo_le_browser_app_strings.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_le_browser_app_defines.h"
#include "csr_bt_gatt_demo_le_browser_app_ui_sef.h"
#include "csr_bt_gatt_demo_db_utils.h"

#include "csr_bt_gatt_demo_le_browser_app_menus.h"
 
/* Helper functions */
void commonLeBrowserPopupSet(CsrBtLeBrowserAppInstData *inst, const char *dialogHeading, const char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonLeBrowserAcceptPopupSet(CsrBtLeBrowserAppInstData *inst, const char *dialogHeading, const char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_LE_BROWSER_APP_OK_UCS2, 
                          NULL, 
                          inst->csrUiVar.hBackEvent,
                          0,
                          inst->csrUiVar.hBackEvent,
                          0);
}

void commonLeBrowserInputPopupSet(CsrBtLeBrowserAppInstData *inst, CsrUint8 *dialogMessage, CsrUint8 keyMap)
{
    CsrUint8* dialogInputText = NULL;
    CsrUint16 uuid;
    CsrSize size;
    
    uuid = inst->dbElement->currentDescr->uuid.uuid[CSR_BT_GATT_HIGH_BYTE];
    uuid = (uuid << CSR_BT_GATT_LE_BROWSER_BYTELEN) + inst->dbElement->currentDescr->uuid.uuid[CSR_BT_GATT_LOW_BYTE];
    
    if(uuid == CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC)
    {
        size = sizeof(CsrUint8)*inst->dbElement->currentDescr->valueLength + 1;
        dialogInputText = (CsrUint8 *)CsrPmemZalloc(size);
        snprintf((void*)dialogInputText, size, "%s", (char*)inst->dbElement->currentDescr->value);
    }
    else
    {
        if((inst->writeMethod & CSR_BT_LE_BROWSER_FORMATTED_WRITE) == CSR_BT_LE_BROWSER_FORMATTED_WRITE)
        {
            if(inst->dbElement->currentCharac != NULL)
            {
                size = sizeof(CsrUint8)*inst->dbElement->currentCharac->valueLength + 1;
                dialogInputText = (CsrUint8*)CsrPmemZalloc(size);
                /* [QTI] Fix KW issue#831891 through utilizing "snprintf" instead of "CsrMemCpy". */
                snprintf((void*)dialogInputText, size, "%s", (char*)inst->dbElement->currentCharac->value);
            }
            else
            {
                dialogInputText = '\0';
            }
        }
        else
        {
            dialogInputText = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8)*inst->dbElement->currentCharac->valueLength*2 + 1);
            CsrUint8ArrayToHex(inst->dbElement->currentCharac->value, (CsrCharString *)dialogInputText, inst->dbElement->currentCharac->valueLength);
        }
    }
    CsrUiInputdialogSetReqSend(inst->csrUiVar.hInputDialog,
                               CONVERT_TEXT_STRING_2_UCS2("Edit Value:"),
                               CONVERT_TEXT_STRING_2_UCS2(dialogMessage),
                               CSR_UI_ICON_EMPTY,
                               CONVERT_TEXT_STRING_2_UCS2(dialogInputText),
                               CSR_BT_LE_BROWSER_APP_MAX_INPUT_TEXT_LENGTH,
                               keyMap,
                               TEXT_LE_BROWSER_APP_SEND_UCS2,
                               TEXT_LE_BROWSER_APP_CANCEL_UCS2,
                               inst->csrUiVar.hInputSk1Event,
                               inst->csrUiVar.hBackEvent,
                               inst->csrUiVar.hBackEvent,
                               0);
    CsrPmemFree(dialogMessage);
}

void commonLeBrowserPopupShow(CsrBtLeBrowserAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);
}

void commonLeBrowserAcceptPopupShow(CsrBtLeBrowserAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);
}

void commonLeBrowserInputPopupShow(CsrBtLeBrowserAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hInputDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);
}

void commonLeBrowserPopupHide(CsrBtLeBrowserAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}
void commonLeBrowserViewPopupHide(CsrBtLeBrowserAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hInputDialog);
}
void commonLeBrowserEditMenuHide(CsrBtLeBrowserAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hEditMenu);
}

/* Wrapper functions for external use */
void CsrBtLeBrowserViewPopUpHide(CsrBtLeBrowserAppInstData *inst)
{
    commonLeBrowserViewPopupHide(inst);
}

void CsrBtLeBrowserPopUpHide(CsrBtLeBrowserAppInstData *inst)
{
    commonLeBrowserPopupHide(inst);
}

void CsrBtLeBrowserEditMenuHide(CsrBtLeBrowserAppInstData *inst)
{
    commonLeBrowserEditMenuHide(inst);
}
void CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupSet(CsrBtLeBrowserAppInstData *inst, const char *dialogHeading, const char *dialogText)
{
    commonLeBrowserAcceptPopupSet(inst, dialogHeading, dialogText);
}
void CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupShow(CsrBtLeBrowserAppInstData *inst)
{
    commonLeBrowserAcceptPopupShow(inst);
}

/* End of wrappers */
void CsrBtGattAppHandleLeBrowserCsrUiRegisterUnregister(CsrBtLeBrowserAppInstData *inst, CsrBool registered)
{
    CsrBtLeBrowserAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(registered)
    {
        /* Update main menu so it is no longer possible to activate but just to deactivate */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_LE_BROWSER_APP_UNREGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_LE_BROWSER_APP_UNREGISTER_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                0, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_FIRST, 
                                CSR_BT_LE_BROWSER_APP_CONNECT_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_LE_BROWSER_APP_CONNECT_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                0, 
                                csrUiVar->hBackEvent, 
                                0);



        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_LE_BROWSER_APP_REGISTER_KEY);
    }
    else
    {
        /* Update main menu so it is no longer possible to unRegister but just to register */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_LE_BROWSER_APP_REGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_LE_BROWSER_APP_REGISTER_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                0, 
                                csrUiVar->hBackEvent, 
                                0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_LE_BROWSER_APP_UNREGISTER_KEY);
        if(inst->btConnId == 0xff)
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_LE_BROWSER_APP_CONNECT_KEY);
        }
        else
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_LE_BROWSER_APP_DISCONNECT_KEY);
        }

    }
    /* Remove popup */
    commonLeBrowserPopupHide(inst);
}
/* If this is called when a NOTIFICATION INDICATION arrives it checks to see if the descriptor menu
   is on top and then updates the screen otherwise only the local value is updated. */
void CsrBtGattAppHandleLeBrowserCsrUiMenuUpdate(CsrBtLeBrowserAppInstData *inst, CsrUint16 caller)
{
    /*to prevent a panic in UI task */
    if(inst->btConnId != 0xff)
    {  
        if(caller == CSR_BT_GATT_NOTIFICATION_IND)
        {
            CsrUiDisplayGethandleReqSend(CsrSchedTaskQueueGet());
        }
        else 
        {
            CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_CHARAC_DESCR_MENU_STATE);
        }
    }
}

void CsrBtGattAppHandleLeBrowserCsrUiNoSelectedDevAddr(CsrBtLeBrowserAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonLeBrowserPopupHide(inst);
    commonLeBrowserAcceptPopupSet(inst, "Connecting", "No deviceAddr selected in GAP");
    commonLeBrowserAcceptPopupShow(inst);
} 

void CsrBtGattAppHandleLeBrowserCsrUiStillTraversingDb(CsrBtLeBrowserAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonLeBrowserPopupHide(inst);
    commonLeBrowserAcceptPopupSet(inst, "Warning", "Client is still traversing the remote database. please wait a few seconds and try again!");
    commonLeBrowserAcceptPopupShow(inst);
}
void CsrBtGattAppHandleLeBrowserCsrUiHidePopup(CsrBtLeBrowserAppInstData *inst)
{
    if(inst->dbElement->traversingDb == FALSE)
    {
        commonLeBrowserPopupHide(inst);
    }
}

void CsrBtGattAppHandleLeBrowserCsrUiReadValue(CsrBtLeBrowserAppInstData *inst, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonLeBrowserPopupHide(inst);
        CsrBtGattAppHandleLeBrowserCsrUiMenuUpdate(inst, 0);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonLeBrowserPopupHide(inst);
        commonLeBrowserAcceptPopupSet(inst, "Reading", "Could not read value");
        commonLeBrowserAcceptPopupShow(inst);
        CsrBtGattAppHandleLeBrowserCsrUiMenuUpdate(inst, 0);
    }
}
void CsrBtGattAppHandleLeBrowserCsrUiWriteValue(CsrBtLeBrowserAppInstData *inst, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button */
        commonLeBrowserPopupHide(inst);
        CsrBtGattAppHandleLeBrowserCsrUiMenuUpdate(inst, 0);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonLeBrowserPopupHide(inst);
        commonLeBrowserAcceptPopupSet(inst, "Writing", "Could not set value");
        commonLeBrowserAcceptPopupShow(inst);
    }
}


void CsrBtGattAppHandleLeBrowserCsrUiConnectDisconnect(CsrBtLeBrowserAppInstData *inst, 
                                                   CsrBool connected, 
                                                   CsrBool success)
{
    CsrBtLeBrowserAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(connected)
    {
        if(success)
        {
            CsrUint32 addrSize = 15;
            char *addr = CsrPmemAlloc(addrSize);
            snprintf(addr, addrSize, "%04X:%02X:%06X", inst->selectedDeviceAddr.addr.nap, inst->selectedDeviceAddr.addr.uap, inst->selectedDeviceAddr.addr.lap);
            /* Update main menu so it is no longer possible to connect and activate but just to disconnect and CsrBtLeBrowserAppSend file */
            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_FIRST, 
                                    CSR_BT_LE_BROWSER_APP_DISCONNECT_KEY, 
                                    CSR_UI_ICON_DISCONNECT, 
                                    TEXT_LE_BROWSER_APP_DISCONNECT_UCS2, 
                                    CsrUtf82Utf16String((CsrUtf8String *) addr),
                                    csrUiVar->hSk1Event, 
                                    0, 
                                    csrUiVar->hBackEvent, 
                                    0)
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_LE_BROWSER_APP_READ_DATABASE_KEY,
                                    CSR_UI_ICON_FOLDER_EXPLORE,
                                    TEXT_LE_BROWSER_APP_VIEW_PRIM_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event,
                                    0,
                                    csrUiVar->hBackEvent,
                                    0);
            CsrPmemFree(addr);
        }
    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just to connect and activate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_LE_BROWSER_APP_CONNECT_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_LE_BROWSER_APP_CONNECT_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                0, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_LE_BROWSER_APP_UNREGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_LE_BROWSER_APP_UNREGISTER_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                0, 
                                csrUiVar->hBackEvent, 
                                0);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;

        /* Remove popup */
        commonLeBrowserPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}

void CsrBtGattAppHandleLeBrowserSetMainMenuHeader(CsrBtLeBrowserAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("LE Browser"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtLeBrowserAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtLeBrowserAppUiInstData *csrUiVar = &inst->csrUiVar;

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
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hSk2Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hInputSk1Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hInputSk1Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        if (!csrUiVar->hInputDialog)
        {
            
            csrUiVar->hInputDialog = confirm->handle;
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
        if(!csrUiVar->hMainMenu)
        {
            CsrUiMenuSetReqSend(confirm->handle,
                                CONVERT_TEXT_STRING_2_UCS2(inst->profileName),
                                TEXT_SELECT_UCS2, 
                                NULL);

            CsrUiMenuAdditemReqSend(confirm->handle,
                                    CSR_UI_LAST, 
                                    CSR_BT_LE_BROWSER_APP_REGISTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_LE_BROWSER_APP_REGISTER_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent, 0);

            csrUiVar->hMainMenu = confirm->handle;
        }
        else if(!csrUiVar->hPrimMenu)
        {
            CsrUiMenuSetReqSend(confirm->handle,
                                CONVERT_TEXT_STRING_2_UCS2("Primary Services"),
                                TEXT_SELECT_UCS2,
                                NULL);
            csrUiVar->hPrimMenu = confirm->handle;
            CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_PRIMARY_SERVICES_MENU_STATE);
        }
        else if(!csrUiVar->hCharMenu)
        {
            csrUiVar->hCharMenu = confirm->handle;
            CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_CHARAC_MENU_STATE);
        }
        else if(!csrUiVar->hDescMenu)
        {
            csrUiVar->hDescMenu = confirm->handle;
            CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_CHARAC_DESCR_MENU_STATE);
        }
        else if(!csrUiVar->hEditMenu)
        {
            csrUiVar->hEditMenu = confirm->handle;
            CsrUiMenuSetReqSend(confirm->handle,
                                CONVERT_TEXT_STRING_2_UCS2("Edit"),
                                TEXT_SELECT_UCS2,
                                NULL);
            CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_EDIT_MENU_STATE);
        }
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtLeBrowserAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrUtf8String* inputStr = CsrUtf16String2Utf8(confirm->text);
    
    CsrBtLeBrowserAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUint16 writeMethod = 0xFFF & inst->writeMethod;
    CsrUint16 newValueLength = 0;
    CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_2);
    if(inputStr != NULL)
    {
        CsrSize inputStrLen = CsrStrLen((CsrCharString*)inputStr);
        CsrUint8* value2Send = (CsrUint8*)CsrPmemAlloc(inputStrLen+1);
        
        if((inst->writeMethod & CSR_BT_LE_BROWSER_RAW_WRITE) == CSR_BT_LE_BROWSER_RAW_WRITE)
        {/* Input raw data */
            CsrUint16 i;
            CsrUint8 temp = 0x00;
            
            for(i = 0; i < inputStrLen; i++)
            {

                if((inputStr[i] >= '0') && (inputStr[i] <= '9'))
                {
                    temp += (CsrUint8)(inputStr[i] - '0');
                }
                else if((inputStr[i] >= 'A') && (inputStr[i] <= 'F'))
                {
                    temp += (CsrUint8)(inputStr[i] - 'A' + 10);
                }
                else if((inputStr[i] >= 'a') && (inputStr[i] <= 'f'))
                {
                    temp += (CsrUint8)(inputStr[i] - 'a' + 10);
                }
                else
                {
                    /* Illigal hex-value, must return */
                    CsrBtGattAppHandleLeBrowserCsrUiWriteValue(inst, FALSE);
                }
            
                if(i%2 == 1)
                {
                    *(value2Send + newValueLength) = temp;
                    temp = 0x00;
                    newValueLength++;
                }
                else
                {
                    temp = temp << 4;
                }

            }
            /* End of raw data input */
        }
        else if((inst->writeMethod & CSR_BT_LE_BROWSER_FORMATTED_WRITE) == CSR_BT_LE_BROWSER_FORMATTED_WRITE)
        {
            CsrMemCpy(value2Send, inputStr, inputStrLen);
            *(value2Send + inputStrLen) = '\0';
            newValueLength = (CsrUint16)(inputStrLen + 1);
        }

        switch(writeMethod)
        {
            case CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE:
                {
                    /* Updating local version of written value */
                    CsrPmemFree(inst->dbElement->currentCharac->value);
                    /* Using newValueLength will allow for value to contain 0x00 */
                    inst->dbElement->currentCharac->value = (CsrUint8*)CsrPmemAlloc(newValueLength);
                    CsrMemCpy(inst->dbElement->currentCharac->value, value2Send, newValueLength);
                    inst->dbElement->currentCharac->valueLength = newValueLength;

                    /* Sending the value just written */
                    CsrBtGattWriteCmdReqSend(inst->gattId, inst->btConnId, inst->dbElement->currentCharac->valueHandle, newValueLength, value2Send );
                    break;
                }
            case CSR_BT_GATT_CHARAC_PROPERTIES_WRITE:
                {
                    CsrSize tmpLen = CsrStrLen((CsrCharString*)value2Send);
                    inst->dbElement->currentCharac->valueLength = newValueLength;
                    inst->inputStr = (CsrUint8*)CsrPmemAlloc(tmpLen + 1);
                    CsrMemCpy(inst->inputStr, value2Send, tmpLen + 1);

                    CsrBtGattWriteReqSend(inst->gattId, inst->btConnId, inst->dbElement->currentCharac->valueHandle, 0x0000, newValueLength, value2Send);
                    break;
                }
            case CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES:
                {
                    /* Updating local version of written value */
                    CsrPmemFree(inst->dbElement->currentCharac->value);
                    /* Using newValueLength will allow for value to contain 0x00 */
                    inst->dbElement->currentCharac->value = (CsrUint8*)CsrPmemAlloc(newValueLength);
                    CsrMemCpy(inst->dbElement->currentCharac->value, value2Send, newValueLength );
                    inst->dbElement->currentCharac->valueLength = newValueLength;

                    /* Sending the value just written */
                    CsrBtGattWriteSignedCmdReqSend(inst->gattId, inst->btConnId, inst->dbElement->currentCharac->valueHandle, newValueLength, value2Send );
                    
                    break;
                }
            case CSR_BT_LE_BROWSER_APP_RELIABLE_WRITE_METHOD:
                {
                    CsrBtGattAttrWritePairs* attrWritePair = (CsrBtGattAttrWritePairs *)CsrPmemAlloc(sizeof(CsrBtGattAttrWritePairs));
                    CsrSize tmpLen = CsrStrLen((CsrCharString*)inputStr);
                    /* Updating local copy */
                    /* FIXME: update the local copy of the value, not only value length */
                    inst->dbElement->currentCharac->valueLength = newValueLength;
                    
                    attrWritePair->attrHandle = inst->dbElement->currentCharac->valueHandle;
                    attrWritePair->offset = 0;
                    attrWritePair->valueLength = newValueLength;
                    attrWritePair->value = value2Send;

                    inst->inputStr = (CsrUint8*)CsrPmemAlloc(tmpLen + 1);
                    CsrMemCpy(inst->inputStr, inputStr, tmpLen + 1);

                    CsrBtGattReliableWritesReqSend(inst->gattId, inst->btConnId, 1, attrWritePair);

                    break;
                }
            case CSR_BT_LE_BROWSER_APP_WRITE_AUX_METHOD:
                {
                    CsrBtUuid uuid;
                    CsrBtGattAppDbCharacDescrElement *dElem;
                    CsrSize tmpLen = CsrStrLen((CsrCharString*) inputStr);

                    CsrBtUuid16ToUuid(CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC,
                                      &uuid);
                    dElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(inst->dbElement->currentCharac->descrList,
                                                                         &uuid);

                    inst->inputStr = (CsrUint8*) CsrPmemZalloc(tmpLen + 1);
                    CsrMemCpy(inst->inputStr, inputStr, tmpLen);

                    CsrBtGattWriteUserDescriptionReqSend(inst->gattId, inst->btConnId, dElem->descriptorHandle, value2Send); 
                    break;
                }
            default:
                {
                    /* Unknown write method */
                    break;
                }

        }
        
        CsrPmemFree(inputStr);
        CsrPmemFree(confirm->text);
    }
    /* Hide input dialog */
    CsrUiUieHideReqSend(csrUiVar->hInputDialog);

    

}
static void handleCsrUiDisplayGethandleCfm(CsrBtLeBrowserAppInstData *inst, CsrUiDisplayGethandleCfm *confirm)
{
    /* Updates the descriptors menu when a new temperature value arrives to be shown 
       and the descriptors menu is on top.*/
    /*FIXME: Make it general for all active notifications */

    CsrUint16 uuid = (CsrUint16)(inst->dbElement->currentCharac->uuid.uuid[1] << 8);
    uuid += (CsrUint16)inst->dbElement->currentCharac->uuid.uuid[0];

    if((confirm->handle == inst->csrUiVar.hDescMenu) && (uuid == CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC)) 
    {
        CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_CHARAC_DESCR_MENU_STATE);
    }
}

static void handleCsrUiIndicationKey(CsrBtLeBrowserAppInstData *inst, CsrUint16 key)
{
    CsrBtLeBrowserAppUiInstData *csrUiVar = &inst->csrUiVar;
    /* Main Menu handling */
    switch(key)
    {
        case CSR_BT_LE_BROWSER_APP_REGISTER_KEY:
            {
                commonLeBrowserPopupSet(inst, "Registering", "Please wait");
                commonLeBrowserPopupShow(inst);
                CsrBtLeBrowserAppStartRegister(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_UNREGISTER_KEY:
            {
                commonLeBrowserPopupSet(inst, "Unregistering", "Please wait");
                commonLeBrowserPopupShow(inst);
                CsrBtLeBrowserAppStartUnregister(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_CONNECT_KEY:
            {
                commonLeBrowserPopupSet(inst, "Connecting", "Please wait");
                commonLeBrowserPopupShow(inst);
                CsrBtLeBrowserAppStartConnecting(inst);
                inst->csrUiVar.localInitiatedAction = TRUE;
                break;
            }
        case CSR_BT_LE_BROWSER_APP_DISCONNECT_KEY:
            {
                commonLeBrowserPopupSet(inst, "Disconnecting", "Please wait");
                commonLeBrowserPopupShow(inst);
                CsrBtLeBrowserAppStartDisconnect(inst);
                inst->csrUiVar.localInitiatedAction = TRUE;
                break;
            }
        case CSR_BT_LE_BROWSER_APP_READ_DATABASE_KEY:
            {
                if(inst->dbElement->traversingDb == TRUE)
                {
                    commonLeBrowserPopupSet(inst, "LE Browser", "Reading remote database...");
                    commonLeBrowserPopupShow(inst);
                    break;
                }
                else
                {
                    CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_PRIMARY_SERVICES_MENU_STATE);
                    break;
                }
            }
        case CSR_BT_LE_BROWSER_APP_PROPERTIES_KEY:
            {
                CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_PROBERTIES_MENU_STATE);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_CHARAC_VALUE_KEY:
            {
                CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_CHARAC_VALUE_MENU_STATE);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_BROADCAST_KEY:
            {
                /* Toggle  */
                if(inst->dbElement->traversingDb)
                {
                    CsrBtGattAppHandleLeBrowserCsrUiStillTraversingDb(inst);
                }
                else
                {
                    CsrUint16 val = CSR_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST; 
                        
                    commonLeBrowserPopupSet(inst, "LE Browser", "Toggling Broadcasting.");
                    commonLeBrowserPopupShow(inst);
                    if(inst->broadcasting)
                    {
                        val = CSR_BT_GATT_SERVER_CHARAC_CONFIG_DISABLE;

                    }
                    inst->propertyToWrite = CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST;
                    
                    CsrBtLeBrowserAppStartWriteServerConfig(inst, val);
                }
                break;
            }
        case CSR_BT_LE_BROWSER_APP_READ_KEY:
            {
                /* Read this value from remote database again */

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                /* Read new value */
                commonLeBrowserPopupSet(inst, "LE Browser", "Reading Value.");
                commonLeBrowserPopupShow(inst);
                if(inst->btConnId != 0xff) /* Check for connection */
                {
                    CsrBtGattReadReqSend(inst->gattId, inst->btConnId, inst->dbElement->currentCharac->valueHandle,0);
                }
                else
                {
                    /* Throw exception for no connection */
                    CsrGeneralException("CSR_LE_BROWSER_DEMO_APP", 
                                        0, 
                                        key,
                                        "No Connection Available");
                }
                break;
            }
        case CSR_BT_LE_BROWSER_APP_WRITECOMMAND_KEY:
            {
                /* Do a write operation on remote database without expecting confirmation */
                CsrUint32 popupMessageSize = sizeof(CsrUint8)*CSR_BT_LE_BROWSER_POPUPMSG_LEN;
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize); 

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Received Value Length: %X", inst->dbElement->currentCharac->valueLength);
                
                inst->writeMethod = CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE;
                inst->writeMethod |= CSR_BT_LE_BROWSER_FORMATTED_WRITE;

                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);

                break;
            }
        case CSR_BT_LE_BROWSER_APP_WRITECOMMAND_RAW_KEY:
            {
                /* Do a write operation on remote database without expecting confirmation */
                /* Input is taken as unformatted hex-values */
                CsrUint32 popupMessageSize = sizeof(CsrUint8)*CSR_BT_LE_BROWSER_POPUPMSG_LEN;
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize); 

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Received Value Length: %X", inst->dbElement->currentCharac->valueLength);
                
                inst->writeMethod = CSR_BT_GATT_CHARAC_PROPERTIES_WRITE_WITHOUT_RESPONSE;
                inst->writeMethod |= CSR_BT_LE_BROWSER_RAW_WRITE;

                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);

                break;
            }
        case CSR_BT_LE_BROWSER_APP_WRITE_KEY:
            {
                /* Do a write request operation on remote database */
                CsrUint32 popupMessageSize = sizeof(CsrUint8)*CSR_BT_LE_BROWSER_POPUPMSG_LEN;
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize); 

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Received Value Length: %X", inst->dbElement->currentCharac->valueLength);
                
                inst->writeMethod = CSR_BT_GATT_CHARAC_PROPERTIES_WRITE;
                inst->writeMethod |= CSR_BT_LE_BROWSER_FORMATTED_WRITE;

                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_WRITE_RAW_KEY:
            {
                /* Do a write request operation on remote database */
                /* Input is taken as unformatted hex-values */
                CsrUint32 popupMessageSize = sizeof(CsrUint8)*CSR_BT_LE_BROWSER_POPUPMSG_LEN;
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize); 
                
                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Received Value Length: %X", inst->dbElement->currentCharac->valueLength);
                
                inst->writeMethod = CSR_BT_GATT_CHARAC_PROPERTIES_WRITE;
                inst->writeMethod |= CSR_BT_LE_BROWSER_RAW_WRITE;

                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_NOTIFY_KEY:
            {
                /* Toggle notifications */
                if(inst->dbElement->traversingDb)
                {
                    CsrBtGattAppHandleLeBrowserCsrUiStillTraversingDb(inst);
                }
                else
                {
                    CsrUint16 val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION; 
                        
                    commonLeBrowserPopupSet(inst, "LE Browser", "Toggling Notifications.");
                    commonLeBrowserPopupShow(inst);
                    if(inst->tempNotif)
                    {
                        val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
                    }
                    inst->propertyToWrite = CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY;
                    
                    CsrBtLeBrowserAppStartWriteClientConfig(inst, val);
                }
                break;
            }
        case CSR_BT_LE_BROWSER_APP_INDICATE_KEY:
            {
                /* Toggle indications */
                if(inst->dbElement->traversingDb)
                {
                    CsrBtGattAppHandleLeBrowserCsrUiStillTraversingDb(inst);
                }
                else
                {
                    CsrUint16 val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION; 
                        
                    commonLeBrowserPopupSet(inst, "LE Browser", "Toggling Indication.");
                    commonLeBrowserPopupShow(inst);
                    if(inst->tempInd)
                    {
                        val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
                    }
                    inst->propertyToWrite = CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE;
                    
                    CsrBtLeBrowserAppStartWriteClientConfig(inst, val);
                }
                break;
            }
        case CSR_BT_LE_BROWSER_APP_AUTH_WRITES_KEY:
            {
                /* Do an Authenticated Signed Write on remote database */
                CsrUint32 popupMessageSize = sizeof(CsrUint8)*25;
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize); /* Allocation assumes at most 9 bytes valueLength */

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Received Value Length: %X", inst->dbElement->currentCharac->valueLength);
                inst->writeMethod = CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES;
                inst->writeMethod |= CSR_BT_LE_BROWSER_FORMATTED_WRITE;
                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_AUTH_WRITES_RAW_KEY:
            {
                /* Do an Authenticated Signed Write on remote database 
                 * with unformatted input data.*/
                CsrUint32 popupMessageSize = sizeof(CsrUint8)*25;
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize); /* Allocation assumes at most 9 bytes valueLength */
                
                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Received Value Length: %X", inst->dbElement->currentCharac->valueLength);
                inst->writeMethod = CSR_BT_GATT_CHARAC_PROPERTIES_AUTH_SIGNED_WRITES;
                inst->writeMethod |= CSR_BT_LE_BROWSER_RAW_WRITE;
                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_EXT_RELIABLE_WRITE_KEY:
            {
                CsrUint32 popupMessageSize = sizeof("Input Value");
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize);

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Input Value");
                inst->writeMethod = CSR_BT_LE_BROWSER_APP_RELIABLE_WRITE_METHOD;
                inst->writeMethod |= CSR_BT_LE_BROWSER_FORMATTED_WRITE;
                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_EXT_RELIABLE_WRITE_RAW_KEY:
            {
                CsrUint32 popupMessageSize = sizeof("Input Value");
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize);

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_1);

                snprintf((void*)popupMessage, popupMessageSize, "Input Value");
                inst->writeMethod = CSR_BT_LE_BROWSER_APP_RELIABLE_WRITE_RAW_METHOD;
                inst->writeMethod |= CSR_BT_LE_BROWSER_RAW_WRITE;
                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_EXT_WRITABLE_AUX_KEY:
            {
                CsrUint32 popupMessageSize = sizeof("Input User Description");
                CsrUint8* popupMessage = (CsrUint8*)CsrPmemAlloc(popupMessageSize);

                CsrBtGattLeBrowserUpdateDbElement(inst, CSR_BT_GATT_LE_BROWSER_LVL_2);

                snprintf((void*)popupMessage, popupMessageSize, "Input User Description");
                inst->writeMethod = CSR_BT_LE_BROWSER_APP_WRITE_AUX_METHOD;
                inst->writeMethod |= CSR_BT_LE_BROWSER_FORMATTED_WRITE;
                commonLeBrowserInputPopupSet(inst, popupMessage, CSR_UI_KEYMAP_ALPHANUMERIC);
                commonLeBrowserInputPopupShow(inst);
                break;
            }
        case CSR_BT_LE_BROWSER_APP_OK_KEY:
            {
                commonLeBrowserPopupHide(inst);
                break;
            }
        default:
            {
                break;
            }
    }

    if((key >= CSR_BT_LE_BROWSER_APP_PRIMARY_SERVICES_KEY) && (key < CSR_BT_LE_BROWSER_APP_CHARACTERISTICS_KEY))
    {
        inst->selectedPrimService = key - CSR_BT_LE_BROWSER_APP_PRIMARY_SERVICES_KEY;
        if(!csrUiVar->hCharMenu)
        {
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
        else
        {
            CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_CHARAC_MENU_STATE);
        }
    }
    else if((key >= CSR_BT_LE_BROWSER_APP_CHARACTERISTICS_KEY) && (key < CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY))
    {
        inst->selectedCharac = key - CSR_BT_LE_BROWSER_APP_CHARACTERISTICS_KEY;
        if(!csrUiVar->hDescMenu)
        {
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
        else
        {
            CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_CHARAC_DESCR_MENU_STATE);
        }
    }
    else if(key >= CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY)
    {

        inst->selectedDescriptor = key - CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY;

        CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_VALUE_MENU_STATE);
    }
}

static void handleCsrUiUieEventInd(CsrBtLeBrowserAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtLeBrowserAppUiInstData *csrUiVar = &inst->csrUiVar;;

    if (indication->event == csrUiVar->hBackEvent)
    {
        /*hide current shown displayelement*/
        CsrUiUieHideReqSend(indication->displayElement);
    }
    else if(indication->event == csrUiVar->hInputSk1Event)
    {
        CsrUiInputdialogGetReqSend(CsrSchedTaskQueueGet(), csrUiVar->hInputDialog);
    }
    else if (indication->event == csrUiVar->hSk1Event)
    {
        handleCsrUiIndicationKey(inst, indication->key);
    }
    else if (indication->event == csrUiVar->hSk2Event)
    {
        CsrUint16 key;
        if(indication->key >= CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY)
        {
            key = CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY;
        }
        else
        {
            key = indication->key;
        }

        switch(key)
        {
            case CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY:
            case CSR_BT_LE_BROWSER_APP_CHARAC_VALUE_KEY:
                {
                    if(!csrUiVar->hEditMenu)
                    {
                        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
                    }
                    else
                    {
                        CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(inst, CSR_BT_LE_BROWSER_EDIT_MENU_STATE);
                    }
                }
            default:
                {
                    break;
                }
        }
    }
}


void CsrBtGattAppHandleLeBrowserCsrUiPrim(CsrBtLeBrowserAppInstData *inst)
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
        case CSR_UI_DISPLAY_GETHANDLE_CFM:
            {
                handleCsrUiDisplayGethandleCfm(inst, inst->recvMsgP);
                break;
            }
        default:
            {
                break;
            }
    }
}


