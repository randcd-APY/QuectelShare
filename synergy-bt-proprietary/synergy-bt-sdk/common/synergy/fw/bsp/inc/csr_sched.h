#ifndef CSR_SCHED_H__
#define CSR_SCHED_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_types.h"
#include "csr_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* An identifier issued by the scheduler. */
typedef CsrUint32 CsrSchedIdentifier;

/* A task identifier */
typedef CsrUint16 CsrSchedTaskId;

/* A queue identifier */
typedef CsrUint16 CsrSchedQid;
#define CSR_SCHED_QID_INVALID     ((CsrSchedQid) 0xFFFF)

/* A message identifier */
typedef CsrSchedIdentifier CsrSchedMsgId;

/* A timer event identifier */
typedef CsrSchedIdentifier CsrSchedTid;
#define CSR_SCHED_TID_INVALID     ((CsrSchedTid) 0)

/* Scheduler entry functions share this structure */
typedef void (*schedEntryFunction_t)(void **inst);

/* Time constants. */
#define CSR_SCHED_TIME_MAX                ((CsrTime) 0xFFFFFFFF)
#define CSR_SCHED_MILLISECOND             ((CsrTime) (1000))
#define CSR_SCHED_SECOND                  ((CsrTime) (1000 * CSR_SCHED_MILLISECOND))
#define CSR_SCHED_MINUTE                  ((CsrTime) (60 * CSR_SCHED_SECOND))

/* Queue and primitive that identifies the environment */
#define CSR_SCHED_TASK_ID        0xFFFF
#define CSR_SCHED_PRIM                   (CSR_SCHED_TASK_ID)
#define CSR_SCHED_EXCLUDED_MODULE_QUEUE      0xFFFF

/*
 * Background interrupt definitions
 */
typedef CsrUint16 CsrSchedBgint;
#define CSR_SCHED_BGINT_INVALID ((CsrSchedBgint) 0xFFFF)

typedef void (*CsrSchedBgintHandler)(void *);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedBgintReg
 *
 *  DESCRIPTION
 *      Register a background interrupt handler function with the scheduler.
 *        When CsrSchedBgint() is called from the foreground (e.g. an interrupt
 *        routine) the registered function is called.
 *
 *        "cb" is not permitted to be NULL.
 *
 *        If no bgints are available, CSR_SCHED_BGINT_INVALID is returned,
 *        otherwise a CsrSchedBgint value is returned to be used in subsequent
 *        calls to CsrSchedBgint().  id is a possibly NULL identifier used for
 *        logging purposes only.
 *
 *  RETURNS
 *      CsrSchedBgint -- CSR_SCHED_BGINT_INVALID denotes failure to obtain a CsrSchedBgintSet.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
CsrSchedBgint CsrSchedBgintRegStringLog(CsrSchedBgintHandler cb,
    void *context,
    const CsrCharString *id);
#define CsrSchedBgintReg CsrSchedBgintRegStringLog
#else
CsrSchedBgint CsrSchedBgintReg(CsrSchedBgintHandler cb,
    void *context,
    const CsrCharString *id);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedBgintUnreg
 *
 *  DESCRIPTION
 *      Unregister a background interrupt handler function.
 *
 *      ``irq'' is a background interrupt handle previously obtained
 *      from a call to CsrSchedBgintReg().
 *
 *  RETURNS
 *      void.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
void CsrSchedBgintUnregStringLog(CsrSchedBgint bgint);
#define CsrSchedBgintUnreg CsrSchedBgintUnregStringLog
#else
void CsrSchedBgintUnreg(CsrSchedBgint bgint);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedBgintSet
 *
 *  DESCRIPTION
 *      Set background interrupt.
 *
 *  RETURNS
 *      void.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
void CsrSchedBgintSetStringLog(CsrSchedBgint irq);
#define CsrSchedBgintSet CsrSchedBgintSetStringLog
#else
void CsrSchedBgintSet(CsrSchedBgint bgint);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedMessagePut
 *
 *  DESCRIPTION
 *      Sends a message consisting of the integer "mi" and the void * pointer
 *      "mv" to the message queue "q".
 *
 *      "mi" and "mv" are neither inspected nor changed by the scheduler - the
 *      task that owns "q" is expected to make sense of the values. "mv" may
 *      be null.
 *
 *  NOTE
 *      If "mv" is not null then it will typically be a chunk of CsrPmemAlloc()ed
 *      memory, though there is no need for it to be so. Tasks should normally
 *      obey the convention that when a message built with CsrPmemAlloc()ed memory
 *      is given to CsrSchedMessagePut() then ownership of the memory is ceded to the
 *      scheduler - and eventually to the recipient task. I.e., the receiver of
 *      the message will be expected to CsrPmemFree() the message storage.
 *
 *  RETURNS
 *      void.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
void CsrSchedMessagePutStringLog(CsrSchedQid q,
    CsrUint16 mi,
    void *mv,
    CsrUint32 line,
    const CsrCharString *file);
