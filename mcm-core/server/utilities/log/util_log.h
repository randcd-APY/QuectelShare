/***************************************************************************************************
    @file
    util_log.h

    @brief
    Facilitates logging by providing logging utilities.

    Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#ifndef UTIL_LOG
#define UTIL_LOG

#include <diag/log.h>
#include <diag/msgcfg.h>
#include <stdarg.h>

#define RIL_MAX_LOG_MSG_SIZE 512

#define UTIL_LOG_MSG RIL_LOG_DEBUG
#define UTIL_LOG_INFO RIL_LOG_INFO
#define UTIL_LOG_WARN RIL_LOG_WARN
#define UTIL_LOG_ERROR RIL_LOG_ERROR
#define UTIL_LOG_FUNC_ENTRY RIL_LOG_FUNC_ENTRY
#define UTIL_LOG_FUNC_EXIT RIL_LOG_FUNC_RETURN
#define UTIL_LOG_FUNC_ENTRY_WITH_ARGS RIL_LOG_FUNC_ENTRY_WITH_ARGS
#define UTIL_LOG_FUNC_EXIT_WITH_RET RIL_LOG_FUNC_RETURN_WITH_RET


extern void ril_format_log_msg ( char *buf_ptr,
                                 int buf_size,
                                 char *fmt,
                                 ...);

#define RIL_LOG_MSG( lvl, fmt, ... )                                           \
    {                                                                          \
        char log_buf[ RIL_MAX_LOG_MSG_SIZE ];                                  \
        ril_format_log_msg( log_buf,                                           \
                            RIL_MAX_LOG_MSG_SIZE,                              \
                            "RIL %s "#fmt,                                   \
                            __func__,                                          \
                            ##__VA_ARGS__ );                                   \
        MSG_SPRINTF_1( MSG_SSID_ANDROID_QCRIL, lvl, "%s", log_buf );           \
    }                                                                          \


#define RIL_LOG_WARN( ... )   RIL_LOG_MSG( MSG_LEGACY_HIGH, __VA_ARGS__ )

/* Log error level message */
#define RIL_LOG_ERROR( ... )   RIL_LOG_MSG( MSG_LEGACY_ERROR, __VA_ARGS__ )

/* Log fatal level message */
#define RIL_LOG_FATAL( ... )   RIL_LOG_MSG( MSG_LEGACY_FATAL, __VA_ARGS__ )

/* Log Essential messages */
#define RIL_LOG_ESSENTIAL( ... ) RIL_LOG_MSG( MSG_LEGACY_ESSENTIAL, __VA_ARGS__)

/* Log debug level message */
#define RIL_LOG_DEBUG( ... )   RIL_LOG_MSG( MSG_LEGACY_HIGH, __VA_ARGS__ )

/* Log info level message */
#define RIL_LOG_INFO( ...  )   RIL_LOG_MSG( MSG_LEGACY_MED, __VA_ARGS__ )

/* Log verbose level message */
#define RIL_LOG_VERBOSE( ... ) RIL_LOG_MSG( MSG_LEGACY_LOW, __VA_ARGS__ )

/* Log function entry message */
#define RIL_LOG_FUNC_ENTRY()   RIL_LOG_MSG( MSG_LEGACY_LOW, "function entry" )

/* Log function exit message */
#define RIL_LOG_FUNC_RETURN(...)  RIL_LOG_MSG( MSG_LEGACY_LOW, "function exit" )

#define RIL_LOG_FUNC_RETURN_WITH_RET( ... ) RIL_LOG_MSG( MSG_LEGACY_LOW, "function exit with ret %d", __VA_ARGS__ )

#endif
