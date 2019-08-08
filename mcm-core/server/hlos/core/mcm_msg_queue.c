/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/


#include "mcm_msg_queue.h"
#include "mcm_constants.h"

#define MCM_LOCK_MUTEX_QUEUE(queue) pthread_mutex_lock (&queue->queue_mutex)

#define MCM_UNLOCK_MUTEX_QUEUE(queue) pthread_mutex_unlock (&queue->queue_mutex)

//=============================================================================
// FUNCTION: mcm_msg_queue_new <fd> <out: event>
//
// DESCRIPTION:
// create new queue
// @ingroup
//
// @return
//    queue ptr
//=============================================================================

mcm_msg_queue *mcm_msg_queue_new( void )
{
    mcm_msg_queue *queue;

    queue = calloc(1, sizeof(mcm_msg_queue));
    pthread_mutex_init (&queue->queue_mutex,NULL);

    if (!queue) {
        qmi_util_log("%s: Allocation failed\n", __FUNCTION__);
    }

    return queue;
}

//=============================================================================
// FUNCTION: mcm_msg_queue_delete <queue ptr>
//
// DESCRIPTION:
// Delete queue
// @ingroup
//
// @return
//    void
//=============================================================================

void mcm_msg_queue_delete
(
    mcm_msg_queue *queue
)
{
    free(queue);
}

//=============================================================================
// FUNCTION: mcm_msg_queue_enqueue <queue ptr> < event>
//
// DESCRIPTION:
// enqueue event into the queue
// @ingroup
//
// @return
//    void
//=============================================================================

int mcm_msg_queue_enqueue
(
    mcm_msg_queue *queue,
    void *value
)
{
    int rc = 0;
    mcm_msg_queue_node *new_node;

    MCM_LOCK_MUTEX_QUEUE(queue);

    new_node = calloc(1, sizeof(*new_node));

    if (new_node)
    {
        new_node->user_data = value;

        if ( queue->tail)
        {
            new_node->prev = queue->tail;
            new_node->next = NULL;
            queue->tail->next = new_node;
            queue->tail = new_node;
        }
        else
        {
            queue->tail = new_node;
            queue->head = new_node;
        }
    }
    else
    {
        qmi_util_log("%s: Allocation failed\n", __FUNCTION__);
        rc = -1;
    }
    MCM_UNLOCK_MUTEX_QUEUE(queue);

    return rc;
}


//=============================================================================
// FUNCTION: mcm_msg_queue_dequeue <queue ptr>
//
// DESCRIPTION:
// remove event from queue and return queue to calling function.
// @ingroup
//
// @return
//    event ptr
//=============================================================================

void *mcm_msg_queue_dequeue ( mcm_msg_queue *queue )
{
    void *ret = NULL;
    mcm_msg_queue_node *node;

    MCM_LOCK_MUTEX_QUEUE(queue);

    node = queue->head;

    if (node)
    {
        queue->head = queue->head->next;
        if (queue->head)
        {
            queue->head->prev = NULL;
        }
        else
        {
            queue->tail = NULL;
        }
        ret = node->user_data;
        free(node);
    }

    MCM_UNLOCK_MUTEX_QUEUE(queue);
    return ret;
}


