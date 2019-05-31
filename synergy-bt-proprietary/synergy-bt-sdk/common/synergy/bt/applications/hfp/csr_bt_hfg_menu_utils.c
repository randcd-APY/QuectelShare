/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_formatted_io.h"
#include "csr_bt_tasks.h"
#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "csr_bt_hfg_lib.h"
#include "csr_bt_hfg_prim.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_sc_prim.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_util.h"
#include "csr_hci_sco.h"
#include "csr_bt_platform.h"
#include "csr_bt_hfg_demo_app.h"
#include "csr_bt_sc_demo_app.h"
#include "csr_bccmd_lib.h"
#include "csr_sched_init.h"
#include "csr_bt_hfg_app_util.h"
#include "csr_bt_hfg_menu_utils.h"

/* Defines for PTS RF-shielding */
#define ENABLE_TX_VARID  0x4007
#define DISABLE_TX_VARID 0x4008
/* Include for PTS RF-shielding */
#include "csr_bt_cm_lib.h"

#define HFG_SCO_DISC_GAURD (2*CSR_SCHED_SECOND)
#define HFG_DISC_GAURD     (5*CSR_SCHED_SECOND)
#define HFG_DEACT_GAURD    (5*CSR_SCHED_SECOND)

#ifdef USE_HFG_AUDIO
void CsrBtHfgScoDiscTimeout(CsrUint16 mi, void *mv)
{
    hfgInstance_t *inst;
    Connection_t *con;

    inst = (hfgInstance_t*) mv;

    CsrSchedTimerCancel(inst->discScoGuard, NULL, NULL);

    con = getActiveConnection(inst);
    if(con != NULL && con->pendingSLCDisconnect)
    {
        /* Failed in disconnecting SCO, set audioOn to false forcibly */
        con->audioOn = FALSE;
        startDisconnecting(inst);
    }
}
#endif

void CsrBtHfgSLCDiscTimeout(CsrUint16 mi, void *mv)
{
    hfgInstance_t *inst;
    Connection_t *con;
    CsrBtDeviceAddr  addr;
    CsrUint8       i;
    CsrCharString buf[100];
    CsrCharString buf1[10] = TEXT_SUCCESS_STRING;
    inst = (hfgInstance_t*) mv;

    CsrSchedTimerCancel(inst->discSLCGuard, NULL, NULL);

    con = getActiveConnection(inst);
    if(con != NULL)
    {
        con->audioOn = FALSE;
        con->active  = FALSE;
        if(con->instHfIndicators != NULL)
        {
            CsrPmemFree(con->instHfIndicators);
            con->instHfIndicators = NULL;
            con->hfIndCount = 0;
        }
        /* Clear connection instance, but remember address */
        addr.nap         = con->address.nap;
        addr.uap         = con->address.uap;
        addr.lap         = con->address.lap;
        clearConnection(inst, con);
        con->address.nap = addr.nap;
        con->address.uap = addr.uap;
        con->address.lap = addr.lap;
    }

    /* Select a new active instance */
    inst->current = NO_IDX;
    for(i=0; i<MAX_NUM_CONNECTION; i++)
    {
        if(inst->conInst[i].active)
        {
            inst->current = i;
            break;
        }
    }

    snprintf(buf, sizeof(buf), "Disconnect Timer has expired. It looks like the Headset is in bad state!!");

    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DISC_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2(buf1),
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

    CsrBtHfgShowUi(inst, CSR_BT_HFG_DISC_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_HIGH_PRIO);

/*    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DISC_DIALOG_UI].displayHandle);*/

}

void CsrBtHfgDeactTimeout(CsrUint16 mi, void *mv)
{
    hfgInstance_t *inst;
    CsrBtHfgCsrUiType    * csrUiVar;

    inst = (hfgInstance_t*) mv;
    csrUiVar  = &(inst->csrUiVar);

    initHfgData(inst);
    inst->state = stDeactivated;

    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEFAULT_INFO_UI].displayHandle);

    updateMainMenu(inst);
}

void hfgActivate(hfgInstance_t *inst)
{
    if (inst->state == stDeactivated)
    {
        CsrBtHfgHfIndicator *suppHfIndList = NULL;
        CsrUint16 hfIndCount = 0;
        if (inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_HF_INDICATORS)
        {
            suppHfIndList = hfgBuildLocalHfIndicatorList(inst);
            hfIndCount = SUPP_HFG_INDICATORS_COUNT;
        }
        CsrBtHfgActivateReqSendExt(inst->phandle,
               CSR_BT_HFG_AT_MODE_FULL,
               2,
               (CsrCharString *) StringDup(CSR_BT_HFG_SERVICE_NAME),
               inst->hfgSupportedFeatures,
               CSR_BT_HFG_CRH_SETUP,
               (CSR_BT_HFG_SETUP | CSR_BT_HFG_CNF_DISABLE_COPS),
               suppHfIndList, hfIndCount);
        syncSettings(inst);
        inst->state = stIdle;
        inst->copsSupport = FALSE;

        updateMainMenu(inst);

    }
    return;
}


/* Toggle profile activation on/off */
void toggleActivation(hfgInstance_t *inst)
{
    if(inst->state == stDeactivated)
    {
        CsrBtHfgHfIndicator *suppHfIndList = NULL;
        CsrUint16 hfIndCount = 0;
        if (inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_HF_INDICATORS)
        {
            suppHfIndList = hfgBuildLocalHfIndicatorList(inst);
            hfIndCount = SUPP_HFG_INDICATORS_COUNT;
        }
        /* Activate with all defaults settings */
        CsrBtHfgActivateReqSendExt(inst->phandle,
                           inst->parserMode,
                           2,
                           (CsrCharString *) StringDup(CSR_BT_HFG_SERVICE_NAME),
                           inst->hfgSupportedFeatures,
                           CSR_BT_HFG_CRH_SETUP,
                           CSR_BT_HFG_SETUP,
                           suppHfIndList, hfIndCount);

        syncSettings(inst);
        inst->state = stIdle;

        updateMainMenu(inst);
    }
    else
    {
        CsrBtHfgDeactivateReqSend();
        inst->deactGuard = CsrSchedTimerSet(HFG_DEACT_GAURD,
                                       CsrBtHfgDeactTimeout,
                                       0,
                                       (void*)inst);

        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_PLEASE_WAIT_UCS2,
                                        CONVERT_TEXT_STRING_2_UCS2("HFG_DEACTIVATE_REQ is sent!!Waiting for the Confirm."), TEXT_OK_UCS2, NULL);
        /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_HFG_STD_PRIO);
    }
}

