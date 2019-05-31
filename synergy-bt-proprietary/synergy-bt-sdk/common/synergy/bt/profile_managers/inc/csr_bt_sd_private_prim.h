#ifndef CSR_BT_SD_PRIVATE_PRIM_H__
#define CSR_BT_SD_PRIVATE_PRIM_H__

/******************************************************************************
Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_sd_prim.h"
#include "csr_bt_addr.h"
#include "csr_bt_gatt_prim.h"
#include "csr_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtSdPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */


/* Downstream */
#define CSR_BT_SD_REGISTER_AVAILABLE_SERVICE_REQ    ((CsrBtSdPrim) (0x0001 + CSR_BT_SD_PRIM_DOWNSTREAM_HIGHEST))
#define CSR_BT_SD_UPDATE_DEVICE_REQ                 ((CsrBtSdPrim) (0x0002 + CSR_BT_SD_PRIM_DOWNSTREAM_HIGHEST))
#define CSR_BT_SD_DISCOVER_GATT_DATABASE_REQ        ((CsrBtSdPrim) (0x0003 + CSR_BT_SD_PRIM_DOWNSTREAM_HIGHEST))
#define CSR_BT_SD_REMOVE_GATT_DATABASE_REQ          ((CsrBtSdPrim) (0x0004 + CSR_BT_SD_PRIM_DOWNSTREAM_HIGHEST))
#define CSR_BT_SD_ALL_DOWNSTREAM_COUNT              (CSR_BT_SD_REMOVE_GATT_DATABASE_REQ + 1)

/* Upstream */
#define CSR_BT_SD_DISCOVER_GATT_DATABASE_CFM        ((CsrBtSdPrim) (0x0001 + CSR_BT_SD_PRIM_UPSTREAM_HIGHEST))
#define CSR_BT_SD_REMOVE_GATT_DATABASE_CFM          ((CsrBtSdPrim) (0x0002 + CSR_BT_SD_PRIM_UPSTREAM_HIGHEST))
#define CSR_BT_SD_ALL_UPSTREAM_COUNT                (CSR_BT_SD_REMOVE_GATT_DATABASE_CFM + 1)

#define CSR_BT_SD_PRIVATE_PRIM (CSR_BT_SD_PRIM)

/* ---------- End of Primitives ----------*/

typedef struct
{
    CsrBtSdPrim          type;
    CsrBtUuid32          service;
} CsrBtSdRegisterAvailableServiceReq;

#define SD_UPDATE_DEVICE_IGNORE     (0x00000000) /* Excludes other flags */
#define SD_UPDATE_DEVICE_BOND       (0x00000001) /* Excludes SD_UPDATE_DEVICE_DEBOND */
#define SD_UPDATE_DEVICE_DEBOND     (0x00000002) /* Excludes SD_UPDATE_DEVICE_BOND */
#define SD_UPDATE_DEVICE_TRUSTED    (0x00000004) /* Only use together with SD_UPDATE_DEVICE_BOND */
#define SD_UPDATE_SERVICE_CHANGED   (0x00000008) /* Only use by GATT */   
#define SD_UPDATE_CLIENT_CONFIG     (0x00000010) /* Only use by GATT */   
#define SD_UPDATE_CHANGED_DB_RANGE  (0x00000020) /* Only use by GATT */   

typedef struct
{
    CsrBtSdPrim          type;
    CsrBtDeviceAddr      addr;
    CsrBtClassOfDevice   cod;
    CsrUint32            details;
    CsrBtAddressType     addressType;
    CsrUint32            serviceChangeHandle;
    CsrUint16            clientConfigValue;
    CsrUint16            startHandle;
    CsrUint16            endHandle;
} CsrBtSdUpdateDeviceReq;

#define CSR_BT_SD_DISCOVER_GATT_DATABASE_SUBSCRIBE                      ((CsrUint8) 0)
#define CSR_BT_SD_DISCOVER_GATT_DATABASE_COMPLETE                       ((CsrUint8) 0xFF)

typedef struct
{
    CsrBtSdPrim         type;
    CsrSchedQid         phandle;
    CsrBtTypedAddr      addr;
    CsrBtUuid          *uuid;
    CsrUint8            uuidCount;
} CsrBtSdDiscoverGattDatabaseReq;

