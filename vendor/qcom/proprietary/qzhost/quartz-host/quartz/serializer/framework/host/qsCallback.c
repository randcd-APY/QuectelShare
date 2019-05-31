/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#if CONFIG_DAEMON_MODE

#include "qsOSAL.h"
#include "qsCommon.h"
#include "qsHost.h"
#include "qsPack.h"
#include "qsConfig.h"
#include "qsCallback.h"

#define CALLBACK_LIST_INITIAL_SIZE                                      (64)
#define CALLBACK_THREAD_STACK_SIZE                                      (16*1024)
#define CALLBACK_MAILBOX_NUM_SLOTS                                      (50)
#define CALLBACK_SHUTDOWN_TIMEOUT                                       (3000)
#define CALLBACK_MAILBOX_TIMEOUT                                        (3000)

typedef struct CallbackListEntry_s
{
    CallbackParser_t Parser;
    CallbackInfo_t   Info;
} CallbackListEntry_t;

typedef struct CallbackContext_s
{
    Mutex_t              Mutex;
    Mutex_t              LockMutex;
    CallbackListEntry_t *CallbackList;
    uint32_t             CallbackListSize;
    Mailbox_t            Mailbox;
    Thread_t             Thread;
    Event_t              ShutdownEvent;
} CallbackContext_t;

#define CALLBACK_LIST_ALLOC_SIZE(__size__)   (sizeof(CallbackListEntry_t) * (__size__))

typedef enum
{
    CALLBACK_MESSAGE_SHUTDOWN,
    CALLBACK_MESSAGE_BUFFER_AVAILABLE
} CallbackMessage_t;

typedef struct Callback_Mailbox_Message_s
{
    CallbackMessage_t  MessageType;
    uint8_t            ModuleID;
    uint8_t            FileID;
    uint16_t           EventID;
    uint32_t           BufferLength;
    uint8_t           *Buffer;
} Callback_Mailbox_Message_t;

#define CALLBACK_MAILBOX_MESSAGE_SIZE        (sizeof(Callback_Mailbox_Message_t))

static CallbackContext_t *CallbackContext;

static SerStatus_t Callback_ResizeCallbackList(void);
static void *Callback_Thread(void *ThreadParameter);
static void Callback_Mailbox_Delete_Callback(void *MailboxData);

/**
  Resizes the parser callback list.

  @return the status result for resizing the list.
  */
static SerStatus_t Callback_ResizeCallbackList(void)
{
    uint32_t             NewSize;
    SerStatus_t          RetVal;
    CallbackListEntry_t *NewList;

    /* Double the size of the callback list. */
    NewSize = CallbackContext->CallbackListSize * 2;

#if 0
    NewList = QSOSAL_Realloc(CallbackContext->CallbackList, CALLBACK_LIST_ALLOC_SIZE(NewSize));
#else
    NewList = QSOSAL_AllocateMemory(CALLBACK_LIST_ALLOC_SIZE(NewSize));
#endif

    if(NewList != NULL)
    {
        /* Copy the old list.*/
        QSOSAL_MemCopy_S(NewList, CALLBACK_LIST_ALLOC_SIZE(NewSize), CallbackContext->CallbackList, CALLBACK_LIST_ALLOC_SIZE(CallbackContext->CallbackListSize));

        /* Initialize the new portion of the list. */
        QSOSAL_MemInitialize(NewList + CallbackContext->CallbackListSize, 0, CALLBACK_LIST_ALLOC_SIZE(NewSize - CallbackContext->CallbackListSize));

        /* Free the old list. */
        QSOSAL_FreeMemory(CallbackContext->CallbackList);

        /* Point to the new list and update the size. */
        CallbackContext->CallbackList     = NewList;
        CallbackContext->CallbackListSize = NewSize;

        RetVal = ssSuccess;
    }
    else
    {
        RetVal = ssAllocationError;
    }

    return(RetVal);
}

