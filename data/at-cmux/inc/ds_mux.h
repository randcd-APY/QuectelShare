#ifndef DS_MUX_H
#define DS_MUX_H
/******************************************************************************
                                   D S   M U X
                                   H E A D E R   F I L E

  DESCRIPTION
    This is the external header file for the MUX Task. This file
    contains all the functions, definitions and data types needed for
    MUX processing

    DS MUX 27.010 core protocol and logic is implemented in this file


  @file    ds_mux.h

  DESCRIPTION
  Header file for all the MUXING and DEMUXING operations of DS_MUX_MODULE

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#define CLOCKID CLOCK_REALTIME


#include "ds_mux_types.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

#define INVALID_SLEEP_STATUS -1
#define INVALID_TIMER_STATUS -1
#define UART_INACTIVITY_TIMER_BUF_SZ 10


#define DS_MUX_MIN_BUF_LEN 1024

/* 32768 = 1024*32 in bytes. Where 32768(2^15) is the max data supported */
#define DS_MUX_MAX_BUF_NUM_LEN 32

#define DS_MUX_IPR_BUFF_SZ 20
#define DS_MUX_PERM_RDWR 0666

#define DS_MUX_TIMER_ALLOC  0
#define DS_MUX_TIME_DEALLOC 1

#define MAX_NUM_OF_FD           10

#define DS_MUX_SUCCESS_MACRO    0
#define DS_MUX_FAILURE_MACRO  (-1)

#define UART_MAX_TRANSFER_SIZE         2048

#define MAX_COMMAND_STR_LEN            200

#define OPEN_UART_MODE   1
#define OPEN_USB_MODE    2

/* USB TTY Device Init Delay Micro seconds */
#define USB_DEV_INIT_DELAY 1000000

/* SMD_DATA1 is used for Legacy AT commands */
#define SMD_DATA1 "/dev/smd7" //for usb also same SMD will be used
#define SMD_DATA2 "/dev/smd9"
#define SMD_DATA3 "/dev/smd10"
#define SMD_DATA4 "/dev/smd8"

#define NO_OF_SMD_DATA_CHANNELS 3

#define DS_MUX_LS_FILE "/data/qti_ppp_ls_uds_file"
#define DS_MUX_UNIX_PATH_MAX    108


#define UART_SERIAL_PORT "/dev/ttyHS1" //Serial UART port
#define USB_SERIAL_PORT "/dev/android_serial_device"
#define USB_DUN_TTY_PORT "/dev/ttyGS1"


#define SERVER_TIMER_FILE "/var/run/ds_mux_timer_server"
#define CLIENT_TIMER_FILE "/var/run/ds_mux_timer_client"
#define MAX_BUF_LEN_TIMER_IPC 256

#define SERVER_UART_TIMER_FILE "/var/run/ds_mux_uart_timer_server"
#define CLIENT_UART_TIMER_FILE "/var/run/ds_mux_uart_timer_client"


#define USERIAL_OP_CLK_ON   0x5441
#define USERIAL_OP_CLK_OFF  0x5442
#define USERIAL_OP_CLK_STATE 0x5443

#define DS_MUX_MAX_AT_TOKENS 100

#define DS_MUX_FRM_SZ_EXPT_INFO_FLD   10
#define DS_MUX_CTL_FRM_SZ             31

#define DS_MUX_FEW_EXTRA_BYTES        20


#define SIG  SIGUSR1
#define SIG2 SIGUSR2

#define OK      1
#define ERROR   0

#define MAX_TOKEN_SIZE 20
#define MAX_TOKENS 44

#define TIOCM_DTR 0x002
#define TIOCM_CD  0x040
#define TIOCM_RI  0x080

#define TIOCM_RTS 0x004


#define BAUD_RATE_300      300
#define BAUD_RATE_600      600
#define BAUD_RATE_1200     1200
#define BAUD_RATE_1800     1800
#define BAUD_RATE_2400     2400
#define BAUD_RATE_4800     4800
#define BAUD_RATE_9600     9600
#define BAUD_RATE_19200    19200
#define BAUD_RATE_38400    38400
#define BAUD_RATE_57600    57600
#define BAUD_RATE_115200   115200
#define BAUD_RATE_230400   230400
#define BAUD_RATE_460800   460800

#define DEFAULT_BAUD_RATE_UART_9600 9600
#define DEFAULT_BAUD_RATE_USB_115200 115200

/*--------------------------------------------------------------------------
 DLCI control block: Data types to hold dlci config
---------------------------------------------------------------------------*/

