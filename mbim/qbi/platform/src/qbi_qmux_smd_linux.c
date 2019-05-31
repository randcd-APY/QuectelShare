/*!
  @file
  qbi_qmux_smd_linux.c

  @brief
  Linux implememtation for EXT_QMUX messages exchanges directly over SMD port
  This replaces QMUXD Client APIs qmi_qmu* since QMUX transport will be removed
  from QCCI.
*/

/*=============================================================================

  Copyright (c) 2007-2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
03/13/18  ha   Change smdcntl8/DATA40 to smdcntl0/DATA5 to allow QTI and QBI
               to send control msgs to modem simultaneously.
07/27/17  rv   Added pthread attr to make sure resources are released after 
               pthread cancel
09/10/15  hz   Added support for qmux over smd
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_qmux_smd.h"
#include "qbi_common.h"
#include "qbi_qmux.h"

#include "control_service_v01.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! SMD device special file for QMUX msg I/O with Q6 modem */
#define QBI_QMUX_SMD_DEVFILE_PATH "/dev/smdcntl0"

#define SMD_PKT_IOCTL_MAGIC (0xC2)
#define SMD_PKT_IOCTL_BLOCKING_WRITE \
  _IOR(SMD_PKT_IOCTL_MAGIC, 0, unsigned int)

/*=============================================================================

  Private Typedefs

=============================================================================*/

typedef struct {
  /*! Device file descriptor used for msg IO with QMUX SMD port */
  int dev_fd;

  /*! Buffer used for received data from read() on SMD port */
  unsigned char rx_buf[5000];

  /*! Thread id of QMUX SMD RX reader thread */
  pthread_t  rx_th_id;

  /*! Registered callback function for received QMUX messages */
  qbi_qmux_smd_msg_cb_f *rx_cb_f;
} qbi_qmux_smd_info_s;


/*=============================================================================

  Private Variables

=============================================================================*/

static qbi_qmux_smd_info_s qbi_qmux_smd_info;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/
static void qbi_qmux_smd_rx_msg
(
  unsigned char *rx_buf_ptr,
  int            rx_buf_len
);

