/***********************************************************************
 * tftp_os_la.c
 *
 * Short description.
 * Copyright (c) 2014,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header$ $DateTime$ $Author$

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-06-05   rp    Change uid/gid for RAM-dump files.
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_comdef.h"
#include "tftp_file.h"
#include "tftp_server_config.h"
#include "tftp_os.h"
#include "tftp_log.h"


#if !defined (TFTP_LA_BUILD)
  #error "This file is to be included only in LA builds."
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fts.h>
#include <string.h>

void
tftp_os_init (void)
{
  umask (TFTP_SERVER_UMASK);
}

static int32
tftp_os_open_file (const char *path, int32 oflags, uint16 mode,
                     int32 uid, int32 gid)
{
  int32 result, fd, result2, result3;

  TFTP_LOG_INFO ("open : [%d] [%d] [%d] [%d] [%s]",
                 uid, gid, mode, oflags, path);

  /* This mode when used is modified by umask */
  result = open (path, oflags, mode);
  if (result < 0)
  {
    result = -(errno);
    TFTP_LOG_ERR ("open failed: [%d] [%s]", errno, strerror(errno));
  }
  else
  {
    if (uid != -1 || gid != -1)
    {
      fd = result;
      result2 = fchown(fd, uid, gid);
      if (result2 != 0)
      {
        TFTP_LOG_INFO ("fchown failed: [%d] [%s] [%s]",
          errno, path, strerror(errno));

        result3 = fchown(fd, -1, gid);
        if (result3 != 0)
        {
          TFTP_LOG_INFO ("fchown gid failed: [%d] [%s] [%s]",
            errno, path, strerror(errno));
        }
      }
    }
  }

  return result;
}

int32
tftp_os_file_open_helper(const char *path, int32 oflags, uint16 mode_bits,
                           int32 uid, int32 gid, tftp_file_handle *handle,
                           const char *stream_mode)
{
  int32 open_res, fd;

  open_res = tftp_os_open_file (path, oflags, mode_bits, uid, gid);
  if (open_res < 0)
  {
    return open_res;
  }

  /* If we are here then the file was successfully opened. Now get a file
     stream from the fd  */
  fd = open_res;
  open_res = tftp_file_open_stream (handle, fd, stream_mode);
  return open_res;
}

int32
tftp_os_mkdir (const char *path, uint16 mode, int32 uid, int32 gid)
{
  int32 result;

  result = mkdir (path, mode); /* This mode when used is modified by umask */
  if (result != 0 && errno != EEXIST)
  {
    result = -(errno);
    TFTP_LOG_INFO ("mkdir failed: [%d] [%s] [%s]",
      errno, path, strerror(errno));
    return result;
  }
  else
  {
    (void) tftp_os_chown (path, uid, gid);
  }
  return 0;
}

int32
tftp_os_chown (const char *path, int32 uid, int32 gid)
{
  int32 result = 0;

  if (uid != -1 || gid != -1)
  {
    result = chown(path, uid, gid);
    if (result != 0)
    {
      result = -(errno);
      TFTP_LOG_DBG ("chown failed: [%d] [%s] [%s]",
        errno, path, strerror(errno));

      //todo : fix this
      result = chown(path, -1, gid);
      if (result != 0)
      {
        result = -(errno);
        TFTP_LOG_ERR ("chown gid failed: [%d] [%d] [%d] [%s] [%s]",
         uid, gid, errno, path, strerror(errno));
      }
    }
  }

  return result;
}

int32
tftp_os_symlink (const char *symlink_path, const char *target_path,
                 int is_dir)
{
  int32 result;

  (void) is_dir;

  result = symlink (target_path, symlink_path);
  if (result != 0)
  {
    result = -(errno);
    TFTP_LOG_INFO ("symlink failed: [%d] [%s] [%s]",
      errno, symlink_path, strerror(errno));
  }
  return result;
}

int32 tftp_os_unlink (const char *path)
{
  int32 result;

  result = unlink (path);
  if (result != 0 && errno != ENOENT)
  {
    result = -(errno);
    TFTP_LOG_ERR ("unlink failed: [%d] [%s] [%s]",
      errno, path, strerror(errno));
  }
  return result;
}

int32
tftp_os_datasync_for_fd (int32 fd)
{
  int32 result = 0;

  result = fdatasync(fd);
  if (result != 0)
  {
    result = -(errno);
  }
  return result;
}

int32
tftp_os_datasync (const char *path)
{
  int32 result = 0;
  int fd;

  fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    return -(errno);
  }

  result = fdatasync(fd);
  if (result != 0)
  {
    result = -(errno);
  }

  close(fd);
  return result;
}

int32
tftp_os_rename (const char *oldpath, const char *newpath)
{
  int32 result = 0;

  result = tftp_os_datasync (oldpath);
  if (result != 0)
  {
    return result;
  }

  result = rename(oldpath, newpath);
  if (result != 0)
  {
    result = -(errno);
  }
  return result;
}

int32
tftp_os_lchown_recursive (char *path, int32 uid, int32 gid)
{
  char *paths[2] = {path, NULL};
  FTSENT *ftsent;
  FTS *fts;
  int result = 0;

  fts = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR | FTS_XDEV, NULL);
  if (!fts)
  {
     result = -errno;
     TFTP_LOG_ERR ("fts_open failed for path %s with error %d (%s)\n", path,
                    -errno, strerror(errno));
     return result;
  }

  for (ftsent = fts_read(fts); ftsent != NULL; ftsent = fts_read(fts))
  {
    result = lchown(ftsent->fts_accpath, uid, gid);
    if (result)
    {
      result = -errno;
      TFTP_LOG_ERR ("lchown failed for path %s with error %d (%s)\n",
                    ftsent->fts_accpath, -errno, strerror(errno));
    }
  }
  fts_close(fts);
  return result;
}
