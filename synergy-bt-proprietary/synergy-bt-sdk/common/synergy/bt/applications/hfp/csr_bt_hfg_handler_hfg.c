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
#include "csr_bt_hfg_app_util.h"
#include "csr_bt_hfg_menu_utils.h"


#ifdef USE_HFG_RIL
static void InitHfgRilRegisterInfo(ConnxHfgRilRegisterInfo *registerInfo, hfgInstance_t *inst);
static CsrBtHfgConnectionId GetHfgConnectionId(hfgInstance_t *inst);

static void SetCallId(hfgInstance_t *inst, uint32_t call_id);
static void ClearCallId(hfgInstance_t *inst);

static void SendClccRequest(hfgInstance_t *inst);

static void StartAudio(hfgInstance_t *inst);
static void StopAudio(hfgInstance_t *inst);

static void GetCallList(hfgInstance_t *inst);

static void HandleDialResult(hfgInstance_t *inst, uint16_t cmee_result);

static void GenerateDTMF(hfgInstance_t *inst, char dtmf);

static void GetSubscriberNum(hfgInstance_t *inst);

static void HfgRilCallEndRespCb(ConnxContext context, uint16_t cmee_result);
static void HfgRilAnswerRespCb(ConnxContext context, uint16_t cmee_result);
static void HfgRilGetSubscriberNumRespCb(ConnxContext context, uint16_t cmee_result, const char *phone_number);
static void HfgRilGetCurrentCallListRespCb(ConnxContext context, uint16_t cmee_result, ConnxHfgCallInfo *call_list, uint32_t call_count);
static void HfgRilSetCallNotificationIndRespCb(ConnxContext context, uint16_t cmee_result);
static void HfgRilSetCallWaitingNotificationRespCb(ConnxContext context, uint16_t cmee_result);
static void HfgRilGenerateDtmfRespCb(ConnxContext context, uint16_t cmee_result);
static void HfgRilCallHandlingRespCb(ConnxContext context, uint16_t cmee_result);
static void HfgRilGetCopsRespCb(ConnxContext context, uint16_t cmee_result, char *cops);
static void HfgRilDialRespCb(ConnxContext context, uint16_t cmee_result, uint32_t call_id);

static void HfgRilVoiceCallIndCb(ConnxContext context, uint16_t cmee_result, ConnxHfgCallInfo *call_list, uint32_t call_count);

static void HfgRilHandleEmptyCallList(hfgInstance_t *inst);
static void HfgRilHandleCallActiveStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
static void HfgRilHandleCallHeldStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
static void HfgRilHandleOutgoingCallStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
static void HfgRilHandleOutgoingCallAlertStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
static void HfgRilHandleIncomingCallStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
static void HfgRilHandleCallWaitingStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
static void HfgRilHandleCallEndStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
static void HfgRilHandleUnknownCallStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count);
#endif


/****************************************************************************************
*
*
*
*
*****************************************************************************************/
static void handleMptyCallStatus(hfgInstance_t *inst)
{
   CsrUint8 i;
   CsrUint8 numberOfActiveCalls = 0;


   for (i=0;i<MAX_CALLS;i++)
    {
     if ((inst->calls[i].isSet) && (inst->calls[i].status == CS_ACTIVE))
     {
         numberOfActiveCalls++;
     }
    }
    /* Now
       - if no active calls exist:          do nothing;
       - if there is only one active call:  make sure it is not marked as multiparty
       - if there are more than one active calls: marke them all as multiparty       */
    if (numberOfActiveCalls > 1)
    {
       for (i=0;((i<MAX_CALLS) && (numberOfActiveCalls != 0));i++)
        {
            if ((inst->calls[i].isSet) && (inst->calls[i].status == CS_ACTIVE))
            {
               numberOfActiveCalls--;
               inst->calls[i].mpty = MULTIPARTY_CALL;
            }
        }
    }
    else
    {
        if (numberOfActiveCalls == 1)
        {
         for (i=0;((i<MAX_CALLS) && (numberOfActiveCalls != 0));i++)
            {
                if ((inst->calls[i].isSet) && (inst->calls[i].status == CS_ACTIVE))
                {
                   numberOfActiveCalls--;
                   inst->calls[i].mpty = NOT_MULTIPARTY_CALL;
                }
            }
        }
    }
}



/* Handle SLC connection */
static void handleServiceConnectInd(hfgInstance_t *inst, CsrBtHfgServiceConnectInd *prim)
{
    Connection_t *con;
    CsrCharString buf[150], buf1[70];
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrBool fail = TRUE;

    con = getAddrConnection(inst, &(prim->deviceAddr));
    if(con == NULL)
    {
        con = getUnusedConnection(inst);
    }

#if 0
/*Dialog is already shown in Block mode. It has to be unblocked*/
    if(inst->dialogShow)
        CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEFAULT_INFO_UI].displayHandle, CSR_UI_INPUTMODE_AUTO);
#endif

    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "Could not find instance for new connection\n");
    }
    else
    {
        if(prim->resultCode == CSR_BT_RESULT_CODE_HFG_SUCCESS &&
           prim->resultSupplier == CSR_BT_SUPPLIER_HFG)
        {
            char   *name;
            CsrUint8 call;

            fail = FALSE;

            con->index                = prim->connectionId;
            con->connType             = prim->connectionType;
            con->hfSupportedFeatures  = prim->supportedFeatures;
            con->address              = prim->deviceAddr;
            con->remoteVersion        = prim->remoteVersion;
            con->active               = TRUE;
            con->inbandRinging        = (CsrBool)((inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_INBAND_RINGING) ? TRUE : FALSE);

            inst->current = getIndex(inst, con);
            if(prim->hfSupportedHfIndicators != NULL)
            {
                RemoteHfIndicators *indList;
                CsrUint16 count;

                indList = (RemoteHfIndicators *)CsrPmemZalloc(prim->hfSupportedHfIndicatorsCount * sizeof(RemoteHfIndicators));
                con->instHfIndicators = indList;
                con->hfIndCount = prim->hfSupportedHfIndicatorsCount;

                count = 0;
                while(count < prim->hfSupportedHfIndicatorsCount)
                {
                    CsrBtHfgHfIndicator *rcvdInd;
                    indList[count].indId = prim->hfSupportedHfIndicators[count];
                    rcvdInd = hfgFetchLocalHfIndicator(inst,indList[count].indId);
                    if(rcvdInd != NULL)
                    {
                        indList[count].status = rcvdInd->status;
                    }
                    else
                    {
                        indList[count].status = CSR_BT_HFP_HF_INDICATOR_STATE_DISABLE;
                    }
                    count++;
                }
                CsrPmemFree(prim->hfSupportedHfIndicators);
            }

            name = "<n/a>";
            if(prim->serviceName != NULL)
            {
                name = prim->serviceName;
            }

            snprintf(buf, sizeof(buf), "Established '%s' type service level connection to '%s'\n",
                   (con->connType == CSR_BT_HFG_CONNECTION_HFG ? "HFP" : "old HS"),
                   name);

            if (con->connType == CSR_BT_HFG_CONNECTION_HFG)
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Remote HFP version supported: 0x%02x \n", con->remoteVersion);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE,"Remote device %s codec negotiation.\n",
                    ((con->hfSupportedFeatures & CSR_BT_HF_SUPPORT_CODEC_NEGOTIATION) ? "SUPPORTS" : "DOES NOT SUPPORT") );
                

            }

            /* If we have an incoming call in the list */
            call = callGetStatus(inst, CS_INCOMING);
            if(call != NO_CALL)
            {
                /* Begin ringing... */
                CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                             CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                             CSR_BT_INCOMING_CALL_SETUP_VALUE);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Incoming call in list, sent 'call-setup' indicator...\n");

                if(con->inbandRinging && !con->audioOn)
                {
                    CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
                    snprintf(buf1, sizeof(buf1), "Inband ringing is enabled, sent audio open request...\n");
                    CsrStrLCat(buf, buf1, sizeof(buf));
                }

                CsrBtHfgRingReqSend(con->index,
                               RING_REP_RATE,
                               RING_NUM_OF_RINGS,
                               (CsrCharString *) StringDup(RING_NUMBER),
                               (CsrCharString *) StringDup((char*)inst->name),
                               NUMBER_TYPE_INT);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent ring request...\n");
            }
            else
            {
                call = callGetStatus(inst, CS_ACTIVE);
                if(call != NO_CALL)
                {
                    CsrBtHfgStatusIndicatorSetReqSend(prim->connectionId,
                                                CSR_BT_GATHERED_CALL_INDICATORS,
                                                CSR_BT_CALL_ACTIVE_NO_CALL_SETUP);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Active call indicators sent (no setup, active)...\n");

                    if(!con->audioOn && inst->autoAudioTransfer)
                    {
                        CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
                        snprintf(buf1, sizeof(buf1), "Call active, request to open audio...\n");
                        CsrStrLCat(buf, buf1, sizeof(buf));
                    }
                }
            }

#ifdef USE_HFG_RIL
            OpenRil(inst);
#endif
        }
        else
        {
            snprintf(buf, sizeof(buf), "Connection attempt failed, error code 0x%02X\n supplier 0x%02X", prim->resultCode, prim->resultSupplier);

            con->active = FALSE;
        }
        if(fail)
        {
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_HIGH_PRIO);

            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        }
        else
        {
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_SUCC_CON_DIALOG_UI, TEXT_SUCCESS_UCS2,
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_SUCC_CON_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_HIGH_PRIO);
        }

        if(inst->dialogShow) /*Dialog is shown already becasue its a outgoing con. Hide it*/
        {
            CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_CANCEL_CON_DIALOG_UI].displayHandle);
            inst->dialogShow = FALSE;
        }
        inst->state = stIdle;
        updateMainMenu(inst);
    }
}

/* Disconnect SLC */
static void handleDisconnectInd(hfgInstance_t *inst, CsrBtHfgDisconnectInd *prim)
{
    Connection_t *con;
    CsrBtDeviceAddr  addr;
    CsrUint8       i;
    CsrCharString buf[100];
    CsrCharString buf1[10] = TEXT_SUCCESS_STRING;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    if(inst->dialogShow)
        CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DISC_DIALOG_UI].displayHandle, CSR_UI_INPUTMODE_AUTO);

    CsrSchedTimerCancel(inst->discSLCGuard,NULL,NULL);

    con = getIdConnection(inst, prim->connectionId);
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

    if(prim->reasonCode == RFC_ABNORMAL_DISCONNECT && prim->reasonSupplier == CSR_BT_SUPPLIER_RFCOMM)
    {
        snprintf(buf, sizeof(buf),"Disconnected due to link-loss\n");
        CsrStrLCpy(buf1, TEXT_FAILED_STRING, sizeof(buf1));
    }
    else if(!prim->localTerminated)

    {
        snprintf(buf, sizeof(buf), "Disconnected with error code 0x%02X supplier 0x%02X\n", prim->reasonCode, prim->reasonSupplier);
    }
    else
    {
        snprintf(buf, sizeof(buf), "Disconnected from local device (code 0x%02X supplier 0x%02X)\n", prim->reasonCode, prim->reasonSupplier);
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DISC_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2(buf1),
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

    /* Show the CSR_BT_HFG_DISC_DIALOG_UI on the display                   */
    if(!inst->dialogShow) /*Remote device has disconnected the connection so Dialog UI has to be shown*/
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DISC_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_HIGH_PRIO);

/*    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DISC_DIALOG_UI].displayHandle);*/

#ifdef USE_HFG_RIL
    CloseRil(inst);
#endif
}

/* Incoming SCO loopback */
static void handleIncomingSco(CsrUint8 *data)
{
    CsrHciSendScoData(data);
}

/* Audio connection */
static void handleAudioInd(hfgInstance_t *inst, CsrBtHfgAudioConnectInd *prim)
{
    Connection_t *con;
    CsrCharString buf[50];
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    con = getIdConnection(inst, prim->connectionId);

    CsrSchedTimerCancel(inst->discScoGuard, NULL, NULL);

    /* Always notify */
    snprintf((char *)buf, sizeof(buf), "Audio has been turned %s", (((prim->resultCode == CSR_BT_RESULT_CODE_HFG_SUCCESS) && (prim->resultSupplier == CSR_BT_SUPPLIER_HFG)) ? "on" : "off"));

    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "NO_CONNECTION\n");
    }
    else
    {
        /* Setup SCO-over-HCI */
        if((prim->resultCode == CSR_BT_RESULT_CODE_HFG_SUCCESS) && (prim->resultSupplier == CSR_BT_SUPPLIER_HFG))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, " (gain %i)", con->speakerGain);
            CsrHciRegisterScoHandle(prim->scoHandle, (CsrHciScoHandlerFuncType)handleIncomingSco);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, " - using PCM slot %i\n", prim->pcmSlot);
        }

        /* Store settings */
        con->audioOn   = (((prim->resultCode == CSR_BT_RESULT_CODE_HFG_SUCCESS) && (prim->resultSupplier == CSR_BT_SUPPLIER_HFG)) ? TRUE : FALSE);
        con->scoHandle = prim->scoHandle;

