#ifndef CSR_BT_GAP_APP_HANDLER_H__
#define CSR_BT_GAP_APP_HANDLER_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #1 $
******************************************************************************/


#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_bt_ui_strings.h"
#include "csr_message_queue.h"
#include "csr_bt_sc_demo_app.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_GAP_STD_PRIO                             0x01


/* app csrUi UI index number                            */
#define CSR_BT_GAP_SD_MENU_UI                           0x00
#define CSR_BT_GAP_SD_SEARCH_DEVICES_UI                 0x01
#define CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI               0x02
#define CSR_BT_GAP_SD_TRUSTED_DEVICES_UI                0x03
#define CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI        0x04
#define HIGHEST_NUM_OF_SD_UI                            0x05

#define CSR_BT_GAP_SC_MENU_UI                           0x00 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_PASSKEY_UI                        0x01 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_DEBOND_UI                         0x02 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_MODE_UI                           0x03 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_DISP_CAPB_UI                      0x04 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_SSP_PASSKEY_UI                    0x05 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_SSP_NOTIFICATION_UI               0x06 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_SSP_JUST_WORKS_UI                 0x07 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_SSP_COMPARE_UI                    0x08 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_AUTHORISE_UI                      0x09 + HIGHEST_NUM_OF_SD_UI
#define CSR_BT_GAP_SC_AUTH_REQ_MENU_UI                  0x0A + HIGHEST_NUM_OF_SD_UI
#define HIGHEST_NUM_OF_SC_UI                            0x0B + HIGHEST_NUM_OF_SD_UI

#define CSR_BT_GAP_CM_MENU_UI                           0x00 + HIGHEST_NUM_OF_SC_UI
#define CSR_BT_GAP_CM_SET_LOCAL_NAME_UI                 0x01 + HIGHEST_NUM_OF_SC_UI
#define CSR_BT_GAP_CM_CUSTOM_COD_UI                     0x02 + HIGHEST_NUM_OF_SC_UI
#define CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI      0x03 + HIGHEST_NUM_OF_SC_UI
#define CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI      0x04 + HIGHEST_NUM_OF_SC_UI
#define HIGHEST_NUM_OF_CM_UI                            0x05 + HIGHEST_NUM_OF_SC_UI

#define CSR_BT_GAP_GATT_MENU_UI                         0x00 + HIGHEST_NUM_OF_CM_UI
#define CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI             0x01 + HIGHEST_NUM_OF_CM_UI
#define HIGHEST_NUM_OF_GATT_UI                          0x02 + HIGHEST_NUM_OF_CM_UI

#define CSR_BT_GAP_MAIN_MENU_UI                         0x00 + HIGHEST_NUM_OF_GATT_UI
#define CSR_BT_GAP_DEFAULT_INFO_UI                      0x01 + HIGHEST_NUM_OF_GATT_UI
#define NUMBER_OF_CSR_BT_GAP_APP_UI                     0x02 + HIGHEST_NUM_OF_GATT_UI


typedef void (* CsrUiEventHandlerFuncType)(void * instData, CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key);

typedef struct
{
    CsrUieHandle                    displayHandle;
    CsrUieHandle                    sk1EventHandle;
    CsrUieHandle                    sk2EventHandle;
    CsrUieHandle                    backEventHandle;
    CsrUieHandle                    deleteEventHandle;
    CsrUiEventHandlerFuncType       sk1EventHandleFunc;
    CsrUiEventHandlerFuncType       sk2EventHandleFunc;
    CsrUiEventHandlerFuncType       deleteEventHandleFunc;
    CsrUiEventHandlerFuncType       backEventHandleFunc;
} CsrBtGapDisplayHandlesType;


typedef struct
{
    CsrUint8                        uiIndex;
    CsrUint8                        eventState;
    CsrSchedTid                     popupTimerId;
    CsrBtGapDisplayHandlesType      displayesHandlers[NUMBER_OF_CSR_BT_GAP_APP_UI];
} CsrBtGapCsrUiType;

typedef struct
{
    CsrBtDeviceName             name;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtAddressType            deviceAddrType;
    CsrUint32                   SspNtfnNumericValue;  /* numeric value given for SSP notification */
    CsrBool                     proximityPairing;
    CsrUint8                    authRequirements;
} CsrBtGapScInstData;

typedef struct
{
    CsrUint8                    lmpVersion;
    CsrUint8                    hciVersion;
    CsrUint16                   hciRevision;
    CsrUint16                   manufacturerName;
    CsrUint16                   lmpSubversion;
    CsrBtDeviceName             localName;
    CsrBtDeviceAddr             localDeviceAddr;
    CsrBtAddressType            localDeviceAddrType;
    CsrBtOwnAddressType         ownAddressType;
} CsrBtGapCmInstData;


typedef struct profileUieTag
{
    CsrSchedQid                 pHandle;
    CsrUint16                   key;
    struct profileUieTag *      next;
} profileUie;

typedef struct
{
    CsrUint16                   csrUiKey;
    CsrBtDeviceName             deviceName;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtAddressType            deviceAddrType;
    CsrUint32                   deviceStatus;
    CsrBool                     authorised;
} CsrBtGapDeviceListType;

#define DEVICE_LIST_POOL_SIZE 20

typedef struct CsrBtGapDeviceKeyListStructTag
{
    CsrUint16                                   numberOfMessagesInThisPool;
    struct CsrBtGapDeviceKeyListStructTag*      nextQueuePool;
    CsrBtGapDeviceListType                      deviceListEntryArray[DEVICE_LIST_POOL_SIZE];
} CsrBtGapDeviceKeyListType;


typedef struct
{
    CsrUint8                        state;
    CsrSchedQid                          phandle;
    CsrBtClassOfDevice              codFilter;
    CsrUint8                        searchService;
    CsrBool                         availableServices;
    CsrBool                         updateHsList;
    CsrUint32                       searchConfig;
} CsrBtGapSdInstData;

typedef struct
{
    CsrBool                         initialized;
    CsrBtGapCsrUiType               csrUiVar;
    CsrBtGapScInstData              scVar;
    CsrBtGapSdInstData              sdVar;
    CsrBtGapCmInstData              cmVar;
    CsrBtGapDeviceKeyListType       * searchList;
    CsrUint16                       devicesInSearchList;
    CsrBtDeviceName                 selectedDeviceName;
    CsrBtDeviceAddr                 selectedDeviceAddr;
    CsrBtAddressType                selectedDeviceAddrType;
    CsrUint32                       selectedDeviceStatus;
    CsrUint16                       mainMenuEntries;
    profileUie                      * profileHandleList;

    CsrMessageQueueType            * saveQueue;
    CsrBool                         restoreFlag;

    CsrUint16                       backLogEntries;
    CsrUint16                       maxBackLogEntries;
    void *recvMsgP;
    CsrSchedQid                     proximityPairingHandle;
    CsrBool                         performBonding;
} CsrBtGapInstData;

typedef struct
{
    CsrCharString                  *className;
    CsrUint32                       majorDeviceMask;
    CsrUint32                       minorDeviceMask;
} CsrBtGapClassOfDeviceData;

typedef void (* CsrBtGapEventType)(CsrBtGapInstData * instData);

typedef void (* CsrBtGapUpdateServiceType)(CsrUieHandle displayHandle, CsrUieHandle sk1EventHandle,
                                        CsrUieHandle backEventHandle, CsrBool availableServices);

#ifdef __cplusplus
}
#endif



#endif /* endif CSR_BT_GAP_APP_HANDLER_H__ */
