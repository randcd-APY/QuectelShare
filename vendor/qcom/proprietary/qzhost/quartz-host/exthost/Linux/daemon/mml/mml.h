/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef MML_H_
#define MML_H_

int mml_init(void* pCxt);
int mml_deinit(void* pCxt);
int mml_open_q(void* cxt, uint8_t qid, uint8_t dir, sem_t* sem);
int mml_enqueue(void* pCxt, void* buffer, uint8_t dir);
void* mml_dequeue(void* cxt, uint8_t qid, uint8_t dir);
#endif
