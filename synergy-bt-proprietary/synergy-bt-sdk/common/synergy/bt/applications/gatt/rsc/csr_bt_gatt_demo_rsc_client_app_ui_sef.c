/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_rsc_client_app.h"
#include "csr_bt_gatt_demo_rsc_client_app_strings.h"
#include "csr_bt_gatt_demo_rsc_utils.h"
#include "csr_bt_gatt_demo_defines.h" 

#define CSR_BT_RSCC_FEATURE_ISLM_MASK       0x01
#define CSR_BT_RSCC_FEATURE_TDM_MASK        0x02
#define CSR_BT_RSCC_FEATURE_W_R_MASK        0x04
#define CSR_BT_RSCC_FEATURE_CP_MASK         0x08
#define CSR_BT_RSCC_FEATURE_MSL_MASK        0x10

#define CSR_BT_RSCC_SENSOR_LOCATION_MAX_LENGTH  8
static char *rsc_sensor_location[]={"Other","On Top","In Shoe","On Hip"};

#define CSR_BT_RSC_OTHER    0x00
#define CSR_BT_RSC_TOP      0x01
#define CSR_BT_RSC_SHOE     0x02
#define CSR_BT_RSC_HIP      0x03

/* Helper functions */
CsrDouble rscSignedPow(CsrInt8 base, CsrInt8 exp)
{
    if (exp == 0)
    {
        return 1;
    }
    else
    {
        if (exp > 0)
        {
            return (CsrDouble)(rscSignedPow(base, (CsrInt8)(exp-1)) * base);
        }
        else
        {
            return (CsrDouble)(rscSignedPow(base, (CsrInt8)(exp+1)) / base);
        }
    }
}

CsrUtf16String *commonRsccGetValueString(CsrBtRsccAppInstData *inst, CsrUint16 uuid)
{
    CsrBtGattAppDbPrimServiceElement *pElem;
    char *ret              =  NULL;
    CsrUtf16String *ret16  =  NULL;

    pElem = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(inst->dbPrimServices,CsrBtRsccAppUuid16ToUuid128(CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE));

    if(pElem != NULL)
    {
        switch(uuid)
        {
            case CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC:
            {
                CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                            CsrBtRsccAppUuid16ToUuid128(uuid));
                if(cElem != NULL && cElem->value != NULL)
                {
                    CsrBool running;
                    CsrBool strideAvail;
                    CsrBool distanceAvail;
                    CsrFloat finalSpeed;
                    CsrUint16 speed;    /* 1/256 m/s */
                    CsrUint8 cadance;   /* 1/min (RPM) */
                    CsrUint16 stride;   /* 1/100 m (or centimeter) */
                    CsrUint32 distance; /* 1/10 m (or decimeter) */
                    CsrUint32 retSize = 256;

                    ret = CsrPmemZalloc(retSize);
                    CsrBtRscAppGetMeasurementValues(cElem->value, 
                                                    &running,  
                                                    &strideAvail, 
                                                    &distanceAvail, 
                                                    &speed, 
                                                    &cadance, 
                                                    &stride, 
                                                    &distance);                                       
                    finalSpeed = (CsrFloat)((CsrFloat)speed / 256);

                    if(!strideAvail && !distanceAvail)
                    {
                        snprintf(ret, retSize, "%s: %.03f m/s, %d RPM", (running ? "Run" : "Walk"), finalSpeed, cadance);
                    }
                    else if(strideAvail && !distanceAvail)
                    {
                        snprintf(ret, retSize, "%s: %.03f m/s, %d RPM, %d cm", (running ? "Run" : "Walk"), finalSpeed, cadance, stride);
                    }
                    else if(!strideAvail && distanceAvail)
                    {
                        snprintf(ret, retSize, "%s: %.03f m/s, %d RPM, %u dm", (running ? "Run" : "Walk"), finalSpeed, cadance, distance);
                    }
                    else /* All */
                    {
                        snprintf(ret, retSize, "%s: %.03f m/s, %d RPM, %d cm, %u dm", (running ? "Run" : "Walk"), finalSpeed, cadance, stride, distance);
                    }                    
                }
                else 
                {
                    ret = (char*)CsrStrDup("Not started yet");
                }

                break;
            }
            case CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC:
            {
                CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                            CsrBtRsccAppUuid16ToUuid128(uuid));
                if(cElem != NULL && (cElem->value != NULL))
                {
                    CsrUint16 tmpFeatureMask = (CsrUint16)(cElem->value[0] | (cElem->value[1] << 8));
                    CsrUint32 retSize = 20;
                    ret = CsrPmemZalloc(retSize);
                    snprintf(ret, retSize, "Found: 0x%04x", (tmpFeatureMask & 0x001F));
                }
                else 
                {
                    ret = (char*)CsrStrDup("Not found yet");
                }

                break;
            }

            case CSR_BT_GATT_UUID_SENSOR_LOCATION_CHARAC:
            {
                CsrBtGattAppDbCharacElement *cElem = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList, 
                                                                                            CsrBtRsccAppUuid16ToUuid128(uuid));
                if(cElem != NULL && cElem->value != NULL)
                {
                    CsrUint32 retSize = CSR_BT_RSCC_SENSOR_LOCATION_MAX_LENGTH;
                    ret = CsrPmemZalloc(retSize);
                    if(cElem->value[0] <= CSR_BT_RSC_HIP)
                    {
                        snprintf(ret, retSize, "%s", CsrStrDup(rsc_sensor_location[(CsrUint8)cElem->value[0]]));
                    }
                    else
                    {
                        ret = (char*)CsrStrDup("Not RSC location");
                    }
                }
                else 
                {
                    ret = (char*)CsrStrDup("Not found yet");
                }

                break;
            }
            default:
            {
                ret = (char*)CsrStrDup("Unknown UUID");
                break;
            }
        }
    }

    if (ret == NULL)
    {
        if(inst->traversingDb)
        {
            ret = (char*)CsrStrDup("Loading...");
        }
        else 
        {
            ret = (char*)CsrStrDup("Not Found");
        }
    }
    else
    {
        /* Do nothing */
    }

    ret16 = CsrUtf82Utf16String((CsrUtf8String *)ret);
    CsrPmemFree(ret);
    return ret16;
}

