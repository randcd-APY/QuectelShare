/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <string.h>
#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"

#include "csr_app_lib.h"

#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_ip_ether_lib.h"
#include "csr_bt_util.h"

#include "csr_bt_bsl_lib.h"
#include "csr_bt_spp_lib.h"
#include "csr_bt_pan_app.h"
#include "csr_bt_pan_app_private_prim.h"
#include "csr_bt_pan_app_ui_sef.h"
#include "csr_bt_pan_app_task.h"
#include "csr_bt_pan_strings.h"
#include "csr_ip_ifconfig_prim.h"
#include "csr_bt_ip_prim.h"
#include "csr_ip_ether_prim.h"
#include "csr_framework_ext.h"

#define DATA_PACKET_LEN                   1500

#ifdef CSR_BT_BSL_RAW_DATA_THROUGHPUT
#define NUM_PACKETS_TO_BE_SENT    10000

static CsrUint16 num_pkts_sent;
#endif /* CSR_BT_BSL_RAW_DATA_THROUGHPUT */


/* Forward declarations for handler functions */
void CsrBtPanAppHandlePanPrim(CsrBtPanAppGlobalInstData *panData);
void CsrBtPanAppHandleEtherPrim(CsrBtPanAppGlobalInstData *panData);

/******************************************************************************
CsrBtPanAppStartActivate:
*****************************************************************************/
void CsrBtPanAppStartActivate(CsrBtPanAppGlobalInstData *panData)
{
    CsrBtBslActivateReqSend(TRUE, panData->localRole, CSR_BT_BSL_PANU_ROLE|CSR_BT_BSL_NAP_ROLE|CSR_BT_BSL_GN_ROLE);
}


void CsrBtPanAppStartSelect(CsrBtPanAppGlobalInstData *panData)
{
    CsrBtPanAppHandlePanCsrUiRoleSelected(panData, TRUE);
}

/****************************************************************************** 
CsrBtPanAppStartConnecting:
*****************************************************************************/
void CsrBtPanAppStartConnecting(CsrBtPanAppGlobalInstData *panData,CsrUint16 remoteRole)
{
    commonPanPopupSet(panData, "Connecting", "Please wait");
    commonPanPopupShow(panData);
    panData->csrUiVar.localInitiatedAction = TRUE;
    CsrBtBslConnectReqSend(panData->selectedDeviceAddr, panData->localRole,
         remoteRole);
}

/******************************************************************************
CsrBtPanAppStartDisconnect:
*****************************************************************************/
void CsrBtPanAppStartDisconnect(CsrBtPanAppGlobalInstData *panData)
{
    CsrBtBslDisconnectReqSend(panData->id);
}

static void saveLocalAddr(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrBtCmReadLocalBdAddrCfm* readBdAddrCfm;
    ETHER_ADDR thisAddress;

    readBdAddrCfm = (CsrBtCmReadLocalBdAddrCfm*) instanceData->recvMsgP;
    thisAddress = CsrBtConvBdAddr2EtherBig(&readBdAddrCfm->deviceAddr);
    instanceData->macAddress[0] = thisAddress.w[0];
    instanceData->macAddress[1] = thisAddress.w[1];
    instanceData->macAddress[2] = thisAddress.w[2];
}
#ifndef CSR_BT_BSL_RAW_DATA_THROUGHPUT
void CsrBtPanAppAddEherIf(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrIpEtherIfAddReqSend(CSR_BT_PAN_APP_IFACEQUEUE, CSR_IP_IFCONFIG_IF_TYPE_WIRED, (CsrUint8 * )&instanceData->macAddress[0],
                    CSR_IP_ETHER_ENCAPSULATION_ETHERNET, MAX_ETHER_FRAME_SIZE, frameTxFunction, (CsrIpEtherIpContext ) instanceData);

}
#else
void CsrBtPanAppAddEherIf(CsrBtPanAppGlobalInstData* instanceData)
{
        ETHER_ADDR    etherAddr, thisAddress;
        CsrUint8        *data;
        CsrUint16 j;
        etherAddr = CsrBtConvBdAddr2EtherBig( &(instanceData->selectedDeviceAddr) );
        thisAddress.w[0] = instanceData->macAddress[0];
        thisAddress.w[1] = instanceData->macAddress[1];
        thisAddress.w[2] = instanceData->macAddress[2];

        for (j=0; j < CSR_BT_MAX_BSL_QUEUE_LENGTH; j++)
        {
            data = (CsrUint8 *) CsrPmemAlloc(DATA_PACKET_LEN);
            memcpy(data, &num_pkts_sent, 2);
            CsrBtBslDataReqSend(instanceData->id, 0x1234, &etherAddr, &thisAddress, DATA_PACKET_LEN, data);
            num_pkts_sent++;
        }

}
#endif /* CSR_BT_BSL_RAW_DATA_THROUGHPUT */