#ifdef USE_HFG_AUDIO
        if (con->audioOn)
        {
            /* Audio is transferred into HF side. */
            ConnxHfgAudioStart(con->codecToUse == CSR_BT_WBS_MSBC_CODEC ? 
                               BT_HFG_AUDIO_16K_SAMPLE_RATE : BT_HFG_AUDIO_8K_SAMPLE_RATE);
        }
        else
        {
            /* Audio is transferred into AG side. */
            ConnxHfgAudioStop();
            if(con->pendingSLCDisconnect)
            {
                /* Suppose only Single-Handsfree is supported */
                con->pendingSLCDisconnect = FALSE;
                startDisconnecting(inst);
            }
        }
#endif
    }

    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

    if(!inst->dialogShow) /* If the dialog is already not shown*/
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
    else
        CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEFAULT_INFO_UI].displayHandle, CSR_UI_INPUTMODE_AUTO);

    inst->dialogShow = FALSE;
}

/* Audio settings indication */
static void handleStatusAudioInd(hfgInstance_t *inst, CsrBtHfgStatusAudioInd *prim)
{
    CsrCharString buf[600];

    switch(prim->audioType)
    {
        case CSR_BT_HFG_AUDIO_SCO_STATUS:
            {
                CsrBtHfgAudioScoStatus *audio;
                audio = (CsrBtHfgAudioScoStatus *)prim->audioSetting;

                CsrSnprintf(buf, 600, "SCO audio status indication for instance %u received:\n \
                                        linkType:       0x%02x\n \
                                        txInterval:     0x%02x\n \
                                        weSco:          0x%02x\n \
                                        rxPacketLength: 0x%04x\n \
                                        txPacketLength: 0x%04x\n \
                                        airMode:        0x%02x\n \
                                        resultCode:     0x%02x\n \
                                        resultSupplier: 0x%02x\n",
                                        prim->connectionId,
                                        audio->linkType,
                                        audio->txInterval,
                                        audio->weSco,
                                        audio->rxPacketLength,
                                        audio->txPacketLength,
                                        audio->airMode,
                                        audio->resultCode,
                                        audio->resultSupplier);
            }
            break;
        default:
            snprintf(buf, sizeof(buf), "Unknown audio status indication received, type 0x%02X\n",
                   prim->audioType);
            break;
    }
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

}

/* Answer call */
static void handleAnswerInd(hfgInstance_t *inst, CsrBtHfgAnswerInd *prim)
{
    Connection_t *con;
#ifndef USE_HFG_RIL
    CsrUint8 call;
#endif
    CsrCharString buf[200] = "";
    CsrCharString buf1[100] = "";

    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;

    con = getIdConnection(inst, prim->connectionId);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "Answer indication received for unknown connection\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
        return;
    }

    /* [QTI] Postpone to notify CIEV until the new CLCC is retrieved from MODEM. 
       Otherwise, CLCC status may well be mis-matched. */
#ifndef USE_HFG_RIL
    CsrBtHfgStatusIndicatorSetReqSend(prim->connectionId,
                                CSR_BT_GATHERED_CALL_INDICATORS,
                                CSR_BT_CALL_ACTIVE_NO_CALL_SETUP);
    snprintf(buf, sizeof(buf),  "Answer indication received, indicators to activate call sent...\n");

    /* Set incoming as active */
    call = callChangeStatus(inst, CS_INCOMING, CS_ACTIVE);
    if(call != NO_CALL)
    {
        snprintf(buf1, sizeof(buf1), "Incoming call is now active (index %i)\n", call);
        CsrStrLCat(buf, buf1, sizeof(buf));
    }
#endif

    /* Setup audio */
    if(!con->audioOn)
    {
        CsrBtHfgAudioConnectReqSend(prim->connectionId,PCM_SLOT, PCM_SLOT_REALLOCATE);
        snprintf(buf1, sizeof(buf1), "Hfg Audio Connect Req is sent!Wait for response\n");
        inputMode = CSR_UI_INPUTMODE_BLOCK;
        inst->dialogShow = TRUE;
        CsrStrLCat(buf, buf1, sizeof(buf));
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);

#ifdef USE_HFG_RIL
    ConnxHfgRilAnswer(inst->hfgRilHandle);
#endif
}

/* Reject call */
static void handleRejectInd(hfgInstance_t *inst, CsrBtHfgRejectInd *prim)
{
    Connection_t *con;
    /* [QTI] Fix KW issue#834067 through defining a appropriate buffer size. */
    CsrCharString buf[BUFFER_SIZE] = "";
#ifndef USE_HFG_RIL
    CsrCharString buf1[100] = "";
    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;
    CsrUint8 call;
#endif

    con = getIdConnection(inst, prim->connectionId);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "Reject received for unknown connection\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

        return;
    }

    /* [QTI] Postpone to notify CIEV until the new CLCC is retrieved from MODEM.
       Otherwise, CLCC status may well be mis-matched. */
#ifndef USE_HFG_RIL
    call = callUnsetStatus(inst, CS_ALERTING);
    if(call != NO_CALL)
    {
        snprintf(buf, sizeof(buf), "Outgoing/alerting call rejected (index %i)\n", call);
    }
    else
    {
        call = callUnsetStatus(inst, CS_DIALING);
        if(call != NO_CALL)
        {
            snprintf(buf, sizeof(buf), "Outgoing/dialing call rejected (index %i)\n", call);
        }
    }

    
    /* Incoming/ongoing call being rejected */
    /* Release active calls */
    if ( callGetStatus(inst,CS_ACTIVE) != NO_CALL)
    {
        do
        {
            call = callUnsetStatus(inst, CS_ACTIVE);
            if(call != NO_CALL)
            {
                snprintf(buf, sizeof(buf), "Released ongoing call (index %i)\n", call);
            }
        }while(call != NO_CALL);
    }
    /* At least one active call has been removed */
    /* Send the proper indication to the HF */
    if ( callGetStatus(inst,CS_HELD) == NO_CALL)
    { /* No held calls found  and no active calls found at this stage either */
           CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                   CSR_BT_CALL_STATUS_INDICATOR,
                                   CSR_BT_NO_CALL_ACTIVE_VALUE);
    }
    else
    { /* Held call(s) found */
           CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                   CSR_BT_CALL_HELD_INDICATOR,
                                   CSR_BT_CALL_HELD_NO_ACTIVE_CALL_VALUE);
    }
    /* Now reject incoming calls if any */
    if ( callGetStatus(inst,CS_INCOMING) != NO_CALL)
    {
        do
        {
            call = callUnsetStatus(inst, CS_INCOMING);
            if(call != NO_CALL)
            {
                snprintf(buf, sizeof(buf), "Rejected incoming call (index %i)\n", call);
            }
        }while(call != NO_CALL);
    }
    /* Put waiting call as incoming (if it exist) */
    call = callChangeStatus(inst, CS_WAITING, CS_INCOMING);
    if(call != NO_CALL)
    {
        /* The call setup indicator is not changed, so do not send an indicator */
        snprintf(buf1, sizeof(buf1), "Waiting call added and set as incoming (index %i)\n", call);
        CsrStrLCat(buf, buf1, sizeof(buf));
    }
    else
    {/* At this point no incoming call exists.. */
        CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                     CSR_BT_NO_CALL_SETUP_VALUE);
    }

    snprintf(buf1, sizeof(buf1), "Outgoing call rejected, status indicators set for no-active, etc...\n");
    CsrStrLCat(buf,buf1, sizeof(buf));

    /* are there any active calls left? Shouldn't be */
    call = callGetStatus(inst,CS_ACTIVE);
    /* Turn audio on or off */
    if(!con->audioOn &&
       (call != NO_CALL))
    {
        /* Audio was off, while active call existed; open it */
        CsrBtHfgAudioConnectReqSend(prim->connectionId,PCM_SLOT, PCM_SLOT_REALLOCATE);
        snprintf(buf1, sizeof(buf1), "Audio requested on...\n");
        inputMode = CSR_UI_INPUTMODE_BLOCK;
    }
    else if((con->audioOn) &&
            (call == NO_CALL))
    {
        /* Audio was on, close it */
        CsrBtHfgAudioDisconnectReqSend(prim->connectionId);
        snprintf(buf1, sizeof(buf1), "Audio requested off...\n");
        inputMode = CSR_UI_INPUTMODE_BLOCK;
    }
    if(inputMode == CSR_UI_INPUTMODE_BLOCK)
    {
        CsrStrLCat(buf, buf1, sizeof(buf));
        inst->dialogShow = TRUE;
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);
#else
    TerminateCall(inst);
#endif
}

static void handleCallHandlingBtrhInd(hfgInstance_t *inst,
                                      Connection_t *con,
                                      CsrBtHfgCallHandlingInd *prim)
{
    CsrUint8 call;
    CsrCharString buf[100];
    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;
    CsrCharString heading[30] = TEXT_SUCCESS_STRING;

    switch(prim->value)
    {
        case CSR_BT_BTRH_PUT_ON_HOLD:
            {
                call = callChangeStatus(inst, CS_INCOMING, CS_HELD);
                if(call != NO_CALL)
                {
                    snprintf(buf, sizeof(buf), "BTRH: Put incoming call on hold (index %i)\n",
                           call);

                    /* Stop ringing */
                    CsrBtHfgRingReqSend(con->index,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   0);

#ifdef USE_HFG_RIL
                    ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_BTRH_PUT_ON_HOLD, 0);
#endif

                    /* Call is held (in network) */
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_SUCCESS,
                                           CSR_BT_HFG_BTRH_INCOMING_ON_HOLD);

                    /* But is actually active... */

                    CsrBtHfgStatusIndicatorSetReqSend(prim->connectionId,
                                                CSR_BT_GATHERED_CALL_INDICATORS,
                                                CSR_BT_CALL_ACTIVE_NO_CALL_SETUP);
                    /* If no other active calls exist, close audio if open */
                    if ((con->audioOn) && (callGetStatus(inst, CS_ACTIVE) == NO_CALL))
                    {
                        CsrBtHfgAudioDisconnectReqSend(con->index);
                        inputMode = CSR_UI_INPUTMODE_BLOCK;
                        CsrStrLCpy(heading, TEXT_PLEASE_WAIT_STRING, sizeof(heading));
                    }

                }
                else
                {
                    snprintf(buf, sizeof(buf), "BTRH: Hold non-existing incoming call\n");
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_NO_NETWORK_SERVICE,
                                           CSR_BT_HFG_BTRH_IGNORE);
                }
                break;
            }

        case CSR_BT_BTRH_ACCEPT_INCOMING:
            {
                call = callChangeStatus(inst, CS_HELD, CS_ACTIVE);
                if(call != NO_CALL)
                {
                    snprintf(buf, sizeof(buf), "BTRH: Activate held call  (index %i)\n",
                           call);

#ifdef USE_HFG_RIL
                    ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_BTRH_ACCEPT_INCOMING, 0);
#endif

                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_SUCCESS,
                                           CSR_BT_HFG_BTRH_ACCEPT_INCOMING);
                    if(!con->audioOn)
                    {
                        CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
                        inputMode = CSR_UI_INPUTMODE_BLOCK;
                        CsrStrLCat(heading, TEXT_PLEASE_WAIT_STRING, sizeof(heading));
                        snprintf(buf, sizeof(buf), "Active calls now present, request audio on...\n");
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "BTRH: Accept non-existing held call\n");
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_NO_NETWORK_SERVICE,
                                           CSR_BT_HFG_BTRH_IGNORE);
                }
                break;
            }

        case CSR_BT_BTRH_REJECT_INCOMING:
            {
                call = callUnsetStatus(inst, CS_INCOMING);
                if (call == NO_CALL)
                {  /* There is no incoming alerting call: is there a held call? */
                    call = callUnsetStatus(inst, CS_HELD);
                }
                if(call != NO_CALL)
                {
                    snprintf(buf, sizeof(buf), "BTRH: Reject incoming call (index %i)\n", call);

                    /* Stop ringing */
                    CsrBtHfgRingReqSend(con->index,
                                   0,
                                   0,
                                   NULL,
                                   NULL,
                                   0);

#ifdef USE_HFG_RIL
                    ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_BTRH_REJECT_INCOMING, 0);
#endif

                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_SUCCESS,
                                           CSR_BT_HFG_BTRH_REJECT_INCOMING);
                    /* No calls are active anymore */
                    CsrBtHfgStatusIndicatorSetReqSend(prim->connectionId,
                                                 CSR_BT_CALL_STATUS_INDICATOR,
                                                 CSR_BT_NO_CALL_ACTIVE_VALUE);

                }
                else
                {
                    snprintf(buf, sizeof(buf), "BTRH: Reject non-existing incoming call\n");
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_NO_NETWORK_SERVICE,
                                           CSR_BT_HFG_BTRH_ACCEPT_INCOMING);
                }
                break;
            }

        case CSR_BT_BTRH_READ_STATUS:
            {
                /* If any calls are held, we must reply with a BTRH
                 * 'held' as well as ok.  If none on hold, just answer
                 * ok */
                call = callGetStatus(inst, CS_HELD);
                if(call != NO_CALL)
                {
                    snprintf(buf, sizeof(buf), "BTRH: Query status received - call is being held...\n");
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_SUCCESS,
                                           CSR_BT_HFG_BTRH_INCOMING_ON_HOLD);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "BTRH: Query status received - no current calls on hold...\n");
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_SUCCESS,
                                           CSR_BT_HFG_BTRH_IGNORE);
                }
                break;
            }
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(heading),
                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);
}

