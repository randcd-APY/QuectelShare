/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_prox_client_app.h"
#include "csr_bt_gatt_demo_prox_client_app_strings.h"
#include "csr_bt_gatt_demo_defines.h"


static char *prox_present_substate[]={"Not Supported", "Unknown", "Not Present", "Present"};
static char *prox_discharging_substate[]={"Not Supported", "Unknown", "Not Discharging", "Discharging"};
static char *prox_charging_substate[]={"Unknown", "Not Chargeable", "Not Charging (Chargeable)", "Charging (Chargeable)"};
static char *prox_critically_low_level_substate[]={"Not Supported", "Unknown", "Good Level", "Critically Low Level"};
static char *prox_service_required[]={"Unknown","No Service Required","Service Required"};

/* Helper functions */

CsrUtf16String *commonProxcGetValueString(CsrBtProxcAppInstData *inst, CsrUint16 uuid)
{

    CsrBtGattAppDbPrimServiceElement *pElem;
    CsrUtf16String *ret16  =  NULL;
    char *ret              =  NULL;

    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(inst->dbElement->dbPrimServices,CsrBtProxcAppUuid16ToUuid128(uuid));
    if(pElem != NULL)
    {
        switch(uuid)
        {
            case CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE: 
                {
                    CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                                CsrBtProxcAppUuid16ToUuid128(CSR_BT_GATT_UUID_ALERT_LEVEL_CHARAC));
                    if(cElem != NULL && cElem->value == NULL)
                    {
                        /*fix as we know that linkloss and iAlert values are write-only but default 0x00*/
                        cElem->value = CsrPmemZalloc(sizeof(CsrUint8));
                        cElem->valueLength = sizeof(CsrUint8);

                    }
                    if(cElem != NULL && cElem->value != NULL)
                    {
                        switch((CsrUint8)cElem->value[0])
                        {
                            case CSR_BT_GATT_APP_ALERT_LEVEL_OFF:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Alarm: Off"));            
                                }
                            case CSR_BT_GATT_APP_ALERT_LEVEL_LOW:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Value: Low"));
                                }
                            case CSR_BT_GATT_APP_ALERT_LEVEL_HIGH:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Value: High"));
                                }
                            default:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Value: Off")); /* the field is write-only but default is off */
                                }
                        }
                    }
                    break;
                }
            case CSR_BT_GATT_UUID_LINK_LOSS_SERVICE:
                {
                    CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                                CsrBtProxcAppUuid16ToUuid128(CSR_BT_GATT_UUID_ALERT_LEVEL_CHARAC));

                    if(cElem != NULL && cElem->value == NULL)
                    {
                        /*fix as we know that linkloss and iAlert values are write-only but default 0x00*/
                        cElem->value = CsrPmemZalloc(sizeof(CsrUint8));
                        cElem->valueLength = sizeof(CsrUint8);

                    }
                    if(cElem != NULL && cElem->value != NULL)
                    {
                        switch((CsrUint8)cElem->value[0])
                        {
                            case CSR_BT_GATT_APP_ALERT_LEVEL_OFF:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Value: Off"));            
                                }
                            case CSR_BT_GATT_APP_ALERT_LEVEL_LOW:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Value: Low"));
                                }
                            case CSR_BT_GATT_APP_ALERT_LEVEL_HIGH:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Value: High"));
                                }
                            default:
                                {
                                    return CsrUtf82Utf16String((CsrUtf8String *)("Unknown"));
                                }
                        }
                    } 
                    break;
                }

        case CSR_BT_GATT_UUID_TX_POWER_SERVICE:
            {
                CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                            CsrBtProxcAppUuid16ToUuid128(CSR_BT_GATT_UUID_TX_POWER_LEVEL_CHARAC));
                if(cElem != NULL && cElem->value != NULL)
                {
                    CsrUint32 retSize = 20;
                    ret =  CsrPmemAlloc(retSize);
                    snprintf(ret, retSize, "TX: %d RSSI: %d", (CsrInt8)cElem->value[0], (CsrInt8)inst->rssiLevel);
                    ret16 = CsrUtf82Utf16String((CsrUtf8String *)ret);
                    CsrPmemFree(ret);
                    return ret16;
                }
                break;
            }
        case CSR_BT_GATT_UUID_BATTERY_SERVICE:
            {
                CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                            CsrBtProxcAppUuid16ToUuid128(CSR_BT_GATT_UUID_BATTERY_LEVEL_CHARAC));
                if(cElem != NULL && cElem->value != NULL)
                {
                    CsrUint32 retSize = 12;
                    ret = CsrPmemAlloc(retSize);
                    snprintf(ret, retSize, "Level %d%%", (CsrUint8)cElem->value[0]);
                    ret16 =  CsrUtf82Utf16String((CsrUtf8String *)ret);
                    CsrPmemFree(ret);
                    return ret16;
                }
                break;

            }
        default:
            {
                return CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup("Unknown UUID"));
            }
        }
    }
        if(inst->dbElement->traversingDb)
        {
            return CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup("Loading..."));
        }
        else 
        {
            return CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup("Not Found"));
        }

}

