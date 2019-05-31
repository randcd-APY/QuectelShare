/******************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_gatt_demo_db_utils.h"
#include "csr_bt_gatt_prim.h"
#include "csr_bt_gatt_lib.h"
#include "csr_list.h"
#include "csr_pmem.h"
#include "csr_bt_gatt_demo_defines.h"

static void getDescriptors(CsrBtGattId gattId,
                           CsrBtConnId btConnId,
                           CsrBtGattDbElement* dbElement)
{
    if (CsrBtGattAppDbFindNextCharacWithProperty(&dbElement->currentPrimService,
                                                 &dbElement->currentCharac,
                                                 0))
    {
        CsrUint16 startHandle = 0;
        CsrUint16 endHandle = 0xFFFF;
        startHandle = dbElement->currentCharac->declarationHandle + 1;
        if (dbElement->currentCharac->next)
        {
            /*still not the last charac in prim service so get endhandle from next charac*/
            endHandle = ((CsrBtGattAppDbCharacElement*) dbElement->currentCharac->next)->declarationHandle
                        - 1;
        }
        else
        {
            /*get endhandle from prim service */
            if (dbElement->currentPrimService)
            {
                endHandle = dbElement->currentPrimService->endHandle;
            }
            /* move to next prim service*/
        }
        CsrBtGattDiscoverAllCharacDescriptorsReqSend(gattId,
                                                     btConnId,
                                                     startHandle,
                                                     endHandle);
    }
    else
    {
        /*we are done traversing the DB*/
        dbElement->traversingDb = FALSE;
    }
}

static void getDescriptorValue(CsrBtGattId gattId,
                          CsrBtConnId btConnId,
                          CsrBtGattDbElement *dbElement)
{
    CsrBtGattAppDbCharacDescrElement* desc = dbElement->currentDescr;

    if (desc)
    {
        CsrBtUuid16 uuidDesc;
        uuidDesc = (desc->uuid.uuid[1] << 8) + desc->uuid.uuid[0];
        switch (uuidDesc)
        {
            case CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC:
            {
                CsrBtGattReadExtendedPropertiesReqSend(gattId,
                                                       btConnId,
                                                       desc->descriptorHandle);
                break;
            }
            case CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC:
            {
                CsrBtGattReadUserDescriptionReqSend(gattId,
                                                    btConnId,
                                                    desc->descriptorHandle);
                break;
            }
            case CSR_BT_GATT_UUID_EXTERNAL_REPORT_REFERENCE_DESC:
                /* Fall through */
            case CSR_BT_GATT_UUID_REPORT_REFERENCE_DESC:
                /* fall through */
            default:
            {
                CsrBtGattReadProfileDefinedDescriptorReqSend(gattId,
                                                             btConnId,
                                                             desc->descriptorHandle,
                                                             0);
                break;
            }
        }
    }
    else
    {
        getDescriptors(gattId, btConnId, dbElement);
    }
}

void CsrUint8ArrayToHex(CsrUint8* value, CsrCharString *str, CsrUint16 length)
{
    CsrUint8 count;
    CsrUint8 index = 0;
    CsrUint8 currentValue, temp;
    if (str == NULL)
    {
        str = (CsrCharString*) CsrPmemZalloc(sizeof(CsrUint8) * length * 2 + 1);
    }

    for (count = 0; count < length; count++)
    {
        temp = value[count];
        currentValue = (CsrUint8) (temp & 0x0F);

        str[index + 1] = (char) (
                         currentValue > 9 ?
                                            currentValue + 55 :
                                            currentValue + '0');

        temp >>= 4;
        currentValue = (CsrUint8) (temp & 0x0F);

        str[index] = (char) (
                     currentValue > 9 ?
                                        currentValue + 55 :
                                        currentValue + '0');

        index += 2;
        if (index > length * 2)
        {
            break;
        }
    }
}

void CsrBtGattAppInitDbCharacDescrList(CsrCmnListElm_t *elem)
{
    /* Initialize a CsrBtGattDbCharacDescrElement. This function is called every
     * time a new entry is made on the queue list */
    CsrBtGattAppDbCharacDescrElement *dElem =
                    (CsrBtGattAppDbCharacDescrElement *) elem;
    dElem->descriptorHandle = CSR_BT_GATT_ATTR_HANDLE_INVALID;
    dElem->value = NULL;
    dElem->valueLength = 0;
}

