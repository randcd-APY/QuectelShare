#ifndef CSR_BT_THERMC_APP_UI_SEF_H__
#define CSR_BT_THERMC_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_gatt_demo_therm_client_app.h"
#ifdef __cplusplus
extern "C" {
#endif


void CsrBtGattAppHandleThermcCsrUiRegisterUnregister(CsrBtThermcAppInstData *inst, CsrBool registered);
void CsrBtGattAppHandleThermcCsrUiConnectDisconnect(CsrBtThermcAppInstData *inst, CsrBool connected, CsrBool success);
void CsrBtGattAppHandleThermcSetMainMenuHeader(CsrBtThermcAppInstData *inst);
void CsrBtGattAppHandleThermcCsrUiNoSelectedDevAddr(CsrBtThermcAppInstData *inst);
void CsrBtGattAppHandleThermcCsrUiReadValue(CsrBtThermcAppInstData *inst, char *str, CsrBool success);
void CsrBtGattAppHandleThermcCsrUiWriteValue(CsrBtThermcAppInstData *inst,  CsrBool success);
void CsrBtGattAppHandleThermcCsrUiMenuUpdate(CsrBtThermcAppInstData *inst);



void CsrBtGattAppHandleThermcCsrUiPrim(CsrBtThermcAppInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_THERMC_APP_UI_SEF_H__ */