/* Start connecting to other device */
void startHfgConnecting(hfgInstance_t *inst,
                            CsrBtHfgConnection type)
{
    Connection_t *con;
    char buf[100];

    con = getUnusedConnection(inst);
    if((getNumActive(inst) >= MAX_NUM_CONNECTION) ||
       (con == NULL))
    {
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_ALREADY_CON_DIALOG_UI, TEXT_FAILED_UCS2,
                                        TEXT_HFG_MAX_CON_REACHED_UCS2, TEXT_OK_UCS2, NULL);
        /* Show the CSR_BT_HFG_ALREADY_CON_DIALOG_UI on the display                   */
        CsrBtHfgShowUi(inst, CSR_BT_HFG_ALREADY_CON_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
    }
    else
    {
        if((con->address.nap == 0) &&
           (con->address.uap == 0) &&
           (con->address.lap == 0))
        {
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                            TEXT_HFG_NO_TARGET_DEVICE_UCS2, TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
        }
        else
        {
            Connection_t *con2 = getAddrConnection(inst,&(con->address));
            if ( (con2 != NULL) && (con2->active))
            {
                snprintf(buf, sizeof(buf),"Connection already established to %04X:%02X:%06X\n",con->address.nap,con->address.uap,con->address.lap);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_ALREADY_CON_DIALOG_UI, TEXT_FAILED_UCS2,
                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
                CsrBtHfgShowUi(inst, CSR_BT_HFG_ALREADY_CON_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

            }
            else
            {
                inst->current = getIndex(inst, con);
                con->active   = TRUE;

                CsrBtHfgServiceConnectReqSend(con->address, type);
                snprintf(buf, sizeof(buf),"Connecting to %04X:%02X:%06X with connection type '%s'...\n",
                        con->address.nap,
                        con->address.uap,
                        con->address.lap,
                        (type == CSR_BT_HFG_CONNECTION_AG ? "HS/AG" : "autodetect"));

                CsrBtHfgSetDialog(inst, CSR_BT_HFG_CANCEL_CON_DIALOG_UI, TEXT_PLEASE_WAIT_UCS2,
                                                CONVERT_TEXT_STRING_2_UCS2(buf), CONVERT_TEXT_STRING_2_UCS2("Cancel"), NULL);
                /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
                CsrBtHfgShowUi(inst, CSR_BT_HFG_CANCEL_CON_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
                inst->dialogShow = TRUE;
            }
        }
    }
}

/* Start cancel connect */
void startCancelConnect(hfgInstance_t *inst)
{
    Connection_t *con;
    char buf[40];
    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf),"No active connection selected\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

    }
    else
    {
        con->active               = FALSE;
        CsrBtHfgCancelConnectReqSend(con->address);
        snprintf(buf, sizeof(buf),"Cancelling connection attempt...\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

    }
}

/* Start disconnect */
void startDisconnecting(hfgInstance_t *inst)
{
    Connection_t *con;
    CsrCharString buf[40];
    CsrUint8 handle = CSR_BT_HFG_DEFAULT_INFO_UI;
    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;

    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf),"No active connection selected\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    else
    {
#ifdef USE_HFG_AUDIO
        /* Disconnect SCO and SLC one by one */
        if(con->audioOn)
        {
            con->pendingSLCDisconnect = TRUE;

            inst->discScoGuard = CsrSchedTimerSet(HFG_SCO_DISC_GAURD,
                                                  CsrBtHfgScoDiscTimeout,
                                                  0,
                                                  (void*)inst);

            CsrBtHfgAudioDisconnectReqSend(con->index);
        }
        else
#endif
        {
            CsrBtHfgDisconnectReqSend(con->index);

            inst->discSLCGuard = CsrSchedTimerSet(HFG_DISC_GAURD,
                                                  CsrBtHfgSLCDiscTimeout,
                                                  0,
                                                  (void*)inst);

            snprintf(buf, sizeof(buf),"Disconnecting...\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DISC_DIALOG_UI, TEXT_PLEASE_WAIT_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            handle = CSR_BT_HFG_DISC_DIALOG_UI;
            inputMode = CSR_UI_INPUTMODE_BLOCK;
            inst->dialogShow = TRUE;
        }
    }

    /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
    CsrBtHfgShowUi(inst, handle , inputMode, CSR_BT_HFG_STD_PRIO);

}

/* Traverse/toggle connections */
void toggleHfgConnections(hfgInstance_t *inst)
{
    CsrUint8 cnt;
    CsrCharString buf[40];

    /* Safely get next index */
    if(inst->current == NO_IDX)
    {
        inst->current = 0;
    }

    cnt = 0;
    while(TRUE)
    {
        inst->current++;
        if(inst->current >= MAX_NUM_CONNECTION)
        {
            inst->current = 0;
        }

        if(inst->conInst[inst->current].active)
        {
            /* Found active, bail out */
            break;
        }

        if(cnt >= MAX_NUM_CONNECTION)
        {
            /* All connections were investigated, but none are active,
             * bail out */
            inst->current = NO_IDX;
            break;
        }
        cnt++;
    }

    if(inst->current != NO_IDX)
    {
        CsrSnprintf(buf, 40, "New active connection is %u...\n", inst->current);
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    else
    {
        snprintf(buf, sizeof(buf), "No active connections found...\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

/*Toggle Rf Shield*/
void startToggleRfShield(hfgInstance_t *inst)
{
    CsrCharString buf[30];
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    if (inst->rfShield == FALSE)
    {
        inst->rfShield = TRUE;
        CsrBccmdWriteReqSend(CSR_BT_HFG_APP_IFACEQUEUE, DISABLE_TX_VARID, 0, 0, NULL);
        snprintf(buf, sizeof(buf), "RF shield activated\n");
    }
    else
    {
        inst->rfShield = FALSE;
        CsrBccmdWriteReqSend(CSR_BT_HFG_APP_IFACEQUEUE, ENABLE_TX_VARID, 0, 0, NULL);
        snprintf(buf, sizeof(buf), "RF shield deactivated\n");
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].displayHandle, CSR_UI_INPUTMODE_AUTO);
}

#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
void handleAtCmdChoiceMenu(hfgInstance_t *inst, CsrUint8 key)
{
    CsrUint8 idx = 0xFF;
    CsrUint8 bitValue = 0;
    CsrUint8 bitIndex = 0xFF;
    CsrCharString buf[40];
    CsrUint8 *parserCmdList = (CsrUint8*) CsrPmemAlloc(CSR_BT_HFG_APP_NUMBER_AT_CMD);


    switch (key)
    {
        case CSR_BT_HFG_FWD_UNHANDLED_CMDS_KEY: /* Unknown commands */
            bitValue = CSR_BT_HFG_APP_BIT_0;
            idx = 0;
            bitIndex = 0;
            break;
        case CSR_BT_HFG_ATA_KEY: /* ATA */
            bitValue = CSR_BT_HFG_APP_BIT_1;
            idx = 0;
            bitIndex = 1;
            break;

        case CSR_BT_HFG_VTS_KEY: /* AT+VTS */
            bitValue = CSR_BT_HFG_APP_BIT_1;
            idx = 4;
            bitIndex = 33; /* 4*8 + 0 +1 --> byte 4, bit 1 */
            break;

        case CSR_BT_HFG_BINP_KEY:/* AT+BINP */
            bitValue = CSR_BT_HFG_APP_BIT_2;
            idx = 4;
            bitIndex = 34; /* 4*8 + 1 + 1 --> byte 4, bit 2 */
            break;

        case CSR_BT_HFG_COPS_Q_KEY:/* AT+COPS? */
            bitValue = CSR_BT_HFG_APP_BIT_5;
            idx = 2;
            bitIndex = 21; /* 2*8 + 4 +1 --> byte 2, bit 5 */
            break;

        case CSR_BT_HFG_COPS_E_KEY:/* AT+COPS= */
            bitValue = CSR_BT_HFG_APP_BIT_6;
            idx = 2;
            bitIndex = 22; /* 2*8 + 5 + 1 --> byte 2, bit 6 */
            break;

        case CSR_BT_HFG_BLDN_KEY:/* AT+BLDN */
            bitValue = CSR_BT_HFG_APP_BIT_2;
            idx = 1;
            bitIndex = 10; /* 1*8 + 1 + 1 --> byte 1, bit 2 */
            break;

        case CSR_BT_HFG_BIA_E_KEY:/* AT+BIA= */
            bitValue = CSR_BT_HFG_APP_BIT_7;
            idx = 3;
            bitIndex = 31; /* 3*8 + 6 + 1 --> byte 3, bit 7 */
            break;

        case CSR_BT_HFG_AT_CMD_HANDLING_KEY: /*Config ALL AT Cmds */
            idx = 0xFF;
            break;

        default:
            break;
    }

    if (idx != 0xFF)
    {
        CsrBool handleInHfg = FALSE; /* value 0 means handle AT cmd in application */
        if ((inst->parserCmdList[idx] & bitValue) != 0)
        {/* It had value 1 and shall change to 0: handle in application */
            inst->parserCmdList[idx] &= ~bitValue;
        }
        else
        {/* Set bit to 1: handle AT-cmd in HFG */
            inst->parserCmdList[idx] |= bitValue;
            handleInHfg = TRUE;
        }
        CsrBtHfgConfigSingleAtcmdReqSend(CSR_BT_HFG_APP_IFACEQUEUE,bitIndex,handleInHfg);
        snprintf(buf, sizeof(buf), "Sent HFG_CONFIG_SINGLE_ATCMD_REQ!!");
    }
    else
    {/* Done: configure all */
        CsrMemCpy(parserCmdList, inst->parserCmdList, CSR_BT_HFG_APP_NUMBER_AT_CMD);
        CsrBtHfgConfigAtcmdHandlingReqSend(CSR_BT_HFG_APP_IFACEQUEUE, parserCmdList, CSR_BT_HFG_APP_NUMBER_AT_CMD);
        snprintf(buf, sizeof(buf), "Sent HFG_CONFIG_ATCMD_HANDLING _REQ");
    }

    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_PLEASE_WAIT_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_HFG_STD_PRIO);
    inst->dialogShow = TRUE;
}
#endif

/* Toggle audio on/off */
void toggleAudio(hfgInstance_t *inst)
{
    Connection_t *con;
    CsrCharString buf[40];
    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;

    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    else
    {
        if(INVERT(con->audioOn))
        {
            CsrBtHfgAudioConnectReqSend(con->index, PCM_SLOT, PCM_SLOT_REALLOCATE);
        }
        else
        {
            CsrBtHfgAudioDisconnectReqSend(con->index);
        }
        snprintf(buf, sizeof(buf), "Sent audio '%s' request...\n", (INVERT(con->audioOn) ? "on" : "off"));
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_PLEASE_WAIT_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        inputMode = CSR_UI_INPUTMODE_BLOCK;
        inst->dialogShow = TRUE;
    }
    /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);

}

/* perform outgoing call */
void startOutgoingCall(hfgInstance_t *inst, char *ringNumber)
{
    Connection_t *con;
    CsrUint8       call;
    CsrCharString buf[75];

    con = getActiveConnection(inst);

    /* Add new call to list */
    call = callGetUnused(inst);
    if(call == NO_CALL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    else
    {
        inst->calls[call].isSet  = TRUE;
        inst->calls[call].dir    = MOBILE_ORIGINATED;
        inst->calls[call].status = CS_ACTIVE;   /* [QTI] Should be "active", instead of "dialing". */
        inst->calls[call].mode   = VOICE;
        inst->calls[call].mpty   = NOT_MULTIPARTY_CALL;
        inst->calls[call].numType   = NUMBER_TYPE_INT;
        CsrMemCpy(inst->calls[call].number,
               ringNumber,
               CsrStrLen(ringNumber)+1);

        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "New outgoing call (to %s) added (index %i)\n", ringNumber, (CsrUint8 )(call+1));
        /* First simulate outgoing call - dialing */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                     CSR_BT_OUTGOING_CALL_SETUP_VALUE);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent 'call-setup' status indicator...\n");
        /* Then, outgoing call: remote alerting */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                     CSR_BT_OUTGOING_REMOTE_ALERT_VALUE);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent 'call-setup' status indicator...\n");
        /* Finally, simulate the remote device accepts the call: no call setup; call active */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                            CSR_BT_GATHERED_CALL_INDICATORS,
                                            CSR_BT_CALL_ACTIVE_NO_CALL_SETUP);
        inst->calls[call].status = CS_ACTIVE;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Active call indicators sent (no setup, active)...\n");
        if (con != NULL)
        {
            if(!con->audioOn && inst->autoAudioTransfer)
            {
                CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
                snprintf(buf, sizeof(buf), "Call active, request to open audio...\n");
            }
            else
            {
                snprintf(buf, sizeof(buf), "Call active, audio already open...\n");
            }
        }
        else
        {
            snprintf(buf, sizeof(buf), "Outgoing call started from AG, no HF-connection exists yet...\n");
        }

        CsrBtHfgSetDialog(inst, CSR_BT_HFG_SUCC_CON_DIALOG_UI, TEXT_SUCCESS_UCS2,
                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        CsrBtHfgShowUi(inst, CSR_BT_HFG_SUCC_CON_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_HIGH_PRIO);
        inst->state = stIdle;
        updateMainMenu(inst);
    }

}

