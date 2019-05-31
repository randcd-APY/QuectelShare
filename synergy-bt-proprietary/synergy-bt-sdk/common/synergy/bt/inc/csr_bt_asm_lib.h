#ifndef CSR_BT_ASM_LIB_H__
#define CSR_BT_ASM_LIB_H__

/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_asm_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_msg_transport.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*************************** Utility Functions ********************************/
typedef struct CsrBtAsmListSt
{
    struct CsrBtAsmListSt *next;
} CsrBtAsmList;

/* This shall allocate memory for the resources (non-contiguous), 
 * the resources shall be zero-initialized (except the next field).
 * This shall return a linked list if more than one resource is requested.
 */
CsrBtAsmAudioResource *CsrBtAsmAllocateAudioResource(CsrUint8 count);

/* This shall free all the audio resources in the list */
void CsrBtAsmFreeAudioResource(CsrBtAsmAudioResource *res);

/* This shall allocate memory for the audio stream, each audio stream shall be 
 * zero-initialized (except the next field). This shall return a linked list 
 * if more than one audio stream is requested.
 *
 * Note:
 * 1. each audio stream shall be a different memory i.e. non-contiguous.
 * 2. memory for audioStream->res should be allocated separately by 
 *    applications using CsrBtAsmAllocateAudioResource().
 */
CsrBtAsmAudioStream *CsrBtAsmAllocateAudioStream(CsrUint8 count);

/* This shall free the audio resources pointed by audio stream and the 
 * audio stream itself in the list.
 */
void CsrBtAsmFreeAudioStream(CsrBtAsmAudioStream *audioStream);

/* This shall allocate memory for audio resource config.
 * This shall return a linked list if more than one config is requested.
 * Note: 
 * 1. each config shall be different memory i.e. the configs shall  
 *    be non-contiguous.
 * 2. if it is source or sink applications have to allocate memory separately  
 *    for epCfg (CsrBtAsmSrcSnkConfigKeyValue)
 * 3. if it is an operator then applications have to allocate memory separately 
 *    for opCfg (CsrBtAsmOperatorConfigKeyValue) and opCfg->value.
 */
CsrBtAsmAudioResourceConfig *CsrBtAsmAllocateAudioResourceConfig(CsrUint8 count);

/* This shall free the audio resource config. 
 * Note:
 * 1. source/sink epCfg (CsrBtAsmSrcSnkConfigKeyValue) shall be freed.
 * 2. opCfg (CsrBtAsmOperatorConfigKeyValue) and opCfg->value both shall be 
 *    freed.
 */
void CsrBtAsmFreeAudioResourceConfig(CsrBtAsmAudioResourceConfig *resConfig);

/* This shall allocate memory for audio resource sync pair.
 * This shall return a linked list if more than one pair is requested.
 * Note: 
 * 1. each pair shall be from a different memory block i.e. the pairs shall be 
 *    non-contiguous.
 * 2. applications have to allocate memory separately for resA and resB.
 */
CsrBtAsmSyncAudioResourcePair *CsrBtAsmAllocateSyncAudioResourcePair(CsrUint8 count);

/* This shall free the audio resource sync pair. resA and resB as well would be 
 * freed.
 */
void CsrBtAsmFreeSyncAudioResourcePair(CsrBtAsmSyncAudioResourcePair *syncPair);

/* Returns total number of nodes in the list */
CsrUint8 CsrBtAsmFindTotalElements(CsrBtAsmList *ele);

/* Frees all the nodes in the list */
void CsrBtAsmFreeElement(CsrBtAsmList *element);

/* This return a duplicated copy (linked list) of audio resources from 
 * the resource list in 'res'.
 */
CsrBtAsmAudioResource *CsrBtAsmCopyAudioResources(CsrBtAsmAudioResource *res);

/* This return a duplicated copy (linked list) of audio stream and 
 * audio resources from the audio stream list in 'as'.
 */
