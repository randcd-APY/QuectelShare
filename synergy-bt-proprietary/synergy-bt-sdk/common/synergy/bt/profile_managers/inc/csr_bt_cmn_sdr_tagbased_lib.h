#ifndef CSR_BT_CMN_SDR_TAGBASED_LIB_H__
#define CSR_BT_CMN_SDR_TAGBASED_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009-2014 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sd_prim.h"

#include "csr_bt_cmn_linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDR_ENTRY_SIZE_TAG_LENGTH                   2
#define SDR_ENTRY_SIZE_TAG_ID                       2

#define SDR_ENTRY_SIZE_SERVICE_CSR_BT_RESULT        2
#define SDR_ENTRY_SIZE_SERVICE_CSR_BT_TASK          2
#define SDR_ENTRY_SIZE_SERVICE_CSR_BT_TASK_RESULT   2
#define SDR_ENTRY_SIZE_SERVICE_LOCAL_SERVER_CH      2
#define SDR_ENTRY_SIZE_SERVICE_EXTENDED_SEARCH      2
#define SDR_ENTRY_SIZE_SERVICE_HANDLE               4
#define SDR_ENTRY_SIZE_SERVICE_NOF_ATTRIBUTES       2
#define SDR_ENTRY_SIZE_SERVICE_UUID_TYPE            2
#define SDR_ENTRY_SIZE_SERVICE_UINT16               2
#define SDR_ENTRY_SIZE_SERVICE_UUID32               4
#define SDR_ENTRY_SIZE_SERVICE_UUID128             16

#define SDR_ENTRY_SIZE_ATTRIBUTE_UUID               2
#define SDR_ENTRY_SIZE_ATTRIBUTE_CSR_BT_RESULT      2
#define SDR_ENTRY_SIZE_ATTRIBUTE_CSR_BT_TASK        2
#define SDR_ENTRY_SIZE_ATTRIBUTE_CSR_BT_TASK_RESULT 2


#define SDR_ENTRY_INDEX_TAG_LENGTH                   (0)
#define SDR_ENTRY_INDEX_TAG_ID                       (SDR_ENTRY_INDEX_TAG_LENGTH                   + SDR_ENTRY_SIZE_TAG_LENGTH)

#define SDR_ENTRY_INDEX_SERVICE_CSR_BT_RESULT        (SDR_ENTRY_INDEX_TAG_ID                       + SDR_ENTRY_SIZE_TAG_ID)
#define SDR_ENTRY_INDEX_SERVICE_CSR_BT_TASK          (SDR_ENTRY_INDEX_SERVICE_CSR_BT_RESULT        + SDR_ENTRY_SIZE_SERVICE_CSR_BT_RESULT)
#define SDR_ENTRY_INDEX_SERVICE_CSR_BT_TASK_RESULT   (SDR_ENTRY_INDEX_SERVICE_CSR_BT_TASK          + SDR_ENTRY_SIZE_SERVICE_CSR_BT_TASK)
#define SDR_ENTRY_INDEX_SERVICE_LOCAL_SERVER_CH      (SDR_ENTRY_INDEX_SERVICE_CSR_BT_TASK_RESULT   + SDR_ENTRY_SIZE_SERVICE_CSR_BT_TASK_RESULT)
#define SDR_ENTRY_INDEX_SERVICE_EXTENDED_SEARCH      (SDR_ENTRY_INDEX_SERVICE_LOCAL_SERVER_CH      + SDR_ENTRY_SIZE_SERVICE_LOCAL_SERVER_CH)
#define SDR_ENTRY_INDEX_SERVICE_HANDLE               (SDR_ENTRY_INDEX_SERVICE_EXTENDED_SEARCH      + SDR_ENTRY_SIZE_SERVICE_EXTENDED_SEARCH)
#define SDR_ENTRY_INDEX_SERVICE_NOF_ATTRIBUTES       (SDR_ENTRY_INDEX_SERVICE_HANDLE               + SDR_ENTRY_SIZE_SERVICE_HANDLE)
#define SDR_ENTRY_INDEX_SERVICE_UUID_TYPE            (SDR_ENTRY_INDEX_SERVICE_NOF_ATTRIBUTES       + SDR_ENTRY_SIZE_SERVICE_NOF_ATTRIBUTES)
#define SDR_ENTRY_INDEX_SERVICE_UUID                 (SDR_ENTRY_INDEX_SERVICE_UUID_TYPE            + SDR_ENTRY_SIZE_SERVICE_UUID_TYPE)

