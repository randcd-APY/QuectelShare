#ifndef CSR_BT_HF_APP_SEF_H__
#define CSR_BT_HF_APP_SEF_H__

#include "csr_synergy.h"
/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "csr_bt_hf_demo_app.h"
#include "csr_bt_hf_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

void hfAppActivateReqHandler(DemoInstdata_t *instData, CsrBtHfAppActivateReq *msg);
void hfAppConnectReqHandler(DemoInstdata_t *instData, CsrBtHfAppConnectReq *msg);
void hfAppDisconnectReqHandler(DemoInstdata_t *instData, CsrBtHfAppDisconnectReq *msg);
void hfAppDialReqHandler(DemoInstdata_t *instData, CsrBtHfAppDialReq *msg);
void hfAppCallAnsRejReq(DemoInstdata_t *instData, CsrBtHfAppCallAnsRejReq *msg);
void hfAppAudioConnectReqhandler(DemoInstdata_t *instData, CsrBtHfAppAudioConnectReq *msg);
void hfAppAudioDisconnectReqhandler(DemoInstdata_t *instData, CsrBtHfAppAudioDisconnectReq *msg);
InstHfIndicators *fetchRemoteHfIndicator(DemoInstdata_t *inst, CsrBtHfpHfIndicatorId indID);
CsrBtHfpHfIndicatorId *CsrBtHfBuildLocalHfIndicatorList(DemoInstdata_t * inst);

#ifdef __cplusplus
}
#endif



#endif /* CSR_BT_HF_APP_SEF_H__ */

