/****************************************************************************

Copyright (c) 2012-2014 Qualcomm Technologies International, Ltd. 
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
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_rsc_server_app.h"
#include "csr_bt_gatt_demo_rsc_server_app_strings.h"
#include "csr_bt_gatt_demo_rsc_server_db.h"

/* Helper functions */

void commonRscsPopupSet(CsrBtRscsAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonRscsStopPopupSet(CsrBtRscsAppInstData *inst, char *dialogHeading, char *dialogText)
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

void commonRscsAcceptPopupSet(CsrBtRscsAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_OK_UCS2,
                          NULL, 
                          inst->csrUiVar.hBackEvent,
                          0,
                          inst->csrUiVar.hBackEvent,
                          0);
}

void commonRscsPopupShow(CsrBtRscsAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_RSCS_APP_CSR_UI_PRI);
}

void commonRscsAcceptPopupShow(CsrBtRscsAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_RSCS_APP_CSR_UI_PRI);
}

void commonRscsPopupHide(CsrBtRscsAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}

void CsrBtGattAppHandleRscsCsrUiRegisterUnregister(CsrBtRscsAppInstData *inst, CsrBool registered)
{
    CsrBtRscsAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(registered)
    {
        /* Update main menu so it is no longer possible to activate but just to deactivate */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_FIRST,
                                CSR_BT_RSCS_APP_ACTIVATE_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCS_APP_ACTIVATE_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST,
                                CSR_BT_RSCS_APP_UNREGISTER_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCS_APP_UNREGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetcursorReqSend(csrUiVar->hMainMenu,CSR_BT_RSCS_APP_ACTIVATE_KEY);


        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_RSCS_APP_REGISTER_KEY);
    }
    else
    {
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);
        
        /* Update main menu so it is no longer possible to unRegister but just to register */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCS_APP_REGISTER_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCS_APP_REGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);

    }
    /* Remove popup */
    commonRscsPopupHide(inst);
}

void CsrBtGattAppHandleRscsCsrUiMenuUpdate(CsrBtRscsAppInstData *inst)
{
       /*Could be used to show more info in the menu */
}



void CsrBtGattAppHandleRscsCsrUiActivateDeactivate(CsrBtRscsAppInstData *inst,
                                                   CsrBool activated, 
                                                   CsrBool success)
{
    CsrBtRscsAppUiInstData *csrUiVar = &inst->csrUiVar;

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
                                    CSR_BT_RSCS_APP_DEACTIVATE_KEY,
                                    CSR_UI_ICON_DISCONNECT, 
                                    TEXT_RSCS_APP_DEACTIVATE_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *) addr),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_RSCS_APP_TOGGLE_RUNWALK_TYPE_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_RSCS_APP_TOGGLE_RUNWALK_TYPE_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.flags&0x04)?"Running":"Walking"))),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCS_APP_TOGGLE_MULTI_SENSOR_LOCATION_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION,
                                    TEXT_RSCS_APP_TOGGLE_MULTI_SENSOR_LOCATION_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.feature&CSR_BT_RSCS_APP_FEATURE_MULTI_SENSOR_LOCATION)?"On":"Off"))),
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent,
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCS_APP_TOGGLE_TOTAL_DISTANCE_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION,
                                    TEXT_RSCS_APP_TOGGLE_TOTAL_DISTANCE_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.feature&CSR_BT_RSCS_APP_FEATURE_TOTAL_DISTANCE)?"On":"Off"))),
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent,
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCS_APP_TOGGLE_SENSOR_CALIBRATION_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION,
                                    TEXT_RSCS_APP_TOGGLE_SENSOR_CALIBRATION_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.feature&CSR_BT_RSCS_APP_FEATURE_SENSOR_CALIBRATION)?"On":"Off"))),
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent,
                                    0);

            CsrUiMenuSetcursorReqSend(csrUiVar->hMainMenu,CSR_BT_RSCS_APP_DEACTIVATE_KEY);
        }
    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just to connect and activate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_FIRST,
                                CSR_BT_RSCS_APP_ACTIVATE_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCS_APP_ACTIVATE_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCS_APP_UNREGISTER_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCS_APP_UNREGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetcursorReqSend(csrUiVar->hMainMenu,CSR_BT_RSCS_APP_ACTIVATE_KEY);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;

        /* Remove popup */
        commonRscsPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}



void CsrBtGattAppHandleRscsSetMainMenuHeader(CsrBtRscsAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("RSC Srv"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtRscsAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtRscsAppUiInstData *csrUiVar = &inst->csrUiVar;

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
                                CSR_BT_RSCS_APP_REGISTER_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_RSCS_APP_REGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event,
                                csrUiVar->hSk2Event,
                                csrUiVar->hBackEvent, 0);

        csrUiVar->hMainMenu = confirm->handle;
    }
}

static void handleCsrUiInputDialogGetCfm(CsrBtRscsAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrBtRscsAppUiInstData *csrUiVar = &inst->csrUiVar;

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(csrUiVar->hSelectFileInputDialog);

    if (confirm->text)
    { /* De-allocate the text message to prevent a memory leak */
        CsrPmemFree(confirm->text);
    }

}