/* Call handling (CHLD/BTRH) */
static void handleCallHandlingInd(hfgInstance_t *inst, CsrBtHfgCallHandlingInd *prim)
{
    CsrUint8       waitingBefore;
    CsrUint8       waitingAfter;
    CsrUint8       activeBefore;
    CsrUint8       activeAfter;
    CsrUint8       call;
    Connection_t *con;
    /* [QTI] Fix KW issue#834068 ~ 834072 through defining a appropriate buffer size. */
    CsrCharString buf[BUFFER_SIZE], buf1[75];

    con = getIdConnection(inst, prim->connectionId);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "Call handling command received for unknown connection\n");
        /* [QTI] Fix KW issue#834074 ~ 834085 through adding return. */
        return;
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Call handling command received. Opcode %i, index %i\n",
           prim->value, prim->index);

    waitingBefore = callGetStatus(inst, CS_WAITING);
    activeBefore  = callGetStatus(inst, CS_ACTIVE);

    switch(prim->value)
    {
        case CSR_BT_RELEASE_ALL_HELD_CALL:
            {
                /* Send call handling response and handle the command
                 * afterwards */

                CsrUint8 releasedNofHeldCalls = 0;

                snprintf(buf, sizeof(buf), "Call handling command: Release all held calls\n");
                CsrBtHfgCallHandlingResSend(prim->connectionId,
                                       CSR_BT_CME_SUCCESS,
                                       CSR_BT_HFG_BTRH_IGNORE);

                /* Release held calls */
                do
                {
                    call = callUnsetStatus(inst, CS_HELD);
                    if(call != NO_CALL)
                    {
                        snprintf(buf1, sizeof(buf1), "Released held call (index %i)\n", call);
                        CsrStrLCat(buf, buf1, sizeof(buf));
                        releasedNofHeldCalls++;
                    }
                }
                while(call != NO_CALL);


                if (releasedNofHeldCalls != 0)
                {/* Update status flags! There are no more held calls and some held call(s) has/have been released */
                    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                     CSR_BT_CALL_HELD_INDICATOR,
                                     CSR_BT_NO_CALL_HELD_VALUE);
                }

                /* Notify if no more active calls */
                if(callGetStatus(inst, CS_ACTIVE) == NO_CALL)
                {
                    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                 CSR_BT_CALL_STATUS_INDICATOR,
                                                 CSR_BT_NO_CALL_ACTIVE_VALUE);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "No more active/held calls - no-call-active indicator sent...\n");
                }

                /* AT+CHLD=0 sets the User Determined User Busy (UDUB) status for incoming and waiting calls; that is
                   waiting calls are rejected! Do it */
                do
                {
                    call = callUnsetStatus(inst, CS_WAITING);
                    if(call != NO_CALL)
                    {
                        snprintf(buf1, sizeof(buf1), "Rejected waiting call (index %i)\n", call);
                        CsrStrLCat(buf, buf1, sizeof(buf));
                    }
                }
                while(call != NO_CALL);

#ifdef USE_HFG_RIL
                ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_RELEASE_ALL_HELD_CALL, 0);
#endif

                /* Check multiparty status of calls */
                handleMptyCallStatus(inst);
                break;
            }

        case CSR_BT_RELEASE_ACTIVE_ACCEPT:
            {
                /* Send call handling response and handle the command
                 * afterwards */
                snprintf(buf, sizeof(buf), "Call handling command: Release active call, accept held, waiting or incoming.\n");
                CsrBtHfgCallHandlingResSend(prim->connectionId,
                                       CSR_BT_CME_SUCCESS,
                                       CSR_BT_HFG_BTRH_IGNORE);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent call handling response...\n");

                /* Release active calls */
                do
                {
                    call = callUnsetStatus(inst, CS_ACTIVE);
                    if(call != NO_CALL)
                    {
                        snprintf(buf1, sizeof(buf1), "Released active call (index %i)\n", call);
                        CsrStrLCat(buf, buf1, sizeof(buf));
                    }
                }
                while(call != NO_CALL);

                /* Put waiting as active, otherwise put held as active, otherwise put incoming as active */
                call = callChangeStatus(inst, CS_HELD, CS_ACTIVE);
                if(call != NO_CALL)
                {
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Held call set as active (index %i)\n", call);

                    if (callGetStatus(inst, CS_HELD) == NO_CALL)
                    {/* No held calls left */
                     /* Send indication that there is an active call */
                       if (activeBefore == NO_CALL)
                       {
                           CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                        CSR_BT_CALL_STATUS_INDICATOR,
                                                        CSR_BT_CALL_ACTIVE_VALUE);
                       }
                       CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                    CSR_BT_CALL_HELD_INDICATOR,
                                                    CSR_BT_NO_CALL_HELD_VALUE);

                    }
                    else
                    {/* Still some held call(s) */
                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                    CSR_BT_CALL_HELD_INDICATOR,
                                                    CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE);
                    }
                }
                else
                {
                    call = callChangeStatus(inst, CS_WAITING, CS_ACTIVE);
                    if (call != NO_CALL)
                    {  
                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                 CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                                 CSR_BT_NO_CALL_SETUP_VALUE);

                        CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Waiting call set as active (index %i)\n", call);
                    }
                    else
                    {
                        call = callChangeStatus(inst, CS_INCOMING, CS_ACTIVE);
                        if(call != NO_CALL)
                        {
                            /* This is not totally in complience with the specification (3GPP 22.030),
                               but is implemented to be able to interopt with specific headsets,
                               which send down CHLD=1 even though no held or waiting calls are present,
                               only an incoming call */
                            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Incoming call set as active (index %i)\n", call);
                            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                 CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                                 CSR_BT_NO_CALL_SETUP_VALUE);
                        }
                        else
                        {
                           if (activeBefore != NO_CALL)
                           {
                               CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                            CSR_BT_CALL_STATUS_INDICATOR,
                                                            CSR_BT_NO_CALL_ACTIVE_VALUE);
                           }
                       }
                    }
                }

#ifdef USE_HFG_RIL
                ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_RELEASE_ACTIVE_ACCEPT, 0);
#endif
                /* Check multiparty status of calls */
                handleMptyCallStatus(inst);
                break;
            }

        case CSR_BT_RELEASE_SPECIFIED_CALL:
            {
                /* Send call handling response and handle the command
                 * afterwards */
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Call handling command: Release specified call (%i)\n", prim->index);
                CsrBtHfgCallHandlingResSend(prim->connectionId,
                                       CSR_BT_CME_SUCCESS,
                                       CSR_BT_HFG_BTRH_IGNORE);

                /* Find specific call */
                if((prim->index < MAX_CALLS) && (prim->index > 0) &&
                   (inst->calls[prim->index-1].isSet))
                {
#ifdef USE_HFG_RIL
                    ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_RELEASE_SPECIFIED_CALL, prim->index);
#endif

                    inst->calls[prim->index-1].isSet = FALSE;
                    snprintf(buf, sizeof(buf), "Released specific call (index %i)\n", prim->index);
                }

                /* Notify if no more active calls */
                if (callGetStatus(inst, CS_HELD) == NO_CALL)
                {/* No held calls left */
                   if ( callGetStatus(inst, CS_ACTIVE) == NO_CALL)
                        { /* No active calls */
                            CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "No held or active calls exist - send proper active-calls indicator...\n");
                            CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                                         CSR_BT_CALL_STATUS_INDICATOR,
                                                         CSR_BT_NO_CALL_ACTIVE_VALUE);
                        }

                }
                else
                {
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Held calls exist - send proper held-calls indicator...\n");
                    if ( callGetStatus(inst, CS_ACTIVE) == NO_CALL)
                        { /* No active calls */
                            CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                                         CSR_BT_CALL_HELD_INDICATOR,
                                                         CSR_BT_CALL_HELD_NO_ACTIVE_CALL_VALUE);
                        }
                        else
                        {/* Active call(s) exist */
                            CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                                         CSR_BT_CALL_HELD_INDICATOR,
                                                         CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE);
                        }
                }

                /* Check multiparty status of calls */
                handleMptyCallStatus(inst);
                break;
            }

        case CSR_BT_HOLD_ACTIVE_ACCEPT:
            {
                CsrUint8 pickup;

                /* Send call handling response and handle the command
                 * afterwards */
                snprintf(buf, sizeof(buf), "Call handling command: Hold active, accept held\n");

                /* Find currently held call */
                pickup = callGetStatus(inst, CS_HELD);
                if (NO_CALL == pickup)
                {
                    pickup = callGetStatus(inst, CS_WAITING);
                    if (NO_CALL == pickup)
                    {
                        pickup = callGetStatus(inst, CS_INCOMING);
                    }
                }
                call = callGetStatus(inst, CS_ACTIVE);


                if ((pickup != NO_CALL) || (call != NO_CALL))
                {
#ifdef USE_HFG_RIL
                    ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_HOLD_ACTIVE_ACCEPT, 0);
#endif

                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_SUCCESS,
                                           CSR_BT_HFG_BTRH_IGNORE);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent call handling response...\n");

                    if ((pickup != NO_CALL) && (call != NO_CALL))
                    {
                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                          CSR_BT_CALL_HELD_INDICATOR,
                                                          CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE);
                    }
                    else if (pickup != NO_CALL)
                    {
                        CsrUint8 heldCall;
                        heldCall = callGetStatus(inst, CS_HELD);
                        if (heldCall != NO_CALL)
                        {
                            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                              CSR_BT_CALL_HELD_INDICATOR,
                                                              CSR_BT_NO_CALL_HELD_VALUE);
                        }
                        CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                                          CSR_BT_CALL_STATUS_INDICATOR,
                                                          CSR_BT_CALL_ACTIVE_VALUE);
                    }
                    else
                    {
                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                          CSR_BT_CALL_HELD_INDICATOR,
                                                          CSR_BT_CALL_HELD_NO_ACTIVE_CALL_VALUE);
                        CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                                          CSR_BT_CALL_STATUS_INDICATOR,
                                                          CSR_BT_NO_CALL_ACTIVE_VALUE);
                    }

                    /* Put active calls on hold*/
                    do
                    {
                        call = callChangeStatus(inst, CS_ACTIVE, CS_HELD);
                        if(call != NO_CALL)
                        {
                            snprintf(buf1, sizeof(buf1), "Active call put on hold (index %i)\n", call);
                            CsrStrLCat(buf, buf1, sizeof(buf));
                        }
                    }
                    while(call != NO_CALL);

                    /* Put call on active */
                    if (pickup != NO_CALL)
                    {
                        inst->calls[pickup-1].status = CS_ACTIVE;
                        snprintf(buf1, sizeof(buf1), "Call (index %i) is now active.\n", pickup-1);
                        CsrStrLCat(buf, buf1, sizeof(buf));
                    }
                }
                else
                {
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_UNKNOWN,
                                           CSR_BT_HFG_BTRH_IGNORE);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Sent call handling response...\n");
                }
                /* Check multiparty status of calls */
                handleMptyCallStatus(inst);
                break;
            }
        case CSR_BT_REQUEST_PRIVATE_WITH_SPECIFIED:
            {
                CsrUint8 i;

                /* Send call handling response and handle the command
                 * afterwards */
                snprintf(buf, sizeof(buf), "Call handling command: Private consultation mode with call (%i)\n",
                       prim->index);

#ifdef USE_HFG_RIL
                ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_REQUEST_PRIVATE_WITH_SPECIFIED, prim->index);
