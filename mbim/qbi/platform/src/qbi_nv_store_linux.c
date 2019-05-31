/*!
  @file
  qbi_nv_store_linux.c

  @brief
  Non-volatile storage functionality implemented via C standard I/O library for
  a Linux filesystem.
*/

/*=============================================================================

  Copyright (c) 2012-2014, 2017-2018 Qualcomm Technologies, Inc.
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
06/26/18  mm   Removed persistent and added non persistent cache for DSSA
03/19/18  ha   Add support for mhi uevent and deprecate pcie_enabled NV
01/05/18  ha   Add NV for pcie_enabled
08/18/17  rv   Add NV for Single PDP item
07/27/17  rv   Add function to delete item
07/18/17  rv   Add item to maintain QBI version
06/28/17  rv   Add item for operator config
03/22/17  rv   Rename item For Provision Context
04/02/14  hz   Add item for SMS storage preference
06/06/13  bd   Add file read API for ERI support
01/22/13  bd   Add item for last firmware update session info
11/21/12  bd   Add runtime DeviceType configuration
06/01/12  bd   Rework REGISTER_STATE storage to only maintain net_sel_pref
04/12/12  bd   Changed directory to /var/lib/mbimd, fixed open flags
02/07/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_nv_store.h"

#include "qbi_common.h"
#include "qbi_svc_bc_mbim.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! Default value used for QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE when no runtime
    setting is present */
#define QBI_NV_STORE_LINUX_DEFAULT_DEVICE_TYPE QBI_SVC_BC_DEVICE_TYPE_REMOTE

/*! Path to directory containing all configuration item files
    @note We are technically violating the Unix filesystem hierarchy by putting
    this in /usr, but the system partition (mounted as /) gets formatted during
    recovery-based updates. But the userdata partition (mounted as /usr) is not
    formatted, so we use that partition for the purpose of persistence. */
#define QBI_NV_STORE_LINUX_BASE_PATH "/data/mbimd"

/*! Maximum length of an absolute path to a configuration item or file */
#define QBI_NV_STORE_LINUX_PATH_MAX (64)

/*! Filesystem permissions for new directories (0755) */
#define QBI_NV_STORE_LINUX_DIR_PERMS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

/*! Filesystem permissions for newly added files/configuration items (0644) */
#define QBI_NV_STORE_LINUX_FILE_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*! Return value from standard APIs indicating an error occurred */
#define QBI_NV_STORE_LINUX_ERROR (-1)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Variables

=============================================================================*/

static const char *qbi_nv_store_linux_cfg_item_filenames[QBI_NV_STORE_CFG_ITEM_END] = {
  "net_sel_pref",            /* QBI_NV_STORE_CFG_ITEM_NET_SEL_PREF */
  "device_type",             /* QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE */
  "fwupd_info",              /* QBI_NV_STORE_FWUPD_LAST_SESSION_INFO */
  "firmware_id",             /* QBI_NV_STORE_FIRMWARE_ID */
  "concat_plmn_spn",         /* QBI_NV_STORE_CFG_ITEM_CONCAT_PLMN_NAME_SPN */
  "wds_call_type",           /* QBI_NV_STORE_CFG_ITEM_WDS_CALL_TYPE */
  "limit_dev_cap",           /* QBI_NV_STORE_CFG_ITEM_LIMIT_DEV_CAP_TO_BAND_PREF */
  "sms_storage_pref",        /* QBI_NV_STORE_CFG_ITEM_SMS_STORAGE_PREF */
  "context_profile_data",    /* QBI_NV_STORE_CFG_ITEM_PROVISION_CONTEXT_PROFILE_DATA */
  "operator_config",         /* QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG */
  "qbi_version",             /* QBI_NV_STORE_CFG_ITEM_QBI_VERSION */
  "single_pdp_support",      /* QBI_NV_STORE_CFG_ITEM_SINGLE_PDP_SUPPORT */
  "single_wds",              /* QBI_NV_STORE_CFG_ITEM_SINGLE_WDS */
  "ssr_enable",              /* QBI_NV_STORE_CFG_ITEM_SSR_ENABLE */
};

