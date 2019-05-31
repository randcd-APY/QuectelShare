#ifndef _CONNX_BAS_COMMON_H_
#define _CONNX_BAS_COMMON_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t   ConnxBasPrim;

/****************************************************************************
 * Start of primitive definitions
 ****************************************************************************/
/* Prim Downstream Lowest */
#define CONNX_BAS_PRIM_DOWNSTREAM_LOWEST              (0x0000)

/****************************************************************************/

/* Prim Upstream Lowest */
#define CONNX_BAS_PRIM_UPSTREAM_LOWEST                (0x0000 + 0x8000)

#define CONNX_BAS_INIT_HF_CALL_AUDIO_IND              ((ConnxBasPrim)(0x0001+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_START_HF_CALL_AUDIO_IND             ((ConnxBasPrim)(0x0002+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_STOP_HF_CALL_AUDIO_IND              ((ConnxBasPrim)(0x0003+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_DEINIT_HF_CALL_AUDIO_IND            ((ConnxBasPrim)(0x0004+CONNX_BAS_PRIM_UPSTREAM_LOWEST))

#define CONNX_BAS_INIT_HFG_CALL_AUDIO_IND             ((ConnxBasPrim)(0x0005+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_START_HFG_CALL_AUDIO_IND            ((ConnxBasPrim)(0x0006+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_STOP_HFG_CALL_AUDIO_IND             ((ConnxBasPrim)(0x0007+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_DEINIT_HFG_CALL_AUDIO_IND           ((ConnxBasPrim)(0x0008+CONNX_BAS_PRIM_UPSTREAM_LOWEST))

#define CONNX_BAS_INIT_A2DP_STREAMING_IND             ((ConnxBasPrim)(0x0009+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_START_A2DP_STREAMING_IND            ((ConnxBasPrim)(0x000A+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_STOP_A2DP_STREAMING_IND             ((ConnxBasPrim)(0x000B+CONNX_BAS_PRIM_UPSTREAM_LOWEST))
#define CONNX_BAS_DEINIT_A2DP_STREAMING_IND           ((ConnxBasPrim)(0x000C+CONNX_BAS_PRIM_UPSTREAM_LOWEST))

#define CONNX_BAS_PRIM_UPSTREAM_HIGHEST               ((ConnxBasPrim)(0x000D+CONNX_BAS_PRIM_UPSTREAM_LOWEST))

/****************************************************************************
 * End of primitive definitions
 ****************************************************************************/
typedef struct
{
    ConnxBasPrim		  type; 						  /* primitive/message identity */
    uint16_t			  sampleFreq;					  /* handsfree audio data frequecy */
} ConnxBtAudioServiceReq;

typedef struct
{
    ConnxBasPrim		  type; 						  /* primitive/message identity */
} ConnxBasInitHfCallAudioInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
    uint16_t              sampleFreq;                     /* handsfree audio data frequecy */
} ConnxBasStartHfCallAudioInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
} ConnxBasStopHfCallAudioInd;

typedef struct
{
    ConnxBasPrim		  type; 						  /* primitive/message identity */
} ConnxBasDeinitHfCallAudioInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
} ConnxBasInitHfgCallAudioInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
    uint16_t              sampleFreq;                     /* audio gateway data frequecy */
} ConnxBasStartHfgCallAudioInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
} ConnxBasStopHfgCallAudioInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
} ConnxBasDeinitHfgCallAudioInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
} ConnxBasInitA2dpStreamingInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
    uint16_t              sampleFreq;                     /* audio/video audio data frequecy */
} ConnxBasStartA2dpStreamingInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
} ConnxBasStopA2dpStreamingInd;

typedef struct
{
    ConnxBasPrim          type;                           /* primitive/message identity */
} ConnxBasDeinitA2dpStreamingInd;

#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_BAS_COMMON_H_ */

