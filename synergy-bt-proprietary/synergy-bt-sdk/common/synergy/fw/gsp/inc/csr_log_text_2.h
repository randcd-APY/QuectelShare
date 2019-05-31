#ifndef CSR_LOG_TEXT_2_H__
#define CSR_LOG_TEXT_2_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_log_configure.h"
#include "csr_log_text.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Undefine the macros from csr_log_text.h */
#undef CSR_LOG_TEXT_CRITICAL
#undef CSR_LOG_TEXT_CONDITIONAL_CRITICAL
#undef CSR_LOG_TEXT_BUFFER_CRITICAL
#undef CSR_LOG_TEXT_BUFFER_CONDITIONAL_CRITICAL
#undef CSR_LOG_TEXT_ERROR
#undef CSR_LOG_TEXT_CONDITIONAL_ERROR
#undef CSR_LOG_TEXT_BUFFER_ERROR
#undef CSR_LOG_TEXT_BUFFER_CONDITIONAL_ERROR
#undef CSR_LOG_TEXT_WARNING
#undef CSR_LOG_TEXT_CONDITIONAL_WARNING
#undef CSR_LOG_TEXT_BUFFER_WARNING
#undef CSR_LOG_TEXT_BUFFER_CONDITIONAL_WARNING
#undef CSR_LOG_TEXT_INFO
#undef CSR_LOG_TEXT_CONDITIONAL_INFO
#undef CSR_LOG_TEXT_BUFFER_INFO
#undef CSR_LOG_TEXT_BUFFER_CONDITIONAL_INFO
#undef CSR_LOG_TEXT_DEBUG
#undef CSR_LOG_TEXT_CONDITIONAL_DEBUG
#undef CSR_LOG_TEXT_BUFFER_DEBUG
#undef CSR_LOG_TEXT_BUFFER_CONDITIONAL_DEBUG
#undef CSR_LOG_TEXT_ASSERT
#undef CSR_LOG_TEXT_UNHANDLED_PRIMITIVE

/* Log Text Handle */
typedef struct CsrLogTextHandle CsrLogTextHandle;

/* Use CSR_LOG_TEXT_REGISTER to register a component before using the text
   logging interface from that component. Initially the handle pointer must be
   initialised to NULL, for the registration to take effect. On return the
   handle pointer will be set to a non-NULL value, and subsequent registrations
   with the same handle pointer will have no effect as the macro will do nothing
   if the handle pointer is not NULL. This allows the call to be placed in code
   paths that may be executed multiple times without causing multiple
   registrations. When the log system is deinitialised, all (registered) handle
   pointers will automatically be reset to NULL. */
#ifdef CSR_LOG_ENABLE
void CsrLogTextRegister2(CsrLogTextHandle **handle, const CsrCharString *originName, CsrUint16 subOriginsCount, const CsrCharString *subOrigins[]);
#define CSR_LOG_TEXT_REGISTER(handle, taskName, subOriginsCount, subOrigins) {if (*(handle) == NULL) {CsrLogTextRegister2(handle, taskName, subOriginsCount, subOrigins);}}
#define CSR_LOG_TEXT_HANDLE_DEFINE(name) CsrLogTextHandle * name = NULL
#define CSR_LOG_TEXT_HANDLE_DECLARE(name) extern CsrLogTextHandle * name
#else
#define CSR_LOG_TEXT_REGISTER(handle, taskName, subOriginsCount, subOrigins)
#define CSR_LOG_TEXT_HANDLE_DEFINE(name) extern CsrLogTextHandle * name
#define CSR_LOG_TEXT_HANDLE_DECLARE(name) extern CsrLogTextHandle * name
#endif

/* CRITICAL: Conditions that are threatening to the integrity/stability of the
   system as a whole. */
