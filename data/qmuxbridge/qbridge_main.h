/******************************************************************************

                        QBRIDGE_MAIN.H

******************************************************************************/

/******************************************************************************

  @file    qbridge_main.h
  @brief   Handle initialization of basic routine for communication with
           modem and USB Layer

  ---------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when       who        what, where, why
--------   ---        --------------------------------------------------------
05/02/18   rv         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <cutils/memory.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <semaphore.h>
#include <poll.h>

#include "data_port_mapper_v01.h"

/*--------------------------------------------------------------------------
 Generic definitions
---------------------------------------------------------------------------*/
#define QBRIDGE_SUCCESS                   (0)
#define QBRIDGE_FAILURE                   (-1)
#define QBRIDGE_TRUE                      (1)
#define QBRIDGE_FALSE                     (0)
#define QBRIDGE_QMI_CLIENT_MAX_RETRY      (10)
#define QBRIDGE_FILE_NODE_OPEN_MAX_RETRY  (10)
#define QBRIDGE_MAX_FILE_NAME_SIZE        (50)
#define QBRIDGE_ERROR                     (-1)

#define MAX_QMI_PACKET_SIZE               (8192)

/*! Device node used to communicate on modem side.*/
#define QBRIDGE_SMD_DEV_FILE           "/dev/smdcntl1"
/*! Device node used to communicate on HOST side */
#define QBRIDGE_USB_PH_DEV_FILE        "/dev/gps"


#define SMD_PKT_IOCTL_MAGIC (0xC2)
#define SMD_PKT_IOCTL_BLOCKING_WRITE \
  _IOR(SMD_PKT_IOCTL_MAGIC, 0, unsigned int)

/*! @brief Helper macro to return if a pointer is NULL
*/
#define QBRIDGE_CHECK_NULL_PTR_RET(ptr) \
  if ((ptr) == NULL) \
  { \
    QBRIDGE_LOG_E_0("Unexpected NULL pointer!"); \
    return; \
  }

/*! @brief Helper macro to return FALSE if a pointer is NULL
*/
#define QBRIDGE_CHECK_NULL_PTR_RET_FALSE(ptr) \
  if ((ptr) == NULL) \
  { \
    QBRIDGE_LOG_E_0("Unexpected NULL pointer!"); \
    return FALSE; \
  }

/*! @brief Helper macro to return NULL if a pointer is NULL
*/
#define QBRIDGE_CHECK_NULL_PTR_RET_NULL(ptr) \
  if ((ptr) == NULL) \
  { \
    QBRIDGE_LOG_E_0("Unexpected NULL pointer!"); \
    return NULL; \
  }

#define QBRIDGE_ARG_NOT_USED(arg) (void) arg

#define QBRIDGE_MEMSCPY(dst, dst_size, src, bytes_to_copy) \
  (void) memcpy(dst, src, MIN(dst_size, bytes_to_copy))

#define QBRIDGE_MEMSET memset

#define QBRIDGE_OS_MALLOC(size)  malloc(size)

/*--------------------------------------------------------------------------
  RMNET DECALRATIONS
---------------------------------------------------------------------------*/
typedef void * qmi_qmux_if_hndl_t;

typedef struct
{
  int           ph_iface_fd;
  char          ph_iface_device_file[QBRIDGE_MAX_FILE_NAME_SIZE];
  boolean       usb_enabled;
}ph_iface_info;

typedef struct {
  /*! Device file descriptor used for msg IO with SMD port */
  int dev_fd;

  /*! Buffer used for received data from read() on SMD port */
  unsigned char rx_buf[MAX_QMI_PACKET_SIZE];
} qbridge_qmux_smd_info_s;

/*! State of the context. */
typedef enum {
  QBRIDGE_STATE_CLOSED  = 0,
  QBRIDGE_STATE_OPENING = 1,
  QBRIDGE_STATE_OPENED  = 2
} qbridge_state_e;

/*-------------------------------------------------------------------------
  RmNet config parameters
--------------------------------------------------------------------------*/
typedef struct{
  ph_iface_info                ph_iface;

  /*! Used to store thread id of the SMD thread */
  pthread_t                    rx_th_id;

  qbridge_qmux_smd_info_s      qbridge_qmux_smd_info;
  /*! QMUXBRIDGE states */
  qbridge_state_e              state;
  /*! TRUE if SSR is in progress */
  uint32                       ssr_in_progress;
}qbridge_rmnet_param;

/*===========================================================================
  FUNCTION: qbridge_send_msg_to_host
===========================================================================*/
/*!
    @brief Sends a raw QMUX message to the host

    @details

    @param data
    @param len

    @return None
*/
/*=========================================================================*/
void qbridge_send_msg_to_host
(
  void      *data,
  int32     len
);

/*===========================================================================
  FUNCTION: qbridge_set_state
===========================================================================*/
/*!
    @brief Set the qmuxbridge internal state machine

    @details Set the qmuxbridge internal state machine

    @param state to be set to

    @return void
*/
/*=========================================================================*/
void qbridge_set_state
(
  qbridge_state_e state
);

/*===========================================================================
  FUNCTION: qbridge_query_state
===========================================================================*/
/*!
    @brief Queries the current qmuxbridge internal state machine

    @details Queries the qmuxbridge internal state machine

    @param none

    @return qbridge_state_e
*/
/*=========================================================================*/
qbridge_state_e qbridge_query_state
(
);