/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.Qualcomm Technologies International, Ltd. Confidential and Proprietary.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include <stdio.h>
#include "csr_synergy.h"
#include "csr_bt_pan_app.h"
#include "csr_framework_ext.h"

/* Uncomment if you want to configure network via script */
/* #define PAN_CONFIG_NETWORK */

#if defined(USE_MDM_PLATFORM) || defined(USE_IMX_PLATFORM)
#define BT_PAN_SCRIPT       "bt_pan.sh"

#define BT_PANU_ROLE        "panu"
#define BT_NAP_ROLE         "nap"
#define BT_UNKNOWN_ROLE     "unknown"

CsrCharString *map2PanRoleString(CsrBtBslPanRole localRole)
{
    switch (localRole)
    {
        case CSR_BT_BSL_PANU_ROLE:
            return BT_PANU_ROLE;

        case CSR_BT_BSL_NAP_ROLE:
            return BT_NAP_ROLE;

        default:
            return BT_UNKNOWN_ROLE;
    }
}

#ifdef PAN_CONFIG_NETWORK
void launchPanScript(CsrBool start, CsrBtBslPanRole localRole)
{
    char str[256];

    /* e.g. 'bt_pan.sh start nap' */
    snprintf(str, sizeof(str), "%s %s %s",
            BT_PAN_SCRIPT, 
            start ? "start" : "stop",
            map2PanRoleString(localRole));

    system(str);  
}
#endif

void configNetwork(CsrBtPanAppGlobalInstData* instanceData, CsrBool start)
{
#ifdef PAN_CONFIG_NETWORK
    CsrBtBslPanRole localRole = instanceData->localRole;

    launchPanScript(start, localRole);
#endif
}
#endif

/******************************************************************************
 *  Ether IP prim handlers.
 ******************************************************************************/
void etherAddHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrIpEtherIfAddCfm* etherIfAddCfm = (CsrIpEtherIfAddCfm*) instanceData->recvMsgP;
    if (etherIfAddCfm->result == CSR_RESULT_SUCCESS )
    {
        instanceData->frameRxFunction = etherIfAddCfm->frameRxFunction;
        instanceData->ipContext = etherIfAddCfm->ipContext;
        instanceData->encapsulation = etherIfAddCfm->encapsulation;
        instanceData->ifHandle = etherIfAddCfm->ifHandle;
        CsrIpEtherIfLinkUpReqSend(CSR_BT_PAN_APP_IFACEQUEUE, instanceData->ifHandle);
    }
}

void etherRemoveHandler(CsrBtPanAppGlobalInstData* instanceData)
{
#if defined(USE_MDM_PLATFORM) || defined(USE_IMX_PLATFORM)
    configNetwork(instanceData, FALSE);
#endif
}

void etherLinkUpHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrIpEtherIfLinkUpCfm* etherIfLinkCfm = (CsrIpEtherIfLinkUpCfm*) instanceData->recvMsgP;
    if (etherIfLinkCfm->result != CSR_RESULT_SUCCESS || etherIfLinkCfm->ifHandle != instanceData->ifHandle)
    {
        CsrIpEtherIfLinkUpReqSend(CSR_BT_PAN_APP_IFACEQUEUE, instanceData->ifHandle);
    }
    else
    {
        instanceData->linkUp = TRUE;
        while(instanceData->rxQSize > 0)
        {
            instanceData->rxQSize--;
            passToEther(instanceData, &instanceData->rxQ[instanceData->rxQSize]);
        }
        CsrIpEtherIfFlowControlResumeReqSend(CSR_BT_PAN_APP_IFACEQUEUE, instanceData->ifHandle, CSR_IP_ETHER_PRIORITY_ALL);
#if defined(USE_MDM_PLATFORM) || defined(USE_IMX_PLATFORM)
        configNetwork(instanceData, TRUE);
#endif
    }
}

void releaseRxQ(CsrBtPanAppGlobalInstData* instanceData)
{
    instanceData->linkUp = FALSE;
    while (instanceData->rxQSize)
    {
        instanceData->rxQSize--;
        CsrMemFree(instanceData->rxQ[instanceData->rxQSize].payload);
    }
}

void etherLinkDownHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrIpEtherIfLinkDownCfm* etherIfLinkCfm = (CsrIpEtherIfLinkDownCfm*) instanceData->recvMsgP;
    if (etherIfLinkCfm->result != CSR_RESULT_SUCCESS )
    {
        CsrIpEtherIfLinkDownReqSend(CSR_BT_PAN_APP_IFACEQUEUE, instanceData->ifHandle);
    }
    else
    {
        releaseRxQ(instanceData);
    }
}

void etherMulticastAddrAddHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrIpEtherIfMulticastAddrAddInd* prim = (CsrIpEtherIfMulticastAddrAddInd*) instanceData->recvMsgP;
    CsrResult result = CSR_RESULT_FAILURE;
    if (prim->ifHandle == instanceData->ifHandle)
    {
        if (instanceData->multicastAddressesCount < MAX_MULTICAST_ADDRESSES - 1)
        {
            instanceData->multicastAddresses[instanceData->multicastAddressesCount] =
                    *(ETHER_ADDR *) prim->multicastMacAddr;
            /*
             instanceData->multicastAddresses[instanceData->multicastAddressesCount].w[0] = *((CsrUint16 *)prim->multicastMacAddr);
             instanceData->multicastAddresses[instanceData->multicastAddressesCount].w[1] = *((CsrUint16 *)prim->multicastMacAddr + 1);
             instanceData->multicastAddresses[instanceData->multicastAddressesCount].w[2] = *((CsrUint16 *)prim->multicastMacAddr + 2);*/
            instanceData->multicastAddressesCount++;
            result = CSR_RESULT_SUCCESS;
        }
    }
    CsrIpEtherIfMulticastAddrAddResSend(prim->ifHandle, result);
}

void etherMulticastAddrRemoveHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrIpEtherIfMulticastAddrRemoveInd* prim = (CsrIpEtherIfMulticastAddrRemoveInd*) instanceData->recvMsgP;
    CsrResult result = CSR_RESULT_FAILURE;
    if (prim->ifHandle == instanceData->ifHandle)
    {
        CsrUint16 i;
        for (i = 0; i < instanceData->multicastAddressesCount; i++)
        {
            if (CsrMemCmp(&instanceData->multicastAddresses[i], &prim->multicastMacAddr, sizeof(ETHER_ADDR)) == 0)
            {
                result = CSR_RESULT_SUCCESS;
                instanceData->multicastAddressesCount--;
                instanceData->multicastAddresses[i] =
                        instanceData->multicastAddresses[instanceData->multicastAddressesCount];
                break;
            }
        }
    }
    CsrIpEtherIfMulticastAddrRemoveResSend(prim->ifHandle, result);
}

void etherMulticastAddrFlushHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrIpEtherIfMulticastAddrFlushInd* prim = (CsrIpEtherIfMulticastAddrFlushInd*) instanceData->recvMsgP;
    CsrResult result = CSR_RESULT_FAILURE;
    if (prim->ifHandle == instanceData->ifHandle)
    {
        instanceData->multicastAddressesCount = 0;
        result = CSR_RESULT_SUCCESS;
    }
    CsrIpEtherIfMulticastAddrFlushResSend(prim->ifHandle, result);
}

CsrResult frameTxFunction(CsrUint8 *destinationMac, CsrUint8 *sourceMac, CsrUint16 frameLength, CsrUint8 *frame,
        CsrIpEtherIfContext ifContext)
{
    CsrBtPanAppGlobalInstData *instanceData = (CsrBtPanAppGlobalInstData *) ifContext;
    CsrUint8 *packet = CsrMemDup(frame + 2, frameLength - 2);
    CsrUint16 packetType = (frame[0] << 8) | (frame[1]);

#ifndef EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE
    if (instanceData->outstandingDataReq < CSR_BT_MAX_BSL_QUEUE_LENGTH)
    {
        instanceData->outstandingDataReq++;
        CsrBtBslDataReqSend(instanceData->id, packetType, destinationMac, sourceMac, frameLength - 2, packet);

        /* If Outstanding Data Req reaches BSL Queue length then pause the flow from IP stack */
        if (instanceData->outstandingDataReq == CSR_BT_MAX_BSL_QUEUE_LENGTH)
            {
            instanceData->flowPaused = TRUE;
            CsrIpEtherIfFlowControlPauseReqSend(CSR_BT_PAN_APP_IFACEQUEUE, instanceData->ifHandle, CSR_IP_ETHER_PRIORITY_ALL);
            }
    }
    else if (!instanceData->txQFull)
    {
        /* Race condition where flow control pause is not handled yet by Ether IP resulting in IP data presented to BSL app 
         * In such case Queue the data in BSL app */
        instanceData->txQ[instanceData->txQWrIdx].id = instanceData->id;
        instanceData->txQ[instanceData->txQWrIdx].etherType = packetType;
        instanceData->txQ[instanceData->txQWrIdx].dstAddr = *((ETHER_ADDR *) destinationMac);
        instanceData->txQ[instanceData->txQWrIdx].srcAddr = *((ETHER_ADDR *) sourceMac);
        instanceData->txQ[instanceData->txQWrIdx].length = frameLength - 2;
        instanceData->txQ[instanceData->txQWrIdx].payload= packet;
        instanceData->txQWrIdx++;
        instanceData->txQWrIdx &= MAX_TX_DATA_QUEUE_MASK;

        if (instanceData->txQRdIdx == instanceData->txQWrIdx) 
        {
            /* Even Tx Queue is full return failure */
            instanceData->txQFull = TRUE;
        }

    }
    else
    {
        CsrMemFree(packet);
        return (CSR_IP_ETHER_RESULT_NOT_SENT);
    }
#else
    CsrBtBslDataReqSend(instanceData->id, packetType, destinationMac, sourceMac, frameLength - 2, packet);
#endif /* EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE */

    return (CSR_RESULT_SUCCESS );
}

