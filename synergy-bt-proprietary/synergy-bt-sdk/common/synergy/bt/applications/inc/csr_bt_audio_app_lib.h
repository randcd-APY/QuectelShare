#ifndef CSR_BT_AUDIO_APP_LIB_H__
#define CSR_BT_AUDIO_APP_LIB_H__

/****************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_audio_app_prim.h"
#include "csr_pmem.h"
#include "csr_msg_transport.h"
#include "csr_bt_audio_app_task.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CsrBtAudioAppStartCallAudioReqSend(_appQid, _scoHandle, _pcmSlot, _codecToUse)\
{\
    CsrBtAudioAppStartCallAudioReq* msgP = (CsrBtAudioAppStartCallAudioReq*)CsrPmemAlloc(sizeof(CsrBtAudioAppStartCallAudioReq));\
    msgP->type = CSR_BT_AUDIO_APP_START_CALL_AUDIO_REQ;\
    msgP->appQid = _appQid;\
    msgP->scoHandle = _scoHandle;\
    msgP->pcmSlot = _pcmSlot;\
    msgP->codecToUse = _codecToUse;\
    CsrMsgTransport(CSR_BT_AUDIO_APP_IFACEQUEUE, CSR_BT_AUDIO_APP_PRIM, msgP);\
}

#define CsrBtAudioAppStopCallAudioReqSend(_appQid, _scoHandle)\
{\
    CsrBtAudioAppStopCallAudioReq* msgP = (CsrBtAudioAppStopCallAudioReq*)CsrPmemAlloc(sizeof(CsrBtAudioAppStopCallAudioReq));\
    msgP->type = CSR_BT_AUDIO_APP_STOP_CALL_AUDIO_REQ;\
    msgP->appQid = _appQid;\
    msgP->scoHandle = _scoHandle;\
    CsrMsgTransport(CSR_BT_AUDIO_APP_IFACEQUEUE, CSR_BT_AUDIO_APP_PRIM, msgP);\
}

#define CsrBtAudioAppGenerateRingReqSend(_appQid)\
{\
    CsrBtAudioAppGenerateRingReq* msgP = (CsrBtAudioAppGenerateRingReq*)CsrPmemAlloc(sizeof(CsrBtAudioAppGenerateRingReq));\
    msgP->type = CSR_BT_AUDIO_APP_GENERATE_RING_REQ;\
    msgP->appQid = _appQid;\
    CsrMsgTransport(CSR_BT_AUDIO_APP_IFACEQUEUE, CSR_BT_AUDIO_APP_PRIM, msgP);\
}

#define CsrBtAudioAppStopRingReqSend(_appQid)\
{\
    CsrBtAudioAppStopRingReq* msgP = (CsrBtAudioAppStopRingReq*)CsrPmemAlloc(sizeof(CsrBtAudioAppStopRingReq));\
    msgP->type = CSR_BT_AUDIO_APP_STOP_RING_REQ;\
    msgP->appQid = _appQid;\
    CsrMsgTransport(CSR_BT_AUDIO_APP_IFACEQUEUE, CSR_BT_AUDIO_APP_PRIM, msgP);\
}

#define CsrBtAudioAppStartMusicStreamReqSend(_appQid, _codecType, _asmAvAclHandle, _asmAvLocalCid, _sampFreq, _cp)\
{\
    CsrBtAudioAppStartMusicStreamReq* msgP = (CsrBtAudioAppStartMusicStreamReq*)CsrPmemAlloc(sizeof(CsrBtAudioAppStartMusicStreamReq));\
    msgP->type = CSR_BT_AUDIO_APP_START_MUSIC_STREAM_REQ;\
    msgP->appQid = _appQid;\
    msgP->codecType = _codecType;\
    msgP->asmAvAclHandle = _asmAvAclHandle;\
    msgP->asmAvLocalCid = _asmAvLocalCid;\
    msgP->sampFreq = _sampFreq;\
    msgP->cp = _cp;\
    CsrMsgTransport(CSR_BT_AUDIO_APP_IFACEQUEUE, CSR_BT_AUDIO_APP_PRIM, msgP);\
}

#define CsrBtAudioAppStopMusicStreamReqSend(_appQid)\
{\
    CsrBtAudioAppStopMusicStreamReq* msgP = (CsrBtAudioAppStopMusicStreamReq*)CsrPmemAlloc(sizeof(CsrBtAudioAppStopMusicStreamReq));\
    msgP->type = CSR_BT_AUDIO_APP_STOP_MUSIC_STREAM_REQ;\
    msgP->appQid = _appQid;\
    CsrMsgTransport(CSR_BT_AUDIO_APP_IFACEQUEUE, CSR_BT_AUDIO_APP_PRIM, msgP);\
}

#define CsrBtAudioAppSetAudioGainReqsend(_appQid, _micGain, _spkrGain)\
{\
    CsrBtAudioAppSetAudioGainReq* msgP = (CsrBtAudioAppSetAudioGainReq*)CsrPmemAlloc(sizeof(CsrBtAudioAppSetAudioGainReq));\
    msgP->type = CSR_BT_AUDIO_APP_SET_AUDIO_GAIN_REQ;\
    msgP->appQid = _appQid;\
    msgP->micGain = _micGain;\
    msgP->spkrGain = _spkrGain;\
    CsrMsgTransport(CSR_BT_AUDIO_APP_IFACEQUEUE, CSR_BT_AUDIO_APP_PRIM, msgP);\
}

#ifdef __cplusplus
}
#endif
#endif /*CSR_BT_AUDIO_APP_LIB_H__*/
