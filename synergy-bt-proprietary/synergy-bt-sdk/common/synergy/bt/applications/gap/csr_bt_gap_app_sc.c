/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_bt_gap_app_handler.h"
#include "csr_bt_gap_app_util.h"
#include "csr_bt_gap_app_sc_event_handler.h"
#include "csr_bt_gap_app_ui_strings.h"
#include "csr_util.h"
#include "csr_formatted_io.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_gap_app_prim.h"

#define PROFILE_SC CsrStrDup("SC")
#define GAP_POPUP_TIMEOUT (3*CSR_SCHED_SECOND)

extern CsrUint8  defGlobalPinLen;
extern CsrUint8  defGlobalPin[CSR_BT_PASSKEY_MAX_LEN];

void CsrBtGapPopupTimeout(CsrUint16 mi, void *mv)
{
    CsrBtGapInstData * inst = (CsrBtGapInstData *) mv;
    CsrBtGapCsrUiType * csrUiVar  = &(inst->csrUiVar);
    CSR_UNUSED(mi);

    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
    CsrBtGapHideUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI);

    csrUiVar->popupTimerId = 0;
}


static void csrBtScPasskeyIndFunc(
    void                *instData,          /* the applications own instance data                                           */
    CsrUint32            cod,                /* device class of peer device                                                  */
    CsrBtDeviceAddr      deviceAddr,         /* address of device for which a passkey is requested                           */
    CsrUtf8String        *deviceName,         /* name of device for which a passkey is requested                              */
    CsrUint8             paired,             /* actual pair status of the device                                             */
    CsrBtAddressType     addressType         /* type of address of device for which a passkey is requested               */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.


    The application layer is requested to insert a Bluetooth passkey.
    The application may insert the passkey directly, e.g. in a headset
    application, or may request the user through the MMI to insert the right passkey.
    */
    CsrBtGapInstData * inst = instData;

    inst->scVar.deviceAddr     = deviceAddr;
    inst->scVar.deviceAddrType = addressType;
    CsrUtf8StrNCpyZero(inst->scVar.name, deviceName, sizeof(CsrBtDeviceName));

    if (defGlobalPinLen != 0)
    {
        CsrBtScPasskeyResSendEx(TRUE, inst->scVar.deviceAddr, defGlobalPinLen, defGlobalPin, TRUE, TRUE,inst->scVar.deviceAddrType);
    }
    else
    {/* Set the menu on the CSR_BT_GAP_SC_PASSKEY_UI                               */
        CsrBtGapSetInputDialog(inst, CSR_BT_GAP_SC_PASSKEY_UI,
            CONVERT_TEXT_STRING_2_UCS2(deviceName), TEXT_SC_PASSKEY_UCS2,
            CSR_UI_ICON_KEY, NULL, CSR_BT_PASSKEY_MAX_LEN,
            CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);
        CsrBtGapShowUi(inst, CSR_BT_GAP_SC_PASSKEY_UI, CSR_UI_INPUTMODE_AUTO, 1);
    }
    CsrPmemFree(deviceName);
}

