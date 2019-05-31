/******************************************************************************
#@file    log_util.h
#@brief   Provides interface for clients to access logging functionality
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef LOG_UTIL_H
#define LOG_UTIL_H

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

#define TRUE 1
#define FALSE 0
#define MAX_LOG_FILE 3
#define LOG_FILE_NAME "mcm-log_"
#define LOG_FILE_EXTN ".txt"
#define LOG_CONFIG_FILE "/data/mcm-core/log_config.txt"
#define LOG_CONFIG_FILE_DIR "/data/mcm-core/"
#define LOG_FILE_CAPTURE_DIR "/data/mcm-core/log/"
#define MAX_LOG_MSG_SIZE                512

void log_util_format_msg( char *buf_ptr, int buf_size, char *fmt, ... );
void write_log_to_file(char *buf);
void acquire_lock();
void release_lock();

#define LOG_MSG_INFO( ...  )   LOG_MSG_FILE( __VA_ARGS__ )
#define LOG_MSG_FILE(fmt, ... )    \
    { \
        char log_buf[ MAX_LOG_MSG_SIZE ];\
        char log_fmt[ MAX_LOG_MSG_SIZE ];\
        acquire_lock(); \
        strlcpy(log_fmt, "%s: ", sizeof(log_fmt));\
        strlcat(log_fmt, fmt, sizeof(log_fmt));\
        log_util_format_msg( log_buf, MAX_LOG_MSG_SIZE, log_fmt, __func__, ##__VA_ARGS__ );\
        write_log_to_file(&log_buf);\
        release_lock();\
    }\

typedef enum {
    LOG_UTIL_NONE = 0,
    LOG_UTIL_DISABLE = LOG_UTIL_NONE, /*Logging Disable*/
    LOG_UTIL_CONSOLE, /*Logging to console*/
    LOG_UTIL_FILE, /*Logging to File*/
    LOG_UTIL_MAX
} log_util_type_e;

/*=========================================================================
  FUNCTION:  log_util_init
===========================================================================*/
/*
   @brief
   Based on the option mentioned in configuration file, it will decide whether
   to "Disable logging" or "Print to console" or "Log to File".
*/

log_util_type_e log_util_init();

/*=========================================================================
  FUNCTION:  log_util_close
===========================================================================*/

void log_util_close();

#endif
