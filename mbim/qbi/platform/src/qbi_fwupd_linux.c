

/*!
  @file
  qbi_fwupd_linux.c

  @brief
  Linux implementation of common firmware update API
*/

/*=============================================================================

  Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
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
08/27/14  ss   Code updates for 9x35 upgrade through QBI
01/22/13  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_fwupd.h"

#include "qbi_common.h"
#include "qbi_nv_store.h"
#include "qbi_util.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <unistd.h>

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! Full path to status file used to communicate with recovery mode */
#define QBI_FWUPD_LINUX_RECOVERY_STATUS_FILE "/cache/recovery/last_install"
/*! Message set in the recovery status file */
#define QBI_FWUPD_LINUX_RECOVERY_STATUS_SUCCESS "/data/update.zip\n1\n"

/*! Recovery command file path and it's contents */
#define QBI_FWUPD_LINUX_RECOVERY_CMD "--update_package=/data/update.zip\n"
#define QBI_FWUPD_LINUX_RECOVERY_CMD_FILE "/cache/recovery/command"

#define QBI_FWUPD_LINUX_RECOVERY_TARGET_PATH "/data/update.zip"

/*! Buffer size used when reading the recovery status file */
#define QBI_FWUPD_LINUX_RECOVERY_STATUS_LEN_MAX (40)

/*! Macro that calls qbi_fwupd_session_abort and also includes errno at the end
    of the error message string */
#define qbi_fwupd_session_abort_errno(handle, fmt, args...) \
  qbi_fwupd_session_abort(handle, fmt ": %s (%d)", ## args, \
                          strerror(errno), errno)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Platform-specific session handle. Typedef in qbi_fwupd_linux.h, opaque
    handle to outside modules. */
struct qbi_fwupd_session_handle_struct {
  qbi_ctx_s *ctx;

  /*! Status of the current update session */
  qbi_fwupd_session_status_e status;

  /*! File descriptor for update package */
  int fd;

  /*! Buffer containing error message string (ASCII, NULL-terminated) */
  qbi_util_buf_s error_msg_buf;
};

/*! Structure of QBI_NV_STORE_CFG_ITEM_FWUPD_LAST_SESSION_INFO */
typedef PACK(struct) {
  /*! Flag set to TRUE when an update session is rebooting into recovery mode to
      complete the update, and therefore the recovery status file needs to be
      checked to determine the result. */
  boolean pending_reboot;

  /*! Set to TRUE if the last session failed to complete */
  boolean update_failed;

  /*! String providing error information to help debug failed updates. ASCII,
      NULL-terminated. */
  char error_msg[QBI_FWUPD_ERROR_MSG_ASCII_MAX_LEN];
} qbi_fwupd_linux_last_session_info_s;

/*=============================================================================

  Private Variables

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_fwupd_recovery_status_file_read
(
  qbi_fwupd_linux_last_session_info_s *last_session_info
);

static boolean qbi_fwupd_recovery_cmd_file_update
(
  qbi_fwupd_session_handle_s *handle
);

static void qbi_fwupd_session_abort
(
  qbi_fwupd_session_handle_s *handle,
  const char                 *error_msg_fmt,
  ...
#ifdef __GNUC__
/* Enable compiler warnings for error_msg using a GCC attribute */
)  __attribute__ ((format (printf, 2, 3)));
#else
);
#endif

static void qbi_fwupd_session_status_set
(
  qbi_fwupd_session_handle_s *handle,
  qbi_fwupd_session_status_e  new_status
);

