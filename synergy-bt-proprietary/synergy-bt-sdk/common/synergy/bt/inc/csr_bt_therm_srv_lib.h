#ifndef CSR_BT_THERM_SRV_LIB_H__
#define CSR_BT_THERM_SRV_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_therm_srv_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtThermSrvMsgTransport(void* msg);

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtThermSrvActivateReqSend
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
#define CsrBtThermSrvActivateReqSend(_appHandle,_clientConfig,_clientConfigSize,_eventMask) { \
        CsrBtThermSrvActivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtThermSrvActivateReq)); \
        msg__->type              =  CSR_BT_THERM_SRV_ACTIVATE_REQ;      \
        msg__->appHandle         =  _appHandle;                         \
        msg__->clientConfig      =  _clientConfig;                      \
        msg__->clientConfigSize  =  _clientConfigSize;                  \
        msg__->eventMask         =  _eventMask;                         \
        CsrBtThermSrvMsgTransport(msg__);}


    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtThermSrvDeactivateReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server in disabled mode
     *
     *    PARAMETERS
     *             appHandle : Queue id for the app activating the server
     *
     *----------------------------------------------------------------------------*/
#define CsrBtThermSrvDeactivateReqSend() {                              \
        CsrBtThermSrvDeactivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtThermSrvDeactivateReq)); \
        msg__->type              =  CSR_BT_THERM_SRV_DEACTIVATE_REQ;    \
        CsrBtThermSrvMsgTransport(msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtThermSrvUpdateTemperatureReqSend
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
#define CsrBtThermSrvUpdateTemperatureReqSend(_tempData, _tempDataSize) { \
        CsrBtThermSrvUpdateTemperatureReq *msg__ = CsrPmemAlloc(sizeof(CsrBtThermSrvUpdateTemperatureReq)); \
        msg__->type              =  CSR_BT_THERM_SRV_UPDATE_TEMPERATURE_REQ; \
        msg__->tempData          =  _tempData;                          \
        msg__->tempDataSize      =  _tempDataSize;                      \
        CsrBtThermSrvMsgTransport(msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtThermSrvUpdateBattLevelReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server battery Level value
     *
     *    PARAMETERS
     *         battLevel : The new battery level Value
     *         battMask  : bitmask identifying the state of the battery
     *         serviceRequired: service required information
     *----------------------------------------------------------------------------*/
#define CsrBtThermSrvUpdateBattLevelReqSend(_battLevel, _battMask, _serviceRequired) {    \
        CsrBtThermSrvUpdateBattLevelReq *msg__ = CsrPmemAlloc(sizeof(CsrBtThermSrvUpdateBattLevelReq)); \
        msg__->type              =  CSR_BT_THERM_SRV_UPDATE_BATT_LEVEL_REQ; \
        msg__->battLevel         =  _battLevel;                         \
        msg__->battMask          =  _battMask;                          \
        msg__->serviceRequired   =  _serviceRequired;                   \
        CsrBtThermSrvMsgTransport(msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtThermSrvFreeUpstreamMessageContents
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
void CsrBtThermSrvFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtThermSrvUpdateHealthInfoReqSend
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
#define CsrBtThermSrvUpdateHealthInfoReqSend(_infoType, _infoLength, _info) { \
        CsrBtThermSrvUpdateHealthInfoReq *msg__ = CsrPmemAlloc(sizeof(CsrBtThermSrvUpdateHealthInfoReq)); \
        msg__->type              =  CSR_BT_THERM_SRV_UPDATE_HEALTH_INFO_REQ; \
        msg__->infoType          =  _infoType;                          \
        msg__->infoLength        =  _infoLength;                        \
        msg__->info              =  _info;                              \
        CsrBtThermSrvMsgTransport(msg__);}


#ifdef __cplusplus
}
#endif

#endif /* _THERM_SRV_PRIM_H */

