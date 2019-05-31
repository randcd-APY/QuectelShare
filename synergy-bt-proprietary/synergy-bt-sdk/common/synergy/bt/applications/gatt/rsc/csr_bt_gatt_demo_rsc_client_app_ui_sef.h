#ifndef CSR_BT_RSCC_APP_UI_SEF_H__
#define CSR_BT_RSCC_APP_UI_SEF_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_gatt_demo_rsc_client_app.h"
#ifdef __cplusplus
extern "C" {
#endif

void CsrBtGattAppHandleRsccCsrUiRegisterUnregister(CsrBtRsccAppInstData *inst, CsrBool registered);
void CsrBtGattAppHandleRsccCsrUiConnectDisconnect(CsrBtRsccAppInstData *inst, CsrBool connected, CsrBool success);
void CsrBtGattAppHandleRsccSetMainMenuHeader(CsrBtRsccAppInstData *inst);
void CsrBtGattAppHandleRsccCsrUiNoSelectedDevAddr(CsrBtRsccAppInstData *inst);
void CsrBtGattAppHandleRsccCsrUiReadValue(CsrBtRsccAppInstData *inst, char *str, CsrBool success);
void CsrBtGattAppHandleRsccCsrUiWriteValue(CsrBtRsccAppInstData *inst,  CsrBool success);
void CsrBtGattAppHandleRsccCsrUiWriteError81(CsrBtRsccAppInstData *inst);
void CsrBtGattAppHandleRsccCsrUiMenuUpdate(CsrBtRsccAppInstData *inst);
void CsrBtGattAppHandleRsccCsrUiTraversingdb(CsrBtRsccAppInstData *inst, CsrBool traversing);
void CsrBtGattAppHandleRsccCsrUiShowIndInfo(CsrBtRsccAppInstData *inst, CsrUint16 valueLength, CsrUint8* value);
void CsrBtGattAppHandleRsccCsrUiPrim(CsrBtRsccAppInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_RSCC_APP_UI_SEF_H__ */