/* Handles mailbox messages for callbacks, allowing events to be     */
/* executed on a separate thread.                                    */
static void *Callback_Thread(void *ThreadParameter)
{
    uint32_t                   Index;
    Callback_Mailbox_Message_t MailboxMessage;
    CallbackListEntry_t        CallbackListEntry;
    Boolean_t                  MutexHeld;
    Boolean_t                  ShutdownThread;

    ShutdownThread = FALSE;
    while(!ShutdownThread)
    {
        /* Block on the mailbox. */
        if(QSOSAL_WaitMailbox(CallbackContext->Mailbox, &MailboxMessage))
        {
            switch(MailboxMessage.MessageType)
            {
                case CALLBACK_MESSAGE_BUFFER_AVAILABLE:
                    /* Verify the module is initialized and the parameters are valid. */
                    if(MailboxMessage.Buffer != NULL)
                    {
                        if(QSOSAL_WaitMutex(CallbackContext->LockMutex, QSOSAL_INFINITE_WAIT))
                        {
                            if(QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT))
                            {
                                MutexHeld = TRUE;

                                /* Find the callback. */
                                for(Index = 0; Index < CallbackContext->CallbackListSize; Index++)
                                {
                                    if((CallbackContext->CallbackList[Index].Info.FileID == MailboxMessage.FileID) && (CallbackContext->CallbackList[Index].Info.CallbackID == MailboxMessage.EventID))
                                    {
                                        /* Copy the entry. */
                                        QSOSAL_MemCopy(&CallbackListEntry, &CallbackContext->CallbackList[Index], sizeof(CallbackListEntry_t));

                                        /* Release the mutex. */
                                        QSOSAL_ReleaseMutex(CallbackContext->Mutex);
                                        MutexHeld = FALSE;

                                        /* Call the parser function. */
                                        (*(CallbackListEntry.Parser))(Index, &(CallbackListEntry.Info), MailboxMessage.BufferLength, (uint8_t *)MailboxMessage.Buffer);

                                        break;
                                    }
                                }

                                if(MutexHeld)
                                {
                                    /* Release the mutex. */
                                    QSOSAL_ReleaseMutex(CallbackContext->Mutex);
                                }
                            }

                            QSOSAL_ReleaseMutex(CallbackContext->LockMutex);
                        }

                        QSOSAL_FreeMemory(MailboxMessage.Buffer);
                    }
                    break;
                case CALLBACK_MESSAGE_SHUTDOWN:
                    ShutdownThread = TRUE;
                    break;
            }
        }
    }

    /* Signal that we have shut down the thread. */
    QSOSAL_SetEvent(CallbackContext->ShutdownEvent);

    return(NULL);
}

/* Frees the memory of waiting mailbox message upon mailbox deletion. */
static void Callback_Mailbox_Delete_Callback(void *MailboxData)
{
    Callback_Mailbox_Message_t *MailboxMessage;

    if(MailboxData)
    {
        MailboxMessage = (Callback_Mailbox_Message_t *)MailboxData;

        switch(MailboxMessage->MessageType)
        {
            case CALLBACK_MESSAGE_BUFFER_AVAILABLE:
                /* Free the buffer associated with this message. */
                if(MailboxMessage->Buffer)
                {
                    QSOSAL_FreeMemory(MailboxMessage->Buffer);
                }
                break;
            default:
                break;
        }
    }
}

/**
  Initiializes the parser module.

  @return the status result of the parser initialization.
  */