typedef struct
{
    CsrBtSdPrim         type;
    CsrSchedQid         phandle;
    CsrBtTypedAddr      addr;
} CsrBtSdRemoveGattDatabaseReq;

/* This is sent to all subscribers every time there is change in cached database */
typedef struct
{
    CsrBtSdPrim         type;
    CsrBtTypedAddr      addr;
    CsrCmnList_t       *serviceList;    /* Read-only. Owned by SD. */
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtSdDiscoverGattDatabaseCfm;

/* This message confirms removal of subscription for peer server's database */
typedef struct
{
    CsrBtSdPrim         type;
    CsrBtTypedAddr      addr;
} CsrBtSdRemoveGattDatabaseCfm;


typedef CsrUint8 CsrBtSdGattServiceType;
#define CSR_BT_SD_GATT_SERVICE_TYPE_PRIMARY         ((CsrBtSdGattServiceType) 0)
#define CSR_BT_SD_GATT_SERVICE_TYPE_SECONDARY       ((CsrBtSdGattServiceType) 1)

/* Characteristic descriptor element */
typedef struct CsrBtSdGattDbCharacDescElementTag
{
    struct CsrBtSdGattDbCharacDescElementTag   *next;               /* Must be first */
    struct CsrBtSdGattDbCharacDescElementTag   *prev;               /* Must be second */
    CsrBtUuid                                   uuid;               /* Characteristic Descriptor UUID */
    CsrBtGattHandle                             characHandle;       /* Characteristic Value Handle */
    CsrBtGattHandle                             descriptorHandle;   /* The handle of the Characteristic Descriptor declaration */
    CsrUint16                                   descLength;         /* Descriptor value length */
    CsrUint8                                   *desc;               /* Descriptor value */
} CsrBtSdGattDbCharacDescElement;

/* Characteristic element */
typedef struct CsrBtSdGattDbCharacElementTag
{
    struct CsrBtSdGattDbCharacElementTag   *next;               /* Must be first */
    struct CsrBtSdGattDbCharacElementTag   *prev;               /* Must be second */
    CsrBtUuid                               uuid;               /* Characteristic UUID */
    CsrBtGattHandle                         valueHandle;        /* Characteristic Value Handle */
    CsrBtGattHandle                         declarationHandle;  /* Handle for the characteristic declaration*/
    CsrBtGattPropertiesBits                 property;           /* Characteristic Property */
    CsrUint16                               valueLength;        /* Characteristic value length */
    CsrUint8                               *value;              /* Characteristic value */
    CsrCmnList_t                            descrList;          /* List of descriptors */
} CsrBtSdGattDbCharacElement;

/* Service element */
typedef struct CsrBtSdGattDbServiceElementTag
{
    struct CsrBtSdGattDbServiceElementTag  *next;           /* Must be first */
    struct CsrBtSdGattDbServiceElementTag  *prev;           /* Must be second */
    CsrBtUuid                               uuid;           /* Service UUID */
    CsrBtGattHandle                         startHandle;    /* Service start handle */
    CsrCmnList_t                            characList;     /* List of characteristics */
    CsrCmnList_t                            inclSrvList;    /* List of included services */
    CsrBtGattHandle                         endHandle;      /* End handle of service */
    CsrBtSdGattServiceType                  serviceType;    /* Service type - Primary/Secondary */
} CsrBtSdGattDbServiceElement;

/* Included service element */
typedef struct CsrBtSdGattDbInclSrvElementTag
{
    struct CsrBtSdGattDbInclSrvElementTag  *next;              /* Must be first */
    struct CsrBtSdGattDbInclSrvElementTag  *prev;              /* Must be second */
    CsrBtSdGattDbServiceElement            *includedService;   /* Included service */
} CsrBtSdGattDbInclSrvElement;

/* Composite GATT and connection identifier */
typedef struct
{
    CsrBtGattId gattId;
    CsrBtConnId btConnId;
} CsrBtSdGattConnIdentifier;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_SD_PRIVATE_PRIM_H__ */

