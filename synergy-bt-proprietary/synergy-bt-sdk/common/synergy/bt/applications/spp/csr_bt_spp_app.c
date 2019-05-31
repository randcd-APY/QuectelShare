/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"

#include <string.h>
#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"

#include "csr_app_lib.h"

#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"

#include "csr_bt_spp_lib.h"
#include "csr_bt_spp_app.h"
#include "csr_bt_spp_app_private_prim.h"
#include "csr_bt_spp_app_ui_sef.h"
#include "csr_bt_spp_app_task.h"

/* Forward declarations for handler functions */
void CsrBtSppAppHandleSppPrim(CsrBtSppAppGlobalInstData *sppData);

/******************************************************************************
CsrBtSppAppStartActivate:
*****************************************************************************/
void CsrBtSppAppStartActivate(CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppAppInstData    *ptr;

    ptr = &sppData->sppInstance;

    ptr->serverOrClient        = SERVER;
    CsrBtSppActivateReqSend(sppData->queueId, sppData->phandle, MAX_ACTIVATE_TIME, CSR_BT_SPP_SERVER_ROLE, (CsrCharString *) CsrStrDup(ptr->comPortName));
}

/******************************************************************************
CsrBtSppAppStartConnecting:
*****************************************************************************/
void CsrBtSppAppStartConnecting(CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppAppInstData    *ptr;

    ptr        = &sppData->sppInstance;

    ptr->serverOrClient     = CLIENT;
    CsrBtSppConnectReqSend(sppData->queueId,
        sppData->phandle,
        (sppData->selectedDeviceAddr),
        FALSE,
        NULL,
        CSR_BT_SPP_CLIENT_ROLE);
}

/******************************************************************************
CsrBtSppAppStartDisconnect:
*****************************************************************************/
void CsrBtSppAppStartDisconnect(CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppAppInstData    *ptr;
    ptr = &sppData->sppInstance;

    CsrBtSppDisconnectReqSend(sppData->queueId, ptr->serverChannel);
}

/******************************************************************************
CsrBtSppAppStartDeactivate:
*****************************************************************************/
void CsrBtSppAppStartDeactivate(CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppDeactivateReqSend(sppData->queueId, sppData->phandle);
}

/******************************************************************************
CsrBtSppAppSendingTheSelectedFile:
*****************************************************************************/
void CsrBtSppAppSendingTheSelectedFile(CsrBtSppAppGlobalInstData *sppData, CsrBtSppAppInstData *instData)
{
    CsrUint32                bytesToRead;

    if( (instData->currentFileSize - instData->currentFilePosition) <= instData->framesize)
    {
        bytesToRead = instData->currentFileSize - instData->currentFilePosition;
    }
    else
    {
        bytesToRead = instData->framesize;
    }

    if(bytesToRead > 0)
    {
        CsrUint16                bytesRead;
        CsrUint8                    * body;

        body = (CsrUint8 *) CsrPmemAlloc(bytesToRead);
        bytesRead = (CsrUint16)CsrBtFread(body, 1, bytesToRead, instData->currentFileHandle);
        instData->currentFilePosition += bytesRead;
        CsrBtSppDataReqSend(sppData->queueId, instData->serverChannel, bytesRead, body);

        CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), FALSE, "Sending %d bytes of %d, Sent %d ; remaining: %d",
            bytesRead,
            instData->currentFileSize,
            instData->currentFilePosition,
            instData->currentFileSize - instData->currentFilePosition);
    }
    if(instData->currentFilePosition >= instData->currentFileSize)
    {
        CsrBtFclose(instData->currentFileHandle);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "File transfer operation finished successfully. %d bytes sent",instData->currentFilePosition);
        instData->currentFilePosition    = 0;
        instData->currentFileHandle        = NULL;
        instData->CsrBtSppAppSending                = FALSE;

        CsrBtSppAppHandleSppCsrUiSendFile(sppData, FALSE);
    }
}

