/*****< QSOSALapi.h >**********************************************************/
/*      Copyright 2000 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  QSOSALAPI - Stonestreet One Kernel API Type Definitions,   				   */
/*            Constants, and Prototypes.                                      */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   05/30/01  D. Lange       Initial creation.                               */
/*   10/24/17  R. McCord      Added replacements for banned functions.        */
/******************************************************************************/
#ifndef __QSOSALAPIH__
#define __QSOSALAPIH__

#include <stdio.h>              /* sprintf(), snprintf() prototypes.          */
#include <stdint.h>

   /* Miscellaneous Type definitions that should already be defined,    */
   /* but are necessary.                                                */
#ifndef NULL
   #define NULL ((void *)0)
#endif

#ifndef TRUE
   #define TRUE (1 == 1)
#endif

#ifndef FALSE
   #define FALSE (0 == 1)
#endif

   /* The following preprocessor definitions control the inclusion of   */
   /* debugging output.                                                 */
   /*                                                                   */
   /*    - DEBUG_ENABLED                                                */
   /*         - When defined enables debugging, if no other debugging   */
   /*           preprocessor definitions are defined then the debugging */
   /*           output is logged to a file (and included in the         */
   /*           driver).                                                */
   /*                                                                   */
   /*          - DEBUG_ZONES                                            */
   /*              - When defined (only when DEBUG_ENABLED is defined)  */
   /*                forces the value of this definition (unsigned long)*/
   /*                to be the Debug Zones that are enabled.            */
#define DBG_ZONE_CRITICAL_ERROR           (1 << 0)
#define DBG_ZONE_ENTER_EXIT               (1 << 1)
#define DBG_ZONE_QSOSAL                   (1 << 2)
#define DBG_ZONE_GENERAL                  (1 << 3)
#define DBG_ZONE_DEVELOPMENT              (1 << 4)
#define DBG_ZONE_VENDOR                   (1 << 5)

#define DBG_ZONE_ANY                      ((unsigned long)-1)

#ifndef DEBUG_ZONES
   #define DEBUG_ZONES                    DBG_ZONE_CRITICAL_ERROR
#endif

#ifndef MAX_DBG_DUMP_BYTES
   #define MAX_DBG_DUMP_BYTES             (((unsigned int)-1) - 1)
#endif

#ifdef DEBUG_ENABLED
   #define DBG_MSG(_zone_, _x_)           do { if(QSOSAL_TestDebugZone(_zone_)) QSOSAL_OutputMessage _x_; } while(0)
   #define DBG_DUMP(_zone_, _x_)          do { if(QSOSAL_TestDebugZone(_zone_)) QSOSAL_DumpData _x_; } while(0)
#else
   #define DBG_MSG(_zone_, _x_)
   #define DBG_DUMP(_zone_, _x_)
#endif

   /* The following constant defines a special length of time that      */
   /* specifies that there is to be NO Timeout waiting for some Event to*/
   /* occur (Mutexes, Semaphores, Events, etc).                         */
#define QSOSAL_INFINITE_WAIT                              (0xFFFFFFFF)

#define QSOSAL_NO_WAIT                                     0

   /* The following type definition defines a QSOSAL Kernel API Event     */
   /* Handle.                                                           */
typedef void *Event_t;

   /* The following type definition defines a QSOSAL Kernel API Mutex     */
   /* Handle.                                                           */
typedef void *Mutex_t;

   /* The following type definition defines a QSOSAL Kernel API Thread    */
   /* Handle.                                                           */
typedef void *ThreadHandle_t;

   /* The following type definition defines a QSOSAL Kernel API Mailbox   */
   /* Handle.                                                           */
typedef void *Mailbox_t;

	/* The following defines a QSOSAL Boolean type.						*/
typedef uint8_t Boolean_t;

   /* The following MACRO is a utility MACRO that exists to calculate   */
   /* the offset position of a particular structure member from the     */
   /* start of the structure.  This MACRO accepts as the first          */
   /* parameter, the physical name of the structure (the type name, NOT */
   /* the variable name).  The second parameter to this MACRO represents*/
   /* the actual structure member that the offset is to be determined.  */
   /* This MACRO returns an unsigned integer that represents the offset */
   /* (in bytes) of the structure member.                               */
