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
#include "csr_bt_gatt_demo_le_browser_app_private_prim.h"
#include "csr_bt_gatt_demo_le_browser_app_ui_sef.h"
#include "csr_bt_gatt_app_task.h"
#include "csr_bt_gatt_demo_db_utils.h"
#include "csr_bt_gatt_demo_le_browser_app_menus.h"
#include "csr_bt_gatt_demo_le_browser_app_defines.h"

/* Forward declarations for handler functions */
void CsrBtLeBrowserAppHandleLeBrowserPrim(CsrBtLeBrowserAppInstData *leBrowserData);



CsrBtUuid *CsrBtLeBrowserAppUuid16ToUuid128(CsrUint16 uuid)
{
    CsrBtUuid *realUuid = CsrPmemAlloc(sizeof(CsrBtUuid));
    CSR_COPY_UINT16_TO_LITTLE_ENDIAN(uuid, realUuid->uuid);
    
    realUuid->length = CSR_BT_UUID16_SIZE;

    return realUuid;
} 


/******************************************************************************
CsrBtLeBrowserAppStartRegister:
 *****************************************************************************/
void CsrBtLeBrowserAppStartRegister(CsrBtLeBrowserAppInstData *leBrowserData)
{
    CsrBtGattRegisterReqSend(leBrowserData->phandle,
                             1234);
}
/******************************************************************************
CsrBtLeBrowserAppStartUnregister:
 *****************************************************************************/
void CsrBtLeBrowserAppStartUnregister(CsrBtLeBrowserAppInstData *leBrowserData)
{
    CsrBtGattUnregisterReqSend(leBrowserData->gattId);
}
/******************************************************************************
CsrBtLeBrowserAppStartConnecting:
 *****************************************************************************/
void CsrBtLeBrowserAppStartConnecting(CsrBtLeBrowserAppInstData *leBrowserData)
{
    if(leBrowserData->selectedDeviceAddr.addr.uap != 0)
    {
        CsrBtGattCentralReqSend(leBrowserData->gattId,
                                (leBrowserData->selectedDeviceAddr),
                                CSR_BT_GATT_FLAGS_NONE,
                                0);
    }
    else
    {
        /*no device addr have been selected in GAP yet*/        
        CsrBtGattAppHandleLeBrowserCsrUiNoSelectedDevAddr(leBrowserData);
    }
}
/******************************************************************************
CsrBtLeBrowserAppStartReadUuid:
 *****************************************************************************/
void CsrBtLeBrowserAppStartReadUuid(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 uuid)
{
    CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend(leBrowserData->gattId, leBrowserData->btConnId, uuid);
}

void CsrBtLeBrowserAppStartReadValue(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 pUuid, CsrUint16 cUuid)
{
    CsrUint16                         valueHandle;
    CsrBtGattAppDbPrimServiceElement  *pElem;
    CsrBtGattAppDbCharacElement       *cElem;
 
    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(leBrowserData->dbElement->dbPrimServices,
                                                         CsrBtLeBrowserAppUuid16ToUuid128(pUuid));
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtLeBrowserAppUuid16ToUuid128(cUuid));
    valueHandle = cElem->valueHandle;

    CsrBtGattReadReqSend(leBrowserData->gattId,
                         leBrowserData->btConnId,
                         valueHandle,
                         0);
}
void CsrBtLeBrowserAppStartWriteServerConfig(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 value)
{
    CsrBtGattLeBrowserUpdateDbElement(leBrowserData, CSR_BT_GATT_LE_BROWSER_LVL_1);

    if(leBrowserData->dbElement->currentCharac == NULL)
    {
        return; /* the charac was not found */
    }
    
    leBrowserData->dbElement->currentDescr = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(leBrowserData->dbElement->currentCharac->descrList, 
                                                         CsrBtLeBrowserAppUuid16ToUuid128(CSR_BT_GATT_SERVER_CHARACTERISTIC_CONFIGURATION));
    if(leBrowserData->dbElement->currentDescr == NULL)
    {
        return; /* the charac descriptor was not found */
    }

    CsrBtGattWriteServerConfigurationReqSend(leBrowserData->gattId,
                                             leBrowserData->btConnId,
                                             leBrowserData->dbElement->currentCharac->valueHandle,value);
}

