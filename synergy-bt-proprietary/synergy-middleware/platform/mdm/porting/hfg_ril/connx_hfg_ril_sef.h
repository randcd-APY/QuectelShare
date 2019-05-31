/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_HFG_RIL_SEF_H_
#define _CONNX_HFG_RIL_SEF_H_

#include "connx_hfg_ril_main.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Handler for mcm voice response. */
void HandleMcmVoiceGetCallsResp(void *resp, uint32 resp_len);
void HandleMcmVoiceDialResp(void *resp, uint32 resp_len);
void HandleMcmVoiceGetCallStatusResp(void *resp, uint32 resp_len);
void HandleMcmVoiceDtmfResp(void *resp, uint32 resp_len);
void HandleMcmVoiceStartDtmfResp(void *resp, uint32 resp_len);
void HandleMcmVoiceStopDtmfResp(void *resp, uint32 resp_len);
void HandleMcmVoiceMuteResp(void *resp, uint32 resp_len);
void HandleMcmVoiceFlashResp(void *resp, uint32 resp_len);
void HandleMcmVoiceHangupResp(void *resp, uint32 resp_len);
void HandleMcmVoiceCommandResp(void *resp, uint32 resp_len);
void HandleMcmVoiceAutoAnswerResp(void *resp, uint32 resp_len);
void HandleMcmVoiceEventRegisterResp(void *resp, uint32 resp_len);
void HandleMcmVoiceGetCallForwardingStatusResp(void *resp, uint32 resp_len);
void HandleMcmVoiceSetCallForwardingResp(void *resp, uint32 resp_len);
void HandleMcmVoiceGetCallWaitingStatusResp(void *resp, uint32 resp_len);
void HandleMcmVoiceSetCallWaitingResp(void *resp, uint32 resp_len);
void HandleMcmVoiceGetClirResp(void *resp, uint32 resp_len);
void HandleMcmVoiceSetClirResp(void *resp, uint32 resp_len);
void HandleMcmVoiceSetFacilityLockResp(void *resp, uint32 resp_len);
void HandleMcmVoiceChangeCallBarringPasswordResp(void *resp, uint32 resp_len);
void HandleMcmVoiceSendUssdResp(void *resp, uint32 resp_len);
void HandleMcmVoiceCancelUssdResp(void *resp, uint32 resp_len);
void HandleMcmVoiceCommonDialResp(void *resp, uint32 resp_len);
void HandleMcmVoiceUpdateEcallMsdResp(void *resp, uint32 resp_len);

/* Handler for mcm voice indicator. */
void HandleMcmVoiceCallInd(void *ind, uint32 ind_len);
void HandleMcmVoiceMuteInd(void *ind, uint32 ind_len);
void HandleMcmVoiceDtmfInd(void *ind, uint32 ind_len);
void HandleMcmVoiceReceiveUssdInd(void *ind, uint32 ind_len);

/* Handler for mcm sim response. */
void HandleMcmSimGetPhoneNumberResp(void *resp, uint32 resp_len);

/* Handler for mcm network response. */
void HandleMcmNwGetOperatorNameResp(void *resp, uint32 resp_len);


#ifdef __cplusplus
extern "C" }
#endif

#endif  /* _CONNX_HFG_RIL_SEF_H_ */