/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_IPC_COMMON_H_
#define _CONNX_IPC_COMMON_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


/* IPC type. */
typedef uint16_t                           ConnxIpcType;

#define IPC_SOCKET                         ((ConnxIpcType) 0x0000)
#define MAX_IPC_TYPE                       ((ConnxIpcType) 0x0001)
#define IPC_UNKNOWN                        ((ConnxIpcType) 0xFFFF)

#define IS_IPC_SOCKET(type)                ((type) == IPC_SOCKET)
#define IS_VALID_IPC_TYPE(type)            ((type) < MAX_IPC_TYPE)

typedef uint32_t                            ConnxIpcFlag;
typedef uint16_t                            ConnxSocketRole;

#define SOCKET_SERVER                       ((ConnxIpcFlag)0x00000000)
#define SOCKET_CLIENT                       ((ConnxIpcFlag)0x00000001)

#define MAX_SOCKET_ROLE                     (2)

#define IPC_FLAG_MASK_BITS_SOCKET_ROLE      ((ConnxIpcFlag)0x0000000F)

typedef void (* ConnxIpcCbDataInd)(ConnxHandle handle, ConnxContext context, void *data, size_t size);
typedef void (* ConnxIpcCbClose)(ConnxHandle handle, ConnxContext context);
typedef void (* ConnxIpcCbReserved)(ConnxHandle handle, ConnxContext context);

typedef struct
{
    size_t                size;           /* Total size in byte for IPC server register information. */
    ConnxIpcType          type;
    ConnxIpcFlag          flag;
    ConnxContext          appContext;     /* App context. */
    char                 *name;
    ConnxIpcCbDataInd     dataIndCb;      /* Callback for IPC data ind. */
    ConnxIpcCbClose       closeCb;        /* Callback for IPC close. */
    ConnxIpcCbReserved    reservedCb;     /* Reserved. Callback for specific IPC function */
} ConnxIpcRegisterInfo;


bool ConnxIpcValildRegisterInfo(ConnxIpcRegisterInfo *registerInfo);

ConnxIpcRegisterInfo *ConnxIpcDuplicateRegisterInfo(ConnxIpcRegisterInfo *registerInfo);

void ConnxIpcFreeRegisterInfo(ConnxIpcRegisterInfo *registerInfo);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_IPC_COMMON_H_ */
