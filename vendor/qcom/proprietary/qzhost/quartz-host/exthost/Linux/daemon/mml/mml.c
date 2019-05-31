/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <syslog.h>
#include "iotd_context.h"
#include "queue.h"
#include "htc.h"
#include "ipcManager.h"

int mml_init(void* pCxt)
{
    int i, j;
    /*initialize all Service queues*/
    MML_CXT_T* mml_cxt = GET_MML_CXT(pCxt);

    for(i = 0; i < IOTD_MAX_NUM_Q; i++)
    {
        /*There are two queues per Service ID*/
        for(j = 0; j < 2; j++)
        {
            queue_init(&(mml_cxt->sq[i][j].q));
        }
    }
    return(htc_init(pCxt));
}


int mml_deinit(void* pCxt)
{
    return(htc_deinit(pCxt));
}


int32_t mml_enqueue(void* cxt, void* buffer, uint8_t dir)
{
    MML_CXT_T* mml_cxt = GET_MML_CXT(cxt);
    QUEUE_T *q = NULL;
    uint8_t sqid = 0, qid = 0;

    if(!cxt || !buffer || !IS_Q_DIR_VALID(dir))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"MML enqueue: Invalid inputs\n");
        return IOTD_ERROR;
    }

    /* Find Service QID and derive queue ID */
    sqid = *((uint8_t*)buffer + HTC_HEADER_LEN);
    qid = GET_Q_ID(sqid);
    if(qid > IOTD_MAX_NUM_Q)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"MML enqueue: Invalid q id\n");
        return IOTD_ERROR;
    }

    q = &(mml_cxt->sq[qid][dir].q);
    if(!q->notify_sem)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"MML queue for devId %u, sqId %u not in use\n", GET_DEV_ID(sqid), GET_SQ_ID(sqid));
        return IOTD_ERROR;
    }

    /*Enqueue to corresponding queue*/
    if(queue_enq(q, buffer) == -1)
    {
        /*Handle failure*/
        IOTD_LOG(LOG_TYPE_CRIT,"MML %s enqueue failed, Qid\n", (dir ? "Rx":"Tx"), qid);
        return IOTD_ERROR;
    }

    /*Signal handler thread*/
    sem_post(q->notify_sem);
    return IOTD_OK;
}

int mml_open_q(void* cxt, uint8_t qid, uint8_t dir, sem_t* sem)
{
    MML_CXT_T* mml_cxt = GET_MML_CXT(cxt);

    if(qid > IOTD_MAX_NUM_Q){
        IOTD_LOG(LOG_TYPE_CRIT,"MML Open: invalid q id\n");
        return IOTD_ERROR;
    }
    return queue_register_sem(&(mml_cxt->sq[qid][dir].q), sem);
}

int32_t mml_close_q(void* cxt, uint8_t qid, uint8_t dir)
{
    /*To be implemented*/
    return IOTD_OK;
}


void* mml_dequeue(void* cxt, uint8_t qid, uint8_t dir)
{
    MML_CXT_T* mml_cxt = GET_MML_CXT(cxt);

    if(!cxt || !IS_Q_DIR_VALID(dir))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"MML dequeue: Invalid inputs\n");
        return NULL;
    }

    if(qid > IOTD_MAX_NUM_Q)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"MML dequeue: Invalid q id %d\n",qid);
        return NULL;
    }

    return(queue_deq(&(mml_cxt->sq[qid][dir].q)));
}
