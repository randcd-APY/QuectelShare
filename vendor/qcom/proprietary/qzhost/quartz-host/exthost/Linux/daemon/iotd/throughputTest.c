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
#include "htc.h"

/*****************************************************************************************************
 *                                    throughput test                                                *
*****************************************************************************************************/ 

/****************************************************************************************************/        
/*
 * Function: run_throughput_test
 * Input:
 * Returns:
 *
 */
int run_throughput_test(void *pCxt)
{
    
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    IOTDMGMT_CXT_T* pMgmtCxt = GET_MGMT_CXT(iotdCxt);
    HTC_CXT_T* pHTCCxt = GET_HTC_CXT(pCxt);
    cfg_ini* cfg = &(iotdCxt->cfg); 
    HTC_IFACE_CXT_T* pIfaceCxt = pHTCCxt->iface_cxt[0];
    long start_send, start_recv;
    long len = 0;
    int i, interval;
    struct timespec  end, tm ;
    struct timespec time_to_wait = {0, 0};

    /* if throughput_test is disable, return */
    if(cfg->config_system.throughput_test_enable == 0)
        return 1;
    
    /* only run one time */
    if( pMgmtCxt->run_once > 0 ) 
        return 1;
    
    pMgmtCxt->run_once++;
    
    /*init test setting */
    interval = cfg->config_system.throughput_test_interval;  // test interval
    pMgmtCxt->buf_len = cfg->config_system.throughput_test_packet_len;            // send and receive buffer length
    pMgmtCxt->test_mode = cfg->config_system.throughput_test_mode; 

    pMgmtCxt->send_count = pMgmtCxt->recv_count=0;
    
    /* init buffer */
    pMgmtCxt->buf_s = malloc(pMgmtCxt->buf_len+QS_IOTD_HEADER_LEN);
    pMgmtCxt->buf_r = malloc(pMgmtCxt->buf_len+QS_IOTD_HEADER_LEN);
    if( (pMgmtCxt->buf_s == NULL) || (pMgmtCxt->buf_r == NULL) )
    {
        printf("Error to malloc....\n");
        return -1;
    } 
    
    /* init sending buffer */
    for(i=0; i < pMgmtCxt->buf_len; i++)
    {
        pMgmtCxt->buf_s[i] = (uint8_t ) i & 0xff;
    }

    // set test mode at sending package 
    if(pMgmtCxt->test_mode == IOTD_TEST_ECHO_MODE_SENDONLY) {
        pMgmtCxt->buf_s[0] = IOTD_TEST_ECHO_MODE_SENDONLY;
    } else if(pMgmtCxt->test_mode == IOTD_TEST_ECHO_MODE_LOOPBACK) {
        pMgmtCxt->buf_s[0] = IOTD_TEST_ECHO_MODE_LOOPBACK;
    } 
        
    //set target time
    clock_gettime(CLOCK_REALTIME, &end);
    end.tv_sec += interval;
    
    htc_get_statistics(pIfaceCxt, &start_send, &start_recv);
    printf("throughput test start with %s mode\n", (pMgmtCxt->test_mode == IOTD_TEST_ECHO_MODE_LOOPBACK)?"loopback":"send only");
    
    while(1) {
        clock_gettime(CLOCK_REALTIME, &tm);
        if( (tm.tv_sec >= end.tv_sec) && (tm.tv_nsec >= end.tv_nsec) )
            break;
        /* send echo command with test data */
        pMgmtCxt->buf_s[1]= (uint8_t) pMgmtCxt->send_count;
        if( iotdManagement_Cmd_Echo(pCxt, pMgmtCxt->buf_s, pMgmtCxt->buf_len) == -1)
        {
            printf("\nFail to send test data ....\nTest abort\n");
            break;
        }
        printf(".");
        
        if( pMgmtCxt->test_mode == IOTD_TEST_ECHO_MODE_LOOPBACK )
        {
            time_to_wait.tv_sec = time(NULL) + 3;
            /* wait echo response from target */
            if( pthread_cond_timedwait(&pMgmtCxt->cond, &pMgmtCxt->lock, &time_to_wait) == ETIMEDOUT )
            {
                iotd_log (0,"Iotd Manager fail to get echo response from target\n");
                break;
            }        

            /* compare receive data */
            if( memcmp(pMgmtCxt->buf_s, pMgmtCxt->buf_r, pMgmtCxt->buf_len) != 0 )
            {
                printf("\nReceived data don't match original data\nTest abort\n");
                break;
            }
            len += pMgmtCxt->recv_len;
        } 
        pMgmtCxt->send_count++;
    }

    htc_get_statistics(pIfaceCxt, &(pMgmtCxt->send_count), &(pMgmtCxt->recv_count));
    
    pMgmtCxt->send_count -= start_send;
    pMgmtCxt->recv_count -= start_recv;
    /* print test result */
    if(pMgmtCxt->test_mode == IOTD_TEST_ECHO_MODE_SENDONLY)
    {
        printf("Sent %ld bytes with %d packet size in %d seconds (%ldkbps)\n", 
                    pMgmtCxt->send_count , 
                    pMgmtCxt->buf_len+IOTD_HEADER_LEN, 
                    interval, 
                    pMgmtCxt->send_count/interval/1000*8);
    } else {  /* test_mode == IOTD_TEST_ECHO_MODE_LOOPBACK */
        printf("\nSent %ld bytes and Received %ld bytes with %d packet size in %d seconds (%ldkbps)\n", 
                    pMgmtCxt->send_count, 
                    pMgmtCxt->recv_count, 
                    pMgmtCxt->buf_len+IOTD_HEADER_LEN, 
                    interval, 
                    (pMgmtCxt->send_count+ pMgmtCxt->recv_count)/interval/1000*8);
    }

    /* force callback return at once */
    pMgmtCxt->test_mode = IOTD_TEST_ECHO_MODE_LOOPBACK;
    /* test is done */
    if( pMgmtCxt->buf_s != NULL)
    {
        free(pMgmtCxt->buf_s);
        pMgmtCxt->buf_s = NULL;
    }
    if( pMgmtCxt->buf_r != NULL)
    {
        free(pMgmtCxt->buf_r);
        pMgmtCxt->buf_r = NULL;
    }

    return 1;
}
/*****************************************************************************************************
 *                                    end throughput test                                            *
******************************************************************************************************/ 