#endif

                CsrBtHfgCallHandlingResSend(prim->connectionId,
                                       CSR_BT_CME_SUCCESS,
                                       CSR_BT_HFG_BTRH_IGNORE);

                /* Put all calls on hold, except specific which is to be active */
                for(i=0; i<MAX_CALLS; i++)
                {
                    if(inst->calls[i].isSet)
                    {
                        if((i) != prim->index - 1)
                        {
                            inst->calls[i].status = CS_HELD;
                            snprintf(buf1, sizeof(buf1), "Private consultation, put call on hold (index %i)\n", i);
                            CsrStrLCat(buf, buf1, sizeof(buf));
                            /* and send held indication... */
                            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                         CSR_BT_CALL_HELD_INDICATOR,
                                                         CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE);
                        }
                    }
                }

                /* Put specific call on active */
                if((prim->index < MAX_CALLS) && (prim->index > 0) &&
                   (inst->calls[prim->index-1].isSet))
                {
                    inst->calls[prim->index-1].status = CS_ACTIVE;
                    snprintf(buf1, sizeof(buf1), "Private consultation, set call as active (index %i)\n",
                           prim->index);
                    CsrStrLCat(buf, buf1, sizeof(buf));
                }
                /* Check multiparty status of calls */
                handleMptyCallStatus(inst);
                break;
            }

        case CSR_BT_ADD_CALL:
            {
                /* Send call handling response and handle the command
                 * afterwards */
                snprintf(buf, sizeof(buf), "Call handling command: Add call\n");
                CsrBtHfgCallHandlingResSend(prim->connectionId,
                                       CSR_BT_CME_SUCCESS,
                                       CSR_BT_HFG_BTRH_IGNORE);

                /* Put held on active */
                do
                {
                    call = callChangeStatus(inst, CS_HELD, CS_ACTIVE);
                    if(call != NO_CALL)
                    {
                        snprintf(buf1, sizeof(buf1), "Held call added and set as active (index %i)\n", call);
                        CsrStrLCat(buf, buf1, sizeof(buf));

#ifdef USE_HFG_RIL
                        ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_ADD_CALL, 0);
#endif

                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                                     CSR_BT_NO_CALL_SETUP_VALUE);
                        snprintf(buf1, sizeof(buf1), "No calls being setup, sent no-call-setup indicator...\n");
                        CsrStrLCat(buf, buf1, sizeof(buf));
                        if (callGetStatus(inst, CS_HELD) == NO_CALL)
                        {
                            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                         CSR_BT_CALL_HELD_INDICATOR,
                                                         CSR_BT_NO_CALL_SETUP_VALUE);
                            snprintf(buf1, sizeof(buf1),"No calls held, sent held-calls indicator...\n");
                            CsrStrLCat(buf, buf1, sizeof(buf));
                        }
                    }
                }
                while((call != NO_CALL) && (inst->calls[call - 1].mpty == MULTIPARTY_CALL));

                /* Check multiparty status of calls */
                handleMptyCallStatus(inst);
                break;
            }

        case CSR_BT_CONNECT_TWO_CALLS:
            {
                /* Send call handling response and handle the command
                 * afterwards */
                snprintf(buf, sizeof(buf), "Call handling command: Explicit call transfer\n");
                if((callGetStatus(inst, CS_ACTIVE) != NO_CALL) && (callGetStatus(inst, CS_HELD) != NO_CALL))
                {
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_SUCCESS,
                                           CSR_BT_HFG_BTRH_IGNORE);

                    /*  Explicit call transfer */
                    call = callUnsetStatus(inst, CS_ACTIVE);
                    if(call != NO_CALL)
                    {
                        snprintf(buf1, sizeof(buf1), "Explicit call transfer, dropped active call (index %i)\n", call);
                        CsrStrLCat(buf, buf1, sizeof(buf));
                    }
                    call = callUnsetStatus(inst, CS_HELD);
                    if(call != NO_CALL)
                    {
                        snprintf(buf1, sizeof(buf1),"Explicit call transfer, dropped held call (index %i)\n", call);
                        CsrStrLCat(buf, buf1, sizeof(buf));
                    }

#ifdef USE_HFG_RIL
                    ConnxHfgRilCallHandling(inst->hfgRilHandle, CONNX_HFG_CONNECT_TWO_CALLS, 0);
#endif

                    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                 CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                                 CSR_BT_NO_CALL_SETUP_VALUE);
                    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                 CSR_BT_CALL_HELD_INDICATOR,
                                                 CSR_BT_NO_CALL_HELD_VALUE);

                    /* Put waiting call as active (if it exist) */
                    call = callChangeStatus(inst, CS_WAITING, CS_ACTIVE);
                    if(call != NO_CALL)
                    {
                        snprintf(buf1, sizeof(buf1), "Waiting call added and set as active (index %i)\n", call);
                        CsrStrLCat(buf, buf1, sizeof(buf));
                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                     CSR_BT_CALL_STATUS_INDICATOR,
                                                     CSR_BT_CALL_ACTIVE_VALUE);
                    }

                    if (callGetStatus(inst, CS_ACTIVE) == NO_CALL)
                    {/* No active calls left */
                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                     CSR_BT_CALL_STATUS_INDICATOR,
                                                     CSR_BT_NO_CALL_ACTIVE_VALUE);
                    }

                }
                else
                {
                    /* there is NOT an active call AND a held call, which can be connected */
                    CsrBtHfgCallHandlingResSend(prim->connectionId,
                                           CSR_BT_CME_UNKNOWN,
                                           CSR_BT_HFG_BTRH_IGNORE);
                    snprintf(buf1, sizeof(buf1), "Sent call handling response (ERROR, no two calls exist to connect).\n");
                    CsrStrLCat(buf, buf1, sizeof(buf));
                }
                break;
            }

        case CSR_BT_BTRH_PUT_ON_HOLD:
            /* Fall through */
        case CSR_BT_BTRH_ACCEPT_INCOMING:
            /* Fall through */
        case CSR_BT_BTRH_REJECT_INCOMING:
            /* Fall through */
        case CSR_BT_BTRH_READ_STATUS:
            {
                /* Handle BTRHs in special function */
                handleCallHandlingBtrhInd(inst, con, prim);
                /* Check multiparty status of calls */
                handleMptyCallStatus(inst);
                /* Break out completely! */
                return;
            }

        default:
            {
                snprintf(buf, sizeof(buf), "Unknown call handling command received. Opcode %i, index %i\n",
                       prim->value, prim->index);
                CsrBtHfgCallHandlingResSend(prim->connectionId,
                                       CSR_BT_CME_NO_NETWORK_SERVICE,
                                       CSR_BT_HFG_BTRH_IGNORE);

                /* Break out completely */
                return;
            }
    }

    waitingAfter = callGetStatus(inst, CS_WAITING);
    activeAfter = callGetStatus(inst, CS_ACTIVE);

    if((waitingBefore != NO_CALL) &&
       (waitingAfter == NO_CALL))
    {
        /* We went from waiting calls to no waiting calls */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                     CSR_BT_NO_CALL_SETUP_VALUE);
    }

    if((activeBefore != NO_CALL) &&
       (activeAfter == NO_CALL) &&
       (con->audioOn))
    {
        /* We went from active call(s) to no active call(s),
         * so turn audio off */
        CsrBtHfgAudioDisconnectReqSend(con->index);
        snprintf(buf1, sizeof(buf1), "No more active calls, audio requested off...\n");
        CsrStrLCat(buf, buf1, sizeof(buf));
    }

    if((activeAfter != NO_CALL) &&
       (activeBefore == NO_CALL) &&
       !(con->audioOn))
    {
        /* We went from no active calls to active calls, so
         * request audio */
        CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
        snprintf(buf1, sizeof(buf1), "Active calls now present, request audio on...\n");
        CsrStrLCat(buf, buf1, sizeof(buf));
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

}

/* Dial indication */
static void handleDialInd(hfgInstance_t *inst, CsrBtHfgDialInd *prim)
{
    char *num;
    CsrUint8 call,numLen;
    Connection_t *con;
    CsrCharString buf[400] = "";
    CsrCharString buf1[100] = "";

    con = getIdConnection(inst, prim->connectionId);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf), "Dial command received for unknown connection\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
        return;
    }

    /* Number as string */
    if(prim->number != NULL)
    {
        num = (char *) prim->number;
        /* update last dialed number storage */
        /* [QTI] Fix the issue that the last digit is over-ridden in redial number. */
        CsrStrLCpy((char*)(inst->redialNumber), num, sizeof(inst->redialNumber));

        snprintf(buf, sizeof(buf), "Redial list updated\n");
    }
    else
    {
        if ( prim->command == CSR_BT_HFG_DIAL_REDIAL )
        {/* Redial command received */
           if ( CsrStrLen((char*)(inst->redialNumber)) == 0 )
           {/* But no last dialed number registered */
               CsrBtHfgDialResSend(con->index,CSR_BT_CME_NO_NETWORK_SERVICE);
               num = "<n/a>";
            snprintf(buf, sizeof(buf), "CSR_BT_HFG_DIAL_REDIAL command is receievd and redialNumber is NULL!!\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            return;
           }
           else
           {/* Use redial number */
                num =(char*)&(inst->redialNumber);
           }
        }
        else
        {
            num = "<n/a>";
        }
    }

    /* Any active calls should be placed on hold now */
    call = callChangeStatus(inst, CS_ACTIVE, CS_HELD);
    if(call != NO_CALL)
    {
        snprintf(buf1, sizeof(buf1), "Active call put on hold (index %i)\n", call);
        CsrStrLCat(buf, buf1, sizeof(buf));
    }

    /* Add new call to list */
    call = callGetUnused(inst);
    if(call != NO_CALL)
    {
        inst->calls[call].isSet = TRUE;
        inst->calls[call].dir = MOBILE_ORIGINATED;
        inst->calls[call].status = CS_DIALING;
        inst->calls[call].mode = VOICE;
        inst->calls[call].mpty = NOT_MULTIPARTY_CALL;

        if(num[0] == '+')
        {/*if + is present num type shall be 145*/
            inst->calls[call].numType = NUMBER_TYPE_INT;
        }
        else
        {
            inst->calls[call].numType = NUMBER_TYPE_STD;
        }

        numLen = (CsrUint8) CsrStrLen(num);
        CsrMemCpy(inst->calls[call].number,
               num,
               numLen+1);
        snprintf(buf1, sizeof(buf1), "Added new outgoing call to list (index %i)\n", (CsrUint8 )(call+1));
        CsrStrLCat(buf, buf1, sizeof(buf));
    }
    else
    {
        snprintf(buf1, sizeof(buf1), "Could not find empty call list entry for new call\n");
        CsrStrLCat(buf, buf1, sizeof(buf));
    }

    /* Show special menu */
    inst->current = getIndex(inst, con);
    inst->outgoingCall = TRUE;
    con->dialOk = FALSE;

    snprintf(buf1, sizeof(buf1), "Dial command received:");
    CsrStrLCat(buf, buf1, sizeof(buf));
    switch(prim->command)
    {
        case CSR_BT_HFG_DIAL_NUMBER:
            {
#ifdef USE_HFG_RIL
                if (!DialNumber(inst, num))
                    break;
#endif

                snprintf(buf1, sizeof(buf1), "Dial number: '%s'\n", num);
                CsrStrLCat(buf, buf1, sizeof(buf));
                inst->outgoingCall = TRUE;
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DIAL_INFO_DIALOG_UI, TEXT_SUCCESS_UCS2,
                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

                CsrUiDialogSetReqSend(inst->csrUiVar.displayesHandlers[CSR_BT_HFG_DIAL_INFO_DIALOG_UI].displayHandle,
                                                            TEXT_SUCCESS_UCS2, CONVERT_TEXT_STRING_2_UCS2(buf),
                                                            TEXT_OK_UCS2, NULL,
                                                            inst->csrUiVar.displayesHandlers[CSR_BT_HFG_DIAL_INFO_DIALOG_UI].sk1EventHandle, CSR_UI_DEFAULTACTION,
                                                            inst->csrUiVar.displayesHandlers[CSR_BT_HFG_DIAL_INFO_DIALOG_UI].backEventHandle, CSR_UI_DEFAULTACTION);

                CsrBtHfgShowUi(inst, CSR_BT_HFG_DIAL_INFO_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
                break;
            }

        case CSR_BT_HFG_DIAL_MEMORY:
            {
                /* Simulating that valid memory location number are 0-9 */
                if (strlen(num) <= 2)
                {
                    if (num[0] >= 0x30 && num[0] <= 0x39)
                    {
                        snprintf(buf, sizeof(buf), "Dial memory: '%s'\n", num);
                        inst->outgoingCall = TRUE;

                        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DIAL_INFO_DIALOG_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

                        CsrUiDialogSetReqSend(inst->csrUiVar.displayesHandlers[CSR_BT_HFG_DIAL_INFO_DIALOG_UI].displayHandle,
                                                                    TEXT_SUCCESS_UCS2, CONVERT_TEXT_STRING_2_UCS2(buf),
                                                                    TEXT_OK_UCS2, NULL,
                                                                    inst->csrUiVar.displayesHandlers[CSR_BT_HFG_DIAL_INFO_DIALOG_UI].sk1EventHandle, CSR_UI_DEFAULTACTION,
                                                                    inst->csrUiVar.displayesHandlers[CSR_BT_HFG_DIAL_INFO_DIALOG_UI].backEventHandle, CSR_UI_DEFAULTACTION);

                        CsrBtHfgShowUi(inst, CSR_BT_HFG_DIAL_INFO_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "The memory location number given (%c) is invalid. Valid numbers are 0-9\n \
                                                Returning an error message...\n ", num[0]);

                        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

                        CsrBtHfgDialResSend(con->index,CSR_BT_CME_UNKNOWN);
                    }
                }
                /* [QTI] Fix KW issue#834073 through defining a appropriate maximum number length. */
                else if (strlen(num) <= MAX_NUM_LEN)
                {
                    snprintf(buf, sizeof(buf), "The memory location number given (%s) is invalid. Valid numbers are 0-9\n \
                                                Returning an error message...\n", num);
                    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

                    CsrBtHfgDialResSend(con->index,CSR_BT_CME_UNKNOWN);
                }
                break;
            }

        case CSR_BT_HFG_DIAL_REDIAL:
            {
#ifdef USE_HFG_RIL
                if (!DialNumber(inst, num))
                    break;
#endif

                snprintf(buf, sizeof(buf), "Redial last number: '%s'\n",num);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DIAL_INFO_DIALOG_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DIAL_INFO_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

                break;
            }

        default:
            {
                snprintf(buf, sizeof(buf), "Unknown command '%i'\n",
                       prim->command);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
                return;
            }
    }
}

/* Call list */
static void handleCallListInd(hfgInstance_t *inst, CsrBtHfgCallListInd *prim)
{
#ifdef USE_HFG_RIL
    GetCallList(inst);
#else
    CsrUint8 i;
    CsrUint8 j;
    CsrBool last;

    /* Count number of calls to be able to detect the last one */
    j = 0;
    for(i=0; i<MAX_CALLS; i++)
    {
        if(inst->calls[i].isSet)
        {
            j++;
            last = (j+1 >= callGetCount(inst)
                    ? TRUE
                    : FALSE);

            /* Send entry */
            CsrBtHfgCallListResSend(prim->connectionId,
                               last,
                               (CsrUint8) (i+1),
                               inst->calls[i].dir,
                               inst->calls[i].status,
                               inst->calls[i].mode,
                               inst->calls[i].mpty,
                               (CsrCharString *) StringDup((char*)(inst->calls[i].number)),
                               inst->calls[i].numType,
                               CSR_BT_CME_SUCCESS);
        }
    }

    /* Send empty notification? */
    if(j == 0)
    {
        CsrBtHfgCallListResSend(prim->connectionId,
                           TRUE, 0, 0, 0, 0, 0, NULL, 0, CSR_BT_CME_SUCCESS);
    }
#endif
}

/* Handle HFG primitives */
void handleHfgPrim(hfgInstance_t *inst)
{
    CsrBtHfgPrim *msg;
    CsrUint16  *event;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrCharString buf[400] = "";
    CsrCharString buf1[60] = "";
#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
    CsrUint8 *parserCmdList = (CsrUint8*) CsrPmemAlloc(CSR_BT_HFG_APP_NUMBER_AT_CMD);
#endif
    msg   = (CsrBtHfgPrim*)inst->recvMsgP;
    event = (CsrBtHfgPrim*)msg;

#if 0
    /* Dialog is shown in BLOCK mode. so unblock it*/
    if(inst->dialogShow)
    {
        CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEFAULT_INFO_UI].displayHandle, CSR_UI_INPUTMODE_AUTO);
    }