void commonRsccPopupSet(CsrBtRsccAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          NULL, NULL, 0,0,0,0);
}

void commonRsccAcceptPopupSet(CsrBtRsccAppInstData *inst, char *dialogHeading, char *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hCommonDialog, 
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading), 
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_RSCC_APP_OK_UCS2,
                          NULL, 
                          inst->csrUiVar.hBackEvent,
                          0,
                          inst->csrUiVar.hBackEvent,
                          0);
}

void commonRsccPopupShow(CsrBtRsccAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog,
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_BLOCK, 
                        CSR_BT_RSCC_APP_CSR_UI_PRI);
}

void commonRsccAcceptPopupShow(CsrBtRsccAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hCommonDialog, 
                        CsrSchedTaskQueueGet(), 
                        CSR_UI_INPUTMODE_AUTO, 
                        CSR_BT_RSCC_APP_CSR_UI_PRI);
}
void commonRsccInputPopupSet(CsrBtRsccAppInstData *inst, char *dialogMessage, CsrUint8 keyMap)
{
    CsrUint8* dialogInputText = NULL;

    dialogInputText = '\0';
    CsrUiInputdialogSetReqSend(inst->csrUiVar.hInputDialog,
                               CONVERT_TEXT_STRING_2_UCS2("Edit Value:"),
                               CONVERT_TEXT_STRING_2_UCS2(dialogMessage),
                               CSR_UI_ICON_EMPTY,
                               CONVERT_TEXT_STRING_2_UCS2(dialogInputText),
                               CSR_BT_RSCC_APP_MAX_INPUT_TEXT_LENGTH,
                               keyMap,
                               TEXT_RSCC_APP_SEND_INPUT_UCS2,
                               TEXT_RSCC_APP_CANCEL_INPUT_UCS2,
                               inst->csrUiVar.hInputSk1Event,
                               inst->csrUiVar.hBackEvent,
                               inst->csrUiVar.hBackEvent,
                               0);
}

void commonRsccInputPopupShow(CsrBtRsccAppInstData *inst)
{
    CsrUiUieShowReqSend(inst->csrUiVar.hInputDialog,
                        CsrSchedTaskQueueGet(),
                        CSR_UI_INPUTMODE_AUTO,
                        CSR_BT_RSCC_APP_CSR_UI_PRI);
}

void commonRsccInputpopupHide(CsrBtRsccAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hInputDialog);
}

