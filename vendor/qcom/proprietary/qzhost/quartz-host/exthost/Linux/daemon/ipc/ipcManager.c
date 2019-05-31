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


#define MAX_MESSAGES 15
#define MAX_MSG_SIZE 2048
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

/*
 * Function:ipcm_find_free_entry(
 * Input: pCxt- IPC context
 * Returns: array index if entry is found. -1 if no entry is found
 * Description: find a free entry in client table.
 */
int ipcm_find_free_entry(void* pCxt)
{
    IPC_CXT_T* pIPCCxt = (IPC_CXT_T*)pCxt;
    int i;

    for(i = 0; i < IOTD_MAX_CLIENTS; i++)
    {
        if(pIPCCxt->iotd_client[i].in_use == 0){
            return i;
        }
    }
    return -1;
}

/*
 * Function: ipcm_find_entry
 * Input: pCxt- IPC context. cookie- unique identifier that identifies a client
 * Returns: 0 on success or -1 if entry is not found
 * Description: Find an entry in client table based on cookie value.
 */
int ipcm_find_entry(void* pCxt, uint32_t cookie)
{
    IPC_CXT_T* pIPCCxt = (IPC_CXT_T*)pCxt;
    int i;

    for(i = 0; i < IOTD_MAX_CLIENTS; i++)
    {
        if(pIPCCxt->iotd_client[i].in_use == 1){
            if(pIPCCxt->iotd_client[i].cookie == cookie){
                IOTD_LOG(LOG_TYPE_VERBOSE,"IPCM: Handle found\n");
                return i;
            }
        }
    }
    IOTD_LOG(LOG_TYPE_CRIT ,"IPCM: Handle not found\n");
    return -1;

}


/*
 * Function: remove_client_entry
 * Input: pCxt- IPC context
 *        index- client entry index

 * Description: remove the client from client table. This will be called when an
 *              application sends exit notification.  
*/
int32_t remove_client_entry(IPC_CXT_T* pIPCCxt, int32_t index)
{
    IPC_CLIENT_T* IPC_client = NULL;

    IPC_client  = &(pIPCCxt->iotd_client[index]);
    
    IPC_client->in_use = 0;
    IPC_client->cookie = 0;
    memset(IPC_client->client_q, 0, 32);

    if(-1 == mq_close(IPC_client->handle))
    {
        IOTD_LOG(LOG_TYPE_CRIT ,"IPCM: Client queue close failed\n");
        return IOTD_ERROR;
    }
    return IOTD_OK;
}

/*
 * Function: send_client
 * Input: pCxt- IPC context
 *        index- client entry index
 *        buf- pointer to buffer to be transmitted
 *        size- size in bytes to be transmitted
 * Returns: IOTD_OK on success, IOTD_ERROR on send failure
 * Description: This function sends a packet to an application over a message queue. 
 *              If an application has terminated, the send operation will timeout and 
 *              the client entry will be removed.
 */
int send_client(void* pCxt, uint8_t index, uint8_t* buf, uint16_t size)
{
    IPC_CXT_T* pIPCCxt = (IPC_CXT_T*)pCxt;
    IPC_CLIENT_T* IPC_client = &(pIPCCxt->iotd_client[index]);
    int sz;
    struct timespec timeout;

    timeout.tv_sec = IPCM_SEND_TIMEOUT;
    timeout.tv_nsec = 0;
    if ((sz = mq_timedsend(IPC_client->handle, (const char *)buf, size , 0, &timeout)) == -1)
    {
        IOTD_LOG(LOG_TYPE_CRIT ,"IPCM: Send failed %s\n",strerror(errno));
        /*Something has gone wrong with the client, remove the client entry*/
        remove_client_entry(pIPCCxt, index);
        return IOTD_ERROR;
    }
    return IOTD_OK;
}

/*
 * Function: send_server_exit
 * Input: pCxt- IOT context, msg- pointer to exit message
 * Returns:
 * Description: Send server exit message to all clients. This indicated that target
 *              has died.
 */