void passToEther(CsrBtPanAppGlobalInstData* instanceData, CsrBtBslDataInd* dataInd)
{
    CsrUint8* packet = CsrPmemAlloc(dataInd->length + 2);
    packet[0] = (CsrUint8) ((dataInd->etherType >> 8) & 0xFF);
    packet[1] = (CsrUint8) (dataInd->etherType & 0xFF);
    CsrMemCpy(&packet[2], dataInd->payload, dataInd->length);
    instanceData->frameRxFunction((CsrUint8*) &dataInd->dstAddr.w[0], (CsrUint8*) &dataInd->srcAddr.w[0], 0,
            dataInd->length + 2, (CsrUint8*) packet, instanceData->ipContext);
    CsrPmemFree(dataInd->payload);
    CsrPmemFree(packet);
}
#ifndef EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE
#ifndef CSR_BT_BSL_RAW_DATA_THROUGHPUT
void bslDataCfmHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrBtBslDataCfm* dataCfm = (CsrBtBslDataCfm*) instanceData->recvMsgP;

    if (dataCfm->id == instanceData->id)
        instanceData->outstandingDataReq--;

    if ((instanceData->txQFull) || (instanceData->txQRdIdx != instanceData->txQWrIdx))
    {
        CsrUint16 idx = instanceData->txQRdIdx;

        instanceData->outstandingDataReq++;
        CsrBtBslDataReqSend(instanceData->txQ[idx].id,
                instanceData->txQ[idx].etherType,
                &instanceData->txQ[idx].dstAddr,
                &instanceData->txQ[idx].srcAddr,
                instanceData->txQ[idx].length,
                instanceData->txQ[idx].payload);

        instanceData->txQRdIdx++;
        instanceData->txQRdIdx &= MAX_TX_DATA_QUEUE_MASK;

        instanceData->txQFull = FALSE;
        return;
    }

    if ((instanceData->flowPaused == TRUE) && (instanceData->outstandingDataReq <= 0))
    {
        instanceData->flowPaused = FALSE;
        CsrIpEtherIfFlowControlResumeReqSend(CSR_BT_PAN_APP_IFACEQUEUE, instanceData->ifHandle, CSR_IP_ETHER_PRIORITY_ALL);
    }
}
#else
void bslDataCfmHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrBtBslDataCfm* dataCfm = (CsrBtBslDataCfm*) instanceData->recvMsgP;

    if (num_pkts_sent < NUM_PACKETS_TO_BE_SENT)
    {
        ETHER_ADDR    etherAddr, thisAddress;
        CsrUint8        *data;

        etherAddr = CsrBtConvBdAddr2EtherBig( &(instanceData->selectedDeviceAddr) );
        thisAddress.w[0] = instanceData->macAddress[0];
        thisAddress.w[1] = instanceData->macAddress[1];
        thisAddress.w[2] = instanceData->macAddress[2];

            data = (CsrUint8 *) CsrPmemAlloc(DATA_PACKET_LEN);
            memcpy(data, &num_pkts_sent, 2);
            CsrBtBslDataReqSend(instanceData->id, 0x1234, &etherAddr, &thisAddress, DATA_PACKET_LEN, data);
            num_pkts_sent++;
    }
}
#endif /* CSR_BT_BSL_RAW_DATA_THROUGHPUT */
#endif /* EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE */


void bslDataHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    CsrBtBslDataInd* dataInd = (CsrBtBslDataInd*) instanceData->recvMsgP;
    if (instanceData->linkUp == TRUE)
    {
        passToEther(instanceData, dataInd);
    }
    else if(instanceData->rxQSize < MAX_RX_DATA_QUEUE)
    {
        instanceData->rxQ[instanceData->rxQSize] = *dataInd;
        instanceData->rxQSize++;
    }
    else
    {
        CsrMemFree(dataInd->payload);
    }
}

void bslDisconnectHandler(CsrBtPanAppGlobalInstData* instanceData)
{
    if (!instanceData->csrUiVar.localInitiatedAction)
    {
        CsrBtBslDisconnectInd * disconnectInd;
        CsrBtBslDisconnectRes * disconnectRes;

        disconnectInd = (CsrBtBslDisconnectInd *) instanceData->recvMsgP;
        disconnectRes = (CsrBtBslDisconnectRes *) CsrPmemAlloc(sizeof(CsrBtBslDisconnectRes));
        disconnectRes->type = CSR_BT_BSL_DISCONNECT_RES;
        disconnectRes->id = disconnectInd->id;
        CsrSchedMessagePut(CSR_BT_BSL_IFACEQUEUE, CSR_BT_BSL_PRIM, disconnectRes);
    }
    CsrIpEtherIfRemoveReqSend(CSR_BT_PAN_APP_IFACEQUEUE, instanceData->ifHandle);
    
}

/******************************************************************************
CsrBtSppAppInitInstanceData:
*****************************************************************************/
void CsrBtPanAppInitInstanceData(CsrBtPanAppGlobalInstData *panData)
{
    CsrMemSet(panData,0,sizeof(CsrBtPanAppGlobalInstData));
    
}
static void csrBtPanAppSendPanAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{

    CsrBtPanAppRegisterCfm *prim;

    prim = (CsrBtPanAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtPanAppRegisterCfm));
    prim->type = CSR_BT_PAN_APP_REGISTER_CFM;
    prim->queueId = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_PAN_APP_PRIM,prim);
}

