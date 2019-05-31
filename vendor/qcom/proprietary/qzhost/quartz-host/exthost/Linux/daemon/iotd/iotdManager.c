/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <time.h>
#include <syslog.h>
#include <errno.h>
#include <signal.h>
#include "iotd_context.h"
#include "mml.h"
#include "bufpool.h"
#include "iotdManager.h"
#include "ipcManager.h"
#include "qsCommon.h"
#include "qapi_ver.h"
#include "unistd.h"

/****************************************************************************************************/
extern int run_daemon; 

/****************************************************************************************************/
static void *build_iotdMgmt_cmdPacket(void* pCxt, uint8_t cmd, uint8_t* cmd_buf, uint16_t cmd_buf_size);

/****************************************************************************************************/
    
/****************************************************************************************************/        
/*
 * Function:
 * Input:
 * Returns:
 *
 */
static void *build_iotdMgmt_cmdPacket(void* pCxt, uint8_t cmd, uint8_t* cmd_buf, uint16_t cmd_buf_size)
{
    IOTDMGMT_CXT_T* pMgmtCxt = GET_MGMT_CXT(pCxt);
    int pkt_size;
    uint8_t *ptr, *ptr_buf;

    pkt_size = QS_IOTD_HEADER_LEN + sizeof(cmd) + cmd_buf_size;
    while(1)
    {
        if( (ptr = (uint8_t *)buf_alloc(pkt_size)) != NULL )
            break;
        usleep(1000);
    }

    ptr_buf = ptr;
    /* init header */
    memset(ptr, 0, QS_IOTD_HEADER_LEN);

    /* HTC Header */
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16((uint16_t *) ptr_buf, pkt_size);

    /* MML Heater */
    ptr_buf += HTC_HEADER_LEN;
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(ptr_buf, pMgmtCxt->map.q_id[0]);

    ptr_buf = ptr+  QS_IOTD_HEADER_LEN;

    /* fill packet */
    /* set command */
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8( ptr_buf, cmd);
    if( cmd_buf_size > 0 )
        memcpy(ptr_buf + sizeof(cmd), cmd_buf, cmd_buf_size);

    return ptr;
}

static void iotdManagement_Timer_Callback (union sigval sig_val)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*) sig_val.sival_ptr;
    send_server_exit(iotdCxt, QS_TARGET_ASSERT);    
    IOTD_LOG (LOG_TYPE_CRIT,"Iotd Manager fail to get Heart Beat from target \n");
    sleep(1);
    run_daemon = 0;
}


/*
 * Function: iotdManagement_Timer_Create
 * Input: IOTD Context
 * Returns:
 *
 */
static int32_t iotdManagement_Timer_Create(void* pCxt)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt; 
    IOTDMGMT_CXT_T* pMgmtCxt = GET_MGMT_CXT(iotdCxt);
    cfg_ini* cfg = &(iotdCxt->cfg);    
    struct sigevent timer_se;
    struct itimerspec ts;
    
    timer_se.sigev_notify = SIGEV_THREAD;
    timer_se.sigev_value.sival_ptr = pCxt;
    timer_se.sigev_notify_attributes = NULL;
    timer_se.sigev_notify_function = iotdManagement_Timer_Callback;
    
    /* create timer to monitor heart beat  */
    if( timer_create(CLOCK_REALTIME, &timer_se, &(pMgmtCxt->timer_id)) != 0 )
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: Fail to create timer\n");
        return IOTD_ERROR;   
    }

    ts.it_value.tv_sec = cfg->config_system.heart_beat_interval;    
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    timer_settime(pMgmtCxt->timer_id, 0, &ts, 0);

    return IOTD_OK;    
}

/*
 * Function: iotdManagement_thread
 * Input: IOTD context
 * Returns:
 *
 */
