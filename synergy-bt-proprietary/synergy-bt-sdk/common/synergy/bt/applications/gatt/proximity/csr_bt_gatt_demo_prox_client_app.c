/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
*****************************************************************************/

#include "csr_synergy.h"

#include <string.h>
#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_list.h"
#include "csr_exceptionhandler.h"

#include "csr_app_lib.h"

#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"

#include "csr_bt_cm_prim.h"
#include "csr_bt_cm_lib.h"

#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_prox_client_app_private_prim.h"
#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)
#include "csr_bt_gatt_demo_prox_client_app_ui_sef.h"
#include "csr_bt_gatt_app_task.h"
#include "csr_bt_gatt_demo_db_utils.h"

/* Forward declarations for handler functions */
void CsrBtProxcAppHandleProxcPrim(CsrBtProxcAppInstData *proxcData);
void CsrBtProxcAppHandleCmPrim(CsrBtProxcAppInstData *proxcData);

CsrBtUuid *CsrBtProxcAppUuid16ToUuid128(CsrUint16 uuid)
{
    CsrBtUuid *realUuid = CsrPmemAlloc(sizeof(CsrBtUuid));
    CSR_COPY_UINT16_TO_LITTLE_ENDIAN(uuid, realUuid->uuid);
    
    realUuid->length = CSR_BT_UUID16_SIZE;

    return realUuid;
} 

void CsrBtProxcAppRssiTimerInterupt(CsrUint16 mi, void *mv)
{
    CsrBtProxcAppInstData *inst = (CsrBtProxcAppInstData*)mv;
    CsrBtDeviceAddr addr;
    addr.lap = inst->connectedDeviceAddr.addr.lap;
    addr.uap = inst->connectedDeviceAddr.addr.uap;
    addr.nap = inst->connectedDeviceAddr.addr.nap;
    CsrBtCmReadRssiReqSend(inst->phandle, addr);
    inst->rssi_timer = CsrSchedTimerSet(CSR_BT_PROXC_APP_RSSI_TIMER_INTERVAL,
                                        CsrBtProxcAppRssiTimerInterupt,
                                        mi,
                                        mv);

}

/******************************************************************************
CsrBtProxcAppStartRegister:
 *****************************************************************************/
void CsrBtProxcAppStartRegister(CsrBtProxcAppInstData *proxcData)
{
    CsrBtGattRegisterReqSend(proxcData->phandle,
                             1234);
}
/******************************************************************************
CsrBtProxcAppStartUnregister:
 *****************************************************************************/
void CsrBtProxcAppStartUnregister(CsrBtProxcAppInstData *proxcData)
{
    CsrBtGattUnregisterReqSend(proxcData->gattId);
}
/******************************************************************************
CsrBtProxcAppStartConnecting:
 *****************************************************************************/
void CsrBtProxcAppStartConnecting(CsrBtProxcAppInstData *proxcData)
{
    if(proxcData->selectedDeviceAddr.addr.uap != 0)
    {
        if(proxcData->selectedInfo & CSR_BT_SD_STATUS_RADIO_LE)
        {
            CsrBtGattCentralReqSend(proxcData->gattId,
                                    proxcData->selectedDeviceAddr,
                                    CSR_BT_GATT_FLAGS_NONE,
                                    0); /* preferred mtu */
        }
        else
        {
            CsrBtGattBredrConnectReqSend(proxcData->gattId,
                                         proxcData->selectedDeviceAddr,
                                         CSR_BT_GATT_FLAGS_NONE);
        }
    }
    else
    {
        /*no device addr have been selected in GAP yet*/        
        CsrBtGattAppHandleProxcCsrUiNoSelectedDevAddr(proxcData);
    }
}
/******************************************************************************
CsrBtProxcAppStartReadUuid:
 *****************************************************************************/