/* Start ringing */
void startRinging(hfgInstance_t *inst, char *ringNumber)
{
    Connection_t *con;
    CsrUint8       call;
    CsrCharString buf[75];

    con = getActiveConnection(inst);

    /* Add new call to list */
    call = callGetUnused(inst);
    if(call == NO_CALL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    else
    {
        inst->calls[call].isSet  = TRUE;
        inst->calls[call].dir    = MOBILE_TERMINATED;
        inst->calls[call].status = CS_INCOMING;
        inst->calls[call].mode   = VOICE;
        inst->calls[call].mpty   = NOT_MULTIPARTY_CALL;
        inst->calls[call].numType   = NUMBER_TYPE_INT;
        CsrMemCpy(inst->calls[call].number,
               ringNumber,
               CsrStrLen(ringNumber)+1);

        if(callGetStatus(inst, CS_ACTIVE) == NO_CALL)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "New incoming call (from %s) added (index %i)\n", ringNumber, (CsrUint8 )(call+1));

            /* No call active, so start normal ring */
            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                         CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                         CSR_BT_INCOMING_CALL_SETUP_VALUE);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent 'call-setup' status indicator...\n");

            if(con != NULL)
            {
                if(con->inbandRinging)
                {
                    CsrBtHfgAudioConnectReqSend(con->index, PCM_SLOT, PCM_SLOT_REALLOCATE);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Inband ringing is enabled, sent audio open request...\n");
                }

                CsrBtHfgRingReqSend(con->index,
                               RING_REP_RATE,
                               RING_NUM_OF_RINGS,
                               (CsrCharString *) StringDup(ringNumber),
                               (CsrCharString *) StringDup((char*)inst->name),
                               NUMBER_TYPE_INT);
                snprintf(buf, sizeof(buf), "Sent ring request...\n");
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2("Incoming Call parameters have been set!!"), TEXT_OK_UCS2, NULL);
            }
        }
        else
        {
            /* Modify incoming call to be waiting */
            inst->calls[call].status = CS_WAITING;
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "New waiting call (from %s) added (index %i)\n", ringNumber, call);

            /* Call already active, so start call waiting */
            if(con != NULL)
            {
                CsrBtHfgCallWaitingReqSend(con->index,
                                      (CsrCharString *) StringDup(ringNumber),
                                      (CsrCharString *) StringDup((char*)inst->name),
                                      NUMBER_TYPE_INT);
                snprintf(buf, sizeof(buf), "Sent call waiting notification...\n");
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            }

            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                         CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                         CSR_BT_INCOMING_CALL_SETUP_VALUE);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent 'call-setup' status indicator...\n");
        }
    }
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

