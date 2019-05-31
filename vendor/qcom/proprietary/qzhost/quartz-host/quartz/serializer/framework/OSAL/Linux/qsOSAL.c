/*****< QSOSALkrnl.c >*********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  QSOSALKRNL - Stonestreet One Bluetooth Stack Kernel Implementation.       */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   05/30/01  D. Lange       Initial creation.                               */
/******************************************************************************/
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <unistd.h>        /* Included for POSIX version verification.        */
#include <stdlib.h>        /* Included for malloc and free                    */
#include <string.h>        /* Included for memcpy, memset, strcpy and strlen  */
#include <strings.h>       /* Included for strncasecmp function.              */
#include <pthread.h>       /* Included for thread and mutex related functions.*/
#include <semaphore.h>     /* Included for semaphore related functions.       */
#include <time.h>          /* Included for nanosleep.                         */
#include <sys/time.h>      /* Included for accessing system time.             */
#include <signal.h>        /* Included for signals handling.                  */
#include <errno.h>         /* Included for global errno variable.             */
#include <stdarg.h>        /* Included for variable argument handling.        */
#include <limits.h>        /* Included for primitive range constants.         */
#include <stdint.h>        /* Included for additional integer types.          */

#include "qsOSAL.h"          /* QSOSAL Prototypes/Constants.            */

/* The following constant allows QSOSAL Kernel safe function         */
/* implementations to use CRT functions.                             */
/* * NOTE * If QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION is defined*/
/*          and set to 1, then the QSOSAL Kernel safe functions will */
/*          use an implementation that uses CRT functions. If NOT    */
/*          defined or set to 0, then the QSOSAL Kernel safe         */
/*          functions will use an implementation without CRT         */
/*          functions. This functionality has been primarily provided*/
/*          since static analysis will flag some CRT functions as    */
/*          banned functions.                                        */
#ifndef QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION
#define QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION     1
#endif

/* The following constant defines the maximum number of characters   */
/* that may be output by the QSOSAL_OutputMessage() function.        */
#ifndef QSOSAL_KERNEL_MAX_OUTPUT_MESSAGE_LENGTH
#define QSOSAL_KERNEL_MAX_OUTPUT_MESSAGE_LENGTH         127
#endif

/* The following constant specifies the maximum number of kernel     */
/* objects which can be created simultaneously.                      */
#ifndef QSOSAL_KERNEL_MAX_OBJECTS

#define QSOSAL_KERNEL_MAX_OBJECTS                       1024

#endif

/* The following type declartion represents the entire state         */
/* information for a Mutex type.  This stucture is used with all of  */
/* the Mutex functions contained in this module.                     */
typedef struct _tagMutexHeader_t
{
    sem_t          Lock;
    sem_t          Semaphore;
    ThreadHandle_t ThreadHandle;
    unsigned int   Count;
} MutexHeader_t;

/* The following type declaration represents the entire state        */
/* information for an Event_t.  This structure is used with all of   */
/* the Event functions contained in this module.                     */
typedef struct _tagEventHeader_t
{
    Mutex_t      Mutex;
    Boolean_t    IsSet;
    sem_t        Semaphore;
    unsigned int Count;
} EventHeader_t;

/* The following type declaration represents the entire state        */
/* information for a Mailbox.  This structure is used with all of    */
/* the Mailbox functions contained in this module.                   */
typedef struct _tagMailboxHeader_t
{
    Event_t       Event;
    Mutex_t       Mutex;
    unsigned int  HeadSlot;
    unsigned int  TailSlot;
    unsigned int  OccupiedSlots;
    unsigned int  NumberSlots;
    unsigned int  SlotSize;
    void         *Slots;
} MailboxHeader_t;

/* The following type declaration represents the storage mechanism   */
/* for other kernel object types (Mutex_t, Event_t, etc).            */
typedef struct _tagObject_t
{
    int        Users;
    uintptr_t  Handle;
    void      *Object;
} Object_t;

static unsigned long DebugZoneMask = DEBUG_ZONES;

/* The following array is used to store active kernel objects.       */
static Object_t ObjectList[QSOSAL_KERNEL_MAX_OBJECTS];

/* The following constant represents the number of bytes that are    */
/* displayed on an individual line when using the DumpData()         */
/* function.                                                         */
#define MAXIMUM_BYTES_PER_ROW                  (16)

/* Internal Function Prototypes.                                     */
static uint8_t ConsoleWrite(char *Message, int Length);
static int GetCurrentTime(struct timespec *Time);
static int TimespecCompare(struct timespec *Time1, struct timespec *Time2);
static void TimespecIncrement(struct timespec *Time, unsigned int Milliseconds);
static Boolean_t WaitForSemaphore(sem_t *Semaphore, struct timespec *WaitLimit);

static uintptr_t StoreObject(void *Object);
static void *RetrieveObject(uintptr_t Handle);
static void ReleaseObject(uintptr_t Handle);
static void *ClearObject(uintptr_t Handle);
static Boolean_t ObjectSlotInUse(uintptr_t Handle);

static Boolean_t ReleaseMutex(MutexHeader_t *MutexHeader);

/* The following function is used to send a string of characters to  */
/* the Console or Output device.  The function takes as its first    */
/* parameter a pointer to a string of characters to be output.  The  */
/* second parameter indicates the number of bytes in the character   */
/* string that is to be output.                                      */
static uint8_t ConsoleWrite(char *Message, int Length)
{
    /* Check to make sure that the Debug String appears to be semi-valid.*/
    if((Message) && (Length))
        printf("%s", Message);

    return(0);
}

/* Obtain the current system time and store in the specified timespec*/
/* structure.  Returns zero upon success.                            */
static int GetCurrentTime(struct timespec *Time)
{
    int            ret_val;
    struct timeval CurrentTime;

    if(Time)
    {
        if(!gettimeofday(&CurrentTime, NULL))
        {
            Time->tv_sec  = CurrentTime.tv_sec;
            Time->tv_nsec = (CurrentTime.tv_usec * 1000L);

            ret_val = 0;
        }
        else
            ret_val = -1;
    }
    else
        ret_val = -1;

    return(ret_val);
}

/* Compare two timespec structures.  Both structures must be defined */
/* such that both tv_sec and tv_nsec are positive and 0 <= tv_nsec < */
/* 1000000000L.  Returns -1, 0, or 1 if (Time1 < Time2), (Time1 ==   */
/* Time2), or (Time1 > Time2), respectively.                         */
static int TimespecCompare(struct timespec *Time1, struct timespec *Time2)
{
    int ret_val;

    if((Time1) && (Time2))
    {
        if(Time1->tv_sec < Time2->tv_sec)
            ret_val = -1;
        else
        {
            if(Time1->tv_sec > Time2->tv_sec)
                ret_val = 1;
            else
            {
                /* Time1 and Time2 seconds are equal, so decide comparison  */
                /* based on the nanosecond field.                           */
                if(Time1->tv_nsec < Time2->tv_nsec)
                    ret_val = -1;
                else
                {
                    if(Time1->tv_nsec > Time2->tv_nsec)
                        ret_val = 1;
                    else
                        ret_val = 0;
                }
            }
        }
    }
    else
        ret_val = 0;

    return(ret_val);
}

/* Increment a timespec by a given number of milliseconds.  The      */
/* timespec will be updated in-place and will be normalized (0 <=    */
/* tv_nsec < 1000000000L).                                           */
static void TimespecIncrement(struct timespec *Time, unsigned int Milliseconds)
{
    if(Time)
    {
        Time->tv_sec  += (Milliseconds / 1000);
        Time->tv_nsec += ((Milliseconds % 1000) * 1000000L);

        if(Time->tv_nsec > 1000000000L)
        {
            Time->tv_sec  += 1;
            Time->tv_nsec -= 1000000000L;
        }
    }
}

/* Wait for a semaphore to become available.  The second parameter is*/
/* an absolute time at which to stop waiting, defined with respect to*/
/* the Epoch.  If the wait limit is NULL, wait indefinitely.         */
static Boolean_t WaitForSemaphore(sem_t *Semaphore, struct timespec *WaitLimit)
{
    int             Result;
    sigset_t        SigSet;
    Boolean_t       Done;
    Boolean_t       ret_val;
    struct timespec CurrentTime;

    /* Note that Linux can return prematurely from system calls.  This   */
    /* occurs especially when using the gdb debugger.  In these cases the*/
    /* return code should be EINTR meaning the call should be handled by */
    /* higher layers.  However, our higher layers don't handle restarts, */
    /* so this code instead will retry the call internally.              */

    if(Semaphore)
    {
        Done = FALSE;

        /* Note that Linux can return prematurely from system calls.  This*/
        /* occurs especially when using the gdb debugger.  In these cases */
        /* the return code should be EINTR meaning the call should be     */
        /* handled by higher layers.  However, our higher layers don't    */
        /* handle restarts, so this code instead will retry the call      */
        /* internally.                                                    */
        while(!Done)
        {
            if(!WaitLimit)
            {
                /* We are supposed to wait forever, Now pend on the event   */
                /* semaphore until a set event occurs.                      */
                Result = sem_wait(Semaphore);
            }
            else
            {
                /* We are supposed to wait only until the specified time.   */

#if ((_POSIX_VERSION >= 200112L) && ((_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600)))

                /* The timed-wait API is available, which we will prefer    */
                /* over the older try-wait API due it the high timing       */
                /* accuracy offered by timed-wait.                          */
                Result = sem_timedwait(Semaphore, WaitLimit);

#else
#warning Using legacy mutex wait mechanism

                /* Only the try-wait API is available for performing a      */
                /* non-blocking wait on the semaphor.  This function returns*/
                /* immediately if the semaphore is unavailable, so we       */
                /* will simulate the timeout by checking the semaphor       */
                /* periodically.                                            */
                Result = sem_trywait(Semaphore);

#endif

            }

            if(!Result)
            {
                /* Semaphore acquired successfully.                         */
                Done    = TRUE;
                ret_val = TRUE;
            }
            else
            {
                switch(errno)
                {
                    case EINTR:
                        /* The call was interrupted by a signal handler.      */
                        sigpending(&SigSet);

                        /* The TRAP and PROFILE signals are used in debugging,*/
                        /* so ignore them.  For any other signal, go ahead and*/
                        /* quit.                                              */
                        if(!((sigismember(&SigSet, SIGTRAP)) || (sigismember(&SigSet, SIGPROF))))
                        {
                            Done    = TRUE;
                            ret_val = FALSE;
                        }

                        break;
                    case EAGAIN:
                        /* The operation could not be performed without       */
                        /* blocking (i.e., sem_trywait could not immediately  */
                        /* acquire the semaphore).  Check whether we have time*/
                        /* to try again.                                      */
                        GetCurrentTime(&CurrentTime);

                        if(TimespecCompare(&CurrentTime, WaitLimit) < 0)
                        {
                            /* We were unable to get the semaphore, and we     */
                            /* have not run out of time to acquire it, so      */
                            /* sleep for the shortest time possible (1 or more */
                            /* milliseconds) and try again.                    */
                            QSOSAL_Delay(1);
                        }
                        break;
                    case EINVAL:
                    case ETIMEDOUT:
                    default:
                        /* Either (EINVAL) the semaphore is invalid and/or    */
                        /* the wait limit is not within the valid range or    */
                        /* (ETIMEDOUT) the call timed out before the semaphore*/
                        /* could be locked.  In either case, break the loop   */
                        /* and flag an error.                                 */
                        Done    = TRUE;
                        ret_val = FALSE;

                        break;
                }
            }
        }
    }
    else
        ret_val = FALSE;

    return(ret_val);
}

