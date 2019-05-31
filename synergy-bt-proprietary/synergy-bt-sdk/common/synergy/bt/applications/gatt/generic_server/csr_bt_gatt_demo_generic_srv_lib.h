#ifndef CSR_BT_GENERIC_SRV_LIB_H__
#define CSR_BT_GENERIC_SRV_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_types.h"
#include "csr_msg_transport.h"
#include "csr_bt_profiles.h"
#include "csr_bt_gatt_demo_generic_srv_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtGenericSrvActivateReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server in discoverable mode and in connectable mode
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
                                                              CsrBtGenericSrvEventMask eventMask);

#define CsrBtGenericSrvActivateReqSend(_appHandle,_clientConfig,_clientConfigSize,_eventMask) { \
    CsrBtGenericSrvActivateReq *msg__;\
    msg__=CsrBtGenericSrvActivateReq_struct(_appHandle,_clientConfig,_clientConfigSize,_eventMask); \
    CsrMsgTransport(CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE,CSR_BT_GENERIC_SRV_PRIM, msg__);}


    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtGenericSrvDeactivateReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server in disabled mode
     *
     *    PARAMETERS
     *             appHandle : Queue id for the app activating the server
     *                         
     *----------------------------------------------------------------------------*/
    CsrBtGenericSrvDeactivateReq *CsrBtGenericSrvDeactivateReq_struct(void);

#define CsrBtGenericSrvDeactivateReqSend() {\
    CsrBtGenericSrvDeactivateReq *msg__;\
    msg__=CsrBtGenericSrvDeactivateReq_struct();\
    CsrMsgTransport(CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE,CSR_BT_GENERIC_SRV_PRIM, msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtGenericSrvUpdateTemperatureReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server Temperature value  
     *
     *    PARAMETERS
     *               tempData : The temperature value packed as 
     *               1byte: type (Fahrenheit/celcius)
     *             2-*byte: value as float
     *                         
     *----------------------------------------------------------------------------*/
    CsrBtGenericSrvUpdateTemperatureReq *CsrBtGenericSrvUpdateTemperatureReq_struct(CsrUint8 *tempData, CsrUint16 tempDataSize);

#define CsrBtGenericSrvUpdateTemperatureReqSend(_instData, _instDataSize) {\
    CsrBtGenericSrvUpdateTemperatureReq *msg__;\
    msg__=CsrBtGenericSrvUpdateTemperatureReq_struct(_instData, _instDataSize);\
    CsrMsgTransport(CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE, CSR_BT_GENERIC_SRV_PRIM, msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtGenericSrvUpdateBattLevelReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server battery Level value  
     *
     *    PARAMETERS
     *         battLevel : The new battery level Value
     *         battMask  : bitmask identifying the state of the battery                
     *                         
     *----------------------------------------------------------------------------*/
    CsrBtGenericSrvUpdateBattLevelReq *CsrBtGenericSrvUpdateBattLevelReq_struct(CsrUint16 battLevel, CsrBtGenericSrvBatteryMask battMask);
#define CsrBtGenericSrvUpdateBattLevelReqSend(_battLevel, _battMask) {\
    CsrBtGenericSrvUpdateBattLevelReq *msg__;\
    msg__=CsrBtGenericSrvUpdateBattLevelReq_struct(_battLevel, _battMask);\
    CsrMsgTransport(CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE, CSR_BT_GENERIC_SRV_PRIM, msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtGenericSrvFreeUpstreamMessageContents
     *
     *  DESCRIPTION
     *      During Bluetooth shutdown all allocated payload in the Synergy BT THERM_SRV
     *      message must be deallocated. This is done by this function
     *
     *
     *    PARAMETERS
     *      eventClass :  Must be CSR_BT_THERM_SRV_PRIM,
     *      msg:          The message received from Synergy BT THERM_SRV
     *----------------------------------------------------------------------------*/
    void CsrBtGenericSrvFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


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
    CsrBtGenericSrvUpdateHealthInfoReq *CsrBtGenericSrvUpdateHealthInfoReq_struct(CsrBtGenericSrvInfoType infoType, CsrUint16 infoLength, CsrUtf8String *info);
#define CsrBtGenericSrvUpdateHealthInfoReqSend(_infoType, _infoLength, _info) {\
    CsrBtGenericSrvUpdateHealthInfoReq *msg__;\
    msg__=CsrBtGenericSrvUpdateHealthInfoReq_struct(_infoType,_infoLength,_info);\
    CsrMsgTransport(CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE, CSR_BT_GENERIC_SRV_PRIM, msg__);}


#ifdef __cplusplus
}
#endif

#endif /* _GENERIC_SRV_PRIM_H */

