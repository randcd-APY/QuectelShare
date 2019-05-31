#ifndef CSR_BT_THERMS_APP_UI_SEF_H__
#define CSR_BT_THERMS_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_bt_gatt_demo_therm_server_app.h"
#ifdef __cplusplus
extern "C" {
#endif


    void CsrBtGattAppHandleThermsCsrUiRegisterUnregister(CsrBtThermsAppInstData *inst,
                                                        CsrBool registered);
    void CsrBtGattAppHandleThermsCsrUiActivateDeactivate(CsrBtThermsAppInstData *inst,
                                                        CsrBool connected,
                                                        CsrBool success);
    void CsrBtGattAppHandleThermsSetMainMenuHeader(CsrBtThermsAppInstData *inst);
    void CsrBtGattAppHandleThermsCsrUiMenuUpdate(CsrBtThermsAppInstData *inst);
    void CsrBtGattAppHandleThermsCsrUiValueUpdate(CsrBtThermsAppInstData *inst, 
                                                 CsrUint16 handle, 
                                                 CsrUint16 value);



        void CsrBtGattAppHandleThermsCsrUiPrim(CsrBtThermsAppInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_THERMS_APP_UI_SEF_H__ */

