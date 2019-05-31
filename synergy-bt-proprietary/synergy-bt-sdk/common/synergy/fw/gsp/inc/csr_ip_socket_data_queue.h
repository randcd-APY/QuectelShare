#ifndef CSR_IP_SOCKET_DATA_QUEUE_H__
#define CSR_IP_SOCKET_DATA_QUEUE_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Context data for a Socket Data Queue instance */
typedef struct CsrIpSocketDataQueueContext CsrIpSocketDataQueueContext;

/* Create a new Socket Data Queue instance for a specified socketHandle */
CsrIpSocketDataQueueContext *CsrIpSocketDataQueueInit(CsrUint16 socketHandle);

/* Send or enqueue TCP data */
void CsrIpSocketDataQueueTcpReqSend(CsrIpSocketDataQueueContext *context,
                                    CsrUint32                    dataLength,
                                    CsrUint8                    *data);

/* Send or enqueue UDP data */
void CsrIpSocketDataQueueUdpReqSend(CsrIpSocketDataQueueContext *context,
                                    CsrUint16                    dataLength,
                                    CsrUint8                    *data,
                                    const CsrUint8               ipAddress[16],
                                    CsrUint16                    port);

/* Send or enqueue RAW data */
void CsrIpSocketDataQueueRawReqSend(CsrIpSocketDataQueueContext *context,
                                    CsrUint16                    dataLength,
                                    CsrUint8                    *data,
                                    const CsrUint8               ipAddress[16]);

/* Send the next TCP data in the queue. Returns TRUE when all data has been delivered. */
CsrBool CsrIpSocketDataQueueTcpCfmReceived(CsrIpSocketDataQueueContext *context);

/* Send the next UDP data in the queue. Returns TRUE when all data has been delivered. */
CsrBool CsrIpSocketDataQueueUdpCfmReceived(CsrIpSocketDataQueueContext *context);

/* Send the next RAW data in the queue. Returns TRUE when all data has been delivered. */
CsrBool CsrIpSocketDataQueueRawCfmReceived(CsrIpSocketDataQueueContext *context);

/* Destroy a Socket Data Queue instance after use */
void CsrIpSocketDataQueueDeinit(CsrIpSocketDataQueueContext *context);

#ifdef __cplusplus
}
#endif

#endif
