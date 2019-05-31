/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $

******************************************************************************/

#ifndef _CONNX_LOG_H_
#define _CONNX_LOG_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENABLE_CONNX_LOG

#define DEBUG_ERROR             (0x00000001)
#define DEBUG_WARN              (0x00000002)
#define DEBUG_MESSAGE           (0x00000004)
#define DEBUG_OUTPUT            (0x00000008)

#define DEBUG_BT_ERROR          (0x00000010)
#define DEBUG_BT_WARN           (0x00000020)
#define DEBUG_BT_MESSAGE        (0x00000040)
#define DEBUG_BT_OUTPUT         (0x00000080)

#define DEBUG_HFG_ERROR         (0x00000100)
#define DEBUG_HFG_WARN          (0x00000200)
#define DEBUG_HFG_MESSAGE       (0x00000400)
#define DEBUG_HFG_OUTPUT        (0x00000800)

#define DEBUG_IPC_ERROR         (0x00001000)
#define DEBUG_IPC_WARN          (0x00002000)
#define DEBUG_IPC_MESSAGE       (0x00004000)
#define DEBUG_IPC_OUTPUT        (0x00008000)

#define OUTPUT_MODE_NONE        (0x00000000)
#define OUTPUT_MODE_CONSOLE     (0x00000001)
#define OUTPUT_MODE_FILE        (0x00000002)

#define DEFAULT_OUTPUT_MODE     (OUTPUT_MODE_CONSOLE)

#define DEFAULT_OUTPUT_MASK     (DEBUG_ERROR | DEBUG_WARN | DEBUG_MESSAGE | \
                                 DEBUG_BT_ERROR | DEBUG_BT_WARN | DEBUG_BT_MESSAGE | DEBUG_BT_OUTPUT | \
                                 DEBUG_HFG_ERROR | DEBUG_HFG_WARN | DEBUG_HFG_MESSAGE | \
                                 DEBUG_IPC_ERROR | DEBUG_IPC_WARN | DEBUG_IPC_MESSAGE)

#define TEXT(str)               (char *)(str)


#if defined(ENABLE_CONNX_LOG) || defined(ENABLE_LOG) || defined(_DEBUG)

void DumpBuff(uint32_t mask, const uint8_t *buffer, uint32_t bufferSize);
void DebugOut(uint32_t mask, char *format, ...);
void DebugInitialize(uint32_t outputMode, uint32_t outputMask, const char *outputFilename);
void DebugFlushBuffers(void);
void DebugUninitialize(void);

#define IFLOG(c)    c

#else

void DumpBuff(uint32_t mask, const uint8_t *buffer, uint32_t bufferSize) {}
void DebugOut(uint32_t mask, char *format, ...) {}
void DebugInitialize(uint32_t outputMode, uint32_t outputMask, const char *outputFilename) {}
void DebugFlushBuffers(void) {}
void DebugUninitialize(void) {}

#define IFLOG(c)

#endif

#define FUNC_ENTER_STR          "+++"
#define FUNC_LEAVE_STR          "---"

#define FUNC_ENTER()            IFLOG(DebugOut(DEBUG_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, FUNC_ENTER_STR))
#define FUNC_LEAVE()            IFLOG(DebugOut(DEBUG_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, FUNC_LEAVE_STR))

#define BT_FUNC_ENTER()         IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, FUNC_ENTER_STR))
#define BT_FUNC_LEAVE()         IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, FUNC_LEAVE_STR))

#define HFG_FUNC_ENTER()        IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, FUNC_ENTER_STR))
#define HFG_FUNC_LEAVE()        IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> %s"), __FUNCTION__, FUNC_LEAVE_STR))


#ifdef __cplusplus
extern "C" }
#endif

#endif  /* _CONNX_LOG_H_ */