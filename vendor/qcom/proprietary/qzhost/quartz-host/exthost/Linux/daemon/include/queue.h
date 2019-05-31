/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <semaphore.h>
#include <pthread.h>

/* 
1.	Each Service Queue is represented by a pair of queues (TX, RX)

2.	When an interface is initialized, it will take ownership of one or more queues. Ownership means that it will receive packets on these queues only. 

3.	Any module can post to a queue, but only one module can receive on a given queue.

4.	Each queue has an “indicate” function associated with it. At init time, the interface will register a callback function with the queue (that will wake up the interface thread). Any producer posting to the queue will invoke the corresponding callback.

*/

typedef struct _node
{
    void* pBuf;
    struct _node* next;
} NODE_T;


typedef struct _queue
{
    NODE_T* head;
    NODE_T* tail;
    unsigned short count;
    sem_t* notify_sem;    /*Notify semphore is registered by the queue owner*/
    pthread_mutex_t lock;
} QUEUE_T;

void queue_init(QUEUE_T* q);

int queue_enq(QUEUE_T* q, void* buf);

void* queue_deq(QUEUE_T* q);

int32_t queue_register_sem(QUEUE_T* q, sem_t* sem);

#endif
