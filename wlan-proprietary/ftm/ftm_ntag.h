/*=========================================================================
                       NFC FTM HEADER File
Description
   This file contains the definitions of the function used to
   test read/write and fd NTAG functionalities.

Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef _FTM_NTAG
#define _FTM_NTAG

#include "msg.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "errno.h"
#include <linux/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"

#define BUFFER_SIZE 16 //EACH BLOCK has 16 bytes of data
#define WRITEDELAY  5
#define LOG_NTAG_ERROR( ... )        printf( __VA_ARGS__ )
#define LOG_NTAG_MESSAGE( ... )      printf( __VA_ARGS__ )
#define NTAG_FD_STATE               _IOW(0xE9, 0x01, unsigned int)
#define NTAG_SET_OFFSET             _IOW(0xE9, 0x02, unsigned int)

/* error codes */
enum {
    NTAG_SUCCESS,
    NTAG_ERROR,
    NTAG_INVALID_LENGTH,
    NTAG_NULL_POINTER,
    NTAG_IOCTL_FAIL,
};

int ftm_ntag_dispatch_test(int argc , char ** argv);

#endif // _FTM_NTAG