static void csrBtScBondCfmFunc(
    void                    *inst,              /* the applications own instance data                                           */
    CsrBool                 addedToScDbList,    /* If TRUE the device is added to the Security Manager's device database        */
    CsrBtDeviceAddr         deviceAddr,         /* address of device which is bonded to.                                        */
    CsrBtClassOfDevice      cod,                /* the class of device of the remote device                                     */
    CsrBtSupplier           supplier, 
    CsrBtResultCode         resultCode,         /* the result of the outgoing bonding procedure                                 */
    CsrBtAddressType        addressType         /* type of address of device to bond with.                                      */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    This is the confimation of an outgoing pairing/bonding procedure (initiated by a CSR_BT_SC_BOND_REQ).

    The following results can be returned:
    - CSR_BT_SUCCESS:               The outgoing pairing procedure is a success.
    - CSR_BT_BONDING_FAILED:        The outgoing pairing procedure fail because a wrong passkey
                                    is entered, if it is not possible to establish a
                                    connection toward the remote device.
    - CSR_BT_BONDING_NOT_ALLOWED:   The application were not allowed to initiate a CSR_BT_SC_BOND_REQ.
    - BONDING_CANCELLED:            The outgoing pairing procedure is cancelled. A
                                    CSR_BT_SC_CANCEL_BOND_REQ has been initiated from the application.
    - CSR_BT_INTERNAL_ERROR:        The previously stored link key for a given device could not be remove.
    */
    CsrBtGapInstData * localInst = (CsrBtGapInstData *) inst;

    if(supplier == CSR_BT_SUPPLIER_SC && resultCode == CSR_BT_RESULT_CODE_SC_SUCCESS)
    {

        CsrBtGapCsrUiType * csrUiVar  = &(localInst->csrUiVar);
        
        if ((localInst->proximityPairingHandle != 0xFFFF) && (localInst->scVar.proximityPairing))
        {
            CsrBtGapAppPairProximityDeviceCfm *prim = (CsrBtGapAppPairProximityDeviceCfm *)CsrPmemAlloc(sizeof(CsrBtGapAppPairProximityDeviceCfm));
            
            prim->type         = CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_CFM;
            prim->deviceAddr   = localInst->searchList->deviceListEntryArray[0].deviceAddr;
            prim->friendlyName = CsrUtf8StrDup(localInst->searchList->deviceListEntryArray[0].deviceName);
            prim->resultCode   = CSR_BT_GAP_APP_SUCCESS;
            prim->addressType  = addressType;

            CsrSchedMessagePut(localInst->proximityPairingHandle, CSR_BT_GAP_APP_PRIM, prim);
            localInst->proximityPairingHandle = 0xFFFF;
            return;
        }

        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                                        TEXT_SC_BONDING_SUCCESS_UCS2, TEXT_OK_UCS2, NULL);

        csrUiVar->popupTimerId = CsrSchedTimerSet(GAP_POPUP_TIMEOUT,
                                 CsrBtGapPopupTimeout,
                                 0,
                                 (void*)inst);

    }
    else
    {
        if ((localInst->proximityPairingHandle != 0xFFFF) && (localInst->scVar.proximityPairing))
        {
            CsrBtGapAppPairProximityDeviceCfm *prim = (CsrBtGapAppPairProximityDeviceCfm *)CsrPmemAlloc(sizeof(CsrBtGapAppPairProximityDeviceCfm));
            
            prim->type         = CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_CFM;
            CsrMemSet(&prim->deviceAddr, 0, sizeof(CsrBtDeviceAddr));
            prim->friendlyName = NULL;
            prim->resultCode   = CSR_BT_GAP_APP_BONDING_FAILED;
            prim->addressType  = addressType;

            CsrSchedMessagePut(localInst->proximityPairingHandle, CSR_BT_GAP_APP_PRIM, prim);
            localInst->proximityPairingHandle = 0xFFFF;
            return;
        }
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                                TEXT_SC_BONDING_FAILED_UCS2, TEXT_OK_UCS2, NULL);
    }
    CsrBtGapDisplaySetInputMode(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO);
}

static void csrBtScDeBondCfmFunc(
    void                *inst,          /* the applications own instance data                                           */
    CsrBtDeviceAddr      deviceAddr,         /* address of device that has been debonded.                                    */
    CsrBtSupplier supplier, CsrBtResultCode resultCode, /* the result of the outgoing bonding procedure                                 */
    CsrBtAddressType         addressType        /* type of address of device that has been debonded.                                   */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    This is the confimation of an de-bond procedure (initiated by a CSR_BT_SC_DEBOND_REQ).

    The de-bonding procedure is a fully local procedure, which remove the link key,
    i.e. no information is exchanged between the devices for which the bond exists.

    Please note that if the link key is currently in use for a connection,
    then the link key will not be deleted from the HCI layer until all the connections
    are disconnected.

    The following results can be returned:
    - CSR_BT_SUCCESS:              The de-bond procedure is a success, e.g the link key from
                                    the local device.
    - CSR_BT_INTERNAL_ERROR:       Only happens in very rare cases. If it happens the remote
                                    device must still be considered as paired, as the link key
                                    is not remove.
    */
    if(supplier == CSR_BT_SUPPLIER_SC && resultCode == CSR_BT_RESULT_CODE_SC_SUCCESS)
    {
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_DEBOND_UCS2,
                                        TEXT_SUCCESS_UCS2, TEXT_OK_UCS2, NULL);

    }
    else
    {
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_DEBOND_UCS2,
                                        TEXT_FAILED_UCS2, TEXT_OK_UCS2, NULL);
    }

    CsrBtGapDisplaySetInputMode(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO);
}

