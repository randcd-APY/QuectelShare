/******************************************************************************

                        QBRIDGE_SMD.C

******************************************************************************/

/******************************************************************************

  @file    qbridge_smd.c
  @brief   Handles initialization and interaction with SMD Interface

  ---------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
09/10/18    nk         Added check for smd buffer len prior to read
05/02/18    rv         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "qbridge_log_qxdm.h"
#include "qbridge_qmux.h"
#include "qbridge_main.h"
#include "qbridge_smd.h"

/*=============================================================================

  Private Function Prototypes

=============================================================================*/
static void *qbridge_qmux_smd_rx_thread
(
  qbridge_rmnet_param  *qbridge_config_param
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_qmux_smd_rx_thread
===========================================================================*/
/*!
    @brief QMUX SMD RX reader thread

    @details
    Forever loop - blocking read on QMUX SMD file descriptor
    Each msg read is passed to qbridge_send_msg_to_host(msg, msglen)

    @param qbridge_config_param
*/
/*=========================================================================*/
static void *qbridge_qmux_smd_rx_thread
(
  qbridge_rmnet_param  *qbridge_config_param
)
{
  int32  num_read = 0;
  int  rc = 0;
  struct pollfd pollfd;
  unsigned char *temp_buf = NULL;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET(qbridge_config_param);

  pollfd.fd = qbridge_config_param->qbridge_qmux_smd_info.dev_fd;
  pollfd.events = POLLIN | POLLPRI | POLLERR;

  QBRIDGE_LOG_D_1("QMUXBRIDGE :: QMI/SMD Buffer Size %d",MAX_QMI_PACKET_SIZE);
  /* Forever loop */
  while (SMD_TRUE)
  {
    if (qbridge_config_param->qbridge_qmux_smd_info.dev_fd < 0)
    {
      QBRIDGE_LOG_E_1("QMUXBRIDGE :: SMD FD is invalid %d.Exiting Loop ",
                      qbridge_config_param->qbridge_qmux_smd_info.dev_fd);
      break;
    }

    if ((rc = poll( &pollfd, 1, -1 )) < 0)
    {
      QBRIDGE_LOG_E_2("QMUXBRIDGE :: Got error from poll() call = %d"
                      ", errno=%d ", rc, errno);
      sleep(1);
    }
    else if ((num_read = 
               read(qbridge_config_param->qbridge_qmux_smd_info.dev_fd,
                   (void *)qbridge_config_param->qbridge_qmux_smd_info.rx_buf,
                   sizeof(qbridge_config_param->qbridge_qmux_smd_info.rx_buf)))
                   < 0)
    {
      QBRIDGE_LOG_E_2("QMUXBRIDGE :: Read failed with return value %d "
                      "errno %d ",num_read,errno);

      /* If error code is ETOOSMALL i.e. 525 we allocate a
         large buffer , read the data in large buffer and
         drop the same because data more than 8K is not
         supported for now */
      if ((num_read == QBRIDGE_ERROR) && (errno == 525))
      {
        if (NULL == temp_buf)
        {
          temp_buf = qbridge_mem_malloc_clear(SMD_DATA_TEMP_BUF_LEN);
          QBRIDGE_CHECK_NULL_PTR_RET_NULL(temp_buf);
          QBRIDGE_LOG_D_1("QMUXBRIDGE :: Allocated buffer of size %d",
                           SMD_DATA_TEMP_BUF_LEN);
        }

        /* Faking read to make it successful and dropping the packet */
        if ((num_read =
             read(qbridge_config_param->qbridge_qmux_smd_info.dev_fd,
                 (void *)temp_buf,
                 SMD_DATA_TEMP_BUF_LEN)) < 0)
        {
          QBRIDGE_LOG_E_2("QMUXBRIDGE :: Read failed even with dynamic buffer"
                          "size with return value %d errno %d ",num_read,errno);

        }
        else
        {
          QBRIDGE_LOG_D_1("QMUXBRIDGE :: Read buffer of size %d,Dropping.",num_read);
          QBRIDGE_MEMSET(temp_buf, 0 ,SMD_DATA_TEMP_BUF_LEN);
          free(temp_buf);
          temp_buf = NULL;
        }
      }
      else
      {
        /* Recovery mechanism to wait and re-try reading */
        sleep(1);
      }
    }
    else
    {
      qbridge_send_msg_to_host(
            (unsigned char *)qbridge_config_param->qbridge_qmux_smd_info.rx_buf,
             num_read);
    }
  }

  if (temp_buf != NULL)
  {
    free(temp_buf);
    temp_buf = NULL;
  }

  QBRIDGE_LOG_D_0("QMUXBRIDGE :: Exiting :: Blocking read on QMUX SMD "
                  "file descriptor");
  pthread_exit(0);

  return NULL;
} /* qbridge_qmux_smd_rx_thread */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_qmux_smd_wait()
===========================================================================*/
/*!
    @brief

    @details Wait for QMUX thread to exit

    @param

    @return void
*/
/*=========================================================================*/
void qbridge_qmux_smd_wait
(
  qbridge_rmnet_param  *qbridge_config_param
)
{
  uint32        num_read = 0;
  unsigned char rx_buf[SMD_DATA_BUF_LEN] = {0,};
  uint32        ret_val = 0;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET(qbridge_config_param);

  while (SMD_TRUE)
  {
    if (qbridge_config_param->ph_iface.ph_iface_fd < 0)
    {
      QBRIDGE_LOG_D_0("QMUXBRIDGE :: usb fd is invalid.Retrying.");
      qbridge_config_param->ph_iface.ph_iface_fd = DEFAULT_FD;
      ret_val = qbridge_file_open(&(qbridge_config_param->ph_iface.ph_iface_fd));
      if (ret_val != QBRIDGE_SUCCESS) 
      {
        QBRIDGE_LOG_D_0("QMUXBRIDGE :: unable to open usb interface.");
        break;
      }
      else
      {
        continue;
      }
    }
    else if ((num_read = read(qbridge_config_param->ph_iface.ph_iface_fd,
                               (void *)rx_buf,
                               sizeof(rx_buf)))
                    < 0)
    {
      QBRIDGE_LOG_E_2("QMUXBRIDGE :: Read failed with return value %d, "
                      "errno %d ", num_read,errno);
    }
    else
    {
      if (SMD_DATA_BUF_LEN >= num_read &&
        qbridge_qmux_tx_to_modem((qbridge_qmux_msg_s *)rx_buf, num_read))
      {
        QBRIDGE_LOG_D_1("QMUXBRIDGE :: Written %d bytes to SMD",num_read);
      }
      else
      {
        QBRIDGE_LOG_D_1("QMUXBRIDGE :: Failed to write %d bytes to "
                        "SMD.",num_read);
      }
    }
  }

  QBRIDGE_LOG_D_0("QMUXBRIDGE :: Bailing Out from Main Thread.");
  return;
} /* qbridge_qmux_smd_wait() */

/*===========================================================================
  FUNCTION: qbridge_qmux_smd_init
===========================================================================*/
/*!
    @brief Opens QMUX SMD port and starts receiver thread for QMUX msg IO.

    @details
    Open QMUX SMD device file, create rx thread then send sync msg.

    @param rx_cb_f Callback function executed for each received QMUX msg

    @return boolean
*/
/*=========================================================================*/
boolean qbridge_qmux_smd_init
(
  qbridge_rmnet_param  *qbridge_config_param
)
{
  int  blocking_write = SMD_BLOCKING_WRITE_TRUE;
  pthread_attr_t  attr;
  int  res = 0;
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET_FALSE(qbridge_config_param);

  QBRIDGE_LOG_STR_1("QMUXBRIDGE :: Opening %s",QBRIDGE_SMD_DEV_FILE);
  qbridge_config_param->qbridge_qmux_smd_info.dev_fd =
                           open(QBRIDGE_SMD_DEV_FILE, O_RDWR);
  if (qbridge_config_param->qbridge_qmux_smd_info.dev_fd < 0)
  {
    QBRIDGE_LOG_E_1("QMUXBRIDGE :: Unable to open SMD port, errno %d "
                    ,errno);
    return QBRIDGE_FAILURE;
  }
  else
  {
    QBRIDGE_LOG_D_1("QMUXBRIDGE :: Opened QMUX SMD port FD %d"
                    ,qbridge_config_param->qbridge_qmux_smd_info.dev_fd);
  }

  /* Set write call to be blocking for SMD port */
  (void)ioctl(qbridge_config_param->qbridge_qmux_smd_info.dev_fd,
              SMD_PKT_IOCTL_BLOCKING_WRITE, &blocking_write);

  res = pthread_attr_init(&attr);
  if (res != 0)
  {
     QBRIDGE_LOG_E_0("QMUXBRIDGE :: Attribute init failed");
  }
  //This will make sure that created thread will have its resources
  //released immediately after pthread_cancel is sent.
  res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (res != 0)
  {
     QBRIDGE_LOG_E_0("QMUXBRIDGE :: Setting detached state failed");
  }
  res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  if (res != 0)
  {
     QBRIDGE_LOG_E_0("QMUXBRIDGE :: Setting cancel state failed");
  }
  /* Spawn RX thread and pass to it the QMUX connection ID */
  if ((pthread_create(&qbridge_config_param->rx_th_id,
                      &attr,
                      qbridge_qmux_smd_rx_thread,
                      qbridge_config_param))
      != 0)
  {
    QBRIDGE_LOG_E_0("QMUXBRIDGE :: Could not create QMUX SMD RX thread");
    return QBRIDGE_FAILURE;
  }

  pthread_attr_destroy(&attr);

  return QBRIDGE_SUCCESS;
} /* qbridge_qmux_smd_init() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_qmux_smd_close
===========================================================================*/
/*!
    @brief Cancel receiver thread and close QMUX SMD port and sets the fd
           to default.

    @details
    Cancel rx thread and close SMD port

    @param qbridge_config_param

    @return void
*/
/*=========================================================================*/
void qbridge_qmux_smd_close
(
  qbridge_rmnet_param  *qbridge_config_param
)
{
/*-------------------------------------------------------------------------*/
  QBRIDGE_CHECK_NULL_PTR_RET(qbridge_config_param);

  (void)pthread_cancel(qbridge_config_param->rx_th_id);

  (void)close(qbridge_config_param->qbridge_qmux_smd_info.dev_fd);
  qbridge_config_param->qbridge_qmux_smd_info.dev_fd = DEFAULT_FD;

  return;
} /* qbridge_qmux_smd_close() */
