/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_gap_app_sc_event_handler.h"

typedef void (* csrBtScAppStateHandlerType)(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr);

static void csrBtScPasskeyIndHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScPasskeyInd            * prim = (CsrBtScPasskeyInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScPasskeyIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScPasskeyIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScPasskeyIndFunc(instData, prim->cod, prim->deviceAddr, prim->deviceName,prim->paired, prim->addressType);
    }
}

static void csrBtScBondCfmHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScBondCfm               * prim = (CsrBtScBondCfm *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScBondCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScBondCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScBondCfmFunc(instData, prim->addedToScDbList, prim->deviceAddr, prim->cod, prim->resultSupplier, prim->resultCode, prim->addressType);
    }
}

static void csrBtScDeBondCfmHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScDebondCfm             * prim = (CsrBtScDebondCfm *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScDeBondCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScDeBondCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScDeBondCfmFunc(instData, prim->deviceAddr, prim->resultSupplier, prim->resultCode, prim->addressType);
    }
}

static void csrBtScSetSecurityModeCfmHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScSetSecurityModeCfm * prim = (CsrBtScSetSecurityModeCfm *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScSetSecurityModeCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScSetSecurityModeCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScSetSecurityModeCfmFunc(instData, prim->resultSupplier, prim->resultCode);
    }
}

static void csrBtScEncryptionCfmHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScEncryptionCfm        * prim = (CsrBtScEncryptionCfm *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScEncryptionCfmFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScEncryptionCfmFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScEncryptionCfmFunc(instData, prim->encType, prim->deviceAddr, prim->resultSupplier, prim->resultCode);
    }
}

static void csrBtScAuthoriseIndHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScAuthoriseInd         * prim = (CsrBtScAuthoriseInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScAuthoriseIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScAuthoriseIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScAuthoriseIndFunc(instData, prim->deviceAddr, prim->deviceName, prim->serviceId, prim->addressType);
    }
}

static void csrBtScBondIndHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScBondInd               * prim = (CsrBtScBondInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScBondIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScBondIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScBondIndFunc(instData, prim->addedToScDbList, prim->deviceAddr, prim->cod, prim->resultSupplier, prim->resultCode, prim->addressType);
    }
}

static void csrBtScSspPasskeyInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScSspPasskeyInd    * prim = (CsrBtScSspPasskeyInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScSspPasskeyIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScSspPasskeyIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScSspPasskeyIndFunc(instData, prim->cod, prim->deviceAddr, prim->deviceName,prim->paired,prim->authRequirements, prim->addressType);
    }
}

static void csrBtScSspNotificationInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScSspNotificationInd    * prim = (CsrBtScSspNotificationInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScSspNotificationIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScSspNotificationIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScSspNotificationIndFunc(instData, prim->cod, prim->deviceAddr, prim->deviceName,prim->paired,
                                                                prim->authRequirements,prim->numericValue, prim->addressType);
    }
}

static void csrBtScSspCompareInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScSspCompareInd    * prim = (CsrBtScSspCompareInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScSspCompareIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScSspCompareIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScSspCompareIndFunc(instData, prim->cod, prim->deviceAddr, prim->deviceName,prim->paired,
                                                                prim->authRequirements,prim->numericValue, prim->addressType);
    }
}

static void csrBtScSspJustworksInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScSspJustworksInd  * prim = (CsrBtScSspJustworksInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScSspJustworksIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScSspJustworksIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScSspJustworksIndFunc(instData, prim->cod, prim->deviceAddr, prim->deviceName,prim->paired,prim->authRequirements, prim->addressType);
    }
}

static void csrBtScSspKeypressNotificationInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScSspKeypressNotificationInd  * prim = (CsrBtScSspKeypressNotificationInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScSspKeypressNotificationIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScSspKeypressNotificationIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScSspKeypressNotificationIndFunc(instData, prim->deviceAddr, prim->notificationType, prim->addressType);
    }
}

static void csrBtScModeInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScModeInd   * prim = (CsrBtScModeInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScModeIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScModeIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScModeIndFunc(instData, prim->mode);
    }
}

static void csrBtScRebondInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScRebondInd   * prim = (CsrBtScRebondInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScRebondIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScRebondIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScRebondIndFunc(instData, prim->cod, prim->deviceAddr, prim->deviceName, prim->addressType);
    }
}

