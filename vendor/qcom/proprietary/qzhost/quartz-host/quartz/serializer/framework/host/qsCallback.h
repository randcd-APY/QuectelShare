/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifdef CONFIG_DAEMON_MODE

#ifndef __QS_CALLBACK_H__
#define __QS_CALLBACK_H__

#include "qsCommon.h"

/**
   Information for a callback function registered with this module.
*/
typedef struct CallbackInfo_s
{
   uint16_t  TargetID;       /** ID of target device */
   uint16_t  ModuleID;       /** ID of the module, identical for all callbacks in a module. */
   uint16_t  FileID;         /** ID of the file, identical for all callbacks in a file. */
   uint16_t  CallbackID;     /** ID of the callback, identical for all callbacks of one type. */
   uint32_t  CallbackKey;    /** Callback key, generally returned by a registration function. */
   void     *AppFunction;    /** Application callback function. */
   uint32_t  AppParam;       /** Application callback parameter. */
} CallbackInfo_t;

/**
   Definition of a function which handles parsing the callback.

   @param CallbackHandle Handle of the callback info.
   @param CallbackInfo   Information for the callback function.
   @param BufferLength   Length of the packed data.
   @param Buffer         Buffer of the packed data for the callback.
*/

typedef void (*CallbackParser_t)(uint32_t CallbackHandle, CallbackInfo_t *CallbackInfo, uint16_t BufferLength, uint8_t *Buffer);

/**
   Initiializes the callback module.

   @return the status result of the parser initialization.
*/
SerStatus_t Callback_Initialize(void);

/**
   Shuts down the callback module.
*/
void Callback_Shutdown(void);

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
SerStatus_t Callback_Register(uint32_t *CallbackHandle, CallbackParser_t CallbackParser, const CallbackInfo_t *CallbackInfo);

/**
   Updates the callback key for a registered callback function.

   @param CallbackHandle Callback handle returned from successful registration.
   @param CallbackKey    The new value for the callback key.

   @return ssSuccess if the callback was found successfully or a negative value
           if there was an error.
*/
SerStatus_t Callback_UpdateKey(uint32_t CallbackHandle, uint32_t CallbackKey);

/**
   Retrieves the callback info for the specified handle.

   @param CallbackHandle Callback handle returned from successful registration.
   @param CallbackInfo   Information for the callback.

   @return ssSuccess if the callback was found successfully or a negative value
           if there was an error.
*/
SerStatus_t Callback_Retrieve(uint32_t CallbackHandle, CallbackInfo_t *CallbackInfo);

/**
   Unregisters a callback function based on the callback key and identifiers.

   @param ModuleID    ID of the callback's module to unregister.
   @param FileID      ID of the callback's file to unregister.
   @param CallbackID  ID of the callback's type to unregister.
   @param CallbackKey Key specific to the callback to unregister.
*/
void Callback_UnregisterByKey(uint16_t ModuleID, uint16_t FileID, uint16_t CallbackID, uint32_t CallbackKey);

/**
   Unregisters a callback function based on the CallbackHandle returned from
   Callback_Register().

   @param CallbackHandle Handle of the callback to unregister.
*/
void Callback_UnregisterByHandle(uint32_t CallbackHandle);

/**
   Unregisters callback functions based on the module. All callbacks
   registered for the module and instance will be unregistered.

   @param ModuleID ID of the module of the callbacks to unregister.
*/
void Callback_UnregisterByModule(uint16_t ModuleID);

/**
   Processes callback data received from a remote device. It will invoke the
   correct parser for the callback function.

   @param ModuleID     ID of the callback's module.
   @param FileID       ID of the callback's file.
   @param EventID      ID of the callback's event.
   @param BufferLength Length of the buffer that was received.
   @param Buffer       Data that was received.  Note that it is expected that
                       the callback ID is packed into the begining of this
                       buffer.
*/
void Callback_Process(uint8_t ModuleID, uint8_t FileID, uint16_t EventID, uint32_t BufferLength, uint8_t *Buffer);

/**
   Locks the callback processing preventing any future callbacks from being
   issued to the module until the unlocked.

   Note, this function may prevent callbacks to all modules, not just the
   specified module.

   @param ModuleID ID of the callback's module to lock.

   @return True if the lock was aquired successfully or false if there was an
           error.
*/
Boolean_t Callback_Lock(uint8_t ModuleID);

/**
   Unlocks callback processing for a module.

   @param ModuleID ID of the callback's module to unlock.
*/
void Callback_Unlock(uint8_t ModuleID);

#endif // __QS_CALLBACK_H__

#endif /* CONFIG_DAEMON_MODE */