#endif
    switch(*event)
    {
        case CSR_BT_HFG_DEACTIVATE_CFM:
            {
                CsrSchedTimerCancel(inst->deactGuard,NULL,NULL);

                initHfgData(inst);
                inst->state = stDeactivated;

/*                CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEFAULT_INFO_UI].displayHandle, CSR_UI_INPUTMODE_AUTO);*/
/*                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_HFG_DEACTIVATION_UCS2,*/
/*                                    TEXT_HFG_DEACTIVATED_UCS2, TEXT_OK_UCS2, NULL);*/
                CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEFAULT_INFO_UI].displayHandle);
/*                CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEACT_UI].displayHandle);*/

                updateMainMenu(inst);

                break;
            }

        case CSR_BT_HFG_SERVICE_CONNECT_IND:
            {
                CsrBtHfgServiceConnectInd *prim;
                prim = (CsrBtHfgServiceConnectInd*)msg;

                handleServiceConnectInd(inst, prim);

                CsrPmemFree(prim->serviceName);
            }
            break;

        case CSR_BT_HFG_DISCONNECT_IND:
            {
                CsrBtHfgDisconnectInd *prim;
                prim = (CsrBtHfgDisconnectInd*)msg;

                handleDisconnectInd(inst, prim);
            }
            break;

        case CSR_BT_HFG_AUDIO_DISCONNECT_CFM:
            {
                Connection_t *con;
                CsrBtHfgAudioDisconnectCfm *prim;
                prim = (CsrBtHfgAudioDisconnectCfm*)msg;
                con = getIdConnection(inst, prim->connectionId);

                /*CSR_BT_HFG_AUDIO_DISCONNECT_REQ is already sent and dialog is displayed in BLOCK mode*/
                CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DEFAULT_INFO_UI].displayHandle, CSR_UI_INPUTMODE_AUTO);

				/* [QTI] Fix KW issue#834090 through adding the check "con". */
                if (con != NULL)
                {
                    con->audioOn = FALSE;
                }

#ifdef USE_HFG_AUDIO
                ConnxHfgAudioStop();
