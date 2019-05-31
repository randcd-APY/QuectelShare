#ifndef CSR_BT_AVRCP_IMAGING_PRIVATE_LIB_H__
#define CSR_BT_AVRCP_IMAGING_PRIVATE_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2015 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_avrcp_imaging_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/**** Client ****/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientConnectReqSend
 *
 *  DESCRIPTION
 *      This API is used to connect to remote device Cover Art OBEX service.
 *
 *    PARAMETERS
 *      deviceAddr:       Remote device address
 *      coverArtpsm:      AVRCP Cover Art service PSM
 *      connectionId:     AVRCP Unique Connection identifier for a remote device 
 *      security:         Outgoing security of the obex connection.
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientConnectReqSend(_connectionId, _deviceAddr, _coverArtpsm, _security){ \
        CsrBtAvrcpImagingClientConnectReq *msg = \
            (CsrBtAvrcpImagingClientConnectReq*)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_CONNECT_REQ; \
        msg->pHandle = CsrSchedTaskQueueGet(); \
        msg->connectionId = _connectionId;\
        msg->deviceAddr = _deviceAddr; \
        msg->coverArtpsm = _coverArtpsm;\
        msg->security = _security;\
        CsrMsgTransport(CSR_BT_AVRCP_IMAGING_IFACEQUEUE, CSR_BT_AVRCP_IMAGING_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientCancelConnectReqSend
 *
 *  DESCRIPTION
 *      This API is used to cancel the connect to remote device Cover Art OBEX service.
 *
 *    PARAMETERS
 *      deviceAddr:       Remote device address
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientCancelConnectReqSend(_deviceAddr){ \
        CsrBtAvrcpImagingClientCancelConnectReq *msg = \
            (CsrBtAvrcpImagingClientCancelConnectReq*)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_CANCEL_CONNECT_REQ; \
        msg->deviceAddr = _deviceAddr; \
        CsrMsgTransport(CSR_BT_AVRCP_IMAGING_IFACEQUEUE, CSR_BT_AVRCP_IMAGING_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientDisconnectReqSend
 *
 *  DESCRIPTION
 *      This API is used for OBEX disconnect and or OBEX underlying transport (L2CAP)
 *
 *    PARAMETERS
 *      connectionId:         AVRCP Unique Connection identifier for a remote device 
 *      releaseTransport:   Decides if OBEX underlying transport (L2CAP) is released
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientDisconnectReqSend(_connectionId, _serviceOnly){ \
        CsrBtAvrcpImagingClientDisconnectReq *msg = \
            (CsrBtAvrcpImagingClientDisconnectReq*)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_DISCONNECT_REQ; \
        msg->connectionId = _connectionId;\
        msg->serviceOnly = _serviceOnly;\
        CsrMsgTransport(CSR_BT_AVRCP_IMAGING_IFACEQUEUE, CSR_BT_AVRCP_IMAGING_PRIM, msg);}

/**** Server ****/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingServerActivateReqSend
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingServerActivateReqSend(_psm, _address, _connId, _security, _mpHandle){ \
        CsrBtAvrcpImagingServerActivateReq *msg = \
            (CsrBtAvrcpImagingServerActivateReq *)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_SERVER_ACTIVATE_REQ; \
        msg->pHandle = CsrSchedTaskQueueGet(); \
        msg->psm = _psm; \
        msg->address = _address; \
        msg->connId = _connId; \
        msg->security = _security; \
        msg->mpHandle = _mpHandle; \
        CsrMsgTransport(CSR_BT_AVRCP_IMAGING_IFACEQUEUE, CSR_BT_AVRCP_IMAGING_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingServerAssociateConnectionReqSend
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingServerAssociateConnectionReqSend(_connId, _address, _mpHandle){ \
        CsrBtAvrcpImagingServerAssociateConnectionReq *msg = \
            (CsrBtAvrcpImagingServerAssociateConnectionReq *)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_SERVER_ASSOCIATE_CONNECTION_REQ; \
        msg->address = _address; \
        msg->connId = _connId; \
        msg->mpHandle = _mpHandle; \
        CsrMsgTransport(CSR_BT_AVRCP_IMAGING_IFACEQUEUE, CSR_BT_AVRCP_IMAGING_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingServerMpHandleUpdateReqSend
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingServerMpHandleUpdateReqSend(_connId, _mpHandle){ \
        CsrBtAvrcpImagingServerMpHandleUpdateReq *msg = \
            (CsrBtAvrcpImagingServerMpHandleUpdateReq *)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_SERVER_MP_HANDLE_UPDATE_REQ; \
        msg->mpHandle = _mpHandle; \
        msg->connId = _connId; \
        CsrMsgTransport(CSR_BT_AVRCP_IMAGING_IFACEQUEUE, CSR_BT_AVRCP_IMAGING_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingServerDeactivateReqSend
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingServerDeactivateReqSend(_psm, _connId){ \
        CsrBtAvrcpImagingServerDeactivateReq *msg = \
            (CsrBtAvrcpImagingServerDeactivateReq *)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_SERVER_DEACTIVATE_REQ; \
        msg->psm = _psm; \
        msg->connId = _connId; \
        CsrMsgTransport(CSR_BT_AVRCP_IMAGING_IFACEQUEUE, CSR_BT_AVRCP_IMAGING_PRIM, msg);}

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_AVRCP_IMAGING_PRIVATE_LIB_H__ */

