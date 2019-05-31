/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _HTC_EVENT_SIGNAL_H_
#define _HTC_EVENT_SIGNAL_H_

#include <pthread.h>

typedef struct _htc_event_signal_s {
	pthread_cond_t  cond;
	pthread_mutex_t mutex;
	uint32_t   cond_var;
} htc_event_signal_t;

#define	HTC_EVENT_WAIT_ALL 		0x01
#define	HTC_EVENT_WAIT_ANY      0x00
#define HTC_EVENT_WAIT_MASK		0x01

#define	HTC_EVENT_CLEAR_ALL 	0x02
#define	HTC_EVENT_CLEAR_NONE    0x00
#define	HTC_EVENT_CLEAR_MASK    0x02

#define HTC_EVENT_WAIT_ANY_CLEAR_ALL	(HTC_EVENT_WAIT_ANY | HTC_EVENT_CLEAR_ALL)


#define	TARGET_INIT_DONE		0x01

#define EVENT_TASK_WAKEUP		0x01

htc_event_signal_t *htc_signal_init();
uint32_t htc_signal_wait(htc_event_signal_t *spitool_signal, 
								uint32_t wait_for_cond, uint32_t type);
uint32_t htc_signal_set(htc_event_signal_t *spitool_signal, uint32_t set_cond);
#endif