/******************************************************************************
* Init function called by the scheduler upon initialisation.
* This function is used to boot the demo application.
*****************************************************************************/
void CsrBtPanAppInit(void **gash)
{
    CsrBtPanAppGlobalInstData    *panData;
    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemAlloc(sizeof(CsrBtPanAppGlobalInstData));
    panData = (CsrBtPanAppGlobalInstData *) *gash;
    CsrBtPanAppInitInstanceData(panData);

    panData->localRole = DEFAULT_PAN_ROLE;

    panData->phandle = CsrSchedTaskQueueGet() ;

    panData->initialized = FALSE;
    panData->ipAddress[0] = DEFAULT_IP_ADDRESS_0;
    panData->ipAddress[1] = DEFAULT_IP_ADDRESS_1;
    panData->subnetMask[0] = DEFAULT_SUBNET_MASK_0;
    panData->subnetMask[1] = DEFAULT_SUBNET_MASK_1;
    panData->gatewayIpAddress[0] = DEFAULT_GATEWAY_IP_ADDRESS_0;
    panData->gatewayIpAddress[1] = DEFAULT_GATEWAY_IP_ADDRESS_1;

    CsrAppRegisterReqSend(CSR_BT_PAN_APP_IFACEQUEUE,
                          TECH_BT,
                          PROFILE_PAN);
    CsrUiUieCreateReqSend(CSR_BT_PAN_APP_IFACEQUEUE,
                          CSR_UI_UIETYPE_EVENT);
    CsrBtBslRegisterReqSend(CSR_BT_PAN_APP_IFACEQUEUE, CSR_BT_PAN_APP_IFACEQUEUE);
    CsrBtCmReadLocalBdAddrReqSend(CSR_BT_PAN_APP_IFACEQUEUE);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtPanAppDeinit(void **gash)
{
    CsrUint16 msg_type=0;
    void *msg_data=NULL;
    CsrBtPanAppGlobalInstData *panData;

    panData = (CsrBtPanAppGlobalInstData *) (*gash);
    /* get a message from the demoapplication message queue. The message is returned in prim
    and the event type in eventType */
    while ( CsrSchedMessageGet(&msg_type, &msg_data) )
    {
        switch (msg_type)
        {
        case CSR_BT_BSL_PRIM:
            CsrBtBslFreeUpstreamMessageContents(msg_type, msg_data);
            break;
        }
        CsrPmemFree(msg_data);
    }
    panData->ifHandle = 0;
    panData->frameRxFunction = NULL;
    panData->encapsulation = CSR_IP_ETHER_ENCAPSULATION_ETHERNET;
    releaseRxQ(panData);
    CsrPmemFree(panData->appHandles);
    CsrPmemFree(panData);
 }
#else
void CsrBtPanAppDeinit(void **gash)
{
    CSR_UNUSED(gash);
}
#endif

/******************************************************************************
* This is the demo application handler function. All primitives sent
* to the demo application will be received in here. The scheduler
* ensures that the function is activated when a signal is put on the
* demo application signal queue.
*****************************************************************************/
void CsrBtPanAppHandler(void **gash)
{
    CsrBtPanAppGlobalInstData *panData;
    CsrUint16    eventType=0;

    /* get a message from the demoapplication message queue. The message is returned in prim
    and the event type in eventType */
    panData = (CsrBtPanAppGlobalInstData *) (*gash);

    if(!panData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &panData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&panData->saveQueue, &eventType , &panData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
          a message from the scheduler                                                   */
            panData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &panData->recvMsgP);
        }
        else
        { /* A message has been restored from the local queed                               */
            ;
        }
    }

    switch (eventType)
    {
        case CSR_BT_BSL_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) panData->recvMsgP;
            switch(*prim)
            {
                case CSR_BT_BSL_SECURITY_IN_CFM:
                case CSR_BT_BSL_SECURITY_OUT_CFM:
                {
                    /* handleScSecLevelPrim(&sppData->scInstData);*/
                    break;
                }
                default:
                {
                    CsrBtPanAppHandlePanPrim(panData);
                    break;
                }
            }
            break;
        }
        case CSR_IP_ETHER_PRIM:
        {
            CsrBtPanAppHandleEtherPrim(panData);
            break;
        }
        case CSR_BT_CM_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) panData->recvMsgP;
            switch (*prim)
            {
                case CSR_BT_CM_READ_LOCAL_BD_ADDR_CFM :
                {
                    saveLocalAddr(panData);
                     break;
                }
                default:
                    CsrBtCmFreeUpstreamMessageContents(eventType, prim);
                    break;
            }
            break;
        }
        case CSR_BT_PAN_APP_PRIM:
        {
            CsrPrim *type = (CsrPrim *) panData->recvMsgP;

            switch(*type)
            {
                case CSR_BT_PAN_APP_REGISTER_REQ:
                { /* only received by controling pan demo app instance */
                    if(panData->initialized)
                    {
                        CsrBtPanAppRegisterReq *prim = panData->recvMsgP;
                        CsrUint8 i;
                        CsrSchedQid queueId = CSR_SCHED_QID_INVALID;

                        for(i=0; i< panData->numberOfPanInstances;i++)
                        {
                            if(panData->appHandles[i].panAppHandle == CSR_SCHED_QID_INVALID)
                            {
                                panData->appHandles[i].panAppHandle = prim->phandle;
                                queueId = panData->appHandles[i].panProfileHandle;
                                break;
                            }
                        }
                        if(queueId != CSR_SCHED_QID_INVALID)
                        {
                            csrBtPanAppSendPanAppRegisterCfm(prim->phandle, queueId,i);
                        }
                        else
                        {
                            CsrGeneralException("CSR_PAN_DEMO_APP", 0, *type, "No CsrBtPan profile Queue Handle Available");
                        }
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_PAN, FALSE, "Received CSR_BT_PAN_APP_REGISTER_REQ from phandle = %d", prim->phandle);

                    }
                    else
                    {
                        CsrMessageQueuePush(&panData->saveQueue, CSR_BT_PAN_APP_PRIM, 
                        panData->recvMsgP);
                        panData->recvMsgP = NULL;
                    }
                    break;
                }
                case CSR_BT_PAN_APP_REGISTER_CFM:
                { /* Only received by extra pan demo app instances */
                    CsrBtPanAppRegisterCfm *prim = panData->recvMsgP;

                    panData->queueId = prim->queueId;
                    CsrBtPanAppHandlePanSetMainMenuHeader(panData);

                    panData->initialized = TRUE;

                    CsrAppRegisterReqSend(panData->phandle, TECH_BT, 
                    PROFILE_PAN);
                    break;
                }
                default:
                { /* Do nothing */
                    break;
                }
            }
            break;
        }
        case CSR_BT_GAP_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) panData->recvMsgP;

            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *prim = panData->recvMsgP;
                panData->selectedDeviceAddr = prim->deviceAddr;

                CsrAppBacklogReqSend(TECH_BT, PROFILE_PAN, FALSE, "Received selected DeviceAddr: %04X:%02X:%06X", prim->deviceAddr.nap, prim->deviceAddr.uap, prim->deviceAddr.lap);
                CsrUiUieShowReqSend(panData->csrUiVar.hMainMenu, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_AUTO, 
                CSR_BT_PAN_APP_CSR_UI_PRI);
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_PAN_APP", 0, *prim, "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtPanAppHandler,");
            }
            break;
        }
        case CSR_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) panData->recvMsgP;

            if (*prim == CSR_APP_TAKE_CONTROL_IND)
            { /* We need to query the GAP task for the currently selected device addr */
                CsrBtGapAppGetSelectedDeviceReqSend(CsrSchedTaskQueueGet());
            }
            else if (*prim == CSR_APP_REGISTER_CFM)
            { /* Just ignore this primitive since we only register one menu item we don't need to remember the controlId */
                ;
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_PAN_APP", 0, *prim, "####### Unhandled CSR_APP_PRIM in CsrBtPanAppHandler,");
            }
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrBtPanAppHandlePanCsrUiPrim(panData);
            break;
        }

        default:
        {
            /* unexpected primitive received */
            CsrGeneralException("CSR_BT_PAN_APP", 0, *((CsrPrim *) panData->recvMsgP), "####### default in CsrBtPanAppHandler,");
        }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
    * the pointer (prim) must be set to NULL in order not to free it here
    */
    CsrPmemFree(panData->recvMsgP);
}

