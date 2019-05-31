#ifndef CSR_BT_PROXC_APP_UI_SEF_H__
#define CSR_BT_PROXC_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_gatt_demo_prox_client_app.h"
#ifdef __cplusplus
extern "C" {
#endif


void CsrBtGattAppHandleProxcCsrUiRegisterUnregister(CsrBtProxcAppInstData *inst, CsrBool registered);
void CsrBtGattAppHandleProxcCsrUiConnectDisconnect(CsrBtProxcAppInstData *inst, CsrBool connected, CsrBool success);
void CsrBtGattAppHandleProxcSetMainMenuHeader(CsrBtProxcAppInstData *inst);
void CsrBtGattAppHandleProxcCsrUiNoSelectedDevAddr(CsrBtProxcAppInstData *inst);
void CsrBtGattAppHandleProxcCsrUiReadValue(CsrBtProxcAppInstData *inst, char *str, CsrBool success);
void CsrBtGattAppHandleProxcCsrUiWriteValue(CsrBtProxcAppInstData *inst,  CsrBool success);
void CsrBtGattAppHandleProxcCsrUiMenuUpdate(CsrBtProxcAppInstData *inst);



void CsrBtGattAppHandleProxcCsrUiPrim(CsrBtProxcAppInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PROXC_APP_UI_SEF_H__ */