#endif

                if((prim->resultCode == CSR_BT_RESULT_CODE_HFG_SUCCESS) && (prim->resultSupplier == CSR_BT_SUPPLIER_HFG))
                {
                    snprintf(buf, sizeof(buf), "Audio has been turned off\n");
                    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

                    CsrHciDeRegisterScoHandle(prim->scoHandle);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Audio OFF failed with resultCode=0x%04X and resultSupplier=0x%04X\n", prim->resultCode, prim->resultSupplier);
                    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                }
                inst->dialogShow = FALSE;
            }
            break;

        case CSR_BT_HFG_AUDIO_DISCONNECT_IND:
            {
                Connection_t *con;
                CsrBtHfgAudioDisconnectInd *prim;

                prim = (CsrBtHfgAudioDisconnectInd*)msg;
                con = getIdConnection(inst, prim->connectionId);

                CsrHciDeRegisterScoHandle(prim->scoHandle);

                /* [QTI] Fix KW issue#834091 through adding the check "con". */
                if (con != NULL)
                {
                    con->audioOn = FALSE;
                }

#ifdef USE_HFG_AUDIO
                ConnxHfgAudioStop();
#endif

                snprintf(buf, sizeof(buf), "Audio has been turned OFF by peer with reasonCode=0x%04X and reasonSupplier=0x%04X\n", prim->reasonCode, prim->reasonSupplier);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;

        case CSR_BT_HFG_AUDIO_CONNECT_IND:
        case CSR_BT_HFG_AUDIO_CONNECT_CFM:
            {
                CsrBtHfgAudioConnectInd *prim;
                prim = (CsrBtHfgAudioConnectInd*)msg;

                handleAudioInd(inst, prim);
            }
            break;

        case CSR_BT_HFG_AUDIO_ACCEPT_CONNECT_IND:
            {
                CsrBtHfgAudioAcceptConnectInd *prim;
                prim = (CsrBtHfgAudioAcceptConnectInd*)msg;

                snprintf(buf, sizeof(buf), "Received HFG Audio Accept Connect Ind, sending Response...\n");
                /* Simple reply */
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

                CsrBtHfgAudioAcceptConnectResSend(prim->connectionId, HCI_SUCCESS, NULL,
                                    PCM_SLOT,
                                    PCM_SLOT_REALLOCATE);
            }
            break;

        case CSR_BT_HFG_SECURITY_IN_CFM:
            {
                CsrBtHfgSecurityInCfm *prim;
                prim = (CsrBtHfgSecurityInCfm*)msg;

                /* Inform user */
                snprintf(buf, sizeof(buf), "Incoming security level changed, error code 0x%02X, supplier 0x%02X\n",
                       prim->resultCode, prim->resultSupplier);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;

        case CSR_BT_HFG_SECURITY_OUT_CFM:
            {
                CsrBtHfgSecurityOutCfm *prim;
                prim = (CsrBtHfgSecurityOutCfm*)msg;

                /* Inform user */
                snprintf(buf, sizeof(buf), "Outgoing security level changed, error code 0x%02X, supplier 0x%02X\n",
                       prim->resultCode, prim->resultSupplier);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

            }
            break;

        case CSR_BT_HFG_STATUS_LP_IND:
            {
                CsrBtHfgStatusLpInd *prim;
                CsrUint8 disconnectedIndex = 0;
                const char *modes[] =
                    {
                        "active",
                        "hold",
                        "sniff",
                        "disconnected"
                    };

                disconnectedIndex = 3;
                prim = (CsrBtHfgStatusLpInd*)msg;
                if (prim->currentMode == CSR_BT_LINK_STATUS_DISCONNECTED)
                {
                    prim->currentMode = disconnectedIndex;
                }
                if (prim->oldMode == CSR_BT_LINK_STATUS_DISCONNECTED)
                {
                    prim->oldMode = disconnectedIndex;
                }
                if (prim->wantedMode == CSR_BT_LINK_STATUS_DISCONNECTED)
                {
                    prim->wantedMode = disconnectedIndex;
                }
                /* Inform user */
                if ((prim->currentMode <= disconnectedIndex) &&
                    (prim->oldMode <= disconnectedIndex) &&
                    (prim->wantedMode <= disconnectedIndex))
                {
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Mode change: id: 0x%04X - code: 0x%02X (suppl 0x%02X) - cur: %s - old: %s - want: %s - %s request\n",
                           prim->connectionId,
                           prim->resultCode,
                           prim->resultSupplier,
                           modes[prim->currentMode],
                           modes[prim->oldMode],
                           modes[prim->wantedMode],
                           (prim->remoteReq ? "remote" : "local"));
                }
            }
            break;

        case CSR_BT_HFG_STATUS_AUDIO_IND:
            {
                CsrBtHfgStatusAudioInd *prim;
                prim = (CsrBtHfgStatusAudioInd*)msg;

                handleStatusAudioInd(inst, prim);

                CsrPmemFree(prim->audioSetting);
            }
            break;

        case CSR_BT_HFG_RING_CFM:
            {
                CsrBtHfgRingCfm *prim;
                prim = (CsrBtHfgRingCfm*)msg;

                /* Inform user */
                snprintf(buf, sizeof(buf), "Ringing completed for connection 0x%04X\n",
                       prim->connectionId);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;

        case CSR_BT_HFG_ANSWER_IND:
            {
                CsrBtHfgAnswerInd *prim;
                prim = (CsrBtHfgAnswerInd*)msg;

                handleAnswerInd(inst, prim);
            }
            break;

        case CSR_BT_HFG_REJECT_IND:
            {
                CsrBtHfgRejectInd *prim;
                prim = (CsrBtHfgRejectInd*)msg;

                handleRejectInd(inst, prim);
            }
            break;

        case CSR_BT_HFG_CALL_HANDLING_IND:
            {
                CsrBtHfgCallHandlingInd *prim;
                prim = (CsrBtHfgCallHandlingInd*)msg;

                handleCallHandlingInd(inst, prim);
            }
            break;

        case CSR_BT_HFG_DIAL_IND:
            {
                CsrBtHfgDialInd *prim;
                prim = (CsrBtHfgDialInd*)msg;

                handleDialInd(inst, prim);

                CsrPmemFree(prim->number);
            }
            break;

        case CSR_BT_HFG_SPEAKER_GAIN_IND:
            {
                CsrBtHfgSpeakerGainInd *prim;
                Connection_t *con;
                prim = (CsrBtHfgSpeakerGainInd*)msg;

                /* Inform user and store setting */
                con = getIdConnection(inst, prim->connectionId);
                if(con != NULL)
                {
                    con->speakerGain = prim->gain;
                }
                snprintf(buf, sizeof(buf), "Speaker gain set by headset to %i\n",
                       prim->gain);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;

        case CSR_BT_HFG_MIC_GAIN_IND:
            {
                CsrBtHfgMicGainInd *prim;
                Connection_t *con;
                prim = (CsrBtHfgMicGainInd*)msg;

                /* Inform user and store setting */
                con = getIdConnection(inst, prim->connectionId);
                if(con != NULL)
                {
                    con->micGain = prim->gain;
                }
                snprintf(buf, sizeof(buf), "Microphone gain set by headset to %i\n",
                       prim->gain);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;

        case CSR_BT_HFG_AT_CMD_IND:
            {
                CsrBtHfgAtCmdInd *prim;
                prim = (CsrBtHfgAtCmdInd*)msg;

                snprintf(buf, sizeof(buf), "Received raw AT command - cmee is %s:\n >>>%s<<<\n",
                       (prim->cmeeEnabled ? "on" : "off"), prim->command);
                if (inst->parserMode != CSR_BT_HFG_AT_MODE_FULL)
                {
                    snprintf(buf1, sizeof(buf1), "AT cmd received not handled as it should be...\n");
                }
                CsrStrLCat(buf, buf1, sizeof(buf));
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

                CsrPmemFree(prim->command);
            }
            break;

        case CSR_BT_HFG_OPERATOR_IND:
            {
#ifdef USE_HFG_RIL
                ConnxHfgRilGetCurrentOperatorSelection(inst->hfgRilHandle);
#else
                CsrBtHfgOperatorInd *prim;
                prim = (CsrBtHfgOperatorInd*)msg;

                /* Inform user and reply */
                snprintf(buf, sizeof(buf), "Received a operator indication, sending reply...\n");
                if (inst->copsSupport)
                {
                    CsrBtHfgOperatorResSend(prim->connectionId,
                                       3, /* mode always 3 */
                                       (CsrCharString *) StringDup(OPERATOR_NAME),
                                       CSR_BT_CME_SUCCESS);
                    snprintf(buf1, sizeof(buf1), "\tName: '%s', mode %i\n",
                               OPERATOR_NAME, 3);
                }
                else
                {
                    CsrBtHfgOperatorResSend(prim->connectionId,
                                       3, /* mode always 3 */
                                       NULL,
                                       CSR_BT_CME_OPERATION_NOT_SUPPORTED); /* No operator name support */
                    snprintf(buf1, sizeof(buf1), "\tOperator name not available\n");
                }
                CsrStrLCat(buf, buf1, sizeof(buf));
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
#endif
            }
            break;

        case CSR_BT_HFG_CALL_LIST_IND:
            {
                CsrBtHfgCallListInd *prim;
                prim = (CsrBtHfgCallListInd*)msg;
                if (inst->callListCmdSupport)
                {
                    handleCallListInd(inst, prim);
                }
                else
                {
                    CsrCharString *body = CsrPmemAlloc(10);
                    body[0] = '\r';
                    body[1] = '\n';
                    body[2] = 'E';
                    body[3] = 'R';
                    body[4] = 'R';
                    body[5] = 'O';
                    body[6] = 'R';
                    body[7] = '\r';
                    body[8] = '\n';
                    body[9] = 0;

                    snprintf(buf, sizeof(buf), "Call list indication not supported: answer ERROR\n");
                    CsrBtHfgAtCmdReqSend(prim->connectionId,body);
                    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
                }

            }
            break;

        case CSR_BT_HFG_SUBSCRIBER_NUMBER_IND:
            {
#ifdef USE_HFG_RIL
                GetSubscriberNum(inst);
#else
                CsrBtHfgSubscriberNumberInd *prim;
                prim = (CsrBtHfgSubscriberNumberInd*)msg;

                /* Send two numbers and inform user */
                snprintf(buf, sizeof(buf), "Subscriber-number indicator received, sending replies...\n \
                                                     \tNumber: '%s', number-type: %i, service: %i\n \
                                                     \tNumber: '%s', number-type: %i, service: %i\n",
                                                    SUB_NUMBER_1, NUMBER_TYPE_INT, VOICE,
                                                    SUB_NUMBER_2, NUMBER_TYPE_INT, VOICE);
                CsrBtHfgSubscriberNumberResSend(prim->connectionId,
                                           FALSE,
                                           (CsrCharString *) StringDup(SUB_NUMBER_1),
                                           NUMBER_TYPE_INT,
                                           VOICE,
                                           CSR_BT_CME_SUCCESS);

                CsrBtHfgSubscriberNumberResSend(prim->connectionId,
                                           TRUE,
                                           (CsrCharString *) StringDup(SUB_NUMBER_2),
                                           NUMBER_TYPE_INT,
                                           VOICE,
                                           CSR_BT_CME_SUCCESS);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
#endif
            }
            break;

        case CSR_BT_HFG_GENERATE_DTMF_IND:
            {
                CsrBtHfgGenerateDtmfInd *prim;
                prim = (CsrBtHfgGenerateDtmfInd*)msg;

                /* Inform user */
                snprintf(buf, sizeof(buf), "Received DTMF indication for tone '%c' (0x%02x)\n",
                       prim->dtmf, prim->dtmf);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

#ifdef USE_HFG_RIL
                GenerateDTMF(inst, (char)prim->dtmf);
#endif
            }
            break;

        case CSR_BT_HFG_NOISE_ECHO_IND:
            {
                CsrBtHfgNoiseEchoInd *prim;
                Connection_t *con;
                prim = (CsrBtHfgNoiseEchoInd*)msg;

                con = getIdConnection(inst, prim->connectionId);
                /* Inform user and store setting */
                /* [QTI] Fix KW issue#834092/834093 through adding the check "con". */
                if (con != NULL)
                {
                    if (prim->nrec == TRUE)
                    {
                        con->nrec = TRUE;
                        /* TRUE */
                    }
                    else
                    {
                        con->nrec = FALSE;
                    }
                    snprintf(buf, sizeof(buf), "Received noise/echo supported feature '%s'\n", (con->nrec ? "on" : "off"));
                }

                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;

        case CSR_BT_HFG_BT_INPUT_IND:
            {
                CsrBtHfgBtInputInd *prim;
                prim = (CsrBtHfgBtInputInd*)msg;

                /* Inform user, send response */
                if(prim->request == 1)
                {
                    CsrBtHfgBtInputResSend(prim->connectionId,
                                      CSR_BT_CME_SUCCESS,
                                      (CsrCharString *) StringDup(BINP_RES));
                    snprintf(buf, sizeof(buf), "Received a BINP 1 'voice tag' indication, sent number reply..\n");
                }
                else
                {
                    CsrBtHfgBtInputResSend(prim->connectionId,
                                      CSR_BT_CME_NO_NETWORK_SERVICE,
                                      NULL);
                    snprintf(buf, sizeof(buf), buf, "Received unknown BINP request '%i', sent error reply...\n", prim->request);
                }
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;

        case CSR_BT_HFG_VOICE_RECOG_IND:
            {
                CsrBtHfgVoiceRecogInd *prim;
                Connection_t *con;
                CsrCharString heading[30] = TEXT_SUCCESS_STRING;
                CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;

                prim = (CsrBtHfgVoiceRecogInd*)msg;

                /* Inform user and store setting */

                con = getIdConnection(inst, prim->connectionId);
                /* [QTI] Fix KW issue#834094/834095/834100 through removing redundant code. */
                if(con != NULL)
                {
                    con->voiceRecognition = prim->bvra;

                    if((!con->audioOn && con->voiceRecognition) ||
                       (con->audioOn && !con->voiceRecognition))
                    {
                        snprintf(buf1, sizeof(buf1), "Requesting audio %s for voice recognition...\n",
                               (con->voiceRecognition ? "on" : "off"));
                        inputMode = CSR_UI_INPUTMODE_BLOCK;
                        CsrStrLCat(heading, TEXT_PLEASE_WAIT_STRING, sizeof(heading));
                        if(con->voiceRecognition)
                        {
                            CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
                        }
                        else
                        {
                            CsrBtHfgAudioDisconnectReqSend(con->index);
                        }

                        CsrBtHfgVoiceRecogResSend(prim->connectionId, CSR_BT_CME_SUCCESS);
                    }
                    else
                    {
                        snprintf(buf1, sizeof(buf1), "%s voice recognition...\n", (con->voiceRecognition ? "Activating" : "Deactivating"));
                        CsrBtHfgVoiceRecogResSend(prim->connectionId, CSR_BT_CME_SUCCESS);
                    }
                    CsrStrLCat(buf, buf1, sizeof(buf));
                }
                else
                {
                    CsrBtHfgVoiceRecogResSend(prim->connectionId, CSR_BT_CME_OPERATION_NOT_ALLOWED);
                }
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(heading),
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, inputMode, CSR_BT_HFG_STD_PRIO);
            }
            break;
#ifdef CSR_BT_INSTALL_HFG_CONFIG_AUDIO
        case CSR_BT_HFG_CONFIG_ATCMD_HANDLING_CFM:
            {
                CsrBtHfgConfigAtcmdHandlingCfm *prim;

                prim = (CsrBtHfgConfigAtcmdHandlingCfm *)msg;

                if (!prim->result) /* HFG_SUCCESS */
                {
                    snprintf(buf, sizeof(buf), "AT cmd configuration successful! \n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "AT cmd configuration failed. Error code: %d\n",prim->result);
                }
                /* No matter what, update the local settings with the actual settings....*/
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

                CsrMemCpy(inst->parserCmdList,prim->bitwiseIndicators,CSR_BT_HFG_APP_NUMBER_AT_CMD);
                CsrPmemFree(prim->bitwiseIndicators);
            }
            break;
#endif
#ifdef CSR_BT_INSTALL_HFG_CONFIG_SINGLE_ATCMD
        case CSR_BT_HFG_CONFIG_SINGLE_ATCMD_CFM:
            {
                CsrBtHfgConfigSingleAtcmdCfm *prim;

                prim = (CsrBtHfgConfigSingleAtcmdCfm *)msg;

                if (!prim->result) /* HFG_SUCCESS */
                {
                    snprintf(buf, sizeof(buf), "Single AT cmd configuration successful! \n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Single AT cmd configuration failed. Error code: %d\n",prim->result);
                }
                /* We need to sync'e with the profile...*/
                CsrMemCpy(parserCmdList, inst->parserCmdList, CSR_BT_HFG_APP_NUMBER_AT_CMD);
                CsrBtHfgConfigAtcmdHandlingReqSend(CSR_BT_HFG_APP_IFACEQUEUE, parserCmdList, CSR_BT_HFG_APP_NUMBER_AT_CMD);
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
#endif
        case CSR_BT_HFG_HF_INDICATOR_VALUE_IND:
        {
            CsrBtHfgHfIndicatorValueInd *prim;
            Connection_t *con;
            RemoteHfIndicators *remHfInd;

            prim = (CsrBtHfgHfIndicatorValueInd*)msg;
            con = getIdConnection(inst, prim->connectionId);
            remHfInd = hfgFetchRemoteHfIndicator(inst, prim->indId);


            if((con != NULL) && (remHfInd != NULL))
            {
                remHfInd->value = prim->value;
                if(prim->indId == CSR_BT_HFP_ENHANCED_SAFETY_HF_IND)
                {
                    if(prim->value)
                    {
                        snprintf(buf, sizeof(buf), "Enhanced Safety Enabled Received \n");
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "Enhanced Safety Disabled Received \n");
                    }
                }
                else if(prim->indId == CSR_BT_HFP_BATTERY_LEVEL_HF_IND)
                {
                    snprintf(buf, sizeof(buf), "Battery Level %d Received\n", prim->value);
                }
            }
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

            break;
        }

#ifdef USE_HFG_AUDIO
        case CSR_BT_HFG_SELECTED_CODEC_IND:
        {
            CsrBtHfgSelectedCodecInd *prim;         
            Connection_t *con;

            prim = (CsrBtHfgSelectedCodecInd *)msg;
            con = getIdConnection(inst, prim->connectionId);
            /* [QTI] Fix KW issue#834096 through adding the check "con". */
            if(con != NULL)
            {
                con->codecToUse = prim->codecToUse;
            }

            break;
        }
#endif

        default:
            snprintf(buf, sizeof(buf), "Unknown HFG primitive received: 0x%04x,\n", *event);
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            CsrBtHfgFreeUpstreamMessageContents(CSR_BT_HFG_PRIM, event);
            break;
    }
}

#ifdef USE_HFG_RIL
void OpenRil(hfgInstance_t *inst)
{
    ConnxHfgRilRegisterInfo registerInfo;

    HFG_FUNC_ENTER();

    if (!inst)
        return;

    InitHfgRilRegisterInfo(&registerInfo, inst);

    inst->hfgRilHandle = ConnxHfgRilOpen(&registerInfo);

    ConnxHfgRilSetCallNotificationIndication(inst->hfgRilHandle, true);

    /* Enable call waiting notification. */
    ConnxHfgRilSetCallWaitingNotification(inst->hfgRilHandle, true);

    /* Retrieve current call list, after MCM is opened. */
    SendClccRequest(inst);

    HFG_FUNC_LEAVE();
}

void CloseRil(hfgInstance_t *inst)
{
    HFG_FUNC_ENTER();

    if (!inst)
        return;

    if (inst->hfgRilHandle != NULL)
    {
        ConnxHfgRilClose(inst->hfgRilHandle);

        inst->hfgRilHandle = NULL;
    }

    HFG_FUNC_LEAVE();
}

static void InitHfgRilRegisterInfo(ConnxHfgRilRegisterInfo *registerInfo, hfgInstance_t *inst)
{
    if (!registerInfo)
        return;

    CsrMemSet(registerInfo, 0, sizeof(ConnxHfgRilRegisterInfo));

    registerInfo->size                                  = sizeof(ConnxHfgRilRegisterInfo);
    registerInfo->context                               = (ConnxContext)inst;

    registerInfo->call_end_resp_cb                      = HfgRilCallEndRespCb;
    registerInfo->answer_resp_cb                        = HfgRilAnswerRespCb;
    registerInfo->get_subscriber_num_resp_cb            = HfgRilGetSubscriberNumRespCb;
    registerInfo->get_clcc_resp_cb                      = HfgRilGetCurrentCallListRespCb;
    registerInfo->set_call_notification_ind_resp_cb     = HfgRilSetCallNotificationIndRespCb;
    registerInfo->set_call_waiting_notification_resp_cb = HfgRilSetCallWaitingNotificationRespCb;
    registerInfo->generate_dtmf_resp_cb                 = HfgRilGenerateDtmfRespCb;
    registerInfo->call_handling_resp_cb                 = HfgRilCallHandlingRespCb;
    registerInfo->get_cops_resp_cb                      = HfgRilGetCopsRespCb;
    registerInfo->dial_resp_cb                          = HfgRilDialRespCb;

    registerInfo->voice_call_ind_cb                     = HfgRilVoiceCallIndCb;
}

static CsrBtHfgConnectionId GetHfgConnectionId(hfgInstance_t *inst)
{
    Connection_t *hfg_connection;

    if (!inst)
        return CSR_BT_HFG_CONNECTION_ALL;

    /* Only retrieve the 1st HFG connection if avaiable. */
    hfg_connection = &inst->conInst[0]; 

    /* [TODO] Is it necessary to get HFG connection information using LOCK/UNLOCK
       due to thread's synchronization? */
    return hfg_connection->index;
}

static void SetCallId(hfgInstance_t *inst, uint32_t call_id)
{
    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_id: %d"), __FUNCTION__, call_id));

    HFG_LOCK(inst);

    inst->call_id = call_id;

    HFG_UNLOCK(inst);
}

static void ClearCallId(hfgInstance_t *inst)
{
    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    HFG_LOCK(inst);

    inst->call_id = INVALID_HFG_CALL_ID;

    HFG_UNLOCK(inst);
}

void StoreCallList(hfgInstance_t *inst, ConnxHfgCallInfo *call_list, uint32_t call_count)
{
    CsrUint32 actual_call_count = CSRMIN(CONNX_COUNT_OF(inst->call_list), call_count);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    HFG_LOCK(inst);

    if (call_list && call_count)
    {
        CsrUint32 index;
        ConnxHfgCallInfo *dst, *src;

        for (index = 0; index < actual_call_count; index++)
        {
            dst = &inst->call_list[index];
            src = &call_list[index];

            dst->idx     = src->idx;
            dst->dir     = src->dir;
            dst->stat    = src->stat;
            dst->mode    = src->mode;
            dst->mpy     = src->mpy;
            dst->numType = src->numType;

            CsrStrLCpy(dst->number, src->number, sizeof(dst->number));

            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> index: %d, dir: %d, status: %d, mode: %d, multiparty: %d, numType: %d, number: '%s' "),
                           __FUNCTION__, dst->idx, dst->dir, dst->stat, dst->mode, dst->mpy, dst->numType, dst->number));

        }

        inst->call_count = call_count;
    }
    else
    {
        inst->call_count = 0;
    }

    HFG_UNLOCK(inst);
}

void ClearCallList(hfgInstance_t *inst)
{
    HFG_LOCK(inst);

    if (inst->call_count)
    {
        CsrUint32 index;
        ConnxHfgCallInfo *dst;
        ConnxHfgCallInfo *call_list = inst->call_list;
        CsrUint32 call_count = CONNX_COUNT_OF(inst->call_list);

        inst->call_count = 0;

        CsrMemSet(inst->call_list, 0, sizeof(inst->call_list));

        for (index = 0; index < call_count; index++)
        {
            dst = &call_list[index];

            dst->stat = CONNX_HFG_CALL_STATUS_UNKNOWN;
        }
    }

    HFG_UNLOCK(inst);
}

static void GetCallList(hfgInstance_t *inst)
{
    CsrBtHfgConnectionId connectionId = GetHfgConnectionId(inst);
    ConnxHfgCallInfo *dst;
    CsrUint8 index = 0;

    HFG_LOCK(inst);

    if (!inst->call_count)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> no call"), __FUNCTION__));

        CsrBtHfgCallListResSend(connectionId,
                                TRUE,
                                0,
                                0,
                                0,
                                0,
                                0,
                                NULL,
                                0,
                                CSR_BT_CME_SUCCESS);

        HFG_UNLOCK(inst);

        return;
    }

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, inst->call_count));

    for (index = 0; index < inst->call_count; index++)
    {
        dst = &inst->call_list[index];
    
        CsrBtHfgCallListResSend(connectionId,
                                (index == (inst->call_count - 1)) ? TRUE : FALSE,
                                (CsrUint8) (index + 1),
                                dst->dir,
                                dst->stat,
                                dst->mode,
                                dst->mpy,
                                (CsrCharString *) StringDup(dst->number),
                                dst->numType,
                                CSR_BT_CME_SUCCESS);
    }

    HFG_UNLOCK(inst);
}

