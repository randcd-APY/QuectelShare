/******************************************************************************
#@file    mcm_ssr_util.h
#@brief   To notify SSR status to client
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef MCM_SSR_UTIL_H
#define MCM_SSR_UTIL_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define SSR_CLIENT_THREAD_NAME "mcm_ssr_client_thread"
#define SSR_SOCKET_PATH "/dev/mcm_ssr"
#define SSR_CLIENT_NUM_LISTEN_QUEUE 20

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

typedef void (*ssr_status_cb)(void);

typedef enum {
  MCM_SSR_RADIO_STATE_UNAVAILABLE_V01 = 0, /**< Radio unavailable. */
  MCM_SSR_RADIO_STATE_AVAILABLE_V01 = 1 /**< Radio available. */
}mcm_ssr_radio_state_t;

uint8_t mcm_ssr_client_init(void);
uint8_t mcm_ssr_client_send_radio_available(void);
uint8_t create_client_attach_thread(void);
void *client_attach_thread_func(void *buf);
uint8_t mcm_ssr_server_init(ssr_status_cb cb);
void mcm_ssr_reset_client_reported_status(void);
uint8_t mcm_ssr_is_client_reported_status(void);
void acquire_lock(void);
void release_lock(void);

#endif