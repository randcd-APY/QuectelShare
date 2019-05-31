/****************************************************************************

Copyright (c) 2011-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

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
#include "csr_bt_gatt_demo_therm_client_app_private_prim.h"
#include "csr_bt_gatt_demo_therm_client_app_ui_sef.h"
#include "csr_bt_gatt_app_task.h"
#include "csr_bt_gatt_demo_db_utils.h"

/* Forward declarations for handler functions */
void CsrBtThermcAppHandleThermcPrim(CsrBtThermcAppInstData *thermcData);



CsrBtUuid *CsrBtThermcAppUuid16ToUuid128(CsrUint16 uuid)
{
    CsrBtUuid *realUuid = CsrPmemAlloc(sizeof(CsrBtUuid));
    CSR_COPY_UINT16_TO_LITTLE_ENDIAN(uuid, realUuid->uuid);
    
    realUuid->length = CSR_BT_UUID16_SIZE;

    return realUuid;
} 


/******************************************************************************
CsrBtThermcAppStartRegister:
 *****************************************************************************/
void CsrBtThermcAppStartRegister(CsrBtThermcAppInstData *thermcData)
{
    CsrBtGattRegisterReqSend(thermcData->phandle,
                             1234);
}
/******************************************************************************
CsrBtThermcAppStartUnregister:
 *****************************************************************************/
void CsrBtThermcAppStartUnregister(CsrBtThermcAppInstData *thermcData)
{
    CsrBtGattUnregisterReqSend(thermcData->gattId);
}
/******************************************************************************
CsrBtThermcAppStartConnecting:
 *****************************************************************************/
void CsrBtThermcAppStartConnecting(CsrBtThermcAppInstData *thermcData)
{
    if(thermcData->selectedDeviceAddr.addr.uap != 0)
    {
        CsrBtGattCentralReqSend(thermcData->gattId,
                                (thermcData->selectedDeviceAddr),
                                CSR_BT_GATT_FLAGS_NONE,
                                0);
    }
    else
    {
        /*no device addr have been selected in GAP yet*/        
        CsrBtGattAppHandleThermcCsrUiNoSelectedDevAddr(thermcData);
    }
}
/******************************************************************************
CsrBtThermcAppStartReadUuid:
 *****************************************************************************/
void CsrBtThermcAppStartReadUuid(CsrBtThermcAppInstData *thermcData, CsrUint16 uuid)
{
    CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend(thermcData->gattId, thermcData->btConnId, uuid);

}
void CsrBtThermcAppStartReadValue(CsrBtThermcAppInstData *thermcData, CsrUint16 pUuid, CsrUint16 cUuid)
{
    CsrUint16                         valueHandle;
    CsrBtGattAppDbPrimServiceElement  *pElem;
    CsrBtGattAppDbCharacElement       *cElem;
 
    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(thermcData->dbElement->dbPrimServices,
                                                         CsrBtThermcAppUuid16ToUuid128(pUuid));
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtThermcAppUuid16ToUuid128(cUuid));
    valueHandle = cElem->valueHandle;

    CsrBtGattReadReqSend(thermcData->gattId,
                         thermcData->btConnId,
                         valueHandle,
                         0);

}

void CsrBtThermcAppStartWriteClientConfig(CsrBtThermcAppInstData *thermcData, CsrUint16 pUuid, CsrUint16 cUuid, CsrUint16 value)
{

    CsrBtGattAppDbPrimServiceElement  *pElem;
    CsrBtGattAppDbCharacElement       *cElem;
    CsrBtGattAppDbCharacDescrElement  *dElem;

    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(thermcData->dbElement->dbPrimServices,
                                                         CsrBtThermcAppUuid16ToUuid128(pUuid));
    if(pElem == NULL)
    {
        return; /* the prim Service was not found */
    }
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtThermcAppUuid16ToUuid128(cUuid));
    if(cElem == NULL)
    {
        return; /* the charac was not found */
    }
    
    dElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(cElem->descrList, 
                                                         CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION));
    if(dElem == NULL)
    {
        return; /* the charac descriptor was not found */
    }
    CsrBtGattWriteClientConfigurationReqSend(thermcData->gattId,
                                             thermcData->btConnId,
                                             dElem->descriptorHandle,
                                             cElem->valueHandle,value);

      }