/* The following function is provided to allow a mechanism to store a*/
/* kernel object in the global object list in a thread-safe manner.  */
/* This function returns a handle which can be used to access the    */
/* object if the object was successfully stored, or zero if no       */
/* available storage slots were be found.                            */
static uintptr_t StoreObject(void *Object)
{
    static uintptr_t NextHandle = 0;

    Boolean_t    Result;
    uintptr_t    Handle;
    unsigned int Index;
    unsigned int EndHandle;
    unsigned int StartHandle;

    Result = FALSE;

    Handle = __sync_add_and_fetch(&NextHandle, 1);

    /* Zero is an invalid handle, so skip to the next available one.     */
    if(!Handle)
        Handle   = __sync_add_and_fetch(&NextHandle, 1);

    StartHandle = Handle;
    EndHandle   = (Handle + QSOSAL_KERNEL_MAX_OBJECTS);
    Index       = ((Handle - 1) % QSOSAL_KERNEL_MAX_OBJECTS);

    while((!Result) && ((StartHandle <= EndHandle) ? ((Handle < EndHandle) && (StartHandle <= Handle)) : ((Handle < EndHandle) || (StartHandle <= Handle))))
    {
        /* Check whether the slot is currently unusued and place a claim  */
        /* on the slot by incrementing the Users count.                   */
        /* ** NOTE ** A Users count of 0 indicates an unused slot.  A     */
        /*            positive value is an "invalid" claim (that is, the  */
        /*            slot is claimed but the contents are not valid).    */
        /*            Conversely, a negative value indicates a "valid"    */
        /*            slot with INT_MIN meaning "unclaimed".              */
        if(__sync_fetch_and_add(&(ObjectList[Index].Users), 1) == 0)
        {
            /* The slot was unused, so store the object.                   */
            ObjectList[Index].Object = Object;

            /* Map this object slot to the new handle.                     */
            ObjectList[Index].Handle = Handle;

            Result = TRUE;

            /* Mark the slot as valid and release the claim on the slot.   */
            /* ** NOTE ** This atomic operation doubles as a full          */
            /*            memory-barrier which guarantees the Object and   */
            /*            Handle writes will be complete before the slot is*/
            /*            unlocked.                                        */
            /* ** NOTE ** The slot is marked as valid by setting its Users */
            /*            count to INT_MIN.  We do this using an atomic    */
            /*            ADD/SUB operation here, rather than an explicit  */
            /*            SET, so that if other threads are currently      */
            /*            attempting to access this slot (and have         */
            /*            incremented the count into the positive range),  */
            /*            the count will correctly remain valid even after */
            /*            those threads release their claims (decrement the*/
            /*            count).  The additional decrement of 1 releases  */
            /*            the claim placed previously.                     */
            __sync_fetch_and_add(&(ObjectList[Index].Users), (int)(((unsigned int)INT_MIN)-1));
        }
        else
        {
            /* This slot is not unused.  Undo the change to the Users      */
            /* count.                                                      */
            __sync_fetch_and_add(&(ObjectList[Index].Users), -1);
        }

        /* If the object could not be stored in this slot, obtain the next*/
        /* available slot index.                                          */
        if(!Result)
        {
            Handle = __sync_add_and_fetch(&NextHandle, 1);

            /* Zero is an invalid handle, so skip to the next available.   */
            while(!Handle)
                Handle = __sync_add_and_fetch(&NextHandle, 1);

            Index = ((Handle - 1) % QSOSAL_KERNEL_MAX_OBJECTS);
        }
    }

    return((Result) ? (Handle) : 0);
}

/* The following function is provided to allow a mechanism to        */
/* atomically retrieve the kernel object associated with a given     */
/* handle from the global object list and place a claim on the       */
/* associated object list slot in a thread-safe manner.  This        */
/* function returns a pointer to the object if the object was found, */
/* or NULL if the handle was invalid.                                */
static void *RetrieveObject(uintptr_t Handle)
{
    void         *Object = NULL;
    unsigned int  Index  = ((Handle - 1) % QSOSAL_KERNEL_MAX_OBJECTS);

    /* Place a claim on this object slot.                                */
    /* ** NOTE ** The slot is only valid if the Users count was negative.*/
    /*            Zero, or any positive count, indicates an invalid slot.*/
    if(__sync_fetch_and_add(&(ObjectList[Index].Users), 1) < 0)
    {
        /* Obtain the object.                                             */
        /* ** NOTE ** Atomically incrementing the Users count also served */
        /*            as a full memory-barrier, so it is safe to read the */
        /*            Object address directly.                            */
        Object = ObjectList[Index].Object;

        /* Confirm that the slot is still associated with the requested   */
        /* handle.                                                        */
        if(ObjectList[Index].Handle != Handle)
            Object = NULL;

        /* If the object is not valid, remove the claim on this slot.     */
        if(Object == NULL)
            __sync_fetch_and_add(&(ObjectList[Index].Users), -1);
    }
    else
    {
        /* The slot was not valid, so reverse the change to the Users     */
        /* count.                                                         */
        __sync_fetch_and_add(&(ObjectList[Index].Users), -1);
    }

    return(Object);
}

/* The following function is provided to allow a mechanism to        */
/* release a claim on the global object list slot associated with a  */
/* particular handle.                                                */
static void ReleaseObject(uintptr_t Handle)
{
    __sync_fetch_and_add(&(ObjectList[(Handle - 1) % QSOSAL_KERNEL_MAX_OBJECTS].Users), -1);
}

/* The following function is provided to allow a mechanism to        */
/* atomically retrieve a kernel object associated with a given handle*/
/* from the global object list and clear the associated storage slot */
/* in a thread-safe manner.  This function returns a pointer to the  */
/* object if the object was found and cleared, or NULL if the handle */
/* was invalid.                                                      */
static void *ClearObject(uintptr_t Handle)
{
    Object_t     *Object;
    unsigned int  Index  = ((Handle - 1) % QSOSAL_KERNEL_MAX_OBJECTS);

    /* Memory-barrier prior to reading the Object pointer.               */
    __sync_synchronize();

    Object                   = ObjectList[Index].Object;
    ObjectList[Index].Object = NULL;

    /* Set the Users count to a positive value to mark this slot as      */
    /* unused.                                                           */
    __sync_fetch_and_add(&(ObjectList[Index].Users), (int)(((unsigned int)INT_MIN) * -1));

    return(Object);
}

/* The following function is provided to allow a mechanism to check  */
/* whether a slot in the global object list is currently in use by a */
/* thread.  This function returns TRUE if the slot is currently in   */
/* use, or FALSE if no threads currently claim the slot.             */
static Boolean_t ObjectSlotInUse(uintptr_t Handle)
{
    int Users = __sync_fetch_and_add(&(ObjectList[(Handle - 1) % QSOSAL_KERNEL_MAX_OBJECTS].Users), 0);

    return((Users != INT_MIN) && (Users != 0));
}

/* The following function is a helper function responsible for       */
/* releasing a Mutex that was successfully acquired with the         */
/* QSOSAL_WaitMutex() function. This function accepts as input the   */
/* MutexHeader (the mutex object) obtained from a call to            */
/* RetrieveObject. This function returns TRUE if the mutex was       */
/* released, or FALSE if the mutex was not owned by the current      */
/* thread.                                                           */
static Boolean_t ReleaseMutex(MutexHeader_t *MutexHeader)
{
    Boolean_t      ret_val;

    /* Before proceeding any further we need to make sure that the       */
    /* parameter that was passed to us appears semi-valid.               */
    if(MutexHeader)
    {
        /* The parameter apprears to be semi-valid now acquire lock that  */
        /* protects internal members.                                     */
        if(!sem_wait(&(MutexHeader->Lock)))
        {
            /* Check to see if the Mutex is currently owned (and if this   */
            /* thread is the owner).                                       */
            if(QSOSAL_CurrentThreadHandle() == MutexHeader->ThreadHandle)
            {
                /* The current thread owns the mutex, so note that the mutex*/
                /* has been released and check whether this thread holds any*/
                /* remaining claims on the mutex.                           */
                if(!(--(MutexHeader->Count)))
                {
                    /* The mutex is no longer being used by the current      */
                    /* thread.  Go ahead and clear out the current Thread    */
                    /* Handle.                                               */
                    MutexHeader->ThreadHandle = NULL;

                    /* Signal waiting threads that this mutex is available.  */
                    sem_post(&(MutexHeader->Semaphore));

                    /* Release the lock we acquired earlier.                 */
                    sem_post(&(MutexHeader->Lock));

                    /* The lock has been released, so return success.        */
                    ret_val = TRUE;
                }
                else
                {
                    /* Release the lock we acquired earlier.                 */
                    sem_post(&(MutexHeader->Lock));

                    /* The current thread holds additional claims on the     */
                    /* mutex, so just return success.                        */
                    ret_val = TRUE;
                }
            }
            else
            {
                /* Calling thread does not own the Mutex.  Release the lock */
                /* we acquired earlier and return failure.                  */
                sem_post(&(MutexHeader->Lock));

                ret_val = FALSE;
            }
        }
        else
        {
            /* The mutex internal lock could not be claimed, so return     */
            /* failure.                                                    */
            ret_val = FALSE;
        }
    }
    else
    {
        /* The mutex object is not valid, so return failure.              */
        ret_val = FALSE;
    }

    return(ret_val);
}

