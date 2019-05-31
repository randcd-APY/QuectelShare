/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* size to make read buffer */
#define MAX_BUF 64

/* name of named pipe to read from */
#define FIFO_NAME "/tmp/homesecuritydev"

/* owning user of the named pipe */
#define UID 0

/* owning group of the named pipe */
#define GID 1301

/* modes */
#define MODE_SHUTDOWN "SHUTDOWN"
#define MODE_OFF "OFF"
#define MODE_STAY "STAY"
#define MODE_AWAY "AWAY"

/* events */
#define EVENT_DOOR "DOOR"
#define EVENT_MOTION "MOTION"
#define EVENT_PANIC "PANIC"
#define EVENT_DONT_PANIC "HANG_UP"
#define EVENT_CODE "CODE"

/* states */
#define SYSTEM_OFF 1
#define SYSTEM_STAY 2
#define SYSTEM_AWAY 3
#define SYSTEM_DOOR 4
#define SYSTEM_MOTION 5
#define SYSTEM_PANIC 6
#define SYSTEM_CODE 7
#define SYSTEM_DONT_PANIC 42

/* extras */
#define TRUE 1
#define FALSE 0