/* Answer */
void startAnswer(hfgInstance_t *inst)
{
    CsrUint8 call;
    Connection_t *con;
    CsrCharString buf[90];
    CsrCharString inputMode = CSR_UI_INPUTMODE_AUTO;
    CsrBool dataReady = FALSE;

    con = getActiveConnection(inst);
    call = callChangeStatus(inst, CS_INCOMING, CS_ACTIVE);
    if(call == NO_CALL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Incoming call answered and set active (index %i)\n", call);

        /* Stop ringing and turn on SCO */
        if(con != NULL)
        {
#ifdef USE_HFG_RIL
            ConnxHfgRilAnswer(inst->hfgRilHandle);
#endif

            CsrBtHfgRingReqSend(con->index,
                           0,
                           0,
                           NULL,
                           NULL,
                           0);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Ringing stopped for connection %i\n", con->index);

            if(!con->audioOn)
            {
                CsrBtHfgAudioConnectReqSend(con->index, PCM_SLOT, PCM_SLOT_REALLOCATE);
                snprintf(buf, sizeof(buf), "Requested audio on for answered call...\n");
                inputMode = CSR_UI_INPUTMODE_BLOCK;
                inst->dialogShow = TRUE;
                dataReady = TRUE;
            }
        }

        /* Set indicators */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                     CSR_BT_GATHERED_CALL_INDICATORS,
                                     CSR_BT_CALL_ACTIVE_NO_CALL_SETUP);
        if(!dataReady)
        {
            snprintf(buf, sizeof(buf), "Sent 'call-active' status indicator and 'no-call-setup' setup indicator...\n");
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent 'call-active' status indicator and 'no-call-setup' setup indicator...\n");
        }
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

    }
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);
}