static void csrBtScSetSecurityModeCfmFunc(
    void                *inst,          /* the applications own instance data                                           */
    CsrBtSupplier supplier, CsrBtResultCode resultCode /* the result of the outgoing bonding procedure                                 */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    This is the confimation of the set security level procedure
    (initiated by a CSR_BT_SC_SET_SECURITY_MODE_REQ).

    Please note that the set security level procedure is a local procedure,
    and that the security level setting is applied for the device and not
    just for single connection between two devices.

    The following results can be returned:
    - CSR_BT_SUCCESS:               The security level has been set.
    - SECURITY_LEVEL_NOT_SET:       Fail to set the security level.
    */
    if(supplier == CSR_BT_SUPPLIER_SC && resultCode == CSR_BT_RESULT_CODE_SC_SUCCESS)
    {
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_MODE_UCS2,
                                        TEXT_SUCCESS_UCS2, TEXT_OK_UCS2, NULL);

    }
    else
    {
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_MODE_UCS2,
                                        TEXT_FAILED_UCS2, TEXT_OK_UCS2, NULL);
    }

    CsrBtGapDisplaySetInputMode(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO);
}

static void csrBtScEncryptionCfmFunc(
    void                *instData,          /* the applications own instance data                                           */
    CsrUint16            encType,            /* encryption Type                                                              */
    CsrBtDeviceAddr       deviceAddr,         /* address of device for which encryption is requested                          */
    CsrBtSupplier supplier, CsrBtResultCode resultCode  /* the result of the outgoing bonding procedure                        */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    This is the confimation of an encryption procedure (initiated by a CSR_BT_SC_ENCRYPTION_REQ).

    Please note the if the application have not been paired at the time the encryption
    procedure is initiated (switching encryption on), pairing will be initiated at
    link-level as part of the encryption procedure. As a result of this, a request
    for a passkey will be sent to the application prior to switching the encryption on.

    The following results can be returned:
    - CSR_BT_SUCCESS:                The requested encryption mode has been set.
    - ERROR_UNSPECIFIED:      The requested encryption mode could not set.
    */

}