CsrBtAsmAudioStream *CsrBtAsmCopyAudioStreams(CsrBtAsmAudioStream *as);

/* This return a duplicated copy (linked list) of resource config from the 
 * config list in 'cfg'.
 */
CsrBtAsmAudioResourceConfig *CsrBtAsmCopyAudioResourceConfig(CsrBtAsmAudioResourceConfig *cfg);

/* This return a duplicated copy (linked list) of sync pair from the 
 * audio sync resource pair list in 'cfg'.
 */
CsrBtAsmSyncAudioResourcePair *CsrBtAsmCopySyncAudioResourcePair(CsrBtAsmSyncAudioResourcePair *sync);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmActivateReqSend
 *
 *  DESCRIPTION
 *      This request shall activate ASM. Following things are done in order:
 *      1. Activate Synergy Framework DSPM.
 *      2. Enable 'SCO Streams' which would allow applications 
 *         to acquire SCO via BlueCore DSPManager or HydraCore Audio Subsystem.
 *      3. For BlueCore devices it would as well trigger the patching process.
 *         Note: Application has to register with Synergy Framework DSPM to 
 *         supply patches when the chip demands it. And this registration 
 *         has to happen before this request is sent.
 *         As on BlueCore the 1st create_operator_c/capability_download would 
 *         trigger the framework patching. Where as on Hydra, as soon as 
 *         ACCMD relay is enabled the curator would load the dsp patches so 
 *         Synergy BT would not be involved. ACCMD relay is enabled when 
 *         ASM has sent CsrDspmActivateReqSend.
 *      4. Based on buildId applications have to send the Chip specific 
 *         audio interfaces (such as PCM/I2S/) and inbuilt DSP features 
 *         (NB, WBS, CVC, From-Air-Shunt.) supported. 
 *
 *      Note:
 *      1. On receiving this request if ASM is already activated then it 
 *         immediately returns CSR_BT_ASM_ACTIVATE_CFM with 
 *         CSR_BT_RESULT_CODE_ASM_SUCCESS.
 *
 *  PARAMETERS
 *      _appQid   - Application handle
 *      _chipInfo - Chip specific information required for ASM to know what 
 *                  hardware audio interfaces & its slots and in-built 
 *                  DSP features which are being used on a platform.
 *                  Since audio interfaces would have source (TX) and sink (RX)
 *                  the same has to be provided in the 
 *                  chipInfo (sourceInfo, sourceInfoCount, sinkInfo, 
 *                  sinkInfoCount) and on-chip Audio features in 
 *                  chipInfo (operatorInfo).
 *
 *  CONFIRMATION
 *      For this request applications shall receive CSR_BT_ASM_ACTIVATE_CFM with
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS on successful activation.
 *               2. CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS if any of the 
 *                  _chipInfo parameters are invalid.
 *               3. CSR_BT_RESULT_CODE_ASM_FAILURE if SCO stream enable failed.
 *----------------------------------------------------------------------------*/
#define CsrBtAsmActivateReqSend(_appQid, _chipInfo)                 do\
{                                                                     \
    CsrBtAsmActivateReq *msg = CsrPmemAlloc(sizeof(*msg));            \
    msg->type = CSR_BT_ASM_ACTIVATE_REQ;                              \
    msg->appQid = _appQid;                                            \
    msg->chipInfo = _chipInfo;                                        \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);     \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmDeactivateReqSend
 *
 *  DESCRIPTION
 *      This request shall deactivate ASM. Following things are done in order:
 *      1. If there are active audio streams it then stops and disconnects those.
 *      2. If any of the DSP capabilities are instantiated then destroys 
 *         these operators.
 *      2. Removes the on-the fly added (downloaded) capabilities if not already
 *         removed.
 *      3. Closes the source and sink endpoints of hardware audio interfaces
 *         which are acquired during the lifetime if not released.
 *      4. Disables SCO stream.
 *      5. Deactivate Synergy Framework DSPM.
 *
 *  PARAMETERS
 *      _appQid   - Application handle
 *
 *  CONFIRMATION
 *      For this request applications shall receive CSR_BT_ASM_DEACTIVATE_CFM
 *----------------------------------------------------------------------------*/
