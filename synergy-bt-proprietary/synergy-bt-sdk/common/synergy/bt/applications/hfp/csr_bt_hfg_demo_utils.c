/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
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
#include "csr_bt_sc_prim.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_util.h"
#include "csr_hci_sco.h"
#include "csr_bt_platform.h"
#include "csr_bt_hfg_demo_app.h"

/* Utility: Get call status as string */
char *callGetStringStatus(hfgInstance_t *inst, CsrUint8 index)
{
    char *status;

    if(index < MAX_CALLS)
    {
        switch(inst->calls[index].status)
        {
            case CS_ACTIVE:
                status = "Active";
                break;

            case CS_HELD:
                status = "On hold";
                break;

            case CS_DIALING:
                status = "Dialing";
                break;

            case CS_ALERTING:
                status = "Alerting";
                break;

            case CS_INCOMING:
                status = "Incoming";
                break;

            case CS_WAITING:
                status = "Waiting";
                break;

            default:
                status = "<undefined>";
                break;
        }
    }
    else
    {
        status = "<invalid>";
    }
    return status;
}

/* Utility: Get next CSR_UNUSED call index */
CsrUint8 callGetUnused(hfgInstance_t *inst)
{
    CsrUint8 i;
    for(i=0; i<MAX_CALLS; i++)
    {
        if(!(inst->calls[i].isSet))
        {
            return i;
        }
    }
    return NO_CALL;
}

/* Utility: Count number of present calls */
CsrUint8 callGetCount(hfgInstance_t *inst)
{
    CsrUint8 i;
    CsrUint8 j;

    j=0;
    for(i=0; i<MAX_CALLS; i++)
    {
        if(inst->calls[i].isSet)
        {
            j++;
        }
    }
    return j+1;
}

/* Utility: Return index given call status */
CsrUint8 callGetStatus(hfgInstance_t *inst, CallStatus_t st)
{
    CsrUint8 i;
    for(i=0; i<MAX_CALLS; i++)
    {
        if(inst->calls[i].isSet &&
           (inst->calls[i].status == st))
        {
            return i+1;
        }
    }
    return NO_CALL;
}

/* Utility: Change a single call status */
CsrUint8 callChangeStatus(hfgInstance_t *inst, CallStatus_t old, CallStatus_t new)
{
    CsrUint8 i;
    for(i=0; i<MAX_CALLS; i++)
    {
        if(inst->calls[i].isSet && (inst->calls[i].status == old))
        {

#ifdef USE_HFG_RIL
            HFG_LOCK(inst);
#endif

            inst->calls[i].status = new;

#ifdef USE_HFG_RIL
            HFG_UNLOCK(inst);
#endif

            return i+1;
        }
    }
    return NO_CALL;
}

/* Utility: Release (unset) a single call based on status */
CsrUint8 callUnsetStatus(hfgInstance_t *inst, CallStatus_t st)
{
    CsrUint8 i;
    for(i=0; i<MAX_CALLS; i++)
    {
        if(inst->calls[i].isSet && (inst->calls[i].status == st))
        {
#ifdef USE_HFG_RIL
            HFG_LOCK(inst);
#endif

            inst->calls[i].isSet = FALSE;
            inst->calls[i].number[0] = '\0';

#ifdef USE_HFG_RIL
            HFG_UNLOCK(inst);
#endif

            return i+1;
        }
    }
    return NO_CALL;
}

/* Utility: Release (unset) a single call based on index */
void callUnsetIndex(hfgInstance_t *inst, CsrUint8 i)
{
    if(i < MAX_CALLS)
    {
#ifdef USE_HFG_RIL
        HFG_LOCK(inst);
#endif

        inst->calls[i].isSet = FALSE;
        inst->calls[i].status = CS_ACTIVE; /*0 This is the default value, since its used with isSet, there is no problem*/
        inst->calls[i].number[0] = '\0';

#ifdef USE_HFG_RIL
        HFG_UNLOCK(inst);
#endif
    }
}

/* Utility function: Return pointer to active connection */
Connection_t *getActiveConnection(hfgInstance_t *inst)
{
    /* [QTI] Fix KW issue#834057 ~ 834061/834604/833544 ~ 833559/833852 ~ 833854 through adjusting the condition. */
    if((inst->current == NO_IDX) ||
       (inst->current >= MAX_NUM_CONNECTION))
    {
        return NULL;
    }
    return &(inst->conInst[inst->current]);
}

/* Utility function: Return pointer to connection given instance index */
Connection_t *getIdConnection(hfgInstance_t *inst, CsrBtHfgConnectionId id)
{
    CsrUint8 i;

    for(i=0; i<MAX_NUM_CONNECTION; i++)
    {
        if(inst->conInst[i].index == id)
        {
            return &(inst->conInst[i]);
        }
    }
    return NULL;
}

/* Utility function: Return pointer to connection given address */
Connection_t *getAddrConnection(hfgInstance_t *inst, CsrBtDeviceAddr *addr)
{
    CsrUint8 i;

    for(i=0; i<MAX_NUM_CONNECTION; i++)
    {
        if((inst->conInst[i].address.nap == addr->nap) &&
           (inst->conInst[i].address.uap == addr->uap) &&
           (inst->conInst[i].address.lap == addr->lap))
        {
            return &(inst->conInst[i]);
        }
    }
    return NULL;
}


/* Utility function: Return CSR_UNUSED connection */
Connection_t *getUnusedConnection(hfgInstance_t *inst)
{
    CsrUint8 i;
    for(i=0; i<MAX_NUM_CONNECTION; i++)
    {
        if(!(inst->conInst[i].active))
        {
            return &(inst->conInst[i]);
        }
    }
    return NULL;
}

/* Utility function: Return index given instance */
CsrUint8 getIndex(hfgInstance_t *inst, Connection_t *con)
{
    CsrUint8 i;
    for(i=0; i<MAX_NUM_CONNECTION; i++)
    {
        if(&(inst->conInst[i]) == con)
        {
            return i;
        }
    }
    return NO_IDX;
}

/* Utility function: Return number of active connections */
CsrUint8 getNumActive(hfgInstance_t *inst)
{
    CsrUint8 i;
    CsrUint8 j;

    j = 0;
    for(i=0; i<MAX_NUM_CONNECTION; i++)
    {
        if(inst->conInst[i].active)
        {
            j++;
        }
    }
    return j;
}


/* Initialize profile by synching local indicators */
void syncSettings(hfgInstance_t *inst)
{
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_SERVICE_INDICATOR,
                                 (CsrUint8)(inst->regStatus
                                           ? CSR_BT_SERVICE_PRESENT_VALUE
                                           : CSR_BT_NO_SERVICE_VALUE));
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_ROAM_INDICATOR,
                                 (CsrUint8)(inst->roamStatus
                                           ? CSR_BT_ROAM_ON
                                           : CSR_BT_ROAM_OFF));
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_SIGNAL_STRENGTH_INDICATOR,
                                 inst->signal);
    CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                 CSR_BT_BATTERY_CHARGE_INDICATOR,
                                 inst->battery);

}