static void csrBtScAuthoriseIndFunc(
    void                *instData,          /* the applications own instance data                                           */
    CsrBtDeviceAddr       deviceAddr,         /* address of device that must be authorised                                    */
    CsrUtf8String       *deviceName,         /* name of device which must be authorised                                      */
    uuid16_t             serviceId,          /* the id of the service for which authorisation is required                    */
    CsrBtAddressType     addressType        /* type of address of device that must be authorised                             */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    The application is requested to grant access to the service defined by "serviceId".
    If access is granted by the application the remote device is allowed to used
    this service, otherwise not.

    Please note that the authorisation procedure is a local procedure,
    which does not involve any over the air procedure. Please also note
    that the application must response to a CSR_BT_SC_AUTHORISE_IND,
    because the SC will not accept other signals before this is done.
    */
    /* Set the menu on the CSR_BT_GAP_SC_AUTHORISE_UI                               */
   CsrBtGapInstData *inst = instData;
   CsrCharString displayString[50];

   CsrSnprintf(displayString, 50, "\nAllow connection to service %u from device %s (%04X:%02X:%06X) [Yes/No]: ",
               serviceId,
           deviceName,
           deviceAddr.nap,
           deviceAddr.uap,
           deviceAddr.lap);

   inst->scVar.deviceAddr     = deviceAddr;
   inst->scVar.deviceAddrType = addressType;
   CsrUtf8StrNCpyZero(inst->scVar.name, deviceName, sizeof(CsrBtDeviceName));

   CsrBtGapSetInputDialog(inst, CSR_BT_GAP_SC_AUTHORISE_UI,
     NULL, CONVERT_TEXT_STRING_2_UCS2(displayString),
     CSR_UI_ICON_KEY,  NULL, 0,
     CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, TEXT_NO_UCS2);

   /* Show the CSR_BT_GAP_SC_AUTHORISE_UI on the display                                  */
   CsrBtGapShowUi(inst, CSR_BT_GAP_SC_AUTHORISE_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

    CsrPmemFree(deviceName);

}

static void csrBtScBondIndFunc(
    void                    *inst,              /* the applications own instance data                                           */
    CsrBool                 addedToScDbList,    /* If TRUE the device is added to the Security Manager's device database        */
    CsrBtDeviceAddr         deviceAddr,         /* address of device that has been bonded                                       */
    CsrBtClassOfDevice      cod,                /* the class of device of the remote device                                     */
    CsrBtSupplier           supplier, 
    CsrBtResultCode         resultCode,         /* the result of the outgoing bonding procedure                                 */
    CsrBtAddressType        addressType         /* type of address of device to bond with.                                      */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    This is the confimation of a incoming pairing/bonding procedure.

    The following results can be returned:
    - CSR_BT_SUCCESS:              The incoming pairing procedure is a success.
    - CSR_BT_BONDING_FAILED:       The incoming pairing procedure fail because a wrong passkey
                            is entered, if it is not possible to establish a connection
                            toward the remote device. (only in the case where CSR_BT initiate pairing)

    Please note that from the application point of view it will always look like a
    remote device has initiate the pairing procedure.
    */
    CsrBtGapInstData * localInst = (CsrBtGapInstData *) inst;

    if(supplier == CSR_BT_SUPPLIER_SC && resultCode == CSR_BT_RESULT_CODE_SC_SUCCESS)
    {

        CsrBtGapCsrUiType * csrUiVar  = &(localInst->csrUiVar);

        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                                        TEXT_SC_BONDING_SUCCESS_UCS2, TEXT_OK_UCS2, NULL);

        csrUiVar->popupTimerId = CsrSchedTimerSet(GAP_POPUP_TIMEOUT,
                                 CsrBtGapPopupTimeout,
                                 0,
                                 (void*)inst);
    }
    else
    {
        CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                                        TEXT_SC_BONDING_FAILED_UCS2, TEXT_OK_UCS2, NULL);
    }

    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

}

