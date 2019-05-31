#ifndef CSR_BT_HOGH_APP_H__
#define CSR_BT_HOGH_APP_H__

/****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_list.h"
#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_app_lib.h"
#include "csr_message_queue.h"
#include "csr_file.h"
#include "csr_util.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_platform.h"
#include "csr_bt_gatt_app_task.h"
#include "csr_log_text_2.h"

#include "csr_bt_hogh_prim.h"
#include "csr_bt_hogh_lib.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CSR_BT_HOGH_APP_CSR_UI_PRI          1

#define CSR_BT_HOGH_APP_PREFERRED_MTU       0

#define CSR_BT_HOGH_APP_MAX_REPORT_LEN      10      /* Number of digits in 32-bit value */
#define ADDRESS_STRING_LEN                  20

#define CSR_BT_HOGH_SCAN_INTERVAL_FAST              (CsrUint16)(22.5 / 0.625)
#define CSR_BT_HOGH_SCAN_WINDOW_FAST                (CsrUint16)(11.25 / 0.625)

#define CSR_BT_HOGH_SCAN_INTERVAL_BONDED            (CsrUint16)(40 / 0.625)
#define CSR_BT_HOGH_SCAN_WINDOW_BONDED              (CsrUint16)(30 / 0.625)

#define CSR_BT_HOGH_CONN_INTERVAL_MIN_FAST          (CsrUint16)(7.5 / 1.25)
#define CSR_BT_HOGH_CONN_INTERVAL_MAX_FAST          (CsrUint16)(20 / 1.25)

#define CSR_BT_HOGH_CONN_LATENCY_FAST               (CsrUint16)(1)

/* Key defines */
#define CSR_BT_HOGH_APP_CONN_KEY(_connId)           (_connId & 0x0F)

#define CSR_BT_HOGH_APP_SERVICE_KEY(_connId, _serviceId)                        \
    (((_serviceId & 0x0F) << 4) | CSR_BT_HOGH_APP_CONN_KEY(_connId))

#define CSR_BT_HOGH_APP_REPORT_KEY(_connId, _serviceId, _reportId, _reportType) \
    (((_reportType & 0x0F) << 12) |                                             \
     ((_reportId & 0x0F) << 8) |                                                \
     CSR_BT_HOGH_APP_SERVICE_KEY(_connId, _serviceId))

#define CSR_BT_HOGH_APP_ADD_DEVICE_KEY              CSR_BT_HOGH_APP_CONN_KEY(CSR_BT_HOGH_CONN_ID_INVALID)

#define CSR_BT_HOGH_APP_CONN_ID_FROM_KEY(_key)      ((_key >> 0) & 0x0F)
#define CSR_BT_HOGH_APP_SERV_ID_FROM_KEY(_key)      ((_key >> 4) & 0x0F)
#define CSR_BT_HOGH_APP_REPORT_ID_FROM_KEY(_key)    ((_key >> 8) & 0x0F)
#define CSR_BT_HOGH_APP_REPORT_TYPE_FROM_KEY(_key)  ((_key >> 12) & 0x0F)

typedef struct CsrBtHoghAppService CsrBtHoghAppService;

typedef struct
{
    CsrCmnListElm_t cmnListInfo;

    CsrBtHoghReportType reportType;
    CsrUint8 reportId;

    CsrUint16 reportLen;
    CsrUint8 *report;

    CsrBtHoghAppService *service;
} CsrBtHoghAppReportInfo;

typedef struct CsrBtHoghAppConnection CsrBtHoghAppConnection;

struct CsrBtHoghAppService
{
    CsrCmnListElm_t cmnListInfo;

    CsrUint8 serviceId;
    CsrBtHoghReportTypeFlag supportedReports;

    CsrUint16 reportMapLength;
    CsrUint8 *reportMap;
    CsrBtHidsInformation hidInformation;

    CsrCmnList_t reports; /* List of CsrBtHoghAppReportInfo */

    CsrBtHidsProtocol protocol;

    CsrBtHoghAppConnection *conn;
};

typedef struct CsrBtHoghAppInstData CsrBtHoghAppInstData;