#ifdef STRUCTURE_OFFSET
#define QSOSAL_STRUCTURE_OFFSET(_x, _y)              STRUCTURE_OFFSET(_x,_y)
#else
#define QSOSAL_STRUCTURE_OFFSET(_x, _y)              ((unsigned int )&(((_x *)0)->_y))
#endif

   /* The following type declaration represents the Prototype for a     */
   /* Thread Function.  This function represents the Thread that will be*/
   /* executed when passed to the QSOSAL_CreateThread() function.         */
   /* * NOTE * Once a Thread is created there is NO way to kill it.  The*/
   /*          Thread must exit by itself.                              */
typedef void *(*Thread_t)(void *ThreadParameter);

   /* The following type declaration represents the Prototype for the   */
   /* function that is passed to the QSOSAL_DeleteMailbox() function to   */
   /* process all remaining Queued Mailbox Messages.  This allows a     */
   /* mechanism to free any resources that are attached with each queued*/
   /* Mailbox message.                                                  */
typedef void (*QSOSAL_MailboxDeleteCallback_t)(void *MailboxData);

#ifdef _QSOSAL_KERNEL_USE_DEPRECATED_FUNCTIONS

   /* The following functions are deprecated.                           */
   /* * NOTE * The following mappings have been added in order to be    */
   /*          backwards compatible with older stack versions.          */
#define QSOSAL_SprintF sprintf
#define QSOSAL_StringCopy strcpy

#endif

   /* The following function is responsible for delaying the current    */
   /* task for the specified duration (specified in Milliseconds).      */
   /* * NOTE * Very small timeouts might be smaller in granularity than */
   /*          the system can support !!!!                              */
void QSOSAL_Delay(unsigned long MilliSeconds);

   /* The following function is responsible for creating an actual Mutex*/
   /* (Binary Semaphore).  The Mutex is unique in that if a Thread      */
   /* already owns the Mutex, and it requests the Mutex again it will be*/
   /* granted the Mutex.  This is in Stark contrast to a Semaphore that */
   /* will block waiting for the second acquisition of the Sempahore.   */
   /* This function accepts as input whether or not the Mutex is        */
   /* initially Signalled or not.  If this input parameter is TRUE then */
   /* the caller owns the Mutex and any other threads waiting on the    */
   /* Mutex will block.  This function returns a NON-NULL Mutex Handle  */
   /* if the Mutex was successfully created, or a NULL Mutex Handle if  */
   /* the Mutex was NOT created.  If a Mutex is successfully created, it*/
   /* can only be destroyed by calling the QSOSAL_CloseMutex() function   */
   /* (and passing the returned Mutex Handle).                          */
Mutex_t QSOSAL_CreateMutex(Boolean_t CreateOwned);

   /* The following function is responsible for waiting for the         */
   /* specified Mutex to become free.  This function accepts as input   */
   /* the Mutex Handle to wait for, and the Timeout (specified in       */
   /* Milliseconds) to wait for the Mutex to become available.  This    */
   /* function returns TRUE if the Mutex was successfully acquired and  */
   /* FALSE if either there was an error OR the Mutex was not acquired  */
   /* in the specified Timeout.  It should be noted that Mutexes have   */
   /* the special property that if the calling Thread already owns the  */
   /* Mutex and it requests access to the Mutex again (by calling this  */
   /* function and specifying the same Mutex Handle) then it will       */
   /* automatically be granted the Mutex.  Once a Mutex has been granted*/
   /* successfully (this function returns TRUE), then the caller MUST   */
   /* call the QSOSAL_ReleaseMutex() function.                            */
   /* * NOTE * There must exist a corresponding QSOSAL_ReleaseMutex()     */
   /*          function call for EVERY successful QSOSAL_WaitMutex()      */
   /*          function call or a deadlock will occur in the system !!! */
Boolean_t QSOSAL_WaitMutex(Mutex_t Mutex, unsigned long Timeout);

   /* The following function is responsible for releasing a Mutex that  */
   /* was successfully acquired with the QSOSAL_WaitMutex() function.     */
   /* This function accepts as input the Mutex that is currently owned. */
   /* * NOTE * There must exist a corresponding QSOSAL_ReleaseMutex()     */
   /*          function call for EVERY successful QSOSAL_WaitMutex()      */
   /*          function call or a deadlock will occur in the system !!! */