#define SDR_ENTRY_INDEX_ATTRIBUTE_UUID               (SDR_ENTRY_INDEX_TAG_ID                       + SDR_ENTRY_SIZE_TAG_ID)
#define SDR_ENTRY_INDEX_ATTRIBUTE_CSR_BT_RESULT      (SDR_ENTRY_INDEX_ATTRIBUTE_UUID               + SDR_ENTRY_SIZE_ATTRIBUTE_UUID)
#define SDR_ENTRY_INDEX_ATTRIBUTE_CSR_BT_TASK        (SDR_ENTRY_INDEX_ATTRIBUTE_CSR_BT_RESULT      + SDR_ENTRY_SIZE_ATTRIBUTE_CSR_BT_RESULT)
#define SDR_ENTRY_INDEX_ATTRIBUTE_CSR_BT_TASK_RESULT (SDR_ENTRY_INDEX_ATTRIBUTE_CSR_BT_TASK        + SDR_ENTRY_SIZE_ATTRIBUTE_CSR_BT_TASK)
#define SDR_ENTRY_INDEX_ATTRIBUTE_DATA               (SDR_ENTRY_INDEX_ATTRIBUTE_CSR_BT_TASK_RESULT + SDR_ENTRY_SIZE_ATTRIBUTE_CSR_BT_TASK_RESULT)




/* Defines a invalid service-record handle. */
#define CMN_SDR_INVALID_SERVICE_HANDLE       (0)

#define SDR_SDC_PERFORM_EXT_RFC_SDC_SEARCH ((CsrUint16) (0x0001))

#define SDR_DUMMY_TASK         ((CsrUint8) (0x01))
#define SDR_SDC_SEARCH_SUCCESS ((CsrUint8) (0x00))
#define SDR_SDC_SEARCH_FAILED  ((CsrUint8) (0x01))


/* **************************************************** */
/* **************************************************** */
/* CLEANUP THE LINKED LIST */
/* **************************************************** */
/* **************************************************** */

    /* Removes all the entries in the linked list, where the status is not a success.
       Returns a new pointer to a linked list with the entries removed!
     */
    CmnCsrBtLinkedListStruct *CsrBtUtilSdrRemoveNonSuccessStatusEntries(CmnCsrBtLinkedListStruct *bll_p);

/* **************************************************** */
/* **************************************************** */
/* CREATE SDR LINKED LIST */
/* **************************************************** */
/* **************************************************** */

    /* Add a new service-record entry to the linked list pointed to by bll.
       If the bll pointer is NULL, a new list is started!
       The new entry is based on the linked list entry templateIndex
       in the linked list templateBll.
       The function returns the Synergy BT-linked-list
       to which the new entry was added!
       The newly created data area in the entry is initialised to NULL
       and zeroes,
       except the service handle UUID-type and service handle UUID,
       which are taken from the data area in the template entry.
       The attribute-structure is also copied to the data area
       in the new entry, but with zero-length attribute data!
       Only the same UUID-types for SDR's can be used.
       A mix of UUID-types (32 bit vs 128 bit for example)
       in the same linked list is illegal,
       and will result in a NULL pointer being returned!
    */
    CmnCsrBtLinkedListStruct *CsrBtUtilSdrCreateServiceHandleEntryFromTemplate(CmnCsrBtLinkedListStruct *bll,
                                                                        CsrUint16                *newBllIndex,
                                                                        CmnCsrBtLinkedListStruct *templateBll,
                                                                        CsrUintFast16                 templateIndex);


    /* Add a new service-handle entry to the linked list pointed to by bll.
       If the bll pointer is NULL, a new list is started!
       The function returns the Synergy BT-linked-list
       to which the new entry was added!
       The data area in the created entry is initialised to NULL and zeroes,
       except the service handle UUID-type and service handle UUID.
       The number of attributes is furthermore set to 0.
       Only the same UUID-types for SDR's can be used (choose 32 bit or 128 bit function).
       A mix of UUID-types (32 bit vs 128 bit for example)
       in the same linked list is illegal,
       and will result in a NULL pointer being returned!
    */
    CmnCsrBtLinkedListStruct *CsrBtUtilSdrCreateServiceHandleEntryFromUuid32(CmnCsrBtLinkedListStruct *bll,
                                                                      CsrBtUuid32                 serviceUuid32,
                                                                      CsrUint16                *newIndex);

    CmnCsrBtLinkedListStruct *CsrBtUtilSdrCreateServiceHandleEntryFromUuid128(CmnCsrBtLinkedListStruct *bll,
                                                                       CsrBtUuid128               *serviceUuid128,
                                                                       CsrUint16                *newIndex);

    /* Create and insert an SDR attribute, of the type attributeUuid
       and containing attribute data, placed in attributeData
       and attributeDataLen bytes long
     */
    CsrBool CsrBtUtilSdrCreateAndInsertAttribute(CmnCsrBtLinkedListStruct *bll,
                                          CsrUintFast16                 serviceHandleIndex,
                                          CsrUint16                 attributeUuid,
                                          CsrUint8                 *attributeData,
                                          CsrUint16                 attributeDataLen);