void CsrBtLeBrowserAppStartWriteClientConfig(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 value)
{
    CsrBtUuid *uuid = CsrBtLeBrowserAppUuid16ToUuid128(CSR_BT_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION);
    CsrBtGattLeBrowserUpdateDbElement(leBrowserData, CSR_BT_GATT_LE_BROWSER_LVL_1);

    if(leBrowserData->dbElement->currentCharac == NULL)
    {
        return; /* the charac was not found */
    }
    
    leBrowserData->dbElement->currentDescr = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(leBrowserData->dbElement->currentCharac->descrList, uuid);
    if(leBrowserData->dbElement->currentDescr == NULL)
    {
        return; /* the charac descriptor was not found */
    }

    CsrBtGattWriteClientConfigurationReqSend(leBrowserData->gattId,
                                             leBrowserData->btConnId,
                                             leBrowserData->dbElement->currentDescr->descriptorHandle,
                                             leBrowserData->dbElement->currentCharac->valueHandle,value);
    CsrPmemFree(uuid);
}
void CsrBtLeBrowserAppStartWriteValue(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 pUuid, CsrUint16 cUuid)
{
    CsrUint16                         valueHandle;
    CsrBtGattAppDbPrimServiceElement  *pElem;
    CsrBtGattAppDbCharacElement       *cElem;

    /*Find Primary Service containing the Charac */
    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(leBrowserData->dbElement->dbPrimServices,
                                                         CsrBtLeBrowserAppUuid16ToUuid128(pUuid));
    /*Find charac containing the handleValue */
    cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                   CsrBtLeBrowserAppUuid16ToUuid128(cUuid));
    valueHandle = cElem->valueHandle;
    
    leBrowserData->writeValueLength = 1;
    leBrowserData->writeValue = CsrPmemAlloc(1);
    switch(pUuid)
    {
        case CSR_BT_GATT_UUID_LINK_LOSS_SERVICE:
            {
                if(cElem->value[0]>=CSR_BT_GATT_APP_ALERT_LEVEL_HIGH)
                {
                    leBrowserData->writeValue[0]  = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                    cElem->value[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                }
                else
                {
                    cElem->value[0]           += 1;
                    leBrowserData->writeValue[0]  = (CsrUint8)cElem->value[0];
                }
                break;
            }
        case CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE:
            {
                if(CSR_BT_GATT_APP_ALERT_LEVEL_OFF == cElem->value[0])
                {
                    cElem->value[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_HIGH;
                    leBrowserData->writeValue[0]  = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_HIGH;
                }
                else
                {
                    cElem->value[0]            = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                    leBrowserData->writeValue[0]  = (CsrUint8)CSR_BT_GATT_APP_ALERT_LEVEL_OFF;
                }

                break;
            }
    }
    CsrBtGattWriteReqSend(leBrowserData->gattId,
                          leBrowserData->btConnId,
                          valueHandle,
                          0,
                          leBrowserData->writeValueLength, 
                          leBrowserData->writeValue);
}

void CsrBtLeBrowserAppSetDescriptor(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 type)
{
    CsrBtGattLeBrowserUpdateDbElement(leBrowserData, CSR_BT_GATT_LE_BROWSER_LVL_2);

    switch(type)
    {
        case CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST:
        {
            if(leBrowserData->broadcasting == TRUE)
            {
                if(leBrowserData->dbElement->currentDescr->value == NULL)
                {
                    leBrowserData->dbElement->currentDescr->value = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8));
                }
                *leBrowserData->dbElement->currentDescr->value = CSR_BT_LE_BROWSER_APP_PROPERDY_ENABLED;
                leBrowserData->dbElement->currentDescr->valueLength = CSR_BT_LE_BROWSER_APP_PROPERTY_VALUE_LEN;
            }
            else
            {
                if(leBrowserData->dbElement->currentDescr->value == NULL)
                {
                    leBrowserData->dbElement->currentDescr->value = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8));
                }
                *leBrowserData->dbElement->currentDescr->value = CSR_BT_LE_BROWSER_APP_PROPERTY_DISABLED;
                leBrowserData->dbElement->currentDescr->valueLength = CSR_BT_LE_BROWSER_APP_PROPERTY_VALUE_LEN;
            }
            break;
        }
        case CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE:
        {
            if(leBrowserData->tempInd == TRUE)
            {
                if(leBrowserData->dbElement->currentDescr->value == NULL)
                {
                    leBrowserData->dbElement->currentDescr->value = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8));
                }
                *leBrowserData->dbElement->currentDescr->value = CSR_BT_LE_BROWSER_APP_PROPERDY_ENABLED;
                leBrowserData->dbElement->currentDescr->valueLength = CSR_BT_LE_BROWSER_APP_PROPERTY_VALUE_LEN;
            }
            else
            {
                if(leBrowserData->dbElement->currentDescr->value == NULL)
                {
                    leBrowserData->dbElement->currentDescr->value = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8));
                }
                *leBrowserData->dbElement->currentDescr->value = CSR_BT_LE_BROWSER_APP_PROPERTY_DISABLED;
                leBrowserData->dbElement->currentDescr->valueLength = CSR_BT_LE_BROWSER_APP_PROPERTY_VALUE_LEN;
            }
            break;
        }
        case CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY:
        {
            if(leBrowserData->tempNotif == TRUE)
            {
                if(leBrowserData->dbElement->currentDescr->value == NULL)
                {
                    leBrowserData->dbElement->currentDescr->value = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8));
                }
                *leBrowserData->dbElement->currentDescr->value = CSR_BT_LE_BROWSER_APP_PROPERDY_ENABLED;
                leBrowserData->dbElement->currentDescr->valueLength = CSR_BT_LE_BROWSER_APP_PROPERTY_VALUE_LEN;
            }
            else
            {
                if(leBrowserData->dbElement->currentDescr->value == NULL)
                {
                    leBrowserData->dbElement->currentDescr->value = (CsrUint8*)CsrPmemZalloc(sizeof(CsrUint8));
                }
                *leBrowserData->dbElement->currentDescr->value = CSR_BT_LE_BROWSER_APP_PROPERTY_DISABLED;
                leBrowserData->dbElement->currentDescr->valueLength = CSR_BT_LE_BROWSER_APP_PROPERTY_VALUE_LEN;
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/******************************************************************************
CsrBtLeBrowserAppStartDisconnect:
 *****************************************************************************/
void CsrBtLeBrowserAppStartDisconnect(CsrBtLeBrowserAppInstData *leBrowserData)
{

    CsrBtGattDisconnectReqSend(leBrowserData->gattId, leBrowserData->btConnId);
    /*reset BT connection ID */
    leBrowserData->btConnId = 0xff;
}

/******************************************************************************
CsrBtLeBrowserAppInitInstanceData:
 *****************************************************************************/
void CsrBtLeBrowserAppClearHandles(CsrBtLeBrowserAppInstData *leBrowserData){

    /*init list for local database structure copy */ 

    CsrCmnListInit(&leBrowserData->dbElement->dbPrimServices, 0, CsrBtGattAppInitDbPrimList, CsrBtGattAppFreeDbPrimList);
    leBrowserData->dbElement->dbPrimServicesCount               = 0;


}
void CsrBtLeBrowserAppInitInstanceData(CsrBtLeBrowserAppInstData *leBrowserData)
{
    CsrMemSet(leBrowserData,0,sizeof(CsrBtLeBrowserAppInstData));
    leBrowserData->dbElement = (CsrBtGattDbElement*)CsrPmemZalloc(sizeof(CsrBtGattDbElement));

    leBrowserData->btConnId                          = 0xff;
    leBrowserData->csrUiVar.hMainMenu                = CSR_UI_DEFAULTACTION;
    leBrowserData->csrUiVar.hPrimMenu                = CSR_UI_DEFAULTACTION;
    leBrowserData->csrUiVar.hCharMenu                = CSR_UI_DEFAULTACTION;
    leBrowserData->csrUiVar.hDescMenu                = CSR_UI_DEFAULTACTION;
    leBrowserData->csrUiVar.hEditMenu                = CSR_UI_DEFAULTACTION;
    leBrowserData->csrUiVar.hInputDialog             = CSR_UI_DEFAULTACTION;
    leBrowserData->csrUiVar.hCommonDialog            = CSR_UI_DEFAULTACTION;
    leBrowserData->tempNotif                         = FALSE;
    leBrowserData->tempInd                           = FALSE;
    leBrowserData->broadcasting                      = FALSE;
    leBrowserData->resultCode                        = 0;
    leBrowserData->writeMethod                       = 0;
    leBrowserData->inputStr                          = NULL;
    leBrowserData->setNewValue                       = FALSE;
    leBrowserData->propertyToWrite                   = 0;
    leBrowserData->dbElement->traversingDb           = FALSE;
    leBrowserData->selectedPrimService               = 0;
    leBrowserData->selectedCharac                    = 0;
    leBrowserData->selectedDescriptor                = 0;
    CsrBtLeBrowserAppClearHandles(leBrowserData);
}

static void csrBtLeBrowserAppSendLeBrowserAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtLeBrowserAppRegisterReq *prim;

    prim           = (CsrBtLeBrowserAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtLeBrowserAppRegisterReq));
    prim->type     = CSR_BT_LE_BROWSER_APP_REGISTER_REQ;
    prim->phandle  = pHandle;

    CsrSchedMessagePut(CSR_BT_LE_BROWSER_APP_IFACEQUEUE, CSR_BT_LE_BROWSER_APP_PRIM,prim);
}

