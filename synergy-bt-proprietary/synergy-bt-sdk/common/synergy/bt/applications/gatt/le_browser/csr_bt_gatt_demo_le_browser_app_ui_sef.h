#ifndef CSR_BT_LE_BROWSERC_APP_UI_SEF_H__
#define CSR_BT_LE_BROWSERC_APP_UI_SEF_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_bt_gatt_demo_le_browser_app.h"
#ifdef __cplusplus
extern "C" {
#endif


void CsrBtGattAppHandleLeBrowserCsrUiRegisterUnregister(CsrBtLeBrowserAppInstData *inst, CsrBool registered);
void CsrBtGattAppHandleLeBrowserCsrUiConnectDisconnect(CsrBtLeBrowserAppInstData *inst, CsrBool connected, CsrBool success);
void CsrBtGattAppHandleLeBrowserSetMainMenuHeader(CsrBtLeBrowserAppInstData *inst);
void CsrBtGattAppHandleLeBrowserCsrUiNoSelectedDevAddr(CsrBtLeBrowserAppInstData *inst);
void CsrBtGattAppHandleLeBrowserCsrUiReadValue(CsrBtLeBrowserAppInstData *inst, CsrBool success);
void CsrBtGattAppHandleLeBrowserCsrUiWriteValue(CsrBtLeBrowserAppInstData *inst,  CsrBool success);
void CsrBtGattAppHandleLeBrowserCsrUiMenuUpdate(CsrBtLeBrowserAppInstData *inst, CsrUint16);
void CsrBtGattAppHandleLeBrowserCsrUiStillTraversingDb(CsrBtLeBrowserAppInstData *inst);

void CsrBtGattAppHandleLeBrowserCsrUiHidePopup(CsrBtLeBrowserAppInstData *inst);
void CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupSet(CsrBtLeBrowserAppInstData *inst, const char *dialogHeading, const char *dialogText);
void CsrBtGattAppHandleLeBrowserCsrUiAcceptPopupShow(CsrBtLeBrowserAppInstData *inst);
void CsrBtGattAppHandleLeBrowserCsrUiViewPopupSet(CsrBtLeBrowserAppInstData *inst, const char *dialogHeading, const char *dialogText);
void CsrBtGattAppHandleLeBrowserCsrUiViewPopupShow(CsrBtLeBrowserAppInstData *inst);

void CsrBtGattAppHandleLeBrowserCsrUiListDatabaseEntries(CsrBtLeBrowserAppInstData *inst, CsrUint16 menuState);
void CsrBtGattAppHandleLeBrowserCsrUiPrim(CsrBtLeBrowserAppInstData *inst);

void CsrBtLeBrowserPopUpHide(CsrBtLeBrowserAppInstData *inst);   
void CsrBtLeBrowserEditMenuHide(CsrBtLeBrowserAppInstData *inst);


/* Menu states */
#define CSR_BT_LE_BROWSER_MAIN_MENU_STATE                   0x00
#define CSR_BT_LE_BROWSER_PRIMARY_SERVICES_MENU_STATE       0x01
#define CSR_BT_LE_BROWSER_CHARAC_MENU_STATE                 0x02
#define CSR_BT_LE_BROWSER_CHARAC_DESCR_MENU_STATE           0x03
#define CSR_BT_LE_BROWSER_VALUE_MENU_STATE                  0x04
#define CSR_BT_LE_BROWSER_PROBERTIES_MENU_STATE             0x05
#define CSR_BT_LE_BROWSER_CHARAC_VALUE_MENU_STATE           0x06
#define CSR_BT_LE_BROWSER_EDIT_MENU_STATE                   0x07

#define CSR_BT_LE_BROWSER_RAW_WRITE                         0x1000
#define CSR_BT_LE_BROWSER_FORMATTED_WRITE                   0x2000



#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_LE_BROWSER_APP_UI_SEF_H__ */