void CsrBtGattAppFreeDbCharacDescrList(CsrCmnListElm_t *elem)
{
    /* CsrPmemFree local pointers in the CsrBtGattAppDbCharacDescrElement. 
     * This function is called every time a element is removed from the
     * Characteristics list */
    CsrBtGattAppDbCharacDescrElement *dElem =
                    (CsrBtGattAppDbCharacDescrElement *) elem;

    CsrPmemFree(dElem->value);
}
void CsrBtGattAppInitDbCharacList(CsrCmnListElm_t *elem)
{
    /* Initialize a CsrBtGattDbCharacElement. This function is called every
     * time a new entry is made on the queue list */
    CsrBtGattAppDbCharacElement *cElem = (CsrBtGattAppDbCharacElement *) elem;
    cElem->valueHandle = CSR_BT_GATT_ATTR_HANDLE_INVALID;
    cElem->value = NULL;
    cElem->valueLength = 0;
    CsrCmnListInit(&cElem->descrList,
                   0,
                   CsrBtGattAppInitDbCharacDescrList,
                   CsrBtGattAppFreeDbCharacDescrList);
}

void CsrBtGattAppFreeDbCharacList(CsrCmnListElm_t *elem)
{
    /* CsrPmemFree local pointers in the CsrBtGattAppDbCharacElement. 
     * This function is called every time a element is removed from the
     * Characteristics list */
    CsrBtGattAppDbCharacElement *cElem = (CsrBtGattAppDbCharacElement *) elem;
    CsrCmnListDeinit(&(cElem->descrList));
    CsrPmemFree(cElem->value);
}

void CsrBtGattAppInitDbPrimList(CsrCmnListElm_t *elem)
{
    /* Initialize a CsrBtGattDbPrimElement. This function is called every
     * time a new entry is made on the queue list */
    CsrBtGattAppDbPrimServiceElement *pElem =
                    (CsrBtGattAppDbPrimServiceElement *) elem;
    pElem->startHandle = CSR_BT_GATT_ATTR_HANDLE_INVALID;
    pElem->endHandle = CSR_BT_GATT_ATTR_HANDLE_INVALID;
    CsrCmnListInit(&pElem->characList,
                   0,
                   CsrBtGattAppInitDbCharacList,
                   CsrBtGattAppFreeDbCharacList);

}

void CsrBtGattAppFreeDbPrimList(CsrCmnListElm_t *elem)
{
    /* CsrPmemFree local pointers in the CsrBtGattAppDbPrimElement.  
     * This function is called every time a element is removed from the
     * Primary Service List list */
    CsrBtGattAppDbPrimServiceElement *pElem =
                    (CsrBtGattAppDbPrimServiceElement *) elem;
    CsrCmnListDeinit(&(pElem->characList));
}

CsrBool CsrBtGattAppDbFindPrimServiceByUuid(CsrCmnListElm_t *elem, void *value)
{ /* Return TRUE if uuid matches*/
    CsrBtUuid uuid = *(CsrBtUuid *) value;
    CsrUint8 i;
    CsrBtGattAppDbPrimServiceElement *element =
                    (CsrBtGattAppDbPrimServiceElement *) elem;
    if (element->uuid.length != uuid.length)
    {
        return FALSE;
    }
    for (i = 0; i < element->uuid.length; i++)
    {
        if (element->uuid.uuid[i] != uuid.uuid[i])
        {
            return FALSE;
        }
    }

    /*the uuid is the same */
    return TRUE;
}