static void csrBtScLeSecurityInd(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandlerPtr)
{
    CsrBtScLeSecurityInd   * prim = (CsrBtScLeSecurityInd *) message;

    if(csrBtScAppEventHandlerPtr->csrBtScLeSecurityIndFunc == NULL)
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, prim->type, "csrBtScRebondIndFunc Event function not defined\n");
    }
    else
    {
        csrBtScAppEventHandlerPtr->csrBtScLeSecurityIndFunc(instData, prim->resultSupplier, prim->resultCode, prim->address, prim->addedToScDbList );
    }
}

static const csrBtScAppStateHandlerType csrBtScAppStateHandler[CSR_BT_SC_PRIM_UPSTREAM_COUNT] =
{
    csrBtScPasskeyIndHandler,                       /* CSR_BT_SC_PASSKEY_IND                           */
    csrBtScBondCfmHandler,                          /* CSR_BT_SC_BOND_CFM                              */
    csrBtScDeBondCfmHandler,                        /* CSR_BT_SC_DEBOND_CFM                            */
    csrBtScSetSecurityModeCfmHandler,               /* CSR_BT_SC_SET_SECURITY_LEVEL_CFM                */
    csrBtScEncryptionCfmHandler,                    /* CSR_BT_SC_ENCRYPTION_CFM                        */
    csrBtScAuthoriseIndHandler,                     /* CSR_BT_SC_AUTHORISE_IND                         */
    csrBtScBondIndHandler,                          /* CSR_BT_SC_BOND_IND                              */
    NULL,                                           /* CSR_BT_SC_READ_DEVICE_RECORD_IND, private msg   */
    NULL,                                           /* CSR_BT_SC_READ_DEVICE_RECORD_CFM, private msg   */
    NULL,                                           /* CSR_BT_SC_UPDATE_TRUST_LEVEL_IND, private msg   */
    NULL,                                           /* CSR_BT_SC_AUTHENTICATE_CFM, private msg         */
    csrBtScSspPasskeyInd,                           /* CSR_BT_SC_SSP_PASSKEY_IND                       */
    csrBtScSspNotificationInd,                      /* CSR_BT_SC_SSP_NOTIFICATION_IND                  */
    csrBtScSspCompareInd,                           /* CSR_BT_SC_SSP_COMPARE_IND                       */
    csrBtScSspJustworksInd,                         /* CSR_BT_SC_SSP_JUSTWORKS_IND                     */
    NULL,                                           /* CSR_BT_SC_READ_LOCAL_OOB_DATA_CFM, not used     */
    NULL,                                           /* CSR_BT_SC_SSP_PAIRING_IND                       */
    csrBtScSspKeypressNotificationInd,              /* CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_IND         */
    csrBtScModeInd,                                 /* CSR_BT_SC_MODE_IND                              */
    csrBtScRebondInd,                               /* CSR_BT_SC_REBOND_IND                            */
    NULL,                                           /* CSR_BT_SC_DEBUG_MODE_CFM, not used              */
    NULL,                                           /* CSR_BT_SC_AUTHORISE_EXT_IND                     */
    NULL,                                           /* CSR_BT_SC_DATABASE_CFM                          */
    NULL,                                           /* CSR_BT_SC_CANCEL_ACCEPT_BOND_IND                */
    csrBtScLeSecurityInd,                           /* CSR_BT_SC_LE_SECURITY_IND                       */
    NULL,                                           /* CSR_BT_SC_ENCRYPTION_KEY_SIZE_CFM               */
    NULL,                                           /* CSR_BT_SC_LOCAL_OOB_DATA_EXPIRED_IND            */
};



void CsrBtScAppEventHandler(void * instData, void * message, CsrBtScAppEventHandlerStructType * csrBtScAppEventHandler)
{
    CsrBtScPrim * primType = (CsrBtScPrim *) message;
    CsrUint16   index     = (CsrUint16)(*primType - CSR_BT_SC_PRIM_UPSTREAM_LOWEST);

    if(index < CSR_BT_SC_PRIM_UPSTREAM_COUNT && csrBtScAppStateHandler[index] != NULL)
    {
        csrBtScAppStateHandler[index](instData, message, csrBtScAppEventHandler);
    }
    else
    {
        CsrGeneralException("CSR_BT_GAP_SC", 0, *primType, "Received CSR_BT SC primitive is unknown or private\n");
    }
}
