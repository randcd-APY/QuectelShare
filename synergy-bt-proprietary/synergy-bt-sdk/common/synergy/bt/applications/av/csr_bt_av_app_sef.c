/******************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include <stdio.h>

#include "csr_bt_av_app_task.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_av2.h"
#include "csr_bt_av_app_sef.h"
#include "csr_bt_profiles.h"


void av2HandleAvAppPrim(av2instance_t *instData, void *msg)
{
    CsrBtAvAppPrim    *primType;

    primType = (CsrBtAvAppPrim *) msg;
    switch (*primType)
    {
        case CSR_BT_AV_APP_ACTIVATE_REQ:
        {
            CsrBtAvAppActivateReq *prim;
            prim = (CsrBtAvAppActivateReq*)msg;
            avAppActivateReqHandler(instData, prim);
        }
        break;
        
        case CSR_BT_AV_APP_CONNECT_REQ:
        {
            CsrBtAvAppConnectReq *prim;
            prim = (CsrBtAvAppConnectReq*)msg;
            avAppConnectReqHandler(instData, prim);
        }
        break;

        case CSR_BT_AV_APP_DISCONNECT_REQ:
        {
            CsrBtAvAppDisconnectReq *prim;
            prim = (CsrBtAvAppDisconnectReq*)msg;
            avAppDisconnectReqHandler(instData, prim);
        }
        break;

        case CSR_BT_AV_APP_PLAY_REQ:
        {
            CsrBtAvAppPlayReq *prim;
            prim = (CsrBtAvAppPlayReq*)msg;
            avAppPlayReqHandler(instData, prim);
        }
        break;

        case CSR_BT_AV_APP_PAUSE_REQ:
        {
            CsrBtAvAppPauseReq *prim;
            prim = (CsrBtAvAppPauseReq*)msg;
            avAppPauseReqHandler(instData, prim);
        }
        break; 
    }

}

void avAppActivateReqHandler(av2instance_t *instData, CsrBtAvAppActivateReq *msg)
{
    instData->appHandle = msg->pHandle;

    if(msg->avAppRole == CSR_BT_AV_APP_SOURCE_ROLE)
    {
        instData->avConfig = DA_AV_CONFIG_AV_SRC;
    }
    else
    {
        instData->avConfig = DA_AV_CONFIG_AV_SNK;
    }
#ifdef USE_AVROUTER
    /*initialize with SBC*/
    instData->codecsSupported = CSR_BT_AV_SBC;

    if(msg->supportedCodec & CSR_BT_AV_MP3_CODEC_SUPPORT)
    {
        instData->codecsSupported |= CSR_BT_AV_MPEG12_AUDIO;
    }
    if(msg->supportedCodec & CSR_BT_AV_APTX_CODEC_SUPPORT)
    {
        instData->codecsSupported |= CSR_BT_AV_APTX_AUDIO;
    }
#endif
    startActivate(instData);
}

void avAppConnectReqHandler(av2instance_t *instData, CsrBtAvAppConnectReq *msg)
{
    instData->remoteAddr[instData->currentConnection] = msg->deviceAddr;
    startConnecting(instData);
}

void avAppDisconnectReqHandler(av2instance_t *instData, CsrBtAvAppDisconnectReq *msg)
{
    startDisconnect(instData);
}

void avAppPlayReqHandler(av2instance_t *instData, CsrBtAvAppPlayReq *msg)
{
    startStream(instData);

}

void avAppPauseReqHandler(av2instance_t *instData, CsrBtAvAppPauseReq *msg)
{
    suspendStream(instData);
}



