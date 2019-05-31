/******************************************************************************

                        DS_MUX_SMD_TTY.C

******************************************************************************/

/******************************************************************************

  @file    ds_mux_smd_tty.c
  @brief   Qualcomm Technologies, Inc. Tethering Interface.
           This file contains DS_MUX interaction with SMD

  DESCRIPTION
  Implementation file for DS_MUX inteaction with SMD.

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

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/19/14   cp         Initial version

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


#include "ds_mux_main.h"

ds_mux_smd_tty_config*                smd_tty_info = NULL;

extern ds_mux_smd_tty_config          smd_tty_fd1;
extern ds_mux_smd_data_config         smd_data_fds;
extern boolean                        ds_mux_bridge_active;


/*===========================================================================
                          FUNCTION DEFINITIONS
============================================================================*/
/*===========================================================================*/

int ds_mux_set_dtr_high(int fd)
{
  int             dtr_sig;
  int             ret = DS_MUX_SUCCESS_MACRO;
  /*-------------------------------------------------------------------------
    Set DTR high
   -------------------------------------------------------------------------*/
  dtr_sig = 0;

  LOG_MSG_INFO1("Setting DTR bit HIGH",0, 0, 0);

  /*--------------------------------------------------------------------------
  TIOCM_DTR - Data Terminal Ready
  TIOCM_RTS - Request To Send
  TIOCM_CD - Data Carrier Detect
  ----------------------------------------------------------------------------*/

  dtr_sig |= (TIOCM_DTR|TIOCM_RTS|TIOCM_CD);

  LOG_MSG_INFO1("DTR bit will be set to dtr_sig:%d ",
                dtr_sig, 0, 0);

  if((ioctl(fd, TIOCMSET, (void *)&dtr_sig)) == -1)
  {
    LOG_MSG_ERROR("Ioctl call to set DTR bit failed. DTR %d error num=%d error=%s",
                   dtr_sig, errno,strerror (errno));
    ret = DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOG_MSG_INFO1("Success Setting DTR bit HIGH ",0, 0, 0);
    ret=DS_MUX_SUCCESS_MACRO;
  }
  return ret;
}

int ds_mux_set_dtr_low(int fd)
{
  int             dtr_sig;
  int             ret = DS_MUX_SUCCESS_MACRO;
  /*-------------------------------------------------------------------------
    Set DTR low
    -------------------------------------------------------------------------*/
  dtr_sig = 0;

  LOG_MSG_INFO1("Setting DTR bit LOW", 0, 0, 0);

  dtr_sig |= (~(TIOCM_DTR |TIOCM_RTS |TIOCM_CD));

  LOG_MSG_INFO1("Will clear DTR bit. DTR %d", dtr_sig, 0, 0);

  if(ioctl(fd, TIOCMSET, (void *)&dtr_sig) == -1)
  {
    LOG_MSG_ERROR("Ioctl call to reset DTR bit failed. DTR %d error num=%d error=%s",
                   dtr_sig, errno,strerror (errno));
    ret = DS_MUX_FAILURE_MACRO;
  }
  else
  {

    LOG_MSG_INFO1("Success Setting DTR bit LOW ",0, 0, 0);
    ret=DS_MUX_SUCCESS_MACRO;
  }
  return ret;
}