SerStatus_t Callback_Initialize(void)
{
    SerStatus_t RetVal;

    if(CallbackContext == NULL)
    {
        /* Allocate the callback context. */
        CallbackContext = QSOSAL_AllocateMemory(sizeof(CallbackContext_t));
        if(CallbackContext != NULL)
        {
            /* Create the mutex for the callback module. */
            CallbackContext->Mutex     = QSOSAL_CreateMutex(FALSE);
            CallbackContext->LockMutex = QSOSAL_CreateMutex(FALSE);
            if((CallbackContext->Mutex != NULL) && (CallbackContext->LockMutex != NULL))
            {
                /* Allocate the initial callback list. */
                CallbackContext->CallbackListSize = CALLBACK_LIST_INITIAL_SIZE;
                CallbackContext->CallbackList     = (CallbackListEntry_t *)QSOSAL_AllocateMemory(CALLBACK_LIST_ALLOC_SIZE(CallbackContext->CallbackListSize));

                if(CallbackContext->CallbackList != NULL)
                {
                    /* Initialize the callback list. */
                    QSOSAL_MemInitialize(CallbackContext->CallbackList, 0, CALLBACK_LIST_ALLOC_SIZE(CallbackContext->CallbackListSize));

                    /* Create the shutdown event. */
                    if((CallbackContext->ShutdownEvent = QSOSAL_CreateEvent(FALSE)))
                    {
                        /* Create the mailbox. */
                        if((CallbackContext->Mailbox = QSOSAL_CreateMailbox(CALLBACK_MAILBOX_NUM_SLOTS, CALLBACK_MAILBOX_MESSAGE_SIZE)) != NULL)
                        {
                            /* Create the mailbox thread. */
                            if((CallbackContext->Thread = QSOSAL_CreateThread(Callback_Thread, CALLBACK_THREAD_STACK_SIZE, NULL)) != NULL)
                            {
                                /* Set the module callbacks if the module is initialized. */
#ifdef QS_WPAN_INIT_BLE_THREAD
                                if(QsSetModuleCb(QS_MODULE_BLE, (void *)Callback_Process) == ssSuccess)
                                {
#endif
#ifdef QS_WPAN_INIT_HMI_THREAD
                                    if(QsSetModuleCb(QS_MODULE_HMI, (void *)Callback_Process) == ssSuccess)
                                    {
#endif
#ifdef QS_WPAN_INIT_TWN_THREAD
                                        if(QsSetModuleCb(QS_MODULE_THREAD, (void *)Callback_Process) == ssSuccess)
                                        {
#endif
#ifdef QS_WPAN_INIT_ZB_THREAD
                                            if(QsSetModuleCb(QS_MODULE_ZIGBEE, (void *)Callback_Process) == ssSuccess)
                                            {
#endif
#ifdef QS_WPAN_INIT_HERH_THREAD
                                                if(QsSetModuleCb(QS_MODULE_HERH, (void *)Callback_Process) == ssSuccess)
                                                {
#endif
#ifdef QS_WPAN_INIT_COEX_THREAD
                                                    if(QsSetModuleCb(QS_MODULE_COEX, (void *)Callback_Process) == ssSuccess)
                                                    {
#endif
                                                        RetVal = ssSuccess;

#ifdef QS_WPAN_INIT_COEX_THREAD
                                                    }
                                                    else
                                                        RetVal = ssFailure;
#endif
#ifdef QS_WPAN_INIT_HERH_THREAD
                                                }
                                                else
                                                    RetVal = ssFailure;
#endif
#ifdef QS_WPAN_INIT_ZB_THREAD
                                            }
                                            else
                                                RetVal = ssFailure;
#endif
#ifdef QS_WPAN_INIT_TWN_THREAD
                                        }
                                        else
                                            RetVal = ssFailure;
#endif
#ifdef QS_WPAN_INIT_HMI_THREAD
                                    }
                                    else
                                        RetVal = ssFailure;
#endif
#ifdef QS_WPAN_INIT_BLE_THREAD
                                }
                                else
                                    RetVal = ssFailure;
#endif
                            }
                            else
                            {
                                RetVal = ssAllocationError;
                            }
                        }
                        else
                        {
                            RetVal = ssAllocationError;
                        }
                    }
                    else
                    {
                        RetVal = ssAllocationError;
                    }
                }
                else
                {
                    RetVal = ssAllocationError;
                }
            }
            else
            {
                RetVal = ssAllocationError;
            }
        }
        else
        {
            RetVal = ssAllocationError;
        }

        if(RetVal != ssSuccess)
        {
            /* Clean up all the resources. */
            Callback_Shutdown();
        }
    }
    else
    {
        RetVal = ssFailure;
    }

    return(RetVal);
}

/**
  Shuts down the parser module.
  */