void CsrBtThermcAppStartWriteValue(CsrBtThermcAppInstData *thermcData, CsrUint16 pUuid, CsrUint16 cUuid)
{
    CsrUint16                         valueHandle;
    CsrBtGattAppDbPrimServiceElement  *pElem;
    CsrBtGattAppDbCharacElement       *cElem;

    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(thermcData->dbElement->dbPrimServices,
                                                         CsrBtThermcAppUuid16ToUuid128(pUuid));
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtThermcAppUuid16ToUuid128(cUuid));
    valueHandle = cElem->valueHandle;
    
    
    thermcData->writeValueLength = 1;
    thermcData->writeValue = CsrPmemAlloc(1);
    switch(pUuid)
    {
        case CSR_BT_GATT_UUID_LINK_LOSS_SERVICE:
            {
                if(cElem->value[0]>=CSR_BT_GATT_APP_ALERT_LEVEL_HIGH)
                {
                    thermcData->writeValue[0]  = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                    cElem->value[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;

                }
                else
                {
                    cElem->value[0]           += 1;
                    thermcData->writeValue[0]  = (CsrUint8)cElem->value[0];
                }
                break;
            }
        case CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE:
            {
                if(CSR_BT_GATT_APP_ALERT_LEVEL_OFF == cElem->value[0])
                {
                    cElem->value[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_HIGH;
                    thermcData->writeValue[0]  = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_HIGH;
                }
                else
                {
                    cElem->value[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                    thermcData->writeValue[0]  = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                }

                break;
            }
    }
    CsrBtGattWriteReqSend(thermcData->gattId,
                          thermcData->btConnId,
                          valueHandle,
                          0,
                          thermcData->writeValueLength, 
                          thermcData->writeValue);

}



/******************************************************************************
CsrBtThermcAppStartDisconnect:
 *****************************************************************************/
void CsrBtThermcAppStartDisconnect(CsrBtThermcAppInstData *thermcData)
{

    CsrBtGattDisconnectReqSend(thermcData->gattId, thermcData->btConnId);
    /*reset BT connection ID */
    thermcData->btConnId = 0xff;
}

/******************************************************************************
CsrBtThermcAppInitInstanceData:
 *****************************************************************************/
void CsrBtThermcAppClearHandles(CsrBtThermcAppInstData *thermcData){

    /*init list for local database structure copy */ 
    
    CsrCmnListInit(&thermcData->dbElement->dbPrimServices, 0, CsrBtGattAppInitDbPrimList, CsrBtGattAppFreeDbPrimList);
    thermcData->dbElement->dbPrimServicesCount           = 0;
    thermcData->dbElement->currentCharac                 = NULL;
    thermcData->dbElement->currentPrimService            = NULL;
    thermcData->dbElement->currentDescr                  = NULL;
}

void CsrBtThermcAppInitInstanceData(CsrBtThermcAppInstData *thermcData)
{
    CsrMemSet(thermcData,0,sizeof(CsrBtThermcAppInstData));
    thermcData->dbElement = (CsrBtGattDbElement*)CsrPmemAlloc(sizeof(CsrBtGattDbElement));

    thermcData->btConnId                          = 0xff;
    thermcData->csrUiVar.hMainMenu                = CSR_UI_DEFAULTACTION;
    thermcData->csrUiVar.hCommonDialog            = CSR_UI_DEFAULTACTION;
    thermcData->tempNotif                         = FALSE;
    thermcData->dbElement->traversingDb           = FALSE;
    CsrBtThermcAppClearHandles(thermcData);
}

static void csrBtThermcAppSendThermcAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtThermcAppRegisterReq *prim;

    prim           = (CsrBtThermcAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtThermcAppRegisterReq));
    prim->type     = CSR_BT_THERMC_APP_REGISTER_REQ;
    prim->phandle  = pHandle;

    CsrSchedMessagePut(CSR_BT_THERMC_APP_IFACEQUEUE, CSR_BT_THERMC_APP_PRIM,prim);
}

static void csrBtThermcAppSendThermcAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtThermcAppRegisterCfm *prim;

    prim           = (CsrBtThermcAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtThermcAppRegisterCfm));
    prim->type     = CSR_BT_THERMC_APP_REGISTER_CFM;
    prim->queueId  = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_THERMC_APP_PRIM,prim);
}

static char * csrBtThermcAppReturnThermcInstName(CsrBtThermcAppInstData *instance)
{
    char *buf;
    CsrUint32 bufSize = CsrStrLen("Thermometer cli 0000")+1;
    buf  = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "Thermometer cli %04X", instance->phandle);
    return buf;
}

/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtThermcAppInit(void **gash)
{
    CsrBtThermcAppInstData    *thermcData;

    /* allocate and initialise instance data space */
    *gash      = (void *) CsrPmemAlloc(sizeof(CsrBtThermcAppInstData));
    thermcData = (CsrBtThermcAppInstData *) *gash;

    CsrBtThermcAppInitInstanceData(thermcData);

    thermcData->phandle     = CsrSchedTaskQueueGet();
    thermcData->initialized = TRUE;
    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    csrBtThermcAppSendThermcAppRegisterReq(thermcData->phandle);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtThermcAppDeinit(void **gash)
{
    CsrUint16               msg_type;
    void                    *msg_data;
    CsrBtThermcAppInstData  *thermcData;

    thermcData = (CsrBtThermcAppInstData *) (*gash);
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
    CsrPmemFree(thermcData->dbElement);

    CsrPmemFree(thermcData);
}
#else
void CsrBtThermcAppDeinit(void **gash)
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
void CsrBtThermcAppHandler(void **gash)
{
           CsrBtThermcAppInstData  *thermcData;
           CsrUint16               eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    thermcData = (CsrBtThermcAppInstData *) (*gash);

    if(!thermcData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &thermcData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&thermcData->saveQueue, &eventType , &thermcData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
             a message from the scheduler                                                   */
            thermcData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &thermcData->recvMsgP);
        }
        else
        { /* A message has been restored from the local queed                               */
            ;
        }
    }

    switch (eventType)
    {
            
        case CSR_BT_GATT_PRIM:
            {
                CsrBtThermcAppHandleThermcPrim(thermcData);
                break;
            }

        case CSR_BT_THERMC_APP_PRIM:
            {
                CsrPrim *type = (CsrPrim *) thermcData->recvMsgP;

                switch(*type)
                {
                    case CSR_BT_THERMC_APP_REGISTER_REQ:
                        { /* only received by controling thermc demo app instance */
                            if(thermcData->initialized)
                            {
                                CsrBtThermcAppRegisterReq *prim  = thermcData->recvMsgP;
                                CsrSchedQid queueId              = CSR_SCHED_QID_INVALID;
                                thermcData->thermcAppHandle      = prim->phandle;
                                queueId                          = thermcData->thermcProfileHandle;
   
                                if(queueId != CSR_SCHED_QID_INVALID)
                                {
                                    csrBtThermcAppSendThermcAppRegisterCfm(prim->phandle, queueId,1);
                                }
                                else
                                {
                                    CsrGeneralException("CSR_THERMC_DEMO_APP", 
                                                        0, 
                                                        *type,
                                                        "No CsrBtThermc profile Queue Handle Available");
                                }
                            }
                            else
                            {
                                CsrMessageQueuePush(&thermcData->saveQueue, CSR_BT_GATT_APP_PRIM, thermcData->recvMsgP);
                                thermcData->recvMsgP = NULL;
                            }
                            break;
                        }
                    case CSR_BT_THERMC_APP_REGISTER_CFM:
                        { /* Only received by extra thermc demo app instances */

                            thermcData->profileName = csrBtThermcAppReturnThermcInstName(thermcData);

                            CsrBtGattAppHandleThermcSetMainMenuHeader(thermcData);

                            thermcData->initialized = TRUE;

                            CsrAppRegisterReqSend(thermcData->phandle, TECH_BTLE, PROFILE_NAME(thermcData->profileName));
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
                CsrPrim *prim = (CsrPrim *) thermcData->recvMsgP;
                if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
                {
                    CsrBtGapAppGetSelectedDeviceCfm *prim    = thermcData->recvMsgP;
                    thermcData->selectedDeviceAddr.addr.lap  = prim->deviceAddr.lap;
                    thermcData->selectedDeviceAddr.addr.nap  = prim->deviceAddr.nap;
                    thermcData->selectedDeviceAddr.addr.uap  = prim->deviceAddr.uap;
                    thermcData->selectedDeviceAddr.type     = (prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? TBDADDR_RANDOM:TBDADDR_PUBLIC;
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(thermcData->profileName), 
                                         FALSE, 
                                         "Received selected DeviceAddr: %04X:%02X:%06X (%s)", 
                                         prim->deviceAddr.nap, 
                                         prim->deviceAddr.uap, 
                                         prim->deviceAddr.lap,
                                         (CsrBtAddrIsRandom(thermcData->selectedDeviceAddr))? "Random":"Public");
                    /*update UI*/
                    CsrUiUieShowReqSend(thermcData->csrUiVar.hMainMenu,
                                        CsrSchedTaskQueueGet(),
                                        CSR_UI_INPUTMODE_AUTO,
                                        CSR_BT_THERMC_APP_CSR_UI_PRI);
                }
                else
                {
                    /* unexpected primitive received */
                    CsrGeneralException("CSR_BT_THERMC_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtThermcAppHandler,");
                }
                break;
            }
        case CSR_APP_PRIM:
            {
                CsrPrim *prim = (CsrPrim *) thermcData->recvMsgP;

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
                    CsrGeneralException("CSR_BT_THERMC_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_APP_PRIM in CsrBtThermcAppHandler,");
                }
                break;
            }
        case CSR_UI_PRIM:
            {
                CsrBtGattAppHandleThermcCsrUiPrim(thermcData);
                break;
            }

        default:
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_THERMC_APP",
                                    0, 
                                    *((CsrPrim *) thermcData->recvMsgP),
                                    "####### default in CsrBtThermcAppHandler,");
            }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(thermcData->recvMsgP);
}