void commonRsccPopupHide(CsrBtRsccAppInstData *inst)
{
    CsrUiUieHideReqSend(inst->csrUiVar.hCommonDialog);
}

void CsrBtGattAppHandleRsccCsrUiRegisterUnregister(CsrBtRsccAppInstData *inst, CsrBool registered)
{
    CsrBtRsccAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(registered)
    {
        /* Update main menu so it is no longer possible to activate but just to deactivate */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_FIRST, 
                                CSR_BT_RSCC_APP_CONNECT_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCC_APP_CONNECT_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCC_APP_CONNECT_BREDR_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCC_APP_CONNECT_BREDR_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCC_APP_UNREGISTER_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCC_APP_UNREGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        
        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_RSCC_APP_REGISTER_KEY);
    }
    else
    {
        /* Update main menu so it is no longer possible to unRegister but just to register */
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCC_APP_REGISTER_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCC_APP_REGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);

        CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                   CSR_BT_RSCC_APP_UNREGISTER_KEY);
        if(inst->btConnId == 0xff)
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_RSCC_APP_CONNECT_KEY);
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_RSCC_APP_CONNECT_BREDR_KEY);
        }
        else
        {
            CsrUiMenuRemoveitemReqSend(csrUiVar->hMainMenu, 
                                       CSR_BT_RSCC_APP_DISCONNECT_KEY);
        }
    }
    /* Remove popup */
    commonRsccPopupHide(inst);
}
void CsrBtGattAppHandleRsccCsrUiTraversingdb(CsrBtRsccAppInstData *inst, CsrBool traversing)
{
    if(traversing)
    {
        /*show popup to block screen while traversing remote database*/
        commonRsccPopupSet(inst,"Updating","Please wait while traversing the remote database...");
        commonRsccPopupShow(inst);
    }else
    {
        commonRsccPopupHide(inst);
    }

}
void CsrBtGattAppHandleRsccCsrUiMenuUpdate(CsrBtRsccAppInstData *inst)
{
    /*to prevent a panic in UI task */
    if(inst->btConnId != 0xff)
    {
        char ret[12];
        CsrBtRsccAppUiInstData *csrUiVar = &inst->csrUiVar;

        CsrMemSet(ret,0,12);
        snprintf(ret, sizeof(ret), "Update: %s", (inst->rscNotif==TRUE)?"On ":"Off");
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_RSCC_APP_WRITE_NOTIFICATION_KEY,
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_RSCC_APP_TOGGLE_MEASUREMENT_UCS2,
                                CsrUtf82Utf16String((CsrUtf8String *)ret),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_RSCC_APP_MEASUREMENT_INFO_KEY,
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_RSCC_APP_MEASUREMENT_INFO_UCS2,
                                commonRsccGetValueString(inst, CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_RSCC_APP_FEATURE_INFO_KEY,
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_RSCC_APP_FEATURE_INFO_UCS2,
                                commonRsccGetValueString(inst, CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_RSCC_APP_SENSOR_LOCATION_KEY,
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_RSCC_APP_SENSOR_LOCATION_UCS2,
                                commonRsccGetValueString(inst, CSR_BT_GATT_UUID_SENSOR_LOCATION_CHARAC),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);

        CsrMemSet(ret,0,12);
        snprintf(ret, sizeof(ret), "Update: %s", (inst->rscInd==TRUE)?"On ":"Off");
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_RSCC_APP_WRITE_INDICATION_KEY,
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_RSCC_APP_TOGGLE_INDICATION_UCS2,
                                CsrUtf82Utf16String((CsrUtf8String *)ret),
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        
        CsrUiMenuSetitemReqSend(csrUiVar->hMainMenu, 
                                CSR_BT_RSCC_APP_SC_CONTROL_POINT_KEY,
                                CSR_UI_ICON_MARK_INFORMATION, 
                                TEXT_RSCC_APP_SC_CONTROL_POINT_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        
    }

}

void CsrBtGattAppHandleRsccCsrUiNoSelectedDevAddr(CsrBtRsccAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonRsccPopupHide(inst);
    commonRsccAcceptPopupSet(inst, "Connecting", "No deviceAddr selected in GAP");
    commonRsccAcceptPopupShow(inst);
}

void CsrBtGattAppHandleRsccCsrUiStillTraversingDb(CsrBtRsccAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonRsccPopupHide(inst);
    commonRsccAcceptPopupSet(inst, "Warning", "Client is still traversing the remote database. please wait a few seconds and try again!");
    commonRsccAcceptPopupShow(inst);
}

void CsrBtGattAppHandleRsccCsrUiReadValue(CsrBtRsccAppInstData *inst, char *str, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonRsccPopupHide(inst);
        CsrBtGattAppHandleRsccCsrUiMenuUpdate(inst);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonRsccPopupHide(inst);
        commonRsccAcceptPopupSet(inst, "Reading", "Could not read value");
        commonRsccAcceptPopupShow(inst);
        CsrBtGattAppHandleRsccCsrUiMenuUpdate(inst);
    }
}
void CsrBtGattAppHandleRsccCsrUiWriteValue(CsrBtRsccAppInstData *inst, CsrBool success)
{
    if(success)
    {
        /*need to remove previous popup in order to show the one with ok button */
        commonRsccPopupHide(inst);
        CsrBtGattAppHandleRsccCsrUiMenuUpdate(inst);
    }
    else
    {
        /*need to remove previous popup in order to show the one with ok button*/
        commonRsccPopupHide(inst);
        commonRsccAcceptPopupSet(inst, "Writing", "Could not set value");
        commonRsccAcceptPopupShow(inst);
        CsrBtGattAppHandleRsccCsrUiMenuUpdate(inst);
    }
}

void CsrBtGattAppHandleRsccCsrUiWriteError81(CsrBtRsccAppInstData *inst)
{
    /*need to remove previous popup in order to show the one with ok button*/
    commonRsccPopupHide(inst);
    commonRsccAcceptPopupSet(inst, "Writing", "Improperly configured");
    commonRsccAcceptPopupShow(inst);
    CsrBtGattAppHandleRsccCsrUiMenuUpdate(inst);
}



void CsrBtGattAppHandleRsccCsrUiShowFeatureInfo(CsrBtRsccAppInstData *inst)
{
    CsrBtGattAppDbPrimServiceElement *pElem;
    CsrBtGattAppDbCharacElement *cElem;
    
    pElem  = CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(inst->dbPrimServices,CsrBtRsccAppUuid16ToUuid128(CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE));
         
    cElem  = CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(pElem->characList,CsrBtRsccAppUuid16ToUuid128(CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC));
    
    if((cElem != NULL) && (cElem->value != NULL))
    {
        CsrUint32 retSize = 350;
        char *ret = CsrPmemZalloc(retSize);
        
        snprintf(ret, retSize, "Features supported.\n Instantaneous Stride Length: %s \n Total Distance: %s \n W/R status: %s \n Calibration Procedure: %s \n Multiple Sensor Locations: %s",
                      (cElem->value[1] & CSR_BT_RSCC_FEATURE_ISLM_MASK) ?  "Supported" : "Not Supported",
                      (cElem->value[1] & CSR_BT_RSCC_FEATURE_TDM_MASK) ?  "Supported" : "Not Supported",
                      (cElem->value[1] & CSR_BT_RSCC_FEATURE_W_R_MASK) ?  "Supported" : "Not Supported",
                      (cElem->value[1] & CSR_BT_RSCC_FEATURE_CP_MASK) ?  "Supported" : "Not Supported",
                      (cElem->value[1] & CSR_BT_RSCC_FEATURE_MSL_MASK) ?  "Supported" : "Not Supported");
        commonRsccPopupHide(inst);
        commonRsccAcceptPopupSet(inst, "Feature Info", ret);
        commonRsccAcceptPopupShow(inst);
        CsrBtGattAppHandleRsccCsrUiMenuUpdate(inst);
    }       

}

void CsrBtGattAppHandleRsccCsrUiShowIndInfo(CsrBtRsccAppInstData *inst, CsrUint16 valueLength, CsrUint8* value)
{
    CsrUint8 tmpLenght = 0;
    char *tmpString = NULL;
    CsrUint32 tmpStringSize = 0;
    CsrUint8 i;
    if((valueLength - 3) > 0 )
    {
        tmpLenght = (CsrUint8)((CSR_BT_RSCC_SENSOR_LOCATION_MAX_LENGTH+2) * (valueLength - 3));
    }
    tmpStringSize = 50 + valueLength + tmpLenght;
    tmpString = CsrPmemZalloc(tmpStringSize); 
    
    /* Indication response */      
    if ( value[1] == CSR_BT_RSCC_WRITE_OPCODE_RSSL && tmpLenght)
    {
        char *theRest = CsrPmemZalloc(tmpLenght);
        /*start from data*/
        for(i = 3; i < (valueLength); i++)
        {
            CsrStrLCat(theRest, CsrStrDup(rsc_sensor_location[value[i]]), tmpLenght);
            CsrStrLCat(theRest, CsrStrDup(", "), tmpLenght);
        }
         
        snprintf(tmpString, tmpStringSize, "OpCode: %x\n Request: %x\n Response: %x\n Parameter: %s", value[0],value[1],value[2],theRest);
    }
    else
    {
        snprintf(tmpString, tmpStringSize, "OpCode: %x\n Request: %x\n Response: %x\n ", value[0],value[1],value[2]);
    }
        
    commonRsccPopupHide(inst);
    commonRsccAcceptPopupSet(inst, "SC Control Point Ind", tmpString);
    commonRsccAcceptPopupShow(inst);
        
}

void CsrBtGattAppHandleRsccCsrUiConnectDisconnect(CsrBtRsccAppInstData *inst,
                                                   CsrBool connected, 
                                                   CsrBool success)
{
    CsrBtRsccAppUiInstData *csrUiVar = &inst->csrUiVar;

    if(connected)
    {
        if(success)
        {
            CsrUint32 addrSize = 15;
            char *addr = CsrPmemAlloc(addrSize);
            char ret[12];
            CsrMemSet(ret,0,12);
            snprintf(addr, addrSize, "%04X:%02X:%06X", inst->selectedDeviceAddr.addr.nap, inst->selectedDeviceAddr.addr.uap, inst->selectedDeviceAddr.addr.lap);
            /* Update main menu so it is no longer possible to connect and activate but just to disconnect and CsrBtRsccAppSend file */
            CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_FIRST, 
                                    CSR_BT_RSCC_APP_DISCONNECT_KEY,
                                    CSR_UI_ICON_DISCONNECT, 
                                    TEXT_RSCC_APP_DISCONNECT_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *) addr),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
    
            snprintf(ret, sizeof(ret), "Update: %s", (inst->rscNotif==TRUE)?"On ":"Off");
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_RSCC_APP_WRITE_NOTIFICATION_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_RSCC_APP_TOGGLE_MEASUREMENT_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *)ret),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_RSCC_APP_MEASUREMENT_INFO_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_RSCC_APP_MEASUREMENT_INFO_UCS2,
                                    commonRsccGetValueString(inst, CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCC_APP_FEATURE_INFO_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_RSCC_APP_FEATURE_INFO_UCS2,
                                    commonRsccGetValueString(inst, CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCC_APP_SENSOR_LOCATION_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_RSCC_APP_SENSOR_LOCATION_UCS2,
                                    commonRsccGetValueString(inst, CSR_BT_GATT_UUID_SENSOR_LOCATION_CHARAC),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            snprintf(ret, sizeof(ret), "Update: %s", (inst->rscInd==TRUE)?"On ":"Off");
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                    CSR_UI_LAST, 
                                    CSR_BT_RSCC_APP_WRITE_INDICATION_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_RSCC_APP_TOGGLE_INDICATION_UCS2,
                                    CsrUtf82Utf16String((CsrUtf8String *)ret),
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCC_APP_SC_CONTROL_POINT_KEY,
                                    CSR_UI_ICON_MARK_INFORMATION, 
                                    TEXT_RSCC_APP_SC_CONTROL_POINT_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);


        }
    }
    else
    {
        /* Update main menu so it is no longer possible to disconnect but just to connect and activate */
        CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMainMenu);

        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCC_APP_CONNECT_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCC_APP_CONNECT_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCC_APP_CONNECT_BREDR_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCC_APP_CONNECT_BREDR_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
        CsrUiMenuAdditemReqSend(csrUiVar->hMainMenu, 
                                CSR_UI_LAST, 
                                CSR_BT_RSCC_APP_UNREGISTER_KEY,
                                CSR_UI_ICON_NONE, 
                                TEXT_RSCC_APP_UNREGISTER_UCS2,
                                NULL,
                                csrUiVar->hSk1Event, 
                                csrUiVar->hSk2Event, 
                                csrUiVar->hBackEvent, 
                                0);
    }

    if(csrUiVar->localInitiatedAction)
    {
        csrUiVar->localInitiatedAction = FALSE;

        /* Remove popup */
        commonRsccPopupHide(inst);

        CsrUiDisplaySetinputmodeReqSend(csrUiVar->hMainMenu, CSR_UI_INPUTMODE_AUTO);
    }
}



