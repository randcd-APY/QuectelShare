/*!
  @file
  qbi_fwupd.h

  @brief
  Firmware update procedure platform abstraction layer
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/22/13  bd   Added module
=============================================================================*/

#ifndef QBI_FWUPD_H
#define QBI_FWUPD_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! Maximum length of an expanded session error message, in bytes. Includes NULL
    termination. */
#define QBI_FWUPD_ERROR_MSG_ASCII_MAX_LEN (256)

/*! Type of file system of QBI_FWUPD_LINUX_RECOVERY_FS_DEVICE */
#define QBI_FWUPD_LINUX_RECOVERY_FS_TYPE "ubifs"

/*! The file system to be mounted */
#define QBI_FWUPD_LINUX_RECOVERY_FS_DEVICE "/dev/ubi0_2"

/*! The file system on which mounting is being done */
#define QBI_FWUPD_LINUX_RECOVERY_FS_MOUNT "/cache"

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Update file identifiers */
typedef enum {
  QBI_FWUPD_FILE_ID_INVALID = -1,
    /*!< Invalid file ID */

  QBI_FWUPD_FILE_ID_LE_PACKAGE = 0,
    /*!< Linux Enablement (LE) update package. Only valid for LE session type.
         Update file consists of compressed update package. */
} qbi_fwupd_file_id_e;

/*! Provides the result of a file operation */
typedef enum {
  QBI_FWUPD_FILE_RESULT_SUCCESS = 0,
    /*!< Operation succeeded */

  QBI_FWUPD_FILE_RESULT_FAILURE = 1,
    /*!< General failure */

  QBI_FWUPD_FILE_RESULT_INVALID_STATE = 2,
    /*!< Operation not allowed in the current session state */

  QBI_FWUPD_FILE_RESULT_UNSUPPORTED_FILE = 3,
    /*!< File type not supported in this session */

  QBI_FWUPD_FILE_RESULT_NO_SPACE = 4,
    /*!< Not enough storage space for the requested operation */

  QBI_FWUPD_FILE_RESULT_WRITE_ERROR = 5,
    /*!< Error received from filesytem while attempting to write */
} qbi_fwupd_file_result_e;

/*! Parameters for file transfer, returned in qbi_fwupd_file_open() */
typedef struct {
  /*! The maximum number of bytes that should be sent in each call to
      qbi_fwupd_file_write(). The intent of this parameter is to tune for
      performance, rather than indicate capability. */
  uint32 max_write_size;

  /*! Maximum number of write transactions that should be outstanding at a time.
      Although qbi_fwupd_file_write() is synchronous, the transfer of file data
      is expected to occur in another thread of execution, */
  uint32 max_window_size;
} qbi_fwupd_file_transfer_params_s;

/*! Status of an update session. Flow through the states dependends on the
    implementation. */
typedef enum {
  QBI_FWUPD_SESSION_STATUS_NONE = 0,
    /*!< Session is not active */

  QBI_FWUPD_SESSION_STATUS_ERROR = 1,
    /*!< Session has encountered an unrecoverable error; qbi_fwupd_session_close
       must be called to clean up resources. */

  QBI_FWUPD_SESSION_STATUS_WAITING_FOR_FILE = 2,
    /*!< Waiting to receive the next file; no file currently open */

  QBI_FWUPD_SESSION_STATUS_WRITING_FILE = 3,
    /*!< A file is open, and we are writing data to it */

  QBI_FWUPD_SESSION_STATUS_READY_TO_REBOOT = 4,
    /*!< All update files received, and the next step requires a device reboot.
         qbi_fwupd_session_continue must be called to proceed. */

  QBI_FWUPD_SESSION_STATUS_REBOOTING = 5,
    /*!< A reboot command has been issued to the system */
} qbi_fwupd_session_status_e;

/*! Update session types. A platform implmentation is not required to implement
    all available session types. */
typedef enum {
  QBI_FWUPD_SESSION_TYPE_NONE = 0,
    /*! No session type available */

  QBI_FWUPD_SESSION_TYPE_LE = 1,
    /*!< Linux Enablement (LE) update session; based on Android FOTA */
} qbi_fwupd_session_type_e;

/*! Structure used to communicate information about the last update session. */
typedef struct {
  qbi_fwupd_session_type_e session_type;
  boolean                  failed;
  char                     error_msg[QBI_FWUPD_ERROR_MSG_ASCII_MAX_LEN];
} qbi_fwupd_session_last_result_s;

/*! Opaque update session handle structure. The contents of the structure are
    private to the implementation, therefore outside modules must only maintain
    a pointer to this type. */
typedef struct qbi_fwupd_session_handle_struct qbi_fwupd_session_handle_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
  FUNCTION: qbi_fwupd_mount_device_to_dir
===========================================================================*/
/*!
    @brief Mounts the device to a specific directory

    @details

    @param type
    @param device to mount
    @param directory to mount on

    @return void
*/
/*=========================================================================*/
void qbi_fwupd_mount_device_to_dir
(
  void
);

/*===========================================================================
  FUNCTION: qbi_fwupd_unmount_device_to_dir
===========================================================================*/
/*!
    @brief Unmounts the device from a specific directory

    @details

    @param directory to unmount

    @return void
*/
/*=========================================================================*/
void qbi_fwupd_unmount_device_to_dir
(
  void
);
#endif /* QBI_FWUPD_H */

