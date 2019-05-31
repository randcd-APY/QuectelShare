/****************************************************************************

Copyright (c) 2012-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/
#include "csr_synergy.h"
#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"

#include "csr_app_lib.h"

#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"

#include "csr_bt_cm_prim.h"
#include "csr_bt_cm_lib.h"

#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_rsc_utils.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_rsc_server_app_private_prim.h"
#include "csr_bt_gatt_demo_rsc_server_app_ui_sef.h"
#include "csr_bt_gatt_demo_rsc_server_db.h"
#include "csr_bt_gatt_app_task.h"

/* Forward declarations for handler functions */
void CsrBtRscsAppHandleRscsPrim(CsrBtRscsAppInstData *rscsData);
void CsrBtRscsAppHandleSdPrim(CsrBtRscsAppInstData *rscsData);


static void csrBtRscsAppClientConfigInitList(CsrCmnListElm_t *elem)
{
    /* Initialise a CsrBtRscsClientConfigElement. This function is called every
     * time a new entry is made on the queue list */
    CsrBtRscsAppClientConfigElement *cElem  =  (CsrBtRscsAppClientConfigElement *) elem;
    CsrMemSet(&cElem->deviceAddr, 0, sizeof(cElem->deviceAddr));
    cElem->measurementValue                   =  NULL;
    cElem->scCtrlPointValue                   =  NULL;
    cElem->paired                             =  FALSE;
    cElem->connectionFailed                   =  FALSE;
}

static void csrBtRscsAppFreeClientConfigList(CsrCmnListElm_t *elem)
{
    /* CsrPmemFree local pointers in the CsrBtRscsAppClientConfigElement.
     * This function is called every time a element is removed from the
     * ClientConfig List list */
    CsrBtRscsAppClientConfigElement *cElem = (CsrBtRscsAppClientConfigElement *) elem;
    if(NULL != cElem->measurementValue)
    {
        CsrPmemFree(cElem->measurementValue);
    }
    if(NULL != cElem->scCtrlPointValue)
    {
        CsrPmemFree(cElem->scCtrlPointValue);
    }
}

CsrBool CsrBtRscsAppFindClientConfigByAddr(CsrCmnListElm_t *elem, void *value)
{ /* Return TRUE if uuid matches*/
    CsrBtDeviceAddr *addr             = (CsrBtDeviceAddr *) value;
    CsrBtRscsAppClientConfigElement *element = (CsrBtRscsAppClientConfigElement *)elem;
    if(element->deviceAddr.lap != addr->lap || element->deviceAddr.uap != addr->uap || element->deviceAddr.nap != addr->nap)
    { 
        return FALSE;
    }
    /*the uuid is the same */
    return TRUE;
}

int rscSimplePseudoRand(int lim)
{
    static long a = 1;

    a = (a * 32719 + 3) % 32749;
    return ((a % lim) + 1);
}

void CsrBtRscsAppUpdateTimerInterupt(CsrUint16 mi, void *mv)
{
    CsrBtRscsAppInstData *inst = (CsrBtRscsAppInstData*)mv;
    CsrBtRscsAppClientConfigElement *cElem = CSR_BT_RSCS_APP_FIND_CLIENT_CONFIG_BY_ADDR(inst->clientConfigList, (void*)(&inst->connectedDeviceAddr.addr));
    if(cElem)
    {
        if(NULL != cElem->measurementValue && ((CsrUint16*)cElem->measurementValue)[0] == CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION)
        {
            CsrUint8 *value = NULL;
            CsrUint8 size = 0;
            CsrUint16 speed, speedLimit;
            if(inst->rscData.flags & 0x04)
            {
                speed = 700;
                speedLimit = 300;
            }
            else
            {
                speed = 400;
                speedLimit = 150;
            }
            inst->rscData.distance += (CsrUint32)rscSimplePseudoRand(20); /* increment distance with random value below 10  (format is in meter with resolution of 1/10m )*/
            inst->rscData.speed = (CsrUint16)rscSimplePseudoRand(speed);     /* set running speed to random value below 30 (format is seconds with resolution of 1/256 m/s)   */
            inst->rscData.cadance = (CsrUint8)rscSimplePseudoRand(10);    /* set cadance to random value below 10 () */
            inst->rscData.stride = (CsrUint8)rscSimplePseudoRand(100);    /* set cadance to random value below 140 () */
            if(inst->rscData.stride < 50)
            { /* give a somewhat realistic stride value */
                inst->rscData.stride += 50;
            }
            if(inst->rscData.speed < speedLimit)
            { /* give a somewhat realistic stride value */
                inst->rscData.speed += speedLimit;
            }
            size = csrBtRscAppBuildMeasurementValue(inst->rscData.flags, inst->rscData.speed, inst->rscData.cadance, inst->rscData.stride, inst->rscData.distance, &value);

            CsrBtGattNotificationEventReqSend(inst->gattId, inst->connectedBtConnId,
                                              (CsrUint16)(CSR_BT_GATT_APP_RSC_MEASUREMENT_HANDLE+inst->dbStartHandle+1),size,(CsrUint8*)value);
        }
    }
    inst->update_timer = CsrSchedTimerSet(CSR_BT_RSCS_APP_UPDATE_TIMER_INTERVAL,
                                          CsrBtRscsAppUpdateTimerInterupt,
                                          mi,
                                          mv);

}

/******************************************************************************
CsrBtRscsAppCrank
 *****************************************************************************/
void CsrBtRscsAppCrank(CsrBtRscsAppInstData *rscsData)
{
    /* Crank the state - i.e. listen or disconnect or whatever we need
     * to do */
    switch (rscsData->state)
    {
        case CSR_BT_RSCS_ST_LISTEN:
        {
            /* accept on both */
            if (rscsData->leBtConnId == CSR_BT_CONN_ID_INVALID)
            {
                CsrBtTypedAddr addr;
                CsrBtAddrZero(&addr);
                CsrBtGattPeripheralReqSend(rscsData->gattId,
                                           addr,
                                           CSR_BT_GATT_FLAGS_UNDIRECTED,
                                           0);
            }
            if (rscsData->bredrBtConnId == CSR_BT_RSCS_BTCONN_UNSET)
            {
                CsrBtGattBredrAcceptReqSend(rscsData->gattId,
                                            CSR_BT_GATT_FLAGS_NONE);
            }
        }
        break;

        case CSR_BT_RSCS_ST_LE:
        {
            /* connected on LE - disconnect BR/EDR*/
            if (rscsData->bredrBtConnId != CSR_BT_RSCS_BTCONN_UNSET)
            {
                CsrBtGattDisconnectReqSend(rscsData->gattId,
                                           rscsData->bredrBtConnId);
            }
        }
        break;

        case CSR_BT_RSCS_ST_BREDR:
        {
            /* connected on BR/EDR - disconnect LE */
            if (rscsData->leBtConnId != CSR_BT_CONN_ID_INVALID)
            {
                CsrBtGattDisconnectReqSend(rscsData->gattId,
                                           rscsData->leBtConnId);
            }
        }
        break;

        case CSR_BT_RSCS_ST_STOP:
        {
            /* disconnect all */
            if (rscsData->leBtConnId != CSR_BT_CONN_ID_INVALID)
            {
                CsrBtGattDisconnectReqSend(rscsData->gattId,
                                           rscsData->leBtConnId);
            }
            if (rscsData->bredrBtConnId != CSR_BT_RSCS_BTCONN_UNSET)
            {
                CsrBtGattDisconnectReqSend(rscsData->gattId,
                                           rscsData->bredrBtConnId);
            }
        }
        break;
    }
}


/******************************************************************************
CsrBtRscsAppStartRegister:
 *****************************************************************************/
void CsrBtRscsAppStartRegister(CsrBtRscsAppInstData *rscsData)
{
    CsrBtGattRegisterReqSend(rscsData->phandle,
                             1234);
}
/******************************************************************************
CsrBtRscsAppStartUnregister:
 *****************************************************************************/
void CsrBtRscsAppStartUnregister(CsrBtRscsAppInstData *rscsData)
{
    rscsData->state = CSR_BT_RSCS_ST_STOP;
    CsrBtRscsAppCrank(rscsData);
    CsrBtGattUnregisterReqSend(rscsData->gattId);
}
/******************************************************************************
CsrBtRscsAppStartActivate:
 *****************************************************************************/
void CsrBtRscsAppStartActivate(CsrBtRscsAppInstData *rscsData)
{
    /* Crank state */
    CsrBtRscsAppCrank(rscsData);

}
/******************************************************************************
CsrBtRscsAppStartDeactivating:
 *****************************************************************************/
void CsrBtRscsAppStartDeactivate(CsrBtRscsAppInstData *rscsData)
{
    rscsData->state = CSR_BT_RSCS_ST_STOP;
    CsrBtRscsAppCrank(rscsData);
}



/******************************************************************************
CsrBtRscsAppStartDisconnect:
 *****************************************************************************/
void CsrBtRscsAppStartDisconnect(CsrBtRscsAppInstData *rscsData)
{
    /*stop update timer*/
    CsrSchedTimerCancel(rscsData->update_timer,
                        0,
                        NULL);

    /* Disconnecting. Assume we want to accept new connection */
    rscsData->state = CSR_BT_RSCS_ST_LISTEN;

    /* Disconnect current connection */
    if (rscsData->state == CSR_BT_RSCS_ST_BREDR)
    {
        CsrBtGattDisconnectReqSend(rscsData->gattId,
                                   rscsData->bredrBtConnId);
    }
    else
    {
        CsrBtGattDisconnectReqSend(rscsData->gattId,
                                   rscsData->leBtConnId);
    }
}

/******************************************************************************
CsrBtRscsAppInitInstanceData:
 *****************************************************************************/
void CsrBtRscsAppClearHandles(CsrBtRscsAppInstData *rscsData){

}
void CsrBtRscsAppInitInstanceData(CsrBtRscsAppInstData *rscsData)
{
    CsrMemSet(rscsData,0,sizeof(CsrBtRscsAppInstData));


    rscsData->csrUiVar.hMainMenu                = CSR_UI_DEFAULTACTION;
    rscsData->csrUiVar.hCommonDialog            = CSR_UI_DEFAULTACTION;
    rscsData->leBtConnId                        = CSR_BT_CONN_ID_INVALID;
    rscsData->connectedBtConnId                 = CSR_BT_CONN_ID_INVALID;
    rscsData->bredrBtConnId                     = CSR_BT_RSCS_BTCONN_UNSET;
    rscsData->rscData.distance                  = 0;
    rscsData->rscData.speed                     = 0;
    rscsData->rscData.cadance                   = 0;
    rscsData->rscData.stride                    = 0;
    rscsData->rscData.sensorLocation            = 0;
    rscsData->rscData.feature                   = 0x001F; /*all features supported*/
    rscsData->rscData.flags                     = 0x07; /*contains stride + distance + "is running" */
    CsrCmnListInit(&rscsData->clientConfigList,
                   0,
                   csrBtRscsAppClientConfigInitList,
                   csrBtRscsAppFreeClientConfigList);
    CsrBtRscsAppClearHandles(rscsData);
}

static void csrBtRscsAppSendRscsAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtRscsAppRegisterReq *prim;

    prim = (CsrBtRscsAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtRscsAppRegisterReq));
    prim->type = CSR_BT_RSCS_APP_REGISTER_REQ;
    prim->phandle = pHandle;

    CsrSchedMessagePut(CSR_BT_RSCS_APP_IFACEQUEUE, CSR_BT_RSCS_APP_PRIM,prim);
}

static void csrBtRscsAppSendRscsAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtRscsAppRegisterCfm *prim;

    prim = (CsrBtRscsAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtRscsAppRegisterCfm));
    prim->type = CSR_BT_RSCS_APP_REGISTER_CFM;
    prim->queueId = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_RSCS_APP_PRIM,prim);
}

static char * csrBtRscsAppReturnRscsInstName(CsrBtRscsAppInstData *instance)
{
    char *buf;
    CsrUint32 bufSize = CsrStrLen("RSC Srv")+1;
    buf = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "RSC Srv");
    return buf;
}

/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtRscsAppInit(void **gash)
{
    CsrBtRscsAppInstData    *rscsData;

    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemAlloc(sizeof(CsrBtRscsAppInstData));
    rscsData = (CsrBtRscsAppInstData *) *gash;
    CsrBtRscsAppInitInstanceData(rscsData);


    rscsData->phandle = CsrSchedTaskQueueGet() ;

    rscsData->initialized = TRUE;
    rscsData->state = CSR_BT_RSCS_ST_LISTEN;
    rscsData->bredrBtConnId = CSR_BT_RSCS_BTCONN_UNSET;

    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    csrBtRscsAppSendRscsAppRegisterReq(rscsData->phandle);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtRscsAppDeinit(void **gash)
{
    CsrUint16 msg_type;
    void *msg_data;
    CsrBtRscsAppInstData *rscsData;

    rscsData = (CsrBtRscsAppInstData *) (*gash);
    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    while ( CsrSchedMessageGet(&msg_type, &msg_data) )
    {
        switch (msg_type)
        {
            case CSR_BT_GATT_PRIM:
                CsrBtGattFreeUpstreamMessageContents(msg_type, msg_data);
                break;
        }
        CsrPmemFree(msg_data);
    }
    CsrCmnListDeinit(&rscsData->clientConfigList);
    CsrPmemFree(rscsData);
}
#else
void CsrBtRscsAppDeinit(void **gash)
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
void CsrBtRscsAppHandler(void **gash)
{
    CsrBtRscsAppInstData *rscsData;
    CsrUint16    eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    rscsData = (CsrBtRscsAppInstData *) (*gash);

    if(!rscsData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &rscsData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&rscsData->saveQueue, &eventType , &rscsData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
             a message from the scheduler                                                   */
            rscsData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &rscsData->recvMsgP);
        }
        else
        { /* A message has been restored from the local queed                               */
            ;
        }
    }

    switch (eventType)
    {
        case CSR_BT_SD_PRIM:
        {
            CsrBtRscsAppHandleSdPrim(rscsData);
            break;
        }
        case CSR_BT_GATT_PRIM:
        {
            CsrBtRscsAppHandleRscsPrim(rscsData);
            break;
        }

        case CSR_BT_RSCS_APP_PRIM:
        {
            CsrPrim *type = (CsrPrim *) rscsData->recvMsgP;

            switch(*type)
            {
                case CSR_BT_RSCS_APP_REGISTER_REQ:
                { /* only received by controling rscs demo app instance */
                    if(rscsData->initialized)
                    {
                        CsrBtRscsAppRegisterReq *prim = rscsData->recvMsgP;
                        CsrSchedQid queueId = CSR_SCHED_QID_INVALID;

                        rscsData->rscsAppHandle = prim->phandle;
                        queueId = rscsData->rscsProfileHandle;
                        if(queueId != CSR_SCHED_QID_INVALID)
                        {
                            csrBtRscsAppSendRscsAppRegisterCfm(prim->phandle, queueId,1);
                        }
                        else
                        {
                            CsrGeneralException("CSR_RSCS_DEMO_APP",
                                                0,
                                                *type,
                                                "No CsrBtRscs profile Queue Handle Available");
                        }
                    }
                    else
                    {
                        CsrMessageQueuePush(&rscsData->saveQueue, CSR_BT_RSCS_APP_PRIM, rscsData->recvMsgP);
                        rscsData->recvMsgP = NULL;
                    }
                    break;
                }
                case CSR_BT_RSCS_APP_REGISTER_CFM:
                { /* Only received by extra rscs demo app instances */
                    rscsData->profileName = csrBtRscsAppReturnRscsInstName(rscsData);

                    CsrBtGattAppHandleRscsSetMainMenuHeader(rscsData);

                    rscsData->initialized = TRUE;

                    CsrAppRegisterReqSend(rscsData->phandle, TECH_BTLE, PROFILE_NAME(rscsData->profileName));
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
            CsrPrim *prim = (CsrPrim *) rscsData->recvMsgP;
            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *prim = rscsData->recvMsgP;
                rscsData->selectedDeviceAddr.addr.lap = prim->deviceAddr.lap;
                rscsData->selectedDeviceAddr.addr.nap = prim->deviceAddr.nap;
                rscsData->selectedDeviceAddr.addr.uap = prim->deviceAddr.uap;
                rscsData->selectedDeviceAddr.type     = (prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? TBDADDR_RANDOM:TBDADDR_PUBLIC;
                CsrAppBacklogReqSend(TECH_BTLE,
                                     PROFILE_NAME(rscsData->profileName),
                                     FALSE,
                                     "Received selected DeviceAddr: %04X:%02X:%06X (%s)",
                                     prim->deviceAddr.nap,
                                     prim->deviceAddr.uap,
                                     prim->deviceAddr.lap,
                                     (CsrBtAddrIsRandom(rscsData->selectedDeviceAddr))? "Random":"Public");
                /*update UI*/
                CsrUiUieShowReqSend(rscsData->csrUiVar.hMainMenu,
                                    CsrSchedTaskQueueGet(),
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_RSCS_APP_CSR_UI_PRI);
            }
            else
            {
                CsrPrim *prim = (CsrPrim *) rscsData->recvMsgP;
                if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
                {
                    CsrBtGapAppGetSelectedDeviceCfm *prim = rscsData->recvMsgP;
                    rscsData->selectedDeviceAddr.addr.lap = prim->deviceAddr.lap;
                    rscsData->selectedDeviceAddr.addr.nap = prim->deviceAddr.nap;
                    rscsData->selectedDeviceAddr.addr.uap = prim->deviceAddr.uap;
                    rscsData->selectedDeviceAddr.type     = (prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? TBDADDR_RANDOM:TBDADDR_PUBLIC;
                    rscsData->selectedInfo                = prim->deviceStatus;
                    
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(rscsData->profileName),
                                         FALSE, 
                                         "Received selected DeviceAddr: %04X:%02X:%06X (%s)", 
                                         prim->deviceAddr.nap, 
                                         prim->deviceAddr.uap, 
                                         prim->deviceAddr.lap,
                                         (CsrBtAddrIsRandom(rscsData->selectedDeviceAddr))? "Random":"Public");
                    /*update UI*/
                    CsrUiUieShowReqSend(rscsData->csrUiVar.hMainMenu,
                                        CsrSchedTaskQueueGet(),
                                        CSR_UI_INPUTMODE_AUTO,
                                        CSR_BT_RSCS_APP_CSR_UI_PRI);
                    
                }
                else
                {
                    /* unexpected primitive received */
                    CsrGeneralException("CSR_BT_RSCS_APP",
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtRscsAppHandler,");
                }
                break;
            }
            break;
        }
        case CSR_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) rscsData->recvMsgP;

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
                CsrGeneralException("CSR_BT_RSCS_APP",
                                    0, 
                                    *prim,
                                    "####### Unhandled CSR_APP_PRIM in CsrBtRscsAppHandler,");
            }
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrBtGattAppHandleRscsCsrUiPrim(rscsData);
            break;
        }

        default:
        {
            /* unexpected primitive received */
            CsrGeneralException("CSR_BT_RSCS_APP",
                                0,
                                *((CsrPrim *) rscsData->recvMsgP),
                                "####### default in CsrBtRscsAppHandler,");
        }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(rscsData->recvMsgP);
}
/******************************************************************************
 * Function to handle all SD-primitives.
 ******************************************************************************/
