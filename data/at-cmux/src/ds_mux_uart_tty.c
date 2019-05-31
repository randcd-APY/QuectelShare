/******************************************************************************

                        DS_MUX_UART_TTY.C

******************************************************************************/

/******************************************************************************

  @file    ds_mux_uart_tty.c
  @brief   Qualcomm Technologies, Inc. Tethering Interface. This file contains
           DS_MUX interaction with UART and USB.

  DESCRIPTION
  Implementation file for DS_MUX inteaction with UART and USB.

  ---------------------------------------------------------------------------
  Copyright (c) 2014, 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------


******************************************************************************/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/


/******************************************************************************


******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <termios.h>


#include "ds_mux_main.h"

/* To store UART fd */
tty_config*                           tty_info = NULL;
static ds_mux_usb_line_state_config*  usb_line_state_info = NULL;
ds_mux_usb_line_state_config          usb_line_state_config_info;
ds_mux_uart_timer_config*             uart_timer_config = NULL;
unsigned int                          timer_client_sockfd;
unsigned int                          uart_timer_client_sockfd;
unsigned int                          uart_timer_sockfd;



extern boolean            ds_mux_bridge_active;
extern boolean            avoid_enter_mux_mode;
extern set_baud_rate      set_baud_cb;
extern struct sigevent    sev;
extern tty_config*        tty_info;
extern int                baud_rate;



/*---------------------------------------------------------------------------
IOCTL to USB for Changing the mode to SMD.
---------------------------------------------------------------------------*/
typedef unsigned long   u32;
#define QTI_USB_XPORT_IOCTL_MAGIC 'G'
#define QTI_USB_CHANGE_XPORT_TYPE _IOW(QTI_USB_XPORT_IOCTL_MAGIC, 0, u32)
/*===========================================================================
                          FUNCTION DEFINITIONS
============================================================================*/