#define CsrBtAsmDeactivateReqSend(_appQid)                     do\
{                                                                \
    CsrBtAsmDeactivateReq *msg = CsrPmemAlloc(sizeof(*msg));     \
    msg->type = CSR_BT_ASM_DEACTIVATE_REQ;                       \
    msg->appQid = _appQid;                                       \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);\
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmGetAudioResourceReqSend
 *
 *  DESCRIPTION
 *      Any audio interface (PCM/I2S/CODEC) have source (TX) and sink (RX) 
 *      with each source / sink having several channels or several slots. 
 *      And on-chip DSP features (WBS, SBC Decoding, aptX Decoding, ...) which
 *      are called operators have inputs where they consume audio, do the 
 *      required transformation and yield the transformed audio at its output.
 *      Each  channels or slots of audio interfaces and input or output 
 *      of operatorsare called endpoints. In ASM these endpoints are 
 *      represented as AudioResource (CsrBtAsmAudioResource).
 *
 *      This request shall acquire these audio resources specificed as a list
 *      from the chip.
 *      If any of the requested resources were already fetched/instantiated 
 *      previously then it shall use the instance id from the bank.
 *
 *      Note: Exception is for the Audio Resource category 'Operator', 
 *      where every request shall instantiate (create) the operator 
 *      and provide the instantiated id.
 *
 *  PARAMETERS
 *      _appQid            - Application handle
 *      _asmRes            - List of resources to be fetched.
 *      _continueOnFailure - If TRUE then it directs ASM to continue on 
 *                           fetching other resrouces if there is a failure.
 *                           If FALSE then ASM shall not proceed fetching 
 *                           other resources.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_GET_AUDIO_RESOURCE_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if all the resources were
 *                  fetched without any failures.
 *               2. CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS if _asmRes is NULL.
 *               3. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in acquiring the resource(s) from the chip. 
 *                  Applications have to traverse through the 'appRes' for 
 *                  finding failures. If continueOnFailure was TRUE then there
 *                  could be more failures, otherwise only one resource got
 *                  failed to be acquired.
 *
 *      appRes - Contains the list of audio resources (in the same order as 
 *               provided by the application) which were acquired by ASM.
 *               'appRes's result and id has following meaning:
 *               result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS - this resource
 *                           was fetched successfully. appRes->id would have
 *                           unique instance identifier for that audio resource.
 *                        2. CSR_BT_RESULT_CODE_ASM_RESOURCE_NOT_FOUND - this
 *                           resource was not found in the database and 
 *                           is not available for applications.
 *                        3. CSR_BT_RESULT_CODE_ASM_RESOURCE_FAILED - this 
 *                           resource failed to be acquired no other 
 *                           information is available.
 *----------------------------------------------------------------------------*/
#define CsrBtAsmGetAudioResourceReqSend\
(\
    _appQid,          \
    _asmRes,          \
    _continueOnFailure\
)\
do\
{                                                                 \
    CsrBtAsmGetAudioResourceReq *msg = CsrPmemAlloc(sizeof(*msg));\
    msg->type = CSR_BT_ASM_GET_AUDIO_RESOURCE_REQ;                \
    msg->appQid = _appQid;                                        \
    msg->asmRes = CsrBtAsmCopyAudioResources(_asmRes);            \
    CsrBtAsmFreeAudioResource(_asmRes);                           \
    msg->continueOnFailure = _continueOnFailure;                  \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg); \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmReleaseAudioResourceReqSend
 *
 *  DESCRIPTION
 *      This shall release all the resources in the list. For source and sink
 *      ASM shall close these interfaces and if the resource is an operator
 *      it would be destroyed.
 *      Note:
 *      1. If any audio resource in the list is part of an Audio Stream 
 *         which was previously connected and started then it is not 
 *         necessary for applications to stop and disconnect before 
 *         releasing the resources. ASM shall internally stop and disconnect 
 *         that Audio Stream, release the Audio Stream and then release the 
 *         Audio Resource.
 *
 *  PARAMETERS
 *      _appQid            - Application handle
 *      _asmRes            - List of resources to be released.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_RELEASE_AUDIO_RESOURCE_CFM with following parameters:
 *----------------------------------------------------------------------------*/