void Callback_Shutdown(void)
{
    Callback_Mailbox_Message_t MailboxMessage;

    if(CallbackContext != NULL)
    {
        if(CallbackContext->ShutdownEvent)
        {
            if(CallbackContext->Mailbox)
            {
                if(CallbackContext->Thread)
                {
                    /* Send a shutdown message and wait for the thread to close. */
                    MailboxMessage.MessageType = CALLBACK_MESSAGE_SHUTDOWN;
                    QSOSAL_AddMailbox(CallbackContext->Mailbox, &MailboxMessage);
                    QSOSAL_WaitEvent(CallbackContext->ShutdownEvent, CALLBACK_SHUTDOWN_TIMEOUT);
                }

                QSOSAL_DeleteMailbox(CallbackContext->Mailbox, Callback_Mailbox_Delete_Callback);
            }

            QSOSAL_CloseEvent(CallbackContext->ShutdownEvent);
        }

        /* Cleanup the lock mutex. */
        if(CallbackContext->LockMutex != NULL)
        {
            QSOSAL_CloseMutex(CallbackContext->LockMutex);
        }

        /* Cleanup the other resources. */
        if(CallbackContext->Mutex != NULL)
        {
            QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT);

            /* Free the callback list. */
            if (CallbackContext->CallbackList)
            {
                QSOSAL_FreeMemory(CallbackContext->CallbackList);
                CallbackContext->CallbackList = NULL;
            }

            /* Delete the mutex. */
            QSOSAL_ReleaseMutex(CallbackContext->Mutex);
            QSOSAL_CloseMutex(CallbackContext->Mutex);
        }

        /* Free the context. */
        QSOSAL_FreeMemory(CallbackContext);
        CallbackContext = NULL;
    }
}

/**
  Registers a callback function with the provided information.

  @param CallbackHandle Pointer to where the callback handle will be stored upon
  successful return.
  @param CallbackParser Function that will handle parsing the callback.
  @param CallbackInfo   Information for the callback to be registered.  The
  ModuleID, FileID, CallbackID, and AppFunction must all be valid.

  @return ssSuccess if the callback was registered successfully or a negative
  value if there was an error.
  */
SerStatus_t Callback_Register(uint32_t *CallbackHandle, CallbackParser_t CallbackParser, const CallbackInfo_t *CallbackInfo)
{
    uint32_t    Index;
    SerStatus_t RetVal = ssSuccess;

    /* Initialize the module if it hasn't been initialized yet. */
    if (CallbackContext == NULL)
        Callback_Initialize();

    /* Verify the module is initialized and the parameters are valid. */
    if((CallbackContext != NULL) && (CallbackHandle != NULL) && (CallbackParser != NULL) && (CallbackInfo != NULL) && (CallbackInfo->ModuleID >= QS_MODULE_WLAN) && (CallbackInfo->ModuleID <= QS_MODULE_HERH) && (CallbackInfo->AppFunction != NULL))
    {
        if(QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Search for an available entry in the callback list. */
            for(Index = 0; Index < CallbackContext->CallbackListSize; Index ++)
            {
                /* If this callback entry has already been registered.*/
                if((CallbackContext->CallbackList[Index].Info.TargetID == CallbackInfo->TargetID) &&
                   (CallbackContext->CallbackList[Index].Info.ModuleID == CallbackInfo->ModuleID) &&
                        (CallbackContext->CallbackList[Index].Info.FileID == CallbackInfo->FileID) &&
                        (CallbackContext->CallbackList[Index].Info.CallbackID == CallbackInfo->CallbackID) &&
                        (CallbackContext->CallbackList[Index].Info.CallbackKey == CallbackInfo->CallbackKey))
                {
                    /* Return the handle to the caller.*/
                    *CallbackHandle = Index;
                    break;
                }
                else
                {
                    /* If a callback entry is available. */
                    if(CallbackContext->CallbackList[Index].Parser == NULL)
                    {
                        /* Store the callback entry. */
                        CallbackContext->CallbackList[Index].Parser = CallbackParser;
                        QSOSAL_MemCopy_S(&(CallbackContext->CallbackList[Index].Info), sizeof(CallbackInfo_t), CallbackInfo, sizeof(CallbackInfo_t));

                        /* Return the handle to the caller. */
                        *CallbackHandle = Index;
                        break;
                    }
                }
            }

            /* If we could not add the callback entry because the list is full. */
            if(Index == CallbackContext->CallbackListSize)
            {
                /* Resize the list so we can add the callback entry. */
                RetVal = Callback_ResizeCallbackList();

                /* If an error has not occured. */
                if(RetVal == ssSuccess)
                {
                    /* Store the callback entry. */
                    CallbackContext->CallbackList[Index].Parser = CallbackParser;
                    QSOSAL_MemCopy_S(&(CallbackContext->CallbackList[Index].Info), sizeof(CallbackInfo_t), CallbackInfo, sizeof(CallbackInfo_t));

                    /* Return the handle to the caller. */
                    *CallbackHandle = Index;
                }
            }

            QSOSAL_ReleaseMutex(CallbackContext->Mutex);
        }
        else
        {
            RetVal = ssFailure;
        }
    }
    else
    {
        RetVal = ssInvalidParameter;
    }

    return(RetVal);
}