#if defined(CSR_LOG_ENABLE) && !defined(CSR_LOG_LEVEL_TEXT_CRITICAL_DISABLE)
void CsrLogTextCritical2(CsrLogTextHandle *handle, CsrUint16 subOrigin, const CsrCharString *formatString, ...);
void CsrLogTextBufferCritical2(CsrLogTextHandle *handle, CsrUint16 subOrigin, CsrSize bufferLength, const void *buffer, const CsrCharString *formatString, ...);
#define CSR_LOG_TEXT_CRITICAL(handle_subOrigin_formatString_varargs) CsrLogTextCritical2 handle_subOrigin_formatString_varargs
#define CSR_LOG_TEXT_CONDITIONAL_CRITICAL(condition, logtextargs) {if (condition) {CSR_LOG_TEXT_CRITICAL(logtextargs);}}
#define CSR_LOG_TEXT_BUFFER_CRITICAL(handle_subOrigin_length_buffer_formatString_varargs) CsrLogTextBufferCritical2 handle_subOrigin_length_buffer_formatString_varargs
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_CRITICAL(condition, logtextbufferargs) {if (condition) {CSR_LOG_TEXT_BUFFER_CRITICAL(logtextbufferargs);}}
#else
#define CSR_LOG_TEXT_CRITICAL(handle_subOrigin_formatString_varargs)
#define CSR_LOG_TEXT_CONDITIONAL_CRITICAL(condition, logtextargs)
#define CSR_LOG_TEXT_BUFFER_CRITICAL(handle_subOrigin_length_buffer_formatString_varargs)
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_CRITICAL(condition, logtextbufferargs)
#endif

/* ERROR: Malfunction of a component rendering it unable to operate correctly,
   causing lack of functionality but not loss of system integrity/stability. */
#if defined(CSR_LOG_ENABLE) && !defined(CSR_LOG_LEVEL_TEXT_ERROR_DISABLE)
void CsrLogTextError2(CsrLogTextHandle *handle, CsrUint16 subOrigin, const CsrCharString *formatString, ...);
void CsrLogTextBufferError2(CsrLogTextHandle *handle, CsrUint16 subOrigin, CsrSize bufferLength, const void *buffer, const CsrCharString *formatString, ...);
#define CSR_LOG_TEXT_ERROR(handle_subOrigin_formatString_varargs) CsrLogTextError2 handle_subOrigin_formatString_varargs
#define CSR_LOG_TEXT_CONDITIONAL_ERROR(condition, logtextargs) {if (condition) {CSR_LOG_TEXT_ERROR(logtextargs);}}
#define CSR_LOG_TEXT_BUFFER_ERROR(handle_subOrigin_length_buffer_formatString_varargs) CsrLogTextBufferError2 handle_subOrigin_length_buffer_formatString_varargs
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_ERROR(condition, logtextbufferargs) {if (condition) {CSR_LOG_TEXT_BUFFER_ERROR(logtextbufferargs);}}
#else
#define CSR_LOG_TEXT_ERROR(handle_subOrigin_formatString_varargs)
#define CSR_LOG_TEXT_CONDITIONAL_ERROR(condition, logtextargs)
#define CSR_LOG_TEXT_BUFFER_ERROR(handle_subOrigin_length_buffer_formatString_varargs)
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_ERROR(condition, logtextbufferargs)
#endif

/* WARNING: Conditions that are unexpected and indicative of possible problems
   or violations of specifications, where the result of such deviations does not
   lead to malfunction of the component. */
#if defined(CSR_LOG_ENABLE) && !defined(CSR_LOG_LEVEL_TEXT_WARNING_DISABLE)
void CsrLogTextWarning2(CsrLogTextHandle *handle, CsrUint16 subOrigin, const CsrCharString *formatString, ...);
void CsrLogTextBufferWarning2(CsrLogTextHandle *handle, CsrUint16 subOrigin, CsrSize bufferLength, const void *buffer, const CsrCharString *formatString, ...);
#define CSR_LOG_TEXT_WARNING(handle_subOrigin_formatString_varargs) CsrLogTextWarning2 handle_subOrigin_formatString_varargs
#define CSR_LOG_TEXT_CONDITIONAL_WARNING(condition, logtextargs) {if (condition) {CSR_LOG_TEXT_WARNING(logtextargs);}}
#define CSR_LOG_TEXT_BUFFER_WARNING(handle_subOrigin_length_buffer_formatString_varargs) CsrLogTextBufferWarning2 handle_subOrigin_length_buffer_formatString_varargs
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_WARNING(condition, logtextbufferargs) {if (condition) {CSR_LOG_TEXT_BUFFER_WARNING(logtextbufferargs);}}
#else
#define CSR_LOG_TEXT_WARNING(handle_subOrigin_formatString_varargs)
#define CSR_LOG_TEXT_CONDITIONAL_WARNING(condition, logtextargs)
#define CSR_LOG_TEXT_BUFFER_WARNING(handle_subOrigin_length_buffer_formatString_varargs)
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_WARNING(condition, logtextbufferargs)
#endif

/* INFO: Important events that may aid in determining the conditions under which
   the more severe conditions are encountered. */
