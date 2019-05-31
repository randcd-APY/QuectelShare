#ifndef CSR_BT_WL_DB_H__
#define CSR_BT_WL_DB_H__

/****************************************************************************

Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_types.h"
#include "bluetooth.h"
#include "csr_bt_addr.h"
#include "csr_bt_sc_db.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CsrBtWlDbRecord
{
    CsrBtTypedAddr addr;
    struct CsrBtWlDbRecord* next;
    struct CsrBtWlDbRecord* prev;
}CsrBtWlDbRecord;

#define CSR_BT_SC_DB_LE_KEY_NONE          (0x0000)
#define CSR_BT_SC_DB_LE_KEY_ENC_CENTRAL   (0x0001)
#define CSR_BT_SC_DB_LE_KEY_ID            (0x0002)
#define CSR_BT_SC_DB_LE_KEY_SIGN          (0x0004)
#define CSR_BT_SC_DB_LE_KEY_DIV           (0x0008)
#define CSR_BT_SC_DB_LE_KEY_SIGN_COUNTER  (0x0010)

/* Version alternates - do not touch! */
#if defined(CSR_BT_LE_ENABLE) && defined(CSR_AMP_ENABLE)
#define CSR_BT_WL_DB_FILE_ALTERNATE       (3)
#elif defined(CSR_BT_LE_ENABLE)
#define CSR_BT_WL_DB_FILE_ALTERNATE       (2)
#elif defined(CSR_AMP_ENABLE)
#define CSR_BT_WL_DB_FILE_ALTERNATE       (1)
#else
#define CSR_BT_WL_DB_FILE_ALTERNATE       (0)
#endif

#define CSR_BT_WL_DB_FILE_VERSION         ((CsrUint32) (0xFFFF0002 + CSR_BT_WL_DB_FILE_ALTERNATE))

#define CSR_BT_WL_DB_FILE_VERSION_SIZE    0

#if defined(USE_ATLAS7_PLATFORM) || defined(USE_MSM_PLATFORM)
#define CSR_BT_WL_DB_FILE_NAME            (((CsrUint8*)DATA_STORAGE_PATH "/" "wl_db.db"))
#elif defined(USE_MDM_PLATFORM)
#define CSR_BT_WL_DB_FILE_NAME            ((const CsrUtf8String *) (DEFAULT_DATA_STORAGE_PATH "wl_db.db"))
#elif defined(USE_IMX_PLATFORM)
#define CSR_BT_WL_DB_FILE_NAME            ((const CsrUtf8String *) ("/home/root/" "wl_db.db"))
#endif

/*********************************************************************************
  read/write a device record from/to the WL database.
********************************************************************************/
void csrBtWlDbGet(CsrBtWlDbRecord** head, CsrBtWlDbRecord** tail);
CsrInt32 csrBtWlDbGetList();
CsrBool csrBtWlDbCheckAddrExist(CsrBtTypedAddr *address);
void csrBtWlDbUpdate(CsrBtTypedAddr *address);
CsrResult CsrBtWlDbRemove(CsrBtTypedAddr *addr);
CsrBool csrBtWlDbCheckEmpty();
void csrBtWlDbprint(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail);
/*********************************************************************************
  operation of the WL Link structure.
********************************************************************************/
void csrBtWlInsertNode(CsrBtWlDbRecord *_data);
void csrBtWlAddNode(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail, CsrBtWlDbRecord* _data);
CsrBool csrBtWlDelNode(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail, CsrBtWlDbRecord* _data);
void csrBtWlNodeFree(CsrBtWlDbRecord* head, CsrBtWlDbRecord* tail);

#ifdef __cplusplus
}
#endif

#endif /* ndef __WL_DB_H */
