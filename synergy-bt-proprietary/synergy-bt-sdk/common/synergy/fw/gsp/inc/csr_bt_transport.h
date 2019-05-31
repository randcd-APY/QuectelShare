#ifndef CSR_BT_TRANSPORT_H__
#define CSR_BT_TRANSPORT_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * csrBtTransportRxFunc defines the interface for the callback
 * that the upper layer registers with the transport driver
 * layer.
 *
 * This function MUST be called in a context which is
 * synchronized with the upper layer such that it is possible
 * to access data structures in a safe manner.  This can e.g.
 * be done by registering a background interrupt in the start
 * callback and calling the receive callback in the background
 * interrupt handler function.
 *
 * Parameters:
 *
 *  arg     Upper layer context data that was registered along
 *          with the callback itself when starting a transport
 *          driver.
 *
 *  channel Indicates whether the packet is HCI, ACL, or SCO.
 *
 *  buf     A pointer to the packet from the lower layer.  If the
 *          upper layer cannot process the packet synchronously
 *          (i.e. entirely from within the execution of the
 *          callback), it MUST copy the buffer contents to a
 *          buffer of its own as the buffer is owned by the lower
 *          layer and only valid during the execution of the
 *          callback.
 *
 *  bufLen  The length in bytes of the packet provided in the
 *          buffer.
 */

typedef void (*csrBtTransportRxFunc)(void           *arg,
                                     CsrUint8        channel,
                                     const CsrUint8 *buf,
                                     CsrSize         bufLen);

/*
 * csrBtTransportStartFunc defines the interface that the
 * upper layer uses for starting a transport driver.
 *
 * It is based on the upper layer passing a callback function
 * of type csrBtTransportRxFunc that is called by the lower
 * layer when a packet is ready.  Along with the callback,
 * the upper layer provides a pointer to some context data
 * that the upper layer needs when processing packets.  The
 * lower layer treats this pointer as an opaque pointer and
 * thus never accesses it -- it is only passed to the upper
 * layer callback function.
 *
 * The function also provides context information that the
 * upper layer uses when making further calls to the transport
 * driver.
 *
 * Parameters:
 *
 *  driverArg   A pointer to the lower layer context
 *              information.
 *
 *  rxArg       A pointer to the upper layer context data which
 *              is passed to the upper layer callback function.
 *
 *  rxFunc      A pointer to the upper layer callback function.
 *
 *
 * Returns:
 *
 *  TRUE    The transport was requested to start correctly.
 *
 *  FALSE   The transport was unable to start.
 *
 */

typedef CsrBool (*csrBtTransportStartFunc)(void                *driverArg,
                                           void                *rxArg,
                                           csrBtTransportRxFunc rxFunc);


/*
 * csrBtTransportStopFunc defines the interface that the
 * upper layer uses to request the lower layer to stop the
 * transport driver.
 *
 * Parameters:
 *
 *  driverArg   A pointer to the lower layer context
 *              information.
 */

typedef void (*csrBtTransportStopFunc)(void *driverArg);

/*
 * csrBtTransportStackState contains information about the
 * state of the upper layer that may be relevant for the
 * operation of the lower layer.
 *
 * Members:
 *
 *  aclCount    Number of currently established ACL connections.
 *  scoCount    Number of currently established SCO connections.
 *  leCount     Number of currently established BT LE connections.
 */
typedef struct
{
    CsrUint8 aclCount;
    CsrUint8 scoCount;
    CsrUint8 leCount;
} csrBtTransportStackState;

/*
 * csrBtTransportInformFunc defines the interface that the
 * upper layer uses to inform the lower layer about current
 * stack state that may be relevant for the operation of the
 * lower layer.  For instance, this can be used to not allocate
 * memory for ACL and SCO data buffers until this is actually
 * needed, and on some host interfaces it can be used to defer
 * starting up parts of the host interface until it is needed.
 *
 * The most obvious example of the latter is USB, where there
 * may be a runtime cost in enabling data reception from the
 * SCO interface that should be avoided if possible.
 *
 * It is important to note that the upper stack provides
 * absolute state information and it is thus up to the lower
 * layer to track state transitions if required.
 *
 * Parameters:
 *
 *  driverArg   A pointer to the lower layer context
 *              information.
 *
 *  state       A pointer to a csrBtTransportStackState
 *              with the current state of the stack.
 *              The pointer is owned by the upper layer.
 *
 */
typedef void (*csrBtTransportInformFunc)(void                           *driverArg,
                                         const csrBtTransportStackState *state);

/*
 * csrBtTransportTxFunc defines the interface that the upper
 * layer uses to transmit data over the lower layer.
 * Data transmission as seen from the upper layer is a
 * blocking call, and thus the data buffer is only valid
 * during the execution of the transmission callback.
 *
 * NOTE:  The return value is used to signal whether the
 * packet is sent or not.  The upper layer considers failure
 * to transmit as a permanent error, and it is thus important
 * to try hard to send data.  In particular, temporary and
 * transient conditions such as lower layer buffer full should
 * under no circumstances cause transmit failure to be
 * signalled.
 *
 * Parameters:
 *
 *  driverArg   A pointer to the lower layer context
 *              information.
 *
 *  channel     Indicates whether the packet is HCI, ACL, or SCO.
 *
 *  buf         A pointer to the packet from the lower layer.
 *              If the lower layer cannot process the packet
 *              synchronously (i.e. entirely from within the
 *              execution of the callback), it MUST copy the
 *              buffer contents to a buffer of its own as the
 *              buffer is owned by the upper layer and only
 *              valid during the execution of the callback.
 *
 *  bufLen      The length in bytes of the packet provided in
 *              the buffer.
 *
 * Returns:
 *
 *  TRUE    The packet was sent or scheduled for transmission.
 *
 *  FALSE   The packet was not sent.
 */

typedef CsrBool (*csrBtTransportTxFunc)(void           *driverArg,
                                        CsrUint8        channel,
                                        const CsrUint8 *buf,
                                        CsrSize         bufLen);

/*
 * CsrBtTransport is the interface through which a transport
 * exposes its cardinal functionality.
 */

typedef struct
{
    csrBtTransportStartFunc  start;
    csrBtTransportStopFunc   stop;
    csrBtTransportInformFunc inform;
    csrBtTransportTxFunc     tx;
} CsrBtTransport;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_TRANSPORT_H__ */