int send_server_exit(void* pCxt, uint8_t msg)
{
    IOTD_CXT_T* cxt = (IOTD_CXT_T*)pCxt;
    IPC_CXT_T* pIPCCxt = GET_IPC_CXT(cxt);
    
    uint8_t resp_buf[18] = {'\0'};
    uint16_t total_size;
    int j;

    total_size = IOTD_HEADER_LEN + 1;

    /*Set Control message flag*/
    QS_SET_CONTROL_BIT(resp_buf);

    /*Set message type*/
    resp_buf[QS_CONTROL_MSG_TYPE_OFFSET] = msg;

    /* Send the control packet to client using DIAG service */
    pIPCCxt = GET_DIAG_CXT(cxt);
    if( pIPCCxt != NULL ) {
        for(j = 0; j < IOTD_MAX_CLIENTS; j++)
        {
            /*For now, send to all clients.*/
            if(pIPCCxt->iotd_client[j].in_use == 1){
                send_client(pIPCCxt, j, resp_buf, total_size);
            }
        }
    }
 
    /* Send the control packet to client using QAPI service */
    pIPCCxt = GET_IPC_CXT(cxt);
    if( pIPCCxt != NULL ) {
        for(j = 0; j < IOTD_MAX_CLIENTS; j++)
        {
            /*For now, send to all clients*/
            if(pIPCCxt->iotd_client[j].in_use == 1){
                send_client(pIPCCxt, j, resp_buf, total_size);
            }
        }
    }    
    return IOTD_OK;
}


/*
 * Function: process_hello
 * Input: pCxt- IPC context
 *        buf - received buffer
 *        size - size of packet 
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 * Description: Process HELLO message from a client. This is used by a client
 *              to associate with the daemon. A new entry for the client will be created.
 *              The message contains information about client's RX message queue.  
 */
int process_hello(void* pCxt, uint8_t* buf, uint16_t size)
{
    char resp_buf[IOTD_HEADER_LEN+18] = {'\0'};
    IPC_CXT_T* pIPCCxt = (IPC_CXT_T*)pCxt;
    int32_t index = ipcm_find_free_entry((void*)pIPCCxt);
    IPC_CLIENT_T* IPC_client;

    if(index == -1){
        /*No free entries, To-Do- handle failure, send NACK??*/
        IOTD_LOG (LOG_TYPE_CRIT ,"ERROR: IPCM: no client entries found\n");
        return IOTD_ERROR;
    }
    IPC_client  = &(pIPCCxt->iotd_client[index]);

    memcpy(IPC_client->client_q, &buf[IPCM_CLIENT_QNAME_OFFSET], size - IPCM_CLIENT_QNAME_OFFSET );
    memcpy(&IPC_client->cookie, &buf[QS_IOTD_COOKIE_OFFSET], 4);
    /*Mark this entry in use*/
    IPC_client->in_use = 1;

    if ((IPC_client->handle = mq_open((const char *)IPC_client->client_q, O_WRONLY)) == -1)
    {
        /*open on client queue failed, mark this entry free, do not send response*/
        IOTD_LOG (LOG_TYPE_CRIT ,"ERROR: IPCM: mq_open (server) %s\n",strerror(errno));
        IPC_client->in_use = 0;
        return IOTD_ERROR;
    }

    /*Set Control message flag*/
    QS_SET_CONTROL_BIT(resp_buf);

    /*Set message type*/
    resp_buf[QS_CONTROL_MSG_TYPE_OFFSET] = QS_HELLO_RESP;

    /*Send the message to application*/
    send_client(pCxt, index, (uint8_t*)resp_buf, IOTD_HEADER_LEN + 1);

    buf_free(buf);

    return IOTD_OK;
}

/*
 * Function: process_client_exit
 * Input: pCxt- IPC context
 *        buf - received buffer
 *        size - size of packet 
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 * Description: Process exit message from a client. This is used by a client
 *              to dis-associate with the daemon. The client entry will be removed.
 */

