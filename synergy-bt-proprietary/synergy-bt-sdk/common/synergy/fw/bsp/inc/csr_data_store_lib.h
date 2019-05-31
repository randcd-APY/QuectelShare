#ifndef CSR_DATA_STORE_LIB_H__
#define CSR_DATA_STORE_LIB_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_data_store_prim.h"
#include "csr_msg_transport.h"
#include "csr_data_store_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_CREATE_REQ

   Description
        Creates a new data store

 * -------------------------------------------------------------------- */
CsrDataStoreCreateReq *CsrDataStoreCreateReq_struct(
    CsrSchedQid queueId,
    CsrUtf8String *storeName,
    CsrUint16 keyLength,
    CsrUint32 recordLength);
#define CsrDataStoreCreateReqSend(_queueId, _storeName, _keyLength, _recordLength){ \
        CsrDataStoreCreateReq *__msg; \
        __msg = CsrDataStoreCreateReq_struct(_queueId, _storeName, _keyLength, _recordLength); \
        CsrMsgTransport(CSR_DATA_STORE_IFACEQUEUE, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_OPEN_REQ

   Description
        Open am existing data store

 * -------------------------------------------------------------------- */
CsrDataStoreOpenReq *CsrDataStoreOpenReq_struct(
    CsrSchedQid queueId,
    CsrUtf8String *storeName);
#define CsrDataStoreOpenReqSend(_queueId, _storeName){ \
        CsrDataStoreOpenReq *__msg; \
        __msg = CsrDataStoreOpenReq_struct(_queueId, _storeName); \
        CsrMsgTransport(CSR_DATA_STORE_IFACEQUEUE, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_CLOSE_REQ

   Description
        Close an open data store handle

 * -------------------------------------------------------------------- */
CsrDataStoreCloseReq *CsrDataStoreCloseReq_struct(
    CsrSchedQid queueId,
    CsrDataStoreHandle storeHandle);
#define CsrDataStoreCloseReqSend(_queueId, _storeHandle){ \
        CsrDataStoreCloseReq *__msg; \
        __msg = CsrDataStoreCloseReq_struct(_queueId, _storeHandle); \
        CsrMsgTransport(CSR_DATA_STORE_IFACEQUEUE, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_DELETE_REQ

   Description
        Deletes an entire data store

 * -------------------------------------------------------------------- */
CsrDataStoreDeleteReq *CsrDataStoreDeleteReq_struct(
    CsrSchedQid queueId,
    CsrUtf8String *storeName);

#define CsrDataStoreDeleteReqSend(_queueId, _storeName){ \
        CsrDataStoreDeleteReq *__msg; \
        __msg = CsrDataStoreDeleteReq_struct(_queueId, _storeName); \
        CsrMsgTransport(CSR_DATA_STORE_IFACEQUEUE, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_RECORD_READ_REQ

   Description
        Reads a record from the specified data store

 * -------------------------------------------------------------------- */
CsrDataStoreRecordReadReq *CsrDataStoreRecordReadReq_struct(
    CsrSchedQid queueId,
    CsrDataStoreHandle storeHandle,
    CsrUint16 keyLength,
    CsrUint8 *key);
#define CsrDataStoreRecordReadReqSend(_queueId, _storeHandle, _keyLength, _key){ \
        CsrDataStoreRecordReadReq *__msg; \
        __msg = CsrDataStoreRecordReadReq_struct(_queueId, _storeHandle, _keyLength, _key); \
        CsrMsgTransport(CSR_DATA_STORE_IFACEQUEUE, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_RECORD_WRITE_REQ

   Description
        Writes a new record to the specified data store

 * -------------------------------------------------------------------- */
CsrDataStoreRecordWriteReq *CsrDataStoreRecordWriteReq_struct(
    CsrSchedQid queueId,
    CsrDataStoreHandle storeHandle,
    CsrUint16 keyLength,
    CsrUint8 *key,
    CsrUint32 recordLength,
    CsrUint8 *record);
#define CsrDataStoreRecordWriteReqSend(_queueId, _storeHandle, _keyLength, _key, _recordLength, _record){ \
        CsrDataStoreRecordWriteReq *__msg; \
        __msg = CsrDataStoreRecordWriteReq_struct(_queueId, _storeHandle, _keyLength, _key, _recordLength, _record); \
        CsrMsgTransport(CSR_DATA_STORE_IFACEQUEUE, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_RECORD_DELETE_REQ

   Description
        Deletes a specific record in a data store

 * -------------------------------------------------------------------- */
CsrDataStoreRecordDeleteReq *CsrDataStoreRecordDeleteReq_struct(
    CsrSchedQid queueId,
    CsrDataStoreHandle storeHandle,
    CsrUint16 keyLength,
    CsrUint8 *key);
#define CsrDataStoreRecordDeleteReqSend(_queueId, _storeHandle, _keyLength, _key){ \
        CsrDataStoreRecordDeleteReq *__msg; \
        __msg = CsrDataStoreRecordDeleteReq_struct(_queueId, _storeHandle, _keyLength, _key); \
        CsrMsgTransport(CSR_DATA_STORE_IFACEQUEUE, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_CREATE_CFM

   Description
        Confirmation with the result of a CSR_DATA_STORE_CREATE_REQ

 * -------------------------------------------------------------------- */
CsrDataStoreCreateCfm *CsrDataStoreCreateCfm_struct(
    CsrUtf8String *storeName,
    CsrResult result,
    CsrDataStoreHandle storeHandle);
#define CsrDataStoreCreateCfmSend(_queueId, _storeName, _result, _storeHandle){ \
        CsrDataStoreCreateCfm *__msg; \
        __msg = CsrDataStoreCreateCfm_struct(_storeName, _result, _storeHandle); \
        CsrMsgTransport(_queueId, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_OPEN_CFM

   Description
        Confirmation with the result of a CSR_DATA_STORE_OPEN_REQ

 * -------------------------------------------------------------------- */
CsrDataStoreOpenCfm *CsrDataStoreOpenCfm_struct(
    CsrUtf8String *storeName,
    CsrResult result,
    CsrDataStoreHandle storeHandle,
    CsrUint16 keyLength,
    CsrUint32 recordLength);
#define CsrDataStoreOpenCfmSend(_queueId, _storeName, _result, _storeHandle, _keyLength, _recordLength){ \
        CsrDataStoreOpenCfm *__msg; \
        __msg = CsrDataStoreOpenCfm_struct(_storeName, _result, _storeHandle, _keyLength, _recordLength); \
        CsrMsgTransport(_queueId, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_CLOSE_CFM

   Description
        Confirmation with the result of a CSR_DATA_STORE_CLOSE_REQ

 * -------------------------------------------------------------------- */
CsrDataStoreCloseCfm *CsrDataStoreCloseCfm_struct(
    CsrDataStoreHandle storeHandle);
#define CsrDataStoreCloseCfmSend(_queueId, _storeHandle){ \
        CsrDataStoreCloseCfm *__msg; \
        __msg = CsrDataStoreCloseCfm_struct(_storeHandle); \
        CsrMsgTransport(_queueId, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_DELETE_CFM

   Description
        Confirmation with the result of a CSR_DATA_STORE_DELETE_REQ

 * -------------------------------------------------------------------- */
CsrDataStoreDeleteCfm *CsrDataStoreDeleteCfm_struct(
    CsrUtf8String *storeName,
    CsrResult result);
#define CsrDataStoreDeleteCfmSend(_queueId, _storeName, _result){ \
        CsrDataStoreDeleteCfm *__msg; \
        __msg = CsrDataStoreDeleteCfm_struct(_storeName, _result); \
        CsrMsgTransport(_queueId, CSR_DATA_STORE_PRIM, __msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_RECORD_READ_CFM

   Description
        Confirmation with the result of a CSR_DATA_STORE_RECORD_READ_REQ

 * -------------------------------------------------------------------- */
CsrDataStoreRecordReadCfm *CsrDataStoreRecordReadCfm_struct(
    CsrDataStoreHandle storeHandle,
    CsrResult result,
    CsrUint16 keyLength,
    CsrUint8 *key,
    CsrUint32 recordLength,
    CsrUint8 *record);
#define CsrDataStoreRecordReadCfmSend(_queueId, _storeHandle, _result, _keyLength, _key, _recordLength, _record){ \
        CsrDataStoreRecordReadCfm *__msg; \
        __msg = CsrDataStoreRecordReadCfm_struct(_storeHandle, _result, _keyLength, _key, _recordLength, _record); \
        CsrMsgTransport(_queueId, CSR_DATA_STORE_PRIM, __msg); \
}
/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_RECORD_WRITE_CFM

   Description
        Confirmation with the result of a CSR_DATA_STORE_RECORD_WRITE_REQ

 * -------------------------------------------------------------------- */
CsrDataStoreRecordWriteCfm *CsrDataStoreRecordWriteCfm_struct(
    CsrDataStoreHandle storeHandle,
    CsrResult result,
    CsrUint16 keyLength,
    CsrUint8 *key);
#define CsrDataStoreRecordWriteCfmSend(_queueId, _storeHandle, _result, _keyLength, _key){ \
        CsrDataStoreRecordWriteCfm *__msg; \
        __msg = CsrDataStoreRecordWriteCfm_struct(_storeHandle, _result, _keyLength, _key); \
        CsrMsgTransport(_queueId, CSR_DATA_STORE_PRIM, __msg); \
}
/* --------------------------------------------------------------------
   Name
       CSR_DATA_STORE_RECORD_DELETE_CFM

   Description
        Confirmation with the result of a CSR_DATA_STORE_RECORD_DELETE_REQ

 * -------------------------------------------------------------------- */
CsrDataStoreRecordDeleteCfm *CsrDataStoreRecordDeleteCfm_struct(
    CsrDataStoreHandle storeHandle,
    CsrResult result,
    CsrUint16 keyLength,
    CsrUint8 *key);
#define CsrDataStoreRecordDeleteCfmSend(_queueId, _storeHandle, _result, _keyLength, _key){ \
        CsrDataStoreRecordDeleteCfm *__msg; \
        __msg = CsrDataStoreRecordDeleteCfm_struct(_storeHandle, _result, _keyLength, _key); \
        CsrMsgTransport(_queueId, CSR_DATA_STORE_PRIM, __msg); \
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDataStoreFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR DATA_STORE upstream messages
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_DATA_STORE_PRIM,
 *      message:      The message received from CSR DATA_STORE
 *----------------------------------------------------------------------------*/
void CsrDataStoreFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrDataStoreFreeDownstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR DATA_STORE downstream messages
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_DATA_STORE_PRIM,
 *      message:      The message received from an external task
 *----------------------------------------------------------------------------*/
void CsrDataStoreFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif
