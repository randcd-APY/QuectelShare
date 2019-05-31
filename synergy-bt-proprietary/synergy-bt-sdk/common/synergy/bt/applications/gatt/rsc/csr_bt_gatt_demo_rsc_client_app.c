/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include <string.h>
#include "csr_synergy.h"
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
#include "csr_bt_gatt_demo_rsc_client_app_private_prim.h"
#include "csr_bt_gatt_demo_rsc_client_app_ui_sef.h"
#include "csr_bt_gatt_app_task.h"
#include "csr_bt_gatt_demo_db_utils.h"



/* Forward declarations for handler functions */
void CsrBtRsccAppHandleRsccPrim(CsrBtRsccAppInstData *rsccData);

void CsrBtRsccAppHandleCmPrim(CsrBtRsccAppInstData *rsccData);



CsrBtUuid *CsrBtRsccAppUuid16ToUuid128(CsrUint16 uuid)
{
    CsrBtUuid *realUuid = CsrPmemAlloc(sizeof(CsrBtUuid));
    CSR_COPY_UINT16_TO_LITTLE_ENDIAN(uuid, realUuid->uuid);
    
    realUuid->length = CSR_BT_UUID16_SIZE;

    return realUuid;
} 


/******************************************************************************
CsrBtRsccAppStartRegister:
 *****************************************************************************/
void CsrBtRsccAppStartRegister(CsrBtRsccAppInstData *rsccData)
{
    CsrBtGattRegisterReqSend(rsccData->phandle, 5678);
}
/******************************************************************************
CsrBtRsccAppStartUnregister:
 *****************************************************************************/
void CsrBtRsccAppStartUnregister(CsrBtRsccAppInstData *rsccData)
{
    CsrBtGattUnregisterReqSend(rsccData->gattId);
}
/******************************************************************************
CsrBtRsccAppStartConnecting:
 *****************************************************************************/
void CsrBtRsccAppStartConnecting(CsrBtRsccAppInstData *rsccData)
{
    if(rsccData->selectedDeviceAddr.addr.uap != 0)
    {
        if( (rsccData->selectedInfo & CSR_BT_SD_STATUS_RADIO_BREDR) ||
            (rsccData->transport == CSR_BT_RSC_TRANSPORT_BREDR) )
        {
            rsccData->transport = CSR_BT_RSC_TRANSPORT_BREDR;
            CsrBtGattBredrConnectReqSend(rsccData->gattId,
                                         rsccData->selectedDeviceAddr,
                                         CSR_BT_GATT_FLAGS_NONE);
        }
        else
        {
            rsccData->transport = CSR_BT_RSC_TRANSPORT_LE;
            CsrBtGattCentralReqSend(rsccData->gattId,
                                    rsccData->selectedDeviceAddr,
                                    CSR_BT_GATT_FLAGS_NONE,
                                    0);
        }
        
    }
    else
    {
        /*no device addr have been selected in GAP yet*/        
        CsrBtGattAppHandleRsccCsrUiNoSelectedDevAddr(rsccData);
    }
}
/******************************************************************************
CsrBtRsccAppStartReadUuid:
 *****************************************************************************/
void CsrBtRsccAppStartReadUuid(CsrBtRsccAppInstData *rsccData, CsrUint16 uuid)
{
    CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend(rsccData->gattId, rsccData->btConnId, uuid);

}
void CsrBtRsccAppStartReadValue(CsrBtRsccAppInstData *rsccData, CsrUint16 pUuid, CsrUint16 cUuid)
{
    CsrUint16                         valueHandle;
    CsrBtGattAppDbPrimServiceElement  *pElem;
    CsrBtGattAppDbCharacElement       *cElem;
 
    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(rsccData->dbPrimServices,
                                                         CsrBtRsccAppUuid16ToUuid128(pUuid));
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtRsccAppUuid16ToUuid128(cUuid));
    valueHandle = cElem->valueHandle;

    CsrBtGattReadReqSend(rsccData->gattId,
                         rsccData->btConnId,
                         valueHandle,
                         0);

}