void CsrBtRscsAppHandleSdPrim(CsrBtRscsAppInstData *rscsData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) rscsData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_SD_READ_DEVICE_INFO_CFM:
        {
            /*lets see if the device is paired or not - add it to client config list if it does not exist */
            CsrBtSdReadDeviceInfoCfm *cfm = (CsrBtSdReadDeviceInfoCfm*)rscsData->recvMsgP;
            CsrBtRscsAppClientConfigElement *cElem = CSR_BT_RSCS_APP_FIND_CLIENT_CONFIG_BY_ADDR(rscsData->clientConfigList, (void*)(&cfm->deviceAddr));
            if(!cElem)
            {
                cElem = CSR_BT_RSCS_APP_ADD_CLIENT_CONFIG(rscsData->clientConfigList);
                cElem->deviceAddr.lap = cfm->deviceAddr.lap;
                cElem->deviceAddr.nap = cfm->deviceAddr.nap;
                cElem->deviceAddr.uap = cfm->deviceAddr.uap;
            }
            cElem->paired = ((cfm->deviceStatus & CSR_BT_SD_STATUS_PAIRED) == CSR_BT_SD_STATUS_PAIRED)?TRUE:FALSE;
            break;
        }
        default:
        {
            CsrAppBacklogReqSend(TECH_BTLE,
                                 PROFILE_NAME(rscsData->profileName),
                                 TRUE,
                                 "####### default in SD prim handler 0x%04x,",
                                 *primType);
            CsrBtGattFreeUpstreamMessageContents(CSR_BT_SD_PRIM, rscsData->recvMsgP);
            /* unexpected primitive received */

        }


    }
}

