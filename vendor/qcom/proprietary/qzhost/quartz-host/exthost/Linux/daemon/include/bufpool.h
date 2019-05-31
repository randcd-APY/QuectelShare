/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef BUFPOOL_H
#define BUFPOOL_H

#define MAX_BUFFER_SIZE          (2048)
#define BUFPOOL_WAIT_TIME_US  (5000)
/** Buffer pool APIs*/

int bufpool_alloc(void* pCxt);
int bufpool_free(void* pCxt);
int buf_init(void* pCxt);
void* buf_alloc(uint32_t size);
void buf_free(void* buf);

#endif
