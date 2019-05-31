#ifndef CSR_BT_SD_PRIVATE_LIB_H__
#define CSR_BT_SD_PRIVATE_LIB_H__

/******************************************************************************
Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sd_private_prim.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_addr.h"
#include "csr_bt_uuids.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdRegisterAvailableServiceReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SD_REGISTER_AVAILABLE_SERVICE_REQ message
 *      to the SD message queue.
 *
 *  PARAMETERS
 *      service:  The service a given client wants to search/connect to
 *----------------------------------------------------------------------------*/
void CsrBtSdRegisterAvailableServiceReqSend(CsrBtUuid32 service);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdUpdateDeviceReqSend
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *      details: Use the SD_UPDATE_DEVICE_[...] flags specified in csr_bt_sd_private_prim.h
 *----------------------------------------------------------------------------*/
#define CsrBtSdUpdateBondReqSend(_addr,_cod,_details,_addressType,_servChange,_clConf) { \
        CsrBtSdUpdateDeviceReq *msg__;                                  \
        msg__ = (CsrBtSdUpdateDeviceReq*)CsrPmemZalloc(sizeof(CsrBtSdUpdateDeviceReq)); \
        msg__->type = CSR_BT_SD_UPDATE_DEVICE_REQ;                      \
        msg__->addr = _addr;                                            \
        msg__->cod = _cod;                                              \
        msg__->details = _details;                                      \
        msg__->addressType = _addressType;                              \
        msg__->serviceChangeHandle = _servChange;                       \
        msg__->clientConfigValue = _clConf;                             \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdUpdateDeBondReqSend(_addr,_addressType) { \
        CsrBtSdUpdateDeviceReq *msg__;                                  \
        msg__ = (CsrBtSdUpdateDeviceReq*)CsrPmemZalloc(sizeof(CsrBtSdUpdateDeviceReq)); \
        msg__->type = CSR_BT_SD_UPDATE_DEVICE_REQ;                      \
        msg__->addr = _addr;                                            \
        msg__->details = SD_UPDATE_DEVICE_DEBOND;                       \
        msg__->addressType = _addressType;                              \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdUpdateServiceChangedReqSend(_addr,_addressType,_serviceChangeHandle) { \
        CsrBtSdUpdateDeviceReq *msg__;                                  \
        msg__ = (CsrBtSdUpdateDeviceReq*)CsrPmemZalloc(sizeof(CsrBtSdUpdateDeviceReq)); \
        msg__->type = CSR_BT_SD_UPDATE_DEVICE_REQ;                      \
        msg__->addr = _addr;                                            \
        msg__->details = SD_UPDATE_SERVICE_CHANGED;                     \
        msg__->addressType = _addressType;                              \
        msg__->serviceChangeHandle = _serviceChangeHandle;              \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdUpdateClientConfigValueReqSend(_addr,_addressType,_clientConfigValue) { \
        CsrBtSdUpdateDeviceReq *msg__;                                  \
        msg__ = (CsrBtSdUpdateDeviceReq*)CsrPmemZalloc(sizeof(CsrBtSdUpdateDeviceReq)); \
        msg__->type = CSR_BT_SD_UPDATE_DEVICE_REQ;                      \
        msg__->addr = _addr;                                            \
        msg__->details = SD_UPDATE_CLIENT_CONFIG;                       \
        msg__->addressType = _addressType;                              \
        msg__->clientConfigValue = _clientConfigValue;                  \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdUpdateChangedDbRangeReqSend(_addr,_addressType,_startHandle,_endHandle) { \
        CsrBtSdUpdateDeviceReq *msg__;                                  \
        msg__ = (CsrBtSdUpdateDeviceReq*)CsrPmemZalloc(sizeof(CsrBtSdUpdateDeviceReq)); \
        msg__->type = CSR_BT_SD_UPDATE_DEVICE_REQ;                      \
        msg__->addr = _addr;                                            \
        msg__->details = SD_UPDATE_CHANGED_DB_RANGE;                    \
        msg__->addressType = _addressType;                              \
        msg__->startHandle = _startHandle;                              \
        msg__->endHandle = _endHandle;                                  \
        CsrBtSdMsgTransport(msg__);}


/*************************** Search GATT services *****************************
 *  This API searches requested GATT services on peer GATT server.
 *  SD creates an LE ACL connection with peer device, if it does not exist already.
 *  It performs GATT service discovery procedure to find requested services.
 *  This is followed by Characteristic and Descriptor discovery on found services.
 *  It tries to read characteristic and descriptor values as well. It may raise
 *  security level if required to read characteristic or descriptor values.
 *  SD registers the requested application/profile as a subscriber to database of
 *  requested device.
 *  Any change in GATT database of the requested device due to service/database
 *  search by other application/profile or service change on peer server are also
 *  reported to this application/profile.
 *
 *  Sending CSR_BT_SD_DISCOVER_GATT_DATABASE_SUBSCRIBE as uuidCount causes it
 *  to behave as CsrBtSdSubscribeGattDatabaseReqSend().
 *  Sending CSR_BT_SD_DISCOVER_GATT_DATABASE_COMPLETE as uuidCount causes it
 *  to behave as CsrBtSdDiscoverGattDatabaseReqSend()
 *
 *  Parameters:
 *      pHandle         -   Task handle of application/profile
 *      addr            -   Address of peer device
 *      uuid            -   List of 16-bit GATT services UUID
 *      uuidCount       -   Number of GATT services UUID in the list
 *****************************************************************************/
#define CsrBtSdDiscoverGattServicesReqSend(_pHandle,                    \
                                           _addr,                       \
                                           _uuid,                       \
                                           _uuidCount)                  \
    do                                                                  \
    {                                                                   \
        CsrBtSdDiscoverGattDatabaseReq *msg__;                          \
        msg__ = (CsrBtSdDiscoverGattDatabaseReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_SD_DISCOVER_GATT_DATABASE_REQ;             \
        msg__->phandle = _pHandle;                                      \
        msg__->uuid = _uuid;                                            \
        msg__->uuidCount = _uuidCount;                                  \
        CsrBtAddrCopy(&(msg__->addr), &(_addr));                        \
        CsrBtSdMsgTransport(msg__);                                     \
    } while(0)


/*************************** Search GATT database *****************************
 *  This API searches complete GATT database on peer GATT server.
 *  SD creates an LE ACL connection with peer device, if it does not exist already.
 *  It performs GATT service discovery procedure to find requested services.
 *  This is followed by Characteristic and Descriptor discovery on found services.
 *  It tries to read characteristic and descriptor values as well. It may raise
 *  security level if required to read characteristic or descriptor values.
 *  SD registers the requested application/profile as a subscriber to database of
 *  requested device.
 *  Any change in GATT database of the requested device due to service/database
 *  search by other application/profile or service change on peer server are also
 *  reported to this application/profile.
 *
 *  Parameters:
 *      pHandle         -   Task handle of application/profile
 *      addr            -   Address of peer device
 *****************************************************************************/
#define CsrBtSdDiscoverGattDatabaseReqSend(_pHandle, _addr)             \
    CsrBtSdDiscoverGattServicesReqSend(_pHandle,                        \
                                       _addr,                           \
                                       NULL,                            \
                                       CSR_BT_SD_DISCOVER_GATT_DATABASE_COMPLETE)


/*************************** Subscribe GATT database *****************************
 *  This API adds requester application/profile as subscriber to peer GATT server.
 *  This does not initiate GATT service search.
 *  Any change in GATT database of the requested device due to service/database
 *  search by other application/profile or service change on peer server are also
 *  reported to this application/profile.
 *
 *  This will primarily used by SC for bonded devices on Bonding or Initialisation.
 *
 *  Parameters:
 *      pHandle         -   Task handle of application/profile
 *      addr            -   Address of peer device
 *****************************************************************************/
#define CsrBtSdSubscribeGattDatabaseReqSend(_pHandle, _addr)            \
    CsrBtSdDiscoverGattDatabaseReqSend(_pHandle,                        \
                                       _addr,                           \
                                       NULL,                            \
                                       CSR_BT_SD_DISCOVER_GATT_DATABASE_SUBSCRIBE)


/***************************** Remove database ********************************
 *  This API removes subscription of an Application/profile for the requested
 *  peer device.
 *  Application/Profiles should un-subscribe if device is not of interest anymore.
 *  SD releases the cached database when there are no more subscriber for a peer
 *  device.
 *  This API can also be used to cancel an ongoing search procedure. However
 *  search procedure will be stopped only if there were no other subscriber for
 *  the peer device.
 *
 *  Parameters:
 *      pHandle         -   Task handle of application/profile
 *      addr            -   Address of peer device
 *****************************************************************************/
#define CsrBtSdRemoveGattDatabaseReqSend(_pHandle, _addr)               \
    do                                                                  \
    {                                                                   \
        CsrBtSdRemoveGattDatabaseReq *msg__;                            \
        msg__ = (CsrBtSdRemoveGattDatabaseReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_SD_REMOVE_GATT_DATABASE_REQ;               \
        msg__->phandle = _pHandle;                                      \
        CsrBtAddrCopy(&(msg__->addr), &(_addr));                        \
        CsrBtSdMsgTransport(msg__);                                     \
    } while(0)

/********************* SD GATT database utility functions *********************/
/* Checks if a service contains requested handle */
CsrBool CsrBtSdGattDbHandleInService(CsrCmnListElm_t *service,
                                     void *pHandle);

/* Checks if a service contains the requested UUID */
CsrBool CsrBtSdGattDbCompareServiceUuid(CsrCmnListElm_t *service,
                                        const void *pUuid);

/* Checks if a service has the requested 16-bit UUID */
CsrBool CsrBtSdGattDbCompareServiceUuid16(CsrCmnListElm_t *service,
                                          CsrBtUuid16 uuid16);

/* Checks if a characteristic has the requested UUID */
CsrBool CsrBtSdGattDbCompareCharacUuid(CsrCmnListElm_t *charac,
                                       const void *pUuid);

/* Checks if a characteristic has the requested 16-bit UUID */
CsrBool CsrBtSdGattDbCompareCharacUuid16(CsrCmnListElm_t *charac,
                                         CsrBtUuid16 uuid16);

/* Checks if a characteristic descriptor has the requested UUID */
CsrBool CsrBtSdGattDbCompareDescUuid(CsrCmnListElm_t *desc,
                                     const void *pUuid);

/* Checks if a characteristic descriptor has the requested 16-bit UUID */
CsrBool CsrBtSdGattDbCompareDescUuid16(CsrCmnListElm_t *desc,
                                       CsrBtUuid16 uuid16);

/* Checks if included service start with same handle */
CsrBool CsrBtSdGattDbCompareIncludedServiceHandle(CsrCmnListElm_t *inclService,
                                                  const void *pHandle);

/* Checks if a service starting with requested handle is included in requested service */
CsrBool CsrBtSdGattDbIsServiceHandleIncluded(CsrCmnListElm_t *service,
                                             void *pHandle);

/* Finds characteristic which contains requested value handle from service list */
CsrBtSdGattDbCharacElement *CsrBtSdGattDbFindServiceCharacByValueHandle(CsrCmnList_t *serviceList,
                                                                        void *valueHandle);


/**************************** Service helper macros **************************/
/* Extract first service from service list */
#define CSR_BT_SD_GATT_DB_GET_FIRST_SERVICE(_serviceList)                           \
    ((CsrBtSdGattDbServiceElement *) CsrCmnListElementGetFirst(&(_serviceList)))

/* Finds service which has contains the requested handle */
#define CSR_BT_SD_GATT_DB_FIND_SERVICE_BY_HANDLE(_serviceList, _handle)                                     \
    ((CsrBtSdGattDbServiceElement *) CsrCmnListSearchOffsetUint16(&(_serviceList),                          \
                                                                  CsrOffsetOf(CsrBtSdGattDbServiceElement,  \
                                                                              startHandle),                 \
                                                                  (_handle)))

/* Finds service from service list by UUID */
#define CSR_BT_SD_GATT_DB_FIND_SERVICE_BY_UUID(_serviceList, _pUuid)                    \
    ((CsrBtSdGattDbServiceElement *) CsrCmnListSearch(&(_serviceList),                  \
                                                      (CsrCmnListSearchFunc_t) CsrBtSdGattDbCompareServiceUuid, \
                                                      (void *) (_pUuid)))

/* Finds service from service list by 16-bit UUID */
#define CSR_BT_SD_GATT_DB_FIND_SERVICE_BY_UUID16(_serviceList, _uuid16)                 \
    ((CsrBtSdGattDbServiceElement *) CsrCmnListSearch(&(_serviceList),                  \
                                                      (CsrCmnListSearchFunc_t) CsrBtSdGattDbCompareServiceUuid16, \
                                                      (void *) (_uuid16)))


/*********************** Included service helper macros ***********************/
/* Extracts first include service element from a include service list */
#define CSR_BT_SD_GATT_DB_GET_FIRST_INC_SERVICE(_inclSrvList)                           \
    ((CsrBtSdGattDbInclSrvElement *) CsrCmnListElementGetFirst(&(_inclSrvList)))

/* Finds include service element which points to a included service starting at requested handle */
#define CSR_BT_SD_GATT_DB_FIND_INCLUDED_SERVICE_BY_HANDLE(_inclSrvList, _pHandle)       \
    ((CsrBtSdGattDbInclSrvElement *) CsrCmnListSearch(&(_inclSrvList),                  \
                                                      (CsrCmnListSearchFunc_t) CsrBtSdGattDbCompareIncludedServiceHandle, \
                                                      (_pHandle)))

/* Finds the service which includes the service starting at requested handle */
#define CSR_BT_SD_GATT_DB_FIND_PARENT_SERVICE(_serviceList, _handle)                    \
    ((CsrBtSdGattDbServiceElement *) CsrCmnListSearch((_serviceList),                   \
                                                      (CsrCmnListSearchFunc_t) CsrBtSdGattDbIsServiceHandleIncluded, \
                                                      &(_handle)))


/****************** Characteristic helper macro *******************************/
/* Extracts first characteristic from characteristic list */
#define CSR_BT_SD_GATT_DB_GET_FIRST_CHARAC(_characList)                                 \
    ((CsrBtSdGattDbCharacElement *) CsrCmnListElementGetFirst(&(_characList)))

/* Finds characteristic from characteristic list by value handle */
#define CSR_BT_SD_GATT_DB_FIND_CHARAC_BY_VALUE_HANDLE(_characList, _valueHandle)        \
    ((CsrBtSdGattDbCharacElement *) CsrCmnListSearchOffsetUint16(&(_characList),        \
                                                                 CsrOffsetOf(CsrBtSdGattDbCharacElement, \
                                                                             valueHandle), \
                                                                 (_valueHandle)))

/* Finds characteristic from characteristic list by UUID */
#define CSR_BT_SD_GATT_DB_FIND_CHARAC_BY_UUID(_characList, _pUuid)                      \
    ((CsrBtSdGattDbCharacElement *) CsrCmnListSearch(&(_characList),                    \
                                                     (CsrCmnListSearchFunc_t) CsrBtSdGattDbCompareCharacUuid, \
                                                     (void *) (_pUuid)))

/* Finds characteristic from characteristic list by 16-bit UUID */
#define CSR_BT_SD_GATT_DB_FIND_CHARAC_BY_UUID16(_characList, _uuid16)                   \
    ((CsrBtSdGattDbCharacElement *) CsrCmnListSearch(&(_characList),                    \
                                                     (CsrCmnListSearchFunc_t) CsrBtSdGattDbCompareCharacUuid16, \
                                                     (void *) (_uuid16)))


/******************** Characteristic descriptors helper macros ****************/
/* Extracts first characteristic descriptor from descriptor list */
#define CSR_BT_SD_GATT_DB_GET_FIRST_CHARAC_DESC(_descList)                              \
    ((CsrBtSdGattDbCharacDescElement *) CsrCmnListElementGetFirst(&(_descList)))

/* Finds characteristic descriptor from descriptor list by UUID */
#define CSR_BT_SD_GATT_DB_FIND_CHARAC_DESC_BY_UUID(_descList, _pUuid)                   \
    ((CsrBtSdGattDbCharacDescElement *) CsrCmnListSearch(&(_descList),                  \
                                                         (CsrCmnListSearchFunc_t) CsrBtSdGattDbCompareDescUuid, \
                                                         (void *) (_pUuid)))

/* Finds characteristic descriptor from descriptor list 16-bit UUID */
#define CSR_BT_SD_GATT_DB_FIND_CHARAC_DESC_BY_UUID16(_descList, _uuid16)                \
    ((CsrBtSdGattDbCharacDescElement *) CsrCmnListSearch(&(_descList),                  \
                                                         (CsrCmnListSearchFunc_t) CsrBtSdGattDbCompareDescUuid16, \
                                                         (void *) (_uuid16)))

#ifdef CSR_BT_LE_ENABLE
/***************** Functions for enabling notifications ***********************/
/* Enables notification/indication of the characteristic, if applicable */
void CsrBtSdGattEnableCharacNotification(CsrBtSdGattDbCharacElement *charac,
                                         CsrBtSdGattConnIdentifier *gattConnId);

/* Enables notification/indication of characteristics in the service, wherever applicable */
void CsrBtSdGattEnableServiceNotifications(CsrBtSdGattDbServiceElement *service,
                                           CsrBtSdGattConnIdentifier *gattConnId);

/* Enables all possible notifications/indications for all the found services */
#define CSR_BT_SD_GATT_ENABLE_ALL_NOTIFICATIONS(_serviceList, _gattConnId)                  \
    (CsrCmnListIterate((_serviceList),                                                      \
                       (CsrCmnListIterateFunc_t *) CsrBtSdGattEnableServiceNotifications,   \
                       (void *) (_gattConnId)))
#endif /* CSR_BT_LE_ENABLE */

/* Function to deallocate contents of SD private upstream messages */
void CsrBtSdPrivateFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_SD_PRIVATE_LIB_H__ */