/******************************************************************************
CsrBtSppAppOpenTheSelectedFile:
*****************************************************************************/
void CsrBtSppAppOpenTheSelectedFile(CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppAppInstData    *ptr;

    ptr = &sppData->sppInstance;

    ptr->currentFilePosition = 0;

    if (ptr->currentFileHandle == NULL)
    {
        int        result;
        CsrBtStat_t statBuffer;

        CsrMemSet(&statBuffer, 0, sizeof(CsrBtStat_t));
        statBuffer.st_size    = 0;
        result                = CsrBtStat(ptr->fileName, &statBuffer);
        if(!result)
        {
            if((ptr->currentFileHandle = CsrBtFopen(ptr->fileName, "rb")) == NULL)
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Fail. could not open the file %s", ptr->fileName);
            }
            else
            {
                ptr->currentFileSize = statBuffer.st_size;
                ptr->CsrBtSppAppSending         = TRUE;
                CsrBtSppAppHandleSppCsrUiSendFile(sppData, TRUE);
                CsrBtSppAppSendingTheSelectedFile(sppData, ptr);

                CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Starting transfering the file %s", ptr->fileName);
            }
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Fail. could not open the file %s", ptr->fileName);
        }
    }
    else
    {
        if(ptr->currentFileHandle != NULL)
        {
            CsrBtFclose(ptr->currentFileHandle);
            ptr->currentFileHandle    = NULL;
        }
        CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Fail.currentFileHandle is not NULL");
    }
}

/******************************************************************************
CsrBtSppAppSendingNextPieceofFile:
*****************************************************************************/
void CsrBtSppAppSendingNextPieceofFile(CsrBtSppAppGlobalInstData *sppData, CsrBtSppAppInstData *instData)
{
    if(instData->currentFileHandle != NULL)
    {
        CsrBtSppAppSendingTheSelectedFile(sppData, instData);
    }
    else
    {
        instData->currentFilePosition = 0;
    }
}


/******************************************************************************
CsrBtSppAppCancelFileTransfer:
*****************************************************************************/
void CsrBtSppAppCancelFileTransfer(CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppAppInstData    *ptr;

    ptr = &sppData->sppInstance;

    if(ptr->currentFileHandle != NULL)
    {
        CsrBtFclose(ptr->currentFileHandle);
        ptr->currentFilePosition    = 0;
        ptr->currentFileHandle        = NULL;
        ptr->CsrBtSppAppSending                = FALSE;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "The file transferring for file %s is cancelled", ptr->fileName);

        CsrBtSppAppHandleSppCsrUiSendFile(sppData, FALSE);
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "The file transferring could not be cancelled");
    }
}


/******************************************************************************
csrBtSppAppTimeOut:
*****************************************************************************/
static void csrBtSppAppTimeOut(CsrSchedQid queueId, CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppAppInstData *instData;
    double avgSpeed;
    double elapsed;
    double wrapAroundTime;
    double totalTime;

    instData = &sppData->sppInstance;
    instData->timerFlag        = FALSE;
    if(instData->writingFileHandle != NULL)
    {
        CsrBtFclose(instData->writingFileHandle);
        instData->writingFileHandle    = NULL;

        elapsed = instData->lastTime - instData->CsrBtSppAppStartTime;
        wrapAroundTime = (double) instData->nWrapArounds * WRAPAROUND_TIME_SEC;
        totalTime = wrapAroundTime + elapsed/1000000;
        if(instData->byteCounter > 0 && (elapsed > 0 || wrapAroundTime > 0))
        {
            avgSpeed = instData->byteCounter * 8.0 / (totalTime * 1000);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "%s has been copied to disk.", instData->outputFileName);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "The transfer speed was %f Kbits/Sec.", avgSpeed);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Amount of bytes transferred is  %u Kbytes", instData->byteCounter/1024);
            instData->byteCounter    = 0;
            instData->nWrapArounds   = 0;
        }
    }
}