static void *qbi_qmux_smd_rx_thread
(
  void *in_param
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmux_smd_rx_msg
===========================================================================*/
/*!
    @brief Parse message and pass onto registered call back.

    @details

    @param msg_buf
    @param msg_len Size of message in bytes
*/
/*=========================================================================*/
static void qbi_qmux_smd_rx_msg
(
  unsigned char *rx_buf_ptr,
  int            rx_buf_len
)
{
  unsigned short     length;
  int                rem_bytes;
  int                msg_len = 0;
  qbi_qmux_if_type_t if_type;
  qbi_qmux_msg_s    *qmux_msg = (qbi_qmux_msg_s *)rx_buf_ptr;
/*-------------------------------------------------------------------------*/
  /* If there are multiple QMI messages, extract one at a time and deliver to
     registed call back */
  for (rem_bytes = rx_buf_len; rem_bytes > 0;
       rem_bytes -= (msg_len + QBI_QMUX_HDR_LEN_BYTES))
  {
    /* Read the I/F byte, make sure it is type QMUX */
    if_type = qmux_msg->if_type;
    if (if_type != QBI_QMUX_IF_TYPE_QMUX)
    {
      QBI_LOG_E_1("qbi_qmux_smd_rx_msg: Received invalid I/F byte = %d\n", if_type);
      return;
    }

    /* Read the message length */
    length = qmux_msg->qmux_hdr.length;
    QBI_LOG_D_1("Received QMUX msg length(%d)", length);

    /* verify length is at least (QMI_QMUX_HDR_SIZE -1) long. Per spec,
       length of the QMUX message includes the QMUX header itself, but not
       the preamble I/F type. */
    if (length < (QBI_QMUX_HDR_LEN_BYTES - 1))
    {
      QBI_LOG_E_1("qbi_qmux_smd_rx_msg: Received invalid length(%d)", length);
      return;
    }

    /* Verify that the length of the packet is same as length field minus 1. */
    msg_len = length + 1;
    if (rem_bytes < (int)msg_len)
    {
      QBI_LOG_E_2("qbi_qmux_smd_rx_msg: ERROR - packet rem_bytes < length (%d)"
                  " + 1\n", rem_bytes, (int)length);
      return;
    }

    /* Pass msg to registered callback */
    if (qbi_qmux_smd_info.rx_cb_f == NULL)
    {
      QBI_LOG_E_0("qbi_qmux_smd_rx_msg: NO registered callback for RX msg");
      return;
    }
    qbi_qmux_smd_info.rx_cb_f((unsigned char *)qmux_msg, msg_len);

    /* Go to the beginning of the next QMI message (if any) */
    qmux_msg = (qbi_qmux_msg_s *)((char *)qmux_msg + msg_len);
  }
} /* qbi_qmux_smd_rx_msg() */

/*===========================================================================
  FUNCTION: qbi_qmux_smd_rx_thread
===========================================================================*/
/*!
    @brief QMUX SMD RX reader thread

    @details
    Forever loop - blocking read on QMUX SMD file descriptor
    Each msg read is passed to qbi_qmux_smd_rx_msg(msg, msglen)

    @param in_param - not used
*/
/*=========================================================================*/
static void *qbi_qmux_smd_rx_thread
(
  void *in_param
)
{
  struct pollfd pollfd;
  int           rc;
  int           num_read;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(in_param);

  pollfd.fd = qbi_qmux_smd_info.dev_fd;
  pollfd.events = POLLIN | POLLPRI | POLLERR;

  /* Forever loop */
  while (1)
  {
    if (qbi_qmux_smd_info.dev_fd < 0)
    {
      break;
    }

    if ((rc = poll( &pollfd, 1, -1 )) < 0)
    {
      QBI_LOG_E_3("Got error from poll() call = %d, errno=%d, %s", rc, errno,
                  strerror(errno));
      sleep(1);
    }
    else if ((num_read = read(qbi_qmux_smd_info.dev_fd,
                              (void *)qbi_qmux_smd_info.rx_buf,
                              sizeof(qbi_qmux_smd_info.rx_buf)))
              < 0)
    {
      QBI_LOG_E_2("Read failed with return value %d, errno %d", num_read,
                  errno);
      sleep(1);
    }
    else
    {
      qbi_qmux_smd_rx_msg(qbi_qmux_smd_info.rx_buf, num_read);
    }
  }

  pthread_exit(0);
  return NULL;
} /* qbi_qmux_smd_rx_thread */

/*===========================================================================
  FUNCTION: qbi_qmux_smd_send_ctl_sync_msg()
===========================================================================*/
/*!
    @brief Send QMI_CTL_SYNC_REQ

    @details Send QMI_CTL_SYNC_REQ to tell modem to stop sending SYNC INDs.

    @param

    @return void
*/
/*=========================================================================*/
static void qbi_qmux_smd_send_ctl_sync_msg
(
  void
)
{
  qbi_qmux_msg_s     qmux_msg;
/*-------------------------------------------------------------------------*/
  QBI_LOG_D_1("Sending initial QMI_CTL_SYNC msg length(%d)",
              QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES);

  QBI_MEMSET(&qmux_msg, 0, sizeof(qbi_qmux_msg_s));

  qmux_msg.if_type                = QBI_QMUX_IF_TYPE_QMUX;
  qmux_msg.qmux_hdr.length        = QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES - 1;
  qmux_msg.sdu.qmi_ctl.hdr.txn_id = 1;
  qmux_msg.sdu.qmi_ctl.msg.msg_id = QMI_CTL_SYNC_REQ_V01;

  (void) qbi_qmux_smd_write_msg((unsigned char *)&qmux_msg,
                                QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES);
}  /* qbi_qmux_smd_send_ctl_sync_msg() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmux_smd_close
===========================================================================*/
/*!
    @brief Cancel receiver thread and close QMUX SMD port.

    @details
    Cancel rx thread and close SMD port

    @param

    @return void
*/
/*=========================================================================*/
void qbi_qmux_smd_close
(
  void
)
{
/*-------------------------------------------------------------------------*/
  (void)pthread_cancel(qbi_qmux_smd_info.rx_th_id);
  (void)close(qbi_qmux_smd_info.dev_fd);

  qbi_qmux_smd_info.dev_fd = -1;
} /* qbi_qmux_smd_close() */

/*===========================================================================
  FUNCTION: qbi_qmux_smd_init
===========================================================================*/
/*!
    @brief Opens QMUX SMD port and starts receiver thread for QMUX msg IO.

    @details
    Open QMUX SMD device file, create rx thread then send sync msg.

    @param rx_cb_f Callback function executed for each received QMUX msg

    @return boolean
*/
/*=========================================================================*/
boolean qbi_qmux_smd_init
(
  qbi_qmux_smd_msg_cb_f *rx_cb_f
)
{
  int blocking_write = 1;
  pthread_attr_t attr;
  int res;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(rx_cb_f);

  qbi_qmux_smd_info.dev_fd = open(QBI_QMUX_SMD_DEVFILE_PATH, O_RDWR);
  if (qbi_qmux_smd_info.dev_fd < 0)
  {
    QBI_LOG_E_1("Unable to open SMD port, errno %d.  Device filename in next"
                " log", errno);
    QBI_LOG_E_0(QBI_QMUX_SMD_DEVFILE_PATH);
    return FALSE;
  }

  qbi_qmux_smd_info.rx_cb_f =  rx_cb_f;

  /* Set write call to be blocking for SMD port */
  (void)ioctl(qbi_qmux_smd_info.dev_fd, SMD_PKT_IOCTL_BLOCKING_WRITE,
              &blocking_write);

  res = pthread_attr_init(&attr);
  if (res != 0)
  {
     QBI_LOG_E_0("Attribute init failed");
  }
  //This will make sure that created thread will have its resources
  //released immediately after pthread_cancel is sent.
  res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (res != 0)
  {
     QBI_LOG_E_0("Setting detached state failed");
  }
  res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  if (res != 0)
  {
     QBI_LOG_E_0("Setting cancel state failed");
  }
  /* Spawn RX thread and pass to it the QMUX connection ID */
  if ((pthread_create(&qbi_qmux_smd_info.rx_th_id,
                      &attr,
                      qbi_qmux_smd_rx_thread,
                      NULL))
      != 0)
  {
    QBI_LOG_E_0("Could not create QMUX SMD RX thread");
    return FALSE;
  }

  pthread_attr_destroy(&attr);
  qbi_qmux_smd_send_ctl_sync_msg();

  return TRUE;
} /* qbi_qmux_smd_init() */

/*===========================================================================
  FUNCTION: qbi_qmux_smd_write_msg
===========================================================================*/
/*!
    @brief Write QMUX message to SMD port.

    @details

    @param write_msg
    @param write_msg_len

    @return boolean
*/
/*=========================================================================*/
boolean qbi_qmux_smd_write_msg
(
  unsigned char *write_msg,
  uint32         write_msg_len
)
{
  int rc;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(write_msg);

  if (qbi_qmux_smd_info.dev_fd < 0)
  {
    QBI_LOG_E_0("Write aborted - SMD port not open");
    return FALSE;
  }

  if ((rc = write(qbi_qmux_smd_info.dev_fd, (void *) write_msg,
                  write_msg_len))
      < 0)
  {
    QBI_LOG_E_2("Write failed, rc = %d, errno = %d!\n", rc, errno);
    return FALSE;
  }

  return TRUE;
} /* qbi_qmux_smd_write_msg() */

