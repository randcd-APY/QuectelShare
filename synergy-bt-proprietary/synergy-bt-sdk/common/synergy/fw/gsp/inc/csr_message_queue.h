#ifndef CSR_MESSAGE_QUEUE_H__
#define CSR_MESSAGE_QUEUE_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUMBER_OF_MESSAGES_IN_ONE_POOL    10

typedef struct
{
    CsrUint16 event;
    void     *message;
} CsrMessageQueueEntryType;

typedef struct CsrMessageQueueStructTag
{
    CsrUint16                        numberOfMessagesInThisPool;
    CsrUint16                        nextEntryNumber;
    struct CsrMessageQueueStructTag *nextQueuePool;
    CsrMessageQueueEntryType         messageQueueEntryArray[NUMBER_OF_MESSAGES_IN_ONE_POOL];
} CsrMessageQueueType;

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMessageQueuePush
 *
 *  DESCRIPTION
 *        This function will save a message (actually event + message) in an
 *        "internal" queue. The queue should be a part of the individual task'
 *        instance data (gash). The queue pointer should be initialized to NULL
 *        in the task init function. The task should call this function with the
 *        address of the pointer to the queue. The CsrMessageQueuePush function will then
 *        maintain the pointer properly. The task using these functions should NOT
 *        access the queue pointer directly, but only through the supplied
 *        functions. The queue can grow unrestricted, only limited by the memory
 *        pool.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
void CsrMessageQueuePush(CsrMessageQueueType **messageQueue, CsrUint16 event, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMessageQueueRestore
 *
 *  DESCRIPTION
 *        This function will restore the "saved" messages back on to the task'
 *        message queue. It will keep the order of the saved messages, BUT will
 *        NOT put the saved messages back in front of any pending messages on
 *        the task' queue. WARNING: Do only use this function if the order of the
 *        messages are irrelevant. If the order is important, use the CsrMessageQueuePop
 *        function to "pop" one message at the time of the save queue.
 *
 *        This function also manipulates the "messageQueue" pointer.
 *
 *  RETURNS
 *      The number of restored messages.
 *
 *----------------------------------------------------------------------------*/
CsrUint16 CsrMessageQueueRestore(CsrMessageQueueType **messageQueue, CsrSchedQid queue);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMessageQueuePop
 *
 *  DESCRIPTION
 *        This function will pop the first (if any) saved message of the queue.
 *
 *        If the order of the saved messages are significant, use this function
 *        to do a restore instead of the "CsrMessageQueueRestore" function. When doing
 *        this be careful! DO NOT POP AND PROCESS ALL THE MESSAGES AT ONCE! This
 *        can result in "hicups" in the scheduler. You are not allowed to do
 *        lengthy processing in a scheduler task. Instead do the following:
 *
 *            1) Send a message (house-cleaning) to yourself and set a popping
 *               flag in your task' data.
 *            2) As long as the CsrMessageQueuePop returns TRUE do NOT do a CsrMessageGet
 *               but keep processing the popped messages (one a the time, every
 *               time the scheduler calls the task function).
 *            3) When PopMessages returns FALSE, clear the popping flag and
 *               continue as usual. And discard the house-keeping message, when
 *               received.
 *
 *        This function also manipulates the "messageQueue" pointer.
 *
 *  RETURNS
 *      TRUE if a message was "popped", FALSE if no messages were available.
 *
 *----------------------------------------------------------------------------*/
CsrBool CsrMessageQueuePop(CsrMessageQueueType **messageQueue, CsrUint16 *event, void **message);

#ifdef __cplusplus
}
#endif

#endif /* CSR_MESSAGE_QUEUE_H__ */
