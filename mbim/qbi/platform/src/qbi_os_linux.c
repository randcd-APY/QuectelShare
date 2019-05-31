/*!
  @file
  qbi_os_linux.c

  @brief
  Operating system specific functions for Linux
*/

/*=============================================================================

  Copyright (c) 2012-2013, 2015-2016, 2018 Qualcomm Technologies, Inc.
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
05/08/18  nk   Removing temporary workaround for closing control port for GPS 
03/13/18  ha   Change smdcntl8/DATA40 to smdcntl0/DATA5 to allow QTI and QBI
               to send control msgs to modem simultaneously.
11/05/15  hz   Add OS wake lock support
09/10/15  hz   Add support for qmux over smd
10/14/13  bd   Add event logging facility for performance monitoring
07/23/13  bd   Disable unneeded QCCI ports at init to speed up MBIM_OPEN
02/11/13  bd   Add cross-platform support for QCCI setup, teardown, instance
05/21/12  bd   Add cross-platform QCCI OS parameters support
02/07/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_os_linux.h"

#include "qbi_common.h"
#include "qbi_hc.h"
#include "qbi_log.h"
#include "qbi_os.h"
#include "qbi_os_qcci.h"

#include "data_port_mapper_v01.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! Prefix for log messages written to /dev/kmsg. Using the default syslog
    priority and facility.
    @see https://www.kernel.org/doc/Documentation/ABI/testing/dev-kmsg */
#define QBI_OS_LINUX_KMSG_PREFIX "mbimd: "

/*! Magic value used to check whether an encoded pointer is valid (supports
    up to 64-bit architectures) */
#define QBI_OS_LINUX_ENCODED_POINTER_MAGIC ((uintptr_t) 0x17C9AE71D6E5F832)

/* When an error is encountered trying to initialize the random pointer, delay
   0.1s before retrying */
#define QBI_OS_LINUX_FPTR_RAND_FAILURE_DELAY_US (100000)

/*! Timeout for synchronous QCCI requests */
#define QBI_OS_LINUX_QCCI_TIMEOUT_MS (30000)

/* Wakelock related */
#define QBI_OS_LINUX_WAKELOCK_NAME   "mbimd"
#define QBI_OS_LINUX_WAKELOCK_FILE   "/sys/power/wake_lock"
#define QBI_OS_LINUX_WAKEUNLOCK_FILE "/sys/power/wake_unlock"

/* Control Port name for DPM */
#define QBI_OS_LINUX_CNTL_PORT_FOR_DPM "DATA5_CNTL"

/*=============================================================================

  Private Variables

=============================================================================*/

/*! Random value used to encode function pointers */
static uintptr_t qbi_os_linux_fptr_rand;

/*! Flag for OS wakelock support */
static boolean qbi_os_linux_wakelock_support_enabled = FALSE;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_os_linux_dpm_close
(
  const qbi_ctx_s *ctx
);

static boolean qbi_os_linux_dpm_get_gps_port_info
(
  const qbi_ctx_s                *ctx,
  char                           *control_port_name,
  uint32                          control_port_name_len,
  data_ep_id_type_v01            *data_ep_id,
  hardware_accl_port_details_v01 *bam_info
);

static boolean qbi_os_linux_dpm_get_port_info
(
  const qbi_ctx_s                *ctx,
  char                           *control_port_name,
  uint32                          control_port_name_len,
  data_ep_id_type_v01            *data_ep_id,
  hardware_accl_port_details_v01 *bam_info
);

static boolean qbi_os_linux_dpm_open
(
  const qbi_ctx_s *ctx
);

static boolean qbi_os_linux_dpm_send_qmi_req
(
  uint32 msg_id,
  void  *req_data,
  uint32 req_data_len,
  void  *rsp_data,
  uint32 rsp_data_len
);

static void qbi_os_linux_fptr_rand_init
(
  void
);

static const char *qbi_os_linux_log_event_to_str
(
  qbi_os_log_event_e event
);