/**
  Updates the callback key for a registered callback function.

  @param CallbackHandle Callback handle returned from successful registration.
  @param CallbackKey    The new value for the callback key.

  @return ssSuccess if the callback was found successfully or a negative value
  if there was an error.
  */
SerStatus_t Callback_UpdateKey(uint32_t CallbackHandle, uint32_t CallbackKey)
{
    SerStatus_t RetVal;

    /* Verify the module is initialized and the parameters are valid. */
    if((CallbackContext != NULL) && (CallbackHandle < CallbackContext->CallbackListSize))
    {
        if(QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Make sure the callback ID is in use. */
            if(CallbackContext->CallbackList[CallbackHandle].Parser != NULL)
            {
                CallbackContext->CallbackList[CallbackHandle].Info.CallbackKey = CallbackKey;

                RetVal = ssSuccess;
            }
            else
            {
                RetVal = ssInvalidParameter;
            }

            QSOSAL_ReleaseMutex(CallbackContext->Mutex);
        }
        else
        {
            RetVal = ssFailure;
        }
    }
    else
    {
        RetVal = ssInvalidParameter;
    }

    return(RetVal);
}

/**
  Retrieves the callback info for the specified handle.

  @param CallbackHandle Callback handle returned from successful registration.
  @param CallbackInfo   Information for the callback.

  @return ssSuccess if the callback was found successfully or a negative value
  if there was an error.
  */
SerStatus_t Callback_Retrieve(uint32_t CallbackHandle, CallbackInfo_t *CallbackInfo)
{
    SerStatus_t RetVal;

    /* Verify the module is initialized and the parameters are valid. */
    if((CallbackContext != NULL) && (CallbackHandle < CallbackContext->CallbackListSize))
    {
        if(QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Make sure the callback ID is in use. */
            if(CallbackContext->CallbackList[CallbackHandle].Parser != NULL)
            {
                /* Copy the callback info. */
                QSOSAL_MemCopy_S(CallbackInfo, sizeof(CallbackInfo_t), &(CallbackContext->CallbackList[CallbackHandle].Info), sizeof(CallbackInfo_t));
                RetVal = ssSuccess;
            }
            else
            {
                RetVal = ssInvalidParameter;
            }

            QSOSAL_ReleaseMutex(CallbackContext->Mutex);
        }
        else
        {
            RetVal = ssFailure;
        }
    }
    else
    {
        RetVal = ssInvalidParameter;
    }

    return(RetVal);
}

/**
  Unregisters a callback function based on the callback key and identifiers.

  @param ModuleID    ID of the callback's module to unregister.
  @param FileID      ID of the callback's file to unregister.
  @param CallbackID  ID of the callback's type to unregister.
  @param CallbackKey Key specific to the callback to unregister.
  */
void Callback_UnregisterByKey(uint16_t ModuleID, uint16_t FileID, uint16_t CallbackID, uint32_t CallbackKey)
{
    uint32_t Index;

    /* Verify the module is initialized and the parameters are valid. */
    if(CallbackContext != NULL)
    {
        if(QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Search for the entry in the callback list. */
            for(Index = 0; Index < CallbackContext->CallbackListSize; Index ++)
            {
                /* Match to the given parameters. */
                if((CallbackContext->CallbackList[Index].Info.ModuleID == ModuleID) && \
                        (CallbackContext->CallbackList[Index].Info.FileID == FileID) && \
                        (CallbackContext->CallbackList[Index].Info.CallbackID == CallbackID) && \
                        (CallbackContext->CallbackList[Index].Info.CallbackKey == CallbackKey))
                {
                    /* Found an available entry. */
                    break;
                }
            }

            if(Index < CallbackContext->CallbackListSize)
            {
                /* Memset the entry to unregister it. */
                QSOSAL_MemInitialize(&(CallbackContext->CallbackList[Index]), 0, sizeof(CallbackListEntry_t));
            }

            QSOSAL_ReleaseMutex(CallbackContext->Mutex);
        }
    }
}