static void csrBtRscsAppHandleClientConfigWrite(CsrBtRscsAppInstData* rscsData,
                                                CsrBtGattDbAccessWriteInd* ind,
                                                CsrUint16 handle)
{
    CsrBtResultCode result = CSR_BT_GATT_RESULT_SUCCESS;
    CsrBtRscsAppClientConfigElement *cElem = CSR_BT_RSCS_APP_FIND_CLIENT_CONFIG_BY_ADDR(rscsData->clientConfigList,
                                                                                        (void* )(&rscsData->connectedDeviceAddr.addr));
    if (!cElem)
    {
        /*We really should never get in-here as the clientConfig should have been added elsewhere */
        /* no previous clientConfig record in list
         * lets add one */
        cElem = CSR_BT_RSCS_APP_ADD_CLIENT_CONFIG(rscsData->clientConfigList);
        cElem->deviceAddr = rscsData->connectedDeviceAddr.addr;
        cElem->measurementValue = NULL;
        cElem->scCtrlPointValue = NULL;
        cElem->connectionFailed = FALSE;
    }

    if (ind->writeUnit[0].valueLength == 2) /* Check if length matches */
    {
        CsrUint16 clientConfigVal = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(ind->writeUnit[0].value);

        switch (handle)
        {
            case CSR_BT_GATT_APP_RSC_MEASUREMENT_CC_HANDLE:
            {
                if (clientConfigVal == CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT
                    || clientConfigVal == CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION)
                {
                    /*update with latest write value - this is stored per device */
                    CsrPmemFree(cElem->measurementValue);
                    cElem->measurementValue = ind->writeUnit[0].value;
                }
                else
                {
                    /* Illegal value for this client configuration descriptor */
                    result = CSR_BT_GATT_ACCESS_RES_CLIENT_CONFIG_IMPROPERLY_CONF;
                }
                break;
            }

            case CSR_BT_GATT_APP_RSC_SC_CONTROL_POINT_CC_HANDLE:
            {
                if (clientConfigVal == CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT
                    || clientConfigVal == CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION)
                {
                    /*update with latest write value - this is stored per device */
                    CsrPmemFree(cElem->scCtrlPointValue);
                    cElem->scCtrlPointValue = ind->writeUnit[0].value;
                }
                else
                {
                    /* Illegal value for this client configuration descriptor */
                    result = CSR_BT_GATT_ACCESS_RES_CLIENT_CONFIG_IMPROPERLY_CONF;
                }
                    break;
            }

            default:
            result = CSR_BT_GATT_ACCESS_RES_UNLIKELY_ERROR;
                break;
        }
    }
    else
    {
        result = CSR_BT_GATT_ACCESS_RES_INVALID_LENGTH;
    }

    if (result != CSR_BT_GATT_RESULT_SUCCESS)
    {
        CsrPmemFree(ind->writeUnit[0].value);
    }

    CsrBtGattDbWriteAccessResSend(rscsData->gattId,
                                  ind->btConnId,
                                  ind->writeUnit[0].attrHandle,
                                  result);

    /* the value pointers have been passed on, free the member
     * and set reference to NULL to avoid free of values in use.*/
    CsrPmemFree(ind->writeUnit);
    ind->writeUnit = NULL;
}

/******************************************************************************
 * Function to handle all rscs-primitives.
 ******************************************************************************/