static void qbi_os_linux_timer_notify_cb
(
  union sigval cb_data
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_os_linux_dpm_close
===========================================================================*/
/*!
    @brief Closes QBI's control+data ports via data port mapper

    @details

    @param ctx

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_os_linux_dpm_close
(
  const qbi_ctx_s *ctx
)
{
  dpm_close_port_req_msg_v01 *qmi_req;
  dpm_close_port_resp_msg_v01 qmi_rsp;
  dpm_close_port_req_msg_v01  *qmi_req_gps;
  dpm_close_port_resp_msg_v01  qmi_rsp_gps;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  qmi_req = QBI_MEM_MALLOC_CLEAR(sizeof(dpm_close_port_req_msg_v01));
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);

  QBI_MEMSET(&qmi_rsp, 0, sizeof(qmi_rsp));
  if (!qbi_os_linux_dpm_get_port_info(
        ctx, qmi_req->control_port_list[0].port_name,
        sizeof(qmi_req->control_port_list[0].port_name),
        &qmi_req->data_port_list[0], NULL))
  {
    QBI_LOG_E_0("Couldn't get port details!");
  }
  else
  {
    qmi_req->control_port_list_valid = TRUE;
    qmi_req->control_port_list_len = 1;
    qmi_req->data_port_list_valid = TRUE;
    qmi_req->data_port_list_len = 1;

    QBI_LOG_D_0("Sending QMI_DPM_CLOSE_PORT_REQ");
    success = qbi_os_linux_dpm_send_qmi_req(
      QMI_DPM_CLOSE_PORT_REQ_V01, qmi_req, sizeof(dpm_close_port_req_msg_v01),
      &qmi_rsp, sizeof(qmi_rsp));
    if (success && qmi_rsp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      QBI_LOG_E_1("Port mapper close failed with error %d", qmi_rsp.resp.error);
      success = FALSE;
    }
  }

  QBI_MEM_FREE(qmi_req);
  return success;
} /* qbi_os_linux_dpm_close() */

/*===========================================================================
  FUNCTION: qbi_os_linux_dpm_get_gps_port_info
===========================================================================*/
/*!
    @brief Gets control and data port interface IDs, etc. for QMI DPM

    @details

    @param ctx
    @param control_port_name
    @param control_port_name_len
    @param data_ep_id
    @param bam_info Optional (can be NULL)

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_os_linux_dpm_get_gps_port_info
(
  const qbi_ctx_s                *ctx,
  char                           *control_port_name,
  uint32                          control_port_name_len,
  data_ep_id_type_v01            *data_ep_id,
  hardware_accl_port_details_v01 *bam_info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(control_port_name);

  /* Currently hard-coding control port name. This has been stable across
     multiple chipsets, so no changes are expected. */
  QBI_STRLCPY(control_port_name, "DATA39_CNTL", control_port_name_len);

  return qbi_hc_dpm_get_data_port_info(ctx, data_ep_id, bam_info);
} /* qbi_os_linux_dpm_get_gps_port_info() */

/*===========================================================================
  FUNCTION: qbi_os_linux_dpm_get_port_info
===========================================================================*/
/*!
    @brief Gets control and data port interface IDs, etc. for QMI DPM

    @details

    @param ctx
    @param control_port_name
    @param control_port_name_len
    @param data_ep_id
    @param bam_info Optional (can be NULL)

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_os_linux_dpm_get_port_info
(
  const qbi_ctx_s                *ctx,
  char                           *control_port_name,
  uint32                          control_port_name_len,
  data_ep_id_type_v01            *data_ep_id,
  hardware_accl_port_details_v01 *bam_info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(control_port_name);

  /* Currently hard-coding control port name. This has been stable across
     multiple chipsets, so no changes are expected. */
  QBI_STRLCPY(control_port_name, QBI_OS_LINUX_CNTL_PORT_FOR_DPM,
    control_port_name_len);

  return qbi_hc_dpm_get_data_port_info(ctx, data_ep_id, bam_info);
} /* qbi_os_linux_dpm_get_port_info() */

/*===========================================================================
  FUNCTION: qbi_os_linux_dpm_open
===========================================================================*/
/*!
    @brief Opens QBI's control+data ports via data port mapper

    @details

    @param ctx

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_os_linux_dpm_open
(
  const qbi_ctx_s *ctx
)
{
  dpm_open_port_req_msg_v01 *qmi_req;
  dpm_open_port_resp_msg_v01 qmi_rsp;
  dpm_open_port_req_msg_v01 *qmi_req_gps;
  dpm_open_port_resp_msg_v01 qmi_rsp_gps;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  qmi_req = QBI_MEM_MALLOC_CLEAR(sizeof(dpm_open_port_req_msg_v01));
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);

  /* Response allocated on the stack since it's just the result code */
  QBI_MEMSET(&qmi_rsp, 0, sizeof(qmi_rsp));
  if (!qbi_os_linux_dpm_get_port_info(
        ctx, qmi_req->control_port_list[0].port_name,
        sizeof(qmi_req->control_port_list[0].port_name),
        &qmi_req->control_port_list[0].default_ep_id,
        &qmi_req->hardware_data_port_list[0]))
  {
    QBI_LOG_E_0("Couldn't get port details!");
  }
  else
  {
    qmi_req->control_port_list_valid = TRUE;
    qmi_req->control_port_list_len = 1;
    qmi_req->hardware_data_port_list_valid = TRUE;
    qmi_req->hardware_data_port_list_len = 1;

    QBI_LOG_D_0("Sending QMI_DPM_OPEN_PORT_REQ");
    success = qbi_os_linux_dpm_send_qmi_req(
      QMI_DPM_OPEN_PORT_REQ_V01, qmi_req, sizeof(dpm_open_port_req_msg_v01),
      &qmi_rsp, sizeof(qmi_rsp));
    if (success && qmi_rsp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      QBI_LOG_E_1("Port mapper open failed with error %d", qmi_rsp.resp.error);
      success = FALSE;
    }
  }

  QBI_MEM_FREE(qmi_req);
  return success;
} /* qbi_os_linux_dpm_open() */

/*===========================================================================
  FUNCTION: qbi_os_linux_dpm_send_qmi_req
===========================================================================*/
/*!
    @brief Sends a QMI DPM request (synchronous)

    @details

    @param msg_id
    @param req_data
    @param req_data_len
    @param rsp_data
    @param rsp_data_len

    @return boolean TRUE if the request was sent and a response was received
*/
/*=========================================================================*/
static boolean qbi_os_linux_dpm_send_qmi_req
(
  uint32 msg_id,
  void  *req_data,
  uint32 req_data_len,
  void  *rsp_data,
  uint32 rsp_data_len
)
{
  qmi_client_error_type err;
  qmi_client_os_params os_params;
  qmi_client_type handle;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(req_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp_data);

  QBI_LOG_D_0("Fetching DPM client ID");
  qbi_os_qcci_os_params_init(&os_params);
  err = qmi_client_init_instance(
    dpm_get_service_object_v01(), QMI_CLIENT_INSTANCE_ANY, NULL, NULL,
    &os_params, QBI_OS_LINUX_QCCI_TIMEOUT_MS, &handle);
  if (err != QMI_NO_ERR)
  {
    QBI_LOG_E_1("Error %d while trying to allocate a client for QMI_DPM", err);
  }
  else
  {
    QBI_LOG_D_1("Sending request (msg_id 0x%02x)", msg_id);
    err = qmi_client_send_msg_sync(
      handle, msg_id, req_data, req_data_len, rsp_data, rsp_data_len,
      QBI_OS_LINUX_QCCI_TIMEOUT_MS);
    if (err != QMI_NO_ERR)
    {
      QBI_LOG_E_1("Error %d while sending QMI_DPM request", err);
    }
    else
    {
      success = TRUE;
    }

    QBI_LOG_D_0("Releasing client ID");
    err = qmi_client_release(handle);
    if (err != QMI_NO_ERR)
    {
      QBI_LOG_E_1("Error %d while trying to release QMI_DPM client", err);
    }
    QBI_LOG_D_0("Done");
  }

  return success;
} /* qbi_os_linux_dpm_send_qmi_req() */

/*===========================================================================
  FUNCTION: qbi_os_linux_fptr_rand_init
===========================================================================*/
/*!
    @brief Initializes the random data used for encoding function pointers

    @details
*/
/*=========================================================================*/
static void qbi_os_linux_fptr_rand_init
(
  void
)
{
  int fd;
  int ret;
/*-------------------------------------------------------------------------*/
  while (!qbi_os_linux_fptr_rand)
  {
    fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1)
    {
      QBI_LOG_E_1("Error opening /dev/urandom: %d", errno);
      usleep(QBI_OS_LINUX_FPTR_RAND_FAILURE_DELAY_US);
    }
    else
    {
      ret = read(fd, &qbi_os_linux_fptr_rand, sizeof(qbi_os_linux_fptr_rand));
      if (ret != sizeof(qbi_os_linux_fptr_rand))
      {
        QBI_LOG_E_3("read() returned %d, expected %d (errno %d)",
                    ret, sizeof(qbi_os_linux_fptr_rand), errno);
        usleep(QBI_OS_LINUX_FPTR_RAND_FAILURE_DELAY_US);
      }
      close(fd);
    }
  }
} /* qbi_os_linux_fptr_rand_init() */

/*===========================================================================
  FUNCTION: qbi_os_linux_log_event_to_str
===========================================================================*/
/*!
    @brief Maps an event enum to informative string

    @details

    @param event

    @return const char* Pointer to string, or NULL if not recognized
*/
/*=========================================================================*/
static const char *qbi_os_linux_log_event_to_str
(
  qbi_os_log_event_e log_event
)
{
  const char *str = NULL;
/*-------------------------------------------------------------------------*/
  switch (log_event)
  {
#ifdef FEATURE_QBI_VERBOSE_KMSG_LOGGING
    /* These messages disabled by default since they are too verbose */
    case QBI_OS_LOG_EVENT_MBIM_OPEN:
      str = QBI_OS_LINUX_KMSG_PREFIX "MBIM_OPEN received\n";
      break;
    case QBI_OS_LOG_EVENT_ATTACHED:
      str = QBI_OS_LINUX_KMSG_PREFIX "Attached to network\n";
      break;
    case QBI_OS_LOG_EVENT_CONNECT_REQ:
      str = QBI_OS_LINUX_KMSG_PREFIX "Connect request received\n";
      break;
    case QBI_OS_LOG_EVENT_CONNECTED:
      str = QBI_OS_LINUX_KMSG_PREFIX "Connected\n";
      break;
    case QBI_OS_LOG_EVENT_IP_CFG_REQ:
      str = QBI_OS_LINUX_KMSG_PREFIX "IP config request received\n";
      break;
#endif /* FEATURE_QBI_VERBOSE_KMSG_LOGGING */

    case QBI_OS_LOG_EVENT_QBI_INIT:
      str = QBI_OS_LINUX_KMSG_PREFIX "Proc start\n";
      break;
    case QBI_OS_LOG_EVENT_MBIM_OPEN_DONE:
      str = QBI_OS_LINUX_KMSG_PREFIX "MBIM_OPEN_DONE sent\n";
      break;
    case QBI_OS_LOG_EVENT_IP_CFG_RESP:
      str = QBI_OS_LINUX_KMSG_PREFIX "IP config response sent\n";
      break;
    default:
      QBI_LOG_E_1("Unexpected log event %d", log_event);
  }

  return str;
} /* qbi_os_linux_log_event_to_str() */