/* **************************************************** */
/* **************************************************** */
/* INSERT DATA IN LINKED LIST */
/* **************************************************** */
/* **************************************************** */

    /* Inserts the service handle, associated to a service UUID
       (choose 32 bit or 128 bit function).
       The service handle (serviceHandle) will be inserted
       at the first 'empty' spot in the blls structure,
       which matches the service UUID in serviceUuid.
       FALSE is returned if no spot is found, otherwise TRUE.
     */
    CsrBool CsrBtUtilSdrInsertServiceHandleAtUuid32 (CmnCsrBtLinkedListStruct *bll,
                                              CsrBtUuid32                 serviceUuid,
                                              CsrBtUuid32                 serviceHandle);

    CsrBool CsrBtUtilSdrInsertServiceHandleAtUuid128(CmnCsrBtLinkedListStruct *bll,
                                              CsrBtUuid128               *serviceUuid,
                                              CsrBtUuid32                 serviceHandle);


    /* Inserts the attribute data, at the position in the bll linked list,
       which corresponds serviceHandleIndex and attributeIndex.
       The results for the attribute-retrieval is also inserted
       in the bll linked list.
       FALSE is returned if the indexes do not exist, otherwise TRUE.
    */
    CsrBool CsrBtUtilSdrInsertAttributeDataAtAttributeUuid(CmnCsrBtLinkedListStruct *bll,
                                                    CsrUintFast16                 serviceHandleIndex,
                                                    CsrUintFast16                 attributeUuidIndex,
                                                    CsrUint16                 dataLen,
                                                    CsrUint8                  *data);


    /* Insert the local server channel,
       by the servicehandleIndex in the Synergy BT linked list pointed to by bll.
       FALSE is returned if the index does not exist, otherwise TRUE.
    */
    CsrBool CsrBtUtilSdrInsertLocalServerChannel(CmnCsrBtLinkedListStruct *bll,
                                          CsrUintFast16                 serviceHandleIndex,
                                          CsrUint16                  localServerChannel);

    CsrBool CsrBtUtilSdrInsertPerformExtendedSearch(CmnCsrBtLinkedListStruct *bll_p,
                                                    CsrUintFast16                serviceHandleIndex);

    /* Insert the length of the data into the Synergy BT linked list entry,
       pointed to by serviceHandlerindex.
       if the length is zero, the data is freed as well.
       If the length is anything else, the length variable is set,
       BUT NO MEMORY IS ALLOCATED !!!
       FALSE is returned if the index does not exist, otherwise TRUE.
    */
    CsrBool CsrBtUtilSdrInsertSdrLength(CmnCsrBtLinkedListStruct *bll,
                                 CsrUintFast16                 serviceHandleIndex,
                                 CsrUint16                 length);

