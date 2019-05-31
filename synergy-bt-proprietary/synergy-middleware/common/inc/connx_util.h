/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_UTIL_H_
#define _CONNX_UTIL_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Thread priorities */
#define CONNX_THREAD_PRIORITY_HIGHEST           ((uint16_t) 0)
#define CONNX_THREAD_PRIORITY_HIGH              ((uint16_t) 1)
#define CONNX_THREAD_PRIORITY_NORMAL            ((uint16_t) 2)
#define CONNX_THREAD_PRIORITY_LOW               ((uint16_t) 3)
#define CONNX_THREAD_PRIORITY_LOWEST            ((uint16_t) 4)

#define SET_DWORD(low_word, high_word)          ((uint32_t) (low_word) | ((uint32_t) (high_word) << 16))
#define GET_LOWORD(val)                         ((uint16_t) ((val) & 0x0000FFFF))
#define GET_HIWORD(val)                         ((uint16_t) (((val) & 0xFFFF0000) >> 16))

#define SET_CONNX_EVENT_DATA(event, flag)       SET_DWORD(event, flag)
#define GET_CONNX_EVENT(val)                    GET_LOWORD(val)
#define GET_CONNX_FLAG(val)                     GET_HIWORD(val)

#define CONNX_EVENT_WAIT_INFINITE               ((uint16_t) 0xFFFF)

/*------------------------------------------------------------------*/
/* Endian conversion */
/*------------------------------------------------------------------*/
#define CONNX_GET_UINT16_FROM_LITTLE_ENDIAN(ptr)        \
    (((uint16_t) ((uint8_t *) (ptr))[0]) | ((uint16_t) ((uint8_t *) (ptr))[1]) << 8)

#define CONNX_GET_UINT32_FROM_LITTLE_ENDIAN(ptr)        \
    (((uint32_t) ((uint8_t *) (ptr))[0]) | ((uint32_t) ((uint8_t *) (ptr))[1]) << 8 | \
    ((uint32_t) ((uint8_t *) (ptr))[2]) << 16 | ((uint32_t) ((uint8_t *) (ptr))[3]) << 24)

#define CONNX_COPY_UINT16_TO_LITTLE_ENDIAN(uint, ptr)   \
    ((uint8_t *) (ptr))[0] = ((uint8_t) ((uint) & 0x00FF)); \
    ((uint8_t *) (ptr))[1] = ((uint8_t) ((uint) >> 8))

#define CONNX_COPY_UINT32_TO_LITTLE_ENDIAN(uint, ptr)   \
    ((uint8_t *) (ptr))[0] = ((uint8_t) ((uint) & 0x000000FF)); \
    ((uint8_t *) (ptr))[1] = ((uint8_t) (((uint) >> 8) & 0x000000FF)); \
    ((uint8_t *) (ptr))[2] = ((uint8_t) (((uint) >> 16) & 0x000000FF)); \
    ((uint8_t *) (ptr))[3] = ((uint8_t) (((uint) >> 24) & 0x000000FF))

#define CONNX_GET_UINT16_FROM_BIG_ENDIAN(ptr) \
    (((uint16_t) ((uint8_t *) (ptr))[1]) | \
    ((uint16_t) ((uint8_t *) (ptr))[0]) << 8)

#define CONNX_GET_UINT24_FROM_BIG_ENDIAN(ptr) \
    (((uint24_t) ((uint8_t *) (ptr))[2]) | \
    ((uint24_t) ((uint8_t *) (ptr))[1]) << 8 | \
    ((uint24_t) ((uint8_t *) (ptr))[0]) << 16)

#define CONNX_GET_UINT32_FROM_BIG_ENDIAN(ptr)   \
    (((uint32_t) ((uint8_t *) (ptr))[3]) | \
    ((uint32_t) ((uint8_t *) (ptr))[2]) << 8 | \
    ((uint32_t) ((uint8_t *) (ptr))[1]) << 16 | \
    ((uint32_t) ((uint8_t *) (ptr))[0]) << 24)

#define CONNX_COPY_UINT16_TO_BIG_ENDIAN(uint, ptr)  \
    ((uint8_t *) (ptr))[1] = ((uint8_t) ((uint) & 0x00FF)); \
    ((uint8_t *) (ptr))[0] = ((uint8_t) ((uint) >> 8))

#define CONNX_COPY_UINT24_TO_BIG_ENDIAN(uint, ptr)  \
    ((uint8_t *) (ptr))[2] = ((uint8_t) ((uint) & 0x000000FF)); \
    ((uint8_t *) (ptr))[1] = ((uint8_t) (((uint) >> 8) & 0x000000FF)); \
    ((uint8_t *) (ptr))[0] = ((uint8_t) (((uint) >> 16) & 0x000000FF))

