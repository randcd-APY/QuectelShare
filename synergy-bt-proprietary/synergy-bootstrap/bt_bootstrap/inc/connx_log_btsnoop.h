/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#ifndef _CONNX_LOG_BTSNOOP_H_
#define _CONNX_LOG_BTSNOOP_H_

#include "connx_util.h"
#include "bt_hci_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif


ConnxHandle BtsnoopCreate(const char *fileName);

void BtsnoopDestroy(ConnxHandle handle);

bool BtsnoopWriteRecord(ConnxHandle handle, bool received, const void *data, size_t dataLength);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_LOG_BTSNOOP_H_ */