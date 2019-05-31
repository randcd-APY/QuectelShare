#ifndef CSR_BT_HOGH_LIB_H_
#define CSR_BT_HOGH_LIB_H_

/*****************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_hogh_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_addr.h"

#ifdef __cplusplus
extern "C"
{
#endif


/***************************** Register Request *******************************
 * Registers application with HOGH. All upstream primitives are sent to
 * registered application.
 * This API must be called before any other API and only once.
 *
 * pHandle              -   Application task handle
 *****************************************************************************/
void CsrBtHoghRegisterReqSend(CsrSchedQid pHandle);


/**************************** Add Device request ******************************
 * Adds a device for connection
 * Add Device confirm returns the hoghConnId for connection.
 *
 * HOGH bonds with HID device and enables notification on its own.
 *
 * If HID service is not found on the remote device, HOGH removes the connection.
 *
 * "addr" can be empty in which case HOGH scans and connects to any device
 * advertising HID service support. See CsrBtHoghAddAnyDeviceReqSend()
 *
 * HID host can handle multiple Add Device requests at a time.
 *
 * addr                 -   HID device address
 * mtu                  -   Preferred MTU
 * connAttemptTimeout   -   Add device procedure fails if connection could not
 *                          established in this much time.
 *****************************************************************************/
void CsrBtHoghAddDeviceReqSend(CsrBtTypedAddr addr,
                               CsrUint16 mtu,
                               CsrUint16 connAttemptTimeout);


/************************* Add Any Device request *******************************
 * Attempts connection on any device advertising support for HID service
 * HOGH rejects a Add Any Device request while previous Add Any Device request
 * is trying to find a HID device.
 * Add Any Device request does not timesout while searching for HID devices.
 *
 * mtu                  -   Preferred MTU
 * connAttemptTimeout   -   Add device procedure fails if connection could not
 *                          estabilished in this much time.
 *                          This time period applies during connection phase and
 *                          not during scanning phase.
 *****************************************************************************/
#define CsrBtHoghAddAnyDeviceReqSend(_mtu, _connAttemptTimeout)         \
    do                                                                  \
    {                                                                   \
        CsrBtTypedAddr addr;                                            \
        CsrBtAddrZero(&addr);                                            \
        CsrBtHoghAddDeviceReqSend(addr, _mtu, _connAttemptTimeout);     \
    } while(0)


/*********************** Connection Parameter request *************************
 * Changes scan parameters to use for reconnection and preferred connection
 * parameters to use after service discovery.
 * During service discovery HOGP uses aggressive connection parameters
 * recommended by HOGP specification.
 * Remote requested connection parameters are preferred over application
 * requested connection parameters.
 *
 * Scan parameters provided by application are written into Scan Window Interval
 * characteristics in Scan Parameter service, if found on HID device.
 *
 * HOGH uses default values defined in user config header by default.
 *
 * hoghConnId           -   HOGH connection ID
 * scanInterval         -   Scan interval to be used for reconnections
 * scanWindow           -   Scan window to be used reconnections
 * connIntervalMin      -   Minimum connection interval
 * connIntervalMax      -   Maximum connection interval
 * connLatency          -   Slave connection latency
 * supervisionTimeout   -   Connection supervision timeout
******************************************************************************/
void CsrBtHoghConnParamReqSend(CsrBtHoghConnId hoghConnId,
                               CsrUint16 scanInterval,
                               CsrUint16 scanWindow,
                               CsrUint16 connIntervalMin,
                               CsrUint16 connIntervalMax,
                               CsrUint16 connLatency,
                               CsrUint16 supervisionTimeout);


/************************* Configure Timeout request **************************
 * Changes timeout for disconnection and reconnection.
 * HOGH disconnects after idleTimeout and attempts reconnection after
 * reconnectTimeout.
 *
 * Sending CSR_BT_HOGH_TIMEOUT_IMMEDIATE as idleTimeout causes immediate
 * disconnection but this value is not retained. See CsrBtHoghDisconnectReqSend()
 * Using CSR_BT_HOGH_TIMEOUT_NEVER as idleTimeout disables idle timeout disconnection.
 *
 * Sending CSR_BT_HOGH_TIMEOUT_IMMEDIATE as connectTimeout causes HOGH to attempt
 * reconnection immediately on disconnection.
 * If connectTimeout is set to CSR_BT_HOGH_TIMEOUT_NEVER, HOGH does not attempt
 * reconnection at all after disconnection.
 *
 * By default HOGH uses default idle timeout and reconnection timeout values
 * defined in user config header
 *
 * hoghConnId           -   HOGH connection ID
 * idleTimeout          -   Idle disconnection timeout. Time = idleTimeout * 0.1 seconds
 * reconnectTimeout     -   Reconnection timeout. Time = reconnectTimeout * 0.1 seconds
 *****************************************************************************/
void CsrBtHoghTimeoutReqSend(CsrBtHoghConnId hoghConnId,
                             CsrUint16 idleTimeout,
                             CsrUint16 reconnectTimeout);