void commonProxcPopupSet(CsrBtProxcAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonProxcAcceptPopupSet(CsrBtProxcAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_PROXC_APP_OK_UCS2, 
                          NULL, 
                          inst->csrUiVar.hBackEvent,
                          0,
                          inst->csrUiVar.hBackEvent,
                          0);
}

void commonProxcPopupShow(CsrBtProxcAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_PROXC_APP_CSR_UI_PRI);
}

void commonProxcAcceptPopupShow(CsrBtProxcAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_PROXC_APP_CSR_UI_PRI);
}

void commonProxcPopupHide(CsrBtProxcAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}

void CsrBtGattAppHandleProxcCsrUiRegisterUnregister(CsrBtProxcAppInstData *inst, CsrBool registered)
{
    CsrBtProxcAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(registered)
    {
        /* Update main menu so it is no longer possible to activate but just to deactivate */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_PROXC_APP_UNREGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_PROXC_APP_UNREGISTER_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_FIRST, 
                                CSR_BT_PROXC_APP_CONNECT_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_PROXC_APP_CONNECT_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);


        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_PROXC_APP_REGISTER_KEY);
    }
    else
    {
        /* Update main menu so it is no longer possible to unRegister but just to register */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_PROXC_APP_REGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_PROXC_APP_REGISTER_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_PROXC_APP_UNREGISTER_KEY);
        if(inst->btConnId == 0xff)
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_PROXC_APP_CONNECT_KEY);
        }
        else
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_PROXC_APP_DISCONNECT_KEY);
        }

    }
    /* Remove popup */
    commonProxcPopupHide(inst);
}

void CsrBtGattAppHandleProxcCsrUiMenuUpdate(CsrBtProxcAppInstData *inst)
{

    /*to prevent a panic in UI task */
    if(inst->btConnId != 0xff)
    {
        CsrBtProxcAppUiInstData *csrUiVar = &inst->csrUiVar;
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_PROXC_APP_WRITE_LINKLOSS_KEY, 
                                CSR_UI_ICON_LOUDSPEAKER, 
                                TEXT_PROXC_APP_WRITE_LINKLOSS_UCS2, 
                                commonProxcGetValueString(inst, CSR_BT_GATT_UUID_LINK_LOSS_SERVICE),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_PROXC_APP_WRITE_IMMEDIATE_ALERT_KEY, 
                                CSR_UI_ICON_BELL, 
                                TEXT_PROXC_APP_WRITE_IMMEDIATE_ALERT_UCS2, 
                                commonProxcGetValueString(inst, CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_PROXC_APP_READ_TX_POWER_KEY, 
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_PROXC_APP_READ_TX_POWER_UCS2, 
                                commonProxcGetValueString(inst, CSR_BT_GATT_UUID_TX_POWER_SERVICE),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_PROXC_APP_BATTERY_INFO_KEY, 
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_PROXC_APP_BATTERY_INFO_UCS2, 
                                commonProxcGetValueString(inst, CSR_BT_GATT_UUID_BATTERY_SERVICE),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
    }

}

void CsrBtGattAppHandleProxcCsrUiNoSelectedDevAddr(CsrBtProxcAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonProxcPopupHide(inst);
    commonProxcAcceptPopupSet(inst, "Connecting", "No deviceAddr selected in GAP");
    commonProxcAcceptPopupShow(inst);
}

void CsrBtGattAppHandleProxcCsrUiStillTraversingDb(CsrBtProxcAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonProxcPopupHide(inst);
    commonProxcAcceptPopupSet(inst, "Warning", "Client is still traversing the remote database. please wait a few seconds and try again!");
    commonProxcAcceptPopupShow(inst);
}


void CsrBtGattAppHandleProxcCsrUiReadValue(CsrBtProxcAppInstData *inst, char *str, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonProxcPopupHide(inst);
        CsrBtGattAppHandleProxcCsrUiMenuUpdate(inst);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonProxcPopupHide(inst);
        commonProxcAcceptPopupSet(inst, "Reading", "Could not read value");
        commonProxcAcceptPopupShow(inst);
        CsrBtGattAppHandleProxcCsrUiMenuUpdate(inst);
    }
}
void CsrBtGattAppHandleProxcCsrUiWriteValue(CsrBtProxcAppInstData *inst, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button */
        commonProxcPopupHide(inst);
        CsrBtGattAppHandleProxcCsrUiMenuUpdate(inst);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonProxcPopupHide(inst);
        commonProxcAcceptPopupSet(inst, "Writing", "Could not set value");
        commonProxcAcceptPopupShow(inst);
        CsrBtGattAppHandleProxcCsrUiMenuUpdate(inst);
    }
}

