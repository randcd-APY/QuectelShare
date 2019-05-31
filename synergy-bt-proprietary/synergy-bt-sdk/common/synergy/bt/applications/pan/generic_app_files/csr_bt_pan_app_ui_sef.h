#ifndef CSR_BT_PAN_APP_UI_SEF_H__
#define CSR_BT_PAN_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_pan_app.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrBtPanAppHandlePanCsrUiActivateDeactivate(CsrBtPanAppGlobalInstData *inst, CsrBool activated);
void CsrBtPanAppHandlePanCsrUiConnectDisconnect(CsrBtPanAppGlobalInstData *inst, CsrBool connected, CsrBool success);
void CsrBtPanAppHandlePanSetMainMenuHeader(CsrBtPanAppGlobalInstData *inst);
void CsrBtPanAppHandlePanCsrUiRoleSelected(CsrBtPanAppGlobalInstData *inst, CsrBool activated);
void CsrBtPanAppHandlePanCsrUiConnectSelected(CsrBtPanAppGlobalInstData *inst);
void CsrBtPanAppHandlePanCsrUiPrim(CsrBtPanAppGlobalInstData *inst);
void commonPanPopupSet(CsrBtPanAppGlobalInstData *inst, char *dialogHeading, char *dialogText);
void commonPanPopupShow(CsrBtPanAppGlobalInstData *inst);
void commonPanPopupHide(CsrBtPanAppGlobalInstData *inst);
void CsrBtPanSetDialog(CsrBtPanAppGlobalInstData * inst, CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2);
void CsrBtPanShowUi(CsrBtPanAppGlobalInstData * inst,
                                CsrUint16 inputMode, CsrUint16 priority);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PAN_APP_UI_SEF_H__ */

