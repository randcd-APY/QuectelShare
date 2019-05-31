/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef IPC_MGR_H
#define IPC_MGR_H


#include "iotd_context.h"


#define IPCM_CLIENT_QNAME_OFFSET  (IOTD_HEADER_LEN + 1)  /*Offset to q string in Hello Packet*/
#define IPCM_MODULE_ID_OFFSET  (IOTD_HEADER_LEN + 2)

#define IPCM_GET_PACKET_TYPE(_x)  (((char*)_x)[IPCM_MODULE_ID_OFFSET+1])
#define IPCM_SEND_TIMEOUT          (3)  /*Time in seconds to wait for an IPC TX*/

int ipcManager_init(void* pCxt);
int ipcManager_deinit(void* pCxt);
void process_msg(IPC_CXT_T* pCxt, uint8_t* buf, uint16_t size, uint8_t sqId);
int send_client(void* pCxt, uint8_t index, uint8_t* buf, uint16_t size);
int send_server_exit(void* pCxt, uint8_t msg);
int32_t ipcManager_queue_init(IPC_CXT_T* pCxt);

int32_t diagManager_init(void* pCxt);
int32_t diagManager_deinit(void* pCxt);
#endif
