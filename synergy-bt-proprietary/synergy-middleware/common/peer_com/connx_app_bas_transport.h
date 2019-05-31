#ifndef _CONNX_APP_ASM_TRANSPORT_H_
#define _CONNX_APP_ASM_TRANSPORT_H_

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


typedef struct
{
    size_t                     size;                  /* Total size in byte for bas register information. */
    uint16_t                   ipcType;               /* ipc type. */
} ConnxAppBasRegisterInfo;

ConnxHandle ConnxAppBasTransportOpen(ConnxContext appContext);

void ConnxAppBasTransportClose(ConnxHandle transportHandle);

void ConnxAppBasTransportSend(ConnxHandle transportHandle, void *data, size_t size);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_APP_ASM_TRANSPORT_H_ */