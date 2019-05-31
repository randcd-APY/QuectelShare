/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#ifndef EXCLUDE_CSR_BT_THERM_SRV_MODULE

#include <string.h>
#include "csr_util.h"
#include "csr_pmem.h"
#include "csr_exceptionhandler.h"
#include "csr_app_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_gatt_demo_therm_server_app_private_prim.h"
#include "csr_bt_gatt_demo_therm_server_app_ui_sef.h"
#include "csr_bt_therm_srv_lib.h"
#include "csr_bt_therm_srv_prim.h"
#include "csr_bt_gatt_app_task.h"

/* Forward declarations for handler functions */
void CsrBtThermsAppHandleThermSrvPrim(CsrBtThermsAppInstData *thermsData);
void CsrBtThermsAppHandleCmPrim(CsrBtThermsAppInstData *thermsData);
void CsrBtThermsAppHandleSdPrim(CsrBtThermsAppInstData *thermsData);



/*used to simulate temperature changes */
int simplePseudoRand(int lim)
{
    static long a = 1; 
    a = (a * 32719 + 3) % 32749;
    return ((a % lim) + 1);
}

void CsrBtThermsAppUpdateTimerInterupt(CsrUint16 mi, void *mv)
{
    CsrBtThermsAppInstData *inst = (CsrBtThermsAppInstData*)mv;
    CsrUint8 *value = CsrPmemAlloc(5); /* 1xflags + 4xdata */
    float   fVal =(float)simplePseudoRand(100);
    value[0] = inst->tempType;
    CsrMemCpy(&value[1], &fVal, 4);

    CsrBtThermSrvUpdateTemperatureReqSend(value,5);
    inst->update_timer = CsrSchedTimerSet(CSR_BT_THERMS_APP_UPDATE_TIMER_INTERVAL,
                                          CsrBtThermsAppUpdateTimerInterupt,
                                          mi,
                                          mv);

}

/******************************************************************************
CsrBtThermsAppStartActivate:
*****************************************************************************/
void CsrBtThermsAppStartActivate(CsrBtThermsAppInstData *thermsData)
{
    CsrBtThermSrvActivateReqSend(thermsData->phandle, NULL, 0, 0xFFFF);
    CsrBtGattAppHandleThermsCsrUiActivateDeactivate(thermsData, 
                                                    TRUE, 
                                                    TRUE);
}
/******************************************************************************
CsrBtThermsAppStartDeactivating:
*****************************************************************************/
void CsrBtThermsAppStartDeactivate(CsrBtThermsAppInstData *thermsData)
{
    CsrBtThermSrvDeactivateReqSend();

}



/******************************************************************************
CsrBtThermsAppStartDisconnect:
*****************************************************************************/
void CsrBtThermsAppStartDisconnect(CsrBtThermsAppInstData *thermsData)
{
    /*stop update timer*/
    CsrSchedTimerCancel(thermsData->update_timer,NULL,NULL);

    /*we actually deactivate as the server will keep being reconnectable as long as it is activated */
    CsrBtThermSrvDeactivateReqSend();
    /*reset BT connection ID */
    thermsData->btConnId = 0xff;
}

/******************************************************************************
CsrBtThermsAppInitInstanceData:
*****************************************************************************/
void CsrBtThermsAppClearHandles(CsrBtThermsAppInstData *thermsData)
{
    ;
}

void CsrBtThermsAppInitInstanceData(CsrBtThermsAppInstData *thermsData)
{
    CsrMemSet(thermsData,0,sizeof(CsrBtThermsAppInstData));

    thermsData->btConnId                          = 0xff;
    thermsData->csrUiVar.hMainMenu                = CSR_UI_DEFAULTACTION;
    thermsData->csrUiVar.hCommonDialog            = CSR_UI_DEFAULTACTION;
    thermsData->tempType                          = CSR_BT_THERMS_APP_TEMP_TYPE_CELSIUS;
    CsrBtThermsAppClearHandles(thermsData);
}

static void csrBtThermsAppSendThermsAppRegisterReq(CsrSchedQid pHandle)
{
    CsrBtThermsAppRegisterReq *prim;

    prim = (CsrBtThermsAppRegisterReq *) CsrPmemAlloc(sizeof(CsrBtThermsAppRegisterReq));
    prim->type = CSR_BT_THERMS_APP_REGISTER_REQ;
    prim->phandle = pHandle;

    CsrSchedMessagePut(CSR_BT_THERMS_APP_IFACEQUEUE, CSR_BT_THERMS_APP_PRIM,prim);
}