/* Reject */
void startReject(hfgInstance_t *inst)
{
    Connection_t *con;
    CsrUint8 call;
    CsrCharString buf[110];
    CsrCharString inputMode = CSR_UI_INPUTMODE_AUTO;

    /* Close audio if it's on */
    con = getActiveConnection(inst);
    if(con != NULL)
    {
#ifdef USE_HFG_RIL
        TerminateCall(inst);
#endif

        CsrBtHfgRingReqSend(con->index,
                       0,
                       0,
                       NULL,
                       NULL,
                       0);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent ring stop request...\n");

        if(con->audioOn)
        {
            CsrBtHfgAudioDisconnectReqSend(con->index);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent audio disconnect request...\n");
            snprintf(buf, sizeof(buf), "Sent Audio Disconnect Request!!\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_PLEASE_WAIT_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            inputMode = CSR_UI_INPUTMODE_BLOCK;
            inst->dialogShow = TRUE;
        }
    }

    call = callUnsetStatus(inst, CS_ACTIVE);
    if(call != NO_CALL)
    {
        /* Reject ongoing call */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                     CSR_BT_CALL_STATUS_INDICATOR,
                                     CSR_BT_NO_CALL_ACTIVE_VALUE);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent 'no-call-active' indicator to reject ongoing call...\n Active call has been dropped (index %i)\n", call);
    }
    else
    {
        call = callUnsetStatus(inst, CS_INCOMING);
        if(call != NO_CALL)
        {
            snprintf(buf, sizeof(buf), "Waiting call has been dropped (index %i)\n", call);
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

            /* Reject incoming call */
            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                         CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                         CSR_BT_NO_CALL_SETUP_VALUE);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent 'no-call-setup' indicator to reject incoming call...\n");
        }
    }

    if(call == NO_CALL)
    {
        snprintf(buf, sizeof(buf), "No active/incoming call to reject\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);
}

/* Put incoming call on hold */
void startPutOnHold(hfgInstance_t *inst)
{
    Connection_t *con;
    CsrUint8       call;
    CsrCharString buf[70];

    /* Do we have any incoming calls? */
    call = callGetStatus(inst, CS_INCOMING);
    if(call != NO_CALL)
    {
        call = callChangeStatus(inst, CS_INCOMING, CS_HELD);
        if(call != NO_CALL)
        {
#ifdef USE_HFG_RIL
            ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_BTRH_PUT_ON_HOLD, 0);
#endif

            CsrBtHfgCallHandlingReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                   CSR_BT_HFG_BTRH_INCOMING_ON_HOLD);

            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                     CSR_BT_GATHERED_CALL_INDICATORS,
                                     CSR_BT_CALL_ACTIVE_NO_CALL_SETUP);

            con = getActiveConnection(inst);
            if(con != NULL)
            {
                /* If active connection exists, Stop ringing */
                CsrBtHfgRingReqSend(con->index,
                               0,
                               0,
                               NULL,
                               NULL,
                               0);
                /* And close audio if connection exists and no other active call present */
                if ((con->audioOn) && (callGetStatus(inst, CS_ACTIVE) == NO_CALL))
                {
                    CsrBtHfgAudioDisconnectReqSend(con->index);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE,  "Sent Audio Disconnect Request!!\n");
               }
            }
            snprintf(buf, sizeof(buf), "Call handling command: Incoming call is now on hold (index %i)\n", call);
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

        }
        else
        {
            snprintf(buf, sizeof(buf), "No incoming calls to put on hold\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        }
    }
    else
    {
            snprintf(buf, sizeof(buf), "No incoming calls to put on hold\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

/* Answer call on hold */
void startAnswerCallOnHold(hfgInstance_t *inst)
{
    CsrUint8       call;
    CsrCharString buf[60];

    /* Do we have any held calls? */
    call = callGetStatus(inst, CS_HELD);
    if(call != NO_CALL)
    {
        call = callChangeStatus(inst, CS_HELD, CS_ACTIVE);
        if(call != NO_CALL)
        {
#ifdef USE_HFG_RIL
            ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_BTRH_ACCEPT_INCOMING, 0);
#endif

            CsrBtHfgCallHandlingReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                   CSR_BT_HFG_BTRH_ACCEPT_INCOMING);

            snprintf(buf, sizeof(buf), "Call handling command: Answer call on hold (index %i)\n", call);
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

        }
        else
        {
            snprintf(buf, sizeof(buf), "No held calls to answer\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        }
    }
    else
    {
        snprintf(buf, sizeof(buf), "No held calls to answer\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

/* Reject call on hold */
void startRejectCallOnHold(hfgInstance_t *inst)
{
    CsrUint8       call;
    CsrCharString buf[70];

    /* Do we have any held calls? */
    call = callGetStatus(inst, CS_HELD);
    if(call != NO_CALL)
    {
        call = callUnsetStatus(inst, CS_HELD);
        if(call != NO_CALL)
        {
#ifdef USE_HFG_RIL
            ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_BTRH_REJECT_INCOMING, 0);
#endif

            CsrBtHfgCallHandlingReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                   CSR_BT_HFG_BTRH_REJECT_INCOMING);
            if (callGetStatus(inst, CS_ACTIVE) == NO_CALL)
            {
                CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                             CSR_BT_CALL_STATUS_INDICATOR,
                                             CSR_BT_NO_CALL_ACTIVE_VALUE);
            }

            if (callGetStatus(inst, CS_HELD) == NO_CALL)
            {
                CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                             CSR_BT_CALL_HELD_INDICATOR,
                                             CSR_BT_NO_CALL_HELD_VALUE);
            }
            snprintf(buf, sizeof(buf), "Call handling command: Held call is now Rejected (index %i)\n", call);
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        }
        else
        {
        snprintf(buf, sizeof(buf), "No held calls to reject\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        }
    }
    else
    {
        snprintf(buf, sizeof(buf), "No held calls to reject\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

/* Return enable/disable string given CsrBool */
char *settingOn(int e)
{
    static char *enable  = "enabled";
    static char *disable = "disabled";

    return ((e != 0) ? enable : disable);
}

/* Display features about local and remote sides */
void displayFeatures(hfgInstance_t *inst)
{
    Connection_t *con;
    CsrCharString buf[2000];
    CsrCharString buf1[1000];

    CsrMemSet(buf, 0, 1500);
    CsrMemSet(buf1, 0, 500);

    snprintf((char*)buf, sizeof(buf), "Locally supported HFG features:\n Supported features bitmask:(0x%08x)\n Three way calling: (%s)\n \
        Echo/noise reduction: (%s)\n, Voice recognition: (%s)\n Inband ringing: (%s)\n, Attach number to voice tag: (%s)\n, \
        Ability to reject call: (%s)\n, Enhanced call status: (%s)\n Enhanced call control: (%s)\n Extended error codes: (%s)\n",
                        inst->hfgSupportedFeatures,
                        settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_THREE_WAY_CALLING),
                        settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION),
                        settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_VOICE_RECOGNITION),
                        settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_INBAND_RINGING),
                        settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_ATTACH_NUMBER_TO_VOICE_TAG),
                           settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_ABILITY_TO_REJECT_CALL),
                           settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_ENHANCED_CALL_STATUS),
                           settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_ENHANCED_CALL_CONTROL),
                           settingOn(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_EXTENDED_ERROR_CODES));


    con = getActiveConnection(inst);
    if(con != NULL)
    {
    snprintf((char*)buf1, sizeof(buf1), "Locally activated HFG features:\n Echo/noise reduction(%s)\n Voice recognition (%s)\n \
        Inband ringing (%s)\n Remotely supported HF/HS features:\n Supported features bitmask (0x%08x)\n \
        Echo/noise reduction (%s)\n Call waiting/3-way calling (%s)\n Calling line presentation (%s)\n \
        Voice recognition (%s)\n Remote volume control (%s)\n Enhanced call status            (%s)\n \
        Enhanced call control (%s)\n",
                        settingOn(con->nrec),
                        settingOn(con->voiceRecognition),
                        settingOn(con->inbandRinging),
                        con->hfSupportedFeatures,
                        settingOn(con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_EC_NR_FUNCTION),
                        settingOn(con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_CALL_WAITING_THREE_WAY_CALLING),
                        settingOn(con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_CLI_PRESENTATION_CAPABILITY),
                        settingOn(con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION),
                        settingOn(con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL),
                        settingOn(con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_ENHANCED_CALL_STATUS),
                        settingOn(con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_ENHANCED_CALL_CONTROL));
    }
    CsrStrLCat(buf, buf1, sizeof(buf));

    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

