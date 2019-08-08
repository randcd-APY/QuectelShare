/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#ifndef MCM_MSG_QUEUE_H
#define MCM_MSG_QUEUE_H

#include <pthread.h>

struct signal_data_type
{
    pthread_mutex_t           cond_mutex;
    pthread_cond_t            cond_var;
};

struct msg_queue_node
{
    void *user_data;
    struct msg_queue_node *next;
    struct msg_queue_node *prev;
};

struct msg_queue
{
    struct msg_queue_node *head;
    struct msg_queue_node *tail;
    pthread_mutex_t       queue_mutex;
};


typedef struct msg_queue mcm_msg_queue;
typedef struct msg_queue_node mcm_msg_queue_node;
typedef struct signal_data_type mcm_signal_data_type;

mcm_msg_queue *mcm_msg_queue_new( void );

void mcm_msg_queue_delete ( mcm_msg_queue *queue );

#define MCM_INIT_SIGNAL(signal_ptr) \
do \
{ \
    pthread_mutex_init (&(signal_ptr.cond_mutex),NULL); \
    pthread_cond_init (&(signal_ptr.cond_var),NULL); \
} while (0)

/* Macro to destroy signal data */
#define MCM_DESTROY_SIGNAL(signal_ptr) \
do \
{ \
    pthread_cond_destroy (&(signal_ptr.cond_var)); \
    pthread_mutex_destroy (&(signal_ptr.cond_mutex)); \
} while (0)

#define MCM_WAIT_FOR_SIGNAL(signal_ptr) \
do \
{ \
    pthread_mutex_lock (&(signal_ptr.cond_mutex)); \
    pthread_cond_wait (&(signal_ptr.cond_var),&(signal_ptr.cond_mutex)); \
    pthread_mutex_unlock (&(signal_ptr.cond_mutex)); \
} while (0)

#define MCM_SEND_SIGNAL(signal_ptr) \
do \
{ \
    pthread_mutex_lock (&(signal_ptr.cond_mutex)); \
    pthread_cond_signal (&(signal_ptr.cond_var)); \
    pthread_mutex_unlock (&(signal_ptr.cond_mutex)); \
} while (0)


#endif // MCM_MSG_QUEUE_H

