/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* size to make read buffer */
#define MAX_BUF 64

/* name of named pipe to read from */
#define FIFO_NAME "/tmp/packagetrackingdev"

/* owning user of the named pipe */
#define UID 0

/* owning group of the named pipe */
#define GID 1301

/* Default values */
int updateTime = 1;