static void csrBtLeBrowserAppSendLeBrowserAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtLeBrowserAppRegisterCfm *prim;

    prim           = (CsrBtLeBrowserAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtLeBrowserAppRegisterCfm));
    prim->type     = CSR_BT_LE_BROWSER_APP_REGISTER_CFM;
    prim->queueId  = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_LE_BROWSER_APP_PRIM,prim);
}

static char * csrBtLeBrowserAppReturnLeBrowserInstName(CsrBtLeBrowserAppInstData *instance)
{
    char *buf;
    CsrUint32 bufSize = CsrStrLen("LE Browser") + 1;
    buf  = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "LE Browser");
    instance->dbElement->traversingDb = TRUE;
    return buf;
}



/******************************************************************************
 * Init function called by the scheduler upon initialisation.
 * This function is used to boot the demo application.
 *****************************************************************************/
void CsrBtLeBrowserAppInit(void **gash)
{
    CsrBtLeBrowserAppInstData    *leBrowserData;

    /* allocate and initialise instance data space */
    *gash      = (void *) CsrPmemAlloc(sizeof(CsrBtLeBrowserAppInstData));
    leBrowserData = (CsrBtLeBrowserAppInstData *) *gash;

    CsrBtLeBrowserAppInitInstanceData(leBrowserData);

    leBrowserData->phandle     = CsrSchedTaskQueueGet();
    leBrowserData->initialized = TRUE;
    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    csrBtLeBrowserAppSendLeBrowserAppRegisterReq(leBrowserData->phandle);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtLeBrowserAppDeinit(void **gash)
{
    CsrUint16               msg_type;
    void                    *msg_data;
    CsrBtLeBrowserAppInstData  *leBrowserData;

    leBrowserData = (CsrBtLeBrowserAppInstData *) (*gash);
    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    CsrCmnListDeinit(&leBrowserData->dbElement->dbPrimServices);
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
    
    CsrPmemFree(leBrowserData->dbElement);

    CsrPmemFree(leBrowserData);
}
#else
void CsrBtLeBrowserAppDeinit(void **gash)
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
void CsrBtLeBrowserAppHandler(void **gash)
{
           CsrBtLeBrowserAppInstData  *leBrowserData;
           CsrUint16               eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
       and the event type in eventType */
    leBrowserData = (CsrBtLeBrowserAppInstData *) (*gash);

    if(!leBrowserData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &leBrowserData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&leBrowserData->saveQueue, &eventType , &leBrowserData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
             a message from the scheduler                                                   */
            leBrowserData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &leBrowserData->recvMsgP);
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
                CsrBtLeBrowserAppHandleLeBrowserPrim(leBrowserData);
                break;
            }

        case CSR_BT_LE_BROWSER_APP_PRIM:
            {
                CsrPrim *type = (CsrPrim *) leBrowserData->recvMsgP;

                switch(*type)
                {
                    case CSR_BT_LE_BROWSER_APP_REGISTER_REQ:
                        { /* only received by controling leBrowser demo app instance */
                            if(leBrowserData->initialized)
                            {
                                CsrBtLeBrowserAppRegisterReq *prim  = leBrowserData->recvMsgP;
                                CsrSchedQid queueId              = CSR_SCHED_QID_INVALID;
                                leBrowserData->leBrowserAppHandle      = prim->phandle;
                                queueId                          = leBrowserData->leBrowserProfileHandle;
   
                                if(queueId != CSR_SCHED_QID_INVALID)
                                {
                                    csrBtLeBrowserAppSendLeBrowserAppRegisterCfm(prim->phandle, queueId,1);
                                }
                                else
                                {
                                    CsrGeneralException("CSR_LE_BROWSER_DEMO_APP", 
                                                        0, 
                                                        *type,
                                                        "No CsrBtLeBrowser profile Queue Handle Available");
                                }
                            }
                            else
                            {
                                CsrMessageQueuePush(&leBrowserData->saveQueue, CSR_BT_GATT_APP_PRIM, leBrowserData->recvMsgP);
                                leBrowserData->recvMsgP = NULL;
                            }
                            break;
                        }
                    case CSR_BT_LE_BROWSER_APP_REGISTER_CFM: 
                        { /* Required to register LE Browser with Synergy */

                            leBrowserData->profileName = csrBtLeBrowserAppReturnLeBrowserInstName(leBrowserData);

                            CsrBtGattAppHandleLeBrowserSetMainMenuHeader(leBrowserData);

                            leBrowserData->initialized = TRUE;

                            CsrAppRegisterReqSend(leBrowserData->phandle, TECH_BTLE, PROFILE_NAME(leBrowserData->profileName));
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
                CsrPrim *prim = (CsrPrim *) leBrowserData->recvMsgP;
                if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
                {
                    CsrBtGapAppGetSelectedDeviceCfm *prim    = leBrowserData->recvMsgP;
                    leBrowserData->selectedDeviceAddr.addr.lap  = prim->deviceAddr.lap;
                    leBrowserData->selectedDeviceAddr.addr.nap  = prim->deviceAddr.nap;
                    leBrowserData->selectedDeviceAddr.addr.uap  = prim->deviceAddr.uap;
                    leBrowserData->selectedDeviceAddr.type     = (prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? TBDADDR_RANDOM:TBDADDR_PUBLIC;
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(leBrowserData->profileName), 
                                         FALSE, 
                                         "Received selected DeviceAddr: %04X:%02X:%06X (%s)", 
                                         prim->deviceAddr.nap, 
                                         prim->deviceAddr.uap, 
                                         prim->deviceAddr.lap,
                                         (CsrBtAddrIsRandom(leBrowserData->selectedDeviceAddr))? "Random":"Public");
                    /*update UI*/
                    CsrUiUieShowReqSend(leBrowserData->csrUiVar.hMainMenu,
                                        CsrSchedTaskQueueGet(),
                                        CSR_UI_INPUTMODE_AUTO,
                                        CSR_BT_LE_BROWSER_APP_CSR_UI_PRI);
                }
                else
                {
                    /* unexpected primitive received */
                    CsrGeneralException("CSR_BT_LE_BROWSER_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtLeBrowserAppHandler,");
                }
                break;
            }
        case CSR_APP_PRIM:
            {
                CsrPrim *prim = (CsrPrim *) leBrowserData->recvMsgP;

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
                    CsrGeneralException("CSR_BT_LE_BROWSER_APP", 
                                        0, 
                                        *prim, 
                                        "####### Unhandled CSR_APP_PRIM in CsrBtLeBrowserAppHandler,");
                }
                break;
            }
        case CSR_UI_PRIM:
            {
                CsrBtGattAppHandleLeBrowserCsrUiPrim(leBrowserData);
                break;
            }

        default:
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_LE_BROWSER_APP",
                                    0, 
                                    *((CsrPrim *) leBrowserData->recvMsgP),
                                    "####### default in CsrBtLeBrowserAppHandler,");
                break;
            }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
     * the pointer (prim) must be set to NULL in order not to free it here
     */
    CsrPmemFree(leBrowserData->recvMsgP);
}


