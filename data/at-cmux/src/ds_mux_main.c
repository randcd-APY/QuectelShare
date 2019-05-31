/*===============================================================================

                             DS_MUX_MAIN.C

GENERAL DESCRIPTION
  This file is the starts of the DS_MUX Deamon. It has the main() function

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/


#include "ds_mux_main.h"
#include "ds_mux_ipc.h"
#include "ds_mux_utils.h"

ds_mux_smd_tty_config      smd_tty_fd1; //For legacy AT commands. DLC0

ds_mux_sk_fd_set_info_t    sk_fdset;

ds_mux_state_info_type     ds_mux_state_info;

ds_mux_smd_data_config     smd_data_fds; //for smd 1, 2 and 3

unsigned int               timer_ds_mux_sockfd;

unsigned int               timer_client_sockfd;

extern int                 baud_rate;



/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
  FUNCTION IsMuxMode
==========================================================================*/
/*!
@brief
  Checks if we are in MUX Mode or not.

@parameters
  None.

@return
  true  - If we are in MUX mode.
  flase - if we are not in MUX mode.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
static boolean IsMuxMode(void)
{
  int i = 0;
  FILE *cmd = NULL;
  char mode = 0;
  const char process[] = "cat /etc/data/mux_mode";
  char ipr_buff[DS_MUX_IPR_BUFF_SZ];

  cmd = popen(process, "r");
  if(cmd)
    mode = fgetc(cmd);

  if ( mode == 'Y' )
  {
    LOGI("In MUX mode.");
    return TRUE;
  }
  else
  {
    LOGI(" Not in MUX mode.");
    return FALSE;
  }

}



int main(int argc, char *argv[])
{
  int retval, min, mode;
  int_32 inactive_timer_value;

  #ifdef  SYSLOG_ENABLE
    openlog(SYSLOG_PREFIX, LOG_NDELAY | LOG_PID | LOG_CONS, LOG_USER|LOG_ERR);
  #endif

  if (TRUE != Diag_LSM_Init(NULL))
  {
     printf("Diag_LSM_Init failed !!");
  }

  if ( !IsMuxMode() )
  {
    exit(0);
  }
  LOGI("=*=STARTING MUX MODULE=*=");

  ds_mux_register_uart_timer_sig_handler();

  //initialize MUX mode. Initially MUX mode is closed.
  ds_mux_set_mux_state(DS_MUX_STATE_CLOSED);

  /* To open and read IPR from /data/ipr file */
  ds_mux_open_ipr_file();

  memset(&sk_fdset, 0, sizeof(ds_mux_sk_fd_set_info_t));

  min = MIN(USB_MODE_MAX_ARGS, UART_MODE_MAX_ARGS);

  if( argc < min )
   {
      LOGI("Error. There should be 2 arguments to open in UART mode.\
            Correct Usage: <binary name> <arg1:1> <arg2: UART port name>\
            Error. There should be 1 argument to open in USB mode.\
            Correct Usage: <binary name> <arg1:2>");
      exit(0);
  }

  if(atoi(argv[1]) == OPEN_UART_MODE)
  {
    if(argc < UART_MODE_MAX_ARGS)
    {
      LOGI("Error. There should be 2 arguments to open in UART mode.\
            Correct Usage: <binary name> <arg1:1> <arg2: UART port name>");
      exit(0);
    }
    LOGI("entered UART mode");

    mode = OPEN_UART_MODE;
  }
  else if(atoi(argv[1]) == OPEN_USB_MODE)
  {
    if(argc < USB_MODE_MAX_ARGS)
    {
      LOGI("Error. There should be 1 argument to open in USB mode.\
            Correct Usage: <binary name> <arg1:2>");
      exit(0);
    }
    LOGI("entered USB mode");

    mode = OPEN_USB_MODE;
  }
  else
  {
    LOGI("ERROR: INVALID MODE. There should be 2 arguments to open in UART mode.\
          Correct Usage: <binary name> <arg1:1> <arg2: UART port name>\
          Error. There should be 1 argument to open in USB mode.\
          Correct Usage: <binary name> <arg1:2>");
    exit(0);
  }

  ds_mux_read_ipr_file(mode);

  /* Listen for break signal only if it is USB Mode*/
  if(mode == OPEN_USB_MODE)
  {
    //Open USB port for legacy AT commands
    open_uart_fr_legacy_at_cmds(OPEN_USB_MODE , NULL);
    open_usb_line_state();
  }
  else if(mode == OPEN_UART_MODE)
  {
    inactive_timer_value = atoi( argv[3] );
    //Open UART port for legacy AT commands
    open_uart_fr_legacy_at_cmds(OPEN_UART_MODE , argv[2]);
    ds_mux_set_uart_timer_config(inactive_timer_value);
    ds_mux_listen_msgs_frm_uart_inactivity_timer(inactive_timer_value);
    ds_mux_create_uart_timer_client_socket();
  }
    //call the socket to keep listening for msgs coming from ATFWD
  ds_mux_listen_fr_msgs_from_atfwd();

  //initialize SMD for legacy AT commands
  open_smd_fr_legacy_at_cmds();

  /* Create timer -> ds_mux T2 timer socket */
  ds_mux_listen_msgs_frm_t2_timer();
  ds_mux_create_timer_client_socket();


  //Start the listener to listen on UART, SMD, SOCKET between ATFWD and DS_MUX
  retval = ds_mux_listener_start(&sk_fdset);

  if(retval != DS_MUX_SUCCESS_MACRO)
  {
    LOG_MSG_ERROR("Failed to start the listener on UART, SMD and SOCKET between ATFWD and DS_MUX ", 0, 0, 0);
  }
  return 0;
}