/* The following function is responsible for delaying the current    */
/* task for the specified duration (specified in Milliseconds).      */
/* * NOTE * Very small timeouts might be smaller in granularity than */
/*          the system can support !!!!                              */
void QSOSAL_Delay(unsigned long MilliSeconds)
{
    int             ret_val;
    sigset_t        SigSet;
    Boolean_t       Done;
    struct timespec Requested, Remaining;

    /* Note that in Linux, a signal may interrupt the sleep call, in     */
    /* which case the return value is -1, errno is set to EINTR, and the */
    /* remaining time to sleep is written to to the remaining structure. */
    /* Since debugging under Linux works by using signals, this function */
    /* needs to take this into account.                                  */

    /* First, format up the entire requested time (in nanoseconds).      */
    Requested.tv_sec  = MilliSeconds/1000;
    Requested.tv_nsec = (MilliSeconds - Requested.tv_sec*1000)*1000000;

    /* Now loop until we are either forced to exit OR we have waited for */
    /* the Requested time.                                               */
    Done = FALSE;
    while(!Done)
    {
        ret_val = nanosleep(&Requested, &Remaining);

        if((ret_val) && (errno == EINTR))
        {
            /* A signal has interrupted us, check what it was.             */
            sigpending(&SigSet);

            /* If trap or profile signal, sleep again.                     */
            if((sigismember(&SigSet, SIGTRAP)) || (sigismember(&SigSet, SIGPROF)))
                Requested = Remaining;
            else
                Done = TRUE;
        }
        else
            Done = TRUE;
    }
}

/* The following function is responsible for creating an actual Mutex*/
/* (Binary Semaphore). The Mutex is unique in that if a Thread       */
/* already owns the Mutex, and it requests the Mutex again it will be*/
/* granted the Mutex. This is in Stark contrast to a Semaphore that  */
/* will block waiting for the second acquisition of the Sempahore.   */
/* This function accepts as input whether or not the Mutex is        */
/* initially Signalled or not. If this input parameter is TRUE then  */
/* the caller owns the Mutex and any other threads waiting on the    */
/* Mutex will block. This function returns a NON-NULL Mutex Handle if*/
/* the Mutex was successfully created, or a NULL Mutex Handle if the */
/* Mutex was NOT created. If a Mutex is successfully created, it can */
/* only be destroyed by calling the QSOSAL_CloseMutex() function (and*/
/* passing the returned Mutex Handle).                               */
Mutex_t QSOSAL_CreateMutex(Boolean_t CreateOwned)
{
    Mutex_t        ret_val;
    MutexHeader_t *MutexHeader;

    /* Before proceeding allocate memory for the mutex header and verify */
    /* that the allocate was successful.                                 */
    if((MutexHeader = (MutexHeader_t *)QSOSAL_AllocateMemory(sizeof(MutexHeader_t))) != NULL)
    {
        /* The memory for the mutex header was successfully allocated, now*/
        /* Initialize all semaphores.                                     */
        if(!sem_init(&(MutexHeader->Lock), 0, 1))
        {
            /* The memory for the semaphore was successfully allocated, now*/
            /* attempt to create the semaphore.                            */
            if(!sem_init(&(MutexHeader->Semaphore), 0, (CreateOwned)?0:1))
            {
                /* The semaphore was successfully created, now check to see */
                /* if the mutex is being created as owned.                  */
                if(CreateOwned)
                {
                    /* The mutex is being created as owned so initialize the */
                    /* count and Thread Handle values within the mutex header*/
                    /* structure appropriately.                              */
                    MutexHeader->Count        = 1;
                    MutexHeader->ThreadHandle = QSOSAL_CurrentThreadHandle();
                }
                else
                {
                    /* The mutex is not being created as owned so initialize */
                    /* the count and Thread Handle value within the mutex    */
                    /* header structure appropriately.                       */
                    MutexHeader->Count        = 0;
                    MutexHeader->ThreadHandle = NULL;
                }

                /* The mutex has now been successfully created.  Store      */
                /* the mutex object and set the return value to the mutex   */
                /* handle.                                                  */
                ret_val = (Mutex_t)StoreObject(MutexHeader);

                if(!ret_val)
                {
                    /* An error occurred while attempting to store the mutex */
                    /* object, free all resources that have been allocated.  */
                    sem_destroy(&(MutexHeader->Semaphore));
                    sem_destroy(&(MutexHeader->Lock));
                    QSOSAL_FreeMemory(MutexHeader);
                }
            }
            else
            {
                /* An error occured while attempting to create the          */
                /* semaphore, set the return value to NULL and free all     */
                /* resources that have been allocated.                      */
                sem_destroy(&(MutexHeader->Lock));
                QSOSAL_FreeMemory(MutexHeader);

                ret_val = (Mutex_t)NULL;
            }
        }
        else
        {
            /* We were unable to allocate the memory required for the      */
            /* creation of the semaphore, set the return value to NULL and */
            /* free all resources that have been allocated.                */
            QSOSAL_FreeMemory(MutexHeader);

            ret_val = (Mutex_t)NULL;
        }
    }
    else
    {
        /* An error occured while trying to allocate memory for the       */
        /* MutexHeader, set the return value to NULL.                     */
        ret_val = (Mutex_t)NULL;
    }

    return(ret_val);
}

/* The following function is responsible for waiting for the         */
/* specified Mutex to become free. This function accepts as input the*/
/* Mutex Handle to wait for, and the Timeout (specified in           */
/* Milliseconds) to wait for the Mutex to become available. This     */
/* function returns TRUE if the Mutex was successfully acquired and  */
/* FALSE if either there was an error OR the Mutex was not acquired  */
/* in the specified Timeout. It should be noted that Mutexes have the*/
/* special property that if the calling Thread already owns the Mutex*/
/* and it requests access to the Mutex again (by calling this        */
/* function and specifying the same Mutex Handle) then it will       */
/* automatically be granted the Mutex. Once a Mutex has been granted */
/* successfully (this function returns TRUE), then the caller MUST   */
/* call the QSOSAL_ReleaseMutex() function.                          */
/* * NOTE * There must exist a corresponding QSOSAL_ReleaseMutex()   */
/*          function call for EVERY successful QSOSAL_WaitMutex()    */
/*          function call or a deadlock will occur in the system !!! */
Boolean_t QSOSAL_WaitMutex(Mutex_t Mutex, unsigned long Timeout)
{
    Boolean_t       ret_val;
    MutexHeader_t   *MutexHeader;
    struct timespec WaitLimit;
    struct timespec *WaitLimitPtr;

    /* Calculate the requested timeout.  This is done immediately to     */
    /* guarantee the highest possible accurancy.                         */
    if(Timeout == QSOSAL_INFINITE_WAIT)
        WaitLimitPtr = NULL;
    else
    {
        GetCurrentTime(&WaitLimit);
        TimespecIncrement(&WaitLimit, Timeout);

        WaitLimitPtr = &WaitLimit;
    }

    /* Before proceeding any further we need to make sure that the       */
    /* parameters that were passed to us appear semi-valid.              */
    if((Mutex) && ((MutexHeader = (MutexHeader_t *)RetrieveObject((uintptr_t)Mutex)) != NULL))
    {
        /* The parameters apprear to be semi-valid check if the thread    */
        /* asking for the Mutex is the current thread that owns it.       */
        if(!sem_wait(&(MutexHeader->Lock)))
        {
            if(QSOSAL_CurrentThreadHandle() == MutexHeader->ThreadHandle)
            {
                /* This thread owns the Mutex so it is not forced to block, */
                /* increament the count signifying another call to wait has */
                /* been made by the calling thread.                         */
                MutexHeader->Count++;

                /* Release the lock we acquired earlier.                    */
                sem_post(&(MutexHeader->Lock));

                /* Mutex acquired successfully.                             */
                ret_val = TRUE;
            }
            else
            {
                /* Release the lock we acquired earlier.                    */
                /* * NOTE * This is safe because the actual waiting for the */
                /*          actual Semaphore itself will serialize any      */
                /*          concurrent threads that make it to this point.  */
                sem_post(&(MutexHeader->Lock));

                /* The calling thread does not currently own the mutex so   */
                /* it must try to acquire it.  Wait the requested amount of */
                /* time for the Mutex to be signalled.                      */
                ret_val = WaitForSemaphore(&(MutexHeader->Semaphore), WaitLimitPtr);

                /* If the semaphore was signaled successfully, attempt to   */
                /* claim the mutex.                                         */
                if(ret_val)
                {
                    /* The Mutex was signalled within the timeout.           */
                    /* Re-acquire the internal lock of the Mutex.            */
                    if(!sem_wait(&(MutexHeader->Lock)))
                    {
                        /* Confirm whether the Mutex is still available to be */
                        /* acquired.                                          */
                        if(MutexHeader->ThreadHandle == NULL)
                        {
                            /* The Mutex is still available, so save the       */
                            /* current Thread Handle to show that the calling  */
                            /* thread now owns the Mutex.                      */
                            MutexHeader->ThreadHandle = QSOSAL_CurrentThreadHandle();

                            /* Reset the lock count.                           */
                            MutexHeader->Count = 1;
                        }
                        else
                        {
                            /* The Mutex is already owned by some other thread.*/
                            /* This probably indicates that the Mutex is being */
                            /* destroyed.  Return failure to the caller.       */
                            ret_val = FALSE;
                        }

                        /* Release the internal lock of the Mutex.            */
                        sem_post(&(MutexHeader->Lock));
                    }
                    else
                    {
                        /* The internal lock could not be acquired.  Signal   */
                        /* the Mutex again so another thread can acquire the  */
                        /* mutex.  Return failure to the caller.              */
                        sem_post(&(MutexHeader->Semaphore));

                        ret_val = FALSE;
                    }
                }
            }
        }
        else
            ret_val = FALSE;

        /* Release the object claim for MutexHeader.                      */
        ReleaseObject((uintptr_t)Mutex);
    }
    else
        ret_val = FALSE;

    return(ret_val);
}

/* The following function is responsible for releasing a Mutex that  */
/* was successfully acquired with the QSOSAL_WaitMutex() function.   */
/* This function accepts as input the Mutex that is currently owned. */
/* * NOTE * There must exist a corresponding QSOSAL_ReleaseMutex()   */
/*          function call for EVERY successful QSOSAL_WaitMutex()    */
/*          function call or a deadlock will occur in the system !!! */
void QSOSAL_ReleaseMutex(Mutex_t Mutex)
{
    MutexHeader_t *MutexHeader;

    /* Before proceeding any further we need to make sure that the       */
    /* parameter that was passed to us appears semi-valid.               */
    if((Mutex) && ((MutexHeader = (MutexHeader_t *)RetrieveObject((uintptr_t)Mutex)) != NULL))
    {
        /* Release the mutex lock.                                        */
        ReleaseMutex(MutexHeader);

        /* Release the claim on the mutex object.                         */
        ReleaseObject((uintptr_t)Mutex);
    }
}

/* The following function is responsible for destroying a Mutex that */
/* was created successfully via a successful call to the             */
/* QSOSAL_CreateMutex() function. This function accepts as input the */
/* Mutex Handle of the Mutex to destroy. Once this function is       */
/* completed the Mutex Handle is NO longer valid and CANNOT be used. */
/* Calling this function will cause all outstanding                  */
/* QSOSAL_WaitMutex() functions to fail with an error.               */
void QSOSAL_CloseMutex(Mutex_t Mutex)
{
    MutexHeader_t *MutexHeader;

    /* First, ensure no other threads are currently holding this lock.   */
    /* ** NOTE ** This must be done before removing the object from the  */
    /*            global list so that another thread which holds this    */
    /*            mutex will be able to release the mutex by its handle. */
    if(QSOSAL_WaitMutex(Mutex, QSOSAL_INFINITE_WAIT))
    {
        /* We now own the lock on this mutex, so remove the object from   */
        /* the global list.                                               */
        if((MutexHeader = (MutexHeader_t *)ClearObject((uintptr_t)Mutex)) != NULL)
        {
            /* Wait for all users of this handle to clear.                 */
            while(ObjectSlotInUse((uintptr_t)Mutex))
            {
                sem_post(&(MutexHeader->Semaphore));
                QSOSAL_Delay(1);
            }

            /* At this point, no other threads are accessing or are able   */
            /* to access this mutex object, so it is safe to release all   */
            /* resources and de-allocate the memory associated with the    */
            /* mutex.                                                      */
            sem_destroy(&(MutexHeader->Semaphore));

            sem_destroy(&(MutexHeader->Lock));

            QSOSAL_FreeMemory(MutexHeader);
        }
        else
        {
            /* The mutex lock was obtained but the object was then not     */
            /* found in the global list, so release the lock by handle.    */
            /* ** WARNING ** It should not be possible for this to occur   */
            /*               because the object could only have been       */
            /*               removed from the global list by a thread which*/
            /*               owns this mutex -- had another thread already */
            /*               started destroying this mutex, the above      */
            /*               attempt to lock the mutex would have failed.  */
            QSOSAL_ReleaseMutex(Mutex);
        }
    }
}

/* The following function is responsible for creating an actual      */
/* Event. The Event is unique in that it only has two states. These  */
/* states are Signalled and Non-Signalled. Functions are provided to */
/* allow the setting of the current state and to allow the option of */
/* waiting for an Event to become Signalled. This function accepts as*/
/* input whether or not the Event is initially Signalled or not. If  */
/* this input parameter is TRUE then the state of the Event is       */
/* Signalled and any QSOSAL_WaitEvent() function calls will          */
/* immediately return. This function returns a NON-NULL Event Handle */
/* if the Event was successfully created, or a NULL Event Handle if  */
/* the Event was NOT created. If an Event is successfully created, it*/
/* can only be destroyed by calling the QSOSAL_CloseEvent() function */
/* (and passing the returned Event Handle).                          */
Event_t QSOSAL_CreateEvent(Boolean_t CreateSignalled)
{
    Event_t        ret_val     = NULL;
    EventHeader_t *EventHeader = NULL;

    /* Before procedding allocate memory for the event header and verify */
    /* that the allocate was successful.                                 */
    if((EventHeader = (EventHeader_t *)QSOSAL_AllocateMemory(sizeof(EventHeader_t))) != NULL)
    {
        /* The memory for the semaphore has been successfully allocated,  */
        /* now create the mutex to be used to protect the event.          */
        if((EventHeader->Mutex = QSOSAL_CreateMutex(FALSE)) != NULL)
        {
            /* The mutex has been created, now create the semaphore to be  */
            /* used as the event.                                          */
            if(!sem_init(&(EventHeader->Semaphore), 0, (CreateSignalled)?1:0))
            {
                /* The semaphore to be used as the event was successfully   */
                /* created, now set the state of the semaphore.             */
                EventHeader->IsSet = CreateSignalled;
                EventHeader->Count = (CreateSignalled)?0:1;

                /* The Event has now been successfully created.  Store      */
                /* the event object and set the return value to the Event   */
                /* handle.                                                  */
                ret_val = (Event_t)StoreObject(EventHeader);

                if(!ret_val)
                {
                    /* An error occurred while attempting to store the event */
                    /* object, clean up the semaphore.  Other resources will */
                    /* be freed below.                                       */
                    sem_destroy(&(EventHeader->Semaphore));
                }
            }
        }
    }

    /* Check if an error occured while creating the Event Header, if so  */
    /* deallocate the Event Header.                                      */
    if((!ret_val) && (EventHeader))
    {
        if(EventHeader->Mutex)
            QSOSAL_CloseMutex(EventHeader->Mutex);

        QSOSAL_FreeMemory(EventHeader);
    }

    return((Event_t)ret_val);
}