static const char *qbi_nv_store_linux_file_id_filenames[QBI_NV_STORE_FILE_ID_END] = {
  "ERI.bin",        /* QBI_NV_STORE_FILE_ID_ERI_VZW */
  "ERI.txt",        /* QBI_NV_STORE_FILE_ID_ERI_SPR */
  "ERIInt.txt",     /* QBI_NV_STORE_FILE_ID_INTL_ROAM_SPR */
};

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_nv_store_linux_build_path
(
  const qbi_ctx_s *ctx,
  const char      *filename,
  char            *path,
  uint32           path_len
);

static boolean qbi_nv_store_linux_cfg_item_to_path
(
  const qbi_ctx_s        *ctx,
  qbi_nv_store_cfg_item_e item,
  char                   *path,
  uint32                  path_len
);

static boolean qbi_nv_store_linux_file_id_to_path
(
  const qbi_ctx_s        *ctx,
  qbi_nv_store_file_id_e  file_id,
  char                   *path,
  uint32                  path_len
);

static void qbi_nv_store_linux_mkdir_p
(
  const char *path_in
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_nv_store_linux_build_path
===========================================================================*/
/*!
    @brief Builds an absolute path to a file/configuration item by
    concatenating the base path, context ID, and a filename

    @details

    @param ctx
    @param filename
    @param path Buffer to populate with the absolute path. Size should be
    at least QBI_NV_STORE_LINUX_PATH_MAX. Guaranteed to be NULL terminated.
    @param path_len

    @return boolean TRUE on success, FALSE on NULL pointer
*/
/*=========================================================================*/
static boolean qbi_nv_store_linux_build_path
(
  const qbi_ctx_s *ctx,
  const char      *filename,
  char            *path,
  uint32           path_len
)
{
  int chars_written;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(filename);
  QBI_CHECK_NULL_PTR_RET_FALSE(path);

  chars_written = QBI_SNPRINTF(
    path, path_len, "%s/%d/%s", QBI_NV_STORE_LINUX_BASE_PATH, ctx->id, filename);
  if (chars_written >= path_len)
  {
    QBI_LOG_W_1("Item configuration path truncated! Needed %d bytes",
                (chars_written + 1));

    /* Ensure NULL termination and return success - we can still operate
       safely with a truncated filename */
    path[path_len - 1] = '\0';
  }

  return TRUE;
} /* qbi_nv_store_linux_build_path() */

/*===========================================================================
  FUNCTION: qbi_nv_store_linux_cfg_item_to_path
===========================================================================*/
/*!
    @brief Maps a configuration item enum to an absolute path to the
    associated file

    @details

    @param ctx
    @param item
    @param path
    @param path_len

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_nv_store_linux_cfg_item_to_path
(
  const qbi_ctx_s        *ctx,
  qbi_nv_store_cfg_item_e item,
  char                   *path,
  uint32                  path_len
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  if (item >= QBI_NV_STORE_CFG_ITEM_END ||
      qbi_nv_store_linux_cfg_item_filenames[item] == NULL)
  {
    QBI_LOG_E_1("Invalid configuration item %d", item);
  }
  else
  {
    success = qbi_nv_store_linux_build_path(
      ctx, qbi_nv_store_linux_cfg_item_filenames[item], path, path_len);
  }

  return success;
} /* qbi_nv_store_linux_cfg_item_to_path() */

/*===========================================================================
  FUNCTION: qbi_nv_store_linux_file_id_to_path
===========================================================================*/
/*!
    @brief Maps a file ID enum to its associated absolute path

    @details

    @param ctx
    @param item
    @param path
    @param path_len

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_nv_store_linux_file_id_to_path
(
  const qbi_ctx_s        *ctx,
  qbi_nv_store_file_id_e  file_id,
  char                   *path,
  uint32                  path_len
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  if (file_id >= QBI_NV_STORE_FILE_ID_END ||
      qbi_nv_store_linux_file_id_filenames[file_id] == NULL)
  {
    QBI_LOG_E_1("Invalid file ID %d", file_id);
  }
  else
  {
    success = qbi_nv_store_linux_build_path(
      ctx, qbi_nv_store_linux_file_id_filenames[file_id], path, path_len);
  }

  return success;
} /* qbi_nv_store_linux_file_id_to_path() */

/*===========================================================================
  FUNCTION: qbi_nv_store_linux_mkdir_p
===========================================================================*/
/*!
    @brief Creates a directory and needed parent directories

    @details
    If given a path to a file (i.e. no trailing slash), only creates parent
    directories for the file. For example, given "/path/to/file", this
    function will try to create "/path" and "/path/to"; given
    "/path/to/dir/", this function will try to create "/path", "/path/to",
    and "/path/to/dir".

    @param path_in
*/
/*=========================================================================*/
static void qbi_nv_store_linux_mkdir_p
(
  const char *path_in
)
{
  int ret;
  uint32 offset;
  uint32 len;
  char   path[QBI_NV_STORE_LINUX_PATH_MAX];
/*-------------------------------------------------------------------------*/
  len = MIN(QBI_STRLEN(path_in), (sizeof(path) - 1));
  path[0] = '/';

  /* Copy path_in into path, one byte at a time, creating each directory
     along the way */
  for (offset = 1; offset < len; offset++)
  {
    if (path_in[offset] == '/')
    {
      path[offset] = '\0';
      ret = mkdir(path, QBI_NV_STORE_LINUX_DIR_PERMS);
      if (ret == QBI_NV_STORE_LINUX_ERROR && errno != EEXIST)
      {
        QBI_LOG_STR_2("Error creating directory %s: %d", path, errno);
      }
      path[offset] = '/';
    }
    else
    {
      path[offset] = path_in[offset];
    }
  }
} /* qbi_nv_store_linux_mkdir_p() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_nv_store_cfg_item_read
===========================================================================*/
/*!
    @brief Read a configuration item from non-volatile storage

    @details

    @param ctx
    @param item
    @param data
    @param data_len

    @return boolean TRUE if entire configuration item read successfully,
    FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_nv_store_cfg_item_read
(
  qbi_ctx_s              *ctx,
  qbi_nv_store_cfg_item_e item,
  void                   *data,
  uint32                  data_len
)
{
  int fd;
  int ret;
  char path[QBI_NV_STORE_LINUX_PATH_MAX];
  struct stat statbuf;
  uint32 bytes_read = 0;
  boolean success = FALSE;
  uint8 *file_buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);

  if (!qbi_nv_store_linux_cfg_item_to_path(ctx, item, path, sizeof(path)))
  {
    QBI_LOG_E_1("Couldn't map configuration item %d to path!", item);
  }
  else
  {
    fd = open(path, O_RDONLY);
    if (fd == QBI_NV_STORE_LINUX_ERROR)
    {
      QBI_LOG_W_2("Couldn't open cfg item %d: %d", item, errno);
    }
    else
    {
      ret = fstat(fd, &statbuf);
      if (ret == QBI_NV_STORE_LINUX_ERROR)
      {
        QBI_LOG_E_2("Couldn't stat cfg item %d: %d", item, errno);
      }
      else if (statbuf.st_size != data_len)
      {
        QBI_LOG_E_3("Bad file size for cfg item %d: got %d, expected %d",
                    item, statbuf.st_size, data_len);
      }
      else
      {
        file_buf = (uint8 *) data;
        do
        {
          ret = read(fd, &file_buf[bytes_read], (data_len - bytes_read));
        } while (ret > 0 && (bytes_read += ret) < data_len);

        if (ret == QBI_NV_STORE_LINUX_ERROR)
        {
          QBI_LOG_E_3("Error reading from cfg item %d after %d bytes read: %d",
                      item, bytes_read, errno);
        }
        else
        {
          success = TRUE;
        }
      }

      if (close(fd) == QBI_NV_STORE_LINUX_ERROR)
      {
        QBI_LOG_E_2("Error closing cfg item %d after read: %d", item, errno);
      }
    }

    if (!success && item == QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE &&
        data_len == sizeof(uint32))
    {
      QBI_LOG_W_1("No runtime device type set, using default %d",
                  QBI_NV_STORE_LINUX_DEFAULT_DEVICE_TYPE);
      *((uint32 *) data) = QBI_NV_STORE_LINUX_DEFAULT_DEVICE_TYPE;
      success = TRUE;
    }

    if (!success && item == QBI_NV_STORE_CFG_ITEM_SSR_ENABLE &&
        data_len == sizeof(uint8))
    {
      QBI_LOG_D_0("ssr is enabled for linux platform");
      *((uint8 *) data) = TRUE;
      success = TRUE;
    }
  }

  return success;
} /* qbi_nv_store_cfg_item_read() */

/*===========================================================================
  FUNCTION: qbi_nv_store_cfg_item_write
===========================================================================*/
/*!
    @brief Write a configuration item to non-volatile storage

    @details

    @param ctx
    @param item
    @param data
    @param data_len

    @return boolean TRUE if entire configuration item written successfully,
    FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_nv_store_cfg_item_write
(
  qbi_ctx_s              *ctx,
  qbi_nv_store_cfg_item_e item,
  void                   *data,
  uint32                  data_len
)
{
  int fd;
  int ret;
  char path[QBI_NV_STORE_LINUX_PATH_MAX];
  const uint8 *file_buf;
  uint32 bytes_written = 0;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);

  if (!qbi_nv_store_linux_cfg_item_to_path(ctx, item, path, sizeof(path)))
  {
    QBI_LOG_E_1("Couldn't map configuration item %d to path!", item);
  }
  else
  {
    fd = creat(path, QBI_NV_STORE_LINUX_FILE_PERMS);
    if (fd == QBI_NV_STORE_LINUX_ERROR)
    {
      QBI_LOG_W_2("Couldn't create/open cfg item %d: %d. Building directory "
                  "tree and retrying", item, errno);
      qbi_nv_store_linux_mkdir_p(path);
      fd = creat(path, QBI_NV_STORE_LINUX_FILE_PERMS);
      if (fd == QBI_NV_STORE_LINUX_ERROR)
      {
        QBI_LOG_E_2("Error while creating cfg item %d: %d", item, errno);
      }
    }

    if (fd != QBI_NV_STORE_LINUX_ERROR)
    {
      file_buf = (const uint8 *) data;
      do
      {
        ret = write(fd, &file_buf[bytes_written], (data_len - bytes_written));
      } while (ret > 0 && (bytes_written += ret) < data_len);

      if (ret == QBI_NV_STORE_LINUX_ERROR)
      {
        QBI_LOG_E_3("Error writing to cfg item %d after %d bytes written: %d",
                    item, bytes_written, errno);
      }
      else
      {
        if (fsync(fd) == QBI_NV_STORE_LINUX_ERROR)
        {
          QBI_LOG_E_2("Error fsyncing cfg item %d after write: %d", item, errno);
        }
        if (close(fd) == QBI_NV_STORE_LINUX_ERROR)
        {
          QBI_LOG_E_2("Error closing cfg item %d after write: %d", item, errno);
        }
        else
        {
          success = TRUE;
        }
      }
    }
  }

  return success;
} /* qbi_nv_store_cfg_item_write() */

/*===========================================================================
  FUNCTION: qbi_nv_store_encrypted_pin1_save
===========================================================================*/
/*!
    @brief Saves an encrypted PIN1 value to a platform-dependent temporary
    storage area

    @details
    Intended to support silent PIN re-verification after modem subsystem
    restart. Implementation of this function is optional and may be a no-op
    for platforms where PIN re-verification is not supported/not applicable
    (e.g. MDMs).

    @param ctx
    @param enc_pin1_data
    @param enc_pin1_len
*/
/*=========================================================================*/
void qbi_nv_store_encrypted_pin1_save
(
  qbi_ctx_s   *ctx,
  const uint8 *enc_pin1_data,
  uint32       enc_pin1_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ctx);
  QBI_ARG_NOT_USED(enc_pin1_data);
  QBI_ARG_NOT_USED(enc_pin1_len);
} /* qbi_nv_store_encrypted_pin1_save() */

