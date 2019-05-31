#ifndef CSR_LOG_CLEARTEXT_H__
#define CSR_LOG_CLEARTEXT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_log_formats.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The cleartext log formatter can be configured using either:
 *
 *  - CSR_LOG_CLEARTEXT_FORMAT (config/csr_usr_config.h)
 *  - 'outputFormatString' parameter given to CsrLogCleartextCreate()
 *
 * Note:
 *  - template names (e.g. CSR_LOG_CLEARTEXT_TEMPLATE_LOG_LEVEL_NAME) will be space/left aligned
 *  - template values (e.g. CSR_LOG_CLEARTEXT_TEMPLATE_TIME_HIGH) will be space/right aligned
 *  - each template parameter may only appears once in the template string
 *  - the output will be shortened according to CSR_LOG_TEXT_MAX_STRING_LEN.
 *  - a newline will automatically be appended by the formatter
 */

/* Timestamp each log entry. using CsrTimeUtcGet() */
#define CSR_LOG_CLEARTEXT_TEMPLATE_YEAR             "$y$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_MONTH            "$mo$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_DAY              "$d$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_HOUR             "$h$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_MIN              "$mi$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_TIME_SEC         "$ts$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_TIME_MSEC        "$tm$"
/* Timestamp each log entry. using CsrTimeGet() */
#define CSR_LOG_CLEARTEXT_TEMPLATE_TIME_HIGH        "$th$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_TIME_LOW         "$tl$"
/* Add loglevel name/literal value (see CSR_LOG_LEVEL_TEXT_* defines) */
#define CSR_LOG_CLEARTEXT_TEMPLATE_LOG_LEVEL_NAME   "$ln$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_LOG_LEVEL_ID     "$li$"
/* Add task name/literal value  */
#define CSR_LOG_CLEARTEXT_TEMPLATE_TASK_NAME        "$tn$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_TASK_ID          "$ti$"
/* Add suborigin name/literal value  */
#define CSR_LOG_CLEARTEXT_TEMPLATE_SUBORIGIN_NAME   "$sn$"
#define CSR_LOG_CLEARTEXT_TEMPLATE_SUBORIGIN_ID     "$si$"
/* Add string provided by the caller of CsrLogText*() */
#define CSR_LOG_CLEARTEXT_TEMPLATE_STRING           "$s$"
/* Add buffer provided by the caller of CsrLogText*() */
#define CSR_LOG_CLEARTEXT_TEMPLATE_BUFFER           "$b$"

CsrLog *CsrLogCleartextCreate(void *ltHdl, const CsrCharString *outputFormatString, CsrBool schedEvents);
void CsrLogCleartextDestroy(CsrLog *l);

#ifdef __cplusplus
}
#endif

#endif /* CSR_LOG_CLEARTEXT_H__ */