void CsrBtRsccAppStartWriteClientConfig(CsrBtRsccAppInstData *rsccData, CsrUint16 pUuid, CsrUint16 cUuid, CsrUint16 value)
{

    CsrBtGattAppDbPrimServiceElement  *pElem;
    CsrBtGattAppDbCharacElement       *cElem;
    CsrBtGattAppDbCharacDescrElement  *dElem;

    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(rsccData->dbPrimServices,
                                                         CsrBtRsccAppUuid16ToUuid128(pUuid));
    if(pElem == NULL)
    {
        return; /* the prim Service was not found */
    }
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtRsccAppUuid16ToUuid128(cUuid));
    if(cElem == NULL)
    {
        return; /* the charac was not found */
    }
    
    dElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(cElem->descrList, 
                                                         CsrBtRsccAppUuid16ToUuid128(CSR_BT_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION));
    if(dElem == NULL)
    {
        return; /* the charac descriptor was not found */
    }
    CsrBtGattWriteClientConfigurationReqSend(rsccData->gattId,
                                             rsccData->btConnId,
                                             dElem->descriptorHandle,
                                             cElem->valueHandle,
                                             value);

}

void CsrBtRsccAppStartWriteValue(CsrBtRsccAppInstData *rsccData, CsrUint8 type, CsrUint16 valueLength, CsrUint8* value)
{
    switch(type)
    {  /* Add timeout notifications of ATT timeout (30 sec) if no indicattion is recieved */
        case CSR_BT_RSCC_WRITE_OPCODE_RFU: /* used to add own value */
        {
            rsccData->writeValueLength = valueLength;
            rsccData->writeValue = value;
            break;
        }
        case CSR_BT_RSCC_WRITE_OPCODE_STD:
        {   
            rsccData->writeValueLength = CSR_BT_RSCC_WRITE_OPCODE_STD_LENGTH;     
            rsccData->writeValue = CsrPmemAlloc(CSR_BT_RSCC_WRITE_OPCODE_STD_LENGTH);
            rsccData->writeValue[0] = CSR_BT_RSCC_WRITE_OPCODE_STD;
            rsccData->writeValue[1] = 0x00;
            rsccData->writeValue[2] = 0x00;
            rsccData->writeValue[3] = 0x00;
            rsccData->writeValue[4] = 0x00;
            break;
        }
        case CSR_BT_RSCC_WRITE_OPCODE_SSC:
        {
            rsccData->writeValueLength = CSR_BT_RSCC_WRITE_OPCODE_SSC_LENGTH;     
            rsccData->writeValue = CsrPmemAlloc(CSR_BT_RSCC_WRITE_OPCODE_SSC_LENGTH);
            rsccData->writeValue[0] = CSR_BT_RSCC_WRITE_OPCODE_SSC;         
            break;
        }
        case CSR_BT_RSCC_WRITE_OPCODE_USL:
        {
            rsccData->writeValueLength = CSR_BT_RSCC_WRITE_OPCODE_USL_LENGTH;     
            rsccData->writeValue = CsrPmemAlloc(CSR_BT_RSCC_WRITE_OPCODE_USL_LENGTH);
            rsccData->writeValue[0] = CSR_BT_RSCC_WRITE_OPCODE_USL;
            rsccData->writeValue[1] = 0x02;
            break;
        }
        case CSR_BT_RSCC_WRITE_OPCODE_RSSL:
        {
            rsccData->writeValueLength = CSR_BT_RSCC_WRITE_OPCODE_RSSL_LENGTH;     
            rsccData->writeValue = CsrPmemAlloc(CSR_BT_RSCC_WRITE_OPCODE_RSSL_LENGTH);
            rsccData->writeValue[0] = CSR_BT_RSCC_WRITE_OPCODE_RSSL;
            
            break;
        }
       
    }
    rsccData->rscScWriteReq = TRUE;
    CsrBtGattWriteReqSend(rsccData->gattId,
                          rsccData->btConnId,
                          rsccData->scControlPointHandle,
                          0,
                          rsccData->writeValueLength,
                          rsccData->writeValue);

}



/******************************************************************************
CsrBtRsccAppStartDisconnect:
 *****************************************************************************/
void CsrBtRsccAppStartDisconnect(CsrBtRsccAppInstData *rsccData)
{

    CsrBtGattDisconnectReqSend(rsccData->gattId, rsccData->btConnId);
    /*reset BT connection ID */
    rsccData->btConnId = 0xff;
}

/******************************************************************************
CsrBtRsccAppInitInstanceData:
 *****************************************************************************/
void CsrBtRsccAppClearHandles(CsrBtRsccAppInstData *rsccData){

    /*init list for local database structure copy */                              
    CsrCmnListInit(&rsccData->dbPrimServices, 0, CsrBtGattAppInitDbPrimList, CsrBtGattAppFreeDbPrimList);
    rsccData->dbPrimServicesCount           = 0;
    rsccData->currentCharac                 = NULL;
    rsccData->currentPrimService            = NULL;

}
void CsrBtRsccAppInitInstanceData(CsrBtRsccAppInstData *rsccData)
{
    CsrMemSet(rsccData,0,sizeof(CsrBtRsccAppInstData));

    rsccData->btConnId                          = 0xff;
    rsccData->csrUiVar.hMainMenu                = CSR_UI_DEFAULTACTION;
    rsccData->csrUiVar.hCommonDialog            = CSR_UI_DEFAULTACTION;
    rsccData->rscNotif                          = FALSE;
    rsccData->rscInd                            = FALSE;
    rsccData->rscWriteCC                        = FALSE;
    rsccData->rscScWriteReq                     = FALSE;
    rsccData->traversingDb                      = FALSE;
    rsccData->transport                         = CSR_BT_RSC_TRANSPORT_LE;
    CsrBtRsccAppClearHandles(rsccData);
}

static void csrBtRsccAppSendRsccAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtRsccAppRegisterReq *prim;

    prim           = (CsrBtRsccAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtRsccAppRegisterReq));
    prim->type     = CSR_BT_RSCC_APP_REGISTER_REQ;
    prim->phandle  = pHandle;

    CsrSchedMessagePut(CSR_BT_RSCC_APP_IFACEQUEUE, CSR_BT_RSCC_APP_PRIM, prim);
}

static void csrBtRsccAppSendRsccAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtRsccAppRegisterCfm *prim;

    prim           = (CsrBtRsccAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtRsccAppRegisterCfm));
    prim->type     = CSR_BT_RSCC_APP_REGISTER_CFM;
    prim->queueId  = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_RSCC_APP_PRIM,prim);
}

static char * csrBtRsccAppReturnRsccInstName(CsrBtRsccAppInstData *instance)
{
    char *buf;
    CsrUint32 bufSize = CsrStrLen("RSC Client 0000")+1;
    buf  = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "RSC Client %04X", instance->phandle);
    return buf;
}

