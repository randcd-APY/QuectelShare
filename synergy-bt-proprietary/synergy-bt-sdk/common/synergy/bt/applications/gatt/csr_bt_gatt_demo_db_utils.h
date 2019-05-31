#ifndef CSR_BT_GATT_APP_DB_UTILS_H__
#define CSR_BT_GATT_APP_DB_UTILS_H__

/******************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*  profile manager include files */
#include "csr_types.h"
#include "csr_bt_sc_demo_app.h"
#include "csr_ui_lib.h"
#include  "csr_bt_gatt_prim.h"
#include "csr_list.h"
#include "csr_bt_gatt_demo_defines.h"

#define CSR_BT_GATT_PRIM_NOT_RECOGNIZED             0x0000
#define CSR_BT_GATT_PRIM_HANDLED                    0x0001
#define CSR_BT_GATT_SERVICES_CFM_ERROR              0x0002
#define CSR_BT_GATT_CHARAC_CFM_ERROR                0x0004
#define CSR_BT_GATT_CHARAC_DESCRIPTOR_CFM_ERROR     0x0008
#define CSR_BT_GATT_READ_CFM_ERROR                  0x0010
#define CSR_BT_GATT_READ_CFM_SUCCESS                0x1000
#define CSR_BT_GATT_SERVICES_CFM_SUCCESS            0x2000

#define CSR_BT_GATT_LOW_BYTE   0
#define CSR_BT_GATT_HIGH_BYTE  1

typedef struct CsrBtGattAppDbCharacDescrElementTag
{
    struct CsrBtGattAppDbCharacDescrElementTag *next; /*must be first */
    struct CsrBtGattAppDbCharacDescrElementTag *prev; /*must be second */
    CsrBtUuid                                   uuid; /* Characteristic Descriptor UUID */
    CsrBtGattHandle                             descriptorHandle; /* The handle of the Characteristic Descriptor declaration */
    CsrBtGattHandle                             valueHandle; /* Characteristic Value Handle */
    CsrUint16                                   valueLength;
    CsrUint8                                   *value;

} CsrBtGattAppDbCharacDescrElement;

typedef struct CsrBtGattAppDbCharacElementTag
{
    struct CsrBtGattAppDbCharacElementTag  *next; /*must be first */
    struct CsrBtGattAppDbCharacElementTag  *prev; /*must be second */
    CsrBtGattHandle                         declarationHandle; /* Handle for the characteristic declaration*/
    CsrBtGattPropertiesBits                 property; /* Characteristic Property */
    CsrBtUuid                               uuid; /* Characteristic UUID */
    CsrBtGattHandle                         valueHandle; /* Characteristic Value Handle */
    CsrUint16                               valueLength;
    CsrUint8                               *value;
    CsrCmnList_t                            descrList;
} CsrBtGattAppDbCharacElement;

typedef struct CsrBtGattAppDbPrimElementTag
{
    struct CsrBtGattAppDbPrimElementTag    *next; /* must be first */
    struct CsrBtGattAppDbPrimElementTag    *prev; /* must be second */
    CsrUint16                               id; /* Needed as several prim services can have same UUID */
    CsrBtUuid                               uuid;
    CsrUint16                               startHandle;
    CsrUint16                               endHandle;
    CsrCmnList_t                            characList;
} CsrBtGattAppDbPrimServiceElement;

typedef struct
{
    /*database values and handles (local copy) */
    CsrCmnList_t                        dbPrimServices;
    CsrUint16                           dbPrimServicesCount;
    CsrBtGattAppDbPrimServiceElement   *currentPrimService; /* used for deciding position in database */
    CsrBtGattAppDbCharacElement        *currentCharac; /* used for deciding position in database */
    CsrBtGattAppDbCharacDescrElement   *currentDescr; /* used for deciding position in database */
    CsrBool                             traversingDb;
} CsrBtGattDbElement;

void CsrBtGattAppInitDbPrimList(CsrCmnListElm_t *elem);
void CsrBtGattAppFreeDbPrimList(CsrCmnListElm_t *elem);
void CsrBtGattAppInitDbCharacList(CsrCmnListElm_t *elem);
void CsrBtGattAppFreeDbCharacList(CsrCmnListElm_t *elem);
void CsrBtGattAppInitDbCharacDescrList(CsrCmnListElm_t *elem);
void CsrBtGattAppFreeDbCharacDescrList(CsrCmnListElm_t *elem);

CsrBool CsrBtGattAppTestValueHandle(CsrCmnListElm_t *elem, void *value);
CsrBool CsrBtGattAppTestValueHandleInterval(CsrCmnListElm_t *elem, void *value);
CsrBtGattAppDbCharacElement *CsrBtGattAppDbFindCharacByValueHandle(CsrCmnList_t *cmnList,
                                                                   void *value);

