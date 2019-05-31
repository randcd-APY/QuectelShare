#ifndef CSR_BT_HIDD_LIB_H__
#define CSR_BT_HIDD_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_hidd_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtHiddMsgTransport(CsrSchedQid phandle, void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHiddActivateReqSend
 *
 *  DESCRIPTION
 *      Activates the HID device service and enables connection to a HID
 *      host device
 *
 *    PARAMETERS
 *      hiddInstanceId      For multiple HIDD instances specify the
 *                          ID of the HIDD instance queue.
 *      appHandle           Application handle to receive signals
 *      qosCtrl             Quality of Service for control channel
 *      qosIntr             Quality of Service for interrupt channel
 *      flushTimeout        Flush timeout for channels
 *      deviceAddr          Device address for device to connect to (optional)
 *      deviceIdSdpLen      Length of device id service record
 *      deviceIdSdp         Device ID service record for registering service
 *      hidSdpLen           Length of HID device service record
 *      hidSdp              HID device service record for registering service
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHiddActivateReqSend(_hiddInstanceId, _appHandle, _qosCtrl, _qosIntr, _flushTimeout, _deviceAddr, _deviceIdSdpLen, _deviceIdSdp, _hidSdpLen, _hidSdp) { \
        CsrBtHiddActivateReq *msg__ = (CsrBtHiddActivateReq *) CsrPmemZalloc(sizeof(CsrBtHiddActivateReq)); \
        msg__->type              = CSR_BT_HIDD_ACTIVATE_REQ;            \
        msg__->appHandle         = _appHandle;                          \
        if (_qosCtrl)                                                   \
        {                                                               \
            msg__->qosCtrl       = _qosCtrl;                            \
            msg__->qosCtrlCount  = 1;                                   \
        }                                                               \
        if (_qosIntr)                                                   \
        {                                                               \
            msg__->qosIntr       = _qosIntr;                            \
            msg__->qosIntrCount  = 1;                                   \
        }                                                               \
        msg__->flushTimeout      = _flushTimeout;                       \
        msg__->deviceIdSdpLen    = _deviceIdSdpLen;                     \
        msg__->deviceIdSdp       = _deviceIdSdp;                        \
        msg__->hidSdpLen         = _hidSdpLen;                          \
        msg__->hidSdp            = _hidSdp;                             \
        msg__->deviceAddr        = _deviceAddr;                         \
        CsrBtHiddMsgTransport(_hiddInstanceId, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHiddDeactivateReqSend
 *
 *  DESCRIPTION
 *      Deactivates the HID device service and disconnects if connected
 *
 *    PARAMETERS
 *      hiddInstanceId      For multiple HIDD instances specify the
 *                          ID of the HIDD instance queue.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHiddDeactivateReqSend(_hiddInstanceId) {                   \
        CsrBtHiddDeactivateReq *msg__ = (CsrBtHiddDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtHiddDeactivateReq)); \
        msg__->type          = CSR_BT_HIDD_DEACTIVATE_REQ;              \
        CsrBtHiddMsgTransport(_hiddInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHiddControlResSend
 *
 *  DESCRIPTION
 *      Response to control request
 *
 *    PARAMETERS
 *      hiddInstanceId      For multiple HIDD instances specify the
 *                          ID of the HIDD instance queue.
 *      transactionType     Transaction type
 *      parameter           Parameters dependent on the above
 *      dataLen             Length of data (optional)
 *      data                Data (optional)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHiddControlResSend(_hiddInstanceId, _transactionType, _parameter, _dataLen, _data) { \
        CsrBtHiddControlRes *msg__ = (CsrBtHiddControlRes *) CsrPmemAlloc(sizeof(CsrBtHiddControlRes)); \
        msg__->type              = CSR_BT_HIDD_CONTROL_RES;             \
        msg__->transactionType   = _transactionType;                    \
        msg__->parameter         = _parameter;                          \
        msg__->dataLen           = _dataLen;                            \
        msg__->data              = _data;                               \
        CsrBtHiddMsgTransport(_hiddInstanceId, msg__);}




/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHiddDataReqSend
 *
 *  DESCRIPTION
 *      Data send request to send reports
 *
 *    PARAMETERS
 *      hiddInstanceId      For multiple HIDD instances specify the
 *                          ID of the HIDD instance queue.
 *      reportType          Report type
 *      reportLen           Length of report
 *      report              Report
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHiddDataReqSend(_hiddInstanceId, _reportLen, _report) {    \
        CsrBtHiddDataReq *msg__ = (CsrBtHiddDataReq *) CsrPmemZalloc(sizeof(CsrBtHiddDataReq)); \
        msg__->type          = CSR_BT_HIDD_DATA_REQ;                    \
        msg__->reportLen     = _reportLen;                              \
        msg__->report        = _report;                                 \
        CsrBtHiddMsgTransport(_hiddInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHiddUnplugReqSend
 *
 *  DESCRIPTION
 *      Request to unplug virtual cabled devices
 *
 *    PARAMETERS
 *      hiddInstanceId      For multiple HIDD instances specify the
 *                          ID of the HIDD instance queue.
 *      CsrBtDeviceAddr        Address of device to unplug
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHiddUnplugReqSend(_hiddInstanceId, _deviceAddr) {          \
        CsrBtHiddUnplugReq *msg__ = (CsrBtHiddUnplugReq *) CsrPmemAlloc(sizeof(CsrBtHiddUnplugReq)); \
        msg__->type          = CSR_BT_HIDD_UNPLUG_REQ;                  \
        msg__->deviceAddr    = _deviceAddr;                             \
        CsrBtHiddMsgTransport(_hiddInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HiddModeChangeReq
 *
 *  DESCRIPTION
 *      Change the power mode of the connection
 *
 *    PARAMETERS
 *      hiddInstanceId      For multiple HIDD instances specify the
 *                          ID of the HIDD instance queue.
 *      mode                Power mode to change to
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHiddModeChangeReqSend(_hiddInstanceId, _mode) {            \
        CsrBtHiddModeChangeReq *msg__ = (CsrBtHiddModeChangeReq *) CsrPmemAlloc(sizeof(CsrBtHiddModeChangeReq)); \
        msg__->type  = CSR_BT_HIDD_MODE_CHANGE_REQ;                     \
        msg__->mode  = _mode;                                           \
        CsrBtHiddMsgTransport(_hiddInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HiddSecurityInReq
 *      HiddSecurityOutReq
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *    PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHiddSecurityInReqSend(_appHandle, _secLevel) {             \
        CsrBtHiddSecurityInReq *msg = (CsrBtHiddSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtHiddSecurityInReq)); \
        msg->type = CSR_BT_HIDD_SECURITY_IN_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHiddMsgTransport(CSR_BT_HIDD_IFACEQUEUE, msg);}

#define CsrBtHiddSecurityOutReqSend(_appHandle, _secLevel) {            \
        CsrBtHiddSecurityOutReq *msg = (CsrBtHiddSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtHiddSecurityOutReq)); \
        msg->type = CSR_BT_HIDD_SECURITY_OUT_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHiddMsgTransport(CSR_BT_HIDD_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHiddFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT HIDD
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_HIDD_PRIM,
 *      msg:          The message received from Synergy BT HIDD
 *----------------------------------------------------------------------------*/
void CsrBtHiddFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);



#ifdef __cplusplus
}
#endif

#endif

