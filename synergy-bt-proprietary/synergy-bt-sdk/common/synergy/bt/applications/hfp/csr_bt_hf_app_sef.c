/******************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include <stdio.h>

#include "csr_bt_hf_app_task.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_hf_lib.h"
#include "csr_bt_hf_demo_app.h"
#include "csr_bt_hf_app_sef.h"
#include "csr_bt_hf_app_ui_sef.h"


void hfAppActivateReqHandler(DemoInstdata_t *instData, CsrBtHfAppActivateReq *recMsg)
{
    instData->appHandle = recMsg->pHandle;

    CsrBtHfActivateReqSendExt(instData->app_hdl, recMsg->maxHFConns, recMsg->maxHSConns, recMsg->maxSimulConns,
                           instData->conInstData[instData->currentConnection].localSupportedFeatures,
                           0, 5,
                           NULL, 0);
}

void hfAppConnectReqHandler(DemoInstdata_t *instData, CsrBtHfAppConnectReq *recMsg)
{
    instData->appHandle = recMsg->pHandle;
    if (!instData->connReqPending)
    {/*  start a connection to address specified */
        BD_ADDR_T       theAddr;

        theAddr.lap = recMsg->deviceAddr.lap;
        theAddr.uap = recMsg->deviceAddr.uap;
        theAddr.nap = recMsg->deviceAddr.nap;

        CsrBtHfServiceConnectReqSend(theAddr, CSR_BT_HF_CONNECTION_UNKNOWN);
        instData->connReqPending = TRUE;
    }
    else
    {
        /*Connection Request already pending, wait */
    }
}

void hfAppDisconnectReqHandler(DemoInstdata_t *instData,
                               CsrBtHfAppDisconnectReq *recMsg)
{
    instData->appHandle = recMsg->pHandle;
    if (instData->conInstData[instData->currentConnection].linkType != 
                                                   CSR_BT_HF_CONNECTION_UNKNOWN)
    {
        CsrUint32 conId = 
                       instData->conInstData[instData->currentConnection].conId;

        CsrBtHfDisconnectReqSend(conId);
    }
    else
    {
        /*Connection doesn't Exist*/
    }
}

void hfAppDialReqHandler(DemoInstdata_t *instData, CsrBtHfAppDialReq *recMsg)
{
    instData->appHandle = recMsg->pHandle;
    if (instData->conInstData[instData->currentConnection].linkType == 
                                                        CSR_BT_HF_CONNECTION_HF)
    {
        if(recMsg->dialType==CSR_BT_HF_DIAL_NUMBER)
        {
            CsrUint8 *phoneNum = recMsg->theNumber;
            CsrUint8  numberLength = (CsrUint8)CsrStrLen((CsrCharString *) phoneNum);
            
            if(numberLength != 0)
            {
                CsrUint8 *numberToDial;
                
                numberToDial = CsrPmemAlloc(numberLength+3);
                CsrMemCpy(numberToDial, phoneNum, numberLength);
                numberToDial[numberLength++] = ';';
                numberToDial[numberLength++] = '\r';
                numberToDial[numberLength] = '\0';

                CsrBtHfDialReqSend(instData->conInstData[instData->currentConnection].conId,
                                   CSR_BT_HF_DIAL_NUMBER,

                                   (CsrCharString *) numberToDial);
            }
        }
        else
        {
            CsrBtHfDialReqSend(instData->conInstData[instData->currentConnection].conId, 
                               CSR_BT_HF_DIAL_REDIAL,NULL);
        }
    }
    else
    {
        /*Error*/
    }
}

void hfAppCallAnsRejReq(DemoInstdata_t *instData, CsrBtHfAppCallAnsRejReq *recMsg)
{
    instData->appHandle = recMsg->pHandle;
    if(recMsg->acceptRejReq == CSR_BT_HF_ANSWER_CALL)
    {
        CsrBtHfAnswerReqSend(instData->conInstData[instData->currentConnection].conId);
    }
    else
    {
        CsrBtHfCallEndReqSend(instData->conInstData[instData->currentConnection].conId);
    }
}

void hfAppAudioConnectReqhandler(DemoInstdata_t *instData, 
                                 CsrBtHfAppAudioConnectReq *recMsg)
{
    instData->appHandle = recMsg->pHandle;
    if (instData->conInstData[instData->currentConnection].linkType != 
                                                   CSR_BT_HF_CONNECTION_UNKNOWN)
    {
        if (instData->conInstData[instData->currentConnection].audioOn == TRUE)
        { /* Audio off*/
            CsrBtHfAudioDisconnectReqSend(
                     instData->conInstData[instData-> currentConnection].conId, 0xFFFF);
        }
        else
        {/* audio On */
            CsrBtHfAudioConnectReqSend(
                              instData->conInstData[instData-> currentConnection].conId,
                              0, NULL, PCM_SLOT, PCM_SLOT_REALLOCATE);
        }
    }
    else
    {

    }
}

InstHfIndicators *fetchRemoteHfIndicator(DemoInstdata_t *inst, CsrBtHfpHfIndicatorId indID)
{
    InstHfIndicators *remoteHfIndicators;
    CsrUint16 indCount;
    remoteHfIndicators = inst->conInstData[inst->currentConnection].instHfIndicators;
    indCount = inst->conInstData[inst->currentConnection].hfIndCount;

    while(indCount > 0)
    {
        if(remoteHfIndicators->indId == indID)
        {
            return(remoteHfIndicators);
        }
        remoteHfIndicators++;
        indCount--;
    }

    return NULL;
}

CsrBtHfpHfIndicatorId *CsrBtHfBuildLocalHfIndicatorList(DemoInstdata_t * inst)
{
    CsrBtHfpHfIndicatorId *suppHfIndList;
    suppHfIndList = (CsrBtHfpHfIndicatorId *) CsrPmemZalloc(
                                                     SUPP_HF_INDICATORS_COUNT * sizeof(CsrBtHfpHfIndicatorId));
    CsrMemCpy(suppHfIndList, inst->suppHfIndicators, SUPP_HF_INDICATORS_COUNT * sizeof(CsrBtHfpHfIndicatorId));

    return suppHfIndList;
}