/******************************************************************************
CsrBtSppAppDataIndHandler:
*****************************************************************************/
void CsrBtSppAppDataIndHandler(CsrBtSppAppGlobalInstData *sppData)
{
    CsrBtSppDataInd            *myPrim;
    CsrBtSppAppInstData    *instData;
    TIME                      currentTime;

    myPrim = (CsrBtSppDataInd *) sppData->recvMsgP;

    currentTime = CsrTimeGet(NULL);

    instData = &sppData->sppInstance;
    if (instData == NULL)
    {
        CsrGeneralException("CSR_BT_SPP_APP", 0, 0,
            "No instData return when receiving a CSR_BT_SPP_DATA_IND exit demo");
    }
    CsrBtSppDataResSend(sppData->queueId, instData->serverChannel);

    if(instData->writingFileHandle == NULL)
    {
        if((instData->writingFileHandle = CsrBtFopen(instData->outputFileName, "wb")) == NULL)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Fail. could not open the file for writing");
        }
        else
        {
            instData->byteCounter    = myPrim->payloadLength;
            instData->CsrBtSppAppStartTime        = currentTime;
            instData->timerFlag        = TRUE;
            instData->timeId        = CsrSchedTimerSet(TIME_WITH_NO_DATA, (void (*) (CsrUint16, void *)) csrBtSppAppTimeOut, (CsrSchedQid) sppData->queueId, (void *) sppData);
            CsrBtFwrite(myPrim->payload, sizeof(CsrUint8), myPrim->payloadLength, instData->writingFileHandle);

            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Data transfer initated from peer");
        }
    }
    else
    {
        CsrSchedTimerCancel(instData->timeId,NULL,NULL);
        instData->byteCounter    = myPrim->payloadLength + instData->byteCounter;
        instData->timerFlag        = TRUE;
        instData->timeId        = CsrSchedTimerSet(TIME_WITH_NO_DATA, (void (*) (CsrUint16, void *)) csrBtSppAppTimeOut, (CsrSchedQid) sppData->queueId, (void *) sppData);
        CsrBtFwrite(myPrim->payload, sizeof(CsrUint8), myPrim->payloadLength, instData->writingFileHandle);

        if (instData->lastTime > currentTime)
        {
            ++instData->nWrapArounds;
        }
    }
    instData->lastTime    = currentTime;
    CsrPmemFree(myPrim->payload);
}

/******************************************************************************
CsrBtSppAppInitInstanceData:
*****************************************************************************/
void CsrBtSppAppInitInstanceData(CsrBtSppAppGlobalInstData *sppData)
{
    CsrMemSet(sppData,0,sizeof(CsrBtSppAppGlobalInstData));

    sppData->sppInstance.serverChannel             = 0xff;
}

static void csrBtSppAppSendSppAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtSppAppRegisterReq *prim;

    prim = (CsrBtSppAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtSppAppRegisterReq));
    prim->type = CSR_BT_SPP_APP_REGISTER_REQ;
    prim->phandle = pHandle;

    CsrSchedMessagePut(CSR_BT_SPP_APP_IFACEQUEUE, CSR_BT_SPP_APP_PRIM,prim);
}

static void csrBtSppAppSendSppAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtSppAppRegisterCfm *prim;

    prim = (CsrBtSppAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtSppAppRegisterCfm));
    prim->type = CSR_BT_SPP_APP_REGISTER_CFM;
    prim->queueId = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_SPP_APP_PRIM,prim);
}

static void csrBtSppAppSendSppAppHouseCleaning(CsrBtSppAppGlobalInstData    *sppData)
{
    if(sppData->saveQueue)
    {
        CsrBtSppAppHouseCleaning *prim;

        sppData->restoreFlag = TRUE;

        prim = (CsrBtSppAppHouseCleaning *) CsrPmemAlloc(sizeof(CsrBtSppAppHouseCleaning));
        prim->type = CSR_BT_SPP_APP_HOUSE_CLEANING;
        CsrSchedMessagePut(CSR_BT_SPP_APP_IFACEQUEUE, CSR_BT_SPP_APP_PRIM,prim);
    }
}

static char * csrBtSppAppReturnSppInstName(CsrUint16 instance)
{
    char *buf;
    CsrUint32 bufSize = CsrStrLen("SPP instance xxxx")+1;
    buf = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "SPP instance %04X", instance);
    return buf;
}

