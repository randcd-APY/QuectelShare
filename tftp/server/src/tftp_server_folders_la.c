/***********************************************************************
 * tftp_server_folders_la.c
 *
 * Short description.
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
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
2017-05-31   rp    Port to SLPI image.
2015-07-30   mj    Use tftp_strlen instead of strlen.
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-06-30   nr    Support connected sockets and muti-poll.
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_server_config.h"
#include "tftp_server_folders.h"
#include "tftp_assert.h"
#include "tftp_file.h"
#include "tftp_os.h"
#include "tftp_log.h"
#include "tftp_string.h"

#if !defined (TFTP_LA_BUILD)
  #error "This file should only be compiled for LA builds."
#endif

#if defined (TFTP_SIMULATOR_BUILD)
  #define PATH_PREFIX  SIM_OUTPUT_DIR"/system"
#elif defined (TFTP_LE_BUILD_ONLY)
  #define PATH_PREFIX  "/system"
#else
  #define PATH_PREFIX  "/vendor"
#endif

#if defined (TFTP_SIMULATOR_BUILD)
  #define DATA_PATH_PREFIX  SIM_OUTPUT_DIR
#elif defined (TFTP_LE_BUILD_ONLY)
  #define DATA_PATH_PREFIX  "/data"
#else
  #define DATA_PATH_PREFIX  ""
#endif

struct tftp_server_folders_path_prefix_map_type
{
  uint32 instance_id;
  const char *path_prefix;
};

static struct tftp_server_folders_path_prefix_map_type
  tftp_server_folders_path_prefix_map[] =
{
  {TFTP_SERVER_INSTANCE_ID_MSM_MPSS,    PATH_PREFIX"/rfs/msm/mpss"},
  {TFTP_SERVER_INSTANCE_ID_MSM_ADSP,    PATH_PREFIX"/rfs/msm/adsp"},
  {TFTP_SERVER_INSTANCE_ID_MDM_MPSS,    PATH_PREFIX"/rfs/mdm/mpss"},
  {TFTP_SERVER_INSTANCE_ID_MDM_ADSP,    PATH_PREFIX"/rfs/mdm/adsp"},
  {TFTP_SERVER_INSTANCE_ID_MDM_TN,      PATH_PREFIX"/rfs/mdm/tn"},
  {TFTP_SERVER_INSTANCE_ID_APQ_GSS,     PATH_PREFIX"/rfs/apq/gnss"},
  {TFTP_SERVER_INSTANCE_ID_MSM_SLPI,    PATH_PREFIX"/rfs/msm/slpi"},
  {TFTP_SERVER_INSTANCE_ID_MDM_SLPI,    PATH_PREFIX"/rfs/mdm/slpi"},
};

/* One entry is required per prefix above. */
static const char* tftp_server_folders_shared_real_path_list[] = {
  PATH_PREFIX"/rfs/msm/mpss/hlos/",
  PATH_PREFIX"/rfs/msm/adsp/hlos/",
  PATH_PREFIX"/rfs/msm/slpi/hlos/",
  PATH_PREFIX"/rfs/mdm/mpss/hlos/",
  PATH_PREFIX"/rfs/mdm/adsp/hlos/",
  PATH_PREFIX"/rfs/mdm/slpi/hlos/",
  PATH_PREFIX"/rfs/mdm/tn/hlos/",
  PATH_PREFIX"/rfs/apq/gnss/hlos/",
};

/* One entry is required per prefix above. */
static const char* tftp_server_folders_ramdumps_real_path_list[] = {
  PATH_PREFIX"/rfs/msm/mpss/ramdumps/",
  PATH_PREFIX"/rfs/msm/adsp/ramdumps/",
  PATH_PREFIX"/rfs/msm/slpi/ramdumps/",
  PATH_PREFIX"/rfs/mdm/mpss/ramdumps/",
  PATH_PREFIX"/rfs/mdm/adsp/ramdumps/",
  PATH_PREFIX"/rfs/mdm/slpi/ramdumps/",
  PATH_PREFIX"/rfs/mdm/tn/ramdumps/",
  PATH_PREFIX"/rfs/apq/gnss/ramdumps/",
};

