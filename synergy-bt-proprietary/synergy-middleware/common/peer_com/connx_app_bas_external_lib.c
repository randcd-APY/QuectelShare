/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include <stdlib.h>

#include "connx_bas_common.h"
#include "connx_peer_com_sef.h"
#include "connx_util.h"

void ConnxAppBtAudioServiceReq(ConnxBasPrim type, uint16_t sampleRate)
{
    uint8_t *tmpPayload;
    size_t size = sizeof(ConnxBtAudioServiceReq);
    uint8_t *payload = (ConnxBtAudioServiceReq *) malloc(size);
    
    /* [QTI] Fix KW issue#266943. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    
    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);
    CONNX_ADD_UINT16_TO_XAP(tmpPayload, sampleRate);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasInitHfAudio()
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasInitHfCallAudioInd);
    uint8_t *payload = (ConnxBasInitHfCallAudioInd *) malloc(size);
    
    /* [QTI] Fix KW issue#266944. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_INIT_HF_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasStartHfAudio(uint16_t sample_rate)
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasStartHfCallAudioInd);
    uint8_t *payload = (ConnxBasStartHfCallAudioInd *) malloc(size);
    
    /* [QTI] Fix KW issue#266945. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_START_HF_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);
    CONNX_ADD_UINT16_TO_XAP(tmpPayload, sample_rate);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasStopHfAudio()
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasStopHfCallAudioInd);
    uint8_t *payload = (ConnxBasStopHfCallAudioInd *) malloc(size);
    
    /* [QTI] Fix KW issue#266946. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_STOP_HF_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasDeinitHfAudio()
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasDeinitHfCallAudioInd);
    uint8_t *payload = (ConnxBasDeinitHfCallAudioInd *) malloc(size);
    
    /* [QTI] Fix KW issue#266947. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_DEINIT_HF_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasInitHfgAudio()
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasInitHfgCallAudioInd);
    uint8_t *payload = (ConnxBasInitHfgCallAudioInd *) malloc(size);
    
    /* [QTI] Fix KW issue#266948. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_INIT_HFG_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasStartHfgAudio(uint16_t sample_rate)
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasStartHfgCallAudioInd);
    uint8_t *payload = (ConnxBasStartHfgCallAudioInd *) malloc(size);
    
    /* [QTI] Fix KW issue#266949. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_START_HFG_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);
    CONNX_ADD_UINT16_TO_XAP(tmpPayload, sample_rate);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasStopHfgAudio()
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasStopHfgCallAudioInd);
    uint8_t *payload = (ConnxBasStopHfgCallAudioInd *) malloc(size);    
    
    /* [QTI] Fix KW issue#266950. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_STOP_HFG_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);

    connx_app_bas_transport_send(payload, size);
}

void ConnxAppBasDeinitHfgAudio()
{
    uint8_t *tmpPayload;
    ConnxBasPrim type;
    size_t size = sizeof(ConnxBasDeinitHfgCallAudioInd);
    uint8_t *payload = (ConnxBasDeinitHfgCallAudioInd *) malloc(size);
    
    /* [QTI] Fix KW issue#266951. */
    if(!payload)
        return;
    
    tmpPayload = payload;
    type = CONNX_BAS_DEINIT_HFG_CALL_AUDIO_IND;

    CONNX_ADD_UINT16_TO_XAP(tmpPayload, type);

    connx_app_bas_transport_send(payload, size);
}


