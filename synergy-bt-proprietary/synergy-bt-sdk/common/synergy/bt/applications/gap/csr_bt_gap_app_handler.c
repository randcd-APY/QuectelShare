/******************************************************************************

Copyright (c) 2009-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include <stdio.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_arg_search.h"
#include "csr_app_main.h"

#include "csr_bt_gap_app_ui_sef.h"
#include "csr_bt_gap_app_prim.h"
#include "csr_bt_gap_app_handler.h"
#include "csr_bt_gap_app_util.h"
#include "csr_bt_gap_app_sd.h"
#include "csr_bt_gap_app_sc.h"
#include "csr_bt_gap_app_cm.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_util.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_sc_demo_app.h"

BD_ADDR_T defGlobalBdAddr;
CsrUint8  defGlobalPinLen;
CsrUint8  defGlobalPin[CSR_BT_PASSKEY_MAX_LEN];

#define PROFILE_GAP CsrUtf8StrDup((const CsrUtf8String *) "GAP")

static const CsrBtGapEventType csrUiMsgHandlers[CSR_UI_PRIM_UPSTREAM_COUNT] =
{ /* Jump tabel to handle Upstream CSR_UI messages send from the Phone Emulator UI (CSR_UI)     */
    CsrBtGapCsrUiUieCreateCfmHandler,               /* CSR_UI_UIE_CREATE_CFM                  */
    NULL,                                           /* CSR_UI_MENU_GET_CFM                    */
    NULL,                                           /* CSR_UI_MENU_GETITEM_CFM                */
    NULL,                                           /* CSR_UI_MENU_GETCURSOR_CFM              */
    NULL,                                           /* CSR_UI_EVENT_GET_CFM                   */
    NULL,                                           /* CSR_UI_DIALOG_GET_CFM                  */
    CsrBtGapCsrUiInputdialogGetCfmHandler,          /* CSR_UI_INPUTDIALOG_GET_CFM             */
    NULL,                                           /* CSR_UI_IDLESCREEN_GET_CFM              */
    NULL,                                           /* CSR_UI_DISPLAY_GETHANDLE_CFM           */
    NULL,                                           /* CSR_UI_KEYDOWN_IND                     */
    CsrBtGapCsrUiEventIndHandler,                   /* CSR_UI_EVENT_IND                       */
};


static void initAppCsrUiInitDisplayesHandlers(CsrBtGapCsrUiType *csrUiVar)
{
    CsrUint8 i;

    for (i = 0; i < NUMBER_OF_CSR_BT_GAP_APP_UI; i++)
    {
        csrUiVar->displayesHandlers[i].displayHandle                 = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].sk1EventHandle                = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].sk2EventHandle                = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].backEventHandle               = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].deleteEventHandle             = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].sk1EventHandleFunc            = NULL;
        csrUiVar->displayesHandlers[i].sk2EventHandleFunc            = NULL;
        csrUiVar->displayesHandlers[i].deleteEventHandleFunc         = NULL;
        csrUiVar->displayesHandlers[i].backEventHandleFunc           = NULL;
    }
}

static void initAppCsrUiVar(CsrBtGapCsrUiType *csrUiVar)
{ /* Initialize the csrUiVar under inst                                                  */
    csrUiVar->eventState     = CSR_BT_GAP_CREATE_SK1_EVENT;
    csrUiVar->uiIndex        = CSR_BT_GAP_DEFAULT_INFO_UI;
    csrUiVar->popupTimerId   = 0;
    initAppCsrUiInitDisplayesHandlers(csrUiVar);
}

static void initAppCmVar(CsrBtGapCmInstData *cmVar)
{ /* Initialize the csrUiVar under inst                                                  */
    CsrBtCmReadLocalVersionReqSend(CsrSchedTaskQueueGet());
    CsrBtCmReadLocalBdAddrReqSend(CsrSchedTaskQueueGet());
#ifdef CSR_BT_INSTALL_CM_READ_LOCAL_NAME
    CsrBtCmReadLocalNameReqSend(CsrSchedTaskQueueGet());
#endif
}

static void initAppSdVar(CsrBtGapSdInstData *sdVar)
{
    sdVar->searchConfig = CSR_BT_SD_SEARCH_SHOW_UNKNOWN_DEVICE_NAMES;
    /* | CSR_BT_SD_SEARCH_CONTINUE_AFTER_RSSI_SORTING; Do not set the continue flag!
    Since we clear the UI when we restart the search, we wont get any results.*/
}