#define CONNX_COPY_UINT32_TO_BIG_ENDIAN(uint, ptr)  \
    ((uint8_t *) (ptr))[3] = ((uint8_t) ((uint) & 0x000000FF)); \
    ((uint8_t *) (ptr))[2] = ((uint8_t) (((uint) >> 8) & 0x000000FF)); \
    ((uint8_t *) (ptr))[1] = ((uint8_t) (((uint) >> 16) & 0x000000FF)); \
    ((uint8_t *) (ptr))[0] = ((uint8_t) (((uint) >> 24) & 0x000000FF))

/*------------------------------------------------------------------*/
/* XAP conversion macros */
/*------------------------------------------------------------------*/  

#define CONNX_LSB16(a) ((uint8_t) ((a) & 0x00ff))
#define CONNX_MSB16(b) ((uint8_t) ((b) >> 8))

#define CONNX_CONVERT_8_FROM_XAP(output, input) \
    (output) = ((uint8_t) (input)[0]);(input) += 1

#define CONNX_CONVERT_16_FROM_XAP(output, input) \
    (output) = (uint16_t) ((((uint16_t) (input)[1]) << 8) | \
                            ((uint16_t) (input)[0]));(input) += 2

#define CONNX_CONVERT_24_FROM_XAP(output, input) \
    (output) = (((uint32_t) (input)[2]) << 16) | \
               (((uint32_t) (input)[1]) << 8) | \
               ((uint32_t) (input)[0]);input += 3

#define CONNX_CONVERT_32_FROM_XAP(output, input) \
    (output) = (((uint32_t) (input)[3]) << 24) | \
               (((uint32_t) (input)[2]) << 16) | \
               (((uint32_t) (input)[1]) << 8) | \
               ((uint32_t) (input)[0]);input += 4

#define CONNX_ADD_UINT8_TO_XAP(output, input) \
    (output)[0] = (input);(output) += 1

#define CONNX_ADD_UINT16_TO_XAP(output, input) \
    (output)[0] = ((uint8_t) ((input) & 0x00FF));  \
    (output)[1] = ((uint8_t) ((input) >> 8));(output) += 2

#define CONNX_ADD_UINT24_TO_XAP(output, input) \
    (output)[0] = ((uint8_t) ((input) & 0x00FF)); \
    (output)[1] = ((uint8_t) (((input) >> 8) & 0x00FF)); \
    (output)[2] = ((uint8_t) ((input) >> 16));(output) += 3

#define CONNX_ADD_UINT32_TO_XAP(output, input) \
    (output)[0] = ((uint8_t) ((input) & 0x00FF)); \
    (output)[1] = ((uint8_t) (((input) >> 8) & 0x00FF)); \
    (output)[2] = ((uint8_t) (((input) >> 16) & 0x00FF)); \
    (output)[3] = ((uint8_t) ((input) >> 24));(output) += 4

#define CONNX_MAX(a, b)    (((a) > (b)) ? (a) : (b))
#define CONNX_MIN(a, b)    (((a) < (b)) ? (a) : (b))


/* Thread API */
ConnxHandle ConnxThreadCreate(void (*threadFunction)(void *pointer),
                              void *pointer,
                              uint32_t stackSize,
                              uint16_t priority,
                              const char *threadName);

void ConnxThreadDestroy(ConnxHandle threadHandle);

void ConnxThreadSleep(uint16_t sleepTimeInMs);

/* Event API */
ConnxHandle ConnxEventCreate();

ConnxResult ConnxEventWait(ConnxHandle eventHandle, uint16_t timeoutInMs, uint32_t *eventBits);

void ConnxEventSet(ConnxHandle eventHandle, uint32_t eventBits);

void ConnxEventDestroy(ConnxHandle eventHandle);

/* Mutex API */
ConnxHandle ConnxMutexCreate();

void ConnxMutexLock(ConnxHandle mutexHandle);

void ConnxMutexUnlock(ConnxHandle mutexHandle);

void ConnxMutexDestroy(ConnxHandle mutexHandle);

/* Misc API */
bool ConnxScriptLaunch(const char *script, const char *cmd);

uint32_t ConnxStrToInt(const char *str);
char *ConnxStrDup(const char *str);
char *ConnxStrNCpyZero(char *dest, const char *src, size_t count);
size_t ConnxStrLCpy(char *dst, const char *src, size_t size);
size_t ConnxStrLCat(char *dst, const char *src, size_t size);

char *ConnxCreateFullFileName(const char *path, const char *file_name);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_UTIL_H_ */