void CsrBtProxcAppStartReadUuid(CsrBtProxcAppInstData *proxcData, CsrUint16 uuid)
{
    CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend(proxcData->gattId, proxcData->btConnId, uuid);

}
void CsrBtProxcAppStartReadValue(CsrBtProxcAppInstData *proxcData, CsrUint16 pUuid, CsrUint16 cUuid)
{
    CsrUint16  valueHandle;
    CsrBtGattAppDbPrimServiceElement *pElem;
    CsrBtGattAppDbCharacElement *cElem;
    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(proxcData->dbElement->dbPrimServices,
                                                         CsrBtProxcAppUuid16ToUuid128(pUuid));
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtProxcAppUuid16ToUuid128(cUuid));
    valueHandle = cElem->valueHandle;

    CsrBtGattReadReqSend(proxcData->gattId,
                         proxcData->btConnId,
                         valueHandle,
                         0);

}

void CsrBtProxcAppStartWriteValue(CsrBtProxcAppInstData *proxcData, CsrUint16 pUuid, CsrUint16 cUuid)
{
    CsrUint16  valueHandle;
    CsrBtGattAppDbPrimServiceElement *pElem;
    CsrBtGattAppDbCharacElement *cElem;
    CsrUint8 *writeValue = NULL;
    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(proxcData->dbElement->dbPrimServices,
                                                         CsrBtProxcAppUuid16ToUuid128(pUuid));
    if(pElem == NULL)
    {
        return; /* the prim Service was not found */
    }
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtProxcAppUuid16ToUuid128(cUuid));

    if(cElem == NULL)
    {
        return; /* the charac was not found */
    }
    /* [QTI] Fix KW issue#831928 through moving the issued statement from above to here. */
    valueHandle = cElem->valueHandle;

    if(cElem->value == NULL)
    {
        /*fix as we know that linkloss and iAlert values are write-only but default 0x00*/
        cElem->value = CsrPmemZalloc(sizeof(CsrUint8));
        cElem->valueLength = sizeof(CsrUint8);

    }
    
    proxcData->writeValueLength = 1;
    writeValue = CsrPmemZalloc(1);
    proxcData->writeValue = CsrPmemZalloc(1);
    switch(pUuid)
    {
        case CSR_BT_GATT_UUID_LINK_LOSS_SERVICE:
            {
                if(cElem->value[0]>=CSR_BT_GATT_APP_ALERT_LEVEL_HIGH)
                {
                    writeValue[0]   = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                    cElem->value[0] = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;

                }
                else
                {
                    cElem->value[0] += 1;
                    writeValue[0]    = (CsrUint8)cElem->value[0];
                }
                CsrBtGattWriteReqSend(proxcData->gattId,
                                      proxcData->btConnId,
                                      valueHandle,
                                      0,
                                      proxcData->writeValueLength,
                                      writeValue);
                break;
            }
        case CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE:
            {
                if(CSR_BT_GATT_APP_ALERT_LEVEL_OFF != cElem->value[0])
                {
                    cElem->value[0]          = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                    writeValue[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                    proxcData->writeValue[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                }
                else
                {
                    cElem->value[0]          = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_HIGH;
                    writeValue[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_HIGH;
                    proxcData->writeValue[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_HIGH;

                }
                CsrBtGattWriteCmdReqSend(proxcData->gattId,
                                      proxcData->btConnId,
                                      valueHandle,
                                      proxcData->writeValueLength,
                                      writeValue);
                break;
            }
    }

    CsrAppBacklogReqSend(TECH_BTLE,      PROFILE_NAME(proxcData->profileName),
                                         TRUE,
                                         "wrote value %d to valuehandle %d",
                                         writeValue,
                                         valueHandle);

}



/******************************************************************************
CsrBtProxcAppStartDisconnect:
 *****************************************************************************/
void CsrBtProxcAppStartDisconnect(CsrBtProxcAppInstData *proxcData)
{
    /*stop RSSI timer*/
    CsrSchedTimerCancel(proxcData->rssi_timer,NULL,NULL);


    CsrBtGattDisconnectReqSend(proxcData->gattId, proxcData->btConnId);
    /*reset BT connection ID */
    proxcData->btConnId = 0xff;
}

/******************************************************************************
CsrBtProxcAppInitInstanceData:
 *****************************************************************************/
void CsrBtProxcAppClearHandles(CsrBtProxcAppInstData *proxcData){

    /*init list for local database structure copy */                          
    CsrCmnListInit(&proxcData->dbElement->dbPrimServices, 0, CsrBtGattAppInitDbPrimList, CsrBtGattAppFreeDbPrimList);
    proxcData->dbElement->dbPrimServicesCount  =  0;
    proxcData->dbElement->traversingDb        =  FALSE;
    

}
void CsrBtProxcAppInitInstanceData(CsrBtProxcAppInstData *proxcData)
{
    CsrMemSet(proxcData,0,sizeof(CsrBtProxcAppInstData));
proxcData->dbElement = (CsrBtGattDbElement*)CsrPmemZalloc(sizeof(CsrBtGattDbElement));

    proxcData->btConnId                          = 0xff;
    proxcData->csrUiVar.hMainMenu                = CSR_UI_DEFAULTACTION;
    proxcData->csrUiVar.hCommonDialog            = CSR_UI_DEFAULTACTION;
    CsrBtProxcAppClearHandles(proxcData);
}

static void csrBtProxcAppSendProxcAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtProxcAppRegisterReq *prim;

    prim = (CsrBtProxcAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtProxcAppRegisterReq));
    prim->type = CSR_BT_PROXC_APP_REGISTER_REQ;
    prim->phandle = pHandle;

    CsrSchedMessagePut(CSR_BT_PROXC_APP_IFACEQUEUE, CSR_BT_PROXC_APP_PRIM,prim);
}

static void csrBtProxcAppSendProxcAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtProxcAppRegisterCfm *prim;

    prim = (CsrBtProxcAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtProxcAppRegisterCfm));
    prim->type = CSR_BT_PROXC_APP_REGISTER_CFM;
    prim->queueId = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_PROXC_APP_PRIM,prim);
}

