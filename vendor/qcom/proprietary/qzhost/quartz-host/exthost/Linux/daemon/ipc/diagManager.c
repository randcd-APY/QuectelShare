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
#include "mml.h"
#include "bufpool.h"
#include "errno.h"
#include "ipcManager.h"
#include "qsCommon.h"
#include <unistd.h>


#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 2048
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

/*
 * Function: diagManager_TXthread
 * Description- Thread to handle all DIAG messages from an application. It
 *             will dequeue all messages from diag message queue and enqueue
 *             it to the appropriate service queue. MML will then send it out
 *             on the corresponding service queue.
 * Input: IOT Context pointer
 * Returns:
 */
void* diagManager_TXthread(void* arg)
{
    char* inbuf;
    IOTD_CXT_T* cxt = (IOTD_CXT_T*)arg;
    IPC_CXT_T* pIPCCxt = GET_DIAG_CXT(cxt);
    int sz;

    IOTD_LOG(LOG_TYPE_INFO,"Diag Manager: Tx thread started\n");

    /* This is the IPC TX thread. It will handle all incoming IPC messages and
     * process them. All QAPI messages will be queued to MML layer*/
    while (1)
    {
        while((inbuf = buf_alloc(MSG_BUFFER_SIZE)) == NULL){
            /*Lets wait for buffer to be available*/
            usleep(BUFPOOL_WAIT_TIME_US);
        }
        // get the oldest message with highest priority
        if ((sz = mq_receive (pIPCCxt->iotd_server, inbuf, MSG_BUFFER_SIZE, NULL)) == -1)
        {
            IOTD_LOG (LOG_TYPE_CRIT,"Diag Manager: Server error, mq_receive %s\n",strerror(errno));
            exit (1);
        }

        if(sz > 1)
        {
#ifdef PKT_DBG
    {
         int j = 0;
         printf("RX from Client %d bytes\n",sz);
         for(j=0;j < sz;j++)
	     printf("%x ",inbuf[j]);

         printf("\n");
    }
#endif
            process_msg(pIPCCxt, (uint8_t *)inbuf, sz, APPLY_SQ_ID(inbuf, IOTD_SQ_ID_DEBUG));
        } else {
            buf_free(inbuf);
            IOTD_LOG(LOG_TYPE_CRIT,"Diag Manager: Incorrect message size\n");
        }
    }
}

/*
 * Function: diagManager_RXthread: waits in Diag packets on DIAG service q. The received packet is
 *                    sent to application that is listening on diag posix msg q.
 * Input: IOTD context
 * Returns:
 *
 */
void* diagManager_RXthread(void* arg)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)arg;
    IPC_CXT_T* pIPCCxt = GET_DIAG_CXT(iotdCxt);
    void* buf = NULL;
    uint16_t total_Size = 0, i = 0;
    uint8_t q_id = 0;

    if(!pIPCCxt)
    {
        pthread_exit(arg);
    }
    IOTD_LOG(LOG_TYPE_INFO,"Starting diagManager_RXthread\n");

    while(1)
    {
        sem_wait(&(pIPCCxt->rx_sem));

        /* Dequeue and process buffers from all registered queues. */
        for(i = 0; i < pIPCCxt->map.num_q; i++)
        {
            /* Get queue ID from the registered service QIDs */
            q_id = GET_Q_ID(pIPCCxt->map.q_id[i]);
            while((buf = mml_dequeue(iotdCxt, q_id, IOTD_DIRECTION_RX)) != NULL)
            {
                /*To Do- Send response based on process cookie*/
                memcpy(&total_Size, (uint8_t *)buf, 2);
                IOTD_LOG(LOG_TYPE_VERBOSE,"Diag Manager: Packet received, queue:%u, size:%u\n", q_id, total_Size);
                for(i = 0; i < IOTD_MAX_CLIENTS; i++)
                {
                    /*For now, send to all clients. To-Do- need to come up with a better option*/
                    if(pIPCCxt->iotd_client[i].in_use == 1){
                        /*Reset control bit to indicate data frame*/ 
                        QS_RESET_CONTROL_BIT(buf);
                        send_client(pIPCCxt, i, ((uint8_t*)buf), total_Size);
                    }
                }
                buf_free(buf);
                buf = NULL;
            }
        }
    }
}



/*
 * Function: diagManager_init
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t diagManager_init(void* pCxt)
{
    struct mq_attr attr;
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    IPC_CXT_T* pIPCCxt = GET_DIAG_CXT(iotdCxt);
    uint32_t i = 0;
    cfg_ini* cfg = &(iotdCxt->cfg);

    /*Create backwards link*/
    pIPCCxt->iotd_cxt = iotdCxt;

    /**** get configuration from config file*/
    pIPCCxt->map.num_q = cfg->config_diag.num_service_q;

    /*Set up queues*/
    for(i=0; i < pIPCCxt->map.num_q; i++){
        pIPCCxt->map.q_id[i] = cfg->config_diag.qid[i];
    }

    /* Set other q_id to 255 to indicate that these aren't used */
    for(i = pIPCCxt->map.num_q; i < IOTD_MAX_NUM_Q; i++){
        pIPCCxt->map.q_id[i] = 255;
    }

    strncpy(pIPCCxt->server_q, cfg->config_diag.ipc_server, strlen(cfg->config_diag.ipc_server));
    /********************************/
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((pIPCCxt->iotd_server = mq_open((const char *)pIPCCxt->server_q, O_RDONLY | O_CREAT, 0666, &attr)) == -1)
    {
        IOTD_LOG (LOG_TYPE_CRIT,"Diag Manager: Server error, mq_open %d\n",strerror(errno));
        exit (1);
    }

    if(pthread_create(&(pIPCCxt->tx_thread), NULL, diagManager_TXthread,pCxt) != 0)
    {
        IOTD_LOG (LOG_TYPE_CRIT,"Diag Manager: Tx thread creation failed %d\n",strerror(errno));
        exit(1);
    }


    if(pthread_create(&(pIPCCxt->rx_thread), NULL, diagManager_RXthread,pCxt) != 0)
    {
        IOTD_LOG (LOG_TYPE_CRIT,"Diag Manager: Rx thread creation failed %d\n",strerror(errno));
        exit(1);
    }

    sem_init(&(pIPCCxt->rx_sem), 0, 1);

    if(IOTD_OK != ipcManager_queue_init(pIPCCxt))
    {
        IOTD_LOG (LOG_TYPE_CRIT,"Diag Manager: Queue initialization failed\n");
        exit(1);
    }

    for(i=0;i < IOTD_MAX_CLIENTS; i++)
    {
        pIPCCxt->iotd_client[i].handle = 0;
        pIPCCxt->iotd_client[i].in_use = 0;
    }

    IOTD_LOG (LOG_TYPE_INFO,"Diag manager initialized.\n");
    return IOTD_OK;
}

/*
 * Function: ipcManager_deinit
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t diagManager_deinit(void* pCxt)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    IPC_CXT_T* pIPCCxt = GET_DIAG_CXT(iotdCxt);

    IOTD_LOG(LOG_TYPE_INFO,"Diag manager deinitialized\n");

    if(pIPCCxt->tx_thread)
        pthread_cancel(pIPCCxt->tx_thread);
    if(pIPCCxt->rx_thread)
        pthread_cancel(pIPCCxt->rx_thread);

    mq_close(pIPCCxt->iotd_server);
    mq_unlink(pIPCCxt->server_q);
    return IOTD_OK;
}