void QSOSAL_ReleaseMutex(Mutex_t Mutex);

   /* The following function is responsible for destroying a Mutex that */
   /* was created successfully via a successful call to the             */
   /* QSOSAL_CreateMutex() function.  This function accepts as input the  */
   /* Mutex Handle of the Mutex to destroy.  Once this function is      */
   /* completed the Mutex Handle is NO longer valid and CANNOT be used. */
   /* Calling this function will cause all outstanding QSOSAL_WaitMutex() */
   /* functions to fail with an error.                                  */
void QSOSAL_CloseMutex(Mutex_t Mutex);

   /* The following function is responsible for creating an actual      */
   /* Event.  The Event is unique in that it only has two states.  These*/
   /* states are Signalled and Non-Signalled.  Functions are provided to*/
   /* allow the setting of the current state and to allow the option of */
   /* waiting for an Event to become Signalled.  This function accepts  */
   /* as input whether or not the Event is initially Signalled or not.  */
   /* If this input parameter is TRUE then the state of the Event is    */
   /* Signalled and any QSOSAL_WaitEvent() function calls will immediately*/
   /* return.  This function returns a NON-NULL Event Handle if the     */
   /* Event was successfully created, or a NULL Event Handle if the     */
   /* Event was NOT created.  If an Event is successfully created, it   */
   /* can only be destroyed by calling the QSOSAL_CloseEvent() function   */
   /* (and passing the returned Event Handle).                          */
Event_t QSOSAL_CreateEvent(Boolean_t CreateSignalled);

   /* The following function is responsible for waiting for the         */
   /* specified Event to become Signalled.  This function accepts as    */
   /* input the Event Handle to wait for, and the Timeout (specified in */
   /* Milliseconds) to wait for the Event to become Signalled.  This    */
   /* function returns TRUE if the Event was set to the Signalled State */
   /* (in the Timeout specified) or FALSE if either there was an error  */
   /* OR the Event was not set to the Signalled State in the specified  */
   /* Timeout.  It should be noted that Signals have a special property */
   /* in that multiple Threads can be waiting for the Event to become   */
   /* Signalled and ALL calls to QSOSAL_WaitEvent() will return TRUE      */
   /* whenever the state of the Event becomes Signalled.                */
Boolean_t QSOSAL_WaitEvent(Event_t Event, unsigned long Timeout);

   /* The following function is responsible for changing the state of   */
   /* the specified Event to the Non-Signalled State.  Once the Event is*/
   /* in this State, ALL calls to the QSOSAL_WaitEvent() function will    */
   /* block until the State of the Event is set to the Signalled State. */
   /* This function accepts as input the Event Handle of the Event to   */
   /* set to the Non-Signalled State.                                   */
void QSOSAL_ResetEvent(Event_t Event);

   /* The following function is responsible for changing the state of   */
   /* the specified Event to the Signalled State.  Once the Event is in */
   /* this State, ALL calls to the QSOSAL_WaitEvent() function will       */
   /* return.  This function accepts as input the Event Handle of the   */
   /* Event to set to the Signalled State.                              */
void QSOSAL_SetEvent(Event_t Event);

   /* The following function is responsible for destroying an Event that*/
   /* was created successfully via a successful call to the             */
   /* QSOSAL_CreateEvent() function.  This function accepts as input the  */
   /* Event Handle of the Event to destroy.  Once this function is      */
   /* completed the Event Handle is NO longer valid and CANNOT be used. */
   /* Calling this function will cause all outstanding QSOSAL_WaitEvent() */
   /* functions to fail with an error.                                  */
void QSOSAL_CloseEvent(Event_t Event);

   /* The following function is provided to allow a mechanism to        */
   /* actually allocate a Block of Memory (of at least the specified    */
   /* size).  This function accepts as input the size (in Bytes) of the */
   /* Block of Memory to be allocated.  This function returns a NON-NULL*/
   /* pointer to this Memory Buffer if the Memory was successfully      */
   /* allocated, or a NULL value if the memory could not be allocated.  */
void *QSOSAL_AllocateMemory(unsigned long MemorySize);

   /* The following function is responsible for de-allocating a Block of*/
   /* Memory that was successfully allocated with the                   */
   /* QSOSAL_AllocateMemory() function.  This function accepts a NON-NULL */
   /* Memory Pointer which was returned from the QSOSAL_AllocateMemory()  */
   /* function.  After this function completes the caller CANNOT use ANY*/
   /* of the Memory pointed to by the Memory Pointer.                   */