#ifdef CSR_LOG_INCLUDE_FILE_NAME_AND_LINE_NUMBER
#define CsrSchedMessagePut(q, mi, mv) CsrSchedMessagePutStringLog((q), (mi), (mv), __LINE__, __FILE__)
#else
#define CsrSchedMessagePut(q, mi, mv) CsrSchedMessagePutStringLog((q), (mi), (mv), 0, NULL)
#endif
#else
void CsrSchedMessagePut(CsrSchedQid q,
    CsrUint16 mi,
    void *mv);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedMessageBroadcast
 *
 *  DESCRIPTION
 *      Sends a message to all tasks.
 *
 *      The user must supply a "factory function" that is called once
 *      for every task that exists. The "factory function", msg_build_func,
 *      must allocate and initialise the message and set the msg_build_ptr
 *      to point to the message when done.
 *
 *  NOTE
 *      N/A
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
void CsrSchedMessageBroadcastStringLog(CsrUint16 mi,
    void *(*msg_build_func)(void *),
    void *msg_build_ptr,
    CsrUint32 line,
    const CsrCharString *file);
#ifdef CSR_LOG_INCLUDE_FILE_NAME_AND_LINE_NUMBER
#define CsrSchedMessageBroadcast(mi, fn, ptr) CsrSchedMessageBroadcastStringLog((mi), (fn), (ptr), __LINE__, __FILE__)
#else
#define CsrSchedMessageBroadcast(mi, fn, ptr) CsrSchedMessageBroadcastStringLog((mi), (fn), (ptr), 0, NULL)
#endif
#else
void CsrSchedMessageBroadcast(CsrUint16 mi,
    void *(*msg_build_func)(void *),
    void *msg_build_ptr);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedMessageGet
 *
 *  DESCRIPTION
 *      Obtains a message from the message queue belonging to the calling task.
 *      The message consists of one or both of a CsrUint16 and a void *.
 *
 *  RETURNS
 *      CsrBool - TRUE if a message has been obtained from the queue, else FALSE.
 *      If a message is taken from the queue, then "*pmi" and "*pmv" are set to
 *      the "mi" and "mv" passed to CsrSchedMessagePut() respectively.
 *
 *      "pmi" and "pmv" can be null, in which case the corresponding value from
 *      them message is discarded.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
CsrBool CsrSchedMessageGetStringLog(CsrUint16 *pmi, void **pmv);
#define CsrSchedMessageGet CsrSchedMessageGetStringLog
#else
CsrBool CsrSchedMessageGet(CsrUint16 *pmi, void **pmv);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedTimerSet
 *
 *  DESCRIPTION
 *      Causes the void function "fn" to be called with the arguments
 *      "fniarg" and "fnvarg" after "delay" has elapsed.
 *
 *      "delay" must be less than half the range of a CsrTime.
 *
 *      CsrSchedTimerSet() does nothing with "fniarg" and "fnvarg" except
 *      deliver them via a call to "fn()".   (Unless CsrSchedTimerCancel()
 *      is used to prevent delivery.)
 *
 *  NOTE
 *      The function will be called at or after "delay"; the actual delay will
 *      depend on the timing behaviour of the scheduler's tasks.
 *
 *  RETURNS
 *      CsrSchedTid - A timed event identifier, can be used in CsrSchedTimerCancel().
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
CsrSchedTid CsrSchedTimerSetStringLog(CsrTime delay,
    void (*fn)(CsrUint16 mi, void *mv),
    CsrUint16 fniarg,
    void *fnvarg,
    CsrUint32 line,
    const CsrCharString *file);
#ifdef CSR_LOG_INCLUDE_FILE_NAME_AND_LINE_NUMBER
#define CsrSchedTimerSet(d, fn, fni, fnv) CsrSchedTimerSetStringLog((d), (fn), (fni), (fnv), __LINE__, __FILE__)
#else
#define CsrSchedTimerSet(d, fn, fni, fnv) CsrSchedTimerSetStringLog((d), (fn), (fni), (fnv), 0, NULL)
#endif
#else
CsrSchedTid CsrSchedTimerSet(CsrTime delay,
    void (*fn)(CsrUint16 mi, void *mv),
    CsrUint16 fniarg,
    void *fnvarg);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedTimerCancel
 *
 *  DESCRIPTION
 *      Attempts to prevent the timed event with identifier "eventid" from
 *      occurring.
 *
 *  RETURNS
 *      CsrBool - TRUE if cancelled, FALSE if the event has already occurred.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_LOG_ENABLE
CsrBool CsrSchedTimerCancelStringLog(CsrSchedTid eventid,
    CsrUint16 *pmi,
    void **pmv,
    CsrUint32 line,
    const CsrCharString *file);
#ifdef CSR_LOG_INCLUDE_FILE_NAME_AND_LINE_NUMBER
#define CsrSchedTimerCancel(e, pmi, pmv) CsrSchedTimerCancelStringLog((e), (pmi), (pmv), __LINE__, __FILE__)
#else
#define CsrSchedTimerCancel(e, pmi, pmv) CsrSchedTimerCancelStringLog((e), (pmi), (pmv), 0, NULL)
#endif
#else
CsrBool CsrSchedTimerCancel(CsrSchedTid eventid,
    CsrUint16 *pmi,
    void **pmv);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedTaskQueueGet
 *
 *  DESCRIPTION
 *      Return the queue identifier for the currently running queue
 *
 *  RETURNS
 *      CsrSchedQid - The current task queue identifier, or 0xFFFF if not available.
 *
 *----------------------------------------------------------------------------*/
CsrSchedQid CsrSchedTaskQueueGet(void);

#ifdef __cplusplus
}
#endif

#endif