/******************************************************************************
* Function to handle all pan-primitives.
******************************************************************************/
void CsrBtPanAppHandlePanPrim(CsrBtPanAppGlobalInstData *panData)
{
    CsrPrim *primType;
    primType = (CsrPrim *) panData->recvMsgP;
    switch (*primType)
    {

        case CSR_BT_BSL_ACTIVATE_CFM:
        {
            if (panData->localRole == CSR_BT_PAN_NAP_ROLE)
            {
                CsrBtPanSetDialog(panData,  NULL, TEXT_PAN_APP_NAP_ROLE_SELECTED_UCS2, TEXT_OK_UCS2, NULL);
            }
            else if (panData->localRole == CSR_BT_PAN_PANU_ROLE)
            {
                CsrBtPanSetDialog(panData,  NULL, TEXT_PAN_APP_PANU_ROLE_SELECTED_UCS2, TEXT_OK_UCS2, NULL);
            }

            CsrBtPanShowUi(panData,  CSR_UI_INPUTMODE_AUTO, CSR_BT_PAN_APP_CSR_UI_PRI);
            CsrBtPanAppHandlePanCsrUiActivateDeactivate(panData, panData->connected);
            break;
        }
        
        case CSR_BT_BSL_CONNECT_IND:
        {
            CsrBtBslConnectInd        *myPrim;
            myPrim = (CsrBtBslConnectInd *) panData->recvMsgP;
            
            panData->id=myPrim->id;
            if (myPrim->resultSupplier == CSR_BT_SUPPLIER_BSL &&
                myPrim->resultCode == CSR_BT_RESULT_CODE_BSL_SUCCESS)
            {
               CsrBtPanSetDialog(panData,  NULL, TEXT_PAN_APP_CONNECT_SUCCESS_UCS2, TEXT_OK_UCS2, NULL);
               CsrBtPanShowUi(panData,  CSR_UI_INPUTMODE_AUTO, CSR_BT_PAN_APP_CSR_UI_PRI);
               panData->connected=TRUE;
               CsrBtPanAppAddEherIf(panData);
               CsrBtPanAppHandlePanCsrUiConnectDisconnect(panData, TRUE,TRUE);
            }
            else
            {
                CsrBtPanSetDialog(panData,  TEXT_FAILED_UCS2, TEXT_PAN_APP_CONNECT_FAIL_UCS2, TEXT_OK_UCS2, NULL);
                CsrBtPanShowUi(panData,  CSR_UI_INPUTMODE_AUTO, CSR_BT_PAN_APP_CSR_UI_PRI);
                CsrBtPanAppHandlePanCsrUiConnectDisconnect(panData, FALSE,TRUE);
            }
            
            break;
        }
        case CSR_BT_BSL_DISCONNECT_IND:
        {            
            CsrBtPanSetDialog(panData, NULL, TEXT_PAN_APP_DISCONNECT_SUCCESS_UCS2, TEXT_OK_UCS2, NULL);
            CsrBtPanShowUi(panData,  CSR_UI_INPUTMODE_AUTO, CSR_BT_PAN_APP_CSR_UI_PRI);
            panData->connected=FALSE;
            bslDisconnectHandler(panData);
            CsrBtPanAppHandlePanCsrUiConnectDisconnect(panData, FALSE, TRUE);

            break;
        }
        case CSR_BT_BSL_STATUS_IND:
        {
            break;
        }
        case CSR_BT_BSL_DATA_IND:
        {
            bslDataHandler(panData);
            break;
        }
#ifndef EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE
        case CSR_BT_BSL_DATA_CFM:
        {
            bslDataCfmHandler(panData);
            break;
        }
#endif /* EXCLUDE_CSR_BT_BSL_FLOW_CONTROL_FEATURE */
         default:
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_PAN, TRUE, "####### default in bsl prim handler 0x%04x,",
                *primType);
            CsrBtBslFreeUpstreamMessageContents(CSR_BT_BSL_PRIM, panData->recvMsgP);
            /* unexpected primitive received */
        }

    }
}
/******************************************************************************
* Function to handle all Ether IP-primitives.
******************************************************************************/
void CsrBtPanAppHandleEtherPrim(CsrBtPanAppGlobalInstData *panData)
{
    CsrPrim *primType;
    
    primType = (CsrPrim *) panData->recvMsgP;
    switch (*primType)
    {

        case CSR_IP_ETHER_IF_ADD_CFM :
        {
            etherAddHandler(panData);
            break;
        }
        case CSR_IP_ETHER_IF_LINK_UP_CFM :
        {
            etherLinkUpHandler(panData);
            break;
        }
        case CSR_IP_ETHER_IF_LINK_DOWN_CFM :
        {
            etherLinkDownHandler(panData);
            break;
        }
        case CSR_IP_ETHER_IF_REMOVE_CFM :
        {
            etherRemoveHandler(panData);
            break;
        }
        case CSR_IP_ETHER_IF_MULTICAST_ADDR_ADD_IND :
        {
            etherMulticastAddrAddHandler(panData);
            break;
        }
        case CSR_IP_ETHER_IF_MULTICAST_ADDR_REMOVE_IND :
        {
            etherMulticastAddrRemoveHandler(panData);
            break;
        }
        case CSR_IP_ETHER_IF_MULTICAST_ADDR_FLUSH_IND :
        {
            etherMulticastAddrFlushHandler(panData);
            break;
        }
         default:
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_PAN, TRUE, "####### default in Ether ip prim handler 0x%04x,",
                *primType);
            CsrBtBslFreeUpstreamMessageContents(CSR_IP_ETHER_PRIM, panData->recvMsgP);
            /* unexpected primitive received */
        }
    }
}