static char * csrBtProxcAppReturnProxcInstName(CsrBtProxcAppInstData *instance)
{
    size_t bufSize = CsrStrLen("Proximity cli 0000")+1;
    char *buf;
    buf = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "Proximity cli %04X", instance->phandle);
    return buf;
}

/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtProxcAppInit(void **gash)
{
    CsrBtProxcAppInstData    *proxcData;

    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemAlloc(sizeof(CsrBtProxcAppInstData));
    proxcData = (CsrBtProxcAppInstData *) *gash;
    CsrBtProxcAppInitInstanceData(proxcData);

    proxcData->phandle = CsrSchedTaskQueueGet() ;

    proxcData->initialized = TRUE;

    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    csrBtProxcAppSendProxcAppRegisterReq(proxcData->phandle);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtProxcAppDeinit(void **gash)
{
    CsrUint16 msg_type;
    void *msg_data;
    CsrBtProxcAppInstData *proxcData;

    proxcData = (CsrBtProxcAppInstData *) (*gash);
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
    CsrPmemFree(proxcData->dbElement);
    CsrPmemFree(proxcData);
}
#else
void CsrBtProxcAppDeinit(void **gash)
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
void CsrBtProxcAppHandler(void **gash)
{
    CsrBtProxcAppInstData *proxcData;
    CsrUint16    eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    proxcData = (CsrBtProxcAppInstData *) (*gash);

    if(!proxcData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &proxcData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&proxcData->saveQueue, &eventType , &proxcData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
             a message from the scheduler                                                   */
            proxcData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &proxcData->recvMsgP);
        }
        else
        { /* A message has been restored from the local queed                               */
            ;
        }
    }

    switch (eventType)
    {
        case CSR_BT_CM_PRIM:
            {
                CsrBtProxcAppHandleCmPrim(proxcData);
                break;
            }
        case CSR_BT_GATT_PRIM:
            {
                CsrBtProxcAppHandleProxcPrim(proxcData);
                break;
            }

        case CSR_BT_PROXC_APP_PRIM:
            {
                CsrPrim *type = (CsrPrim *) proxcData->recvMsgP;

                switch(*type)
                {
                    case CSR_BT_PROXC_APP_REGISTER_REQ:
                        { /* only received by controling proxc demo app instance */
                            if(proxcData->initialized)
                            {
                                CsrBtProxcAppRegisterReq *prim = proxcData->recvMsgP;
                                CsrSchedQid queueId = CSR_SCHED_QID_INVALID;

                                proxcData->proxcAppHandle = prim->phandle;
                                queueId = proxcData->proxcProfileHandle;
                                if(queueId != CSR_SCHED_QID_INVALID)
                                {
                                    csrBtProxcAppSendProxcAppRegisterCfm(prim->phandle, queueId,1);
                                }
                                else
                                {
                                    CsrGeneralException("CSR_PROXC_DEMO_APP", 
                                                        0, 
                                                        *type,
                                                        "No CsrBtProxc profile Queue Handle Available");
                                }
                            }
                            else
                            {
                                CsrMessageQueuePush(&proxcData->saveQueue, CSR_BT_GATT_APP_PRIM, proxcData->recvMsgP);
                                proxcData->recvMsgP = NULL;
                            }
                            break;
                        }
                    case CSR_BT_PROXC_APP_REGISTER_CFM:
                        { /* Only received by extra proxc demo app instances */

                            proxcData->profileName = csrBtProxcAppReturnProxcInstName(proxcData);

                            CsrBtGattAppHandleProxcSetMainMenuHeader(proxcData);

                            proxcData->initialized = TRUE;

                            CsrAppRegisterReqSend(proxcData->phandle, TECH_BTLE, PROFILE_NAME(proxcData->profileName));
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
                CsrPrim *prim = (CsrPrim *) proxcData->recvMsgP;
                if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
                {
                    CsrBtGapAppGetSelectedDeviceCfm *prim = proxcData->recvMsgP;
                    proxcData->selectedDeviceAddr.addr.lap = prim->deviceAddr.lap;
                    proxcData->selectedDeviceAddr.addr.nap = prim->deviceAddr.nap;
                    proxcData->selectedDeviceAddr.addr.uap = prim->deviceAddr.uap;
                    proxcData->selectedDeviceAddr.type     = ((prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)
                                                              ? TBDADDR_RANDOM
                                                              : TBDADDR_PUBLIC);
                    proxcData->selectedInfo                = prim->deviceStatus;

                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(proxcData->profileName), 
                                         FALSE, 
                                         "Received selected DeviceAddr: %04X:%02X:%06X (%s)", 
                                         prim->deviceAddr.nap, 
                                         prim->deviceAddr.uap, 
                                         prim->deviceAddr.lap,
                                         (CsrBtAddrIsRandom(proxcData->selectedDeviceAddr))? "Random":"Public");
                    /*update UI*/
                    CsrUiUieShowReqSend(proxcData->csrUiVar.hMainMenu,
                                        CsrSchedTaskQueueGet(),
                                        CSR_UI_INPUTMODE_AUTO,
                                        CSR_BT_PROXC_APP_CSR_UI_PRI);
                }
                else
                {
                    /* unexpected primitive received */
                    CsrGeneralException("CSR_BT_PROXC_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtProxcAppHandler,");
                }
                break;
            }
        case CSR_APP_PRIM:
            {
                CsrPrim *prim = (CsrPrim *) proxcData->recvMsgP;

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
                    CsrGeneralException("CSR_BT_PROXC_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_APP_PRIM in CsrBtProxcAppHandler,");
                }
                break;
            }
        case CSR_UI_PRIM:
            {
                CsrBtGattAppHandleProxcCsrUiPrim(proxcData);
                break;
            }

        default:
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_PROXC_APP",
                                    0, 
                                    *((CsrPrim *) proxcData->recvMsgP),
                                    "####### default in CsrBtProxcAppHandler,");
            }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(proxcData->recvMsgP);
}