static void csrBtScSspPasskeyIndFunc(
    void                    *instData,                  /* the application own instance data                                        */
    CsrUint32                cod,                        /* device class of peer device                                              */
    CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
    CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
    CsrUint8                 paired,                     /* actual pair status of the device                                         */
    CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
    CsrBtAddressType         addressType                 /* type of address of device for which a passkey is requested               */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    The message can only occur if both the local and the remote device supports
    Secure Simple Pairing (SSP).

    The application layer is requested to send a Bluetooth passkey back in the
    CSR_BT_SC_SSP_PASSKEY_RES message.
    The application may insert the passkey directly, e.g. in a headset
    application, or may request the user through the MMI to insert the right passkey.

    The paired parameter specifies whether the two devices already are paired or not
    and which pairing that eventually is present:
    - CSR_BT_SC_PAIRING_NONE       - no bond exists
    - CSR_BT_SC_PAIRING_LEGACY     - paired with legacy method
    - CSR_BT_SC_PAIRING_NO_MITM    - paired with SSP and no MITM
    - CSR_BT_SC_PAIRING_MITM       - paired with SSP and MITM

    The authRequirements parameter specifies the authetication requirements requested
    by the remote device:
    - HCI_MITM_NOT_REQUIRED_NO_BONDING
    - HCI_MITM_REQUIRED_NO_BONDING
    - HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_NOT_REQUIRED_GENERAL_BONDING
    - HCI_MITM_REQUIRED_GENERAL_BONDING
    */

    CsrBtGapInstData * inst = instData;

    inst->scVar.deviceAddr     = deviceAddr;
    inst->scVar.deviceAddrType = addressType;
    CsrUtf8StrNCpyZero(inst->scVar.name, deviceName, sizeof(CsrBtDeviceName));

    if (defGlobalPinLen != 0)
    {
        CsrUint32 numericValue = CsrStrToInt((char*)defGlobalPin);

        CsrBtScSspPasskeyResSendEx(TRUE, inst->scVar.deviceAddr, numericValue, TRUE, TRUE, addressType);
    }
    else
    {/* Set the menu on the CSR_BT_GAP_SC_SSP_PASSKEY_UI                               */
        CsrBtGapSetInputDialog(inst, CSR_BT_GAP_SC_SSP_PASSKEY_UI,
            CONVERT_TEXT_STRING_2_UCS2(deviceName), TEXT_SC_SSP_PASSKEY_UCS2,
            CSR_UI_ICON_KEY, NULL, CSR_BT_PASSKEY_MAX_LEN,
            CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);
        CsrBtGapShowUi(inst, CSR_BT_GAP_SC_SSP_PASSKEY_UI, CSR_UI_INPUTMODE_AUTO, 1);
    }
    CsrPmemFree(deviceName);
    
}

static void csrBtScSspNotificationIndFunc(
    void                    *instData,                  /* the applications own instance data                                       */
    CsrUint32                cod,                        /* device class of peer device                                              */
    CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
    CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
    CsrUint8                 paired,                     /* actual pair status of the device                                         */
    CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
    CsrUint32                numericalValue,              /* number the MMI shall show on the display to enable the user to input it
                                                           on a remote device                                                       */
    CsrBtAddressType         addressType                 /* type of address of device for which a passkey is requested              */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    The message can only occur if both the local and the remote device supports
    Secure Simple Pairing (SSP).

    The application layer is requested to show the numericalValue parameter in the
    display to allow the user to input this value on the remote device. Then the
    application shall send the CSR_BT_SC_SSP_NOTIFICATION_RES message.

    The paired parameter specifies whether the two devices already are paired or not
    and which pairing that eventually is present:
    - CSR_BT_SC_PAIRING_NONE       - no bond exists
    - CSR_BT_SC_PAIRING_LEGACY     - paired with legacy method
    - CSR_BT_SC_PAIRING_NO_MITM    - paired with SSP and no MITM
    - CSR_BT_SC_PAIRING_MITM       - paired with SSP and MITM

    The authRequirements parameter specifies the authetication requirements requested
    by the remote device:
    - HCI_MITM_NOT_REQUIRED_NO_BONDING
    - HCI_MITM_REQUIRED_NO_BONDING
    - HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_NOT_REQUIRED_GENERAL_BONDING
    - HCI_MITM_REQUIRED_GENERAL_BONDING
    */
    CsrBtGapInstData * inst = instData;

    inst->scVar.deviceAddr     = deviceAddr;
    inst->scVar.deviceAddrType = addressType;
    inst->scVar.SspNtfnNumericValue = numericalValue;

    CsrUtf8StrNCpyZero(inst->scVar.name, deviceName, sizeof(CsrBtDeviceName));

    /* Set the menu on the CSR_BT_GAP_SC_SSP_NOTIFICATION_UI                               */
    CsrBtGapSetInputDialog(inst, CSR_BT_GAP_SC_SSP_NOTIFICATION_UI,
        CONVERT_TEXT_STRING_2_UCS2(deviceName), TEXT_SC_SSP_NOTIFICATION_UCS2,
        CSR_UI_ICON_KEY,  CsrUint32ToUtf16String(numericalValue), CSR_BT_PASSKEY_MAX_LEN,
        CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);
    CsrBtGapShowUi(inst, CSR_BT_GAP_SC_SSP_NOTIFICATION_UI, CSR_UI_INPUTMODE_AUTO, 1);
    CsrPmemFree(deviceName);
}

