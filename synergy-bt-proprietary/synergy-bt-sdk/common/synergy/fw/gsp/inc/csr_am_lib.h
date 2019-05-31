#ifndef CSR_AM_LIB_H__
#define CSR_AM_LIB_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_am_prim.h"
#include "csr_msg_transport.h"
#include "csr_am_task.h"
#include "csr_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrAmInitReqSend
 *
 *  DESCRIPTION
 *      This function prepares the AM for use from an application
 *
 *
 *  PARAMETERS
 *      pHandle       : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrAmInitReq *CsrAmInitReq_struct(CsrSchedQid pHandle);

#define CsrAmInitReqSend(_ph){ \
        CsrAmInitReq *msg__; \
        msg__ = CsrAmInitReq_struct(_ph); \
        CsrMsgTransport(CSR_AM_IFACEQUEUE, CSR_AM_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrAmAudioPathConnectReqSend
 *
 *  DESCRIPTION
 *      This function establishes an audio stream.
 *
 *
 *  PARAMETERS
 *      pHandle       : The identity of the calling process.
 *      source        : Type of source endpoint to use (fm, codec,pcm,...).
 *      sourceR       : Type of source endpoint to use for the right side of a stereo connection
 *      sinkCount     : Number of sink endpoint elements in the "sinkId" field
 *      sinkId        : Type of sink to use (fm, codec,pcm,...).
 *      sinkConfigCount : number of configuration entries
 *      sinkConfig    : Set of configruation values for the sinks which configuration is intended
 *      audioType     : FM, WBS, CVSD,.....
 *      stereo        : boolean to indicate whether stereo audio is wanted.
 *----------------------------------------------------------------------------*/
CsrAmAudioPathConnectReq *CsrAmAudioPathConnectReq_struct(CsrSchedQid pHandle,
    CsrEndpointType source,
    CsrEndpointType sourceR,
    CsrUint8 sinkCount,
    CsrAmEndPointDefType *sinkId,
    CsrUint8 sinkConfigCount,
    CsrAmSinkConfigType *sinkConfig,
    CsrAmAudioType audioType,
    CsrBool stereo);

#define CsrAmAudioPathConnectReqSend(_ph, _s, _sr, _sc, _sk, _cc, _sf, _at, _st){ \
        CsrAmAudioPathConnectReq *msg__; \
        msg__ = CsrAmAudioPathConnectReq_struct(_ph, _s, _sr, _sc, _sk, _cc, _sf, _at, _st); \
        CsrMsgTransport(CSR_AM_IFACEQUEUE, CSR_AM_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrAmAudioPathAddSinkReqSend
 *
 *  DESCRIPTION
 *      This function adds a sink endpoint to an established audio path
 *
 *
 *  PARAMETERS
 *      pHandle         : The identity of the calling process.
 *      amConnectionId  : connection Id.
 *      sinkCount       : number of sinks to add
 *      newSinkId       : the sink to add.
 *      sinkConfigCount : number of configuration entries
 *      sinkConfig      : Set of configruation values for the sinks which
 *                        configuration is intended
 *----------------------------------------------------------------------------*/
CsrAmAudioPathAddSinkReq *CsrAmAudioPathAddSinkReq_struct(CsrSchedQid pHandle,
    CsrUint16 amConnectionId,
    CsrUint8 sinkCount,
    CsrAmEndPointDefType *newSinkId,
    CsrUint8 sinkConfigCount,
    CsrAmSinkConfigType *sinkConfig);

#define CsrAmAudioPathAddSinkReqSend(_ph, _a, _sc, _n, _c, _s){ \
        CsrAmAudioPathAddSinkReq *msg__; \
        msg__ = CsrAmAudioPathAddSinkReq_struct(_ph, _a, _sc, _n, _c, _s); \
        CsrMsgTransport(CSR_AM_IFACEQUEUE, CSR_AM_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrAmAudioPathRemoveSinkReqSend
 *
 *  DESCRIPTION
 *      This function removes a sink endpoint from an established audio path
 *
 *
 *  PARAMETERS
 *      pHandle         : The identity of the calling process.
 *      amConnectionId  : connection Id.
 *      sinkId          : the sink to remove.
 *----------------------------------------------------------------------------*/
CsrAmAudioPathRemoveSinkReq *CsrAmAudioPathRemoveSinkReq_struct(CsrSchedQid pHandle,
    CsrUint16 amConnectionId,
    CsrAmEndPointDefType sinkId);

#define CsrAmAudioPathRemoveSinkReqSend(_ph, _a, _s){ \
        CsrAmAudioPathRemoveSinkReq *msg__; \
        msg__ = CsrAmAudioPathRemoveSinkReq_struct(_ph, _a, _s); \
        CsrMsgTransport(CSR_AM_IFACEQUEUE, CSR_AM_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrAmAudioPathConfigReqSend
 *
 *  DESCRIPTION
 *      This function configures an established audio path
 *
 *
 *  PARAMETERS
 *      pHandle         : The identity of the calling process.
 *      amConnectionId  : connection Id.
 *      epType          : config the source, sink or all
 *      sinkId          : if the sink is to be configured, what sink.
 *      configDataLen   : length of the config data
 *      configData      : pointer to the configuration data
 *----------------------------------------------------------------------------*/
CsrAmAudioPathConfigReq *CsrAmAudioPathConfigReq_struct(CsrSchedQid pHandle,
    CsrUint16 amConnectionId,
    CsrAmEndpointType epType,
    CsrUint16 sinkId,
    CsrUint16 configDataLen,
    CsrUint8 *configData);

#define CsrAmAudioPathConfigReqSend(_ph, _a, _e, _s, _cl, _cd){ \
        CsrAmAudioPathConfigReq *msg__; \
        msg__ = CsrAmAudioPathConfigReq_struct(_ph, _a, _e, _s, _cl, _cd); \
        CsrMsgTransport(CSR_AM_IFACEQUEUE, CSR_AM_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrAmAudioPathReleaseReqSend
 *
 *  DESCRIPTION
 *      This function releases an established audio path
 *
 *
 *  PARAMETERS
 *      pHandle         : The identity of the calling process.
 *      amConnectionId  : connection Id.
 *----------------------------------------------------------------------------*/
CsrAmAudioPathReleaseReq *CsrAmAudioPathReleaseReq_struct(CsrSchedQid pHandle,
    CsrUint16 amConnectionId);

#define CsrAmAudioPathReleaseReqSend(_ph, _a){ \
        CsrAmAudioPathReleaseReq *msg__; \
        msg__ = CsrAmAudioPathReleaseReq_struct(_ph, _a); \
        CsrMsgTransport(CSR_AM_IFACEQUEUE, CSR_AM_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrAmFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR AM upstream messages
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_AM_PRIM,
 *      msg:          The message received from CSR AM
 *----------------------------------------------------------------------------*/
void CsrAmFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);


#ifdef __cplusplus
}
#endif

#endif
