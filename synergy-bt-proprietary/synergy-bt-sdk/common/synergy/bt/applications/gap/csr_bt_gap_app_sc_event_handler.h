#ifndef CSR_BT_GAP_APP_SC_EVENT_HANDLER_H__
#define CSR_BT_GAP_APP_SC_EVENT_HANDLER_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/
#include "csr_types.h"
#include "csr_bt_sc_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definiton of the struct type for the SC application event functions  */
struct CsrBtScAppEventHandlerStructType
{
    /* csrBtScPasskeyIndFunc             */
    void (*csrBtScPasskeyIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrUint32                cod,                        /* device class of peer device                                              */
            CsrBtDeviceAddr           deviceAddr,                 /* address of device for which a passkey is requested                       */
            CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
            CsrUint8                 paired,                     /* actual pair status of the device                                         */
            CsrBtAddressType         addressType);               /* type of address of device for which a passkey is requested               */

    /* csrBtScBondCfmFunc                */
    void (*csrBtScBondCfmFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrBool                  addedToScDbList,            /* If TRUE the device is added to the Security Manager's device database    */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device to bond with.                                          */
            CsrBtClassOfDevice       cod,                        /* The class of device of the remote device                                 */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode, /* result                                                                   */
            CsrBtAddressType         addressType);               /* type of address of device to bond with.                                  */

    /* csrBtScDeBondCfmFunc              */
    void (*csrBtScDeBondCfmFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device that has been debonded.                                */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode, /* result                                                                   */
            CsrBtAddressType         addressType);               /* type of address of device that has been debonded.                        */

    /* csrBtScSetSecurityModeCfmFunc    */
    void (*csrBtScSetSecurityModeCfmFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode);/* result                                                                   */

    /* csrBtScEncryptionCfmFunc          */
    void (*csrBtScEncryptionCfmFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrUint16                encType,                    /* encryption Type                                                          */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device for which encryption is requested                      */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode);/* result                                                                   */
            
    /* csrBtScAuthoriseIndFunc           */
    void (*csrBtScAuthoriseIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device that must be authorised                                */
            CsrUtf8String            *deviceName,                 /* name of device which must be authorised                                  */
            uuid16_t                 serviceId,                  /* the id of the service for which authorisation is required                */
            CsrBtAddressType         addressType);               /* type of address of device that must be authorised                         */

    /* csrBtScBondIndFunc                */
    void (*csrBtScBondIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrBool                  addedToScDbList,            /* If TRUE the device is added to the Security Manager's device database    */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device to bond with.                                          */
            CsrBtClassOfDevice       cod,                        /* The class of device of the remote device                                 */
            CsrBtSupplier supplier,  CsrBtResultCode resultCode, /* result                                                                   */
            CsrBtAddressType         addressType);               /* type of address of device to bond with.                                  */
            
    /* csrBtScSspPasskeyIndFunc             */
    void (*csrBtScSspPasskeyIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrUint32                cod,                        /* device class of peer device                                              */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
            CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
            CsrUint8                 paired,                     /* actual pair status of the device                                         */
            CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
            CsrBtAddressType         addressType);               /* type of address of device for which a passkey is requested              */


    /* csrBtScSspNotificationIndFunc             */
    void (*csrBtScSspNotificationIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrUint32                cod,                        /* device class of peer device                                              */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
            CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
            CsrUint8                 paired,                     /* actual pair status of the device                                         */
            CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
            CsrUint32                numericalValue,            /* number the MMI shall show on the display to enable the user to input it
                                                                   on a remote device                                                       */
            CsrBtAddressType         addressType);               /* type of address of device for which a passkey is requested              */

    /* csrBtScSspCompareInd             */
    void (*csrBtScSspCompareIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrUint32                cod,                        /* device class of peer device                                              */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
            CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
            CsrUint8                 paired,                     /* actual pair status of the device                                         */
            CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
            CsrUint32                numericalValue,             /* number the MMI shall show on the display to enable the user to compare it
                                                                   with value shown on a remote device                                      */
            CsrBtAddressType         addressType);               /* type of address of device for which a passkey is requested              */

    /* csrBtScSspJustworksInd             */
    void (*csrBtScSspJustworksIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrUint32                cod,                        /* device class of peer device                                              */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
            CsrUtf8String            *deviceName,                /* name of device for which a passkey is requested                          */
            CsrUint8                 paired,                     /* actual pair status of the device                                         */
            CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
            CsrBtAddressType         addressType);               /* type of address of device for which a passkey is requested              */


    /* csrBtScSspKeypressNotificationInd             */
    void (*csrBtScSspKeypressNotificationIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
            CsrUint8                 notificationType,           /* the reason from the remote device for requesting additional time         */
            CsrBtAddressType         addressType);               /* type of address of device for which a passkey is requested              */

    /* csrBtScModeInd             */
    void (*csrBtScModeIndFunc)
        (   void                     *instData,                  /* the application own instance data                                        */
            CsrUint8                 mode);                      /* the actual idle pairing mode behaviour of the local device               */

    /* csrBtScRebondInd           */
    void (*csrBtScRebondIndFunc)
        (   void                    *instData,                   /* the application own instance data                                        */
            CsrUint32                cod,                        /* device class of peer device                                              */
            CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a rebond is requested                        */
            CsrUtf8String            *deviceName,                /* name of device for which a rebond is requested                           */
            CsrBtAddressType         addressType);               /* type of address of device for which a rebond is requested                */

    void (*csrBtScLeSecurityIndFunc)
        (   void                    *instData,                  /* the application own instance data                */
            CsrBtSupplier           supplier, 
            CsrBtResultCode         resultCode,                 /* the result of the outgoing bonding procedure     */
            CsrBtTypedAddr          address,                    /* Address of device to bond with.                  */
            CsrBool                 addedToScDbList);

};


typedef struct CsrBtScAppEventHandlerStructType CsrBtScAppEventHandlerStructType;


extern void CsrBtScAppEventHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType *csrBtScAppEventHandler);


#ifdef __cplusplus
}
#endif

#endif /* endif CSR_BT_GAP_APP_SC_EVENT_HANDLER_H__ */

