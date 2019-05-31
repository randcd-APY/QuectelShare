/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#ifndef EXCLUDE_CSR_BT_THERM_SRV_MODULE

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_therm_srv_prim.h"
#include "csr_bt_therm_srv_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_therm_server_app.h"
#include "csr_bt_gatt_demo_therm_server_app_strings.h"

/* Helper functions */

CsrUtf8String *commonThermsGetValueString( CsrUint16 value)
{
    switch(value)
    {
        case CSR_BT_GATT_APP_ALERT_LEVEL_OFF:
        {
            return (CsrUtf8String *)CsrStrDup("Alert level changed to: Off");            
        }
        case CSR_BT_GATT_APP_ALERT_LEVEL_LOW:
        {
            return (CsrUtf8String *)CsrStrDup("Akert level changed to: Low");
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
void commonThermsPopupSet(CsrBtThermsAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonThermsStopPopupSet(CsrBtThermsAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_STOP_UCS2, 
                          NULL, 
                          inst->csrUiVar.hSk1Event,
                          0,
                          0,
                          0);
}


void commonThermsAcceptPopupSet(CsrBtThermsAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                        CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                        CONVERT_TEXT_STRING_2_UCS2(dialogText),
                        TEXT_THERMS_APP_OK_UCS2, 
                        NULL, 
                        inst->csrUiVar.hBackEvent,
                        0,
                        inst->csrUiVar.hBackEvent,
                        0);
}

void commonThermsPopupShow(CsrBtThermsAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_THERMS_APP_CSR_UI_PRI);
}

void commonThermsAcceptPopupShow(CsrBtThermsAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_THERMS_APP_CSR_UI_PRI);
}

void commonThermsPopupHide(CsrBtThermsAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}

void CsrBtGattAppHandleThermsCsrUiMenuUpdate(CsrBtThermsAppInstData *inst)
{
    /*Could be used to show more info in the menu */
}
void CsrBtGattAppHandleThermsCsrUiValueUpdate(CsrBtThermsAppInstData *inst, 
                                              CsrUint16 handle, 
                                              CsrUint16 value)
{

    switch(handle - inst->dbStartHandle)
    {
        case CSR_BT_THERM_SRV_TEMP_MEASUREMENT_CC_HANDLE:
        {
            commonThermsAcceptPopupSet(inst, "Temperature measurement", (char*)commonThermsGetValueString(value));
            commonThermsAcceptPopupShow(inst);
            break;
        }
        default:
        {
            CsrUint32 retSize = 16;
            CsrUtf8String *ret = CsrPmemAlloc(retSize);
            snprintf((char*)ret, retSize, "handle: 0x%04X", handle-inst->dbStartHandle);
            commonThermsAcceptPopupSet(inst, "Error", (char*)ret);
            commonThermsAcceptPopupShow(inst);
        }
    }


}


void CsrBtGattAppHandleThermsCsrUiActivateDeactivate(CsrBtThermsAppInstData *inst, 
                                                     CsrBool activated, 
                                                     CsrBool success)
{
    CsrBtThermsAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(activated)
    {
        if(success)
        {
            /* Update main menu so it is no longer possible to connect and activate but just to deactivate */
            CsrUint32 addrSize = 15;
            char *addr = CsrPmemAlloc(addrSize);
            snprintf(addr, addrSize, "%04X:%02X:%06X", inst->connectedDeviceAddr.addr.nap, inst->connectedDeviceAddr.addr.uap, inst->connectedDeviceAddr.addr.lap);

            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_FIRST, 
                                    CSR_BT_THERMS_APP_DEACTIVATE_KEY, 
                                    CSR_UI_ICON_DISCONNECT, 
                                    TEXT_THERMS_APP_DEACTIVATE_UCS2, 
                                    CsrUtf82Utf16String((CsrUtf8String *) addr),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_THERMS_APP_TOGGLE_TEMP_TYPE_KEY, 
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_THERMS_APP_TOGGLE_TEMP_TYPE_UCS2, 
                                    CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->tempType)?"Fahrenheit":"Celsius"))),
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
                                CSR_BT_THERMS_APP_ACTIVATE_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_THERMS_APP_ACTIVATE_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;

        /* Remove popup */
        commonThermsPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}



void CsrBtGattAppHandleThermsSetMainMenuHeader(CsrBtThermsAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("Thermometer Srv"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtThermsAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtThermsAppUiInstData *csrUiVar = &inst->csrUiVar;

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
                                CSR_BT_THERMS_APP_ACTIVATE_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_THERMS_APP_ACTIVATE_UCS2,
                                NULL,
                                csrUiVar->hSk1Event,
                                csrUiVar->hSk2Event,
                                csrUiVar->hBackEvent, 0);

        csrUiVar->hMainMenu = confirm->handle;
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtThermsAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrBtThermsAppUiInstData *csrUiVar = &inst->csrUiVar;

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(csrUiVar->hSelectFileInputDialog);

    CsrPmemFree(confirm->text);

}

static void handleCsrUiUieEventInd(CsrBtThermsAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtThermsAppUiInstData *csrUiVar = &inst->csrUiVar;;

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
        if(indication->displayElement == csrUiVar->hCommonDialog)
        {
            /* Stop peripheral req */
            inst->showDisconnect = FALSE;
            CsrUiUieHideReqSend(indication->displayElement);
            CsrBtThermsAppStartDisconnect(inst);               
        }
        else
        {
            switch(indication->key)
            {
                case CSR_BT_THERMS_APP_ACTIVATE_KEY:
                {
                    commonThermsStopPopupSet(inst, "Advertising", "Please wait");
                    commonThermsAcceptPopupShow(inst);
                    CsrBtThermsAppStartActivate(inst);
                    break;
                }
                case CSR_BT_THERMS_APP_DEACTIVATE_KEY:
                {
                    commonThermsPopupSet(inst, "Deactivating", "Please wait");
                    commonThermsPopupShow(inst);
                    CsrBtThermsAppStartDeactivate(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
                case CSR_BT_THERMS_APP_TOGGLE_TEMP_TYPE_KEY:
                {
                    if(inst->tempType == CSR_BT_THERMS_APP_TEMP_TYPE_CELSIUS)
                    {
                        inst->tempType = CSR_BT_THERMS_APP_TEMP_TYPE_FAHRENHEIT;
                    }
                    else 
                    {
                        inst->tempType = CSR_BT_THERMS_APP_TEMP_TYPE_CELSIUS;
                    }
                    CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                            CSR_BT_THERMS_APP_TOGGLE_TEMP_TYPE_KEY, 
                                            CSR_UI_ICON_MARK_INFORMATION, 
                                            TEXT_THERMS_APP_TOGGLE_TEMP_TYPE_UCS2, 
                                            CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->tempType)?"Fahrenheit":"Celsius"))),
                                            csrUiVar->hSk1Event, 
                                            csrUiVar->hSk2Event, 
                                            csrUiVar->hBackEvent, 
                                            0);

                    break;
                }
                case CSR_BT_THERMS_APP_OK_KEY:
                {
                    commonThermsPopupHide(inst);
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


void CsrBtGattAppHandleThermsCsrUiPrim(CsrBtThermsAppInstData *inst)
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
