#ifndef _DS_MUX_UTILS_H_
#define _DS_MUX_UTILS_H_

/******************************************************************************

                           DS_MUX_UTILS.H

******************************************************************************/

/******************************************************************************

                                   D S   M U X   U T I L S

                                   H E A D E R   F I L E

  DESCRIPTION
    This is the external header file for the MUX UTILS. This file
    contains all the functions, definitions and data types needed
    for decoding and encoding MUX packets.

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

/*===========================================================================

                      EDIT HISTORY FOR FILE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <netinet/ether.h>
#include <stdbool.h>

#include "ds_mux_types.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

#define DS_MUX_MAX_NUM_OF_FD           12

#define DS_MUX_UTIL_CRC_TABLE_ELEMENTS 256

#define TTY_BREAK                      1
#define TTY_WAKEUP                     0xFD

#define SET_DTR_HIGH                   1
#define SET_DTR_LOW                    0

#define INFO_FLD_LEN_IF_LEN_FLD_IS_ONE_BYTE 128

#define ONE_SECOND 1

#define ds_abort()  abort()

#define ds_assert(a)                                            \
        if (!(a)) {                                             \
            fprintf(stderr, "%s, %d: assertion (a) failed!",    \
                    __FILE__,                                   \
                    __LINE__);                                  \
            ds_abort();                                         \
        }


   /*============================================================
                  Log Message Macros
   =============================================================*/

#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR

    /*============================================================
                  Log Message Print Api's
     =============================================================*/

#undef PRINT_MSG

                  /** Macro to print the log message information. */
#define PRINT_MSG( level, fmtString, x, y, z)                         \
                          MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                                         __FUNCTION__, x, y, z);


#undef LOG_MSG_INFO1
#undef LOG_MSG_INFO2
#undef LOG_MSG_INFO3
#undef LOG_MSG_ERROR


#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
                  {                                                                     \
                    PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
                  }
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
                  {                                                                     \
                    PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
                  }
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
                  {                                                                     \
                    PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
                  }
#define LOG_MSG_INFO1_6( fmtString, a, b, c, d, e, f)                 \
                  {                                                                     \
                    PRINT_MSG_6 ( LOG_MSG_INFO1_LEVEL, fmtString, a, b, c, d, e, f);    \
                  }
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
                  {                                                                     \
                    PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
                  }
#define LOG_MSG_ERROR_6( fmtString, a, b, c, d, e, f)                 \
                  {                                                                     \
                    PRINT_MSG_6( LOG_MSG_ERROR_LEVEL, fmtString, a, b, c, d, e, f);     \
                  }


#define DS_MUX_MEM_FREE(mem_ptr)                     \
                  free(mem_ptr)

/*===========================================================================
                              VARIABLE DECLARARTIONS
===========================================================================*/
/*---------------------------------------------------------------------------
   Function pointer registered with the socket listener
   This function is used for reading from a socket on receipt of an incoming
   netlink event
---------------------------------------------------------------------------*/
typedef int (* ds_mux_sock_thrd_fd_read_f) (int fd);

typedef enum
{
  DS_MUX_T2_TIMER = 0x0,
  DS_MUX_PSC_TIMER,
  DS_MUX_UART_INACTIVITY_TIMER
}ds_mux_timer_type;

typedef struct
{
  ds_mux_timer_type timer_id_type;
  void* user_data;
}ds_mux_timer_common_data;

typedef struct
{
    timer_t timer_id;
    boolean is_running;
}ds_mux_timer_data_t;

typedef struct
{
  ds_mux_timer_common_data  uart_inactivity_timer_data;
  ds_mux_timer_data_t       uart_inactivity_timer_config;
  boolean                   uart_in_sleep;
  int_32                    timer_val;
}ds_mux_uart_timer_config;

/*--------------------------------------------------------------------------
   Stores the mapping of a socket descriptor and its associated read
   function
---------------------------------------------------------------------------*/
typedef struct
{
 int sk_fd;
 ds_mux_sock_thrd_fd_read_f read_func;
} ds_mux_sk_fd_map_info_t;

/*--------------------------------------------------------------------------
   Stores the socket information associated with netlink sockets required
   to listen to netlink events
---------------------------------------------------------------------------*/
typedef struct
{
 ds_mux_sk_fd_map_info_t sk_fds[DS_MUX_MAX_NUM_OF_FD];
 fd_set fdset;
 int num_fd;
 int max_fd;
} ds_mux_sk_fd_set_info_t;

/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/

int ds_mux_addfd_map
(
  ds_mux_sk_fd_set_info_t   *fd_set,
  int                            fd,
  ds_mux_sock_thrd_fd_read_f   read_f,
  int maxfds
);

int ds_mux_clear_fd
(
   ds_mux_sk_fd_set_info_t *fd_set,
   int                     fd
);

int ds_mux_atfwd_listener_init
(
  ds_mux_sk_fd_set_info_t*   sk_fdset,
  ds_mux_sock_thrd_fd_read_f read_f,
  ds_mux_sk_info_t*          sk_info,
  int                        max_fds
);


int ds_mux_listener_start
(
  ds_mux_sk_fd_set_info_t    * sk_fd_set
);


uint8 ds_mux_utils_calc_fcs_per_char(uint8          , uint8 );

#endif

