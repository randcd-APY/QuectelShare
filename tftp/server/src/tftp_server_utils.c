/***********************************************************************
 * tftp_server_utils.c
 *
 * Utils functions for the server.
 * Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Utils functions for the server.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header$ $DateTime$ $Author$

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-06-05   rp    Improve TFTP log stats.
2015-01-05   dks   Compiling server for TN Apps.
2014-07-18   rp    tftp and ipc-router integration changes from target.
2014-06-04   rp    Switch to IPCRouter sockets.
2014-01-20   dks   Fix compilation issues.
2013-12-17   nr    Create

===========================================================================*/

#include "tftp_server_config.h"
#include "tftp_server_utils.h"
#include "tftp_string.h"
#include "tftp_log.h"
#include "tftp_server_folders.h"
#include "tftp_os.h"

#include <stdio.h>

static char *tftp_server_bad_path_substrings[] =
{
  OS_BAD_PATH_STRING1, /*  "/../" or "\..\"  */
  OS_BAD_PATH_STRING2, /* "/.." or "\.." */
  OS_BAD_PATH_STRING3, /* INVALID_OS_PATH_SEPARATOR */
  0
};

static int32
tftp_server_utils_is_path_bad (char *filepath)
{
  char *string, **bad_substring;
  int is_path_bad = 0;

  bad_substring = tftp_server_bad_path_substrings;
  while (*bad_substring != NULL)
  {
    string = strstr (filepath, *bad_substring++);
    if (string != NULL)
    {
      is_path_bad = 1;
      break;
    }
  }
  return is_path_bad;
}


int32
tftp_server_utils_normalize_path (char *raw_filepath, uint32 instance_id,
                                   char *normalized_filepath_buf,
                                   uint32 normalized_filepath_buf_len)
{
  int32 result;
  char *tmp_path;
  const char *prefix_path;

  prefix_path = tftp_server_folders_lookup_path_prefix (instance_id);
  if (prefix_path == NULL)
  {
    TFTP_LOG_ERR ("Unknown file path instance : [%d]", instance_id);
    return -EPERM;
  }

  tmp_path = raw_filepath;
  while(*tmp_path)
  {
    if (*tmp_path == INVALID_OS_PATH_SEPARATOR)
    {
      *tmp_path = OS_PATH_SEPARATOR;
    }
    tmp_path++;
  }

  if (raw_filepath[0] == OS_PATH_SEPARATOR)
  {
    raw_filepath++;
  }

  result = snprintf (normalized_filepath_buf, normalized_filepath_buf_len,
                     "%s%c%s", prefix_path, OS_PATH_SEPARATOR, raw_filepath);
  if ((uint32) result >= normalized_filepath_buf_len)
  {
    return -ENAMETOOLONG;
  }

  result = tftp_server_utils_is_path_bad (normalized_filepath_buf);
  if (result != 0)
  {
    return -EPERM;
  }

  TFTP_LOG_INFO ("file [%s] : [%s]", raw_filepath, normalized_filepath_buf);

  return 0;
}