/*===========================================================================
  FUNCTION: qbi_os_linux_timer_notify_cb
===========================================================================*/
/*!
    @brief Timer notification callback, invoked as if it were the entry point
    of a new thread (though a new, clean thread is not guaranteed)

    @details
    Invokes the callback function provided in qbi_os_timer_init()

    @param cb_data
*/
/*=========================================================================*/
static void qbi_os_linux_timer_notify_cb
(
  union sigval cb_data
)
{
  qbi_os_timer_t *timer;
  qbi_os_timer_cb_f *timer_cb = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(cb_data.sival_ptr);

  timer = (qbi_os_timer_t *) cb_data.sival_ptr;
  if (timer->magic != QBI_OS_LINUX_TIMER_MAGIC)
  {
    QBI_LOG_E_0("Received timer callback for invalid timer");
  }
  else if (!timer->active)
  {
    QBI_LOG_W_0("Received callback for inactive timer");
  }
  else if (!qbi_os_fptr_decode(
             &timer->cb_func, (qbi_os_void_f **) &timer_cb, FALSE))
  {
    QBI_LOG_E_0("Callback function for timer is invalid!");
  }
  else
  {
    timer->active = FALSE;
    timer_cb(timer->cb_data);
  }
} /* qbi_os_linux_timer_notify_cb() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_os_fptr_decode
===========================================================================*/
/*!
    @brief Decode a OS-specific encoded function pointer to get the original
    function pointer

    @details
    The qbi_os_encoded_fptr_t value must be either zero-initialized or
    previously passed to qbi_os_fptr_encode(). Other usage may be treated
    as memory corruption and cause QBI processing to terminate.

    @param encoded_fptr
    @param fptr
    @param allow_null Controls whether this function returns TRUE or FALSE
    on decoding a NULL pointer. If the calling code will never pass a NULL
    pointer to qbi_os_fptr_encode(), then set this to FALSE.

    @return boolean TRUE on success, FALSE on failure.
*/
/*=========================================================================*/
boolean qbi_os_fptr_decode
(
  const qbi_os_encoded_fptr_t *encoded_fptr,
  qbi_os_void_f              **fptr,
  boolean                      allow_null
)
{
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(encoded_fptr);
  QBI_CHECK_NULL_PTR_RET_FALSE(fptr);

  if ((encoded_fptr->magic ^ encoded_fptr->fptr) ==
        QBI_OS_LINUX_ENCODED_POINTER_MAGIC)
  {
    *fptr = (qbi_os_void_f *) (encoded_fptr->fptr ^ qbi_os_linux_fptr_rand);
    if (*fptr != NULL || allow_null)
    {
      result = TRUE;
    }
  }
  else if (!encoded_fptr->magic)
  {
    *fptr = NULL;
  }
  else
  {
    /* If magic is invalid, it must be set to 0, otherwise we assume that the
       memory was corrupted (possible buffer overflow), so other memory is
       likely invalid as well. Terminate processing now to allow for recovery. */
    QBI_LOG_E_0("Detected possible corruption!");
    exit(-3);
  }

  return result;
} /* qbi_os_fptr_decode() */

/*===========================================================================
  FUNCTION: qbi_os_fptr_encode
===========================================================================*/
/*!
    @brief Encodes a function pointer to protect against accidental
    corruption and directed malicious attack

    @details
    If a function pointer is rarely set, the recommended approach is to
    zero-initialize the qbi_os_encoded_fptr_t, allowing qbi_os_fptr_decode()
    to be safely used and avoiding the need to always encode a NULL pointer.

    @param encoded_fptr
    @param fptr Function pointer to encode. Can be NULL.
*/
/*=========================================================================*/
void qbi_os_fptr_encode
(
  qbi_os_encoded_fptr_t *encoded_fptr,
  qbi_os_void_f         *fptr
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(encoded_fptr);

  if (!qbi_os_linux_fptr_rand)
  {
    qbi_os_linux_fptr_rand_init();
  }

  /* Two XOR operations are performed: one against a random value established
     at runtime (making the resulting function pointer hard to predict for an
     attacker intending to replace it with a value of their choosing), then this
     result is also XORed against a known constant (preventing decode of unset
     values, and accidental corruption). */
  encoded_fptr->fptr = ((uintptr_t) fptr ^ qbi_os_linux_fptr_rand);
  encoded_fptr->magic =
    (encoded_fptr->fptr ^ QBI_OS_LINUX_ENCODED_POINTER_MAGIC);
} /* qbi_os_fptr_encode() */

