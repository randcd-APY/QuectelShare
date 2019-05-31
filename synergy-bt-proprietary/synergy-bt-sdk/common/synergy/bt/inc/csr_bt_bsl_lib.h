#ifndef CSR_BT_BSL_LIB_H__
#define CSR_BT_BSL_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2001-2014 Qualcomm Technologies International, Ltd. 
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

/* Common put_message function to reduce code size */
void CsrBtBslMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslRegisterReqSend
 *
 *  DESCRIPTION
 *      This signal is used for registering the application handles in the BSL layer.
 *
 *  PARAMETERS
 *        theConnPhandle:       handle responsible for all connection setup and disconnect
 *        theDataPhandle:        handle responsible for all data handling.
 *                                            The connPhandle and dataPhandle can be identical.
 *----------------------------------------------------------------------------*/
#define CsrBtBslRegisterReqSend(_theConnPhandle, _theDataPhandle){      \
        CsrBtBslRegisterReq *msg = (CsrBtBslRegisterReq *)CsrPmemAlloc(sizeof(CsrBtBslRegisterReq)); \
        msg->type        = CSR_BT_BSL_REGISTER_REQ;                     \
        msg->connPhandle = _theConnPhandle;                             \
        msg->dataPhandle = _theDataPhandle;                             \
        CsrBtBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used for activating the local device for incoming connections.
 *
 *  PARAMETERS
 *        theSingleUser:        TRUE if only one connection is allowed
 *        theLocalRole:         The allowed/supported local roles according to the PAN profile
 *        theRemoteRole:     The allowed/supported roles of the remote device connecting to 
 *                                        the local device
 *----------------------------------------------------------------------------*/
#define CsrBtBslActivateReqSend(_theSingleUser, _theLocalRole, _theRemoteRole){ \
        CsrBtBslActivateReq *msg = (CsrBtBslActivateReq *)CsrPmemAlloc(sizeof(CsrBtBslActivateReq)); \
        msg->type       = CSR_BT_BSL_ACTIVATE_REQ;                      \
        msg->singleUser = _theSingleUser;                               \
        msg->localRole  = _theLocalRole;                                \
        msg->remoteRole = _theRemoteRole;                               \
        CsrBtBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslConnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used for initiating connection towards a remote device.
 *
 *  PARAMETERS
 *        theBdAddr:       The Bluetooth device address to which a connection must be established.
 *        theLocalRole:     The allowed/supported local role according to the PAN profile
 *        theRemoteRole: The allowed/supported role of the remote device connecting to the 
 *                                   local device
 *----------------------------------------------------------------------------*/
#define CsrBtBslConnectReqSend(_theBdAddr, _theLocalRole, _theRemoteRole){ \
        CsrBtBslConnectReq *msg = (CsrBtBslConnectReq *)CsrPmemAlloc(sizeof(CsrBtBslConnectReq)); \
        msg->type       = CSR_BT_BSL_CONNECT_REQ;                       \
        msg->bdAddr     = _theBdAddr;                                   \
        msg->remoteRole = _theRemoteRole;                               \
        msg->localRole  = _theLocalRole;                                \
        CsrBtBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslDisconnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used for disconnecting an already established connection.
 *
 *  PARAMETERS
 *        theId:    Unique identifier of this connection. Received in the connect indication signal.
 *----------------------------------------------------------------------------*/
#define CsrBtBslDisconnectReqSend(_theId){                              \
        CsrBtBslDisconnectReq *msg = (CsrBtBslDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtBslDisconnectReq)); \
        msg->type = CSR_BT_BSL_DISCONNECT_REQ;                          \
        msg->id   = _theId;                                             \
        CsrBtBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslDisconnectResSend
 *
 *  DESCRIPTION
 *      This signal is used for disconnect response for disconnect indication received.
 *
 *  PARAMETERS
 *        theId:    Unique identifier of this connection. Received in the connect indication signal.
 *----------------------------------------------------------------------------*/
#define CsrBtBslDisconnectResSend(_theId){                              \
        CsrBtBslDisconnectRes *msg = (CsrBtBslDisconnectRes *)CsrPmemAlloc(sizeof(CsrBtBslDisconnectRes)); \
        msg->type = CSR_BT_BSL_DISCONNECT_RES;                          \
        msg->id   = _theId;                                             \
        CsrBtBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslDataReqSend
 *
 *  DESCRIPTION
 *      This signal is used for sending data to a remote device for which a connection is 
 *      already established.
 *
 *  PARAMETERS
 *        theId:                Unique identifier of this connection
 *        theEtherType:     Type of Ethernet packet
 *        theDstAddr:       Ethernet address of destination for this data packet
 *        theSrcAddr:       Ethernet address of source for this data packet
 *        theLength:        Length in bytes for the data payload
 *        thePayload:       Pointer reference to the payload data
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
        CsrBtBslMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslMulticastDataReqSend
 *
 *  DESCRIPTION
 *      This signal is used for sending multicast data. I.e. send payload data to all connected 
 *      remote devices and possibly own higher layer stack.
 *
 *  PARAMETERS
 *        theId:                Defines whether the packet should be sent to local stack or not.
 *        theEtherType:     Type of Ethernet packet
 *        theDstAddr:       Ethernet address of destination for this data packet
 *        theSrcAddr:       Ethernet address of source for this data packet
 *        theLength:        Length in bytes for the data payload
 *        thePayload:       Pointer reference to the payload data
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
        CsrBtBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslServiceSearchReqSend
 *
 *  DESCRIPTION
 *      Request service record information from remote device
 *
 *  PARAMETERS
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
        CsrBtBslMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslSecurityInReqSend
 *      CsrBtBslSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBslSecurityInReqSend(_appHandle, _secLevel) {              \
        CsrBtBslSecurityInReq *msg = (CsrBtBslSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtBslSecurityInReq)); \
        msg->type = CSR_BT_BSL_SECURITY_IN_REQ;                         \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtBslMsgTransport(msg);}

#define CsrBtBslSecurityOutReqSend(_appHandle,  _secLevel) {            \
        CsrBtBslSecurityOutReq *msg = (CsrBtBslSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtBslSecurityOutReq)); \
        msg->type = CSR_BT_BSL_SECURITY_OUT_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtBslMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBslCancelConnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used for cancelling a previously initiated connection attempt
 *
 *  PARAMETERS
 *        bdAddr:       The Bluetooth device address to which connection cancel must be issued
 *----------------------------------------------------------------------------*/
#define CsrBtBslCancelConnectReqSend(_bdAddr){                          \
        CsrBtBslCancelConnectReq *msg = (CsrBtBslCancelConnectReq*)CsrPmemAlloc(sizeof(CsrBtBslCancelConnectReq)); \
        msg->type = CSR_BT_BSL_CANCEL_CONNECT_REQ;                      \
        msg->bdAddr = *((BD_ADDR_T *) _bdAddr);                         \
        CsrBtBslMsgTransport(msg);}


void CsrBtBslFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif
