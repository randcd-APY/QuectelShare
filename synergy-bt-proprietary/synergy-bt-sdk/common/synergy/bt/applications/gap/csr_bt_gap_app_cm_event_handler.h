#ifndef CSR_BT_GAP_APP_CM_EVENT_HANDLER_H__
#define CSR_BT_GAP_APP_CM_EVENT_HANDLER_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/


#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_cm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definiton of the struct type for the CM application event functions  */
struct CsrBtCmAppEventHandlerStructType
{
    /* csrBtCmSetLocalNameCfmFunc                */
    void (*csrBtCmSetLocalNameCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode);                    /* the result of the set local name operation                                   */

    /* csrBtCmReadLocalDeviceAddressCfmFunc      */
    void (*csrBtCmReadLocalDeviceAddressCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtDeviceAddr            deviceAddr);                /* the local Bluetooth device address                                           */

    /* csrBtCmWriteLinkSupervTimeoutCfmFunc      */
    void (*csrBtCmWriteLinkSupervTimeoutCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode,                     /* the result of the write link super vision timeout operation                  */
            CsrBtDeviceAddr            deviceAddr);                /* the Bluetooth device address of which the link supervision timeout is change */

    /* csrBtCmReadLocalNameCfmFunc               */
    void (*csrBtCmReadLocalNameCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrUtf8String           *localName);                 /* the friendly name of the local Bluetooth device in utf8                      */

    /* csrBtCmReadTxPowerLevelCfmFunc            */
    void (*csrBtCmReadTxPowerLevelCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtDeviceAddr            deviceAddr,                 /* the Bluetooth device address from which the tx power level is read           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode,                     /* the result of the read tx power level operation                              */
            CsrInt8                  powerLevel);                /* the tx power value                                                           */

    /* csrBtCmGetLinkQualityCfmFunc              */
    void (*csrBtCmGetLinkQualityCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtDeviceAddr            deviceAddr,                 /* the Bluetooth device address from which the link quality is read             */
            CsrBtSupplier supplier, CsrBtResultCode resultCode,                     /* the result of the get link quality operation                                 */
            CsrUint8                 linkQuality);               /* the link quality value                                                       */

    /* csrBtCmReadRssiCfmFunc                    */
    void (*csrBtCmReadRssiCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtDeviceAddr          deviceAddr,                 /* the Bluetooth device address from which the rssi is read                     */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode,                     /* the result of the read rssi operation                                        */
            CsrUint8                 rssi);                      /* the rssi value                                                               */

    /* csrBtCmWriteCodCfmFunc                    */
    void (*csrBtCmWriteCodCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode);                    /* the result of the write class of device operation                            */

    /* csrBtCmReadCodCfmFunc                     */
    void (*csrBtCmReadCodCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode,                     /* the result of the read class of device operation                             */
            CsrUint24                classOfDevice);             /* the class of device value                                                    */

    /* csrBtCmReadLocalVersionCfmFunc            */
    void (*csrBtCmReadLocalVersionCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrUint8                 lmpVersion,  /* the lmp version of the local device                                          */
            CsrUint8                 hciVersion,
            CsrUint16                hciRevision,
            CsrUint16                manufacturerName,
            CsrUint16                lmpSubversion);

    /* csrBtCmReadRemoteVersionCfmFunc           */
    void (*csrBtCmReadRemoteVersionCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode,                     /* the result of the read remote lmp version operation                          */
            CsrBtDeviceAddr          deviceAddr,                 /* the Bluetooth device address from which the lmp version must be read         */
            CsrUint8                 lmpVersion,                 /* the lmp version of the remote device                                         */
            CsrUint16                manufacturerName,           /* the name of manufacturer                                                     */
            CsrUint16                lmpSubversion);             /* the subversion of LMP                                                        */

    /* csrBtCmReadScanEnableCfmFunc              */
    void (*csrBtCmReadScanEnableCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode,                     /* the result of the read scan enable operation                                 */
            CsrUint8                 scanEnable);                /* the value of the scan enable parameter                                       */

    /* csrBtCmWriteScanEnableCfmFunc              */
    void (*csrBtCmWriteScanEnableCfmFunc)
         (  void                    *instData,                  /* the applications own instance data                                           */
            CsrBtSupplier supplier, CsrBtResultCode resultCode);                    /* the result of the write scan enable operation                                */

    /* csrBtCmSetOwnAddressTypeCfmFunc        */
    void (*csrBtCmSetOwnAddressTypeCfmFunc)
        (   void           *instData,    /* the applications own instance data              */
            CsrBtSupplier   supplier,    /* the result supplier of the own address type cfm */
            CsrBtResultCode resultCode); /* the result of the own address type cfm          */

};

typedef struct CsrBtCmAppEventHandlerStructType CsrBtCmAppEventHandlerStructType;


extern void CsrBtCmAppEventHandler(void * instData, void * message, CsrBtCmAppEventHandlerStructType *csrBtCmAppEventHandler);


#ifdef __cplusplus
}
#endif

#endif /* endif CSR_BT_GAP_APP_CM_EVENT_HANDLER_H__ */