/* **************************************************** */
/* **************************************************** */
/* RETRIEVE DATA FROM LINKED LIST */
/* **************************************************** */
/* **************************************************** */

    /* Retrieves the service handle, in the linked list
       starting from *bll at the entry with index serviceHandleIndex.
       The retrieved service handle is placed in the UuidType parameter.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetServiceHandle(CmnCsrBtLinkedListStruct *bll,
                                  CsrUintFast16                 serviceHandleIndex,
                                  CsrBtUuid32                *serviceHandle);

    /* Retrieves the UUID-type, in the linked list
       starting from *bll at the entry with index serviceHandleIndex.
       The retrieved UUID-type is placed in the UuidType parameter.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetServiceUuidType(CmnCsrBtLinkedListStruct *bll,
                                    CsrUintFast16                 serviceHandleIndex,
                                    CsrUint16                *serviceUuidType);



    /* Retrieves the UUID (choose 32 bit or 128 bit function), in the linked list
       starting from *bll at the entry with index serviceHandleIndex.
       The retrieved UUID is placed in the Uuid parameter.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetServiceUuid32 (CmnCsrBtLinkedListStruct *bll,
                                   CsrUintFast16                 serviceHandleIndex,
                                   CsrBtUuid32                *serviceUuid);

    CsrBool CsrBtUtilSdrGetServiceUuid128(CmnCsrBtLinkedListStruct *bll,
                                   CsrUintFast16                 serviceHandleIndex,
                                   CsrBtUuid128              **serviceUuid);

    /* Retrieves the UUID (choose 32 bit or 128 bit function), in the linked list
       starting from *bll at the entry with index serviceHandleIndex.
       The retrieved UUID is placed in the Uuid parameter.
       The functions also retrieves the corresponding result codes:
           The overall Synergy BT result code
           The Synergy BT task ID and the corresponding result code
           from the Synergy BT task.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetServiceUuid32AndResult (CmnCsrBtLinkedListStruct *bll,
                                            CsrUintFast16                 serviceHandleIndex,
                                            CsrBtUuid32                *serviceUuid,
                                            CsrUint16                *bchsResult,
                                            CsrUint16                *bchsTask,
                                            CsrUint16                *bchsTaskResult);

    CsrBool CsrBtUtilSdrGetServiceUuid128AndResult(CmnCsrBtLinkedListStruct *bll,
                                            CsrUintFast16                 serviceHandleIndex,
                                            CsrBtUuid128              **serviceUuid,
                                            CsrUint16                *bchsResult,
                                            CsrUint16                *bchsTask,
                                            CsrUint16                *bchsTaskResult);

    /* Retrieves the UUID (choose 32 bit or 128 bit function), in the linked list
       starting from *bll at the entry with index serviceHandleIndex.
       The retrieved UUID is placed in the Uuid parameter.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetServiceUuid32 (CmnCsrBtLinkedListStruct *bll,
                                   CsrUintFast16                 serviceHandleIndex,
                                   CsrBtUuid32                *serviceUuid);

    CsrBool CsrBtUtilSdrGetServiceUuid128(CmnCsrBtLinkedListStruct *bll,
                                   CsrUintFast16                 serviceHandleIndex,
                                   CsrBtUuid128              **serviceUuid);

    /* Retrieves the first index in the bll linked list,
       which matches the service UUID (choose 32 bit or 128 bit function).
       If the UUID does not exist, FALSE is returned, otherwise TRUE
    */
    CsrBool CsrBtUtilSdrGetIndexForServiceUuid32(CmnCsrBtLinkedListStruct *bll,
                                          CsrBtUuid32                 serviceUuid,
                                          CsrUint16                *serviceIndex);

    CsrBool CsrBtUtilSdrGetIndexForServiceUuid128(CmnCsrBtLinkedListStruct *bll,
                                           CsrBtUuid128               *serviceUuid,
                                           CsrUint16                *serviceIndex);

    /* Retrieves the first index in the bll linked list,
       which matches the service handle.
       If the service handle does not exist, FALSE is returned, otherwise TRUE
    */
    CsrBool CsrBtUtilSdrGetIndexForServiceHandle(CmnCsrBtLinkedListStruct *bll,
                                          CsrBtUuid32                 serviceHandle,
                                          CsrUint16                *serviceIndex);


    /* Returns the size of ampty attribute (one without any data!) */