static void csrBtScSspCompareIndFunc(
    void                    *instData,                  /* the applications own instance data                                       */
    CsrUint32                cod,                        /* device class of peer device                                              */
    CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
    CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
    CsrUint8                 paired,                     /* actual pair status of the device                                         */
    CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
    CsrUint32                numericalValue,             /* number the MMI shall show on the display to enable the user to compare it
                                                           with value shown on a remote device                                      */
    CsrBtAddressType         addressType                 /* type of address of device for which a passkey is requested              */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    The message can only occur if both the local and the remote device supports
    Secure Simple Pairing (SSP).

    The application layer is requested to show the numericalValue parameter in the
    display to enable the user verify this value agains the value shown on the
    remote device. AFter user acknowledge the application shall send the
    CSR_BT_SC_SSP_JUSTWORKS_RES message.

    The paired parameter specifies whether the two devices already are paired or not
    and which pairing that eventually is present:
    - CSR_BT_SC_PAIRING_NONE       - no bond exists
    - CSR_BT_SC_PAIRING_LEGACY     - paired with legacy method
    - CSR_BT_SC_PAIRING_NO_MITM    - paired with SSP and no MITM
    - CSR_BT_SC_PAIRING_MITM       - paired with SSP and MITM

    The authRequirements parameter specifies the authetication requirements requested
    by the remote device:
    - HCI_MITM_NOT_REQUIRED_NO_BONDING
    - HCI_MITM_REQUIRED_NO_BONDING
    - HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_NOT_REQUIRED_GENERAL_BONDING
    - HCI_MITM_REQUIRED_GENERAL_BONDING
    */
    CsrBtGapInstData * inst = instData;

    inst->scVar.deviceAddr     = deviceAddr;
    inst->scVar.deviceAddrType = addressType;
    CsrUtf8StrNCpyZero(inst->scVar.name, deviceName, sizeof(CsrBtDeviceName));

    /* Set the menu on the CSR_BT_GAP_SC_SSP_COMPARE_UI                               */
    CsrBtGapSetInputDialog(inst, CSR_BT_GAP_SC_SSP_COMPARE_UI,
        CONVERT_TEXT_STRING_2_UCS2(deviceName), TEXT_SC_SSP_COMPARE_UCS2,
        CSR_UI_ICON_KEY,  CsrUint32ToUtf16String(numericalValue), CSR_BT_PASSKEY_MAX_LEN,
        CSR_UI_KEYMAP_NUMERIC,TEXT_YES_UCS2, TEXT_NO_UCS2);
    CsrBtGapShowUi(inst, CSR_BT_GAP_SC_SSP_COMPARE_UI, CSR_UI_INPUTMODE_AUTO, 1);
    CsrPmemFree(deviceName);
}