/******************************************************************************
* Init function called by the scheduler upon initialisation.
* This function is used to boot the demo application.
*****************************************************************************/
void CsrBtSppAppInit(void **gash)
{
    CsrBtSppAppGlobalInstData    *sppData;

    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemAlloc(sizeof(CsrBtSppAppGlobalInstData));
    sppData = (CsrBtSppAppGlobalInstData *) *gash;
    CsrBtSppAppInitInstanceData(sppData);

    sppData->phandle = CsrSchedTaskQueueGet() ;

    sppData->initialized = FALSE;

    /* Start creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    if(sppData->phandle == CSR_BT_SPP_APP_IFACEQUEUE)
    {
        CsrBtSppGetInstancesQidReqSend(sppData->phandle);
    }
    else
    {
        csrBtSppAppSendSppAppRegisterReq(sppData->phandle);
    }
}

#ifdef ENABLE_SHUTDOWN
void CsrBtSppAppDeinit(void **gash)
{
    CsrUint16 msg_type=0;
    void *msg_data=NULL;
    CsrBtSppAppGlobalInstData *sppData;

    sppData = (CsrBtSppAppGlobalInstData *) (*gash);
    /* get a message from the demoapplication message queue. The message is returned in prim
    and the event type in eventType */
    while ( CsrSchedMessageGet(&msg_type, &msg_data) )
    {
        switch (msg_type)
        {
        case CSR_BT_SPP_PRIM:
            CsrBtSppFreeUpstreamMessageContents(msg_type, msg_data);
            break;
        }
        CsrPmemFree(msg_data);
    }

    if (sppData->sppInstance.timerFlag)
    {
        CsrSchedTimerCancel(sppData->sppInstance.timeId, NULL, NULL);
    }

    if (sppData->sppInstance.currentFileHandle != NULL)
    {
        CsrBtFclose(sppData->sppInstance.currentFileHandle);
    }

    if (sppData->sppInstance.writingFileHandle != NULL)
    {
        CsrBtFclose(sppData->sppInstance.writingFileHandle);
    }

    if (sppData->serviceNameList)
    {
        CsrPmemFree(sppData->serviceNameList);
        sppData->serviceNameList = NULL;
    }

    if (sppData->profileName)
    {
        CsrPmemFree(sppData->profileName);
        sppData->profileName = NULL;
    }

    CsrPmemFree(sppData->appHandles);

    CsrPmemFree(sppData);
}
#else
void CsrBtSppAppDeinit(void **gash)
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
void CsrBtSppAppHandler(void **gash)
{
    CsrBtSppAppGlobalInstData *sppData;
    CsrUint16    eventType=0;

    /* get a message from the demoapplication message queue. The message is returned in prim
    and the event type in eventType */
    sppData = (CsrBtSppAppGlobalInstData *) (*gash);

    if(!sppData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &sppData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&sppData->saveQueue, &eventType , &sppData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
          a message from the scheduler                                                   */
            sppData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &sppData->recvMsgP);
        }
        else
        { /* A message has been restored from the local queed                               */
            ;
        }
    }

    switch (eventType)
    {
        case CSR_BT_SPP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) sppData->recvMsgP;
            switch(*prim)
            {
                case CSR_BT_SPP_SECURITY_IN_CFM:
                case CSR_BT_SPP_SECURITY_OUT_CFM:
                {
                    /* handleScSecLevelPrim(&sppData->scInstData);*/
                    break;
                }
                default:
                {
                    CsrBtSppAppHandleSppPrim(sppData);
                    break;
                }
            }
            break;
        }

        case CSR_BT_SPP_APP_PRIM:
        {
            CsrPrim *type = (CsrPrim *) sppData->recvMsgP;

            switch(*type)
            {
                case CSR_BT_SPP_APP_REGISTER_REQ:
                { /* only received by controling spp demo app instance */
                    if(sppData->initialized)
                    {
                        CsrBtSppAppRegisterReq *prim = sppData->recvMsgP;
                        CsrUint8 i;
                        CsrSchedQid queueId = CSR_SCHED_QID_INVALID;

                        for(i=0; i< sppData->numberOfSppInstances;i++)
                        {
                            if(sppData->appHandles[i].sppAppHandle == CSR_SCHED_QID_INVALID)
                            {
                                sppData->appHandles[i].sppAppHandle = prim->phandle;
                                queueId = sppData->appHandles[i].sppProfileHandle;
                                break;
                            }
                        }
                        if(queueId != CSR_SCHED_QID_INVALID)
                        {
                            csrBtSppAppSendSppAppRegisterCfm(prim->phandle, queueId,i);
                        }
                        else
                        {
                            CsrGeneralException("CSR_SPP_DEMO_APP", 0, *type, "No CsrBtSpp profile Queue Handle Available");
                        }
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), FALSE, "Received CSR_BT_SPP_APP_REGISTER_REQ from phandle = %d", prim->phandle);

                    }
                    else
                    {
                        CsrMessageQueuePush(&sppData->saveQueue, CSR_BT_SPP_APP_PRIM, sppData->recvMsgP);
                        sppData->recvMsgP = NULL;
                    }
                    break;
                }
                case CSR_BT_SPP_APP_REGISTER_CFM:
                { /* Only received by extra spp demo app instances */
                    CsrBtSppAppRegisterCfm *prim = sppData->recvMsgP;

                    sppData->queueId = prim->queueId;
                    snprintf(sppData->sppInstance.comPortName, sizeof(sppData->sppInstance.comPortName), "COM%d",prim->instance);
                    snprintf(sppData->sppInstance.outputFileName, sizeof(sppData->sppInstance.outputFileName), "CSR_BT_SPP%02d.txt",prim->instance);
                    sppData->profileName = csrBtSppAppReturnSppInstName(prim->instance);

                    CsrBtSppAppHandleSppSetMainMenuHeader(sppData);

                    sppData->initialized = TRUE;

                    CsrAppRegisterReqSend(sppData->phandle, TECH_BT, PROFILE_NAME(sppData->profileName));
                    break;
                }
                case CSR_BT_SPP_APP_HOUSE_CLEANING:
                default:
                { /* Do nothing */
                    break;
                }
            }
            break;
        }
        case CSR_BT_GAP_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) sppData->recvMsgP;

            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *prim = sppData->recvMsgP;
                sppData->selectedDeviceAddr = prim->deviceAddr;

                CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), FALSE, "Received selected DeviceAddr: %04X:%02X:%06X", prim->deviceAddr.nap, prim->deviceAddr.uap, prim->deviceAddr.lap);
                CsrUiUieShowReqSend(sppData->csrUiVar.hMainMenu, CsrSchedTaskQueueGet(), CSR_UI_INPUTMODE_AUTO, CSR_BT_SPP_APP_CSR_UI_PRI);
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_SPP_APP", 0, *prim, "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtSppAppHandler,");
            }
            break;
        }
        case CSR_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) sppData->recvMsgP;

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
                CsrGeneralException("CSR_BT_SPP_APP", 0, *prim, "####### Unhandled CSR_APP_PRIM in CsrBtSppAppHandler,");
            }
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrBtSppAppHandleSppCsrUiPrim(sppData);
            break;
        }

        default:
        {
            /* unexpected primitive received */
            CsrGeneralException("CSR_BT_SPP_APP", 0, *((CsrPrim *) sppData->recvMsgP), "####### default in CsrBtSppAppHandler,");
        }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
    * the pointer (prim) must be set to NULL in order not to free it here
    */
    CsrPmemFree(sppData->recvMsgP);
}