CsrBool CsrBtGattAppDbFindPrimServiceByUuid(CsrCmnListElm_t *elem, void *value);
CsrBool CsrBtGattAppDbFindPrimServiceByHandle(CsrCmnListElm_t *elem, void *value);
CsrBool CsrBtGattAppDbFindCharacByUuid(CsrCmnListElm_t *elem, void *value);
CsrBool CsrBtGattAppDbFindCharacDescrByUuid(CsrCmnListElm_t *elem, void *value);
CsrBool CsrBtGattAppDbFindNextCharacWithProperty(CsrBtGattAppDbPrimServiceElement **pElem,
                                                 CsrBtGattAppDbCharacElement **cElem,
                                                 CsrBtGattPropertiesBits property);
CsrBool CsrBtGattAppDbFindNextCharacDescriptor(CsrBtGattAppDbCharacElement **cElem,
                                               CsrBtGattAppDbCharacDescrElement **dElem);

CsrUint16 CsrBtGattDiscoverService(CsrBtGattDbElement *dbElement,
                                   void* recvMsgP,
                                   CsrBtGattId gattId,
                                   CsrBtConnId btConnId);

void CsrUint8ArrayToHex(CsrUint8* value, CsrCharString *str, CsrUint16 length);

/* Prim service helper macros */
#define CSR_BT_GATT_APP_DB_ADD_PRIM_SERVICE(_List) \
    (CsrBtGattAppDbPrimServiceElement *)CsrCmnListElementAddLast(&(_List), \
                                                                 sizeof(CsrBtGattAppDbPrimServiceElement))

#define CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_UUID(_List,_uuid) \
    ((CsrBtGattAppDbPrimServiceElement *)CsrCmnListSearch(&(_List), \
                                                        CsrBtGattAppDbFindPrimServiceByUuid, \
                                                        (void *)(_uuid)))

#define CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_HANDLE(_List,_handle) \
    ((CsrBtGattAppDbPrimServiceElement *)CsrCmnListSearch(&(_List), \
                                                        CsrBtGattAppDbFindPrimServiceByHandle, \
                                                        (void *)(_handle)))

#define CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(_List) \
    (CsrBtGattAppDbPrimServiceElement *)CsrCmnListElementGetFirst(&(_List))

#define CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_ID(_List,_id) \
    ((CsrBtGattAppDbPrimServiceElement *)CsrCmnListSearch(&(_List), \
                                                        CsrBtGattAppDbFindPrimServiceById, \
                                                        (void *)(_id)))

#define CSR_BT_GATT_APP_DB_REMOVE_PRIM_SERVICE(_List,_Elem) \
    (CsrCmnListElementRemove((CsrCmnList_t *)&(_List), \
                             (CsrCmnListElm_t *)(_Elem)))

/* Charac descriptors helper macros */
#define CSR_BT_GATT_APP_DB_ADD_CHARAC_DESCR(_List) \
    (CsrBtGattAppDbCharacDescrElement *)CsrCmnListElementAddLast(&(_List), \
                                                                 sizeof(CsrBtGattAppDbCharacDescrElement))

#define CSR_BT_GATT_APP_DB_FIND_CHARAC_DESCR_BY_UUID(_List,_uuid) \
    ((CsrBtGattAppDbCharacDescrElement *)CsrCmnListSearch(&(_List), \
                                             CsrBtGattAppDbFindCharacDescrByUuid, \
                                             (void *)(_uuid)))

#define CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC_DESCR(_List) \
    (CsrBtGattAppDbCharacDescrElement *)CsrCmnListElementGetFirst(&(_List))

#define CSR_BT_GATT_APP_DB_REMOVE_CHARAC_DESCR(_List,_Elem) \
    (CsrCmnListElementRemove((CsrCmnList_t *)&(_List), \
                             (CsrCmnListElm_t *)(_Elem)))

/* Charac helper macro */
#define CSR_BT_GATT_APP_DB_ADD_CHARAC(_List) \
    (CsrBtGattAppDbCharacElement *)CsrCmnListElementAddLast(&(_List), \
                                                                 sizeof(CsrBtGattAppDbCharacElement))

#define CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_UUID(_List,_uuid) \
    ((CsrBtGattAppDbCharacElement *)CsrCmnListSearch(&(_List), \
                                             CsrBtGattAppDbFindCharacByUuid, \
                                             (void *)(_uuid)))

#define CSR_BT_GATT_APP_DB_FIND_CHARAC_BY_VALUE_HANDLE(_List, _Handle) \
    (CsrBtGattAppDbCharacElement *)CsrBtGattAppDbFindCharacByValueHandle(&(_List), (void *)(_Handle))

#define CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(_List) \
    (CsrBtGattAppDbCharacElement *)CsrCmnListElementGetFirst(&(_List))

#define CSR_BT_GATT_DB_ITERATE(_List,_func,_dataPtr) \
    (CsrCmnListIterate(&(_List), (_func), (void *)(_dataPtr)))

#define CSR_BT_GATT_APP_DB_REMOVE_CHARAC(_List,_Elem) \
    (CsrCmnListElementRemove((CsrCmnList_t *)&(_List), \
                             (CsrCmnListElm_t *)(_Elem)))

#ifdef __cplusplus
}
#endif

#endif