void CsrBtGattAppHandleRsccSetMainMenuHeader(CsrBtRsccAppInstData *inst)
{
    if (inst->csrUiVar.hMainMenu)
    { /* If the Main menu generation completed before we got the profile name we set it here */
        CsrUiMenuSetReqSend(inst->csrUiVar.hMainMenu, 
                            CONVERT_TEXT_STRING_2_UCS2("RSC Client"),
                            TEXT_SELECT_UCS2,
                            NULL);
    }
}


static void handleCsrUiInputDialogGetCfm(CsrBtRsccAppInstData *inst, CsrUiInputdialogGetCfm *confirm)
{
    CsrUtf8String* inputStr = CsrUtf16String2Utf8(confirm->text);

    CsrBtRsccAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUint16 newValueLength = 0;
    if(inputStr != NULL)
    {
        CsrSize inputStrLen = CsrStrLen((CsrCharString*)inputStr);
        CsrUint8* value2Send = (CsrUint8*)CsrPmemAlloc(inputStrLen+1);
        CsrUint16 i;
        CsrUint8 temp = 0x00;

        for(i = 0; i < inputStrLen; i++)
        {

            if((inputStr[i] >= '0') && (inputStr[i] <= '9'))
            {
                temp += (CsrUint8)(inputStr[i] - '0');
            }
            else if((inputStr[i] >= 'A') && (inputStr[i] <= 'F'))
            {
                temp += (CsrUint8)(inputStr[i] - 'A' + 10);
            }
            else if((inputStr[i] >= 'a') && (inputStr[i] <= 'f'))
            {
                temp += (CsrUint8)(inputStr[i] - 'a' + 10);
            }
            else
            {
                /* Illigal hex-value, must return */
                CsrBtGattAppHandleRsccCsrUiWriteValue(inst, FALSE);
            }

            if(i%2 == 1)
            {
                *(value2Send + newValueLength) = temp;
                temp = 0x00;
                newValueLength++;
            }
            else
            {
                temp = temp << 4;
            }

        }
            /* End of raw data input */

        CsrBtRsccAppStartWriteValue(inst,0,newValueLength,value2Send);

        CsrPmemFree(inputStr);
        CsrPmemFree(confirm->text);
    }
    /* Hide input dialog */
    CsrUiUieHideReqSend(csrUiVar->hInputDialog);
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtRsccAppInstData *inst, CsrUiUieCreateCfm *confirm)
{
    CsrBtRsccAppUiInstData *csrUiVar = &inst->csrUiVar;

    if (confirm->elementType == CSR_UI_UIETYPE_EVENT)
    {
        if (!csrUiVar->hBackEvent)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hBackEvent = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSk1Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hSk1Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hInputSk1Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());
            csrUiVar->hInputSk1Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if (!csrUiVar->hSk2Event)
        {
            CsrUiEventSetReqSend(confirm->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            csrUiVar->hSk2Event = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        if (!csrUiVar->hInputDialog)
        {
            csrUiVar->hInputDialog = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        if (!csrUiVar->hCommonDialog)
        {
            csrUiVar->hCommonDialog = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
    if (confirm->elementType == CSR_UI_UIETYPE_MENU)
    {
        if(!(csrUiVar->hMainMenu))
        {
            CsrUiMenuSetReqSend(confirm->handle,
                            CONVERT_TEXT_STRING_2_UCS2(inst->profileName),
                            TEXT_SELECT_UCS2, 
                            NULL);

            CsrUiMenuAdditemReqSend(confirm->handle,
                                    CSR_UI_LAST, 
                                    CSR_BT_RSCC_APP_REGISTER_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_RSCC_APP_REGISTER_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent, 0);

            csrUiVar->hMainMenu = confirm->handle;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
        else
        { /* SC Control Point menu */
            CsrUiMenuSetReqSend(confirm->handle,
                                TEXT_RSCC_APP_SC_CONTROL_POINT_UCS2,
                                TEXT_SELECT_UCS2, 
                                NULL);

            CsrUiMenuAdditemReqSend(confirm->handle,
                                    CSR_UI_FIRST, 
                                    CSR_BT_RSCC_APP_SET_TOTAL_DISTANCE_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_RSCC_APP_SET_TOTAL_DISTANCE_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(confirm->handle,
                                    CSR_UI_LAST, 
                                    CSR_BT_RSCC_APP_START_CALIBRATION_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_RSCC_APP_START_CALIBRATION_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event,
                                    csrUiVar->hSk2Event,
                                    csrUiVar->hBackEvent, 
                                    0);            
            CsrUiMenuAdditemReqSend(confirm->handle, 
                                    CSR_UI_LAST, 
                                    CSR_BT_RSCC_APP_UPDATE_SENSOR_LOCATION_KEY,
                                    CSR_UI_ICON_NONE, 
                                    TEXT_RSCC_APP_UPDATE_SENSOR_LOCATION_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(confirm->handle,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCC_APP_REQ_SENSOR_LOCATIONS_KEY,
                                    CSR_UI_ICON_NONE, 
                                    TEXT_RSCC_APP_REQ_SENSOR_LOCATIONS_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            CsrUiMenuAdditemReqSend(confirm->handle,
                                    CSR_UI_LAST,
                                    CSR_BT_RSCC_APP_WRITE_RAW_KEY,
                                    CSR_UI_ICON_NONE, 
                                    TEXT_RSCC_APP_WRITE_RAW_UCS2,
                                    NULL,
                                    csrUiVar->hSk1Event, 
                                    csrUiVar->hSk2Event, 
                                    csrUiVar->hBackEvent, 
                                    0);
            csrUiVar->hSCControlPointMenu = confirm->handle;
        }
    }
}


static void handleCsrUiUieEventInd(CsrBtRsccAppInstData *inst, CsrUiEventInd *indication)
{
    CsrBtRsccAppUiInstData *csrUiVar = &inst->csrUiVar;;

    if (indication->event == csrUiVar->hBackEvent)
    {
        /*hide current shown displayelement*/
        CsrUiUieHideReqSend(indication->displayElement);
    }
    else if(indication->event == csrUiVar->hInputSk1Event)
    {
        CsrUiInputdialogGetReqSend(CsrSchedTaskQueueGet(), csrUiVar->hInputDialog);
    }
    else if (indication->event == csrUiVar->hSk1Event)
    {
        /* Main Menu handling */
        switch(indication->key)
        {
            case CSR_BT_RSCC_APP_REGISTER_KEY:
            {
                commonRsccPopupSet(inst, "Registering", "Please wait");
                commonRsccPopupShow(inst);
                CsrBtRsccAppStartRegister(inst);
                break;
            }
            case CSR_BT_RSCC_APP_UNREGISTER_KEY:
            {
                commonRsccPopupSet(inst, "Unregistering", "Please wait");
                commonRsccPopupShow(inst);
                CsrBtRsccAppStartUnregister(inst);
                break;
            }
            case CSR_BT_RSCC_APP_CONNECT_BREDR_KEY:
            case CSR_BT_RSCC_APP_CONNECT_KEY:
            {
                commonRsccPopupSet(inst, "Connecting", "Please wait");
                commonRsccPopupShow(inst);
                if(indication->key == CSR_BT_RSCC_APP_CONNECT_BREDR_KEY)
                {
                    inst->transport = CSR_BT_RSC_TRANSPORT_BREDR;
                }
                else
                {
                    inst->transport = CSR_BT_RSC_TRANSPORT_LE;
                }
                CsrBtRsccAppStartConnecting(inst);
                inst->csrUiVar.localInitiatedAction = TRUE;
                break;
            }
            case CSR_BT_RSCC_APP_DISCONNECT_KEY:
            {
                commonRsccPopupSet(inst, "Disconnecting", "Please wait");
                commonRsccPopupShow(inst);
                CsrBtRsccAppStartDisconnect(inst);
                inst->csrUiVar.localInitiatedAction = TRUE;
                break;
            }
            case CSR_BT_RSCC_APP_WRITE_NOTIFICATION_KEY :
            {
                if(inst->traversingDb)
                {
                    CsrBtGattAppHandleRsccCsrUiStillTraversingDb(inst);
                }
                else
                {
                    CsrUint16 val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION; 
                    commonRsccPopupSet(inst, "RSC", "Toggling measurement notifications.");
                    commonRsccPopupShow(inst);
                    if(inst->rscNotif)
                    {
                        val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
                    }

                    CsrBtRsccAppStartWriteClientConfig(inst, CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE, CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC, val);
                }
                break;
            }
            case CSR_BT_RSCC_APP_FEATURE_INFO_KEY:
            {
                if(inst->traversingDb)
                {
                    CsrBtGattAppHandleRsccCsrUiStillTraversingDb(inst);
                }
                else
                {
                    /*we have already found the valuehandle */
                    CsrBtGattAppHandleRsccCsrUiShowFeatureInfo(inst);
                }
                break;
            }
            case CSR_BT_RSCC_APP_WRITE_INDICATION_KEY:
            {
                if(inst->traversingDb)
                {
                    CsrBtGattAppHandleRsccCsrUiStillTraversingDb(inst);
                }
                else
                {
                    CsrUint16 val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION; 
                    if(inst->rscInd)
                    {
                        val = CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT;
                    }
                    
                    
                    commonRsccPopupSet(inst, "RSC", "Toggling SC Control Point indications");
                    commonRsccPopupShow(inst);                    

                    inst->rscWriteCC    = TRUE;
                    inst->rscScWriteReq = TRUE;
                    CsrBtRsccAppStartWriteClientConfig(inst, CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE, CSR_BT_GATT_UUID_SC_CONTROL_POINT_CHARAC, val);
                
                }
                break;
            }
            case CSR_BT_RSCC_APP_SC_CONTROL_POINT_KEY:
            {
                CsrUiUieShowReqSend(inst->csrUiVar.hSCControlPointMenu,
                                    CsrSchedTaskQueueGet(),
                                    CSR_UI_INPUTMODE_AUTO,
                                    CSR_BT_RSCC_APP_CSR_UI_PRI);
                break;
            }
            case CSR_BT_RSCC_APP_SET_TOTAL_DISTANCE_KEY:
            {
                CsrBtRsccAppStartWriteValue(inst, CSR_BT_RSCC_WRITE_OPCODE_STD, 0, NULL);
                break;
            }
            case CSR_BT_RSCC_APP_START_CALIBRATION_KEY:
            {
                CsrBtRsccAppStartWriteValue(inst, CSR_BT_RSCC_WRITE_OPCODE_SSC, 0, NULL);
                break;
            }
            case CSR_BT_RSCC_APP_UPDATE_SENSOR_LOCATION_KEY:
            {
                CsrBtRsccAppStartWriteValue(inst, CSR_BT_RSCC_WRITE_OPCODE_USL, 0, NULL);
                break;
            }
            case CSR_BT_RSCC_APP_REQ_SENSOR_LOCATIONS_KEY:
            {
                CsrBtRsccAppStartWriteValue(inst, CSR_BT_RSCC_WRITE_OPCODE_RSSL, 0, NULL);
                break;
            }
            case CSR_BT_RSCC_APP_WRITE_RAW_KEY:
            {
                /* Do a write operation on remote database without expecting confirmation */
                /* Input is taken as unformatted hex-values */               
                commonRsccPopupHide(inst);           
                commonRsccInputPopupSet(inst, "Input SC Control Point Write", CSR_UI_KEYMAP_ALPHANUMERIC);
                commonRsccInputPopupShow(inst);

                break;
            }
            case CSR_BT_RSCC_APP_OK_KEY:
            {
                break;
            }
            
            default:
            {
                break;
            }
        }
    }
    else if (indication->event == csrUiVar->hSk2Event)
    {
        CsrUiUieHideReqSend(indication->displayElement);
    }
}


void CsrBtGattAppHandleRsccCsrUiPrim(CsrBtRsccAppInstData *inst)
{
    switch(*((CsrUint16 *) inst->recvMsgP))
    {
        case CSR_UI_UIE_CREATE_CFM:
        {
            handleCsrUiUieCreateCfm(inst, inst->recvMsgP);
            break;
        }

        case CSR_UI_INPUTDIALOG_GET_CFM:
        {
            handleCsrUiInputDialogGetCfm(inst, inst->recvMsgP);
            break;
        }

        case CSR_UI_KEYDOWN_IND:
        {
            break;
        }

        case CSR_UI_EVENT_IND:
        {
            handleCsrUiUieEventInd(inst, inst->recvMsgP);
            break;
        }
        default:
        {
            break;
        }
    }
}


