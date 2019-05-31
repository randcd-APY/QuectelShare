#ifndef CSR_BT_HOGD_APP_H_
#define CSR_BT_HOGD_APP_H_

/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_list.h"
#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_message_queue.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_gatt_demo_db_utils.h"
#include "csr_bt_gatt_app_task.h"
#include "csr_bt_hids_common.h"
#include "csr_bt_dis_common.h"
#include "csr_bt_scps_common.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define PROFILE_NAME(name)                          CsrUtf8StrDup((CsrUtf8String *) name)
#define ADDRESS_STRING_LEN                          32
#define CSR_BT_HOGD_APP_ADVERT_DATA_LEN             4

#define CSR_BT_HOGD_APP_ADVERTISING_INTERVAL_MIN    0x0030  /* (30 milliseconds) / 0.625 */
#define CSR_BT_HOGD_APP_ADVERTISING_INTERVAL_MAX    0x0050  /* (50 milliseconds) / 0.625 */

#define CSR_BT_HOGD_APP_CSR_UI_PRI                  1

#define CSR_BT_HOGD_APP_PREFFERED_MTU               0

#define CSR_BT_HOGD_APP_RSSI_TIMER_INTERVAL         1000000
#define MOUSE_LEFT_BUTTON                           1
#define MOUSE_RIGHT_BUTTON                          2
#define MOUSE_MIDDLE_BUTTON                         4
#define MOUSE_BACK_BUTTON                           8
#define MOUSE_FORWARD_BUTTON                        16

/* Report structure for Mouse in boot protocol mode */
typedef struct
{
    CsrUint8 buttons;
    CsrInt8 positionX;
    CsrInt8 positionY;
} CsrBtHogdAppMouseBootReport;

/* Report reference characteristic descriptor */
typedef struct
{
    CsrUint8 reportId;
    CsrUint8 reportType;
} CsrBtHogdAppDescReportReference;

typedef struct
{
    CsrUint8 buttons;
    CsrInt8 positionX;
    CsrInt8 positionY;
    CsrInt8 wheelVertical;
    CsrInt8 wheelHorizontal;
    CsrUint8 resolutionVertical;
    CsrUint8 resolutionHorizontal;
} CsrBtHogdAppMouseData;

typedef struct
{
    CsrUint16 scanInterval;
    CsrUint16 scanWindow;
} CsrBtHogdAppClientScanParameters;

typedef struct
{
    CsrBtHogdAppMouseData mouseData;
    CsrBtHogdAppClientScanParameters scanParameters;
    CsrUint8 batteryPercentage;
    CsrUint8 protocol;
} CsrBtHogdAppProfileData;

typedef struct
{
    /*database values and handles */
    CsrBtGattDb *dbPrimServices;
    CsrBtGattHandle hStart;
    CsrBtGattHandle hEnd;

    CsrBtGattHandle hScanParameter;
} CsrBtHogdAppDatabase;

typedef struct CsrBtHogdAppConnection
{
    CsrCmnListElm_t list;
    CsrBtTypedAddr connectedDeviceAddr;
    CsrBool whitelist;
    CsrBool mouseInputReportNotify;
    CsrBool mouseBootInputReportNotify;
    CsrBool batteryLevelNotify;
    CsrBool scanRefreshNotify;
} CsrBtHogdAppConnection;

typedef struct
{
    CsrUieHandle hMenuMain;
    CsrUieHandle hEventMainSk1;
    CsrUieHandle hEventMainActivateWhitelist;

    CsrUieHandle hEventBack;

    CsrUieHandle hMenuMouse;
    CsrUieHandle hEventButtonClick;
    CsrUieHandle hEventButtonPressRelease;

    CsrUieHandle hDialogInputPositionX;
    CsrUieHandle hEventInputPositionX;

    CsrUieHandle hDialogInputPositionY;
    CsrUieHandle hEventInputPositionY;

    CsrUieHandle hDialogInputScroll;
    CsrUieHandle hEventInputScroll;

    CsrUieHandle hDialogInputBattery;
    CsrUieHandle hEventInputBattery;

    CsrUieHandle hDialogCommon;
} CsrBtHogdAppUiInstData;

/* CSR_BT_HOGD instance data */
typedef struct
{
    void *recvMsgP;
    CsrBtGattId gattId; /* gattId that this demo app identifies with */
    CsrBtConnId btConnId;
    CsrBtHogdAppConnection *activeConnection;
    CsrCmnList_t *connections;
    CsrBtHogdAppProfileData profileData;
    CsrBtHogdAppUiInstData csrUiVar;
    CsrBtHogdAppDatabase db;
    CsrBool whiteListConnect;
    CsrBool lowPower; /* Corresponds to HID control point characteristic in HID service. TRUE=Suspend; FALSE=Exit Suspend */
} CsrBtHogdAppInstData;