static void csrBtScSspJustworksIndFunc(
    void                    *instData,                  /* the applications own instance data                                       */
    CsrUint32                cod,                        /* device class of peer device                                              */
    CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
    CsrUtf8String            *deviceName,                 /* name of device for which a passkey is requested                          */
    CsrUint8                 paired,                     /* actual pair status of the device                                         */
    CsrUint32                authRequirements,           /* The authentication type requested by the remote device                   */
    CsrBtAddressType         addressType                 /* type of address of device for which a passkey is requested              */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    The message can only occur if both the local and the remote device supports
    Secure Simple Pairing (SSP).

    The application layer is requested to accept an incoming pair and it shall
    send the CSR_BT_SC_SSP_JUSTWORKS_RES message as accept. However, the application
    layer may request acknowledge from the user before sending the acknowledge
    but this is not required.

    The paired parameter specifies whether the two devices already are paired or not
    and which pairing that eventually is present:
    - CSR_BT_SC_PAIRING_NONE       - no bond exists
    - CSR_BT_SC_PAIRING_LEGACY     - paired with legacy method
    - CSR_BT_SC_PAIRING_NO_MITM    - paired with SSP and no MITM
    - CSR_BT_SC_PAIRING_MITM       - paired with SSP and MITM

    The authRequirements parameter specifies the authetication requirements requested
    by the remote device:
    - HCI_MITM_NOT_REQUIRED_NO_BONDING
    - HCI_MITM_REQUIRED_NO_BONDING
    - HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_REQUIRED_DEDICATED_BONDING
    - HCI_MITM_NOT_REQUIRED_GENERAL_BONDING
    - HCI_MITM_REQUIRED_GENERAL_BONDING
    */
    CsrBtGapInstData * inst = instData;

    inst->scVar.deviceAddr     = deviceAddr;
    inst->scVar.deviceAddrType = addressType;
    CsrUtf8StrNCpyZero(inst->scVar.name, deviceName, sizeof(CsrBtDeviceName));

    /* Set the menu on the CSR_BT_GAP_SC_SSP_JUST_WORKS_UI                               */
    CsrBtGapSetInputDialog(inst, CSR_BT_GAP_SC_SSP_JUST_WORKS_UI,
        CONVERT_TEXT_STRING_2_UCS2(deviceName), TEXT_SC_JUST_WORKS_UCS2,
        CSR_UI_ICON_KEY, NULL, 0,
        CSR_UI_KEYMAP_NUMERIC, TEXT_YES_UCS2, TEXT_NO_UCS2);
    CsrBtGapShowUi(inst, CSR_BT_GAP_SC_SSP_JUST_WORKS_UI, CSR_UI_INPUTMODE_AUTO, 1);
    CsrPmemFree(deviceName);
}

static void csrBtScSspKeypressNotificationIndFunc(
    void                    *instData,                  /* the applications own instance data                                       */
    CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a passkey is requested                       */
    CsrUint8                 notificationType,           /* the reason from the remote device for requesting additional time         */
    CsrBtAddressType         addressType                  /* type of address of device for which a passkey is requested              */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    The message can only occur if both the local and the remote device supports
    Secure Simple Pairing (SSP).

    The application layer is informed from the remote device that the user is doing
    something in relation to the pair process and by this it can be used to restart
    the timers used in the pairing process. The specific event is described in
    the notificationType parameter as:
    - HCI_NOTIFICATION_TYPE_PASSKEY_STARTED
    - HCI_NOTIFICATION_TYPE_PASSKEY_DIGIT_ENTERED
    - HCI_NOTIFICATION_TYPE_PASSKEY_DIGIT_ERASED
    - HCI_NOTIFICATION_TYPE_PASSKEY_CLEARED
    - HCI_NOTIFICATION_TYPE_PASSKEY_COMPLETED
    */
}

static void csrBtScModeIndFunc(
    void                *instData,                  /* the applications own instance data                                       */
    CsrUint8             mode                        /* the actual idle pairing mode behaviour of the local device               */
    )
{
    /*
    This message is send to the tasks specified in the parameter "hdl"
    in the CSR_BT_SC_ACTIVATE_REQ message.

    The message occurs as information to the application that the actual pairing
    mode is changed. The message occurs when after the expiry of a timer set
    by the duration parameter present in the CSR_BT_SC_MODE_REQ message.

    The mode parameter can be:
    - SEC_MODE_PAIRABLE     - allow pairing
    - SEC_MODE_NON_PAIRABLE - reject pairing
    - SEC_MODE_NON_BONDABLE - allow pairing if remote device does not request
                              dedicated/general bonding else reject.
    */
}


static void csrBtScRebondIndFunc(
    void                    *instData,                   /* the application own instance data                                        */
    CsrUint32                cod,                        /* device class of peer device                                              */
    CsrBtDeviceAddr          deviceAddr,                 /* address of device for which a rebond is requested                        */
    CsrUtf8String            *deviceName,                /* name of device for which a rebond is requested                           */
    CsrBtAddressType         addressType                 /* type of address of device for which a rebond is requested                */
    )
{
    /*
      This message is send to the tasks specified in the parameter "hdl"
      in the CSR_BT_SC_ACTIVATE_REQ message.

      The application layer is requested to accept an rebonding and it shall
      send the CSR_BT_SC_REBOND_RES message as accept.
    */
    CsrPmemFree(deviceName);

}

