#ifndef CSR_BT_AV_APP_SEF_H__
#define CSR_BT_AV_APP_SEF_H__

/****************************************************************************

Copyright (c) 2012-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"

#include "csr_bt_av2.h"
#include "csr_bt_av_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

void avAppActivateReqHandler(av2instance_t *instData, CsrBtAvAppActivateReq *msg);
void avAppConnectReqHandler(av2instance_t *instData, CsrBtAvAppConnectReq *msg);
void avAppDisconnectReqHandler(av2instance_t *instData, CsrBtAvAppDisconnectReq *msg);
void avAppPlayReqHandler(av2instance_t *instData, CsrBtAvAppPlayReq *msg);
void avAppPauseReqHandler(av2instance_t *instData, CsrBtAvAppPauseReq *msg);







#ifdef __cplusplus
}
#endif



#endif /* CSR_BT_AV_APP_SEF_H__ */