void QSOSAL_FreeMemory(void *MemoryPointer);

   /* The following function is responsible for copying a block of      */
   /* memory of the specified size from the specified source pointer    */
   /* to the specified destination memory pointer.  This function       */
   /* accepts as input a pointer to the memory block that is to be      */
   /* Destination Buffer (first parameter), a pointer to memory block   */
   /* that points to the data to be copied into the destination buffer, */
   /* and the size (in bytes) of the Data to copy.  The Source and      */
   /* Destination Memory Buffers must contain AT LEAST as many bytes    */
   /* as specified by the Size parameter.                               */
   /* ** NOTE ** This function is deprecated and should NOT be used.    */
   /*            This function has been replaced by the QSOSAL_MemCopy_S()*/
   /*            function.                                              */
   /* * NOTE * This function does not allow the overlapping of the      */
   /*          Source and Destination Buffers !!!!                      */
void QSOSAL_MemCopy(void *Destination, const void *Source, unsigned long Size);

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
   /*          required, then the QSOSAL_MemMove_S() function MUST be     */
   /*          used.                                                    */
void QSOSAL_MemCopy_S(void *Destination, unsigned long DestSize, const void *Source, unsigned long Count);

   /* The following function is responsible for moving a block of       */
   /* memory of the specified size from the specified source pointer    */
   /* to the specified destination memory pointer.  This function       */
   /* accepts as input a pointer to the memory block that is to be      */
   /* Destination Buffer (first parameter), a pointer to memory block   */
   /* that points to the data to be copied into the destination buffer, */
   /* and the size (in bytes) of the Data to copy.  The Source and      */
   /* Destination Memory Buffers must contain AT LEAST as many bytes    */
   /* as specified by the Size parameter.                               */
   /* ** NOTE ** This function is deprecated and should NOT be used.    */
   /*            This function has been replaced by the QSOSAL_MemMove_S()*/
   /*            function.                                              */
   /* * NOTE * This function DOES allow the overlapping of the          */
   /*          Source and Destination Buffers.                          */
void QSOSAL_MemMove(void *Destination, const void *Source, unsigned long Size);

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
   /*          QSOSAL_MemCopy_S() function.                               */
void QSOSAL_MemMove_S(void *Destination, unsigned long DestSize, const void *Source, unsigned long Count);

   /* The following function is provided to allow a mechanism to fill a */
   /* block of memory with the specified value.  This function accepts  */
   /* as input a pointer to the Data Buffer (first parameter) that is to*/
   /* filled with the specified value (second parameter).  The final    */
   /* parameter to this function specifies the number of bytes that are */
   /* to be filled in the Data Buffer.  The Destination Buffer must     */
   /* point to a Buffer that is AT LEAST the size of the Size parameter.*/
void QSOSAL_MemInitialize(void *Destination, unsigned char Value, unsigned long Size);

   /* The following function is provided to allow a mechanism to Compare*/
   /* two blocks of memory to see if the two memory blocks (each of size*/
   /* Size (in bytes)) are equal (each and every byte up to Size bytes).*/
   /* This function returns a negative number if Source1 is less than   */
   /* Source2, zero if Source1 equals Source2, and a positive value if  */
   /* Source1 is greater than Source2.                                  */
int QSOSAL_MemCompare(const void *Source1, const void *Source2, unsigned long Size);

   /* The following function is provided to allow a mechanism to Compare*/
   /* two blocks of memory to see if the two memory blocks (each of size*/
   /* Size (in bytes)) are equal (each and every byte up to Size bytes) */
   /* using a Case-Insensitive Compare.  This function returns a        */
   /* negative number if Source1 is less than Source2, zero if Source1  */
   /* equals Source2, and a positive value if Source1 is greater than   */
   /* Source2.                                                          */
int QSOSAL_MemCompareI(const void *Source1, const void *Source2, unsigned long Size);

   /* The following function is provided to allow a mechanism to copy a */
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
void QSOSAL_StringCopy_S(char *Destination, unsigned long DestSize, const char *Source);

   /* The following function is provided to allow a mechanism to        */
   /* determine the Length (in characters) of the specified NULL        */
   /* Terminated ASCII (character) String.  This function accepts as    */
   /* input a pointer to a NULL Terminated ASCII String and returns the */
   /* number of characters present in the string (NOT including the     */
   /* terminating NULL character).                                      */
unsigned int QSOSAL_StringLength(const char *Source);

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
int QSOSAL_SprintF_S(char *Destination, unsigned long DestSize, const char *Format, ...);

   /* The following function is provided to allow a means for the       */
   /* programmer to create a separate thread of execution.  This        */
   /* function accepts as input the Function that represents the Thread */
   /* that is to be installed into the system as its first parameter.   */
   /* The second parameter is the size of the Threads Stack (in bytes)  */
   /* required by the Thread when it is executing.  The final parameter */
   /* to this function represents a parameter that is to be passed to   */
   /* the Thread when it is created.  This function returns a NON-NULL  */
   /* Thread Handle if the Thread was successfully created, or a NULL   */
   /* Thread Handle if the Thread was unable to be created.  Once the   */
   /* thread is created, the only way for the Thread to be removed from */
   /* the system is for the Thread function to run to completion.       */
   /* * NOTE * There does NOT exist a function to Kill a Thread that is */
   /*          present in the system.  Because of this, other means     */
   /*          needs to be devised in order to signal the Thread that it*/
   /*          is to terminate.                                         */
ThreadHandle_t QSOSAL_CreateThread(Thread_t ThreadFunction, unsigned int StackSize, void *ThreadParameter);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef ThreadHandle_t (*PFN_QSOSAL_CreateThread_t)(Thread_t ThreadFunction, unsigned int StackSize, void *ThreadParameter);
#endif

   /* The following function is provided to allow a mechanism to        */
   /* retrieve the handle of the thread which is currently executing.   */
   /* This function require no input parameters and will return a valid */
   /* ThreadHandle upon success.                                        */