/******************************************************************************
 * Function to handle all leBrowser-primitives.
 ******************************************************************************/
void CsrBtLeBrowserAppHandleLeBrowserPrim(CsrBtLeBrowserAppInstData *leBrowserData)
{
    CsrPrim *primType;
    CsrUint16 discoverServiceReturnValue;
    primType = (CsrPrim *) leBrowserData->recvMsgP;

    /*Use generic Service Discovery*/
    if(leBrowserData->dbElement->traversingDb == TRUE)
    {
        
        discoverServiceReturnValue = CsrBtGattDiscoverService(leBrowserData->dbElement, leBrowserData->recvMsgP, leBrowserData->gattId, leBrowserData->btConnId);
        if((discoverServiceReturnValue & CSR_BT_GATT_PRIM_HANDLED) == CSR_BT_GATT_PRIM_HANDLED)
        {
            if((discoverServiceReturnValue & CSR_BT_GATT_SERVICES_CFM_ERROR) == CSR_BT_GATT_SERVICES_CFM_ERROR)
            {
                /*Update UI with error */
    /*FIXME                   CsrBtGattAppHandleLeBrowserCsrUiDiscoverServices(leBrowserData, 0x0000, FALSE);*/
            }
            else if((discoverServiceReturnValue & CSR_BT_GATT_CHARAC_CFM_ERROR) == CSR_BT_GATT_CHARAC_CFM_ERROR)
            {
    /*FIXME              Error handling*/
                /* CSR_BT_GATT_DISCOVER_CHARAC_CFM */
            }
            else if((discoverServiceReturnValue & CSR_BT_GATT_CHARAC_DESCRIPTOR_CFM_ERROR) == CSR_BT_GATT_CHARAC_DESCRIPTOR_CFM_ERROR)
            {
                /*FIXME: Error handling*/
            }
            else if((discoverServiceReturnValue & CSR_BT_GATT_READ_CFM_ERROR) == CSR_BT_GATT_READ_CFM_ERROR)
            {
                   /*update UI*/
                CsrBtGattAppHandleLeBrowserCsrUiReadValue(leBrowserData, FALSE);
            }
            else if(leBrowserData->dbElement->traversingDb == FALSE)
            {
                if(!leBrowserData->csrUiVar.hPrimMenu)
                {
                    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
                }
                else
                {
                    CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(leBrowserData, CSR_BT_LE_BROWSER_PRIMARY_SERVICES_MENU_STATE);
                    CsrBtGattAppHandleLeBrowserCsrUiHidePopup(leBrowserData);
                }
                return;
            }
            else
            {
            
                return;            /* No error */
            }
            return;

        }
    }
    
    switch (*primType)
    {
        case CSR_BT_GATT_REGISTER_CFM:
            {
                CsrBtGattRegisterCfm *cfm = (CsrBtGattRegisterCfm*)leBrowserData->recvMsgP;
                if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    leBrowserData->gattId = cfm->gattId;
                    /*update UI*/
                    CsrBtGattAppHandleLeBrowserCsrUiRegisterUnregister(leBrowserData, TRUE);
                }
                else
                {
                    /*update UI*/
                    CsrBtGattAppHandleLeBrowserCsrUiRegisterUnregister(leBrowserData, FALSE);
                }

                break;
            }
        case CSR_BT_GATT_UNREGISTER_CFM:
            {
                    /*update UI*/
                CsrBtGattAppHandleLeBrowserCsrUiRegisterUnregister(leBrowserData, FALSE);

                break;
            }
        case CSR_BT_GATT_CONNECT_IND:
            {
                CsrBtGattConnectInd  *ind;

                ind                   = (CsrBtGattConnectInd *) leBrowserData->recvMsgP;
                leBrowserData->btConnId  = ind->btConnId;

                if (ind->resultSupplier == CSR_BT_SUPPLIER_GATT &&
                    ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(leBrowserData->profileName), 
                                         TRUE, 
                                         "Successfully Connected with: %04X:%02X:%06X", 
                                         ind->address.addr.nap, 
                                         ind->address.addr.uap, 
                                         ind->address.addr.lap);
                    leBrowserData->connectedDeviceAddr.addr.nap = ind->address.addr.nap;
                    leBrowserData->connectedDeviceAddr.addr.lap = ind->address.addr.lap;
                    leBrowserData->connectedDeviceAddr.addr.uap = ind->address.addr.uap;

                    /* Start treversing the DB to get all relevant info */
                    leBrowserData->dbElement->traversingDb = TRUE;
                    CsrBtGattDiscoverAllPrimaryServicesReqSend(leBrowserData->gattId, leBrowserData->btConnId);

                }
                else
                {
                    CsrAppBacklogReqSend(TECH_BTLE, 
                                         PROFILE_NAME(leBrowserData->profileName), 
                                         TRUE, 
                                         "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d", 
                                         ind->address.addr.nap, 
                                         ind->address.addr.uap, 
                                         ind->address.addr.lap, 
                                         ind->resultCode, 
                                         ind->resultSupplier);
                }
                /*update UI*/
                CsrBtGattAppHandleLeBrowserCsrUiConnectDisconnect(leBrowserData, 
                                                                TRUE,
                                                              (CsrBool)((ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                         ind->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));
                break;
            }
        case CSR_BT_GATT_DISCONNECT_IND:
            {
                CsrBtGattDisconnectInd  *ind;

                ind  = (CsrBtGattDisconnectInd *) leBrowserData->recvMsgP;

                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(leBrowserData->profileName), 
                                     TRUE, 
                                     "Disconnected from: %04X:%02X:%06X", 
                                     ind->address.addr.nap, 
                                     ind->address.addr.uap, 
                                     ind->address.addr.lap);
                /*reset instance data */
                /*reset the btConnId */
                leBrowserData->btConnId = 0xff;

                /*Clear all handles in instance */
                CsrBtLeBrowserAppClearHandles(leBrowserData);

                /*update UI*/
                CsrBtGattAppHandleLeBrowserCsrUiConnectDisconnect(leBrowserData, 
                                                              FALSE,
                                                              TRUE);


                break;
            }
        case CSR_BT_GATT_CENTRAL_CFM:
            {
                CsrBtGattCentralCfm *cfm;
                cfm = (CsrBtGattCentralCfm*) leBrowserData->recvMsgP;

                if(cfm->resultCode != CSR_BT_GATT_RESULT_SUCCESS)
                {
                    /*We only care about non-successful as success will be handled on CONNECT_IND*/
                    CsrBtGattAppHandleLeBrowserCsrUiConnectDisconnect(leBrowserData, 
                                                                  TRUE,
                                                                  (CsrBool)((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS && 
                                                                             cfm->resultSupplier == CSR_BT_SUPPLIER_GATT) ? TRUE : FALSE));

                }
                break;
            }
        case CSR_BT_GATT_WRITE_CFM :
            {
                CsrBtGattWriteCfm *cfm;
                cfm = (CsrBtGattWriteCfm*)leBrowserData->recvMsgP;

                if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    if(leBrowserData->propertyToWrite == CSR_BT_GATT_CHARAC_PROPERTIES_BROADCAST)
                    {
                        if(leBrowserData->broadcasting)
                        {
                            leBrowserData->broadcasting = FALSE;
                        }
                        else
                        {
                            leBrowserData->broadcasting = TRUE;
                        }
                    }
                    else if(leBrowserData->propertyToWrite == CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY)
                    {
                        if(leBrowserData->tempNotif)
                        {
                            leBrowserData->tempNotif = FALSE;
                        }
                        else 
                        {
                            leBrowserData->tempNotif = TRUE;
                        }
                        CsrBtLeBrowserAppSetDescriptor(leBrowserData, CSR_BT_GATT_CHARAC_PROPERTIES_NOTIFY);
                        leBrowserData->propertyToWrite = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
                    }
                    else if(leBrowserData->propertyToWrite == CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE)
                    {
                        if(leBrowserData->tempInd)
                        {
                            leBrowserData->tempInd = FALSE;
                        }
                        else 
                        {
                            leBrowserData->tempInd = TRUE;
                        }
                        CsrBtLeBrowserAppSetDescriptor(leBrowserData, CSR_BT_GATT_CHARAC_PROPERTIES_INDICATE);
                        leBrowserData->propertyToWrite = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
                    }
                    else if((leBrowserData->writeMethod & CSR_BT_GATT_CHARAC_PROPERTIES_WRITE) == CSR_BT_GATT_CHARAC_PROPERTIES_WRITE)
                    {
                        /* Updating local version of written value */
                        CsrSize tmpLen = CsrStrLen((CsrCharString*)leBrowserData->inputStr);
                        CsrPmemFree(leBrowserData->dbElement->currentCharac->value);
                        leBrowserData->dbElement->currentCharac->value = (CsrUint8*)CsrPmemAlloc(tmpLen + 1);
                        CsrMemCpy(leBrowserData->dbElement->currentCharac->value, leBrowserData->inputStr, tmpLen + 1);
                        leBrowserData->dbElement->currentCharac->valueLength = (CsrUint16)(tmpLen + 1);

                        /* Done writing */
                        leBrowserData->writeMethod = 0;
                        CsrPmemFree(leBrowserData->inputStr);
                    }
                    else if((leBrowserData->writeMethod & CSR_BT_LE_BROWSER_APP_RELIABLE_WRITE_METHOD) == CSR_BT_LE_BROWSER_APP_RELIABLE_WRITE_METHOD)
                    {
                        /* Updating local version of written value */
                        CsrSize tmpLen = CsrStrLen((CsrCharString*)leBrowserData->inputStr);
                        CsrPmemFree(leBrowserData->dbElement->currentCharac->value);
                        leBrowserData->dbElement->currentCharac->value = (CsrUint8*)CsrPmemZalloc(tmpLen + 1);
                        CsrMemCpy(leBrowserData->dbElement->currentCharac->value, leBrowserData->inputStr, tmpLen + 1);
                        leBrowserData->dbElement->currentCharac->valueLength = (CsrUint16)(tmpLen + 1);
                        
                        /* Done writing */
                        leBrowserData->writeMethod = 0;
                        CsrPmemFree(leBrowserData->inputStr);

                    }
                    else if((leBrowserData->writeMethod & CSR_BT_LE_BROWSER_APP_WRITE_AUX_METHOD) == CSR_BT_LE_BROWSER_APP_WRITE_AUX_METHOD)
                    {
                        CsrBtUuid currentUuid;
                        CsrBtGattAppDbCharacDescrElement *dElem;
                        CsrSize tmpLen = CsrStrLen((CsrCharString *) leBrowserData->inputStr);

                        CsrBtUuid16ToUuid(CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC, &currentUuid);
                        dElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(leBrowserData->dbElement->currentCharac->descrList,
                                                                             &currentUuid);

                        CsrPmemFree(dElem->value);
                        dElem->value = (CsrUint8 *) CsrPmemAlloc(tmpLen + 1);
                        CsrMemCpy(dElem->value, leBrowserData->inputStr, tmpLen + 1);
                        dElem->valueLength = (CsrUint16) (tmpLen + 1);
                        
                        /* Done writing */
                        leBrowserData->writeMethod = 0;
                        CsrPmemFree(leBrowserData->inputStr);
                    }
                    /*update UI*/
                    CsrBtGattAppHandleLeBrowserCsrUiWriteValue(leBrowserData,  TRUE);

                    CsrBtLeBrowserEditMenuHide(leBrowserData);
                }
                else
                {
                    if(leBrowserData->inputStr != NULL)
                    {
                        CsrPmemFree(leBrowserData->inputStr);
                    }
                    /* Done writing */
                    leBrowserData->writeMethod = 0;
                    /*update UI with error message*/
                    CsrBtGattAppHandleLeBrowserCsrUiWriteValue(leBrowserData, FALSE);
                }
                
                break;
            }
        case CSR_BT_GATT_READ_CFM:
            {
                CsrBtGattReadCfm *cfm;
                cfm = (CsrBtGattReadCfm*)leBrowserData->recvMsgP;

                if(cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
                {
                    /*store the values */
                    leBrowserData->dbElement->currentCharac->value       = cfm->value;
                    leBrowserData->dbElement->currentCharac->valueLength = cfm->valueLength;
   
                    CsrBtGattAppHandleLeBrowserCsrUiReadValue(leBrowserData, TRUE);

                    CsrBtLeBrowserPopUpHide(leBrowserData);
                    CsrBtLeBrowserEditMenuHide(leBrowserData);
                    CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(leBrowserData, CSR_BT_LE_BROWSER_CHARAC_DESCR_MENU_STATE);

                }
                else
                {

                    CsrBtGattAppHandleLeBrowserCsrUiReadValue(leBrowserData, FALSE);
                }
                break;
            }
        case CSR_BT_GATT_NOTIFICATION_IND :  
            {
                CsrBtGattAppDbCharacElement *cElem;
                CsrBtGattNotificationInd *ind            =  (CsrBtGattNotificationInd*)leBrowserData->recvMsgP;

                cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_VALUE_HANDLE(leBrowserData->dbElement->dbPrimServices, &ind->valueHandle);
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
 
                CsrBtGattAppHandleLeBrowserCsrUiMenuUpdate(leBrowserData, CSR_BT_GATT_NOTIFICATION_IND);

                break;
            }
        default:
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(leBrowserData->profileName), 
                                     TRUE, 
                                     "####### default in gatt prim handler 0x%04x,",
                                     *primType);
                CsrBtGattFreeUpstreamMessageContents(CSR_BT_GATT_PRIM, leBrowserData->recvMsgP);
                /* unexpected primitive received */
                break;
            }
    }
}