#define CsrBtAsmReleaseAudioResourceReqSend(_appQid, _asmRes)       do\
{                                                                     \
    CsrBtAsmReleaseAudioResourceReq *msg = CsrPmemAlloc(sizeof(*msg));\
    msg->type = CSR_BT_ASM_RELEASE_AUDIO_RESOURCE_REQ;                \
    msg->appQid = _appQid;                                            \
    msg->asmRes = _asmRes;                                            \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);     \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmConfigAudioResourceReqSend
 *
 *  DESCRIPTION
 *      This request shall configure the audio resources in the list.
 *      If the resource is an operator then it shall send a operator message
 *      to it.
 *
 *  PARAMETERS
 *      _appQid - Application handle
 *      _config - List of config with each node corresponding to 
 *                an unique Audio Resource. Each config node could hold 
 *                multiple configurations for that Audio Resource.
 *                It has following parameters:
 *                next    - points to the next Audio Resource config.
 *                id      - Instance identifier of an Audio Resource returned in 
 *                          CSR_BT_ASM_GET_AUDIO_RESOURCE_CFM.
 *                epCfg   - Source or sink config key value pair of type
 *                          CsrBtAsmSrcSnkConfigKeyValue. For key 
 *                          value definitions refer csr_bt_asm_prim.h
 *                opCfg   - Key value message for an operator.  For operator 
 *                          key value definitions refer csr_bt_asm_prim.h
 *                cfgSize - Total number of source/sink configurations (epCfg)
 *                          or operator messages (opCfg).
 *                result  - Has no meaning in the request and shall be set to 
 *                          zero.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_CONFIG_AUDIO_RESOURCE_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if all the resources were
 *                  configured without any failures.
 *               2. CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS if _config is NULL.
 *               3. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in configuring the resource(s) on the chip. 
 *                  Applications have to traverse through the 'config' for 
 *                  finding failures.
 *
 *      config - Contains the list of audio resource config (in the same order
 *               as provided by the application) which were configured by ASM.
 *               'config's result has following meaning:
 *               result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS - this particular 
 *                           audio resource was configured successfully.
 *                        2. CSR_BT_RESULT_CODE_ASM_FAILURE - this 
 *                           resource failed to be acquired no further
 *                           information is available.
 *----------------------------------------------------------------------------*/