void updateCievCallParameters(hfgInstance_t *inst)
{
    CsrUint8 cievCall[3], x;

    cievCall[0] = CSR_BT_NO_CALL_ACTIVE_VALUE;
    cievCall[1] = CSR_BT_NO_CALL_SETUP_VALUE;
    cievCall[2] = CSR_BT_NO_CALL_HELD_VALUE;
/*
    if ((CS_ACTIVE == inst->calls[inst->callTarget].status) && (inst->calls[inst->callTarget].isSet))
    {
        cievCall[0] = CSR_BT_CALL_ACTIVE_VALUE;
    }
*/
    for (x=0; x<MAX_CALLS; x++)
    {
        if ((CS_ACTIVE == inst->calls[x].status) && (inst->calls[x].isSet))
        {
            cievCall[0] = CSR_BT_CALL_ACTIVE_VALUE;
        }
        else if ((CS_DIALING == inst->calls[x].status))
        {
            cievCall[1] = CSR_BT_OUTGOING_CALL_SETUP_VALUE;
        }
        else if ((CS_ALERTING == inst->calls[x].status)  || (CS_INCOMING == inst->calls[x].status) || (CS_WAITING == inst->calls[x].status))
        {
            cievCall[1] = CSR_BT_INCOMING_CALL_SETUP_VALUE;
        }
        else if (CS_HELD == inst->calls[x].status)
        {
            cievCall[2] = CSR_BT_CALL_HELD_NO_ACTIVE_CALL_VALUE;
        }
    }
    if ((cievCall[0] == CSR_BT_CALL_ACTIVE_VALUE) && (cievCall[2] == CSR_BT_CALL_HELD_NO_ACTIVE_CALL_VALUE))
    {
        cievCall[2] = CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE;
    }
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_CALL_STATUS_INDICATOR,
                                 cievCall[0]);
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                 cievCall[1]);
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_CALL_HELD_INDICATOR,
                                 cievCall[2]);
}