/******************************************************************************
 * Function to handle all thermc-primitives.
 ******************************************************************************/
void CsrBtThermcAppHandleThermcPrim(CsrBtThermcAppInstData *thermcData)
{
    CsrPrim *primType;
    CsrUint32 discoverServiceReturnValue;
    primType = (CsrPrim *) thermcData->recvMsgP;
    /*Use generic Service Discovery*/
    discoverServiceReturnValue = CsrBtGattDiscoverService(thermcData->dbElement, thermcData->recvMsgP, thermcData->gattId, thermcData->btConnId);
    if(discoverServiceReturnValue & CSR_BT_GATT_PRIM_HANDLED)
    {
        if(discoverServiceReturnValue & CSR_BT_GATT_SERVICES_CFM_ERROR)
        {
            /*Update UI with error */
/*FIXME                   CsrBtGattAppHandleThermcCsrUiDiscoverServices(thermcData, 0x0000, FALSE);*/
        }
        else if(discoverServiceReturnValue & CSR_BT_GATT_CHARAC_CFM_ERROR)
        {
/*FIXME              Error handling*/
            /* CSR_BT_GATT_DISCOVER_CHARAC_CFM */
        }
        else if(discoverServiceReturnValue & CSR_BT_GATT_CHARAC_DESCRIPTOR_CFM_ERROR)
        {
            /*FIXME: Error handling*/
        }
        else if(discoverServiceReturnValue & CSR_BT_GATT_READ_CFM_ERROR)
        {
               /*update UI*/
            CsrBtGattAppHandleThermcCsrUiReadValue(thermcData, NULL, FALSE);
        }
        else
        {
            CsrBtGattAppHandleThermcCsrUiMenuUpdate(thermcData);
            return;            /* No error */
        }

    }
    
    switch (*primType)
    {
        case CSR_BT_GATT_REGISTER_CFM:
            {
                CsrBtGattRegisterCfm *cfm = (CsrBtGattRegisterCfm*)thermcData->recvMsgP;
                if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    thermcData->gattId = cfm->gattId;
                    /*update UI*/
                    CsrBtGattAppHandleThermcCsrUiRegisterUnregister(thermcData, TRUE);
                }
                else
                {
                    /*update UI*/
                    CsrBtGattAppHandleThermcCsrUiRegisterUnregister(thermcData, FALSE);
                }

                break;
            }
        case CSR_BT_GATT_UNREGISTER_CFM:
            {
                    /*update UI*/
                CsrBtGattAppHandleThermcCsrUiRegisterUnregister(thermcData, FALSE);

                break;
            }
        case CSR_BT_GATT_CONNECT_IND:
            {
                CsrBtGattConnectInd  *ind;

                ind                   = (CsrBtGattConnectInd *) thermcData->recvMsgP;
                thermcData->btConnId  = ind->btConnId;

                if (ind->resultSupplier == CSR_BT_SUPPLIER_GATT &&
                    ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(thermcData->profileName), 
                                         TRUE, 
                                         "Successfully Connected with: %04X:%02X:%06X", 
                                         ind->address.addr.nap, 
                                         ind->address.addr.uap, 
                                         ind->address.addr.lap);
                    thermcData->connectedDeviceAddr.addr.nap = ind->address.addr.nap;
                    thermcData->connectedDeviceAddr.addr.lap = ind->address.addr.lap;
                    thermcData->connectedDeviceAddr.addr.uap = ind->address.addr.uap;

                    /* Start treversing the DB to get all relevant info */
                    thermcData->dbElement->traversingDb = TRUE;
                    CsrBtGattDiscoverAllPrimaryServicesReqSend(thermcData->gattId, thermcData->btConnId);

                }
                else
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(thermcData->profileName), 
                                         TRUE, 
                                         "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d", 
                                         ind->address.addr.nap, 
                                         ind->address.addr.uap, 
                                         ind->address.addr.lap, 
                                         ind->resultCode, 
                                         ind->resultSupplier);
                }
                /*update UI*/
                CsrBtGattAppHandleThermcCsrUiConnectDisconnect(thermcData, 
                                                              TRUE,
                                                              (CsrBool)((ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                         ind->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));
                break;
            }
        case CSR_BT_GATT_DISCONNECT_IND:
            {
                CsrBtGattDisconnectInd  *ind;

                ind  = (CsrBtGattDisconnectInd *) thermcData->recvMsgP;

                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(thermcData->profileName), 
                                     TRUE, 
                                     "Disconnected from: %04X:%02X:%06X", 
                                     ind->address.addr.nap, 
                                     ind->address.addr.uap, 
                                     ind->address.addr.lap);
                /*reset instance data */
                /*reset the btConnId */
                thermcData->btConnId = 0xff;

                /*Clear all handles in instance */
                CsrBtThermcAppClearHandles(thermcData);

                /*update UI*/
                CsrBtGattAppHandleThermcCsrUiConnectDisconnect(thermcData, 
                                                              FALSE,
                                                              TRUE);


                break;
            }
        case CSR_BT_GATT_CENTRAL_CFM:
            {
                CsrBtGattCentralCfm *cfm;
                cfm = (CsrBtGattCentralCfm*) thermcData->recvMsgP;

                if(cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
                {
                    /*We only care about non-successful as success will be handled on CONNECT_IND*/
                    CsrBtGattAppHandleThermcCsrUiConnectDisconnect(thermcData, 
                                                                  TRUE,
                                                                  (CsrBool)((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                             cfm->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));

                }
                break;
            }
        case CSR_BT_GATT_WRITE_CFM :
            {
                CsrBtGattWriteCfm *cfm;
                cfm = (CsrBtGattWriteCfm*)thermcData->recvMsgP;

                if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    if(thermcData->tempNotif)
                    {
                        thermcData->tempNotif = FALSE;
                    }
                    else 
                    {
                        thermcData->tempNotif = TRUE;
                    }

                    
                    /*update UI*/
                    CsrBtGattAppHandleThermcCsrUiWriteValue(thermcData,  TRUE);
                    /*update sub-title in menu*/
                    CsrBtGattAppHandleThermcCsrUiMenuUpdate(thermcData);

                }
                else
                {
                    /*update UI with error message*/
                    CsrBtGattAppHandleThermcCsrUiWriteValue(thermcData, FALSE);
                }
                break;
            }
        case CSR_BT_GATT_NOTIFICATION_IND :  
            {
                /* find temprature characteristic */
                CsrBtGattNotificationInd *ind            =  (CsrBtGattNotificationInd*)thermcData->recvMsgP;
                CsrBtGattAppDbPrimServiceElement *pElem  =  CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(thermcData->dbElement->dbPrimServices, CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE));
                CsrBtGattAppDbCharacElement *cElem       =  CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList,
                                                                                            CsrBtThermcAppUuid16ToUuid128(CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC));
                /*if we found it, then update value*/
                if(cElem)
                {
                    if(cElem->value)
                    {
                        CsrPmemFree(cElem->value);
                        cElem->value = NULL;
                    }
                    cElem->value        =  ind->value;
                    cElem->valueLength  =  ind->valueLength;
                }
                CsrBtGattAppHandleThermcCsrUiMenuUpdate(thermcData);
                break;
            }
        default:
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(thermcData->profileName), 
                                     TRUE, 
                                     "####### default in gatt prim handler 0x%04x,",
                                     *primType);
                CsrBtGattFreeUpstreamMessageContents(CSR_BT_GATT_PRIM, thermcData->recvMsgP);
                /* unexpected primitive received */
            }
    }
}

