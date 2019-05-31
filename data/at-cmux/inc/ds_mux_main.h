#ifndef DS_MUX_MAIN_H
#define DS_MUX_MAIN_H
/*===========================================================================

                        D S _  M U X _ MAIN . H

DESCRIPTION
  This is the main header file for the DS_MUX_MODULE. It contains all other header files
  of DS_MUX_MODULE

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

/*===========================================================================

                      EDIT HISTORY FOR FILE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>


#include "ds_util.h"
#include "ds_mux_ipc.h"
#include "ds_mux_utils.h"
#include "ds_mux_defs.h"
#include "ds_mux_io.h"
#include "ds_mux.h"
#include "ds_mux_types.h"



#define SYSLOG_ENABLE
#ifdef SYSLOG_ENABLE
#include <stdarg.h>
#include <syslog.h>
#define SYSLOG_PREFIX "DS_MUX:"
#define LOGI(x...) syslog(LOG_ERR, x)
#endif

#define DS_MUX_MAX_RETRY                       10
#define SMD_DATA1_INIT_DELAY                   1000000

#define UART_MODE_MAX_ARGS 4
#define USB_MODE_MAX_ARGS  2

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void open_uart_fr_legacy_at_cmds(int, char*);
void open_smd_fr_legacy_at_cmds(void);
void ds_mux_set_mux_state(ds_mux_state_enum_type);
void ds_mux_ipc_atfwd_mux(void);
int ds_smd_tty_recv_msg(int);
void ds_mux_set_mux_state(ds_mux_state_enum_type);
void ds_mux_validate_atfwd_cmux_params(ds_mux_at_cmd_request*);




extern ds_mux_sk_fd_set_info_t   sk_fdset;
extern ds_mux_smd_tty_config     smd_tty_fd1; //For legacy AT commands
extern ds_mux_smd_tty_config     smd_tty_fd2; //For DLC0
extern ds_mux_smd_tty_config     smd_tty_fd3; //For DLC1
extern ds_mux_smd_tty_config     smd_tty_fd4; //For DLC2

extern tty_config                uart_tty_fd;
extern   boolean                 file_log_mode;

#endif
