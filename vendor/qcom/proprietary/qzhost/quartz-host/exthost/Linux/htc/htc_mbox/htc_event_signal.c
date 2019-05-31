/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
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

#include <pthread.h>
#include <unistd.h>

#include  "htc_event_signal.h"

htc_event_signal_t *htc_signal_init()
{
	htc_event_signal_t *evt_signal;

	evt_signal = malloc(sizeof(htc_event_signal_t));
	if (evt_signal == NULL)
		return NULL;
	
	evt_signal->cond_var = 0;
	pthread_mutex_init(&evt_signal->mutex, NULL);
	pthread_cond_init(&evt_signal->cond, NULL);
	
	return  evt_signal;
}

uint32_t htc_signal_wait(htc_event_signal_t *evt_signal, 
								uint32_t wait_for_cond, uint32_t type)
{
	uint32_t var32;
	
	pthread_mutex_lock(&evt_signal->mutex); //mutex lock
	
	if ((type & HTC_EVENT_WAIT_MASK) == HTC_EVENT_WAIT_ALL)
	{
		while((evt_signal->cond_var & wait_for_cond) != wait_for_cond)
		{
			pthread_cond_wait(&evt_signal->cond, &evt_signal->mutex); //wait for the condition
		}		
	} 
	else
	{
		while((evt_signal->cond_var & wait_for_cond) == 0)
		{
			pthread_cond_wait(&evt_signal->cond, &evt_signal->mutex); //wait for the condition
		}				
	}
	var32 = evt_signal->cond_var;
	if ((type & HTC_EVENT_CLEAR_MASK) == HTC_EVENT_CLEAR_ALL)
		evt_signal->cond_var = 0; 
	
	pthread_mutex_unlock(&evt_signal->mutex);
	
	return var32;
}

uint32_t htc_signal_set(htc_event_signal_t *evt_signal, uint32_t set_cond)
{
	pthread_mutex_lock(&evt_signal->mutex); //mutex lock
	
	evt_signal->cond_var |= set_cond;	
	pthread_cond_signal(&evt_signal->cond); //wait for the condition

	pthread_mutex_unlock(&evt_signal->mutex);
	
	return evt_signal->cond_var;
}