#define CsrBtAsmConfigAudioResourceReqSend(_appQid, _config)       do\
{                                                                    \
    CsrBtAsmConfigAudioResourceReq *msg = CsrPmemAlloc(sizeof(*msg));\
    msg->type = CSR_BT_ASM_CONFIG_AUDIO_RESOURCE_REQ;                \
    msg->appQid = _appQid;                                           \
    msg->config = CsrBtAsmCopyAudioResourceConfig(_config);          \
    CsrBtAsmFreeElement((CsrBtAsmList*)_config);                     \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);    \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmSyncAudioResourceReqSend
 *
 *  DESCRIPTION
 *      This request shall synchronize a pair of Audio Resources in a list.
 *      Since Audio Resources are nothing but endpoints and this request shall
 *      sync these endpoints in order to prevent timing drifts.
 *
 *  PARAMETERS
 *      _appQid   - Application handle
 *      _syncPair - List of Audio Resource pairs to be synchronized with each  
 *                  node corresponding to one pair of two endpoints.
 *                  It has following parameters:
 *                  next - points to the next Audio Resource config.
 *                  resA - Audio Resource A which needs to be synchronized with
 *                         Audio Resource B
 *                  resB - Audio Resource B
 *                         Note on resA and resB:
 *                         1. Following restrictions apply on (resA, resB) pair:-
 *                            (resA, resB) - both shall be external sources
 *                                         - both shall be external sinks
 *                                         - both internal sources
 *                                         - both internal sinks
 *                                         - one internal sink and one internal
 *                                           source.
 *                                           (For instance send and receive endpoints 
 *                                            on the same CVC operator has to be 
 *                                            synchronized)
 *                         2. Any other combinations would fail.
 *                         3. resB - can be NULL to break the existing 
 *                            synchronization with resA.
 *
 *                  catA - Indicates if resA is either Source or Sink endpoint.
 *                  catB - Indicates if resB is either Source or Sink endpoint.
 *                         Note on catA and catB:
 *                         1. Since operators coud have multiple sources 
 *                         (internal sources) and multiple sinks (internal sinks) 
 *                         application has to explicitly specify if it is 
 *                         Source pair or Sink pair of an operator which is be 
 *                         requested to synchronize.
 *                         2. catA shall always hold either 
 *                         CSR_BT_ASM_AR_CAT_SOURCE or CSR_BT_ASM_AR_CAT_SINK
 *                         It shall not be set to CSR_BT_ASM_AR_CAT_OPERATOR.
 *                result  - Has no meaning in the request and shall be set to 
 *                          zero.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_SYNC_AUDIO_RESOURCE_CFM with following parameters:
 *
 *      result   - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if all the sync pairs were
 *                    synchronized without any failures.
 *                 2. CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS if _syncPair is NULL.
 *                 3. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                    failure in synchronizing the resources on the chip. 
 *                    Applications have to traverse through the 'syncPair' for 
 *                    finding failures.
 *
 *      syncPair - Contains the list of sync pair (in the same order
 *                 as provided by the application) which were synchronized by ASM.
 *                 'syncPair's result has following meaning:
 *                  result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS - this particular 
 *                              sync pair was synchronized successfully.
 *                           2. CSR_BT_RESULT_CODE_ASM_FAILURE - this pair
 *                              failed to be synchronized and no further
 *                              information is available.
 *----------------------------------------------------------------------------*/
#define CsrBtAsmSyncAudioResourceReqSend(_appQid, _syncPair)     do\
{                                                                  \
    CsrBtAsmSyncAudioResourceReq *msg = CsrPmemAlloc(sizeof(*msg));\
    msg->type = CSR_BT_ASM_SYNC_AUDIO_RESOURCE_REQ;                \
    msg->appQid = _appQid;                                         \
    msg->syncPair = CsrBtAsmCopySyncAudioResourcePair(_syncPair);  \
    CsrBtAsmFreeElement((CsrBtAsmList*)_syncPair);                 \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);  \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmConnectAudioResourceReqSend
 *
 *  DESCRIPTION
 *      Any Audio use case such as Hands-Free or AV streaming on the chip 
 *      involves connecting the Audio Resources (endpoints) to each other 
 *      forming an logical stream where audio flows from Audio Resource 
 *      (one endpoint) to the other. Such streams in ASM is referred to as 
 *      Audio Stream. This request shall connect the endpoints in the 
 *      Audio Stream provided by application and return an unique identifier 
 *      representing the Audio Stream. This unique identifier should be used 
 *      by applications for to start/stop and disconnect the Audio Streams.
 *      (On the chip when two endpoints are connected then a transform is 
 *      said to be established and it returns a transform id which shall be 
 *      internally stored in ASM. Data shall not start flowing from one 
 *      Audio Resource (endpoint) to another untill the Audio Stream is started.)
 *
 *      Note:
 *      1. Before calling this request all the Audio Resources should be 
 *         successfully acquired using the CsrBtAsmGetAudioResourceReqSend() API.
 *      2. If connection fails for any reason then previous successfully
 *         connected Audio Streams in the list shall be disconnected.
 *
 *  PARAMETERS
 *      _appQid      - Application handle
 *      _audioStream - List of Audio Stream.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_CONNECT_AUDIO_RESOURCE_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if all the resources in the
 *                  Audio Stream list were connected successfully.
 *               2. CSR_BT_RESULT_CODE_ASM_INVALID_PARAMETERS if _audioStream
 *                  is NULL.
 *               3. CSR_BT_RESULT_CODE_ASM_RESOURCE_BUSY if any of the 
 *                  Audio Resource in the Audio Stream is being used in some
 *                  other Audio Stream.
 *               4. CSR_BT_RESULT_CODE_ASM_SHORT_STREAM if any of the stream
 *                  within the Audio Stream is having only one Audio Resource
 *                  or no Audio Resource at all. Because for connection to be
 *                  established there should be minimum two Audio Resource.
 *               5. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in connecting the resource(s) on the chip. 
 *                  Applications have to traverse through the 'appRes' for 
 *                  finding failures. Previous successfully connected 
 *                  Audio Streams in the list shall be disconnected.
 *
 *      audioStreamId - A non-zero unique identifier representing the 
 *                      successfully connected Audio Stream. If the 
 *                      Audio Stream failed to connect then this shall be 
 *                      Invalid (zero value).
 *----------------------------------------------------------------------------*/