static void qbi_fwupd_linux_set_cpu_performance_mode
(
  void
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_fwupd_recovery_status_file_read
===========================================================================*/
/*!
    @brief Reads the recovery status file to determine the result of the
    update operation

    @details

    @param last_session_info On successful return of this function, will be
    populated with the session result. Will not be modified if this function
    returns FALSE.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_fwupd_recovery_status_file_read
(
  qbi_fwupd_linux_last_session_info_s *last_session_info
)
{
  boolean success = FALSE;
  int fd;
  ssize_t ret;
  uint32 total_bytes_read = 0;
  char status[QBI_FWUPD_LINUX_RECOVERY_STATUS_LEN_MAX];
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(last_session_info);

  QBI_LOG_I_0("Reading recovery status file");
  qbi_fwupd_mount_device_to_dir();
  fd = open(QBI_FWUPD_LINUX_RECOVERY_STATUS_FILE, O_RDONLY);
  if (fd == -1)
  {
    QBI_LOG_E_1("Couldn't open recovery status file for reading: %d", errno);
    qbi_fwupd_unmount_device_to_dir();
  }
  else
  {
    QBI_MEMSET(status, 0, sizeof(status));
    do
    {
      /* Subtract 1 from the buffer size to ensure we leave NULL termination */
      ret = read(fd, &status[total_bytes_read],
                 (sizeof(status) - 1 - total_bytes_read));
    } while (ret > 0 && (total_bytes_read += ret) < (sizeof(status) - 1));

    if (ret < 0)
    {
      QBI_LOG_E_1("Error reading recovery status file: %d", errno);
    }
    else
    {
      success = TRUE;
      QBI_MEMSET(
        last_session_info, 0, sizeof(qbi_fwupd_linux_last_session_info_s));
      if (QBI_STRNCMP(status, QBI_FWUPD_LINUX_RECOVERY_STATUS_SUCCESS,
                      sizeof(status)))
      {
        QBI_LOG_STR_1("Recovery stage of update session failed: '%s'", status);
        last_session_info->update_failed = TRUE;
        QBI_STRLCPY(last_session_info->error_msg, "Failed during recovery",
                    sizeof(last_session_info->error_msg));
      }
      else
      {
        QBI_LOG_I_0("Recovery stage of update session was successful");
        /* Leaving last_session_info set to 0 will result in update_failed set
           to FALSE and error_msg empty */
      }
    }

    qbi_fwupd_unmount_device_to_dir();
    (void) close(fd);
  }

  return success;
} /* qbi_fwupd_recovery_status_file_read() */

