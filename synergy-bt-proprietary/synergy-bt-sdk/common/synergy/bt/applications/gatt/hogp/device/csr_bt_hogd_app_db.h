#ifndef CSR_BT_HOGD_APP_DB_H_
#define CSR_BT_HOGD_APP_DB_H_

/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_hogd_app.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Device information service values */
#define MANUFACTURER_NAME                   ((const CsrUint8*) "CSR")
#define MODEL_NUMBER                        ((const CsrUint8*) "1.0")
#define SERIAL_NUMBER                       ((const CsrUint8*) "1.0")
#define HARDWARE_REVISION_NUMBER            ((const CsrUint8*) "1.0")
#define FIRMWARE_REVISION_NUMBER            ((const CsrUint8*) "1.0")
#define SOFTWARE_REVISION_NUMBER            ((const CsrUint8*) "1.0")

/******************************************************************************
 * Database handle offsets
 * These has been calculated based on database generated in csr_bt_hogd_app_db.c
 ******************************************************************************/
#define HIDS_HANDLE_START                                   0

#define HIDS_PROTOCOL_HANDLE_OFFSET         HIDS_HANDLE_START + 3
#define HIDS_INPUT_REPORT_HANDLE_OFFSET     HIDS_HANDLE_START + 5
#define HIDS_INPUT_REPORT_CLIENT_CONFIG_HANDLE_OFFSET   HIDS_HANDLE_START + 6
#define HIDS_FEATURE_REPORT_HANDLE_OFFSET   HIDS_HANDLE_START + 9
#define HIDS_BOOT_MOUSE_INPUT_HANDLE_OFFSET HIDS_HANDLE_START + 14
#define HIDS_BOOT_MOUSE_INPUT_CLIENT_CONFIG_HANDLE_OFFSET   HIDS_HANDLE_START + 15
#define HIDS_CONTROL_POINT_HANDLE_OFFSET    HIDS_HANDLE_START + 19

#define HIDS_HANDLE_COUNT                   19

#define BAS_HANDLE_START                    (HIDS_HANDLE_START + HIDS_HANDLE_COUNT)

#define BAS_BATTERY_LEVEL_HANDLE_OFFSET     BAS_HANDLE_START + 3
#define BAS_BATTERY_LEVEL_CLIENT_CONFIG_HANDLE_OFFSET   BAS_HANDLE_START + 4

#define BAS_HANDLE_COUNT                    4

#define DI_HANDLE_START                     (BAS_HANDLE_START + BAS_HANDLE_COUNT)

#define DI_HANDLE_COUNT                     13

#define SCP_HANDLE_START                    (DI_HANDLE_START + DI_HANDLE_COUNT)

#define SCP_SCAN_INTERVAL_WINDOW_HANDLE_OFFSET          SCP_HANDLE_START + 3
#define SCP_SCAN_REFRESH_HANDLE_OFFSET                  SCP_HANDLE_START + 5
#define SCP_SCAN_REFRESH_CLIENT_CONFIG_HANDLE_OFFSET    SCP_HANDLE_START + 6

#define SCP_HANDLE_COUNT                    6

#define CSR_BT_HOGD_DB_HANDLE_COUNT         (SCP_HANDLE_COUNT + SCP_HANDLE_START)
/*********************** End of Database handle offsets **********************/


void CsrBtHogdAppCreateDb(CsrBtHogdAppInstData *inst);

#endif /* CSR_BT_HOGD_APP_DB_H_ */