CsrBool DialNumber(hfgInstance_t *inst, char *number)
{
    uint32_t result;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> number: %s "), __FUNCTION__, number ? number : ""));

    result = ConnxHfgRilDial(inst->hfgRilHandle, number);

    if (result == CONNX_HFG_RIL_RESULT_SUCCESS)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Dial OK"), __FUNCTION__));

        return TRUE;
    }
    else
    {
        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Dial fail, err: 0x%x"), __FUNCTION__, result));

        HandleDialResult(inst, CSR_BT_CME_AG_FAILURE);

        return FALSE;
    }
}

static void HandleDialResult(hfgInstance_t *inst, uint16_t cmee_result)
{
    CsrBtHfgConnectionId connectionId = GetHfgConnectionId(inst);
    CsrCharString buf[256];

    /* Only handle error. */
    if (cmee_result != CSR_BT_CME_SUCCESS)
    {
        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Notify dial fail, err: 0x%x"), __FUNCTION__, cmee_result));

        CsrBtHfgDialResSend(connectionId, cmee_result);

        snprintf(buf, sizeof(buf), "Dial fail, error: 0x%04x", cmee_result);

        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                          CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
    }
}

static void SendClccRequest(hfgInstance_t *inst)
{
    uint32_t result;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    result = ConnxHfgRilGetCurrentCallList(inst->hfgRilHandle);

    if (result != CONNX_HFG_RIL_RESULT_SUCCESS)
    {
        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Fail to send CLCC request, err: 0x%x"), __FUNCTION__, result));

        ClearCallList(inst);
    }
}

void TerminateCall(hfgInstance_t *inst)
{
    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_id: %d"), __FUNCTION__, inst->call_id));

    if (inst->call_id != INVALID_HFG_CALL_ID)
    {
        ConnxHfgRilCallEnd(inst->hfgRilHandle, inst->call_id);

        IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> Clear call id"), __FUNCTION__));

        ClearCallId(inst);
    }

    IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> Clear call list"), __FUNCTION__));

    ClearCallList(inst);
}

static void GenerateDTMF(hfgInstance_t *inst, char dtmf)
{
    uint32_t call_id = inst->call_id;    

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> dtmf: '%c', call_id: %d"), __FUNCTION__, dtmf, call_id));

    ConnxHfgRilGenerateDTMF(inst->hfgRilHandle, call_id, dtmf);
}

static void GetSubscriberNum(hfgInstance_t *inst)
{
    uint32_t result;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    result = ConnxHfgRilGetSubscriberNumInfo(inst->hfgRilHandle);

    if (result != CONNX_HFG_RIL_RESULT_SUCCESS)
    {
        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Get number fail, err: 0x%x"), __FUNCTION__, result));

        /* Only respond with NULL number. */
        CsrBtHfgSubscriberNumberResSend(CSR_BT_HFG_CONNECTION_ALL,
                                        TRUE,
                                        NULL,
                                        0,
                                        0,
                                        CSR_BT_CME_SUCCESS);
    }
}

static void StartAudio(hfgInstance_t *inst)
{
    Connection_t *con = NULL;

    con = getActiveConnection(inst);
    if (!con)
    {
        return;
    }

    if (!con->audioOn)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Start HF audio"), __FUNCTION__));

        CsrBtHfgAudioConnectReqSend(con->index, PCM_SLOT, PCM_SLOT_REALLOCATE);        
    }
}

static void StopAudio(hfgInstance_t *inst)
{
    Connection_t *con = NULL;

    con = getActiveConnection(inst);
    if (!con)
    {
        return;
    }

    if (con->audioOn)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Stop HF audio"), __FUNCTION__));

        CsrBtHfgAudioDisconnectReqSend(con->index);        
    }
}

static void HfgRilCallEndRespCb(ConnxContext context, uint16_t cmee_result)
{
    OUTPUT_HFG_RESULT(cmee_result);

    /* Synergy BT stack already respond with "OK" for "AT+CHUP". */
}

static void HfgRilAnswerRespCb(ConnxContext context, uint16_t cmee_result)
{
    OUTPUT_HFG_RESULT(cmee_result);

    /* Needn't handle, because Synergy BT stack already respond with "OK" for "ATA". */
}

static void HfgRilGetSubscriberNumRespCb(ConnxContext context, uint16_t cmee_result, const char *phone_number)
{
    hfgInstance_t *inst = (hfgInstance_t *)context;
    CsrBtHfgConnectionId connectionId = GetHfgConnectionId(inst);

    OUTPUT_HFG_RESULT(cmee_result);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> phone_number: %s"),
                   __FUNCTION__, phone_number ? phone_number : ""));

    /* Always respond success. */
    CsrBtHfgSubscriberNumberResSend(connectionId,
                                    TRUE,
                                    phone_number ? CsrStrDup(phone_number) : NULL,
                                    NUMBER_TYPE_INT,
                                    VOICE,
                                    CSR_BT_CME_SUCCESS);
}

static void HfgRilGetCurrentCallListRespCb(ConnxContext context, uint16_t cmee_result, ConnxHfgCallInfo *call_list, uint32_t call_count)
{
    hfgInstance_t *inst = (hfgInstance_t *)context;
    Connection_t *con = NULL;
    ConnxHfgCallInfo *cur_call = NULL;
    uint32_t index = 0;
    uint8_t call_status;

    CSR_UNUSED(cmee_result);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (!inst)
    {
        /* Invalid context. */
        return;
    }

    con = getActiveConnection(inst);
    if (!con)
    {
        /* NOT handle if HFP is disconnected. */
        return;
    }

    IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    if (!call_list || !call_count)
    {
        StopAudio(inst);

        HfgRilHandleEmptyCallList(inst);
        return;
    }

    StoreCallList(inst, call_list, call_count);

    StartAudio(inst);

    for (index = 0; index < call_count; index++)
    {
        cur_call = &call_list[index];
        call_status = cur_call->stat;

        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call idx: %d, status: %x"),
                       __FUNCTION__, cur_call->idx, call_status));

        switch (call_status)
        {
            case CONNX_HFG_CALL_STATUS_ACTIVE:
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call active"), __FUNCTION__));

                HfgRilHandleCallActiveStatus(inst, cur_call, call_count);
                break;
            }

            case CONNX_HFG_CALL_STATUS_HELD:
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call held"), __FUNCTION__));

                HfgRilHandleCallHeldStatus(inst, cur_call, call_count);
                break;
            }

            case CONNX_HFG_CALL_STATUS_OUTGOING:
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call outgoing"), __FUNCTION__));

                HfgRilHandleOutgoingCallStatus(inst, cur_call, call_count);
                break;
            }

            case CONNX_HFG_CALL_STATUS_ALERT:
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call outgoing alert"), __FUNCTION__));

                HfgRilHandleOutgoingCallAlertStatus(inst, cur_call, call_count);
                break;
            }

            case CONNX_HFG_CALL_STATUS_INCOMING:
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call incoming"), __FUNCTION__));

                HfgRilHandleIncomingCallStatus(inst, cur_call, call_count);
                break;
            }

            case CONNX_HFG_CALL_STATUS_WAITING:
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call waiting"), __FUNCTION__));

                HfgRilHandleCallWaitingStatus(inst, cur_call, call_count);
                break;
            }

            case CONNX_HFG_CALL_STATUS_END:
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call end"), __FUNCTION__));

                HfgRilHandleCallEndStatus(inst, cur_call, call_count);
                break;
            }

            default:
            {
                /* Unknown call status. */
                IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Call status unknown (%02x)"), __FUNCTION__, call_status));

                HfgRilHandleUnknownCallStatus(inst, cur_call, call_count);
                break;
            }
        }
    }
}

static void HfgRilSetCallNotificationIndRespCb(ConnxContext context, uint16_t cmee_result)
{
    CSR_UNUSED(context);
    CSR_UNUSED(cmee_result);

    /* Needn't handle, because Synergy BT stack already respond with "OK" for "AT+CLIP". */
}