/******************************************************************************
* Function to handle all spp-primitives.
******************************************************************************/
void CsrBtSppAppHandleSppPrim(CsrBtSppAppGlobalInstData *sppData)
{
    CsrPrim *primType;
    CsrBtSppAppInstData *instData;

    primType = (CsrPrim *) sppData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_SPP_ACTIVATE_CFM:
        {
            instData = &sppData->sppInstance;

            CsrBtSppAppHandleSppCsrUiActivateDeactivate(sppData, TRUE);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), FALSE, "Activated");
            break;
        }
        case CSR_BT_SPP_CONNECT_IND:
        {
            CsrBtSppConnectInd        *myPrim;
            CsrUint8    modemSetting    = 0;
            CsrUint8 breakSignal        = 0;

            myPrim = (CsrBtSppConnectInd *) sppData->recvMsgP;
            instData = &sppData->sppInstance;
            instData->serverChannel = myPrim->serverChannel;

            if (myPrim->resultSupplier == CSR_BT_SUPPLIER_SPP &&
                myPrim->resultCode == CSR_BT_RESULT_CODE_SPP_SUCCESS)
            {
                instData->framesize        = myPrim->profileMaxFrameSize;
                instData->CsrBtSppAppSending        = FALSE;
                if(instData->serverOrClient == CLIENT)
                {
                    if(CSR_BT_SPP_CLIENT_ROLE == CSR_BT_DTE)
                    {
                        modemSetting = CSR_BT_MODEM_RTS_MASK | CSR_BT_MODEM_DTR_MASK;
                    }
                    else
                    {
                        modemSetting = CSR_BT_MODEM_DSR_MASK | CSR_BT_MODEM_CTS_MASK;
                    }
                }
                else
                {
                    if(CSR_BT_SPP_SERVER_ROLE == CSR_BT_DCE)
                    {
                        modemSetting = CSR_BT_MODEM_DSR_MASK | CSR_BT_MODEM_CTS_MASK;
                    }
                    else
                    {
                        modemSetting = CSR_BT_MODEM_RTS_MASK | CSR_BT_MODEM_DTR_MASK;
                    }
                }
                CsrBtSppControlReqSend(sppData->queueId, instData->serverChannel, modemSetting, breakSignal);

                CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Successfully Connected with: %04X:%02X:%06X", myPrim->deviceAddr.nap, myPrim->deviceAddr.uap, myPrim->deviceAddr.lap);
            }
            else
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d", myPrim->deviceAddr.nap, myPrim->deviceAddr.uap, myPrim->deviceAddr.lap, myPrim->resultCode, myPrim->resultSupplier);
            }
            CsrBtSppAppHandleSppCsrUiConnectDisconnect(sppData, TRUE,(CsrBool)((myPrim->resultCode == CSR_BT_RESULT_CODE_SPP_SUCCESS && myPrim->resultSupplier == CSR_BT_SUPPLIER_SPP) ? TRUE : FALSE));
            break;
        }
        case CSR_BT_SPP_DEACTIVATE_CFM:
        {
            instData = &sppData->sppInstance;

            CsrBtSppAppHandleSppCsrUiActivateDeactivate(sppData, FALSE);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), FALSE, "Activated");
            break;
        }
        case CSR_BT_SPP_DATA_IND:
        {
            CsrBtSppAppDataIndHandler(sppData);
            break;
        }
        case CSR_BT_SPP_DATA_CFM:
        {
            instData = &sppData->sppInstance;

            CsrBtSppAppSendingNextPieceofFile(sppData, instData);
            break;
        }
        case CSR_BT_SPP_CONTROL_IND:
        {
            break;
        }
        case CSR_BT_SPP_DISCONNECT_IND:
        {
            CsrBtSppDisconnectInd    *myPrim;

            myPrim = (CsrBtSppDisconnectInd *) sppData->recvMsgP;
            instData = &sppData->sppInstance;

            instData->CsrBtSppAppSending        = FALSE;
            instData->byteCounter    = 0;
            if(instData->currentFileHandle != NULL)
            {
                CsrBtFclose(instData->currentFileHandle);
                instData->currentFileHandle = NULL;
            }
            if(instData->writingFileHandle != NULL)
            {
                CsrSchedTimerCancel(instData->timeId,NULL,NULL);
                instData->timerFlag        = FALSE;
                CsrBtFclose(instData->writingFileHandle);
                instData->writingFileHandle    = NULL;
            }

            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Disconnected from: %04X:%02X:%06X", myPrim->deviceAddr.nap, myPrim->deviceAddr.uap, myPrim->deviceAddr.lap);
            CsrBtSppAppHandleSppCsrUiConnectDisconnect(sppData, FALSE, TRUE);

            break;
        }
        case CSR_BT_SPP_PORTNEG_IND:
        {
            RFC_PORTNEG_VALUES_T m;
            CsrBtSppPortnegInd *myPrim;

            myPrim = (CsrBtSppPortnegInd *) sppData->recvMsgP;
            CsrBtPortParDefault(&m);
            CsrBtSppPortnegResSend(myPrim->queueId, myPrim->serverChannel, &m);
            break;
        }
        case CSR_BT_SPP_SERVICE_NAME_IND :
        {
            CsrBtSppServiceNameInd    *myPrim;

            myPrim        = (CsrBtSppServiceNameInd *) sppData->recvMsgP;
            sppData->serviceNameList        = myPrim->serviceNameList;
            sppData->serviceNameListSize    = myPrim->serviceNameListSize;

            CsrBtSppAppHandleSppCsrUiServiceNameList(sppData, myPrim->serviceNameListSize, myPrim->serviceNameList);

            break;
        }

        case CSR_BT_SPP_GET_INSTANCES_QID_CFM:
        {
            CsrBtSppGetInstancesQidCfm *myPrim;
            CsrUint8 i;

            myPrim = (CsrBtSppGetInstancesQidCfm *) sppData->recvMsgP;

            sppData->numberOfSppInstances = myPrim->phandlesListSize;

            CsrPmemFree(sppData->appHandles);
            sppData->appHandles = CsrPmemAlloc(sizeof(CsrBtSppAppProfileHandleList) * sppData->numberOfSppInstances);

            for(i=0;i<sppData->numberOfSppInstances;i++)
            {
                sppData->appHandles[i].sppAppHandle = CSR_SCHED_QID_INVALID;
                sppData->appHandles[i].sppProfileHandle = myPrim->phandlesList[i];
            }

            CsrPmemFree(myPrim->phandlesList);

            sppData->queueId = sppData->appHandles[0].sppProfileHandle;
            sppData->appHandles[0].sppAppHandle = sppData->phandle;

            sppData->profileName = csrBtSppAppReturnSppInstName(0);
            CsrAppRegisterReqSend(sppData->phandle, TECH_BT, PROFILE_NAME(sppData->profileName));
            snprintf(sppData->sppInstance.comPortName, sizeof(sppData->sppInstance.comPortName), "COM%d",0);
            snprintf(sppData->sppInstance.outputFileName, sizeof(sppData->sppInstance.outputFileName), "CSR_BT_SPP%02d.txt",0);

            CsrBtSppAppHandleSppSetMainMenuHeader(sppData);

            sppData->initialized = TRUE;
            csrBtSppAppSendSppAppHouseCleaning(sppData);

            break;
        }

        case CSR_BT_SPP_STATUS_IND:
        {
            CsrBtSppStatusInd *myPrim;

            myPrim  = (CsrBtSppStatusInd*)sppData->recvMsgP;

            if(myPrim->connect)
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Data status indication: Connect");
            }
            else
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Data status indication: Disconnect");
            }
            break;
        }

        case CSR_BT_SPP_REGISTER_DATA_PATH_HANDLE_CFM:
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Data path handle registered");
            break;
        }

        case CSR_BT_SPP_DATA_PATH_STATUS_IND:
        {
            CsrBtSppDataPathStatusInd *prim;
            char *events[4] = { "Opened", "Closed", "Lost", "Undefined" };
            char *what;

            prim = (CsrBtSppDataPathStatusInd*)sppData->recvMsgP;
            if(prim->status <= 2)
            {
                what = events[prim->status];
            }
            else
            {
                what = events[3];
            }
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "Data path application status indication: %s", what);
            break;
        }

        default:
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_NAME(sppData->profileName), TRUE, "####### default in spp prim handler 0x%04x,",
                *primType);
            CsrBtSppFreeUpstreamMessageContents(CSR_BT_SPP_PRIM, sppData->recvMsgP);
            /* unexpected primitive received */
        }
    }
}