typedef struct
{
   uint8                     dlci_id;
  /**< Self-reference ID   */

  uint_32                     flow_control_mask;
  /**< Flow control mask   */

  ds_mux_dlci_state_enum_type state;
  /**< Currnet connection sate   */
  /*--------------------------------------------------------------------------
    DLCI Parameters, Parameters will be set through PN messages,
    AT +CMUX command or having default values
  --------------------------------------------------------------------------*/
  ds_mux_subset_enum_type  frame_type;
   /**< 0-UIH, 1-UI, 2-I frames.            */
  uint8                    priprity;
   /**< The priority is a number in the range 0-63.
      Not supported, All DLCIs will be having same priority*/
  uint16                   frame_size_N1;
   /**< Max frame size (1-32768).           */
  uint16                   response_timer_T1;
   /**<  Time UE waits for an acknowledgement before
   resorting to other action (e.g. transmitting a frame)
   default 100ms min value 10 ms.
 */
  uint8                     re_transmissions_N2;
   /**< Max re-tries N2 (0-255).            */

  uint8                     curr_transmissions_N2;

  uint16                    response_timer_T2;
   /**< Time mux control channel waits before
 re-transmitting a command default 300ms and
 min value 20 ms.
 */
  uint_32                     wake_up_timer_T3;
   /**<  Time UE waits for response for a
 power wake-up command default 10ms and min
 value 1 ms.
 */
  uint8                       window_size_k;
  /**< Window size default 2, range (1-7).
     Not supported*/

  ds_mux_cl_type_enum_type    cl_type;
  /**< Convergence layer type*/

  ds_mux_io_frame_type *curr_msg_frame;

  ds_mux_timer_data_t  t2_timer_data;

  ds_mux_timer_data_t psc_timer_data;

  /**< Convergence layer type*/
  char*                     pending_rx_cl4_data_ptr;
} ds_mux_dlci_param_type;


typedef struct
{

  ds_mux_state_enum_type       ds_mux_state;
  boolean                      is_initiator;
}ds_mux_state_info_type;

/*-------------------------------------------------------------------------
  DUN message handling status
--------------------------------------------------------------------------*/
typedef struct{
  int                 fd;
  boolean             is_uart_mode;
}tty_config;

/*-------------------------------------------------------------------------
  DUN message handling status
--------------------------------------------------------------------------*/
typedef struct{
  int                  smd_fd;
}ds_mux_smd_tty_config;

typedef struct
{
  int smd_data_fd[NO_OF_SMD_DATA_CHANNELS];
}ds_mux_smd_data_config;

typedef struct{
  int usb_fd;
}ds_mux_usb_tty_config;

/*-------------------------------------------------------------------------
 handling line state of USB
--------------------------------------------------------------------------*/
typedef struct{
  int                  line_state_fd;
}ds_mux_usb_line_state_config;

/*-------------------------------------------------------------------------
  FD Set.
--------------------------------------------------------------------------*/
extern ds_mux_sk_fd_set_info_t   sk_fdset;
extern ds_mux_usb_tty_config   usb_tty_config_info;

typedef struct
{

  uint16   mask;
  /**<  represent fields        */
  ds_mux_mode_enum_type   operating_mode;
  /**< Mode 0 - basic, 1- advanced.        */
  ds_mux_subset_enum_type  subset;
  /**< 0-UIH, 1-UI, 2-I frames.            */
  ds_mux_port_speed_enum_type  port_speed;
  /**< port speed valid values: 1-6.       */
  uint16  frame_size_N1;
  /**< Max frame size (1-32768).           */
  uint16  response_timer_T1;
  /**<  Time UE waits for an acknowledgement before
   resorting to other action (e.g. transmitting a frame)
   default 100ms min value 10 ms.
 */
  uint8   re_transmissions_N2;
  /**< Max re-tries N2 (0-255).            */

  uint16  response_timer_T2;
  /**< Time mux control channel waits before
 re-transmitting a command default 300ms and
 min value 20 ms.
 */
  uint_32  wake_up_timer_T3;
  /**<  Time UE waits for response for a
 power wake-up command default 10ms and min
 value 1 ms.
 */
  uint8   window_size_k;
  /**< Window size default 2, range (1-7). */

} dlci_cmux_param_type;