static const char* tftp_server_folders_data_folders_list[] = {
  DATA_PATH_PREFIX"/persist/rfs/shared/",
  DATA_PATH_PREFIX"/persist/rfs/msm/mpss/",
  DATA_PATH_PREFIX"/persist/rfs/msm/adsp/",
  DATA_PATH_PREFIX"/persist/rfs/msm/slpi/",
  DATA_PATH_PREFIX"/persist/rfs/mdm/mpss/",
  DATA_PATH_PREFIX"/persist/rfs/mdm/adsp/",
  DATA_PATH_PREFIX"/persist/rfs/mdm/slpi/",
  DATA_PATH_PREFIX"/persist/rfs/mdm/tn/",
  DATA_PATH_PREFIX"/persist/rfs/apq/gnss/",
};

const char* tftp_server_folders_hlos_shared_data_folders_list[] = {
  DATA_PATH_PREFIX"/persist/hlos_rfs/shared/",
};

const char* tftp_server_folders_ramdump_folders_list[] = {
  DATA_PATH_PREFIX"/data/vendor/tombstones/rfs/modem/",
  DATA_PATH_PREFIX"/data/vendor/tombstones/rfs/lpass/",
  DATA_PATH_PREFIX"/data/vendor/tombstones/rfs/tn/",
  DATA_PATH_PREFIX"/data/vendor/tombstones/rfs/slpi/",
};


static int32
tftp_server_folders_create_ramdump_folders (void)
{
  uint32 i, num_entries;
  int32 result, error_count;
  const char *dir_path;

  num_entries = (sizeof (tftp_server_folders_ramdump_folders_list) /
                sizeof (tftp_server_folders_ramdump_folders_list[0]));

  error_count = 0;

  for ( i = 0; i < num_entries; ++i )
  {
    dir_path = tftp_server_folders_ramdump_folders_list[i];
    TFTP_ASSERT (dir_path != NULL);
    if ((dir_path == NULL) || (*dir_path == '\0'))
    {
      continue;
    }

    result = tftp_os_auto_mkdir (dir_path, TFTP_SHARED_DIR_MODE,
                           TFTP_RAMDUMP_UID, TFTP_RAMDUMP_GID);
    if (result != 0)
    {
      error_count++;
      TFTP_LOG_ERR("Failed to auto_dir for(%s) errno = %d (%s)", dir_path,
                    result, strerror(-result));
    }
    tftp_os_mkdir (dir_path, TFTP_SHARED_DIR_MODE,
                   TFTP_RAMDUMP_UID, TFTP_RAMDUMP_GID);
  }

  result = 0;
  if (error_count > 0)
  {
    result = 1;
  }
  return result;
}


/* TODO: Fix to chmod and chown for sharing.  */
static int32
tftp_server_folders_create_hlos_shared_data_folders (void)
{
  uint32 i, num_entries;
  int32 result, error_count;
  const char *dir_path;

  num_entries = (sizeof (tftp_server_folders_hlos_shared_data_folders_list) /
                sizeof (tftp_server_folders_hlos_shared_data_folders_list[0]));

  error_count = 0;

  for ( i = 0; i < num_entries; ++i )
  {
    dir_path = tftp_server_folders_hlos_shared_data_folders_list[i];
    TFTP_ASSERT (dir_path != NULL);
    if ((dir_path == NULL) || (*dir_path == '\0'))
    {
      continue;
    }

    result = tftp_os_auto_mkdir (dir_path, TFTP_SHARED_DIR_MODE, -1,
                                  TFTP_SHARED_GID);
    if (result != 0)
    {
      error_count++;
      TFTP_LOG_ERR("Failed to auto_dir for(%s) errno = %d (%s)", dir_path,
                    result, strerror(-result));
    }
    tftp_os_mkdir (dir_path, TFTP_SHARED_DIR_MODE, -1, TFTP_SHARED_GID);
  }

  result = 0;
  if (error_count > 0)
  {
    result = 1;
  }
  return result;
}