#define CsrBtUtilSdrGetEmptyAttributeSize(_size_p) (*(_size_p) = SDR_ENTRY_INDEX_ATTRIBUTE_DATA)

    /* Returns the number of attributes in the entry
       with index serviceHandleIndex in the linked list starting from *bll.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetNofAttributes(CmnCsrBtLinkedListStruct *bll,
                                  CsrUintFast16                 serviceHandleIndex,
                                  CsrUintFast16                *nofAttributes);

    /* Returns the local server channel in the entry
       with index serviceHandleIndex in the linked list starting from *bll.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetLocalServerChannel(CmnCsrBtLinkedListStruct *bll,
                                       CsrUintFast16                 serviceHandleIndex,
                                       CsrUint16                 *localServerChannel);

    /* TRUE is returned if CsrBtCmSdcRfcExtendedSearchReqSend must be
       started, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrPerformExtendedSearch(CmnCsrBtLinkedListStruct *bll_p,
                                              CsrUintFast16                serviceHandleIndex);

    /* Returns the attribute UUID, with index attributeIndex in the entry
       with index serviceHandleIndex in the linked list starting from *bll.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    CsrBool CsrBtUtilSdrGetAttributeUuid(CmnCsrBtLinkedListStruct *bll,
                                  CsrUintFast16             serviceHandleIndex,
                                  CsrUintFast16             attributeUuidIndex,
                                  CsrUint16                *attributeUuid);

    /* Returns the pointer to the Attribute tag in the bll entry
       with index serviceHandleIndex and the attribute with index attributeIndex.
       The number of bytes from the start of the bll->data to the start of the attribute tag
       is returned in nofBytesToAttribute.
       Please Note: It is NOT the number of bytes until the attribute data
       in the attribute-tag which is returned!
    */
    CsrUint8 *CsrBtUtilSdrGetAttributePointer(CmnCsrBtLinkedListStruct *bll,
                                       CsrUintFast16                 serviceHandleIndex,
                                       CsrUintFast16                 attributeIndex,
                                       CsrUint16                *nofBytesToAttribute);

    /* extracts the service class ID list from the attributes in the Synergy BT
       linked list entry at index serviceHandleIndex.
       The service class ID list is returned as a pointer to a list of
       service class IDs' (CsrUint32), which contains nofServiceClassIds entries.
    */
    CsrBool CsrBtUtilSdrGetServiceClassIdList(CmnCsrBtLinkedListStruct *bll,
                                       CsrUintFast16                 serviceHandleIndex,
                                       CsrUint16                *nofServiceClassIds,
                                       CsrUint32               **serviceClassIdList);

    /* Extract the value (returned in a CsrUint32) from the attribute in
       the Synergy BT linked list, pointed to by serviceHandleIndex
       and the attribute UUID attributeUuid
     */
    CsrBool CsrBtUtilSdrGetUintAttributeDataValueFromAttributeUuid(CmnCsrBtLinkedListStruct *bll,
                                                            CsrUintFast16                 serviceHandleIndex,
                                                            CsrUint16                 attributeUuid,
                                                            CsrUint32                *returnValue);

    /* Extract the value (returned in a string pointer with length stringLen)
       from the attribute in the Synergy BT linked list, pointed to by serviceHandleIndex
       and the attribute UUID attributeUuid
     */
    CsrBool CsrBtUtilSdrGetStringAttributeFromAttributeUuid(CmnCsrBtLinkedListStruct *bll,
                                                     CsrUintFast16                 serviceHandleIndex,
                                                     CsrUint16                 attributeUuid,
                                                     CsrUint8                **string,
                                                     CsrUint16                *stringLen);

   /* Return a pointer in 'data' to the Data Element Sequence data, with the length of 'dataLen'.
    */
    CsrBool CsrBtUtilSdrGetDataElementSequenceDataAttributeFromAttributeUuid(CmnCsrBtLinkedListStruct *bll,
                                                                      CsrUintFast16                serviceHandleIndex,
                                                                      CsrUint16                attributeUuid,
                                                                      CsrUint8                 **data,
                                                                      CsrUint32                *dataLen);

    /* Return a pointer in 'data' to the unsigned integer data, with the length of 'dataLen'.
    */
    CsrBool CsrBtUtilSdrGetUintDataAttributeFromAttributeUuid(CmnCsrBtLinkedListStruct *bll_p,
                                                              CsrUintFast16                serviceHandleIndex,
                                                              CsrUint16                attributeUuid,
                                                              CsrUint8                 **data,
                                                              CsrUint32                *dataLen);

    /* Return a pointer in 'data' to the UUID,(a universally unique identifier) data, with
       the length of 'dataLen'.
    */
    CsrBool CsrBtUtilSdrGetUuidDataAttributeFromAttributeUuid(CmnCsrBtLinkedListStruct *bll_p,
                                                          CsrUintFast16        serviceHandleIndex,
                                                          CsrUint16        attributeUuid,
                                                          CsrUint8         **data,
                                                          CsrUint32        *dataLen);

#ifdef __cplusplus
}
#endif

#endif

