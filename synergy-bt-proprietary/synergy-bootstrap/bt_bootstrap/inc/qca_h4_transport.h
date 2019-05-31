/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _QCA_H4_TRANSPORT_H_
#define _QCA_H4_TRANSPORT_H_

#include "bt_h4_transport.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*HandleH4PacketFunc)(const uint8_t *data, uint32_t dataLength);


bool QCA_OpenH4Transport(char *device, uint32_t baudrate, HandleH4PacketFunc h4PacketHandler);

void QCA_CloseH4Transport(void);

int QCA_SendHciCommand(uint8_t *buf, int totalSize);


#ifdef __cplusplus
}
#endif

#endif  /* _QCA_H4_TRANSPORT_H_ */