/* task functions */
void CsrBtGapInit(void **gash)
{
    CsrBtGapInstData *inst;
    CsrUint32 addr[6];
    CsrUint32 majorDeviceMask,minorDeviceMask;
    CsrCharString *parameter, *value;

#ifdef CSR_USE_QCA_CHIP
    /* QC chips does not have a default name and there is no VC cmd to 
        set the local name other than the standard HCI cmd */
    CsrUtf8String   *localDeviceName;

    if ((localDeviceName = (CsrUtf8String *)CsrAppMainBluecoreLocalDeviceNameGet()) != NULL) 
    {
        CsrBtCmSetLocalNameReqSend(CsrSchedTaskQueueGet(), CsrUtf8StrDup(localDeviceName));
    }
    else
    {
        CsrBtCmSetLocalNameReqSend(CsrSchedTaskQueueGet(), CsrUtf8StrDup((const CsrUtf8String *)"QSynergyBT"));
    } 
#endif

    inst = (CsrBtGapInstData *) CsrPmemZalloc(sizeof(CsrBtGapInstData));
    *gash = (void *) inst;

    CsrMemSet(inst, 0, sizeof(CsrBtGapInstData));

    initAppCsrUiVar(&inst->csrUiVar);
    initAppCmVar(&inst->cmVar);
    initAppSdVar(&inst->sdVar);

    if (CsrArgSearch(NULL, "--bt-remote-address", &parameter, &value) && (value != NULL) &&
        (sscanf(value, "%2x%2x:%2x:%2x%2x%2x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]) == 6))
    {
        defGlobalBdAddr.nap = (addr[0] << 8) | addr[1];
        defGlobalBdAddr.uap = (CsrUint8)addr[2];
        defGlobalBdAddr.lap = (addr[3] << 16) | (addr[4] << 8) | addr[5];
    }

    if (CsrArgSearch(NULL, "--bt-pin-code", &parameter, &value) && (value != NULL))
    {
        defGlobalPinLen = (CsrUint8)CsrStrLen(value);
        if (defGlobalPinLen > CSR_BT_PASSKEY_MAX_LEN)
        {
            defGlobalPinLen = CSR_BT_PASSKEY_MAX_LEN;
        }
        CsrMemCpy(defGlobalPin, value, defGlobalPinLen);
    }
    CsrBtBdAddrCopy(&inst->selectedDeviceAddr, &defGlobalBdAddr);

    CsrBtScActivateReqSend(CsrSchedTaskQueueGet());
    inst->scVar.proximityPairing = FALSE;
    inst->scVar.authRequirements = CSR_BT_DEFAULT_AUTH_REQUIREMENTS;
    inst->proximityPairingHandle = 0xFFFF;
    inst->performBonding         = FALSE;

    /* Start creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    CsrUiStatusBatterySetReqSend(100);
    CsrUiStatusRadiometerSetReqSend(100);

    /* Set Class Of Device */
#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
    if (CsrArgSearch(NULL, "--bt-dev", &parameter, &value) && (value != NULL))
    {
        if (!CsrStrCmp(value, "phone"))
        {
            CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(), CSR_BT_PHONE_MAJOR_DEVICE_MASK, CSR_BT_CELLULAR_MINOR_DEVICE_MASK);
        }
        else
        {
            CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(), CSR_BT_AV_MAJOR_DEVICE_MASK, CSR_BT_HANDSFREE_AV_MINOR_DEVICE_CLASS);
        }
    }
    else
    {
        /* Default as carkit(handsfree) device. */
        CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(), CSR_BT_AV_MAJOR_DEVICE_MASK, CSR_BT_HANDSFREE_AV_MINOR_DEVICE_CLASS);
    }
#else
    if (CsrArgSearch(NULL, "--bt-cod", &parameter, &value) && (value != NULL))

    {
        sscanf(value, "%6x,%6x", &majorDeviceMask, &minorDeviceMask);
        CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(),majorDeviceMask, minorDeviceMask);
    }
    else
    {
        CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(),CSR_BT_AV_MAJOR_DEVICE_MASK, CSR_BT_HANDSFREE_AV_MINOR_DEVICE_CLASS);
    }
#endif

    CsrAppRegisterReqSend(CsrSchedTaskQueueGet(), TECH_BT, PROFILE_GAP);
#ifdef CSR_BT_LE_ENABLE
    CsrAppRegisterReqSend(CsrSchedTaskQueueGet(), TECH_BTLE, PROFILE_GAP);
#endif

}

void CsrBtGapDeinit(void **gash)
{
    CsrBtGapInstData *inst;
    inst = *gash;

    CsrPmemFree(inst);
}

