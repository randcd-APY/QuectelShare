#ifndef CSR_BT_GATT_APP_TASK_H__
#define CSR_BT_GATT_APP_TASK_H__

/****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/


#include "csr_types.h"
#include "csr_bt_app_prim.h"
#include "csr_bt_gatt_prim.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_PROXS_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_PROXC_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_PROXC_APP_EXTRA_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM


#define CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_GENERIC_SERVER_APP_EXTRA_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_LE_BROWSER_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_LE_BROWSER_APP_EXTRA_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_THERMC_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_THERMC_APP_EXTRA_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_THERMS_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_HIDC_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_RSCS_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_RSCC_APP_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM
#define CSR_BT_RSCC_APP_EXTRA_IFACEQUEUE_PRIM CSR_BT_GATT_APP_PRIM

/* Queue definition */
extern CsrUint16 CSR_BT_PROXC_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_PROXC_APP_EXTRA_IFACEQUEUE;
extern CsrUint16 CSR_BT_PROXS_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_GENERIC_SERVER_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_GENERIC_SERVER_APP_EXTRA_IFACEQUEUE;
extern CsrUint16 CSR_BT_LE_BROWSER_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_LE_BROWSER_APP_EXTRA_IFACEQUEUE;
extern CsrUint16 CSR_BT_RSCC_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_RSCC_APP_EXTRA_IFACEQUEUE;
extern CsrUint16 CSR_BT_RSCS_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_THERMC_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_THERMC_APP_EXTRA_IFACEQUEUE;
extern CsrUint16 CSR_BT_THERMS_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_HOGH_APP_IFACEQUEUE;
extern CsrUint16 CSR_BT_HOGD_APP_IFACEQUEUE;

/* Task definition for Proximity Client */
void CsrBtProxcAppInit(void ** gash);
void CsrBtProxcAppDeinit(void ** gash);
void CsrBtProxcAppHandler(void ** gash);
/* Task definition for Proximity Server */
void CsrBtProxsAppInit(void ** gash);
void CsrBtProxsAppDeinit(void ** gash);
void CsrBtProxsAppHandler(void ** gash);
/* Task definition for Generic Server */
void CsrBtGenericServerAppInit(void ** gash);
void CsrBtGenericServerAppDeinit(void ** gash);
void CsrBtGenericServerAppHandler(void ** gash);

/* Task definition for LE Browser */
void CsrBtLeBrowserAppInit(void ** gash);
void CsrBtLeBrowserAppDeinit(void ** gash);
void CsrBtLeBrowserAppHandler(void ** gash);
/* Task definition for Thermometer Client */
void CsrBtThermcAppInit(void ** gash);
void CsrBtThermcAppDeinit(void ** gash);
void CsrBtThermcAppHandler(void ** gash);
/* Task definition for Thermometer server */
void CsrBtThermsAppInit(void ** gash);
void CsrBtThermsAppDeinit(void ** gash);
void CsrBtThermsAppHandler(void ** gash);

/* Task definition for Hid Host application */
void CsrBtHoghAppInit(void **gash);
void CsrBtHoghAppDeinit(void **gash);
void CsrBtHoghAppHandler(void **gash);

/* Task definition for HID device application */
void CsrBtHogdAppInit(void **gash);
void CsrBtHogdAppDeinit(void **gash);
void CsrBtHogdAppHandler(void **gash);

/* Task definition for Running Speed and Cadance Client */
void CsrBtRsccAppInit(void ** gash);
void CsrBtRsccAppDeinit(void ** gash);
void CsrBtRsccAppHandler(void ** gash);
/* Task definition for Running Speed and Cadance Server */
void CsrBtRscsAppInit(void ** gash);
void CsrBtRscsAppDeinit(void ** gash);
void CsrBtRscsAppHandler(void ** gash);


#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GATT_APP_TASK_H__ */