#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
/* Send our SCO settings to profile */
void syncScoSettings(hfgInstance_t *inst)
{
    CsrUint32 *txBandwidth;
    CsrUint32 *rxBandwidth;
    CsrUint16 *maxLatency;
    CsrUint16 *voiceSettings;
    hci_pkt_type_t *audioQuality;
    CsrUint8 *reTxEffort;

    txBandwidth = CsrPmemAlloc(sizeof(CsrUint32));
    *txBandwidth = inst->sco.txBandwidth;
    CsrBtHfgConfigAudioReqSend(CSR_BT_HFG_CONNECTION_ALL,
                          CSR_BT_HFG_AUDIO_TX_BANDWIDTH,
                          (CsrUint8 *)txBandwidth,
                          sizeof(CsrUint32));

    rxBandwidth = CsrPmemAlloc(sizeof(CsrUint32));
    *rxBandwidth = inst->sco.rxBandwidth;
    CsrBtHfgConfigAudioReqSend(CSR_BT_HFG_CONNECTION_ALL,
                          CSR_BT_HFG_AUDIO_RX_BANDWIDTH,
                          (CsrUint8 *)rxBandwidth,
                          sizeof(CsrUint32));

    maxLatency = CsrPmemAlloc(sizeof(CsrUint16));
    *maxLatency = inst->sco.maxLatency;
    CsrBtHfgConfigAudioReqSend(CSR_BT_HFG_CONNECTION_ALL,
                          CSR_BT_HFG_AUDIO_MAX_LATENCY,
                          (CsrUint8 *)maxLatency,
                          sizeof(CsrUint16));

    voiceSettings = CsrPmemAlloc(sizeof(CsrUint16));
    *voiceSettings = inst->sco.voiceSettings;
    CsrBtHfgConfigAudioReqSend(CSR_BT_HFG_CONNECTION_ALL,
                          CSR_BT_HFG_AUDIO_VOICE_SETTINGS,
                          (CsrUint8 *)voiceSettings,
                          sizeof(CsrUint16));

    audioQuality = CsrPmemAlloc(sizeof(CsrUint16));
    *audioQuality = inst->sco.audioQuality;
    CsrBtHfgConfigAudioReqSend(CSR_BT_HFG_CONNECTION_ALL,
                          CSR_BT_HFG_AUDIO_SUP_PACKETS,
                          (CsrUint8 *)audioQuality,
                          sizeof(CsrUint16));

    reTxEffort = CsrPmemAlloc(sizeof(CsrUint16));
    *reTxEffort = inst->sco.reTxEffort;
    CsrBtHfgConfigAudioReqSend(CSR_BT_HFG_CONNECTION_ALL,
                          CSR_BT_HFG_AUDIO_RETRANSMISSION,
                          reTxEffort,
                          sizeof(CsrUint16));
}
#endif

/* Utility function: Duplicate a string */
CsrUint8 *StringDup(char *str)
{
    CsrUint8 *res;
    CsrUint32 len;

    if (!str)
        return NULL;

    /* Copy */
    len = CsrStrLen(str) + 1;
    res = CsrPmemAlloc(len);
    CsrMemCpy(res, str, len);
    return res;
}

CsrBtHfgHfIndicator *hfgFetchLocalHfIndicator(hfgInstance_t *inst, CsrBtHfpHfIndicatorId indID)
{
    CsrBtHfgHfIndicator *localHfIndicators;
    CsrUint16 indCount;
    localHfIndicators = inst->suppHfIndicators;
    indCount = SUPP_HFG_INDICATORS_COUNT;

    while(indCount > 0)
    {
        if(localHfIndicators->hfIndicatorID == indID)
        {
            return(localHfIndicators);
        }
        localHfIndicators++;
        indCount--;
    }
    return NULL;
}

RemoteHfIndicators *hfgFetchRemoteHfIndicator(hfgInstance_t *inst, CsrBtHfpHfIndicatorId indID)
{
    RemoteHfIndicators *remoteHfIndicators;
    Connection_t *con;
    CsrUint16 indCount;

    con = getActiveConnection(inst);
    /* [QTI] Fix KW issue#833890 through adding the check "con". */
    if (con != NULL)
    {
        remoteHfIndicators = con->instHfIndicators;
        indCount = con->hfIndCount;
        
        while(indCount > 0)
        {
            if(remoteHfIndicators->indId == indID)
            {
                return(remoteHfIndicators);
            }
            remoteHfIndicators++;
            indCount--;
        }
    }

    return NULL;
}

CsrBtHfgHfIndicator *hfgBuildLocalHfIndicatorList(hfgInstance_t * inst)
{
    CsrBtHfgHfIndicator *suppHfIndList;
    suppHfIndList = (CsrBtHfgHfIndicator *) CsrPmemZalloc(
                                                    SUPP_HFG_INDICATORS_COUNT * sizeof(CsrBtHfgHfIndicator));
    CsrMemCpy(suppHfIndList, inst->suppHfIndicators, SUPP_HFG_INDICATORS_COUNT * sizeof(CsrBtHfgHfIndicator));

    return suppHfIndList;
}