/* Handle call alteration */
void handleCallAlterMenu(hfgInstance_t *inst, CsrUint8 key)
{
    CsrCharString buf[65];
    /* Escape switches back to main menu */
    switch(key)
    {
        case CSR_BT_HFG_CL_CHANGE_ACTIVE_KEY:
            {
                /* Set to active call */
                inst->calls[inst->callTarget].isSet   = TRUE;
                inst->calls[inst->callTarget].dir     = MOBILE_ORIGINATED;
                inst->calls[inst->callTarget].status  = CS_ACTIVE;
                inst->calls[inst->callTarget].mode    = VOICE;
                inst->calls[inst->callTarget].mpty    = FALSE;
                CsrMemCpy(inst->calls[inst->callTarget].number,
                       SUB_NUMBER_2,
                       CsrStrLen(SUB_NUMBER_2)+1);

                updateCievCallParameters(inst);
                snprintf(buf, sizeof(buf), "Call status changed to Active!!");
                break;
            }
        case CSR_BT_HFG_CL_DROP_CALL_KEY:
            {
                CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
                /* Drop */
                callUnsetIndex(inst, inst->callTarget);
                updateCievCallParameters(inst);
                snprintf(buf, sizeof(buf), "Call is dropped!!");
                CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI].displayHandle);
                break;
            }
        case CSR_BT_HFG_CL_TOGGLE_DIR_KEY:
            {
                /* Toggle direction */
                inst->calls[inst->callTarget].dir++;
                if(inst->calls[inst->callTarget].dir > MOBILE_TERMINATED)
                {
                    inst->calls[inst->callTarget].dir = MOBILE_ORIGINATED;
                }
                snprintf(buf, sizeof(buf), "Direction is Toggled!!");
                break;
            }
        case CSR_BT_HFG_CL_TRAVERSE_STATUS_KEY:
            {
                /* Traverse status */
                inst->calls[inst->callTarget].status++;
                if(inst->calls[inst->callTarget].status > CS_WAITING)
                {
                    inst->calls[inst->callTarget].status = CS_ACTIVE;
                }
                updateCievCallParameters(inst);
                snprintf(buf, sizeof(buf), "Status Traverse(active/held/dial/alert/incoming/wait) is done!!");
                break;
            }
        case CSR_BT_HFG_CL_TOGGLE_MULTIPARTY_KEY:
            {
                /* Toggle multiparty */
                inst->calls[inst->callTarget].mpty++;
                if(inst->calls[inst->callTarget].mpty > MULTIPARTY_CALL)
                {
                    inst->calls[inst->callTarget].mpty = NOT_MULTIPARTY_CALL;
                }
                snprintf(buf, sizeof(buf), "Multiparty status is Toggled!!");
                break;
            }
        default:
            {
                CsrGeneralException("CSR_BT_HFG", 0, 0,
                               "Unexpected menu key is received");
                break;
            }
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

}
void displayCallOptions(hfgInstance_t *inst)
{
    CsrCharString buf[200];
    CsrUint8 i = inst->callTarget;

    CsrSnprintf((char*)buf, 200, "Selected call information\n \
    Index       %i\n \
    Exists      %s\n \
    Direction   %s\n \
    Status      %s\n \
    Mode        %i\n \
    Multiparty  %s\n \
    Number      '%s'\n ", i+1, (inst->calls[i].isSet ? "yes" : "no"),
                                        (inst->calls[i].dir == MOBILE_TERMINATED ? "incoming" : "outgoing"),
                                        callGetStringStatus(inst, i),
                                        inst->calls[i].mode,
                                        (inst->calls[i].mpty == MULTIPARTY_CALL ? "yes" : "no"),
                                        inst->calls[i].number);
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

/* Global indicator - roam status */
void toggleRoam(hfgInstance_t *inst)
{
    /* Send to 'all' */
    CsrCharString buf[50];
    inst->roamStatus = INVERT(inst->roamStatus);
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_ROAM_INDICATOR,
                                 (CsrUint8)(inst->roamStatus
                                           ? CSR_BT_SERVICE_PRESENT_VALUE
                                           : CSR_BT_NO_SERVICE_VALUE));

    snprintf(buf, sizeof(buf), "Sent roam '%s' status indicator...\n",
           (inst->roamStatus ? "on" : "off"));
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

}

/* Global indicator - cellular registration status */
void toggleCellRegStatus(hfgInstance_t *inst)
{
    CsrCharString buf[55];
    /* Send to all */
    inst->regStatus = INVERT(inst->regStatus);
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_SERVICE_INDICATOR,
                                 (CsrUint8)(inst->regStatus
                                           ? CSR_BT_SERVICE_PRESENT_VALUE
                                           : CSR_BT_NO_SERVICE_VALUE));
    snprintf(buf, sizeof(buf), "Sent cellular registration '%s' status indicator...\n",
           (inst->regStatus ? "on" : "off"));
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

}

/* Global indicator - signal strength */
void toggleSignal(hfgInstance_t *inst, CsrBool inc)
{
    CsrCharString buf[50];
    if(inc && (inst->signal < CSR_BT_SIGNAL_STRENGTH_LEVEL_5))
    {
        inst->signal++;
    }
    else if(!inc && (inst->signal > CSR_BT_SIGNAL_STRENGTH_LEVEL_0))
    {
        inst->signal--;
    }

    /* Send to 'all' */
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_SIGNAL_STRENGTH_INDICATOR,
                                 inst->signal);
    snprintf(buf, sizeof(buf), "Send signal strength indicator with value %i...\n", inst->signal);
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

}

/* Global Indicator - battery level */
void toggleBattery(hfgInstance_t *inst, CsrBool inc)
{
    CsrCharString buf[50];
    if(inc && (inst->battery < CSR_BT_BATTERY_CHARGE_LEVEL_5))
    {
        inst->battery++;
    }
    else if(!inc && (inst->battery > CSR_BT_BATTERY_CHARGE_LEVEL_0))
    {
        inst->battery--;
    }

    /* Send to 'all' */
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_BATTERY_CHARGE_INDICATOR,
                                 inst->battery);
    snprintf(buf, sizeof(buf), "Send battery charge indicator with value %i...\n", inst->battery);

    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

}

/* Connection indicator indicator - mic gain */
void toggleMic(hfgInstance_t *inst, CsrBool inc)
{
    Connection_t *con;
    CsrCharString buf[60], result[10];
    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrStrLCpy(result, TEXT_FAILED_STRING, sizeof(result));
    }
    else
    {
        if (con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL)
        {
            if(inc && (con->micGain < MAX_MIC_GAIN))
            {
                con->micGain++;
            }
            else if(!inc && (con->micGain > MIN_MIC_GAIN))
            {
                con->micGain--;
            }

            CsrBtHfgMicGainReqSend(con->index,
                              con->micGain);
            snprintf(buf, sizeof(buf), "Send microphone gain value %i...\n", con->micGain);
            CsrStrLCpy(result, TEXT_SUCCESS_STRING, sizeof(result));
        }
        else
        {
            snprintf(buf, sizeof(buf), "Remote volume control not activated on HF side.\n");
            CsrStrLCpy(result, TEXT_FAILED_STRING, sizeof(result));
        }
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(result),
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
}

