#ifndef CSR_BT_PROXS_APP_UI_SEF_H__
#define CSR_BT_PROXS_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_bt_gatt_demo_prox_server_app.h"
#ifdef __cplusplus
extern "C" {
#endif


    void CsrBtGattAppHandleProxsCsrUiRegisterUnregister(CsrBtProxsAppInstData *inst,
                                                        CsrBool registered);
    void CsrBtGattAppHandleProxsCsrUiActivateDeactivate(CsrBtProxsAppInstData *inst,
                                                        CsrBool connected,
                                                        CsrBool success);
    void CsrBtGattAppHandleProxsSetMainMenuHeader(CsrBtProxsAppInstData *inst);
    void CsrBtGattAppHandleProxsCsrUiMenuUpdate(CsrBtProxsAppInstData *inst);
    void CsrBtGattAppHandleProxsCsrUiValueUpdate(CsrBtProxsAppInstData *inst, 
                                                 CsrUint16 handle, 
                                                 CsrUint16 value);



        void CsrBtGattAppHandleProxsCsrUiPrim(CsrBtProxsAppInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PROXS_APP_UI_SEF_H__ */