static void csrBtThermsAppSendThermsAppRegisterCfm(CsrSchedQid phandle, CsrSchedQid queueId, CsrUint8 instance)
{
    CsrBtThermsAppRegisterCfm *prim;

    prim = (CsrBtThermsAppRegisterCfm *) CsrPmemAlloc(sizeof(CsrBtThermsAppRegisterCfm));
    prim->type = CSR_BT_THERMS_APP_REGISTER_CFM;
    prim->queueId = queueId;
    prim->instance = instance;

    CsrSchedMessagePut(phandle, CSR_BT_THERMS_APP_PRIM,prim);
}

static char * csrBtThermsAppReturnThermsInstName(CsrBtThermsAppInstData *instance)
{
    char *buf;
    CsrUint32 bufSize = CsrStrLen("Thermometer Srv")+1;
    buf = CsrPmemAlloc(bufSize);

    snprintf(buf, bufSize, "Thermometer Srv");
    return buf;
}

/******************************************************************************
* Init function called by the scheduler upon initialisation.
* This function is used to boot the demo application.
*****************************************************************************/
void CsrBtThermsAppInit(void **gash)
{
    CsrBtThermsAppInstData    *thermsData;

    /* allocate and initialise instance data space */
    *gash = (void *) CsrPmemAlloc(sizeof(CsrBtThermsAppInstData));
    thermsData = (CsrBtThermsAppInstData *) *gash;
    CsrBtThermsAppInitInstanceData(thermsData);

    thermsData->phandle = CsrSchedTaskQueueGet() ;

    thermsData->initialized = TRUE;
    thermsData->showDisconnect = TRUE;
    
    /* Start UI creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

    csrBtThermsAppSendThermsAppRegisterReq(thermsData->phandle);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtThermsAppDeinit(void **gash)
{
    CsrUint16 msg_type;
    void *msg_data;
    CsrBtThermsAppInstData *thermsData;

    thermsData = (CsrBtThermsAppInstData *) (*gash);
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

    CsrPmemFree(thermsData);
}
#else
void CsrBtThermsAppDeinit(void **gash)
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
void CsrBtThermsAppHandler(void **gash)
{
    CsrBtThermsAppInstData *thermsData;
    CsrUint16    eventType;

    /* get a message from the demoapplication message queue. The message is returned in prim
    and the event type in eventType */
    thermsData = (CsrBtThermsAppInstData *) (*gash);

    if(!thermsData->restoreFlag)
    { /* No messages on the local save queue                                                */
        CsrSchedMessageGet(&eventType , &thermsData->recvMsgP);
    }
    else
    {
        if(!CsrMessageQueuePop(&thermsData->saveQueue, &eventType , &thermsData->recvMsgP))
        { /* No more messages on the local save queue. Call CsrGetMessage to receive
          a message from the scheduler                                                   */
            thermsData->restoreFlag = FALSE;
            CsrSchedMessageGet(&eventType , &thermsData->recvMsgP);
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
            CsrBtThermsAppHandleSdPrim(thermsData);
            break;
        }
        case CSR_BT_THERM_SRV_PRIM:
        {
            CsrBtThermsAppHandleThermSrvPrim(thermsData);
            break;
        }

        case CSR_BT_THERMS_APP_PRIM:
        {
            CsrPrim *type = (CsrPrim *) thermsData->recvMsgP;

            switch(*type)
            {
            case CSR_BT_THERMS_APP_REGISTER_REQ:
                { /* only received by controling therms demo app instance */
                    if(thermsData->initialized)
                    {
                        CsrBtThermsAppRegisterReq *prim = thermsData->recvMsgP;
                        CsrSchedQid queueId = CSR_SCHED_QID_INVALID;

                        thermsData->thermsAppHandle = prim->phandle;
                        queueId = thermsData->thermsProfileHandle;
                        if(queueId != CSR_SCHED_QID_INVALID)
                        {
                            csrBtThermsAppSendThermsAppRegisterCfm(prim->phandle, queueId,1);
                        }
                        else
                        {
                            CsrGeneralException("CSR_THERMS_DEMO_APP", 
                                0, 
                                *type,
                                "No CsrBtTherms profile Queue Handle Available");
                        }
                    }
                    else
                    {
                        CsrMessageQueuePush(&thermsData->saveQueue, CSR_BT_THERMS_APP_PRIM, thermsData->recvMsgP);
                        thermsData->recvMsgP = NULL;
                    }
                    break;
                }
            case CSR_BT_THERMS_APP_REGISTER_CFM:
                { /* Only received by extra therms demo app instances */
                    thermsData->profileName = csrBtThermsAppReturnThermsInstName(thermsData);

                    CsrBtGattAppHandleThermsSetMainMenuHeader(thermsData);

                    thermsData->initialized = TRUE;

                    CsrAppRegisterReqSend(thermsData->phandle, TECH_BTLE, PROFILE_NAME(thermsData->profileName));
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
            CsrPrim *prim = (CsrPrim *) thermsData->recvMsgP;
            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *prim = thermsData->recvMsgP;
                thermsData->selectedDeviceAddr.addr.lap = prim->deviceAddr.lap;
                thermsData->selectedDeviceAddr.addr.nap = prim->deviceAddr.nap;
                thermsData->selectedDeviceAddr.addr.uap = prim->deviceAddr.uap;
                thermsData->selectedDeviceAddr.type     = (prim->deviceStatus & CSR_BT_SD_STATUS_PRIVATE_ADDR)? TBDADDR_RANDOM:TBDADDR_PUBLIC;
                CsrAppBacklogReqSend(TECH_BTLE, 
                                    PROFILE_NAME(thermsData->profileName), 
                                    FALSE, 
                                    "Received selected DeviceAddr: %04X:%02X:%06X (%s)", 
                                    prim->deviceAddr.nap, 
                                    prim->deviceAddr.uap, 
                                    prim->deviceAddr.lap,
                                    (CsrBtAddrIsRandom(thermsData->selectedDeviceAddr))? "Random":"Public");
                /*update UI*/
                CsrUiUieShowReqSend(thermsData->csrUiVar.hMainMenu,
                                    CsrSchedTaskQueueGet(),
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_THERMS_APP_CSR_UI_PRI);
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_THERMS_APP", 
                                    0, 
                                    *prim, 
                                    "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtThermsAppHandler,");
            }
            break;
        }
        case CSR_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) thermsData->recvMsgP;

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
                CsrGeneralException("CSR_BT_THERMS_APP", 
                                    0, 
                                    *prim, 
                                    "####### Unhandled CSR_APP_PRIM in CsrBtThermsAppHandler,");
            }
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrBtGattAppHandleThermsCsrUiPrim(thermsData);
            break;
        }

        default:
        {
            /* unexpected primitive received */
            CsrGeneralException("CSR_BT_THERMS_APP",
                                0, 
                                *((CsrPrim *) thermsData->recvMsgP),
                                "####### default in CsrBtThermsAppHandler,");
        }
    }
    /* free the received signal again. If the signal for some reason must be stored in the application
    * the pointer (prim) must be set to NULL in order not to free it here
    */
    CsrPmemFree(thermsData->recvMsgP);
}
/******************************************************************************
* Function to handle all SD-primitives.
******************************************************************************/
void CsrBtThermsAppHandleSdPrim(CsrBtThermsAppInstData *thermsData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) thermsData->recvMsgP;
    switch (*primType)
    {
        default:
        {
            CsrAppBacklogReqSend(TECH_BTLE, 
                                PROFILE_NAME(thermsData->profileName), 
                                TRUE, 
                                "####### default in SD prim handler 0x%04x,",
                                *primType);
            CsrBtGattFreeUpstreamMessageContents(CSR_BT_SD_PRIM, thermsData->recvMsgP);
            /* unexpected primitive received */

        }


    }
}