typedef struct
{

  uint16   mask;
  /**<  represent fields        */
  ds_mux_at_fwd_mode_enum_type   operating_mode;
  /**< Mode 0 - basic, 1- advanced.        */
  ds_mux_at_fwd_subset_enum_type  subset;
  /**< 0-UIH, 1-UI, 2-I frames.            */
  ds_mux_at_fwd_port_speed_enum_type  port_speed;
  /**< port speed valid values: 1-6.       */
  uint16  frame_size_N1;
  /**< Max frame size (1-32768).           */
  uint16  response_timer_T1;
  /**<  Time UE waits for an acknowledgement before
   resorting to other action (e.g. transmitting a frame)
   default 100ms min value 10 ms.
 */
  uint8   re_transmissions_N2;
  /**< Max re-tries N2 (0-255).            */

  uint16  response_timer_T2;
  /**< Time mux control channel waits before
 re-transmitting a command default 300ms and
 min value 20 ms.
 */
  uint_32  wake_up_timer_T3;
  /**<  Time UE waits for response for a
 power wake-up command default 10ms and min
 value 1 ms.
 */
  uint8   window_size_k;
  /**< Window size default 2, range (1-7). */

}at_fwd_cmux_cmd_params_struct;


/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================

FUNCTION DS_MUX_CLEAR_FD()

DESCRIPTION

  This function
  - Removes the fd to the list of FD on which select call listens.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/
int ds_mux_clear_fd
(
   ds_mux_sk_fd_set_info_t *fd_set,
   int                     fd
);

/*===========================================================================

FUNCTION DS_MUX_UART_TTY_LISTENER_INIT()

DESCRIPTION

  This function
  - sets up DS_MUX to start listening for AT commands coming on UART.

DEPENDENCIES
  None.

RETURN VALUE
  DS_MUX_SUCCESS_MACRO on success
  DS_MUX_FAILURE_MACRO on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int ds_mux_uart_tty_listener_init
(
  tty_config  * config_info,
  ds_mux_sk_fd_set_info_t * fd_set,
  ds_mux_sock_thrd_fd_read_f read_f,
  int mode,
  char* device_name
);

/*===========================================================================

FUNCTION DS_MUX_SMD_TTY_LISTENER_INIT()

DESCRIPTION

  This function
  - sets up DS_MUX to start listening for AT commands coming on SMD TTY device.

DEPENDENCIES
  None.

RETURN VALUE
  DS_MUX_SUCCESS_MACRO on success
  DS_MUX_FAILURE_MACRO on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int ds_mux_smd_tty_listener_init
(
  ds_mux_smd_tty_config  * smd_tty_config_info,
  ds_mux_sk_fd_set_info_t * fd_set,
  ds_mux_sock_thrd_fd_read_f read_f
);
/*===========================================================================

FUNCTION DS_MUX_USB_TTY_RECV_MSG()

DESCRIPTION

  This function
  - receives AT commands from UART.

DEPENDENCIES
  None.

RETURN VALUE
  DS_MUX_SUCCESS_MACRO on success
  DS_MUX_FAILURE_MACRO on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int ds_mux_uart_tty_recv_msg
(
   int usb_tty_fd
);

/*===========================================================================

FUNCTION DS_MUX_USB_TTY_SEND_MSG()

DESCRIPTION

  This function
  - send AT commands to UART

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

void ds_mux_uart_tty_send_msg
(
   void      *data,
   uint32_t   len
);

void ds_mux_tokenize_cmux_input(char[MAX_TOKENS][MAX_TOKEN_SIZE + 1], int);

typedef void (*ds_mux_to_atfwd_response_callback)(boolean, char*);

typedef int (* set_baud_rate) ();

int ds_mux_smd_ch_open(void);

ds_mux_dlci_param_type*  ds_mux_get_alloc_conn_ptr(uint_32);

void ds_mux_close_down_mux(void);

ds_mux_result_enum_type  ds_mux_logical_process_v24_signal(uint8, uint8);

typedef void (* t2_timer_handler) (int, siginfo_t *, void *);

int ds_mux_timer_start(ds_mux_timer_data_t* , uint16, ds_mux_timer_type);

ds_mux_result_enum_type ds_mux_util_encode_cmd_input(char *,ds_mux_io_frame_type*);

char* ds_mux_allocate_memory_and_read (int fd, int* n_bytes);

boolean ds_mux_get_is_uart_mode();

void ds_mux_set_is_uart_mode( boolean );


void ds_mux_update_dlci_params
(
  ds_mux_dlci_param_type     *dlci_conn,
  ds_mux_subset_enum_type     frame_type,
  ds_mux_cl_type_enum_type    cl_type,
  uint8                       priprity,
  uint16                      response_timer_T1,
  uint16                      frame_size_N1,
  uint8                       re_transmissions_N2,
  uint8                       window_size_k
);

#endif
