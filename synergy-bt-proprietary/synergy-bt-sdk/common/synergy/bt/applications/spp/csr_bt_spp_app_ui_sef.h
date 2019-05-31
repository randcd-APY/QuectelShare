#ifndef CSR_BT_SPP_APP_UI_SEF_H__
#define CSR_BT_SPP_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CsrBtSppAppHandleSppCsrUiActivateDeactivate(CsrBtSppAppGlobalInstData *inst, CsrBool activated);
void CsrBtSppAppHandleSppCsrUiConnectDisconnect(CsrBtSppAppGlobalInstData *inst, CsrBool connected, CsrBool success);
void CsrBtSppAppHandleSppCsrUiServiceNameList(CsrBtSppAppGlobalInstData *inst, CsrUint16 listEntries, CsrBtSppServiceName *serviceNameList);
void CsrBtSppAppHandleSppCsrUiSendFile(CsrBtSppAppGlobalInstData *inst, CsrBool CsrBtSppAppStarted);
void CsrBtSppAppHandleSppSetMainMenuHeader(CsrBtSppAppGlobalInstData *inst);



void CsrBtSppAppHandleSppCsrUiPrim(CsrBtSppAppGlobalInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_SPP_APP_UI_SEF_H__ */