/*===========================================================================
  FUNCTION: qbi_nv_store_file_read
===========================================================================*/
/*!
    @brief Reads the complete contents of a file into a buffer

    @details

    @param ctx
    @param file_id
    @param buf Pointer to an initialized buffer to use for storing the file
    contents. If this function returns TRUE, the caller is responsible for
    freeing the memory associated with the buffer.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_nv_store_file_read
(
  qbi_ctx_s             *ctx,
  qbi_nv_store_file_id_e file_id,
  qbi_util_buf_s        *buf
)
{
  int fd;
  int ret;
  char path[QBI_NV_STORE_LINUX_PATH_MAX];
  struct stat statbuf;
  uint32 bytes_read = 0;
  boolean success = FALSE;
  uint8 *file_buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(buf);

  if (!qbi_nv_store_linux_file_id_to_path(ctx, file_id, path, sizeof(path)))
  {
    QBI_LOG_E_1("Couldn't map file ID %d to path!", file_id);
  }
  else
  {
    fd = open(path, O_RDONLY);
    if (fd == QBI_NV_STORE_LINUX_ERROR)
    {
      QBI_LOG_W_2("Couldn't open file ID %d: %d", file_id, errno);
    }
    else
    {
      ret = fstat(fd, &statbuf);
      if (ret == QBI_NV_STORE_LINUX_ERROR)
      {
        QBI_LOG_E_2("Couldn't stat file ID %d: %d", file_id, errno);
      }
      else if (qbi_util_buf_alloc_dont_clear(buf, statbuf.st_size) == NULL)
      {
        QBI_LOG_E_2("Couldn't allocate %d bytes for file ID %d",
                    statbuf.st_size, file_id);
      }
      else
      {
        file_buf = (uint8 *) buf->data;
        do
        {
          ret = read(fd, &file_buf[bytes_read], (buf->size - bytes_read));
        } while (ret > 0 && (bytes_read += ret) < buf->size);

        if (bytes_read != buf->size)
        {
          QBI_LOG_E_4("Error reading file ID %d after %d/%d bytes read: %d",
                      file_id, bytes_read, buf->size, errno);
          qbi_util_buf_free(buf);
        }
        else
        {
          success = TRUE;
        }
      }

      if (close(fd) == QBI_NV_STORE_LINUX_ERROR)
      {
        QBI_LOG_E_2("Error closing file ID %d after read: %d", file_id, errno);
      }
    }
  }

  return success;
} /* qbi_nv_store_file_read() */

/*===========================================================================
  FUNCTION: qbi_nv_store_cfg_item_delete
===========================================================================*/
/*!
    @brief Delete the configuration item

    @details

    @param ctx
    @param qbi_nv_store_cfg_item_e item
 
    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_nv_store_cfg_item_delete
(
  qbi_ctx_s             *ctx,
  qbi_nv_store_cfg_item_e item
)
{
  int fd;
  char path[QBI_NV_STORE_LINUX_PATH_MAX];
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  if (!qbi_nv_store_linux_cfg_item_to_path(ctx, item, path, sizeof(path)))
  {
    QBI_LOG_E_1("Couldn't map configuration item %d to path!", item);
  }
  else
  {
    fd = remove(path);
    if (fd == QBI_NV_STORE_LINUX_ERROR)
    {
       QBI_LOG_E_2("Unable to delete path %s errno %d !", path, errno);
       success = FALSE;
    }
    else
    {
       success = TRUE;
    }
  }

  return success;
}
