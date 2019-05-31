#ifndef CSR_BT_RSCS_APP_UI_SEF_H__
#define CSR_BT_RSCS_APP_UI_SEF_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_gatt_demo_rsc_server_app.h"
#ifdef __cplusplus
extern "C" {
#endif

void CsrBtGattAppHandleRscsCsrUiRegisterUnregister(CsrBtRscsAppInstData *inst,
                                                   CsrBool registered);
void CsrBtGattAppHandleRscsCsrUiActivateDeactivate(CsrBtRscsAppInstData *inst,
                                                   CsrBool connected,
                                                   CsrBool success);
void CsrBtGattAppHandleRscsSetMainMenuHeader(CsrBtRscsAppInstData *inst);
void CsrBtGattAppHandleRscsCsrUiMenuUpdate(CsrBtRscsAppInstData *inst);
void CsrBtGattAppHandleRscsCsrUiValueUpdate(CsrBtRscsAppInstData *inst,
                                            CsrUint16 handle, 
                                            CsrUint16 value);
void CsrBtGattAppHandleRscsCsrUiPrim(CsrBtRscsAppInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_RSCS_APP_UI_SEF_H__ */

