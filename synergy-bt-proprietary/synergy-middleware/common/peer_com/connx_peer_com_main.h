#ifndef _CONNX_PEER_COM_MAIN_H_
#define _CONNX_PEER_COM_MAIN_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "connx_common_def.h"
#include "connx_util.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Peer com mode: IPC or HAL. */
#define CSR_PEER_COM_MODE_IPC       0
#define CSR_PEER_COM_MODE_HAL       1

#define PCI_GET_INIT(p)                     ((p)->init)
#define PCI_GET_MODE(p)                     ((p)->peer_com_mode)
#define PCI_GET_MUTEX_HANDLE(p)             ((p)->mutexHandle)
#define PCI_GET_MAIN_TRANSPORT_HANDLE(p)    ((p)->mainTransportHandle)

#define PCI_LOCK(inst)                      ConnxMutexLock(PCI_GET_MUTEX_HANDLE(inst))
#define PCI_UNLOCK(inst)                    ConnxMutexUnlock(PCI_GET_MUTEX_HANDLE(inst))


typedef struct
{
    bool                  init;                   /* TRUE: peer com instance is initialized, FALSE: NOT. */
    uint32_t              peer_com_mode;          /* Peer com mode. */
    ConnxHandle           mutexHandle;            /* Mutex handle. */
    ConnxHandle           mainTransportHandle;    /* IPC transport handle. */
} ConnxPeerComInstance;

ConnxPeerComInstance *GetPeerComInst(void);

#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_PEER_COM_MAIN_H_ */

