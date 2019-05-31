/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <queue.h>
#include "iotd_context.h"


void* node_alloc()
{
    NODE_T* node = malloc(sizeof(NODE_T));
    return (void*)node;
}

void node_free(NODE_T* node)
{
    if(node)
    {
        free(node);
    }
}

void queue_init(QUEUE_T* q)
{
    q->head = q->tail = NULL;
    q->count = 0;
    q->notify_sem = NULL;
    pthread_mutex_init(&q->lock, NULL);
}


int queue_enq(QUEUE_T* q, void* buf)
{

    NODE_T *temp = node_alloc();

    if(temp != NULL)
    {
        temp->pBuf = buf;
        temp->next = NULL;
    }
    else
    {
        return -1;
    }

    pthread_mutex_lock(&q->lock);

    if (q->tail == NULL)
    {
        q->tail = q->head = temp;
        pthread_mutex_unlock(&q->lock);
        return 0;
    }

    /* Add the new node at the end of queue and change tail*/
    q->tail->next = temp;
    q->tail = temp;
    pthread_mutex_unlock(&q->lock);
    return 0;
}

void* queue_deq(QUEUE_T* q)
{
    NODE_T* temp;
    void* buf;

    pthread_mutex_lock(&q->lock);
    /* If queue is empty, return NULL.*/
    if (q->head == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    /* Store previous front and move front one node ahead*/
    temp = q->head;
    q->head = q->head->next;

    /* If front becomes NULL, then change rear also as NULL*/
    if (q->head == NULL)
    {
        q->tail = NULL;
    }

    buf = temp->pBuf;
    node_free(temp);
    pthread_mutex_unlock(&q->lock);
    return buf;

}

int32_t queue_register_sem(QUEUE_T* q, sem_t* sem)
{

    if(q == NULL){
        return IOTD_ERROR;
    }

    if(!q->notify_sem)
    {
        q->notify_sem = sem;
        return IOTD_OK;
    }
    return IOTD_ERROR;
}