/*===========================================================================
  FUNCTION: qbi_os_init
===========================================================================*/
/*!
    @brief Performs any OS layer specific initialization

    @details
    This function should perform all initialization required for OS-specific
    functionality to work. It is the first QBI initialization function
    executed.
*/
/*=========================================================================*/
void qbi_os_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
  /* Detect OS wakelock support */
  if (access(QBI_OS_LINUX_WAKELOCK_FILE, F_OK) == 0 &&
      access(QBI_OS_LINUX_WAKEUNLOCK_FILE, F_OK) == 0)
  {
    QBI_LOG_I_0("Wakelock support is enabled");
    qbi_os_linux_wakelock_support_enabled = TRUE;
  }
} /* qbi_os_init() */

/*===========================================================================
  FUNCTION: qbi_os_log_event
===========================================================================*/
/*!
    @brief Logs a key event occurrence outside of the usual logging facility
    (e.g. DIAG)

    @details
    Can be used for simplified measurement of key performance indicators.

    @param log_event
*/
/*=========================================================================*/
void qbi_os_log_event
(
  qbi_os_log_event_e log_event
)
{
  int fd;
  const char *msg;
/*-------------------------------------------------------------------------*/
#ifndef FEATURE_QBI_VERBOSE_KMSG_LOGGING
  if (log_event != QBI_OS_LOG_EVENT_QBI_INIT &&
      log_event != QBI_OS_LOG_EVENT_MBIM_OPEN_DONE &&
      log_event != QBI_OS_LOG_EVENT_IP_CFG_RESP)
  {
    return;
  }
#endif /* FEATURE_QBI_VERBOSE_KMSG_LOGGING */

  msg = qbi_os_linux_log_event_to_str(log_event);
  QBI_CHECK_NULL_PTR_RET(msg);

  /* On Linux we log a string to /dev/kmsg, so it shows up in dmesg with a
     kernel timestamp */
  fd = open("/dev/kmsg", O_WRONLY);
  if (fd < 0)
  {
    QBI_LOG_E_2("Couldn't open /dev/kmsg for event %d: %d", log_event, errno);
  }
  else
  {
    (void) write(fd, msg, QBI_STRLEN(msg));
    (void) close(fd);
    QBI_LOG_I_1("Logged event %d", log_event);
  }
} /* qbi_os_log_event() */

/*===========================================================================
  FUNCTION: qbi_os_mutex_init
===========================================================================*/
/*!
    @brief Initialize a mutex

    @details

    @param mutex

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_os_mutex_init
(
  qbi_os_mutex_t *mutex
)
{
  int pthread_err;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(mutex);
  pthread_err = pthread_mutex_init(mutex, NULL);
  if (pthread_err != 0)
  {
    QBI_LOG_E_1("Error %d while trying to initialize mutex", pthread_err);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_os_mutex_init() */

