#ifndef CSR_DSPM_LIB_H__
#define CSR_DSPM_LIB_H__
/*****************************************************************************

Copyright (c) 2011-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_dspm_prim.h"
#include "csr_msg_transport.h"
#include "csr_dspm_task.h"
#include "csr_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrDspmFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#define CsrDspmActivateReqSend(qid__) \
    { \
        CsrDspmActivateReq *req__ = CsrPmemAlloc(sizeof(*req__));   \
        req__->type = CSR_DSPM_ACTIVATE_REQ;                        \
        req__->qid = qid__;                                         \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__); \
    }

#define CsrDspmDeactivateReqSend(qid__) \
    { \
        CsrDspmDeactivateReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_DEACTIVATE_REQ;                      \
        req__->qid = qid__;                                         \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__); \
    }

#define CsrDspmStreamCpOpenReqSend(qid__, cpInfoCount__, cpInfo__) \
    { \
        CsrDspmStreamCpOpenReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_STREAM_CP_OPEN_REQ;                    \
        req__->qid = qid__;                                           \
        req__->cpInfoCount = cpInfoCount__;                           \
        req__->cpInfo = cpInfo__;                                     \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);   \
    }

#define CsrDspmStreamCpCloseReqSend(qid__, cpidCount__, cpid__) \
    { \
        CsrDspmStreamCpCloseReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_STREAM_CP_CLOSE_REQ;                    \
        req__->qid = qid__;                                            \
        req__->cpidCount = cpidCount__;                                \
        req__->cpid = cpid__;                                          \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);    \
    }

#define CsrDspmStreamScoEnableReqSend(qid__, enable__) \
    { \
        CsrDspmStreamScoEnableReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_STREAM_SCO_ENABLE_REQ;                    \
        req__->qid = qid__;                                              \
        req__->enable = enable__;                                        \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);      \
    }

#define CsrDspmStreamConfigureReqSend(qid__, configInfoCount__, configInfo__) \
    { \
        CsrDspmStreamConfigureReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_STREAM_CONFIGURE_REQ;                     \
        req__->qid = qid__;                                              \
        req__->configInfoCount = configInfoCount__;                      \
        req__->configInfo = configInfo__;                                \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);      \
    }

#define CsrDspmStreamSyncReqSend(qid__, syncInfoCount__, syncInfo__) \
    { \
        CsrDspmStreamSyncReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_STREAM_SYNC_REQ;                     \
        req__->qid = qid__;                                         \
        req__->syncInfoCount = syncInfoCount__;                     \
        req__->syncInfo = syncInfo__;                               \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__); \
    }

#define CsrDspmOperatorCreateReqSend(qid__, operatorInfoCount__, operatorInfo__) \
    { \
        CsrDspmOperatorCreateReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_OPERATOR_CREATE_REQ;                     \
        req__->qid = qid__;                                             \
        req__->operatorInfoCount = operatorInfoCount__;                 \
        req__->operatorInfo = operatorInfo__;                           \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);     \
    }

#define CsrDspmOperatorCreateExReqSend(qid__, capability__, keyValueCount__, keyValue__) \
    { \
        CsrDspmOperatorCreateExReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_OPERATOR_CREATE_EX_REQ;                  \
        req__->qid = qid__;                                             \
        req__->capability = capability__;                               \
        req__->keyValueCount = keyValueCount__;                                     \
        req__->keyValue = keyValue__;                                   \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);     \
    }

#define CsrDspmOperatorDestroyReqSend(qid__, opidCount__, opid__) \
    { \
        CsrDspmOperatorDestroyReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_OPERATOR_DESTROY_REQ;                     \
        req__->qid = qid__;                                              \
        req__->opidCount = opidCount__;                                  \
        req__->opid = opid__;                                            \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);      \
    }

#define CsrDspmOperatorMessageReqSend(qid__, operatorMessageInfoCount__, operatorMessageInfo__) \
    { \
        CsrDspmOperatorMessageReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_OPERATOR_MESSAGE_REQ;                     \
        req__->qid = qid__;                                              \
        req__->operatorMessageInfoCount = operatorMessageInfoCount__;    \
        req__->operatorMessageInfo = operatorMessageInfo__;              \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);      \
    }

#define CsrDspmOperatorStartReqSend(qid__, opidCount__, opid__) \
    { \
        CsrDspmOperatorStartReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_OPERATOR_START_REQ;                     \
        req__->qid = qid__;                                            \
        req__->opidCount = opidCount__;                                \
        req__->opid = opid__;                                          \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);    \
    }

#define CsrDspmOperatorStopReqSend(qid__, opidCount__, opid__) \
    { \
        CsrDspmOperatorStopReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_OPERATOR_STOP_REQ;                     \
        req__->qid = qid__;                                           \
        req__->opidCount = opidCount__;                               \
        req__->opid = opid__;                                         \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);   \
    }

#define CsrDspmOperatorResetReqSend(qid__, opidCount__, opid__) \
    { \
        CsrDspmOperatorResetReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_OPERATOR_RESET_REQ;                     \
        req__->qid = qid__;                                            \
        req__->opidCount = opidCount__;                                \
        req__->opid = opid__;                                          \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);    \
    }

#define CsrDspmConnectionCreateReqSend(qid__, connectionInfoCount__, connectionInfo__) \
    { \
        CsrDspmConnectionCreateReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_CONNECTION_CREATE_REQ;                     \
        req__->qid = qid__;                                               \
        req__->connectionInfoCount = connectionInfoCount__;               \
        req__->connectionInfo = connectionInfo__;                         \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);       \
    }

#define CsrDspmConnectionDestroyReqSend(qid__, cidCount__, cid__) \
    { \
        CsrDspmConnectionDestroyReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_CONNECTION_DESTROY_REQ;                     \
        req__->qid = qid__;                                                \
        req__->cidCount = cidCount__;                                      \
        req__->cid = cid__;                                                \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);        \
    }

#define CsrDspmCapabilityDownloadReqSend(qid__, capability__, versionMajor__, versionMinor__, data__) \
    { \
        CsrDspmCapabilityDownloadReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_CAPABILITY_DOWNLOAD_REQ;                     \
        req__->qid = qid__;                                                 \
        req__->capability = capability__;                                   \
        req__->versionMajor = versionMajor__;                               \
        req__->versionMinor = versionMinor__;                               \
        req__->data = data__;                                               \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);         \
    }

#define CsrDspmCapabilityRemoveReqSend(qid__, capability__) \
    { \
        CsrDspmCapabilityRemoveReq *req__ = CsrPmemAlloc(sizeof(*req__)); \
        req__->type = CSR_DSPM_CAPABILITY_REMOVE_REQ;                     \
        req__->qid = qid__;                                               \
        req__->capability = capability__;                                 \
        CsrMsgTransport(CSR_DSPM_IFACEQUEUE, CSR_DSPM_PRIM, req__);       \
    }

#define CsrDspmActivateCfmSend(queue__) \
    { \
        CsrDspmActivateCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_ACTIVATE_CFM;                      \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);           \
    }

#define CsrDspmDeactivateCfmSend(queue__) \
    { \
        CsrDspmDeactivateCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_DEACTIVATE_CFM;                      \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);             \
    }

#define CsrDspmDeactivateIndSend(queue__) \
    { \
        CsrDspmDeactivateInd *ind__ = CsrPmemAlloc(sizeof(*ind__)); \
        ind__->type = CSR_DSPM_DEACTIVATE_IND;                      \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, ind__);             \
    }

#define CsrDspmStreamCpOpenCfmSend(queue__, result__, successes__, cpInfoCount__, cpInfo__) \
    { \
        CsrDspmStreamCpOpenCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_STREAM_CP_OPEN_CFM;                    \
        cfm__->result = result__;                                     \
        cfm__->successes = successes__;                               \
        cfm__->cpInfoCount = cpInfoCount__;                           \
        cfm__->cpInfo = cpInfo__;                                     \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);               \
    }

#define CsrDspmStreamCpCloseCfmSend(queue__, cpidCount__, cpid__) \
    { \
        CsrDspmStreamCpCloseCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_STREAM_CP_CLOSE_CFM;                    \
        cfm__->cpidCount = cpidCount__;                                \
        cfm__->cpid = cpid__;                                          \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                \
    }

#define CsrDspmStreamScoEnableCfmSend(queue__, result__) \
    { \
        CsrDspmStreamScoEnableCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_STREAM_SCO_ENABLE_CFM;                    \
        cfm__->result = result__;                                        \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                  \
    }

#define CsrDspmStreamConfigureCfmSend(queue__, result__, successes__, configInfoCount__, configInfo__) \
    { \
        CsrDspmStreamConfigureCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_STREAM_CONFIGURE_CFM;                     \
        cfm__->result = result__;                                        \
        cfm__->successes = successes__;                                  \
        cfm__->configInfoCount = configInfoCount__;                      \
        cfm__->configInfo = configInfo__;                                \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                  \
    }

#define CsrDspmStreamSyncCfmSend(queue__, result__, successes__, syncInfoCount__, syncInfo__) \
    { \
        CsrDspmStreamSyncCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_STREAM_SYNC_CFM;                     \
        cfm__->result = result__;                                   \
        cfm__->successes = successes__;                             \
        cfm__->syncInfoCount = syncInfoCount__;                     \
        cfm__->syncInfo = syncInfo__;                               \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);             \
    }

#define CsrDspmOperatorCreateCfmSend(queue__, result__, successes__, operatorInfoCount__, operatorInfo__) \
    { \
        CsrDspmOperatorCreateCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_OPERATOR_CREATE_CFM;                     \
        cfm__->result = result__;                                       \
        cfm__->successes = successes__;                                 \
        cfm__->operatorInfoCount = operatorInfoCount__;                 \
        cfm__->operatorInfo = operatorInfo__;                           \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                 \
    }

#define CsrDspmOperatorCreateExCfmSend(queue__, result__, opid__) \
    { \
        CsrDspmOperatorCreateExCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_OPERATOR_CREATE_EX_CFM;                     \
        cfm__->result = result__;                                       \
        cfm__->opid = opid__;                           \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                 \
    }

#define CsrDspmOperatorDestroyCfmSend(queue__, result__, successes__, opidCount__, opid__) \
    { \
        CsrDspmOperatorDestroyCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_OPERATOR_DESTROY_CFM;                     \
        cfm__->result = result__;                                        \
        cfm__->successes = successes__;                                  \
        cfm__->opidCount = opidCount__;                                  \
        cfm__->opid = opid__;                                            \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                  \
    }

#define CsrDspmOperatorMessageCfmSend(queue__, result__, successes__, operatorMessageInfoCount__, operatorMessageInfo__) \
    { \
        CsrDspmOperatorMessageCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_OPERATOR_MESSAGE_CFM;                     \
        cfm__->result = result__;                                        \
        cfm__->successes = successes__;                                  \
        cfm__->operatorMessageInfoCount = operatorMessageInfoCount__;    \
        cfm__->operatorMessageInfo = operatorMessageInfo__;              \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                  \
    }

#define CsrDspmOperatorMessageIndSend(queue__, opid__, messageLength__, message__) \
    { \
        CsrDspmOperatorMessageInd *ind__ = CsrPmemAlloc(sizeof(*ind__)); \
        ind__->type = CSR_DSPM_OPERATOR_MESSAGE_IND;                     \
        ind__->opid = opid__;                                            \
        ind__->messageLength = messageLength__;                          \
        ind__->message = message__;                                      \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, ind__);                  \
    }

#define CsrDspmOperatorStartCfmSend(queue__, result__, successes__, opidCount__, opid__) \
    { \
        CsrDspmOperatorStartCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_OPERATOR_START_CFM;                     \
        cfm__->result = result__;                                      \
        cfm__->successes = successes__;                                \
        cfm__->opidCount = opidCount__;                                \
        cfm__->opid = opid__;                                          \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                \
    }

#define CsrDspmOperatorStopCfmSend(queue__, result__, successes__, opidCount__, opid__) \
    { \
        CsrDspmOperatorStopCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_OPERATOR_STOP_CFM;                     \
        cfm__->result = result__;                                     \
        cfm__->successes = successes__;                               \
        cfm__->opidCount = opidCount__;                               \
        cfm__->opid = opid__;                                         \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);               \
    }

#define CsrDspmOperatorResetCfmSend(queue__, result__, successes__, opidCount__, opid__) \
    { \
        CsrDspmOperatorResetCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_OPERATOR_RESET_CFM;                     \
        cfm__->result = result__;                                      \
        cfm__->successes = successes__;                                \
        cfm__->opidCount = opidCount__;                                \
        cfm__->opid = opid__;                                          \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                \
    }

#define CsrDspmConnectionCreateCfmSend(queue__, result__, successes__, connectionInfoCount__, connectionInfo__) \
    { \
        CsrDspmConnectionCreateCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_CONNECTION_CREATE_CFM;                     \
        cfm__->result = result__;                                         \
        cfm__->successes = successes__;                                   \
        cfm__->connectionInfoCount = connectionInfoCount__;               \
        cfm__->connectionInfo = connectionInfo__;                         \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                   \
    }

#define CsrDspmConnectionDestroyCfmSend(queue__, cidCount__, cid__) \
    { \
        CsrDspmConnectionDestroyCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_CONNECTION_DESTROY_CFM;                     \
        cfm__->cidCount = cidCount__;                                      \
        cfm__->cid = cid__;                                                \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                    \
    }

#define CsrDspmCapabilityDownloadCfmSend(queue__, result__, capability__) \
    { \
        CsrDspmCapabilityDownloadCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_CAPABILITY_DOWNLOAD_CFM;                     \
        cfm__->result = result__;                                           \
        cfm__->capability = capability__;                                   \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                     \
    }

#define CsrDspmCapabilityRemoveCfmSend(queue__, result__, capability__) \
    { \
        CsrDspmCapabilityRemoveCfm *cfm__ = CsrPmemAlloc(sizeof(*cfm__)); \
        cfm__->type = CSR_DSPM_CAPABILITY_REMOVE_CFM;                     \
        cfm__->result = result__;                                         \
        cfm__->capability = capability__;                                 \
        CsrMsgTransport(queue__, CSR_DSPM_PRIM, cfm__);                   \
    }

#ifdef __cplusplus
}
#endif

#endif