/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtRsccAppInit(void **gash)
{
    CsrBtRsccAppInstData    *rsccData;

    /* allocate and initialise instance data space */
    *gash      = (void *) CsrPmemAlloc(sizeof(CsrBtRsccAppInstData));
    rsccData = (CsrBtRsccAppInstData *) *gash;

    CsrBtRsccAppInitInstanceData(rsccData);

    rsccData->phandle     = CsrSchedTaskQueueGet();
    rsccData->initialized = TRUE;
    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    csrBtRsccAppSendRsccAppRegisterReq(rsccData->phandle);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtRsccAppDeinit(void **gash)
{
    CsrUint16               msg_type;
    void                    *msg_data;
    CsrBtRsccAppInstData  *rsccData;

    rsccData = (CsrBtRsccAppInstData *) (*gash);
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

    CsrPmemFree(rsccData);
}
#else
void CsrBtRsccAppDeinit(void **gash)
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
void CsrBtRsccAppHandler(void **gash)
{
           CsrBtRsccAppInstData  *rsccData;
           CsrUint16               eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    rsccData = (CsrBtRsccAppInstData *) (*gash);

    if(!rsccData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &rsccData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&rsccData->saveQueue, &eventType , &rsccData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
             a message from the scheduler                                                   */
            rsccData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &rsccData->recvMsgP);
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
            CsrBtRsccAppHandleCmPrim(rsccData);
            break;
        }
        case CSR_BT_GATT_PRIM:
        {
            CsrBtRsccAppHandleRsccPrim(rsccData);
            break;
        }

        case CSR_BT_RSCC_APP_PRIM:
        {
            CsrPrim *type = (CsrPrim *) rsccData->recvMsgP;

            switch(*type)
            {
                case CSR_BT_RSCC_APP_REGISTER_REQ:
                { /* only received by controling rscc demo app instance */
                    if(rsccData->initialized)
                    {
                        CsrBtRsccAppRegisterReq *prim   = rsccData->recvMsgP;
                        CsrSchedQid queueId             = CSR_SCHED_QID_INVALID;
                        rsccData->rsccAppHandle         = prim->phandle;
                        queueId                         = rsccData->rsccProfileHandle;

                        if(queueId != CSR_SCHED_QID_INVALID)
                        {
                            csrBtRsccAppSendRsccAppRegisterCfm(prim->phandle, queueId,1);
                        }
                        else
                        {
                            CsrGeneralException("CSR_RSCC_DEMO_APP",
                                                0, 
                                                *type,
                                                "No CsrBtRscc profile Queue Handle Available");
                        }
                    }
                    else
                    {
                        CsrMessageQueuePush(&rsccData->saveQueue, CSR_BT_GATT_APP_PRIM, rsccData->recvMsgP);
                        rsccData->recvMsgP = NULL;
                    }
                    break;
                }
                case CSR_BT_RSCC_APP_REGISTER_CFM:
                { /* Only received by extra rscc demo app instances */

                    rsccData->profileName = csrBtRsccAppReturnRsccInstName(rsccData);

                    CsrBtGattAppHandleRsccSetMainMenuHeader(rsccData);

                    rsccData->initialized = TRUE;

                    CsrAppRegisterReqSend(rsccData->phandle, TECH_BTLE, PROFILE_NAME(rsccData->profileName));
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
            CsrPrim *prim = (CsrPrim *) rsccData->recvMsgP;
            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *prim   = rsccData->recvMsgP;

                rsccData->selectedDeviceAddr.addr.lap   = prim->deviceAddr.lap;
                rsccData->selectedDeviceAddr.addr.nap   = prim->deviceAddr.nap;
                rsccData->selectedDeviceAddr.addr.uap   = prim->deviceAddr.uap;
                rsccData->selectedDeviceAddr.type       = (prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? TBDADDR_RANDOM:TBDADDR_PUBLIC;
                rsccData->selectedInfo                  = prim->deviceStatus;

                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(rsccData->profileName),
                                     FALSE, 
                                     "Received selected DeviceAddr: %04X:%02X:%06X (%s)", 
                                     prim->deviceAddr.nap, 
                                     prim->deviceAddr.uap, 
                                     prim->deviceAddr.lap,
                                     (CsrBtAddrIsRandom(rsccData->selectedDeviceAddr))? "Random":"Public");
                /*update UI*/
                CsrUiUieShowReqSend(rsccData->csrUiVar.hMainMenu,
                                    CsrSchedTaskQueueGet(),
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_RSCC_APP_CSR_UI_PRI);
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_RSCC_APP",
                                    0, 
                                    *prim, 
                                    "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtRsccAppHandler,");
            }
            break;
        }
        case CSR_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) rsccData->recvMsgP;

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
                CsrGeneralException("CSR_BT_RSCC_APP",
                                    0, 
                                    *prim, 
                                    "####### Unhandled CSR_APP_PRIM in CsrBtRsccAppHandler,");
            }
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrBtGattAppHandleRsccCsrUiPrim(rsccData);
            break;
        }

        default:
        {
            /* unexpected primitive received */
            CsrGeneralException("CSR_BT_RSCC_APP",
                                0, 
                                *((CsrPrim *) rsccData->recvMsgP),
                                "####### default in CsrBtRsccAppHandler,");
        }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(rsccData->recvMsgP);
}


/******************************************************************************
 * Function to handle all cm-primitives.
 ******************************************************************************/
void CsrBtRsccAppHandleCmPrim(CsrBtRsccAppInstData *rsccData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) rsccData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_CM_READ_RSSI_CFM:
        {
            CsrBtCmReadRssiCfm *cfm = (CsrBtCmReadRssiCfm*)rsccData->recvMsgP;
            rsccData->rssiLevel = cfm->rssi;
            /*update UI*/
            CsrBtGattAppHandleRsccCsrUiMenuUpdate(rsccData);

            break;
        }
    }
}
/******************************************************************************
 * Function to handle all rscc-primitives.
 ******************************************************************************/