void CsrBtRscsAppHandleRscsPrim(CsrBtRscsAppInstData *rscsData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) rscsData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_GATT_REGISTER_CFM:
        {
            CsrBtGattRegisterCfm *cfm = (CsrBtGattRegisterCfm*)rscsData->recvMsgP;
            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                rscsData->gattId = cfm->gattId;
                /*update UI*/
                CsrBtGattAppHandleRscsCsrUiRegisterUnregister(rscsData, TRUE);
                /* subscribe to notifications about new physical link connections/loss - needed for clientConfig */
                CsrBtGattSetEventMaskReqSend(cfm->gattId,CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_NONE /*PHYSICAL_LINK_STATUS*/);
            }
            else
            {
                /*update UI*/
                CsrBtGattAppHandleRscsCsrUiRegisterUnregister(rscsData, FALSE);
            }

            break;
        }
        case CSR_BT_GATT_SET_EVENT_MASK_CFM:
        {
            /*event mask set, now lets move on to DB setup */
            /* Allocate and add DB in Gatt*/
            CsrBtGattDbAllocReqSend(rscsData->gattId, CSR_BT_RSCS_DB_HANDLE_COUNT, CSR_BT_RSCS_DB_PREFERRED_HANDLE );
            break;
        }
        case CSR_BT_GATT_PHYSICAL_LINK_STATUS_IND:
        {
            /* a physical link has either been connected or disconnected
             * we need to get info about the connection in order to add/remove the item from
             * the client config list
             */
            CsrBtGattPhysicalLinkStatusInd *ind = (CsrBtGattPhysicalLinkStatusInd*)rscsData->recvMsgP;
            CsrBtSdReadDeviceInfoReqSendEx(rscsData->phandle, ind->address.addr, ind->address.type, 0);
            break;
        }
        case CSR_BT_GATT_DB_ADD_CFM:
        {
            break;
        }
        case CSR_BT_GATT_UNREGISTER_CFM:
        {
            /*update UI*/
            CsrBtGattAppHandleRscsCsrUiRegisterUnregister(rscsData, FALSE);

            break;
        }
        case CSR_BT_GATT_DB_ALLOC_CFM:
        {
            CsrBtGattDbAllocCfm *cfm = (CsrBtGattDbAllocCfm*)rscsData->recvMsgP;
            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                CsrBtGattDb *db = CsrBtRscsDbCreate(cfm->start);
                CsrBtGattDbAddReqSend(rscsData->gattId, db);
                rscsData->dbStartHandle = cfm->start;
                rscsData->dbEndHandle = cfm->end;


            }
            else
            {
                /*We could show error about not being able to allocate */
            }


            break;
        }
        case CSR_BT_GATT_CONNECT_IND:
        {
            CsrBtGattConnectInd        *ind;

            ind = (CsrBtGattConnectInd *) rscsData->recvMsgP;
            /* Crank state to disconnect the other */
            CsrBtRscsAppCrank(rscsData);


            if (ind->resultSupplier == CSR_BT_SUPPLIER_GATT &&
                    ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                CsrAppBacklogReqSend(TECH_BTLE,
                                     PROFILE_NAME(rscsData->profileName),
                                     TRUE,
                                     "Successfully Connected with: %04X:%02X:%06X",
                                     ind->address.addr.nap,
                                     ind->address.addr.uap,
                                     ind->address.addr.lap);
                rscsData->connectedDeviceAddr.addr.nap = ind->address.addr.nap;
                rscsData->connectedDeviceAddr.addr.lap = ind->address.addr.lap;
                rscsData->connectedDeviceAddr.addr.uap = ind->address.addr.uap;
                rscsData->update_timer = CsrSchedTimerSet(CSR_BT_RSCS_APP_UPDATE_TIMER_INTERVAL,
                                                          CsrBtRscsAppUpdateTimerInterupt,
                                                          0,
                                                          (void*)rscsData);

                if (ind->btConnId == rscsData->leBtConnId)
                {
                    rscsData->state = CSR_BT_RSCS_ST_LE;
                    rscsData->connectedBtConnId = rscsData->leBtConnId;
                }
                else
                {
                    rscsData->bredrBtConnId = ind->btConnId;
                    rscsData->state = CSR_BT_RSCS_ST_BREDR;
                    rscsData->connectedBtConnId = rscsData->bredrBtConnId;
                }

            }
            else
            {
                CsrAppBacklogReqSend(TECH_BTLE,
                                     PROFILE_NAME(rscsData->profileName),
                                     TRUE,
                                     "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d",
                                     ind->address.addr.nap,
                                     ind->address.addr.uap,
                                     ind->address.addr.lap,
                                     ind->resultCode,
                                     ind->resultSupplier);
                if (ind->btConnId == rscsData->leBtConnId)
                {
                    rscsData->leBtConnId = CSR_BT_CONN_ID_INVALID;
                    rscsData->state = CSR_BT_RSCS_ST_LISTEN;
                }
                else
                {
                    rscsData->bredrBtConnId = CSR_BT_RSCS_BTCONN_UNSET;
                    rscsData->state = CSR_BT_RSCS_ST_LISTEN;
                }

            }

            /* Crank state */
            CsrBtRscsAppCrank(rscsData);

            /*update UI*/
            CsrBtGattAppHandleRscsCsrUiActivateDeactivate(rscsData,
                                                          TRUE,
                                                          (CsrBool)((ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS &&
                                                                  ind->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));
            break;
        }
        case CSR_BT_GATT_DISCONNECT_IND:
        {
            CsrBtGattDisconnectInd    *ind;

            ind      = (CsrBtGattDisconnectInd *) rscsData->recvMsgP;
            /* Determine which one got disconnected */
            
            if (ind->btConnId == rscsData->connectedBtConnId)
            {
                rscsData->connectedBtConnId = CSR_BT_CONN_ID_INVALID;
                            /*update UI*/
                CsrBtGattAppHandleRscsCsrUiActivateDeactivate(rscsData,
                                                              FALSE,
                                                              TRUE);
                /* Crank state in case we need to reconnect */
                rscsData->state = CSR_BT_RSCS_ST_LISTEN;
                CsrBtRscsAppCrank(rscsData);

                CsrAppBacklogReqSend(TECH_BTLE,
                                     PROFILE_NAME(rscsData->profileName),
                                     TRUE,
                                     "Disconnected from: %04X:%02X:%06X",
                                     ind->address.addr.nap,
                                     ind->address.addr.uap,
                                     ind->address.addr.lap);
                /*reset instance data */

                /*Stop the UPDATE TIMER */
                CsrSchedTimerCancel(rscsData->update_timer,
                                    0,
                                    NULL);
                /*Clear all handles in instance */
                CsrBtRscsAppClearHandles(rscsData);

            }

            if (ind->btConnId == rscsData->leBtConnId)
            {
                rscsData->leBtConnId = CSR_BT_CONN_ID_INVALID;
            }
            else if (ind->btConnId == rscsData->bredrBtConnId)
            {
                rscsData->bredrBtConnId = CSR_BT_RSCS_BTCONN_UNSET;
            }
         
            break;
        }
        case CSR_BT_GATT_PERIPHERAL_CFM:
        {
            CsrBtGattPeripheralCfm *cfm;
            cfm = (CsrBtGattPeripheralCfm*) rscsData->recvMsgP;

            if(cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
            {
                /*We only care about non-successful as success will be handled on CONNECT_IND*/
                CsrBtGattAppHandleRscsCsrUiActivateDeactivate(rscsData,
                                                              TRUE,
                                                              (CsrBool)((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS &&
                                                                      cfm->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));
                rscsData->leBtConnId = CSR_BT_CONN_ID_INVALID;

            }  else if ((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                    && (cfm->resultSupplier == CSR_BT_SUPPLIER_GATT))
            {
                /* ConnId assigned, but not yet connected */
                rscsData->leBtConnId = cfm->btConnId;
            }
            break;
        }
        case CSR_BT_GATT_BREDR_ACCEPT_CFM:
        {
            CsrBtGattBredrAcceptCfm *cfm;
            cfm = (CsrBtGattBredrAcceptCfm*) rscsData->recvMsgP;

            if ((cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
                    || (cfm->resultSupplier != CSR_BT_SUPPLIER_GATT))
            {
                /* We only care about non-successful as success
                 * will be handled on CONNECT_IND*/
                CsrBtGattAppHandleRscsCsrUiActivateDeactivate(rscsData,
                                                              TRUE,
                                                              FALSE); /* error */
                rscsData->bredrBtConnId = CSR_BT_RSCS_BTCONN_UNSET;
            }
            else if ((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                    && (cfm->resultSupplier == CSR_BT_SUPPLIER_GATT))
            {
                /* ConnId assigned, but not yet connected */
                rscsData->bredrBtConnId = cfm->btConnId;
            }
            break;
        }

        case CSR_BT_GATT_DB_ACCESS_WRITE_IND:
        {
            CsrBtGattDbAccessWriteInd *ind = (CsrBtGattDbAccessWriteInd*) rscsData->recvMsgP;
            CsrUint16 handle = ind->writeUnit[0].attrHandle - rscsData->dbStartHandle;

            switch(handle)
            {
                case CSR_BT_GATT_APP_RSC_MEASUREMENT_CC_HANDLE: /*client config change*/
                {
                    csrBtRscsAppHandleClientConfigWrite(rscsData, ind, handle);
                    break;
                }
                case CSR_BT_GATT_APP_RSC_SC_CONTROL_POINT_HANDLE+1: /*sc_controlpoint value handle*/
                {
                    CsrUint8  *response = NULL;
                    CsrUint8   respSize = 0;

                    /* Validate if Client Characteristic Configuration Descriptor is configured for Indications */
                    CsrBtRscsAppClientConfigElement *cElem = CSR_BT_RSCS_APP_FIND_CLIENT_CONFIG_BY_ADDR(rscsData->clientConfigList, (void*)(&rscsData->connectedDeviceAddr.addr));
                    if(!cElem || !(*(cElem->scCtrlPointValue) & (CsrUint8)CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION))
                    {
                        /* send error */
                        CsrBtGattDbWriteAccessResSend(rscsData->gattId,
                                                    rscsData->connectedBtConnId,
                                                    ind->writeUnit[0].attrHandle,
                                                    CSR_BT_GATT_ACCESS_RES_IMPROPERLY_CONFIGURED);

                    }
                    else
                    {
                        /*we need to respond according to the opCode in [0]*/
                        switch(ind->writeUnit[0].value[0])
                        {
                            case 0x01: /*set cumulative value*/
                            {
                                respSize = 3;
                                response    = CsrPmemZalloc(respSize);
                                response[1] = ind->writeUnit[0].value[0];
                                response[0] = 0x10; /* Response opCode */

                                if(rscsData->rscData.feature & CSR_BT_RSCS_APP_FEATURE_TOTAL_DISTANCE) 
                                {
                                    response[2] = 0x01; /*success */
                                    rscsData->rscData.distance = (CsrUint32)((ind->writeUnit[0].value[1]<<24)|(ind->writeUnit[0].value[2]<<16)|(ind->writeUnit[0].value[3]<<8)|(ind->writeUnit[0].value[4]));

                                }
                                else
                                {
                                    response[2] = 0x02; /* not supported */
                                }
                                break;
                            }
                            case 0x02: /* Start Sensor Calibration */
                            {
                                respSize = 3;
                                /* no value for calibration is stored */
                                response    = CsrPmemZalloc(respSize);
                                response[1] = ind->writeUnit[0].value[0];
                                response[0] = 0x10; /* Response opCode */
                                if(rscsData->rscData.feature & CSR_BT_RSCS_APP_FEATURE_SENSOR_CALIBRATION) 
                                {
                                    response[2] = 0x01; /*success */
                                }
                                else
                                {
                                    response[2] = 0x02; /* not supported */
                                }
                                break;
                            }
                            case 0x03: /* update Sensor location */
                            {
                                CsrUint8 x = 0;
                                CsrBool found = FALSE;
                                CsrUint8 locations[] = CSR_BT_RSCS_APP_SUPPORTED_SENSOR_LOCATIONS;
                                respSize = 3;
                                response = CsrPmemZalloc(respSize);
                                response[1] = ind->writeUnit[0].value[0];
                                response[0] = 0x10; /* Response opCode */
                                if(rscsData->rscData.feature & CSR_BT_RSCS_APP_FEATURE_MULTI_SENSOR_LOCATION) 
                                {
                                    for(x=0;x < sizeof(locations);x++)
                                    {
                                        if(locations[x] == ind->writeUnit[0].value[1])
                                        {
                                            found = TRUE;
                                        }
                                    }
                                    if(found)
                                    {
                                        rscsData->rscData.sensorLocation = ind->writeUnit[0].value[1];
                                        response[2] = 0x01; /* success */
                                    }
                                    else
                                    {
                                        response[2] = 0x03; /* bad parameter */
                                    }
                                }
                                else
                                {
                                    response[2] = 0x02; /* not supported*/
                                }
                                break;
                            }
                            case 0x04: /* request supported sensor locations */
                            {
                                CsrUint8 locations[] = CSR_BT_RSCS_APP_SUPPORTED_SENSOR_LOCATIONS;

                                CsrUint8 x = 0;
                                respSize = 3;
                                if(rscsData->rscData.feature & CSR_BT_RSCS_APP_FEATURE_MULTI_SENSOR_LOCATION) 
                                {
                                    respSize += CSR_BT_RSCS_APP_SUPPORTED_SENSOR_LOCATIONS_COUNT;
                                }
                                response = CsrPmemZalloc(respSize);
                                response[1] = ind->writeUnit[0].value[0];
                                response[0] = 0x10; /* Response opCode */
                                if(rscsData->rscData.feature & CSR_BT_RSCS_APP_FEATURE_MULTI_SENSOR_LOCATION) 
                                {
                                    response[2] = 0x01; /*success */
                                    for(x=0;x<sizeof(locations); x++)
                                    {
                                        response[3+x] = locations[x];
                                    }
                                }
                                else
                                {
                                    response[2] = 0x02; /* not supported */
                                }
                                break;
                            }
                            default:
                            {
                                respSize = 3;
                                response    = CsrPmemZalloc(respSize);
                                response[1] = ind->writeUnit[0].value[0];
                                response[0] = 0x10; /* Response opCode */
                                response[2] = 0x04; /*operation failed */
                                break;
                            }

                        }
                        /*allow write */
                        CsrBtGattDbWriteAccessResSend(rscsData->gattId,
                                                    rscsData->connectedBtConnId,
                                                    ind->writeUnit[0].attrHandle,
                                                    CSR_BT_GATT_ACCESS_RES_SUCCESS);

                        /*send updated notification */
                        CsrBtGattIndicationEventReqSend(rscsData->gattId, rscsData->connectedBtConnId,
                                                        (CsrUint16)(ind->writeUnit[0].attrHandle),respSize,(CsrUint8*)response);
                    }
                    break;
                }
                case CSR_BT_GATT_APP_RSC_SC_CONTROL_POINT_CC_HANDLE: /*client config change*/
                {
                    csrBtRscsAppHandleClientConfigWrite(rscsData, ind, handle);
                    break;
                }
            }
            break;
        }
        case CSR_BT_GATT_DB_ACCESS_READ_IND:
        {
            CsrBtGattDbAccessReadInd *ind = (CsrBtGattDbAccessReadInd*)rscsData->recvMsgP;
            /* [QTI] Fix KW issue#32601. */
            CsrUint8 *val = NULL;
            CsrUint8 size = 0;
            CsrUint16 tmpHandle = ind->attrHandle - rscsData->dbStartHandle;
            switch(tmpHandle)
            {
                case CSR_BT_GATT_APP_RSC_MEASUREMENT_HANDLE+1: /* measurement value handle */
                {
                    size = csrBtRscAppBuildMeasurementValue(rscsData->rscData.flags, rscsData->rscData.speed, rscsData->rscData.cadance, rscsData->rscData.stride, rscsData->rscData.distance, &val);
                    break;
                }
                case CSR_BT_GATT_APP_RSC_MEASUREMENT_CC_HANDLE: /* measurement value handle */
                {
                    CsrBtRscsAppClientConfigElement *cElem = CSR_BT_RSCS_APP_FIND_CLIENT_CONFIG_BY_ADDR(rscsData->clientConfigList, (void*)(&rscsData->connectedDeviceAddr.addr));
                    if(!cElem)
                    {
                        /*We really should never get in-here as the clientConfig should have been added elsewhere */
                        /* no previous clientConfig record in list
                         * lets add one */
                        cElem = CSR_BT_RSCS_APP_ADD_CLIENT_CONFIG(rscsData->clientConfigList);
                        cElem->deviceAddr = rscsData->connectedDeviceAddr.addr;
                    }
                    /*update with latest write value - this is stored per device */
                    size = sizeof(CsrUint16);
                    val = CsrPmemZalloc(size);/*default value is 0 (off)*/
                    break;
                }
                case CSR_BT_GATT_APP_RSC_FEATURE_HANDLE+1: /*feature value handle */
                {
                    size = sizeof(rscsData->rscData.feature);
                    val = CsrPmemZalloc(size);
                    val[0] = (CsrUint8)((rscsData->rscData.feature & 0x00FF));
                    val[1] = (CsrUint8)(rscsData->rscData.feature & 0xFF00 >> 8);
                    break;
                }
                case CSR_BT_GATT_APP_RSC_SENSOR_LOCATION_HANDLE+1: /*sensor location value handle */
                {
                    size = sizeof(rscsData->rscData.sensorLocation);
                    val = CsrPmemZalloc(size);
                    val[0] = (CsrUint8)(rscsData->rscData.sensorLocation);
                    
                    break;
                }
                case CSR_BT_GATT_APP_RSC_SC_CONTROL_POINT_HANDLE+1:
                {
                    break;
                }
                case CSR_BT_GATT_APP_RSC_SC_CONTROL_POINT_CC_HANDLE:
                {
                    CsrBtRscsAppClientConfigElement *cElem = CSR_BT_RSCS_APP_FIND_CLIENT_CONFIG_BY_ADDR(rscsData->clientConfigList, (void*)(&rscsData->connectedDeviceAddr.addr));
                    if(!cElem)
                    {
                        /*We really should never get in-here as the clientConfig should have been added elsewhere */
                        /* no previous clientConfig record in list
                            * lets add one */
                        cElem = CSR_BT_RSCS_APP_ADD_CLIENT_CONFIG(rscsData->clientConfigList);
                        cElem->deviceAddr = rscsData->connectedDeviceAddr.addr;
                    }
                    /*update with latest write value - this is stored per device */
                    size = sizeof(CsrUint16);
                    val = CsrPmemZalloc(size); /*default value is 0 (off)*/
                    break;
                }
            }


            if(size)
            {
                if(ind->offset)
                {/* we need to support offsetted read requests*/
                    CsrUint8 * valOffsetted = NULL;
                    size = size-ind->offset;
                    valOffsetted = CsrPmemZalloc(size);
                    CsrMemCpy(valOffsetted, &val[ind->offset], size);
                    CsrBtGattDbReadAccessResSend(rscsData->gattId,
                                                 rscsData->connectedBtConnId,
                                                 ind->attrHandle,
                                                 CSR_BT_GATT_ACCESS_RES_SUCCESS,
                                                 size,
                                                 valOffsetted);
                    CsrPmemFree(val);
                }
                else
                {
                    /* tell Gatt that read is ok*/
                    CsrBtGattDbReadAccessResSend(rscsData->gattId,
                                                 rscsData->connectedBtConnId,
                                                 ind->attrHandle,
                                                 CSR_BT_GATT_ACCESS_RES_SUCCESS,
                                                 size,
                                                 val);
                }
            }
            else
            {
                CsrBtGattDbReadAccessResSend(rscsData->gattId,
                                             rscsData->connectedBtConnId,
                                             ind->attrHandle,
                                             CSR_BT_GATT_ACCESS_RES_INVALID_HANDLE,
                                             0,
                                             NULL);
            }
            break;
        }
        default:
        {
            CsrAppBacklogReqSend(TECH_BTLE,
                                 PROFILE_NAME(rscsData->profileName),
                                 TRUE,
                                 "####### default in gatt prim handler 0x%04x,",
                                 *primType);
            CsrBtGattFreeUpstreamMessageContents(CSR_BT_GATT_PRIM, rscsData->recvMsgP);
            /* unexpected primitive received */
        }
    }
}

