#ifndef CSR_BT_GNSS_SERVER_LIB_H__
#define CSR_BT_GNSS_SERVER_LIB_H__

/******************************************************************************

Copyright (c) 2013-2016 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_bt_gnss_server_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

void CsrBtGnssServerMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssServerActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to activate a server instance and make it accessible
 *       from a remote device.
 *
 *      GNSS server would create a new independent server instance.
 *      If successful, GNSS server would reply back with server-instance-id and
 *      result in CSR_BT_GNSS_SERVER_ACTIVATION_CFM.
 *      Application should use server-instance-id for all further interactions.
 *
 *      If no more server instance are available, GNSS server would reply with
 *      CSR_BT_RESULT_CODE_GNSS_SERVER_MAX_INSTANCE in CSR_BT_GNSS_SERVER_ACTIVATION_CFM.
 *
 *  PARAMETERS
 *      phandle:        Application handle
 *      secLevel:       Minimum incoming security level requested by application.
 *                      The application must specify one of the following values:
 *                      1. CSR_BT_SEC_DEFAULT : Use default security settings
 *                      2. CSR_BT_SEC_MANDATORY : Use mandatory security settings
 *                      3. CSR_BT_SEC_SPECIFY : Specify new security settings
 *
 *                      If CSR_BT_SEC_SPECIFY is set the following values can be OR'ed additionally:
 *                      1. CSR_BT_SEC_AUTHORISATION: Require authorisation
 *                      2. CSR_BT_SEC_AUTHENTICATION: Require authentication
 *                      3. CSR_BT_SEC_SEC_ENCRYPTION: Require encryption (implies authentication)
 *                      4. CSR_BT_SEC_MITM: Require MITM protection (implies encryption)
 *
 *                      CSR GNSS server mandates authentication and encryption (CSR_BT_GNSS_MANDATORY_SECURITY_INCOMING).
 *                      A default security level (CSR_BT_GNSS_SERVER_DEFAULT_SECURITY_INCOMING) is defined in csr_bt_usr_config.h.
 *----------------------------------------------------------------------------*/
#define CsrBtGnssServerActivateReqSend(_phandle, _secLevel) do{         \
        CsrBtGnssServerActivateReq *msg = (CsrBtGnssServerActivateReq *)\
            CsrPmemAlloc(sizeof(CsrBtGnssServerActivateReq));           \
        msg->type = CSR_BT_GNSS_SERVER_ACTIVATE_REQ;                    \
        msg->phandle = _phandle;                                        \
        msg->secLevel = _secLevel;                                      \
        CsrBtGnssServerMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssServerDeactivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to deactivate a server instance and make it
 *      inaccessible from other devices.
 *
 *      Application can request for deactivation of GNSS server instance any time.
 *      GNSS server would take care of instance state (disconnection, data
 *      transmission, etc).
 *      GNSS server would reply back with CSR_BT_GNSS_SERVER_DEACTIVATION_CFM
 *      (deactivation confirmation).
 *      Any data encountered for same server instance, after deactivation
 *      request is received, would be discarded.
 *
 *  PARAMETERS
 *      instanceId:         Server instance id
 *----------------------------------------------------------------------------*/
#define CsrBtGnssServerDeactivateReqSend(_instanceId) do{               \
        CsrBtGnssServerDeactivateReq *msg = (CsrBtGnssServerDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtGnssServerDeactivateReq)); \
        msg->type = CSR_BT_GNSS_SERVER_DEACTIVATE_REQ;                  \
        msg->instanceId = _instanceId;                                  \
        CsrBtGnssServerMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssServerDisconnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used to disconnect a connection; the server instance
 *      would again be accessible for connection from remote devices.
 *
 *      Any data encountered after this signal would be ignored.
 *      Server instance would reply back with CSR_BT_GNSS_SERVER_DISCONNECT_IND
 *      (disconnection indication).
 *
 *  PARAMETERS
 *      instanceId:         Server instance id
 *      connId:             Connection id
 *----------------------------------------------------------------------------*/