static void csrBtScLeSecurityIndFunc(
    void                    *instData,                  /* the application own instance data                */
    CsrBtSupplier           supplier, 
    CsrBtResultCode         resultCode,                 /* the result of the outgoing bonding procedure     */
    CsrBtTypedAddr          address,                    /* Address of device to bond with.                  */
    CsrBool                 addedToScDbList)
{
    /*
      This message is send to the tasks specified in the parameter "hdl"
      in the CSR_BT_SC_ACTIVATE_REQ message.

      The application layer is requested to accept SC_LE_SECUROTY_IND
    */
    CsrBtGapInstData * localInst = (CsrBtGapInstData *) instData;

    if(supplier == CSR_BT_SUPPLIER_SC && resultCode == CSR_BT_RESULT_CODE_SC_SUCCESS)
    {
        CsrBtGapCsrUiType * csrUiVar  = &(localInst->csrUiVar);

        CsrBtGapSetDialog(instData, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                          TEXT_SC_SSP_COMPLETE_UCS2, TEXT_OK_UCS2, NULL);

        csrUiVar->popupTimerId = CsrSchedTimerSet(GAP_POPUP_TIMEOUT,
                                 CsrBtGapPopupTimeout,
                                 0,
                                 (void*)instData);
    }
    else
    {
        CsrBtGapSetDialog(instData, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                          TEXT_SC_SSP_FAILED_UCS2, TEXT_OK_UCS2, NULL);
    }
}


/*
Setup of the functions.
The table below shall be configured for each application that can receive
messages from the CSR_BT SC.
If some of the messages will never occur in the specific application the function reference
can be set to "NULL" in the table.
*/
static const struct CsrBtScAppEventHandlerStructType csrBtScAppEventFunctions =
{
    csrBtScPasskeyIndFunc,                   /* CSR_BT_SC_PASSKEY_IND                       */
    csrBtScBondCfmFunc,                      /* CSR_BT_SC_BOND_CFM                          */
    csrBtScDeBondCfmFunc,                    /* CSR_BT_SC_DEBOND_CFM                        */
    csrBtScSetSecurityModeCfmFunc,           /* CSR_BT_SC_SET_SECURITY_MODE_CFM             */
    csrBtScEncryptionCfmFunc,                /* CSR_BT_SC_ENCRYPTION_CFM                    */
    csrBtScAuthoriseIndFunc,                 /* CSR_BT_SC_AUTHORISE_IND                     */
    csrBtScBondIndFunc,                      /* CSR_BT_SC_BOND_IND                          */

    csrBtScSspPasskeyIndFunc,                /* CSR_BT_SC_SSP_PASSKEY_IND                   */
    csrBtScSspNotificationIndFunc,           /* CSR_BT_SC_SSP_NOTIFICATION_IND              */
    csrBtScSspCompareIndFunc,                /* CSR_BT_SC_SSP_COMPARE_IND                   */
    csrBtScSspJustworksIndFunc,              /* CSR_BT_SC_SSP_JUSTWORKS_IND                 */
    csrBtScSspKeypressNotificationIndFunc,   /* CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_IND     */
    csrBtScModeIndFunc,                      /* CSR_BT_SC_MODE_IND                          */
    csrBtScRebondIndFunc,                    /* CSR_BT_SC_REBOND_IND                        */
    csrBtScLeSecurityIndFunc,                /* CSR_BT_SC_LE_SECURITY_IND                   */
};

/* setup the global functional pointer that shall be overloaded to the event function */
const CsrBtScAppEventHandlerStructType * const CsrBtGapCsrBtScAppEventFunctionsPtr = &csrBtScAppEventFunctions;