ds_mux_result_enum_type send_msg_ds_mux_to_smd(uint8 dlci, void* data, int len)
{
  int_32 ret;

  if(dlci == DS_MUX_DLCI_ID_1)
  {
    LOG_MSG_INFO1("Writing on DLCI 1 ", 0, 0, 0);
    ret = write(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_1_IDX], (char*)data, len);
  }
  else if(dlci == DS_MUX_DLCI_ID_2)
  {
    LOG_MSG_INFO1("Writing on DLCI 2 ", 0, 0, 0);
    ret = write(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_2_IDX], (char*)data, len);
  }
  else if(dlci = DS_MUX_DLCI_ID_3)
  {
    LOG_MSG_INFO1("Writing on DLCI 3 ", 0, 0, 0);
    ret = write(smd_data_fds.smd_data_fd[SMD_DATA_FDS_DLCI_3_IDX], (char*)data, len);
  }
  if (ret == -1)
  {
    LOG_MSG_ERROR("Couldn't send message to modem: %d", errno, 0, 0);
  }
  else if (ret != len)
  {
    LOG_MSG_ERROR("Unexpected return value when writing to device file: got %d, "
                  "expected %d (errno %d)", ret, len, errno);
    return DS_MUX_FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Successfully sent message to modem\n", 0, 0, 0);
  }
  return DS_MUX_SUCCESS;
}


