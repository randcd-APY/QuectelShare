/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
*****************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_therm_client_app.h"
#include "csr_bt_gatt_demo_therm_client_app_strings.h"
#include "csr_bt_gatt_demo_defines.h"
 

/* constants */
static char *therm_present_substate[]={"Not Supported", "Unknown", "Not Present", "Present"};
static char *therm_discharging_substate[]={"Not Supported", "Unknown", "Not Discharging", "Discharging"};
static char *therm_charging_substate[]={"Unknown", "Not Chargeable", "Not Charging (Chargeable)", "Charging (Chargeable)"};
static char *therm_critically_low_level_substate[]={"Not Supported", "Unknown", "Good Level", "Critically Low Level"};
static char *therm_service_required[]={"Unknown","No Service Required","Service Required"};


/* Helper functions */
CsrDouble signedPow(CsrInt8 base, CsrInt8 exp)
{
    if (exp == 0)
    {
        return 1;
    }
    else
    {
        if (exp > 0)
        {
            return (CsrDouble)(signedPow(base, (CsrInt8)(exp-1)) * base);
        }
        else
        {
            return (CsrDouble)(signedPow(base, (CsrInt8)(exp+1)) / base);
        }
    }
}

CsrUtf16String *commonThermcGetValueString(CsrBtThermcAppInstData *inst, CsrUint16 uuid)
{

    CsrBtGattAppDbPrimServiceElement *pElem;
    char *ret              =  NULL;
    CsrUint32 retSize      = 0;
    CsrUtf16String *ret16  =  NULL;

    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(inst->dbElement->dbPrimServices,CsrBtThermcAppUuid16ToUuid128(uuid));

    if(pElem != NULL)
    {
        switch(uuid)
        {
        case CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE:
            {
                CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                            CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC));
                if(cElem != NULL && cElem->value != NULL)
                {
                    char type  =  'C';/* Celcius */
                    /* first byte is flags */
                    CsrUint8 flags = (CsrUint8)cElem->value[0];
                    /* next 4 bytes are IEEE-11073 float format (little endian) */
                    CsrFloat temp = 0;
                    CsrInt32 x = 0;
                    CsrDouble z = signedPow(10,(CsrInt8)(cElem->value[4]));

                    CsrMemCpy(&x, &cElem->value[1], 3);
                    if (x & 0x00800000)
                    {/* take care of the sign */
                        x |= 0xFF000000;
                    }

                    temp = (CsrFloat)(x * z);

                    if(flags & 0x01)
                    {
                        type = 'F'; /* Fahrenheit */
                    }
                    retSize = 25;
                    ret = CsrPmemAlloc(retSize);
                    snprintf(ret, retSize, "%.2f%c Update: %s", temp, type,  (inst->tempNotif==TRUE)?"On ":"Off");
                }
                else 
                {
                    retSize = 12;
                    ret = CsrPmemAlloc(retSize);
                    snprintf(ret, retSize, "Update: %s", (inst->tempNotif==TRUE)?"On ":"Off");
                }

                break;
            }
        case CSR_BT_GATT_UUID_BATTERY_SERVICE:
            {
                CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                            CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_UUID_BATTERY_LEVEL_CHARAC));
                if(cElem != NULL && cElem->value != NULL)
                {
                    retSize = 12;
                    ret = CsrPmemAlloc(retSize);
                    snprintf(ret, retSize, "Level %d%%", (CsrUint8)cElem->value[0]);
                }
                break;

            }
        default:
            {
                ret = (char*)CsrStrDup("Unknown UUID");
                break;
            }
        }
    }

    if (ret == NULL)
    {
        if(inst->dbElement->traversingDb)
        {
            ret = (char*)CsrStrDup("Loading...");
        }
        else 
        {
            ret = (char*)CsrStrDup("Not Found");
        }
    }
    else
    {
        /* Do nothing */
    }

    ret16 = CsrUtf82Utf16String((CsrUtf8String *)ret);
    CsrPmemFree(ret);
    return ret16;
}

void commonThermcPopupSet(CsrBtThermcAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonThermcAcceptPopupSet(CsrBtThermcAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_THERMC_APP_OK_UCS2, 
                          NULL, 
                          inst->csrUiVar.hBackEvent,
                          0,
                          inst->csrUiVar.hBackEvent,
                          0);
}

void commonThermcPopupShow(CsrBtThermcAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_THERMC_APP_CSR_UI_PRI);
}

void commonThermcAcceptPopupShow(CsrBtThermcAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_THERMC_APP_CSR_UI_PRI);
}

void commonThermcPopupHide(CsrBtThermcAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}