/******************************************************************************
 * Function to handle all cm-primitives.
 ******************************************************************************/
void CsrBtProxcAppHandleCmPrim(CsrBtProxcAppInstData *proxcData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) proxcData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_CM_READ_RSSI_CFM:
            {
                CsrBtCmReadRssiCfm *cfm = (CsrBtCmReadRssiCfm*)proxcData->recvMsgP;
                proxcData->rssiLevel = cfm->rssi;
                /*update UI*/
                CsrBtGattAppHandleProxcCsrUiMenuUpdate(proxcData);

                break;
            }

    }
}

/******************************************************************************
 * Function to handle all proxc-primitives.
 ******************************************************************************/
void CsrBtProxcAppHandleProxcPrim(CsrBtProxcAppInstData *proxcData)
{
    CsrPrim *primType;
    CsrUint32 discoverServiceReturnValue;
    primType = (CsrPrim *) proxcData->recvMsgP;

    /*Use generic Service Discovery*/
    
    discoverServiceReturnValue = CsrBtGattDiscoverService(proxcData->dbElement, proxcData->recvMsgP, proxcData->gattId, proxcData->btConnId);
    if(discoverServiceReturnValue & CSR_BT_GATT_PRIM_HANDLED)
    {
        if(discoverServiceReturnValue & CSR_BT_GATT_SERVICES_CFM_ERROR)
        {
        }
        else if(discoverServiceReturnValue & CSR_BT_GATT_CHARAC_CFM_ERROR)
        {
        }
        else if(discoverServiceReturnValue & CSR_BT_GATT_CHARAC_DESCRIPTOR_CFM_ERROR)
        {
        }
        else if(discoverServiceReturnValue & CSR_BT_GATT_READ_CFM_ERROR)
        {
            /* Failure */
            CsrBtGattAppHandleProxcCsrUiReadValue(proxcData, NULL, FALSE);
        }
        else
        {
                /*update menu with new value */
            CsrBtGattAppHandleProxcCsrUiMenuUpdate(proxcData);
            return;            /* No error */
        }

    }
    switch (*primType)
    {
        case CSR_BT_GATT_REGISTER_CFM:
            {
                CsrBtGattRegisterCfm *cfm = (CsrBtGattRegisterCfm*)proxcData->recvMsgP;
                if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    proxcData->gattId = cfm->gattId;
                    /*update UI*/
                    CsrBtGattAppHandleProxcCsrUiRegisterUnregister(proxcData, TRUE);
                }
                else
                {
                    /*update UI*/
                    CsrBtGattAppHandleProxcCsrUiRegisterUnregister(proxcData, FALSE);
                }

                break;
            }
        case CSR_BT_GATT_UNREGISTER_CFM:
            {
                    /*update UI*/
                CsrBtGattAppHandleProxcCsrUiRegisterUnregister(proxcData, FALSE);

                break;
            }
        case CSR_BT_GATT_CONNECT_IND:
            {
                CsrBtGattConnectInd        *ind;

                ind = (CsrBtGattConnectInd *) proxcData->recvMsgP;
                proxcData->btConnId = ind->btConnId;

                if (ind->resultSupplier == CSR_BT_SUPPLIER_GATT &&
                    ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(proxcData->profileName), 
                                         TRUE, 
                                         "Successfully Connected with: %04X:%02X:%06X", 
                                         ind->address.addr.nap, 
                                         ind->address.addr.uap, 
                                         ind->address.addr.lap);
                    proxcData->connectedDeviceAddr.addr.nap = ind->address.addr.nap;
                    proxcData->connectedDeviceAddr.addr.lap = ind->address.addr.lap;
                    proxcData->connectedDeviceAddr.addr.uap = ind->address.addr.uap;
                    /*Start RSSI timer */
                    proxcData->rssi_timer = CsrSchedTimerSet(CSR_BT_PROXC_APP_RSSI_TIMER_INTERVAL,
                                                             CsrBtProxcAppRssiTimerInterupt,
                                                             0,
                                                             (void*)proxcData);

                    /* Start treversing the DB to get all relevant info */
                    proxcData->dbElement->traversingDb = TRUE;
                    CsrBtGattDiscoverAllPrimaryServicesReqSend(proxcData->gattId, proxcData->btConnId);

                }
                else
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(proxcData->profileName), 
                                         TRUE, 
                                         "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d", 
                                         ind->address.addr.nap, 
                                         ind->address.addr.uap, 
                                         ind->address.addr.lap, 
                                         ind->resultCode, 
                                         ind->resultSupplier);
                }
                /*update UI*/
                CsrBtGattAppHandleProxcCsrUiConnectDisconnect(proxcData, 
                                                              TRUE,
                                                              (CsrBool)((ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                         ind->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));
                break;
            }
        case CSR_BT_GATT_DISCONNECT_IND:
            {
                CsrBtGattDisconnectInd    *ind;

                ind      = (CsrBtGattDisconnectInd *) proxcData->recvMsgP;

                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(proxcData->profileName), 
                                     TRUE, 
                                     "Disconnected from: %04X:%02X:%06X", 
                                     ind->address.addr.nap, 
                                     ind->address.addr.uap, 
                                     ind->address.addr.lap);
                /*reset instance data */
                /*reset the btConnId */
                proxcData->btConnId = 0xff;
                /*Stop the RSSI TIMER */
                CsrSchedTimerCancel(proxcData->rssi_timer,NULL,NULL);
                /*Clear all handles in instance */
                CsrBtProxcAppClearHandles(proxcData);

                /*update UI*/
                CsrBtGattAppHandleProxcCsrUiConnectDisconnect(proxcData, 
                                                              FALSE,
                                                              TRUE);


                break;
            }
        case CSR_BT_GATT_CENTRAL_CFM:
            {
                CsrBtGattCentralCfm *cfm;
                cfm = (CsrBtGattCentralCfm*) proxcData->recvMsgP;

                if(cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
                {
                    /*We only care about non-successful as success will be handled on CONNECT_IND*/
                    CsrBtGattAppHandleProxcCsrUiConnectDisconnect(proxcData, 
                                                                  TRUE,
                                                                  (CsrBool)((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                             cfm->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));

                }
                break;
            }
        case CSR_BT_GATT_BREDR_CONNECT_CFM:
            {
                CsrBtGattBredrConnectCfm *cfm;
                cfm = (CsrBtGattBredrConnectCfm*) proxcData->recvMsgP;

                if(cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
                {
                    /*We only care about non-successful as success will be handled on CONNECT_IND*/
                    CsrBtGattAppHandleProxcCsrUiConnectDisconnect(proxcData, 
                                                                  TRUE,
                                                                  (CsrBool)((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                             cfm->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));

                }
                break;
            }
        case CSR_BT_GATT_WRITE_CFM:
            {
                CsrBtGattWriteCfm *cfm;
                cfm = (CsrBtGattWriteCfm*)proxcData->recvMsgP;

                if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    /*update UI*/
                    CsrBtGattAppHandleProxcCsrUiWriteValue(proxcData,  TRUE);
                    /*update sub-title in menu*/
                    CsrBtGattAppHandleProxcCsrUiMenuUpdate(proxcData);
                }
                else
                {
                    CsrAppBacklogReqSend(TECH_BTLE,
                                         PROFILE_NAME(proxcData->profileName),
                                         TRUE,
                                         "failed to write value %d - error code: 0x%04X Supplier: 0x%04X",
                                         proxcData->writeValue[0],
                                         cfm->resultCode, cfm->resultSupplier);
                    /*update UI with error message*/
                    CsrBtGattAppHandleProxcCsrUiWriteValue(proxcData, FALSE);
                }
                break;
            }
        default:
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(proxcData->profileName), 
                                     TRUE, 
                                     "####### default in gatt prim handler 0x%04x,",
                                     *primType);
                CsrBtGattFreeUpstreamMessageContents(CSR_BT_GATT_PRIM, proxcData->recvMsgP);
                /* unexpected primitive received */
            }
    }
}

