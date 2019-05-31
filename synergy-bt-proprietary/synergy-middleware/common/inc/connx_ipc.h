#ifndef _CONNX_IPC_H_
#define _CONNX_IPC_H_

/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_ipc_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxIpcOpen
 *
 *  DESCRIPTION
 *        Open IPC.
 *
 *  PARAMETERS
 *        context:           Context for IPC transport
 *----------------------------------------------------------------------------*/

ConnxHandle ConnxIpcOpen(ConnxContext context);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxIpcClose
 *
 *  DESCRIPTION
 *        Close IPC.
 *
 *  PARAMETERS
 *        handle:            Handle of message transport
 *----------------------------------------------------------------------------*/

void ConnxIpcClose(ConnxHandle handle);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxIpcDataSend
 *
 *  DESCRIPTION
 *        Send data through IPC transport.
 *
 *  PARAMETERS
 *        handle:            Handle of IPC transport
 *        data:              Buffer of data
 *        size:              Buffer size in byte of data
 *
 *  RETURN
 *        bytesSent          Actual size in byte of data which has been sent
 *                           "-1" indicates failure
 *----------------------------------------------------------------------------*/

void ConnxIpcDataSend(ConnxHandle handle, void *data, size_t size);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxIpcGetContext
 *
 *  DESCRIPTION
 *        Get the context for IPC.
 *
 *  PARAMETERS
 *        handle:            Handle of IPC transport
 *
 *  RETURN
 *        context            IPC context
 *----------------------------------------------------------------------------*/

ConnxContext ConnxIpcGetContext(ConnxHandle handle);


#ifdef __cplusplus
}
#endif

#endif /* _CONNX_IPC_H_ */