#define CsrBtGnssServerDisconnectReqSend(_instanceId, _connId) do{      \
        CsrBtGnssServerDisconnectReq *msg = (CsrBtGnssServerDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtGnssServerDisconnectReq)); \
        msg->type = CSR_BT_GNSS_SERVER_DISCONNECT_REQ;                  \
        msg->instanceId = _instanceId;                                  \
        msg->connId = _connId;                                          \
        CsrBtGnssServerMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssServerDataReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send data to remote device.
 *
 *      Server instance would reply back with CSR_BT_GNSS_SERVER_DATA_CFM (data
 *      confirmation primitive).
 *      This signal is applicable only in connected state else server instance
 *      would reply back with "no connection" result code in data confirmation
 *      primitive.
 *
 *  PARAMETERS
 *      instanceId:         Server instance id
 *      connId:             Connection id
 *      payload:            Data pointer
 *      payloadLength:      Length of payload
 *                          Should be less than maxFrameSize returned in connection indication.
 *----------------------------------------------------------------------------*/
#define CsrBtGnssServerDataReqSend(_instanceId, _connId, _payload, _payloadLength) do{ \
        CsrBtGnssServerDataReq *msg = (CsrBtGnssServerDataReq *) CsrPmemAlloc(sizeof(CsrBtGnssServerDataReq)); \
        msg->type = CSR_BT_GNSS_SERVER_DATA_REQ;                        \
        msg->instanceId = _instanceId;                                  \
        msg->connId = _connId;                                          \
        msg->payload = _payload;                                        \
        msg->payloadLength = _payloadLength;                            \
        CsrBtGnssServerMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssServerLpmReqSend
 *
 *  DESCRIPTION
 *      This signal is used to set low power mode policy of server instance.
 *
 *      GNSS server supports three power modes - Auto, Active (manual) and
 *      sniff (manual) mode.
 *
 *      Auto mode - Server would take control of power mode and application
 *      would not be notified of mode change.
 *      Server would remain in sniff mode during idle time. It would temporarily
 *      move to active mode on receiving data request from application.
 *      Server would move back to sniff mode after duration of activiToSniffTime
 *      milliseconds. Moving back to sniff mode would get postponed if another
 *      data request is received from application during this period.
 *
 *      Manual mode (active and sniff) - Server would stay in requested mode.
 *      Low power mode interface would be transparent to application.
 *      All mode change events (CSR_BT_GNSS_SERVER_MODE_CHANGE_IND) would be
 *      forwarded to application.
 *      activeToSniffTime is not used and should be 0 in this mode.
 *
 *
 *      By default GNSS server remain in auto mode. Default value of activaToSniffTime
 *      (CSR_BT_GNSS_SERVER_LPM_TIME_DEFAULT) is defined in csr_bt_usr_config.h.
 *      Application can change power mode any number of time after activation.
 *
 *      Any change made before a connection would be saved and would come into
 *      effect on connection. Low power preference would be maintained across
 *      connections in a server instance.
 *
 *      Note: Power mode is not enforced, instead it would just be voted for.
 *
 *      This signal is optional and can be featurised out by enabling CMake cache
 *      parameter EXCLUDE_CSR_BT_GNSS_SERVER_MODULE_OPTIONAL.
 *
 *  PARAMETERS
 *      instanceId:         Server instance id
 *      mode:               Power mode. Valid values are - CSR_BT_GNSS_SERVER_LPM_AUTO,
 *                          CSR_BT_GNSS_SERVER_LPM_SNIFF and CSR_BT_GNSS_SERVER_LPM_ACTIVE
 *      activeToSniffTime:  Time in milliseconds to stay in active power mode in auto mode.
 *                          100 milliseconds is lower bound in auto mode.
 *                          Default value for auto mode (CSR_BT_GNSS_SERVER_LPM_TIME_DEFAULT)
 *                          is defined in csr_bt_usr_config.h.
 *----------------------------------------------------------------------------*/
#define CsrBtGnssServerLpmReqSend(_instanceId, _mode, _activeToSniffTime) do{   \
        CsrBtGnssServerLpmReq *msg = (CsrBtGnssServerLpmReq *) CsrPmemAlloc(sizeof(CsrBtGnssServerLpmReq)); \
        msg->type = CSR_BT_GNSS_SERVER_LPM_REQ;                                 \
        msg->instanceId = _instanceId;                                          \
        msg->mode = _mode;                                                      \
        msg->activeToSniffTime = _activeToSniffTime;                            \
        CsrBtGnssServerMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssServerFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT GNSS_SERVER
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass:         Must be CSR_BT_GNSS_SERVER_PRIM,
 *      msg:                The message received from Synergy BT GNSS_SERVER
 *----------------------------------------------------------------------------*/
void CsrBtGnssServerFreeUpstreamMessageContents(CsrUint16 eventClass,
                                                void *message);

#ifdef __cplusplus
}
#endif

#endif