int32_t process_client_exit(void* pCxt, uint8_t* buf, uint16_t size)
{
    int32_t index = 0, res = IOTD_OK;
    uint32_t cookie = 0;
    IPC_CXT_T* pIPCCxt = (IPC_CXT_T*)(pCxt);
    
    memcpy((uint8_t*)&cookie, &((uint8_t *)buf)[6], 4);
    index = ipcm_find_entry(pIPCCxt, cookie);
    if(index == -1)
    {
        IOTD_LOG(LOG_TYPE_CRIT ,"IPCM: Handle not found\n");
        return IOTD_ERROR;
    }

    /*Delete client entry*/
    res = remove_client_entry(pIPCCxt, index);

    buf_free(buf);
    IOTD_LOG (LOG_TYPE_CRIT ,"IPCM: Client %d exited\n", index);

    return res;
}

/*
 * Function:process_msg
 * Description: Process messages received from applications via posix msg q.
 * Input: pCxt - IPC context
 *        buf - buffer received from app
 *        size - size in bytes of buffer
 *        sqId - Service Q ID- used to determine destination service Q
 * Returns:
 *
 */
void process_msg(IPC_CXT_T* pCxt, uint8_t* buf, uint16_t size, uint8_t sqId)
{
    uint8_t CtrlType = QS_GET_CONTROL_BIT(buf);
    uint8_t type;

    if(CtrlType){

        type = buf[QS_CONTROL_MSG_TYPE_OFFSET];       
        buf[IOTD_HEADER_LEN] = sqId;
 
        switch(type)
        {
            case QS_HELLO_REQ:
            if((IOTD_OK != process_hello(pCxt, buf, size)))
            {
                IOTD_LOG(LOG_TYPE_CRIT,"ERROR: IPCM: client Hello failed\n");
            }
            break;

            case QS_CLIENT_EXIT:
            if(IOTD_OK != process_client_exit(pCxt, buf, size))
            {
                IOTD_LOG(LOG_TYPE_CRIT,"ERROR: IPCM: client exit failed\n");
            }
            break;

            default:
                break;
       }
   } else {
            /*This is a QAPI message, enqueue it based on sq ID for
              transmission to Quartz*/
            buf[HTC_HEADER_LEN] = sqId;

            /*Write packet size to HTC header. Size includes HTC+MML header size*/
            WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(buf, size);

            /*Enqueue the packet to correct queue*/
            if(mml_enqueue(pCxt->iotd_cxt, buf, IOTD_DIRECTION_TX) != IOTD_OK){
                buf_free(buf);
            }
           
    }
}


/*
 * Function:ipcManager_TXthread
 * Input:Iotd context
 * Returns:
 * Description: This is the IPC Manager TX thread. It received packets from applications
 *              on server (iotdq) message queue and processes them. All QAPI frames are
 *              enqueued to MML layer, which then sends them to the target.
 */
void* ipcManager_TXthread(void* arg)
{
    char* inbuf;
    IOTD_CXT_T* cxt = (IOTD_CXT_T*)arg;
    IPC_CXT_T* pIPCCxt = GET_IPC_CXT(cxt);
    int sz = -1;

    IOTD_LOG(LOG_TYPE_INFO,"IPCM: QAPI Tx thread started\n");

    /*This is the IPC TX thread. It will handle all incoming IPC messages and
      process them. All QAPI messages will be queued to MML layer*/
    while (1)
    {
        while((inbuf = buf_alloc(MSG_BUFFER_SIZE)) == NULL){
            /*Lets wait for buffer to be available*/
            IOTD_LOG (LOG_TYPE_CRIT,"IPCM: Waiting for buf\n");
            usleep(BUFPOOL_WAIT_TIME_US);
        }
        // get the oldest message with highest priority
        if ((sz = mq_receive (cxt->ipc_cxt.iotd_server, inbuf, MSG_BUFFER_SIZE, NULL)) == -1)
        {
            IOTD_LOG (LOG_TYPE_CRIT,"IPCM: Server error, mq_receive %s\n",strerror(errno));
            exit (1);
        }

        if(sz > 1)
        {
            process_msg(pIPCCxt, (uint8_t *)inbuf, sz, APPLY_SQ_ID(inbuf, IOTD_SQ_ID_QAPI));
        } else {
            buf_free(inbuf);
            IOTD_LOG(LOG_TYPE_CRIT,"IPCM: Incorrect message size\n");
        }
    }
}

/*
 * Function: ipcManager_RXthread
 * Description: this thread listens on QAPI service q for incoming QAPI
 * responses from Quartz. All packets are then sent to applications via posix msg q.
 * Input: IOTD context
 * Returns:
 *
 */
void* ipcManager_RXthread(void* arg)
{
    IOTD_CXT_T* cxt = (IOTD_CXT_T*)arg;
    IPC_CXT_T* pIPCCxt = GET_IPC_CXT(cxt);
    void *buf = NULL;
    uint16_t total_Size = 0;
    uint32_t cookie = 0, index = 0;
    uint8_t packetType, q_id = 0;
    uint16_t i = 0, j = 0;

    IOTD_LOG(LOG_TYPE_INFO,"Starting ipcManager_RXthread\n");

    while(1)
    {
        sem_wait(&(pIPCCxt->rx_sem));

        /* Dequeue and process buffers from all registered queues. */
        for(i = 0; i < pIPCCxt->map.num_q; i++)
        {
            /* Get queue ID from the registered service QIDs */
            q_id = GET_Q_ID(pIPCCxt->map.q_id[i]);
            while((buf = mml_dequeue(cxt, q_id, IOTD_DIRECTION_RX)) != NULL)
            {
#ifdef GPIO_PROFILING
                toggleGPIO();
#endif
                /*To Do- Send response based on process cookie*/
                memcpy(&total_Size, ((uint8_t *)buf), 2);
 
                IOTD_LOG(LOG_TYPE_VERBOSE,"IPCM: Packet dequeued, queue:%u, size:%u\n", q_id, total_Size);

                packetType = IPCM_GET_PACKET_TYPE(buf);
                /*Reset control bit to indicate data frame*/ 
                QS_RESET_CONTROL_BIT(buf);
                switch (packetType)
                {
                    case QS_RETURN_E:
                        memcpy((uint8_t*)&cookie, &((uint8_t *)buf)[QS_IOTD_COOKIE_OFFSET], 4);
                        index = ipcm_find_entry(pIPCCxt, cookie);
                        if(index == -1){
                            /*No free entries, To-Do- handle failure, send NACK??*/
                            IOTD_LOG (LOG_TYPE_CRIT,"IPCM: handle not found\n");
                            buf_free(buf);
                            return NULL;
                        }
                        send_client(pIPCCxt, index, (uint8_t*)buf, 
                                    total_Size + QS_IOTD_HEADER_LEN);
                        break;

                    case QS_ASYNC_E:
                        for(j = 0; j < IOTD_MAX_CLIENTS; j++)
                        {
                            /*For now, send to all clients. To-Do- need to come up with a better option*/
                            if(pIPCCxt->iotd_client[j].in_use == 1){
                                send_client(pIPCCxt, j, (uint8_t*)buf, 
                                            total_Size + QS_IOTD_HEADER_LEN);
                            }
                        }
                        break;

                    default:
                        IOTD_LOG(LOG_TYPE_CRIT, "IPCM: Invalid packet type %d\n",packetType);
                        break;
                }
                buf_free(buf);
            }
        }
    }
}

/*
 * Function: ipcManager_queue_init
 * Input: IOTD Context
 * Returns: IOTD_OK on success, IOTD_ERROR on failure
 * Description- open all the queues associated with IPC manager. This information comes from config file.
 */
