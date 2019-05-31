#ifndef CSR_BT_PROX_SRV_LIB_H__
#define CSR_BT_PROX_SRV_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2012 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_prox_srv_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtProxSrvMsgTransport(void* msg);

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtProxSrvActivateReqSend
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
#define CsrBtProxSrvActivateReqSend(_appHandle,_clientConfig,_clientConfigSize,_eventMask) { \
        CsrBtProxSrvActivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtProxSrvActivateReq)); \
        msg__->type              =  CSR_BT_PROX_SRV_ACTIVATE_REQ;       \
        msg__->appHandle         =  _appHandle;                         \
        msg__->clientConfig      =  _clientConfig;                      \
        msg__->clientConfigSize  =  _clientConfigSize;                  \
        msg__->eventMask         =  _eventMask;                         \
        CsrBtProxSrvMsgTransport(msg__);}


    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtProxSrvDeactivateReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server in disabled mode
     *
     *    PARAMETERS
     *             appHandle : Queue id for the app activating the server
     *
     *----------------------------------------------------------------------------*/
#define CsrBtProxSrvDeactivateReqSend() {                               \
        CsrBtProxSrvDeactivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtProxSrvDeactivateReq)); \
        msg__->type              =  CSR_BT_PROX_SRV_DEACTIVATE_REQ;     \
        CsrBtProxSrvMsgTransport(msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtProxSrvUpdateTxPowerReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server Tx power value
     *
     *    PARAMETERS
     *               txPower : The new TX Power Value
     *
     *----------------------------------------------------------------------------*/
#define CsrBtProxSrvUpdateTxPowerReqSend(_txPower) {                    \
        CsrBtProxSrvUpdateTxPowerReq *msg__ = CsrPmemAlloc(sizeof(CsrBtProxSrvUpdateTxPowerReq)); \
        msg__->type              =  CSR_BT_PROX_SRV_UPDATE_TX_POWER_REQ; \
        msg__->txPower           =  _txPower;                           \
        CsrBtProxSrvMsgTransport(msg__);}


    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtProxSrvUpdateBattLevelReqSend
     *
     *  DESCRIPTION
     *      Set the proximity server battery Level value
     *
     *    PARAMETERS
     *         battLevel : The new battery level Value
     *         battMask  : bitmask identifying the state of the battery
     *         serviceRequired: service required field
     *----------------------------------------------------------------------------*/
#define CsrBtProxSrvUpdateBattLevelReqSend(_battLevel, _battMask, _serviceRequired) {     \
        CsrBtProxSrvUpdateBattLevelReq *msg__ = CsrPmemAlloc(sizeof(CsrBtProxSrvUpdateBattLevelReq)); \
        msg__->type              =  CSR_BT_PROX_SRV_UPDATE_BATT_LEVEL_REQ; \
        msg__->battLevel         =  _battLevel;                         \
        msg__->battMask          =  _battMask;                          \
        msg__->serviceRequired   = _serviceRequired;                    \
        CsrBtProxSrvMsgTransport(msg__);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtProxSrvFreeUpstreamMessageContents
     *
     *  DESCRIPTION
     *      During Bluetooth shutdown all allocated payload in the Synergy BT PROX_SRV
     *      message must be deallocated. This is done by this function
     *
     *
     *    PARAMETERS
     *      eventClass :  Must be CSR_BT_PROX_SRV_PRIM,
     *      msg:          The message received from Synergy BT PROX_SRV
     *----------------------------------------------------------------------------*/
void CsrBtProxSrvFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif /* _PROX_SRV_PRIM_H */