void* iotdManagement_thread(void* arg)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)arg;
    IOTDMGMT_CXT_T* pMgmtCxt = GET_MGMT_CXT(iotdCxt);
    cfg_ini* cfg = &(iotdCxt->cfg);     
    struct itimerspec ts;
    qapi_FW_Ver_t *ver;
    uint8_t *buf = NULL;
    uint8_t q_id = 0;
    int i = 0;
    
    if(!pMgmtCxt)
    {
        pthread_exit(arg);
    }
    IOTD_LOG(LOG_TYPE_INFO,"Starting iotdManagement_thread\n");
   /* send hello to target */
    if( iotdManagement_Cmd_Hello(iotdCxt) != IOTD_OK )
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Command Send Failed\n");
        pthread_exit(arg);        
    }

    while(1)
    {
        sem_wait(&(pMgmtCxt->rx_sem));

        /*Iterate through all the queues that are associated with this interface*/
        for(i = 0; i < pMgmtCxt->map.num_q; i++)
        {
            /* Get queue ID from the registered service QIDs */
            q_id = GET_Q_ID(pMgmtCxt->map.q_id[i]);
            while((buf = mml_dequeue(iotdCxt, q_id, IOTD_DIRECTION_RX)) != NULL)
            {
                uint8_t cmdType = GET_MGMT_CMD_TYPE(buf);

                //IOTD_LOG(0,"Iotd Manager: Command %d, status: %d\n",cmdType, GET_MGMT_RESP_STATUS(buf));
                switch(cmdType)
                {
                    case MGMT_MSG_HELLO:
                        ver = (qapi_FW_Ver_t *) &buf[MGMT_CMD_TYPE_OFFSET+2];
                        pMgmtCxt->target_ver.qapi_Version_Number = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&ver->qapi_Version_Number);
                        pMgmtCxt->target_ver.crm_Build_Number = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&ver->crm_Build_Number);
                        IOTD_LOG(LOG_TYPE_CRIT, "Iotd Manager: Recv MGMT_MSG_HELLO resp\n");
                        IOTD_LOG(LOG_TYPE_CRIT, "Target QAPI Ver: %d.%d.%d    CRM  Num: %d\n",
                                    (ver->qapi_Version_Number&__QAPI_VERSION_MAJOR_MASK)>>__QAPI_VERSION_MAJOR_SHIFT,
                                    (ver->qapi_Version_Number&__QAPI_VERSION_MINOR_MASK)>>__QAPI_VERSION_MINOR_SHIFT,
                                    (ver->qapi_Version_Number&__QAPI_VERSION_NIT_MASK)>>__QAPI_VERSION_NIT_SHIFT,
                                    ver->crm_Build_Number);
                        pthread_mutex_lock(&pMgmtCxt->lock);
                        pMgmtCxt->target_initialized = 1;
                        pthread_cond_signal(&pMgmtCxt->cond);
                        pthread_mutex_unlock(&pMgmtCxt->lock);

                        break;

                    case MGMT_MSG_RESET:
                        //IOTD_LOG(0, "Iotd Manager: Recv MGMT_MSG_RESET resp\n");
                        break;

                    case MGMT_MSG_GET_STATUS:
                        pMgmtCxt->target_status = (uint8_t) buf[MGMT_CMD_TYPE_OFFSET+2];
                        //IOTD_LOG(0, "Iotd Manager: Recv MGMT_MSG_GET_STATUS resp\n");
                        break;

                    case MGMT_MSG_GET_VERSION:
                        pMgmtCxt->target_ver.qapi_Version_Number = READ_UNALIGNED_LITTLE_ENDIAN_UINT32((&buf[MGMT_CMD_TYPE_OFFSET+2]));
                        pMgmtCxt->target_ver.crm_Build_Number = READ_UNALIGNED_LITTLE_ENDIAN_UINT32((&buf[MGMT_CMD_TYPE_OFFSET+2+4]));
                        //IOTD_LOG(0, "Iotd Manager: Recv MGMT_MSG_GET_VERSION resp\n");
                        break;

                    case MGMT_MSG_HEART_BEAT:
                        if( pMgmtCxt->timer_start != 0 ) {
                                /* receive heart beat from target, reset monitor timer */
                                ts.it_value.tv_sec = cfg->config_system.heart_beat_interval;    
                                ts.it_value.tv_nsec = 0;
                                ts.it_interval.tv_sec = 0;
                                ts.it_interval.tv_nsec = 0;
                                timer_settime(pMgmtCxt->timer_id, 0, &ts, 0);
                        }
                        //IOTD_LOG(0, "Iotd Manager: Recv HB\n");
                        break;

                    case MGMT_MSG_ECHO:
                        /* receive echo response  */
                        pMgmtCxt->recv_len = pMgmtCxt->buf_len;
                        /* echo response format */
                        /* cmd :   1 Byte */
                        /* status: 1 Byte */
                        /* echo mode: 1 Byte */
                        /*  ....:   echo data */
                        if( pMgmtCxt->test_mode == IOTD_TEST_ECHO_MODE_LOOPBACK ) {
                            memcpy(pMgmtCxt->buf_r, buf+QS_IOTD_HEADER_LEN+2, pMgmtCxt->recv_len);
                            pthread_cond_signal(&pMgmtCxt->cond);
                        }
                        //IOTD_LOG(0, "Iotd Manager: ECHO\n");
                        break;
                     case MGMT_MSG_DBG:
			{
			    char* data = GET_MGMT_RESP_DATA(buf);
			    printf("QZ LOG: %s\n",data);
  			    break;   
			}
                    default:
                        //IOTD_LOG(0,"Iotd Manager: Invalid command %d\n",cmdType);
                        break;
                }
                buf_free(buf);
                buf = NULL;
            }
        }
    }
}