CsrBool CsrBtGattAppDbFindPrimServiceByHandle(CsrCmnListElm_t *elem, void *value)
{ /* Return TRUE if service contains the handle */
    CsrBtGattHandle handle = *(CsrBtGattHandle *) value;
    CsrBtGattAppDbPrimServiceElement *elemService =
                    (CsrBtGattAppDbPrimServiceElement *) elem;
    if (handle >= elemService->startHandle &&
                    handle <= elemService->endHandle)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

CsrBool CsrBtGattAppDbFindCharacByUuid(CsrCmnListElm_t *elem, void *value)
{ /* Return TRUE if uuid matches*/
    CsrBtUuid uuid = *(CsrBtUuid *) value;
    CsrUint8 i;
    CsrBtGattAppDbCharacElement *element = (CsrBtGattAppDbCharacElement *) elem;
    if (element->uuid.length != uuid.length)
    {
        return FALSE;
    }
    for (i = 0; i < element->uuid.length; i++)
    {
        if (element->uuid.uuid[i] != uuid.uuid[i])
        {
            return FALSE;
        }
    }
    /*the uuid is the same */
    return TRUE;
}
CsrBool CsrBtGattAppTestValueHandleInterval(CsrCmnListElm_t *elem, void *value)
{
    CsrBtGattAppDbPrimServiceElement *element =
                    (CsrBtGattAppDbPrimServiceElement *) elem;
    CsrBtGattHandle valueHandle = *(CsrBtGattHandle *) value;

    if ((element->startHandle < valueHandle)
        && (element->endHandle > valueHandle))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
CsrBool CsrBtGattAppTestValueHandle(CsrCmnListElm_t *elem, void *value)
{
    CsrBtGattAppDbCharacElement *element = (CsrBtGattAppDbCharacElement *) elem;
    CsrBtGattHandle valueHandle = *(CsrBtGattHandle *) value;

    if (element->valueHandle == valueHandle)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
CsrBtGattAppDbCharacElement *CsrBtGattAppDbFindCharacByValueHandle(CsrCmnList_t *cmnList,
                                                                   void *value)
{ /* Return TRUE if uuid matches*/
    CsrBtGattHandle valueHandle = *(CsrBtGattHandle *) value;

    CsrBtGattAppDbPrimServiceElement *element =
                    (CsrBtGattAppDbPrimServiceElement *) CsrCmnListSearch(cmnList,
                                                                          CsrBtGattAppTestValueHandleInterval,
                                                                          &valueHandle);

    return (CsrBtGattAppDbCharacElement *) CsrCmnListSearch(&(element->characList),
                                                            CsrBtGattAppTestValueHandle,
                                                            &valueHandle);

}

CsrBool CsrBtGattAppDbFindCharacDescrByUuid(CsrCmnListElm_t *elem, void *value)
{ /* Return TRUE if uuid matches*/

    CsrBtUuid uuid = *(CsrBtUuid *) value;
    CsrUint8 i;
    CsrBtGattAppDbCharacDescrElement *element =
                    (CsrBtGattAppDbCharacDescrElement *) elem;
    if (element->uuid.length != uuid.length)
    {
        return FALSE;
    }
    for (i = 0; i < element->uuid.length; i++)
    {
        if (element->uuid.uuid[i] != uuid.uuid[i])
        {
            return FALSE;
        }
    }
    /*the uuid is the same */
    return TRUE;
}
/* Find next readable characteristic element 
 * pElem    : Current primary Service Element
 * cElem    : Current characteristic Element, if NULL it will start with first charac in pElem.
 * property : Property that a charac value needs to obey (can be a mask of multiple properties)
 * The elements will get updated when using this function so use temp pointers if you do not want to get them updated
 */
CsrBool CsrBtGattAppDbFindNextCharacWithProperty(CsrBtGattAppDbPrimServiceElement **pElem,
                                                 CsrBtGattAppDbCharacElement **cElem,
                                                 CsrBtGattPropertiesBits property)
{
    CsrBtGattAppDbPrimServiceElement *primElem =
                    (CsrBtGattAppDbPrimServiceElement *) *pElem;
    CsrBtGattAppDbCharacElement *characElem =
                    (CsrBtGattAppDbCharacElement *) *cElem;
    if (characElem == NULL)
    {
        /* */
        characElem = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(primElem->characList);
    }
    else
    {
        /* move to next charac element in list */
        characElem = characElem->next;

    }
    while (primElem != NULL )
    {
        while (characElem != NULL )
        {
            if (((characElem->property & property) == property)
                || property == 0 /* passthrough if property is 0*/)
            {
                /* found a usable element  update pointers and return */
                *pElem = primElem;
                *cElem = characElem;
                return TRUE;
            }
            else
            {
                /* move to next charac element in list */
                characElem = characElem->next;
            }

        }
        /* no more readable charac elements in this */
        primElem = primElem->next;
        /* reset CharacElem - it will get updated as the first thing in the next loop */
        if (primElem != NULL)
        {
            characElem = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(primElem->characList);
        }
    }
    /* could not find any element that mached the property in all of the elements */
    *pElem = NULL;
    *cElem = NULL;
    return FALSE;
}

/* Find next readable characteristic element 
 * cElem    : Current characteristic Element
 * dElem    : Current characteristic descriptor Element, if NULL it will start with first descr in cElem.
 * The elements will get updated when using this function so use temp pointers if you do not want to get them updated
 */
CsrBool CsrBtGattAppDbFindNextCharacDescriptor(CsrBtGattAppDbCharacElement **cElem,
                                               CsrBtGattAppDbCharacDescrElement **dElem)
{
    CsrBtGattAppDbCharacDescrElement *descrElem =
                    (CsrBtGattAppDbCharacDescrElement *) *dElem;
    CsrBtGattAppDbCharacElement *characElem =
                    (CsrBtGattAppDbCharacElement *) *cElem;
    if (descrElem == NULL)
    {
        /* */
        descrElem = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC_DESCR(characElem->descrList);
    }
    while (characElem != NULL )
    {
        while (descrElem->next != NULL )
        {
            /* move to next charac element in list */
            descrElem = descrElem->next;
            *cElem = characElem;
            *dElem = descrElem;
            return TRUE;
        }
        /* no more readable charac elements in this */
        characElem = characElem->next;
        /* reset descrElem - it will get updated as the first thing in the next loop */
        if (characElem != NULL)
        {
            descrElem = CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC_DESCR(characElem->descrList);
        }
    }
    /* could not find any element that mached the property in all of the elements */
    *dElem = NULL;
    *cElem = NULL;
    return FALSE;
}

CsrUint16 CsrBtGattDiscoverService(CsrBtGattDbElement *dbElement,
                                   void* recvMsgP,
                                   CsrBtGattId gattId,
                                   CsrBtConnId btConnId)
{
    CsrPrim *primType;

    CsrInt32 retVal = CSR_BT_GATT_PRIM_HANDLED;

    primType = (CsrPrim *) recvMsgP;
    switch (*primType)
    {
        case CSR_BT_GATT_DISCOVER_SERVICES_IND:
        {
            CsrBtGattDiscoverServicesInd *ind;
            ind = (CsrBtGattDiscoverServicesInd*) recvMsgP;
            /*check to see if we already have this prim service*/
            if (!CSR_BT_GATT_APP_DB_FIND_PRIM_SERVICE_BY_HANDLE(dbElement->dbPrimServices,
                                                              &ind->startHandle))
            {
                CsrBtGattAppDbPrimServiceElement *elem =
                                CSR_BT_GATT_APP_DB_ADD_PRIM_SERVICE(dbElement->dbPrimServices);
                elem->uuid = ind->uuid;
                elem->startHandle = ind->startHandle;
                elem->endHandle = ind->endHandle;
                elem->id = dbElement->dbPrimServicesCount++;
            }
            break;
        }
        case CSR_BT_GATT_DISCOVER_SERVICES_CFM:
        {
            CsrBtGattDiscoverServicesCfm *cfm;
            cfm = (CsrBtGattDiscoverServicesCfm*) recvMsgP;

            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                /* Start with first primary service element and find all characteristics */
                dbElement->currentPrimService =
                                CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(dbElement->dbPrimServices);

                /* [QTI] Fix KW issue#831696 through adding the check "dbElement->currentPrimService". */
                if (dbElement->currentPrimService)
                {
                    CsrBtGattDiscoverAllCharacOfAServiceReqSend(gattId,
                                                                btConnId,
                                                                dbElement->currentPrimService->startHandle,
                                                                dbElement->currentPrimService->endHandle);
                }
                
                retVal |= CSR_BT_GATT_SERVICES_CFM_SUCCESS;
            }
            else
            {
                retVal |= CSR_BT_GATT_SERVICES_CFM_ERROR;
            }
            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_IND:
        {
            CsrBtGattDiscoverCharacInd *ind;
            CsrBtGattAppDbCharacElement *cElem =
                            CSR_BT_GATT_APP_DB_ADD_CHARAC(dbElement->currentPrimService->characList);
            ind = (CsrBtGattDiscoverCharacInd*) recvMsgP;
            cElem->valueHandle = ind->valueHandle;
            cElem->declarationHandle = ind->declarationHandle;
            cElem->property = ind->property;
            cElem->uuid = ind->uuid;
            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_CFM:
        {
            CsrBtGattDiscoverCharacCfm *cfm;
            cfm = (CsrBtGattDiscoverCharacCfm*) recvMsgP;
            if ((cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS
                 && cfm->resultSupplier == CSR_BT_SUPPLIER_GATT)
                ||
                /* Returning NOT FOUND if no element found in recived interval,
                 but should be handles as a cfm success recieved after an indication of found charac */
                (cfm->resultCode == ATT_RESULT_ATTR_NOT_FOUND
                 && cfm->resultSupplier == CSR_BT_SUPPLIER_ATT))
            {
                /*move on to next prim service (if any and find all of its charac*/
                dbElement->currentPrimService =
                                dbElement->currentPrimService->next;
                if (dbElement->currentPrimService)
                {
                    CsrBtGattDiscoverAllCharacOfAServiceReqSend(gattId,
                                                                btConnId,
                                                                dbElement->currentPrimService->startHandle,
                                                                dbElement->currentPrimService->endHandle);
                }
                else
                {
                    /* we go back to the beginning and start over */
                    dbElement->currentPrimService =
                                    CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(dbElement->dbPrimServices);

                    dbElement->currentCharac = NULL;

                    if (CsrBtGattAppDbFindNextCharacWithProperty(&dbElement->currentPrimService,
                                                                 &dbElement->currentCharac,
                                                                 CSR_BT_GATT_CHARAC_PROPERTIES_READ))
                    {
                        CsrBtGattReadReqSend(gattId,
                                             btConnId,
                                             dbElement->currentCharac->valueHandle,
                                             0);
                    }
                    else
                    {
                        /* Done Reading!
                         * we need to reset the "current" pointers and start working on the descriptors*/
                        dbElement->currentPrimService =
                                        CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(dbElement->dbPrimServices);
                        /*start from the first charac */
                        dbElement->currentCharac = NULL;
                        getDescriptors(gattId, btConnId, dbElement);
                    }
                }
            }
            else
            {
                retVal |= CSR_BT_GATT_CHARAC_CFM_ERROR;
            }

            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_IND:
        {
            /* add new element to currentCharac descrList */
            CsrBtGattDiscoverCharacDescriptorsInd *ind =
                            (CsrBtGattDiscoverCharacDescriptorsInd*) recvMsgP;

            dbElement->currentDescr =
                            CSR_BT_GATT_APP_DB_ADD_CHARAC_DESCR(dbElement->currentCharac->descrList);
            dbElement->currentDescr->descriptorHandle = ind->descriptorHandle;
            dbElement->currentDescr->uuid = ind->uuid;
            break;
        }
        case CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_CFM:
        {
            /* Move on to next Charac or if at last one, then move to next Prim Service first charac */
            CsrBtGattDiscoverCharacDescriptorsCfm *cfm =
                            (CsrBtGattDiscoverCharacDescriptorsCfm*) recvMsgP;

            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS
                || cfm->resultCode == CSR_BT_GATT_RESULT_INVALID_HANDLE_RANGE)

            {
                dbElement->currentDescr =
                                (CsrBtGattAppDbCharacDescrElement *) dbElement->currentCharac->descrList.first;
                getDescriptorValue(gattId,
                                   btConnId,
                                   dbElement);
            }
            else
            {
                retVal |= CSR_BT_GATT_CHARAC_DESCRIPTOR_CFM_ERROR;
            }
            break;
        }
        case CSR_BT_GATT_READ_CFM:
        {
            CsrUint16 startHandle, endHandle = 0;
            CsrBtGattReadCfm *cfm;
            cfm = (CsrBtGattReadCfm*) recvMsgP;

            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                /*store the values */
                dbElement->currentCharac->value = cfm->value;
                dbElement->currentCharac->valueLength = cfm->valueLength;

                if (CsrBtGattAppDbFindNextCharacWithProperty(&dbElement->currentPrimService,
                                                             &dbElement->currentCharac,
                                                             CSR_BT_GATT_CHARAC_PROPERTIES_READ))
                {
                    CsrBtGattReadReqSend(gattId,
                                         btConnId,
                                         dbElement->currentCharac->valueHandle,
                                         0);
                }
                else
                {
                    /* Done Reading!
                     * we need to reset the "current" pointers and start working on the descriptors*/
                    dbElement->currentPrimService =
                                    CSR_BT_GATT_APP_DB_GET_FIRST_PRIM_SERVICE(dbElement->dbPrimServices);

                    /* [QTI] Fix KW issue#831697 through adding the check "dbElement->currentPrimService". */
                    if (dbElement->currentPrimService)
                    {
                        /*start from the first charac (we know that there is at least one) */
                        dbElement->currentCharac =
                                        CSR_BT_GATT_APP_DB_GET_FIRST_CHARAC(dbElement->currentPrimService->characList);
                        startHandle = dbElement->currentCharac->declarationHandle
                                      + 1;
                        if (dbElement->currentCharac->next)
                        {
                            /* there are more characs in this prim service so we use the start of the next one as end handle*/
                            endHandle = ((CsrBtGattAppDbCharacElement*) dbElement->currentCharac->next)->declarationHandle
                                        - 1;
                        }
                        else
                        {
                            /* We are at the last charac in the current prim service so endhandle
                             * is start of next prim service (NOTE: we expect them to be sorted) */
                            endHandle = ((CsrBtGattAppDbPrimServiceElement*) dbElement->currentPrimService->next)->startHandle
                                        - 1;
                        }
                        CsrBtGattDiscoverAllCharacDescriptorsReqSend(gattId,
                                                                     btConnId,
                                                                     startHandle,
                                                                     endHandle);
                    }
                }
                retVal |= CSR_BT_GATT_READ_CFM_SUCCESS;
            }
            else
            {

                retVal |= CSR_BT_GATT_READ_CFM_ERROR;
            }
            break;
        }
        case CSR_BT_GATT_READ_EXTENDED_PROPERTIES_CFM:
        {
            CsrBtGattReadExtendedPropertiesCfm *ind;

            ind = (CsrBtGattReadExtendedPropertiesCfm*) recvMsgP;
            if (ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                dbElement->currentDescr->value =
                                (CsrUint8*) CsrPmemAlloc(sizeof(CsrBtGattExtPropertiesBits));
                CsrMemCpy(dbElement->currentDescr->value,
                          &ind->extProperties,
                          sizeof(CsrBtGattExtPropertiesBits));
            }
            dbElement->currentDescr = dbElement->currentDescr->next;
            getDescriptorValue(gattId, btConnId, dbElement);
            break;
        }
        case CSR_BT_GATT_READ_USER_DESCRIPTION_CFM:
        {
            CsrBtGattReadUserDescriptionCfm *ind =
                            (CsrBtGattReadUserDescriptionCfm *) recvMsgP;
            if (ind->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                CsrBtGattAppDbCharacDescrElement *desc = dbElement->currentDescr;
                CsrSize strLength = CsrStrLen((CsrCharString*) ind->usrDescription);
                desc->value = ind->usrDescription;
                desc->valueLength = (CsrUint16) strLength + 1;
            }
            dbElement->currentDescr = dbElement->currentDescr->next;
            getDescriptorValue(gattId,
                               btConnId,
                               dbElement);
            break;
        }
        case CSR_BT_GATT_READ_PROFILE_DEFINED_DESCRIPTOR_CFM:
        {
            CsrBtGattReadProfileDefinedDescriptorCfm *cfm =
                            (CsrBtGattReadProfileDefinedDescriptorCfm *) recvMsgP;
            if (cfm->resultCode == CSR_BT_GATT_RESULT_SUCCESS)
            {
                dbElement->currentDescr->value = cfm->value;
                dbElement->currentDescr->valueLength = cfm->valueLength;
            }
            dbElement->currentDescr = dbElement->currentDescr->next;
            getDescriptorValue(gattId,
                               btConnId,
                               dbElement);
            break;
        }

        default:
        {
            /* If the switch parameter is not recognized. */
            retVal = CSR_BT_GATT_PRIM_NOT_RECOGNIZED;
            break;
        }
    }

    return retVal;
}