static void handleCsrUiUieEventInd(CsrBtRscsAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtRscsAppUiInstData *csrUiVar = &inst->csrUiVar;;

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
            CsrUiUieHideReqSend(indication->displayElement);
            CsrBtRscsAppStartDisconnect(inst);
        }
        else
        {
            switch(indication->key)
            {
                case CSR_BT_RSCS_APP_REGISTER_KEY:
                {
                    commonRscsPopupSet(inst, "Registering", "Please wait");
                    commonRscsPopupShow(inst);
                    CsrBtRscsAppStartRegister(inst);
                    break;
                }
                case CSR_BT_RSCS_APP_UNREGISTER_KEY:
                {
                    commonRscsPopupSet(inst, "Unregistering", "Please wait");
                    commonRscsPopupShow(inst);
                    CsrBtRscsAppStartUnregister(inst);
                    break;
                }
                case CSR_BT_RSCS_APP_ACTIVATE_KEY:
                {
                    commonRscsStopPopupSet(inst, "Advertising", "Please wait");
                    commonRscsAcceptPopupShow(inst);
                    CsrBtRscsAppStartActivate(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
                case CSR_BT_RSCS_APP_DEACTIVATE_KEY:
                {
                    commonRscsPopupSet(inst, "Deactivating", "Please wait");
                    commonRscsPopupShow(inst);
                    CsrBtRscsAppStartDeactivate(inst);
                    inst->csrUiVar.localInitiatedAction = TRUE;
                    break;
                }
                case CSR_BT_RSCS_APP_TOGGLE_RUNWALK_TYPE_KEY:
                {
                    if(inst->rscData.flags & 0x04) /*running*/
                    {
                        inst->rscData.flags = 0x03; /*walking*/
                    }
                    else 
                    {
                        inst->rscData.flags = 0x07; /*running*/
                    }
                    CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                            CSR_BT_RSCS_APP_TOGGLE_RUNWALK_TYPE_KEY,
                                            CSR_UI_ICON_MARK_INFORMATION, 
                                            TEXT_RSCS_APP_TOGGLE_RUNWALK_TYPE_UCS2,
                                            CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.flags&0x04)?"Running":"Walking"))),
                                            csrUiVar->hSk1Event, 
                                            csrUiVar->hSk2Event, 
                                            csrUiVar->hBackEvent, 
                                            0);

                    break;
                }
                case CSR_BT_RSCS_APP_TOGGLE_MULTI_SENSOR_LOCATION_KEY:
                {

                    inst->rscData.feature ^= CSR_BT_RSCS_APP_FEATURE_MULTI_SENSOR_LOCATION; /*Toggle*/

                    CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu,
                                            CSR_BT_RSCS_APP_TOGGLE_MULTI_SENSOR_LOCATION_KEY,
                                            CSR_UI_ICON_MARK_INFORMATION,
                                            TEXT_RSCS_APP_TOGGLE_MULTI_SENSOR_LOCATION_UCS2,
                                            CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.feature&CSR_BT_RSCS_APP_FEATURE_MULTI_SENSOR_LOCATION)?"On":"Off"))),
                                            csrUiVar->hSk1Event,
                                            csrUiVar->hSk2Event,
                                            csrUiVar->hBackEvent,
                                            0);

                    break;
                }
                case CSR_BT_RSCS_APP_TOGGLE_TOTAL_DISTANCE_KEY:
                {

                    inst->rscData.feature ^= CSR_BT_RSCS_APP_FEATURE_TOTAL_DISTANCE; /*Toggle*/
                    inst->rscData.flags   ^= 0x02; /*distance flag*/

                    CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu,
                                            CSR_BT_RSCS_APP_TOGGLE_TOTAL_DISTANCE_KEY,
                                            CSR_UI_ICON_MARK_INFORMATION,
                                            TEXT_RSCS_APP_TOGGLE_TOTAL_DISTANCE_UCS2,
                                            CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.feature&CSR_BT_RSCS_APP_FEATURE_TOTAL_DISTANCE)?"On":"Off"))),
                                            csrUiVar->hSk1Event,
                                            csrUiVar->hSk2Event,
                                            csrUiVar->hBackEvent,
                                            0);

                    break;
                }
                case CSR_BT_RSCS_APP_TOGGLE_SENSOR_CALIBRATION_KEY:
                {

                    inst->rscData.feature ^= CSR_BT_RSCS_APP_FEATURE_SENSOR_CALIBRATION; /*Toggle*/

                    CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu,
                                            CSR_BT_RSCS_APP_TOGGLE_SENSOR_CALIBRATION_KEY,
                                            CSR_UI_ICON_MARK_INFORMATION,
                                            TEXT_RSCS_APP_TOGGLE_SENSOR_CALIBRATION_UCS2,
                                            CsrUtf82Utf16String((CsrUtf8String *)CsrStrDup(((inst->rscData.feature&CSR_BT_RSCS_APP_FEATURE_SENSOR_CALIBRATION)?"On":"Off"))),
                                            csrUiVar->hSk1Event,
                                            csrUiVar->hSk2Event,
                                            csrUiVar->hBackEvent,
                                            0);

                    break;
                }

                case CSR_BT_RSCS_APP_OK_KEY:
                {
                    commonRscsPopupHide(inst);
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


void CsrBtGattAppHandleRscsCsrUiPrim(CsrBtRscsAppInstData *inst)
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