/*
 * Function: iotdManagement_queue_init
 * Input: IOTD Context
 * Returns:
 *
 */
static int32_t iotdManagement_queue_init(void* pCxt)
{
    IOTD_CXT_T* cxt = (IOTD_CXT_T*)pCxt;
    IOTDMGMT_CXT_T* pMgmtCxt = GET_MGMT_CXT(cxt);
    uint8_t q_id = 0;
    int i;
    
    for(i=0; i<pMgmtCxt->map.num_q; i++)
    {
        /* Get queue ID from the registered service QIDs */
        q_id = GET_Q_ID(pMgmtCxt->map.q_id[i]);
        if(IOTD_OK != mml_open_q(pMgmtCxt->iotd_cxt, q_id, IOTD_DIRECTION_RX,  &(pMgmtCxt->rx_sem)))
        {
            return IOTD_ERROR;
        }
    }
    return IOTD_OK;
}

/*
 * Function: iotdManagement_Cmd_Reset
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t iotdManagement_Cmd_Reset(void* pCxt)
{
    void *rxBuf = build_iotdMgmt_cmdPacket(pCxt, (uint8_t)MGMT_MSG_RESET, NULL, 0);
    if( rxBuf == NULL) return IOTD_ERROR;
    if(IOTD_OK != mml_enqueue(pCxt, rxBuf, IOTD_DIRECTION_TX))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: Send cmd reset failed\n");
        return IOTD_ERROR;
    }
    IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: Send cmd reset\n");
    return IOTD_OK;
}

/*
 * Function: iotdManagement_Cmd_GetStatus
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t iotdManagement_Cmd_GetStatus(void* pCxt)
{
    void *rxBuf = build_iotdMgmt_cmdPacket(pCxt, (uint8_t)MGMT_MSG_GET_STATUS, NULL, 0);
    if( rxBuf == NULL) return IOTD_ERROR;
    if(IOTD_OK != mml_enqueue(pCxt, rxBuf, IOTD_DIRECTION_TX))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: Send cmd GetStatus failed\n");
        return IOTD_ERROR;
    }
    IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: send cmd GetStatus\n");
    return IOTD_OK;
}

/*
 * Function: iotdManagement_Cmd_Echo
 * Input: IOTD Context
 *
 *        echo_buf: echo data within echo command to send
 *        echo_buf[0]: ----  echo parameter
 *                     0 : target need echo
 *                     1 : target drop the packet and response 
 *        echo_buf_size: echo data length to send
 *
 * Returns:
 *
 */
int32_t iotdManagement_Cmd_Echo(void* pCxt, uint8_t* echo_buf, uint16_t echo_buf_size)
{
    void *rxBuf = build_iotdMgmt_cmdPacket(pCxt, (uint8_t)MGMT_MSG_ECHO,echo_buf, echo_buf_size);
    if( rxBuf == NULL) return IOTD_ERROR;
    if(IOTD_OK != mml_enqueue(pCxt, rxBuf, IOTD_DIRECTION_TX))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: Send Echo Cmd failed\n");
        return IOTD_ERROR;
    }
    //IOTD_LOG(0,"Iotd Manager: Send Echo Cmd\n");
    return IOTD_OK;
}

/*
 * Function: iotdManagement_Cmd_Hello
 * Input: IOTD Context
 * Returns:
 *
 */