#define CsrBtAsmConnectAudioResourceReqSend(_appQid, _audioStream)  do\
{                                                                     \
    CsrBtAsmConnectAudioResourceReq *msg = CsrPmemAlloc(sizeof(*msg));\
    msg->type = CSR_BT_ASM_CONNECT_AUDIO_RESOURCE_REQ;                \
    msg->appQid = _appQid;                                            \
    msg->audioStream = _audioStream;                                  \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);     \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmDisconnectAudioResourceReqSend
 *
 *  DESCRIPTION
 *      This shall disconnect the previously connected Audio Resources in the
 *      Audio Stream list represented by the audio stream id returned in the 
 *      CsrBtAsmConnectAudioResourceReqSend API. (Internally ASM shall 
 *      disconnect all the transforms on the chip.)
 *
 *      Note:
 *      1. If the Audio Stream is started then this request shall make sure 
 *         the Audio Stream is stopped before disconnecting the Audio Stream.
 *
 *  PARAMETERS
 *      _appQid        - Application handle
 *      _audioStreamId - Unique identifier representing the Audio Stream.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_DISCONNECT_AUDIO_RESOURCE_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if all the resources in the
 *                  Audio Stream list were disconnected successfully.
 *               2. CSR_BT_RESULT_CODE_ASM_STREAM_NOT_FOUND if no such 
 *                  Audio Stream exists in ASM.
 *               3. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in disconnecting the resource on the chip.
 *
 *      audioStreamId - A non-zero unique identifier representing the 
 *                      successfully connected Audio Stream. 
 *                      Once successfully disconnected then the audioStreamId 
 *                      representing the Audio Stream shall be invalid.
 *----------------------------------------------------------------------------*/
#define CsrBtAsmDisconnectAudioResourceReqSend(_appQid, _audioStreamId) do\
{                                                                         \
    CsrBtAsmDisconnectAudioResourceReq *msg = CsrPmemAlloc(sizeof(*msg)); \
    msg->type = CSR_BT_ASM_DISCONNECT_AUDIO_RESOURCE_REQ;                 \
    msg->appQid = _appQid;                                                \
    msg->audioStreamId = _audioStreamId;                                  \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);         \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmStartAudioStreamReqSend
 *
 *  DESCRIPTION
 *      This request shall start all the operators in the Audio Stream list
 *      represented by _audioStreamId. (Data shall start flowing from one
 *      AudioResource (endpoint) to another).
 *
 *  PARAMETERS
 *      _appQid        - Application handle
 *      _audioStreamId - Unique identifier representing the Audio Stream.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_START_AUDIO_STREAM_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if all the Audio Stream was
 *                  started successfully.
 *               2. CSR_BT_RESULT_CODE_ASM_STREAM_NOT_FOUND if no such 
 *                  Audio Stream exists in ASM.
 *               3. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in starting the Audio Stream (operators) on the chip.
 *
 *      audioStreamId - A non-zero unique identifier representing the 
 *                      successfully connected Audio Stream. 
 *----------------------------------------------------------------------------*/