#define CSR_BT_HOGD_APP_FIND_CONNECTION_BY_ADDR(_list, _pAddr) CsrCmnListSearch(_list, CsrBtHogdAppSearchConnByAddr, _pAddr)

#define CSR_BT_HOGD_APP_ADD_CONNECTION(_list)   CsrCmnListElementAddFirst(_list, sizeof(CsrBtHogdAppConnection))

#define BUTTON_PRESSED(_pMouse, _button)            (_pMouse->buttons & _button)
#define LEFT_BUTTON_PRESSED(_pMouse)                (BUTTON_PRESSED(_pMouse, MOUSE_LEFT_BUTTON))
#define RIGHT_BUTTON_PRESSED(_pMouse)               (BUTTON_PRESSED(_pMouse, MOUSE_RIGHT_BUTTON))
#define MIDDLE_BUTTON_PRESSED(_pMouse)              (BUTTON_PRESSED(_pMouse, MOUSE_MIDDLE_BUTTON))
#define BACK_BUTTON_PRESSED(_pMouse)                (BUTTON_PRESSED(_pMouse, MOUSE_BACK_BUTTON))
#define FORWARD_BUTTON_PRESSED(_pMouse)             (BUTTON_PRESSED(_pMouse, MOUSE_FORWARD_BUTTON))


#define PRESS_BUTTON(_pMouse, _button)           (_pMouse->buttons |= _button)
#define RELEASE_BUTTON(_pMouse, _button)         (_pMouse->buttons &= ~_button)

#define PRESS_LEFT_BUTTON(_pMouse)               PRESS_BUTTON(_pMouse, MOUSE_LEFT_BUTTON)
#define PRESS_MIDDLE_BUTTON(_pMouse)             PRESS_BUTTON(_pMouse, MOUSE_MIDDLE_BUTTON)
#define PRESS_RIGHT_BUTTON(_pMouse)              PRESS_BUTTON(_pMouse, MOUSE_RIGHT_BUTTON)
#define PRESS_BACK_BUTTON(_pMouse)               PRESS_BUTTON(_pMouse, MOUSE_BACK_BUTTON)
#define PRESS_FORWARD_BUTTON(_pMouse)            PRESS_BUTTON(_pMouse, MOUSE_FORWARD_BUTTON)

#define RELEASE_LEFT_BUTTON(_pMouse)             RELEASE_BUTTON(_pMouse, MOUSE_LEFT_BUTTON)
#define RELEASE_MIDDLE_BUTTON(_pMouse)           RELEASE_BUTTON(_pMouse, MOUSE_MIDDLE_BUTTON)
#define RELEASE_RIGHT_BUTTON(_pMouse)            RELEASE_BUTTON(_pMouse, MOUSE_RIGHT_BUTTON)
#define RELEASE_BACK_BUTTON(_pMouse)             RELEASE_BUTTON(_pMouse, MOUSE_BACK_BUTTON)
#define RELEASE_FORWARD_BUTTON(_pMouse)          RELEASE_BUTTON(_pMouse, MOUSE_FORWARD_BUTTON)

#define GET_FEATURE_REPORT(_pMouse)              (_pMouse->resolutionVertical + (_pMouse->resolutionHorizontal << 2))
#define FEATURE_REPORT_HORIZONTAL_RESOLUTION(_report)   ((_report >> 2) & 0x03)
#define FEATURE_REPORT_VERTICAL_RESOLUTION(_report)     (_report & 0x03)

void CsrBtHogdAppResetConnection(CsrCmnListElm_t *elem);
void CsrBtHogdAppActivate(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppDeactivate(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppDisconnect(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppStartPeripheralRole(CsrBtHogdAppInstData *inst);
CsrBool CsrBtHogdAppSearchConnByAddr(CsrCmnListElm_t *elem, void *value);
CsrBtHogdAppMouseBootReport *CsrBtHogdAppGetMouseBootInputReport(CsrBtHogdAppMouseData *mouseData);
void CsrBtHogdAppRequestScanParams(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppUpdateMouseData(CsrBtHogdAppInstData *inst, CsrBool boot);
void CsrBtHogdAppUpdateBatteryData(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppHandleUiPrim(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppGattPrimHandler(CsrBtHogdAppInstData *inst);

void CsrBtHogdAppUiSetDeactivated(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppUiSetActivated(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppUiSetConnected(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppUiUpdateScanParams(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppUiUpdateWhitelist(CsrBtHogdAppInstData *inst);
void CsrBtHogdAppUiPopup(CsrBtHogdAppInstData *inst,
                         char *dialogHeading,
                         char *dialogText);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_HOGD_APP_H_ */
