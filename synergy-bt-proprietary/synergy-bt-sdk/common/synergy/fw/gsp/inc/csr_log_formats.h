#ifndef CSR_LOG_FORMATS_H__
#define CSR_LOG_FORMATS_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_log.h"
#include "csr_log_text.h"
#include "csr_log_configure.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generic log structure */
typedef struct CsrLog /* non-scheduler log */
{
    void (*lregtech)(void *ltHdl,
        const CsrLogTechInformation *techInfo);
    void (*lbci)(void *ltHdl,
        CsrUint8 channel,
        CsrBool received,
        CsrSize payload_length,
        const void *payload);
    void (*ltrans)(void *ltHdl,
        CsrUint8 transport_type,
        CsrUint8 direction,
        CsrUint32 consumed_length,
        CsrUint32 payload_length,
        const void *payload);
    void (*lexcpstate)(void *ltHdl,
        CsrUint16 prim_type,
        CsrPrim msg_type,
        CsrUint16 state,
        CsrUint32 line,
        const CsrCharString *file);
    void (*lexcpgen)(void *ltHdl,
        CsrUint16 prim_type,
        CsrPrim msg_type,
        const CsrCharString *text,
        CsrUint32 line,
        const CsrCharString *file);
    void (*lexcpwarn)(void *ltHdl,
        CsrUint16 prim_type,
        CsrPrim msg_type,
        const CsrCharString *text,
        CsrUint32 line,
        const CsrCharString *file);
    void (*ltextregister)(void *ltHdl,
        CsrLogTextTaskId taskId,
        const CsrCharString *taskName,
        CsrUint16 subOriginsLen,
        const CsrLogSubOrigin *subOrigins);
    void (*ltextregister2)(void *ltHdl,
        CsrLogTextTaskId taskId,
        const CsrCharString *originName,
        CsrUint16 subOriginsCount,
        const CsrCharString *subOrigins[]);
    void (*ltextprint)(void *ltHdl,
        CsrLogTextTaskId taskId,
        CsrUint16 subOrigin,
        CsrLogLevelText level,
        const CsrCharString *formatString,
        va_list varargs);
    void (*ltextprint2)(void *ltHdl,
        CsrLogTextTaskId taskId,
        CsrUint16 subOrigin,
        const CsrCharString *originName,
        const CsrCharString *subOriginName,
        CsrLogLevelText level,
        const CsrCharString *formatString,
        va_list varargs);
    void (*ltextbufprint)(void *ltHdl,
        CsrLogTextTaskId taskId,
        CsrUint16 subOrigin,
        CsrLogLevelText level,
        CsrSize bufferLength,
        const void *buffer,
        const CsrCharString *formatString,
        va_list varargs);
    void (*ltextbufprint2)(void *ltHdl,
        CsrLogTextTaskId taskId,
        CsrUint16 subOrigin,
        const CsrCharString *originName,
        const CsrCharString *subOriginName,
        CsrLogLevelText level,
        CsrSize bufferLength,
        const void *buffer,
        const CsrCharString *formatString,
        va_list varargs);
    void (*ltasktext)(void *ltHdl,
        const CsrCharString *text,
        CsrUint32 line,
        const CsrCharString *file);
    void (*lproto)(void *ltHdl,
        CsrUint8 protocol,
        CsrUint8 event,
        CsrUint32 line,
        const CsrCharString *file);
    void (*lstatetrans)(void *ltHdl,
        bitmask16_t mask,
        CsrUint32 identifier,
        const CsrCharString *fsm_name,
        CsrUint32 prev_state,
        const CsrCharString *prev_state_str,
        CsrUint32 in_event,
        const CsrCharString *in_event_str,
        CsrUint32 next_state,
        const CsrCharString *next_state_str,
        CsrUint32 line,
        const CsrCharString *file);
    void (*lsave)(void *ltHdl,
        CsrUint16 prim_type,
        const void *msg,
        CsrBool onlyPrimType,
        CsrBool limitPrim);
    void (*lpop)(void *ltHdl,
        CsrUint16 prim_type,
        const void *msg,
        CsrBool onlyPrimType,
        CsrBool limitPrim);
    /* scheduler log */
    void (*lschedinit)(void *ltHdl, CsrUint8 thread_id);
    void (*lscheddeinit)(void *ltHdl, CsrUint8 thread_id);
    void (*lschedstart)(void *ltHdl, CsrUint8 thread_id);
    void (*lschedstop)(void *ltHdl, CsrUint8 thread_id);
    void (*linittask)(void *ltHdl,
        CsrUint8 thread_id,
        CsrSchedQid task_id,
        const CsrCharString *tskName);
    void (*ldeinittask)(void *ltHdl,
        CsrSchedQid task_id);
    void (*lactivate)(void *ltHdl,
        CsrSchedQid task_id);
    void (*ldeactivate)(void *ltHdl,
        CsrSchedQid task_id);
    void (*lputmsg)(void *ltHdl,
        CsrUint32 line,
        const CsrCharString *file,
        CsrSchedQid src_task_id,
        CsrSchedQid dst_taskid,
        CsrSchedMsgId msg_id,
        CsrUint16 prim_type,
        const void *msg,
        CsrBool onlyPrimType,
        CsrBool limitPrim);
    void (*lgetmsg)(void *ltHdl,
        CsrSchedQid src_task_id,
        CsrSchedQid dst_taskid,
        CsrBool get_res,
        CsrSchedMsgId msg_id,
        CsrUint16 prim_type,
        const void *msg,
        CsrBool onlyPrimType,
        CsrBool limitPrim);
    void (*ltimedeventin)(void *ltHdl,
        CsrUint32 line,
        const CsrCharString *file,
        CsrSchedQid task_id,
        CsrSchedTid tid,
        CsrTime requested_delay,
        CsrUint16 fniarg,
        const CsrUint8 *fnvarg);
    void (*ltimedeventfire)(void *ltHdl,
        CsrSchedQid task_id,
        CsrSchedTid tid);
    void (*ltimedeventdone)(void *ltHdl,
        CsrSchedQid task_id,
        CsrSchedTid tid);
    void (*ltimedeventcancel)(void *ltHdl,
        CsrUint32 line,
        const CsrCharString *file,
        CsrSchedQid task_id,
        CsrSchedTid tid,
        CsrBool cancel_res);
    void (*bgintreg)(void *ltHdl,
        CsrUint8 thread_id,
        CsrSchedBgint irq,
        const CsrCharString *callback,
        const void *ptr);
    void (*bgintunreg)(void *ltHdl,
        CsrSchedBgint irq);
    void (*bgintset)(void *ltHdl,
        CsrSchedBgint irq);
    void (*bgintservicestart)(void *ltHdl,
        CsrSchedBgint irq);
    void (*bgintservicedone)(void *ltHdl,
        CsrSchedBgint irq);
    /* log transport handle */
    void *ltHdl;

    /* log extensions */
    CsrUint8 extSize;
    void   **ext;

    /* Internal */
    struct CsrLog *next;
} CsrLog;

/*---------------------------------*/
/*  Log registration */
/*---------------------------------*/
CsrBool CsrLogFormatInstRegister(CsrLog *type);
void CsrLogUnregister(CsrLog *type);

#ifdef __cplusplus
}
#endif

#endif
