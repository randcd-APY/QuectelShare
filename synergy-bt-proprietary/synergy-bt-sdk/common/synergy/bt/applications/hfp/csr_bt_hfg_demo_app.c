/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
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
#include "csr_bt_tasks.h"
#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "csr_bt_hfg_lib.h"
#include "csr_bt_hfg_prim.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_prim.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_util.h"
#include "csr_hci_sco.h"
#include "csr_bt_platform.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_hfg_demo_app.h"
#include "csr_bt_sc_demo_app.h"
#include "csr_bccmd_prim.h"
#include "csr_bccmd_lib.h"
#include "csr_bt_hfg_app_ui_sef.h"
#include "csr_app_lib.h"
#include "csr_bt_hfg_app_util.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_gap_app_prim.h"
#include "csr_bt_gap_app_lib.h"

static const CsrBtHfgEventType csrHfgUiMsgHandlers[CSR_UI_PRIM_UPSTREAM_COUNT] =

{ /* Jump tabel to handle Upstream CSR_UI messages send from the Phone Emulator UI (CSR_UI)     */
    CsrBtHfgCsrUiUieCreateCfmHandler,               /* CSR_UI_UIE_CREATE_CFM                  */
    NULL,                                           /* CSR_UI_MENU_GET_CFM                    */
    NULL,                                           /* CSR_UI_MENU_GETITEM_CFM                */
    NULL,                                           /* CSR_UI_MENU_GETCURSOR_CFM              */
    NULL,                                           /* CSR_UI_EVENT_GET_CFM                   */
    NULL,                                           /* CSR_UI_DIALOG_GET_CFM                  */
    CsrBtHfgCsrUiInputdialogGetCfmHandler,          /* CSR_UI_INPUTDIALOG_GET_CFM             */
    NULL,                                           /* CSR_UI_IDLESCREEN_GET_CFM              */
    CsrBtHfgCsrUiDisplayGetHandleCfmHandler,   /* CSR_UI_DISPLAY_GETHANDLE_CFM           */
    NULL,                                           /* CSR_UI_KEYDOWN_IND                     */
    CsrBtHfgCsrUiEventIndHandler,                   /* CSR_UI_EVENT_IND                       */
};


/* Keypress timer service */
void clearConnection(hfgInstance_t *inst, Connection_t *con)
{
    CsrMemSet(con, 0, sizeof(Connection_t));

    con->index                 = CSR_BT_HFG_CONNECTION_ALL;
    con->connType              = CSR_BT_HFG_CONNECTION_UNKNOWN;
    con->scoHandle             = CSR_SCHED_QID_INVALID;
    con->active                = FALSE;
    con->audioOn               = FALSE;
    con->dialOk                = FALSE;
    con->nrec                  = (CsrBool)(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION    ? TRUE : FALSE);
    con->voiceRecognition      = (CsrBool)(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_VOICE_RECOGNITION ? TRUE : FALSE);
    con->inbandRinging         = (CsrBool)(inst->hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_INBAND_RINGING    ? TRUE : FALSE);
    con->speakerGain           = 7;
    con->micGain               = 7;
    con->hfSupportedFeatures   = 0;
    con->remoteVersion         = 0;
    con->instHfIndicators      = NULL;
    con->hfIndCount            = 0;
#ifdef USE_HFG_AUDIO
    con->pendingSLCDisconnect  = FALSE;
#endif
}

