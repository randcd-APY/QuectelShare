#ifndef CSR_BT_GenericSrv_APP_UI_SEF_H__
#define CSR_BT_GenericSrv_APP_UI_SEF_H__
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
 ****************************************************************************/
#include "csr_synergy.h"
#include "csr_bt_gatt_demo_generic_server_app.h"

#ifdef __cplusplus
extern "C" {
#endif
/* app csrUi event states                                                 */
#define CSR_BT_GENERICSRV_CREATE_SK1_EVENT                                     0x01
#define CSR_BT_GENERICSRV_CREATE_SK2_EVENT                                     0x02
#define CSR_BT_GENERICSRV_CREATE_DEL_EVENT                                     0x03
#define CSR_BT_GENERICSRV_CREATE_BACK_EVENT                                    0x04

void CsrBtGattAppHandleGenericSrvCsrUiRegisterUnregister(CsrBtGenericSrvAppInstData *inst,
                                                         CsrBool registered);
void CsrBtGattAppHandleGenericSrvCsrUiActivateDeactivate(CsrBtGenericSrvAppInstData *inst,
                                                         CsrBool connected,
                                                         CsrBool success);
void CsrBtGattAppHandleGenericSrvSetMainMenuHeader(CsrBtGenericSrvAppInstData *inst);
    
void CsrBtGattAppHandleGenericSrvCsrUiValueUpdate(CsrBtGenericSrvAppInstData *inst, 
                                                  CsrUint16 handle, 
                                                  CsrUint16 value);
void CsrBtGattAppHandleGenericSrvCsrUiPrim(CsrBtGenericSrvAppInstData *inst);
void CsrBtGattAppAcceptPopup(CsrBtGenericSrvAppInstData *inst, char *dialogHeading, char *dialogText);
void CsrBtGattAppHandleGenericSrvCsrUiMenuUpdate(CsrBtGenericSrvAppInstData *inst, CsrBool indication, CsrBool notify);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GenericSrv_APP_UI_SEF_H__ */

