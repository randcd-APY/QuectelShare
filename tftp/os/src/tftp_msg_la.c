/***********************************************************************
 * tftp_msg_la.c
 *
 * Short description.
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header$ $DateTime$ $Author$

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-07-28   rp    Remove unused MAX_LOG_LEVEL.
2014-07-18   rp    tftp and ipc-router integration changes from target.
2014-06-30   nr    Support connected sockets and muti-poll.
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_comdef.h"
#include "tftp_msg.h"
#include "tftp_assert.h"
#include "tftp_threads_la.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>

#if !defined (TFTP_LA_BUILD)
  #error "This file is to be included only in LA builds."
#endif

#define LOG_NDEBUG 0
#define LOG_NDDEBUG 0
#define LOG_NIDEBUG 0

#define LOG_TAG "tftp_server"

#ifdef TFTP_LE_BUILD_ONLY
  #include <sys/syslog.h>

  #define LOGI(...) syslog(LOG_NOTICE, "INFO:"__VA_ARGS__)
  #define LOGV(...) syslog (LOG_NOTICE,"VERB:" __VA_ARGS__)
  #define LOGD(...) syslog (LOG_DEBUG,"DBG:"__VA_ARGS__)
  #define LOGE(...) syslog (LOG_ERR,"ERR:"__VA_ARGS__)
  #define LOGW(...) syslog (LOG_WARNING,"WRN:"__VA_ARGS__)
#else
  #include "cutils/log.h"
  #include "common_log.h"
#endif /* TFTP_LE_BUILD_ONLY */

/*
 *Ring Buffer Implementation*
 */

#define TFTP_RING_BUFFER_SIZE 256
#define TFTP_RING_BUFFER_LENGTH 256
#define TFTP_RING_BUFFER_TIMESTAMP_LENGTH 20

#define TFTP_RMSGI(format, ...) tftp_rmsg("INFO " format "\n", ##__VA_ARGS__)

static char tftp_debug_ring_buffer[TFTP_RING_BUFFER_SIZE][TFTP_RING_BUFFER_LENGTH];

void tftp_rmsg(const char *fmt, ...);

struct tftp_rmsg_desc {
    int index;
    pthread_mutex_t index_mutex;
};
static struct tftp_rmsg_desc rmsg;

void tftp_rmsg_init(void)
{
    pthread_mutex_init(&rmsg.index_mutex, NULL);
    rmsg.index = 0;
    LOGI("Initializing tftp_server RING buffer");

}

#ifndef TFTP_SIMULATOR_BUILD
int tftp_fill_time_buffer(char **buffer)
{
    int len = 0;
    int ret;
    struct timespec cur_time;

    ret = clock_gettime(CLOCK_BOOTTIME, &cur_time);
    if (ret != 0) {
	cur_time.tv_sec = -EINVAL;
	cur_time.tv_nsec = -EINVAL;
    }
    len = snprintf(*buffer, TFTP_RING_BUFFER_TIMESTAMP_LENGTH, "[%ld.%ld]", cur_time.tv_sec, cur_time.tv_nsec);

    return len;
}
#else
int tftp_fill_time_buffer(char **buffer)
{
    return 0;
}
#endif

void tftp_rmsg(const char *format, ...)
{
	va_list args;
	int len = 0, index;
	char *buffer;

	pthread_mutex_lock(&rmsg.index_mutex);

	if (rmsg.index >= TFTP_RING_BUFFER_LENGTH)
	    rmsg.index = 0;

	index = rmsg.index ;
	rmsg.index++;
	pthread_mutex_unlock(&rmsg.index_mutex);

	buffer = tftp_debug_ring_buffer[index];
	buffer[TFTP_RING_BUFFER_TIMESTAMP_LENGTH] = 0;

	len = tftp_fill_time_buffer(&buffer);
	buffer = buffer + len;

	va_start(args, format);
	vsnprintf(buffer, TFTP_RING_BUFFER_SIZE - len, format, args);
	va_end(args);

	return ;
}

void
tftp_msg_init (void)
{
  #ifdef TFTP_LE_BUILD_ONLY
    openlog(LOG_TAG, LOG_PID, LOG_USER);
  #endif /* TFTP_LE_BUILD_ONLY */

  tftp_rmsg_init();
  LOGI("Starting...\n");
}

void
tftp_msg_print (enum tftp_log_msg_level_type msg_level, const char *msg_str)
{
  pid_t mypid, mytid;

  mypid = syscall(SYS_getpid);
  mytid = syscall(SYS_gettid);

  switch (msg_level)
  {
    case TFTP_LOG_MSG_LEVEL_ERROR:
      LOGE ("pid=%u tid=%u tftp-server : %s", mypid, mytid, msg_str);
      break;

    case TFTP_LOG_MSG_LEVEL_WARN:
      LOGW ("pid=%u tid=%u tftp-server : %s", mypid, mytid, msg_str);
      break;

    case TFTP_LOG_MSG_LEVEL_DEBUG:
      LOGD ("pid=%u tid=%u tftp-server : %s", mypid, mytid, msg_str);
      break;

    case TFTP_LOG_MSG_LEVEL_INFO:
      LOGI ("pid=%u tid=%u tftp-server : %s", mypid, mytid, msg_str);
      break;

    default:
      TFTP_ASSERT (0);
      break;
  }
  TFTP_RMSGI ("pid=%u tid=%u tftp-server : %s", mypid, mytid, msg_str);
}
