#ifndef CSR_DATA_STORE_PRIM_H__
#define CSR_DATA_STORE_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrDataStorePrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrDataStorePrim;

typedef void *CsrDataStoreHandle;
#define CSR_DATA_STORE_INVALID_HANDLE NULL

/* Error codes define for the CREATE procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_DATA_STORE_CREATE_FAILURE                   ((CsrResult) 1)
#define CSR_DATA_STORE_CREATE_NOT_ALLOWED               ((CsrResult) 2)

/* Error codes define for the OPEN procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_DATA_STORE_OPEN_FAILURE                     ((CsrResult) 1)
#define CSR_DATA_STORE_OPEN_NOT_EXIST                   ((CsrResult) 2)

/* Error codes define for the DELETE procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_DATA_STORE_DELETE_FAILURE                   ((CsrResult) 1)

/* Error codes define for the RECORD_READ procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_DATA_STORE_RECORD_READ_FAILURE              ((CsrResult) 1)
#define CSR_DATA_STORE_RECORD_READ_NOT_EXIST            ((CsrResult) 2)
#define CSR_DATA_STORE_RECORD_READ_INVALID_HANDLE       ((CsrResult) 3)

/* Error codes define for the RECORD_WRITE procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_DATA_STORE_RECORD_WRITE_FAILURE             ((CsrResult) 1)
#define CSR_DATA_STORE_RECORD_WRITE_INVALID_HANDLE      ((CsrResult) 2)

/* Error codes define for the RECORD_DELETE procedure, if operation succeeded the result is CSR_RESULT_SUCCESS, all other values are reserved for future usage */
#define CSR_DATA_STORE_RECORD_DELETE_FAILURE            ((CsrResult) 1)
#define CSR_DATA_STORE_RECORD_DELETEE_INVALID_HANDLE    ((CsrResult) 2)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST                               (0x0000)

#define CSR_DATA_STORE_CREATE_REQ                       ((CsrDataStorePrim) (0x0000 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DATA_STORE_OPEN_REQ                         ((CsrDataStorePrim) (0x0001 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DATA_STORE_CLOSE_REQ                        ((CsrDataStorePrim) (0x0002 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DATA_STORE_DELETE_REQ                       ((CsrDataStorePrim) (0x0003 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DATA_STORE_RECORD_READ_REQ                  ((CsrDataStorePrim) (0x0004 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DATA_STORE_RECORD_WRITE_REQ                 ((CsrDataStorePrim) (0x0005 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DATA_STORE_RECORD_DELETE_REQ                ((CsrDataStorePrim) (0x0006 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST))

#define CSR_DATA_STORE_PRIM_DOWNSTREAM_HIGHEST                              (0x0006 + CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST                                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_DATA_STORE_CREATE_CFM                       ((CsrDataStorePrim) (0x0000 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST))
#define CSR_DATA_STORE_OPEN_CFM                         ((CsrDataStorePrim) (0x0001 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST))
#define CSR_DATA_STORE_CLOSE_CFM                        ((CsrDataStorePrim) (0x0002 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST))
#define CSR_DATA_STORE_DELETE_CFM                       ((CsrDataStorePrim) (0x0003 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST))
#define CSR_DATA_STORE_RECORD_READ_CFM                  ((CsrDataStorePrim) (0x0004 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST))
#define CSR_DATA_STORE_RECORD_WRITE_CFM                 ((CsrDataStorePrim) (0x0005 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST))
#define CSR_DATA_STORE_RECORD_DELETE_CFM                ((CsrDataStorePrim) (0x0006 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST))

#define CSR_DATA_STORE_PRIM_UPSTREAM_HIGHEST                                (0x0006 + CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST)

#define CSR_DATA_STORE_PRIM_DOWNSTREAM_COUNT            (CSR_DATA_STORE_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_DATA_STORE_PRIM_DOWNSTREAM_LOWEST)
#define CSR_DATA_STORE_PRIM_UPSTREAM_COUNT              (CSR_DATA_STORE_PRIM_UPSTREAM_HIGHEST + 1 - CSR_DATA_STORE_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrDataStorePrim type;
    CsrSchedQid      queueId;
    CsrUtf8String   *storeName;
    CsrUint16        keyLength;
    CsrUint32        recordLength;
} CsrDataStoreCreateReq;

typedef struct
{
    CsrDataStorePrim   type;
    CsrUtf8String     *storeName;
    CsrResult          result;
    CsrDataStoreHandle storeHandle;
} CsrDataStoreCreateCfm;

typedef struct
{
    CsrDataStorePrim type;
    CsrSchedQid      queueId;
    CsrUtf8String   *storeName;
} CsrDataStoreOpenReq;

typedef struct
{
    CsrDataStorePrim   type;
    CsrUtf8String     *storeName;
    CsrResult          result;
    CsrDataStoreHandle storeHandle;
    CsrUint16          keyLength;
    CsrUint32          recordLength;
} CsrDataStoreOpenCfm;

typedef struct
{
    CsrDataStorePrim   type;
    CsrSchedQid        queueId;
    CsrDataStoreHandle storeHandle;
} CsrDataStoreCloseReq;

typedef struct
{
    CsrDataStorePrim   type;
    CsrDataStoreHandle storeHandle;
} CsrDataStoreCloseCfm;

typedef struct
{
    CsrDataStorePrim type;
    CsrSchedQid      queueId;
    CsrUtf8String   *storeName;
} CsrDataStoreDeleteReq;

typedef struct
{
    CsrDataStorePrim type;
    CsrUtf8String   *storeName;
    CsrResult        result;
} CsrDataStoreDeleteCfm;


typedef struct
{
    CsrDataStorePrim   type;
    CsrSchedQid        queueId;
    CsrDataStoreHandle storeHandle;
    CsrUint16          keyLength;
    CsrUint8          *key;
} CsrDataStoreRecordReadReq;

typedef struct
{
    CsrDataStorePrim   type;
    CsrDataStoreHandle storeHandle;
    CsrResult          result;
    CsrUint16          keyLength;
    CsrUint8          *key;
    CsrUint32          recordLength;
    CsrUint8          *record;
} CsrDataStoreRecordReadCfm;

typedef struct
{
    CsrDataStorePrim   type;
    CsrSchedQid        queueId;
    CsrDataStoreHandle storeHandle;
    CsrUint16          keyLength;
    CsrUint8          *key;
    CsrUint32          recordLength;
    CsrUint8          *record;
} CsrDataStoreRecordWriteReq;

typedef struct
{
    CsrDataStorePrim   type;
    CsrDataStoreHandle storeHandle;
    CsrResult          result;
    CsrUint16          keyLength;
    CsrUint8          *key;
} CsrDataStoreRecordWriteCfm;

typedef struct
{
    CsrDataStorePrim   type;
    CsrSchedQid        queueId;
    CsrDataStoreHandle storeHandle;
    CsrUint16          keyLength;
    CsrUint8          *key;
} CsrDataStoreRecordDeleteReq;

typedef struct
{
    CsrDataStorePrim   type;
    CsrDataStoreHandle storeHandle;
    CsrResult          result;
    CsrUint16          keyLength;
    CsrUint8          *key;
} CsrDataStoreRecordDeleteCfm;

#ifdef __cplusplus
}
#endif

#endif