/*===========================================================================
  FUNCTION: qbi_fwupd_recovery_cmd_file_update
===========================================================================*/
/*!
    @brief Updates the recovery command file to inform the recovery system
    that we want to start a new update

    @details

    @param handle

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_fwupd_recovery_cmd_file_update
(
  qbi_fwupd_session_handle_s *handle
)
{
  boolean success = FALSE;
  int fd;
  ssize_t ret;
  uint32 total_bytes_written = 0;
  const char *status = QBI_FWUPD_LINUX_RECOVERY_CMD;
/*-------------------------------------------------------------------------*/
  qbi_fwupd_mount_device_to_dir();
  fd = open(QBI_FWUPD_LINUX_RECOVERY_CMD_FILE,
            (O_CREAT | O_TRUNC | O_WRONLY),
            (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
  if (fd == -1)
  {
    qbi_fwupd_session_abort_errno(handle, "Couldn't open recovery cmd file");
    qbi_fwupd_unmount_device_to_dir();
  }
  else
  {
    do
    {
      ret = write(fd, &status[total_bytes_written],
                  (sizeof(QBI_FWUPD_LINUX_RECOVERY_CMD) - 1 -
                     total_bytes_written));
    } while (ret > 0 && (total_bytes_written += ret) < sizeof(status));

    if (ret <= 0)
    {
      qbi_fwupd_session_abort_errno(handle, "Error writing to recovery cmd file");
    }

    if (close(fd) == -1)
    {
      qbi_fwupd_session_abort_errno(handle, "Error closing recovery cmd file");
    }

    if (qbi_fwupd_session_status_get(handle) != QBI_FWUPD_SESSION_STATUS_ERROR)
    {
      success = TRUE;
    }
  }

  qbi_fwupd_unmount_device_to_dir();
  return success;
} /* qbi_fwupd_recovery_cmd_file_update() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_abort
===========================================================================*/
/*!
    @brief Sets the session's status to error, and saves an error message to
    the handle

    @details
    The error message is not saved to permanent storage until the session is
    closed by qbi_fwupd_session_close().

    @param handle
    @param error_msg_fmt Error message to save to last session result when
    the session is closed. printf-style format string, with variable
    length argument list.
*/
/*=========================================================================*/
static void qbi_fwupd_session_abort
(
  qbi_fwupd_session_handle_s *handle,
  const char                 *error_msg_fmt,
  ...
)
{
  int ret;
  va_list args;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(handle);

  QBI_LOG_E_1("Update session failed at state %d",
              qbi_fwupd_session_status_get(handle));
  if (handle->error_msg_buf.size != 0)
  {
    /* If there is a situation where we encounter an error, then another error
       while attempting to clean up/recover, we only want to keep the first
       error message since it is likely a better indicator of the problem. */
    QBI_LOG_W_0("Session already has error message! Not overwriting");
  }
  else if (qbi_util_buf_alloc(&handle->error_msg_buf,
                              QBI_FWUPD_ERROR_MSG_ASCII_MAX_LEN) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate memory for error message");
  }
  else
  {
    va_start(args, error_msg_fmt);
    /* Pass vsnprintf 1 less than the actual buffer size, to ensure that our
       string is always NULL-terminated (last byte will be initialized to zero
       in qbi_util_buf_alloc()) */
    ret = vsnprintf((char *) handle->error_msg_buf.data,
                    (handle->error_msg_buf.size - 1), error_msg_fmt, args);
    va_end(args);

    if (ret >= (handle->error_msg_buf.size - 1))
    {
      QBI_LOG_W_1("Full error message of length %d characters was truncated",
                  ret);
    }
    else if (ret < 0)
    {
      QBI_LOG_W_0("Error encountered while expanding error message");
    }

    QBI_LOG_STR_1("Error message: '%s'", (char *) handle->error_msg_buf.data);
  }

  qbi_fwupd_session_status_set(handle, QBI_FWUPD_SESSION_STATUS_ERROR);
} /* qbi_fwupd_session_abort() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_status_set
===========================================================================*/
/*!
    @brief Updates a session's status field and logs the state transition

    @details

    @param handle
    @param new_status
*/
/*=========================================================================*/
static void qbi_fwupd_session_status_set
(
  qbi_fwupd_session_handle_s *handle,
  qbi_fwupd_session_status_e  new_status
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(handle);

  QBI_LOG_I_2("Update session status changed from %d to %d",
              handle->status, new_status);
  handle->status = new_status;
} /* qbi_fwupd_session_status_set() */

/*===========================================================================
  FUNCTION: qbi_fwupd_linux_set_cpu_performance_mode
===========================================================================*/
/*!
    @brief Changes the frequency scaling profile for the MDM applications
    processor to favor performance over power consumption

    @details
    Results in a tangible improvement to QDU throughput, reducing the overall
    update time. This setting only lasts from start of the QDU transfer until
    the device reboots into recovery mode.
*/
/*=========================================================================*/
static void qbi_fwupd_linux_set_cpu_performance_mode
(
  void
)
{
  int fd;
/*-------------------------------------------------------------------------*/
  fd = open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", O_WRONLY);
  if (fd == -1)
  {
    QBI_LOG_E_1("Error opening CPU scaling file: %d", errno);
  }
  else
  {
    if (write(fd, "performance", 12) != 12) {
      QBI_LOG_E_1("Error writing CPU scaling mode: %d", errno);
    }
    if (close(fd) == -1) {
      QBI_LOG_E_1("Error closing CPU scaling file: %d", errno);
    }
  }
} /* qbi_fwupd_linux_set_cpu_performance_mode() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_fwupd_mount_device_to_dir
===========================================================================*/
/*!
    @brief Mounts the device to a specific directory

    @details

    @return void
*/
/*=========================================================================*/
void qbi_fwupd_mount_device_to_dir
(
  void
)
{
  char command[50] = {0,};
  int ret = 0;
/*-------------------------------------------------------------------------*/
  snprintf(command,sizeof(command),
           "mount -t %s %s %s",QBI_FWUPD_LINUX_RECOVERY_FS_TYPE,
          QBI_FWUPD_LINUX_RECOVERY_FS_DEVICE,QBI_FWUPD_LINUX_RECOVERY_FS_MOUNT);
  QBI_LOG_STR_1("Sending system command -> %s",command);
  system(command);

  if (ret != -1)
  {
    QBI_LOG_STR_2("Mounted %s to %s",QBI_FWUPD_LINUX_RECOVERY_FS_DEVICE,
                  QBI_FWUPD_LINUX_RECOVERY_FS_MOUNT);
  }

  return;
}/* qbi_fwupd_mount_device_to_dir */

/*===========================================================================
  FUNCTION: qbi_fwupd_unmount_device_to_dir
===========================================================================*/
/*!
    @brief Unmounts the device to a specific directory

    @details

    @return void
*/
/*=========================================================================*/
void qbi_fwupd_unmount_device_to_dir
(
  void
)
{
  int ret = 0;
  char command[50] = {0,};
/*-------------------------------------------------------------------------*/
  snprintf(command,sizeof(command),
           "unmount %s",QBI_FWUPD_LINUX_RECOVERY_FS_MOUNT);
  QBI_LOG_STR_1("Sending system command -> %s",command);
  system(command);

  if (ret != -1)
  {
    QBI_LOG_STR_1("Unmounted %s",
                  QBI_FWUPD_LINUX_RECOVERY_FS_MOUNT);
  }

  return;
}/* qbi_fwupd_unmount_device_to_dir */

/*===========================================================================
  FUNCTION: qbi_fwupd_file_open
===========================================================================*/
/*!
    @brief Opens an update file for writing

    @details
    Prepares an abstract file for writing. Only one file may be open at a
    time.

    @param handle Session handle returned by qbi_fwupd_session_open
    @param file_id File identifier
    @param file_size Expected size of the file
    @param xfer_params Will be populated with parameters for the file
    transfer

    @return qbi_fwupd_file_result_e
*/
/*=========================================================================*/
qbi_fwupd_file_result_e qbi_fwupd_file_open
(
  qbi_fwupd_session_handle_s       *handle,
  qbi_fwupd_file_id_e               file_id,
  uint32                            file_size,
  qbi_fwupd_file_transfer_params_s *xfer_params
)
{
  int ret;
  struct statfs stat;
  qbi_fwupd_file_result_e result = QBI_FWUPD_FILE_RESULT_FAILURE;
/*-------------------------------------------------------------------------*/
  if (handle == NULL || xfer_params == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
  }
  else if (file_id != QBI_FWUPD_FILE_ID_LE_PACKAGE)
  {
    QBI_LOG_E_1("Tried to open unsupported file %d", file_id);
    result = QBI_FWUPD_FILE_RESULT_UNSUPPORTED_FILE;
  }
  else if (qbi_fwupd_session_status_get(handle) !=
             QBI_FWUPD_SESSION_STATUS_WAITING_FOR_FILE)
  {
    QBI_LOG_E_2("Invalid state %d to open file %d",
                qbi_fwupd_session_status_get(handle), file_id);
    result = QBI_FWUPD_FILE_RESULT_INVALID_STATE;
  }
  else
  {
    qbi_fwupd_mount_device_to_dir();
    ret = mkdir("/cache/recovery", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (ret == -1 && errno != EEXIST)
    {
      qbi_fwupd_session_abort_errno(handle, "Couldn't create directory");
      qbi_fwupd_unmount_device_to_dir();
    }
    else
    {
      /*
        In 9x40, /cache partition is ~57MB and too small for update.zip
        that includes MPSS files. Instead store the update.zip in /usr
        Note: /usr partition is /data in boot recovery mode.
      */
      handle->fd = open(QBI_FWUPD_LINUX_RECOVERY_TARGET_PATH,
                        (O_CREAT | O_TRUNC | O_WRONLY),
                        (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
      if (handle->fd == -1)
      {
        qbi_fwupd_session_abort_errno(
          handle, "Couldn't open/create update file");
      }
      else
      {
        ret = fstatfs(handle->fd, &stat);
        if (ret == -1)
        {
          qbi_fwupd_session_abort_errno(handle, "Couldn't get filesystem stats");
        }
        else if (file_size > (stat.f_bfree * stat.f_bsize))
        {
          qbi_fwupd_session_abort(
            handle, "Not enough space for file: have %lu, need %u",
            (stat.f_bfree * stat.f_bsize), file_size);
          result = QBI_FWUPD_FILE_RESULT_NO_SPACE;
        }
        else
        {
          qbi_fwupd_session_status_set(
            handle, QBI_FWUPD_SESSION_STATUS_WRITING_FILE);
          qbi_fwupd_linux_set_cpu_performance_mode();

          xfer_params->max_window_size = 4;
          /* The following value is set to allow the MBIM write msg
             length of the last fragment to align to USB maxpacket
             size of 512 and taking into account the MBIM header.
             This fixes a control path performance issue arising
             in the kernel/USB/dwc3/ep0 layer. */
          xfer_params->max_write_size  = 130896;

          result = QBI_FWUPD_FILE_RESULT_SUCCESS;
        }
      }
    }
  }

  qbi_fwupd_unmount_device_to_dir();
  return result;
} /* qbi_fwupd_file_open() */

/*===========================================================================
  FUNCTION: qbi_fwupd_file_write
===========================================================================*/
/*!
    @brief Writes data to the open file in the session

    @details

    @param handle
    @param data
    @param size Data size, in bytes

    @return qbi_fwupd_file_result_e
*/
/*=========================================================================*/
qbi_fwupd_file_result_e qbi_fwupd_file_write
(
  qbi_fwupd_session_handle_s *handle,
  const void                 *data,
  uint32                      size
)
{
  ssize_t ret;
  const uint8 *buf;
  uint32 total_bytes_written = 0;
  qbi_fwupd_file_result_e result = QBI_FWUPD_FILE_RESULT_FAILURE;
/*-------------------------------------------------------------------------*/
  if (handle == NULL || data == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
  }
  else if (qbi_fwupd_session_status_get(handle) !=
             QBI_FWUPD_SESSION_STATUS_WRITING_FILE)
  {
    QBI_LOG_E_1("Invalid state %d for file write",
                qbi_fwupd_session_status_get(handle));
    qbi_fwupd_session_abort(handle, "Received file write in invalid state");
    result = QBI_FWUPD_FILE_RESULT_INVALID_STATE;
  }
  else
  {
    buf = (const uint8 *) data;
    do
    {
      ret = write(handle->fd, &buf[total_bytes_written],
                  (size - total_bytes_written));
    } while (ret > 0 && (total_bytes_written += ret) < size);

    if (ret <= 0)
    {
      qbi_fwupd_session_abort_errno(handle, "Error writing to update file");
      result = QBI_FWUPD_FILE_RESULT_WRITE_ERROR;
    }
    else
    {
      result = QBI_FWUPD_FILE_RESULT_SUCCESS;
    }
  }

  return result;
} /* qbi_fwupd_file_write() */

/*===========================================================================
  FUNCTION: qbi_fwupd_file_close
===========================================================================*/
/*!
    @brief Closes a file, and ensures all data is committed to storage

    @details
    This operation may fail. Does not verify whether all expected data has
    been received. API users should call qbi_fwupd_session_status_get() to
    determine the next action to take.

    @param handle
*/
/*=========================================================================*/
void qbi_fwupd_file_close
(
  qbi_fwupd_session_handle_s *handle
)
{
  int ret;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(handle);

  if (qbi_fwupd_session_status_get(handle) !=
        QBI_FWUPD_SESSION_STATUS_WRITING_FILE)
  {
    QBI_LOG_E_1("Invalid state %d for file close operation",
                qbi_fwupd_session_status_get(handle));
  }
  else
  {
    ret = fsync(handle->fd);
    if (ret == -1)
    {
      qbi_fwupd_session_abort_errno(handle, "Couldn't flush data to storage");
    }

    ret = close(handle->fd);
    if (ret == -1)
    {
      qbi_fwupd_session_abort_errno(handle, "Couldn't close update file");
    }
    handle->fd = -1;

    /* We only need on file, so once we've received it, we are ready to reboot
       to apply the update. */
    if (qbi_fwupd_session_status_get(handle) != QBI_FWUPD_SESSION_STATUS_ERROR)
    {
      qbi_fwupd_session_status_set(
        handle, QBI_FWUPD_SESSION_STATUS_READY_TO_REBOOT);
    }
  }
} /* qbi_fwupd_file_close() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_close
===========================================================================*/
/*!
    @brief Ends an update session

    @details
    Used to abort or complete an update session. This function releases
    any resources allocated during the session, and stores the result of
    the session such that it can be accessed in later calls to
    qbi_fwupd_session_last_result_get().

    @param handle Pointer to handle for open session. The contained pointer
    will be set to NULL after this function returns.
    @param error_msg ASCII string describing the external error that occurred
    (if relevant). Optional - can be NULL. The caller of this function does
    not need to provide this parameter for an error message to be returned by
    qbi_fwupd_session_last_result_get(): errors occurring inside the firmware
    update layer will internally generate an error message string.
*/
/*=========================================================================*/
void qbi_fwupd_session_close
(
  qbi_fwupd_session_handle_s **handle,
  const char                  *error_msg
)
{
  qbi_fwupd_linux_last_session_info_s last_session_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(handle);
  QBI_CHECK_NULL_PTR_RET(*handle);

  QBI_MEMSET(&last_session_info, 0, sizeof(last_session_info));
  if (qbi_fwupd_session_status_get(*handle) ==
        QBI_FWUPD_SESSION_STATUS_WRITING_FILE)
  {
    QBI_LOG_W_0("Closing file due to early session termination");
    qbi_fwupd_file_close(*handle);
  }

  last_session_info.update_failed = TRUE;
  if ((*handle)->error_msg_buf.data != NULL)
  {
    QBI_STRLCPY(last_session_info.error_msg, (*handle)->error_msg_buf.data,
                sizeof(last_session_info.error_msg));
  }
  else if (error_msg != NULL)
  {
    QBI_STRLCPY(last_session_info.error_msg, error_msg,
                sizeof(last_session_info.error_msg));
  }

  if (!qbi_nv_store_cfg_item_write(
        (*handle)->ctx, QBI_NV_STORE_CFG_ITEM_FWUPD_LAST_SESSION_INFO,
        &last_session_info, sizeof(last_session_info)))
  {
    QBI_LOG_E_0("Couldn't save last session information!");
  }

  qbi_util_buf_free(&((*handle)->error_msg_buf));
  QBI_MEM_FREE(*handle);
  *handle = NULL;
} /* qbi_fwupd_session_close() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_continue
===========================================================================*/
/*!
    @brief Tells the firmware update layer to take action to advance the
    session state

    @details
    Used to continue the update session after reaching a state such as
    QBI_FWUPD_SESSION_STATUS_READY_TO_REBOOT.

    @param handle

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_fwupd_session_continue
(
  qbi_fwupd_session_handle_s *handle
)
{
  boolean result = FALSE;
  int ret;
  qbi_fwupd_linux_last_session_info_s last_session_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(handle);

  if (qbi_fwupd_session_status_get(handle) !=
        QBI_FWUPD_SESSION_STATUS_READY_TO_REBOOT)
  {
    QBI_LOG_E_1("Tried to continue session in invalid state %d",
                qbi_fwupd_session_status_get(handle));
  }
  else if (!qbi_fwupd_recovery_cmd_file_update(handle))
  {
    QBI_LOG_E_0("Couldn't update recovery status file");
  }
  else
  {
    /* Set the flag so we can properly update the last session result at next
       reboot. Note that we ignore errors here since that functionality is
       primarily for debugging and does not interfere with the update */
    QBI_MEMSET(&last_session_info, 0, sizeof(last_session_info));
    (void) qbi_nv_store_cfg_item_read(
      handle->ctx, QBI_NV_STORE_CFG_ITEM_FWUPD_LAST_SESSION_INFO,
      &last_session_info, sizeof(last_session_info));
    last_session_info.pending_reboot = TRUE;
    QBI_LOG_I_1("last session status: '%d", last_session_info.update_failed);

    (void) qbi_nv_store_cfg_item_write(
      handle->ctx, QBI_NV_STORE_CFG_ITEM_FWUPD_LAST_SESSION_INFO,
      &last_session_info, sizeof(last_session_info));

    ret = system("sync;sync;sync;sys_reboot recovery");
    if (ret == -1)
    {
      qbi_fwupd_session_abort_errno(handle, "Couldn't reboot into recovery");
    }
    else
    {
      qbi_fwupd_session_status_set(handle, QBI_FWUPD_SESSION_STATUS_REBOOTING);
      result = TRUE;
    }
  }

  return result;
} /* qbi_fwupd_session_continue() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_last_result_get
===========================================================================*/
/*!
    @brief Retrieves information about the result of the last update session

    @details

    @param ctx
    @param handle Current session handle (if any, can be NULL). Used to
    help differentiate the current from last session.
    @param last_result Will be populated with result information if this
    function returns TRUE

    @return boolean TRUE on success, FALSE on failure. A return value of
    FALSE may simply indicate that no information is available about the
    last session (e.g. an update has not been performed).
*/
/*=========================================================================*/
boolean qbi_fwupd_session_last_result_get
(
  qbi_ctx_s                        *ctx,
  const qbi_fwupd_session_handle_s *handle,
  qbi_fwupd_session_last_result_s  *last_result
)
{
  boolean result_available = FALSE;
  qbi_fwupd_linux_last_session_info_s last_session_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(last_result);

  if (!qbi_nv_store_cfg_item_read(
        ctx, QBI_NV_STORE_CFG_ITEM_FWUPD_LAST_SESSION_INFO,
        &last_session_info, sizeof(last_session_info)))
  {
    QBI_LOG_W_0("Couldn't read last session info from nv");
  }
  else if (last_session_info.pending_reboot &&
           qbi_fwupd_session_status_get(handle) !=
             QBI_FWUPD_SESSION_STATUS_REBOOTING)
  {
    if (!qbi_fwupd_recovery_status_file_read(&last_session_info))
    {
      QBI_LOG_E_0("Couldn't read recovery status file");
    }
    else
    {
      /* Save the result to our NV so we don't need to rely on the recovery
         status file */
      (void) qbi_nv_store_cfg_item_write(
        ctx, QBI_NV_STORE_CFG_ITEM_FWUPD_LAST_SESSION_INFO, &last_session_info,
        sizeof(last_session_info));
      result_available = TRUE;
    }
  }
  else
  {
    result_available = TRUE;
  }

  if (result_available)
  {
    last_result->session_type = QBI_FWUPD_SESSION_TYPE_LE;
    last_result->failed = last_session_info.update_failed;
    QBI_STRLCPY(last_result->error_msg, last_session_info.error_msg,
                sizeof(last_result->error_msg));
  }

  return result_available;
} /* qbi_fwupd_session_last_result_get() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_open
===========================================================================*/
/*!
    @brief Initializes a new update session

    @details
    Allocates resources and prepares for a new firmware update session. The
    handle returned by this function must be passed to
    qbi_fwupd_session_close when the session is complete to ensure resources
    are cleaned up.

    @param ctx
    @param session_type
    @param handle Will point to a newly allocated handle upon successful
    return. Any previous session must be closed via qbi_fwupd_session_close
    before passing the handle back to this function. If no previous session
    existed, the handle must be initialized to NULL before passing to this
    function.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_fwupd_session_open
(
  qbi_ctx_s                   *ctx,
  qbi_fwupd_session_type_e     session_type,
  qbi_fwupd_session_handle_s **handle
)
{
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(handle);

  if (*handle != NULL)
  {
    QBI_LOG_E_0("Non-NULL update session handle passed to open");
  }
  else if (session_type != QBI_FWUPD_SESSION_TYPE_LE)
  {
    QBI_LOG_E_1("Tried to open unsupported session type %d", session_type);
  }
  else
  {
    *handle = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_fwupd_session_handle_s));
    QBI_CHECK_NULL_PTR_RET_FALSE(*handle);

    (*handle)->ctx = ctx;
    (*handle)->status = QBI_FWUPD_SESSION_STATUS_WAITING_FOR_FILE;
    (*handle)->fd = -1;
    qbi_util_buf_init(&((*handle)->error_msg_buf));
    result = TRUE;
  }

  return result;
} /* qbi_fwupd_session_open() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_status_get
===========================================================================*/
/*!
    @brief Retrieves the current status of an active session

    @details

    @param handle Session handle; may be NULL in which case the response will
    always be QBI_FWUPD_SESSION_STATUS_NONE

    @return qbi_fwupd_session_status_e
*/
/*=========================================================================*/
qbi_fwupd_session_status_e qbi_fwupd_session_status_get
(
  const qbi_fwupd_session_handle_s *handle
)
{
  qbi_fwupd_session_status_e status;
/*-------------------------------------------------------------------------*/
  if (handle == NULL)
  {
    status = QBI_FWUPD_SESSION_STATUS_NONE;
  }
  else
  {
    status = handle->status;
  }

  return status;
} /* qbi_fwupd_session_status_get() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_type_get
===========================================================================*/
/*!
    @brief Retrieves the session type of an active session

    @details

    @param handle Session handle; may be NULL in which case the response will
    always be QBI_FWUPD_SESSION_TYPE_NONE

    @return qbi_fwupd_session_type_e
*/
/*=========================================================================*/
qbi_fwupd_session_type_e qbi_fwupd_session_type_get
(
  const qbi_fwupd_session_handle_s *handle
)
{
  qbi_fwupd_session_type_e session_type;
/*-------------------------------------------------------------------------*/
  if (handle == NULL)
  {
    session_type = QBI_FWUPD_SESSION_TYPE_NONE;
  }
  else
  {
    session_type = QBI_FWUPD_SESSION_TYPE_LE;
  }

  return session_type;
} /* qbi_fwupd_session_type_get() */

/*===========================================================================
  FUNCTION: qbi_fwupd_session_type_supported
===========================================================================*/
/*!
    @brief Checks whether a given session type is supported by the platform
    implementation

    @details

    @param session_type

    @return boolean TRUE if supported, FALSE if not
*/
/*=========================================================================*/
boolean qbi_fwupd_session_type_supported
(
  qbi_fwupd_session_type_e session_type
)
{
/*-------------------------------------------------------------------------*/
  return (session_type == QBI_FWUPD_SESSION_TYPE_LE);
} /* qbi_fwupd_session_type_supported() */