/*===========================================================================
  FUNCTION: qbi_os_mutex_lock
===========================================================================*/
/*!
    @brief Locks a mutex

    @details
    The mutex must have previously been intialized with qbi_os_mutex_init

    @param mutex

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_mutex_unlock
*/
/*=========================================================================*/
boolean qbi_os_mutex_lock
(
  qbi_os_mutex_t *mutex
)
{
  int pthread_err;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(mutex);
  pthread_err = pthread_mutex_lock(mutex);
  if (pthread_err != 0)
  {
    QBI_LOG_E_1("Error %d while trying to lock mutex", pthread_err);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_os_mutex_lock() */

/*===========================================================================
  FUNCTION: qbi_os_mutex_unlock
===========================================================================*/
/*!
    @brief Unlocks a mutex

    @details
    The mutex must be held by the calling thread via a previous call to
    qbi_os_mutex_lock.

    @param mutex

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_mutex_lock
*/
/*=========================================================================*/
boolean qbi_os_mutex_unlock
(
  qbi_os_mutex_t *mutex
)
{
  int pthread_err;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(mutex);
  pthread_err = pthread_mutex_unlock(mutex);
  if (pthread_err != 0)
  {
    QBI_LOG_E_1("Error %d while trying to unlock mutex", pthread_err);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_os_mutex_unlock() */

/*===========================================================================
  FUNCTION: qbi_os_qcci_os_params_init
===========================================================================*/
/*!
    @brief Initializes OS-specific parameters for use in QCCI synchronous
    APIs such as qmi_client_init()

    @details

    @param os_params
*/
/*=========================================================================*/
void qbi_os_qcci_os_params_init
(
  qmi_client_os_params *os_params
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(os_params);

  QBI_MEMSET(os_params, 0, sizeof(qmi_client_os_params));
} /* qbi_os_qcci_os_params_init() */

/*===========================================================================
  FUNCTION: qbi_os_qcci_get_service_instance
===========================================================================*/
/*!
    @brief Retrieves the appropriate service instance value to use with
    qmi_client_get_service_instance()

    @details

    @param ctx

    @return qmi_service_instance
*/
/*=========================================================================*/
qmi_service_instance qbi_os_qcci_get_service_instance
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ctx);

  return (qmi_service_instance) QMI_CLIENT_INSTANCE_ANY;
} /* qbi_os_qcci_get_service_instance() */

/*===========================================================================
  FUNCTION: qbi_os_qcci_setup
===========================================================================*/
/*!
    @brief Performs any OS/platform specific setup required before the QCCI
    APIs can be used

    @details
    To be called as part of MBIM_OPEN processing before any QCCI APIs are
    called.

    @param ctx

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_qcci_teardown
*/
/*=========================================================================*/
boolean qbi_os_qcci_setup
(
  const qbi_ctx_s *ctx
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  if (qbi_hc_dpm_is_used(ctx) && !qbi_os_linux_dpm_open(ctx))
  {
    QBI_LOG_E_0("Failed to open port");
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_os_qcci_setup() */

/*===========================================================================
  FUNCTION: qbi_os_qcci_teardown
===========================================================================*/
/*!
    @brief Performs OS/platform specific teardown and resource cleanup

    @details
    To be called as part of MBIM_CLOSE processing, at a point where QCCI APIs
    will not be called until after qbi_os_qcci_setup() is called again.

    @param ctx

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_qcci_setup
*/
/*=========================================================================*/
boolean qbi_os_qcci_teardown
(
  const qbi_ctx_s *ctx
)
{
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  /* In current implementation dpm port will not be closed.*/

  return success;
} /* qbi_os_qcci_teardown() */

/*===========================================================================
  FUNCTION: qbi_os_thread_clear_cmd_ready_sig
===========================================================================*/
/*!
    @brief Clears the flag that indicates whether a command is ready

    @details
    This should be called after qbi_os_thread_cmd_is_ready, and before
    command processing.

    @param thread_info
*/
/*=========================================================================*/
void qbi_os_thread_clear_cmd_ready_sig
(
  qbi_os_thread_info_t *thread_info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(thread_info);
  thread_info->cmd_is_ready = FALSE;
} /* qbi_os_thread_clear_cmd_ready_sig() */

/*===========================================================================
  FUNCTION: qbi_os_thread_cmd_is_ready
===========================================================================*/
/*!
    @brief Checks whether a command is waiting for processing

    @details
    This should only be called after successful return from
    qbi_os_thread_wait_cmd and before qbi_os_thread_clear_cmd_ready_sig.
    If this function returns true, qbi_os_thread_clear_cmd_ready_sig
    should be called before calling qbi_os_thread_wait_cmd again.

    @param thread_info

    @return boolean TRUE if a command is ready, FALSE otherwise

    @see qbi_os_thread_clear_cmd_ready_sig
*/
/*=========================================================================*/
boolean qbi_os_thread_cmd_is_ready
(
  qbi_os_thread_info_t *thread_info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(thread_info);
  return thread_info->cmd_is_ready;
} /* qbi_os_thread_cmd_is_ready() */

/*===========================================================================
  FUNCTION: qbi_os_thread_init
===========================================================================*/
/*!
    @brief Initializes the thread-specific information used to handle
    commands, etc.

    @details
    This must be called from the context of the QBI task.

    @param thread_info Struct containing OS-specific information about the
    new thread. This struct will be initialized in this function
*/
/*=========================================================================*/
void qbi_os_thread_init
(
  qbi_os_thread_info_t *thread_info
)
{
  int pthread_err;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(thread_info);

  QBI_MEMSET(thread_info, 0, sizeof(qbi_os_thread_info_t));
  pthread_err = pthread_mutex_init(&thread_info->cmd_mutex, NULL);
  if (pthread_err != 0)
  {
    QBI_LOG_E_1("Error %d while initializing mutex", pthread_err);
  }

  pthread_err = pthread_cond_init(&thread_info->cmd_cond, NULL);
  if (pthread_err != 0)
  {
    QBI_LOG_E_1("Error %d while initializing condition variable", pthread_err);
  }
} /* qbi_os_thread_init() */

/*===========================================================================
  FUNCTION: qbi_os_thread_notify_cmd
===========================================================================*/
/*!
    @brief Notify the given thread that a command is waiting

    @details

    @param thread_info

    @return boolean TRUE if notify succeeded, FALSE on error
*/
/*=========================================================================*/
boolean qbi_os_thread_notify_cmd
(
  qbi_os_thread_info_t *thread_info
)
{
  int pthread_err;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(thread_info);

  pthread_err = pthread_mutex_lock(&thread_info->cmd_mutex);
  if (pthread_err != 0)
  {
    QBI_LOG_E_1("Error %d trying to acquire lock for thread", pthread_err);
  }
  else
  {
    thread_info->cmd_is_waiting = TRUE;
    pthread_err = pthread_cond_signal(&thread_info->cmd_cond);
    if (pthread_err != 0)
    {
      QBI_LOG_E_1("Error %d trying to signal thread", pthread_err);
    }
    else
    {
      success = TRUE;
    }

    pthread_err = pthread_mutex_unlock(&thread_info->cmd_mutex);
    if (pthread_err != 0)
    {
      QBI_LOG_E_1("Error %d while unlocking mutex", pthread_err);
    }
  }

  return success;
} /* qbi_os_thread_notify_cmd() */

/*===========================================================================
  FUNCTION: qbi_os_thread_wait_cmd
===========================================================================*/
/*!
    @brief Waits for a command notification

    @details
    Waits for notification that a command is ready. Note that spurious
    wakeups from this function are possible, so qbi_os_thread_cmd_is_ready
    should be used to check whether a command is waiting.

    @param thread_info

    @return boolean TRUE on successful wait, FALSE if a fatal error
    occurred and the thread should exit

    @see qbi_os_thread_notify_cmd
*/
/*=========================================================================*/
boolean qbi_os_thread_wait_cmd
(
  qbi_os_thread_info_t *thread_info
)
{
  int pthread_err = 0;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(thread_info);

  pthread_err = pthread_mutex_lock(&thread_info->cmd_mutex);
  if (pthread_err != 0)
  {
    QBI_LOG_E_1("Error %d trying to acquire lock for thread", pthread_err);
  }
  else
  {
    while (!thread_info->cmd_is_waiting && pthread_err == 0)
    {
      pthread_err = pthread_cond_wait(&thread_info->cmd_cond,
                                      &thread_info->cmd_mutex);
    }
    thread_info->cmd_is_ready = thread_info->cmd_is_waiting;
    thread_info->cmd_is_waiting = FALSE;

    if (pthread_err != 0)
    {
      QBI_LOG_E_1("Error %d waiting on command signal condition for thread",
                  pthread_err);
    }
    else
    {
      success = TRUE;
    }

    pthread_err = pthread_mutex_unlock(&thread_info->cmd_mutex);
    if (pthread_err != 0)
    {
      QBI_LOG_E_1("Error %d while unlocking command mutex for thread",
                  pthread_err);
    }
  }

  return success;
} /* qbi_os_thread_wait_cmd() */

/*===========================================================================
  FUNCTION: qbi_os_time_diff
===========================================================================*/
/*!
    @brief Returns the difference between two time values retrieved from
    qbi_os_time_get_ms()

    @details
    Since QBI typically deals with time differences measured in seconds and
    minutes at the most, this implementation may assume that a large
    difference on the order of weeks is the result of rollover, and adjust
    its decision for whether time1 or time2 is more recent accordingly.

    @param time1
    @param time2

    @return qbi_os_time_ms_t Positive difference in milliseconds if time1
    is temporally greater than time2, otherwise negative difference
*/
/*=========================================================================*/
qbi_os_time_ms_t qbi_os_time_diff
(
  qbi_os_time_ms_t time1,
  qbi_os_time_ms_t time2
)
{
/*-------------------------------------------------------------------------*/
  /* Since we can work with 64 bit precision, we don't have to worry about
     rollover. The comment is meant for platforms limited to 32-bit time
     values. */
  return (time1 - time2);
} /* qbi_os_time_diff() */

/*===========================================================================
  FUNCTION: qbi_os_time_get_ms
===========================================================================*/
/*!
    @brief Retrieves a monotonic time reference in milliseconds

    @details
    Depending on the underlying OS, this value may roll over, so comparisons
    should be made with qbi_os_time_diff.

    @return qbi_os_time_t
*/
/*=========================================================================*/
qbi_os_time_ms_t qbi_os_time_get_ms
(
  void
)
{
  int ret;
  struct timespec ts;
  qbi_os_time_ms_t time_ms;
/*-------------------------------------------------------------------------*/
  ret = clock_gettime(CLOCK_MONOTONIC, &ts);
  if (ret != 0)
  {
    QBI_LOG_E_1("Couldn't get monotonic time: errno %d", errno);
    time_ms = 0;
  }
  else
  {
    time_ms = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
  }

  return time_ms;
} /* qbi_os_time_get_ms() */

/*===========================================================================
  FUNCTION: qbi_os_timer_clear
===========================================================================*/
/*!
    @brief Clears an active timer

    @details

    @param timer

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_os_timer_clear
(
  qbi_os_timer_t *timer
)
{
  int ret;
  struct itimerspec its;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(timer);

  if (timer->magic != QBI_OS_LINUX_TIMER_MAGIC)
  {
    QBI_LOG_E_0("Tried clearing uninitialized timer!");
  }
  else
  {
    if (timer->active)
    {
      /* Setting the timer with a zeroed-out timerspec will disarm it */
      QBI_MEMSET(&its, 0, sizeof(its));
      ret = timer_settime(timer->timerid, 0, &its, NULL);
      if (ret != 0)
      {
        QBI_LOG_E_1("Couldn't delete timer: errno %d", errno);
      }
      timer->active = FALSE;
    }
    result = TRUE;
  }

  return result;
} /* qbi_os_timer_clear() */

/*===========================================================================
  FUNCTION: qbi_os_timer_init
===========================================================================*/
/*!
    @brief Initializes the given timer data type to associate with the given
    callback function and user data.

    @details

    @param timer
    @param cb_func
    @param cb_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_os_timer_init
(
  qbi_os_timer_t    *timer,
  qbi_os_timer_cb_f *cb_func,
  void              *cb_data
)
{
  int ret;
  boolean success = FALSE;
  struct sigevent sigev;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(timer);
  QBI_CHECK_NULL_PTR_RET_FALSE(cb_func);

  if (timer->magic == QBI_OS_LINUX_TIMER_MAGIC)
  {
    QBI_LOG_W_0("Re-initializing timer");

    /* Delete & disarm the timer, since we will create a new one below */
    ret = timer_delete(timer->timerid);
    if (ret != 0)
    {
      QBI_LOG_E_1("Error deleting timer: errno %d", errno);
    }
  }

  QBI_MEMSET(timer, 0, sizeof(qbi_os_timer_t));
  qbi_os_fptr_encode(&timer->cb_func, (qbi_os_void_f *) cb_func);
  timer->cb_data = cb_data;

  /* Prepare the sigevent structure such that we will get a callback to
     qbi_os_linux_timer_notify_cb() with *timer as the argument when this timer
     expires. */
  QBI_MEMSET(&sigev, 0, sizeof(sigev));
  sigev.sigev_notify          = SIGEV_THREAD;
  sigev.sigev_notify_function = qbi_os_linux_timer_notify_cb;
  sigev.sigev_value.sival_ptr = timer;

  ret = timer_create(CLOCK_MONOTONIC, &sigev, &timer->timerid);
  if (ret != 0)
  {
    QBI_LOG_E_1("Error creating timer: errno %d", errno);
  }
  else
  {
    timer->magic = QBI_OS_LINUX_TIMER_MAGIC;
    success = TRUE;
  }

  return success;
} /* qbi_os_timer_init() */