void CsrBtRsccAppHandleRsccPrim(CsrBtRsccAppInstData *rsccData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) rsccData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_GATT_REGISTER_CFM:
        {
            CsrBtGattRegisterCfm *cfm = (CsrBtGattRegisterCfm*)rsccData->recvMsgP;
            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                rsccData->gattId = cfm->gattId;
                /*update UI*/
                CsrBtGattAppHandleRsccCsrUiRegisterUnregister(rsccData, TRUE);
            }
            else
            {
                /*update UI*/
                CsrBtGattAppHandleRsccCsrUiRegisterUnregister(rsccData, FALSE);
            }

            break;
        }
        case CSR_BT_GATT_UNREGISTER_CFM:
        {
                /*update UI*/
            CsrBtGattAppHandleRsccCsrUiRegisterUnregister(rsccData, FALSE);

            break;
        }
        case CSR_BT_GATT_CONNECT_IND:
        {
            CsrBtGattConnectInd  *ind;

            ind                   = (CsrBtGattConnectInd *) rsccData->recvMsgP;
            rsccData->btConnId  = ind->btConnId;

            if (ind->resultSupplier == CSR_BT_SUPPLIER_GATT &&
                ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(rsccData->profileName),
                                     TRUE, 
                                     "Successfully Connected with: %04X:%02X:%06X", 
                                     ind->address.addr.nap, 
                                     ind->address.addr.uap, 
                                     ind->address.addr.lap);
                rsccData->connectedDeviceAddr.addr.nap = ind->address.addr.nap;
                rsccData->connectedDeviceAddr.addr.lap = ind->address.addr.lap;
                rsccData->connectedDeviceAddr.addr.uap = ind->address.addr.uap;

                /* Start treversing the DB to get all relevant info */
                rsccData->traversingDb = TRUE;

                CsrBtGattDiscoverAllPrimaryServicesReqSend(rsccData->gattId, rsccData->btConnId);
                /*update UI*/
                CsrBtGattAppHandleRsccCsrUiConnectDisconnect(rsccData,
                                                             TRUE,
                                                             (CsrBool)((ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS &&
                                                                        ind->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));
                CsrBtGattAppHandleRsccCsrUiTraversingdb(rsccData,TRUE);
            }
            else
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(rsccData->profileName),
                                     TRUE, 
                                     "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d", 
                                     ind->address.addr.nap, 
                                     ind->address.addr.uap, 
                                     ind->address.addr.lap, 
                                     ind->resultCode, 
                                     ind->resultSupplier);
                /*update UI*/
                CsrBtGattAppHandleRsccCsrUiConnectDisconnect(rsccData,
                                                             TRUE,
                                                              FALSE);
            }


            break;
        }
        case CSR_BT_GATT_DISCONNECT_IND:
        {
            CsrBtGattDisconnectInd  *ind;

            ind  = (CsrBtGattDisconnectInd *) rsccData->recvMsgP;

            CsrAppBacklogReqSend(TECH_BTLE, 
                                 PROFILE_NAME(rsccData->profileName),
                                 TRUE, 
                                 "Disconnected from: %04X:%02X:%06X", 
                                 ind->address.addr.nap, 
                                 ind->address.addr.uap, 
                                 ind->address.addr.lap);
            /*reset instance data */
            /*reset the btConnId */
            rsccData->btConnId = 0xff;

            /*Clear all handles in instance */
            CsrBtRsccAppClearHandles(rsccData);

            /*update UI*/
            CsrBtGattAppHandleRsccCsrUiConnectDisconnect(rsccData, FALSE, TRUE);


            break;
        }
        case CSR_BT_GATT_CENTRAL_CFM:
        {
            CsrBtGattCentralCfm *cfm;
            cfm = (CsrBtGattCentralCfm*) rsccData->recvMsgP;

            if(cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
            {
                /*We only care about non-successful as success will be handled on CONNECT_IND*/
                CsrBtGattAppHandleRsccCsrUiConnectDisconnect(rsccData,
                                                             TRUE,
                                                            (CsrBool)((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                       cfm->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));

            }
            break;
        }
        case CSR_BT_GATT_BREDR_CONNECT_CFM:
        {
            CsrBtGattBredrConnectCfm *cfm;
            cfm = (CsrBtGattBredrConnectCfm*) rsccData->recvMsgP;

            if(cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
            {
                /* We only care about non-successful as success will be handled on CONNECT_IND */
                CsrBtGattAppHandleRsccCsrUiConnectDisconnect(rsccData,
                                                             TRUE,
                                                            (CsrBool)((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                       cfm->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));

            }
            break;
        }
        case CSR_BT_GATT_DISCOVER_SERVICES_IND:
        {
            CsrBtGattDiscoverServicesInd *ind;
            ind = (CsrBtGattDiscoverServicesInd*) rsccData->recvMsgP;
            /*check to see if we already have this prim service*/
            if(!CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(rsccData->dbPrimServices, &ind->uuid))
            {
                CsrBtGattAppDbPrimServiceElement *elem = CSR_BT_GATT_APP_DB_ADD_PRIM_SERVICE(rsccData->dbPrimServices);
                elem->uuid        = ind->uuid;
                elem->startHandle = ind->startHandle;
                elem->endHandle   = ind->endHandle;
                elem->id          = rsccData->dbPrimServicesCount++;
            }
            break;
        }
        case CSR_BT_GATT_DISCOVER_SERVICES_CFM:
        {
            CsrBtGattDiscoverServicesCfm *cfm;
            cfm = (CsrBtGattDiscoverServicesCfm*)rsccData->recvMsgP;

            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                /* Start with first primary service element and find all characteristics */
                for(rsccData->currentPrimService = CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(rsccData->dbPrimServices);  rsccData->currentPrimService != NULL;  rsccData->currentPrimService = rsccData->currentPrimService->next)
                {
                    if(rsccData->currentPrimService->startHandle != rsccData->currentPrimService->endHandle)
                    {
                        CsrBtGattDiscoverAllCharacOfAServiceReqSend(rsccData->gattId,
                                                                    rsccData->btConnId,
                                                                    rsccData->currentPrimService->startHandle,
                                                                    rsccData->currentPrimService->endHandle);
                        break;
                    }   
                }

            }
            else
            {
                /*Update UI with error */
/*FIXME                   CsrBtGattAppHandleRsccCsrUiDiscoverServices(rsccData, 0x0000, FALSE);*/
            }
            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_IND:
        {
            CsrBtGattDiscoverCharacInd *ind;
            CsrBtGattAppDbCharacElement      *cElem = CSR_BT_GATT_APP_DB_ADD_CHARAC(rsccData->currentPrimService->characList);
            ind = (CsrBtGattDiscoverCharacInd*) rsccData->recvMsgP;
            cElem->valueHandle       = ind->valueHandle;
            cElem->declarationHandle = ind->declarationHandle;
            cElem->property          = ind->property;
            cElem->uuid              = ind->uuid;

            if(CSR_BT_UUID_GET_16(cElem->uuid) == CSR_BT_GATT_UUID_SC_CONTROL_POINT_CHARAC)
            {
                rsccData->scControlPointHandle = cElem->valueHandle;
            }

            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_CFM:
        {
            CsrBtGattDiscoverCharacCfm *cfm;
            cfm = (CsrBtGattDiscoverCharacCfm*) rsccData->recvMsgP;
            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                /*move on to next prim service (if any and find all of its charac*/
                
                for( rsccData->currentPrimService = rsccData->currentPrimService->next;  rsccData->currentPrimService != NULL;  rsccData->currentPrimService = rsccData->currentPrimService->next)
                {
                    if(rsccData->currentPrimService->startHandle != rsccData->currentPrimService->endHandle)
                    {
                        break;
                    }   
                }
                if(rsccData->currentPrimService)
                {
                    CsrBtGattDiscoverAllCharacOfAServiceReqSend(rsccData->gattId,
                                                                rsccData->btConnId,
                                                                rsccData->currentPrimService->startHandle,
                                                                rsccData->currentPrimService->endHandle);
                }
                else
                {
                    CsrBool found = FALSE;
                    rsccData->currentPrimService = CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(rsccData->dbPrimServices);
                    
                    while (!found && (rsccData->currentPrimService != NULL))
                    {
                        rsccData->currentCharac = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(rsccData->currentPrimService->characList);
                        while (!found && (rsccData->currentCharac != NULL))
                        {
                            if (rsccData->currentCharac->property & CSR_BT_GATT_CHARAC_PROPERTIES_READ)
                            {
                                /* Readable handle - stop */
                                CsrBtGattReadReqSend(rsccData->gattId, rsccData->btConnId, rsccData->currentCharac->valueHandle, 0);
                                found = TRUE;
                                break;
                            }
                            rsccData->currentCharac = rsccData->currentCharac->next;
                        }
                        if(!found)
                        {
                            rsccData->currentPrimService = rsccData->currentPrimService->next;
                        }
                    }
                    
                    if (!found)
                    {
                        /* Failure */
                        /*CsrBtGattAppHandleProxcCsrUiReadValue(rsccData, NULL, FALSE);*/
                    }
                }
            }

            break;
        }
        case CSR_BT_GATT_READ_CFM:
        {
            CsrUint16 startHandle, endHandle  = 0;
            CsrBtGattReadCfm *cfm;
            cfm = (CsrBtGattReadCfm*)rsccData->recvMsgP;

            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                /*store the values */
                rsccData->currentCharac->value       = cfm->value;
                rsccData->currentCharac->valueLength = cfm->valueLength;
                /*update menu with new value */
                CsrBtGattAppHandleRsccCsrUiMenuUpdate(rsccData);
            }
            else
            {
                rsccData->currentCharac->value = CsrPmemZalloc(sizeof(CsrUint8));
                rsccData->currentCharac->valueLength = sizeof(CsrUint8);
            }
            
            /* Get next */
            if(CsrBtGattAppDbFindNextCharacWithProperty(&rsccData->currentPrimService,
                                                        &rsccData->currentCharac,
                                                        CSR_BT_GATT_CHARAC_PROPERTIES_READ))
            {
                CsrBtGattReadReqSend(rsccData->gattId, rsccData->btConnId,rsccData->currentCharac->valueHandle,0);
            }
            else
            {
                /* Done Reading!
                    * we need to reset the "current" pointers and start working on the descriptors*/
                rsccData->currentPrimService  = CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(rsccData->dbPrimServices);
                /*start from the first charac (we know that there is at least one) */
                rsccData->currentCharac  = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(rsccData->currentPrimService->characList);
                startHandle = rsccData->currentCharac->declarationHandle + 1;
                
                if(rsccData->currentCharac->next)
                {
                    /* there are more characs in this prim service so we use the start of the next one as end handle*/
                    endHandle = ((CsrBtGattAppDbCharacElement*)rsccData->currentCharac->next)->declarationHandle - 1;
                }
                else
                {
                    /* We are at the last charac in the current prim service so endhandle
                     * is start of next prim service (NOTE: we expect them to be sorted) */
                    endHandle = ((CsrBtGattAppDbPrimServiceElement*)rsccData->currentPrimService->next)->startHandle - 1;
                }
                CsrBtGattDiscoverAllCharacDescriptorsReqSend(rsccData->gattId, rsccData->btConnId, startHandle, endHandle);
            }
            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_IND:
        {
            /* add new element to currentCharac descrList */
            CsrBtGattDiscoverCharacDescriptorsInd *ind;
            CsrBtGattAppDbCharacDescrElement      *dElem = CSR_BT_GATT_APP_DB_ADD_CHARAC_DESCR(rsccData->currentCharac->descrList);
            ind = (CsrBtGattDiscoverCharacDescriptorsInd*) rsccData->recvMsgP;
            dElem->descriptorHandle                      = ind->descriptorHandle;
            dElem->uuid                                  = ind->uuid;
            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_CFM:
        {
            /* Move on to next Charac or if at last one, then move to next Prim Service first charac */
            CsrBtGattDiscoverCharacDescriptorsCfm *cfm;
            CsrUint16 startHandle = 0;
            CsrUint16 endHandle   = 0;
            cfm                   = (CsrBtGattDiscoverCharacDescriptorsCfm*) rsccData->recvMsgP;

            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS || cfm->resultCode == CSR_BT_GATT_RESULT_INVALID_HANDLE_RANGE )

            {
                if(CsrBtGattAppDbFindNextCharacWithProperty(&rsccData->currentPrimService, &rsccData->currentCharac, 0))
                {
                    startHandle  =  rsccData->currentCharac->declarationHandle + 1;
                    if(rsccData->currentCharac->next)
                    {
                        /*still not the last charac in prim service so get endhandle from next charac*/
                        endHandle =  ((CsrBtGattAppDbCharacElement*)rsccData->currentCharac->next)->declarationHandle - 1;
                    }
                    else
                    {              
                        /*get endhandle from prim service */
                        if(rsccData->currentPrimService)
                        {
                            endHandle = rsccData->currentPrimService->endHandle;
                        }
                        else 
                        {
                            endHandle = 0xFFFF;
                        }
                        /* move to next prim service*/
                    }
                    CsrBtGattDiscoverAllCharacDescriptorsReqSend(rsccData->gattId, rsccData->btConnId, startHandle, endHandle);

                }
                else 
                {
                    /*we are done traversing the DB*/
                    rsccData->traversingDb = FALSE;
                    CsrBtGattAppHandleRsccCsrUiTraversingdb(rsccData,FALSE);
                }
            }
            break;
        }
        case CSR_BT_GATT_WRITE_CFM :
        {
            CsrBtGattWriteCfm *cfm;
            cfm = (CsrBtGattWriteCfm*)rsccData->recvMsgP;

            if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                if(!rsccData->rscScWriteReq)
                {
                    if(rsccData->rscNotif)
                    {
                        rsccData->rscNotif = FALSE;
                    }
                    else 
                    {
                        rsccData->rscNotif = TRUE;
                    }
                    /* updating UI popup + menu */
                    CsrBtGattAppHandleRsccCsrUiWriteValue(rsccData,  TRUE);
                }
                else
                {
                    if(rsccData->rscWriteCC)
                    {
                        rsccData->rscWriteCC = FALSE;
                        if(rsccData->rscInd)
                        {
                            rsccData->rscInd = FALSE;
                        }
                        else 
                        {
                            rsccData->rscInd = TRUE;
                        }
                        /* updating UI popup + menu */
                        CsrBtGattAppHandleRsccCsrUiWriteValue(rsccData,  TRUE);
                    }
                    /*update UI*/                 
                    rsccData->rscScWriteReq = FALSE;
                }
            }
            else if(cfm->resultCode == CSR_BT_GATT_ACCESS_RES_IMPROPERLY_CONFIGURED)
            {
                CsrBtGattAppHandleRsccCsrUiWriteError81(rsccData);
            }
            else
            {
                /* updating UI popup + menu */
                CsrBtGattAppHandleRsccCsrUiWriteValue(rsccData, FALSE);
            }
            break;
        }
        case CSR_BT_GATT_NOTIFICATION_IND:  
        {
            /* find measurement characteristic */
            CsrBtGattAppDbCharacElement *cElem;
            CsrBtGattNotificationInd *ind            =  (CsrBtGattNotificationInd*)rsccData->recvMsgP;
            CsrBtGattAppDbPrimServiceElement *pElem  =  CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(rsccData->dbPrimServices, 
                                                                                                     CsrBtRsccAppUuid16ToUuid128(CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE));
            if(ind->valueHandle == rsccData->scControlPointHandle)
            {
                cElem   =  CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList,
                                                                  CsrBtRsccAppUuid16ToUuid128(CSR_BT_GATT_UUID_SC_CONTROL_POINT_CHARAC));            
            }
            else
            {
                cElem   =  CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList,
                                                                  CsrBtRsccAppUuid16ToUuid128(CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC));
            }
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

                /* [QTI] Fix KW issue#831973 through moving the following code into the "cElem" checked code block. */
                if(ind->valueHandle == rsccData->scControlPointHandle)
                {
                    CsrBtGattAppHandleRsccCsrUiShowIndInfo(rsccData, cElem->valueLength, cElem->value);
                }
                else
                { /* Notifications  - update menu */
                    CsrBtGattAppHandleRsccCsrUiMenuUpdate(rsccData);
                }
            }

            break;
        }
        default:
        {
            CsrAppBacklogReqSend(TECH_BTLE, 
                                 PROFILE_NAME(rsccData->profileName),
                                 TRUE, 
                                 "####### default in gatt prim handler 0x%04x,",
                                 *primType);
            CsrBtGattFreeUpstreamMessageContents(CSR_BT_GATT_PRIM, rsccData->recvMsgP);
            /* unexpected primitive received */
        }
    }
}