void CsrBtGattAppHandleThermcCsrUiRegisterUnregister(CsrBtThermcAppInstData *inst, CsrBool registered)
{
    CsrBtThermcAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(registered)
    {
        /* Update main menu so it is no longer possible to activate but just to deactivate */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_THERMC_APP_UNREGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_THERMC_APP_UNREGISTER_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_FIRST, 
                                CSR_BT_THERMC_APP_CONNECT_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_THERMC_APP_CONNECT_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);


        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_THERMC_APP_REGISTER_KEY);
    }
    else
    {
        /* Update main menu so it is no longer possible to unRegister but just to register */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_THERMC_APP_REGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_THERMC_APP_REGISTER_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_THERMC_APP_UNREGISTER_KEY);
        if(inst->btConnId == 0xff)
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_THERMC_APP_CONNECT_KEY);
        }
        else
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_THERMC_APP_DISCONNECT_KEY);
        }

    }
    /* Remove popup */
    commonThermcPopupHide(inst);
}

void CsrBtGattAppHandleThermcCsrUiMenuUpdate(CsrBtThermcAppInstData *inst)
{

    /*to prevent a panic in UI task */
    if(inst->btConnId != 0xff)
    {
        CsrBtThermcAppUiInstData *csrUiVar = &inst->csrUiVar;
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_THERMC_APP_WRITE_TEMPERATURE_KEY, 
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_THERMC_APP_TOGGLE_TEMPERATURE_UCS2, 
                                commonThermcGetValueString(inst, CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_THERMC_APP_BATTERY_INFO_KEY, 
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_THERMC_APP_BATTERY_INFO_UCS2, 
                                commonThermcGetValueString(inst, CSR_BT_GATT_UUID_BATTERY_SERVICE),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
    }

}

void CsrBtGattAppHandleThermcCsrUiNoSelectedDevAddr(CsrBtThermcAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonThermcPopupHide(inst);
    commonThermcAcceptPopupSet(inst, "Connecting", "No deviceAddr selected in GAP");
    commonThermcAcceptPopupShow(inst);
}

void CsrBtGattAppHandleThermcCsrUiStillTraversingDb(CsrBtThermcAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonThermcPopupHide(inst);
    commonThermcAcceptPopupSet(inst, "Warning", "Client is still traversing the remote database. please wait a few seconds and try again!");
    commonThermcAcceptPopupShow(inst);
}

void CsrBtGattAppHandleThermcCsrUiReadValue(CsrBtThermcAppInstData *inst, char *str, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonThermcPopupHide(inst);
        CsrBtGattAppHandleThermcCsrUiMenuUpdate(inst);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonThermcPopupHide(inst);
        commonThermcAcceptPopupSet(inst, "Reading", "Could not read value");
        commonThermcAcceptPopupShow(inst);
        CsrBtGattAppHandleThermcCsrUiMenuUpdate(inst);
    }
}
void CsrBtGattAppHandleThermcCsrUiWriteValue(CsrBtThermcAppInstData *inst, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button */
        commonThermcPopupHide(inst);
        CsrBtGattAppHandleThermcCsrUiMenuUpdate(inst);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonThermcPopupHide(inst);
        commonThermcAcceptPopupSet(inst, "Writing", "Could not set value");
        commonThermcAcceptPopupShow(inst);
        CsrBtGattAppHandleThermcCsrUiMenuUpdate(inst);
    }
}

void CsrBtGattAppHandleThermcCsrUiShowBattInfo(CsrBtThermcAppInstData *inst)
{
    CsrBtGattAppDbPrimServiceElement *pElem;
    CsrBtGattAppDbCharacElement *cElem;
    CsrBtGattAppDbCharacElement *cElem2;

    pElem  = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(inst->dbElement->dbPrimServices,CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_UUID_BATTERY_SERVICE));
    cElem  = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_APP_BATT_MASK_CHARAC_UUID));
    cElem2 = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_APP_BATT_SERVICE_REQUIRED_CHARAC_UUID));
    
    if(cElem != NULL && cElem->value != NULL && cElem2 != NULL && cElem2->value != NULL)
    {
        CsrUint32 retSize = 300;
        char *ret = CsrPmemZalloc(retSize);
        
        snprintf(ret, retSize, "Batt. present: %s\nDischarging: %s\nCharging: %s\nCritical: %s\nService Required: %s",
                    therm_present_substate[(CsrUint8)cElem->value[0] & CSR_BT_GATT_BATT_STATUS_MASK],
                    therm_discharging_substate[((CsrUint8)cElem->value[0]>>2) & CSR_BT_GATT_BATT_STATUS_MASK],
                    therm_charging_substate[((CsrUint8)cElem->value[0]>>4) & CSR_BT_GATT_BATT_STATUS_MASK],
                    therm_critically_low_level_substate[((CsrUint8)cElem->value[0]>>6) & CSR_BT_GATT_BATT_STATUS_MASK],
                    therm_service_required[(CsrUint8)cElem2->value[0] & CSR_BT_GATT_BATT_STATUS_MASK]);
        commonThermcPopupHide(inst);
        commonThermcAcceptPopupSet(inst, "Battery Info", ret);
        commonThermcAcceptPopupShow(inst);
        CsrBtGattAppHandleThermcCsrUiMenuUpdate(inst);
    }

            

}

