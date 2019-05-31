/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary. 

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_profiles.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_gatt_demo_generic_srv_prim.h"


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGenericSrvActivateReqSend
 *
 *  DESCRIPTION
 *      Set the thermometer server in discoverable mode and in connectable mode
 *
 *    PARAMETERS
 *             appHandle : Queue id for the app activating the server
 *          clientConfig : List of stored (persistent) client configurations.
 *                         Set to NULL if no previous client config have been
 *                         stored. The client config list used here is the one
 *                         returned in the DeactivateCfm signal. 
 *      clientConfigSize : number of elements in the clientConfig
 *             eventMask : Bitmask to tell the server which events the app 
 *                         wants notifications about. 
 *                         
 *----------------------------------------------------------------------------*/
CsrBtGenericSrvActivateReq *CsrBtGenericSrvActivateReq_struct(CsrSchedQid  appHandle,
                                                        CsrUint8 *clientConfig,
                                                        CsrUint16 clientConfigSize,
                                                        CsrBtGenericSrvEventMask eventMask)
{
    CsrBtGenericSrvActivateReq *pMsg = CsrPmemAlloc(sizeof(CsrBtGenericSrvActivateReq));

    pMsg->type              =  CSR_BT_GENERIC_SRV_ACTIVATE_REQ;
    pMsg->appHandle         =  appHandle;
    pMsg->clientConfig      =  clientConfig;
    pMsg->clientConfigSize  =  clientConfigSize;
    pMsg->eventMask         =  eventMask;
    return pMsg;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGenericSrvDeactivateReqSend
 *
 *  DESCRIPTION
 *      Set the thermometer server in disabled mode
 *
 *    PARAMETERS
 *             appHandle : Queue id for the app activating the server
 *                         
 *----------------------------------------------------------------------------*/
CsrBtGenericSrvDeactivateReq *CsrBtGenericSrvDeactivateReq_struct()
{
    CsrBtGenericSrvDeactivateReq *pMsg = CsrPmemAlloc(sizeof(CsrBtGenericSrvDeactivateReq));

    pMsg->type              =  CSR_BT_GENERIC_SRV_DEACTIVATE_REQ;
    return pMsg;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGenericSrvUpdateBattLevelReqSend
 *
 *  DESCRIPTION
 *      Set the thermometer server battery Level value  
 *
 *    PARAMETERS
 *         battLevel : The new battery level Value
 *         battMask  : bitmask identifying the state of the battery                
 *                         
 *----------------------------------------------------------------------------*/
CsrBtGenericSrvUpdateBattLevelReq *CsrBtGenericSrvUpdateBattLevelReq_struct(CsrUint16 battLevel, CsrBtGenericSrvBatteryMask battMask)
{
    CsrBtGenericSrvUpdateBattLevelReq *pMsg = CsrPmemAlloc(sizeof(CsrBtGenericSrvUpdateBattLevelReq));

    pMsg->type              =  CSR_BT_GENERIC_SRV_UPDATE_BATT_LEVEL_REQ;
    pMsg->battLevel         =  battLevel;
    pMsg->battMask          =  battMask; 
    return pMsg;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGenericSrvUpdateTemperatureReqSend
 *
 *  DESCRIPTION
 *      Set the thermometer server temperature value  
 *
 *    PARAMETERS
 *         temperature : The new temperature Value
 *         tempType    : type of Temperature:
 *                        CSR_BT_THERM_SRV_TEMP_TYPE_CELSIUS
 *                        CSR_BT_THERM_SRV_TEMP_TYPE_FAHRENHEIT
 *                         
 *----------------------------------------------------------------------------*/
CsrBtGenericSrvUpdateTemperatureReq *CsrBtGenericSrvUpdateTemperatureReq_struct(CsrUint8 *tempData, CsrUint16 tempDataSize)
{
    CsrBtGenericSrvUpdateTemperatureReq *pMsg = CsrPmemAlloc(sizeof(CsrBtGenericSrvUpdateTemperatureReq));

    pMsg->type              =  CSR_BT_GENERIC_SRV_UPDATE_TEMPERATURE_REQ;
    pMsg->tempData          =  tempData;
    pMsg->tempDataSize      =  tempDataSize;
    return pMsg;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGenericSrvUpdateHealthInfoReqSend
 *
 *  DESCRIPTION
 *      Set the health Info for the device  
 *
 *    PARAMETERS
 *         infoType    : type of info:
 *                          CSR_BT_THERM_SRV_HEALTH_INFO_MANUFACT_NAME_STR
 *                          CSR_BT_THERM_SRV_HEALTH_INFO_MODEL_NUM_STR
 *                          CSR_BT_THERM_SRV_HEALTH_INFO_SERIAL_NUM_STR
 *                          CSR_BT_THERM_SRV_HEALTH_INFO_HW_REVISION_STR
 *                          CSR_BT_THERM_SRV_HEALTH_INFO_FW_REVISION_STR
 *                          CSR_BT_THERM_SRV_HEALTH_INFO_SW_REVISION_STR
 *        infoLength   : length of the UTF8 string in info pointer
 *              info   : the UTF8 string containing the value
 *----------------------------------------------------------------------------*/
CsrBtGenericSrvUpdateHealthInfoReq *CsrBtGenericSrvUpdateHealthInfoReq_struct(CsrBtGenericSrvInfoType infoType, CsrUint16 infoLength, CsrUtf8String *info)
{
    CsrBtGenericSrvUpdateHealthInfoReq *pMsg = CsrPmemAlloc(sizeof(CsrBtGenericSrvUpdateHealthInfoReq));

    pMsg->type              =  CSR_BT_GENERIC_SRV_UPDATE_HEALTH_INFO_REQ;
    pMsg->infoType          =  infoType;
    pMsg->infoLength        =  infoLength;
    pMsg->info              =  info;
    return pMsg;
}