#if defined(CSR_LOG_ENABLE) && !defined(CSR_LOG_LEVEL_TEXT_INFO_DISABLE)
void CsrLogTextInfo2(CsrLogTextHandle *handle, CsrUint16 subOrigin, const CsrCharString *formatString, ...);
void CsrLogTextBufferInfo2(CsrLogTextHandle *handle, CsrUint16 subOrigin, CsrSize bufferLength, const void *buffer, const CsrCharString *formatString, ...);
#define CSR_LOG_TEXT_INFO(handle_subOrigin_formatString_varargs) CsrLogTextInfo2 handle_subOrigin_formatString_varargs
#define CSR_LOG_TEXT_CONDITIONAL_INFO(condition, logtextargs) {if (condition) {CSR_LOG_TEXT_INFO(logtextargs);}}
#define CSR_LOG_TEXT_BUFFER_INFO(handle_subOrigin_length_buffer_formatString_varargs) CsrLogTextBufferInfo2 handle_subOrigin_length_buffer_formatString_varargs
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_INFO(condition, logtextbufferargs) {if (condition) {CSR_LOG_TEXT_BUFFER_INFO(logtextbufferargs);}}
#else
#define CSR_LOG_TEXT_INFO(handle_subOrigin_formatString_varargs)
#define CSR_LOG_TEXT_CONDITIONAL_INFO(condition, logtextargs)
#define CSR_LOG_TEXT_BUFFER_INFO(handle_subOrigin_length_buffer_formatString_varargs)
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_INFO(condition, logtextbufferargs)
#endif

/* DEBUG: Similar to INFO, but dedicated to events that occur more frequently. */
#if defined(CSR_LOG_ENABLE) && !defined(CSR_LOG_LEVEL_TEXT_DEBUG_DISABLE)
void CsrLogTextDebug2(CsrLogTextHandle *handle, CsrUint16 subOrigin, const CsrCharString *formatString, ...);
void CsrLogTextBufferDebug2(CsrLogTextHandle *handle, CsrUint16 subOrigin, CsrSize bufferLength, const void *buffer, const CsrCharString *formatString, ...);
#define CSR_LOG_TEXT_DEBUG(handle_subOrigin_formatString_varargs) CsrLogTextDebug2 handle_subOrigin_formatString_varargs
#define CSR_LOG_TEXT_CONDITIONAL_DEBUG(condition, logtextargs) {if (condition) {CSR_LOG_TEXT_DEBUG(logtextargs);}}
#define CSR_LOG_TEXT_BUFFER_DEBUG(handle_subOrigin_length_buffer_formatString_varargs) CsrLogTextBufferDebug2 handle_subOrigin_length_buffer_formatString_varargs
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_DEBUG(condition, logtextbufferargs) {if (condition) {CSR_LOG_TEXT_BUFFER_DEBUG(logtextbufferargs);}}
#else
#define CSR_LOG_TEXT_DEBUG(handle_subOrigin_formatString_varargs)
#define CSR_LOG_TEXT_CONDITIONAL_DEBUG(condition, logtextargs)
#define CSR_LOG_TEXT_BUFFER_DEBUG(handle_subOrigin_length_buffer_formatString_varargs)
#define CSR_LOG_TEXT_BUFFER_CONDITIONAL_DEBUG(condition, logtextbufferargs)
#endif

/* CSR_LOG_TEXT_ASSERT (CRITICAL) */
#ifdef CSR_LOG_ENABLE
#define CSR_LOG_TEXT_ASSERT(origin, suborigin, condition) \
    {if (!(condition)) {CSR_LOG_TEXT_CRITICAL((origin, suborigin, "Assertion \"%s\" failed at %s:%u", #condition, __FILE__, __LINE__));}}
#else
#define CSR_LOG_TEXT_ASSERT(origin, suborigin, condition)
#endif

/* CSR_LOG_TEXT_UNHANDLED_PRIM (CRITICAL) */
#ifdef CSR_LOG_ENABLE
#define CSR_LOG_TEXT_UNHANDLED_PRIMITIVE(origin, suborigin, primClass, primType) \
    CSR_LOG_TEXT_CRITICAL((origin, suborigin, "Unhandled primitive 0x%04X:0x%04X at %s:%u", primClass, primType, __FILE__, __LINE__))
#else
#define CSR_LOG_TEXT_UNHANDLED_PRIMITIVE(origin, suborigin, primClass, primType)
#endif

#ifdef __cplusplus
}
#endif

#endif