struct CsrBtHoghAppConnection
{
    CsrCmnListElm_t cmnListInfo;

    CsrBtHoghConnId connId;
    CsrBtTypedAddr addr;
    CsrUtf8String *deviceName;
    CsrBtDisPnpId pnpId;

    CsrBtHoghStatus status;
    CsrCmnList_t services; /* List of CsrBtHoghAppHoghService */

    CsrUieHandle hMenuConn;

    CsrBtHoghAppInstData *appInst;
};

/* HOGH application instance data */
struct CsrBtHoghAppInstData
{
    void *recvMsgP;

    CsrBtTypedAddr addr;

    CsrCmnList_t connections; /* List of CsrBtHoghAppConnection */

    CsrBtHidsControlPoint suspendState;

    CsrBtHoghAppConnection *activeConnection;
    CsrBtHoghAppReportInfo *activeReport;

    CsrUieHandle hMenuMain;
    CsrUieHandle hDialog;
    CsrUieHandle hInputDialogAddress;
    CsrUieHandle hInputDialogReport;

    CsrUieHandle hEventBack;
    CsrUieHandle hEventSk1;
    CsrUieHandle hEventSk2;
    CsrUieHandle hEventDel;

#ifdef CSR_LOG_ENABLE
    CsrLogTextHandle *logHandle;
#endif
};

#define HOGH_APP_ADD_CONN(_appInst)                                                     \
    ((CsrBtHoghAppConnection *) CsrCmnListElementAddLast(&((_appInst)->connections),    \
                                                         sizeof(CsrBtHoghAppConnection)))

#define HOGH_APP_FIND_CONN(_appInst, _hoghConnId)                                               \
    ((CsrBtHoghAppConnection *) CsrCmnListSearchOffsetUint8(&((_appInst)->connections),         \
                                                            CsrOffsetOf(CsrBtHoghAppConnection, \
                                                                        connId),                \
                                                            (_hoghConnId)))

#define HOGH_APP_FIND_CONN_BY_DISPLAY(_appInst, _displayElement)                                \
    ((CsrBtHoghAppConnection *) CsrCmnListSearchOffsetUint16(&((appInst)->connections),         \
                                                             CsrOffsetOf(CsrBtHoghAppConnection,\
                                                                         hMenuConn),            \
                                                             (_displayElement)))

#define HOGH_APP_ADD_SERVICE(_conn)                                                     \
    ((CsrBtHoghAppService *) CsrCmnListElementAddLast(&((_conn)->services),             \
                                                        sizeof(CsrBtHoghAppService)))

#define HOGH_APP_FIND_SERVICE(_conn, _serviceId)                                                \
    ((CsrBtHoghAppService *) CsrCmnListSearchOffsetUint8(&((_conn)->services),                  \
                                                         CsrOffsetOf(CsrBtHoghAppService,       \
                                                                     serviceId),                \
                                                         (_serviceId)))

#define HOGH_APP_ADD_REPORT(_service)                                                   \
    ((CsrBtHoghAppReportInfo *) CsrCmnListElementAddLast(&((_service)->reports),        \
                                                         sizeof(CsrBtHoghAppReportInfo)))

#define HOGH_APP_GET_FIRST_REPORT(_service)                                             \
    ((CsrBtHoghAppReportInfo *) CsrCmnListElementGetFirst(&((_service)->reports)))

CsrUtf16String *CsrBtHoghAppAddrToString(BD_ADDR_T *addr);

void CsrBtHoghAppUiPrimHandler(CsrBtHoghAppInstData *appInst);
void CsrBtHoghAppUiRefreshConnMenu(CsrBtHoghAppConnection *conn);
void CsrBtHoghAppUiRefreshMainMenu(CsrBtHoghAppInstData *appInst);
void CsrBtHoghAppUiPopup(CsrBtHoghAppInstData *appInst,
                         const CsrCharString *dialogHeading,
                         const CsrCharString *dialogText);

void CsrBtHoghAppPrimHandler(CsrBtHoghAppInstData *appInst);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_HOGH_APP_H__ */