int32_t iotdManagement_Cmd_Hello(void* pCxt)
{
    void *rxBuf = build_iotdMgmt_cmdPacket(pCxt, (uint8_t)MGMT_MSG_HELLO, NULL, 0);
    if( rxBuf == NULL) return IOTD_ERROR;
    if(IOTD_OK != mml_enqueue(pCxt, rxBuf, IOTD_DIRECTION_TX))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: Send cmd hello failed\n");
        return IOTD_ERROR;
    }
    IOTD_LOG(LOG_TYPE_INFO,"Iotd Manager: Send cmd hello\n");
    return IOTD_OK;
}

/*
 * Function: iotdManagement_init
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t iotdManagement_init(void* pCxt)
{
    struct timespec time_to_wait = {0, 0};
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    IOTDMGMT_CXT_T* pMgmtCxt = GET_MGMT_CXT(iotdCxt);
    cfg_ini* cfg;
    int i;
    
    if(iotdCxt == NULL)
    {
        return IOTD_ERROR;
    }

    pMgmtCxt->run_once = 0;
    
    cfg = &(iotdCxt->cfg);
	memset(pMgmtCxt, 0, sizeof(IOTDMGMT_CXT_T));
    pMgmtCxt->iotd_cxt = iotdCxt;
    
    /* Initialize module rx semaphore */
    sem_init(&(pMgmtCxt->rx_sem), 0, 1);

    pthread_mutex_init(&pMgmtCxt->lock, NULL);
    pthread_cond_init(&pMgmtCxt->cond, NULL);
    
    /* Get info from config file*/
    pMgmtCxt->map.num_q = cfg->config_mgmt.num_service_q;
    for(i = 0; i < pMgmtCxt->map.num_q; i++){
        pMgmtCxt->map.q_id[i] = cfg->config_mgmt.qid[i];
    }

    /* Set other q_id to 255 to indicate that these aren't used */
    for(i = pMgmtCxt->map.num_q; i < IOTD_MAX_NUM_Q; i++){
        pMgmtCxt->map.q_id[i] = 255;
    }

    /***********************************************************/
    if(IOTD_OK != iotdManagement_queue_init(pCxt))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"Iotd Manager: Queue initialization failed\n");
        exit(1);
    }

    /* Initialize thread to process management packets */
    if(pthread_create(&(pMgmtCxt->mgmt_thread),NULL, iotdManagement_thread, pCxt) != 0)
    {
        perror("Iotd Manager: Thread creation failed\n");
        exit(1);
    }

     
    /* wait hello response from target */
    pthread_mutex_lock(&pMgmtCxt->lock);
    while(!pMgmtCxt->target_initialized)
    {
        time_to_wait.tv_sec = time(NULL) + 30;
        if(pthread_cond_timedwait(&pMgmtCxt->cond, &pMgmtCxt->lock, &time_to_wait) != 0 )
        {
            IOTD_LOG (LOG_TYPE_CRIT,"Iotd Manager fail to get response from target \n");
            pthread_mutex_unlock(&pMgmtCxt->lock);
            return IOTD_ERROR; 
        }
    }
    pthread_mutex_unlock(&pMgmtCxt->lock);

    if(cfg->config_system.heart_beat_enable)
    {
        if(iotdManagement_Timer_Create(pCxt) != IOTD_OK )
        {
            perror("Iotd Manager: Timer creation failed\n");
            exit(1);
        }
        pMgmtCxt->timer_start = 1;
        IOTD_LOG(LOG_TYPE_INFO,"Heart Beat Monitor is enabled\n");
    } else {
        pMgmtCxt->timer_start = 0;
        IOTD_LOG(LOG_TYPE_INFO,"Heart Beat Monitor is disabled\n");
    }
    
    IOTD_LOG (LOG_TYPE_INFO,"Iotd Manager initialized.\n");
    return IOTD_OK;
}

/*
 * Function: iotdManagement_deinit
 * Input: IOTD context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t iotdManagement_deinit(void* pCxt)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    IOTDMGMT_CXT_T* pMgmtCxt = GET_MGMT_CXT(iotdCxt);

    pthread_cancel(pMgmtCxt->mgmt_thread);
    if(pMgmtCxt->timer_start)
    {
        timer_delete(pMgmtCxt->timer_id);
        pMgmtCxt->timer_start = 0;
    }
    pthread_mutex_destroy(&pMgmtCxt->lock);
    pthread_cond_destroy(&pMgmtCxt->cond);
    IOTD_LOG (LOG_TYPE_INFO,"Iotd Manager deinit\n");

    return IOTD_OK;
}
