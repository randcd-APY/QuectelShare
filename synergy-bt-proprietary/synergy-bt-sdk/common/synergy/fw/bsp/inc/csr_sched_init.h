#ifndef CSR_SCHED_INIT_H__
#define CSR_SCHED_INIT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of supported segments */
#define CSR_SCHED_MAX_SEGMENTS  (15)

/* Start of external queue range */
#define CSR_SCHED_QUEUE_EXTERNAL_LOWEST  (0x7800)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedTaskInit
 *
 *  DESCRIPTION
 *      User-implemented function (usually in a file named tasks.c).
 *
 *  RETURNS
 *      <void>
 *
 *----------------------------------------------------------------------------*/
void CsrSchedTaskInit(void *data);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedInit
 *
 *  DESCRIPTION
 *      Prepare scheduler instance with identifier id. The scheduler instance
 *      will run in its own thread with given priority and stack size.
 *      The function returns the new scheduler instance. Priority and stack
 *      size values are passed unchanged to the CsrThreadCreate() call of the
 *      underlying Framework Extensions API.
 *
 *      Valid id range is 0 to CSR_SCHED_MAX_SEGMENTS - 1
 *
 *      Valid priority and stack size values are determined be the Framework
 *      Extensions API.
 *
 *  RETURNS
 *      The scheduler instance
 *
 *----------------------------------------------------------------------------*/
void *CsrSchedInit(CsrUint16 id, CsrUint16 priority, CsrUint32 stackSize);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedDeinit
 *
 *  DESCRIPTION
 *      Free scheduler. Parameter is the scheduler instance.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrSchedDeinit(void *data);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedRegisterTask
 *
 *  DESCRIPTION
 *      Register a task with the scheduler.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrSchedRegisterTask(CsrSchedQid *queue,
    schedEntryFunction_t init,
    schedEntryFunction_t deinit,
    schedEntryFunction_t handler,
    const CsrCharString *task_id_ver,
    void *data,
    CsrUint16 id);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSched
 *
 *  DESCRIPTION
 *      Run the scheduler
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrSched(void *data);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedStop
 *
 *  DESCRIPTION
 *      Stop scheduler when it's running. NB: No context is given, as
 *      function must be called from within a task.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrSchedStop(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSchedRegisterExternalSend
 *
 *  DESCRIPTION
 *      Register function for external communication.
 *
 *      Valid external queue range starts at CSR_SCHED_QUEUE_EXTERNAL_LOWEST.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrSchedRegisterExternalSend(CsrBool (*f)(CsrSchedQid q, CsrUint16 mi, void *mv));


#ifdef __cplusplus
}
#endif

#endif /* CSR_SCHED_INIT_H__ */
