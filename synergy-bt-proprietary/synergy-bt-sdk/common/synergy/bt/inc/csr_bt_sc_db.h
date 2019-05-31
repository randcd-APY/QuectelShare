#ifndef CSR_BT_SC_DB_H__
#define CSR_BT_SC_DB_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_types.h"
#include "bluetooth.h"
#include "csr_bt_addr.h"

#ifdef CSR_AMP_ENABLE
#include "csr_bt_amp_hci.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 *
 *      SC device database record
 *
 *-----------------------------------------------------------------------*/
typedef struct
{
    BD_ADDR_T            deviceAddr;
    CsrBool              linkkeyValid;
    CsrUint8             linkkeyLen;
    CsrBtDeviceLinkkey   Linkkey;
    CsrBtDeviceName      remoteName;
    CsrBtClassOfDevice   classOfDevice;
    CsrUint32            knownServices11_00_31;
    CsrUint32            knownServices11_32_63;
    CsrUint32            knownServices12_00_31;
    CsrUint32            knownServices13_00_31;
    CsrBool              authorised;
    CsrUint8             linkkeyType;

#ifdef CSR_AMP_ENABLE
    /* For AMP enabled systems, we need to store all AMP keys */
    CsrUint8             ampKeyGenericType;
    CsrUint8             ampKeyGenericKey[CSR_BT_AMP_LINK_KEY_SIZE_GAMP];
    CsrUint8             ampKeyDot11Type;
    CsrUint8             ampKeyDot11Key[CSR_BT_AMP_LINK_KEY_SIZE_802_11];
    CsrUint8             ampKeyUwbType;
    CsrUint8             ampKeyUwbKey[CSR_BT_AMP_LINK_KEY_SIZE_ECMA_368];
#endif

#ifdef CSR_BT_LE_ENABLE
    CsrBtAddressType        addressType;
    CsrUint16               leKeyValid;
    DM_SM_KEY_ENC_CENTRAL_T leKeyEncCentral;
    DM_SM_KEY_ID_T          leKeyId;
    DM_SM_KEY_SIGN_T        leKeySign;
    CsrUint16               leKeyDiv;
    CsrUint32               leKeySignCounter;
    CsrUint16               leKeySize;
    CsrUint16               leSecurityRequirements;
    CsrUint32               serviceChangedHandle;
    CsrUint16               clientConfigValue;
#ifdef CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT
    CsrBtTypedAddr          idAddr;
    CsrUint8                carValue;
    CsrBool                 rpaOnlyPresent;
#endif /* CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT */
#endif
} CsrBtScDbDeviceRecord;

#define CSR_BT_SC_DB_LE_KEY_NONE          (0x0000)
#define CSR_BT_SC_DB_LE_KEY_ENC_CENTRAL   (0x0001)
#define CSR_BT_SC_DB_LE_KEY_ID            (0x0002)
#define CSR_BT_SC_DB_LE_KEY_SIGN          (0x0004)
#define CSR_BT_SC_DB_LE_KEY_DIV           (0x0008)
#define CSR_BT_SC_DB_LE_KEY_SIGN_COUNTER  (0x0010)

/* Version alternates - do not touch! */
#if defined(CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT) && defined(CSR_AMP_ENABLE)
#define CSR_BT_SC_DB_FILE_ALTERNATE    (5)
#elif defined(CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT)
#define CSR_BT_SC_DB_FILE_ALTERNATE    (4)
#elif defined(CSR_BT_LE_ENABLE) && defined(CSR_AMP_ENABLE)
#define CSR_BT_SC_DB_FILE_ALTERNATE    (3)
#elif defined(CSR_BT_LE_ENABLE)
#define CSR_BT_SC_DB_FILE_ALTERNATE    (2)
#elif defined(CSR_AMP_ENABLE)
#define CSR_BT_SC_DB_FILE_ALTERNATE    (1)
#else
#define CSR_BT_SC_DB_FILE_ALTERNATE    (0)
#endif

/* Do not forget to increase the version number if the
 * CsrBtScDbDeviceRecord structure is changed. Also note that you may
 * need to increase the value by 5 due to the ifdef-permutations */
#define CSR_BT_SC_DB_FILE_VERSION      ((CsrUint32) (0xFFFF0002 + CSR_BT_SC_DB_FILE_ALTERNATE))

#define CSR_BT_SC_DB_FILE_VERSION_SIZE (sizeof(CsrUint32))

#if defined(USE_ATLAS7_PLATFORM) || defined(USE_MSM_PLATFORM)


#define DATA_STORAGE_PATH_SUB_0          "var/lib"
#define DATA_STORAGE_PATH_SUB_1          "csr_synergy"
#define DATA_STORAGE_PATH_SUB_2          "data"
#define DATA_STORAGE_PATH                "/" DATA_STORAGE_PATH_SUB_0 "/" DATA_STORAGE_PATH_SUB_1 "/" DATA_STORAGE_PATH_SUB_2
#define CSR_BT_SC_DB_FILE_NAME           ((CsrUint8*)DATA_STORAGE_PATH "/" "sc_db.db")

#elif defined(USE_MDM_PLATFORM)

/* [QTI] Store sc_db file under "/data". */
#define CSR_BT_SC_DB_FILE_NAME           ((const CsrUtf8String *) (DEFAULT_DATA_STORAGE_PATH "sc_db.db"))

#elif defined(USE_IMX_PLATFORM)
#define CSR_BT_SC_DB_FILE_NAME           ((const CsrUtf8String *) ("/home/root/" "sc_db.db"))

#endif

/*********************************************************************************
  read a device record in the NVS database. If not found return NULL in theRecord
********************************************************************************/
CsrBool CsrBtScDbRead(CsrBtDeviceAddr *theAddr, CsrBtScDbDeviceRecord *theRecord);
void CsrBtScDbWrite(CsrBtDeviceAddr *theAddr, CsrBtScDbDeviceRecord *theRecord);
void CsrBtScDbRemove(CsrBtDeviceAddr *theAddr);
CsrBool CsrBtScDbReadFirst(CsrBtScDbDeviceRecord *theRecord);
CsrBool CsrBtScDbReadNext(CsrBtScDbDeviceRecord *theRecord);
CsrBool CsrBTScCheckAndCreateFilePath();

#ifdef __cplusplus
}
#endif

#endif /* ndef __SC_DB_H */
