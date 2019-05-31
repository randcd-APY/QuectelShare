/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <glob.h>           /* Including glob.h for glob() function used in find_pid() */
#include <signal.h>
#include <err.h>
#include <errno.h>
#include <sys/queue.h>
#include <pthread.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <pthread.h>
#include <unistd.h>
#include  "spi_regs.h"
#include  "htc_internal.h"
#include  "hif_spi_rw.h"
#include  "dbg.h"
#include  "hif.h"

#define MAX_BUF 64
sem_t  pkt_avail_signal;
sem_t  pkt_avail_done_signal;
extern uint8_t 	rcv_data_buf[];

/*
 * Function: hif_spi_intr_event_routine
 * Description:    spi interrupt process.
 *                 monitor GPIO device file with poll, when interrupt happen, device file has data that
 *                 can be read, poll return; read the data, make device file ready for next interrupt.
 * Input: 
 *        pdata:     pthread_create pass to this routine, no use here
 * Returns:
 *        none
 *
 */

void *hif_spi_intr_event_routine(void *pdata)
{
    int numEvents;
    char   buf[MAX_BUF];
#ifdef  DEBUG_HTC
    int   len;
#endif
    struct pollfd pollFdArray = {0};

// struct sched_param is used to store the scheduling priority
    struct sched_param params;
    int    policy;
    pthread_t this_thread = pthread_self();
    
    pthread_getschedparam(this_thread, &policy, &params);
// We'll set the priority to the maximum.
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
// Attempt to set thread real-time priority to the SCHED_FIFO policy
    pthread_setschedparam(this_thread, SCHED_FIFO, &params);

	HTCPrintf(HTC_PRINT_GRP5,  "interrupt thread start...\n");
	do
	{
		pollFdArray.fd = htc_spi_ctxt.qzIntFd;
		pollFdArray.events = POLLPRI;

		numEvents = poll(&pollFdArray, 1, -1);  // infinite

#ifdef HTC_TOGGLE_ENABLE
        {
            extern void toggle(void **p, int gpio_pin_num);
            
            toggle(NULL, 49);    
        }
#endif
        
		if ( numEvents <= 0)
            HTCPrintf(HTC_PRINT_GRP5,  "poll error\n");			
			
		if (pollFdArray.revents & POLLPRI) 
		{
            lseek(pollFdArray.fd, 0, SEEK_SET);
#ifdef  DEBUG_HTC
            len = read(pollFdArray.fd, buf, MAX_BUF);
            HTCPrintf(HTC_PRINT_GRP5,  "len = %d\n", len);
#else
            read(pollFdArray.fd, buf, MAX_BUF);
#endif

			hifIRQHandler(NULL);
            HTCPrintf(HTC_PRINT_GRP5,  "prepare to check\n");
        }
	} while(1);

	return NULL;
}

/*
 * Function: hif_spi_int_setup
 * Description:    create SPI interrupt process thread
 *
 * Input: 
 *        none
 * Returns:
 *        none
 *
 */

void hif_spi_int_setup()
{
	char   buf[MAX_BUF];
	pthread_attr_t attr;
	struct sched_param param;
	int wait_event = 0;
	int	iRet;
	int pshared = 0;
	
	if (htc_spi_ctxt.spi_interrupt_init_flag != 0)
	{
		HTCPrintf(HTC_PRINT_GRP5, "spi interrupt thread has been initialized\n");
		return;
	}
	
	HTCPrintf(HTC_PRINT_GRP5, "setup spi interrupt thread...\n");

	sem_init(&pkt_avail_signal, pshared, 0);
	sem_init(&pkt_avail_done_signal, pshared, 0);

	iRet = pthread_attr_init(&attr);
	if (iRet != 0)
	{
		perror("thread attr init fail");
		return;		
	}
	
	iRet = pthread_attr_setstacksize(&attr, SPI_THREAD_STACKSIZE);
	if (iRet != 0)
	{
		perror("set stack size fail");
		return;		
	}
	HTCPrintf(HTC_PRINT_GRP5, "stack size = %d\n", SPI_THREAD_STACKSIZE);

    lseek(htc_spi_ctxt.qzIntFd, 0, SEEK_SET);
    iRet = read(htc_spi_ctxt.qzIntFd, buf, MAX_BUF);
    HTCPrintf(HTC_PRINT_GRP5, "clear INT event len = %d\n", iRet);

    pthread_attr_getschedparam(&attr, &param);
	param.sched_priority++;
    pthread_attr_setschedparam(&attr, &param);
    HTCPrintf(HTC_PRINT_GRP5, "INT thread priority:%d\n", param.sched_priority);
		
    if (pthread_create(&htc_spi_ctxt.thread, &attr, hif_spi_intr_event_routine, &wait_event)) 
	{
        HTCPrintf(HTC_PRINT_GRP5, "can't create thread\n");
        wait_event = 0;
    }
    HTCPrintf(HTC_PRINT_GRP5, "interrupt thread has been created \n");

    iRet = pthread_attr_destroy(&attr);
 
	htc_spi_ctxt.spi_interrupt_init_flag = 1;
}

void hif_spi_deint_setup()
{
	HTCPrintf(HTC_PRINT_GRP5, "hif_spi_deinit\n");
	
	pthread_cancel(htc_spi_ctxt.thread);
}