int32_t ipcManager_queue_init(IPC_CXT_T* pIPCCxt)
{
    int8_t i = 0;
    uint8_t q_id = 0;

    for(i=0; i < pIPCCxt->map.num_q; i++)
    {
        /* Get queue ID from the registered service QIDs */
        q_id = GET_Q_ID(pIPCCxt->map.q_id[i]);
        if(IOTD_OK != mml_open_q(pIPCCxt->iotd_cxt, q_id, IOTD_DIRECTION_RX,  &(pIPCCxt->rx_sem)))
        {
            return IOTD_ERROR;
        }
    }
    return IOTD_OK;
}

/*
 * Function: ipcManager_init
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t ipcManager_init(void* pCxt)
{
    struct mq_attr attr;
    IOTD_CXT_T* cxt = (IOTD_CXT_T*)pCxt;
    IPC_CXT_T* pIPCCxt = GET_IPC_CXT(cxt);
    uint32_t i;
    cfg_ini* cfg = &(cxt->cfg);

    /* Get configuration from config file */
    pIPCCxt->iotd_cxt = pCxt;
    pIPCCxt->map.num_q = cfg->config_qapi.num_service_q;

    /*Set up queues*/
    for(i = 0; i < pIPCCxt->map.num_q; i++){
        pIPCCxt->map.q_id[i] = cfg->config_qapi.qid[i];
    }

    /* Set other q_id to 255 to indicate that these aren't used */
    for(i = pIPCCxt->map.num_q; i < IOTD_MAX_NUM_Q; i++){
        pIPCCxt->map.q_id[i] = 255;
    }

    strncpy(pIPCCxt->server_q, cfg->config_qapi.ipc_server, strlen(cfg->config_qapi.ipc_server));
    /********************************/
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((pIPCCxt->iotd_server = mq_open((const char *)pIPCCxt->server_q, O_RDONLY | O_CREAT, 0666, &attr)) == -1)
    {
        IOTD_LOG (LOG_TYPE_CRIT,"IPCM: Server error, mq_open (server) %s\n",strerror(errno));
        exit (1);
    }

    if(pthread_create(&(pIPCCxt->tx_thread), NULL, ipcManager_TXthread,pCxt) != 0)
    {
        IOTD_LOG (LOG_TYPE_CRIT,"IPCM: Tx thread creation failed, %s\n",strerror(errno));
        exit(1);
    }

    if(pthread_create(&(pIPCCxt->rx_thread), NULL, ipcManager_RXthread,pCxt) != 0)
    {
        IOTD_LOG (LOG_TYPE_CRIT,"IPCM: Rx thread creation failed, %s\n",strerror(errno));
        exit(1);
    }

    sem_init(&(pIPCCxt->rx_sem), 0, 1);

    if(IOTD_OK != ipcManager_queue_init(pIPCCxt))
    {
        IOTD_LOG (LOG_TYPE_CRIT,"IPCM: Queue initialization failed\n");
        exit(1);
    }

    for(i=0;i < IOTD_MAX_CLIENTS; i++)
    {
        pIPCCxt->iotd_client[i].handle = 0;
        pIPCCxt->iotd_client[i].in_use = 0;
    }

    IOTD_LOG (LOG_TYPE_INFO,"IPCManager initialized.\n");

    return IOTD_OK;
}

/*
 * Function: ipcManager_deinit
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t ipcManager_deinit(void* pCxt)
{
    IOTD_CXT_T* cxt = (IOTD_CXT_T*)pCxt;

    IOTD_LOG (LOG_TYPE_INFO,"IPCManager deinit\n");

    if(cxt->ipc_cxt.rx_thread)
        pthread_cancel(cxt->ipc_cxt.rx_thread);
    if(cxt->ipc_cxt.rx_thread)
        pthread_cancel(cxt->ipc_cxt.rx_thread);

    mq_close(cxt->ipc_cxt.iotd_server);
    mq_unlink(cxt->ipc_cxt.server_q);
    return IOTD_OK;
}

