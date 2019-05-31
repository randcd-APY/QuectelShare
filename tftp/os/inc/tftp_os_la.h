/***********************************************************************
 * tftp_os_la.h
 *
 * Short description
 * Copyright (c) 2014,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose description.
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

#ifndef __TFTP_OS_LA_H__
#define __TFTP_OS_LA_H__


#include "tftp_config_i.h"
#include "tftp_comdef.h"

#include "errno.h"
#include <fcntl.h>

#ifndef TFTP_LE_BUILD_ONLY
  #include <private/android_filesystem_config.h>
#ifdef TARGET_FS_CONFIG_GEN
  #include "generated_oem_aid.h"
#endif
#endif

#if !defined (TFTP_LA_BUILD)
  #error "This file should only be compiled for LA build"
#endif

#define MAIN_TYPE

#define OS_PATH_SEPARATOR '/'
#define INVALID_OS_PATH_SEPARATOR '\\'

#define OS_BAD_PATH_STRING1 "/../"
#define OS_BAD_PATH_STRING2 "/.."
#define OS_BAD_PATH_STRING3 "\\"

#ifndef AID_RFS
#define AID_RFS AID_SYSTEM
#endif
#ifndef AID_RFS_SHARED
#define AID_RFS_SHARED AID_SYSTEM
#endif

#ifndef TFTP_LE_BUILD_ONLY
#define TFTP_SHARED_GID  AID_RFS_SHARED
#define TFTP_RAMDUMP_GID AID_SYSTEM
#define TFTP_RAMDUMP_UID AID_SYSTEM
#else
#define TFTP_SHARED_GID  (-1)
#define TFTP_RAMDUMP_GID (-1)
#define TFTP_RAMDUMP_GID (-1)
#define TFTP_RAMDUMP_UID (-1)
#endif


#endif /* not __TFTP_OS_LA_H__ */
