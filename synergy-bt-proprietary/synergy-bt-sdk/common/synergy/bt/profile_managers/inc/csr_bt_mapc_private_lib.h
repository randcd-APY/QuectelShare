#ifndef CSR_BT_MAPC_PRIVATE_LIB_H__
#define CSR_BT_MAPC_PRIVATE_LIB_H__

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_mapc_prim.h"
#include "csr_bt_mapc_private_prim.h"
#include "csr_pmem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**** Client ****/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcAddNotiReqSend
 *
 *  DESCRIPTION
 *      Create notification service for a specific device.
 *
 *    PARAMETERS
 *      deviceAddr:       Remote device address
 *      masInstanceId:    MASInstanceID from SDP record
 *      mapHandle:        Task Handle of the profile initiating this request.
 *      appHandle:        Task Handle of the application for which notification
 *                        events are directed.
 *      security:         Security for the incoming connection
 *      maxFrameSize:     Max frame for the incoming obex packets.
 *      windowSize:       depth of the buffering pool.
 *----------------------------------------------------------------------------*/
#define CsrBtMapcAddNotiReqSend(_deviceAddr, _appHandle, _masInstanceId, _security, _maxFrameSize, _windowSize){ \
        CsrBtMapcAddNotiReq *msg = \
            (CsrBtMapcAddNotiReq*)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_MAPC_ADD_NOTI_REQ; \
        msg->deviceAddr = _deviceAddr; \
        msg->masInstanceId = _masInstanceId;\
        msg->mapHandle = CsrSchedTaskQueueGet(); \
        msg->appHandle = _appHandle;\
        msg->security = _security;\
        msg->maxFrameSize = _maxFrameSize;\
        msg->windowSize = _windowSize;\
        CsrMsgTransport(CSR_BT_MAPC_IFACEQUEUE, CSR_BT_MAPC_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcRemoveNotiReqSend
 *
 *  DESCRIPTION
 *      Remove the previously added notification service.
 *
 *    PARAMETERS
 *      deviceAddr:       Remote device address
 *      masInstanceId:    MASInstanceID from SDP record
 *      mapHandle:        Task Handle of the profile initiating this request.
 *      appHandle:        Task Handle of the application for which notification
 *                        events are directed.
 *      forceDisc:        Force disconnect the MNS connection.
 *----------------------------------------------------------------------------*/
#define CsrBtMapcRemoveNotiReqSend(_deviceAddr, _appHandle, _masInstanceId, _forceDisc){ \
        CsrBtMapcRemoveNotiReq *msg = \
            (CsrBtMapcRemoveNotiReq*)CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_MAPC_REMOVE_NOTI_REQ; \
        msg->deviceAddr = _deviceAddr; \
        msg->masInstanceId = _masInstanceId;\
        msg->mapHandle = CsrSchedTaskQueueGet(); \
        msg->appHandle = _appHandle;\
        msg->forceDisc = _forceDisc;\
        CsrMsgTransport(CSR_BT_MAPC_IFACEQUEUE, CSR_BT_MAPC_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcServiceCleanupSend
 *
 *  DESCRIPTION
 *      Request to cleanup the service.
 *
 *    PARAMETERS
 *      obexInstId: Unique Obex instance number identifying the service to 
 *                  be cleaned up.
 *----------------------------------------------------------------------------*/
#define CsrBtMapcServiceCleanupSend(_obexInstId){ \
        CsrBtMapcServiceCleanup *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_MAPC_SERVICE_CLEANUP; \
        msg->obexInstId = _obexInstId;\
        CsrMsgTransport(CSR_BT_MAPC_IFACEQUEUE, CSR_BT_MAPC_PRIM, msg);}

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_MAPC_PRIVATE_LIB_H__ */