void CsrBtGattAppHandleProxcCsrUiShowBattInfo(CsrBtProxcAppInstData *inst)
{
    CsrBtGattAppDbPrimServiceElement *pElem;
    CsrBtGattAppDbCharacElement *cElem;
    CsrBtGattAppDbCharacElement *cElem2;

    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(inst->dbElement->dbPrimServices,CsrBtProxcAppUuid16ToUuid128(CSR_BT_GATT_UUID_BATTERY_SERVICE));
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, CsrBtProxcAppUuid16ToUuid128(CSR_BT_GATT_APP_BATT_MASK_CHARAC_UUID));
    cElem2 = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, CsrBtProxcAppUuid16ToUuid128(CSR_BT_GATT_APP_BATT_SERVICE_REQUIRED_CHARAC_UUID));

    if(cElem != NULL && cElem->value != NULL && cElem2 != NULL && cElem2->value != NULL)
    {
        CsrUint32 retSize = 300;
        char *ret = CsrPmemZalloc(retSize);
        snprintf(ret, retSize, "Batt. present: %s\nDischarging: %s\nCharging: %s\nCritical: %s\nService Required: %s",
                    prox_present_substate[(CsrUint8)cElem->value[0] & CSR_BT_GATT_BATT_STATUS_MASK],
                    prox_discharging_substate[((CsrUint8)cElem->value[0]>>2) & CSR_BT_GATT_BATT_STATUS_MASK],
                    prox_charging_substate[((CsrUint8)cElem->value[0]>>4) & CSR_BT_GATT_BATT_STATUS_MASK],
                    prox_critically_low_level_substate[((CsrUint8)cElem->value[0]>>6) & CSR_BT_GATT_BATT_STATUS_MASK],
                    prox_service_required[(CsrUint8)cElem2->value[0] & CSR_BT_GATT_BATT_STATUS_MASK]);

        commonProxcPopupHide(inst);
        commonProxcAcceptPopupSet(inst, "Battery Info", ret);
        commonProxcAcceptPopupShow(inst);
        CsrBtGattAppHandleProxcCsrUiMenuUpdate(inst);
    }

}

void CsrBtGattAppHandleProxcCsrUiConnectDisconnect(CsrBtProxcAppInstData *inst, 
                                                   CsrBool connected, 
                                                   CsrBool success)
{
    CsrBtProxcAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(connected)
    {
        if(success)
        {
            CsrUint32 addrSize = 15;
            char *addr = CsrPmemAlloc(addrSize);
            snprintf(addr, addrSize, "%04X:%02X:%06X", inst->selectedDeviceAddr.addr.nap, inst->selectedDeviceAddr.addr.uap, inst->selectedDeviceAddr.addr.lap);
            /* Update main menu so it is no longer possible to connect and activate but just to disconnect and CsrBtProxcAppSend file */
            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_FIRST, 
                                    CSR_BT_PROXC_APP_DISCONNECT_KEY, 
                                    CSR_UI_ICON_DISCONNECT, 
                                    TEXT_PROXC_APP_DISCONNECT_UCS2, 
                                    CsrUtf82Utf16String((CsrUtf8String *) addr),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_PROXC_APP_WRITE_LINKLOSS_KEY, 
                                    CSR_UI_ICON_LOUDSPEAKER, 
                                    TEXT_PROXC_APP_WRITE_LINKLOSS_UCS2, 
                                    commonProxcGetValueString(inst, CSR_BT_GATT_UUID_LINK_LOSS_SERVICE),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_PROXC_APP_WRITE_IMMEDIATE_ALERT_KEY, 
                                    CSR_UI_ICON_BELL, 
                                    TEXT_PROXC_APP_WRITE_IMMEDIATE_ALERT_UCS2, 
                                    commonProxcGetValueString(inst, CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_PROXC_APP_READ_TX_POWER_KEY, 
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_PROXC_APP_READ_TX_POWER_UCS2,
                                    commonProxcGetValueString(inst, CSR_BT_GATT_UUID_TX_POWER_SERVICE),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_PROXC_APP_BATTERY_INFO_KEY, 
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_PROXC_APP_BATTERY_INFO_UCS2, 
                                    commonProxcGetValueString(inst, CSR_BT_GATT_UUID_BATTERY_SERVICE),
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
                                CSR_BT_PROXC_APP_CONNECT_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_PROXC_APP_CONNECT_UCS2, 
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_PROXC_APP_UNREGISTER_KEY, 
                                CSR_UI_ICON_NONE, 
                                TEXT_PROXC_APP_UNREGISTER_UCS2, 
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
        commonProxcPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}



void CsrBtGattAppHandleProxcSetMainMenuHeader(CsrBtProxcAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("Proximity Cli"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtProxcAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtProxcAppUiInstData *csrUiVar = &inst->csrUiVar;

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
                                CSR_BT_PROXC_APP_REGISTER_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_PROXC_APP_REGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event,
                                csrUiVar->hSk2Event,
                                csrUiVar->hBackEvent, 0);

        csrUiVar->hMainMenu = confirm->handle;
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtProxcAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrBtProxcAppUiInstData *csrUiVar = &inst->csrUiVar;

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(csrUiVar->hSelectFileInputDialog);

    CsrPmemFree(confirm->text);

}

static void handleCsrUiUieEventInd(CsrBtProxcAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtProxcAppUiInstData *csrUiVar = &inst->csrUiVar;;

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
            case CSR_BT_PROXC_APP_REGISTER_KEY:
                {
                    commonProxcPopupSet(inst, "Registering", "Please wait");
                    commonProxcPopupShow(inst);
                    CsrBtProxcAppStartRegister(inst);
                    break;
                }
            case CSR_BT_PROXC_APP_UNREGISTER_KEY:
                {
                    commonProxcPopupSet(inst, "Unregistering", "Please wait");
                    commonProxcPopupShow(inst);
                    CsrBtProxcAppStartUnregister(inst);
                    break;
                }
            case CSR_BT_PROXC_APP_CONNECT_KEY:
                {
                    commonProxcPopupSet(inst, "Connecting", "Please wait");
                    commonProxcPopupShow(inst);
                    CsrBtProxcAppStartConnecting(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
            case CSR_BT_PROXC_APP_DISCONNECT_KEY:
                {
                    commonProxcPopupSet(inst, "Disconnecting", "Please wait");
                    commonProxcPopupShow(inst);
                    CsrBtProxcAppStartDisconnect(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
            case CSR_BT_PROXC_APP_WRITE_LINKLOSS_KEY:
                {
                    if(inst->dbElement->traversingDb)
                    {
                        CsrBtGattAppHandleProxcCsrUiStillTraversingDb(inst);
                    }
                    else
                    {
                        /*we have already found the valuehandle */
                        CsrBtProxcAppStartWriteValue(inst, CSR_BT_GATT_UUID_LINK_LOSS_SERVICE, CSR_BT_GATT_UUID_ALERT_LEVEL_CHARAC);
                    }
                    break;
                }
            case CSR_BT_PROXC_APP_WRITE_IMMEDIATE_ALERT_KEY :
                {
                    if(inst->dbElement->traversingDb)
                    {
                        CsrBtGattAppHandleProxcCsrUiStillTraversingDb(inst);
                    }
                    else
                    {
                        commonProxcPopupSet(inst, "Alert", "Telling peer to alert.");
                        commonProxcPopupShow(inst);
                        CsrBtProxcAppStartWriteValue(inst, CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE, CSR_BT_GATT_UUID_ALERT_LEVEL_CHARAC);
                    }
                    break;
                }
            case CSR_BT_PROXC_APP_READ_TX_POWER_KEY:
                {
                    if(inst->dbElement->traversingDb)
                    {
                        CsrBtGattAppHandleProxcCsrUiStillTraversingDb(inst);
                    }
                    else
                    {
                     /* done automatically during traversal now  CsrBtProxcAppStartReadValue(inst, CSR_BT_GATT_UUID_TX_POWER_SERVICE, CSR_BT_GATT_UUID_TX_POWER_LEVEL_CHARAC);*/
                    }
                    break;
                }
            case CSR_BT_PROXC_APP_BATTERY_INFO_KEY:
                {
                    if(inst->dbElement->traversingDb)
                    {
                        CsrBtGattAppHandleProxcCsrUiStillTraversingDb(inst);
                    }
                    else
                    {
                        /*we have already found the valuehandle */
                        CsrBtGattAppHandleProxcCsrUiShowBattInfo(inst);
                    }
                    break;
                }
            case CSR_BT_PROXC_APP_OK_KEY:
                {
                    commonProxcPopupHide(inst);
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


void CsrBtGattAppHandleProxcCsrUiPrim(CsrBtProxcAppInstData *inst)
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