/***************************** Disconnect request *****************************
 * Disconnects the connection immediately.
 * Auto reconnection is disabled.
 *
 * To reconnect application must use CsrBtHoghReconnectReqSend() or
 * CsrBtHoghTimeoutReqSend().
 *
 * Note that this is not same as removing device.
 *
 * hoghConnId           -   HOGH connection ID
 *****************************************************************************/
#define CsrBtHoghDisconnectReqSend(_hoghConnId)                 \
    CsrBtHoghTimeoutReqSend(_hoghConnId,                        \
                            CSR_BT_HOGH_TIMEOUT_IMMEDIATE,      \
                            CSR_BT_HOGH_TIMEOUT_NEVER)


/***************************** Reconnect request ******************************
 * Reconnects immediately when disconnected.
 * Idle timeout disconnection is disabled.
 *
 * To enable idle timeout disconnection, use CsrBtHoghTimeoutReqSend().
 * To disconnect immediately, use CsrBtHoghDisconnectReqSend()
 *
 * hoghConnId           -   HOGH connection ID
 *****************************************************************************/
#define CsrBtHoghReconnectReqSend(_hoghConnId)                  \
    CsrBtHoghTimeoutReqSend(_hoghConnId,                        \
                            CSR_BT_HOGH_TIMEOUT_NEVER,          \
                            CSR_BT_HOGH_TIMEOUT_IMMEDIATE)


/****************************** Security request ******************************
 * Changes security requirements of a connection.
 * Note that security failure leads to removal of HOGP device.
 *
 * hoghConnID           -   HOGH connection ID
 * secReq               -   Security requirements
 *****************************************************************************/
void CsrBtHoghSecurityReqSend(CsrBtHoghConnId hoghConnId,
                              CsrBtGattSecurityFlags secReq);


/***************************** Get Report request *****************************
 * Fetches requested report from HID device.
 *
 * Applications must use reportType and reportId extracted from
 * reportMap received in respective Service indications.
 *
 * Note that HOGH anyway informs application of all report notifications
 * coming from HID device through Report indications.
 *
 * hoghConnId           -   HOGH connection ID
 * serviceId            -   HID service instance ID
 * reportType           -   HID report type
 * reportId             -   HID report ID. Must be set to 0 for Boot or
 *                          Battery reports from stand-alone Battery services
 *****************************************************************************/
void CsrBtHoghGetReportReqSend(CsrBtHoghConnId hoghConnId,
                               CsrUint8 serviceId,
                               CsrBtHoghReportType reportType,
                               CsrUint8 reportId);


/***************************** Set Report request *****************************
 * Writes requested report on HID device. This request is not applicable for
 * Battery reports from stand-alone Battery services.
 *
 * Applications must extract valid reportType and reportId from
 * reportMap received in respective Service indications.
 *
 * hoghConnId           -   HOGH connection ID
 * serviceID            -   HID service instance ID
 * reportType           -   HID report type
 * reportId             -   HID report ID. Must be set to 0 for Boot.
 * reportLength         -   HID report length
 * report               -   Pointer to HID report
 *****************************************************************************/
void CsrBtHoghSetReportReqSend(CsrBtHoghConnId hoghConnId,
                               CsrUint8 serviceId,
                               CsrBtHoghReportType reportType,
                               CsrUint8 reportId,
                               CsrUint16 reportLength,
                               CsrUint8 *report);


/**************************** Set Protocol request ****************************
 * Changes protocol mode of requested service on HID device.
 *
 * This request is applicable only for services having support for both report
 * and boot protocol. All the services along with their capabilities are
 * reported through SERVICE_IND.
 *
 * hoghConnId           -   HOGH connection ID
 * serviceId            -   HID service instance ID
 * protocol             -   Protocol to be set on HID device
 *****************************************************************************/
void CsrBtHoghSetProtocolReqSend(CsrBtHoghConnId hoghConnId,
                                 CsrUint8 serviceId,
                                 CsrBtHidsProtocol protocol);


/****************************** Set State request *****************************
 * Sets state of host. Connected devices are informed of suspend mode of host
 *
 * state                -   Suspend state of HOGH
 *****************************************************************************/
void CsrBtHoghSetStateReqSend(CsrBtHidsControlPoint state);


/*************************** Remove Connection request ************************
 * Removes the connection
 *
 * hoghConnId           -   HOGH connection ID
 *****************************************************************************/
void CsrBtHoghRemoveDeviceReqSend(CsrBtHoghConnId hoghConnId);

/*********************** Release upstream message contents ********************
 * Helper function to deallocate memory contents of in the Synergy BT HOGH
 * upstream messages.
 *
 * eventClass           -   Must be CSR_BT_HOGH_PRIM,
 * message              -   The message received from Synergy BT HOGH
 *****************************************************************************/
void CsrBtHoghFreeUpstreamMessageContents(CsrUint16 eventClass,
                                          void *message);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_HOGH_LIB_H_ */