#define CsrBtAsmStartAudioStreamReqSend(_appQid, _audioStreamId) do\
{                                                                  \
    CsrBtAsmStartAudioStreamReq *msg = CsrPmemAlloc(sizeof(*msg)); \
    msg->type = CSR_BT_ASM_START_AUDIO_STREAM_REQ;                 \
    msg->appQid = _appQid;                                         \
    msg->audioStreamId = _audioStreamId;                           \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);  \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmStopAudioStreamReqSend
 *
 *  DESCRIPTION
 *      This request shall stop all the operators in the Audio Stream list
 *      represented by _audioStreamId. (Data shall stop flowing from one
 *      AudioResource (endpoint) to another and Audio Stream shall 
 *      stay connected).
 *
 *  PARAMETERS
 *      _appQid        - Application handle
 *      _audioStreamId - Unique identifier representing the Audio Stream.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_STOP_AUDIO_STREAM_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if all the Audio Stream was
 *                  stopped successfully.
 *               2. CSR_BT_RESULT_CODE_ASM_STREAM_NOT_FOUND if no such 
 *                  Audio Stream exists in ASM.
 *               3. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in stopping the Audio Stream (operators) on the chip.
 *
 *      audioStreamId - A non-zero unique identifier representing the 
 *                      successfully stopped but connected Audio Stream. 
 *----------------------------------------------------------------------------*/
#define CsrBtAsmStopAudioStreamReqSend(_appQid, _audioStreamId) do\
{                                                                 \
    CsrBtAsmStopAudioStreamReq *msg = CsrPmemAlloc(sizeof(*msg)); \
    msg->type = CSR_BT_ASM_STOP_AUDIO_STREAM_REQ;                 \
    msg->appQid = _appQid;                                        \
    msg->audioStreamId = _audioStreamId;                          \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg); \
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmAddCapabilityReqSend
 *
 *  DESCRIPTION
 *      This request shall download a capability on the chip.
 *      Downloadable capability identifiers range from 
 *      CSR_BT_ASM_AR_TYPE_CAP_DOWNLOAD_lOWER (0x4001 till 0x5fff).
 *
 *  Note:
 *      1. Dowloading capability requires fastpipe. Since fastpipe requires 
 *      buffers to receive the data from host, some RAM has to be reserved.
 *      RAM can be reserved by setting the PSKEY_RESERVE_INIT_TOKENS to required
 *      value. Setting PSKEY_RESERVE_INIT_TOKENS to 1 would
 *      reserve one PSKEY_H_HC_FC_MAX_ACL_PKT_LEN (default=342) bytes.
 *      And Synergy Framework Fastpipe after enabling fastpipe does a resize 
 *      of fastpipe controller buffer to (CSR_FP_LIMIT_HOST). For example 
 *      if CSR_FP_LIMIT_HOST is 2048, PSKEY_H_HC_FC_MAX_ACL_PKT_LEN is 342 
 *      then PSKEY_RESERVE_INIT_TOKENS has to be 6 (6*342=2052).
 *
 *      INIT_TOKENS can be reserved by doing a BCCMD write on id = 0x5053.
 *
 *      Since RAM memory is limited and is shared for other Bluetooth operations
 *      such as ACL, Inquiry, Page Scan operations; the RAM reserved (either
 *      using PSKEY_RESERVE_INIT_TOKENS or via BCCMD write) has to be released
 *      once the download of the capability is completed, otherwise it shall 
 *      impact other Bluetooth operations.
 *      RAM can be released or reclaimed by doing a BCCMD write on id = 0x5054.
 *      
 *      2. All the Downloadable capabilities have to be downloaded before 
 *      starting any other operators. If not there could be undesirable 
 *      consequences; for instance if a WBS operator is started and then 
 *      an aptX capability is downloaded this could lead to drop of 
 *      SCO packets. So it is advised for applications to download the 
 *      capabilities before realizing any Audio use cases.
 *
 *  PARAMETERS
 *      _appQid     - Application handle
 *      _capId      - Capability Identifier of the capability to be downloaded.
 *                    When this capability is downloaded successfully this 
 *                    id shall be used to instantiate this capability as part
 *                    CsrBtAsmGetAudioResourceReqSend().
 *      _data       - This buffer points to the capability to be downloaded.
 *      _dataLength - Size of the capability data held by _data buffer.
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_ADD_CAPABILITY_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if capability was
 *                  downloaded successfully.
 *               2. CSR_BT_RESULT_CODE_ASM_NOT_ACTIVATED if this request is 
 *                  received when ASM is not Activated.
 *               3. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in stopping the Audio Stream (operators) on the chip.
 *
 *      _capId - Capability Identifier of the downloaded capability. 
 *----------------------------------------------------------------------------*/
