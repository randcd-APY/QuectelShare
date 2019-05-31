#ifndef CSR_BT_GAP_APP_SD_EVENT_HANDLER_H__
#define CSR_BT_GAP_APP_SD_EVENT_HANDLER_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #4 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_sd_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definiton of the struct type for the SD application event functions  */
struct CsrBtSdAppEventHandlerStructType
{
    /* csrBtSdReadDeviceInfoCfmFunc        */
    void (*csrBtSdReadDeviceInfoCfmFunc)
        (   void                        *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier,     CsrBtResultCode resultCode, /* the result of the read device info operation                                 */
            CsrBtDeviceAddr             deviceAddr,                 /* the bluetooth address of the device                                          */
            CsrBtClassOfDevice          deviceClass,                /* the class of device of the selected device                                   */
            CsrUint32                   infoLen,                    /* the length of the data present in the info pointer                           */
            CsrUint8                    *info,                      /* additional information about the selected device                             */
            CsrUint32                   deviceStatus);              /* the status of the device (non-paired/paired/which pairing)                   */

    /* csrBtSdSearchResultIndFunc                */
    void (*csrBtSdSearchResultIndFunc)
        (   void                     *instData,                  /* the applications own instance data                                           */
            CsrBtDeviceAddr          deviceAddr,                 /* the bluetooth address of the device                                          */
            CsrBtAddressType         addressType,                /* the address type of the bluestooth address                                   */
            CsrBtClassOfDevice       deviceClass,                /* the class of device of the selected device                                   */
            CsrUint8                 rssi,                       /* the rssi level of the device                                                 */
            CsrUint32                infoLen,                    /* the length of the data present in the info pointer                           */
            CsrUint8                 *info,                      /* additional information about the selected device                             */
            CsrUint32                deviceStatus);              /* the status of the device (non-paired/paired/which pairing)                   */

    /* csrBtSdCloseSearchIndFunc                 */
    void (*csrBtSdCloseSearchIndFunc)
        (   void                    *instData,                      /* the applications own instance data                                           */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode);   /* reports the result of close search                                           */

    /* csrBtSdActivateSearchAgentCfmFunc         */
    void (*csrBtSdActivateSearchAgentCfmFunc)
        (   void                    *instData,                      /* the applications own instance data                                           */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode);   /* reports whether the search agent was activated or not                        */

    /* csrBtSdDeactivateSearchAgentCfmFunc       */
    void (*csrBtSdDeactivateSearchAgentCfmFunc)
        (   void                    *instData,                      /* the applications own instance data                                           */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode);   /* reports the result of deactivating the search agent                          */

    /* csrBtSdReadAvailableServicesCfmFunc       */
    void (*csrBtSdReadAvailableServicesCfmFunc)
        (   void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode, /* the result of the read device info operation                                 */
            CsrBtDeviceAddr         deviceAddr,                 /* the bluetooth address of the device                                          */
            CsrBtClassOfDevice      deviceClass,                /* the class of device of the selected device                                   */
            CsrUint32               infoLen,                    /* the length of the data present in the info pointer                           */
            CsrUint8                *info,                      /* additional information about the selected device                             */
            CsrUint32               deviceStatus);              /* the status of the device (non-paired/paired/which pairing)                   */

    /* csrBtSdCancelReadAvailableServicesCfmFunc */
    void (*csrBtSdCancelReadAvailableServicesCfmFunc)
        (   void                    *instData);                 /* the applications own instance data                                           */

    /* csrBtSdReadDeviceListIndFunc          */
    void (*csrBtSdReadDeviceListIndFunc)
        (   void                     *instData,                  /* the applications own instance data                                           */
            CsrUint32                deviceInfoCount,            /* the number of devices contained in the allocated pointer, *deviceProperties  */
            CsrBtSdDeviceInfoType    *deviceInfo);               /* a list containing all the information related to each device                 */

    /* csrBtSdReadDeviceListCfmFunc          */
    void (*csrBtSdReadDeviceListCfmFunc)
        (   void                     *instData,                  /* the applications own instance data                                           */
            CsrUint32                totalNumOfDevices,          /* the total number of devices present in the SD deviceInfo list                */
            CsrUint32                deviceInfoCount,            /* the number of devices contained in the allocated pointer, *deviceProperties  */
            CsrBtSdDeviceInfoType    *deviceInfo);               /* a list containing all the information related to each device                 */
};


typedef struct CsrBtSdAppEventHandlerStructType CsrBtSdAppEventHandlerStructType;


extern void CsrBtSdAppEventHandler(void * instData, void * message, const CsrBtSdAppEventHandlerStructType *csrBtSdAppEventHandler);


#ifdef __cplusplus
}
#endif

#endif /* endif CSR_BT_GAP_APP_SD_EVENT_HANDLER_H__ */