/******************************************************************************
* Function to handle all therms-primitives.
******************************************************************************/
void CsrBtThermsAppHandleThermSrvPrim(CsrBtThermsAppInstData *thermsData)
{
    CsrPrim *primType;

    primType = (CsrPrim *) thermsData->recvMsgP;
    switch (*primType)
    {
        case CSR_BT_THERM_SRV_DEACTIVATE_CFM:
        {
            CsrBtGattAppHandleThermsCsrUiActivateDeactivate(thermsData, FALSE,TRUE);
            break;
        }
        case CSR_BT_THERM_SRV_CONNECT_IND:
        {
            CsrBtThermSrvConnectInd        *ind;

            ind = (CsrBtThermSrvConnectInd *) thermsData->recvMsgP;

            if (ind->resultSupplier == CSR_BT_SUPPLIER_THERM_SRV &&
                ind->resultCode == CSR_BT_THERM_SRV_RESULT_SUCCESS)
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                    PROFILE_NAME(thermsData->profileName), 
                                    TRUE, 
                                    "Successfully Connected with: %04X:%02X:%06X", 
                                    ind->deviceAddr.addr.nap, 
                                    ind->deviceAddr.addr.uap, 
                                    ind->deviceAddr.addr.lap);
                thermsData->connectedDeviceAddr.addr.nap = ind->deviceAddr.addr.nap;
                thermsData->connectedDeviceAddr.addr.lap = ind->deviceAddr.addr.lap;
                thermsData->connectedDeviceAddr.addr.uap = ind->deviceAddr.addr.uap;
                thermsData->update_timer = CsrSchedTimerSet(CSR_BT_THERMS_APP_UPDATE_TIMER_INTERVAL,
                                                            CsrBtThermsAppUpdateTimerInterupt,
                                                            0,
                                                            (void*)thermsData);

            }
            else
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                    PROFILE_NAME(thermsData->profileName), 
                                    TRUE, 
                                    "Failed to connect with: %04X:%02X:%06X, resultCode=%d, resultSupplier=%d", 
                                    ind->deviceAddr.addr.nap, 
                                    ind->deviceAddr.addr.uap, 
                                    ind->deviceAddr.addr.lap, 
                                    ind->resultCode, 
                                    ind->resultSupplier);
            }
            /*update UI*/
            thermsData->csrUiVar.localInitiatedAction = TRUE;
            CsrBtGattAppHandleThermsCsrUiActivateDeactivate(thermsData, 
                                                            TRUE,
                                                            (CsrBool)((ind->resultCode == CSR_BT_THERM_SRV_RESULT_SUCCESS && 
                                                            ind->resultSupplier == CSR_BT_SUPPLIER_THERM_SRV) ? TRUE : FALSE));
            break;
        }
        case CSR_BT_THERM_SRV_DISCONNECT_IND:
        {
            CsrBtThermSrvDisconnectInd    *ind;
            ind      = (CsrBtThermSrvDisconnectInd *) thermsData->recvMsgP;

            if(thermsData->showDisconnect)
            {
                CsrAppBacklogReqSend(TECH_BTLE, 
                                     PROFILE_NAME(thermsData->profileName), 
                                     TRUE, 
                                     "Disconnected from: %04X:%02X:%06X", 
                                     ind->deviceAddr.addr.nap, 
                                     ind->deviceAddr.addr.uap, 
                                     ind->deviceAddr.addr.lap);
            }
            /*reset the btConnId */
            thermsData->btConnId = 0xff;
            /*Stop the UPDATE TIMER */
            CsrSchedTimerCancel(thermsData->update_timer,NULL,NULL);
            /*Clear all handles in instance */
            CsrBtThermsAppClearHandles(thermsData);

            /*update UI*/
            /*we are still activated as this was initiated by peer and server goes back into advertising state */
            
            if(thermsData->showDisconnect)
            {
                CsrBtGattAppHandleThermsCsrUiActivateDeactivate(thermsData, 
                                                                TRUE,
                                                                TRUE);
            }
            else
            {
                thermsData->showDisconnect = TRUE;
            }            

            break;
        }
        case CSR_BT_THERM_SRV_WRITE_EVENT_IND:
        {
            /*CsrBtThermSrvWriteEventInd *ind = (CsrBtThermSrvWriteEventInd*)thermsData->recvMsgP;
            CsrUint16 handle =  ind->valueHandle - thermsData->dbStartHandle; */

            break;

        }
        default:
        {
            CsrAppBacklogReqSend(TECH_BTLE, 
                                 PROFILE_NAME(thermsData->profileName), 
                                 TRUE, 
                                 "####### default in gatt prim handler 0x%04x,",
                                 *primType);
            CsrBtGattFreeUpstreamMessageContents(CSR_BT_GATT_PRIM, thermsData->recvMsgP);
            /* unexpected primitive received */
        }
    }
}
#endif