ThreadHandle_t QSOSAL_CurrentThreadHandle(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef ThreadHandle_t (*PFN_QSOSAL_CurrentThreadHandle_t)(void);
#endif

   /* The following function is provided to allow a mechanism to create */
   /* a Mailbox.  A Mailbox is a Data Store that contains slots (all of */
   /* the same size) that can have data placed into (and retrieved      */
   /* from).  Once Data is placed into a Mailbox (via the               */
   /* QSOSAL_AddMailbox() function, it can be retrieved by using the      */
   /* QSOSAL_WaitMailbox() function.  Data placed into the Mailbox is     */
   /* retrieved in a FIFO method.  This function accepts as input the   */
   /* Maximum Number of Slots that will be present in the Mailbox and   */
   /* the Size of each of the Slots.  This function returns a NON-NULL  */
   /* Mailbox Handle if the Mailbox is successfully created, or a NULL  */
   /* Mailbox Handle if the Mailbox was unable to be created.           */
Mailbox_t QSOSAL_CreateMailbox(unsigned int NumberSlots, unsigned int SlotSize);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Mailbox_t (*PFN_QSOSAL_CreateMailbox_t)(unsigned int NumberSlots, unsigned int SlotSize);
#endif

   /* The following function is provided to allow a means to Add data to*/
   /* the Mailbox (where it can be retrieved via the QSOSAL_WaitMailbox() */
   /* function.  This function accepts as input the Mailbox Handle of   */
   /* the Mailbox to place the data into and a pointer to a buffer that */
   /* contains the data to be added.  This pointer *MUST* point to a    */
   /* data buffer that is AT LEAST the Size of the Slots in the Mailbox */
   /* (specified when the Mailbox was created) and this pointer CANNOT  */
   /* be NULL.  The data that the MailboxData pointer points to is      */
   /* placed into the Mailbox where it can be retrieved via the         */
   /* QSOSAL_WaitMailbox() function.                                      */
   /* * NOTE * This function copies from the MailboxData Pointer the    */
   /*          first SlotSize Bytes.  The SlotSize was specified when   */
   /*          the Mailbox was created via a successful call to the     */
   /*          QSOSAL_CreateMailbox() function.                           */
Boolean_t QSOSAL_AddMailbox(Mailbox_t Mailbox, void *MailboxData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (*PFN_QSOSAL_AddMailbox_t)(Mailbox_t Mailbox, void *MailboxData);
#endif

   /* The following function is provided to allow a means to retrieve   */
   /* data from the specified Mailbox.  This function will block until  */
   /* either Data is placed in the Mailbox or an error with the Mailbox */
   /* was detected.  This function accepts as its first parameter a     */
   /* Mailbox Handle that represents the Mailbox to wait for the data   */
   /* with.  This function accepts as its second parameter, a pointer to*/
   /* a data buffer that is AT LEAST the size of a single Slot of the   */
   /* Mailbox (specified when the QSOSAL_CreateMailbox() function was     */
   /* called).  The MailboxData parameter CANNOT be NULL.  This function*/
   /* will return TRUE if data was successfully retrieved from the      */
   /* Mailbox or FALSE if there was an error retrieving data from the   */
   /* Mailbox.  If this function returns TRUE then the first SlotSize   */
   /* bytes of the MailboxData pointer will contain the data that was   */
   /* retrieved from the Mailbox.                                       */
   /* * NOTE * This function copies to the MailboxData Pointer the data */
   /*          that is present in the Mailbox Slot (of size SlotSize).  */
   /*          The SlotSize was specified when the Mailbox was created  */
   /*          via a successful call to the QSOSAL_CreateMailbox()        */
   /*          function.                                                */
Boolean_t QSOSAL_WaitMailbox(Mailbox_t Mailbox, void *MailboxData);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef Boolean_t (*PFN_QSOSAL_WaitMailbox_t)(Mailbox_t Mailbox, void *MailboxData);
#endif

   /* The following function is responsible for destroying a Mailbox    */
   /* that was created successfully via a successful call to the        */
   /* QSOSAL_CreateMailbox() function.  This function accepts as input the*/
   /* Mailbox Handle of the Mailbox to destroy.  Once this function is  */
   /* completed the Mailbox Handle is NO longer valid and CANNOT be     */
   /* used.  Calling this function will cause all outstanding           */
   /* QSOSAL_WaitMailbox() functions to fail with an error.  The final    */
   /* parameter specifies an (optional) callback function that is called*/
   /* for each queued Mailbox entry.  This allows a mechanism to free   */
   /* any resources that might be associated with each individual       */
   /* Mailbox item.                                                     */
void QSOSAL_DeleteMailbox(Mailbox_t Mailbox, QSOSAL_MailboxDeleteCallback_t MailboxDeleteCallback);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (*PFN_QSOSAL_DeleteMailbox_t)(Mailbox_t Mailbox, QSOSAL_MailboxDeleteCallback_t MailboxDeleteCallback);
#endif

   /* The following function is used to initialize the Platform module. */
   /* The Platform module relies on some static variables that are used */
   /* to coordinate the abstraction.  When the module is initially      */
   /* started from a cold boot, all variables are set to the proper     */
   /* state.  If the Warm Boot is required, then these variables need to*/
   /* be reset to their default values.  This function sets all static  */
   /* parameters to their default values.                               */
   /* * NOTE * The implementation is free to pass whatever information  */
   /*          required in this parameter.                              */
void QSOSAL_Init(void *UserParam);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (*PFN_QSOSAL_Init_t)(void *UserParam);
#endif

   /* The following function is used to cleanup the Platform module.    */
void QSOSAL_DeInit(void);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (*PFN_QSOSAL_DeInit_t)(void);
#endif

   /* Write out the specified NULL terminated Debugging String to the   */
   /* Debug output.                                                     */
void QSOSAL_OutputMessage(const char *DebugString, ...);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (*PFN_QSOSAL_OutputMessage_t)(const char *DebugString, ...);
#endif

   /* The following function is used to set the Debug Mask that controls*/
   /* which debug zone messages get displayed.  The function takes as   */
   /* its only parameter the Debug Mask value that is to be used.  Each */
   /* bit in the mask corresponds to a debug zone.  When a bit is set,  */
   /* the printing of that debug zone is enabled.                       */
void QSOSAL_SetDebugMask(unsigned long DebugMask);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef void (*PFN_QSOSAL_SetDebugMask_t)(unsigned long DebugMask);
#endif

   /* The following function is a utility function that can be used to  */
   /* determine if a specified Zone is currently enabled for debugging. */
int QSOSAL_TestDebugZone(unsigned long Zone);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (*PFN_QSOSAL_TestDebugZone_t)(unsigned long Zone);
#endif

   /* The following function is responsible for displaying binary debug */
   /* data.  The first parameter to this function is the length of data */
   /* pointed to by the next parameter.  The final parameter is a       */
   /* pointer to the binary data to be displayed.                       */
int QSOSAL_DumpData(unsigned int DataLength, const unsigned char *DataPtr);

#ifdef INCLUDE_BLUETOOTH_API_PROTOTYPES
   typedef int (*PFN_QSOSAL_DumpData_t)(unsigned int DataLength, const unsigned char *DataPtr);
#endif

#endif

