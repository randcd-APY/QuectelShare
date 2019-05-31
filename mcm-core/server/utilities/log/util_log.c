/***************************************************************************************************
    @file
    util_log.c

    @brief
    Facilitates logging by providing logging utilities.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#include "util_log.h"


void ril_format_log_msg ( char *buf_ptr,
                         int buf_size,
                         char *fmt,
                         ...)
{
    va_list ap;
    va_start( ap, fmt );

    if ( NULL != buf_ptr && buf_size > 0 )
    {
        vsnprintf( buf_ptr, buf_size, fmt, ap );
    }

    va_end( ap );
}