/* The following function is responsible for waiting for the         */
/* specified Event to become Signalled. This function accepts as     */
/* input the Event Handle to wait for, and the Timeout (specified in */
/* Milliseconds) to wait for the Event to become Signalled. This     */
/* function returns TRUE if the Event was set to the Signalled State */
/* (in the Timeout specified) or FALSE if either there was an error  */
/* OR the Event was not set to the Signalled State in the specified  */
/* Timeout. It should be noted that Signalls have a special property */
/* in that multiple Threads can be waiting for the Event to become   */
/* Signalled and ALL calls to QSOSAL_WaitEvent() will return TRUE    */
/* whenever the state of the Event becomes Signalled.                */
Boolean_t QSOSAL_WaitEvent(Event_t Event, unsigned long Timeout)
{
    Boolean_t        ret_val;
    EventHeader_t   *TempHeader;
    EventHeader_t   *EventHeader;
    struct timespec  WaitLimit;

    /* Before proceeding any further we need to make sure that the       */
    /* parameters that were passed to us appear semi-valid.              */
    if((Event) && ((EventHeader = (EventHeader_t *)RetrieveObject((uintptr_t)Event)) != NULL))
    {
        /* If a timeout was requested, acquire the current time before any*/
        /* delays are incurred from waiting on the Event's mutex.         */
        if(Timeout != QSOSAL_INFINITE_WAIT)
        {
            /* Calculate the time at which to abort the wait.              */
            GetCurrentTime(&WaitLimit);
            TimespecIncrement(&WaitLimit, Timeout);
        }

        /* The parameters that were passed in appear to be semi-valid, now*/
        /* try to aquire the mutex associated with this event.            */
        if(QSOSAL_WaitMutex(EventHeader->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Check to see if the current state indicates that we are     */
            /* currently reset.                                            */
            if(!(EventHeader->IsSet))
            {
                /* The event is in the reset state.  Increament the count.  */
                EventHeader->Count++;

                /* We are all done updating the variables, now release the  */
                /* mutex.                                                   */
                QSOSAL_ReleaseMutex(EventHeader->Mutex);

                /* Wait for the event to become set.                        */
                if(Timeout == QSOSAL_INFINITE_WAIT)
                    ret_val = WaitForSemaphore(&(EventHeader->Semaphore), NULL);
                else
                    ret_val = WaitForSemaphore(&(EventHeader->Semaphore), &WaitLimit);

                if(ret_val)
                {
                    /* The semaphore was signalled.  Confirm that this object*/
                    /* is still valid before announcing success.             */
                    if((TempHeader = (EventHeader_t *)RetrieveObject((uintptr_t)Event)) != NULL)
                    {
                        /* If the object has changed, return failure.         */
                        if(TempHeader != EventHeader)
                            ret_val = FALSE;

                        /* Release the object claim for TempHeader.           */
                        ReleaseObject((uintptr_t)Event);
                    }
                    else
                    {
                        /* The object slot is invalid, so report failure.     */
                        ret_val = FALSE;
                    }
                }
                else
                {
                    /* A timeout or an error occured, get the mutex so that  */
                    /* we can modify the count variable.                     */
                    if(QSOSAL_WaitMutex(EventHeader->Mutex, QSOSAL_INFINITE_WAIT))
                    {
                        /* Decrement the count.                               */
                        EventHeader->Count--;

                        /* We are all done updating the variables, now release*/
                        /* the mutex.                                         */
                        QSOSAL_ReleaseMutex(EventHeader->Mutex);
                    }
                }
            }
            else
            {
                /* We are all done updating the variables, now release the  */
                /* mutex.                                                   */
                QSOSAL_ReleaseMutex(EventHeader->Mutex);

                /* The event is in the set state, set the return value to   */
                /* true.                                                    */
                ret_val = TRUE;
            }
        }
        else
            ret_val = FALSE;

        /* Release the object claim for EventHeader.                      */
        ReleaseObject((uintptr_t)Event);
    }
    else
        ret_val = FALSE;

    return(ret_val);
}

/* The following function is responsible for changing the state of   */
/* the specified Event to the Non-Signalled State. Once the Event is */
/* in this State, ALL calls to the QSOSAL_WaitEvent() function will  */
/* block until the State of the Event is set to the Signalled State. */
/* This function accepts as input the Event Handle of the Event to   */
/* set to the Non-Signalled State.                                   */
void QSOSAL_ResetEvent(Event_t Event)
{
    EventHeader_t *EventHeader;

    /* Before proceeding any further we need to make sure that the       */
    /* parameter that was passed to us appears to be semi-valid.         */
    if((Event) && ((EventHeader = (EventHeader_t *)RetrieveObject((uintptr_t)Event)) != NULL))
    {
        /* The event that was passed in appears to be semi-valid, now try */
        /* to aquire the mutex associated with this event.                */
        if(QSOSAL_WaitMutex(EventHeader->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* We have successfully aquired the mutex, now check to see if */
            /* the event is currently signaled.                            */
            if(EventHeader->IsSet)
            {
                /* The event is currently set, change the flag and update   */
                /* the count.                                               */
                EventHeader->IsSet = FALSE;
                EventHeader->Count++;

                /* We are all done updating the variables, now release the  */
                /* mutex.                                                   */
                QSOSAL_ReleaseMutex(EventHeader->Mutex);

                /* Reset the event semaphore.                               */
                sem_trywait(&(EventHeader->Semaphore));
            }
            else
            {
                /* We are all done updating the variables, now release the  */
                /* mutex.                                                   */
                QSOSAL_ReleaseMutex(EventHeader->Mutex);
            }
        }

        /* Release the object claim for EventHeader.                      */
        ReleaseObject((uintptr_t)Event);
    }
}

/* The following function is responsible for changing the state of   */
/* the specified Event to the Signalled State. Once the Event is in  */
/* this State, ALL calls to the QSOSAL_WaitEvent() function will     */
/* return. This function accepts as input the Event Handle of the    */
/* Event to set to the Signalled State.                              */
void QSOSAL_SetEvent(Event_t Event)
{
    EventHeader_t *EventHeader;

    /* Before proceeding any further we need to make sure that the       */
    /* parameter that was passed to us appears to be semi-valid.         */
    if((Event) && ((EventHeader = (EventHeader_t *)RetrieveObject((uintptr_t)Event)) != NULL))
    {
        /* The event that was passed in appears to be semi-valid, now try */
        /* to aquire the mutex associated with this event.                */
        if(QSOSAL_WaitMutex(EventHeader->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* We have successfully aquired the mutex, now check to see if */
            /* the event is currently signaled.                            */
            if(!EventHeader->IsSet)
            {
                /* The event is not currently set, change the flag and post */
                /* the semaphore count times.                               */
                while(EventHeader->Count--)
                    sem_post(&(EventHeader->Semaphore));

                EventHeader->IsSet = TRUE;
                EventHeader->Count = 0;
            }

            /* We are all done, now release the mutex.                     */
            QSOSAL_ReleaseMutex(EventHeader->Mutex);
        }

        /* Release the object claim for EventHeader.                      */
        ReleaseObject((uintptr_t)Event);
    }
}

/* The following function is responsible for destroying an Event that*/
/* was created successfully via a successful call to the             */
/* QSOSAL_CreateEvent() function. This function accepts as input the */
/* Event Handle of the Event to destroy. Once this function is       */
/* completed the Event Handle is NO longer valid and CANNOT be used. */
/* Calling this function will cause all outstanding                  */
/* QSOSAL_WaitEvent() functions to fail with an error.               */
void QSOSAL_CloseEvent(Event_t Event)
{
    unsigned int   Count;
    EventHeader_t *EventHeader;

    /* Before proceeding any further we need to make sure that the       */
    /* parameters that are passed to us apper to be semi-valid.          */
    if(Event)
    {
        /* Remove the event object from the global object list.           */
        if((EventHeader = (EventHeader_t *)ClearObject((uintptr_t)Event)) != NULL)
        {
            /* First delete the mutex that was associated with the event.  */
            /* This will cut off any threads that have just called into an */
            /* Event function.                                             */
            QSOSAL_CloseMutex(EventHeader->Mutex);

            /* Flush any threads waiting on this Event.                    */
            Count = EventHeader->Count;

            while(Count--)
                sem_post(&(EventHeader->Semaphore));

            /* Allow waiting threads to resume and release any claims on   */
            /* the Event object.                                           */
            QSOSAL_Delay(1);

            /* Wait for all remaining users of this handle to clear out.   */
            while(ObjectSlotInUse((uintptr_t)Event))
            {
                /* Pump the Event's semaphore in case a thread managed to   */
                /* enter QSOSAL_WaitEvent but not yet start waiting until   */
                /* after we flushed waiting threads.                        */
                sem_post(&(EventHeader->Semaphore));
                QSOSAL_Delay(1);
            }

            /* Next delete the semaphore that was associated with the      */
            /* event.                                                      */
            sem_destroy(&(EventHeader->Semaphore));

            /* Finally delete the memory that was allocated for the event  */
            /* header stucture.                                            */
            QSOSAL_FreeMemory(EventHeader);
        }
    }
}

/* The following function is provided to allow a mechanism to        */
/* actually allocate a Block of Memory (of at least the specified    */
/* size).  This function accepts as input the size (in Bytes) of the */
/* Block of Memory to be allocated.  This function returns a NON-NULL*/
/* pointer to this Memory Buffer if the Memory was successfully      */
/* allocated, or a NULL value if the memory could not be allocated.  */
void *QSOSAL_AllocateMemory(unsigned long MemorySize)
{
    /* Simply wrap the OS supplied memory allocation function.           */
    return(malloc(MemorySize));
}

/* The following function is responsible for de-allocating a Block of*/
/* Memory that was successfully allocated with the                   */
/* QSOSAL_AllocateMemory() function. This function accepts a NON-NULL*/
/* Memory Pointer which was returned from the QSOSAL_AllocateMemory()*/
/* function. After this function completes the caller CANNOT use ANY */
/* of the Memory pointed to by the Memory Pointer.                   */
void QSOSAL_FreeMemory(void *MemoryPointer)
{
    /* Before proceeding any further we need to make sure that the       */
    /* parameters that were passed to us appear semi-valid.              */
    if(MemoryPointer)
        free(MemoryPointer);
}

/* The following function is responsible for copying a block of      */
/* memory of the specified size from the specified source pointer    */
/* to the specified destination memory pointer.  This function       */
/* accepts as input a pointer to the memory block that is to be      */
/* Destination Buffer (first parameter), a pointer to memory block   */
/* that points to the data to be copied into the destination buffer, */
/* and the size (in bytes) of the Data to copy.  The Source and      */
/* Destination Memory Buffers must contain AT LEAST as many bytes    */
/* as specified by the Size parameter.                               */
/* * NOTE * This function does not allow the overlapping of the      */
/*          Source and Destination Buffers !!!!                      */
void QSOSAL_MemCopy(void *Destination, const void *Source, unsigned long Size)
{
    /* Simply wrap the C Run-Time memcpy() function.                     */
    memcpy(Destination, Source, Size);
}

/* The following function is responsible for copying a block of      */
/* memory of the specified size from the specified source pointer to */
/* the specified destination memory pointer. The first parameter to  */
/* this function is a pointer to the destination buffer. The second  */
/* parameter is the size of the destination buffer. The third        */
/* parameter is a pointer to the source buffer. The final parameter  */
/* is the number of bytes to copy from the source buffer to the      */
/* destination buffer.                                               */
/* * NOTE * It is the caller's responsibility to make sure that the  */
/*          input parameters are valid.                              */
/* * NOTE * This function does NOT allow overlapping of the source   */
/*          and destination buffers. If this functionality is        */
/*          required, then the QSOSAL_MemMove_S() function MUST be   */
/*          used.                                                    */
void QSOSAL_MemCopy_S(void *Destination, unsigned long DestSize, const void *Source, unsigned long Count)
{
#if (!QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION)
    Byte_t *_Destination;
    Byte_t *_Source;
#endif

    /* Make sure the input parameters are valid.                         */
    if((Destination) && (DestSize) && (Source) && (Count) && ((size_t)Destination != (size_t)Source) && (Count <= DestSize))
    {

#if QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION

        /* Simply wrap the C Run-Time memcpy() function.                  */
        memcpy(Destination, Source, (size_t)Count);

#else

        /* Store the destination and source as byte pointers.             */
        /* * NOTE * This has been done to enforce consistency across      */
        /*          platforms and prevent void pointer issues that may or */
        /*          may not occur depending on the platform.              */
        _Destination = (Byte_t *)Destination;
        _Source      = (Byte_t *)Source;

        /* Copy the data from the source buffer into the destination      */
        /* buffer.                                                        */
        while(Count--)
        {
            *_Destination++ = *_Source++;
        }

#endif

    }
}

/* The following function is responsible for moving a block of       */
/* memory of the specified size from the specified source pointer    */
/* to the specified destination memory pointer.  This function       */
/* accepts as input a pointer to the memory block that is to be      */
/* Destination Buffer (first parameter), a pointer to memory block   */
/* that points to the data to be copied into the destination buffer, */
/* and the size (in bytes) of the Data to copy.  The Source and      */
/* Destination Memory Buffers must contain AT LEAST as many bytes    */
/* as specified by the Size parameter.                               */
/* * NOTE * This function DOES allow the overlapping of the          */
/*          Source and Destination Buffers.                          */
void QSOSAL_MemMove(void *Destination, const void *Source, unsigned long Size)
{
    /* Simply wrap the C Run-Time memmove() function.                    */
    memmove(Destination, Source, Size);
}

/* The following function is responsible for moving a block of memory*/
/* of the specified size from the specified source pointer to the    */
/* specified destination memory pointer. The first parameter to this */
/* function is a pointer to the destination buffer. The second       */
/* parameter is the size of the destination buffer. The third        */
/* parameter is a pointer to the source buffer. The final parameter  */
/* is the number of bytes to copy from the source buffer to the      */
/* destination buffer. This function returns TRUE if the move was    */
/* successful. Otherwise FALSE is returned.                          */
/* * NOTE * It is the caller's responsibility to make sure that the  */
/*          input parameters are valid.                              */
/* * NOTE * This function DOES allow overlapping of the source and   */
/*          destination buffers. If the source and destination       */
/*          buffers do NOT overlap, then the user should call the    */
/*          QSOSAL_MemCopy_S() function.                             */
void QSOSAL_MemMove_S(void *Destination, unsigned long DestSize, const void *Source, unsigned long Count)
{
#if (!QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION)
    Byte_t *_Destination;
    Byte_t *_Source;
#endif

    /* Make sure the input parameters are valid.                         */
    if((Destination) && (DestSize) && (Source) && (Count) && ((size_t)Destination != (size_t)Source) && (Count <= DestSize))
    {

#if QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION

        /* Simply wrap the C Run-Time memmove() function.                 */
        memmove(Destination, Source, (size_t)Count);

#else

        /* Store the destination and source as byte pointers.             */
        /* * NOTE * This has been done to enforce consistency across      */
        /*          platforms and prevent void pointer issues that may or */
        /*          may not occur depending on the platform.              */
        _Destination = (Byte_t *)Destination;
        _Source      = (Byte_t *)Source;

        /* Check to see if the source buffer overlaps the destination     */
        /* buffer.                                                        */
        /* * NOTE * If this is the case, then copying the data in the     */
        /*          forward direction will overwrite data before it is    */
        /*          copied. Therefore, we will copy the data in the       */
        /*          reverse direction to prevent overwriting the data.    */
        if(((size_t)_Destination > (size_t)_Source) && ((size_t)_Source + Count >= (size_t)_Destination))
        {
            /* Set the Source and Destination buffer addresses to the last */
            /* byte that would have been copied in the forward direction.  */
            _Source      += (Count - 1);
            _Destination += (Count - 1);

            /* Copy the data from the Source buffer into the Destination   */
            /* buffer.                                                     */
            while(Count--)
            {
                *_Destination-- = *_Source--;
            }
        }
        else
        {
            /* Copy the data from the Source buffer into the Destination   */
            /* buffer.                                                     */
            while(Count--)
            {
                *_Destination++ = *_Source++;
            }
        }

#endif

    }
}

/* The following function is provided to allow a mechanism to fill   */
/* a block of memory with the specified value.  This function accepts*/
/* as input a pointer to the Data Buffer (first parameter) that is   */
/* to filled with the specified value (second parameter).  The       */
/* final parameter to this function specifies the number of bytes    */
/* that are to be filled in the Data Buffer.  The Destination        */
/* Buffer must point to a Buffer that is AT LEAST the size of        */
/* the Size parameter.                                               */
void QSOSAL_MemInitialize(void *Destination, unsigned char Value, unsigned long Size)
{
    /* Simply wrap the C Run-Time memset() function.                     */
    memset(Destination, Value, Size);
}

/* The following function is provided to allow a mechanism to        */
/* Compare two blocks of memory to see if the two memory blocks      */
/* (each of size Size (in bytes)) are equal (each and every byte up  */
/* to Size bytes).  This function returns a negative number if       */
/* Source1 is less than Source2, zero if Source1 equals Source2, and */
/* a positive value if Source1 is greater than Source2.              */
int QSOSAL_MemCompare(const void *Source1, const void *Source2, unsigned long Size)
{
    /* Simply wrap the C Run-Time memcmp() function.                     */
    return(memcmp(Source1, Source2, Size));
}

/* The following function is provided to allow a mechanism to Compare*/
/* two blocks of memory to see if the two memory blocks (each of size*/
/* Size (in bytes)) are equal (each and every byte up to Size bytes) */
/* using a Case-Insensitive Compare.  This function returns a        */
/* negative number if Source1 is less than Source2, zero if Source1  */
/* equals Source2, and a positive value if Source1 is greater than   */
/* Source2.                                                          */
int QSOSAL_MemCompareI(const void *Source1, const void *Source2, unsigned long Size)
{
    return(strncasecmp(Source1, Source2, Size));
}

/* source NULL Terminated ASCII (character) string to the specified  */
/* Destination String Buffer. The first parameter is a pointer to the*/
/* destination buffer string. The second parameter is a pointer to   */
/* the source NULL Terminated ASCII (character) string. The final    */
/* parameter is the size of the destination buffer.                  */
/* * NOTE * It is the caller's responsibility to make sure that the  */
/*          input parameters are valid.                              */
/* * NOTE * This function guarantees that the destination buffer     */
/*          string has been NULL terminated provided that the        */
/*          destination buffer is valid and has room for the NULL    */
/*          terminator.                                              */
void QSOSAL_StringCopy_S(char *Destination, unsigned long DestSize, const char *Source)
{
#if (!QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION)
    unsigned long SourceLength;
#endif

    /* Make sure the input parameters are valid.                         */
    if((Destination) && (Source) && (DestSize) && ((size_t)Destination != (size_t)Source))
    {

#if QSOSAL_KERNEL_USE_C_RUN_TIME_IMPLEMENTATION

        /* If the length of the source string is greater than or equal to */
        /* the length of the destination buffer, then the source string   */
        /* will be truncated.                                             */
        if(strlen(Source) >= DestSize)
        {
            /* Use the C Run-Time memcpy() function to copy the truncated  */
            /* source string to the destination buffer.                    */
            /* * NOTE * We cannot modify the source string since it is     */
            /*          constant.                                          */
            memcpy(Destination, Source, (DestSize - 1));
            Destination[DestSize - 1] = '\0';
        }
        else
        {
            /* Simply wrap the C Run-Time strcpy() function.               */
            strcpy(Destination, Source);
        }

#else

        /* If the source string length is greater than or equal to the    */
        /* length of the destination buffer, then the source string will  */
        /* be truncated.                                                  */
        if((SourceLength = strlen(Source)) >= DestSize)
        {
            /* Set the remaining length of the destination buffer to the   */
            /* position of the NULL terminator.                            */
            DestSize--;
        }
        else
        {
            /* Set the remaining length of the destination buffer to the   */
            /* position of the NULL terminator.                            */
            DestSize = SourceLength;
        }

        /* Set the NULL terminator.                                       */
        Destination[DestSize] = '\0';

        /* While there is room in the destination buffer, copy the source */
        /* string to the destination buffer.                              */
        while(DestSize--)
        {
            *(Destination++) = *(Source++);
        }

#endif

    }
    else
    {
        /* If the destination buffer is valid, make sure that it is NULL  */
        /* terminated.                                                    */
        if((Destination) && (DestSize))
        {
            Destination[0] = '\0';
        }
    }
}

/* The following function is provided to allow a mechanism to        */
/* determine the Length (in characters) of the specified NULL        */
/* Terminated ASCII (character) String.  This function accepts as    */
/* input a pointer to a NULL Terminated ASCII String and returns     */
/* the number of characters present in the string (NOT including     */
/* the terminating NULL character).                                  */
unsigned int QSOSAL_StringLength(const char *Source)
{
    /* Simply wrap the C Run-Time strlen() function.                     */
    return(strlen(Source));
}

/* The following function is provided to allow a mechanism to write a*/
/* NULL Terminated ASCII (character) format string to a destination  */
/* NULL Terminated ASCII (character) buffer. The first parameter is a*/
/* pointer to the destination NULL Terminated ASCII (character)      */
/* string. The second parameter is the size of the destination string*/
/* buffer. The third parameter is a pointer to the NULL Terminated   */
/* ASCII (character) format string. The final parameter holds the    */
/* arguments for the format string. If successful, this function     */
/* returns the length of the destination string. Otherwise a negative*/
/* value is returned, which represents an error.                     */
/* * NOTE * It is the caller's responsibility to make sure that the  */
/*          input parameters are valid.                              */
/* * NOTE * This function guarantees that the destination buffer     */
/*          string has been NULL terminated provided that the        */
/*          destination buffer is valid and has room for the NULL    */
/*          terminator.                                              */
int QSOSAL_SprintF_S(char *Destination, unsigned long DestSize, const char *Format, ...)
{
    int     ret_val;
    va_list args;

    va_start(args, Format);

    /* Simply wrap the C Run-Time vsnprintf() function.                  */
    if((ret_val = vsnprintf(Destination, (size_t)DestSize, Format, args)) >= 0)
    {
        /* Check if the string has been truncated.                        */
        /* * NOTE * vsnprintf() returns the number of characters that     */
        /*          would have been written had the destination buffer    */
        /*          been able to hold the entire format string.           */
        if((unsigned long)ret_val >= DestSize)
        {
            /* The string has been truncated so the number of characters   */
            /* written is the maximum that can be stored by the destination*/
            /* buffer.                                                     */
            ret_val = (DestSize - 1);
        }
    }

    va_end(args);

    return(ret_val);
}

/* The following function is provided to allow a means for the       */
/* programmer to create a seperate thread of execution.  This        */
/* function accepts as input the Function that represents the        */
/* Thread that is to be installed into the system as its first       */
/* parameter.  The second parameter is the size of the Threads       */
/* Stack (in bytes) required by the Thread when it is executing.     */
/* The final parameter to this function represents a parameter that  */
/* is to be passed to the Thread when it is created.  This function  */
/* returns a NON-NULL Thread Handle if the Thread was successfully   */
/* created, or a NULL Thread Handle if the Thread was unable to be   */
/* created.  Once the thread is created, the only way for the Thread */
/* to be removed from the system is for the Thread function to run   */
/* to completion.                                                    */
/* * NOTE * There does NOT exist a function to Kill a Thread that    */
/*          is present in the system.  Because of this, other means  */
/*          needs to be devised in order to signal the Thread that   */
/*          it is to terminate.                                      */
ThreadHandle_t QSOSAL_CreateThread(Thread_t ThreadFunction, unsigned int StackSize, void *ThreadParameter)
{
    pthread_attr_t ThreadAttributes;
    ThreadHandle_t ret_val;

    /* Before proceeding any further we need to make sure that the       */
    /* parameters that were passed to us appear semi-valid.              */
    if((ThreadFunction) && (StackSize))
    {
        /* Since we want to set the stack size for this thread we have to */
        /* use the pthread_attr_t structure in the creation of our thread.*/
        /* First we must initialize the structure.                        */
        if(!pthread_attr_init(&ThreadAttributes))
        {
            /* The thread attribute structure was successfully initialize, */
            /* now set the stack size to be the passed stack size.         */
            if(!pthread_attr_setstacksize(&ThreadAttributes, StackSize))
            {
                /* Make sure we create the thread as detached so that it    */
                /* does not have to be joined when the thread exits to      */
                /* release the thread's resources.                          */
                pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_DETACHED);

                /* The stack size has been successfully set, now create the */
                /* thread.                                                  */
                if(pthread_create((pthread_t *)&ret_val, &ThreadAttributes, (void *(*)(void *))ThreadFunction, ThreadParameter))
                {
                    /* There was an error while attempting to spawn the new  */
                    /* thread, set the return value to NULL.                 */
                    ret_val = (ThreadHandle_t)NULL;
                }
            }
            else
            {
                /* We were unable to set the stack size within the thread   */
                /* structure, set the return value to NULL.                 */
                ret_val = (ThreadHandle_t)NULL;
            }

            /* Clean up resources used to create the thread.               */
            pthread_attr_destroy(&ThreadAttributes);
        }
        else
        {
            /* We were unable to initialize the thread structure, set the  */
            /* return value to NULL.                                       */
            ret_val = (ThreadHandle_t)NULL;
        }
    }
    else
    {
        /* One or more of the passed parameters appears to be invalid, set*/
        /* the return value to NULL.                                      */
        ret_val = (ThreadHandle_t)NULL;
    }

    return(ret_val);
}

/* The following function is provided to allow a mechanism to        */
/* retrieve the handle of the thread which is currently executing.   */
/* This function require no input parameters and will return a valid */
/* ThreadHandle upon success.                                        */
ThreadHandle_t QSOSAL_CurrentThreadHandle(void)
{
    return((ThreadHandle_t)pthread_self());
}

/* The following function is provided to allow a mechanism to create */
/* a Mailbox. A Mailbox is a Data Store that contains slots (all of  */
/* the same size) that can have data placed into (and retrieved      */
/* from). Once Data is placed into a Mailbox (via the                */
/* QSOSAL_AddMailbox() function, it can be retreived by using the    */
/* QSOSAL_WaitMailbox() function. Data placed into the Mailbox is    */
/* retrieved in a FIFO method. This function accepts as input the    */
/* Maximum Number of Slots that will be present in the Mailbox and   */
/* the Size of each of the Slots. This function returns a NON-NULL   */
/* Mailbox Handle if the Mailbox is successfully created, or a NULL  */
/* Mailbox Handle if the Mailbox was unable to be created.           */
Mailbox_t QSOSAL_CreateMailbox(unsigned int NumberSlots, unsigned int SlotSize)
{
    Mailbox_t        ret_val;
    MailboxHeader_t *MailboxHeader;

    /* Before proceeding any further we need to make sure that the       */
    /* parameters that were passed to us appear semi-valid.              */
    if((NumberSlots) && (SlotSize))
    {
        /* Parameters appear semi-valid, so now let's allocate enough     */
        /* Memory to hold the Mailbox Header AND enough space to hold     */
        /* all requested Mailbox Slots.                                   */
        if((MailboxHeader = (MailboxHeader_t *)QSOSAL_AllocateMemory(sizeof(MailboxHeader_t)+(NumberSlots*SlotSize))) != NULL)
        {
            /* Memory successfully allocated so now let's create an        */
            /* Event that will be used to signal when there is Data        */
            /* present in the Mailbox.                                     */
            if((MailboxHeader->Event = QSOSAL_CreateEvent(FALSE)) != NULL)
            {
                /* Event created successfully, now let's create a Mutex     */
                /* that will guard access to the Mailbox Slots.             */
                if((MailboxHeader->Mutex = QSOSAL_CreateMutex(FALSE)) != NULL)
                {
                    /* Everything successfully created, now let's initialize */
                    /* the state of the Mailbox such that it contains NO     */
                    /* Data.                                                 */
                    MailboxHeader->NumberSlots   = NumberSlots;
                    MailboxHeader->SlotSize      = SlotSize;
                    MailboxHeader->HeadSlot      = 0;
                    MailboxHeader->TailSlot      = 0;
                    MailboxHeader->OccupiedSlots = 0;
                    MailboxHeader->Slots         = ((unsigned char *)MailboxHeader) + sizeof(MailboxHeader_t);

                    /* All finished, return success to the caller (the       */
                    /* Mailbox Header).                                      */
                    ret_val                      = (Mailbox_t)StoreObject(MailboxHeader);

                    if(!ret_val)
                    {
                        /* Error storing the Mailbox object, so let's free the*/
                        /* Event and Mutex we created, Free the Memory for the*/
                        /* Mailbox itself and return an error to the caller.  */
                        QSOSAL_CloseMutex(MailboxHeader->Mutex);

                        QSOSAL_CloseEvent(MailboxHeader->Event);

                        QSOSAL_FreeMemory(MailboxHeader);
                    }
                }
                else
                {
                    /* Error creating the Mutex, so let's free the Event     */
                    /* we created, Free the Memory for the Mailbox itself    */
                    /* and return an error to the caller.                    */
                    QSOSAL_CloseEvent(MailboxHeader->Event);

                    QSOSAL_FreeMemory(MailboxHeader);

                    ret_val = NULL;
                }
            }
            else
            {
                /* Error creating the Data Available Event, so let's free   */
                /* the Memory for the Mailbox itself and return an error    */
                /* to the caller.                                           */
                QSOSAL_FreeMemory(MailboxHeader);

                ret_val = NULL;
            }
        }
        else
            ret_val = NULL;
    }
    else
        ret_val = NULL;

    /* Return the result to the caller.                                  */
    return(ret_val);
}

/* The following function is provided to allow a means to Add data to*/
/* the Mailbox (where it can be retrieved via the                    */
/* QSOSAL_WaitMailbox() function. This function accepts as input the */
/* Mailbox Handle of the Mailbox to place the data into and a pointer*/
/* to a buffer that contains the data to be added. This pointer      */
/* *MUST* point to a data buffer that is AT LEAST the Size of the    */
/* Slots in the Mailbox (specified when the Mailbox was created) and */
/* this pointer CANNOT be NULL. The data that the MailboxData pointer*/
/* points to is placed into the Mailbox where it can be retrieved via*/
/* the QSOSAL_WaitMailbox() function.                                */
/* * NOTE * This function copies from the MailboxData Pointer the    */
/*          first SlotSize Bytes. The SlotSize was specified when the*/
/*          Mailbox was created via a successful call to the         */
/*          QSOSAL_CreateMailbox() function.                         */
Boolean_t QSOSAL_AddMailbox(Mailbox_t Mailbox, void *MailboxData)
{
    Boolean_t        ret_val;
    MailboxHeader_t *MailboxHeader;

    /* Before proceeding any further make sure that the Mailbox Handle   */
    /* and the MailboxData pointer that was specified appears semi-valid.*/
    if((Mailbox) && (MailboxData) && ((MailboxHeader = (MailboxHeader_t *)RetrieveObject((uintptr_t)Mailbox)) != NULL))
    {
        /* Since we are going to change the Mailbox we need to acquire    */
        /* the Mutex that is guarding access to it.                       */
        if(QSOSAL_WaitMutex(MailboxHeader->Mutex, QSOSAL_INFINITE_WAIT))
        {
            /* Before adding the data to the Mailbox, make sure that the   */
            /* Mailbox is not already full.                                */
            if(MailboxHeader->OccupiedSlots < MailboxHeader->NumberSlots)
            {
                /* Mailbox is NOT full, so add the specified User Data to   */
                /* the next available free Mailbox Slot.                    */
                QSOSAL_MemCopy(&(((unsigned char *)(MailboxHeader->Slots))[MailboxHeader->HeadSlot*MailboxHeader->SlotSize]), MailboxData, MailboxHeader->SlotSize);

                /* Update the Next available Free Mailbox Slot (taking      */
                /* into account wrapping the pointer).                      */
                if(++(MailboxHeader->HeadSlot) == MailboxHeader->NumberSlots)
                    MailboxHeader->HeadSlot = 0;

                /* Update the Number of occupied slots to signify that there*/
                /* was additional Mailbox Data added to the Mailbox.        */
                MailboxHeader->OccupiedSlots++;

                /* Signal the Event that specifies that there is indeed     */
                /* Data present in the Mailbox.                             */
                QSOSAL_SetEvent(MailboxHeader->Event);

                /* Finally, return success to the caller.                   */
                ret_val = TRUE;
            }
            else
                ret_val = FALSE;

            /* All finished with the Mailbox, so release the Mailbox       */
            /* Mutex that we currently hold.                               */
            QSOSAL_ReleaseMutex(MailboxHeader->Mutex);
        }
        else
            ret_val = FALSE;

        ReleaseObject((uintptr_t)Mailbox);
    }
    else
        ret_val = FALSE;

    /* Return the result to the caller.                                  */
    return(ret_val);
}

/* The following function is provided to allow a means to retrieve   */
/* data from the specified Mailbox. This function will block until   */
/* either Data is placed in the Mailbox or an error with the Mailbox */
/* was detected. This function accepts as its first parameter a      */
/* Mailbox Handle that represents the Mailbox to wait for the data   */
/* with. This function accepts as its second parameter, a pointer to */
/* a data buffer that is AT LEAST the size of a single Slot of the   */
/* Mailbox (specified when the QSOSAL_CreateMailbox() function was   */
/* called). The MailboxData parameter CANNOT be NULL. This function  */
/* will return TRUE if data was successfully retrieved from the      */
/* Mailbox or FALSE if there was an error retrieving data from the   */
/* Mailbox. If this function returns TRUE then the first SlotSize    */
/* bytes of the MailboxData pointer will contain the data that was   */
/* retrieved from the Mailbox.                                       */
/* * NOTE * This function copies to the MailboxData Pointer the data */
/*          that is present in the Mailbox Slot (of size SlotSize).  */
/*          The SlotSize was specified when the Mailbox was created  */
/*          via a successful call to the QSOSAL_CreateMailbox()      */
/*          function.                                                */
Boolean_t QSOSAL_WaitMailbox(Mailbox_t Mailbox, void *MailboxData)
{
    Boolean_t        ret_val;
    MailboxHeader_t *MailboxHeader;

    /* Before proceeding any further make sure that the Mailbox Handle   */
    /* and the MailboxData pointer that was specified appears semi-valid.*/
    if((Mailbox) && (MailboxData) && ((MailboxHeader = (MailboxHeader_t *)RetrieveObject((uintptr_t)Mailbox)) != NULL))
    {
        /* Next, we need to block until there is at least one Mailbox     */
        /* Slot occupied by waiting on the Data Available Event.          */
        if(QSOSAL_WaitEvent(MailboxHeader->Event, QSOSAL_INFINITE_WAIT))
        {
            /* Since we are going to update the Mailbox, we need to acquire*/
            /* the Mutex that guards access to the Mailox.                 */
            if(QSOSAL_WaitMutex(MailboxHeader->Mutex, QSOSAL_INFINITE_WAIT))
            {
                /* Let's double check to make sure that there does indeed   */
                /* exist at least one slot with Mailbox Data present in it. */
                if(MailboxHeader->OccupiedSlots)
                {
                    /* Flag success to the caller.                           */
                    ret_val = TRUE;

                    /* Now copy the Data into the Memory Buffer specified    */
                    /* by the caller.                                        */
                    QSOSAL_MemCopy(MailboxData, &((((unsigned char *)MailboxHeader->Slots))[MailboxHeader->TailSlot*MailboxHeader->SlotSize]), MailboxHeader->SlotSize);

                    /* Now that we've copied the data into the Memory Buffer */
                    /* specified by the caller we need to mark the Mailbox   */
                    /* Slot as free.                                         */
                    if(++(MailboxHeader->TailSlot) == MailboxHeader->NumberSlots)
                        MailboxHeader->TailSlot = 0;

                    MailboxHeader->OccupiedSlots--;

                    /* If there is NO more data available in this Mailbox    */
                    /* then we need to flag it as such by Resetting the      */
                    /* Mailbox Data available Event.                         */
                    if(!MailboxHeader->OccupiedSlots)
                        QSOSAL_ResetEvent(MailboxHeader->Event);
                }
                else
                {
                    /* Internal error, flag that there is NO Data present    */
                    /* in the Mailbox (by resetting the Data Available       */
                    /* Event) and return failure to the caller.              */
                    QSOSAL_ResetEvent(MailboxHeader->Event);

                    ret_val = FALSE;
                }

                /* All finished with the Mailbox, so release the Mailbox    */
                /* Mutex that we currently hold.                            */
                QSOSAL_ReleaseMutex(MailboxHeader->Mutex);
            }
            else
                ret_val = FALSE;
        }
        else
            ret_val = FALSE;

        ReleaseObject((uintptr_t)Mailbox);
    }
    else
        ret_val = FALSE;

    /* Return the result to the caller.                                  */
    return(ret_val);
}

/* The following function is responsible for destroying a Mailbox    */
/* that was created successfully via a successful call to the        */
/* QSOSAL_CreateMailbox() function. This function accepts as input   */
/* the Mailbox Handle of the Mailbox to destroy. Once this function  */
/* is completed the Mailbox Handle is NO longer valid and CANNOT be  */
/* used. Calling this function will cause all outstanding            */
/* QSOSAL_WaitMailbox() functions to fail with an error. The final   */
/* parameter specifies an (optional) callback function that is called*/
/* for each queued Mailbox entry. This allows a mechanism to free any*/
/* resources that might be associated with each individual Mailbox   */
/* item.                                                             */
void QSOSAL_DeleteMailbox(Mailbox_t Mailbox, QSOSAL_MailboxDeleteCallback_t MailboxDeleteCallback)
{
    MailboxHeader_t *MailboxHeader;

    /* Before proceeding any further make sure that the Mailbox Handle   */
    /* that was specified appears semi-valid.                            */
    if((Mailbox) && ((MailboxHeader = (MailboxHeader_t *)ClearObject((uintptr_t)Mailbox)) != NULL))
    {
        /* Mailbox appears semi-valid, so let's free all Events and       */
        /* Mutexes, perform any mailbox deletion callback actions, and    */
        /* finally free the Memory associated with the Mailbox itself.    */
        /* ** NOTE ** Free the Mutex first, so that any threads which are */
        /*            awoken when the Event is destroyed will immediately */
        /*            detect that the Mailbox has been cleaned up.        */
        if(MailboxHeader->Mutex)
            QSOSAL_CloseMutex(MailboxHeader->Mutex);

        if(MailboxHeader->Event)
            QSOSAL_CloseEvent(MailboxHeader->Event);

        /* Check to see if a Mailbox Delete Item Callback was specified.  */
        if(MailboxDeleteCallback)
        {
            /* Now loop though all of the occupied slots and call the      */
            /* callback with the slot data.                                */
            while(MailboxHeader->OccupiedSlots)
            {
                (*MailboxDeleteCallback)(&((((unsigned char *)MailboxHeader->Slots))[MailboxHeader->TailSlot*MailboxHeader->SlotSize]));

                /* Now that we've called back with the data, we need to     */
                /* advance to the next slot.                                */
                if(++(MailboxHeader->TailSlot) == MailboxHeader->NumberSlots)
                    MailboxHeader->TailSlot = 0;

                /* Flag that there is one less occupied slot.               */
                MailboxHeader->OccupiedSlots--;
            }
        }

        /* All finished cleaning up the Mailbox, simply free all          */
        /* memory that was allocated for the Mailbox.                     */
        QSOSAL_FreeMemory(MailboxHeader);
    }
}

/* The following function is used to initialize the Platform module. */
/* The Platform module relies on some static variables that are used */
/* to coordinate the abstraction.  When the module is initially      */
/* started from a cold boot, all variables are set to the proper     */
/* state.  If the Warm Boot is required, then these variables need to*/
/* be reset to their default values.  This function sets all static  */
/* parameters to their default values.                               */
/* * NOTE * The implementation is free to pass whatever information  */
/*          required in this parameter.                              */
void QSOSAL_Init(void *UserParam)
{
    /* Verify that the parameter passed in appears valid.                */
    if(UserParam)
    {
    }

    /* Initialize the Debug Zones.                                       */
    DebugZoneMask = DBG_ZONE_ANY;
    DebugZoneMask = DBG_ZONE_CRITICAL_ERROR | DBG_ZONE_ENTER_EXIT | DBG_ZONE_GENERAL | DBG_ZONE_DEVELOPMENT;
}

/* The following function is used to cleanup the Platform module.    */
void QSOSAL_DeInit(void)
{
}

/* Write out the specified NULL terminated Debugging String to the   */
/* Debug output.                                                     */
void QSOSAL_OutputMessage(const char *DebugString, ...)
{
    char    DebugMsgBuffer[128];
    va_list args;

    /* Write out the Data.                                               */
    va_start(args, DebugString);
    vsprintf(DebugMsgBuffer, DebugString, args);

    ConsoleWrite(DebugMsgBuffer, (int)strlen(DebugMsgBuffer));

    va_end(args);
}

/* The following function is used to set the Debug Mask that controls*/
/* which debug zone messages get displayed.  The function takes as   */
/* its only parameter the Debug Mask value that is to be used.  Each */
/* bit in the mask corresponds to a debug zone.  When a bit is set,  */
/* the printing of that debug zone is enabled.                       */
void QSOSAL_SetDebugMask(unsigned long DebugMask)
{
    DebugZoneMask = DebugMask;
}

/* The following function is a utility function that can be used to  */
/* determine if a specified Zone is currently enabled for debugging. */
int QSOSAL_TestDebugZone(unsigned long Zone)
{
    return(DebugZoneMask & Zone);
}

/* The following function is responsible for displaying binary debug */
/* data.  The first parameter to this function is the length of data */
/* pointed to by the next parameter.  The final parameter is a       */
/* pointer to the binary data to be  displayed.                      */
int QSOSAL_DumpData(unsigned int DataLength, const unsigned char *DataPtr)
{
    int           ret_val;
    char          Buffer[80];
    char         *BufPtr;
    char         *HexBufPtr;
    uint8_t        DataByte;
    unsigned int  Index;
    static char   HexTable[] = "0123456789ABCDEF\n";
    static char   Header1[]  = "       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ";
    static char   Header2[]  = " -----------------------------------------------------------------------\n";

    /* Before proceeding any further, lets make sure that the parameters */
    /* passed to us appear semi-valid.                                   */
    if((DataLength > 0) && (DataPtr != NULL))
    {
        /* The parameters which were passed in appear to be at least      */
        /* semi-valid, next write the header out to the file.             */
        QSOSAL_OutputMessage(Header1);
        QSOSAL_OutputMessage(HexTable);
        QSOSAL_OutputMessage(Header2);

        /* Limit the number of bytes that will be displayed in the dump.  */
        if(DataLength > MAX_DBG_DUMP_BYTES)
        {
            DataLength = MAX_DBG_DUMP_BYTES;
        }

        /* Now that the Header is written out, let's output the Debug     */
        /* Data.                                                          */
        QSOSAL_MemInitialize(Buffer, ' ', sizeof(Buffer));
        BufPtr    = Buffer + QSOSAL_SprintF_S(Buffer, sizeof(Buffer), " %05X ", 0);
        HexBufPtr = Buffer + sizeof(Header1)-1;
        for(Index=0; Index < DataLength;)
        {
            Index++;
            DataByte     = *DataPtr++;
            *BufPtr++    = HexTable[(uint8_t)(DataByte >> 4)];
            *BufPtr      = HexTable[(uint8_t)(DataByte & 0x0F)];
            BufPtr      += 2;
            /* Check to see if this is a printable character and not a     */
            /* special reserved character.  Replace the non-printable      */
            /* characters with a period.                                   */
            *HexBufPtr++ = (char)(((DataByte >= ' ') && (DataByte <= '~') && (DataByte != '\\') && (DataByte != '%'))?DataByte:'.');
            if(((Index % MAXIMUM_BYTES_PER_ROW) == 0) || (Index == DataLength))
            {
                /* We are at the end of this line, so terminate the data    */
                /* compiled in the buffer and send it to the output device. */
                *HexBufPtr++ = '\n';
                *HexBufPtr   = 0;
                QSOSAL_OutputMessage(Buffer);
                if(Index != DataLength)
                {
                    /* We have more to process, so prepare for the next line.*/
                    QSOSAL_MemInitialize(Buffer, ' ', sizeof(Buffer));
                    BufPtr    = Buffer + QSOSAL_SprintF_S(Buffer, sizeof(Buffer), " %05X ", Index);
                    HexBufPtr = Buffer + sizeof(Header1)-1;
                }
                else
                {
                    /* Flag that there is no more data.                      */
                    HexBufPtr = NULL;
                }
            }
        }
        /* Check to see if there is partial data in the buffer.           */
        if(HexBufPtr > 0)
        {
            /* Terminate the buffer and output the line.                   */
            *HexBufPtr++ = '\n';
            *HexBufPtr   = 0;
            QSOSAL_OutputMessage(Buffer);
        }
        QSOSAL_OutputMessage("\n");

        /* Finally, set the return value to indicate success.             */
        ret_val = 0;
    }
    else
    {
        /* An invalid parameter was enterer.                              */
        ret_val = -1;
    }
    return(ret_val);
}

