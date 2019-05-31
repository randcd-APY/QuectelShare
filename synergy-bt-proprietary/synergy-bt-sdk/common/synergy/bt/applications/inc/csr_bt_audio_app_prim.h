#ifndef CSR_BT_AUDIO_APP_PRIM_H__
#define CSR_BT_AUDIO_APP_PRIM_H__

/****************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_bt_profiles.h"
#include "csr_bt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrPrim         CsrBtAudioAppPrim;

#define CSR_BT_AUDIO_APP_RESULT_SUCCESS              ((CsrBtResultCode) (0x0000))
#define CSR_BT_AUDIO_APP_RESULT_FAILURE              ((CsrBtResultCode) (0x0001))

/****************************************************************************
 * Start of primitive definitions
 ****************************************************************************/
/* Prim Downstream Lowest */
#define CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST       (0x0000)

#define CSR_BT_AUDIO_APP_START_CALL_AUDIO_REQ                                  \
            ((CsrBtAudioAppPrim)(0x0000+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_STOP_CALL_AUDIO_REQ                                   \
            ((CsrBtAudioAppPrim)(0x0001+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_GENERATE_RING_REQ                                     \
            ((CsrBtAudioAppPrim)(0x0002+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_STOP_RING_REQ                                         \
            ((CsrBtAudioAppPrim)(0x0003+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_START_MUSIC_STREAM_REQ                                \
            ((CsrBtAudioAppPrim)(0x0004+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_STOP_MUSIC_STREAM_REQ                                 \
            ((CsrBtAudioAppPrim)(0x0005+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_SET_AUDIO_GAIN_REQ                                 \
            ((CsrBtAudioAppPrim)(0x0006+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_HIGHEST                               \
    (0x0006 + CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_COUNT                                 \
    (CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST)

/****************************************************************************/

/* Prim Upstream Lowest */
#define CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AUDIO_APP_START_CALL_AUDIO_CFM                                  \
                ((CsrBtAudioAppPrim)(0x0000+CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_STOP_CALL_AUDIO_CFM                                   \
                ((CsrBtAudioAppPrim)(0x0001+CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_GENERATE_RING_CFM                                     \
                ((CsrBtAudioAppPrim)(0x0002+CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_STOP_RING_CFM                                         \
                ((CsrBtAudioAppPrim)(0x0003+CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_START_MUSIC_STREAM_CFM                                \
                ((CsrBtAudioAppPrim)(0x0004+CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_STOP_MUSIC_STREAM_CFM                                 \
                ((CsrBtAudioAppPrim)(0x0005+CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AUDIO_APP_SET_AUDIO_GAIN_CFM                                 \
            ((CsrBtAudioAppPrim)(0x0006+CSR_BT_AUDIO_APP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AUDIO_APP_PRIM_UPTREAM_HIGHEST                                  \
    (0x0007 + CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_AUDIO_APP_PRIM_UPSTREAM_COUNT                                   \
    (CSR_BT_AUDIO_APP_PRIM_UPTREAM_HIGHEST + 1 - CSR_BT_AUDIO_APP_PRIM_UPSTREAM_LOWEST)

/****************************************************************************
 * End of primitive definitions
 ****************************************************************************/

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrUint8                    pcmSlot;                        /* chosen PCM slot */ 
    CsrUint8                    codecToUse;                     /* Selected codec  */
}CsrBtAudioAppStartCallAudioReq;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
}CsrBtAudioAppStopCallAudioReq;

typedef struct
{
    CsrBtAudioAppPrim          type;                           /* primitive/message identity */
    CsrSchedQid                appQid;
}CsrBtAudioAppGenerateRingReq;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
}CsrBtAudioAppStopRingReq;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrUint16                   codecType;
    CsrUint16                   asmAvAclHandle;
    CsrUint16                   asmAvLocalCid;
    CsrUint16                   sampFreq;
    CsrBool                     cp;
}CsrBtAudioAppStartMusicStreamReq;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
}CsrBtAudioAppStopMusicStreamReq;


typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrBtResultCode             result;
}CsrBtAudioAppStartCallAudioCfm;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrBtResultCode             result;
}CsrBtAudioAppStopCallAudioCfm;

typedef struct
{
    CsrBtAudioAppPrim          type;                           /* primitive/message identity */
    CsrSchedQid                appQid;
    CsrBtResultCode            result;
}CsrBtAudioAppGenerateRingCfm;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrBtResultCode             result;
}CsrBtAudioAppStopRingCfm;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrBtResultCode             result;
}CsrBtAudioAppStartMusicStreamCfm;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrBtResultCode             result;
}CsrBtAudioAppStopMusicStreamCfm;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrUint16                   micGain;
    CsrUint16                   spkrGain;
}CsrBtAudioAppSetAudioGainReq;

typedef struct
{
    CsrBtAudioAppPrim           type;                           /* primitive/message identity */
    CsrSchedQid                 appQid;
    CsrBtResultCode             result;
}CsrBtAudioAppSetAudioGainCfm;

#ifdef __cplusplus
}
#endif
#endif /* CSR_BT_AUDIO_APP_PRIM_H__ */


