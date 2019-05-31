/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_COMMON_DEF_H_
#define _CONNX_COMMON_DEF_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef uint16_t    ConnxResult;

#define CONNX_RESULT_SUCCESS                ((ConnxResult) 0x0000)
#define CONNX_RESULT_FAIL                   ((ConnxResult) 0x0001)
#define CONNX_RESULT_TIMEOUT                ((ConnxResult) 0x0002)
#define CONNX_RESULT_SERVICE_UNSUPPORTED    ((ConnxResult) 0x0003)
#define CONNX_RESULT_SERVICE_BUSY           ((ConnxResult) 0x0004)
#define CONNX_RESULT_INVALID_HANDLE         ((ConnxResult) 0x0005)
#define CONNX_RESULT_INVALID_POINTER        ((ConnxResult) 0x0006)
#define CONNX_RESULT_INVALID_PARAMETER      ((ConnxResult) 0x0007)
#define CONNX_RESULT_INVALID_FIRMWARE       ((ConnxResult) 0x0008)
#define CONNX_RESULT_OUT_OF_MEMEORY         ((ConnxResult) 0x0009)
#define CONNX_RESULT_NO_TRANSPORT           ((ConnxResult) 0x000A)

#define IS_CONNX_RESULT_SUCCESS(res)        ((res) == CONNX_RESULT_SUCCESS)

#define CONNX_MAX_PATH_LENGTH               256

#define CONNX_UNUSED(x)                     ((void) (x))

#define CONNX_COUNT_OF(a)                   (sizeof(a)/sizeof((a)[0]))

/* Bluetooth device address length in byte. */
#define BT_ADDR_LENGTH                      6

#define MAX_BUF_SIZE                        256

#define CONNX_PATH                          "/data/"

#define CONNX_HYPHEN                        "-"

#define CONNX_SOCKET                        "ConnxSocket"

/* bt audio service name */
#define BT_AUDIO_SERVICE                    "BtAudioService"

/* Synergy service name. */
#define SYNERGY_SERVICE                     "SynergyService"

#define SOCKET_PATH                         "/data/ConnxSocketBtAudioService-SynergyService"

typedef void *ConnxHandle;

typedef void *ConnxContext;

typedef uint32_t    uint24_t;

typedef struct
{
    uint24_t    lap;   /*!< Lower Address Part 00..23 */
    uint8_t     uap;   /*!< upper Address Part 24..31 */
    uint16_t    nap;   /*!< Non-significant    32..47 */
} BT_ADDR_T;

/* Scatter/Gather buffer. */
typedef struct
{
    uint8_t    *data;
    uint32_t    size;
} SG_BUFFER_T;


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_COMMON_DEF_H_ */