/*===========================================================================

FUNCTION DS_MUX_UART_TIMER_IND

DESCRIPTION

  This function indicates that the timer is fiered.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int ds_mux_uart_timer_ind
(
)
{
  char buf[UART_INACTIVITY_TIMER_BUF_SZ] = "1";
  int16 numBytes=0, len;
  struct sockaddr_un timer_ds_mux;

  timer_ds_mux.sun_family = AF_UNIX;
  strlcpy(timer_ds_mux.sun_path, SERVER_UART_TIMER_FILE, sizeof(SERVER_UART_TIMER_FILE));
  len = strlen(timer_ds_mux.sun_path) + sizeof(timer_ds_mux.sun_family);

  if ((numBytes = sendto(uart_timer_client_sockfd, (void *)buf, 10, 0,
          (struct sockaddr *)&timer_ds_mux, len)) == -1)
  {
    LOG_MSG_ERROR("Sendto failed:errno: %d coud not send from timer context to DS_MUX context", errno, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION DS_MUX_GET_UART_CONFIG

DESCRIPTION

  This function is used to get the uart configuration like UART-fd

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


tty_config* ds_mux_get_uart_config()
{

  if(tty_info != NULL)
  {
    return tty_info;
  }
  else
  {
    LOG_MSG_ERROR("uart config is NULL", 0, 0, 0);
    exit(0);
  }
}
/*===========================================================================

FUNCTION DS_MUX_UART_TIMER_CONFIG

DESCRIPTION

  This function is used to get the UART inactivity timer configuration

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

ds_mux_uart_timer_config* ds_mux_get_uart_timer_config()
{

  if(uart_timer_config != NULL)
  {
    return uart_timer_config;
  }
  else
  {
    LOG_MSG_ERROR("uart timer config is NULL", 0, 0, 0);
    return NULL;
  }
}

/*===========================================================================

FUNCTION DS_MUX_UPDATE_UART_SLEEP_STATUS

DESCRIPTION

  This function updates whether UART is sleeping or not in UART timer configuation

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_update_uart_sleep_status(boolean status)
{

  if(NULL != uart_timer_config)
  {
    uart_timer_config->uart_in_sleep = status;
  }
  else
  {
    LOG_MSG_ERROR("uart timer config is NULL", 0, 0, 0);
  }

}

/*===========================================================================

FUNCTION DS_MUX_GET_UART_SLEEP_STATUS

DESCRIPTION

  This function gets the status of UART whether UART is sleeping or not

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int16 ds_mux_get_uart_sleep_status()
{

  if(NULL != uart_timer_config)
  {
    return uart_timer_config->uart_in_sleep;
  }
  else
  {
    LOG_MSG_ERROR("uart timer config is NULL", 0, 0, 0);
    return INVALID_SLEEP_STATUS;
  }
}

/*===========================================================================

FUNCTION DS_MUX_UPDATE_UART_TIMER_IS_RUNNING_STAE

DESCRIPTION

  This function updates whether the UART inactivity timer is running or not.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_update_uart_timer_is_running_state(boolean state)
{
  if(NULL != uart_timer_config)
  {
    uart_timer_config->uart_inactivity_timer_config.is_running = state;
  }
}

/*===========================================================================

FUNCTION DS_MUX_GET_UART_TIMER_IS_RUNNING_STATE

DESCRIPTION

  This function gets the status of UART inactivity timer whether it is running or not.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int16 ds_mux_get_uart_timer_is_running_state()
{

  if(NULL != uart_timer_config)
  {
    return uart_timer_config->uart_inactivity_timer_config.is_running;
  }
  else
  {
    LOG_MSG_ERROR("uart timer config is NULL", 0, 0, 0);
    return INVALID_TIMER_STATUS;
  }
}

/*===========================================================================

FUNCTION DS_MUX_GET_IS_UART_MODE

DESCRIPTION

  This function tells whether we are in UART or USB mode

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


boolean ds_mux_get_is_uart_mode()
{
  if(NULL != tty_info)
  {
    return tty_info->is_uart_mode;
  }
  else
  {
    LOG_MSG_ERROR("uart_config is NULL", 0, 0, 0);
    exit(0);
  }
}

/*===========================================================================

FUNCTION DS_MUX_SET_IS_UART_MODE

DESCRIPTION

  This function updates whether we are in UART mode or not

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_set_is_uart_mode(boolean val)
{
  if(NULL != tty_info)
  {
    tty_info->is_uart_mode = val;
  }
  else
  {
    LOG_MSG_ERROR("uart_config is NULL", 0, 0, 0);
    exit(0);
  }
}

/*===========================================================================

FUNCTION DS_MUX_ALLOC_UART_INACTIVITY_TIMER

DESCRIPTION

  This function allocates memory for UART inactivity timer

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int ds_mux_alloc_uart_inactivity_timer(int_32 inactive_timer_value)
{
  struct sigaction sa;


  memset(&sa, 0, sizeof(struct sigaction));
  memset(&sev, 0, sizeof(struct sigevent));

  uart_timer_config = (ds_mux_uart_timer_config *) calloc(1, sizeof(ds_mux_uart_timer_config));

  if(uart_timer_config == NULL)
  {
    LOGI("Could not allocate memory to uart_timer_config");
    return DS_MUX_FAILURE_MACRO;
  }
  uart_timer_config->uart_inactivity_timer_data.user_data = NULL;
  uart_timer_config->uart_inactivity_timer_data.timer_id_type = DS_MUX_UART_INACTIVITY_TIMER;

  uart_timer_config->uart_inactivity_timer_config.is_running = FALSE;
  uart_timer_config->timer_val = inactive_timer_value;
  LOGI("uart_inactity_timer_value is %ld", uart_timer_config->timer_val);
  ds_mux_update_uart_sleep_status(TRUE);

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG2;
  sev.sigev_value.sival_ptr = NULL;

  if (timer_create(CLOCKID, &sev, &(uart_timer_config->uart_inactivity_timer_config.timer_id))== -1)
  {
    LOGI(" Failed to create uart inactivity timer");
    if(uart_timer_config != NULL)
    {
      DS_MUX_MEM_FREE(uart_timer_config);
      uart_timer_config = NULL;
    }
    return DS_MUX_FAILURE_MACRO;
  }

  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION DS_MUX_CHECK_AND_START_UART_INACTIVITY_TIMER

DESCRIPTION

  UART inactivity timer starts whenever UART READ and UART WRITE functions are called.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int ds_mux_check_and_start_uart_inactivity_timer()
{
  int ret;
  boolean timer_status = ds_mux_get_uart_timer_is_running_state();

  if(NULL == uart_timer_config)
  {
    LOG_MSG_ERROR("uart timer config is NULL", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  if(TRUE == timer_status)
  {
    LOG_MSG_INFO1("stopping uart inactivity timer",0,0,0);
    ds_mux_timer_stop(&uart_timer_config->uart_inactivity_timer_config);
    ds_mux_update_uart_timer_is_running_state(FALSE);
  }
  else if(INVALID_TIMER_STATUS == timer_status)
  {
    LOG_MSG_ERROR("Inalid timer status", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  ds_mux_update_uart_timer_is_running_state(TRUE);
  ret = ds_mux_timer_start(&uart_timer_config->uart_inactivity_timer_config,
                           uart_timer_config->timer_val,
                           DS_MUX_UART_INACTIVITY_TIMER);
  if(ret != DS_MUX_SUCCESS_MACRO)
  {
    LOG_MSG_INFO1("Failed to start timer", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION DS_MUX_UART_SLEEP

DESCRIPTION

  This function puts UART to sleep

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_uart_sleep()
{
   int ioctl_ret;
   tty_config* uart_config = ds_mux_get_uart_config();
   ioctl_ret = ioctl(uart_config->fd, USERIAL_OP_CLK_OFF);
   if(ioctl_ret == -1)
   {
     LOG_MSG_INFO1("uart failed to sleep errno %d", errno,0,0);
   }
   else
   {
     LOG_MSG_INFO1("*******uart in sleep ret: %d*******",ioctl_ret,0,0);
   }
}

/*===========================================================================

FUNCTION DS_MUX_UART_WAKEUP

DESCRIPTION

  This function wakesup UART

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

void ds_mux_uart_wakeup()
{

  int ioctl_ret;
  tty_config* uart_config = ds_mux_get_uart_config();

  ioctl_ret = ioctl(uart_config->fd, USERIAL_OP_CLK_ON);
  if(ioctl_ret == -1)
  {
    LOG_MSG_INFO1("uart failed to wakeup errno %d", errno,0,0);
  }
  else
  {
    LOG_MSG_INFO1("uart woke up!!!",0,0,0);
  }
}

/*===========================================================================

FUNCTION DS_MUX_SET_UART_TIMER_CONFIG

DESCRIPTION

  This function allocates the UART inactivity timer configuration

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_set_uart_timer_config(int_32          inactive_timer_value)
{
  if(DS_MUX_SUCCESS_MACRO != ds_mux_alloc_uart_inactivity_timer(inactive_timer_value))
  {
    LOG_MSG_ERROR("fail to alloc uart timer config", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION DS_MUX_PROCESS_UART_TIMER_IND

DESCRIPTION

  This function indicates will return to the main thread from the timer context

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

int ds_mux_process_uart_timer_ind
(
  int fd
)
{
  struct sockaddr_storage their_addr;
  socklen_t addr_len = sizeof(struct sockaddr_storage);
  int16 nbytes = 0;
  char buf[UART_INACTIVITY_TIMER_BUF_SZ];
  int16 uart_in_sleep = INVALID_SLEEP_STATUS;

  memset(buf, 0, 10);

  LOG_MSG_INFO1("Received Message from timer_context",0,0,0);
  if ( (nbytes = recvfrom(fd, buf, 10 , 0, (struct sockaddr *)&their_addr, &addr_len)) <= 0 )
  {
    if ( nbytes == 0 )
    {
      LOG_MSG_INFO1("Completed full recv from ds_mux_timer_sockfd context", 0, 0, 0);
    }
    else
    {
      LOG_MSG_ERROR("recvfrom returned error, errno:%d", errno, 0, 0);
    }
  }
  else
  {
    if(atoi(buf) == 1)
    {
      LOG_MSG_INFO1("received uart inactivity signal", 0, 0, 0);
      uart_in_sleep = ds_mux_get_uart_sleep_status();
      if(FALSE == uart_in_sleep)
      {
        ds_mux_uart_sleep();
        ds_mux_update_uart_sleep_status(TRUE);
      }
      else if(uart_in_sleep == INVALID_SLEEP_STATUS)
      {
        LOG_MSG_ERROR("uart is having invalid sleep status", 0, 0, 0);
        return DS_MUX_FAILURE_MACRO;
      }
      ds_mux_update_uart_timer_is_running_state(FALSE);
    }
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION DS_MUX_UART_TIMER_LISTENER_INIT

DESCRIPTION

  This function will map listener function to listen from the UART inactivity timer context

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int ds_mux_uart_timer_listener_init
(
  ds_mux_sk_fd_set_info_t*   sk_fdset,
  ds_mux_sock_thrd_fd_read_f read_f,
  int                        max_fds
)
{
  if (create_server_socket(&uart_timer_sockfd, SERVER_UART_TIMER_FILE) != DS_MUX_SUCCESS_MACRO)
  {
    LOGI("creating uart timer_ds_mux_sockfd socket fails");
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOGI("Uart Timer Socket Creation SUCCESS!!!");
  }

  if( ds_mux_addfd_map(sk_fdset,uart_timer_sockfd, read_f, max_fds) == DS_MUX_FAILURE_MACRO)
  {
    LOGI("cannot add DSMUX atfwd socket for reading");
    close(uart_timer_sockfd);
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOGI("Timer Socket mapped to fn SUCCESS!!!");
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION DS_MUX_LISTEN_FRM_UART_TIMER

DESCRIPTION

  This function will create a server socket for UART inactivity timer.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int ds_mux_listen_frm_uart_timer
(
)
{
  if(ds_mux_uart_timer_listener_init(&sk_fdset,
                                     ds_mux_process_uart_timer_ind,
                                     DS_MUX_MAX_NUM_OF_FD) == DS_MUX_SUCCESS_MACRO)
  {
    LOGI("Socket to receive timer msgs succeeds");
  }
  else
  {
    LOGI("Socket to receive timer msgs Fails");
    return DS_MUX_FAILURE_MACRO;
  }

  return DS_MUX_SUCCESS_MACRO;
}

void ds_mux_listen_msgs_frm_uart_inactivity_timer()
{
  int ret;
  ret = ds_mux_listen_frm_uart_timer();
  if (DS_MUX_SUCCESS_MACRO != ret)
  {
     LOGI("unable to listen msgs from Timer, errno: %d",ret, 0, 0);
  }
}

/*===========================================================================

FUNCTION DS_MUX_CREATE_UART_TIMER_CLIENT_SOCKET

DESCRIPTION

  This function will create a client socket for UART inactivity timer

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_create_uart_timer_client_socket()
{
  int ret;
  struct sockaddr_un client;

  ret = create_socket(&uart_timer_client_sockfd);
  if(ret == DS_MUX_FAILURE_MACRO)
  {
    LOGI("Failed to create the uart client timer socket");
  }
  else
  {
    LOGI("Sucessfully created UART CLIENT timer socket ");
  }
}


/*===========================================================================

FUNCTION DS_MUX_ALLOCATE_MEMORY_AND READ()

DESCRIPTION

  This function
  - Reads and allocated memory

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


char* ds_mux_allocate_memory_and_read (int fd, int* n_bytes)
{
  char *buff = NULL, *tmp = NULL;
  int i=2, index = 0, bytes_allocated;

  buff = (char *) calloc(1, DS_MUX_MIN_BUF_LEN);

  if(NULL == buff)
  {
    LOG_MSG_ERROR("could not allocate memory. calloc failed", 0, 0, 0);
    return NULL;
  }
  bytes_allocated = DS_MUX_MIN_BUF_LEN;

  while(i <= DS_MUX_MAX_BUF_NUM_LEN)
  {
    *n_bytes = read(fd, buff+index, DS_MUX_MIN_BUF_LEN);

    if (*n_bytes <= 0)
    {
      LOG_MSG_ERROR("Failed to read from the dev file errno: %d", errno, 0, 0);
      return buff;
    }
    else if(*n_bytes < DS_MUX_MIN_BUF_LEN)
    {
      LOG_MSG_INFO1("allocated required memory", 0, 0, 0);
      index = index + *n_bytes;
      break;
    }

    tmp = realloc(buff, DS_MUX_MIN_BUF_LEN * i );
    if(tmp != NULL)
    {
         buff = tmp;
         index = index + *n_bytes;
         bytes_allocated = bytes_allocated + DS_MUX_MIN_BUF_LEN;
         memset(buff+index, 0, bytes_allocated - index);
    }
    else
    {
      LOG_MSG_ERROR( "realloc failed",0, 0, 0);
    }

    i++;
  }
  *n_bytes = index;
  LOG_MSG_INFO1("Total bytes read = %d", *n_bytes, 0, 0);
  if(i > DS_MUX_MAX_BUF_NUM_LEN)
  {
    LOG_MSG_ERROR("buffer size exceeding max buffer size 2^15", 0, 0, 0);
  }

  return buff;
}

/*===========================================================================

FUNCTION DS_MUX_UART_TTY_RECV_MSG()

DESCRIPTION

  This function
  - receives AT commands from UART

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


int ds_mux_uart_tty_recv_msg
(
   int uart_tty_fd_param
)
{
  int       ret, i=0, n_bytes = 0;
  char*     uart_rx_buf = NULL;
  int       ret_val;
  u32       uart_mode = 0;
  int       uart_serial_fd = 0;
  int       result = DS_MUX_SUCCESS_MACRO;
  int16   uart_in_sleep = INVALID_SLEEP_STATUS;
  int16   timer_status  = INVALID_TIMER_STATUS;
  ds_mux_uart_timer_config* uart_timer_config_info = ds_mux_get_uart_timer_config();

 /*-------------------------------------------------------------------------*/
  LOG_MSG_INFO1("ds_mux_uart_tty_recv_msg", 0, 0, 0);

  uart_rx_buf = ds_mux_allocate_memory_and_read(uart_tty_fd_param, &n_bytes);

  if(uart_rx_buf == NULL)
  {
    LOG_MSG_ERROR("Failed to allocate memory", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  if (n_bytes <= 0)
  {
    LOG_MSG_ERROR("Failed to read from the dev file %d:%d", uart_tty_fd_param, errno, 0);
    if (n_bytes == 0)
    {
      LOG_MSG_INFO1("\nClosing UART FD %d %d\n",
                    uart_tty_fd_param, tty_info->fd, 0);
      close(tty_info->fd);

      /* Clear the FD from fd set. */
      ds_mux_clear_fd(&sk_fdset, tty_info->fd);
      if ( uart_tty_fd_param != tty_info->fd )
      {
        ds_mux_clear_fd(&sk_fdset, uart_tty_fd_param);
      }
      tty_info->fd = 0;
    }
    if(uart_rx_buf != NULL)
    {
      DS_MUX_MEM_FREE(uart_rx_buf);
      uart_rx_buf = NULL;
    }
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOG_MSG_INFO1("reading from UART %s", uart_rx_buf, 0, 0);
#ifdef MUX_FRAME_LOG
    for(i=0; i<n_bytes ; i++)
    {
       LOG_MSG_INFO1("uart_rx_buf[%d] = %X", i, uart_rx_buf[i], 0);
    }
#endif
  }

  if(ds_mux_get_is_uart_mode())
  {
    ret = ds_mux_check_and_start_uart_inactivity_timer();
    if(DS_MUX_SUCCESS_MACRO != ret)
    {
      LOG_MSG_ERROR("failed to start uart inactivity timer", 0, 0, 0);
    }

    if (*uart_rx_buf == TTY_BREAK)
    {
      LOGI("received break signal ", 0, 0, 0);
      uart_in_sleep = ds_mux_get_uart_sleep_status();
      if(FALSE == uart_in_sleep)
      {
        ds_mux_uart_sleep();
        ds_mux_update_uart_sleep_status(TRUE);
      }
      else if(INVALID_SLEEP_STATUS == uart_in_sleep)
      {
        LOG_MSG_ERROR("uart is having invalid sleep status", 0, 0, 0);
        return DS_MUX_FAILURE_MACRO;
      }
    }
    else if (*uart_rx_buf == TTY_WAKEUP)
    {
      LOG_MSG_INFO1("Received Wakeup signal", 0, 0, 0);
      uart_in_sleep = ds_mux_get_uart_sleep_status();
      if(TRUE == uart_in_sleep)
      {
        ds_mux_uart_wakeup();
        ds_mux_update_uart_sleep_status(FALSE);
      }
      else if(INVALID_SLEEP_STATUS == uart_in_sleep)
      {
        LOG_MSG_ERROR("uart is having invalid sleep status", 0, 0, 0);
        return DS_MUX_FAILURE_MACRO;
      }
    }

    timer_status = ds_mux_get_uart_timer_is_running_state();
    if(FALSE == timer_status)
    {
      ds_mux_update_uart_timer_is_running_state(TRUE);
      if(NULL != uart_timer_config_info)
      {
        ret = ds_mux_timer_start(&uart_timer_config_info->uart_inactivity_timer_config,
                               uart_timer_config_info->timer_val,
                               DS_MUX_UART_INACTIVITY_TIMER);
        if(ret != DS_MUX_SUCCESS_MACRO)
        {
          LOG_MSG_INFO1("Failed to start timer", 0, 0, 0);
          return DS_MUX_FAILURE_MACRO;
        }
      }
      else
      {
        LOG_MSG_ERROR("uart timer config is NULL", 0, 0, 0);
      }
    }
    else if(INVALID_TIMER_STATUS == timer_status)
    {
      LOG_MSG_ERROR("invalid timer status", 0, 0, 0);
      return DS_MUX_FAILURE_MACRO;
    }
  }

  LOG_MSG_INFO1("MUX MODE is %d", ds_mux_bridge_active, 0, 0);
  if(ds_mux_bridge_active)
  {
    if(DS_MUX_SUCCESS != ds_mux_io_rx_frame_hdlr(uart_rx_buf, n_bytes))
    {
      LOG_MSG_ERROR("Could Not handle the Frame", 0, 0, 0);
      if(uart_rx_buf != NULL)
      {
        DS_MUX_MEM_FREE(uart_rx_buf);
        uart_rx_buf = NULL;
      }
      return DS_MUX_FAILURE_MACRO;
    }
    else
    {
      LOG_MSG_INFO1("Successfully handled the frame", 0, 0, 0);
    }
  }
  else
  {
    /* Send message to SMD i.e. to Modem */
    qti_smd_tty_send_msg(uart_rx_buf, n_bytes);
  }

  if(uart_rx_buf != NULL)
  {
    DS_MUX_MEM_FREE(uart_rx_buf);
    uart_rx_buf = NULL;
  }

  return result;

}


/*===========================================================================

FUNCTION DS_MUX_USB_TTY_FILE_OPEN()

DESCRIPTION

  This function
  - opens the device file which is used for interfacing with UART

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


static int ds_mux_usb_tty_file_open
(
   int * usb_tty_fd
)
{
  int qti_usb_tty_fd, usb_serial_fd;
  struct termios tty_port_settings;
  speed_t baud_rate;
  u32 usb_mode = 0;
  char command[MAX_COMMAND_STR_LEN];
  uint32_t time_out = 10;
  uint8_t j = 0;
/*--------------------------------------------------------------------------*/

  /* Check for NULL Args. */
  if (usb_tty_fd == NULL)
  {
    LOGI("NULL Args",0,0,0);
    return DS_MUX_FAILURE_MACRO;
  }

  /* Before opening the TTY device switch the USB mode. */
  /* Open the serial device to call the IOCTL. */
  usb_serial_fd = open(USB_SERIAL_PORT, O_RDWR);
  if(usb_serial_fd == -1)
  {
    LOGI("Could not open serial device file, errno : %d ", errno, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  /* Change the mode to TTY. */
  usb_mode = 0;
  if (ioctl(usb_serial_fd,
            QTI_USB_CHANGE_XPORT_TYPE,
            &usb_mode) < 0)
  {
    LOGI("ioctl failed something wrong check %d.",
                  errno,0,0);
    close(usb_serial_fd);
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOGI("ioctl success, mode changed to SMD",
                  0,0,0);
  }
  /* Close the FD. */
  close(usb_serial_fd);

  while (j++ < time_out)
  {
    qti_usb_tty_fd = open(USB_DUN_TTY_PORT, O_RDWR | O_NOCTTY);
    if (qti_usb_tty_fd != -1)
    {
      break;
    }
    usleep(USB_DEV_INIT_DELAY);
  }

  memset(&tty_port_settings, 0, sizeof(struct termios));

  baud_rate = DEFAULT_BAUD_RATE_USB_115200;
  /* Set the Baud rate. */
  cfsetspeed(&tty_port_settings, baud_rate);

  /* Flush the existing data. */
  tcflush(qti_usb_tty_fd, TCIOFLUSH);

  /* Apply the atributes. */
  tcsetattr(qti_usb_tty_fd,TCSANOW,&tty_port_settings);

  LOGI("Successfully opened USB device file. FD is %d", qti_usb_tty_fd, 0, 0);
  *usb_tty_fd = qti_usb_tty_fd;

  if(DEFAULT_BAUD_RATE_USB_115200 != baud_rate)
  {
    //change baud rate only if it is not the default baud-rate
    set_baud_rate_cb();
  }

  return DS_MUX_SUCCESS_MACRO;

}


/*===========================================================================

FUNCTION DS_MUX_UART_TTY_FILE_OPEN()

DESCRIPTION

  This function
  - opens the device file which is used for interfacing with UART

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


static int ds_mux_uart_tty_file_open
(
   int * uart_tty_fd,
   char* device_name
)
{
  int qti_uart_tty_fd, uart_serial_fd = -1;
  struct termios tty_port_settings;
  char command[MAX_COMMAND_STR_LEN];
  uint32_t time_out = 10;
  uint8_t j = 0;
  uint32_t baud = DEFAULT_BAUD_RATE_UART_9600;
  uint8_t stop_bits = 0;
  struct termios     term;
/*--------------------------------------------------------------------------*/

  memset(&term, 0, sizeof(struct termios));
  /* Check for NULL Args. */
  if (uart_tty_fd == NULL || device_name == NULL)
  {
    LOGI("NULL Args",0,0,0);
    return DS_MUX_FAILURE_MACRO;
  }

  uart_serial_fd = open(device_name, O_RDWR);

  if(uart_serial_fd == -1)
  {
    LOGI("Could not open serial device file, errno : %d ", errno, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  /* Change the mode to TTY. */

  LOGI("Successfully opened UART device file. FD is %d", uart_serial_fd);
  *uart_tty_fd = uart_serial_fd;

  if (tcflush(uart_serial_fd, TCIOFLUSH) < 0)
  {
    LOGI("issue while tcflush ");
    close(uart_serial_fd);
    return DS_MUX_FAILURE_MACRO;
  }

  if (tcgetattr(uart_serial_fd, &term) < 0) {
      LOGI("issue while tcgetattr ");
      close(uart_serial_fd);
      return DS_MUX_FAILURE_MACRO;
  }

  cfmakeraw(&term);
  /* Set RTS/CTS HW Flow Control*/
  term.c_cflag |= (CRTSCTS | stop_bits);

  if (tcsetattr(uart_serial_fd, TCSANOW, &term) < 0) {
     LOGI("issue while tcsetattr ");
     close(uart_serial_fd);
     return DS_MUX_FAILURE_MACRO;
  }

  /* set input/output baudrate */
  cfsetospeed(&term, baud);
  cfsetispeed(&term, baud);

  if(DEFAULT_BAUD_RATE_UART_9600 != baud_rate)
  {
    //change baud rate only if it is not the default baud-rate
    set_baud_rate_cb();
  }

  return DS_MUX_SUCCESS_MACRO;

}

/*===========================================================================

FUNCTION DS_MUX_FILE_OPEN

DESCRIPTION

  This function opens UART/USB file depending on the mode

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void ds_mux_file_open
(
  uint8 mode,
  char* device_name,
  tty_config* tty_info,
  ds_mux_sk_fd_set_info_t* fd_set,
  ds_mux_sock_thrd_fd_read_f read_f
)
{
  int ret_val = DS_MUX_SUCCESS;

  if(mode == OPEN_USB_MODE)
  {
    ret_val = ds_mux_usb_tty_file_open(&(tty_info->fd));
    ds_mux_set_is_uart_mode(FALSE);
  }
  else
  {
    ret_val = ds_mux_uart_tty_file_open(&(tty_info->fd), device_name);
    ds_mux_set_is_uart_mode(TRUE);
  }
  if(ret_val == DS_MUX_FAILURE_MACRO)
  {
    LOGI("Failed to open UART device file. Abort", 0, 0, 0);
    return;
  }
  else
  {
    LOGI("Opened file's fd is %d", tty_info->fd);

    if( ds_mux_addfd_map(fd_set, tty_info->fd, read_f,
                             MAX_NUM_OF_FD) != DS_MUX_SUCCESS_MACRO)
    {
      LOGI("cannot add the fd for reading");
      close(tty_info->fd);
      return;
    }
  }
}


/*===========================================================================

FUNCTION DS_MUX_USB_TTY_LISTENER_INIT()

DESCRIPTION

  This function
  - opens the UART device file
  - adds the UART fd to wait on select call

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

int ds_mux_uart_tty_listener_init
(
  tty_config* config_info,
  ds_mux_sk_fd_set_info_t * fd_set,
  ds_mux_sock_thrd_fd_read_f read_f,
  int mode,
  char* device_name
)
{
  int ret_val;
  FILE * fp = NULL;

/*-------------------------------------------------------------------------*/

  /* Check for NULL Args. */
  if (config_info == NULL || fd_set == NULL)
  {
    LOGI("NULL Args");
    return DS_MUX_FAILURE_MACRO;
  }

  LOGI("Open TTY file to receive AT commands");
  /* Update the configuration. */
  tty_info = config_info;

  ds_mux_file_open(mode, device_name, tty_info, fd_set, read_f);


  return DS_MUX_SUCCESS_MACRO;
}


/*===========================================================================

FUNCTION OPEN_UART_FR_LEGACY_AT_CMDS

DESCRIPTION

  This function
  - This function will open the UART device node

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

void open_uart_fr_legacy_at_cmds(int mode, char* device_name)
{
  int retval;
  tty_info =(tty_config *)malloc(sizeof(tty_config));
  if(tty_info == NULL)
  {
    LOG_MSG_ERROR("couldnot allocate memory for ttyinfo", 0, 0, 0);
    return;
  }
  memset(tty_info, 0, sizeof(tty_config));
  /*---------------------------------------------------------------------
    Call into the UART TTY listener init function which sets up DS_MUX to
    listen to AT Commands coming in from the UART device file.
---------------------------------------------------------------------*/
  retval = ds_mux_uart_tty_listener_init(tty_info, &sk_fdset, ds_mux_uart_tty_recv_msg, mode, device_name);
  if(retval != DS_MUX_SUCCESS_MACRO && tty_info != NULL)
  {
    LOG_MSG_ERROR("Failed to initialize DS_MUX UART TTY listener.", 0, 0, 0);
    DS_MUX_MEM_FREE(tty_info);
    tty_info = NULL;
    return;
  }

}

/*===========================================================================

FUNCTION DS_MUX_USB_LINE_STATE_RECV_MSG()

DESCRIPTION

  This function
  - receives notifications about line state.

DEPENDENCIES
  None.

RETURN VALUE
  DS_MUX_SUCCESS_MACRO on success
  DS_MUX_FAILURE_MACRO on failure


SIDE EFFECTS
  None

/*=========================================================================*/
int ds_mux_usb_line_state_recv_msg
(
  int usb_line_state_fd
)
{
  int line_state = 0, ret, ret_val = DS_MUX_SUCCESS_MACRO;

  ret = read(usb_line_state_fd, &line_state, sizeof(line_state));
  if (ret <= 0)
  {
    LOG_MSG_ERROR("Failed to read from the dev file %d:%d", usb_line_state_fd, errno, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  LOGI("Line State: %d", line_state);

  if ( line_state )
  {
    /*---------------------------------------------------------------------
      Call into the USB TTY listener init function which sets up QTI to
      listen to AT Commands coming in from the USB device file for DUN
    ---------------------------------------------------------------------*/
    if (tty_info == NULL || (tty_info->fd == 0))
    {
      ret_val = ds_mux_uart_tty_listener_init(tty_info,
                                              &sk_fdset,
                                              ds_mux_uart_tty_recv_msg,
                                              OPEN_USB_MODE,
                                              NULL);
      if (ret_val != DS_MUX_SUCCESS_MACRO)
      {
        LOG_MSG_ERROR("Failed to initialize DS_MUX USB TTY listener",0,0,0);
      }
    }
  }
  else
  {
    /* USB cable is disconnected. Clear the TTY FD. */
    if ( tty_info != NULL && tty_info->fd != 0)
    {
      LOGI("\nClosing USB FD %d\n", tty_info->fd);
      close(tty_info->fd);
      /* Clear the FD from fd set. */
      ds_mux_clear_fd(&sk_fdset, tty_info->fd);
      tty_info->fd = 0;
    }
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION DS_MUX_SB_LINE_STATE_OPEN

DESCRIPTION

  This function creates server socket to listen for USB break event

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


static int ds_mux_usb_line_state_open
(
   int * usb_line_state_fd
)
{
  int ds_mux_usb_line_state_fd, val;
  struct sockaddr_un mux_usb_line_state;
  int len;
  struct timeval rcv_timeo;
/*--------------------------------------------------------------------------*/

  /* Check for NULL Args. */
  if (usb_line_state_fd == NULL)
  {
    LOG_MSG_ERROR("NULL Args",0,0,0);
    return DS_MUX_FAILURE_MACRO;
  }
  if ((ds_mux_usb_line_state_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_ERROR("Error creating socket, errno: %d", errno, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  if(fcntl(ds_mux_usb_line_state_fd, F_SETFD, FD_CLOEXEC) < 0)
  {
    LOG_MSG_ERROR("Couldn't set Close on Exec, errno: %d", errno, 0, 0);
  }

  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(ds_mux_usb_line_state_fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
  val = fcntl(ds_mux_usb_line_state_fd, F_GETFL, 0);
  fcntl(ds_mux_usb_line_state_fd, F_SETFL, val | O_NONBLOCK);

  mux_usb_line_state.sun_family = AF_UNIX;
  strlcpy(mux_usb_line_state.sun_path, DS_MUX_LS_FILE, DS_MUX_UNIX_PATH_MAX);
  unlink(mux_usb_line_state.sun_path);
  len = strlen(mux_usb_line_state.sun_path) + sizeof(mux_usb_line_state.sun_family);
  if (bind(ds_mux_usb_line_state_fd, (struct sockaddr *)&mux_usb_line_state, len) == DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
    close(ds_mux_usb_line_state_fd);
    return DS_MUX_FAILURE_MACRO;
  }

  *usb_line_state_fd = ds_mux_usb_line_state_fd;

  return DS_MUX_SUCCESS_MACRO;

}


/*===========================================================================

FUNCTION QTI_USB_LINE_STATE_INIT()

DESCRIPTION

  This function
  - Adds fd to get line state notifications from QTI.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

int ds_mux_usb_line_state_init
(
  ds_mux_usb_line_state_config *line_state_config,
  ds_mux_sk_fd_set_info_t * fd_set,
  ds_mux_sock_thrd_fd_read_f read_f
)
{

  int ret_val;
/*-------------------------------------------------------------------------*/

  /* Check for NULL Args. */
  if (line_state_config == NULL || fd_set == NULL)
  {
    LOGI("NULL Args");
    return DS_MUX_FAILURE_MACRO;
  }

  LOG_MSG_INFO1("Open USB line state fd to get line status indications", 0, 0, 0);
  usb_line_state_info = line_state_config;
  ret_val = ds_mux_usb_line_state_open(&(line_state_config->line_state_fd));

  if(ret_val == DS_MUX_FAILURE_MACRO)
  {
    LOGI("Failed to open Notify object. Abort");
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOGI("Opened file's fd is %d", usb_line_state_info->line_state_fd);

    if (ds_mux_addfd_map(fd_set, usb_line_state_info->line_state_fd, read_f,
                           MAX_NUM_OF_FD) != DS_MUX_SUCCESS_MACRO)
    {
      LOGI("cannot add socket fd for reading");
      close(usb_line_state_info->line_state_fd);
      return DS_MUX_FAILURE_MACRO;
    }
    LOGI("Successfully added the break-event listener fd");
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION OPEN_USB_LINE_STATE

DESCRIPTION

  This function is a wrapper for DS_MUX_USB_LINE_STATE_INIT which will listen to the
  break events from the QTI

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


void open_usb_line_state()
{
  int retval;

  /*---------------------------------------------------------------------
    Create a Notifier fd for the USB break-signal detection
---------------------------------------------------------------------*/
  retval = ds_mux_usb_line_state_init(&usb_line_state_config_info,
                                      &sk_fdset,
                                      ds_mux_usb_line_state_recv_msg);
  if(retval != DS_MUX_SUCCESS_MACRO)
  {
    LOGI("Failed to initialize DS_MUX USB TTY Notifier.");
    return;
  }
}

/*===========================================================================

FUNCTION DS_MUX_CHECK_AND_WAKE_UP_UART

DESCRIPTION

  This function wakesup UART if it is Sleeping

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

void ds_mux_check_and_wake_up_uart()
{
  LOG_MSG_INFO1("func: %s", __func__, 0, 0);
  boolean uart_in_sleep = ds_mux_get_uart_sleep_status();
  if(TRUE == uart_in_sleep)
  {
    ds_mux_uart_wakeup();
    ds_mux_update_uart_sleep_status(FALSE);
  }
  else if(INVALID_SLEEP_STATUS == uart_in_sleep)
  {
    LOG_MSG_ERROR("uart is having invalid sleep status", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

}

/*===========================================================================

FUNCTION DS_MUX_UART_TTY_SEND_MSG

DESCRIPTION

  This function will send data coming from SMD to UART.

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
)
{
  int ret, i;
  char *info = (char *) data;
  char *ok = "OK";
/*-----------------------------------------------------------------------*/

  /* Check for NULL Args. */
  if (data == NULL)
  {
    LOG_MSG_ERROR("NULL Args",0,0,0);
    return;
  }
  else
  {
    LOG_MSG_INFO1("DATA on UART to send to HOST %s", info, 0, 0);
  }

  if(ds_mux_get_is_uart_mode())
  {
    ds_mux_check_and_wake_up_uart();
  }
  ret = write(tty_info->fd, (char*)data, len);
  if (ret == DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_ERROR("Couldn't send message to host: %d", errno, 0, 0);
  }
  else if (ret != len)
  {
    LOG_MSG_ERROR("Unexpected return value when writing to device file: got %d, "
                  "expected %d (errno %d)", ret, len, errno);
  }
  else
  {
#ifdef MUX_FRAME_LOG
    for(i=0; i<ret ; i++)
    {
      LOG_MSG_INFO1("SEND_HOST[%d] = %X", i, info[i], 0);
    }
#endif
    LOG_MSG_INFO1("Successfully sent message to host: %s\n", info, 0, 0);
  }
  if(ds_mux_get_is_uart_mode())
  {
    ret = ds_mux_check_and_start_uart_inactivity_timer();
    if(DS_MUX_SUCCESS_MACRO != ret)
    {
      LOG_MSG_ERROR("failed to start uart inactivity timer", 0, 0, 0);
    }
  }
  if(avoid_enter_mux_mode)
  {
    if(strstr(info, ok) != NULL)
    {
      avoid_enter_mux_mode = FALSE;

      if(set_baud_cb != NULL)
      {
        /* Sleep for a second untill OK is received by the terminal*/
        sleep(ONE_SECOND);
        LOG_MSG_INFO1("calling set baud rate cb", 0, 0, 0);
        ret = set_baud_cb();
        if(ret == DS_MUX_FAILURE_MACRO)
        {
          LOG_MSG_ERROR("Failed to sent baud rate", 0, 0, 0);
        }
      }
      /*Deregister the function once the baud rate is set*/
      set_baud_cb = NULL;
    }
  }
}

/*===========================================================================

FUNCTION SEND_MSG_FROM_DS_MUX_TO_UART

DESCRIPTION

  This function will send data from DS_MUX module to the UART

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


ds_mux_result_enum_type send_msg_from_ds_mux_to_uart
(
  void*      data,
  uint32_t   len
)
{
  ds_mux_uart_tty_send_msg(data, len);
  return DS_MUX_SUCCESS;
}