/**
  Unregisters a callback function based on the CallbackHandle returned from
  Callback_Register().

  @param CallbackHandle Handle of the callback to unregister.
  */
void Callback_UnregisterByHandle(uint32_t CallbackHandle)
{
    /* Verify the module is initialized and the parameters are valid. */
    if((CallbackContext != NULL) && (CallbackHandle < CallbackContext->CallbackListSize))
    {
        if(QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Entry found, memset it to unregister it. */
            QSOSAL_MemInitialize(&(CallbackContext->CallbackList[CallbackHandle]), 0, sizeof(CallbackListEntry_t));

            QSOSAL_ReleaseMutex(CallbackContext->Mutex);
        }
    }
}

/**
  Unregisters callback functions based on the module. All callbacks
  registered for the module and instance will be unregistered.

  @param ModuleID ID of the module of the callbacks to unregister.
  */
void Callback_UnregisterByModule(uint16_t ModuleID)
{
    uint32_t Index;

    /* Verify the module is initialized and the parameters are valid. */
    if(CallbackContext != NULL)
    {
        if(QSOSAL_WaitMutex(CallbackContext->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Search the list for an allocated entry that matches the provided
               information. */
            for(Index = 0; Index < CallbackContext->CallbackListSize; Index ++)
            {
                if((CallbackContext->CallbackList[Index].Parser != NULL) &&
                        (CallbackContext->CallbackList[Index].Info.ModuleID == ModuleID))
                {
                    /* Entry found, memset it to unregister it. */
                    QSOSAL_MemInitialize(&(CallbackContext->CallbackList[Index]), 0, sizeof(CallbackListEntry_t));
                }
            }

            QSOSAL_ReleaseMutex(CallbackContext->Mutex);
        }
    }
}

/**
   Processes callback data received from a remote device. It will invoke the
   correct parser for the callback function.

   @param ModuleID     ID of the callback's module to unregister.
   @param FileID       ID of the callback's file to unregister.
   @param EventID      ID of the callback's event.
   @param BufferLegnth Length of the buffer that was received.
   @param Buffer       Data that was received.  Note that it is expected that
                       the callback handle is packed into the beginning of this
                       buffer.
*/
void Callback_Process(uint8_t ModuleID, uint8_t FileID, uint16_t EventID, uint32_t BufferLength, uint8_t *Buffer)
{
    Callback_Mailbox_Message_t MailboxMessage;

    /* Allocate a buffer to hold the data. */
    MailboxMessage.Buffer = (uint8_t *)QSOSAL_AllocateMemory(BufferLength);

    if (MailboxMessage.Buffer)
    {
        /* Copy the data to the buffer.*/
        QSOSAL_MemCopy(MailboxMessage.Buffer, Buffer, BufferLength);

        MailboxMessage.MessageType  = CALLBACK_MESSAGE_BUFFER_AVAILABLE;
        MailboxMessage.ModuleID     = ModuleID;
        MailboxMessage.FileID       = FileID;
        MailboxMessage.EventID      = EventID;
        MailboxMessage.BufferLength = BufferLength;

        /* Add the message to the mailbox. */
        if(!QSOSAL_AddMailbox(CallbackContext->Mailbox, &MailboxMessage))
        {
            printf("mbox fail\n");
            /* Free the buffer if the mailbox failed. */
            QSOSAL_FreeMemory(MailboxMessage.Buffer);
        }
    }
}

/**
   Locks the callback processing preventing any future callbacks from being
   issued to the module until the unlocked.

   Note, this function may prevent callbacks to all modules, not just the
   specified module.

   @param ModuleID ID of the callback's module to lock.

   @return True if the lock was aquired successfully or false if there was an
           error.
*/
Boolean_t Callback_Lock(uint8_t ModuleID)
{
    return(QSOSAL_WaitMutex(CallbackContext->LockMutex, QSOSAL_INFINITE_WAIT));
}

/**
   Unlocks callback processing for a module.

   @param ModuleID ID of the callback's module to unlock.
*/
void Callback_Unlock(uint8_t ModuleID)
{
    QSOSAL_ReleaseMutex(CallbackContext->LockMutex);
}

#endif /* CONFIG_DAEMON_MODE */