/* Connection indicator - speaker gain */
void toggleSpeaker(hfgInstance_t *inst, CsrBool inc)
{
    Connection_t *con;
    CsrCharString buf[60], result[10];

    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrStrLCpy(result, TEXT_FAILED_STRING, sizeof(result));
    }
    else
    {
        if (con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL)
        {
            if(inc && (con->speakerGain < MAX_SPK_GAIN))
            {
                con->speakerGain++;
            }
            else if(!inc && (con->speakerGain > MIN_SPK_GAIN))
            {
                con->speakerGain--;
            }

            CsrBtHfgSpeakerGainReqSend(con->index,
                                  con->speakerGain);
            snprintf(buf, sizeof(buf), "Send speaker gain value %i...\n", con->speakerGain);
            CsrStrLCpy(result, TEXT_SUCCESS_STRING, sizeof(result));
        }
        else
        {
            snprintf(buf, sizeof(buf), "Remote volume control not activated on HF side.\n");
            CsrStrLCpy(result, TEXT_FAILED_STRING, sizeof(result));
        }
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(result),
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
    }


/* Connection indicator - inband ringing */
void toggleInband(hfgInstance_t *inst, CsrBool on)
{
    Connection_t *con;
    CsrCharString buf[60], result[10];

    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrStrLCpy(result, TEXT_FAILED_STRING, sizeof(result));
    }
    else
    {
        con->inbandRinging = on;
        CsrBtHfgInbandRingingReqSend(con->index,
                                con->inbandRinging);
        snprintf(buf, sizeof(buf), "Sent inband ringing '%s' setting...\n",
               (con->inbandRinging ? "on" : "off"));
        CsrStrLCpy(result, TEXT_SUCCESS_STRING, sizeof(result));
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(result),
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

}

/* Connection indicator - voice recognition */
void toggleVoiceRecog(hfgInstance_t *inst, CsrBool on)
{
    Connection_t *con;
    CsrCharString buf[100], result[10];
    CsrCharString buf1[50] = "";
    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;
    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "No active connection selected\n");
        CsrStrLCpy(result, TEXT_FAILED_STRING, sizeof(result));
    }
    else
    {
        if (con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION)
        {
            con->voiceRecognition = on;
            CsrBtHfgVoiceRecogReqSend(con->index,
                                 con->voiceRecognition);
            snprintf(buf, sizeof(buf), "Sent voice recognition '%s' setting...\n",
                   (con->voiceRecognition ? "on" : "off"));

            if((!con->audioOn && con->voiceRecognition) ||
               (con->audioOn && !con->voiceRecognition))
            {
                snprintf(buf1, sizeof(buf1), "Requesting audio %s for voice recognition...\n",
                       (con->voiceRecognition ? "on" : "off"));
                CsrStrLCat(buf, buf1, sizeof(buf));
                inputMode = CSR_UI_INPUTMODE_BLOCK;
                inst->dialogShow = TRUE;
                if(con->voiceRecognition)
                {
                    CsrBtHfgAudioConnectReqSend(con->index, PCM_SLOT, PCM_SLOT_REALLOCATE);
                }
                else
                {
                    CsrBtHfgAudioDisconnectReqSend(con->index);
                }
            }
            CsrStrLCpy(result, TEXT_SUCCESS_STRING, sizeof(result));
        }
        else
        {
            snprintf(buf, sizeof(buf), "\nVoice recognition feature not supported by HF\n");
            CsrStrLCpy(result, TEXT_FAILED_STRING, sizeof(result));
        }
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(result),
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);
}

CsrUint32 extractNumberFromString(CsrUint8 *string, CsrUint32 len)
{
    CsrUint32 no = 0, x;

    if (len>2)
    {
        if ((string[0] == '0') && ((string[1] == 'x') || (string[1] == 'X')))
        {
            for (x=2; x<len; x++)
            {
                if (string[x] >= '0' && string[x] <= '9')
                {
                    no = ((no*16) + (string[x]-'0'));
                }
                else if ((string[x] >= 'a') && (string[x] <= 'f'))
                {
                    no = ((no*16) + (string[x]-'a'+10));
                }
                else if ((string[x] >= 'A') && (string[x] <= 'F'))
                {
                    no = ((no*16) + (string[x]-'A'+10));
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            no = atoi((char *) string);
        }
    }
    else
    {
        no = atoi((char *) string);
    }
    return no;
}

#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
/* Handle new SCO parameter input */
void handleScoInput(hfgInstance_t* inst, CsrUint8 key)
{
    /* Convert from ASCII to integer */
    CsrUint32 no;

    no = extractNumberFromString(inst->input, inst->inputLength);

    /* Store setting */
    switch(key)
    {
        case CSR_BT_HFG_CHANGE_TX_PATH_KEY:
            inst->sco.txBandwidth = no;
            break;

        case CSR_BT_HFG_CHANGE_RX_PATH_KEY:
            inst->sco.rxBandwidth = no;
            break;

        case CSR_BT_HFG_CHANGE_MAX_LATENCY_KEY:
            inst->sco.maxLatency = (CsrUint16)no;
            break;

        case CSR_BT_HFG_CHANGE_VOICE_CODEC_KEY:
            inst->sco.voiceSettings = (CsrUint16)no;
            break;

        case CSR_BT_HFG_CHANGE_AUDIO_QUALITY_KEY:
            inst->sco.audioQuality = (CsrUint16)no;
            break;

        case CSR_BT_HFG_CHANGE_RETRAN_EFFORT_KEY:
            inst->sco.reTxEffort = (CsrUint8)no;
            break;
    }

    /* Send settings */
    syncScoSettings(inst);

    /* Go back to main menu in idle state */
    inst->inputLength = 0;
    inst->input[0] = '\0';
}
#endif