static int32
tftp_server_folders_create_data_folders (void)
{
  uint32 i, num_entries;
  int32 result, error_count;
  const char *dir_path;

  num_entries = (sizeof (tftp_server_folders_data_folders_list) /
                 sizeof (tftp_server_folders_data_folders_list[0]));

  error_count = 0;

  for ( i = 0; i < num_entries; ++i )
  {
    dir_path = tftp_server_folders_data_folders_list[i];
    TFTP_ASSERT (dir_path != NULL);
    if ((dir_path == NULL) || (*dir_path == '\0'))
    {
      continue;
    }

    result = tftp_os_auto_mkdir (dir_path, TFTP_DEFAULT_DIR_MODE, -1, -1);
    if (result != 0)
    {
      error_count++;
      TFTP_LOG_ERR("Failed to auto_dir for(%s) errno = %d (%s)", dir_path,
                    result, strerror(-result));
    }
    tftp_os_mkdir (dir_path, TFTP_DEFAULT_DIR_MODE, -1, -1);
  }

  result = 0;
  if (error_count > 0)
  {
    result = 1;
  }
  return result;
}

int
tftp_server_folders_init (void)
{
  int result, ret_val = 0;

  result = tftp_server_folders_create_ramdump_folders ();
  if (result != 0)
  {
    ret_val = result;
  }

  result = tftp_server_folders_create_data_folders ();
  if(result != 0)
  {
    ret_val = result;
  }

  result = tftp_server_folders_create_hlos_shared_data_folders ();
  if(result != 0)
  {
    ret_val = result;
  }

  return ret_val;
}

const char*
tftp_server_folders_lookup_path_prefix (uint32 instance_id)
{
  uint32 i, num_entries;
  const char *path_prefix = NULL;
  struct tftp_server_folders_path_prefix_map_type *prefix_entry;

  num_entries = (sizeof (tftp_server_folders_path_prefix_map) /
                 sizeof (tftp_server_folders_path_prefix_map[0]));

  for ( i = 0; i < num_entries; ++i )
  {
    prefix_entry = &tftp_server_folders_path_prefix_map[i];
    if (prefix_entry->instance_id == instance_id)
    {
      path_prefix = prefix_entry->path_prefix;
      break;
    }
  }

  return path_prefix;
}

int
tftp_server_folders_check_if_shared_file (const char *path)
{
  uint32 i, num_entries, length;
  int is_shared_file = 0;
  const char *path_prefix = NULL;

  num_entries = (sizeof (tftp_server_folders_shared_real_path_list) /
                 sizeof (tftp_server_folders_shared_real_path_list[0]));

  for ( i = 0; i < num_entries; ++i )
  {
    path_prefix = tftp_server_folders_shared_real_path_list[i];
    TFTP_ASSERT (path_prefix != NULL);
    if ((path_prefix == NULL) || (*path_prefix == '\0'))
    {
      continue;
    }
    length = tftp_strlen (path_prefix);
    if (strncmp (path, path_prefix, length) == 0)
    {
      TFTP_LOG_INFO ("Found shared path : %s", path);
      is_shared_file = 1;
      break;
    }
  }

  return is_shared_file;
}

int
tftp_server_folders_check_if_ramdump_file (const char *path)
{
  uint32 i, num_entries, length;
  int is_ramdump_file = 0;
  const char *path_prefix = NULL;

  num_entries = (sizeof (tftp_server_folders_ramdumps_real_path_list) /
                sizeof (tftp_server_folders_ramdumps_real_path_list[0]));

  for ( i = 0; i < num_entries; ++i )
  {
    path_prefix = tftp_server_folders_ramdumps_real_path_list[i];
    TFTP_ASSERT (path_prefix != NULL);
    if ((path_prefix == NULL) || (*path_prefix == '\0'))
    {
      continue;
    }
    length = tftp_strlen (path_prefix);
    if (strncmp (path, path_prefix, length) == 0)
    {
      TFTP_LOG_DBG ("Found ramdump path : %s", path);
      is_ramdump_file = 1;
      break;
    }
  }

  return is_ramdump_file;
}

void
tftp_folders_upgrade_uid (void)
{
  #ifndef TFTP_LE_BUILD_ONLY
      tftp_os_lchown_recursive (DATA_PATH_PREFIX"/persist/rfs",
                                AID_RFS, AID_RFS);

      tftp_os_lchown_recursive (DATA_PATH_PREFIX"/persist/hlos_rfs",
                                AID_RFS, AID_RFS_SHARED);
  #endif
}
