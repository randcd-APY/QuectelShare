#ifndef CSR_BT_MAPS_PRIVATE_LIB_H__
#define CSR_BT_MAPS_PRIVATE_LIB_H__

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_maps_prim.h"
#include "csr_bt_maps_private_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/**** Client ****/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsAddNotiReqSend
 *
 *  DESCRIPTION
 *      Create notification service for a specific device.
 *
 *    PARAMETERS
 *      deviceAddr:       Remote device address
 *      masConnId:        MAS connection id for which this notification belongs to.
 *      masInstanceId:    MASInstanceID from SDP record
 *      mapHandle:        Task Handle of the profile initiating this request.
 *      appHandle:        Task Handle of the application for which notification
 *                        events are directed.
 *      security:         Security for the incoming connection
 *      maxFrameSize:     Max frame for the incoming obex packets.
 *      windowSize:       depth of the buffering pool.
 *----------------------------------------------------------------------------*/
#define CsrBtMapsAddNotiReqSend(_deviceAddr, _masConnId, _appHandle, _masInstanceId, _security, _maxFrameSize, _windowSize){ \
        CsrBtMapsAddNotiReq *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_MAPS_ADD_NOTI_REQ; \
        msg->deviceAddr = _deviceAddr; \
        msg->masConnId = _masConnId;\
        msg->masInstanceId = _masInstanceId;\
        msg->mapHandle = CsrSchedTaskQueueGet(); \
        msg->appHandle = _appHandle;\
        msg->security = _security;\
        msg->maxFrameSize = _maxFrameSize;\
        msg->windowSize = _windowSize;\
        CsrMsgTransport(CSR_BT_MAPS_IFACEQUEUE, CSR_BT_MAPS_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsRemoveNotiReqSend
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
 *----------------------------------------------------------------------------*/
#define CsrBtMapsRemoveNotiReqSend(_deviceAddr, _appHandle){ \
        CsrBtMapsRemoveNotiReq *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_MAPS_REMOVE_NOTI_REQ; \
        msg->deviceAddr = _deviceAddr; \
        msg->mapHandle = CsrSchedTaskQueueGet(); \
        msg->appHandle = _appHandle;\
        CsrMsgTransport(CSR_BT_MAPS_IFACEQUEUE, CSR_BT_MAPS_PRIM, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsServiceCleanupSend
 *
 *  DESCRIPTION
 *      Request to cleanup the service.
 *
 *    PARAMETERS
 *      obexInstId: Unique Obex instance number identifying the service to 
 *                  be cleaned up.
 *----------------------------------------------------------------------------*/
#define CsrBtMapsServiceCleanupSend(_obexInstId){ \
        CsrBtMapsServiceCleanup *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_MAPS_SERVICE_CLEANUP; \
        msg->obexInstId = _obexInstId;\
        CsrMsgTransport(CSR_BT_MAPS_IFACEQUEUE, CSR_BT_MAPS_PRIM, msg);}

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_MAPS_PRIVATE_LIB_H__ */