#define CsrBtAsmAddCapabilityReqSend\
(                  \
    _appQid,       \
    _capId,        \
    _data,         \
    _dataLength    \
) do\
{                                                                \
    CsrBtAsmAddCapabilityReq *msg = CsrPmemAlloc(sizeof(*msg));  \
    msg->type = CSR_BT_ASM_ADD_CAPABILITY_REQ;                   \
    msg->appQid = _appQid;                                       \
    msg->capId = _capId;                                         \
    msg->data = _data;                                           \
    msg->dataLength = _dataLength;                               \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg);\
} while (0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAsmRemoveCapabilityReqSend
 *
 *  DESCRIPTION
 *      This request shall remove the downloaded capability on the chip.
 *
 *      Note:
 *      1. Before calling this applications have to make sure the added 
 *         capability is released by calling 
 *         CsrBtAsmReleaseAudioResourceReqSend(). This is a must since 
 *         removing a capability is not possible if the downloaded capability
 *         is created (instantiated via CsrBtAsmGetAudioResourceReqSend()).
 *
 *  PARAMETERS
 *      _appQid     - Application handle
 *      _capId      - Capability Identifier of the successfully downloaded 
 *                    capability via CsrBtAsmAddCapabilityReqSend().
 *
 *  CONFIRMATION
 *      For this request applications shall receive 
 *      CSR_BT_ASM_REMOVE_CAPABILITY_CFM with following parameters:
 *
 *      result - 1. CSR_BT_RESULT_CODE_ASM_SUCCESS if downloaded capability 
 *                  was removed successfully.
 *               2. CSR_BT_RESULT_CODE_ASM_FAILURE indicates there was a 
 *                  failure in removing the downloaded capability.
 *
 *      _capId - Identifier of the removed capability.
 *----------------------------------------------------------------------------*/
#define CsrBtAsmRemoveCapabilityReqSend(_appQid, _capId)        do\
{                                                                 \
    CsrBtAsmRemoveCapabilityReq *msg = CsrPmemAlloc(sizeof(*msg));\
    msg->type = CSR_BT_ASM_REMOVE_CAPABILITY_REQ;                 \
    msg->appQid = _appQid;                                        \
    msg->capId = _capId;                                          \
    CsrMsgTransport(CSR_BT_ASM_IFACEQUEUE, CSR_BT_ASM_PRIM, msg); \
} while (0)

void CsrBtAsmFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif

