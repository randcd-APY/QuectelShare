#ifndef CSR_BT_PROXY_BSL_LIB_H__
#define CSR_BT_PROXY_BSL_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "csr_bt_bsl_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CsrProxyBslMsgTransport(msg) CsrMsgTransport(PROXY_BSL_IFACEQUEUE, CSR_BT_BSL_PRIM, msg)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslRegisterReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        theConnPhandle:
 *        theDataPhandle:
 *----------------------------------------------------------------------------*/
#define CsrBtBslRegisterReqSend(_theConnPhandle, _theDataPhandle){      \
        CsrBtBslRegisterReq *msg = (CsrBtBslRegisterReq *)CsrPmemAlloc(sizeof(CsrBtBslRegisterReq)); \
        msg->type        = CSR_BT_BSL_REGISTER_REQ;                     \
        msg->connPhandle = _theConnPhandle;                             \
        msg->dataPhandle = _theDataPhandle;                             \
        CsrProxyBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslActivateReqSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        theSingleUser:
 *        theLocalRole:
 *        theRemoteRole:
 *----------------------------------------------------------------------------*/
#define CsrBtBslActivateReqSend(_theSingleUser, _theLocalRole, _theRemoteRole){ \
        CsrBtBslActivateReq *msg = (CsrBtBslActivateReq *)CsrPmemAlloc(sizeof(CsrBtBslActivateReq)); \
        msg->type       = CSR_BT_BSL_ACTIVATE_REQ;                      \
        msg->singleUser = _theSingleUser;                               \
        msg->localRole  = _theLocalRole;                                \
        msg->remoteRole = _theRemoteRole;                               \
        CsrProxyBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslConnectReqSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        theBdAddr:
 *        theLocalRole:
 *        theRemoteRole:
 *----------------------------------------------------------------------------*/
#define CsrBtBslConnectReqSend(_theBdAddr, _theLocalRole, _theRemoteRole){ \
        CsrBtBslConnectReq *msg = (CsrBtBslConnectReq *)CsrPmemAlloc(sizeof(CsrBtBslConnectReq)); \
        msg->type       = CSR_BT_BSL_CONNECT_REQ;                       \
        msg->bdAddr     = _theBdAddr;                                   \
        msg->remoteRole = _theRemoteRole;                               \
        msg->localRole  = _theLocalRole;                                \
        CsrProxyBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslDisconnectReqSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        theId:
 *----------------------------------------------------------------------------*/
#define CsrBtBslDisconnectReqSend(_theId){                              \
        CsrBtBslDisconnectReq *msg = (CsrBtBslDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtBslDisconnectReq)); \
        msg->type = CSR_BT_BSL_DISCONNECT_REQ;                          \
        msg->id   = _theId;                                             \
        CsrProxyBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslDisconnectResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        theId:
 *----------------------------------------------------------------------------*/
#define CsrBtBslDisconnectResSend(_theId){                              \
        CsrBtBslDisconnectRes *msg = (CsrBtBslDisconnectRes *)CsrPmemAlloc(sizeof(CsrBtBslDisconnectRes)); \
        msg->type = CSR_BT_BSL_DISCONNECT_RES;                          \
        msg->id   = _theId;                                             \
        CsrProxyBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslDataReqSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        theId:
 *        theEtherType:
 *        theDstAddr:
 *        theSrcAddr:
 *        theLength:
 *        thePayload:
 *----------------------------------------------------------------------------*/
#define CsrBtBslDataReqSend(_theId, _theEtherType, _theDstAddr, _theSrcAddr, _theLength, _thePayload){ \
        CsrBtBslDataReq *msg = (CsrBtBslDataReq *)CsrPmemAlloc(sizeof(CsrBtBslDataReq)); \
        msg->type      = CSR_BT_BSL_DATA_REQ;                           \
        msg->id        = _theId;                                        \
        msg->etherType = _theEtherType;                                 \
        msg->dstAddr   = *((ETHER_ADDR *) _theDstAddr);                 \
        msg->srcAddr   = *((ETHER_ADDR *) _theSrcAddr);                 \
        msg->length    = _theLength;                                    \
        msg->payload   = _thePayload;                                   \
        CsrProxyBslMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslMulticastDataReqSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        theId:
 *        theEtherType:
 *        theDstAddr:
 *        theSrcAddr:
 *        theLength:
 *        thePayload:
 *----------------------------------------------------------------------------*/
#define CsrBtBslMulticastDataReqSend(_theId, _theEtherType, _theDstAddr, _theSrcAddr, _theLength, _thePayload){ \
    CsrBtBslMulticastDataReq *msg = (CsrBtBslMulticastDataReq *)CsrPmemAlloc(sizeof(CsrBtBslMulticastDataReq)); \
        msg->type      = CSR_BT_BSL_MULTICAST_DATA_REQ;                 \
        msg->idNot     = _theId;                                        \
        msg->etherType = _theEtherType;                                 \
        msg->dstAddr   = *((ETHER_ADDR *) _theDstAddr);                 \
        msg->srcAddr   = *((ETHER_ADDR *) _theSrcAddr);                 \
        msg->length    = _theLength;                                    \
        msg->payload   = _thePayload;                                   \
        CsrProxyBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslServiceSearchReqSend
 *
 *  DESCRIPTION
 *      Request service record information from remote device
 *
 *    PARAMETERS
 *        theHandle:            return handle
 *        theAddr:            address of Bluetooth device to search
 *        theRole:            search for panu, gn or nap service record
 *----------------------------------------------------------------------------*/
#define CsrBtBslServiceSearchReqSend(_theHandle, _theBdAddr, _theRole){ \
        CsrBtBslServiceSearchReq *msg = (CsrBtBslServiceSearchReq *)CsrPmemAlloc(sizeof(CsrBtBslServiceSearchReq)); \
        msg->type       = CSR_BT_BSL_SERVICE_SEARCH_REQ;                \
        msg->bd_addr    = _theBdAddr;                                   \
        msg->phandle    = _theHandle;                                   \
        msg->searchRole = _theRole;                                     \
        CsrProxyBslMsgTransport(msg);}


#ifdef __cplusplus
}
#endif

#endif