static void initHfgAppCsrUiInitDisplayesHandlers(CsrBtHfgCsrUiType *csrUiVar)
{
    CsrUint8 i;

    for (i = 0; i < NUMBER_OF_CSR_BT_HFG_APP_UI; i++)
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

static void initHfgAppCsrUiVar(CsrBtHfgCsrUiType *csrUiVar)
{ /* Initialize the csrUiVar under inst                                                  */
    csrUiVar->eventState     =CSR_BT_HFG_CREATE_SK1_EVENT;
    csrUiVar->uiIndex        = CSR_BT_HFG_DEFAULT_INFO_UI;
    csrUiVar->popupTimerId   = 0;
    csrUiVar->inHfgMenu     = FALSE;
    csrUiVar->goToMainmenu = FALSE;
    initHfgAppCsrUiInitDisplayesHandlers(csrUiVar);
}

#ifdef USE_HFG_RIL
void InitRil(hfgInstance_t *inst)
{
    ConnxLogSetting logSetting;

    /* [TODO] Init log in common place. */
    ConnxInitLog(BT_LOG_CONFIG_PATH,
                 BT_LOG_CONFIG_FILE,
                 BT_LOG_FILE,
                 &logSetting);

    ConnxHfgRilInit();

    CsrMutexCreate(&inst->mutexHandle);

    inst->hfgRilHandle = NULL;
    inst->call_id      = 0;
    inst->clccFlag     = FALSE;
    inst->clccTimeout  = DEFAULT_HFG_TIMEOUT;
    inst->clccTimerId  = CSR_SCHED_TID_INVALID;

    ClearCallList(inst);
}

void DeinitRil(hfgInstance_t *inst)
{
    CloseRil(inst);

    ConnxHfgRilDeinit();

    CsrMutexDestroy(&inst->mutexHandle);

    ConnxDeinitLog();
}
#endif

/* Instance initialiser */
void initHfgData(hfgInstance_t *inst)
{

    CsrUint16 i;
    CsrUint32 exclude_codec_negotiation = 0xFFFFFDFF;
    CsrCharString *parameter, *value;

    inst->state                 = stIdle;
    inst->recvMsgP              = NULL;
    inst->selectedDevice        = 0;
    inst->current               = NO_IDX;

    /* [QTI] Temporarily remove the feature of WBS "CSR_BT_HF_SUPPORT_CODEC_NEGOTIATION",
       it doesn't support well, might be caused by mdm9607.dtsi */
    inst->hfgSupportedFeatures  = CSR_BT_HFG_SUP_FEATURES & exclude_codec_negotiation;
    inst->remoteVersion         = 0;
    inst->text[0]               = '\0';
    inst->textLen               = 0;
    inst->name[0]               = '\0';
    inst->nameLen               = 0;
    inst->inputLength           = 0;
    inst->input[0]              = '\0';

    /* Setup default SCO settings */
    inst->sco.txBandwidth       = CSR_BT_ESCO_DEFAULT_CONNECT_TX_BANDWIDTH;
    inst->sco.rxBandwidth       = CSR_BT_ESCO_DEFAULT_CONNECT_RX_BANDWIDTH;
    inst->sco.maxLatency        = CSR_BT_ESCO_DEFAULT_CONNECT_MAX_LATENCY;
    inst->sco.voiceSettings     = CSR_BT_ESCO_DEFAULT_CONNECT_VOICE_SETTINGS;
    inst->sco.audioQuality      = CSR_BT_ESCO_DEFAULT_CONNECT_AUDIO_QUALITY;
    inst->sco.reTxEffort        = CSR_BT_ESCO_DEFAULT_CONNECT_RE_TX_EFFORT;

    /* Set indicators here and send to profile */
    inst->rfShield              = FALSE;
    inst->regStatus             = TRUE;
    inst->roamStatus            = FALSE;
    inst->signal                = CSR_BT_SIGNAL_STRENGTH_LEVEL_5;
    inst->battery               = CSR_BT_BATTERY_CHARGE_LEVEL_5;
    inst->deactGuard            = 0;
    inst->discSLCGuard          = 0;
    inst->discScoGuard          = 0;
    /* Setup connections */
    for(i=0;i<MAX_NUM_CONNECTION;++i)
    {
        clearConnection(inst, &(inst->conInst[i]));
        inst->conInst[i].address.nap = 0;
        inst->conInst[i].address.uap = 0;
        inst->conInst[i].address.lap = 0;
#ifdef USE_HFG_AUDIO
        inst->conInst[i].codecToUse = CSR_BT_WBS_CVSD_CODEC;
#endif
    }

    /* Setup call list */
    for(i=0; i<MAX_CALLS; i++)
    {
        inst->calls[i].isSet = FALSE;
        CsrMemSet(inst->calls[i].number,
               '\0',
               MAX_TEXT_LENGTH);
    }
    /* Set default at position 0 and set count to 1, so inquired
     * devices are added to list after default */
    for(i=0; i<MAX_DISCOVERY_RESULTS_IN_LIST; i++)
    {
        inst->bdAddrList[i].lap = 0;
        inst->bdAddrList[i].nap = 0;
        inst->bdAddrList[i].uap = 0;
    }

    inst->phandle = CSR_BT_HFG_APP_IFACEQUEUE;
    inst->callListCmdSupport = TRUE;
    inst->copsSupport        = TRUE;
    inst->autoAudioTransfer  = TRUE;

    /* redial number init */

    CsrMemSet(inst->redialNumber,'\0',MAX_TEXT_LENGTH);
    inst->parserMode = CSR_BT_HFG_AT_MODE_FULL;
    CsrMemSet(inst->parserCmdList,0xFF,CSR_BT_HFG_APP_NUMBER_AT_CMD);

    inst->callListCmdSupport = TRUE;
    inst->dialogShow = FALSE;
    inst->outgoingCall = FALSE;

    inst->suppHfIndicators[0].hfIndicatorID     = CSR_BT_HFP_ENHANCED_SAFETY_HF_IND;
    inst->suppHfIndicators[0].status            = CSR_BT_HFP_HF_INDICATOR_STATE_DISABLE;
    inst->suppHfIndicators[0].valueMax          = 1;
    inst->suppHfIndicators[0].valueMin          = 0;

    inst->suppHfIndicators[1].hfIndicatorID     = CSR_BT_HFP_BATTERY_LEVEL_HF_IND;
    inst->suppHfIndicators[1].status            = CSR_BT_HFP_HF_INDICATOR_STATE_ENABLE;
    inst->suppHfIndicators[1].valueMax          = 100;
    inst->suppHfIndicators[1].valueMin          = 0;
}

/* Initialize application (called by scheduler on startup) */
void CsrBtHfgAppInit(void **gash)
{
    hfgInstance_t *inst;
    CsrBtHfgHfIndicator *suppHfIndList;

    *gash = (void*)CsrPmemZalloc(sizeof(hfgInstance_t));
    inst = (hfgInstance_t*)*gash;

    CsrMemSet(inst, 0, sizeof(hfgInstance_t));

    /* Initialize instance */
    initHfgData(inst);
    initHfgAppCsrUiVar(&inst->csrUiVar);

    suppHfIndList = hfgBuildLocalHfIndicatorList(inst);

    /* Activate SC and HFG */
    CsrBtHfgActivateReqSendExt(inst->phandle,
                       inst->parserMode,
                       MAX_NUM_CONNECTION,
                       (CsrCharString *) StringDup(CSR_BT_HFG_SERVICE_NAME),
                       inst->hfgSupportedFeatures,
                       CSR_BT_HFG_CRH_SETUP,
                       CSR_BT_HFG_SETUP,
                       suppHfIndList, 
                       SUPP_HFG_INDICATORS_COUNT);

    /* Synchronize */
    syncSettings(inst);

    /* Start creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);

    CsrAppRegisterReqSend(CsrSchedTaskQueueGet(), TECH_BT, PROFILE_HFG);
#if 0    
    /* Testing CM_REGISTER_REQ with a specific server channel */
    CsrBtCmPublicRegisterReqSend(CsrSchedTaskQueueGet(), 0, 10);
#endif

#ifdef USE_HFG_AUDIO
    ConnxHfgAudioInit();
#endif

#ifdef USE_HFG_RIL
    InitRil(inst);
#endif
}

#ifdef ENABLE_SHUTDOWN
void CsrBtHfgAppDeinit(void **gash)
{
    CsrUint16 msg_type=0;
    void *msg_data=NULL;
    hfgInstance_t *inst;
    inst = (hfgInstance_t*)*gash;

    while(CsrSchedMessageGet(&msg_type, &msg_data))
    {
        switch (msg_type)
        {
            case CSR_BT_HFG_PRIM:
            {
                CsrBtHfgFreeUpstreamMessageContents(msg_type, msg_data);
                break;
            }
            case CSR_BCCMD_PRIM:
            {
                CsrBccmdFreeUpstreamMessageContents(CSR_BCCMD_PRIM, msg_data);
                break;
            }
        }
        CsrPmemFree(msg_data);
    }

#ifdef USE_HFG_AUDIO
    ConnxHfgAudioDeinit();
#endif

#ifdef USE_HFG_RIL
    DeinitRil(inst);
#endif

    CsrPmemFree(inst);
}
#else
void CsrBtHfgAppDeinit(void **gash)
{
    hfgInstance_t *inst = (hfgInstance_t*)*gash;

#ifdef USE_HFG_AUDIO
    ConnxHfgAudioDeinit();
#endif

#ifdef USE_HFG_RIL
    DeinitRil(inst);
#endif
}
#endif

/* Handle application message (called by scheduler on messages) */
void CsrBtHfgAppHandler(void * * gash)
{
    hfgInstance_t *inst;
    CsrUint16 event=0;
    /* [QTI] Fix KW issue#833856 through defining a appropriate buffer size. */
    CsrCharString buf[BUFFER_SIZE];

    inst = (hfgInstance_t *) *gash;

    /* Obtains a message from the CSR_BT_HFG_APP_IFACEQUEUE                                         */
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
#if 0
        case CSR_BT_CM_PRIM:
        {
            CsrBtCmPrim *cmPrim = (CsrBtCmPrim *) inst->recvMsgP;
            if (*cmPrim == CSR_BT_CM_REGISTER_CFM)
            {
                CsrBtCmRegisterCfm *prim = (CsrBtCmRegisterCfm *)cmPrim;

                if ((prim->resultCode == CSR_BT_RESULT_CODE_CM_SUCCESS) && 
                    (prim->resultSupplier == CSR_BT_SUPPLIER_CM))
                    
                {/* Success */
                    if (prim->serverChannel == 10)
                    {
                        snprintf(buf, sizeof(buf), "Reg succeess\n");
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "Wrong srv channel: %d\n", prim->serverChannel);
                    }
                    CsrBtCmUnRegisterReqSend(prim->serverChannel);
                }
                else
                {/* Error */
                    snprintf(buf, sizeof(buf), "Reg failed; error: 0x%X\n", prim->resultCode);
                }
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
        }
#endif

        case CSR_APP_PRIM:
        {
            CsrPrim *type = inst->recvMsgP;
            getActiveConnection(inst);

            if (*type == CSR_APP_TAKE_CONTROL_IND)
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HFG, FALSE, "Received Take Control Ind");
                CsrBtGapAppGetSelectedDeviceReqSend(CsrSchedTaskQueueGet());

                updateMainMenu(inst);

                CsrBtHfgShowUi(inst, CSR_BT_HFG_MAIN_MENU_UI, CSR_UI_INPUTMODE_AUTO, 1);
                inst->csrUiVar.inHfgMenu = TRUE;
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
            CsrUint8 i;
            
            if (*type == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *msg = (CsrBtGapAppGetSelectedDeviceCfm *) inst->recvMsgP;
                for(i=0;i<MAX_NUM_CONNECTION;++i)
                {
                    if (inst->conInst[i].active == FALSE)
                    {
                        inst->conInst[i].address.nap = msg->deviceAddr.nap;
                        inst->conInst[i].address.uap = msg->deviceAddr.uap;
                        inst->conInst[i].address.lap = msg->deviceAddr.lap;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_HFG_APP", 0, *type, "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtHfgAppHandler");
            }
            break;
        }

        case CSR_BT_HFG_PRIM:
        {
            handleHfgPrim(inst);
            break;
        }

        case CSR_UI_PRIM:
        {
            CsrUiPrim * primType = (CsrUiPrim *) inst->recvMsgP;
            CsrUint16   index        = (CsrUint16)(*primType - CSR_UI_PRIM_UPSTREAM_LOWEST);

            if (index < CSR_UI_PRIM_UPSTREAM_COUNT && csrHfgUiMsgHandlers[index] != NULL)
            { /* Call the state event function which handle this incoming CSR_UI event message*/
                csrHfgUiMsgHandlers[index](inst);
            }
            else
            { /* An unexpected CSR_UI message is received                                     */
                CsrGeneralException("CSR_APP", 0, *primType,
                                    "Did not handle CSR_UI Upstream message");
            }
            break;
        }
        case CSR_BCCMD_PRIM:
        {
            CsrPrim *type = inst->recvMsgP;

            CsrBccmdFreeUpstreamMessageContents(CSR_BCCMD_PRIM, type);
            break;
        }
        default:
        {
            snprintf(buf, sizeof(buf), "Unknown primitive received: 0x%04x,\n", event);
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            break;
        }

    }

    CsrPmemFree(inst->recvMsgP);
}



void applicationUsage(char *col, char *des, char *opt)
{
}

char* applicationCmdLineParse(int ch, char *optionArgument)
{
    return NULL;
}

void applicationSetup(void)
{
}

char* applicationExtraOptions(void)
{
    return "";
}