void CsrBtGattAppHandleThermcCsrUiConnectDisconnect(CsrBtThermcAppInstData *inst, 
                                                   CsrBool connected, 
                                                   CsrBool success)
{
    CsrBtThermcAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(connected)
    {
        if(success)
        {
            CsrUint32 addrSize = 15;
            char *addr = CsrPmemAlloc(addrSize);
            snprintf(addr, addrSize, "%04X:%02X:%06X", inst->selectedDeviceAddr.addr.nap, inst->selectedDeviceAddr.addr.uap, inst->selectedDeviceAddr.addr.lap);
            /* Update main menu so it is no longer possible to connect and activate but just to disconnect and CsrBtThermcAppSend file */
            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_FIRST, 
                                    CSR_BT_THERMC_APP_DISCONNECT_KEY, 
                                    CSR_UI_ICON_DISCONNECT, 
                                    TEXT_THERMC_APP_DISCONNECT_UCS2, 
                                    CsrUtf82Utf16String((CsrUtf8String *) addr),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_THERMC_APP_WRITE_TEMPERATURE_KEY, 
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_THERMC_APP_TOGGLE_TEMPERATURE_UCS2,
                                    commonThermcGetValueString(inst, CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_THERMC_APP_BATTERY_INFO_KEY, 
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_THERMC_APP_BATTERY_INFO_UCS2, 
                                    commonThermcGetValueString(inst, CSR_BT_GATT_UUID_BATTERY_SERVICE),
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
                                CSR_BT_THERMC_APP_CONNECT_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_THERMC_APP_CONNECT_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_THERMC_APP_UNREGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_THERMC_APP_UNREGISTER_UCS2, 
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
        commonThermcPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}



void CsrBtGattAppHandleThermcSetMainMenuHeader(CsrBtThermcAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("Thermometer Cli"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtThermcAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtThermcAppUiInstData *csrUiVar = &inst->csrUiVar;

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
                                CSR_BT_THERMC_APP_REGISTER_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_THERMC_APP_REGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event,
                                csrUiVar->hSk2Event,
                                csrUiVar->hBackEvent, 0);

        csrUiVar->hMainMenu = confirm->handle;
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtThermcAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrBtThermcAppUiInstData *csrUiVar = &inst->csrUiVar;

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(csrUiVar->hSelectFileInputDialog);

    CsrPmemFree(confirm->text);

}

static void handleCsrUiUieEventInd(CsrBtThermcAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtThermcAppUiInstData *csrUiVar = &inst->csrUiVar;;

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
            case CSR_BT_THERMC_APP_REGISTER_KEY:
                {
                    commonThermcPopupSet(inst, "Registering", "Please wait");
                    commonThermcPopupShow(inst);
                    CsrBtThermcAppStartRegister(inst);
                    break;
                }
            case CSR_BT_THERMC_APP_UNREGISTER_KEY:
                {
                    commonThermcPopupSet(inst, "Unregistering", "Please wait");
                    commonThermcPopupShow(inst);
                    CsrBtThermcAppStartUnregister(inst);
                    break;
                }
            case CSR_BT_THERMC_APP_CONNECT_KEY:
                {
                    commonThermcPopupSet(inst, "Connecting", "Please wait");
                    commonThermcPopupShow(inst);
                    CsrBtThermcAppStartConnecting(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
            case CSR_BT_THERMC_APP_DISCONNECT_KEY:
                {
                    commonThermcPopupSet(inst, "Disconnecting", "Please wait");
                    commonThermcPopupShow(inst);
                    CsrBtThermcAppStartDisconnect(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
            case CSR_BT_THERMC_APP_WRITE_TEMPERATURE_KEY :
                {
                    if(inst->dbElement->traversingDb)
                    {
                        CsrBtGattAppHandleThermcCsrUiStillTraversingDb(inst);
                    }
                    else
                    {
                        CsrUint16 val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION; 
                        commonThermcPopupSet(inst, "Thermometer", "Toggling temperature notifications.");
                        commonThermcPopupShow(inst);
                        if(inst->tempNotif)
                        {
                            val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
                        }

                        CsrBtThermcAppStartWriteClientConfig(inst, CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE, CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC, val);
                    }
                    break;
                }
            case CSR_BT_THERMC_APP_BATTERY_INFO_KEY:
                {
                    if(inst->dbElement->traversingDb)
                    {
                        CsrBtGattAppHandleThermcCsrUiStillTraversingDb(inst);
                    }
                    else
                    {
                        /*we have already found the valuehandle */
                        CsrBtGattAppHandleThermcCsrUiShowBattInfo(inst);
                    }
                    break;
                }
            case CSR_BT_THERMC_APP_OK_KEY:
                {
                    commonThermcPopupHide(inst);
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


void CsrBtGattAppHandleThermcCsrUiPrim(CsrBtThermcAppInstData *inst)
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


