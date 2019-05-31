#ifndef QTI_SOCKSV5_LOG_MSG_H_
#define QTI_SOCKSV5_LOG_MSG_H_

/*==========================================================================

  FILE:  qti_socksv5_log_msg.h

  SERVICES:

  SOCKSv5 header file for diag logging.

==========================================================================*/

/*==========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

==========================================================================*/

/*==========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------------------------
  03/20/17   jt         SOCKSv5 support.
==========================================================================*/

#include "ds_util.h"
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif


/** Macro for a high-level message. */
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_HIGH

/** Macro for a medium-level message. */
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED

/** Macro for a low-level message. */
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW

/**  Macro for a error message. */
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR

/** Macro to print the log message information. */
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);

/** Macro to print a high-level message. */
#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}

/** Macro to print a medium-level message. */
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
/** Macro to print a low-level message. */
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
/** Macro to print an error message. */
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}

#endif