static void HfgRilSetCallWaitingNotificationRespCb(ConnxContext context, uint16_t cmee_result)
{
    OUTPUT_HFG_RESULT(cmee_result);

    /* Needn't handle, because Synergy BT stack already respond with "OK" for "AT+CCWA". */
}

static void HfgRilGenerateDtmfRespCb(ConnxContext context, uint16_t cmee_result)
{
    OUTPUT_HFG_RESULT(cmee_result);

    /* Needn't handle, because Synergy BT stack already respond with "OK" for "AT+VTS". */
}

static void HfgRilCallHandlingRespCb(ConnxContext context, uint16_t cmee_result)
{
    OUTPUT_HFG_RESULT(cmee_result);

    /* TODO */
}

static void HfgRilGetCopsRespCb(ConnxContext context, uint16_t cmee_result, char *cops)
{
    hfgInstance_t *inst = (hfgInstance_t *)context;
    CsrBtHfgConnectionId connectionId = GetHfgConnectionId(inst);

    OUTPUT_HFG_RESULT(cmee_result);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> cops: %s"), __FUNCTION__, cops ? cops : ""));

    if (IS_HFG_CME_SUCCESS(cmee_result))
    {
        /* [QTI] Fix KW issue#834101 through adding the check "cops". */
        if (cops != NULL)
        {
            CsrBtHfgOperatorResSend(connectionId,
                                    3, /* mode always 3 */
                                    (CsrCharString *) StringDup(cops),
                                    CSR_BT_CME_SUCCESS);
        }
    }
    else
    {
        CsrBtHfgOperatorResSend(connectionId,
                                3, /* mode always 3 */
                                NULL,
                                CSR_BT_CME_OPERATION_NOT_SUPPORTED); /* No operator name support */
    }
}

static void HfgRilDialRespCb(ConnxContext context, uint16_t cmee_result, uint32_t call_id)
{
    hfgInstance_t *inst = (hfgInstance_t *)context;
    CsrBtHfgConnectionId connectionId = GetHfgConnectionId(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_id: %d"), __FUNCTION__, call_id));

    if (IS_HFG_CME_SUCCESS(cmee_result))
    {
        CsrBtHfgDialResSend(connectionId, CSR_BT_CME_SUCCESS);

        SetCallId(inst, call_id);
    }
    else
    {
        HandleDialResult(inst, cmee_result);

        ClearCallId(inst);
    }
}

static void HfgRilVoiceCallIndCb(ConnxContext context, uint16_t cmee_result, ConnxHfgCallInfo *call_list, uint32_t call_count)
{
    hfgInstance_t *inst = (hfgInstance_t *)context;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    /* [QTI] After getting MCM_VOICE_CALL_IND_V01 indication,
       client has to query the call info using MCM_VOICE_GET_CALLS_REQ_V01 */
    SendClccRequest(inst);
}

static void HfgRilHandleEmptyCallList(hfgInstance_t *inst)
{
    CsrUint8 call;
    CsrBool notifyNoCallSetup = FALSE;
    CsrBool notifyNoCallActive = FALSE;

    IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    ClearCallList(inst);

    do
    {
        /* Reject dialing call */
        call = callUnsetStatus(inst, CS_DIALING);
        if (call != NO_CALL)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Removed dialing call from list (index %i)"), __FUNCTION__, call));
            notifyNoCallSetup = TRUE;
            break;
        }

        /* Reject alerting call */
        call = callUnsetStatus(inst, CS_ALERTING);
        if (call != NO_CALL)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Removed alerting call from list (index %i)"), __FUNCTION__, call));
            notifyNoCallSetup = TRUE;
            break;
        }

        /* Incoming call terminated by remote side. */
        call = callUnsetStatus(inst, CS_INCOMING);
        if (call != NO_CALL)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Removed incoming call from list (index %i)"), __FUNCTION__, call));
            notifyNoCallSetup = TRUE;
            break;
        }

        /* Active call terminated */
        call = callUnsetStatus(inst, CS_ACTIVE);
        if(call != NO_CALL)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Removed active call from list (index %i)"), __FUNCTION__, call));
            notifyNoCallActive = TRUE;
            break;            
        }
    }
    while (0);

    if (notifyNoCallSetup)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Notify no_call_setup"), __FUNCTION__));

        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                          CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                          CSR_BT_NO_CALL_SETUP_VALUE); 
    }

   else if (notifyNoCallActive)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Notify no_call_active"), __FUNCTION__));

        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                          CSR_BT_CALL_STATUS_INDICATOR,
                                          CSR_BT_NO_CALL_ACTIVE_VALUE);
    }
}

static void HfgRilHandleCallActiveStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    CsrUint8 call;
    CsrBool callStatusChanged = FALSE;
    CsrBool notifyStatus = FALSE;
    CsrBtHfgConnectionId connectionId = GetHfgConnectionId(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (!cur_call || !call_count)
        return;

    /* Check whether the call's previous status is "incoming". */
    call = callChangeStatus(inst, CS_INCOMING, CS_ACTIVE);
    if (call != NO_CALL)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call %d, incoming -> active"), __FUNCTION__, call));
        callStatusChanged = TRUE;
    }
    else
    {
        /* Check whether the call's previous status is "alerting" or "dialing". */
        call = callChangeStatus(inst, CS_ALERTING, CS_ACTIVE);
        if (call != NO_CALL)
        {
            /* Change from "alerting" to "active". */
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call %d, alerting -> active"), __FUNCTION__, call));
            callStatusChanged = TRUE;
        }
        else
        {
            call = callChangeStatus(inst, CS_DIALING, CS_ACTIVE);
            if (call != NO_CALL)
            {
                /* Change from "dialing" to "active". */
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call %d, dialing -> active"), __FUNCTION__, call));
                callStatusChanged = TRUE;
            }
            else
            {
                /* [Fix] CR-2061619 Aready "active". */
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Call arleady active"), __FUNCTION__));
                SetCallId(inst, cur_call->idx);
            }
        }
    }

    notifyStatus = callStatusChanged && IS_SINGLE_CALL(call_count);

    if (callGetStatus(inst, CS_HELD) == NO_CALL)
    {
        if (notifyStatus)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Notify call active"), __FUNCTION__));

            CsrBtHfgStatusIndicatorSetReqSend(connectionId,
                                              CSR_BT_CALL_STATUS_INDICATOR,
                                              CSR_BT_CALL_ACTIVE_VALUE);
        }
    }
    else
    {
        if (notifyStatus)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Notify call held"), __FUNCTION__));

            CsrBtHfgStatusIndicatorSetReqSend(connectionId,
                                              CSR_BT_CALL_HELD_INDICATOR,
                                              CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE);
        }
    }

    if (notifyStatus)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Notify no call setup"), __FUNCTION__));

        CsrBtHfgStatusIndicatorSetReqSend(connectionId,
                                          CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                          CSR_BT_NO_CALL_SETUP_VALUE);
    }
}

static void HfgRilHandleCallHeldStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    if (!cur_call || !call_count)
        return;

    /* TODO */
}

static void HfgRilHandleOutgoingCallStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    CsrBool notifyStatus = IS_SINGLE_CALL(call_count);
    CsrUint8 call;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    if (!cur_call || !call_count)
        return;

    /* FIX-ME
     * Count 1 means no call. this is unreasonable */
    if (callGetCount(inst) <= 1)
    {
        /* Add new call to list */
        call = callGetUnused(inst);
        if (call == NO_CALL)
        {
            IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> NO unused call in list"), __FUNCTION__));
            return;
        }
        else
        {
            HFG_LOCK(inst);

            inst->calls[call].isSet   = TRUE;
            inst->calls[call].dir     = MOBILE_TERMINATED;
            inst->calls[call].status  = CS_DIALING;
            inst->calls[call].mode    = VOICE;
            inst->calls[call].mpty    = NOT_MULTIPARTY_CALL;
            inst->calls[call].numType = NUMBER_TYPE_INT;
            CsrStrLCat((CsrCharString *)inst->calls[call].number,
                       cur_call->number,
                       sizeof(inst->calls[call].number));
            inst->call_id             = cur_call->idx;

            HFG_UNLOCK(inst);
        }
    }

    if (notifyStatus)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Notify outgoing call setup"), __FUNCTION__));

        /* Only notify status indicator in the 1st call. */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                          CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                          CSR_BT_OUTGOING_CALL_SETUP_VALUE);
    }
}

static void HfgRilHandleOutgoingCallAlertStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    CsrUint8 call;
    CsrBool notifyStatus = IS_SINGLE_CALL(call_count);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    if (!cur_call || !call_count)
        return;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call alerting"), __FUNCTION__));

    /* Alerting */
    call = callChangeStatus(inst, CS_DIALING, CS_ALERTING);
    if (call != NO_CALL)
    {
        if (notifyStatus)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Notify outgoing alert, call: %d"), __FUNCTION__, call));

            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                              CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                              CSR_BT_OUTGOING_REMOTE_ALERT_VALUE);
        }
    }
}

static void HfgRilHandleIncomingCallStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    Connection_t *con;
    CsrUint8 call;
    char *newRingNumber = NULL;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    if (!cur_call || !call_count)
        return;

    newRingNumber = cur_call->number;

    SetCallId(inst, cur_call->idx);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call idx: %d, ring number: %s"),
                   __FUNCTION__, cur_call->idx, newRingNumber));

    con = getActiveConnection(inst);

    if (!con)
        return;

    call = callGetStatus(inst, CS_INCOMING);
    if (call != NO_CALL)
    {
        char *oldRingNumber = (char *)inst->calls[call - 1].number;

        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Already has incoming call: %d, old ring number: %s"),
                       __FUNCTION__, call, oldRingNumber));

        if (!CsrStrCmp(newRingNumber, oldRingNumber))
        {
            /* NOT handle the same incoming call. */
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Same ring number"), __FUNCTION__));
            return;
        }
    }
    else
    {
        /* Add new call to list */
        call = callGetUnused(inst);
        if (call == NO_CALL)
        {
            IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> NO unused call in list"), __FUNCTION__));
            return;
        }
        else
        {
            HFG_LOCK(inst);

            inst->calls[call].isSet   = TRUE;
            inst->calls[call].dir     = MOBILE_TERMINATED;
            inst->calls[call].status  = CS_INCOMING;
            inst->calls[call].mode    = VOICE;
            inst->calls[call].mpty    = NOT_MULTIPARTY_CALL;
            inst->calls[call].numType = NUMBER_TYPE_INT;
            CsrStrLCpy((CsrCharString *)inst->calls[call].number,
                       newRingNumber,
                       sizeof(inst->calls[call].number));

            HFG_UNLOCK(inst);
        }
    }

    if (callGetStatus(inst, CS_ACTIVE) == NO_CALL)
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> New incoming call: %d"), __FUNCTION__, call));

        /* No call active, so start normal ring */
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                          CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                          CSR_BT_INCOMING_CALL_SETUP_VALUE);

        if (con != NULL)
        {
            if (con->inbandRinging)
            {
                IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Inband ringing is enabled, sent audio open request..."), __FUNCTION__));

                CsrBtHfgAudioConnectReqSend(con->index, PCM_SLOT, PCM_SLOT_REALLOCATE);
            }

            CsrBtHfgRingReqSend(con->index,
                                RING_REP_RATE,
                                RING_NUM_OF_RINGS,
                                (CsrCharString *) CsrStrDup(newRingNumber),
                                (CsrCharString *) CsrStrDup((char*)inst->name),
                                NUMBER_TYPE_INT);
        }
    }
    else
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Set waiting for call: %d"), __FUNCTION__, call));

        HFG_LOCK(inst);

        /* Modify incoming call to be waiting */
        /* [QTI] Fix KW issue#834105 through adding the check "call". */
        if (call < MAX_CALLS)
        {
            inst->calls[call].status = CS_WAITING;
        }

        HFG_UNLOCK(inst);

        /* Call already active, so start call waiting */
        if (con != NULL)
        {
            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Sent call waiting notification..."), __FUNCTION__));

            CsrBtHfgCallWaitingReqSend(con->index,
                                       CsrStrDup(newRingNumber),
                                       CsrStrDup((char*)inst->name),
                                       NUMBER_TYPE_INT);
        }

        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Sent 'call-setup' status indicator..."), __FUNCTION__));
    
        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                          CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                          CSR_BT_INCOMING_CALL_SETUP_VALUE);
    }
}

static void HfgRilHandleCallWaitingStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    /* TODO */
}

static void HfgRilHandleCallEndStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    /* TODO */
}

static void HfgRilHandleUnknownCallStatus(hfgInstance_t *inst, ConnxHfgCallInfo *cur_call, uint32_t call_count)
{
    IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> call_count: %d"), __FUNCTION__, call_count));

    /* TODO */
}

#endif  /* USE_HFG_RIL */