void CsrBtGapHandler(void **gash)
{
    CsrBtGapInstData *inst;
    CsrUint16 event=0;

    inst = (CsrBtGapInstData *) *gash;

    /* Obtains a message from the CSR_BT_GAP_APP_IFACEQUEUE                                         */
    if(!inst->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&event , &(inst->recvMsgP));
    }
    else
    {
        if(!CsrMessageQueuePop(&inst->saveQueue, &event , &(inst->recvMsgP)))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
             a message from the scheduler                                                   */
            inst->restoreFlag = FALSE;
            CsrSchedMessageGet(&event , &(inst->recvMsgP));
        }
        else
        { /* A message has been restored from the local queed                               */
            ;
        }
    }

    switch (event)
    {
        case CSR_APP_PRIM:
        {
            CsrPrim *type = inst->recvMsgP;

            if (*type == CSR_APP_TAKE_CONTROL_IND)
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_GAP, FALSE, "Received Take Control Ind");
                CsrBtGapShowUi(inst, CSR_BT_GAP_MAIN_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
            }
            else if (*type == CSR_APP_REGISTER_CFM)
            { /* Just ignore this primitive since we only register one menu item we don't need to remember the controlId */
                ;
            }
            break;
        }
        case CSR_BT_GAP_APP_PRIM:
        {
            CsrPrim *type = inst->recvMsgP;

            if (*type == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_REQ)
            {
                CsrBtGapAppGetSelectedDeviceReq *prim = inst->recvMsgP;
                CsrBtGapAppGetSelectedDeviceCfm *cfm = pnew(CsrBtGapAppGetSelectedDeviceCfm);

                CsrAppBacklogReqSend(TECH_BT, PROFILE_GAP, FALSE, "Task (%d) asked for selected device addr returning (%04X:%02X:%06X), devStatus (0x%x) ",prim->phandle,inst->selectedDeviceAddr.nap,inst->selectedDeviceAddr.uap,inst->selectedDeviceAddr.lap, inst->selectedDeviceStatus);

                cfm->type = CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM;
                CsrBtBdAddrCopy(&cfm->deviceAddr, &inst->selectedDeviceAddr);
                cfm->addressType  = inst->selectedDeviceAddrType;
                cfm->deviceStatus = inst->selectedDeviceStatus,
                CsrSchedMessagePut(prim->phandle,CSR_BT_GAP_APP_PRIM,cfm);
            }
            else if (*type == CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_REQ)
            {/* Start proximity search operation and pair afterwards */
                CsrBtGapAppPairProximityDeviceReq *prim = inst->recvMsgP;
                inst->proximityPairingHandle = prim->phandle;
                inst->performBonding         = prim->performBonding;
                inst->sdVar.state = CSR_BT_GAP_SD_STATE_SEARCHING;
                inst->devicesInSearchList = 0;
                CsrBtGapRemoveDeviceInKeyList(&inst->searchList);
                CsrBtGapMenuRemoveAllItems(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI);
                inst->scVar.proximityPairing = TRUE;
                CsrBtSdProximitySearchReqSendEx(CsrSchedTaskQueueGet(),
                                                inst->sdVar.searchConfig, /* searchConfiguration */
                                                4000, /* RSSI buffer timeout */
                                                0, /* Search timeout */
                                                DEFAULT_PP_RSSI_LEVEL, /* RSSI threshold */
                                                prim->classOfDevice, /* Class of Device */
                                                prim->classOfDeviceMask, /* Class of Device mask */
                                                CSR_BT_SD_ACCESS_CODE_GIAC, /* Access code */
                                                0, /* Filter length */
                                                NULL, /* Filter */
                                                DEFAULT_PP_INQ_TX_POWER, /* Inquiry Transmit power level */
                                                DEFAULT_PP_LE_RSSI_LEVEL); /* RSSI threshold */
            }
            break;
        }
        case CSR_BT_SC_PRIM:
        { /* Received an upstream message which is defined in sc_prim.h                     */
            CsrBtScAppEventHandler(inst, inst->recvMsgP, CsrBtGapCsrBtScAppEventFunctionsPtr);
            break;
        }
        case CSR_BT_SD_PRIM:
        { /* Received an upstream message which is defined in sd_prim.h                     */
            CsrBtSdAppEventHandler(inst, inst->recvMsgP, CsrBtGapCsrBtSdAppEventFunctionsPtr);
            break;
        }
        case CSR_BT_CM_PRIM:
        { /* Received an upstream message which is defined in cm_prim.h                     */
            CsrBtCmAppEventHandler(inst, inst->recvMsgP, CsrBtGapCsrBtCmAppEventFunctionsPtr);
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrUiPrim * primType = (CsrUiPrim *) inst->recvMsgP;
            CsrUint16   index        = (CsrUint16)(*primType - CSR_UI_PRIM_UPSTREAM_LOWEST);

            if (index < CSR_UI_PRIM_UPSTREAM_COUNT && csrUiMsgHandlers[index] != NULL)
            { /* Call the state event function which handle this incoming CSR_UI event message*/
                csrUiMsgHandlers[index](inst);
            }
            else
            { /* An unexpected CSR_UI message is received                                     */
                CsrGeneralException("CSR_APP", 0, *primType,
                                    "Did not handle CSR_UI Upstream message");
            }
            break;
        }
    }

    CsrPmemFree(inst->recvMsgP);
}

void CsrBtGapUsage(void)
{
    printf ("--bt-remote-address XXXX:XX:XXXXXX - Remote BT address\n");
    printf ("--bt-pin-code <digits> - PIN code to use for automatic pairing (up to 16 digits)\n");
    printf ("--bt-cod <XXXXXX,XXXXXX> - Class of device, define major and minor device masks according to www.bluetooth.org/Technical/AssignedNumbers/baseband.htm - (000200,000004)\n");
}