/*===========================================================================
  FUNCTION: qbi_os_timer_set
===========================================================================*/
/*!
    @brief Sets the given timer to expire after delay milliseconds

    @details

    @param timer
    @param delay

    @return boolean TRUE if timer successfully set, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_os_timer_set
(
  qbi_os_timer_t  *timer,
  qbi_os_time_ms_t delay
)
{
  int ret;
  struct itimerspec its;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(timer);

  if (timer->magic != QBI_OS_LINUX_TIMER_MAGIC)
  {
    QBI_LOG_E_0("Tried to set uninitialized timer");
  }
  else
  {
    QBI_MEMSET(&its, 0, sizeof(its));

    its.it_value.tv_sec  = (delay / 1000);
    its.it_value.tv_nsec = ((delay % 1000) * 1000000);

    ret = timer_settime(timer->timerid, 0, &its, NULL);
    if (ret != 0)
    {
      QBI_LOG_E_1("Error creating timer: errno %d", errno);
    }
    else
    {
      timer->active = TRUE;
      result = TRUE;
    }
  }

  return result;
} /* qbi_os_timer_set() */

/*===========================================================================
  FUNCTION: qbi_os_timer_wake_lock
===========================================================================*/
/*!
    @brief Set qbi wake lock to keep OS awake

    @details

    @return boolean TRUE if wake lock successfully set, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_os_timer_wake_lock
(
  void
)
{
  int fd;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  if (qbi_os_linux_wakelock_support_enabled)
  {
    fd = open(QBI_OS_LINUX_WAKELOCK_FILE,
              (O_WRONLY | O_APPEND | O_NONBLOCK));
    if (fd < 0)
    {
      QBI_LOG_STR_2("Error opening %s: %d",
                    QBI_OS_LINUX_WAKELOCK_FILE, errno);
    }
    else
    {
      QBI_LOG_I_0("Set wake lock");
      (void) write(fd, QBI_OS_LINUX_WAKELOCK_NAME,
                   QBI_STRLEN(QBI_OS_LINUX_WAKELOCK_NAME));
      (void) close(fd);
      result = TRUE;
    }
  }

  return result;
} /* qbi_os_timer_wake_lock() */

/*===========================================================================
  FUNCTION: qbi_os_timer_wake_unlock
===========================================================================*/
/*!
    @brief Clear qbi wake lock

    @details

    @return boolean TRUE if wake unlock successfully set, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_os_timer_wake_unlock
(
  void
)
{
  int fd;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  if (qbi_os_linux_wakelock_support_enabled)
  {
    fd = open(QBI_OS_LINUX_WAKEUNLOCK_FILE,
              (O_WRONLY | O_APPEND | O_NONBLOCK));
    if (fd < 0)
    {
      QBI_LOG_STR_2("Error opening %s: %d",
                    QBI_OS_LINUX_WAKEUNLOCK_FILE, errno);
    }
    else
    {
      QBI_LOG_I_0("Set wake unlock");
      (void) write(fd, QBI_OS_LINUX_WAKELOCK_NAME,
                   QBI_STRLEN(QBI_OS_LINUX_WAKELOCK_NAME));
      (void) close(fd);
      result = TRUE;
    }
  }

  return result;
} /* qbi_os_timer_wake_unlock() */