static int ds_mux_smd_data_files_open
(
   ds_mux_smd_data_config* smd_data_config_info
)
{
/*--------------------------------------------------------------------------*/
  if (smd_data_config_info == NULL )
  {
    LOG_MSG_ERROR("NULL Args",0,0,0);
    return DS_MUX_FAILURE_MACRO;
  }

  smd_data_config_info->smd_data_fd[0] = open(SMD_DATA2, O_RDWR);
  if(smd_data_config_info->smd_data_fd[0] == -1)
  {
    LOG_MSG_ERROR("Could not open device file SMD_DATA2, errno : %d ",errno, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  smd_data_config_info->smd_data_fd[1] = open(SMD_DATA3, O_RDWR);
  if(smd_data_config_info->smd_data_fd[1] == -1)
  {
    LOG_MSG_ERROR("Could not open device file SMD_DATA3, errno : %d ",errno, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  smd_data_config_info->smd_data_fd[2] = open(SMD_DATA4, O_RDWR);
  if(smd_data_config_info->smd_data_fd[2] == -1)
  {
    LOG_MSG_ERROR("Could not open device file SMD_DATA4, errno : %d ",errno, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }

  LOG_MSG_INFO1("Successfully opened 3 smd data files with fd: %d, %d, %d", smd_data_config_info->smd_data_fd[0], smd_data_config_info->smd_data_fd[1], smd_data_config_info->smd_data_fd[2]);

  return DS_MUX_SUCCESS_MACRO;
}


int ds_mux_smd_data_fds_listener_init
(
  ds_mux_smd_data_config* smd_data_config_info,
  ds_mux_sk_fd_set_info_t* fd_set,
  ds_mux_sock_thrd_fd_read_f read_f
)
{
  int retval;
    /* Check for NULL Args. */
  if (smd_data_config_info == NULL || fd_set == NULL)
  {
    LOG_MSG_ERROR("NULL Args",0,0,0);
    return DS_MUX_FAILURE_MACRO;
  }

  LOG_MSG_INFO1("Open Three SMD channels", 0, 0, 0);

  retval = ds_mux_smd_data_files_open(smd_data_config_info);

  if(retval == DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_ERROR("Failed to open smd device files. Abort", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOG_MSG_INFO1("Opened three smd files with fd: %d, %d, %d", smd_data_config_info->smd_data_fd[0], smd_data_config_info->smd_data_fd[1], smd_data_config_info->smd_data_fd[2]);

    if (ds_mux_addfd_map(fd_set, smd_data_config_info->smd_data_fd[0], read_f, MAX_NUM_OF_FD) ==
                                                             DS_MUX_FAILURE_MACRO)
    {
      LOG_MSG_ERROR("cannot add the smddata1 file for reading",0,0,0);
      close(smd_data_config_info->smd_data_fd[0]);
      return DS_MUX_FAILURE_MACRO;
    }

    if (ds_mux_addfd_map(fd_set, smd_data_config_info->smd_data_fd[1], read_f, MAX_NUM_OF_FD) ==
                                                             DS_MUX_FAILURE_MACRO)
    {
      LOG_MSG_ERROR("cannot add the smddata2 file for reading",0,0,0);
      close(smd_data_config_info->smd_data_fd[1]);
      return DS_MUX_FAILURE_MACRO;
    }

    if (ds_mux_addfd_map(fd_set, smd_data_config_info->smd_data_fd[2], read_f, MAX_NUM_OF_FD) ==
                                                             DS_MUX_FAILURE_MACRO)
    {
      LOG_MSG_ERROR("cannot add the smddata3 file for reading",0,0,0);
      close(smd_data_config_info->smd_data_fd[2]);
      return DS_MUX_FAILURE_MACRO;
    }

  LOG_MSG_INFO1("Added SMD 3 SMD-DATA FDs for reading ", 0, 0, 0);

  }
  return DS_MUX_SUCCESS_MACRO;
}

int ds_mux_smd_ch_open()
{
  int retval;

  LOG_MSG_INFO1("Opening DATA SMD channels", 0, 0, 0);

   memset(&smd_data_fds, 0, sizeof(ds_mux_smd_data_config));

   retval = ds_mux_smd_data_fds_listener_init(&smd_data_fds, &sk_fdset,ds_smd_tty_recv_msg);

  if(retval == DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_ERROR("Failed to initialize DS_MUX SMD TTY listener",0,0,0);
    return DS_MUX_FAILURE_MACRO;
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*-----------------------------------------------------------------------------
FUNCTION QTI_SMD_TTY_RECV_MSG()

DESCRIPTION

  This function
  - receives AT commands from MODEM.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

void open_smd_fr_legacy_at_cmds()
{
    int retval = DS_MUX_SUCCESS_MACRO;
  /*-----------------------------------------------------------------------
  Initialize DS_MUX variables
  ------------------------------------------------------------------------*/

  memset(&smd_tty_fd1, 0, sizeof(ds_mux_smd_tty_config));

  /*---------------------------------------------------------------------
  Call into the SMD TTY listener init function which sets up DS_MUX to
  listen to AT Commands coming in from the Modem for DUN
---------------------------------------------------------------------*/
  retval = ds_mux_smd_tty_listener_init(&smd_tty_fd1,
                                      &sk_fdset,
                                      ds_smd_tty_recv_msg);
  if(retval != DS_MUX_SUCCESS_MACRO)
  {
    LOG_MSG_ERROR("Failed to initialize DS_MUX SMD TTY listener",0,0,0);
    return;
  }
}

int ds_smd_tty_recv_msg
(
   int smd_tty_fd
)
{
  int        ret, n_bytes = 0;
  int        ret_val;
  char*      smd_rx_buf = NULL;
  int_32     frame_len =0;

 /*-------------------------------------------------------------------------*/
  smd_rx_buf = ds_mux_allocate_memory_and_read(smd_tty_fd, &n_bytes);
  if(smd_rx_buf == NULL)
  {
    LOG_MSG_ERROR("Failed to allocate or read memory", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }


  //ATCOP module should now read from SMD

  LOG_MSG_INFO1("smd_rx_buf: %s len read: %d ds_mux_bridge_active: %d", smd_rx_buf, n_bytes, ds_mux_bridge_active);

  if(ds_mux_bridge_active && smd_tty_fd != smd_tty_info->smd_fd)
  {
    ret_val = ds_mux_prepare_send_uih_ui_frame(smd_tty_fd, smd_rx_buf, n_bytes);
    if(ret_val != DS_MUX_SUCCESS)
    {
      LOG_MSG_ERROR("Could-not send UI-UIH frame", 0, 0, 0);
      if(smd_rx_buf != NULL)
      {
        DS_MUX_MEM_FREE(smd_rx_buf);
        smd_rx_buf = NULL;
      }
      return DS_MUX_FAILURE_MACRO;
    }
  }
  else
  {
    // TTY.send to UART
    ds_mux_uart_tty_send_msg(smd_rx_buf, n_bytes);
  }

  if(smd_rx_buf != NULL)
  {
    DS_MUX_MEM_FREE(smd_rx_buf);
    smd_rx_buf = NULL;
  }

  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION QTI_SMD_TTY_FILE_OPEN()

DESCRIPTION

  This function
  - opens the device file which is used for interfacing with MODEM

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/


static int qti_smd_tty_file_open
(
   int * smd_tty_fd
)
{
  uint8 time_out = DS_MUX_MAX_RETRY;
  int qti_smd_tty_fd;
/*--------------------------------------------------------------------------*/

  /* Check for NULL Args. */
  if (smd_tty_fd == NULL)
  {
    LOGI("NULL Args");
    return DS_MUX_FAILURE_MACRO;
  }

  while(time_out)
  {
    qti_smd_tty_fd = open(SMD_DATA1, O_RDWR);
    if(qti_smd_tty_fd != -1)
    {
      LOGI("Opened the device successfully FD is %d", qti_smd_tty_fd);
      break;
    }
    LOGI("Trying to open SMD_DATA1 errno: %d", errno);
    usleep(SMD_DATA1_INIT_DELAY);
    time_out--;
  }
  if(time_out == 0 && qti_smd_tty_fd == -1)
  {
    LOGI("Tried For 10 seconds. Cound not open the device SMD_DATA1, errno : %d ", errno);
    exit(0);
  }

  *smd_tty_fd = qti_smd_tty_fd;

  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION QTI_SMD_TTY_LISTENER_INIT()

DESCRIPTION

  This function
  - opens the SMD TTY device file
  - adds the SMD TTY fd to wait on select call

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

int ds_mux_smd_tty_listener_init
(
  ds_mux_smd_tty_config  * smd_tty_config_info,
  ds_mux_sk_fd_set_info_t * fd_set,
  ds_mux_sock_thrd_fd_read_f read_f
)
{
  int ret_val;
/*-------------------------------------------------------------------------*/
  /* Check for NULL Args. */
  if (smd_tty_config_info == NULL || fd_set == NULL)
  {
    LOGI("NULL Args");
    return DS_MUX_FAILURE_MACRO;
  }

  LOGI("Open SMD TTY file to receive AT commands from modem");
  smd_tty_info = smd_tty_config_info;
  ret_val = qti_smd_tty_file_open(&(smd_tty_info->smd_fd));

  if(ret_val == DS_MUX_FAILURE_MACRO)
  {
    LOGI("Failed to open smd 7 device files. Abort", 0, 0, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  else
  {
    LOGI("Opened files fd is %d", smd_tty_info->smd_fd);

    if (ds_mux_addfd_map(fd_set, smd_tty_info->smd_fd, read_f, MAX_NUM_OF_FD) !=
                                                             DS_MUX_SUCCESS_MACRO)
    {
      LOGI("cannot map the fd",0,0,0);
      close(smd_tty_info->smd_fd);
      return DS_MUX_FAILURE_MACRO;
    }

  }
  return DS_MUX_SUCCESS_MACRO;
}


/*===========================================================================

FUNCTION QTI_SMD_TTY_SEND_MSG()

DESCRIPTION

  This function
  - send AT commands to Modem

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

void qti_smd_tty_send_msg
(
   void      *data,
   uint32_t   len
)
{
  int ret;
  int i = 0;
/*-----------------------------------------------------------------------*/

  /* Check for NULL Args. */
  if (data == NULL)
  {
    LOG_MSG_ERROR("NULL Args",0,0,0);
    return;
  }

  ret = write(smd_tty_info->smd_fd, (char*)data, len);
  if (ret == -1)
  {
    LOG_MSG_ERROR("Couldn't send message to modem: %d", errno, 0, 0);
  }
  else if (ret != len)
  {
    LOG_MSG_ERROR("Unexpected return value when writing to device file: got %d, "
                  "expected %d (errno %d)", ret, len, errno);
  }
  else
  {
    LOG_MSG_INFO1("Successfully sent message to modem\n", 0, 0, 0);
  }
  return;
}
